// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <sstream>
#include <string>
#include <vector>

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/PolyDriver.h>

#include <ColorDebug.h>

#define VOCAB_OK yarp::os::createVocab('o','k')
#define VOCAB_FAILED yarp::os::createVocab('f','a','i','l')

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpPluginLoader
 *
 * @brief Contains roboticslab::OpenraveYarpPluginLoader.
 */

// -----------------------------------------------------------------------------

class OpenraveYarpPluginLoader;

// -----------------------------------------------------------------------------

class OpenPortReader: public yarp::os::PortReader
{
public:
    void setOpenraveYarpPluginLoaderPtr(OpenraveYarpPluginLoader *value) { openraveYarpPluginLoaderPtr = value; }
private:
    OpenraveYarpPluginLoader* openraveYarpPluginLoaderPtr;
    virtual bool read(yarp::os::ConnectionReader& in) override;
};

// -----------------------------------------------------------------------------

class OpenPortPeriodicWrite : yarp::os::PeriodicThread, public yarp::os::Port
{
public:
    OpenPortPeriodicWrite();
    void setOpenraveYarpPluginLoaderPtr(OpenraveYarpPluginLoader *value) { openraveYarpPluginLoaderPtr = value; }
private:
    OpenraveYarpPluginLoader* openraveYarpPluginLoaderPtr;
    virtual void run() override;
};

// -----------------------------------------------------------------------------

/**
 * @ingroup OpenraveYarpPluginLoader
 * @brief Loads one or several YARP Plugin, passing environment pointer.
 */
class OpenraveYarpPluginLoader : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpPluginLoader(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
    {
        __description = "OpenraveYarpPluginLoader plugin.";
        OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPluginLoader::Open, this,_1,_2),"opens OpenraveYarpPluginLoader");

        CD_INFO("Checking for yarp network...\n");
        if ( ! yarp.checkNetwork() )
            CD_ERROR("Found no yarp network (try running \"yarpserver &\")!\n");
        CD_SUCCESS("Found yarp network.\n");

        openPortReader.setOpenraveYarpPluginLoaderPtr(this);
        openPortRpcServer.setReader(openPortReader);
        openPortRpcServer.open("/OpenraveYarpPluginLoader/rpc:s");

        openPortPeriodicWrite.setOpenraveYarpPluginLoaderPtr(this);
        openPortPeriodicWrite.open("/OpenraveYarpPluginLoader/state:o");
    } //-- end OpenraveYarpPluginLoader

    virtual ~OpenraveYarpPluginLoader()
    {
        for(int i=0;i<yarpPlugins.size();i++)
        {
            yarpPlugins[i]->close();
            delete yarpPlugins[i];
            yarpPlugins[i] = 0;
        }

        openPortRpcServer.interrupt();
        openPortPeriodicWrite.interrupt();

        openPortRpcServer.close();
        openPortPeriodicWrite.close();
    } //-- end ~OpenraveYarpPluginLoader

    virtual void Destroy()
    {
        RAVELOG_INFO("module unloaded from environment\n");
    } //-- end Destroy

    std::vector<yarp::os::Property> getYarpPluginsProperties() const { return yarpPluginsProperties; }

    int main(const std::string& cmd)
    {
        CD_DEBUG("[%s]\n", cmd.c_str());
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

        CD_DEBUG("env: '%s'\n",envString.c_str());

        if(envString!="")
        {
            if ( !!GetEnv()->Load(envString.c_str()) )
            {
                CD_SUCCESS("Loaded '%s' environment.\n",envString.c_str());
            }
            else
            {
                CD_DEBUG("Could not load '%s' environment, attempting via yarp::os::ResourceFinder.\n",envString.c_str());

                yarp::os::ResourceFinder rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
                std::string fullEnvString = rf.findFileByName(envString);

                if ( !GetEnv()->Load(fullEnvString.c_str()) )
                {
                    CD_ERROR("Could not load '%s' environment.\n",fullEnvString.c_str());
                    return 1;
                }
                CD_SUCCESS("Loaded '%s' environment.\n",fullEnvString.c_str());
            }
        }

        //-- Open each openString
        for(int i=0;i<openStrings.size();i++)
        {
            CD_DEBUG("open[%d]: [%s]\n",i,openStrings[i].c_str());

            std::istringstream sinput( openStrings[i] );
            std::ostringstream sout;
            if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
            {
                CD_ERROR("%s\n",sout.str().c_str());
                return 1;
            }
            CD_SUCCESS("Open ids: %s\n",sout.str().c_str());
        }
        return 0;
    } //-- end main

    bool Open(std::ostream& sout, std::istream& sinput)
    {
        CD_INFO("Checking for yarp network...\n");
        if ( ! yarp.checkNetwork() )
        {
            CD_ERROR("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            sout << "-1 ";
            return false;
        }
        CD_SUCCESS("Found yarp network.\n");

        std::string s(std::istreambuf_iterator<char>(sinput), {});

        yarp::os::Property options;
        options.fromArguments(s.c_str());

        CD_DEBUG("config: %s\n", options.toString().c_str());

        //-- Get and put pointer to environment
        CD_INFO("penv: %p\n",GetEnv().get());
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
            CD_ERROR("robotIndices not implemented yet. Bye!\n");
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
            CD_INFO("Not using --robotIndex or --robotIndices or --allRobots parameter.\n");

            yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
            yarpPlugin->open(options);

            if( ! yarpPlugin->isValid() )
            {
                CD_ERROR("yarp plugin not valid.\n");
                sout << "-1 ";
                return false;
            }
            CD_SUCCESS("Valid yarp plugin (id %d).\n",yarpPluginsProperties.size());

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
                CD_ERROR("robotIndex %d >= vectorOfRobotPtr.size() %d, not loading yarp plugin. Bye!\n",robotIndex,vectorOfRobotPtr.size());
                sout << "-1 ";
                return false;
            }
            else if (robotIndex < 0)
            {
                CD_ERROR("robotIndex %d < 0, not loading yarp plugin. Bye!\n",robotIndex);
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
                CD_ERROR("manipulatorIndices not implemented yet. Bye!\n");
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
                CD_ERROR("sensorIndices not implemented yet. Bye!\n");
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
                CD_INFO("Not using --manipulatorIndex or --manipulatorIndices or --allManipulators parameter.\n");
                CD_INFO("Not using --sensorIndex or --sensorIndices or --allSensors parameter.\n");

                if( ! options.check("forceName") )
                {
                    options.put("name",robotName);
                }

                yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
                yarpPlugin->open(options);

                if( ! yarpPlugin->isValid() )
                {
                    CD_ERROR("yarp plugin not valid.\n");
                    sout << "-1 ";
                    return false;
                }
                CD_SUCCESS("Valid yarp plugin (id %d).\n",yarpPluginsProperties.size());

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
                    CD_ERROR("manipulatorIndex %d >= vectorOfManipulatorPtr.size() %d, not loading yarp plugin. Bye!\n",manipulatorIndex,vectorOfManipulatorPtr.size());
                    sout << "-1 ";
                    return false;
                }
                else if (manipulatorIndex < 0)
                {
                    CD_ERROR("manipulatorIndex %d < 0, not loading yarp plugin.\n",manipulatorIndex);
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
                    CD_ERROR("yarp plugin not valid.\n");
                    sout << "-1 ";
                    return false;
                }
                CD_SUCCESS("Valid yarp plugin (id %d).\n",yarpPluginsProperties.size());

                yarpPlugins.push_back(yarpPlugin);
                yarpPluginsProperties.push_back(options);
                sout << yarpPluginsProperties.size()-1;
                sout << " ";
            } //-- Iterate through manipulators

            //-- Iterate through sensors
            for(int i=0;i<sensorIndices.size();i++)
            {
                int sensorIndex = sensorIndices[i];
                if(sensorIndex >= vectorOfSensorPtr.size())
                {
                    CD_ERROR("sensorIndex %d >= vectorOfSensorPtr.size() %d, not loading yarp plugin. Bye!\n",sensorIndex,vectorOfSensorPtr.size());
                    sout << "-1 ";
                    return false;
                }
                else if (sensorIndex < 0)
                {
                    CD_ERROR("sensorIndex %d < 0, not loading yarp plugin.\n",sensorIndex);
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
                    CD_ERROR("yarp plugin not valid.\n");
                    sout << "-1 ";
                    return false;
                }
                CD_SUCCESS("Valid yarp plugin (id %d).\n",yarpPluginsProperties.size());

                yarpPlugins.push_back(yarpPlugin);
                yarpPluginsProperties.push_back(options);
                sout << yarpPluginsProperties.size()-1;
                sout << " ";
            } //-- Iterate through sensors
        } //-- Iterate through robots
        return true;
    } //-- end Open

private:
    yarp::os::Network yarp;
    std::vector<yarp::dev::PolyDriver*> yarpPlugins;
    std::vector<yarp::os::Property> yarpPluginsProperties;

    OpenPortReader openPortReader;
    yarp::os::RpcServer openPortRpcServer;

    OpenPortPeriodicWrite openPortPeriodicWrite;
};

// -----------------------------------------------------------------------------

bool OpenPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    CD_DEBUG("Request: %s\n", request.toString().c_str());
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if ( request.get(0).asString() == "help" ) //-- help
    {
        response.addString("Available commands: help, list, open (device ...) ...");
        response.write(*out);
        return true;
    }
    else if ( request.get(0).asString() == "list" ) //-- list
    {
        for (size_t i=0;i<openraveYarpPluginLoaderPtr->getYarpPluginsProperties().size();i++)
        {
            yarp::os::Bottle& b = response.addList();
            b.addInt32(i);
            yarp::os::Property openOptions(openraveYarpPluginLoaderPtr->getYarpPluginsProperties()[i]);
            openOptions.unput("penv");
            openOptions.unput("allManipulators");
            openOptions.unput("allSensors");
            yarp::os::Bottle openOptionsBottle;
            openOptionsBottle.fromString(openOptions.toString());
            b.append(openOptionsBottle);
        }
        //response.addVocab(VOCAB_OK);
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "open" ) //-- open
    {
        std::string cmdStr;
        for(size_t i=1; i<request.size(); i++)
        {
            if(!request.get(i).isList())
            {
                CD_ERROR("Expected list at %d.\n",i);
                response.addVocab(VOCAB_FAILED);
                response.addString("Expected list");
                return response.write(*out);
            }
            yarp::os::Bottle* elem = request.get(i).asList();
            cmdStr.append("--");
            cmdStr.append(elem->toString());
            cmdStr.append(" ");
        }
        CD_DEBUG("%s\n", cmdStr.c_str());

        std::stringstream sout;
        std::stringstream sinput(cmdStr);

        if(!openraveYarpPluginLoaderPtr->Open(sout, sinput))
        {
            response.addVocab(VOCAB_FAILED);
            response.addString("Open failed");
            return response.write(*out);
        }
        response.addVocab(VOCAB_OK);
        int value;
        while(sout >> value)
        {
           response.addInt32(value);
        }
        return response.write(*out);
    }

    response.addVocab(VOCAB_FAILED);
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------

OpenPortPeriodicWrite::OpenPortPeriodicWrite() : yarp::os::PeriodicThread(1.0)
{
    PeriodicThread::start();
}

// -----------------------------------------------------------------------------

void OpenPortPeriodicWrite::run()
{
    if(!Port::isOpen())
        return;

    if(0 == openraveYarpPluginLoaderPtr->getYarpPluginsProperties().size())
        return;

    yarp::os::Bottle info;
    for (size_t i=0;i<openraveYarpPluginLoaderPtr->getYarpPluginsProperties().size();i++)
    {
        yarp::os::Bottle& b = info.addList();
        b.addInt32(i);
        yarp::os::Property openOptions(openraveYarpPluginLoaderPtr->getYarpPluginsProperties()[i]);
        openOptions.unput("penv");
        openOptions.unput("allManipulators");
        openOptions.unput("allSensors");
        yarp::os::Bottle openOptionsBottle;
        openOptionsBottle.fromString(openOptions.toString());
        b.append(openOptionsBottle);
    }
    Port::write(info);
}

// -----------------------------------------------------------------------------

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarppluginloader")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpPluginLoader(penv));
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
