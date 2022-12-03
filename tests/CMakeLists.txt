message(STATUS "Sex tests")

# List sources

file(GLOB_RECURSE TEST_SOURCES "./*.cpp")

# All tests target

add_executable(sex_all_tests ${TEST_SOURCES})
target_link_libraries(sex_all_tests sex ut)
