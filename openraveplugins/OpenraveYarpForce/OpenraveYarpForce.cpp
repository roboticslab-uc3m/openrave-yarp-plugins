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

//YARP_DECLARE_PLUGINS(yarpplugins)

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
        bool collision = false;
        bool alternativeRobotName = false;

        vector<string> funcionArgs;
        while(sinput)
        {
            string funcionArg;
            sinput >> funcionArg;
            funcionArgs.push_back(funcionArg);
        }

        if (funcionArgs.size() > 0)
        {
            if (funcionArgs[0] == "collision")
            {
                RAVELOG_INFO("Will open YarpOpenraveControlboardCollision");
                collision = true;
                if (funcionArgs.size() > 1)
                {
                    if( funcionArgs[1][0] == '/')
                    {
                        RAVELOG_INFO("Will use alternativeRobotName: %s",funcionArgs[1].c_str());
                        alternativeRobotName = true;
                    }
                    else
                    {
                        RAVELOG_INFO("Will not use alternativeRobotName that does not begin with '/': %s",funcionArgs[1].c_str());
                    }
                }
            }
        }
        else
        {
            RAVELOG_INFO("Will open YarpOpenraveControlboard");
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
                std::string manipulatorPortName;
                if(alternativeRobotName)
                {
                    manipulatorPortName += funcionArgs[1];
                }
                else
                {
                    manipulatorPortName += "/";
                    manipulatorPortName += vectorOfRobotPtr[robotPtrIdx]->GetName();
                }
                manipulatorPortName += "/";
                manipulatorPortName += vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName();
                RAVELOG_INFO( "* manipulatorPortName: %s\n",manipulatorPortName.c_str() );

                //--
                yarp::dev::PolyDriver* robotDevice = new yarp::dev::PolyDriver;
                yarp::os::Property options;
                options.put("device","controlboardwrapper2");  //-- ports

                if (collision)
                {
                    options.put("subdevice","YarpOpenraveControlboardCollision");
                    std::string safe("/safe");
                    options.put("name", safe+manipulatorPortName );
                    options.put("remote", manipulatorPortName );
                }
                else
                {
                    options.put("subdevice","YarpOpenraveControlboard");
                    options.put("name", manipulatorPortName );
                }

                yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
                options.put("penv",v);

                options.put("robotIndex",static_cast<int>(robotPtrIdx));
                options.put("manipulatorIndex",static_cast<int>(manipulatorPtrIdx));

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
    if( type == PT_Module && interfacename == "openraveyarpcontrolboard" ) {
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
