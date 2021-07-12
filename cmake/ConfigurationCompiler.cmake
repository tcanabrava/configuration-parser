function(compile_configuration outfiles)
    set(options)
    set(oneValueArgs TARGET EXPORT_HEADER)
    set(multiValueArgs OPTIONS DEPENDS)

    cmake_parse_arguments(_WRAP_CPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(configuration_files ${_WRAP_CPP_UNPARSED_ARGUMENTS})

    if (NOT DEFINED _WRAP_CPP_EXPORT_HEADER)
        set(EXPORT_HEADER_USAGE)
        message("THERES NO EXPORT HEADER")
    else()
        if (_WRAP_CPP_EXPORT_HEADER STREQUAL "")
            set(EXPORT_HEADER_USAGE)
            message("Export header is empty")
        else()
            message("EXPORT HEADER SET AS ${_WRAP_CPP_EXPORT_HEADER}")
            set(EXPORT_HEADER_USAGE "--with-export-header=${_WRAP_CPP_EXPORT_HEADER}")
        endif()
    endif()

    foreach(it ${configuration_files})
        get_filename_component(it ${it} ABSOLUTE)
        get_filename_component(_FILENAME_ ${it} NAME_WE)
        message("File to process ${_FILENAME_}")

        add_custom_command(
            OUTPUT "${_FILENAME_}.cpp" "${_FILENAME_}.h"
            COMMAND confgen ${EXPORT_HEADER_USAGE} ${it}
            WORKING_DIRECTORY ${CMAKE_BUILD_FOLDER}
        )

        list(APPEND outfiles "${_FILENAME_}.cpp")

        qt5_wrap_cpp(_wrapped_moc "${CMAKE_BINARY_DIR}/${_FILENAME_}.h")
        list(APPEND outfiles ${_wrapped_moc})
    endforeach()

    message("GENERATED ${outfile}")
    set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()
