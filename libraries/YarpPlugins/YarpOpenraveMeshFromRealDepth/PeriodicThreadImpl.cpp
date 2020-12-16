// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveMeshFromRealDepth.hpp"

#include <cmath> // M_PI

#include <algorithm> // std::copy
#include <iterator> // std::back_inserter

#include <yarp/os/LogStream.h>
#include <yarp/sig/PointCloudUtils.h>

#include <YarpCloudUtils.hpp>

using namespace roboticslab;

// -----------------------------------------------------------------------------

void YarpOpenraveMeshFromRealDepth::run()
{
    // Acquire depth frame
    yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;

    if (!iRGBDSensor->getDepthImage(depthImage))
    {
        yWarning() << "No frame received!";
        return;
    }

    // Compute point cloud
    yarp::sig::PointCloudXYZ cloud = yarp::sig::utils::depthToPC(depthImage, depthIntrinsicParams, roi, stepX, stepY);

    // Reconstruct surface mesh
    yarp::sig::PointCloudXYZ meshVertices;
    yarp::sig::VectorOf<int> meshIndices;

    if (!YarpCloudUtils::meshFromCloud(cloud, meshVertices, meshIndices, meshOptions))
    {
        yError() << "Reconstruction failed!";
        yarp::os::PeriodicThread::askToStop();
        return;
    }

    // Convert to OpenRAVE triangle mesh
    OpenRAVE::TriMesh raveMesh;
    raveMesh.vertices.reserve(meshVertices.size());
    raveMesh.indices.reserve(meshIndices.size());

    for (auto i = 0; i < meshVertices.size(); i++)
    {
        const auto & vertex = meshVertices(i);
        raveMesh.vertices.push_back(OpenRAVE::Vector(vertex.x, vertex.y, vertex.z));
    }

    std::copy(meshIndices.cbegin(), meshIndices.cend(), std::back_inserter(raveMesh.indices));

    // Paint the mesh
    OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex());
    OpenRAVE::KinBodyPtr meshKinBodyPtr = OpenRAVE::RaveCreateKinBody(penv, "");
    meshKinBodyPtr->SetName("mesh");
    meshKinBodyPtr->InitFromTrimesh(raveMesh, true);

    if (pos.size() + ori.size() != 0)
    {
        OpenRAVE::Transform T = meshKinBodyPtr->GetTransform();

        if (pos.size() != 0)
        {
            T.trans.x = pos.get(0).asFloat64();
            T.trans.y = pos.get(1).asFloat64();
            T.trans.z = pos.get(2).asFloat64();
        }

        if (ori.size() != 0)
        {
            OpenRAVE::Vector axis(ori.get(0).asFloat64(), ori.get(1).asFloat64(), ori.get(2).asFloat64());
            OpenRAVE::dReal angle = ori.get(3).asFloat64();
            T.rot = OpenRAVE::geometry::quatFromAxisAngle(axis, angle * M_PI / 180.0f);
        }

        meshKinBodyPtr->SetTransform(T);
    }

    if (previousMesh)
    {
        penv->Remove(previousMesh);
    }

    penv->Add(meshKinBodyPtr, true);
    previousMesh = meshKinBodyPtr;
}

// -----------------------------------------------------------------------------
