// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_BASE_HPP__
#define __YARP_OPENRAVE_BASE_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <openrave-core.h>

#include <boost/make_shared.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>

namespace roboticslab
{


// Specific for --env parameter
void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername);

/**
 * @ingroup TeoYarp
 * \defgroup YarpOpenraveBase
 *
 * @brief Contains teo::YarpOpenraveBase.
 *
 * @section YarpOpenraveBase_install Installation
 *
 * The plugin is compiled when ENABLE_TeoYarp_YarpOpenraveBase is activated (not default). For further
 * installation steps refer to <a class="el" href="pages.html">your own system installation guidelines</a>.
 */

/**
 * @ingroup YarpOpenraveBase
 * @brief Implements configure.
 * interface class member functions.
 */
class YarpOpenraveBase
{
public:
    YarpOpenraveBase();

    bool configureEnvironment(yarp::os::Searchable& config);
    bool configureOpenravePlugins(yarp::os::Searchable& config);
    bool configureRobot(yarp::os::Searchable& config);
    bool clean();

protected:
    //OpenRAVE//
    OpenRAVE::EnvironmentBasePtr penv;
    OpenRAVE::RobotBasePtr probot;
    std::string robotName;

    // Specific for --env parameter with --view
    boost::thread_group openraveThreads;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_BASE_HPP__

