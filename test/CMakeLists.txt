# for each "test/x.cpp", generate target "x"
# file(GLOB_RECURSE all_tests *.cc)
# foreach(v ${all_tests})
#     string(REGEX MATCH "test/.*" relative_path ${v})
#     message(${relative_path})
#     string(REGEX REPLACE "test/" "" target_name ${relative_path})
#     string(REGEX REPLACE ".cc" "" target_name ${target_name})
#     add_library( ${target_name} spdlog )
#     add_executable(${target_name} ${v})

# endforeach()


# 宏定义 add_test_exec
macro (add_test_exec exec_name)
    add_executable ("${exec_name}" "${exec_name}.cc")
    target_link_libraries ("${exec_name}" spdlog )
endmacro (add_test_exec)

add_test_exec (test_spdlog)
add_test_exec (test_log)
add_test_exec (test_config)
# add_test_exec (test_channel)
# add_test_exec (test_eventloop_thread)