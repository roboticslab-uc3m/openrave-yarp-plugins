// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup openrave_yarp_devices_programs
 * \defgroup OpenraveYarpWorldClientFile OpenraveYarpWorldClientFile
 */

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include "OpenraveYarpWorldClientFile.hpp"

int main(int argc, char *argv[])
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultContext("openraveYarpWorldClientFile");
    rf.setDefaultConfigFile("openraveYarpWorldClientFile.ini");
    rf.configure(argc, argv);

    yInfo() << "Checking for yarp network...";
    yarp::os::Network yarp;

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << "No yarp network found (try running \"yarpserver &\")";
        return 1;
    }

    roboticslab::OpenraveYarpWorldClientFile mod;
    return mod.runModule(rf);
}
