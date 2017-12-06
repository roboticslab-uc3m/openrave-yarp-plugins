// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRGBDSensor::open(yarp::os::Searchable& config) {

    CD_DEBUG("config: %s\n",config.toString().c_str());

    YarpOpenraveBase::configure(config);

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

    if ( ! sensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Laser) )
    {
        CD_ERROR("Sensor %d does not support ST_Laser.\n", sensorIndex );
    }

    // Activate the sensor
    sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Show the sensor image in a separate window // Ok for Laser???
    //sensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);

    // Get pointer to geom properties of sensor
    boost::shared_ptr<OpenRAVE::SensorBase::LaserGeomData const> geomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Laser));

    // Get pointer to sensed data
    sensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserSensorData>(sensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Laser));

    CD_INFO("Laser resolution: %f   %f.\n",geomDataPtr->resolution[0],geomDataPtr->resolution[1]);
    CD_INFO("Laser min_angle: %f   %f.\n",geomDataPtr->min_angle[0],geomDataPtr->min_angle[1]);
    CD_INFO("Laser max_angle: %f   %f.\n",geomDataPtr->max_angle[0],geomDataPtr->max_angle[1]);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::close() {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab

