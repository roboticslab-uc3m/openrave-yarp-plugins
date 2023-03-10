// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveYarpWorldClientMesh.hpp"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/Property.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/PointCloud.h>
#include <yarp/sig/PointCloudUtils.h>

#include <YarpCloudUtils.hpp>

using namespace roboticslab;

namespace
{
    YARP_LOG_COMPONENT(ORYWCM, "rl.OpenraveYarpWorldClientMesh")
}

constexpr auto DEFAULT_PERIOD_S = 1.0;
constexpr auto VOCAB_OK = yarp::os::createVocab32('o','k');
constexpr auto VOCAB_FAILED = yarp::os::createVocab32('f','a','i','l');
constexpr auto PREFIX = "/OpenraveYarpWorldMesh";
constexpr auto MAX_Z = 1.5;
constexpr auto SIGMA_R = 0.1;
constexpr auto SIGMA_S = 50.0;
constexpr auto MAX_EDGE_LENGTH_A = 0.05;
constexpr auto TRIANGLE_PIXEL_SIZE = 10;

/************************************************************************/

OpenraveYarpWorldClientMesh::OpenraveYarpWorldClientMesh()
    : detectedFirst(false)
{}

/************************************************************************/

double OpenraveYarpWorldClientMesh::getPeriod()
{
    return DEFAULT_PERIOD_S;
}

/************************************************************************/

bool OpenraveYarpWorldClientMesh::configure(yarp::os::ResourceFinder &rf)
{
    yCDebug(ORYWCM) << "Config:" << rf.toString();

    if (!rf.check("remote", "remote sensor port to connect to"))
    {
        yCError(ORYWCM) << "Missing --remote parameter";
        return false;
    }

    if (!rf.check("robot", "virtual robot name"))
    {
        yCError(ORYWCM) << "Missing --robot parameter";
        return false;
    }

    if (!rf.check("sensor", "virtual sensor name"))
    {
        yCError(ORYWCM) << "Missing --sensor parameter";
        return false;
    }

    std::string local = PREFIX;
    std::string remote = rf.find("remote").asString();

    yarp::sig::IntrinsicParams depthParams;
    yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;

    {
        yarp::os::Property sensorOptions = {
            {"device", yarp::os::Value("RGBDSensorClient")},
            {"localImagePort", yarp::os::Value(local + "/client/rgbImage:i")},
            {"localDepthPort", yarp::os::Value(local + "/client/depthImage:i")},
            {"localRpcPort", yarp::os::Value(local + "/client/rpc:o")},
            {"remoteImagePort", yarp::os::Value(remote + "/rgbImage:o")},
            {"remoteDepthPort", yarp::os::Value(remote + "/depthImage:o")},
            {"remoteRpcPort", yarp::os::Value(remote + "/rpc:i")}
        };

        yarp::dev::PolyDriver sensorDevice(sensorOptions);

        if (!sensorDevice.isValid())
        {
            yCError(ORYWCM) << "Unable to connect to remote sensor";
            return false;
        }

        yarp::dev::IRGBDSensor * iRGBDSensor;

        if (!sensorDevice.view(iRGBDSensor))
        {
            yCError(ORYWCM) << "Unable to view IRGBDSensor";
            return false;
        }

        yarp::os::Property intrinsic;

        if (!iRGBDSensor->getRgbIntrinsicParam(intrinsic))
        {
            yCError(ORYWCM) << "Unable to retrieve depth intrinsic parameters";
            return 1;
        }

        depthParams.fromProperty(intrinsic);

        for (auto n = 0;; n++)
        {
            iRGBDSensor->getDepthImage(depthImage);

            if (depthImage.getRawImageSize() != 0)
            {
                break;
            }
            else if (n == 10)
            {
                yCError(ORYWCM) << "Unable to acquire depth frame";
                return 1;
            }

            yarp::os::Time::delay(0.1);
        }
    }

    yarp::sig::utils::PCL_ROI roi {0, 0, 0, 0};

    auto stepX = 1;
    auto stepY = 1;

    if (rf.check("roi", "ROI of depth frame encoded as (minX maxX minY maxY)"))
    {
        auto b_roi = rf.findGroup("roi").tail();

        if (b_roi.size() != 4)
        {
            yCError(ORYWCM) << "--roi must be a list of 4 unsigned ints";
            return 1;
        }

        roi.min_x = b_roi.get(0).asInt32();
        roi.max_x = b_roi.get(1).asInt32();
        roi.min_y = b_roi.get(2).asInt32();
        roi.max_y = b_roi.get(3).asInt32();
    }

    if (rf.check("step", "decimation step of depth frame encoded as (stepX stepY)"))
    {
        auto b_step = rf.findGroup("step").tail();

        if (b_step.size() != 2)
        {
            yCError(ORYWCM) << "--step must be a list of 2 unsigned ints";
            return 1;
        }

        stepX = b_step.get(0).asInt32();
        stepY = b_step.get(1).asInt32();
    }

    auto cloud = yarp::sig::utils::depthToPC(depthImage, depthParams, roi, stepX, stepY);

    yarp::sig::VectorOf<yarp::os::Property> meshOptions {
        {
            {"algorithm", yarp::os::Value("CropBox")},
            {"keepOrganized", yarp::os::Value(true)},
            {"maxZ", rf.check("maxZ", yarp::os::Value(MAX_Z), "maximum Z for point cloud truncation")} // default: 1.0
        },
        {
            {"algorithm", yarp::os::Value("FastBilateralFilterOMP")},
            {"sigmaR", rf.check("sigmaR", yarp::os::Value(SIGMA_R), "sigma R")}, // default: 0.05
            {"sigmaS", rf.check("sigmaS", yarp::os::Value(SIGMA_S), "sigma S")} // default: 15.0
        },
        {
            {"algorithm", yarp::os::Value("OrganizedFastMesh")},
            {"maxEdgeLengthA", rf.check("maxEdgeLengthA", yarp::os::Value(MAX_EDGE_LENGTH_A), "max edge length")}, // default: 0.0
            {"trianglePixelSize", rf.check("trianglePixelSize", yarp::os::Value(TRIANGLE_PIXEL_SIZE), "triangle pixel size")}, // default: 1
            {"triangulationType", yarp::os::Value("triangleAdaptiveCut")}, // default: quadMesh
            {"useDepthAsDistance", yarp::os::Value(true)}
        },
        {
            {"algorithm", yarp::os::Value("SimplificationRemoveUnusedVertices")}
        }
    };

    yarp::sig::PointCloudXYZ meshVertices;
    yarp::sig::VectorOf<int> meshIndices;

    if (!YarpCloudUtils::meshFromCloud(cloud, meshVertices, meshIndices, meshOptions))
    {
        yCError(ORYWCM) << "Unable to construct surface mesh from depth frame";
        return false;
    }

    yCInfo(ORYWCM) << "Got mesh of" << meshVertices.size() << "vertices and" << meshIndices.size() / 3 << "faces";

    std::string rpcClientName(remote);
    rpcClientName.append(local);
    rpcClientName.append("/rpc:c");

    if (!rpcClient.open(rpcClientName))
    {
        yCError(ORYWCM) << "Unable to open RPC client";
        return false;
    }

    if (!rpcClient.addOutput("/OpenraveYarpWorld/rpc:s"))
    {
        yCError(ORYWCM) << "Unable to connect to remote RPC server";
        return false;
    }

    std::string callbackPortName(remote);
    rpcClientName.append(local);
    callbackPortName.append("/state:i");

    if (!callbackPort.open(callbackPortName))
    {
        yCError(ORYWCM) << "Unable to open local callback port";
        return false;
    }

    if (!yarp::os::Network::connect("/OpenraveYarpWorld/state:o", callbackPortName))
    {
        yCError(ORYWCM) << "Unable to connect to remote callback port";
        return false;
    }

    callbackPort.useCallback();

    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("mk");
    cmd.addString("mesh");
    cmd.add(rf.find("robot"));
    cmd.add(rf.find("sensor"));

    auto & vertices = cmd.addList();
    auto & indices = cmd.addList();

    yCDebug(ORYWCM) << "Command (redacted mesh):" << cmd.toString();

    for (std::size_t i = 0; i < meshVertices.size(); i++)
    {
        const auto & point = meshVertices(i);
        vertices.addFloat32(point.x);
        vertices.addFloat32(point.y);
        vertices.addFloat32(point.z);
    }

    for (auto index : meshIndices)
    {
        indices.addInt32(index);
    }

    if (!rpcClient.write(cmd, res))
    {
        yCError(ORYWCM) << "Unable to send mesh";
        return false;
    }

    if (res.get(0).asVocab32() == VOCAB_FAILED)
    {
        yCError(ORYWCM) << res.toString();
        return false;
    }

    yCInfo(ORYWCM) << res.toString();

    openedId = res.get(1).asString();

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientMesh::openedInAvailable()
{
    std::lock_guard lock(callbackPort.availableIdsMutex);
    bool innerFound = false;

    for (std::size_t i = 0; i < callbackPort.availableIds.size(); i++)
    {
        if (openedId == callbackPort.availableIds[i])
        {
            innerFound = true;
            break;
        }
    }

    if (!innerFound)
    {
        yCDebug(ORYWCM) << "no";
        return false;
    }

    yCDebug(ORYWCM) << "yes";
    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientMesh::updateModule()
{
    if (callbackPort.lastTime == -1)
    {
        yCDebug(ORYWCM) << "Wait for first read...";
        return true;
    }

    if (!detectedFirst)
    {
        if (openedInAvailable())
        {
            detectedFirst = true;
        }

        yCDebug(ORYWCM) << "Waiting for detectedFirst...";
        return true;
    }

    if (!openedInAvailable())
    {
        yCInfo(ORYWCM) << "not available";
        return false;
    }

    double deltaTime = yarp::os::Time::now() - callbackPort.lastTime;

    if (deltaTime > DEFAULT_PERIOD_S * 2.0)
    {
        yCInfo(ORYWCM) << "deltaTime > DEFAULT_PERIOD_S * 2.0";
        return false;
    }

    return true;
}

/************************************************************************/

bool OpenraveYarpWorldClientMesh::close()
{
    yarp::os::Bottle cmd, res;
    cmd.addString("world");
    cmd.addString("del");
    cmd.addString(openedId);
    rpcClient.write(cmd, res);

    yCInfo(ORYWCM) << res.toString();

    callbackPort.disableCallback();

    callbackPort.interrupt();
    rpcClient.interrupt();

    callbackPort.close();
    rpcClient.close();

    return true;
}

/************************************************************************/

OywCallbackPort::OywCallbackPort()
    : lastTime(-1)
{}

/************************************************************************/

void OywCallbackPort::onRead(yarp::os::Bottle & b)
{
    std::lock_guard lock(availableIdsMutex);
    availableIds.clear();

    for (std::size_t i = 0; i < b.size(); i++)
    {
        yarp::os::Bottle * elems = b.get(i).asList();

        if (elems->check("kinbody"))
        {
            availableIds.push_back(elems->find("kinbody").asString());
        }
    }

    lastTime = yarp::os::Time::now();
}

/************************************************************************/
