// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveMeshFromRealDepth.hpp"

#include <cmath> // M_PI

#include <algorithm> // std::copy
#include <iterator> // std::back_inserter

#include <yarp/os/LogStream.h>
#include <yarp/sig/PointCloudUtils.h>

#include <YarpCloudUtils.hpp>

#include "LogComponent.hpp"

using namespace roboticslab;

// -----------------------------------------------------------------------------

void YarpOpenraveMeshFromRealDepth::run()
{
    // Acquire depth frame
    yarp::sig::ImageOf<yarp::sig::PixelFloat> depthImage;

    if (!iRGBDSensor->getDepthImage(depthImage))
    {
        yCWarning(YORMFRD) << "No frame received!";
        return;
    }

    // Compute point cloud
    yarp::sig::PointCloudXYZ cloud = yarp::sig::utils::depthToPC(depthImage, depthIntrinsicParams, roi, stepX, stepY);

    // Reconstruct surface mesh
    yarp::sig::PointCloudXYZ meshVertices;
    yarp::sig::VectorOf<int> meshIndices;

    if (!YarpCloudUtils::meshFromCloud(cloud, meshVertices, meshIndices, meshOptions))
    {
        yCError(YORMFRD) << "Reconstruction failed!";
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
        raveMesh.vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    std::copy(meshIndices.cbegin(), meshIndices.cend(), std::back_inserter(raveMesh.indices));

    // Paint the mesh
    OpenRAVE::EnvironmentLock lock(penv->GetMutex());
    OpenRAVE::KinBodyPtr meshKinBodyPtr = OpenRAVE::RaveCreateKinBody(penv, "");
    meshKinBodyPtr->SetName("mesh");
    meshKinBodyPtr->InitFromTrimesh(raveMesh, true);
    meshKinBodyPtr->SetTransform(depthSensor->GetTransform());

    if (previousMesh)
    {
        penv->Remove(previousMesh);
    }

    penv->Add(meshKinBodyPtr, OpenRAVE::IAM_AllowRenaming);
    previousMesh = meshKinBodyPtr;
}

// -----------------------------------------------------------------------------
