// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

#include <string>
#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include "LogComponent.hpp"

using namespace roboticslab;

constexpr auto NOT_SET = -1;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveGrabber::open(yarp::os::Searchable& config)
{
    yCDebug(YORG) << "Config:" << config.toString();

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    int sensorIndex = config.check("sensorIndex",yarp::os::Value(NOT_SET),"sensor index").asInt32();

    if (sensorIndex == NOT_SET)
    {
        yCError(YORG) << "sensorIndex" << sensorIndex << "== NOT_SET, not loading yarpPlugin";
        return false;
    }
    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();
    if( (sensorIndex >= vectorOfSensorPtr.size()) || (sensorIndex < 0) )
    {
        yCError(YORG, "sensorIndex %d not within vectorOfSensorPtr of size() %zu, not loading yarpPlugin",sensorIndex,vectorOfSensorPtr.size());
        return false;
    }

    sensorBasePtr = vectorOfSensorPtr.at(sensorIndex)->GetSensor();

    std::string sensorName = sensorBasePtr->GetName();

    yCInfo(YORG) << "Sensor" << sensorIndex << "name:" << sensorName;
    cameraDescriptor.deviceDescription = sensorName;
    cameraDescriptor.busType = BUS_UNKNOWN;

    // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());

    if ( ! sensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Camera) )
    {
        yCError(YORG) << "Sensor" << sensorIndex << "does not support ST_Camera";
        return false;
    }

    // Activate the sensor
    sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Show the sensor image in a separate window
    //sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);

    // Get pointer to geom properties of sensor
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> geomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));

    // Get pointer to sensed data
    sensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraSensorData>(sensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Camera));

    yCInfo(YORG, "Camera width: %d, height: %d",geomDataPtr->width,geomDataPtr->height);
    _width = geomDataPtr->width;
    _height = geomDataPtr->height;

    penv->StopSimulation();
    penv->StartSimulation(0.01);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveGrabber::close()
{
    return true;
}

// -----------------------------------------------------------------------------
