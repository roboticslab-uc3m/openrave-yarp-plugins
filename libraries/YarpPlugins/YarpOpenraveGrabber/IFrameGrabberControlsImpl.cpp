// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ IFrameGrabberControls Related ----------------------------------------

bool YarpOpenraveGrabber::getCameraDescription(CameraDescriptor *camera) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasFeature(int feature, bool *hasFeature) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setFeature(int feature, double value) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getFeature(int feature, double *value) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setFeature(int feature, double  value1, double  value2) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getFeature(int feature, double *value1, double *value2) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasOnOff(int feature, bool *HasOnOff) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setActive(int feature, bool onoff) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getActive(int feature, bool *isActive) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasAuto(int feature, bool *hasAuto) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasManual(int feature, bool *hasManual) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasOnePush(int feature, bool *hasOnePush) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setMode(int feature, FeatureMode mode) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getMode(int feature, FeatureMode *mode) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setOnePush(int feature) { return false; }

// ----------------------------------------------------------------------------

}
