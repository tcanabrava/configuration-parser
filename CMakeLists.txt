project(configurationGeneration)
cmake_minimum_required(VERSION 2.8)

set(SRCS
	main.cpp
	string-helpers.cpp
	statemachineV2.cpp
	dump-settings.cpp
)

add_executable(confgen ${SRCS})
