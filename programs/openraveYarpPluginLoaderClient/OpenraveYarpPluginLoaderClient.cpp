// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpPluginLoaderClient.hpp"

#include <ColorDebug.h>

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
