#include <iostream>
#include "test.pb.h"

int main() {
    // 测试基本的消息序列化/反序列化
    test::TestMessage message;
    message.set_name("test");
    message.set_id(123);
    
    std::string serialized;
    message.SerializeToString(&serialized);
    std::cout << "Serialized size: " << serialized.size() << std::endl;
    
    test::TestMessage deserialized;
    deserialized.ParseFromString(serialized);
    std::cout << "Deserialized: " << deserialized.name() << ", " << deserialized.id() << std::endl;
    
    // 检查protobuf版本
    std::cout << "Protobuf library version: " << GOOGLE_PROTOBUF_VERSION << std::endl;
    
    return 0;
}