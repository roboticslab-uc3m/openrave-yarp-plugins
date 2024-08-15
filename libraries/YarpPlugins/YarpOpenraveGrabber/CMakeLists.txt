yarp_prepare_plugin(YarpOpenraveGrabber
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveGrabber
                    INCLUDE YarpOpenraveGrabber.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=frameGrabber_nws_yarp)

if(NOT SKIP_YarpOpenraveGrabber)

    yarp_add_plugin(YarpOpenraveGrabber YarpOpenraveGrabber.hpp
                                        DeviceDriverImpl.cpp
                                        IFrameGrabberControlsImpl.cpp
                                        IFrameGrabberImageImpl.cpp
                                        LogComponent.hpp
                                        LogComponent.cpp)

    target_link_libraries(YarpOpenraveGrabber YARP::YARP_os
                                              YARP::YARP_dev
                                              OpenRAVE::libopenrave
                                              Boost::boost
                                              YarpOpenraveBase)

    yarp_install(TARGETS YarpOpenraveGrabber
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
