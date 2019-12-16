// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveFTSensors.hpp"

#include <stdint.h> // uint8_t

#include <vector>

#include <yarp/sig/Image.h>

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ ISixAxisForceTorqueSensors Related ----------------------------------------

size_t YarpOpenraveFTSensors::getNrOfSixAxisForceTorqueSensors() const
{
    return 1;
}

// ----------------------------------------------------------------------------

yarp::dev::MAS_status YarpOpenraveFTSensors::getSixAxisForceTorqueSensorStatus(size_t sens_index) const
{
    return yarp::dev::MAS_OK;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveFTSensors::getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const
{
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveFTSensors::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const
{
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveFTSensors::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
{
    return true;
}

// ----------------------------------------------------------------------------

}
