// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_WORLD_HPP__
#define __OPENRAVE_YARP_WORLD_HPP__

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <openrave/openrave.h>
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
# include <openrave/plugin.h>
#endif

#include "OywPortReader.hpp"
#include "OywPeriodicWrite.hpp"

namespace roboticslab
{

/**
 * @ingroup OpenravePlugins
 * @defgroup OpenraveYarpWorld
 * @brief Contains roboticslab::OpenraveYarpWorld.
 */

/**
 * @ingroup OpenraveYarpWorld
 * @brief Opens YARP RpcPort, to control environment.
 */
class OpenraveYarpWorld : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpWorld(OpenRAVE::EnvironmentBasePtr penv);

    ~OpenraveYarpWorld() override;
    virtual void Destroy() override;

    bool addWorldInfo(yarp::os::Bottle & info);
    int main(const std::string & cmd) override;

    bool Open(std::ostream & sout, std::istream & sinput);

private:
    yarp::os::Network yarp;

    OywPortReader oywPortReader;
    yarp::os::RpcServer oywRpcServer;

    OywPeriodicWrite oywPeriodicWrite;
};

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
/**
 * @ingroup OpenraveYarpWorld
 * @brief OpenraveYarpWorld plugin.
 */
class OpenraveYarpWorldPlugin : public RavePlugin
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

#endif // __OPENRAVE_YARP_WORLD_HPP__
