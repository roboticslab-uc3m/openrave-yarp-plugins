// Copyright:
// Author:
// CopyPolicy:

//////////////////////////////////////////////////////////////////////////
//
// This is a configuration file to explain ROBOT_DEVASTATION_ROBOTS to SWIG
//
// SWIG, for the most part, understands ROBOT_DEVASTATION_ROBOTS auto-magically.
// There are a few things that need to be explained:
//  + use of multiple inheritance
//  + use of names that clash with special names in Java/Python/Perl/...
//  + use of templates

%module roboticslab_openrave_yarp_plugins

//%import "yarp.i"

%define SWIG_PREPROCESSOR_SHOULD_SKIP_THIS %enddef

%{
/* Includes the header in the wrapper code */
#include "ISimulation.hpp"
%}

/* Parse the header file to generate wrappers */
%include "ISimulation.hpp"

%{
#include <yarp/dev/PolyDriver.h>
roboticslab::ISimulation *viewISimulation(yarp::dev::PolyDriver& d)
{
    roboticslab::ISimulation *result;
    d.view(result);
    return result;
}
%}
extern roboticslab::ISimulation *viewISimulation(yarp::dev::PolyDriver& d);
