project(configurationGeneration)
cmake_minimum_required(VERSION 2.8)

include_directories(.)

set(PARSER_SRCS
	string-helpers.cpp
	statemachineV2.cpp
	dump-settings.cpp
)

set(SRCS
	main.cpp
)

add_library(parser ${PARSER_SRCS})
add_executable(confgen ${SRCS})

target_link_libraries(confgen parser)

enable_testing()
add_subdirectory(test)

