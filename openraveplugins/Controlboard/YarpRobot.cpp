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

YARP_DECLARE_PLUGINS(TeoYarp)

class YarpRobot : public ModuleBase
{
public:
    YarpRobot(EnvironmentBasePtr penv) : ModuleBase(penv) {
        YARP_REGISTER_PLUGINS(TeoYarp);
        __description = "YarpRobot plugin.";
        RegisterCommand("open",boost::bind(&YarpRobot::Open, this,_1,_2),"opens port");
    }

    virtual ~YarpRobot() {
    }

    void Destroy() {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    int main(const string& cmd) {
        RAVELOG_INFO("module initialized cmd; %s\n", cmd.c_str());
        return 0;
    }

    bool Open(ostream& sout, istream& sinput) {

        sout << "Hello" << endl;

        RAVELOG_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBase* penv_raw = GetEnv().get();

        //-- Get robots
        std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
        GetEnv()->GetRobots(vectorOfRobotPtr);

        //-- For each robot
        for(size_t robotPtrIdx=0;robotPtrIdx<vectorOfRobotPtr.size();robotPtrIdx++)
        {
            RAVELOG_INFO( "Robots[%zu]: %s\n",robotPtrIdx,vectorOfRobotPtr[robotPtrIdx]->GetName().c_str());

            //-- Get manipulators
            std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[robotPtrIdx]->GetManipulators();

            //-- For each manipulator
            for(size_t manipulatorPtrIdx=0;manipulatorPtrIdx<vectorOfManipulatorPtr.size();manipulatorPtrIdx++)
            {
                RAVELOG_INFO( "* Manipulators[%zu]: %s\n",manipulatorPtrIdx,vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName().c_str() );

                //-- Formulate the manipulator port name
                std::string manipulatorPortName("/");
                manipulatorPortName += vectorOfRobotPtr[robotPtrIdx]->GetName();
                manipulatorPortName += "/";
                manipulatorPortName += vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName();
                RAVELOG_INFO( "* manipulatorPortName: %s\n",manipulatorPortName.c_str() );

                yarp::dev::PolyDriver* robotDevice = new yarp::dev::PolyDriver;
                yarp::os::Property options;
                options.put("device","controlboardwrapper2");  //-- ports
                options.put("subdevice","FakeControlboardOR");
                //options.put("device","FakeControlboardOR");
                options.put("name", manipulatorPortName );

                RAVELOG_INFO( " 1 \n" );

                yarp::os::Value v(&penv_raw, sizeof(OpenRAVE::EnvironmentBase*));
                options.put("penv",v);

                RAVELOG_INFO( " 2 \n" );

                options.put("robotIndex",static_cast<int>(robotPtrIdx));
                options.put("manipulatorIndex",static_cast<int>(manipulatorPtrIdx));

                RAVELOG_INFO( " 3 \n" );

                robotDevice->open(options);
                if( ! robotDevice->isValid() )
                {
                    RAVELOG_INFO("Bad\n");
                    return false;
                }
                robotDevices.push_back( robotDevice );
            }
        }
        return true;
    }

private:
    yarp::os::Network yarp;
    std::vector< yarp::dev::PolyDriver* > robotDevices;
};

InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv) {
    if( type == PT_Module && interfacename == "yarprobot" ) {
        return InterfaceBasePtr(new YarpRobot(penv));
    }
    return InterfaceBasePtr();
}

void GetPluginAttributesValidated(PLUGININFO& info) {
    info.interfacenames[PT_Module].push_back("YarpRobot");
}

OPENRAVE_PLUGIN_API void DestroyPlugin() {
    RAVELOG_INFO("destroying plugin\n");
}
