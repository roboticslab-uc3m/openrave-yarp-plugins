// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_SIMULATION_HPP__
#define __YARP_OPENRAVE_SIMULATION_HPP__

#include <yarp/dev/DeviceDriver.h>

#include <openrave/openrave.h>

#include "YarpOpenraveBase.hpp"

#include "../ISimulation.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveSimulation
 *
 * @brief Contains roboticslab::YarpOpenraveSimulation.
 */

/**
 * @ingroup YarpOpenraveSimulation
 * @brief Implements the ISimulation, etc.
 * interface class member functions.
 */
class YarpOpenraveSimulation : YarpOpenraveBase,
                               public yarp::dev::DeviceDriver,
                               public ISimulation
{
public:
    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // ------- ISimulation declarations. Implementation in ISimulationImpl.cpp -------
    bool step(double value) override;
    bool start(double value) override;
    bool stop() override;
    bool getSimulationRawPointerValue(yarp::os::Value& value) override;
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_SIMULATION_HPP__
