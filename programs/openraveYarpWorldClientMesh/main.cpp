// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup openrave_yarp_devices_programs
 * @defgroup OpenraveYarpWorldClientMesh OpenraveYarpWorldClientMesh
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include "OpenraveYarpWorldClientMesh.hpp"

int main(int argc, char * argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setDefaultContext("openraveYarpWorldClientMesh");
    rf.setDefaultConfigFile("openraveYarpWorldClientMesh.ini");
    rf.configure(argc, argv);

    yInfo() << "Checking for yarp network...";
    yarp::os::Network yarp;

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << "Found no yarp network (try running \"yarpserver &\"), bye!";
        return 1;
    }

    yInfo() << "Found yarp network.";

    roboticslab::OpenraveYarpWorldClientMesh mod;
    return mod.runModule(rf);
}
