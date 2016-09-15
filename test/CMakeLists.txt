macro(TEST NAME FILE)
	add_executable(${NAME} ${FILE} )
	target_link_libraries(${NAME} parser)
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

file(GLOB TESTS *.conf)
file(GLOB RESULTS *.expected)

add_custom_target(pre-check  COMMAND cp ${TESTS} ${RESULTS} ${CMAKE_BINARY_DIR}/test)

