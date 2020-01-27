// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include <ColorDebug.h>

#include "OpenraveYarpPlanner.hpp"

// -----------------------------------------------------------------------------

roboticslab::OpenraveYarpPlanner::OpenraveYarpPlanner(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    __description = "OpenraveYarpPlanner plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPlanner::Open, this,_1,_2),"opens OpenraveYarpPlanner");

    CD_INFO("Checking for yarp network...\n");
    if ( ! yarp.checkNetwork() )
        CD_ERROR("Found no yarp network (try running \"yarpserver &\")!\n");
    CD_SUCCESS("Found yarp network.\n");

    oyplPortReader.setOpenraveYarpPlannerPtr(this);
    oyplRpcServer.setReader(oyplPortReader);
    oyplRpcServer.open("/OpenraveYarpPlanner/rpc:s");
}

// -----------------------------------------------------------------------------

roboticslab::OpenraveYarpPlanner::~OpenraveYarpPlanner()
{
    oyplRpcServer.interrupt();

    oyplRpcServer.close();
}

// -----------------------------------------------------------------------------

void roboticslab::OpenraveYarpPlanner::Destroy()
{
    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

int roboticslab::OpenraveYarpPlanner::main(const std::string& cmd)
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
}

// -----------------------------------------------------------------------------

bool roboticslab::OpenraveYarpPlanner::Open(std::ostream& sout, std::istream& sinput)
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
        return true;
    }
    return true;
}

// -----------------------------------------------------------------------------


OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarppluginloader")
    {
        return OpenRAVE::InterfaceBasePtr(new roboticslab::OpenraveYarpPlanner(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpPlanner");
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}

// -----------------------------------------------------------------------------
