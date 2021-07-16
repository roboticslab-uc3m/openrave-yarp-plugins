# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveGrabber
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveGrabber
                    INCLUDE YarpOpenraveGrabber.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=grabberDual)

if(NOT SKIP_YarpOpenraveGrabber)

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveGrabber YarpOpenraveGrabber.hpp
                                        DeviceDriverImpl.cpp
                                        IFrameGrabberControlsImpl.cpp
                                        IFrameGrabberImageImpl.cpp)

    set_target_properties(YarpOpenraveGrabber PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                         LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveGrabber YARP::YARP_os
                                              YARP::YARP_dev
                                              ${OpenRAVE_LIBRARIES}
                                              Boost::boost
                                              YarpOpenraveBase)

    target_include_directories(YarpOpenraveGrabber PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    yarp_install(TARGETS YarpOpenraveGrabber
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
