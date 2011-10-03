#include "ForceSensor.h"
#include <rave/plugin.h>

using namespace std;
using namespace OpenRAVE;

// called to create a new plugin
static boost::shared_ptr<void> s_RegisteredReader;
InterfaceBasePtr CreateInterfaceValidated(InterfaceType type, const std::string& interfacename, std::istream& sinput, EnvironmentBasePtr penv)
{
	if( !s_RegisteredReader )
		s_RegisteredReader = RaveRegisterXMLReader(PT_Sensor,"ForceSensor",ForceSensor::CreateXMLReader);

	switch(type) {
		case PT_Sensor:
			if( interfacename == "forcesensor" )
				return InterfaceBasePtr(new ForceSensor(penv));
			break;
		default:
			break;
	}

	return InterfaceBasePtr();
}

// called to query available plugins
void GetPluginAttributesValidated(PLUGININFO& info)
{
info.interfacenames[PT_Sensor].push_back("ForceSensor");

}

// called before plugin is terminated
OPENRAVE_PLUGIN_API void DestroyPlugin()
{
}

