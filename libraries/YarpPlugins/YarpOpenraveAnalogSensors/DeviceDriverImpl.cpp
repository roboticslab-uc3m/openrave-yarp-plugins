// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveAnalogSensors.hpp"

#include <string>

#include <yarp/os/Value.h>

#include <ColorDebug.h>

namespace roboticslab
{

const int YarpOpenraveAnalogSensors::NOT_SET = -1;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveAnalogSensors::open(yarp::os::Searchable& config)
{
    CD_DEBUG("config: %s\n",config.toString().c_str());

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    int sensorIndex = config.check("sensorIndex",yarp::os::Value(NOT_SET),"sensor index").asInt32();

    if (sensorIndex == NOT_SET)
    {
        CD_ERROR("sensorIndex %d == NOT_SET, not loading yarpPlugin.\n",sensorIndex);
        return false;
    }
    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();
    if( (sensorIndex >= vectorOfSensorPtr.size()) || (sensorIndex < 0) )
    {
        CD_ERROR("sensorIndex %d not within vectorOfSensorPtr of size() %d, not loading yarpPlugin.\n",sensorIndex,vectorOfSensorPtr.size());
        return false;
    }

    vectorOfSensorPtrForForce6Ds.resize(1);
    vectorOfForce6DSensorDataPtr.resize(1);

    vectorOfSensorPtrForForce6Ds[0] = vectorOfSensorPtr.at(sensorIndex)->GetSensor();

    std::string tipo = vectorOfSensorPtrForForce6Ds[0]->GetName();

    printf("Sensor %d name: %s\n",sensorIndex,tipo.c_str());

    // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());

    if ( ! vectorOfSensorPtrForForce6Ds[0]->Supports(OpenRAVE::SensorBase::ST_Force6D) )
    {
        CD_ERROR("Sensor %d does not support ST_Force6D.\n", sensorIndex );
        return false;
    }

    // Activate the sensor
    vectorOfSensorPtrForForce6Ds[0]->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Get pointer to sensed data
    vectorOfForce6DSensorDataPtr[0] = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::Force6DSensorData>(vectorOfSensorPtrForForce6Ds[0]->CreateSensorData(OpenRAVE::SensorBase::ST_Force6D));

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveAnalogSensors::close()
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
