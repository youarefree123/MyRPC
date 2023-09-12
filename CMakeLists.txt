cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# C++ 生成位置无关动态库
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_compile_options(-fPIC)

# 设置 bin 目录
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
# 设置Lib 目录
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# 设置头文件目录
set( INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" )
set( EXAMPLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/example" )


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("Setting default build type to Release")
endif()

project(my_project_name VERSION 0.0.1 LANGUAGES C CXX)

include_directories( ${INCLUDE_PATH} )
include_directories( ${EXAMPLE_PATH} ) 


add_subdirectory(src)

add_subdirectory(test)

add_subdirectory(example)
