// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__
#define __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__

#include <yarp/conf/version.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Property.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/IntrinsicParams.h>
#if YARP_VERSION_MINOR >= 4
# include <yarp/sig/PointCloudUtils.h>
#endif
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

    ~YarpOpenraveMeshFromRealDepth()
    { close(); }

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp -------
    virtual bool open(yarp::os::Searchable & config) override;
    virtual bool close() override;

protected:
    // ------- PeriodicThread declarations. Implementation in PeriodicThreadImpl.cpp -------
    virtual void run() override;

private:
    yarp::dev::PolyDriver sensorDevice;
    yarp::dev::IRGBDSensor * iRGBDSensor {nullptr};
    yarp::sig::IntrinsicParams depthIntrinsicParams;
    yarp::sig::VectorOf<yarp::os::Property> meshOptions;

    OpenRAVE::KinBodyPtr previousMesh {nullptr};
    OpenRAVE::RobotBase::AttachedSensorPtr depthSensor;

#if YARP_VERSION_MINOR >= 4
    yarp::sig::utils::PCL_ROI roi;
    int stepX {1};
    int stepY {1};
#endif
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__
