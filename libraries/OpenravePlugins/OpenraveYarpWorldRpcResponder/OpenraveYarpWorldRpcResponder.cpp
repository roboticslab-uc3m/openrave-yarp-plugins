// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include "OywrrPortReader.hpp"

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpWorldRpcResponder
 *
 * @brief Contains roboticslab::OpenraveYarpWorldRpcResponder.
 */

/**
 * @ingroup OpenraveYarpWorldRpcResponder
 * @brief Opens YARP RpcPort, to control environment.
 */
class OpenraveYarpWorldRpcResponder : public OpenRAVE::ModuleBase
{
public:

    OpenraveYarpWorldRpcResponder(OpenRAVE::EnvironmentBasePtr penv);

    virtual ~OpenraveYarpWorldRpcResponder();
    virtual void Destroy();

    int main(const std::string& cmd);

    bool Open(std::ostream& sout, std::istream& sinput);

private:
    yarp::os::Network yarp;
    yarp::os::RpcServer worldRpcServer;
    OywrrPortReader processor;

    OpenRAVE::RobotBasePtr probot;
};

// -----------------------------------------------------------------------------

OpenraveYarpWorldRpcResponder::OpenraveYarpWorldRpcResponder(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    __description = "OpenraveYarpWorldRpcResponder plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpWorldRpcResponder::Open, this,_1,_2),"opens OpenraveYarpWorldRpcResponder");
}

// -----------------------------------------------------------------------------

OpenraveYarpWorldRpcResponder::~OpenraveYarpWorldRpcResponder()
{
    worldRpcServer.interrupt();
    worldRpcServer.close();
}

// -----------------------------------------------------------------------------

void OpenraveYarpWorldRpcResponder::Destroy()
{

    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

int OpenraveYarpWorldRpcResponder::main(const std::string& cmd)
{
    RAVELOG_INFO("module initialized with \"%s\"\n", cmd.c_str());
    // hard-coding "open", note that actual Open enabled portName selection
    std::istringstream sinput("open");
    std::ostringstream sout;
    if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
        return 1;
    return 0;
}

// -----------------------------------------------------------------------------

bool OpenraveYarpWorldRpcResponder::Open(std::ostream& sout, std::istream& sinput)
{

    std::vector<std::string> funcionArgs;
    while(sinput)
    {
        std::string funcionArg;
        sinput >> funcionArg;
        funcionArgs.push_back(funcionArg);
    }

    std::string portName("/OpenraveYarpWorldRpcResponder/rpc:s");

    if (funcionArgs.size() > 0)
    {
        if( funcionArgs[0][0] == '/')
            portName = funcionArgs[0];
    }
    RAVELOG_INFO("portName: %s\n",portName.c_str());

    if ( !yarp.checkNetwork() )
    {
        RAVELOG_INFO("Found no yarp network (try running \"yarpserver &\"), bye!\n");
        return false;
    }


    RAVELOG_INFO("penv: %p\n",GetEnv().get());
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();


    //-- Get the robot
    std::vector<OpenRAVE::RobotBasePtr> robots;
    penv->GetRobots(robots);
    //-- Robot 0
    probot = robots.at(0);  // which is a RobotBasePtr
    std::printf("OpenRaveYarpWorldRpcResponder using robot 0 (%s) as main robot.\n", probot->GetName().c_str());

    //-- processor
    processor.setEnvironment(penv);
    processor.setRobot(probot);

    //-- world rpc server
    worldRpcServer.open(portName);
    worldRpcServer.setReader(processor);

    return true;

}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpworldrpcresponder" )
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpWorldRpcResponder(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpWorldRpcResponder");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
