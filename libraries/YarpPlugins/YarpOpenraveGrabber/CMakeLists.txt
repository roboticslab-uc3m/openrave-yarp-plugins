# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

# yarp::dev::CameraConfig
if(YARP_VERSION_SHORT VERSION_LESS 2.3.70 AND (NOT DEFINED ENABLE_YarpOpenraveGrabber OR ENABLE_YarpOpenraveGrabber))
    message(WARNING "Unsuitable YARP version (<2.3.70), disabling YarpOpenraveGrabber device")
endif()

yarp_prepare_plugin(YarpOpenraveGrabber
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveGrabber
                    INCLUDE YarpOpenraveGrabber.hpp
                    DEFAULT ON
                    DEPENDS "ENABLE_YarpOpenraveBase;NOT YARP_VERSION_SHORT VERSION_LESS 2.3.70")

if(NOT SKIP_YarpOpenraveGrabber)

    find_package(OpenRAVE REQUIRED)
    find_package(Boost ${OpenRAVE_Boost_VERSION} EXACT REQUIRED COMPONENTS iostreams python thread system)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                        ${OpenRAVE_INCLUDE_DIRS}
                        ${Boost_INCLUDE_DIRS}
                        $<TARGET_PROPERTY:YarpOpenraveBase,INTERFACE_INCLUDE_DIRECTORIES>)

    link_directories(${OpenRAVE_LIBRARY_DIRS}
                     ${Boost_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveGrabber YarpOpenraveGrabber.hpp
                                        DeviceDriverImpl.cpp
                                        IFrameGrabberImageImpl.cpp
                                        $<TARGET_OBJECTS:YarpOpenraveBase>)

    add_dependencies(YarpOpenraveGrabber COLOR_DEBUG)

    set_target_properties(YarpOpenraveGrabber PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXX_FLAGS}"
                                                         LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveGrabber YARP::YARP_OS
                                              YARP::YARP_dev
                                              ${OpenRAVE_CORE_LIBRARIES}
                                              ${Boost_THREAD_LIBRARY}
                                              ${Boost_SYSTEM_LIBRARY})

    yarp_install(TARGETS YarpOpenraveGrabber
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES YarpOpenraveGrabber.ini
                 COMPONENT runtime
                 DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_YarpOpenraveGrabber OFF CACHE BOOL "Enable/disable YarpOpenraveGrabber device" FORCE)

endif()
