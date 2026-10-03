// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveAnalogSensors.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ ISixAxisForceTorqueSensors Related ----------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
yarp::dev::ReturnValue YarpOpenraveAnalogSensors::getNrOfSixAxisForceTorqueSensors(std::size_t & num) const
{
    num = vectorOfSensorPtrForForce6Ds.size();
    return yarp::dev::ReturnValue_ok;
}
#else
std::size_t YarpOpenraveAnalogSensors::getNrOfSixAxisForceTorqueSensors() const
{
    return vectorOfSensorPtrForForce6Ds.size();
}
#endif

// ----------------------------------------------------------------------------

yarp::dev::MAS_status YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorStatus(size_t sens_index) const
{
    return yarp::dev::MAS_OK;
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
yarp::dev::ReturnValue YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const
#else
bool YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const
#endif
{
    name = vectorOfSensorPtrForForce6Ds[sens_index]->GetName();
#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
yarp::dev::ReturnValue YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const
#else
bool YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
yarp::dev::ReturnValue YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
#else
bool YarpOpenraveAnalogSensors::getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const
#endif
{
    if (!vectorOfSensorPtrForForce6Ds[sens_index]->GetSensorData(vectorOfForce6DSensorDataPtr[sens_index]))
    {
        yCDebug(YORAS) << "GetSensorData() failed";
#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
        return yarp::dev::ReturnValue_error_method_failed;
#else
        return false;
#endif
    }

    out.resize(6);
    out[0] = vectorOfForce6DSensorDataPtr[sens_index]->force[0];
    out[1] = vectorOfForce6DSensorDataPtr[sens_index]->force[1];
    out[2] = vectorOfForce6DSensorDataPtr[sens_index]->force[2];
    out[3] = vectorOfForce6DSensorDataPtr[sens_index]->torque[0];
    out[4] = vectorOfForce6DSensorDataPtr[sens_index]->torque[1];
    out[5] = vectorOfForce6DSensorDataPtr[sens_index]->torque[2];
#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------
