// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveAnalogSensors.hpp"

#include <string>

#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveAnalogSensors::open(yarp::os::Searchable& config)
{
    if (!configureEnvironment(config))
        return false;

    if (!configureOpenravePlugins(config))
        return false;

    if (!configureRobot(config))
        return false;

    if (!config.check("ftSensorIndices", "force6D sensor indices"))
    {
        yCError(YORAS) << "Missing ftSensorIndices, not loading yarpPlugin";
        return false;
    }

    yarp::os::Bottle ftSensorIndices = config.findGroup("ftSensorIndices").tail();
    vectorOfSensorPtrForForce6Ds.resize(ftSensorIndices.size());
    vectorOfForce6DSensorDataPtr.resize(ftSensorIndices.size());

    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();

    for (size_t i = 0; i < ftSensorIndices.size(); i++)
    {
        size_t ftSensorIndex = ftSensorIndices.get(i).asInt32();

        if (ftSensorIndex >= vectorOfSensorPtr.size())
        {
            yCError(YORAS, "Sensor with ftSensorIndex %zu not within vectorOfSensorPtr of size() %zu, not loading yarpPlugin", ftSensorIndex, vectorOfSensorPtr.size());
            return false;
        }

        vectorOfSensorPtrForForce6Ds[i] = vectorOfSensorPtr[ftSensorIndex]->GetSensor();

        std::string ftSensorName = vectorOfSensorPtrForForce6Ds[i]->GetName();

        yCInfo(YORAS, "Name of sensor with ftSensorIndex %zu: '%s'", ftSensorIndex, ftSensorName.c_str());

        // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());

        if (!vectorOfSensorPtrForForce6Ds[i]->Supports(OpenRAVE::SensorBase::ST_Force6D))
        {
            yCError(YORAS) << "Sensor with ftSensorIndex" << ftSensorIndex << "does not support ST_Force6D";
            return false;
        }

        // Activate the sensor
        vectorOfSensorPtrForForce6Ds[i]->Configure(OpenRAVE::SensorBase::CC_PowerOn);

        // Get pointer to sensed data
        vectorOfForce6DSensorDataPtr[i] = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::Force6DSensorData>(vectorOfSensorPtrForForce6Ds[i]->CreateSensorData(OpenRAVE::SensorBase::ST_Force6D));
    }

    penv->StopSimulation();
    penv->StartSimulation(0.01);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveAnalogSensors::close()
{
    return true;
}

// -----------------------------------------------------------------------------
