// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__
#define __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Property.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/PointCloudUtils.h>
#include <yarp/sig/Vector.h>

#include <openrave/openrave.h>

#include "YarpOpenraveBase.hpp"

#define DEFAULT_PERIOD 0.1 // [s]
#define DEFAULT_LOCAL_PREFIX "/YarpOpenraveDepthToMesh/client"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * @defgroup YarpOpenraveMeshFromRealDepth
 *
 * @brief Contains roboticslab::YarpOpenraveMeshFromRealDepth.
 */

/**
 * @ingroup YarpOpenraveMeshFromRealDepth
 * @brief Converts depth frames into a 3D mesh.
 */
class YarpOpenraveMeshFromRealDepth : YarpOpenraveBase,
                                      public yarp::os::PeriodicThread,
                                      public yarp::dev::DeviceDriver
{
public:
    YarpOpenraveMeshFromRealDepth() : yarp::os::PeriodicThread(DEFAULT_PERIOD)
    {}

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp -------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

protected:
    // ------- PeriodicThread declarations. Implementation in PeriodicThreadImpl.cpp -------
    void run() override;

private:
    yarp::dev::PolyDriver sensorDevice;
    yarp::dev::IRGBDSensor * iRGBDSensor {nullptr};
    yarp::sig::IntrinsicParams depthIntrinsicParams;
    yarp::sig::VectorOf<yarp::os::Property> meshOptions;

    OpenRAVE::KinBodyPtr previousMesh {nullptr};
    OpenRAVE::RobotBase::AttachedSensorPtr depthSensor;

    yarp::sig::utils::PCL_ROI roi;
    int stepX {1};
    int stepY {1};
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__
