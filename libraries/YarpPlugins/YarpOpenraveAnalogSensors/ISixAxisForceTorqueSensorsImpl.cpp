// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveAnalogSensors.hpp"

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ ISixAxisForceTorqueSensors Related ----------------------------------------

size_t YarpOpenraveAnalogSensors::getNrOfSixAxisForceTorqueSensors() const
{
    return vectorOfSensorPtrForForce6Ds.size();
}

// ----------------------------------------------------------------------------

yarp::dev::MAS_status YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorStatus(size_t sens_index) const
{
    return yarp::dev::MAS_OK;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const
{
    name = vectorOfSensorPtrForForce6Ds[sens_index]->GetName();
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    if(!vectorOfSensorPtrForForce6Ds[sens_index]->GetSensorData(vectorOfForce6DSensorDataPtr[sens_index]))
    {
        CD_DEBUG("GetSensorData fail...\n");
        return false;
    }

    out.resize(6);
    out[0] = vectorOfForce6DSensorDataPtr[sens_index]->force[0];
    out[1] = vectorOfForce6DSensorDataPtr[sens_index]->force[1];
    out[2] = vectorOfForce6DSensorDataPtr[sens_index]->force[2];
    out[3] = vectorOfForce6DSensorDataPtr[sens_index]->torque[0];
    out[4] = vectorOfForce6DSensorDataPtr[sens_index]->torque[1];
    out[5] = vectorOfForce6DSensorDataPtr[sens_index]->torque[2];
    return true;
}

// ----------------------------------------------------------------------------

}
