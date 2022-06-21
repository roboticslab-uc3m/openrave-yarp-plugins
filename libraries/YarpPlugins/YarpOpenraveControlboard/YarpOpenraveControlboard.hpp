// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_CONTROLBOARD_HPP__
#define __YARP_OPENRAVE_CONTROLBOARD_HPP__

#include <vector>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <openrave/openrave.h>

#include "YarpOpenraveBase.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveControlboard
 *
 * @brief Contains roboticslab::YarpOpenraveControlboard.
 */

/**
 * @ingroup YarpOpenraveControlboard
 * @brief Implements the YARP_dev IPositionControl, IVelocityControl, IEncodersTimed, etc.
 * interface class member functions.
 */
class YarpOpenraveControlboard : YarpOpenraveBase,
                                 public yarp::dev::DeviceDriver,
                                 public yarp::dev::IAxisInfo,
                                 public yarp::dev::IControlLimits,
                                 public yarp::dev::IControlMode,
                                 public yarp::dev::IEncodersTimed,
                                 public yarp::dev::IPositionControl,
                                 public yarp::dev::IPositionDirect,
                                 public yarp::dev::IVelocityControl
{
public:
    // -------- DeviceDriver declarations. Implementation in IDeviceImpl.cpp --------
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // ------- IAxisInfo declarations. Implementation in IAxisInfoImpl.cpp -------
    bool getAxisName(int axis, std::string& name) override;
    bool getJointType(int axis, yarp::dev::JointTypeEnum& type) override;

    // ------- IPositionControl declarations. Implementation in IPositionControlImpl.cpp -------
    bool getAxes(int *ax) override;
    bool positionMove(int j, double ref) override;
    bool positionMove(const double *refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const double *deltas) override;
    bool checkMotionDone(int j, bool *flag) override;
    bool checkMotionDone(bool *flag) override;
    bool setRefSpeed(int j, double sp) override;
    bool setRefSpeeds(const double *spds) override;
    bool setRefAcceleration(int j, double acc) override;
    bool setRefAccelerations(const double *accs) override;
    bool getRefSpeed(int j, double *ref) override;
    bool getRefSpeeds(double *spds) override;
    bool getRefAcceleration(int j, double *acc) override;
    bool getRefAccelerations(double *accs) override;
    bool stop(int j) override;
    bool stop() override;
    bool positionMove(const int n_joint, const int *joints, const double *refs) override;
    bool relativeMove(const int n_joint, const int *joints, const double *deltas) override;
    bool checkMotionDone(const int n_joint, const int *joints, bool *flags) override;
    bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override;
    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override;
    bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override;
    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override;
    bool stop(const int n_joint, const int *joints) override;
    bool getTargetPosition(const int joint, double *ref) override;
    bool getTargetPositions(double *refs) override;
    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;

    // ------- IPositionDirect declarations. Implementation in IPositionDirectImpl.cpp -------
    bool setPosition(int j, double ref) override;
    bool setPositions(const int n_joint, const int *joints, const double *refs) override;
    bool setPositions(const double *refs) override;
    bool getRefPosition(const int joint, double *ref) override;
    bool getRefPositions(double *refs) override;
    bool getRefPositions(const int n_joint, const int *joints, double *refs) override;

    //  ---------- IEncoders Declarations. Implementation in IEncodersImpl.cpp ----------
    bool resetEncoder(int j) override;
    bool resetEncoders() override;
    bool setEncoder(int j, double val) override;
    bool setEncoders(const double *vals) override;
    bool getEncoder(int j, double *v) override;
    bool getEncoders(double *encs) override;
    bool getEncoderSpeed(int j, double *sp) override;
    bool getEncoderSpeeds(double *spds) override;
    bool getEncoderAcceleration(int j, double *spds) override;
    bool getEncoderAccelerations(double *accs) override;
    bool getEncodersTimed(double *encs, double *time) override;
    bool getEncoderTimed(int j, double *encs, double *time) override;

    //  --------- IVelocityControl Declarations. Implementation in IVelocityControlImpl.cpp ---------
    bool velocityMove(int j, double sp) override;
    bool velocityMove(const double *sp) override;
    bool velocityMove(const int n_joint, const int *joints, const double *spds) override;
    bool getRefVelocity(const int joint, double *vel) override;
    bool getRefVelocities(double *vels) override;
    bool getRefVelocities(const int n_joint, const int *joints, double *vels) override;

    //  --------- IControlLimits Declarations. Implementation in IControlLimitsImpl.cpp ---------
    bool setLimits(int axis, double min, double max) override;
    bool getLimits(int axis, double *min, double *max) override;
    bool setVelLimits(int axis, double min, double max) override;
    bool getVelLimits(int axis, double *min, double *max) override;

    //  --------- IControlMode Declarations. Implementation in IControlModeImpl.cpp ---------
    bool getControlMode(int j, int *mode) override;
    bool getControlModes(int *modes) override;
    bool getControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlMode(const int j, const int mode) override;
    bool setControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlModes(int *modes) override;

private:
    // General Joint Motion Controller parameters //
    unsigned int axes;
    std::vector<int> controlModes;
    std::vector<double> refSpeeds;  // Exposed.

    //OpenRAVE//
    OpenRAVE::MultiControllerBasePtr multi;
    std::vector<OpenRAVE::ControllerBasePtr> pcontrols;
    std::vector<int> manipulatorIDs;
    std::vector<OpenRAVE::dReal> manipulatorTargetRads;
    std::vector<OpenRAVE::RobotBase::JointPtr> vectorOfJointPtr;

    /**
     * @brief Converts degrees to radians.
     * @param deg Angle value expressed in degrees.
     * @return Same value expressed in radians.
     */
    inline double degToRad(double deg) { return deg * M_PI / 180.0; }

    /**
     * @brief Converts radians to degrees.
     * @param rad Angle value expressed in radians.
     * @return Same value expressed in degrees.
     */
    inline double radToDeg(double rad) { return rad * 180.0 / M_PI; }

    /**
     * @brief Converts degrees to radians, unless joint j is prismatic.
     * @param j Axis to check
     * @param deg Angle value expressed in degrees.
     * @return Same value expressed in radians.
     */
    inline double degToRadIfNotPrismatic(int j, double deg)
    {
        if (vectorOfJointPtr[j]->IsPrismatic(0)) return deg;
        else return degToRad(deg); // revolute, circular
    }

    /**
     * @brief Converts radians to degrees, unless joint j is prismatic.
     * @param j Axis to check
     * @param rad Angle value expressed in radians.
     * @return Same value expressed in degrees.
     */
    inline double radToDegIfNotPrismatic(int j, double rad)
    {
        if (vectorOfJointPtr[j]->IsPrismatic(0)) return rad;
        else return radToDeg(rad); // revolute, circular
    }
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_CONTROLBOARD_HPP__
