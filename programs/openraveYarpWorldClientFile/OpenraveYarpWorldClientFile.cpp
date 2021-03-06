// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpWorldClientFile.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

namespace roboticslab
{

/************************************************************************/

const int OpenraveYarpWorldClientFile::DEFAULT_PERIOD_S = 1.0;
const yarp::conf::vocab32_t OpenraveYarpWorldClientFile::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t OpenraveYarpWorldClientFile::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

/************************************************************************/

OpenraveYarpWorldClientFile::OpenraveYarpWorldClientFile() : detectedFirst(false)
{
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::configure(yarp::os::ResourceFinder &rf)
{
    yDebug() << "OpenraveYarpWorldClientFile config:" << rf.toString();

    if(rf.check("help"))
    {
        yInfo() << "OpenraveYarpWorldClientFile options:";
        yInfo() << "\t--help (this help)\t--from [file.ini]\t--context [path]";
        return false;
    }

    //-- file parameter
    if(!rf.check("file", "file name of object to be loaded"))
    {
        yError() << "Missing file parameter";
        return false;
    }
    std::string fileName = rf.find("file").asString();

    //-- pos parameter
    yarp::os::Bottle pos;
    if(rf.check("pos", "initial cartesian position"))
    {
        pos = rf.findGroup("pos").tail();
        yInfo() << "Using pos:" << pos.toString();
    }
    else
    {
        yInfo() << "Not using pos";
    }

    //-- ori parameter
    yarp::os::Bottle ori;
    if(rf.check("ori", "initial cartesian orientation"))
    {
        ori = rf.findGroup("ori").tail();
        yInfo() << "Using ori:" << ori.toString();
    }
    else
    {
        yInfo() << "Not using ori";
    }

    //-- RpcClient
    std::string rpcClientName("/");
    rpcClientName.append(fileName);
    rpcClientName.append("/");
    rpcClientName.append("OpenraveYarpWorld/rpc:c");
    if(!rpcClient.open(rpcClientName))
    {
        yError() << "!rpcClient.open";
        return false;
    }
    if(!rpcClient.addOutput("/OpenraveYarpWorld/rpc:s"))
    {
        yError() << "RpcServer \"/OpenraveYarpWorld/rpc:s\" not found";
        return false;
    }

    //-- CallbackPort
    std::string callbackPortName("/");
    callbackPortName.append(fileName);
    callbackPortName.append("/");
    callbackPortName.append("OpenraveYarpWorld/state:i");
    if(!callbackPort.open(callbackPortName))
    {
        yError() << "!callbackPort.open";
        return false;
    }
    if(!yarp::os::Network::connect("/OpenraveYarpWorld/state:o",callbackPortName))
    {
        yError() << "Unable to connect state port";
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

    yDebug() << "cmd:" << cmd.toString();
    rpcClient.write(cmd, res);

    if(VOCAB_FAILED == res.get(0).asVocab())
    {
        yError() << "Client error:" << res.toString();
        return false;
    }
    yInfo() << "Client success:" << res.toString();

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
        yDebug() << "Open not available";
        return false;
    }
    callbackPort.availableIdsMutex.unlock();
    yDebug() << "Open available";
    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::updateModule()
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

bool OpenraveYarpWorldClientFile::close()
{
    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("del");
    cmd.addString(openedId);
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

}  // namespace roboticslab
