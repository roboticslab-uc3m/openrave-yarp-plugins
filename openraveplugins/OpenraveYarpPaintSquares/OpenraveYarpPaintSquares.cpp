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

#define DEFAULT_RATE_MS 20.0
#define NSQUARES 16


using namespace std;
using namespace OpenRAVE;

class DataProcessor : public yarp::os::PortReader {

public:
    void setPsqPainted(vector<int>* psqPainted) {
        this->psqPainted = psqPainted;
    }
    void setPsqPaintedSemaphore(yarp::os::Semaphore* psqPaintedSemaphore) {
        this->psqPaintedSemaphore = psqPaintedSemaphore;
    }

private:

    vector<int>* psqPainted;
    yarp::os::Semaphore* psqPaintedSemaphore;

    virtual bool read(yarp::os::ConnectionReader& in)
    {
        yarp::os::Bottle request, response;
        if (!request.read(in)) return false;
        printf("Request: %s\n", request.toString().c_str());
        yarp::os::ConnectionWriter *out = in.getWriter();
        if (out==NULL) return true;

        //--
        if ( request.get(0).asString() == "get" )
        {
            psqPaintedSemaphore->wait();
            for(int i=0; i<psqPainted->size();i++)
                response.addInt(psqPainted->operator[](i));
            psqPaintedSemaphore->post();
            return response.write(*out);
        }
        else if ( request.get(0).asString() == "paint" )
        {

            psqPaintedSemaphore->wait();
            for(int i=0; i<psqPainted->size();i++)
                psqPainted->operator[](i) |= request.get(i+1).asInt();  // logic OR
            psqPaintedSemaphore->post();
            response.addString("ok");
            return response.write(*out);
        }
        else if ( request.get(0).asString() == "reset" )
        {
            psqPaintedSemaphore->wait();
            for(int i=0; i<psqPainted->size();i++)
                psqPainted->operator[](i) = 0;
            psqPaintedSemaphore->post();
            response.addString("ok");
            return response.write(*out);
        }

        response.addString("unknown command");
        return response.write(*out);
    }


};

class OpenraveYarpPaintSquares : public ModuleBase, public yarp::os::RateThread
{
public:
    OpenraveYarpPaintSquares(EnvironmentBasePtr penv) : ModuleBase(penv), RateThread(DEFAULT_RATE_MS) {
        __description = "OpenraveYarpPaintSquares plugin.";
        RegisterCommand("open",boost::bind(&OpenraveYarpPaintSquares::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveYarpPaintSquares() {
        rpcServer.close();
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

        string portName("/openraveYarpPaintSquares/rpc:s");

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

        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();

        _objPtr = penv->GetKinBody("object");
        if(!_objPtr) {
            fprintf(stderr,"error: object \"object\" does not exist.\n");
        } else printf("sucess: object \"object\" exists.\n");

        _wall = penv->GetKinBody("wall");
        if(!_wall) {
            fprintf(stderr,"error: object \"wall\" does not exist.\n");
        } else printf("sucess: object \"wall\" exists.\n");

        std::vector<RobotBasePtr> robots;
        penv->GetRobots(robots);
        std::cout << "Robot 0: " << robots.at(0)->GetName() << std::endl;  // default: teo
        RobotBasePtr probot = robots.at(0);
        probot->SetActiveManipulator("rightArm");
        probot->Grab(_objPtr);

        sqPainted.resize(NSQUARES);

        processor.setPsqPainted(&sqPainted);
        processor.setPsqPaintedSemaphore(&sqPaintedSemaphore);
        rpcServer.setReader(processor);
        rpcServer.open(portName);

        this->start();  // start yarp::os::RateThread (calls run periodically)

        return true;
    }

    virtual void run()
    {
        //RAVELOG_INFO("thread\n");

        //Get new object (pen) position
        T_base_object = _objPtr->GetTransform();
        double T_base_object_x = T_base_object.trans.x;
        double T_base_object_y = T_base_object.trans.y;
        double T_base_object_z = T_base_object.trans.z;

        //Update psqpainted to the new values
        for(int i=0; i<(sqPainted.size()); i++)
        {
            stringstream ss;
            ss << "square" << i;
            Transform pos_square = _wall->GetLink(ss.str())->GetGeometry(0)->GetTransform();

            double pos_square_x = pos_square.trans.x;
            double pos_square_y = pos_square.trans.y;
            double pos_square_z = pos_square.trans.z;
            double dist = sqrt(pow(T_base_object_x-pos_square_x,2)
                                      + pow(T_base_object_y-pos_square_y,2)
                                      + pow(T_base_object_z-pos_square_z,2) );

            if (dist < 0.13)
            {
                sqPaintedSemaphore.wait();
                sqPainted[i]=1;
                sqPaintedSemaphore.post();
            }

            sqPaintedSemaphore.wait();
            int sqPaintedValue = sqPainted[i];
            sqPaintedSemaphore.post();

            if( sqPaintedValue == 1 )
            {
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(RaveVector<float>(0.0, 0.0, 1.0));
            }
            else
            {
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(RaveVector<float>(0.5, 0.5, 0.5));
            }

        }

    }

private:
    yarp::os::Network yarp;
    yarp::os::RpcServer rpcServer;
    DataProcessor processor;

    vector<int> sqPainted;
    yarp::os::Semaphore sqPaintedSemaphore;

    Transform T_base_object;
    KinBodyPtr _objPtr;
    KinBodyPtr _wall;
};

InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv) {
    if( type == PT_Module && interfacename == "openraveyarppaintsquares" ) {
        return InterfaceBasePtr(new OpenraveYarpPaintSquares(penv));
    }
    return InterfaceBasePtr();
}

void GetPluginAttributesValidated(PLUGININFO& info) {
    info.interfacenames[PT_Module].push_back("OpenraveYarpPaintSquares");
}

OPENRAVE_PLUGIN_API void DestroyPlugin() {
    RAVELOG_INFO("destroying plugin\n");
}
