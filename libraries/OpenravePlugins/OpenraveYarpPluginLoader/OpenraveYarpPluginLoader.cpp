// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpPluginLoader.hpp"

#include <algorithm> // std::replace

#include <boost/bind/bind.hpp>
#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/WrapperSingle.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// -----------------------------------------------------------------------------

namespace
{
    bool tryOpenDevice(yarp::os::Property & options, std::vector<yarp::dev::PolyDriver *> & drivers)
    {
        auto * driver = new yarp::dev::PolyDriver;
        drivers.push_back(driver);

        if (options.check("wrap"))
        {
            yCInfo(ORYPL) << "Requested subdevice wrapping mode";
            options.put("wrapping_enabled", 1);
            options.unput("wrap");
            return driver->open(options);
        }

        yarp::os::Property mainOptions(options);
        mainOptions.unput("subdevice");

        yarp::os::Property subOptions(options);
        subOptions.put("device", options.find("subdevice"));
        subOptions.unput("subdevice");

        if (!driver->open(mainOptions))
        {
            yCError(ORYPL) << "Could not open main device";
            return false;
        }

        yarp::dev::WrapperSingle * wrapper = nullptr;

        if (!driver->view(wrapper))
        {
            yCError(ORYPL) << "Could not view WrapperSingle";
            return false;
        }

        auto * subDriver = new yarp::dev::PolyDriver;
        drivers.push_back(subDriver);

        if (!subDriver->open(subOptions))
        {
            yCError(ORYPL) << "Could not open subdevice";
            return false;
        }

        if (!wrapper->attach(subDriver))
        {
            yCError(ORYPL) << "Could not attach subdevice";
            return false;
        }

        return true;
    }

    void setPortNames(yarp::os::Property & options, const std::string & name)
    {
        if (options.check("ros"))
        {
            options.put("node_name", "/" + name + "/node"); // ROS only
            options.put("topic_name", "/" + name + "/topic"); // ROS only
        }
        else if (options.check("ros2"))
        {
            std::string nodeName = name;
            std::replace(nodeName.begin(), nodeName.end(), '/', '_'); // only alphanumerics and underscores allowed
            options.put("node_name", nodeName); // ROS2 only
            options.put("topic_name", "/" + name); // ROS2 only
            options.put("msgs_name", "/" + name); // ROS2 only (control board-specific)
        }
        else
        {
            options.put("name", "/" + name); // YARP only
        }
    }
}

// -----------------------------------------------------------------------------

OpenraveYarpPluginLoader::OpenraveYarpPluginLoader(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    using namespace boost::placeholders;
    __description = "OpenraveYarpPluginLoader plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPluginLoader::Open, this,_1,_2),"opens OpenraveYarpPluginLoader");
    OpenRAVE::InterfaceBase::RegisterCommand("getPenv",boost::bind(&OpenraveYarpPluginLoader::GetPenv, this,_1,_2),"Gets a pointer to the environment");

    yCInfo(ORYPL) << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
        yCError(ORYPL) << "Found no yarp network (try running \"yarpserver &\")";
    yCInfo(ORYPL) << "Found yarp network";

    oyplPortReader.setOpenraveYarpPluginLoaderPtr(this);
    oyplRpcServer.setReader(oyplPortReader);
    oyplRpcServer.open("/OpenraveYarpPluginLoader/rpc:s");

    oyplPeriodicWrite.setOpenraveYarpPluginLoaderPtr(this);
    oyplPeriodicWrite.open("/OpenraveYarpPluginLoader/state:o");
}

// -----------------------------------------------------------------------------

OpenraveYarpPluginLoader::~OpenraveYarpPluginLoader()
{
    for(int i=0;i<yarpPlugins.size();i++)
    {
        yarpPlugins[i]->close();
        delete yarpPlugins[i];
        yarpPlugins[i] = 0;
    }

    oyplRpcServer.interrupt();
    oyplPeriodicWrite.interrupt();

    oyplRpcServer.close();
    oyplPeriodicWrite.close();
}

// -----------------------------------------------------------------------------

void OpenraveYarpPluginLoader::Destroy()
{
    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

bool OpenraveYarpPluginLoader::addYarpPluginsLists(yarp::os::Bottle& info)
{
    for (size_t i=0;i<yarpPluginsProperties.size();i++)
    {
        if(yarpPluginsProperties[i].check("remotelyClosed"))
            continue;
        yarp::os::Bottle& b = info.addList();
        b.addInt32(i);
        yarp::os::Property openOptions(yarpPluginsProperties[i]);
        openOptions.unput("penv");
        openOptions.unput("allManipulators");
        openOptions.unput("allSensors");
        yarp::os::Bottle openOptionsBottle;
        openOptionsBottle.fromString(openOptions.toString());
        b.append(openOptionsBottle);
    }
    return true;
}

// -----------------------------------------------------------------------------

int OpenraveYarpPluginLoader::main(const std::string& cmd)
{
    yCDebug(ORYPL) << cmd;
    std::stringstream ss(cmd);

    //-- Fill openStrings and envString
    std::vector<std::string> openStrings;
    std::string envString("");

    enum mode { none, open, env };
    int currentMode = mode::none;
    while( ! ss.eof() )
    {
        std::string tmp;
        ss >> tmp;

        if(tmp == "open")
        {
            openStrings.emplace_back("open");
            currentMode = mode::open;
        }
        else if(tmp == "env")
        {
            currentMode = mode::env;
        }
        else
        {
            if(currentMode == mode::open)
            {
                openStrings[openStrings.size()-1].append(" ");
                openStrings[openStrings.size()-1].append(tmp);
            }
            else if(currentMode == mode::env)
            {
                envString = tmp;
            }
        }
    }

    yCDebug(ORYPL) << "env:" << envString;

    if(envString!="")
    {
        if ( !!GetEnv()->Load(envString.c_str()) )
        {
            yCInfo(ORYPL) << "Loaded" << envString << "environment";
        }
        else
        {
            yCDebug(ORYPL) << "Could not load" << envString << "environment, attempting via yarp::os::ResourceFinder";

            yarp::os::ResourceFinder & rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
            std::string fullEnvString = rf.findFileByName(envString);

            if ( !GetEnv()->Load(fullEnvString.c_str()) )
            {
                yCError(ORYPL) << "Could not load" << fullEnvString << "environment";
                return 1;
            }
            yCInfo(ORYPL) << "Loaded" << fullEnvString << "environment";
        }
    }

    //-- Open each openString
    for(int i=0;i<openStrings.size();i++)
    {
        yCDebug(ORYPL, "open[%d]: [%s]",i,openStrings[i].c_str());

        std::istringstream sinput( openStrings[i] );
        std::ostringstream sout;
        if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
        {
            yCError(ORYPL) << sout.str();
            return 1;
        }
        yCInfo(ORYPL) << "Open ids:" << sout.str();
    }
    return 0;
}

// -----------------------------------------------------------------------------

bool OpenraveYarpPluginLoader::Open(std::ostream& sout, std::istream& sinput)
{
    yCInfo(ORYPL) << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
    {
        yCError(ORYPL) << "Found no yarp network (try running \"yarpserver &\")";
        sout << "-1 ";
        return false;
    }

    std::string s(std::istreambuf_iterator<char>(sinput), {});

    yarp::os::Property options;
    options.fromArguments(s.c_str());

    yCDebug(ORYPL) << "Config:" << options.toString();

    //-- Get and put pointer to environment
    yCInfo(ORYPL) << "penv:" << GetEnv().get();
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();
    yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
    options.put("penv",v);

    //-- Fill robotIndices from: robotIndex/robotIndices/allRobots
    std::vector<int> robotIndices;

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);

    if( options.check("robotIndex") )
    {
        int robotIndex = options.find("robotIndex").asInt32();
        robotIndices.push_back(robotIndex);
    }
    else if( options.check("robotIndices") )
    {
        yCError(ORYPL) << "robotIndices not implemented yet";
        sout << "-1 ";
        return false;
    }
    else if( options.check("allRobots") )
    {
        for(int i=0;i<vectorOfRobotPtr.size();i++)
            robotIndices.push_back(i);
    }
    else
    {
        yCInfo(ORYPL) << "Not using --robotIndex or --robotIndices or --allRobots parameter";

        yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
        yarpPlugin->open(options);

        if( ! yarpPlugin->isValid() )
        {
            yCError(ORYPL) << "Yarp plugin not valid";
            sout << "-1 ";
            return false;
        }
        yCInfo(ORYPL) << "Valid yarp plugin with id" << yarpPluginsProperties.size();

        yarpPlugins.push_back(yarpPlugin);
        yarpPluginsProperties.push_back(options);
        sout << yarpPluginsProperties.size()-1;
        sout << " ";
        return true;
    }

    if( ! options.check("device") || ! options.check("subdevice") )
    {
        yCError(ORYPL) << "Missing mandatory --device or --subdevice parameters";
        sout << "-1 ";
        return false;
    }

    //-- Iterate through robots
    for(int i=0;i<robotIndices.size();i++)
    {
        int robotIndex = robotIndices[i];
        if( robotIndex >= vectorOfRobotPtr.size())
        {
            yCError(ORYPL, "robotIndex %d >= vectorOfRobotPtr.size() %zu, not loading yarp plugin",robotIndex,vectorOfRobotPtr.size());
            sout << "-1 ";
            return false;
        }
        else if (robotIndex < 0)
        {
            yCError(ORYPL) << "robotIndex" << robotIndex << "< 0, not loading yarp plugin";
            sout << "-1 ";
            return false;
        }
        options.put("robotIndex",robotIndex);

        std::string robotName = vectorOfRobotPtr[ robotIndex ]->GetName();

        //-- Fill manipulatorIndices from: manipulatorIndex/manipulatorIndices/allManipulators
        //-- Fill sensorIndices from: sensorIndex/sensorIndices/allSensors
        std::vector<int> manipulatorIndices;
        std::vector<int> sensorIndices;

        std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[ robotIndex ]->GetManipulators();
        std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = vectorOfRobotPtr[ robotIndex ]->GetAttachedSensors();

        if( options.check("manipulatorIndex") )
        {
            int manipulatorIndex = options.find("manipulatorIndex").asInt32();
            manipulatorIndices.push_back(manipulatorIndex);
        }
        else if( options.check("manipulatorIndices") )
        {
            yCError(ORYPL) << "manipulatorIndices not implemented yet";
            sout << "-1 ";
            return false;
        }
        else if( options.check("allManipulators") )
        {
            for(int i=0;i<vectorOfManipulatorPtr.size();i++)
                manipulatorIndices.push_back(i);
        }
        else if( options.check("sensorIndex") )
        {
            int sensorIndex = options.find("sensorIndex").asInt32();
            sensorIndices.push_back(sensorIndex);
        }
        else if( options.check("sensorIndices") )
        {
            yCError(ORYPL) << "sensorIndices not implemented yet";
            sout << "-1 ";
            return false;
        }
        else if( options.check("allSensors") )
        {
            for(int i=0;i<vectorOfSensorPtr.size();i++)
                sensorIndices.push_back(i);
        }
        else
        {
            yCInfo(ORYPL) << "Not using --manipulatorIndex or --manipulatorIndices or --allManipulators parameter";
            yCInfo(ORYPL) << "Not using --sensorIndex or --sensorIndices or --allSensors parameter";

            if( ! options.check("forceName") )
            {
                setPortNames(options, robotName);
            }

            if( ! tryOpenDevice(options, yarpPlugins) )
            {
                yCError(ORYPL) << "Yarp plugin not valid";
                sout << "-1 ";
                return false;
            }
            yCInfo(ORYPL) << "Valid yarp plugin with id" << yarpPluginsProperties.size();

            yarpPluginsProperties.push_back(options);
            sout << yarpPluginsProperties.size()-1;
            sout << " ";
        }

        //-- Iterate through manipulators
        for(int i=0;i<manipulatorIndices.size();i++)
        {
            int manipulatorIndex = manipulatorIndices[i];
            if(manipulatorIndex >= vectorOfManipulatorPtr.size())
            {
                yCError(ORYPL, "manipulatorIndex %d >= vectorOfManipulatorPtr.size() %zu, not loading yarp plugin",manipulatorIndex,vectorOfManipulatorPtr.size());
                sout << "-1 ";
                return false;
            }
            else if (manipulatorIndex < 0)
            {
                yCError(ORYPL) << "manipulatorIndex" << manipulatorIndex << "< 0, not loading yarp plugin";
                sout << "-1 ";
                return false;
            }
            options.put("manipulatorIndex",manipulatorIndex);

            std::string manipulatorName(robotName);
            manipulatorName += "/";
            manipulatorName += vectorOfManipulatorPtr[ manipulatorIndex ]->GetName();

            if( ! options.check("forceName") )
            {
                setPortNames(options, manipulatorName);
            }

            if( ! tryOpenDevice(options, yarpPlugins) )
            {
                yCError(ORYPL) << "Yarp plugin not valid";
                sout << "-1 ";
                return false;
            }
            yCInfo(ORYPL) << "Valid yarp plugin with id" << yarpPluginsProperties.size();

            yarpPluginsProperties.push_back(options);
            sout << yarpPluginsProperties.size()-1;
            sout << " ";
        } //-- end Iterate through manipulators

        //-- Iterate through sensors
        for(int i=0;i<sensorIndices.size();i++)
        {
            int sensorIndex = sensorIndices[i];
            if(sensorIndex >= vectorOfSensorPtr.size())
            {
                yCError(ORYPL, "sensorIndex %d >= vectorOfSensorPtr.size() %zu, not loading yarp plugin",sensorIndex,vectorOfSensorPtr.size());
                sout << "-1 ";
                return false;
            }
            else if (sensorIndex < 0)
            {
                yCError(ORYPL) << "sensorIndex" << sensorIndex << "< 0, not loading yarp plugin";
                sout << "-1 ";
                return false;
            }
            options.put("sensorIndex",sensorIndex);

            std::string sensorName(robotName);
            sensorName += "/";
            sensorName += vectorOfSensorPtr[ sensorIndex ]->GetName();

            if( ! options.check("forceName") )
            {
                setPortNames(options, sensorName);
            }

            if( ! tryOpenDevice(options, yarpPlugins) )
            {
                yCError(ORYPL) << "Yarp plugin not valid";
                sout << "-1 ";
                return false;
            }
            yCInfo(ORYPL) << "Valid yarp plugin with id" << yarpPluginsProperties.size();

            yarpPluginsProperties.push_back(options);
            sout << yarpPluginsProperties.size()-1;
            sout << " ";
        } //-- end Iterate through sensors
    } //-- end Iterate through robots
    return true;
}

// -----------------------------------------------------------------------------

bool OpenraveYarpPluginLoader::GetPenv(std::ostream& sout, std::istream& sinput)
{
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();
    yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
    yCDebug(ORYPL) << "penvValue:" << v.toString();
    sout << v.toString();
    return true;
}

// -----------------------------------------------------------------------------

bool OpenraveYarpPluginLoader::close(const int i)
{
    if(!yarpPlugins[i]->close())
    {
        yCError(ORYPL) << "Could not close" << i;
        return false;
    }
    yarpPluginsProperties[i].put("remotelyClosed",1);
    yCInfo(ORYPL) << "Closed yarp plugin with id" << i;
    return true;
}

// -----------------------------------------------------------------------------

OpenRAVE::InterfaceBasePtr OpenraveYarpPluginLoaderPlugin::CreateInterface(OpenRAVE::InterfaceType type,
                                                                           const std::string & interfacename,
                                                                           std::istream & sinput,
                                                                           OpenRAVE::EnvironmentBasePtr penv)
{
    if (type == OpenRAVE::PT_Module && interfacename == "openraveyarppluginloader")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpPluginLoader(penv));
    }

    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

const RavePlugin::InterfaceMap & OpenraveYarpPluginLoaderPlugin::GetInterfaces() const
{
    static const RavePlugin::InterfaceMap interfaces = {
        {OpenRAVE::PT_Module, {"OpenraveYarpPluginLoader"}},
    };

    return interfaces;
}

// -----------------------------------------------------------------------------

const std::string & OpenraveYarpPluginLoaderPlugin::GetPluginName() const
{
    static const std::string pluginName = "OpenraveYarpPluginLoader";
    return pluginName;
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API RavePlugin * CreatePlugin() {
    return new OpenraveYarpPluginLoaderPlugin();
}

// -----------------------------------------------------------------------------
