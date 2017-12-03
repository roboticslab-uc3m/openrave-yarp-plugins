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

#include "ColorDebug.hpp"

class OpenraveYarpPluginLoader : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpPluginLoader(OpenRAVE::EnvironmentBasePtr penv) : OpenRAVE::ModuleBase(penv)
    {
        __description = "OpenraveYarpPluginLoader plugin.";
        RegisterCommand("open",boost::bind(&OpenraveYarpPluginLoader::Open, this,_1,_2),"opens port");
    }

    virtual ~OpenraveYarpPluginLoader()
    {
        for(int i=0;i<yarpPlugins.size();i++)
        {
            yarpPlugins[i]->close();
            delete yarpPlugins[i];
        }
    }

    virtual void Destroy()
    {
        RAVELOG_INFO("module unloaded from environment\n");
    }

    /*int main(const string& cmd)
    {
        RAVELOG_INFO("module initialized cmd; %s\n", cmd.c_str());
        return 0;
    }*/

    bool Open(std::ostream& sout, std::istream& sinput)
    {
        if ( ! yarp.checkNetwork() )
        {
            CD_ERROR("Found no yarp network (try running \"yarpserver &\"), bye!\n");
            return false;
        }

        yarp::os::Bottle config;

        while(sinput)
        {
            std::string funcionArg;
            sinput >> funcionArg;
            config.addString(funcionArg);
        }

        CD_DEBUG("config: %s\n", config.toString().c_str());

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

                manipulatorPortName += "/";
                manipulatorPortName += vectorOfManipulatorPtr[manipulatorPtrIdx]->GetName();
                RAVELOG_INFO( "* manipulatorPortName: %s\n",manipulatorPortName.c_str() );

                //--
                yarp::dev::PolyDriver* robotDevice = new yarp::dev::PolyDriver;
                yarp::os::Property options;
                options.put("device","controlboardwrapper2");  //-- ports
                options.put("subdevice","YarpOpenraveControlboard");
                options.put("name", manipulatorPortName );

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
                yarpPlugins.push_back( robotDevice );
            }
        }
        return true;
    }

private:
    yarp::os::Network yarp;
    std::vector< yarp::dev::PolyDriver* > yarpPlugins;
};

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    if( type == OpenRAVE::PT_Module && interfacename == "openraveyarppluginloader")
    {
        return OpenRAVE::InterfaceBasePtr(new OpenraveYarpPluginLoader(penv));
    }
    return OpenRAVE::InterfaceBasePtr();
}

void GetPluginAttributesValidated(OpenRAVE::PLUGININFO& info)
{
    info.interfacenames[OpenRAVE::PT_Module].push_back("OpenraveYarpPluginLoader");
}

OPENRAVE_PLUGIN_API void DestroyPlugin()
{
    RAVELOG_INFO("destroying plugin\n");
}
