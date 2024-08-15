#include "ForceSensor.h"

OpenRAVE::InterfaceBasePtr OpenraveYarpForceSensorPlugin::CreateInterface(OpenRAVE::InterfaceType type,
                                                                          const std::string & interfacename,
                                                                          std::istream & sinput,
                                                                          OpenRAVE::EnvironmentBasePtr penv)
{
    if (type == OpenRAVE::PT_Sensor && interfacename == "forcesensor")
    {
        return OpenRAVE::InterfaceBasePtr(new ForceSensor(penv));
    }

    return OpenRAVE::InterfaceBasePtr();
}

// -----------------------------------------------------------------------------

const RavePlugin::InterfaceMap & OpenraveYarpForceSensorPlugin::GetInterfaces() const
{
    static const RavePlugin::InterfaceMap interfaces = {
        {OpenRAVE::PT_Sensor, {"ForceSensor"}},
    };

    return interfaces;
}

// -----------------------------------------------------------------------------

const std::string & OpenraveYarpForceSensorPlugin::GetPluginName() const
{
    static const std::string pluginName = "OpenraveYarpForceSensorPlugin";
    return pluginName;
}

// -----------------------------------------------------------------------------

OPENRAVE_PLUGIN_API RavePlugin * CreatePlugin() {
    return new OpenraveYarpForceSensorPlugin();
}
