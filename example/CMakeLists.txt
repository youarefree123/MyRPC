# for each "example/x.cpp", generate target "x"

add_subdirectory(callee)
add_subdirectory(caller)
# target_include_directories( callee ./ )