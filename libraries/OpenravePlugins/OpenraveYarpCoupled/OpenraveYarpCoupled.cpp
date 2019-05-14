/**
 * thanks Rosen Diankov
   Every plugin contains a bunch of openrave interfaces, the plugincpp plugin creates a simple OpenRAVE::ModuleBase interface named \b mymodule.
   Inside programs, load the plugin using the RaveLoadPlugin, and then create the module the plugin offers using
   \verbatim
   m=RaveCreateModule(env,"mymodule");
   \endverbatim
   To test things through the command line, do:
   \verbatim
   openrave --loadplugin libplugincpp.so --module mymodule "my args"
   \endverbatim
   This will load liboplugincpp.so and startup module "mymodule". From plugincpp, notice that mymodule
   supports some "commands". These are in-process string-based calls invoked through
   interface->SendCommand function.
   If you are using octave or matlab, then can communicate with openrave through tcp/ip, check out: http://openrave.programmingvision.com/wiki/index.php/OctaveMATLAB
   Most openrave users use python to dynamically interact with openrave. For example:
   \verbatim
   openrave.py -i  --loadplugin libplugincpp.so data/lab1.env.xml
   \endverbatim
   drops into the python promp with the plugin loaded and a scene loaded. Then it is possible to execute the following python commands to create the interface and call a command:
   \verbatim
   m=RaveCreateModule(env,'mymodule')
   env.Add(m,true,'my args')
   m.SendCommand('numbodies')
   \endverbatim
   <b>Full Example Code:</b>
 */

#include <cstdio>

#include <sstream>
#include <string>
#include <vector>

#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/PolyDriver.h>

#include <yarp/sig/Image.h>

#define NULL_JMC_MS 20

class TeoSimRateThread : public yarp::os::PeriodicThread {
     public:

        // Set the Thread Rate in the class constructor
        TeoSimRateThread() : yarp::os::PeriodicThread(NULL_JMC_MS * 0.001) {}  // In seconds

        void setEnvironmentPtr(const OpenRAVE::EnvironmentBasePtr& environmentPtr) {
            this->environmentPtr = environmentPtr;
        }

        void setPtrVectorOfRobotPtr(std::vector< OpenRAVE::RobotBasePtr > * ptrVectorOfRobotPtr) {
            this->ptrVectorOfRobotPtr = ptrVectorOfRobotPtr;
        }

        void setPtrVectorOfSensorPtrForCameras(std::vector< OpenRAVE::SensorBasePtr > * ptrVectorOfSensorPtrForCameras) {
            this->ptrVectorOfSensorPtrForCameras = ptrVectorOfSensorPtrForCameras;
        }

        void setPtrVectorOfSensorPtrForLasers(std::vector< OpenRAVE::SensorBasePtr > * ptrVectorOfSensorPtrForLasers) {
            this->ptrVectorOfSensorPtrForLasers = ptrVectorOfSensorPtrForLasers;
        }

        void setPtrVectorOfSensorPtrForForce6Ds(std::vector< OpenRAVE::SensorBasePtr > * ptrVectorOfSensorPtrForForce6Ds) {
            this->ptrVectorOfSensorPtrForForce6Ds = ptrVectorOfSensorPtrForForce6Ds;
        }

        void setPtrVectorOfCameraSensorDataPtr(std::vector< boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> > * ptrVectorOfCameraSensorDataPtr) {
            this->ptrVectorOfCameraSensorDataPtr = ptrVectorOfCameraSensorDataPtr;
        }

        void setPtrVectorOfLaserSensorDataPtr(std::vector< boost::shared_ptr<OpenRAVE::SensorBase::LaserSensorData> > * setPtrVectorOfLaserSensorDataPtr) {
            this->ptrVectorOfLaserSensorDataPtr = setPtrVectorOfLaserSensorDataPtr;
        }

        void setPtrVectorOfForce6DSensorDataPtr(std::vector< boost::shared_ptr<OpenRAVE::SensorBase::Force6DSensorData> > * setPtrVectorOfForce6DSensorDataPtr) {
            this->ptrVectorOfForce6DSensorDataPtr = setPtrVectorOfForce6DSensorDataPtr;
        }

        void setPtrVectorOfCameraWidth(std::vector<int> * ptrVectorOfCameraWidth) {
            this->ptrVectorOfCameraWidth = ptrVectorOfCameraWidth;
        }

        void setPtrVectorOfCameraHeight(std::vector<int> * ptrVectorOfCameraHeight) {
            this->ptrVectorOfCameraHeight = ptrVectorOfCameraHeight;
        }

        void setPtrVectorOfRgbPortPtr(std::vector< yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* > * ptrVectorOfRgbPortPtr) {
            this->ptrVectorOfRgbPortPtr = ptrVectorOfRgbPortPtr;
        }

        void setPtrVectorOfIntPortPtr(std::vector< yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >* > * ptrVectorOfIntPortPtr) {
            this->ptrVectorOfIntPortPtr = ptrVectorOfIntPortPtr;
        }

        void setPtrVectorOfForce6DPortPtr(std::vector< yarp::os::BufferedPort<yarp::os::Bottle >* > * ptrVectorOfForce6DPortPtr) {
            this->ptrVectorOfForce6DPortPtr = ptrVectorOfForce6DPortPtr;
        }


    // -------- RateThread declarations. Implementation in RateThreadImpl.cpp --------

        /**
         * Loop function. This is the thread itself.
         */
        void run();

    // ------------------------------- Protected -------------------------------------
    protected:
        //
        double jmcMs;
        double lastTime;
        //
        // Rave-specific //
        OpenRAVE::EnvironmentBasePtr environmentPtr;
        std::vector<OpenRAVE::RobotBasePtr> * ptrVectorOfRobotPtr;
        //
        std::vector< OpenRAVE::SensorBasePtr > * ptrVectorOfSensorPtrForCameras;
        std::vector< OpenRAVE::SensorBasePtr > * ptrVectorOfSensorPtrForLasers;
        std::vector< OpenRAVE::SensorBasePtr > * ptrVectorOfSensorPtrForForce6Ds;
        std::vector< boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> > * ptrVectorOfCameraSensorDataPtr;
        std::vector< boost::shared_ptr<OpenRAVE::SensorBase::LaserSensorData> > * ptrVectorOfLaserSensorDataPtr;
        std::vector< boost::shared_ptr<OpenRAVE::SensorBase::Force6DSensorData> > * ptrVectorOfForce6DSensorDataPtr;
        std::vector<int> * ptrVectorOfCameraWidth;
        std::vector<int> * ptrVectorOfCameraHeight;
        std::vector< yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* > * ptrVectorOfRgbPortPtr;
        std::vector< yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >* > * ptrVectorOfIntPortPtr;
        std::vector< yarp::os::BufferedPort<yarp::os::Bottle >* > * ptrVectorOfForce6DPortPtr;
        //
};

void TeoSimRateThread::run() {
    //printf("[TeoSimRateThread] run()\n");

//    environmentPtr->StepSimulation(jmcMs/1000.0);  // StepSimulation must be given in seconds

    for(unsigned int camIter = 0; camIter<ptrVectorOfSensorPtrForCameras->size(); camIter++ ) {
        ptrVectorOfSensorPtrForCameras->at(camIter)->GetSensorData(ptrVectorOfCameraSensorDataPtr->at(camIter));
        //std::vector<uint8_t> currentFrame = pcamerasensordata->vimagedata;
        //printf("Vector size: %d\n",currentFrame.size()); // i.e. 480 * 640 * 3 = 921600;
        yarp::sig::ImageOf<yarp::sig::PixelRgb>& i_imagen = ptrVectorOfRgbPortPtr->at(camIter)->prepare();
        i_imagen.resize(ptrVectorOfCameraWidth->at(camIter),ptrVectorOfCameraHeight->at(camIter));  // Tamaño de la pantalla
        yarp::sig::PixelRgb p;
        for (int i_x = 0; i_x < i_imagen.width(); ++i_x) {
            for (int i_y = 0; i_y < i_imagen.height(); ++i_y) {
                p.r = ptrVectorOfCameraSensorDataPtr->at(camIter)->vimagedata[3*(i_x+(i_y*i_imagen.width()))];
                p.g = ptrVectorOfCameraSensorDataPtr->at(camIter)->vimagedata[1+3*(i_x+(i_y*i_imagen.width()))];
                p.b = ptrVectorOfCameraSensorDataPtr->at(camIter)->vimagedata[2+3*(i_x+(i_y*i_imagen.width()))];
                i_imagen.safePixel(i_x,i_y) = p;
            }
        }
        ptrVectorOfRgbPortPtr->at(camIter)->write();
    }

    for(unsigned int laserIter = 0; laserIter<ptrVectorOfSensorPtrForLasers->size(); laserIter++ ) {
        ptrVectorOfSensorPtrForLasers->at(laserIter)->GetSensorData(ptrVectorOfLaserSensorDataPtr->at(laserIter));
        std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorRanges = ptrVectorOfLaserSensorDataPtr->at(laserIter)->ranges;
        std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorPositions = ptrVectorOfLaserSensorDataPtr->at(laserIter)->positions;
        OpenRAVE::Transform tinv = ptrVectorOfLaserSensorDataPtr->at(laserIter)->__trans.inverse();
        // std::vector< dReal > sensorIntensity = plasersensordata[laserIter]->intensity;
        // printf("[%d] sensorPositions size: %d ",laserIter,sensorPositions.size()); // = 1; xyz of the fixed 3d sensor position.
        // printf("sensorRanges size: %d ",sensorRanges.size()); // 64 * 48 = 3072;
        // printf("sensorIntensity size: %d\n",sensorIntensity.size()); // 64 * 48 = 3072;
        //for(unsigned int i=0;i<sensorRanges.size();i++) {
        //   printf("sensorRanges[%d].x: %f  sensorRanges[%d].y: %f  sensorRanges[%d].z: %f sensorIntensity[%d]: %.2f\n",i,sensorRanges[i].x,i,sensorRanges[i].y,i,sensorRanges[i].z,i,sensorIntensity[i]);  // intensity always 1.0 or 0.0 in 3d sensor
        //}*/
        /*Transform tinv = plasersensordata->__trans.inverse();
        for(size_t i = 0; i < _data->ranges.size(); ++i) {
            float* p = (float*)(&_pointcloud2msg.data.at(i*_pointcloud2msg.point_step));
            if( i < _data->positions.size() ) {
                Vector v = tinv*(_data->ranges[i] + _data->positions[i]);
                p[0] = (float)v.x;
                p[1] = (float)v.y;
                p[2] = (float)v.z;
            } else if( _data->positions.size() > 0 ) {
                Vector v = tinv*(_data->ranges[i] + _data->positions[0]);
                p[0] = (float)v.x;
                p[1] = (float)v.y;
                p[2] = (float)v.z;
            } else {
                Vector v = tinv*_data->ranges[i];
                p[0] = (float)v.x;
                p[1] = (float)v.y;
                p[2] = (float)v.z;
            }
            if( _data->intensity.size()==_data->ranges.size() ) {
                p[3] = _data->intensity[i];
            }
        }*/
        yarp::sig::ImageOf<yarp::sig::PixelInt>& i_depth = ptrVectorOfIntPortPtr->at(laserIter)->prepare();
        if(sensorRanges.size()==3072) i_depth.resize(64,48);  // Tamaño de la pantalla (64,48)
        else if(sensorRanges.size()==12288) i_depth.resize(128,96);
        else if(sensorRanges.size()==49152) i_depth.resize(256,192);
        else if(sensorRanges.size()==307200) i_depth.resize(640,480);
        else if(sensorRanges.size()==4) i_depth.resize(2,2);
        //else printf("[warning] unrecognized laser sensor data size.\n");
        else i_depth.resize(sensorRanges.size(),1);
        for (int i_y = 0; i_y < i_depth.height(); ++i_y) {  // was y in x before
            for (int i_x = 0; i_x < i_depth.width(); ++i_x) {
                //double p = sensorRanges[i_y+(i_x*i_depth.height())].z;
                double p;
                if( sensorPositions.size() > 0 ) {
                    OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*i_depth.height())] + sensorPositions[0]);
                    p = (float)v.z;
                } else {
                    OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*i_depth.height())]);
                    p = (float)v.z;
                }
                i_depth(i_x,i_y) = p*1000.0;  // give mm
            }
        }
        ptrVectorOfIntPortPtr->at(laserIter)->write();
    }

    for(unsigned int force6DIter = 0; force6DIter<ptrVectorOfSensorPtrForForce6Ds->size(); force6DIter++ ) {
        ptrVectorOfSensorPtrForForce6Ds->at(force6DIter)->GetSensorData(ptrVectorOfForce6DSensorDataPtr->at(force6DIter));
        yarp::os::Bottle& b = ptrVectorOfForce6DPortPtr->at(force6DIter)->prepare();
        b.clear();
        b.addDouble( ptrVectorOfForce6DSensorDataPtr->at(force6DIter)->force[0] );
        b.addDouble( ptrVectorOfForce6DSensorDataPtr->at(force6DIter)->force[1] );
        b.addDouble( ptrVectorOfForce6DSensorDataPtr->at(force6DIter)->force[2] );
        b.addDouble( ptrVectorOfForce6DSensorDataPtr->at(force6DIter)->torque[0] );
        b.addDouble( ptrVectorOfForce6DSensorDataPtr->at(force6DIter)->torque[1] );
        b.addDouble( ptrVectorOfForce6DSensorDataPtr->at(force6DIter)->torque[2] );
        ptrVectorOfForce6DPortPtr->at(force6DIter)->write();
    }

}

class OpenraveYarpCoupled : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpCoupled(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv) {
        //YARP_REGISTER_PLUGINS(yarpplugins);
        __description = "OpenraveYarpCoulped plugin.";
        OpenRAVE::InterfaceBase::RegisterCommand("open",boost::bind(&OpenraveYarpCoupled::Open, this,_1,_2),"opens OpenraveYarpCoulped");
    }

    virtual ~OpenraveYarpCoupled() {
        for(int i=0;i<robotDevices.size();i++)
        {
            robotDevices[i]->close();
            delete robotDevices[i];
        }
    }

    virtual void Destroy() {

        RAVELOG_INFO("module unloaded from environment\n");
    }

    /*int main(const string& cmd) {
        RAVELOG_INFO("module initialized cmd; %s\n", cmd.c_str());
        return 0;
    }*/

    bool Open(std::ostream& sout, std::istream& sinput)
    {
        bool alternativeRobotName = false;

        std::vector<std::string> funcionArgs;
        while(sinput)
        {
            std::string funcionArg;
            sinput >> funcionArg;
            funcionArgs.push_back(funcionArg);
        }


        if ( !yarp.checkNetwork() )
        {
            RAVELOG_INFO("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }

        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();

        //-- Get robots
        std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
        GetEnv()->GetRobots(vectorOfRobotPtr);

        for ( unsigned int robotIter = 0; robotIter<vectorOfRobotPtr.size(); robotIter++ ) {
            std::vector<OpenRAVE::RobotBase::AttachedSensorPtr> sensors = vectorOfRobotPtr.at(robotIter)->GetAttachedSensors();
            std::printf("Sensors found on robot %d (%s): %d.\n",robotIter,vectorOfRobotPtr.at(robotIter)->GetName().c_str(),(int)sensors.size());
            for ( unsigned int sensorIter = 0; sensorIter<sensors.size(); sensorIter++ ) {
                OpenRAVE::SensorBasePtr psensorbase = sensors.at(sensorIter)->GetSensor();
                std::string tipo = psensorbase->GetName();
                std::printf("Sensor %d name: %s\n",sensorIter,tipo.c_str());
                // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());
                if (psensorbase->Supports(OpenRAVE::SensorBase::ST_Camera)) {
                    std::printf("Sensor %d supports ST_Camera.\n", sensorIter);
                    // Activate the camera
                    psensorbase->Configure(OpenRAVE::SensorBase::CC_PowerOn);
                    // Show the camera image in a separate window
                    // pcamerasensorbase->Configure(SensorBase::CC_RenderDataOn);
                    // Get some camera parameter info
                    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> pcamerageomdata = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(psensorbase->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));
                    // printf("Camera width: %d, height: %d.\n",pcamerageomdata->width,pcamerageomdata->height);
                    vectorOfCameraWidth.push_back(pcamerageomdata->width);
                    vectorOfCameraHeight.push_back(pcamerageomdata->height);
                    // Get a pointer to access the camera data stream
                    vectorOfCameraSensorDataPtr.push_back(boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraSensorData>(psensorbase->CreateSensorData(OpenRAVE::SensorBase::ST_Camera)));
                    vectorOfSensorPtrForCameras.push_back(psensorbase);  // "save"
                    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* tmpPort = new yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >;
                    std::string tmpName("/");
                    std::string cameraSensorString(psensorbase->GetName());
                    size_t pos = cameraSensorString.find("imageMap");
                    if ( pos != std::string::npos) {
                        tmpName += cameraSensorString.substr (0,pos-1);
                        tmpName += "/imageMap:o";
                    } else {
                        tmpName += cameraSensorString.c_str();
                        tmpName += "/img:o";
                    }
                    tmpPort->open(tmpName);
                    vectorOfRgbPortPtr.push_back(tmpPort);
                } else if (psensorbase->Supports(OpenRAVE::SensorBase::ST_Laser)) {
                    std::printf("Sensor %d supports ST_Laser.\n", sensorIter);
                    // Activate the sensor
                    psensorbase->Configure(OpenRAVE::SensorBase::CC_PowerOn);
                    // Paint the rays in the OpenRAVE viewer
                    psensorbase->Configure(OpenRAVE::SensorBase::CC_RenderDataOn);
                    // Get some laser parameter info
                    // boost::shared_ptr<SensorBase::LaserGeomData> plasergeomdata = boost::dynamic_pointer_cast<SensorBase::LaserGeomData>(psensorbase->GetSensorGeometry(SensorBase::ST_Laser));
                    // printf("Laser resolution: %f   %f.\n",plasergeomdata->resolution[0],plasergeomdata->resolution[1]);
                    // printf("Laser min_angle: %f   %f.\n",plasergeomdata->min_angle[0],plasergeomdata->min_angle[1]);
                    // printf("Laser max_angle: %f   %f.\n",plasergeomdata->max_angle[0],plasergeomdata->max_angle[1]);
                    // Get a pointer to access the laser data stream
                    vectorOfLaserSensorDataPtr.push_back(boost::dynamic_pointer_cast<OpenRAVE::SensorBase::LaserSensorData>(psensorbase->CreateSensorData(OpenRAVE::SensorBase::ST_Laser)));
                    vectorOfSensorPtrForLasers.push_back(psensorbase);  // "save"
                    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >* tmpPort = new yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >;
                    std::string tmpName("/");
                    std::string depthSensorString(psensorbase->GetName());
                    size_t pos = depthSensorString.find("depthMap");
                    if ( pos != std::string::npos) {
                        tmpName += depthSensorString.substr (0,pos-1);
                        tmpName += "/depthMap:o";
                    } else {
                        tmpName += depthSensorString.c_str();
                        tmpName += "/depth:o";
                    }
                    tmpPort->open(tmpName);
                    vectorOfIntPortPtr.push_back(tmpPort);
                } else if (psensorbase->Supports(OpenRAVE::SensorBase::ST_Force6D)) {
                    std::printf("Sensor %d supports ST_Force6D.\n", sensorIter);
                    // Activate the sensor
                    psensorbase->Configure(OpenRAVE::SensorBase::CC_PowerOn);
                    // Get a pointer to access the force6D data stream
                    vectorOfForce6DSensorDataPtr.push_back(boost::dynamic_pointer_cast<OpenRAVE::SensorBase::Force6DSensorData>(psensorbase->CreateSensorData(OpenRAVE::SensorBase::ST_Force6D)));
                    vectorOfSensorPtrForForce6Ds.push_back(psensorbase);  // "save"
                    yarp::os::BufferedPort<yarp::os::Bottle > * tmpPort = new yarp::os::BufferedPort<yarp::os::Bottle >;
                    std::string sensorName = psensorbase->GetName();
                    size_t pos = sensorName.find(":");
                    std::string portName("/");
                    portName += sensorName.substr (pos+1,sensorName.size());
                    tmpPort->open(portName);
                    vectorOfForce6DPortPtr.push_back(tmpPort);
                } else std::printf("Sensor %d not supported.\n", robotIter);
            }
        }

        // Start the RateThread
        teoSimRateThread.setEnvironmentPtr(penv);
        teoSimRateThread.setPtrVectorOfRobotPtr(&vectorOfRobotPtr);

        teoSimRateThread.setPtrVectorOfSensorPtrForCameras(&vectorOfSensorPtrForCameras);
        teoSimRateThread.setPtrVectorOfCameraSensorDataPtr(&vectorOfCameraSensorDataPtr);
        teoSimRateThread.setPtrVectorOfRgbPortPtr(&vectorOfRgbPortPtr);
        teoSimRateThread.setPtrVectorOfIntPortPtr(&vectorOfIntPortPtr);
        teoSimRateThread.setPtrVectorOfForce6DPortPtr(&vectorOfForce6DPortPtr);
        teoSimRateThread.setPtrVectorOfCameraWidth(&vectorOfCameraWidth);
        teoSimRateThread.setPtrVectorOfCameraHeight(&vectorOfCameraHeight);
        teoSimRateThread.setPtrVectorOfSensorPtrForLasers(&vectorOfSensorPtrForLasers);
        teoSimRateThread.setPtrVectorOfLaserSensorDataPtr(&vectorOfLaserSensorDataPtr);
        teoSimRateThread.setPtrVectorOfSensorPtrForForce6Ds(&vectorOfSensorPtrForForce6Ds);
        teoSimRateThread.setPtrVectorOfForce6DSensorDataPtr(&vectorOfForce6DSensorDataPtr);

        //penv->StepSimulation(NULL_JMC_MS/100.0);  // StepSimulation must be given in seconds
        penv->StepSimulation(1.0);  // StepSimulation must be given in seconds


        teoSimRateThread.start();

        return true;
    }

private:
    yarp::os::Network yarp;
    std::vector< yarp::dev::PolyDriver* > robotDevices;
    std::vector< OpenRAVE::SensorBasePtr > vectorOfSensorPtrForCameras;
    std::vector< OpenRAVE::SensorBasePtr > vectorOfSensorPtrForLasers;
    std::vector< OpenRAVE::SensorBasePtr > vectorOfSensorPtrForForce6Ds;
    std::vector< boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> > vectorOfCameraSensorDataPtr;
    std::vector< boost::shared_ptr<OpenRAVE::SensorBase::LaserSensorData> >  vectorOfLaserSensorDataPtr;
    std::vector< boost::shared_ptr<OpenRAVE::SensorBase::Force6DSensorData> >  vectorOfForce6DSensorDataPtr;
    std::vector<int> vectorOfCameraWidth;
    std::vector<int> vectorOfCameraHeight;
    std::vector< yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >* > vectorOfRgbPortPtr;
    std::vector< yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelInt> >* > vectorOfIntPortPtr;
    std::vector< yarp::os::BufferedPort<yarp::os::Bottle >* > vectorOfForce6DPortPtr;
    //
    TeoSimRateThread teoSimRateThread;

};

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv) {
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarpcoupled" ) {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpCoupled(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info) {
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpCoulped");
}

OPENRAVE_PLUGIN_API void DestroyPlugin() {
    RAVELOG_INFO("destroying plugin\n");
}
