// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__
#define __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__

#include <mutex>
#include <vector>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>

namespace roboticslab
{

/**
 * @ingroup OpenraveYarpPluginLoaderClient
 */
class OyplCallbackPort : public yarp::os::BufferedPort<yarp::os::Bottle>
{
public:
    OyplCallbackPort();
    std::vector<int> availableIds;
    std::mutex availableIdsMutex;
    double lastTime;
private:
    void onRead(yarp::os::Bottle& b) override;
};

/**
 * @ingroup OpenraveYarpPluginLoaderClient
 */
class OpenraveYarpPluginLoaderClient : public yarp::os::RFModule
{
public:
    OpenraveYarpPluginLoaderClient();
    bool configure(yarp::os::ResourceFinder &rf) override;

private:
    yarp::os::RpcClient rpcClient;
    OyplCallbackPort callbackPort;
    std::vector<int> openedIds;

    bool openedInAvailable();
    bool detectedFirst;

    double getPeriod() override;
    bool updateModule() override;
    bool close() override;
};

} // namespace roboticslab

#endif // __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__
