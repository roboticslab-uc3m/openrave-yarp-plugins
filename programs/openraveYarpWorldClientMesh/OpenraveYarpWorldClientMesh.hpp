// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_WORLD_CLIENT_MESH_HPP__
#define __OPENRAVE_YARP_WORLD_CLIENT_MESH_HPP__

#include <mutex>
#include <string>
#include <vector>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>

namespace roboticslab
{

/**
 * @ingroup OpenraveYarpWorldClientMesh
 */
class OywCallbackPort : public yarp::os::BufferedPort<yarp::os::Bottle>
{
public:
    OywCallbackPort();
    std::vector<std::string> availableIds;
    std::mutex availableIdsMutex;
    double lastTime;

private:
    virtual void onRead(yarp::os::Bottle & b) override;
};

/**
 * @ingroup OpenraveYarpWorldClientMesh
 */
class OpenraveYarpWorldClientMesh : public yarp::os::RFModule
{
public:
    OpenraveYarpWorldClientMesh();
    bool configure(yarp::os::ResourceFinder & rf) override;

private:
    yarp::os::RpcClient rpcClient;
    OywCallbackPort callbackPort;
    std::string openedId;

    bool openedInAvailable();
    bool detectedFirst;

    double getPeriod() override;
    bool updateModule() override;
    bool close() override;
};

} // namespace roboticslab

#endif // __OPENRAVE_YARP_WORLD_CLIENT_MESH_HPP__
