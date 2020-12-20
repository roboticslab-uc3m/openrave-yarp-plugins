// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_WORLD_CLIENT_FILE__
#define __OPENRAVE_YARP_WORLD_CLIENT_FILE__

#include <mutex>
#include <vector>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/RpcClient.h>

namespace roboticslab
{

/**
 * @ingroup OpenraveYarpWorldClientFile
 */
class OywCallbackPort : public yarp::os::BufferedPort<yarp::os::Bottle>
{
public:
    OywCallbackPort();
    std::vector<std::string> availableIds;
    std::mutex availableIdsMutex;
    double lastTime;
private:
    virtual void onRead(yarp::os::Bottle& b) override;
};

/**
 * @ingroup OpenraveYarpWorldClientFile
 */
class OpenraveYarpWorldClientFile : public yarp::os::RFModule
{
public:
    OpenraveYarpWorldClientFile();
    bool configure(yarp::os::ResourceFinder &rf);

private:
    yarp::os::RpcClient rpcClient;
    OywCallbackPort callbackPort;
    std::string openedId;

    bool openedInAvailable();
    bool detectedFirst;

    virtual double getPeriod() override
    {
        return DEFAULT_PERIOD_S;
    }
    virtual bool updateModule() override;
    virtual bool close() override;


    static const int DEFAULT_PERIOD_S;
    static const yarp::conf::vocab32_t VOCAB_OK;
    static const yarp::conf::vocab32_t VOCAB_FAILED;
};

} // namespace roboticslab

#endif // __OPENRAVE_YARP_WORLD_CLIENT_FILE__
