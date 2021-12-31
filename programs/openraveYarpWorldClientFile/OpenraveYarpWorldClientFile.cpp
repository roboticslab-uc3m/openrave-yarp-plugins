// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpWorldClientFile.hpp"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(ORYWCF, "rl.OpenraveYarpWorldClientFile")
}

/************************************************************************/

constexpr auto DEFAULT_PERIOD_S = 1.0;
constexpr auto VOCAB_OK = yarp::os::createVocab32('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab32('f','a','i','l');

/************************************************************************/

OpenraveYarpWorldClientFile::OpenraveYarpWorldClientFile() : detectedFirst(false)
{
}

/************************************************************************/

double OpenraveYarpWorldClientFile::getPeriod()
{
    return DEFAULT_PERIOD_S;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::configure(yarp::os::ResourceFinder &rf)
{
    yCDebug(ORYWCF) << "Config:" << rf.toString();

    if(rf.check("help"))
    {
        yCInfo(ORYWCF) << "OpenraveYarpWorldClientFile options:";
        yCInfo(ORYWCF) << "\t--help (this help)\t--from [file.ini]\t--context [path]";
        return false;
    }

    //-- file parameter
    if(!rf.check("file", "file name of object to be loaded"))
    {
        yCError(ORYWCF) << "Missing file parameter";
        return false;
    }
    std::string fileName = rf.find("file").asString();

    //-- pos parameter
    yarp::os::Bottle pos;
    if(rf.check("pos", "initial cartesian position"))
    {
        pos = rf.findGroup("pos").tail();
        yCInfo(ORYWCF) << "Using pos:" << pos.toString();
    }
    else
    {
        yCInfo(ORYWCF) << "Not using pos";
    }

    //-- ori parameter
    yarp::os::Bottle ori;
    if(rf.check("ori", "initial cartesian orientation"))
    {
        ori = rf.findGroup("ori").tail();
        yCInfo(ORYWCF) << "Using ori:" << ori.toString();
    }
    else
    {
        yCInfo(ORYWCF) << "Not using ori";
    }

    //-- RpcClient
    std::string rpcClientName("/");
    rpcClientName.append(fileName);
    rpcClientName.append("/");
    rpcClientName.append("OpenraveYarpWorld/rpc:c");
    if(!rpcClient.open(rpcClientName))
    {
        yCError(ORYWCF) << "!rpcClient.open";
        return false;
    }
    if(!rpcClient.addOutput("/OpenraveYarpWorld/rpc:s"))
    {
        yCError(ORYWCF) << "RpcServer \"/OpenraveYarpWorld/rpc:s\" not found";
        return false;
    }

    //-- CallbackPort
    std::string callbackPortName("/");
    callbackPortName.append(fileName);
    callbackPortName.append("/");
    callbackPortName.append("OpenraveYarpWorld/state:i");
    if(!callbackPort.open(callbackPortName))
    {
        yCError(ORYWCF) << "!callbackPort.open";
        return false;
    }
    if(!yarp::os::Network::connect("/OpenraveYarpWorld/state:o",callbackPortName))
    {
        yCError(ORYWCF) << "Unable to connect state port";
        return false;
    }
    callbackPort.useCallback();

    //-- Send command
    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("mk");
    cmd.addString("file");
    cmd.addString(rf.find("file").asString());
    cmd.append(pos);
    cmd.append(ori);

    yCDebug(ORYWCF) << "cmd:" << cmd.toString();
    rpcClient.write(cmd, res);

    if(VOCAB_FAILED == res.get(0).asVocab32())
    {
        yCError(ORYWCF) << "Client error:" << res.toString();
        return false;
    }
    yCInfo(ORYWCF) << "Client success:" << res.toString();

    openedId = res.get(1).asString();

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::openedInAvailable()
{
    callbackPort.availableIdsMutex.lock();
    bool innerFound = false;
    for(size_t i=0; i<callbackPort.availableIds.size(); i++)
    {
        if(openedId == callbackPort.availableIds[i])
        {
            innerFound = true;
            break;
        }
    }
    if(!innerFound)
    {
        callbackPort.availableIdsMutex.unlock();
        yCDebug(ORYWCF) << "Open not available";
        return false;
    }
    callbackPort.availableIdsMutex.unlock();
    yCDebug(ORYWCF) << "Open available";
    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::updateModule()
{
    if(-1 == callbackPort.lastTime)
    {
        yCDebug(ORYWCF) << "Waiting for first read...";
        return true;
    }

    if(!detectedFirst)
    {
        if(openedInAvailable())
        {
            detectedFirst = true;
        }
        yCDebug(ORYWCF) << "Waiting for detectedFirst...";
        return true;
    }

    if(!openedInAvailable())
    {
        yCInfo(ORYWCF) << "!openedInAvailable()";
        return false;
    }

    double deltaTime = yarp::os::Time::now() - callbackPort.lastTime;
    if(deltaTime > DEFAULT_PERIOD_S * 2.0)
    {
        yCInfo(ORYWCF) << "deltaTime > DEFAULT_PERIOD_S * 2.0";
        return false;
    }

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::close()
{
    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("del");
    cmd.addString(openedId);
    rpcClient.write(cmd, res);

    yCInfo(ORYWCF) << "Response on close:" << res.toString();

    callbackPort.disableCallback();

    callbackPort.interrupt();
    rpcClient.interrupt();

    callbackPort.close();
    rpcClient.close();

    return true;
}

/************************************************************************/

OywCallbackPort::OywCallbackPort() : lastTime(-1)
{
}

/************************************************************************/

void OywCallbackPort::onRead(yarp::os::Bottle& b)
{
    availableIdsMutex.lock();
    availableIds.clear();
    for(size_t i=0; i<b.size(); i++)
    {
        yarp::os::Bottle* elems = b.get(i).asList();
        if(elems->check("kinbody"))
        {
            availableIds.push_back(elems->find("kinbody").asString());
        }
    }
    availableIdsMutex.unlock();
    lastTime = yarp::os::Time::now();
}

/************************************************************************/
