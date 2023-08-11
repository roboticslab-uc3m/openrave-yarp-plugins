include_guard(GLOBAL)

macro(install_openrave_plugin target)
    execute_process(COMMAND "${OpenRAVE_ROOT_DIR}/bin/openrave-config" --prefix --plugins-dir
                    OUTPUT_VARIABLE _openrave_config_cmd_output
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    RESULT_VARIABLE _openrave_config_res)

    if(_openrave_config_res)
        message(FATAL_ERROR "Unable to install OpenRAVE plugins. \"${OpenRAVE_ROOT_DIR}/bin/openrave-config\": ${_openrave_config_res}")
    endif()

    string(REPLACE "\n" ";" _openrave_config_results ${_openrave_config_cmd_output})
    list(LENGTH _openrave_config_results _len)

    if(_len EQUAL 2)
        list(GET _openrave_config_results 0 _openrave_install_prefix)
        list(GET _openrave_config_results 1 _openrave_plugin_path)
        file(RELATIVE_PATH _relative_path "${_openrave_install_prefix}" "${_openrave_plugin_path}")
        install(TARGETS ${target} DESTINATION ${_relative_path})
    else()
        message(AUTHOR_WARNING "Unsuccessful call to 'openrave-config', the output was: ${_openrave_config_cmd_output}")
    endif()
endmacro()
