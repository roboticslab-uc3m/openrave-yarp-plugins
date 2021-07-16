// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveMeshFromRealDepth.hpp"

#include <yarp/conf/version.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

using namespace roboticslab;

namespace
{
    constexpr auto MAX_EDGE_LENGTH_A = 0.05;
    constexpr auto TRIANGLE_PIXEL_SIZE = 10;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveMeshFromRealDepth::open(yarp::os::Searchable & config)
{
    yDebug() << "Config:" << config.toString();

    if (!configureEnvironment(config) || !configureOpenravePlugins(config) || !configureRobot(config))
    {
        return false;
    }

    if (!config.check("depthSensorIndex", "depth sensor index"))
    {
        yError() << "Missing mandatory --depthSensorIndex parameter";
        return false;
    }

    auto depthSensorIndex = config.find("depthSensorIndex").asInt32();
    auto sensors = probot->GetAttachedSensors();

    if (depthSensorIndex < 0 || depthSensorIndex >= sensors.size())
    {
        yError() << "Illegal depth sensor index, got" << sensors.size() << "attached sensors";
        return false;
    }

    depthSensor = sensors[depthSensorIndex];

    if (!config.check("remote", "remote sensor port prefix"))
    {
        yError() << "Missing mandatory --remote parameter";
        return false;
    }

    std::string local = config.check("local", yarp::os::Value(DEFAULT_LOCAL_PREFIX), "local port prefix").asString();
    std::string remote = config.find("remote").asString();

    yarp::os::Property sensorOptions {
        {"device", yarp::os::Value("RGBDSensorClient")},
        {"localImagePort", yarp::os::Value(local + "/rgbImage:i")},
        {"localDepthPort", yarp::os::Value(local + "/depthImage:i")},
        {"localRpcPort", yarp::os::Value(local + "/rpc:o")},
        {"remoteImagePort", yarp::os::Value(remote + "/rgbImage:o")},
        {"remoteDepthPort", yarp::os::Value(remote + "/depthImage:o")},
        {"remoteRpcPort", yarp::os::Value(remote + "/rpc:i")}};

    if (!sensorDevice.open(sensorOptions) || !sensorDevice.view(iRGBDSensor))
    {
        return false;
    }

    yarp::os::Property depthIntrinsic;

    if (!iRGBDSensor->getDepthIntrinsicParam(depthIntrinsic))
    {
        yError() << "Unable to retrieve depth intrinsic params";
        return false;
    }

    depthIntrinsicParams.fromProperty(depthIntrinsic);

    if (config.check("roi", "ROI of depth frame encoded as (minX maxX minY maxY)"))
    {
        auto b_roi = config.findGroup("roi").tail();

        if (b_roi.size() != 4)
        {
            yError() << "--roi must be a list of 4 unsigned ints";
            return 1;
        }

        roi.min_x = b_roi.get(0).asInt32();
        roi.max_x = b_roi.get(1).asInt32();
        roi.min_y = b_roi.get(2).asInt32();
        roi.max_y = b_roi.get(3).asInt32();
    }

    if (config.check("step", "decimation step of depth frame encoded as (stepX stepY)"))
    {
        auto b_step = config.findGroup("step").tail();

        if (b_step.size() != 2)
        {
            yError() << "--step must be a list of 2 unsigned ints";
            return 1;
        }

        stepX = b_step.get(0).asInt32();
        stepY = b_step.get(1).asInt32();
    }

    meshOptions = {
        {
            {"algorithm", yarp::os::Value("OrganizedFastMesh")},
            {"maxEdgeLengthA", config.check("maxEdgeLengthA", yarp::os::Value(MAX_EDGE_LENGTH_A), "max edge length")}, // default: 0.0
            {"trianglePixelSize", config.check("trianglePixelSize", yarp::os::Value(TRIANGLE_PIXEL_SIZE), "triangle pixel size")}, // default: 1
            {"triangulationType", yarp::os::Value("triangleAdaptiveCut")}, // default: quadMesh
            {"useDepthAsDistance", yarp::os::Value(true)}
        },
        {
            {"algorithm", yarp::os::Value("SimplificationRemoveUnusedVertices")}
        }
    };

    penv->StopSimulation();
    penv->StartSimulation(0.01);

    double period = config.check("period", yarp::os::Value(DEFAULT_PERIOD), "frame acquisition period [s]").asFloat64();
    yarp::os::PeriodicThread::setPeriod(period);

#if YARP_VERSION_MINOR < 5
    // Wait for the first few frames to arrive. We kept receiving invalid pixel codes
    // from the depthCamera device if started straight away.
    // https://github.com/roboticslab-uc3m/vision/issues/88
    yarp::os::Time::delay(0.1);
#endif

    return yarp::os::PeriodicThread::start();
}

// -----------------------------------------------------------------------------

bool YarpOpenraveMeshFromRealDepth::close()
{
    return sensorDevice.close();
}

// -----------------------------------------------------------------------------
