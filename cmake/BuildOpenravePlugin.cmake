if(COMMAND build_openrave_plugin)
    return()
endif()

macro(build_openrave_plugin name)
    add_library(${name} SHARED ${name}.cpp)

    set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXX_FLAGS}"
                                             LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(${name} ${YARP_LIBRARIES} ${OpenRAVE_LIBRARIES})

    execute_process(COMMAND openrave-config --prefix --plugins-dir
                    OUTPUT_VARIABLE _openrave_config_cmd_output
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(REPLACE "\n" ";" _openrave_config_results ${_openrave_config_cmd_output})
    list(LENGTH _openrave_config_results _len)

    if(_len EQUAL 2)
        list(GET _openrave_config_results 0 _openrave_install_prefix)
        list(GET _openrave_config_results 1 _openrave_plugin_path)
        file(RELATIVE_PATH _relative_path "${_openrave_install_prefix}" "${_openrave_plugin_path}")
        install(TARGETS ${name} DESTINATION ${_relative_path})
    else()
        message(AUTHOR_WARNING "Unsuccessful call to 'openrave-config', the output was: ${_openrave_config_cmd_output}")
    endif()
endmacro()
