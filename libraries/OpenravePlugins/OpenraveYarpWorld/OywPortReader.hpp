// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OYW_PORT_READER_HPP__
#define __OYW_PORT_READER_HPP__

#include <yarp/os/Bottle.h>
#include <yarp/os/PortReader.h>

#include <openrave/openrave.h>

namespace roboticslab
{

class OpenraveYarpWorld;

/**
 * @ingroup OpenraveYarpWorld
 * @brief Processes RPC requests for OpenraveYarpWorld.
 */
class OywPortReader : public yarp::os::PortReader
{
public:
    OywPortReader() : boxCount(0), sboxCount (0), cylCount(0), scylCount(0), sphCount(0), ssphCount(0), meshCount(0), fileCount(0) {}
    void setOpenraveYarpWorldPtr(OpenraveYarpWorld *value) { openraveYarpWorldPtr = value; }

private:
    OpenraveYarpWorld* openraveYarpWorldPtr;

    // box/sbox/cyl/scyl/sph/ssph
    std::vector<OpenRAVE::KinBodyPtr> objKinBodyPtrs;
    int boxCount, sboxCount, cylCount, scylCount, sphCount, ssphCount, meshCount, fileCount;

    int robotDraw;
    double drawRadius, drawR, drawG, drawB;

    // Implement the actual responder (callback on RPC).
    bool read(yarp::os::ConnectionReader& in) override;

    // Internally used members
    bool checkIfString(yarp::os::Bottle& request, int index, yarp::os::Bottle& response);
    bool tryToSetActiveManipulator(const std::string& robot, const std::string& manipulator, yarp::os::Bottle& response);
};

} // namespace roboticslab

#endif // __OYW_PORT_READER_HPP__
