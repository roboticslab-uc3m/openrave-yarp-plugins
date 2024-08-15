yarp_prepare_plugin(YarpOpenraveControlBoard
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveControlBoard
                    INCLUDE YarpOpenraveControlBoard.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=controlBoard_nws_yarp)

if(NOT SKIP_YarpOpenraveControlBoard)

    yarp_add_plugin(YarpOpenraveControlBoard YarpOpenraveControlBoard.hpp
                                             DeviceDriverImpl.cpp
                                             IAxisInfoImpl.cpp
                                             IControlLimitsImpl.cpp
                                             IControlModeImpl.cpp
                                             IEncodersImpl.cpp
                                             IPositionControlImpl.cpp
                                             IPositionDirectImpl.cpp
                                             IVelocityControlImpl.cpp
                                             LogComponent.hpp
                                             LogComponent.cpp)

    target_link_libraries(YarpOpenraveControlBoard YARP::YARP_os
                                                   YARP::YARP_dev
                                                   OpenRAVE::libopenrave
                                                   Boost::boost
                                                   YarpOpenraveBase)

    yarp_install(TARGETS YarpOpenraveControlBoard
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
