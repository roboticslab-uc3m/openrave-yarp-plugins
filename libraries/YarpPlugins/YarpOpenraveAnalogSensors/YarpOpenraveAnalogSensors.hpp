// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_ANALOG_SENSORS_HPP__
#define __YARP_OPENRAVE_ANALOG_SENSORS_HPP__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>

#include <openrave/openrave.h>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "YarpOpenraveBase.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveAnalogSensors
 *
 * @brief Contains roboticslab::YarpOpenraveAnalogSensors.
 */

/**
 * @ingroup YarpOpenraveAnalogSensors
 * @brief Implements the YARP_dev ISixAxisForceTorqueSensors, etc.
 * interface class member functions.
 */
class YarpOpenraveAnalogSensors : YarpOpenraveBase, public yarp::dev::DeviceDriver, public yarp::dev::ISixAxisForceTorqueSensors
{
public:

    // Set the Thread Rate in the class constructor
    YarpOpenraveAnalogSensors() {}

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // ------- ISixAxisForceTorqueSensors declarations. Implementation in ISixAxisForceTorqueSensorsImpl.cpp -------

    virtual size_t getNrOfSixAxisForceTorqueSensors() const override;
    virtual yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const override;
    virtual bool getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const override;
    virtual bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const override;
    virtual bool getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

private:

    // General Analog Sensors parameters //

    //OpenRAVE//
    std::vector<OpenRAVE::SensorBasePtr> vectorOfSensorPtrForForce6Ds;
    std::vector<boost::shared_ptr<OpenRAVE::SensorBase::Force6DSensorData>> vectorOfForce6DSensorDataPtr;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_ANALOG_SENSORS_HPP__
