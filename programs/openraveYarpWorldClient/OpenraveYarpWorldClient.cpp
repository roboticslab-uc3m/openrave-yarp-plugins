// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpWorldClient.hpp"

#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <ColorDebug.h>

namespace roboticslab
{

/************************************************************************/

const int OpenraveYarpWorldClient::DEFAULT_PERIOD_S = 1.0;
const yarp::conf::vocab32_t OpenraveYarpWorldClient::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t OpenraveYarpWorldClient::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

/************************************************************************/

OpenraveYarpWorldClient::OpenraveYarpWorldClient() : detectedFirst(false)
{
}

/************************************************************************/

bool OpenraveYarpWorldClient::configure(yarp::os::ResourceFinder &rf)
{
    if(rf.check("help"))
    {
        std::printf("OpenraveYarpWorldClient options:\n");
        std::printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        CD_DEBUG_NO_HEADER("%s\n",rf.toString().c_str());
        return false;
    }
    CD_DEBUG("config: %s\n", rf.toString().c_str());

    if(!rf.check("file"))
    {
        CD_ERROR("Missing file parameter, bye!\n");
        return false;
    }
    std::string fileName = rf.find("file").asString();

    if(!rpcClient.addOutput("/OpenraveYarpWorld/rpc:s"))
    {
        CD_ERROR("RpcServer \"/OpenraveYarpWorld/rpc:s\" not found, bye!\n");
        return false;
    }

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

    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("mk");
    cmd.addString("file");
    cmd.addString(rf.find("file").asString());

    CD_DEBUG("cmd: %s\n",cmd.toString().c_str());
    rpcClient.write(cmd, res);

    if(VOCAB_FAILED == res.get(0).asVocab())
    {
        CD_ERROR("%s\n", res.toString().c_str());
        return false;
    }
    CD_SUCCESS("%s\n", res.toString().c_str());

    for(size_t i=1; i<res.size(); i++)
        openedIds.push_back(res.get(i).asInt32());

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClient::openedInAvailable()
{
    callbackPort.availableIdsMutex.lock();
    for(size_t openedIdx=0; openedIdx<openedIds.size(); openedIdx++)
    {
        //CD_DEBUG("Is open %d available?\n",openedIds[openedIdx]);
        bool innerFound = false;
        for(size_t i=0; i<callbackPort.availableIds.size(); i++)
        {
            if(openedIds[openedIdx] == callbackPort.availableIds[i])
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
    }
    callbackPort.availableIdsMutex.unlock();
    CD_DEBUG("yes\n");
    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClient::updateModule()
{
    //CD_DEBUG("OpenraveYarpWorldClient alive...\n");

    if(-1 == callbackPort.lastTime) //-- wait for first read
        return true;

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

bool OpenraveYarpWorldClient::close()
{
    CD_INFO("\n");

    yarp::os::Bottle cmd, res;
    cmd.addString("close");
    for(size_t i=0; i<openedIds.size(); i++)
        cmd.addInt32(openedIds[i]);
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

CallbackPort::CallbackPort() : lastTime(-1)
{
}

/************************************************************************/

void CallbackPort::onRead(yarp::os::Bottle& b)
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
