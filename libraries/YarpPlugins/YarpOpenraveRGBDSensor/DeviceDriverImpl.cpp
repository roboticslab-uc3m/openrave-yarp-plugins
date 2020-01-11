// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

#include <string>
#include <vector>

#include <yarp/os/Value.h>

#include <ColorDebug.h>

namespace roboticslab
{

const int YarpOpenraveRGBDSensor::NOT_SET = -1;

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

    int depthSensorIndex = config.check("depthSensorIndex",yarp::os::Value(NOT_SET),"depth sensor index").asInt32();
    int rgbSensorIndex = config.check("rgbSensorIndex",yarp::os::Value(NOT_SET),"RGB sensor index").asInt32();

    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();

    if (depthSensorIndex == NOT_SET)  // Must be checked before comparison  with size which may be unsigned.
    {
        CD_ERROR("depthSensorIndex %d == NOT_SET, not loading yarpPlugin.\n",depthSensorIndex);
        return false;
    }
    else if((depthSensorIndex >= vectorOfSensorPtr.size()) || (depthSensorIndex < 0))
    {
        CD_ERROR("depthSensorIndex %d not in vectorOfSensorPtr of size() %d, not loading yarpPlugin.\n",depthSensorIndex,vectorOfSensorPtr.size());
        return false;
    }

    if (rgbSensorIndex == NOT_SET)  // Must be checked before comparison  with size which may be unsigned.
    {
        CD_WARNING("rgbSensorIndex %d == NOT_SET, special case, running with RGB disabled.\n",rgbSensorIndex);
        rgb = false;
        rgbWidth = 0;
        rgbHeight = 0;
    }
    else if(rgbSensorIndex >= vectorOfSensorPtr.size())
    {
        CD_ERROR("rgbSensorIndex %d >= vectorOfSensorPtr.size() %d, not loading yarpPlugin.\n",rgbSensorIndex,vectorOfSensorPtr.size());
        return false;
    }
    else if (rgbSensorIndex < 0)
    {
        CD_ERROR("rgbSensorIndex %d < 0 (and not NOT_SET), not loading yarpPlugin.\n",rgbSensorIndex);
        return false;
    }

    //-- Depth sensor
    depthSensorBasePtr = vectorOfSensorPtr.at(depthSensorIndex)->GetSensor();

    std::string depthName = depthSensorBasePtr->GetName();

    printf("Depth sensor %d name: %s\n",depthSensorIndex,depthName.c_str());

    // printf("Depth sensor %d description: %s\n",depthSensorIndex,depthSensorBasePtr->GetDescription().c_str());
    if ( ! depthSensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Laser) )
    {
        CD_ERROR("Depth sensor %d does not support ST_Laser.\n", depthSensorIndex );
        return false;
    }

    // Activate the sensor
    depthSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Render data on
    //depthSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);  // Show projected yellow shade

    // Get pointer to geom properties of sensor
    boost::shared_ptr<OpenRAVE::SensorBase::LaserGeomData const> depthGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserGeomData const>(depthSensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Laser));

    // Get pointer to sensed data
    depthSensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserSensorData>(depthSensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Laser));

    CD_INFO("Laser min_angle: %f   %f.\n",depthGeomDataPtr->min_angle[0],depthGeomDataPtr->min_angle[1]);  // boost::array<dReal,2>
    CD_INFO("Laser max_angle: %f   %f.\n",depthGeomDataPtr->max_angle[0],depthGeomDataPtr->max_angle[1]);  // boost::array<dReal,2>
    CD_INFO("Laser resolution: %f   %f.\n",depthGeomDataPtr->resolution[0],depthGeomDataPtr->resolution[1]);  // boost::array<dReal,2>
    CD_INFO("Laser min_range, max_range: %f   %f.\n",depthGeomDataPtr->min_range,depthGeomDataPtr->max_range);
    CD_INFO("Laser time_increment: %f   %f.\n",depthGeomDataPtr->time_increment);
    CD_INFO("Laser time_scan: %f   %f.\n",depthGeomDataPtr->time_scan);

    depthHeight = 0;
    depthWidth = 0;

    depthReady = true;

    //-- RGB sensor
    if(!rgb) //-- treat special no RGB case
    {
        rgbSensorBasePtr = nullptr;
        rgbReady = true;
        return true;
    }

    rgbSensorBasePtr = vectorOfSensorPtr.at(rgbSensorIndex)->GetSensor();

    std::string rgbName = rgbSensorBasePtr->GetName();
    printf("Rgb sensor %d name: %s\n",rgbSensorIndex,rgbName.c_str());
    // printf("Rgb sensor %d description: %s\n",rgbSensorIndex,rgbSensorBasePtr->GetDescription().c_str());

    if ( ! rgbSensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Camera) )
    {
        CD_ERROR("Rgb sensor %d does not support ST_Camera.\n", rgbSensorIndex );
        return false;
    }

    // Activate the sensor
    rgbSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_PowerOn);

    // Render data on
    //rgbSensorBasePtr->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);  // Show the sensor image in a separate window

    // Get pointer to geom properties of sensor
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> rgbGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(rgbSensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));

    // Get pointer to sensed data
    rgbSensorDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraSensorData>(rgbSensorBasePtr->CreateSensorData(OpenRAVE::SensorBase::ST_Camera));

    CD_INFO("Rgb width: %d, height: %d.\n",rgbGeomDataPtr->width,rgbGeomDataPtr->height);
    rgbWidth = rgbGeomDataPtr->width;
    rgbHeight = rgbGeomDataPtr->height;

    rgbReady = true;

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
