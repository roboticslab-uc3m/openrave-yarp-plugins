// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_GRABBER_HPP__
#define __YARP_OPENRAVE_GRABBER_HPP__

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

    YarpOpenraveSimulation() {}

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // ------- ISimulation declarations. Implementation in ISimulationImpl.cpp -------

    virtual bool step(double value);
    virtual bool start(double value);
    virtual bool stop();

private:

    // General Simulation parameters

    // OpenRAVE

};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_GRABBER_HPP__
