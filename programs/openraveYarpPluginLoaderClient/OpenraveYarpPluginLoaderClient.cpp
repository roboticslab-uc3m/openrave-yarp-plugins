// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpPluginLoaderClient.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

namespace roboticslab
{

/************************************************************************/

const int OpenraveYarpPluginLoaderClient::DEFAULT_PERIOD_S = 1.0;
const yarp::conf::vocab32_t OpenraveYarpPluginLoaderClient::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t OpenraveYarpPluginLoaderClient::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

/************************************************************************/

OpenraveYarpPluginLoaderClient::OpenraveYarpPluginLoaderClient() : detectedFirst(false)
{
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::configure(yarp::os::ResourceFinder &rf)
{
    yDebug() << "OpenraveYarpPluginLoaderClient config:" << rf.toString();

    if(rf.check("help"))
    {
        yInfo() << "OpenraveYarpPluginLoaderClient options:";
        yInfo() << "\t--help (this help)\t--from [file.ini]\t--context [path]";
        return false;
    }

    yarp::os::Property openOptions;
    openOptions.fromString(rf.toString());
    openOptions.unput("from");
    yDebug() << "openOptions:" << openOptions.toString();

    if(!openOptions.check("device"))
    {
        yError() << "Missing device parameter";
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
        yError() << "!rpcClient.open";
        return false;
    }
    if(!rpcClient.addOutput("/OpenraveYarpPluginLoader/rpc:s"))
    {
        yError() << "RpcServer \"/OpenraveYarpPluginLoader/rpc:s\" not found";
        return false;
    }

    //-- CallbackPort
    std::string callbackPortName("/");
    callbackPortName.append(deviceName);
    callbackPortName.append("/");
    callbackPortName.append("OpenraveYarpPluginLoader/state:i");
    if(!callbackPort.open(callbackPortName))
    {
        yError() << "!callbackPort.open";
        return false;
    }
    if(!yarp::os::Network::connect("/OpenraveYarpPluginLoader/state:o",callbackPortName))
    {
        yError() << "Unable to connect to state port";
        return false;
    }
    callbackPort.useCallback();

    yarp::os::Bottle openOptionsBottle;
    openOptionsBottle.fromString(openOptions.toString());

    yarp::os::Bottle cmd, res;
    cmd.addString("open");
    cmd.append(openOptionsBottle);
    yDebug() << "cmd:" << cmd.toString();
    rpcClient.write(cmd, res);

    if(VOCAB_FAILED == res.get(0).asVocab())
    {
        yError() << "Client error:" << res.toString();
        return false;
    }

    yInfo() << "Client success:" << res.toString();

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
            yDebug() << "Open not available";
            return false;
        }
    }
    callbackPort.availableIdsMutex.unlock();
    yDebug() << "Open available";
    return true;
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::updateModule()
{
    if(-1 == callbackPort.lastTime)
    {
        yDebug() << "Waiting for first read...";
        return true;
    }

    if(!detectedFirst)
    {
        if(openedInAvailable())
        {
            detectedFirst = true;
        }
        yDebug() << "Waiting for detectedFirst...";
        return true;
    }

    if(!openedInAvailable())
    {
        yInfo() << "!openedInAvailable()";
        return false;
    }

    double deltaTime = yarp::os::Time::now() - callbackPort.lastTime;
    if(deltaTime > DEFAULT_PERIOD_S * 2.0)
    {
        yInfo() << "deltaTime > DEFAULT_PERIOD_S * 2.0";
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

    yInfo() << "Response on close:" << res.toString();

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

}  // namespace roboticslab
