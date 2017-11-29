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

using namespace std;
using namespace OpenRAVE;


#define NULL_JMC_MS 20

#define OBJECT_1 "object"
#define OBJECT_2 "TableKinBody"

#define ERROR_GAIN 50.0
#define ERROR_OFFSET 0.0

//YARP_DECLARE_PLUGINS(yarpplugins)


class TeoSimRateThread : public yarp::os::RateThread {
     public:

        // Set the Thread Rate in the class constructor
        TeoSimRateThread() : RateThread(NULL_JMC_MS) {}  // In ms

        /**
         * Loop function. This is the thread itself.
         */
        void run();

        // public... buahahaha
        KinBodyPtr pObject1;
        KinBodyPtr pObject2;
        yarp::os::Port port;
        // ------------------------------- Private -------------------------------------
        private:

};

void TeoSimRateThread::run() {
    //printf("[TeoSimRateThread] run()\n");
    Transform t1 = pObject1->GetTransform();
    Transform t2 = pObject2->GetTransform();

    /*t.trans.x
    t.trans.y
    t2.trans.z */

    //Ironing for Tstep=5 -> Ziron=-0.06 -> Zobj=-0.135869 -> ForceSensor= -6.968969
    //Ironing for
    // if Z<-0.1

    yarp::os::Bottle out;
    if(t1.trans.z<-0.1){
        out.addDouble(-15);
    }
    else{
        out.addDouble(0);
    }

    //double estimatedForceZ = fabs((ERROR_GAIN * (t2.trans.z - t1.trans.z) ) - ERROR_OFFSET);
    //yarp::os::Bottle out;
    //out.addDouble(t1.trans.z);
    port.write(out);

}

class OpenraveYarpForceEstimator : public ModuleBase
{
public:
    OpenraveYarpForceEstimator(EnvironmentBasePtr penv) : ModuleBase(penv) {
        //YARP_REGISTER_PLUGINS(yarpplugins);
        __description = "OpenraveYarpForceEstimator plugin.";
        RegisterCommand("open",boost::bind(&OpenraveYarpForceEstimator::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveYarpForceEstimator() {
         teoSimRateThread.port.close();
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

        vector<string> funcionArgs;
        while(sinput)
        {
            string funcionArg;
            sinput >> funcionArg;
            funcionArgs.push_back(funcionArg);
        }

        string portName("/openraveYarpForceEstimator/rpc:s");

	if (funcionArgs.size() > 0)
        {
            if( funcionArgs[0][0] == '/')
                portName = funcionArgs[0];
        }
        RAVELOG_INFO("portName: %s\n",portName.c_str());

        if ( !yarp.checkNetwork() )
        {
            RAVELOG_INFO("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }
        teoSimRateThread.port.open(portName);

        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr pEnv = GetEnv();

        teoSimRateThread.pObject1 = pEnv->GetKinBody(OBJECT_1);
        teoSimRateThread.pObject2 = pEnv->GetKinBody(OBJECT_2);

	//Attach object to rightArm	
 	std::vector<RobotBasePtr> robots;
        pEnv->GetRobots(robots);
        std::cout << "Robot 0: " << robots.at(0)->GetName() << std::endl;  // default: teo
        RobotBasePtr probot = robots.at(0);
        probot->SetActiveManipulator("rightArm");
        probot->Grab(teoSimRateThread.pObject1);
        teoSimRateThread.start();

        return true;
    }

private:
    yarp::os::Network yarp;
    TeoSimRateThread teoSimRateThread;

};

InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv) {
    if( type == PT_Module && interfacename == "openraveyarpforceestimator" ) {
        return InterfaceBasePtr(new OpenraveYarpForceEstimator(penv));
    }
    return InterfaceBasePtr();
}

void GetPluginAttributesValidated(PLUGININFO& info) {
    info.interfacenames[PT_Module].push_back("OpenraveYarpForceEstimator");
}

OPENRAVE_PLUGIN_API void DestroyPlugin() {
    RAVELOG_INFO("destroying plugin\n");
}
