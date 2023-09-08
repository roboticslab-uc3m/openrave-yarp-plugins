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
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Property.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Value.h>

namespace
{
    YARP_LOG_COMPONENT(ORYPS, "rl.OpenraveYarpIroning")
}

constexpr auto DEFAULT_RATE_S = 0.1;
constexpr auto DEFAULT_SQUARES_X = 3;
constexpr auto DEFAULT_SQUARES_Y = 3;

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
    }

    void Destroy() override
    {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    int main(const std::string& cmd)
    {
        RAVELOG_INFO("module initialized with \"%s\"\n", cmd.c_str());
        // hard-coding "open"
        std::istringstream sinput("open");
        std::ostringstream sout;
        if( ! OpenRAVE::InterfaceBase::SendCommand(sout,sinput) )
            return 1;
        return 0;
    }

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

        unsigned int squaresX = options.check("squaresX", yarp::os::Value(DEFAULT_SQUARES_X), "number of squares on X").asInt32();
        yCInfo(ORYPS) << "Squares X:" << squaresX;
        unsigned int squaresY = options.check("squaresY", yarp::os::Value(DEFAULT_SQUARES_Y), "number of squares on Y").asInt32();
        yCInfo(ORYPS) << "Squares Y:" << squaresY;

        _penv = GetEnv();
        RAVELOG_INFO("penv: %p\n", _penv.get());

        /*_objPtr = _penv->GetKinBody("object");
        if (!_objPtr)
        {
            yCError(ORYPS) << "error: object \"object\" does not exist.";
            return false;
        }
        std::printf("sucess: object \"object\" exists.\n");*/

        std::vector<OpenRAVE::RobotBasePtr> robots;
        _penv->GetRobots(robots);
        if(robots.size()==0)
        {
            yCError(ORYPS) << "No robot yet"; // default: teo
            return false;
        }
        OpenRAVE::RobotBasePtr probot = robots[0];
        yCInfo(ORYPS) << "Robot 0: " << probot->GetName(); // default: teo
        probot->SetActiveManipulator("rightArm");
        _pRobotManip = probot->GetManipulator("rightArm");

        OpenRAVE::Transform ee = _pRobotManip->GetEndEffector()->GetTransform();
        OpenRAVE::Transform tool;
        //tool.trans = Vector(0.0,0.0,1.3);
        tool.rot = OpenRAVE::geometry::quatFromAxisAngle(OpenRAVE::Vector(0,0,0)); //-- Converts an axis-angle rotation into a quaternion.
        tool.rot = ee.rot;
        OpenRAVE::Transform tcp = ee * tool;
        //Transform tcp = ee;
        yCInfo(ORYPS) << "TCP at" << tcp.trans.x << tcp.trans.y << tcp.trans.z;

        {
            // lock the environment!
            #if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 68, 0)
                OpenRAVE::EnvironmentLock lock(_penv->GetMutex());
            #else
                OpenRAVE::EnvironmentMutex::scoped_lock lock(_penv->GetMutex());
            #endif

            double tableX = 0.4;
            double tableY = 0.4;
            for(unsigned int sboxIdxX = 0; sboxIdxX<squaresX; sboxIdxX++)
            {
                for(unsigned int sboxIdxY = 0; sboxIdxY<squaresY; sboxIdxY++)
                {
                    OpenRAVE::KinBodyPtr objKinBodyPtr = OpenRAVE::RaveCreateKinBody(_penv, "");
                    std::vector<OpenRAVE::AABB> boxes(1);
                    boxes[0].extents = OpenRAVE::Vector(tableX/(2.0*squaresX), tableY/(2.0*squaresY), 0.01);
                    boxes[0].pos = OpenRAVE::Vector(0.6+(0.5+sboxIdxX)*tableX/squaresX, -(tableY/2.0)+(0.5+sboxIdxY)*tableY/squaresY, 0.01);
                    objKinBodyPtr->InitFromBoxes(boxes,true);
                    std::string objName("sbox_");
                    std::ostringstream s;
                    s << sboxIdxX;
                    s << "_";
                    s << sboxIdxY;
                    objName.append(s.str());
                    objKinBodyPtr->SetName(objName);
                    #if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 67, 0)
                        _penv->Add(objKinBodyPtr, OpenRAVE::IAM_AllowRenaming);
                    #else
                        _penv->Add(objKinBodyPtr, true);
                    #endif
                    _objKinBodyPtrs.push_back(objKinBodyPtr);
                }
            }

        }

/*
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 101, 0)
        probot->Grab(_objPtr, rapidjson::Value());
#else
        probot->Grab(_objPtr);
#endif

        sqPainted.resize(squares);
*/
        this->start(); // start yarp::os::PeriodicThread (calls run periodically)

        return true;
    }

    void run() override
    {
        yCInfo(ORYPS) << "thread";

        // Get new object (pen) position
        /*T_base_object = _objPtr->GetTransform();
        double T_base_object_x = T_base_object.trans.x;
        double T_base_object_y = T_base_object.trans.y;
        double T_base_object_z = T_base_object.trans.z;*/

        // Create new object in the scene "palete" to change brush colours.

        /*OpenRAVE::Transform pos_palete_magenta = _palete_magenta->GetLink("palete-magenta")->GetGeometry(0)->GetTransform();
        OpenRAVE::Transform pos_palete_yellow = _palete_yellow->GetLink("palete-yellow")->GetGeometry(0)->GetTransform();
        OpenRAVE::Transform pos_palete_cyan = _palete_cyan->GetLink("palete-cyan")->GetGeometry(0)->GetTransform();*/

        // std::cout<<"Base x obj : "<<T_base_object_x<<std::endl;
        // std::cout<<"Base y obj : "<<T_base_object_y<<std::endl;
        // std::cout<<"Base z obj : "<<T_base_object_z<<std::endl;

        /*double pos_cyan_x = pos_palete_cyan.trans.x;
        double pos_cyan_y = pos_palete_cyan.trans.y;
        double pos_cyan_z = pos_palete_cyan.trans.z;
        double dist_cyan = std::sqrt(std::pow(T_base_object_x - pos_cyan_x, 2) + std::pow(T_base_object_y - pos_cyan_y, 2) + std::pow(T_base_object_z - pos_cyan_z, 2));*/

        // std::cout<<"Pos x obj azul: "<<pos_cyan_x<<std::endl;
        // std::cout<<"Pos y obj azul: "<<pos_cyan_y<<std::endl;
        // std::cout<<"Pos z obj azul: "<<pos_cyan_z<<std::endl;

        /*double pos_yellow_x = pos_palete_yellow.trans.x;
        double pos_yellow_y = pos_palete_yellow.trans.y;
        double pos_yellow_z = pos_palete_yellow.trans.z;
        double dist_yellow = std::sqrt(std::pow(T_base_object_x - pos_yellow_x, 2) + std::pow(T_base_object_y - pos_yellow_y, 2) + std::pow(T_base_object_z - pos_yellow_z, 2));*/

        // std::cout<<"Pos x obj yellow: "<<pos_yellow_x<<std::endl;
        // std::cout<<"Pos y obj verde: "<<pos_yellow_y<<std::endl;
        // std::cout<<"Pos z obj verde: "<<pos_yellow_z<<std::endl;

        /*double pos_magenta_x = pos_palete_magenta.trans.x;
        double pos_magenta_y = pos_palete_magenta.trans.y;
        double pos_magenta_z = pos_palete_magenta.trans.z;
        double dist_magenta = std::sqrt(std::pow(T_base_object_x - pos_magenta_x, 2) + std::pow(T_base_object_y - pos_magenta_y, 2) + std::pow(T_base_object_z - pos_magenta_z, 2));*/

        // std::cout<<"Pos x obj rojo: "<<pos_magenta_x<<std::endl;
        // std::cout<<"Pos y obj rojo: "<<pos_magenta_y<<std::endl;
        // std::cout<<"Pos z obj rojo: "<<pos_magenta_z<<std::endl;

        // std::cout<<"La distancia a azul es: "<<dist_cyan<<std::endl;
        // std::cout<<"La distancia a verde es: "<<dist_yellow<<std::endl;
        // std::cout<<"La distancia a rojo es: "<<dist_magenta<<std::endl;

        // Choose the closer one
        /*if (dist_cyan < dist_magenta && dist_cyan < dist_yellow && dist_cyan < 0.13)
            brushColour = 1;
        if (dist_yellow < dist_magenta && dist_yellow < dist_cyan && dist_yellow < 0.13)
            brushColour = 2;
        if (dist_magenta < dist_yellow && dist_magenta < dist_cyan && dist_magenta < 0.13)
            brushColour = 3;

        std::cout << "El color con el que estoy pintando es: " << brushColour << std::endl;*/

        // Update psqpainted to the new values
        /*for (int i = 0; i < (sqPainted.size()); i++)
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
        }*/
    }

private:
    std::vector<const char *> argv;

    yarp::os::Network yarp;

    OpenRAVE::EnvironmentBasePtr _penv;
    std::vector<OpenRAVE::KinBodyPtr> _objKinBodyPtrs;
    OpenRAVE::RobotBase::ManipulatorPtr _pRobotManip;

    OpenRAVE::Transform T_base_object;
    OpenRAVE::KinBodyPtr _objPtr;
    OpenRAVE::KinBodyPtr _wall;
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
