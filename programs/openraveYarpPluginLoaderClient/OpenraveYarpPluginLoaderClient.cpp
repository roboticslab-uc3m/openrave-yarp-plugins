// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpPluginLoaderClient.hpp"

#include <yarp/conf/version.h>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(ORYPLC, "rl.OpenraveYarpPluginLoaderClient")
}

/************************************************************************/

constexpr auto DEFAULT_PERIOD_S = 1.0;
#if YARP_VERSION_MINOR >= 5
constexpr auto VOCAB_OK = yarp::os::createVocab32('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab32('f','a','i','l');
#else
constexpr auto VOCAB_OK = yarp::os::createVocab('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');
#endif

/************************************************************************/

OpenraveYarpPluginLoaderClient::OpenraveYarpPluginLoaderClient() : detectedFirst(false)
{
}

/************************************************************************/

double OpenraveYarpPluginLoaderClient::getPeriod()
{
    return DEFAULT_PERIOD_S;
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::configure(yarp::os::ResourceFinder &rf)
{
    yCDebug(ORYPLC) << "Config:" << rf.toString();

    if(rf.check("help"))
    {
        yCInfo(ORYPLC) << "OpenraveYarpPluginLoaderClient options:";
        yCInfo(ORYPLC) << "\t--help (this help)\t--from [file.ini]\t--context [path]";
        return false;
    }

    yarp::os::Property openOptions;
    openOptions.fromString(rf.toString());
    openOptions.unput("from");
    yCDebug(ORYPLC) << "openOptions:" << openOptions.toString();

    if(!openOptions.check("device"))
    {
        yCError(ORYPLC) << "Missing device parameter";
        return false;
    }
    std::string deviceName = openOptions.find("device").asString();

    //-- RpcClient
    std::string rpcClientName("/");
    rpcClientName.append(deviceName);
    rpcClientName.append("/");
    rpcClientName.append("OpenraveYarpPluginLoader/rpc:c");
    if(!rpcClient.open(rpcClientName))
    {
        yCError(ORYPLC) << "!rpcClient.open";
        return false;
    }
    if(!rpcClient.addOutput("/OpenraveYarpPluginLoader/rpc:s"))
    {
        yCError(ORYPLC) << "RpcServer \"/OpenraveYarpPluginLoader/rpc:s\" not found";
        return false;
    }

    //-- CallbackPort
    std::string callbackPortName("/");
    callbackPortName.append(deviceName);
    callbackPortName.append("/");
    callbackPortName.append("OpenraveYarpPluginLoader/state:i");
    if(!callbackPort.open(callbackPortName))
    {
        yCError(ORYPLC) << "!callbackPort.open";
        return false;
    }
    if(!yarp::os::Network::connect("/OpenraveYarpPluginLoader/state:o",callbackPortName))
    {
        yCError(ORYPLC) << "Unable to connect to state port";
        return false;
    }
    callbackPort.useCallback();

    yarp::os::Bottle openOptionsBottle;
    openOptionsBottle.fromString(openOptions.toString());

    yarp::os::Bottle cmd, res;
    cmd.addString("open");
    cmd.append(openOptionsBottle);
    yCDebug(ORYPLC) << "cmd:" << cmd.toString();
    rpcClient.write(cmd, res);

#if YARP_VERSION_MINOR >= 5
    if(VOCAB_FAILED == res.get(0).asVocab32())
#else
    if(VOCAB_FAILED == res.get(0).asVocab())
#endif
    {
        yCError(ORYPLC) << "Client error:" << res.toString();
        return false;
    }

    yCInfo(ORYPLC) << "Client success:" << res.toString();

    for(size_t i=1; i<res.size(); i++)
        openedIds.push_back(res.get(i).asInt32());

    return true;
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::openedInAvailable()
{
    callbackPort.availableIdsMutex.lock();
    for(size_t openedIdx=0; openedIdx<openedIds.size(); openedIdx++)
    {
        bool innerFound = false;
        for(size_t i=0; i<callbackPort.availableIds.size(); i++)
        {
            if(openedIds[openedIdx] == callbackPort.availableIds[i])
            {
                innerFound = true;
                break;
            }
        }
        if(!innerFound)
        {
            callbackPort.availableIdsMutex.unlock();
            yCDebug(ORYPLC) << "Open not available";
            return false;
        }
    }
    callbackPort.availableIdsMutex.unlock();
    yCDebug(ORYPLC) << "Open available";
    return true;
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::updateModule()
{
    if(-1 == callbackPort.lastTime)
    {
        yCDebug(ORYPLC) << "Waiting for first read...";
        return true;
    }

    if(!detectedFirst)
    {
        if(openedInAvailable())
        {
            detectedFirst = true;
        }
        yCDebug(ORYPLC) << "Waiting for detectedFirst...";
        return true;
    }

    if(!openedInAvailable())
    {
        yCInfo(ORYPLC) << "!openedInAvailable()";
        return false;
    }

    double deltaTime = yarp::os::Time::now() - callbackPort.lastTime;
    if(deltaTime > DEFAULT_PERIOD_S * 2.0)
    {
        yCInfo(ORYPLC) << "deltaTime > DEFAULT_PERIOD_S * 2.0";
        return false;
    }

    return true;
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::close()
{
    yarp::os::Bottle cmd, res;
    cmd.addString("close");
    for(size_t i=0; i<openedIds.size(); i++)
        cmd.addInt32(openedIds[i]);
    rpcClient.write(cmd, res);

    yCInfo(ORYPLC) << "Response on close:" << res.toString();

    callbackPort.disableCallback();

    callbackPort.interrupt();
    rpcClient.interrupt();

    callbackPort.close();
    rpcClient.close();

    return true;
}

/************************************************************************/

OyplCallbackPort::OyplCallbackPort() : lastTime(-1)
{
}

/************************************************************************/

void OyplCallbackPort::onRead(yarp::os::Bottle& b)
{
    availableIdsMutex.lock();
    availableIds.clear();
    for(size_t i=0; i<b.size(); i++)
    {
        yarp::os::Bottle* elems = b.get(i).asList();
        availableIds.push_back(elems->get(0).asInt32());
    }
    availableIdsMutex.unlock();
    lastTime = yarp::os::Time::now();
}

/************************************************************************/
