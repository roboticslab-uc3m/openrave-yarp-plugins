// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpPluginLoaderClient.hpp"

#include <yarp/os/Vocab.h>

#include <ColorDebug.h>

#define VOCAB_OK yarp::os::createVocab('o','k')
#define VOCAB_FAILED yarp::os::createVocab('f','a','i','l')

namespace roboticslab
{

/************************************************************************/

OpenraveYarpPluginLoaderClient::OpenraveYarpPluginLoaderClient() { }

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::configure(yarp::os::ResourceFinder &rf)
{

    if(rf.check("help"))
    {
        std::printf("OpenraveYarpPluginLoaderClient options:\n");
        std::printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        CD_DEBUG_NO_HEADER("%s\n",rf.toString().c_str());
        return false;
    }

    if(!rpcClient.addOutput("/OpenraveYarpPluginLoader/rpc:s"))
    {
        CD_ERROR("RpcServer \"/OpenraveYarpPluginLoader/rpc:s\" not found, bye!\n");
        return false;
    }

    yarp::os::Property openOptions;
    openOptions.fromString(rf.toString());
    openOptions.unput("from");
    CD_DEBUG("openOptions: %s\n",openOptions.toString().c_str());

    yarp::os::Bottle openOptionsBottle;
    openOptionsBottle.fromString(openOptions.toString());

    yarp::os::Bottle cmd, res;
    cmd.addString("open");
    cmd.append(openOptionsBottle);
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

bool OpenraveYarpPluginLoaderClient::updateModule()
{
    CD_DEBUG("OpenraveYarpPluginLoaderClient alive...\n");
    return true;
}

/************************************************************************/

bool OpenraveYarpPluginLoaderClient::close()
{
    return true;
}

/************************************************************************/

}  // namespace roboticslab
