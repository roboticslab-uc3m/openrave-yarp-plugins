// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

namespace roboticslab
{

// -------------------------------------------------------------------

void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername, int _viewer)
{
    OpenRAVE::ViewerBasePtr viewer = OpenRAVE::RaveCreateViewer(penv,viewername);
    BOOST_ASSERT(!!viewer);

    // attach it to the environment:
    penv->AddViewer(viewer);  // penv->AttachViewer(viewer);

    // finally you call the viewer's infinite loop (this is why you need a separate thread):
    bool showgui = true; // change to false to disable scene view
    if(!_viewer) showgui = false;  // if viewer arg = 0
    viewer->main(showgui);
}

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveControlboard::open(yarp::os::Searchable& config) {

    if ( ( config.check("env") ) && ( config.check("penv") ) )
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
        boost::thread thviewer(boost::bind(SetViewer,penv,"qtcoin",1));
        orThreads.add_thread(&thviewer);
        yarp::os::Time::delay(0.4); // wait for the viewer to init, in [s]


        // Actually load the scene
        std::string envFull = config.find("env").asString();

        if (! penv->Load(envFull.c_str()) ) {
            CD_ERROR("Could not load '%s' environment.\n",envFull.c_str());
            return false;
        }
        CD_SUCCESS("Loaded environment '%s'.\n",envFull.c_str());
    }
    else if ( config.check("penv") )
    {
        //CD_DEBUG("penv: %p\n",*((const OpenRAVE::EnvironmentBase**)(config.find("penv").asBlob())));
        penv = *((OpenRAVE::EnvironmentBasePtr*)(config.find("penv").asBlob()));
    }
    else
    {
        CD_ERROR("Please use --env or --penv parameter. Bye!\n");
        return false;
    }

    int robotIndex = config.check("robotIndex",-1,"robotIndex").asInt();
    if( robotIndex < 0 )  // a.k.a. -1 one line above
    {
        CD_ERROR("Please review robotIndex, currently '%d'.\n", robotIndex);
        return false;
    }
    int manipulatorIndex = config.check("manipulatorIndex",-1,"manipulatorIndex").asInt();
    if( manipulatorIndex < 0 )  // a.k.a. -1 one line above
    {
        CD_ERROR("Please review manipulatorIndex, currently '%d'.\n", manipulatorIndex);
        return false;
    }

    if( config.check("orPlugin") )
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
            CD_ERROR("Bad send 'open' command.\n");
        }
        CD_SUCCESS("Sent 'open' command.\n");
    }

    if( config.check("orPlugins") )
    {
        CD_DEBUG("Found --orPlugins parameter.\n");

        if( ! config.find("orPlugins").isList() )
        {
            CD_ERROR("orPlugins usage from CLI: --orPlugins \"((plugin1 module1 command1 arg11 arg12 ...) (plugin2 module2 command2 arg21 arg22 ...))\". Bye!\n");
            return false;
        }
        yarp::os::Bottle* orPlugins = config.find("orPlugins").asList();
        CD_DEBUG("orPlugins: %s\n",orPlugins->toString().c_str());
        for(int i=0; i<orPlugins->size();i++)
        {
            if( ! orPlugins->get(i).isList() )
            {
                CD_ERROR("orPlugins usage from CLI: --orPlugins \"((plugin1 module1 command1 arg11 arg12 ...) (plugin2 module2 command2 arg21 arg22 ...))\". Bye!\n");
                return false;
            }
            yarp::os::Bottle* orPlugin = orPlugins->get(i).asList();
            CD_DEBUG("orPlugin[%d]: %s\n",i,orPlugin->toString().c_str());
            /*for(int j=0; j<orPlugin->size();j++)
            {
                CD_DEBUG("* orPlugin[%d][%d]: %s\n",i,j,orPlugin->get(j).asString().c_str());
            }*/
            if( orPlugin->size() < 2 )
            {
                CD_ERROR("orPlugins needs at least plugin and module parameters.\n");
                CD_ERROR("orPlugins usage from CLI: --orPlugins \"((plugin1 module1 command1 arg11 arg12 ...) (plugin2 module2 command2 arg21 arg22 ...))\". Bye!\n");
                return false;
            }
            //-- Load plugin (docs say will reload if already loaded)
            std::string orPluginName = orPlugin->get(0).asString();
            if ( ! OpenRAVE::RaveLoadPlugin(orPluginName) )
            {
                CD_ERROR("Could not load plugin '%s'\n",orPluginName.c_str());
                return false;
            }
            //-- Load module from plugin
            std::string orModuleName = orPlugin->get(1).asString();
            OpenRAVE::ModuleBasePtr pModule = OpenRAVE::RaveCreateModule(penv,orPluginName); // create the module
            penv->Add(pModule,true); // load the module, calls main and also enables good destroy.
            //-- Send command if list big enough
            if( orPlugin->size() > 2 )
            {
                std::stringstream cmdin,cmdout;
                for(int j=2;j<orPlugin->size();j++)
                {
                    cmdin << orPlugin->get(j).asString() << " ";
                }
                // RAVELOG_INFO("%s\n",cmdin.str().c_str());
                if( ! pModule->SendCommand(cmdout,cmdin) )
                {
                    CD_ERROR("Bad send '%s' command.\n",cmdin.str().c_str());
                }
                CD_SUCCESS("Sent '%s' command.\n",cmdin.str().c_str());
            }
        }
    }

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    probot = vectorOfRobotPtr[robotIndex];

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();
    manipulatorTargets.resize( axes, 0.0 );
    controlModes.resize( axes, VOCAB_CM_POSITION );

    for(size_t i=0; i<manipulatorIDs.size(); i++)
    {
        OpenRAVE::RobotBase::JointPtr jointPtr = probot->GetJointFromDOFIndex(manipulatorIDs[i]);
        vectorOfJointPtr.push_back(jointPtr);
    }

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        pcontrol = OpenRAVE::RaveCreateController(penv,"idealcontroller");
        probot->SetController(pcontrol,manipulatorIDs,0); // control all manipulator joints
        penv->StopSimulation();
        penv->StartSimulation(0.01);
        probot->SetActiveDOFs(manipulatorIDs);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::close() {
    printf("[YarpOpenraveControlboard] close()\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab

