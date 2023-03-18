#include "ForceSensor.h"

#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
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
#else // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
// called to create a new plugin
static boost::shared_ptr<void> s_RegisteredReader;

OpenRAVE::InterfaceBasePtr CreateInterfaceValidated(OpenRAVE::InterfaceType type,
													const std::string & interfacename,
													std::istream & sinput,
													EnvironmentBasePtr penv)
{
	if (!s_RegisteredReader)
	{
		s_RegisteredReader = RaveRegisterXMLReader(OpenRAVE::PT_Sensor, "ForceSensor", ForceSensor::CreateXMLReader);
	}

	switch (type)
	{
	case OpenRAVE::PT_Sensor:
		if (interfacename == "forcesensor")
			return OpenRAVE::InterfaceBasePtr(new ForceSensor(penv));
		break;
	default:
		break;
	}

	return OpenRAVE::InterfaceBasePtr();
}

// called to query available plugins
void GetPluginAttributesValidated(OpenRAVE::PLUGININFO & info)
{
	info.interfacenames[OpenRAVE::PT_Sensor].push_back("ForceSensor");
}

// called before plugin is terminated
OPENRAVE_PLUGIN_API void DestroyPlugin()
{}
#endif // OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 105, 0)
