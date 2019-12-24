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

    CD_DEBUG("%s\n",rf.toString().c_str());

    if(!rf.check("device"))
    {
        CD_ERROR("Parameter \"device\" not found, bye!\n");
        return false;
    }

    yarp::os::Bottle b(rf.toString());
    //CD_DEBUG("%d: %s\n", b.size(), b.toString().c_str());
    std::string cmdStr;
    for(size_t i=0; i<b.size(); i++)
    {
        yarp::os::Bottle* elem = b.get(i).asList();
        if("from" == elem->get(0).asString())
            continue;
        //CD_DEBUG("* %d: %s\n", i, elem->toString().c_str());
        cmdStr.append("--");
        cmdStr.append(elem->get(0).asString());
        cmdStr.append(" ");
        cmdStr.append(elem->get(1).asString());
        cmdStr.append(" ");
    }

    CD_DEBUG("%s\n",cmdStr.c_str());

    if(!rpcClient.addOutput("/OpenraveYarpPluginLoader/rpc:s"))
    {
        CD_ERROR("RpcServer \"/OpenraveYarpPluginLoader/rpc:s\" not found, bye!\n");
        return false;
    }

    yarp::os::Bottle cmd, res;
    cmd.addString("open");
    cmd.addString(cmdStr);
    rpcClient.write(cmd, res);
    CD_DEBUG("%s\n",res.toString().c_str());

    if(VOCAB_FAILED == res.get(0).asVocab())
    {
        CD_ERROR("Error: %s\n", res.get(1).asString().c_str());
        return false;
    }

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
