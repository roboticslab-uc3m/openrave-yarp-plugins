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

#include "ColorDebug.hpp"

using namespace std;
using namespace OpenRAVE;


#define NULL_JMC_MS 20

#define OBJECT_1 "object"
#define OBJECT_2 "TableKinBody"

#define ERROR_GAIN 50.0
#define ERROR_OFFSET 0.0
#define DEFAULT_PORT_NAME "/openraveYarpForceEstimator/rpc:s"
#define DEFAULT_WRINKLESIZE 4

//Square

//YARP_DECLARE_PLUGINS(yarpplugins)


class DataProcessor : public yarp::os::PortReader {

public:
    void setPsqIroned(vector<int>* psqIroned) {
        this->psqIroned = psqIroned;
    }
    void setPsqIronedSemaphore(yarp::os::Semaphore* psqIronedSemaphore) {
        this->psqIronedSemaphore = psqIronedSemaphore;
    }
    void setPforceValue(float* pforceValue) {
        this->pforceValue = pforceValue;
    }
    void setPforceValueSemaphore(yarp::os::Semaphore* pforceValueSemaphore) {
        this->pforceValueSemaphore = pforceValueSemaphore;
    }

private:

    vector<int>* psqIroned;
    yarp::os::Semaphore* psqIronedSemaphore;
    float* pforceValue;
    yarp::os::Semaphore* pforceValueSemaphore;

    virtual bool read(yarp::os::ConnectionReader& in)
    {
        yarp::os::Bottle request, response;
        if (!request.read(in)) return false;
        CD_DEBUG("Request: %s\n", request.toString().c_str());
        yarp::os::ConnectionWriter *out = in.getWriter();
        if (out==NULL) return true;

        //--
        if ( request.get(0).asString() == "get" )
        {
            psqIronedSemaphore->wait();
            for(int i=0; i<psqIroned->size();i++)
                response.addInt(psqIroned->operator[](i));
            psqIronedSemaphore->post();
            return response.write(*out);
        }
        else if ( request.get(0).asString() == "iron" )
        {

            psqIronedSemaphore->wait();
            for(int i=0; i<psqIroned->size();i++)
                psqIroned->operator[](i) |= request.get(i+1).asInt();  // logic OR
            psqIronedSemaphore->post();
            response.addString("ok");
            return response.write(*out);
        }
        else if ( request.get(0).asString() == "reset" )
        {
            psqIronedSemaphore->wait();
            for(int i=0; i<psqIroned->size();i++)
                psqIroned->operator[](i) = 0;
            psqIronedSemaphore->post();
            response.addString("ok");
            return response.write(*out);
        }
        else if ( request.get(0).asString() == "force" )
        {
            pforceValueSemaphore->wait();
            response.addDouble(pforceValue[0]);
            pforceValueSemaphore->post();
            return response.write(*out);
        }

        response.addString("unknown command");
        return response.write(*out);
    }

};

class TeoSimRateThread : public yarp::os::RateThread {
     public:

        // Set the Thread Rate in the class constructor
        TeoSimRateThread() : RateThread(NULL_JMC_MS) {}  // In ms

        /**
         * Loop function. This is the thread itself.
         */
        void run();

        // public... buahahaha
        //KinBodyPtr pObject1;
        //KinBodyPtr pObject2;
        yarp::os::Port port;
        // Not my fault :D
        KinBodyPtr _objPtr;
        KinBodyPtr _ironWrinkle;

        vector<int> sqIroned;
        yarp::os::Semaphore sqIronedSemaphore;
        yarp::os::Semaphore forceValueSemaphore;
        float forceValue;


        // ------------------------------- Private -------------------------------------
        private:

        Transform T_base_object;

};

void TeoSimRateThread::run() {
    //printf("[TeoSimRateThread] run()\n");

    /*
    if(!_objPtr) {
        fprintf(stderr,"error: object \"object\" does not exist.\n");
    } else printf("sucess: object \"object\" exists.\n");

    if(!_ironWrinkle) {
        fprintf(stderr,"error: object \"ironWrinkle\" does not exist.\n");
    } else printf("sucess: object \"ironWrinkle\" exists.\n");
    */

    Transform t1 = _objPtr->GetTransform();
    Transform t2 = _ironWrinkle->GetTransform();

    /*t.trans.x
    t.trans.y
    t2.trans.z */

    //Ironing for Tstep=5 -> Ziron=-0.06 -> Zobj=-0.135869 -> ForceSensor= -6.968969
    //Ironing for
    // if Z<-0.1

    if(t1.trans.z<-0.1){
        forceValueSemaphore.wait();
        forceValue=-15;
        forceValueSemaphore.post();
    }
    else{
        forceValueSemaphore.wait();
        forceValue=0;
        forceValueSemaphore.post();
    }



    //double estimatedForceZ = fabs((ERROR_GAIN * (t2.trans.z - t1.trans.z) ) - ERROR_OFFSET);
    //yarp::os::Bottle out;
    //out.addDouble(t1.trans.z);
    //port.write(out);

    //Calculate the % of wrinkle that have been ironed.

    //Get new object (pen) position
    T_base_object = _objPtr->GetTransform();
    double T_base_object_x = T_base_object.trans.x;
    double T_base_object_y = T_base_object.trans.y;
    double T_base_object_z = T_base_object.trans.z;
    double T_base_object_ox = T_base_object.rot.x;
    double T_base_object_oy = T_base_object.rot.y;
    double T_base_object_oz = T_base_object.rot.z;
    double T_base_object_ow = T_base_object.rot.w;

    //cout<<"Orientacion X es: "<<T_base_object_ox<<endl;
    //cout<<"Orientacion Y es: "<<T_base_object_oy<<endl;
    //cout<<"Orientacion Z es: "<<T_base_object_oz<<endl;
    //cout<<"Valor W es: "<<T_base_object_ow<<endl;


    //Update psqIroned to the new values
    for(int i=0; i<(sqIroned.size()); i++)
    {
        stringstream ss;
        ss << "square" << i;
        Transform pos_square = _ironWrinkle->GetLink(ss.str())->GetGeometry(0)->GetTransform();

        double pos_square_x = pos_square.trans.x;
        double pos_square_y = pos_square.trans.y;
        double pos_square_z = pos_square.trans.z;
        double dist = sqrt(pow(T_base_object_x-pos_square_x,2)
                                  + pow(T_base_object_y-pos_square_y,2)
                                  + pow(T_base_object_z-pos_square_z,2) );

        if (dist < 0.06)
        {
            sqIronedSemaphore.wait();
            sqIroned[i]=1;
            sqIronedSemaphore.post();
        }

        sqIronedSemaphore.wait();
        int sqIronedValue = sqIroned[i];
        sqIronedSemaphore.post();

        if( sqIronedValue == 1 )
        {
            _ironWrinkle->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(RaveVector<float>(0.0, 0.0, 1.0));
        }
        else
        {
            _ironWrinkle->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(RaveVector<float>(0.5, 0.5, 0.5));
        }

    }

}

class OpenraveYarpForceEstimator : public ModuleBase
{
public:
    OpenraveYarpForceEstimator(EnvironmentBasePtr penv) : ModuleBase(penv) {
        //YARP_REGISTER_PLUGINS(yarpplugins);
        __description = "OpenraveYarpForceEstimator plugin.";
        RegisterCommand("open",boost::bind(&OpenraveYarpForceEstimator::Open, this,_1,_2),"opens OpenraveYarpForceEstimator");
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

        CD_INFO("Checking for yarp network...\n");
        if ( ! yarp.checkNetwork() )
        {
            CD_ERROR("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }
        CD_SUCCESS("Found yarp network.\n");

        //-- Given "std::istream& sinput", create equivalent to "int argc, char *argv[]"
        //-- Note that char* != const char* given by std::string::c_str();
        char* dummyProgramName = "dummyProgramName";
        argv.push_back(dummyProgramName);

        //vector<string> argv;

        while(sinput)
        {
            std::string str;
            sinput >> str;
            if(str.length() == 0)  //-- Omits empty string that is usually at end via openrave.
                continue;
            char *cstr = new char[str.length() + 1];  // pushed to member argv to be deleted in ~.
            strcpy(cstr, str.c_str());

            argv.push_back(cstr);
        }

        yarp::os::Property options;
        options.fromCommand(argv.size(),argv.data());

        CD_DEBUG("config: %s\n", options.toString().c_str());

        std::string portName = options.check("name",yarp::os::Value(DEFAULT_PORT_NAME),"port name").asString();
        CD_INFO("port name: %s\n",portName.c_str());

        int wrinkleSize = options.check("wrinkleSize",DEFAULT_WRINKLESIZE,"wrinkle Size").asInt();
        CD_INFO("wrinkle Size: %d\n",wrinkleSize);

        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();

        teoSimRateThread._objPtr = penv->GetKinBody("object");
        if(!teoSimRateThread._objPtr) {
            fprintf(stderr,"error: object \"object\" does not exist.\n");
        } else printf("sucess: object \"object\" exists.\n");

        teoSimRateThread._ironWrinkle = penv->GetKinBody("ironWrinkle");
        if(!teoSimRateThread._ironWrinkle) {
            fprintf(stderr,"error: object \"ironWrinkle\" does not exist.\n");
        } else printf("sucess: object \"ironWrinkle\" exists.\n");

        std::vector<RobotBasePtr> robots;
        penv->GetRobots(robots);
        std::cout << "Robot 0: " << robots.at(0)->GetName() << std::endl;  // default: teo
        RobotBasePtr probot = robots.at(0);
        probot->SetActiveManipulator("rightArm");
        probot->Grab(teoSimRateThread._objPtr);

        teoSimRateThread.sqIroned.resize(wrinkleSize);

        processor.setPsqIroned(&teoSimRateThread.sqIroned);
        processor.setPsqIronedSemaphore(&teoSimRateThread.sqIronedSemaphore);
        processor.setPforceValue(&teoSimRateThread.forceValue);
        processor.setPforceValueSemaphore(&teoSimRateThread.forceValueSemaphore);
        rpcServer.setReader(processor);
        rpcServer.open(portName);

        teoSimRateThread.start();



        /*


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



        //Attach object to rightArm
        std::vector<RobotBasePtr> robots;
        pEnv->GetRobots(robots);
        std::cout << "Robot 0: " << robots.at(0)->GetName() << std::endl;  // default: teo
        RobotBasePtr probot = robots.at(0);
        probot->SetActiveManipulator("rightArm");
        probot->Grab(teoSimRateThread.pObject1);

        sqPainted.resize(squares);

        processor.setPsqPainted(&sqPainted);
        processor.setPsqPaintedSemaphore(&sqPaintedSemaphore);
        rpcServer.setReader(processor);
        rpcServer.open(portName);

        teoSimRateThread.start();
        */

        return true;
    }

private:
    yarp::os::Network yarp;
    TeoSimRateThread teoSimRateThread;

    std::vector<char *> argv;

    DataProcessor processor;
    yarp::os::RpcServer rpcServer;


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
