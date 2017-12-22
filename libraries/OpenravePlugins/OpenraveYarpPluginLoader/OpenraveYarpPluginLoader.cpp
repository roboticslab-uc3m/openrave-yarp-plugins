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
        //-- Note that we start on element 1, first elem was not via new!!
        for(size_t i=1;i<argv.size();i++)
        {
            //CD_DEBUG("Deleting [%s]\n",argv[i]);
            delete argv[i];
            argv[i] = 0;
        }

        for(int i=0;i<yarpPlugins.size();i++)
        {
            yarpPlugins[i]->close();
            delete yarpPlugins[i];
            yarpPlugins[i] = 0;
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

        //for(size_t i=0;i<argv.size();i++)
        //    CD_DEBUG("argv[%d] is [%s]\n",i,argv[i]);

        yarp::os::Property options;
        options.fromCommand(argv.size(),argv.data());

        CD_DEBUG("config: %s\n", options.toString().c_str());

        //-- Get and put pointer to environment
        CD_INFO("penv: %p\n",GetEnv().get());
        OpenRAVE::EnvironmentBasePtr penv = GetEnv();
        yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
        options.put("penv",v);

        /*
        if( ! options.check("name") )  // Enable bypass if "name" already exists
        {
            //-- If robotIndex (and then if manipulatorIndex), get and put name
            if( options.check("robotIndex") )
            {
                std::string name;

                if( options.check("prefix") )  // Note that not taken into account if using "name"
                {
                    name += options.find("prefix").asString();
                }

                name += "/";
                int robotPtrIdx = options.find("robotIndex").asInt();

                std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
                GetEnv()->GetRobots(vectorOfRobotPtr);

                if(robotPtrIdx >= vectorOfRobotPtr.size())
                {
                    CD_ERROR("robotIndex %d >= vectorOfRobotPtr.size() %d, not loading yarpPlugin.\n",robotPtrIdx,vectorOfRobotPtr.size());
                    return false;
                }
                else if (robotPtrIdx < 0)
                {
                    CD_ERROR("robotIndex %d < 0, not loading yarpPlugin.\n",robotPtrIdx);
                    return false;
                }

                name += vectorOfRobotPtr[ robotPtrIdx ]->GetName();

                if( options.check("manipulatorIndex") )
                {
                    int manipulatorPtrIdx = options.find("manipulatorIndex").asInt();

                    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = vectorOfRobotPtr[ robotPtrIdx ]->GetManipulators();

                    if(manipulatorPtrIdx >= vectorOfManipulatorPtr.size())
                    {
                        CD_ERROR("manipulatorPtrIdx %d >= vectorOfManipulatorPtr.size() %d, not loading yarpPlugin.\n",manipulatorPtrIdx,vectorOfManipulatorPtr.size());
                        return false;
                    }
                    else if (manipulatorPtrIdx < 0)
                    {
                        CD_ERROR("manipulatorPtrIdx %d < 0, not loading yarpPlugin.\n",manipulatorPtrIdx);
                        return false;
                    }

                    name += "/";
                    name += vectorOfManipulatorPtr[ manipulatorPtrIdx ]->GetName();

                    if( options.check("suffix") )  // Note that not taken into account if using "name"
                    {
                        name += options.find("suffix").asString();
                    }
                }

                options.put("name",name);
            }
        }

        CD_DEBUG("post-config: %s\n", options.toString().c_str());
        */

        //-- Fill robotIndeces from: robotIndex/robotIndices/allRobots
        std::vector<int> robotIndices;

        std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
        penv->GetRobots(vectorOfRobotPtr);

        if( options.check("robotIndex") )
        {
            int robotIndex = options.find("robotIndex").asInt();
            robotIndices.push_back(robotIndex);
        }
        else if( options.check("robotIndices") )
        {
            CD_ERROR("Not implemented yet. Bye!\n");
            return false;
        }
        else if( options.check("allRobots") )
        {
            for(int i=0;i<vectorOfRobotPtr.size();i++)
                robotIndices.push_back(i);
        }
        else
        {
            CD_ERROR("Please use --robotIndex or --robotIndices or --allRobots parameter. Bye!\n");
            return false;
        }

  /*
        if(robotIndex >= vectorOfRobotPtr.size())
        {
            CD_ERROR("robotIndex %d >= vectorOfRobotPtr.size() %d, not loading yarpPlugin.\n",robotIndex,vectorOfRobotPtr.size());
            return false;
        }
        else if (robotIndex < 0)
        {
            CD_ERROR("robotIndex %d < 0, not loading yarpPlugin.\n",robotIndex);
            return false;
        }
*/
        /*

        std::vector<int> manipulatorIndexes;

        yarp::dev::PolyDriver* yarpPlugin = new yarp::dev::PolyDriver;
        yarpPlugin->open(options);

        if( ! yarpPlugin->isValid() )
        {
            CD_ERROR("yarpPlugin not valid.\n");
            return false;
        }
        CD_SUCCESS("Valid yarpPlugin.\n");

        yarpPlugins.push_back(yarpPlugin);*/

        return true;
    }

private:
    std::vector<char *> argv;

    yarp::os::Network yarp;
    std::vector<yarp::dev::PolyDriver*> yarpPlugins;
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
