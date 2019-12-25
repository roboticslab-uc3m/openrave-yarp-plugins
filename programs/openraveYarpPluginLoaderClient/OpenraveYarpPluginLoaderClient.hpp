// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__
#define __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__

#include <vector>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>

namespace roboticslab
{

class CallbackPort : public yarp::os::BufferedPort<yarp::os::Bottle>
{
    void onRead(yarp::os::Bottle& b) override;
    std::vector<int> availableIds;
};

/**
 * @ingroup openraveYarpPluginLoaderClient
 *
 * @brief openraveYarpPluginLoaderClient.
 *
 */
class OpenraveYarpPluginLoaderClient : public yarp::os::RFModule
{
public:
    OpenraveYarpPluginLoaderClient();
    bool configure(yarp::os::ResourceFinder &rf);

private:
    yarp::os::RpcClient rpcClient;
    CallbackPort callbackPort;

    std::vector<int> openedIds;

    virtual double getPeriod() override
    {
        return 1.0;
    }
    virtual bool updateModule() override;
    virtual bool close() override;
};

}  // namespace roboticslab

#endif  // __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__
