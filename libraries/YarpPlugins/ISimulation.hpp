// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __I_SIMULATION_HPP__
#define __I_SIMULATION_HPP__

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

        virtual bool step(double value) = 0;
        virtual bool start(double value) = 0;
        virtual bool stop() = 0;

        virtual ~ISimulation() {}
};

}

#endif //-- __I_SIMULATION_HPP__
