function(compile_configuration outfiles)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs OPTIONS DEPENDS)

    cmake_parse_arguments(_WRAP_CPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(configuration_files ${_WRAP_CPP_UNPARSED_ARGUMENTS})

    foreach(it ${configuration_files})
        get_filename_component(it ${it} ABSOLUTE)
        message("File to process ${it}")
    endforeach()
    # set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()
