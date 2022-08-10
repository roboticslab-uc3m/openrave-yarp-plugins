// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"
#include "OpenraveYarpWorld.hpp"

using namespace roboticslab;

// -----------------------------------------------------------------------------

OpenraveYarpWorld::OpenraveYarpWorld(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    using namespace boost::placeholders;
    __description = "OpenraveYarpWorld plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpWorld::Open, this,_1,_2),"opens OpenraveYarpWorld");

    yCInfo(ORYW) << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
        yCError(ORYW) << "Found no yarp network (try running \"yarpserver &\")";
    yCInfo(ORYW) << "Found yarp network";
}

// -----------------------------------------------------------------------------

OpenraveYarpWorld::~OpenraveYarpWorld()
{
    oywRpcServer.interrupt();
    oywPeriodicWrite.interrupt();

    oywRpcServer.close();
    oywPeriodicWrite.close();
}

// -----------------------------------------------------------------------------

void OpenraveYarpWorld::Destroy()
{

    RAVELOG_INFO("module unloaded from environment\n");
}

// -----------------------------------------------------------------------------

bool OpenraveYarpWorld::addWorldInfo(yarp::os::Bottle& info)
{
    //-- Robots
    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    GetEnv()->GetRobots(vectorOfRobotPtr);

    for(size_t robotPtrIdx=0; robotPtrIdx<vectorOfRobotPtr.size(); robotPtrIdx++)
    {
        yarp::os::Bottle& robotList = info.addList();
        robotList.addString("robot");
        robotList.addString(vectorOfRobotPtr[robotPtrIdx]->GetName());

        std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[robotPtrIdx]->GetManipulators();
        for(size_t manipulatorPtrIdx=0; manipulatorPtrIdx<vectorOfManipulatorPtr.size(); manipulatorPtrIdx++)
        {
            yarp::os::Bottle& manipulatorList = robotList.addList();
            manipulatorList.addString("manipulator");
            manipulatorList.addString(vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName());
        }
    }

    // -- KinBodies
    std::vector<OpenRAVE::KinBodyPtr> vectorOfBodiesPtr;
    GetEnv()->GetBodies(vectorOfBodiesPtr);

    for(size_t bodiesPtrIdx=0; bodiesPtrIdx<vectorOfBodiesPtr.size(); bodiesPtrIdx++)
    {
        yarp::os::Bottle& kinbodyList = info.addList();
        kinbodyList.addString("kinbody");
        kinbodyList.addString(vectorOfBodiesPtr[bodiesPtrIdx]->GetName());
    }
    return true;
}

// -----------------------------------------------------------------------------

int OpenraveYarpWorld::main(const std::string& cmd)
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

bool OpenraveYarpWorld::Open(std::ostream& sout, std::istream& sinput)
{
    yCInfo(ORYW) << "Checking for yarp network...";
    if ( ! yarp.checkNetwork() )
    {
        yCError(ORYW) << "Found no yarp network (try running \"yarpserver &\")";
        sout << "-1 ";
        return false;
    }

    std::string s(std::istreambuf_iterator<char>(sinput), {});

    yarp::os::Property options;
    options.fromArguments(s.c_str());

    yCDebug(ORYW) << "Config:" << options.toString();

    yCInfo(ORYW) << "penv:" << GetEnv().get();
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();

    //-- PortReader and RpcServer
    oywPortReader.setOpenraveYarpWorldPtr(this);
    oywRpcServer.open("/OpenraveYarpWorld/rpc:s");
    oywRpcServer.setReader(oywPortReader);

    //-- PeriodicWrite
    oywPeriodicWrite.setOpenraveYarpWorldPtr(this);
    oywPeriodicWrite.open("/OpenraveYarpWorld/state:o");

    return true;
}

// -----------------------------------------------------------------------------

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpworld" )
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpWorld(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].emplace_back("OpenraveYarpWorld");
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}

// -----------------------------------------------------------------------------
