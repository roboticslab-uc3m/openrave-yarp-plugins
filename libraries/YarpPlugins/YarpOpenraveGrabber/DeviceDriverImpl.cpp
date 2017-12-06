// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveGrabber::open(yarp::os::Searchable& config)
{
    CD_DEBUG("config: %s\n",config.toString().c_str());

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    int robotIndex = config.check("robotIndex",-1,"robotIndex").asInt();
    int sensorIndex = config.check("sensorIndex",-1,"sensorIndex").asInt();

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

    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = vectorOfRobotPtr.at(robotIndex)->GetAttachedSensors();
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
    //sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);

    // Get pointer to geom properties of sensor
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> geomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));

    // Get pointer to sensed data
    sensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraSensorData>(sensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Camera));

    CD_INFO("Camera width: %d, height: %d.\n",geomDataPtr->width,geomDataPtr->height);
    _width = geomDataPtr->width;
    _height = geomDataPtr->height;

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveGrabber::close()
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab

