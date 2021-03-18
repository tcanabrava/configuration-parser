project(configurationGeneration)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)

include_directories(.)

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
target_link_libraries(parser ${Boost_LIBRARIES} Qt5::Core)

add_executable(confgen ${configurationGeneration_SRCS})
target_link_libraries(confgen parser Qt5::Core)

add_executable(qtobjgen ${QObjectGeneration_SRCS})
target_link_libraries(qtobjgen parser Qt5::Core)

enable_testing()
add_subdirectory(test)

