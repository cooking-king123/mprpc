set(SRC_LIST callservice.cc ../user.pb.cc)
add_library(consumer  ${SRC_LIST})
target_link_libraries(consumer mprpc ${PROTOBUF_LIBRARIES})