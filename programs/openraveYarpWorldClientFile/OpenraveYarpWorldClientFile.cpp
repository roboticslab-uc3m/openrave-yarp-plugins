// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpWorldClientFile.hpp"

#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <ColorDebug.h>

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
    if(rf.check("help"))
    {
        std::printf("OpenraveYarpWorldClientFile options:\n");
        std::printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        CD_DEBUG_NO_HEADER("%s\n",rf.toString().c_str());
        return false;
    }
    CD_DEBUG("config: %s\n", rf.toString().c_str());

    //-- file parameter
    if(!rf.check("file", "file name of object to be loaded"))
    {
        CD_ERROR("Missing file parameter, bye!\n");
        return false;
    }
    std::string fileName = rf.find("file").asString();

    //-- pos parameter
    yarp::os::Bottle pos;
    if(rf.check("pos", "initial cartesian position"))
    {
        pos = rf.findGroup("pos").tail();
        CD_INFO("Using pos: %s\n", pos.toString().c_str());
    }
    else
    {
        CD_INFO("Not using pos.\n");
    }

    //-- ori parameter
    yarp::os::Bottle ori;
    if(rf.check("ori", "initial cartesian orientation"))
    {
        ori = rf.findGroup("ori").tail();
        CD_INFO("Using ori: %s\n", ori.toString().c_str());
    }
    else
    {
        CD_INFO("Not using ori.\n");
    }

    //-- RpcClient
    std::string rpcClientName("/");
    rpcClientName.append(fileName);
    rpcClientName.append("/");
    rpcClientName.append("OpenraveYarpWorld/rpc:c");
    if(!rpcClient.open(rpcClientName))
    {
        CD_ERROR("!rpcClient.open, bye!\n");
        return false;
    }
    if(!rpcClient.addOutput("/OpenraveYarpWorld/rpc:s"))
    {
        CD_ERROR("RpcServer \"/OpenraveYarpWorld/rpc:s\" not found, bye!\n");
        return false;
    }

    //-- CallbackPort
    std::string callbackPortName("/");
    callbackPortName.append(fileName);
    callbackPortName.append("/");
    callbackPortName.append("OpenraveYarpWorld/state:i");
    if(!callbackPort.open(callbackPortName))
    {
        CD_ERROR("!callbackPort.open, bye!\n");
        return false;
    }
    if(!yarp::os::Network::connect("/OpenraveYarpWorld/state:o",callbackPortName))
    {
        CD_ERROR("bye!\n");
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

    CD_DEBUG("cmd: %s\n",cmd.toString().c_str());
    rpcClient.write(cmd, res);

    if(VOCAB_FAILED == res.get(0).asVocab())
    {
        CD_ERROR("%s\n", res.toString().c_str());
        return false;
    }
    CD_SUCCESS("%s\n", res.toString().c_str());

    openedId = res.get(1).asString();

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::openedInAvailable()
{
    callbackPort.availableIdsMutex.lock();
    //CD_DEBUG("Is open %s available?\n", openedId.c_str());
    bool innerFound = false;
    for(size_t i=0; i<callbackPort.availableIds.size(); i++)
    {
        if(openedId == callbackPort.availableIds[i])
        {
            //CD_DEBUG("Yes\n");
            innerFound = true;
            break;
        }
    }
    if(!innerFound)
    {
        //CD_DEBUG("No\n");
        callbackPort.availableIdsMutex.unlock();
        CD_DEBUG("no\n");
        return false;
    }
    callbackPort.availableIdsMutex.unlock();
    CD_DEBUG("yes\n");
    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::updateModule()
{
    //CD_DEBUG("OpenraveYarpWorldClientFile alive...\n");

    if(-1 == callbackPort.lastTime)
    {
        CD_DEBUG("wait for first read...\n");
        return true;
    }

    if(!detectedFirst)
    {
        if(openedInAvailable())
        {
            detectedFirst = true;
        }
        CD_DEBUG("Waiting for detectedFirst...\n");
        return true;
    }

    if(!openedInAvailable())
    {
        CD_INFO("!openedInAvailable(), bye!\n");
        return false;
    }

    double deltaTime = yarp::os::Time::now() - callbackPort.lastTime;
    if(deltaTime > DEFAULT_PERIOD_S * 2.0)
    {
        CD_INFO("deltaTime > DEFAULT_PERIOD_S * 2.0, bye!\n");
        return false;
    }

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientFile::close()
{
    CD_INFO("\n");

    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("del");
    cmd.addString(openedId);
    rpcClient.write(cmd, res);

    CD_INFO("%s\n", res.toString().c_str());

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
    //CD_DEBUG("* %s\n", b.toString().c_str());
    availableIdsMutex.lock();
    availableIds.clear();
    for(size_t i=0; i<b.size(); i++)
    {
        yarp::os::Bottle* elems = b.get(i).asList();
        //CD_DEBUG("** %s\n", elems->toString().c_str());
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
