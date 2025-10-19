# 查找protobuf库
find_package(Protobuf REQUIRED)

set(SRC_LIST userservice.cc ../user.pb.cc)
add_executable(provider ${SRC_LIST})

# 链接protobuf库
target_link_libraries(provider ${PROTOBUF_LIBRARIES})