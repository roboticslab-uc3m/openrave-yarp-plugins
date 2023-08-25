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
                                                   Boost::boost
                                                   YarpOpenraveBase)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(YarpOpenraveControlBoard OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(YarpOpenraveControlBoard PUBLIC ${OpenRAVE_LIBRARIES})

        set_target_properties(YarpOpenraveControlBoard PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                  LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(YarpOpenraveControlBoard PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    yarp_install(TARGETS YarpOpenraveControlBoard
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
