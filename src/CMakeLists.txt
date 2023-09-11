# 遍历当前目录和子目录
# file(GLOB_RECURSE SRC_DIR_LIST "./*.cpp")

# 只遍历当前目录
file(GLOB  SRC_DIR_LIST "./*.cc")
message("src---${PROJECT_SOURCE_DIR}")


add_library(
        mrpc 
        # STATIC
        SHARED
        ${SRC_DIR_LIST}									
) 

# target_include_directories (
#   base PUBLIC 
#   ${3rd_PATH}/spdlog/include
#   ${INCLUDE_PATH}
# )


# target_link_libraries(
#   base PUBLIC
#   pthread
# ) 


# add_subdirectory (tests) # 指定 编译子目录

