// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

#include <string>
#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include "LogComponent.hpp"

using namespace roboticslab;

constexpr auto NOT_SET = -1;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRGBDSensor::open(yarp::os::Searchable& config)
{
    if (!configureEnvironment(config))
        return false;

    if (!configureOpenravePlugins(config))
        return false;

    if (!configureRobot(config))
        return false;

    int depthSensorIndex = config.check("depthSensorIndex", yarp::os::Value(NOT_SET),"depth sensor index").asInt32();
    int rgbSensorIndex = config.check("rgbSensorIndex", yarp::os::Value(NOT_SET),"RGB sensor index").asInt32();

    std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> vectorOfSensorPtr = probot->GetAttachedSensors();

    if (depthSensorIndex == NOT_SET)  // Must be checked before comparison  with size which may be unsigned.
    {
        yCError(YORRS) << "depthSensorIndex" << depthSensorIndex << "== NOT_SET, not loading yarpPlugin";
        return false;
    }
    else if (depthSensorIndex >= vectorOfSensorPtr.size() || depthSensorIndex < 0)
    {
        yCError(YORRS, "depthSensorIndex %d not in vectorOfSensorPtr of size() %zu, not loading yarpPlugin",depthSensorIndex,vectorOfSensorPtr.size());
        return false;
    }

    if (rgbSensorIndex == NOT_SET)  // Must be checked before comparison  with size which may be unsigned.
    {
        yCWarning(YORRS) << "rgbSensorIndex" << rgbSensorIndex << "== NOT_SET, special case, running with RGB disabled";
        rgb = false;
        rgbWidth = 0;
        rgbHeight = 0;
    }
    else if (rgbSensorIndex >= vectorOfSensorPtr.size())
    {
        yCError(YORRS, "rgbSensorIndex %d >= vectorOfSensorPtr.size() %zu, not loading yarpPlugin",rgbSensorIndex,vectorOfSensorPtr.size());
        return false;
    }
    else if (rgbSensorIndex < 0)
    {
        yCError(YORRS) << "rgbSensorIndex" << rgbSensorIndex << "< 0 (and not NOT_SET), not loading yarpPlugin";
        return false;
    }

    //-- Depth sensor
    depthSensorBasePtr = vectorOfSensorPtr.at(depthSensorIndex)->GetSensor();

    std::string depthName = depthSensorBasePtr->GetName();

    yCInfo(YORRS) << "Depth sensor" << depthSensorIndex << "name:" << depthName;

    // printf("Depth sensor %d description: %s\n",depthSensorIndex,depthSensorBasePtr->GetDescription().c_str());
    if (!depthSensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Laser))
    {
        yCError(YORRS) << "Depth sensor" << depthSensorIndex << "does not support ST_Laser";
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

    yCInfo(YORRS) << "Laser min_angle:" << depthGeomDataPtr->min_angle[0] << depthGeomDataPtr->min_angle[1];  // boost::array<dReal,2>
    yCInfo(YORRS) << "Laser max_angle:" << depthGeomDataPtr->max_angle[0] << depthGeomDataPtr->max_angle[1];  // boost::array<dReal,2>
    yCInfo(YORRS) << "Laser resolution:" << depthGeomDataPtr->resolution[0] << depthGeomDataPtr->resolution[1];  // boost::array<dReal,2>
    yCInfo(YORRS) << "Laser min_range, max_range:" << depthGeomDataPtr->min_range << depthGeomDataPtr->max_range;
    yCInfo(YORRS) << "Laser time_increment:" << depthGeomDataPtr->time_increment;
    yCInfo(YORRS) << "Laser time_scan:" << depthGeomDataPtr->time_scan;

    depthHeight = 0;
    depthWidth = 0;

    depthReady = true;

    //-- RGB sensor
    if (!rgb) //-- treat special no RGB case
    {
        rgbSensorBasePtr = nullptr;
        rgbReady = true;

        penv->StopSimulation();
        penv->StartSimulation(0.01);

        return true;
    }

    rgbSensorBasePtr = vectorOfSensorPtr.at(rgbSensorIndex)->GetSensor();

    std::string rgbName = rgbSensorBasePtr->GetName();
    yCInfo(YORRS) << "Rgb sensor" << rgbSensorIndex << "name:" << rgbName;
    // printf("Rgb sensor %d description: %s\n",rgbSensorIndex,rgbSensorBasePtr->GetDescription().c_str());

    if (!rgbSensorBasePtr->Supports(OpenRAVE::SensorBase::ST_Camera))
    {
        yCError(YORRS) << "Rgb sensor" << rgbSensorIndex << "does not support ST_Camera";
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

    yCInfo(YORRS, "Rgb width: %d, height: %d",rgbGeomDataPtr->width,rgbGeomDataPtr->height);
    rgbWidth = rgbGeomDataPtr->width;
    rgbHeight = rgbGeomDataPtr->height;

    rgbReady = true;

    penv->StopSimulation();
    penv->StartSimulation(0.01);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::close()
{
    return true;
}

// -----------------------------------------------------------------------------
