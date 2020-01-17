// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup openrave_yarp_devices_programs
 * \defgroup OpenraveYarpPluginLoaderClient OpenraveYarpPluginLoaderClient
 */

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include <ColorDebug.h>

#include "OpenraveYarpPluginLoaderClient.hpp"

int main(int argc, char *argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext("openraveYarpPluginLoaderClient");
    rf.setDefaultConfigFile("openraveYarpPluginLoaderClient.ini");
    rf.configure(argc, argv);

    CD_INFO("Checking for yarp network...\n");
    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        CD_ERROR("Found no yarp network (try running \"yarpserver &\"), bye!\n");
        return 1;
    }
    CD_SUCCESS("Found yarp network.\n");

    roboticslab::OpenraveYarpPluginLoaderClient mod;
    return mod.runModule(rf);
}
