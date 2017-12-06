// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveBase.hpp"

#include "ColorDebug.hpp"

namespace roboticslab
{

// -------------------------------------------------------------------

void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername)
{
    OpenRAVE::ViewerBasePtr viewer = OpenRAVE::RaveCreateViewer(penv,viewername);
    BOOST_ASSERT(!!viewer);

    // attach it to the environment:
    penv->AddViewer(viewer);  // penv->AttachViewer(viewer);

    // finally you call the viewer's infinite loop (this is why you need a separate thread):
    viewer->main(true);
}

// -------------------------------------------------------------------

bool YarpOpenraveBase::configure(yarp::os::Searchable& config) {

    CD_DEBUG("config: %s\n",config.toString().c_str());

    int robotIndex = config.check("robotIndex",-1,"robotIndex").asInt();
    int sensorIndex = config.check("sensorIndex",-1,"sensorIndex").asInt();

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
            boost::thread openraveViewerThread(boost::bind(SetViewer,penv,"qtcoin"));
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
    if(robotIndex >= vectorOfRobotPtr.size())
    {
        CD_ERROR("robotIndex %d >= vectorOfRobotPtr.size() %d, not loading yarpPlugin.\n",robotIndex,vectorOfRobotPtr.size());
        return false;
    }
    else if (robotIndex < 0)
    {
        CD_ERROR("robotIndex %d < 0, not loading yarpPlugin.\n",robotIndex);
        return false;
    }

    probot = vectorOfRobotPtr[robotIndex];
    robotName = probot->GetName();

    //-- Sensor-specific part begins

    /*std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = vectorOfRobotPtr.at(robotIndex)->GetAttachedSensors();
    if(sensorIndex >= vectorOfSensorPtr.size())
    {
        CD_ERROR("sensorIndex %d >= vectorOfSensorPtr.size() %d, not loading yarpPlugin.\n",sensorIndex,vectorOfSensorPtr.size());
        return false;
    }
    else if (sensorIndex < 0)
    {
        CD_ERROR("sensorIndex %d < 0, not loading yarpPlugin.\n",sensorIndex);
        return false;
    }

    sensorBasePtr = vectorOfSensorPtr.at(sensorIndex)->GetSensor();

    std::string tipo = sensorBasePtr->GetName();

    printf("Sensor %d name: %s\n",sensorIndex,tipo.c_str());

    // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());

    if ( ! sensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Camera) )
    {
        CD_ERROR("Sensor %d does not support ST_Camera.\n", sensorIndex );
    }

    // Activate the sensor
    sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Show the sensor image in a separate window
    //sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);*/

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveBase::clean() {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab

