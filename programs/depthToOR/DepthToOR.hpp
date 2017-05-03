// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __DEPTH_TO_MESH_HPP__
#define __DEPTH_TO_MESH_HPP__

// PCL
#include <pcl/io/openni_grabber.h>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree_flann.h>
//#include <pcl/surface/mls.h>  // too slow
#include <pcl/ModelCoefficients.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/filter.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
//#include <pcl/io/ply_io.h>  // for file writer
#include <pcl/kdtree/kdtree.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>
//#include <pcl/surface/gp3.h>  // better use poisson; this leaves holes
#include <pcl/surface/poisson.h>
//#include <pcl/surface/vtk_utils.h> // worked in pcl-1.6
#include <pcl/surface/vtk_smoothing/vtk_utils.h> // worked in pcl-1.7
#include <pcl/ros/conversions.h>

#include <pcl/features/normal_3d_omp.h>

// OR
#include <openrave-core.h>
#include <string>
using namespace OpenRAVE;
using namespace std;

class DepthToOR {

    public:
        DepthToOR () {}

        void sensorCallback(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr &cloud);

        void init();
        void run();
        void quit();

    private:
        //-- Sensor
        void initSensor();
        void quitSensor();
        pcl::Grabber* interface;

        //-- OpenRAVE
        void initOR();
        void quitOR();
        RobotBasePtr probot;
        EnvironmentBasePtr penv;
        ControllerBasePtr pcontrol;
        //KinBodyPtr storeSsphKinBodyPtr;
        KinBodyPtr storeMeshKinBodyPtr;
        //KinBodyPtr storeSboxKinBodyPtr;

        //-- Timing
        struct timeval start, end;
        long mtime, seconds, useconds;  
};

#endif  // __DEPTH_TO_MESH_HPP__


