// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <algorithm>  // Thanks: https://notfaq.wordpress.com/2007/08/04/cc-convert-string-to-upperlower-case/

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

    CD_DEBUG("config: %s\n",config.toString().c_str());

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

        if ( config.check("view") )
        {
            boost::thread openraveViewerThread(boost::bind(SetViewer,penv,"qtcoin",1));
            openraveThreads.add_thread(&openraveViewerThread);
            yarp::os::Time::delay(0.4); // wait for the viewer to init, in [s]
        }

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
                }
                CD_SUCCESS("Sent '%s' command.\n",cmdin.str().c_str());
            }
        }
    }

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    probot = vectorOfRobotPtr[robotIndex];
    robotName = probot->GetName();

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();
    manipulatorTargets.resize( axes, 0.0 );
    controlModes.resize( axes, VOCAB_CM_POSITION );

    for(size_t i=0; i<manipulatorIDs.size(); i++)
    {
        OpenRAVE::RobotBase::JointPtr jointPtr = probot->GetJointFromDOFIndex(manipulatorIDs[i]);
        vectorOfJointPtr.push_back(jointPtr);
        CD_DEBUG("Get JointPtr for manipulatorIDs[%d]: %d\n",i,manipulatorIDs[i]);
    }

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        std::vector<int> activeDOFIndices = probot->GetActiveDOFIndices();
        //--- Console output robot active DOF
        //for(size_t i=0; i<activeDOFIndices.size(); i++)
        //{
        //    CD_DEBUG("activeDOFIndices[%d]: %d\n",i,activeDOFIndices[i]);
        //}

        //-- Convert robot controller to multi if not already.
        OpenRAVE::ControllerBasePtr pcontrol = probot->GetController();
        CD_DEBUG("pcontrol: %p, %s\n",pcontrol.get(),pcontrol->GetXMLId().c_str());
        //-- Doing case insensitive check because defaults to IdealController but idealcontroller exists
        std::string controllerName( pcontrol->GetXMLId() );
        std::transform(controllerName.begin(), controllerName.end(), controllerName.begin(), ::tolower);
        if( controllerName == "idealcontroller" )
        {
            CD_INFO("Detected idealcontroller, switch to genericmulticontroller.\n");
            pcontrol = OpenRAVE::RaveCreateMultiController(penv);
            probot->SetController(pcontrol,activeDOFIndices,1);  // idealcontroller -> genericmulticontroller
        }
        else if( controllerName == "genericmulticontroller")
        {
            CD_INFO("Detected genericmulticontroller, which will be used.\n");
        }
        else
        {
            CD_ERROR("Non-treated controller case. Bye!\n");
            return false;
        }
        CD_DEBUG("pcontrol: %p, %s\n",pcontrol.get(),pcontrol->GetXMLId().c_str());

        //-- Safe to assume we have a multicontroller, store for usage.
        multi = boost::dynamic_pointer_cast< OpenRAVE::MultiControllerBase >(pcontrol);

        for(size_t i=0; i<manipulatorIDs.size(); i++)
        {
            OpenRAVE::ControllerBasePtr pindivcontrol = OpenRAVE::RaveCreateController(penv,"idealcontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            std::vector<int> tmpIndices;
            tmpIndices.push_back( manipulatorIDs[i] );
            CD_DEBUG("Attach individual controller for manipulatorIDs[%d]: %d\n",i,tmpIndices[0]);
            multi->AttachController(pindivcontrol, tmpIndices, 0);
            pcontrols.push_back(pindivcontrol);
        }

        penv->StopSimulation();
        penv->StartSimulation(0.01);

        //-- Console output of the robot ConfigurationSpecification
        //OpenRAVE::ConfigurationSpecification activeConfigurationSpecification = probot->GetActiveConfigurationSpecification();
        //for (size_t i = 0; i < activeConfigurationSpecification._vgroups.size(); i++)
        //{
        //    CD_DEBUG("%d, %s, %s\n",i,activeConfigurationSpecification._vgroups[i].name.c_str(), activeConfigurationSpecification._vgroups[i].interpolation.c_str());
        //}

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

