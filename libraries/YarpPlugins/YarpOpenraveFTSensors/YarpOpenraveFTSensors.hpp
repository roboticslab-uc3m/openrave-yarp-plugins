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
 * \defgroup YarpOpenraveFTSensors
 *
 * @brief Contains roboticslab::YarpOpenraveFTSensors.
 */

/**
 * @ingroup YarpOpenraveFTSensors
 * @brief Implements the YARP_dev ISixAxisForceTorqueSensors, etc.
 * interface class member functions.
 */
class YarpOpenraveFTSensors : YarpOpenraveBase, public yarp::dev::DeviceDriver, public yarp::dev::ISixAxisForceTorqueSensors
{
public:

    // Set the Thread Rate in the class constructor
    YarpOpenraveFTSensors() {}

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


private:

    // General FTSensors parameters //

    //OpenRAVE//
    OpenRAVE::SensorBasePtr sensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> sensorDataPtr;

    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_FTSENSORS_HPP__
