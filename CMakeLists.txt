project(configurationGeneration)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 REQUIRED COMPONENTS Core)

include_directories(.)

set(PARSER_SRCS
	string-helpers.cpp
	statemachine.cpp
	dump-settings.cpp
)

set(SRCS
	main.cpp
)

add_library(parser ${PARSER_SRCS})
target_link_libraries(parser Qt5::Core)

add_executable(confgen ${SRCS})

target_link_libraries(confgen parser Qt5::Core)

enable_testing()
add_subdirectory(test)

