// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <ColorDebug.h>

#include "OpenraveYarpWorld.hpp"

// -----------------------------------------------------------------------------

OpenraveYarpWorld::OpenraveYarpWorld(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
{
    __description = "OpenraveYarpWorld plugin.";
    OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpWorld::Open, this,_1,_2),"opens OpenraveYarpWorld");

    CD_INFO("Checking for yarp network...\n");
    if ( ! yarp.checkNetwork() )
        CD_ERROR("Found no yarp network (try running \"yarpserver &\")!\n");
    CD_SUCCESS("Found yarp network.\n");
}

// -----------------------------------------------------------------------------

OpenraveYarpWorld::~OpenraveYarpWorld()
{
    oywRpcServer.interrupt();
    oywRpcServer.close();
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
    /*

    for(size_t robotPtrIdx=0; robotPtrIdx<vectorOfRobotPtr.size(); robotPtrIdx++)
    {
        info << "Robot ["<< robotPtrIdx <<"]"<<" named ["<<vectorOfRobotPtr[robotPtrIdx]->GetName().c_str()<<"]";
        info <<" with manipulators: ";

        //-- Get manipulators
        std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[robotPtrIdx]->GetManipulators();

        //-- For each manipulator
        for(size_t manipulatorPtrIdx=0; manipulatorPtrIdx<vectorOfManipulatorPtr.size(); manipulatorPtrIdx++)
        {
            info <<"("<<manipulatorPtrIdx<<")"<<vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName().c_str()<<" ";
        }
    }

    info << " // "; // -- to separate information!!

    // -- Get bodies
    std::vector<OpenRAVE::KinBodyPtr> vectorOfBodiesPtr;
    pEnv->GetBodies(vectorOfBodiesPtr);

    info << "Total bodies in the environment: ";

    // -- For each body
    for(size_t bodiesPtrIdx=0; bodiesPtrIdx<vectorOfBodiesPtr.size(); bodiesPtrIdx++)
    {
        info << "("<< bodiesPtrIdx <<")"<<vectorOfBodiesPtr[bodiesPtrIdx]->GetName().c_str()<<" ";
    }*/


    /*for (size_t i=0;i<yarpPluginsProperties.size();i++)
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
    }*/
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

    CD_INFO("penv: %p\n",GetEnv().get());
    OpenRAVE::EnvironmentBasePtr penv = GetEnv();

    //-- Get the robot
    std::vector<OpenRAVE::RobotBasePtr> robots;
    penv->GetRobots(robots);
    //-- Robot 0
    probot = robots.at(0);  // which is a RobotBasePtr
    CD_INFO("Using robot 0 (%s) as main robot.\n", probot->GetName().c_str());

    //-- PortReader and RpcServer
    oywPortReader.setEnvironment(penv);
    oywPortReader.setRobot(probot);
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
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpworldrpcresponder" )
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpWorld(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpWorld");
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}

// -----------------------------------------------------------------------------
