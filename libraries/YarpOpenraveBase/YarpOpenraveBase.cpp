// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveBase.hpp"

#include <cassert>

#include <sstream>
#include <vector>

#include <openrave/config.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <boost/bind/bind.hpp>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(YORB, "rl.YarpOpenraveBase")
}

constexpr auto NOT_SET = -1;

// -------------------------------------------------------------------

void roboticslab::SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername)
{
    OpenRAVE::ViewerBasePtr viewer = OpenRAVE::RaveCreateViewer(penv,viewername);
    assert(!!viewer);

    // attach it to the environment:
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
    penv->Add(viewer, OpenRAVE::IAM_StrictNameChecking);
#else
    penv->AddViewer(viewer);
#endif

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
    if (config.check("env","path of environment") && config.check("penv", "internal c++ pointer passed from OpenRAVE plugin to YARP device"))
    {
        yCError(YORB) << "Please do not use --env and --penv simultaneously";
        return false;
    }

    if (config.check("env"))
    {
        yCDebug(YORB) << "Found --env parameter";

        // Initialize OpenRAVE-core
        OpenRAVE::RaveInitialize(true); // Start openrave core
        penv = OpenRAVE::RaveCreateEnvironment(); // Create the main OpenRAVE environment, set the EnvironmentBasePtr
        penv->StopSimulation(); // NEEDED??

        if (config.check("view","open viewer"))
        {
            boost::thread openraveViewerThread(boost::bind(SetViewer, penv,"qtcoin"));
            openraveThreads.add_thread(&openraveViewerThread);
            yarp::os::Time::delay(0.4); // wait for the viewer to init, in [s]
        }

        // Actually load the scene
        std::string envString = config.find("env").asString();

        if (!!penv->Load(envString.c_str()))
        {
            yCInfo(YORB) << "Loaded environment" << envString;
        }
        else
        {
            yCDebug(YORB) << "Could not load" << envString << "environment, attempting via yarp::os::ResourceFinder";

            yarp::os::ResourceFinder rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
            std::string fullEnvString = rf.findFileByName(envString);

            if (!penv->Load(fullEnvString.c_str()))
            {
                yCError(YORB) << "Could not load" << fullEnvString << "environment";
                return false;
            }

            yCInfo(YORB) << "Loaded" << fullEnvString << "environment";
        }
    }
    else if (config.check("penv"))
    {
        if (!config.find("penv").isBlob())
        {
            yCError(YORB) << "penv must be blob containing pointer to environment";
            return false;
        }

        penv = *((OpenRAVE::EnvironmentBasePtr*)(config.find("penv").asBlob()));
    }
    else
    {
        yCError(YORB) << "Please use --env or --penv parameter";
        return false;
    }

    if (config.check("physics", "activate physics engine: 'ode'|..."))
    {
        std::string physics = config.find("physics").asString();
        yCDebug(YORB) << "Found --physics parameter:" << physics;
        penv->SetPhysicsEngine(OpenRAVE::RaveCreatePhysicsEngine(penv, physics));
        penv->GetPhysicsEngine()->SetGravity(OpenRAVE::Vector(0, 0, -9.8));
    }

    if (config.check("collision","use collision engine: 'ode'|..."))
    {
        std::string collision = config.find("collision").asString();
        yCDebug(YORB) << "Found --collision parameter:" << collision;
        penv->SetCollisionChecker(OpenRAVE::RaveCreateCollisionChecker(penv, collision));
    }

    return true;
}

// -------------------------------------------------------------------

bool YarpOpenraveBase::configureOpenravePlugins(yarp::os::Searchable& config)
{
    if (!penv)
    {
        yCError(YORB) << "Note to developers: please call configureEnvironment() first";
        return false;
    }

    if (config.check("orPlugin","name of OpenRAVE plugin to load"))
    {
        yCDebug(YORB) << "Found --orPlugin parameter";

        std::string orPluginAndModuleName = config.find("orPlugin").asString();

        if (!OpenRAVE::RaveLoadPlugin(orPluginAndModuleName))
        {
            yCError(YORB) << "Could not load plugin" << orPluginAndModuleName;
            return false;
        }

        OpenRAVE::ModuleBasePtr pModule = OpenRAVE::RaveCreateModule(penv, orPluginAndModuleName); // create the module
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
        penv->Add(pModule, OpenRAVE::IAM_AllowRenaming); // load the module, calls main and also enables good destroy.
#else
        penv->Add(pModule, true); // load the module, calls main and also enables good destroy.
#endif
        std::stringstream cmdin,cmdout;
        cmdin << "open";
        // RAVELOG_INFO("%s\n",cmdin.str().c_str());
        if (!pModule->SendCommand(cmdout, cmdin))
        {
            yCWarning(YORB) << "Bad send 'open' command";
            //return false;
        }
        yCInfo(YORB) << "Sent 'open' command";
    }

    if (config.check("orPlugins", "list of names of OpenRAVE plugin to load"))
    {
        yCDebug(YORB) << "Found --orPlugins parameter";

        if (!config.find("orPlugins").isList())
        {
            yCError(YORB) << "orPlugins usage from CLI: (read code) ->"; //--orPlugins "(plugin1 (module module1) (commands \"open port\"))"
            return false;
        }

        yarp::os::Bottle orPlugins = config.findGroup("orPlugins");
        yCDebug(YORB) << "orPlugins:" << orPlugins.toString();

        for (int i = 1; i < orPlugins.size(); i++) // elem(0) is "orPlugins"
        {
            if (!orPlugins.get(i).isList())
            {
                yCError(YORB) << "orPlugins usage from CLI: (read code) ->"; //--orPlugins "(plugin1 (module module1) (commands \"open port\"))"
                return false;
            }

            yarp::os::Bottle* orPlugin = orPlugins.get(i).asList();
            yCDebug(YORB, "orPlugin[%d]: %s", i, orPlugin->toString().c_str());
            std::string orPluginName = orPlugin->get(0).asString();
            yCDebug(YORB, "* orPlugin[%d]: plugin: %s", i, orPluginName.c_str());
            std::string orModuleName = orPlugin->find("module").asString();
            yCDebug(YORB, "* orPlugin[%d]: module: %s", i, orModuleName.c_str());

            if (orPlugin->check("commands"))
            {
                yCDebug(YORB, "* orPlugin[%d]: commands: %s", i, orPlugin->find("commands").asString().c_str());
            }

            //-- Load plugin (docs say will reload if already loaded)
            if (!OpenRAVE::RaveLoadPlugin(orPluginName))
            {
                yCError(YORB) << "Could not load plugin" << orPluginName;
                return false;
            }

            //-- Load module from plugin
            OpenRAVE::ModuleBasePtr pModule = OpenRAVE::RaveCreateModule(penv,orModuleName); // create the module
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
            penv->Add(pModule, OpenRAVE::IAM_AllowRenaming); // load the module, calls main and also enables good destroy.
#else
            penv->Add(pModule, true); // load the module, calls main and also enables good destroy.
#endif
            //-- Send command if exist
            if (orPlugin->check("commands"))
            {
                std::stringstream cmdin,cmdout;
                cmdin << orPlugin->find("commands").asString();
                // RAVELOG_INFO("%s\n",cmdin.str().c_str());
                if (!pModule->SendCommand(cmdout,cmdin))
                {
                    yCError(YORB, "Bad send '%s' command", cmdin.str().c_str());
                    return false;
                }

                yCInfo(YORB, "Sent '%s' command", cmdin.str().c_str());
            }
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveBase::configureRobot(yarp::os::Searchable& config)
{
    if (!penv)
    {
        yCError(YORB) << "Note to developers: please call configureEnvironment() first";
        return false;
    }

    int robotIndex = config.check("robotIndex", yarp::os::Value(NOT_SET), "robot index").asInt32();

    if (robotIndex == NOT_SET)
    {
        yCError(YORB) << "robotIndex" << robotIndex << "== NOT_SET, not loading yarpPlugin";
        return false;
    }

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);

    if (robotIndex >= vectorOfRobotPtr.size() || robotIndex < 0)
    {
        yCError(YORB, "robotIndex %d not in vectorOfRobotPtr ofsize() %zu, not loading yarpPlugin", robotIndex, vectorOfRobotPtr.size());
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
