// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_ANALOG_SENSORS_HPP__
#define __YARP_OPENRAVE_ANALOG_SENSORS_HPP__

#include <yarp/conf/version.h>

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
class YarpOpenraveAnalogSensors : YarpOpenraveBase,
                                  public yarp::dev::DeviceDriver,
                                  public yarp::dev::ISixAxisForceTorqueSensors
{
public:
    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // ------- ISixAxisForceTorqueSensors declarations. Implementation in ISixAxisForceTorqueSensorsImpl.cpp -------
#if YARP_VERSION_COMPARE(>=, 4, 1, 0) || defined(YARP_NEXT)
    yarp::dev::ReturnValue getNrOfSixAxisForceTorqueSensors(std::size_t & num) const override;
    yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(std::size_t sens_index) const override;
    yarp::dev::ReturnValue getSixAxisForceTorqueSensorName(std::size_t sens_index, std::string & name) const override;
    yarp::dev::ReturnValue getSixAxisForceTorqueSensorFrameName(std::size_t sens_index, std::string & frameName) const override;
    yarp::dev::ReturnValue getSixAxisForceTorqueSensorMeasure(std::size_t sens_index, yarp::sig::Vector & out, double & timestamp) const override;
#else
    std::size_t getNrOfSixAxisForceTorqueSensors() const override;
    yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(std::size_t sens_index) const override;
    bool getSixAxisForceTorqueSensorName(std::size_t sens_index, std::string & name) const override;
    bool getSixAxisForceTorqueSensorFrameName(std::size_t sens_index, std::string & frameName) const override;
    bool getSixAxisForceTorqueSensorMeasure(std::size_t sens_index, yarp::sig::Vector & out, double & timestamp) const override;
#endif

private:
    //OpenRAVE//
    std::vector<OpenRAVE::SensorBasePtr> vectorOfSensorPtrForForce6Ds;
    std::vector<boost::shared_ptr<OpenRAVE::SensorBase::Force6DSensorData>> vectorOfForce6DSensorDataPtr;
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_ANALOG_SENSORS_HPP__
