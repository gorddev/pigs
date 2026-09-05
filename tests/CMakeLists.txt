set(TEST_FILES
  test_mem.cpp
  test_framebuffer.cpp
  test_scripting.cpp
  test_errors.cpp
)

# 2. Loop through each file to create an independent test executable
foreach(test_file ${TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE pigs)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
