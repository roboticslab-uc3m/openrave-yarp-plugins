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
    RAVELOG_INFO("module initialized with \"%s\"\n", cmd.c_str());
    // hard-coding "open"
    std::istringstream sinput("open");
    std::ostringstream sout;
    if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
        return 1;
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
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpplanner")
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
