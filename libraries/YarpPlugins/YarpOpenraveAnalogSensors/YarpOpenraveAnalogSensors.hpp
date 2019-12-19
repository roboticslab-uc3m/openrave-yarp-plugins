// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_FTSENSORS_HPP__
#define __YARP_OPENRAVE_FTSENSORS_HPP__

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
    /**
     * Open the DeviceDriver.
     * @param config is a list of parameters for the device.
     * Which parameters are effective for your device can vary.
     * See \ref dev_examples "device invocation examples".
     * If there is no example for your device,
     * you can run the "yarpdev" program with the verbose flag
     * set to probe what parameters the device is checking.
     * If that fails too,
     * you'll need to read the source code (please nag one of the
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config);

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close();

    // ------- ISixAxisForceTorqueSensors declarations. Implementation in ISixAxisForceTorqueSensorsImpl.cpp -------
    /**
     * Get the number of six axis force torque sensors exposed by this device.
     */
    virtual size_t getNrOfSixAxisForceTorqueSensors() const override;

    /**
     * Get the status of the specified sensor.
     */
    virtual yarp::dev::MAS_status getSixAxisForceTorqueSensorStatus(size_t sens_index) const override;

    /**
     * Get the name of the specified sensor.
     */
    virtual bool getSixAxisForceTorqueSensorName(size_t sens_index, std::string &name) const override;

    /**
     * Get the name of the frame of the specified sensor.
     */
    virtual bool getSixAxisForceTorqueSensorFrameName(size_t sens_index, std::string &frameName) const override;

    /**
     * Get the last reading of the specified sensor.
     *
     * @param[in] sens_index The index of the specified sensor (should be between 0 and getNrOfSixAxisForceTorqueSensors()-1).
     * @param[out] out The requested measure. The vector should be 6-dimensional.
     *                 The measure is expressed in Newton for the first three elements, Newton Meters for the last three elements.
     * @param[out] timestamp The timestamp of the requested measure, expressed in seconds.
     */
    virtual bool getSixAxisForceTorqueSensorMeasure(size_t sens_index, yarp::sig::Vector& out, double& timestamp) const override;

private:

    // General FTSensors parameters //

    //OpenRAVE//
    OpenRAVE::SensorBasePtr sensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::Force6DSensorData> sensorDataPtr;

    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_FTSENSORS_HPP__
