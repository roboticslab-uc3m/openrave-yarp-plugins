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
                                 public yarp::dev::ITorqueControl,
                                 public yarp::dev::IVelocityControl,
                                 public yarp::dev::IRemoteVariables
{
public:

    YarpOpenraveControlboard() {}

    // -------- DeviceDriver declarations. Implementation in IDeviceImpl.cpp --------

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // ------- IAxisInfo declarations. Implementation in IAxisInfoImpl.cpp -------

    virtual bool getAxisName(int axis, std::string& name);
    virtual bool getJointType(int axis, yarp::dev::JointTypeEnum& type);

    // ------- IPositionControl declarations. Implementation in IPositionControlImpl.cpp -------

    virtual bool getAxes(int *ax);
    virtual bool positionMove(int j, double ref);
    virtual bool positionMove(const double *refs);
    virtual bool relativeMove(int j, double delta);
    virtual bool relativeMove(const double *deltas);
    virtual bool checkMotionDone(int j, bool *flag);
    virtual bool checkMotionDone(bool *flag);
    virtual bool setRefSpeed(int j, double sp);
    virtual bool setRefSpeeds(const double *spds);
    virtual bool setRefAcceleration(int j, double acc);
    virtual bool setRefAccelerations(const double *accs);
    virtual bool getRefSpeed(int j, double *ref);
    virtual bool getRefSpeeds(double *spds);
    virtual bool getRefAcceleration(int j, double *acc);
    virtual bool getRefAccelerations(double *accs);
    virtual bool stop(int j);
    virtual bool stop();
    virtual bool positionMove(const int n_joint, const int *joints, const double *refs);
    virtual bool relativeMove(const int n_joint, const int *joints, const double *deltas);
    virtual bool checkMotionDone(const int n_joint, const int *joints, bool *flags);
    virtual bool setRefSpeeds(const int n_joint, const int *joints, const double *spds);
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs);
    virtual bool getRefSpeeds(const int n_joint, const int *joints, double *spds);
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs);
    virtual bool stop(const int n_joint, const int *joints);
    virtual bool getTargetPosition(const int joint, double *ref);
    virtual bool getTargetPositions(double *refs);
    virtual bool getTargetPositions(const int n_joint, const int *joints, double *refs);

    // ------- IPositionDirect declarations. Implementation in IPositionDirectImpl.cpp -------

    virtual bool setPosition(int j, double ref);
    virtual bool setPositions(const int n_joint, const int *joints, const double *refs);
    virtual bool setPositions(const double *refs);

    //  ---------- IEncoders Declarations. Implementation in IEncodersImpl.cpp ----------

    virtual bool resetEncoder(int j);
    virtual bool resetEncoders();
    virtual bool setEncoder(int j, double val);
    virtual bool setEncoders(const double *vals);
    virtual bool getEncoder(int j, double *v);
    virtual bool getEncoders(double *encs);
    virtual bool getEncoderSpeed(int j, double *sp);
    virtual bool getEncoderSpeeds(double *spds);
    virtual bool getEncoderAcceleration(int j, double *spds);
    virtual bool getEncoderAccelerations(double *accs);

    //  ---------- IEncodersTimed Declarations. Implementation in IEncodersTimedImpl.cpp ----------

    virtual bool getEncodersTimed(double *encs, double *time);
    virtual bool getEncoderTimed(int j, double *encs, double *time);

    //  --------- IVelocityControl Declarations. Implementation in IVelocityControlImpl.cpp ---------

    virtual bool velocityMove(int j, double sp);
    virtual bool velocityMove(const double *sp);
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds);
    virtual bool getRefVelocity(const int joint, double *vel);
    virtual bool getRefVelocities(double *vels);
    virtual bool getRefVelocities(const int n_joint, const int *joints, double *vels);

    //  --------- IControlLimits Declarations. Implementation in IControlLimitsImpl.cpp ---------

    virtual bool setLimits(int axis, double min, double max);
    virtual bool getLimits(int axis, double *min, double *max);
    virtual bool setVelLimits(int axis, double min, double max);
    virtual bool getVelLimits(int axis, double *min, double *max);

    //  --------- IControlMode Declarations. Implementation in IControlModeImpl.cpp ---------

    virtual bool getControlMode(int j, int *mode);
    virtual bool getControlModes(int *modes);
    virtual bool getControlModes(const int n_joint, const int *joints, int *modes);
    virtual bool setControlMode(const int j, const int mode);
    virtual bool setControlModes(const int n_joint, const int *joints, int *modes);
    virtual bool setControlModes(int *modes);

    // -------- ITorqueControl declarations. Implementation in ITorqueControlImpl.cpp --------

    virtual bool getRefTorques(double *t);
    virtual bool getRefTorque(int j, double *t);
    virtual bool setRefTorques(const double *t);
    virtual bool setRefTorque(int j, double t);
    virtual bool setRefTorques(const int n_joint, const int *joints, const double *t);
    virtual bool getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params);
    virtual bool setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params);
    virtual bool getTorque(int j, double *t);
    virtual bool getTorques(double *t);
    virtual bool getTorqueRange(int j, double *min, double *max);
    virtual bool getTorqueRanges(double *min, double *max);

    // ------- IRemoteVariables declarations. Implementation in IRemoteVariablesImpl.cpp -------

    virtual bool getRemoteVariable(std::string key, yarp::os::Bottle& val);
    virtual bool setRemoteVariable(std::string key, const yarp::os::Bottle& val);
    virtual bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys);

    // ------------------------------- Private -------------------------------------

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
        if( (vectorOfJointPtr[j])->IsPrismatic(0) ) return deg;
        else return degToRad(deg);  //  revolute, circular
    }

    /**
     * @brief Converts radians to degrees, unless joint j is prismatic.
     * @param j Axis to check
     * @param rad Angle value expressed in radians.
     * @return Same value expressed in degrees.
     */
    inline double radToDegIfNotPrismatic(int j, double rad)
    {
        if( (vectorOfJointPtr[j])->IsPrismatic(0) ) return rad;
        else return radToDeg(rad);  //  revolute, circular
    }

    static const double DEFAULT_GEN_REF_SPEED;
    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_CONTROLBOARD_HPP__
