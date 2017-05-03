// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "DepthToOR.hpp"

/************************************************************************/

void SetViewer(EnvironmentBasePtr penv, const string& viewername) {
    ViewerBasePtr viewer = RaveCreateViewer(penv,viewername);
    BOOST_ASSERT(!!viewer);
    // attach it to the environment:
    penv->AttachViewer(viewer);
    // finally you call the viewer's infinite loop (this is why you need a separate thread):
    bool showgui = true; // = false;
    viewer->main(showgui);
}

/************************************************************************/

void DepthToOR::init() {
    //-- First initialize the simulator
    this->initOR();
    //-- Then initialize the sensor (starts callback)
    this->initSensor();
        boost::this_thread::sleep (boost::posix_time::seconds (5));
     interface->stop ();
   
}

/************************************************************************/

void DepthToOR::run() {
    while(1){
        boost::this_thread::sleep (boost::posix_time::seconds (5));
        cout << "DepthToOR alive..." << endl;
    }
}

/************************************************************************/

void DepthToOR::quit() {
    this->quitSensor();
    this->quitOR();
}

/************************************************************************/

void DepthToOR::sensorCallback(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr &cloud) {
    //cout << "Begin sensorCallback." << endl;
    gettimeofday(&start, NULL);

    //cout << "Begin removeNaNFromPointCloud, size: " << cloud->points.size() << "." << endl;
    std::vector<int> indices;
    pcl::PointCloud<pcl::PointXYZ>::Ptr modCloud1 (new pcl::PointCloud<pcl::PointXYZ>);
    //-- http://docs.pointclouds.org/1.7.0/group__filters.html#gac463283a9e9c18a66d3d29b28a575064
    //-- removeNaNFromPointCloud can be called with cloud_in == cloud_out
    //-- But can't here because we have a ConstPtr
    pcl::removeNaNFromPointCloud(*cloud, *modCloud1, indices);

    //cout << "Begin VoxelGrid, size: " << modCloud1->points.size() << "." << endl;
    pcl::PointCloud<pcl::PointXYZ>::Ptr modCloud2 (new pcl::PointCloud<pcl::PointXYZ>);
    pcl::VoxelGrid<pcl::PointXYZ> vg;
    vg.setInputCloud (modCloud1);
    //vg.setLeafSize (0.01f, 0.01f, 0.01f);  // mesh
    vg.setLeafSize (0.05f, 0.05f, 0.05f);  // boxes
    vg.filter (*modCloud2);

    // \begin{raw point spheres}
    /*cout << "Begin operating with points, size: " << modCloud2->size() << "." << endl;
    std::vector<Vector> spheres(1);
    for(size_t point_index = 0; point_index < modCloud2->size(); point_index++) {
        spheres.push_back( Vector( modCloud2->points[point_index].x,
                                   modCloud2->points[point_index].y,
                                   modCloud2->points[point_index].z, 0.005 ));  // Last is radius.
    }
    { // lock the environment!
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex());
        KinBodyPtr ssphKinBodyPtr = RaveCreateKinBody(penv,"");
        ssphKinBodyPtr->SetName("ssphFromPCL");
        ssphKinBodyPtr->InitFromSpheres(spheres,true);
        //-- Remove previous
        if (storeSsphKinBodyPtr) penv->Remove(storeSsphKinBodyPtr);
        //-- Add new
        penv->Add(ssphKinBodyPtr,true);
        //-- Store for removal
        storeSsphKinBodyPtr = ssphKinBodyPtr;
    }  // the environment is not locked anymore*/
    // \end{raw point spheres}

    //cout << "Begin normal estimation + Poisson, size: " << modCloud2->points.size() << "." << endl;
    //-- thanks: http://www.pcl-users.org/Access-pcl-PolygonMesh-triangles-data-td4025718.html
    //pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
    pcl::NormalEstimationOMP<pcl::PointXYZ, pcl::Normal> n;  // 
    pcl::PointCloud<pcl::Normal>::Ptr normals (new pcl::PointCloud<pcl::Normal>);
    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
    tree->setInputCloud(modCloud2);
    n.setInputCloud(modCloud2);
    n.setSearchMethod(tree);
    n.setKSearch (40);  // 20
    //n.setKSearch (modCloud2->size());
    //n.setNumberOfThreads(6);  // 1:7.8. 7:7.5. 20:7.4.
    n.compute (*normals);
    // Concatenate the XYZ and normal fields (cloud_with_normals = cloud + normals)
    pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals (new pcl::PointCloud<pcl::PointNormal>);
    pcl::concatenateFields (*modCloud2, *normals, *cloud_with_normals);
    // Create search tree
    pcl::search::KdTree<pcl::PointNormal>::Ptr tree2 (new pcl::search::KdTree<pcl::PointNormal>);
    tree2->setInputCloud (cloud_with_normals);
    pcl::Poisson<pcl::PointNormal> psr;  // Only pcl >= 1.6
    pcl::PolygonMesh triangles;
    psr.setInputCloud(cloud_with_normals);
    psr.setSearchMethod(tree2);
    psr.reconstruct(triangles);

    cout << "Begin operating with triangles, size: " << triangles.polygons.size() << "." << endl;
    pcl::PointCloud<pcl::PointXYZ> meshCloud;
    fromROSMsg (triangles.cloud, meshCloud);
    OpenRAVE::KinBody::Link::TRIMESH raveMesh;
    for(size_t triangle_index = 0; triangle_index < meshCloud.size(); triangle_index++) {
        // vert 0
        raveMesh.indices.push_back(3*triangle_index);
        size_t point0_index = triangles.polygons[triangle_index].vertices[0];
        raveMesh.vertices.push_back( Vector(meshCloud.points[point0_index].x,
                                            meshCloud.points[point0_index].y,
                                            meshCloud.points[point0_index].z) );
        // vert 1
        raveMesh.indices.push_back(1+3*triangle_index);
        size_t point1_index = triangles.polygons[triangle_index].vertices[1];
        raveMesh.vertices.push_back( Vector(meshCloud.points[point1_index].x,
                                            meshCloud.points[point1_index].y,
                                            meshCloud.points[point1_index].z) );
        // vert 2
        raveMesh.indices.push_back(2+3*triangle_index);
        size_t point2_index = triangles.polygons[triangle_index].vertices[2];
        raveMesh.vertices.push_back( Vector(meshCloud.points[point2_index].x,
                                            meshCloud.points[point2_index].y,
                                            meshCloud.points[point2_index].z) );
    }

    { // lock the environment!
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex());
        KinBodyPtr meshKinBodyPtr = RaveCreateKinBody(penv,"");
        meshKinBodyPtr->SetName("m");
        meshKinBodyPtr->InitFromTrimesh(raveMesh,true);
        //-- Remove previous
        if (storeMeshKinBodyPtr) penv->Remove(storeMeshKinBodyPtr);
        //-- Add new
        penv->Add(meshKinBodyPtr,true);
        //-- Store for removal
        storeMeshKinBodyPtr=meshKinBodyPtr;
    }

    // \begin{raw point cubes}
    /*cout << "Begin operating with points, size: " << modCloud2->size() << "." << endl;
    std::vector<AABB> boxes(modCloud2->size());
    for(size_t point_index = 0; point_index < modCloud2->size(); point_index++) {
        boxes[point_index].extents = Vector(0.005,0.005,0.005);
        boxes[point_index].pos = Vector(modCloud2->points[point_index].x,
                                        modCloud2->points[point_index].y,
                                        modCloud2->points[point_index].z);
    }
    { // lock the environment!
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex());
        KinBodyPtr sboxKinBodyPtr = RaveCreateKinBody(penv,"");
        sboxKinBodyPtr->SetName("sboxFromPCL");
        sboxKinBodyPtr->InitFromBoxes(boxes,true);
        //-- Remove previous
        if (storeSboxKinBodyPtr) penv->Remove(storeSboxKinBodyPtr);
        //-- Add new
        penv->Add(sboxKinBodyPtr,true);
        //-- Store for removal
        storeSboxKinBodyPtr = sboxKinBodyPtr;
    }  // the environment is not locked anymore*/
    // \end{raw point cubes}

    //-- Output time
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    cout << "---------------------> Took: " << mtime << " milliseconds." << endl;
}

/************************************************************************/

void DepthToOR::initSensor() {

    interface = new pcl::OpenNIGrabber();

    boost::function<void (const pcl::PointCloud<pcl::PointXYZ>::ConstPtr&)> f =
        boost::bind (&DepthToOR::sensorCallback, this, _1);

    interface->registerCallback (f);

    interface->start ();

}

/************************************************************************/

void DepthToOR::quitSensor() {
    printf("bye!\n");
    interface->stop ();
    delete interface;
    interface = 0;
}

/************************************************************************/

void DepthToOR::initOR() {
    RaveInitialize(true); // start openrave core
    penv = RaveCreateEnvironment(); // create the main environment
    RaveSetDebugLevel(Level_Debug);
    string viewername = "qtcoin";
    boost::thread thviewer(boost::bind(SetViewer,penv,viewername));
}

/************************************************************************/

void DepthToOR::quitOR() {

}

/************************************************************************/

