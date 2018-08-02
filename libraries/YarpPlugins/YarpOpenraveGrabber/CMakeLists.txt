# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveGrabber
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveGrabber
                    INCLUDE YarpOpenraveGrabber.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase)

if(NOT SKIP_YarpOpenraveGrabber)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                    ${OpenRAVE_INCLUDE_DIRS})

link_directories(${OpenRAVE_LIBRARY_DIRS})

yarp_add_plugin(YarpOpenraveGrabber YarpOpenraveGrabber.hpp
                                    DeviceDriverImpl.cpp
                                    IFrameGrabberImageImpl.cpp)

set_target_properties(YarpOpenraveGrabber PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                     LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(YarpOpenraveGrabber YARP::YARP_OS
                                          YARP::YARP_dev
                                          ${OpenRAVE_LIBRARIES}
                                          Boost::boost
                                          ROBOTICSLAB::ColorDebug
                                          YarpOpenraveBaseInterface)

yarp_install(TARGETS YarpOpenraveGrabber
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES YarpOpenraveGrabber.ini
             COMPONENT runtime
             DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
