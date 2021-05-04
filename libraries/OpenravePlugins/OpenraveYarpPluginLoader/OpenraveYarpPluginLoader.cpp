// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include "OpenraveYarpPluginLoader.hpp"

// -----------------------------------------------------------------------------

roboticslab::OpenraveYarpPluginLoader::OpenraveYarpPluginLoader(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    __description = "OpenraveYarpPluginLoader plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPluginLoader::Open, this,_1,_2),"opens OpenraveYarpPluginLoader");
    OpenRAVE::InterfaceBase::RegisterCommand("getPenv",boost::bind(&OpenraveYarpPluginLoader::GetPenv, this,_1,_2),"Gets a pointer to the environment");

    yInfo() << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
        yError() << "Found no yarp network (try running \"yarpserver &\")";
    yInfo() << "Found yarp network";

    oyplPortReader.setOpenraveYarpPluginLoaderPtr(this);
    oyplRpcServer.setReader(oyplPortReader);
    oyplRpcServer.open("/OpenraveYarpPluginLoader/rpc:s");

    oyplPeriodicWrite.setOpenraveYarpPluginLoaderPtr(this);
    oyplPeriodicWrite.open("/OpenraveYarpPluginLoader/state:o");
}

// -----------------------------------------------------------------------------

roboticslab::OpenraveYarpPluginLoader::~OpenraveYarpPluginLoader()
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

void roboticslab::OpenraveYarpPluginLoader::Destroy()
{
    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

bool roboticslab::OpenraveYarpPluginLoader::addYarpPluginsLists(yarp::os::Bottle& info)
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

int roboticslab::OpenraveYarpPluginLoader::main(const std::string& cmd)
{
    yDebug() << cmd;
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
            std::string openString("open");
            openStrings.push_back(openString);
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

    yDebug() << "env:" << envString;

    if(envString!="")
    {
        if ( !!GetEnv()->Load(envString.c_str()) )
        {
            yInfo() << "Loaded" << envString << "environment";
        }
        else
        {
            yDebug() << "Could not load" << envString << "environment, attempting via yarp::os::ResourceFinder";

            yarp::os::ResourceFinder rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
            std::string fullEnvString = rf.findFileByName(envString);

            if ( !GetEnv()->Load(fullEnvString.c_str()) )
            {
                yError() << "Could not load" << fullEnvString << "environment";
                return 1;
            }
            yInfo() << "Loaded" << fullEnvString << "environment";
        }
    }

    //-- Open each openString
    for(int i=0;i<openStrings.size();i++)
    {
        yDebug("open[%d]: [%s]",i,openStrings[i].c_str());

        std::istringstream sinput( openStrings[i] );
        std::ostringstream sout;
        if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
        {
            yError() << sout.str();
            return 1;
        }
        yInfo() << "Open ids:" << sout.str();
    }
    return 0;
}

// -----------------------------------------------------------------------------

bool roboticslab::OpenraveYarpPluginLoader::Open(std::ostream& sout, std::istream& sinput)
{
    yInfo() << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
    {
        yError() << "Found no yarp network (try running \"yarpserver &\")";
        sout << "-1 ";
        return false;
    }

    std::string s(std::istreambuf_iterator<char>(sinput), {});

    yarp::os::Property options;
    options.fromArguments(s.c_str());

    yDebug() << "Config:" << options.toString();

    //-- Get and put pointer to environment
    yInfo() << "penv:" << GetEnv().get();
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
        yError() << "robotIndices not implemented yet";
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
        yInfo() << "Not using --robotIndex or --robotIndices or --allRobots parameter";

        yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
        yarpPlugin->open(options);

        if( ! yarpPlugin->isValid() )
        {
            yError() << "Yarp plugin not valid";
            sout << "-1 ";
            return false;
        }
        yInfo() << "Valid yarp plugin with id" << yarpPluginsProperties.size();

        yarpPlugins.push_back(yarpPlugin);
        yarpPluginsProperties.push_back(options);
        sout << yarpPluginsProperties.size()-1;
        sout << " ";
        return true;
    }

    //-- Iterate through robots
    for(int i=0;i<robotIndices.size();i++)
    {
        int robotIndex = robotIndices[i];
        if( robotIndex >= vectorOfRobotPtr.size())
        {
            yError("robotIndex %d >= vectorOfRobotPtr.size() %zu, not loading yarp plugin",robotIndex,vectorOfRobotPtr.size());
            sout << "-1 ";
            return false;
        }
        else if (robotIndex < 0)
        {
            yError() << "robotIndex" << robotIndex << "< 0, not loading yarp plugin";
            sout << "-1 ";
            return false;
        }
        options.put("robotIndex",robotIndex);

        std::string robotName("/");
        robotName += vectorOfRobotPtr[ robotIndex ]->GetName();

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
            yError() << "manipulatorIndices not implemented yet";
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
            yError() << "sensorIndices not implemented yet";
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
            yInfo() << "Not using --manipulatorIndex or --manipulatorIndices or --allManipulators parameter";
            yInfo() << "Not using --sensorIndex or --sensorIndices or --allSensors parameter";

            if( ! options.check("forceName") )
            {
                options.put("name",robotName);
            }

            yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
            yarpPlugin->open(options);

            if( ! yarpPlugin->isValid() )
            {
                yError() << "Yarp plugin not valid";
                sout << "-1 ";
                return false;
            }
            yInfo() << "Valid yarp plugin with id" << yarpPluginsProperties.size();

            yarpPlugins.push_back(yarpPlugin);
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
                yError("manipulatorIndex %d >= vectorOfManipulatorPtr.size() %zu, not loading yarp plugin",manipulatorIndex,vectorOfManipulatorPtr.size());
                sout << "-1 ";
                return false;
            }
            else if (manipulatorIndex < 0)
            {
                yError() << "manipulatorIndex" << manipulatorIndex << "< 0, not loading yarp plugin";
                sout << "-1 ";
                return false;
            }
            options.put("manipulatorIndex",manipulatorIndex);

            std::string manipulatorName(robotName);
            manipulatorName += "/";
            manipulatorName += vectorOfManipulatorPtr[ manipulatorIndex ]->GetName();

            if( ! options.check("forceName") )
            {
                options.put("name",manipulatorName);
            }

            yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
            yarpPlugin->open(options);

            if( ! yarpPlugin->isValid() )
            {
                yError() << "Yarp plugin not valid";
                sout << "-1 ";
                return false;
            }
            yInfo() << "Valid yarp plugin with id" << yarpPluginsProperties.size();

            yarpPlugins.push_back(yarpPlugin);
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
                yError("sensorIndex %d >= vectorOfSensorPtr.size() %zu, not loading yarp plugin",sensorIndex,vectorOfSensorPtr.size());
                sout << "-1 ";
                return false;
            }
            else if (sensorIndex < 0)
            {
                yError() << "sensorIndex" << sensorIndex << "< 0, not loading yarp plugin";
                sout << "-1 ";
                return false;
            }
            options.put("sensorIndex",sensorIndex);

            std::string sensorName(robotName);
            sensorName += "/";
            sensorName += vectorOfSensorPtr[ sensorIndex ]->GetName();

            if( ! options.check("forceName") )
            {
                options.put("name",sensorName);
            }

            yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
            yarpPlugin->open(options);

            if( ! yarpPlugin->isValid() )
            {
                yError() << "Yarp plugin not valid";
                sout << "-1 ";
                return false;
            }
            yInfo() << "Valid yarp plugin with id" << yarpPluginsProperties.size();

            yarpPlugins.push_back(yarpPlugin);
            yarpPluginsProperties.push_back(options);
            sout << yarpPluginsProperties.size()-1;
            sout << " ";
        } //-- end Iterate through sensors
    } //-- end Iterate through robots
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::OpenraveYarpPluginLoader::GetPenv(std::ostream& sout, std::istream& sinput)
{
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();
    yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
    yDebug() << "penvValue:" << v.toString();
    sout << v.toString();
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::OpenraveYarpPluginLoader::close(const int i)
{
    if(!yarpPlugins[i]->close())
    {
        yError() << "Could not close" << i;
        return false;
    }
    yarpPluginsProperties[i].put("remotelyClosed",1);
    yInfo() << "Closed yarp plugin with id" << i;
    return true;
}

// -----------------------------------------------------------------------------


OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarppluginloader")
    {
        return OpenRAVE::InterfaceBasePtr(new roboticslab::OpenraveYarpPluginLoader(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpPluginLoader");
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}

// -----------------------------------------------------------------------------
