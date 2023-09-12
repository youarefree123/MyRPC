set( ProjectName "callee" )
file(GLOB  SRC_LIST "*.cc")
file(GLOB SRC_PB_LIST "../*.cc")


add_executable( ${ProjectName} ${SRC_LIST} ${SRC_PB_LIST} )
target_link_libraries( ${ProjectName} mrpc protobuf)