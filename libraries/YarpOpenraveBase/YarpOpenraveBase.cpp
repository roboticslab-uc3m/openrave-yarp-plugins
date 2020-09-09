// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveBase.hpp"

#include <cassert>

#include <sstream>
#include <vector>

#include <yarp/os/Bottle.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <boost/bind/bind.hpp>

#include <ColorDebug.h>

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
    //CD_DEBUG("config: %s\n",config.toString().c_str());

    if ( ( config.check("env","path of environment") ) && ( config.check("penv","internal c++ pointer passed from OpenRAVE plugin to YARP device") ) )
    {
        CD_ERROR("Please do not use --env and --penv simultaneously. Bye!\n");
        return false;
    }

    if ( config.check("env") )
    {
        CD_DEBUG("Found --env parameter.\n");

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
            CD_SUCCESS("Loaded environment '%s'.\n",envString.c_str());
        }
        else
        {
            CD_DEBUG("Could not load '%s' environment, attempting via yarp::os::ResourceFinder.\n",envString.c_str());

            yarp::os::ResourceFinder rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
            std::string fullEnvString = rf.findFileByName(envString);

            if ( !penv->Load(fullEnvString.c_str()) )
            {
                CD_ERROR("Could not load '%s' environment.\n",fullEnvString.c_str());
                return false;
            }
            CD_SUCCESS("Loaded '%s' environment.\n",fullEnvString.c_str());
        }
    }
    else if ( config.check("penv") )
    {
        if(!config.find("penv").isBlob())
        {
            CD_ERROR("penv must be blob containing pointer to environment!\n");
            return false;
        }
        //CD_DEBUG("penv: %p\n",*((const OpenRAVE::EnvironmentBase**)(config.find("penv").asBlob())));
        penv = *((OpenRAVE::EnvironmentBasePtr*)(config.find("penv").asBlob()));
    }
    else
    {
        CD_ERROR("Please use --env or --penv parameter. Bye!\n");
        return false;
    }

    if( config.check("physics","activate physics engine: 'ode'|...") )
    {
        std::string physics = config.find("physics").asString();
        CD_DEBUG("Found --physics parameter: %s.\n", physics.c_str());
        penv->SetPhysicsEngine(OpenRAVE::RaveCreatePhysicsEngine(penv, physics));
        penv->GetPhysicsEngine()->SetGravity(OpenRAVE::Vector(0,0,-9.8));
    }

    if( config.check("collision","use collsion engine: 'ode'|...") )
    {
        std::string collision = config.find("collision").asString();
        CD_DEBUG("Found --collision parameter: %s.\n", collision.c_str());
        penv->SetCollisionChecker(OpenRAVE::RaveCreateCollisionChecker(penv, collision));
    }

    return true;
}

// -------------------------------------------------------------------

bool YarpOpenraveBase::configureOpenravePlugins(yarp::os::Searchable& config)
{
    //CD_DEBUG("config: %s\n",config.toString().c_str());

    if( ! penv )
    {
        CD_ERROR("Note to developers: please call configureEnvironment() first.\n");
        return false;
    }

    if( config.check("orPlugin","name of OpenRAVE plugin to load") )
    {
        CD_DEBUG("Found --orPlugin parameter.\n");

        std::string orPluginAndModuleName = config.find("orPlugin").asString();
        if ( ! OpenRAVE::RaveLoadPlugin(orPluginAndModuleName) )
        {
            CD_ERROR("Could not load plugin '%s'\n",orPluginAndModuleName.c_str());
            return false;
        }
        OpenRAVE::ModuleBasePtr pModule = OpenRAVE::RaveCreateModule(penv,orPluginAndModuleName); // create the module
        penv->Add(pModule,true); // load the module, calls main and also enables good destroy.
        std::stringstream cmdin,cmdout;
        cmdin << "open";
        // RAVELOG_INFO("%s\n",cmdin.str().c_str());
        if( ! pModule->SendCommand(cmdout,cmdin) )
        {
            CD_WARNING("Bad send 'open' command.\n");
            //return false;
        }
        CD_SUCCESS("Sent 'open' command.\n");
    }

    if( config.check("orPlugins","list of names of OpenRAVE plugin to load") )
    {
        CD_DEBUG("Found --orPlugins parameter.\n");

        if( ! config.find("orPlugins").isList() )
        {
            CD_ERROR("orPlugins usage from CLI: (read code) ->. Bye!\n");  //--orPlugins "(plugin1 (module module1) (commands \"open port\"))"
            return false;
        }
        yarp::os::Bottle orPlugins = config.findGroup("orPlugins");
        CD_DEBUG("orPlugins: %s\n",orPlugins.toString().c_str());
        for(int i=1; i<orPlugins.size();i++)  // elem(0) is "orPlugins"
        {
            if( ! orPlugins.get(i).isList() )
            {
                CD_ERROR("orPlugins usage from CLI: (read code) ->. Bye!\n");  //--orPlugins "(plugin1 (module module1) (commands \"open port\"))"
                return false;
            }
            yarp::os::Bottle* orPlugin = orPlugins.get(i).asList();
            CD_DEBUG("orPlugin[%d]: %s\n",i,orPlugin->toString().c_str());
            std::string orPluginName = orPlugin->get(0).asString();
            CD_DEBUG("* orPlugin[%d]: plugin: %s\n",i,orPluginName.c_str());
            std::string orModuleName = orPlugin->find("module").asString();
            CD_DEBUG("* orPlugin[%d]: module: %s\n",i,orModuleName.c_str());
            if( orPlugin->check("commands") )
            {
                CD_DEBUG("* orPlugin[%d]: commands: %s\n",i,orPlugin->find("commands").asString().c_str());
            }

            //-- Load plugin (docs say will reload if already loaded)
            if ( ! OpenRAVE::RaveLoadPlugin(orPluginName) )
            {
                CD_ERROR("Could not load plugin '%s'\n",orPluginName.c_str());
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
                    CD_ERROR("Bad send '%s' command.\n",cmdin.str().c_str());
                    return false;
                }
                CD_SUCCESS("Sent '%s' command.\n",cmdin.str().c_str());
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveBase::configureRobot(yarp::os::Searchable& config)
{
    //CD_DEBUG("config: %s\n",config.toString().c_str());

    if( ! penv )
    {
        CD_ERROR("Note to developers: please call configureEnvironment() first.\n");
        return false;
    }

    int robotIndex = config.check("robotIndex",yarp::os::Value(NOT_SET),"robot index").asInt32();

    if (robotIndex == NOT_SET)
    {
        CD_ERROR("robotIndex %d == NOT_SET, not loading yarpPlugin.\n",robotIndex);
        return false;
    }
    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    if((robotIndex >= vectorOfRobotPtr.size()) || (robotIndex < 0))
    {
        CD_ERROR("robotIndex %d not in vectorOfRobotPtr ofsize() %d, not loading yarpPlugin.\n",robotIndex,vectorOfRobotPtr.size());
        return false;
    }

    probot = vectorOfRobotPtr[robotIndex];
    robotName = probot->GetName();

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveBase::clean()
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
