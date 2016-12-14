find_package(Boost COMPONENTS filesystem system REQUIRED)

macro(TEST NAME FILE)
	add_executable(${NAME} ${FILE} )
	target_link_libraries(${NAME} ${Boost_LIBRARIES} parser Qt5::Core)
	add_test(NAME ${NAME} COMMAND  $<TARGET_FILE:${NAME}>)
endmacro()

enable_testing()
add_definitions(-g)

TEST(unittests unittests.cpp)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
	DEPENDS
        pre-check
        unittests
)

FILE(GLOB TESTS *.conf*)

add_custom_target(pre-check  COMMAND cp ${TESTS} ${CMAKE_BINARY_DIR}/test)
