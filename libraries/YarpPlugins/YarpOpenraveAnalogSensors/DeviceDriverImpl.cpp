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

    if( ! config.check("ftSensorIndices","force6D sensor indices"))
    {
        CD_ERROR("Missing ftSensorIndices, not loading yarpPlugin.\n");
        return false;
    }

    if( ! config.find("ftSensorIndices").isList())
    {
        CD_ERROR("Expected ftSensorIndices as List, not loading yarpPlugin.\n");
        return false;
    }

    yarp::os::Bottle* ftSensorIndices = config.find("ftSensorIndices").asList();
    vectorOfSensorPtrForForce6Ds.resize(ftSensorIndices->size());
    vectorOfForce6DSensorDataPtr.resize(ftSensorIndices->size());

    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();
    for(size_t i=0; i<ftSensorIndices->size(); i++)
    {
        size_t ftSensorIndex = ftSensorIndices->get(i).asInt32();
        if( ftSensorIndex >= vectorOfSensorPtr.size() )
        {
            CD_ERROR("sensorIndex %d not within vectorOfSensorPtr of size() %d, not loading yarpPlugin.\n",i,vectorOfSensorPtr.size());
            return false;
        }

        vectorOfSensorPtrForForce6Ds[i] = vectorOfSensorPtr.at(ftSensorIndex)->GetSensor();

        std::string tipo = vectorOfSensorPtrForForce6Ds[i]->GetName();

        printf("Sensor %d name: %s\n",ftSensorIndex,tipo.c_str());

        // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());

        if ( ! vectorOfSensorPtrForForce6Ds[i]->Supports(OpenRAVE::SensorBase::ST_Force6D) )
        {
            CD_ERROR("Sensor %d does not support ST_Force6D.\n", ftSensorIndex );
            return false;
        }

        // Activate the sensor
        vectorOfSensorPtrForForce6Ds[i]->Configure(OpenRAVE::SensorBase::CC_PowerOn);

        // Get pointer to sensed data
        vectorOfForce6DSensorDataPtr[i] = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::Force6DSensorData>(vectorOfSensorPtrForForce6Ds[i]->CreateSensorData(OpenRAVE::SensorBase::ST_Force6D));
    }

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
