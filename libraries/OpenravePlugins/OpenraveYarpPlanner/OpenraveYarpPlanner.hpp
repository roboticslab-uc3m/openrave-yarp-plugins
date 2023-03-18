// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_PLANNER_HPP__
#define __OPENRAVE_YARP_PLANNER_HPP__

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <yarp/dev/PolyDriver.h>

#include <openrave/openrave.h>
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
# include <openrave/plugin.h>
#endif

#include "OypPortReader.hpp"

namespace roboticslab
{

/**
 * @ingroup OpenravePlugins
 * @defgroup OpenraveYarpPlanner
 * @brief Contains roboticslab::OpenraveYarpPlanner.
 */

/**
 * @ingroup OpenraveYarpPlanner
 * @brief Loads one or several YARP Plugin, passing environment pointer.
 */
class OpenraveYarpPlanner : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpPlanner(OpenRAVE::EnvironmentBasePtr penv);
    ~OpenraveYarpPlanner() override;
    void Destroy() override;
    int main(const std::string & cmd) override;

    bool Open(std::ostream & sout, std::istream & sinput);

private:
    yarp::os::Network yarp;
    OypPortReader oypPortReader;
    yarp::os::RpcServer oypRpcServer;
};

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
/**
 * @ingroup OpenraveYarpPlanner
 * @brief OpenraveYarpPlanner plugin.
 */
class OpenraveYarpPlannerPlugin : public RavePlugin
{
public:
    OpenRAVE::InterfaceBasePtr CreateInterface(OpenRAVE::InterfaceType type,
                                               const std::string & interfacename,
                                               std::istream & sinput,
                                               OpenRAVE::EnvironmentBasePtr penv) override;

    const InterfaceMap & GetInterfaces() const override;
    const std::string & GetPluginName() const override;
};
#endif // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)

} // namespace roboticslab

#endif // __OPENRAVE_YARP_PLANNER_HPP__
