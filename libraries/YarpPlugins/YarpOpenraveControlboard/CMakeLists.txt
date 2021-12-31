# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveControlboard
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveControlboard
                    INCLUDE YarpOpenraveControlboard.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=controlBoard_nws_yarp)

if(NOT SKIP_YarpOpenraveControlboard)

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveControlboard YarpOpenraveControlboard.hpp
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

    set_target_properties(YarpOpenraveControlboard PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                              LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveControlboard YARP::YARP_os
                                                   YARP::YARP_dev
                                                   ${OpenRAVE_LIBRARIES}
                                                   Boost::boost
                                                   YarpOpenraveBase)

    target_include_directories(YarpOpenraveControlboard PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    yarp_install(TARGETS YarpOpenraveControlboard
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
