macro(TEST NAME FILE)
	add_executable(${NAME} ${FILE} )
	target_link_libraries(${NAME} parser)
	add_test(NAME ${NAME} COMMAND  $<TARGET_FILE:${NAME}>)
endmacro()

enable_testing()
add_definitions(-g)

TEST(TestIncludes test-includes.cpp)
TEST(TestEmptyClass test-empty-class.cpp)
TEST(TestScoppedPreferences test-scopped-preferences.cpp)
TEST(TestClassArray test-class-array.cpp)
TEST(TestProperties test-properties.cpp)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
	DEPENDS
        pre-check
	TestIncludes
	TestEmptyClass
	TestScoppedPreferences
	TestClassArray
	TestProperties
)

file(GLOB TESTS *.conf)

add_custom_target(pre-check  COMMAND cp ${TESTS} ${CMAKE_BINARY_DIR}/test)

