// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__
#define __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__

#include <yarp/os/Bottle.h>
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
    YarpOpenraveMeshFromRealDepth() : yarp::os::PeriodicThread(DEFAULT_PERIOD),
                                      iRGBDSensor(nullptr),
                                      previousMesh(nullptr),
                                      stepX(1),
                                      stepY(1)
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
    yarp::dev::IRGBDSensor * iRGBDSensor;
    yarp::sig::IntrinsicParams depthIntrinsicParams;
    yarp::sig::VectorOf<yarp::os::Property> meshOptions;
    OpenRAVE::KinBodyPtr previousMesh;
    yarp::os::Bottle pos;
    yarp::os::Bottle ori;
    yarp::sig::utils::PCL_ROI roi;
    int stepX;
    int stepY;
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_MESH_FROM_REAL_DEPTH_HPP__
