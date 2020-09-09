// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __I_SIMULATION_HPP__
#define __I_SIMULATION_HPP__

#include <yarp/os/Value.h>

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 *
  * @brief Base class for simulators.
 */
class ISimulation
{
public:

    /**
     * Start the simulation (time steps will be called internally and continuously by the simulator).
     * Simulation may default to already started, so may not be required upon initialization.
     * @param value duration that the simulation should use within internal continuous calls to its step function
     * @return true/false
     */
    virtual bool start(double value) = 0;

    /**
     * Stops the simulation.
     * @return true/false
     */
    virtual bool stop() = 0;

    /**
     * Take one simulation time step (to be used when the simulation is stopped / not started).
     * @param value duration of simulated time increment
     * @return true/false
     */
    virtual bool step(double value) = 0;

    /**
     * Retrieve a void pointer that can be casted to a world/environment pointer used to control any element of the simulation.
     * @param value storage to return param
     * @param id optional param to specify id in case there are multiple environments/worlds/simulations
     * @return true/false
     */
    virtual bool getSimulationRawPointerValue(yarp::os::Value& value) = 0;

    virtual ~ISimulation() {}
};

}

#endif //-- __I_SIMULATION_HPP__
