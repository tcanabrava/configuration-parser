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
    set(EXTRA_SYSTEM_LIBRARIES stdc++fs)
endif()

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)

include_directories(.)
include_directories(${Boost_INCLUDE_DIRS})

set(PARSER_SRCS
	string-helpers.cpp
	statemachine.cpp
	dump-settings.cpp
	dump_common.cpp
)

set(configurationGeneration_SRCS
	dump-settings.cpp
	main.cpp
)

set(QObjectGeneration_SRCS
	dump-qobject.cpp
	main.cpp
)

add_library(parser ${PARSER_SRCS})
target_link_libraries(parser Boost::filesystem Qt5::Core)

add_executable(confgen ${configurationGeneration_SRCS})
target_link_libraries(confgen parser Qt5::Core ${EXTRA_SYSTEM_LIBRARIES})

add_executable(qtobjgen ${QObjectGeneration_SRCS})
target_link_libraries(qtobjgen parser Qt5::Core ${EXTRA_SYSTEM_LIBRARIES})

enable_testing()
add_subdirectory(test)

