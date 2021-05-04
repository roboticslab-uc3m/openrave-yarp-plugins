// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include "OpenraveYarpPlanner.hpp"

// -----------------------------------------------------------------------------

roboticslab::OpenraveYarpPlanner::OpenraveYarpPlanner(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    __description = "OpenraveYarpPlanner plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPlanner::Open, this,_1,_2),"opens OpenraveYarpPlanner");

    yInfo() << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
        yError() << "Found no yarp network (try running \"yarpserver &\")";
    yInfo() << "Found yarp network";

    oypPortReader.setOpenraveYarpPlannerPtr(this);
    oypRpcServer.setReader(oypPortReader);
    oypRpcServer.open("/OpenraveYarpPlanner/rpc:s");
}

// -----------------------------------------------------------------------------

roboticslab::OpenraveYarpPlanner::~OpenraveYarpPlanner()
{
    oypRpcServer.interrupt();

    oypRpcServer.close();
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
