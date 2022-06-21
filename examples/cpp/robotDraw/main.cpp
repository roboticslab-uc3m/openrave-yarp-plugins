// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/**
 * @ingroup openrave-yarp-plugins-examples
 * \defgroup robotDraw robotDraw
 *
 * @brief robotDraw opens a YARP rpc port to recieve "draw p0x p0y p1x p1y ... pNx pNy" commands.
 *
 * robotDraw opens a YARP rpc port to recieve "draw p0x p0y p1x p1y ... pNx pNy" commands
 * (i.e. "draw 0 0 .1 0 .1 .1 0 .1 0 0"), and draw the points on a plane. It connects to
 * a running \ref cartesianServer module using the C++ implementation of the \ref CartesianClientLib
 * library. Inherited from (original development of) the XGNITIVE repository.
 *
 * @section robotDraw_legal Legal
 *
 * Copyright: (C) 2019 Universidad Carlos III de Madrid
 *
 * Author: <a href="http://roboticslab.uc3m.es/roboticslab/people/jg-victores">Juan G. Victores</a>,
 * <a href="http://www.mendeley.com/profiles/santiago-morante-cendrero/">Santiago Morante</a>
 *
 * CopyPolicy: This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 2.1 or later
 */

#include <cstdio>

#include <string>
#include <vector>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/PolyDriver.h>

#include <ICartesianControl.h>

constexpr auto DEFAULT_PREFIX = "/robotDraw";
constexpr auto DEFAULT_CARTESIAN_REMOTE = "/asibotSim/CartesianControl";
constexpr auto DEFAULT_WORLD_RESPONDER = "/OpenraveYarpWorldRpcResponder";
constexpr auto DEFAULT_HEIGHT = 0.4;

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    yInfo("Run \"%s --help\" for options", argv[0]);
    yInfo("%s checking for yarp network...", argv[0]);

    if (!yarp::os::Network::checkNetwork())
    {
        yError() << argv[0] << "found no yarp network (try running \"yarpserver &\"), bye!";
        return 1;
    }

    std::string prefix = "/robotDraw";
    std::string cartesianRemote = DEFAULT_CARTESIAN_REMOTE;
    std::string worldRemote = DEFAULT_WORLD_RESPONDER;
    double height = DEFAULT_HEIGHT;

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    if (rf.check("help"))
    {
        yInfo("%s options:", argv[0]);
        yInfo("\t--help (this help)\t--from [file.ini]\t--context [path]");
        yInfo("\t--cartesianRemote (default: \"%s\")", cartesianRemote.c_str());
        yInfo("\t--worldRemote (default: \"%s\")", worldRemote.c_str());
        yInfo("\t--height (default: \"%f\")", height);
    }

    if (rf.check("cartesianRemote"))
    {
        cartesianRemote = rf.find("cartesianRemote").asString();
    }

    if (rf.check("worldRemote"))
    {
        worldRemote = rf.find("worldRemote").asString();
    }

    if (rf.check("height"))
    {
        height = rf.find("height").asFloat64();
    }

    yInfo("robotDraw using cartesianRemote: %s, worldRemote: %s, height: %f",
          cartesianRemote.c_str(), worldRemote.c_str(), height);

    if (rf.check("help"))
    {
        return 0;
    }

    yarp::os::Property options;
    options.fromString(rf.toString());
    options.put("device", "CartesianControlClient");
    options.put("cartesianLocal", prefix + "/cartesianClient");
    options.put("cartesianRemote", cartesianRemote);

    yarp::dev::PolyDriver cartesianDevice(options);

    if (!cartesianDevice.isValid())
    {
        yError() << "Cannot open cartesian device";
        return 1;
    }

    roboticslab::ICartesianControl * iCartesianControl;

    if (!cartesianDevice.view(iCartesianControl))
    {
        yError() << "Cannot view iCartesianControl";
        return 1;
    }

    yarp::os::RpcClient worldRpcClient;
    std::string worldRpcClientName = prefix + worldRemote + "/rpc:c";
    worldRemote += "/rpc:s";

    if (!worldRpcClient.open(worldRpcClientName))
    {
        yError() << "Cannot open world RPC client";
        return 1;
    }

    if (!yarp::os::Network::connect(worldRpcClientName, worldRemote))
    {
        yError() << "Cannot connect to" << worldRemote;
        return 1;
    }

    yarp::os::RpcServer pointsRpcServer;
    std::string pointsRpcServerName = prefix + "/points/rpc:s";

    if (!pointsRpcServer.open(pointsRpcServerName))
    {
        yError() << "Cannot open point RPC server";
        return 1;
    }

    std::vector<double> home(5);
    home[0] = 0.0;
    home[1] = -0.1;
    home[2] = 0.9;
    home[3] = 90.0;
    home[4] = 0.0;

    yInfo() << "Homing";

    iCartesianControl->movj(home);
    iCartesianControl->wait();

    std::vector<double> targets(5);
    targets[0] = 0.0;
    targets[1] = -0.45;
    targets[2] = height;
    targets[3] = 180.0;
    targets[4] = 0.0;

    while (true)
    {
        yInfo() << pointsRpcServerName << "on BLOCKING WAIT for draw request";
        yInfo() << "Usage: [draw] r g b p0x p0y p1x p1y ... pNx pNy";
        yInfo() << "Example: [draw] 1 1 1 0 0 .1 0 .1 .1 0 .1 0 0";

        yarp::os::Bottle pointsIn, pointsOut;
        pointsRpcServer.read(pointsIn, true);

        yDebug() << "Got" << pointsIn.toString();

        pointsOut.clear();
        pointsOut.addVocab32('o', 'k');
        pointsRpcServer.reply(pointsOut);

        std::vector<double> aprox0(5);
        aprox0[0] = targets[0] + pointsIn.get(5).asFloat64();
        aprox0[1] = targets[1] + pointsIn.get(6).asFloat64();
        aprox0[2] = targets[2] + 0.1;
        aprox0[3] = targets[3];
        aprox0[4] = targets[4];

        yInfo() << "Moving to aprox0";

        iCartesianControl->movj(aprox0);
        iCartesianControl->wait();

        std::vector<double> p0(5);
        p0[0] = targets[0] + pointsIn.get(5).asFloat64();
        p0[1] = targets[1] + pointsIn.get(6).asFloat64();
        p0[2] = targets[2];
        p0[3] = targets[3];
        p0[4] = targets[4];

        yInfo() << "Moving to p0";

        iCartesianControl->movj(p0);
        iCartesianControl->wait();

        yarp::os::Bottle worldOut, worldIn;

        worldOut.clear();
        worldOut.addString("world");
        worldOut.addString("draw");
        worldOut.addInt32(1); // ON
        worldOut.addFloat64(pointsIn.get(1).asFloat64()); // Radius: 0.005
        worldOut.addFloat64(pointsIn.get(2).asFloat64()); // R: 0.0980392
        worldOut.addFloat64(pointsIn.get(3).asFloat64()); // G: 0.705882
        worldOut.addFloat64(pointsIn.get(4).asFloat64()); // B: 0.686274

        worldRpcClient.write(worldOut, worldIn);

//        for (int i=3; i<(bIn.size());i=i+2) {
        for (unsigned int i = 7; i < pointsIn.size(); i += 2)
        {
            std::vector<double> tmpTargets(5);
            tmpTargets[0] = targets[0] + pointsIn.get(i).asFloat64();
            tmpTargets[1] = targets[1] + pointsIn.get(i + 1).asFloat64();
            tmpTargets[2] = targets[2];
            tmpTargets[3] = targets[3];
            tmpTargets[4] = targets[4];

            iCartesianControl->movj(tmpTargets);
            iCartesianControl->wait();
        }

        worldOut.clear();
        worldOut.addString("world");
        worldOut.addString("draw");
        worldOut.addInt32(0);

        worldRpcClient.write(worldOut, worldIn);

        std::vector<double> aproxN(5);
        aproxN[0] = targets[0] + pointsIn.get(pointsIn.size() - 2).asFloat64();
        aproxN[1] = targets[1] + pointsIn.get(pointsIn.size() - 1).asFloat64();
        aproxN[2] = height + 0.1;
        aproxN[3] = targets[3];
        aproxN[4] = targets[4];

        yInfo() << "Moving to aproxN";

        iCartesianControl->movj(aproxN);
        iCartesianControl->wait();

        yInfo() << "Homing";

        iCartesianControl->movj(home);
        iCartesianControl->wait();
    }

    worldRpcClient.close();
    pointsRpcServer.close();
    cartesianDevice.close();

    return 0;
}
