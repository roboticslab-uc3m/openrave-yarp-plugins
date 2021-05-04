// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveBase.hpp"

#include <cassert>

#include <sstream>
#include <vector>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <boost/bind/bind.hpp>

namespace roboticslab
{

const int YarpOpenraveBase::NOT_SET = -1;

// -------------------------------------------------------------------

void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername)
{
    OpenRAVE::ViewerBasePtr viewer = OpenRAVE::RaveCreateViewer(penv,viewername);
    assert(!!viewer);

    // attach it to the environment:
    penv->AddViewer(viewer);  // penv->AttachViewer(viewer);

    // finally you call the viewer's infinite loop (this is why you need a separate thread):
    viewer->main(true);
}

// -------------------------------------------------------------------

YarpOpenraveBase::YarpOpenraveBase()
{
    penv = OpenRAVE::EnvironmentBasePtr();  // nullptr
}

// -------------------------------------------------------------------

bool YarpOpenraveBase::configureEnvironment(yarp::os::Searchable& config)
{
    if ( ( config.check("env","path of environment") ) && ( config.check("penv","internal c++ pointer passed from OpenRAVE plugin to YARP device") ) )
    {
        yError() << "Please do not use --env and --penv simultaneously";
        return false;
    }

    if ( config.check("env") )
    {
        yDebug() << "Found --env parameter";

        // Initialize OpenRAVE-core
        OpenRAVE::RaveInitialize(true);  // Start openrave core
        penv = OpenRAVE::RaveCreateEnvironment();  // Create the main OpenRAVE environment, set the EnvironmentBasePtr
        penv->StopSimulation();  // NEEDED??

        if ( config.check("view","open viewer") )
        {
            boost::thread openraveViewerThread(boost::bind(SetViewer,penv,"qtcoin"));
            openraveThreads.add_thread(&openraveViewerThread);
            yarp::os::Time::delay(0.4); // wait for the viewer to init, in [s]
        }

        // Actually load the scene
        std::string envString = config.find("env").asString();

        if ( !!penv->Load(envString.c_str()) )
        {
            yInfo() << "Loaded environment" << envString;
        }
        else
        {
            yDebug() << "Could not load" << envString << "environment, attempting via yarp::os::ResourceFinder";

            yarp::os::ResourceFinder rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
            std::string fullEnvString = rf.findFileByName(envString);

            if ( !penv->Load(fullEnvString.c_str()) )
            {
                yError() << "Could not load" << fullEnvString << "environment";
                return false;
            }
            yInfo() << "Loaded" << fullEnvString << "environment";
        }
    }
    else if ( config.check("penv") )
    {
        if(!config.find("penv").isBlob())
        {
            yError() << "penv must be blob containing pointer to environment";
            return false;
        }

        penv = *((OpenRAVE::EnvironmentBasePtr*)(config.find("penv").asBlob()));
    }
    else
    {
        yError() << "Please use --env or --penv parameter";
        return false;
    }

    if( config.check("physics","activate physics engine: 'ode'|...") )
    {
        std::string physics = config.find("physics").asString();
        yDebug() << "Found --physics parameter:" << physics;
        penv->SetPhysicsEngine(OpenRAVE::RaveCreatePhysicsEngine(penv, physics));
        penv->GetPhysicsEngine()->SetGravity(OpenRAVE::Vector(0,0,-9.8));
    }

    if( config.check("collision","use collsion engine: 'ode'|...") )
    {
        std::string collision = config.find("collision").asString();
        yDebug() << "Found --collision parameter:" << collision;
        penv->SetCollisionChecker(OpenRAVE::RaveCreateCollisionChecker(penv, collision));
    }

    return true;
}

// -------------------------------------------------------------------

bool YarpOpenraveBase::configureOpenravePlugins(yarp::os::Searchable& config)
{
    if( ! penv )
    {
        yError() << "Note to developers: please call configureEnvironment() first";
        return false;
    }

    if( config.check("orPlugin","name of OpenRAVE plugin to load") )
    {
        yDebug() << "Found --orPlugin parameter";

        std::string orPluginAndModuleName = config.find("orPlugin").asString();
        if ( ! OpenRAVE::RaveLoadPlugin(orPluginAndModuleName) )
        {
            yError() << "Could not load plugin" << orPluginAndModuleName;
            return false;
        }
        OpenRAVE::ModuleBasePtr pModule = OpenRAVE::RaveCreateModule(penv,orPluginAndModuleName); // create the module
        penv->Add(pModule,true); // load the module, calls main and also enables good destroy.
        std::stringstream cmdin,cmdout;
        cmdin << "open";
        // RAVELOG_INFO("%s\n",cmdin.str().c_str());
        if( ! pModule->SendCommand(cmdout,cmdin) )
        {
            yWarning() << "Bad send 'open' command";
            //return false;
        }
        yInfo() << "Sent 'open' command";
    }

    if( config.check("orPlugins","list of names of OpenRAVE plugin to load") )
    {
        yDebug() << "Found --orPlugins parameter";

        if( ! config.find("orPlugins").isList() )
        {
            yError() << "orPlugins usage from CLI: (read code) ->"; //--orPlugins "(plugin1 (module module1) (commands \"open port\"))"
            return false;
        }
        yarp::os::Bottle orPlugins = config.findGroup("orPlugins");
        yDebug() << "orPlugins:" << orPlugins.toString();
        for(int i=1; i<orPlugins.size();i++)  // elem(0) is "orPlugins"
        {
            if( ! orPlugins.get(i).isList() )
            {
                yError() << "orPlugins usage from CLI: (read code) ->"; //--orPlugins "(plugin1 (module module1) (commands \"open port\"))"
                return false;
            }
            yarp::os::Bottle* orPlugin = orPlugins.get(i).asList();
            yDebug("orPlugin[%d]: %s",i,orPlugin->toString().c_str());
            std::string orPluginName = orPlugin->get(0).asString();
            yDebug("* orPlugin[%d]: plugin: %s",i,orPluginName.c_str());
            std::string orModuleName = orPlugin->find("module").asString();
            yDebug("* orPlugin[%d]: module: %s",i,orModuleName.c_str());
            if( orPlugin->check("commands") )
            {
                yDebug("* orPlugin[%d]: commands: %s",i,orPlugin->find("commands").asString().c_str());
            }

            //-- Load plugin (docs say will reload if already loaded)
            if ( ! OpenRAVE::RaveLoadPlugin(orPluginName) )
            {
                yError() << "Could not load plugin" << orPluginName;
                return false;
            }

            //-- Load module from plugin
            OpenRAVE::ModuleBasePtr pModule = OpenRAVE::RaveCreateModule(penv,orModuleName); // create the module
            penv->Add(pModule,true); // load the module, calls main and also enables good destroy.
            //-- Send command if exist
            if( orPlugin->check("commands") )
            {
                std::stringstream cmdin,cmdout;
                cmdin << orPlugin->find("commands").asString();
                // RAVELOG_INFO("%s\n",cmdin.str().c_str());
                if( ! pModule->SendCommand(cmdout,cmdin) )
                {
                    yError("Bad send '%s' command",cmdin.str().c_str());
                    return false;
                }
                yInfo("Sent '%s' command",cmdin.str().c_str());
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveBase::configureRobot(yarp::os::Searchable& config)
{
    if( ! penv )
    {
        yError() << "Note to developers: please call configureEnvironment() first";
        return false;
    }

    int robotIndex = config.check("robotIndex",yarp::os::Value(NOT_SET),"robot index").asInt32();

    if (robotIndex == NOT_SET)
    {
        yError() << "robotIndex" << robotIndex << "== NOT_SET, not loading yarpPlugin";
        return false;
    }
    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    if((robotIndex >= vectorOfRobotPtr.size()) || (robotIndex < 0))
    {
        yError("robotIndex %d not in vectorOfRobotPtr ofsize() %zu, not loading yarpPlugin",robotIndex,vectorOfRobotPtr.size());
        return false;
    }

    probot = vectorOfRobotPtr[robotIndex];
    robotName = probot->GetName();

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveBase::clean()
{
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
