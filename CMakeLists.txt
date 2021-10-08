#=============================================================================
# SPDX-FileCopyrightText: 2021 Tomaz Canabrava <tcanabrava@kde.org>
# SPDX-FileCopyrightText: Alexander Lohnau <alexander.lohnau@gmx.de>
#
# SPDX-License-Identifier: MIT
#=============================================================================

project(configurationGeneration)
cmake_minimum_required(VERSION 3.10)

if(EXISTS "${CMAKE_BINARY_DIR}/conan_paths.cmake")
    message("Found conan paths")
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
else()
    message("Couldn't find conan paths")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Older versions of gcc don't link automatically to stdc++fs.
    if(NOT APPLE)
        set(EXTRA_SYSTEM_LIBRARIES stdc++fs)
    endif()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/ConfigurationParser")

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)

include_directories(.)
include_directories(${Boost_INCLUDE_DIRS})

set(PARSER_SRCS
	string-helpers.cpp
	statemachine.cpp
	dump_common.cpp
)

set(configurationGeneration_SRCS
	dump-qsettings.cpp
	main.cpp
)

set(QObjectGeneration_SRCS
	dump-qobject.cpp
	main.cpp
)

set(DUMP_KCONFIG_SRCS
	dump-kconfig.cpp
	main.cpp
)

add_library(parser ${PARSER_SRCS})
target_link_libraries(parser Boost::filesystem Qt5::Core)

add_executable(confgen ${configurationGeneration_SRCS})
add_executable(ConfigurationParser::confgen ALIAS confgen)
target_link_libraries(confgen parser Qt5::Core ${EXTRA_SYSTEM_LIBRARIES})
install(TARGETS confgen EXPORT ConfigurationParserTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/ConfigurationParser)

add_executable(kcfggen ${DUMP_KCONFIG_SRCS})
target_link_libraries(kcfggen parser Qt5::Core ${EXTRA_SYSTEM_LIBRARIES})
install(TARGETS kcfggen EXPORT ConfigurationParserTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/ConfigurationParser)

add_executable(qtobjgen ${QObjectGeneration_SRCS})
target_link_libraries(qtobjgen parser Qt5::Core ${EXTRA_SYSTEM_LIBRARIES})
install(TARGETS qtobjgen EXPORT ConfigurationParserTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/ConfigurationParser)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationParserConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationParserConfigVersion.cmake"
    VERSION "0.0.1"
    COMPATIBILITY ExactVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationParserConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ConfigurationParserConfigVersion.cmake"
        "cmake/ConfigurationCompiler.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}")
install(EXPORT ConfigurationParserTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE ConfigurationParserTargets.cmake
        NAMESPACE ConfigurationParser:: )

enable_testing()
add_subdirectory(test)

