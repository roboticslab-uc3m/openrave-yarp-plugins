// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPEN_PORT_READER_HPP__
#define __OPEN_PORT_READER_HPP__

#include <yarp/os/PortReader.h>

namespace roboticslab
{

class OpenraveYarpPlanner;

/**
 * @ingroup OpenraveYarpPlanner
 * @brief Processes RPC requests for OpenraveYarpPlanner.
 */
class OypPortReader: public yarp::os::PortReader
{
public:
    void setOpenraveYarpPlannerPtr(OpenraveYarpPlanner *value) { openraveYarpPlannerPtr = value; }
private:
    OpenraveYarpPlanner* openraveYarpPlannerPtr;

    bool read(yarp::os::ConnectionReader& in) override;

    // Internally used members
    bool checkIfString(yarp::os::Bottle& request, int index, yarp::os::Bottle& response);
    bool tryToSetActiveManipulator(const std::string& robot, const std::string& manipulator, yarp::os::Bottle& response);
};

} // namespace roboticslab

#endif // __OPEN_PORT_READER_HPP__
