function(compile_configuration outfiles)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs OPTIONS DEPENDS)

    cmake_parse_arguments(_WRAP_CPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(configuration_files ${_WRAP_CPP_UNPARSED_ARGUMENTS})

    foreach(it ${configuration_files})
        get_filename_component(it ${it} ABSOLUTE)
        get_filename_component(_FILENAME_ ${it} NAME_WLE)
        message("File to process ${_FILENAME_}")

        add_custom_command(
            OUTPUT "${_FILENAME_}.cpp"
            COMMAND confgen ${it}
            WORKING_DIRECTORY ${CMAKE_BUILD_FOLDER}
        )

        list(APPEND outfiles "${_FILENAME_}.cpp")
    endforeach()
    set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()
