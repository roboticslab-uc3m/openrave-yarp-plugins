// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__
#define __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>

namespace roboticslab
{

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

    virtual double getPeriod() override
    {
        return 1.0;
    }
    virtual bool updateModule() override;
    virtual bool close() override;
};

}  // namespace roboticslab

#endif  // __OPENRAVE_YARP_PLUGIN_LOADER_CLIENT__
