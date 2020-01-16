// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpWorldRpcResponder
 *
 * @brief Contains roboticslab::OpenraveYarpWorldRpcResponder.
 */

/**
 * @ingroup OpenraveYarpWorldRpcResponder
 * @brief Opens YARP RpcPort, to control environment.
 */
class OpenraveYarpWorldRpcResponder : public OpenRAVE::ModuleBase
{
public:

    OpenraveYarpWorldRpcResponder(OpenRAVE::EnvironmentBasePtr penv);

    virtual ~OpenraveYarpWorldRpcResponder();
    virtual void Destroy();

    int main(const std::string& cmd);

    bool Open(std::ostream& sout, std::istream& sinput);

private:
    yarp::os::Network yarp;
    yarp::os::RpcServer worldRpcServer;
    OywrrPortReader processor;

    OpenRAVE::RobotBasePtr probot;
};
