// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_CONTROL_BOARD_HPP__
#define __YARP_OPENRAVE_CONTROL_BOARD_HPP__

#include <vector>

#include <yarp/conf/version.h>

#include <yarp/dev/ControlBoardInterfaces.h>

#include <openrave/openrave.h>

#include "YarpOpenraveBase.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveControlBoard
 *
 * @brief Contains roboticslab::YarpOpenraveControlBoard.
 */

/**
 * @ingroup YarpOpenraveControlBoard
 * @brief Implements the YARP_dev IPositionControl, IVelocityControl, IEncodersTimed, etc.
 * interface class member functions.
 */
class YarpOpenraveControlBoard : YarpOpenraveBase,
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
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getAxisName(int axis, std::string & name) override;
    yarp::dev::ReturnValue getJointType(int axis, yarp::dev::JointTypeEnum & type) override;
#else
    bool getAxisName(int axis, std::string & name) override;
    bool getJointType(int axis, yarp::dev::JointTypeEnum & type) override;
#endif

    // ------- IPositionControl declarations. Implementation in IPositionControlImpl.cpp -------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getAxes(std::size_t & ax) override;
    yarp::dev::ReturnValue positionMove(int j, double ref) override;
    yarp::dev::ReturnValue positionMove(const double * refs) override;
    yarp::dev::ReturnValue positionMove(int n_joint, const int * joints, const double * refs) override;
    yarp::dev::ReturnValue relativeMove(int j, double delta) override;
    yarp::dev::ReturnValue relativeMove(const double * deltas) override;
    yarp::dev::ReturnValue relativeMove(int n_joint, const int * joints, const double * deltas) override;
    yarp::dev::ReturnValue checkMotionDone(int j, bool & flag) override;
    yarp::dev::ReturnValue checkMotionDone(bool & flag) override;
    yarp::dev::ReturnValue checkMotionDone(const std::vector<int> & joints, bool & flags) override;
    yarp::dev::ReturnValue setTrajSpeed(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeeds(const double * spds) override;
    yarp::dev::ReturnValue setTrajSpeeds(int n_joint, const int * joints, const double * spds) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double * accs) override;
    yarp::dev::ReturnValue setTrajAccelerations(int n_joint, const int * joints, const double * accs) override;
    yarp::dev::ReturnValue getTrajSpeed(int j, double * ref) override;
    yarp::dev::ReturnValue getTrajSpeeds(double * spds) override;
    yarp::dev::ReturnValue getTrajSpeeds(int n_joint, const int * joints, double * spds) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double * acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(double * accs) override;
    yarp::dev::ReturnValue getTrajAccelerations(int n_joint, const int * joints, double * accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop() override;
    yarp::dev::ReturnValue stop(int n_joint, const int * joints) override;
    yarp::dev::ReturnValue getTargetPosition(int joint, double * ref) override;
    yarp::dev::ReturnValue getTargetPositions(double * refs) override;
    yarp::dev::ReturnValue getTargetPositions(int n_joint, const int * joints, double * refs) override;
#else
    bool getAxes(int * ax) override;
    bool positionMove(int j, double ref) override;
    bool positionMove(const double * refs) override;
    bool positionMove(int n_joint, const int * joints, const double * refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const double * deltas) override;
    bool relativeMove(int n_joint, const int * joints, const double * deltas) override;
    bool checkMotionDone(int j, bool * flag) override;
    bool checkMotionDone(bool * flag) override;
    bool checkMotionDone(int n_joint, const int * joints, bool * flags) override;
    bool setRefSpeed(int j, double sp) override;
    bool setRefSpeeds(const double * spds) override;
    bool setRefSpeeds(int n_joint, const int * joints, const double * spds) override;
    bool setRefAcceleration(int j, double acc) override;
    bool setRefAccelerations(const double * accs) override;
    bool setRefAccelerations(int n_joint, const int * joints, const double * accs) override;
    bool getRefSpeed(int j, double * ref) override;
    bool getRefSpeeds(double * spds) override;
    bool getRefSpeeds(int n_joint, const int * joints, double * spds) override;
    bool getRefAcceleration(int j, double * acc) override;
    bool getRefAccelerations(double * accs) override;
    bool getRefAccelerations(int n_joint, const int * joints, double * accs) override;
    bool stop(int j) override;
    bool stop() override;
    bool stop(int n_joint, const int * joints) override;
    bool getTargetPosition(int joint, double * ref) override;
    bool getTargetPositions(double * refs) override;
    bool getTargetPositions(int n_joint, const int * joints, double * refs) override;
#endif

    // ------- IPositionDirect declarations. Implementation in IPositionDirectImpl.cpp -------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue setPosition(int j, double ref) override;
    yarp::dev::ReturnValue setPositions(const double * refs) override;
    yarp::dev::ReturnValue setPositions(int n_joint, const int * joints, const double * refs) override;
    yarp::dev::ReturnValue getRefPosition(int joint, double * ref) override;
    yarp::dev::ReturnValue getRefPositions(double * refs) override;
    yarp::dev::ReturnValue getRefPositions(int n_joint, const int * joints, double * refs) override;
#else
    bool setPosition(int j, double ref) override;
    bool setPositions(const double * refs) override;
    bool setPositions(int n_joint, const int * joints, const double * refs) override;
    bool getRefPosition(int joint, double * ref) override;
    bool getRefPositions(double * refs) override;
    bool getRefPositions(int n_joint, const int * joints, double * refs) override;
#endif

    //  ---------- IEncoders Declarations. Implementation in IEncodersImpl.cpp ----------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue resetEncoder(int j) override;
    yarp::dev::ReturnValue resetEncoders() override;
    yarp::dev::ReturnValue setEncoder(int j, double val) override;
    yarp::dev::ReturnValue setEncoders(const double * vals) override;
    yarp::dev::ReturnValue getEncoder(int j, double * v) override;
    yarp::dev::ReturnValue getEncoders(double * encs) override;
    yarp::dev::ReturnValue getEncoderSpeed(int j, double * sp) override;
    yarp::dev::ReturnValue getEncoderSpeeds(double * spds) override;
    yarp::dev::ReturnValue getEncoderAcceleration(int j, double * spds) override;
    yarp::dev::ReturnValue getEncoderAccelerations(double *accs) override;
    yarp::dev::ReturnValue getEncodersTimed(double * encs, double * time) override;
    yarp::dev::ReturnValue getEncoderTimed(int j, double * encs, double * time) override;
#else
    bool resetEncoder(int j) override;
    bool resetEncoders() override;
    bool setEncoder(int j, double val) override;
    bool setEncoders(const double * vals) override;
    bool getEncoder(int j, double * v) override;
    bool getEncoders(double * encs) override;
    bool getEncoderSpeed(int j, double * sp) override;
    bool getEncoderSpeeds(double * spds) override;
    bool getEncoderAcceleration(int j, double * spds) override;
    bool getEncoderAccelerations(double *accs) override;
    bool getEncodersTimed(double * encs, double * time) override;
    bool getEncoderTimed(int j, double * encs, double * time) override;
#endif

    //  --------- IVelocityControl Declarations. Implementation in IVelocityControlImpl.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue velocityMove(int j, double sp) override;
    yarp::dev::ReturnValue velocityMove(const double * sp) override;
    yarp::dev::ReturnValue velocityMove(int n_joint, const int * joints, const double * spds) override;
    yarp::dev::ReturnValue getTargetVelocity(const int joint, double * vel) override;
    yarp::dev::ReturnValue getTargetVelocities(double * vels) override;
    yarp::dev::ReturnValue getTargetVelocities(int n_joint, const int * joints, double * vels) override;
#else
    bool velocityMove(int j, double sp) override;
    bool velocityMove(const double * sp) override;
    bool velocityMove(int n_joint, const int * joints, const double * spds) override;
    bool getRefVelocity(const int joint, double * vel) override;
    bool getRefVelocities(double * vels) override;
    bool getRefVelocities(int n_joint, const int * joints, double * vels) override;
#endif

    //  --------- IControlLimits Declarations. Implementation in IControlLimitsImpl.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue setPosLimits(int axis, double min, double max) override;
    yarp::dev::ReturnValue getPosLimits(int axis, double * min, double * max) override;
    yarp::dev::ReturnValue setVelLimits(int axis, double min, double max) override;
    yarp::dev::ReturnValue getVelLimits(int axis, double * min, double * max) override;
#else
    bool setLimits(int axis, double min, double max) override;
    bool getLimits(int axis, double * min, double * max) override;
    bool setVelLimits(int axis, double min, double max) override;
    bool getVelLimits(int axis, double * min, double * max) override;
#endif

    //  --------- IControlMode Declarations. Implementation in IControlModeImpl.cpp ---------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum> & avail) override;
    yarp::dev::ReturnValue getControlMode(int j, yarp::dev::ControlModeEnum & mode) override;
    yarp::dev::ReturnValue getControlModes(std::vector<yarp::dev::ControlModeEnum> & modes) override;
    yarp::dev::ReturnValue getControlModes(const std::vector<int> & joints, std::vector<yarp::dev::ControlModeEnum> & modes) override;
    yarp::dev::ReturnValue setControlMode(int j, yarp::dev::SelectableControlModeEnum mode) override;
    yarp::dev::ReturnValue setControlModes(const std::vector<yarp::dev::SelectableControlModeEnum> & modes) override;
    yarp::dev::ReturnValue setControlModes(const std::vector<int> & joints, const std::vector<yarp::dev::SelectableControlModeEnum> & modes) override;
#else
    bool getControlMode(int j, int * mode) override;
    bool getControlModes(int * modes) override;
    bool getControlModes(int n_joint, const int * joints, int * modes) override;
    bool setControlMode(int j, int mode) override;
    bool setControlModes(int * modes) override;
    bool setControlModes(int n_joint, const int * joints, int * modes) override;
#endif

private:
    // General Joint Motion Controller parameters //
    unsigned int axes;
    std::vector<yarp::conf::vocab32_t> controlModes;
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
    inline double degToRad(double deg)
    { return deg * M_PI / 180.0; }

    /**
     * @brief Converts radians to degrees.
     * @param rad Angle value expressed in radians.
     * @return Same value expressed in degrees.
     */
    inline double radToDeg(double rad)
    { return rad * 180.0 / M_PI; }

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

#endif // __YARP_OPENRAVE_CONTROL_BOARD_HPP__
