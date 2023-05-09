message(STATUS "Sex tests")

# List sources

file(GLOB_RECURSE TEST_SOURCES "./*.cpp")

# All tests target

add_executable(sex_all_tests ${TEST_SOURCES})
set_target_properties(sex_all_tests PROPERTIES COMPILE_FLAGS "-g -fsanitize=address -fsanitize=undefined")
set_target_properties(sex_all_tests PROPERTIES LINK_FLAGS "-fsanitize=address -fsanitize=undefined")
target_link_libraries(sex_all_tests sex ut)
