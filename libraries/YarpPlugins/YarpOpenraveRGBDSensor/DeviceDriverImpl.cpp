// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRGBDSensor::open(yarp::os::Searchable& config)
{
    CD_DEBUG("config: %s\n",config.toString().c_str());

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    int depthSensorIndex = config.check("depthSensorIndex",-1,"depthSensorIndex").asInt();
    int rgbSensorIndex = config.check("rgbSensorIndex",-1,"rgbSensorIndex").asInt();

    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();
    if(depthSensorIndex >= vectorOfSensorPtr.size())
    {
        CD_ERROR("depthSensorIndex %d >= vectorOfSensorPtr.size() %d, not loading yarpPlugin.\n",depthSensorIndex,vectorOfSensorPtr.size());
        return false;
    }
    else if (depthSensorIndex < 0)
    {
        CD_ERROR("depthSensorIndex %d < 0, not loading yarpPlugin.\n",depthSensorIndex);
        return false;
    }
    if(rgbSensorIndex >= vectorOfSensorPtr.size())
    {
        CD_ERROR("rgbSensorIndex %d >= vectorOfSensorPtr.size() %d, not loading yarpPlugin.\n",rgbSensorIndex,vectorOfSensorPtr.size());
        return false;
    }
    else if (rgbSensorIndex < 0)
    {
        CD_ERROR("rgbSensorIndex %d < 0, not loading yarpPlugin.\n",rgbSensorIndex);
        return false;
    }

    depthSensorBasePtr = vectorOfSensorPtr.at(depthSensorIndex)->GetSensor();
    rgbSensorBasePtr = vectorOfSensorPtr.at(rgbSensorIndex)->GetSensor();

    std::string depthName = depthSensorBasePtr->GetName();
    std::string rgbName = rgbSensorBasePtr->GetName();

    printf("Depth sensor %d name: %s\n",depthSensorIndex,depthName.c_str());
    printf("Rgb sensor %d name: %s\n",rgbSensorIndex,rgbName.c_str());

    // printf("Depth sensor %d description: %s\n",depthSensorIndex,depthSensorBasePtr->GetDescription().c_str());
    // printf("Rgb sensor %d description: %s\n",rgbSensorIndex,rgbSensorBasePtr->GetDescription().c_str());

    if ( ! depthSensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Laser) )
    {
        CD_ERROR("Depth sensor %d does not support ST_Laser.\n", depthSensorIndex );
    }
    if ( ! rgbSensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Camera) )
    {
        CD_ERROR("Rgb sensor %d does not support ST_Camera.\n", rgbSensorIndex );
    }

    // Activate the sensor
    depthSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);
    rgbSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Render data on
    //depthSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);  // Show projected yellow shade
    //rgbSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);  // Show the sensor image in a separate window

    // Get pointer to geom properties of sensor
    boost::shared_ptr<OpenRAVE::SensorBase::LaserGeomData const> depthGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserGeomData const>(depthSensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Laser));
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> rgbGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(rgbSensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));

    // Get pointer to sensed data
    depthSensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserSensorData>(depthSensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Laser));
    rgbSensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraSensorData>(rgbSensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Camera));

    CD_INFO("Laser min_angle: %f   %f.\n",depthGeomDataPtr->min_angle[0],depthGeomDataPtr->min_angle[1]);  // boost::array<dReal,2>
    CD_INFO("Laser max_angle: %f   %f.\n",depthGeomDataPtr->max_angle[0],depthGeomDataPtr->max_angle[1]);  // boost::array<dReal,2>
    CD_INFO("Laser resolution: %f   %f.\n",depthGeomDataPtr->resolution[0],depthGeomDataPtr->resolution[1]);  // boost::array<dReal,2>
    CD_INFO("Laser min_range, max_range: %f   %f.\n",depthGeomDataPtr->min_range,depthGeomDataPtr->max_range);
    CD_INFO("Laser time_increment: %f   %f.\n",depthGeomDataPtr->time_increment);
    CD_INFO("Laser time_scan: %f   %f.\n",depthGeomDataPtr->time_scan);

    CD_INFO("Rgb width: %d, height: %d.\n",rgbGeomDataPtr->width,rgbGeomDataPtr->height);
    rgbWidth = rgbGeomDataPtr->width;
    rgbHeight = rgbGeomDataPtr->height;
    depthHeight = 0;
    depthWidth = 0;

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::close()
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab

