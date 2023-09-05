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

#include <cmath>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <openrave/config.h>
#include <openrave/openrave.h>
#include <openrave/plugin.h>

#include <boost/bind/bind.hpp>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/Property.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Value.h>

namespace
{
    YARP_LOG_COMPONENT(ORYPS, "rl.OpenraveYarpIroning")
}

constexpr auto DEFAULT_RATE_S = 0.1;
constexpr auto DEFAULT_SQUARES = 64;
constexpr auto DEFAULT_PORT_NAME = "/openraveYarpIroning/rpc:s";

class DataProcessor : public yarp::os::PortReader
{
public:
    void setPsqPainted(std::vector<int> *psqPainted)
    {
        this->psqPainted = psqPainted;
    }

    void setPsqPaintedSemaphore(yarp::os::Semaphore *psqPaintedSemaphore)
    {
        this->psqPaintedSemaphore = psqPaintedSemaphore;
    }

private:
    std::vector<int> *psqPainted;
    yarp::os::Semaphore *psqPaintedSemaphore;

    bool read(yarp::os::ConnectionReader &in) override
    {
        yarp::os::Bottle request, response;
        if (!request.read(in))
            return false;
        yCDebug(ORYPS) << "Request:" << request.toString();
        yarp::os::ConnectionWriter *out = in.getWriter();
        if (out == NULL)
            return true;

        //--
        if (request.get(0).asString() == "get")
        {
            psqPaintedSemaphore->wait();
            for (int i = 0; i < psqPainted->size(); i++)
                response.addInt32(psqPainted->operator[](i));
            psqPaintedSemaphore->post();
            return response.write(*out);
        }
        else if (request.get(0).asString() == "paint")
        {

            psqPaintedSemaphore->wait();
            for (int i = 0; i < psqPainted->size(); i++)
                psqPainted->operator[](i) |= request.get(i + 1).asInt32(); // logic OR
            psqPaintedSemaphore->post();
            response.addString("ok");
            return response.write(*out);
        }
        else if (request.get(0).asString() == "reset")
        {
            psqPaintedSemaphore->wait();
            for (int i = 0; i < psqPainted->size(); i++)
                psqPainted->operator[](i) = 0;
            psqPaintedSemaphore->post();
            response.addString("ok");
            return response.write(*out);
        }

        response.addString("unknown command");
        return response.write(*out);
    }
};

class OpenraveYarpIroning : public OpenRAVE::ModuleBase,
                            public yarp::os::PeriodicThread
{
public:
    OpenraveYarpIroning(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv), yarp::os::PeriodicThread(DEFAULT_RATE_S)
    {
        using namespace boost::placeholders;
        __description = "OpenraveYarpIroning plugin.";
        OpenRAVE::InterfaceBase::RegisterCommand("open", boost::bind(&OpenraveYarpIroning::Open, this, _1, _2), "opens OpenraveYarpIroning");

        yCInfo(ORYPS) << "Checking for yarp network... (1)";

        if (!yarp.checkNetwork())
        {
            yCError(ORYPS) << "Found no yarp network (try running \"yarpserver &\")";
        }

        yCInfo(ORYPS) << "Found yarp network";
    }

    ~OpenraveYarpIroning() override
    {
        //-- Note that we start on element 1, first elem was not via new!!
        for (size_t i = 1; i < argv.size(); i++)
        {
            delete[] argv[i];
            argv[i] = 0;
        }

        rpcServer.close();
    }

    void Destroy() override
    {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    /*int main(const string& cmd) {
        RAVELOG_INFO("module initialized cmd; %s\n", cmd.c_str());
        return 0;
    }*/

    bool Open(std::ostream &sout, std::istream &sinput)
    {
        yCInfo(ORYPS) << "Checking for yarp network... (2)";
        if (!yarp.checkNetwork())
        {
            yCError(ORYPS) << "Found no yarp network (try running \"yarpserver &\")";
            return false;
        }

        //-- Given "std::istream& sinput", create equivalent to "int argc, char *argv[]"
        //-- Note that char* != const char* given by std::string::c_str();
        const char *dummyProgramName = "dummyProgramName";
        argv.push_back(dummyProgramName);

        while (sinput)
        {
            std::string str;
            sinput >> str;
            if (str.empty()) //-- Omits empty string that is usually at end via openrave.
                continue;
            char *cstr = new char[str.length() + 1]; // pushed to member argv to be deleted in ~.
            std::strcpy(cstr, str.c_str());
            argv.push_back(cstr);
        }

        yarp::os::Property options;
        options.fromCommand(argv.size(), argv.data());

        yCDebug(ORYPS) << "Config:" << options.toString();

        std::string portName = options.check("name", yarp::os::Value(DEFAULT_PORT_NAME), "port name").asString();
        yCInfo(ORYPS) << "Port name:" << portName;

        int squares = options.check("squares", yarp::os::Value(DEFAULT_SQUARES), "number of squares").asInt32();
        yCInfo(ORYPS) << "Squares:" << squares;

        RAVELOG_INFO("penv: %p\n", GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();

        _objPtr = penv->GetKinBody("object");
        if (!_objPtr)
        {
            std::fprintf(stderr, "error: object \"object\" does not exist.\n");
        }
        else
            std::printf("sucess: object \"object\" exists.\n");

        _wall = penv->GetKinBody("wall");
        if (!_wall)
        {
            std::fprintf(stderr, "error: object \"wall\" does not exist.\n");
        }
        else
            std::printf("sucess: object \"wall\" exists.\n");

        _palete_magenta = penv->GetKinBody("palete-magenta");
        if (!_palete_magenta)
        {
            std::fprintf(stderr, "error: object \"palete-magenta\" does not exist.\n");
        }
        else
            std::printf("sucess: object \"palete-magenta\" exists.\n");

        _palete_yellow = penv->GetKinBody("palete-yellow");
        if (!_palete_yellow)
        {
            std::fprintf(stderr, "error: object \"palete-yellow\" does not exist.\n");
        }
        else
            std::printf("sucess: object \"palete-yellow\" exists.\n");

        _palete_cyan = penv->GetKinBody("palete-cyan");
        if (!_palete_cyan)
        {
            std::fprintf(stderr, "error: object \"palete-cyan\" does not exist.\n");
        }
        else
            std::printf("sucess: object \"palete-cyan\" exists.\n");

        std::vector<OpenRAVE::RobotBasePtr> robots;
        penv->GetRobots(robots);
        std::cout << "Robot 0: " << robots.at(0)->GetName() << std::endl; // default: teo
        OpenRAVE::RobotBasePtr probot = robots.at(0);
        probot->SetActiveManipulator("rightArm");
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 101, 0)
        probot->Grab(_objPtr, rapidjson::Value());
#else
        probot->Grab(_objPtr);
#endif

        sqPainted.resize(squares);

        processor.setPsqPainted(&sqPainted);
        processor.setPsqPaintedSemaphore(&sqPaintedSemaphore);
        rpcServer.setReader(processor);
        rpcServer.open(portName);

        this->start(); // start yarp::os::PeriodicThread (calls run periodically)

        return true;
    }

    void run() override
    {
        // RAVELOG_INFO("thread\n");

        // Get new object (pen) position
        T_base_object = _objPtr->GetTransform();
        double T_base_object_x = T_base_object.trans.x;
        double T_base_object_y = T_base_object.trans.y;
        double T_base_object_z = T_base_object.trans.z;

        // Create new object in the scene "palete" to change brush colours.

        OpenRAVE::Transform pos_palete_magenta = _palete_magenta->GetLink("palete-magenta")->GetGeometry(0)->GetTransform();
        OpenRAVE::Transform pos_palete_yellow = _palete_yellow->GetLink("palete-yellow")->GetGeometry(0)->GetTransform();
        OpenRAVE::Transform pos_palete_cyan = _palete_cyan->GetLink("palete-cyan")->GetGeometry(0)->GetTransform();

        // std::cout<<"Base x obj : "<<T_base_object_x<<std::endl;
        // std::cout<<"Base y obj : "<<T_base_object_y<<std::endl;
        // std::cout<<"Base z obj : "<<T_base_object_z<<std::endl;

        double pos_cyan_x = pos_palete_cyan.trans.x;
        double pos_cyan_y = pos_palete_cyan.trans.y;
        double pos_cyan_z = pos_palete_cyan.trans.z;
        double dist_cyan = std::sqrt(std::pow(T_base_object_x - pos_cyan_x, 2) + std::pow(T_base_object_y - pos_cyan_y, 2) + std::pow(T_base_object_z - pos_cyan_z, 2));

        // std::cout<<"Pos x obj azul: "<<pos_cyan_x<<std::endl;
        // std::cout<<"Pos y obj azul: "<<pos_cyan_y<<std::endl;
        // std::cout<<"Pos z obj azul: "<<pos_cyan_z<<std::endl;

        double pos_yellow_x = pos_palete_yellow.trans.x;
        double pos_yellow_y = pos_palete_yellow.trans.y;
        double pos_yellow_z = pos_palete_yellow.trans.z;
        double dist_yellow = std::sqrt(std::pow(T_base_object_x - pos_yellow_x, 2) + std::pow(T_base_object_y - pos_yellow_y, 2) + std::pow(T_base_object_z - pos_yellow_z, 2));

        // std::cout<<"Pos x obj yellow: "<<pos_yellow_x<<std::endl;
        // std::cout<<"Pos y obj verde: "<<pos_yellow_y<<std::endl;
        // std::cout<<"Pos z obj verde: "<<pos_yellow_z<<std::endl;

        double pos_magenta_x = pos_palete_magenta.trans.x;
        double pos_magenta_y = pos_palete_magenta.trans.y;
        double pos_magenta_z = pos_palete_magenta.trans.z;
        double dist_magenta = std::sqrt(std::pow(T_base_object_x - pos_magenta_x, 2) + std::pow(T_base_object_y - pos_magenta_y, 2) + std::pow(T_base_object_z - pos_magenta_z, 2));

        // std::cout<<"Pos x obj rojo: "<<pos_magenta_x<<std::endl;
        // std::cout<<"Pos y obj rojo: "<<pos_magenta_y<<std::endl;
        // std::cout<<"Pos z obj rojo: "<<pos_magenta_z<<std::endl;

        // std::cout<<"La distancia a azul es: "<<dist_cyan<<std::endl;
        // std::cout<<"La distancia a verde es: "<<dist_yellow<<std::endl;
        // std::cout<<"La distancia a rojo es: "<<dist_magenta<<std::endl;

        // Choose the closer one
        if (dist_cyan < dist_magenta && dist_cyan < dist_yellow && dist_cyan < 0.13)
            brushColour = 1;
        if (dist_yellow < dist_magenta && dist_yellow < dist_cyan && dist_yellow < 0.13)
            brushColour = 2;
        if (dist_magenta < dist_yellow && dist_magenta < dist_cyan && dist_magenta < 0.13)
            brushColour = 3;

        std::cout << "El color con el que estoy pintando es: " << brushColour << std::endl;

        // Update psqpainted to the new values
        for (int i = 0; i < (sqPainted.size()); i++)
        {
            std::stringstream ss;
            ss << "square" << i;
            OpenRAVE::Transform pos_square = _wall->GetLink(ss.str())->GetGeometry(0)->GetTransform();

            double pos_square_x = pos_square.trans.x;
            double pos_square_y = pos_square.trans.y;
            double pos_square_z = pos_square.trans.z;
            double dist = std::sqrt(std::pow(T_base_object_x - pos_square_x, 2) + std::pow(T_base_object_y - pos_square_y, 2) + std::pow(T_base_object_z - pos_square_z, 2));

            if (dist < 0.01 && brushColour == 1) // Paint cyan
            {
                sqPaintedSemaphore.wait();
                sqPainted[i] = 1;
                sqPaintedSemaphore.post();
                std::cout << "He pintado AZUL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
            }
            else if (dist < 0.01 && brushColour == 2) // Paint yellow
            {
                sqPaintedSemaphore.wait();
                sqPainted[i] = 2;
                sqPaintedSemaphore.post();
                std::cout << "He pintado VERDE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
            }

            else if (dist < 0.01 && brushColour == 3) // Paint magenta
            {
                sqPaintedSemaphore.wait();
                sqPainted[i] = 3;
                sqPaintedSemaphore.post();
                std::cout << "He pintado ROJO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
            }

            sqPaintedSemaphore.wait();
            int sqPaintedValue = sqPainted[i];
            sqPaintedSemaphore.post();

            if (sqPaintedValue == 1) // cyan
            {
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(OpenRAVE::RaveVector<float>(0.0, 1.0, 1.0));
            }
            else if (sqPaintedValue == 2) // yellow
            {
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(OpenRAVE::RaveVector<float>(1.0, 1.0, 0.0));
            }
            else if (sqPaintedValue == 3) // magenta
            {
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(OpenRAVE::RaveVector<float>(1.0, 0.0, 0.4));
            }
            else
            {
                _wall->GetLink(ss.str())->GetGeometry(0)->SetDiffuseColor(OpenRAVE::RaveVector<float>(0.5, 0.5, 0.5));
            }
        }
    }

private:
    std::vector<const char *> argv;

    yarp::os::Network yarp;
    yarp::os::RpcServer rpcServer;
    DataProcessor processor;

    std::vector<int> sqPainted;
    yarp::os::Semaphore sqPaintedSemaphore;

    OpenRAVE::Transform T_base_object;
    OpenRAVE::KinBodyPtr _objPtr;
    OpenRAVE::KinBodyPtr _wall;
    OpenRAVE::KinBodyPtr _palete_magenta;
    OpenRAVE::KinBodyPtr _palete_yellow;
    OpenRAVE::KinBodyPtr _palete_cyan;

    // Brush colour
    int brushColour = 1; // Init to cyan colour as default.
};

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
class OpenraveYarpIroningPlugin : public RavePlugin
{
public:
    OpenRAVE::InterfaceBasePtr CreateInterface(OpenRAVE::InterfaceType type,
                                               const std::string &interfacename,
                                               std::istream &sinput,
                                               OpenRAVE::EnvironmentBasePtr penv) override
    {
        if (type == OpenRAVE::PT_Module && interfacename == "openraveyarpironing")
        {
            return OpenRAVE::InterfaceBasePtr(new OpenraveYarpIroning(penv));
        }

        return OpenRAVE::InterfaceBasePtr();
    }

    const InterfaceMap &GetInterfaces() const override
    {
        static const RavePlugin::InterfaceMap interfaces = {
            {OpenRAVE::PT_Module, {"OpenraveYarpIroning"}},
        };

        return interfaces;
    }

    const std::string &GetPluginName() const override
    {
        static const std::string pluginName = "OpenraveYarpIroningPlugin";
        return pluginName;
    }
};

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API RavePlugin *CreatePlugin()
{
    return new OpenraveYarpIroningPlugin();
}
#else  // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type,
                                                    const std::string &interfacename,
                                                    std::istream &sinput,
                                                    OpenRAVE::EnvironmentBasePtr penv)
{
    if (type == OpenRAVE::PT_Module && interfacename == "openraveyarpironing")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpIroning(penv));
    }

    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO &info)
{
    info.interfacenames[OpenRAVE::PT_Module].emplace_back("OpenraveYarpIroning");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
#endif // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
