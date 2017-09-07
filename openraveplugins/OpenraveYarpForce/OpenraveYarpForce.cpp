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
#include <openrave/openrave.h>
#include <openrave/plugin.h>
#include <boost/bind.hpp>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

using namespace std;
using namespace OpenRAVE;


#define NULL_JMC_MS 20

//YARP_DECLARE_PLUGINS(yarpplugins)


class TeoSimRateThread : public yarp::os::RateThread {
     public:

        // Set the Thread Rate in the class constructor
        TeoSimRateThread() : RateThread(NULL_JMC_MS) {}  // In ms

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
         * Initialization method. The thread executes this function
         * when it starts and before "run". This is a good place to
         * perform initialization tasks that need to be done by the
         * thread itself (device drivers initialization, memory
         * allocation etc). If the function returns false the thread
         * quits and never calls "run". The return value of threadInit()
         * is notified to the class and passed as a parameter
         * to afterStart(). Note that afterStart() is called by the
         * same thread that is executing the "start" method.
         */
        bool threadInit();

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


class OpenraveYarpForce : public ModuleBase
{
public:
    OpenraveYarpForce(EnvironmentBasePtr penv) : ModuleBase(penv) {
        //YARP_REGISTER_PLUGINS(yarpplugins);
        __description = "OpenraveYarpForce plugin.";
        RegisterCommand("open",boost::bind(&OpenraveYarpForce::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveYarpForce() {
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

    bool Open(ostream& sout, istream& sinput)
    {
        bool alternativeRobotName = false;

        vector<string> funcionArgs;
        while(sinput)
        {
            string funcionArg;
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
            printf("Sensors found on robot %d (%s): %d.\n",robotIter,vectorOfRobotPtr.at(robotIter)->GetName().c_str(),(int)sensors.size());
            for ( unsigned int sensorIter = 0; sensorIter<sensors.size(); sensorIter++ ) {
                OpenRAVE::SensorBasePtr psensorbase = sensors.at(sensorIter)->GetSensor();
                std::string tipo = psensorbase->GetName();
                printf("Sensor %d name: %s\n",sensorIter,tipo.c_str());
                // printf("Sensor %d description: %s\n",sensorIter,psensorbase->GetDescription().c_str());
                if (psensorbase->Supports(OpenRAVE::SensorBase::ST_Camera)) {
                    printf("Sensor %d supports ST_Camera.\n", sensorIter);
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
                    yarp::os::ConstString tmpName("/");
                    yarp::os::ConstString cameraSensorString(psensorbase->GetName());
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
                    printf("Sensor %d supports ST_Laser.\n", sensorIter);
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
                    yarp::os::ConstString tmpName("/");
                    yarp::os::ConstString depthSensorString(psensorbase->GetName());
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
                    printf("Sensor %d supports ST_Force6D.\n", sensorIter);
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
                } else printf("Sensor %d not supported.\n", robotIter);
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

InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv) {
    if( type == PT_Module && interfacename == "openraveyarpforce" ) {
        return InterfaceBasePtr(new OpenraveYarpForce(penv));
    }
    return InterfaceBasePtr();
}

void GetPluginAttributesValidated(PLUGININFO& info) {
    info.interfacenames[PT_Module].push_back("OpenraveYarpForce");
}

OPENRAVE_PLUGIN_API void DestroyPlugin() {
    RAVELOG_INFO("destroying plugin\n");
}
