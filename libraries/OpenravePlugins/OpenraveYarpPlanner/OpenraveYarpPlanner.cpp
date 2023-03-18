// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpPlanner.hpp"

#include <boost/bind/bind.hpp>
#include <openrave/openrave.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// -----------------------------------------------------------------------------

OpenraveYarpPlanner::OpenraveYarpPlanner(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    using namespace boost::placeholders;
    __description = "OpenraveYarpPlanner plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpPlanner::Open, this,_1,_2),"opens OpenraveYarpPlanner");

    yCInfo(ORYP) << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
        yCError(ORYP) << "Found no yarp network (try running \"yarpserver &\")";
    yCInfo(ORYP) << "Found yarp network";

    oypPortReader.setOpenraveYarpPlannerPtr(this);
    oypRpcServer.setReader(oypPortReader);
    oypRpcServer.open("/OpenraveYarpPlanner/rpc:s");
}

// -----------------------------------------------------------------------------

OpenraveYarpPlanner::~OpenraveYarpPlanner()
{
    oypRpcServer.interrupt();

    oypRpcServer.close();
}

// -----------------------------------------------------------------------------

void OpenraveYarpPlanner::Destroy()
{
    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

int OpenraveYarpPlanner::main(const std::string& cmd)
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

bool OpenraveYarpPlanner::Open(std::ostream& sout, std::istream& sinput)
{
    yCInfo(ORYP) << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
    {
        yCError(ORYP) << "Found no yarp network (try running \"yarpserver &\")";
        sout << "-1 ";
        return false;
    }

    std::string s(std::istreambuf_iterator<char>(sinput), {});

    yarp::os::Property options;
    options.fromArguments(s.c_str());

    yCDebug(ORYP) << "Config:" << options.toString();

    return true;
}

// -----------------------------------------------------------------------------

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
OpenRAVE::InterfaceBasePtr OpenraveYarpPlannerPlugin::CreateInterface(OpenRAVE::InterfaceType type,
                                                                      const std::string & interfacename,
                                                                      std::istream & sinput,
                                                                      OpenRAVE::EnvironmentBasePtr penv)
{
    if (type == OpenRAVE::PT_Module && interfacename == "openraveyarpplanner")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpPlanner(penv));
    }

    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

const RavePlugin::InterfaceMap & OpenraveYarpPlannerPlugin::GetInterfaces() const
{
    static const RavePlugin::InterfaceMap interfaces = {
        {OpenRAVE::PT_Module, {"OpenraveYarpPlanner"}},
    };

    return interfaces;
}

// -----------------------------------------------------------------------------

const std::string & OpenraveYarpPlannerPlugin::GetPluginName() const
{
    static const std::string pluginName = "OpenraveYarpPlannerPlugin";
    return pluginName;
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API RavePlugin * CreatePlugin() {
    return new OpenraveYarpPlannerPlugin();
}
#else // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type,
                                                    const std::string & interfacename,
                                                    std::istream & sinput,
                                                    OpenRAVE::EnvironmentBasePtr penv)
{
    if (type == OpenRAVE::PT_Module && interfacename == "openraveyarpplanner")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpPlanner(penv));
    }

    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO & info)
{
    info.interfacenames[OpenRAVE::PT_Module].emplace_back("OpenraveYarpPlanner");
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
#endif // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)

// -----------------------------------------------------------------------------
