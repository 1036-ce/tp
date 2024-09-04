find_package(GTest REQUIRED)

if (!GTest_FOUNDED) 
	message(FATAL "GTest not found")
endif()

# add_executable(jtest test_map.cpp test_rbtree.cpp)
add_executable(jtest test.cpp)

target_link_libraries(jtest
	PRIVATE
		GTest::GTest)

target_compile_options(jtest PRIVATE -Wall -g -fprofile-arcs -ftest-coverage)
target_link_options(jtest PRIVATE -Wall -g -fprofile-arcs -ftest-coverage)

# message(-- ${CMAKE_BINARY_DIR}/test/CMakeFiles/jtest.dir)
set(DIR ${CMAKE_BINARY_DIR}/test/CMakeFiles/jtest.dir)
# set(DIR ${CMAKE_SOURCE_DIR}/include)

add_custom_target(report
	COMMAND lcov --gcov-tool gcov-12 --capture --directory ${DIR} --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage-report
	COMMAND xdg-open coverage-report/index.html
    DEPENDS jtest
)

add_test(map jtest)
add_test(rbtree jtest)
