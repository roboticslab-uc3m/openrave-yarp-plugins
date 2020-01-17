// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_WORLD_HPP__
#define __OPENRAVE_YARP_WORLD_HPP__

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <openrave/openrave.h>

#include "OywPortReader.hpp"
#include "OywPeriodicWrite.hpp"

namespace roboticslab
{

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpWorld
 *
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

    virtual ~OpenraveYarpWorld();
    virtual void Destroy();

    bool addWorldInfo(yarp::os::Bottle& info);
    int main(const std::string& cmd);

    bool Open(std::ostream& sout, std::istream& sinput);

private:
    yarp::os::Network yarp;

    OywPortReader oywPortReader;
    yarp::os::RpcServer oywRpcServer;

    OywPeriodicWrite oywPeriodicWrite;

    OpenRAVE::RobotBasePtr probot;
};

} // namespace roboticslab

#endif // __OPENRAVE_YARP_WORLD_HPP__
