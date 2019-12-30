// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPENRAVE_YARP_PLUGIN_LOADER_HPP__
#define __OPENRAVE_YARP_PLUGIN_LOADER_HPP__

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <yarp/dev/PolyDriver.h>

#include <openrave/openrave.h>

#include "OpenPortReader.hpp"
#include "OpenPortPeriodicWrite.hpp"

/**
 * @ingroup OpenravePlugins
 * \defgroup OpenraveYarpPluginLoader
 *
 * @brief Contains roboticslab::OpenraveYarpPluginLoader.
 */

/**
 * @ingroup OpenraveYarpPluginLoader
 * @brief Loads one or several YARP Plugin, passing environment pointer.
 */
class OpenraveYarpPluginLoader : public OpenRAVE::ModuleBase
{
public:
    OpenraveYarpPluginLoader(OpenRAVE::EnvironmentBasePtr penv);
    virtual ~OpenraveYarpPluginLoader();
    virtual void Destroy();

    bool addYarpPluginsLists(yarp::os::Bottle& info);
    int main(const std::string& cmd);

    bool Open(std::ostream& sout, std::istream& sinput);
    bool close(const int i);

private:
    yarp::os::Network yarp;
    std::vector<yarp::dev::PolyDriver*> yarpPlugins;
    std::vector<yarp::os::Property> yarpPluginsProperties;

    OpenPortReader openPortReader;
    yarp::os::RpcServer openPortRpcServer;

    OpenPortPeriodicWrite openPortPeriodicWrite;
};

#endif __OPENRAVE_YARP_PLUGIN_LOADER_HPP__
