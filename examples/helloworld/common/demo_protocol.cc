#include "examples/helloworld/common/demo_protocol.h"
#include <arpa/inet.h>
#include "trpc/util/buffer/noncontiguous_buffer.h"
//   uint32_t packet_size{0};
//   uint32_t packet_id{0};
//   uint8_t method_name_length{0};
//   std::string method_name;
//   std::string req_data;

namespace examples::demo {
using namespace trpc;
// DemoRequestProtocol implementation
bool DemoRequestProtocol::ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) {
  if (buff.ByteSize() < 9) {
    TRPC_LOG_ERROR("Buffer size is less than required header size.");
    return false;
  }

  // 解码packet_size以及packet_id
  const char* ptr = buff.FirstContiguous().data();
  memcpy(&packet_size, ptr, sizeof(packet_size));
  memcpy(&packet_id, ptr + 4, sizeof(packet_id));
  method_name_length = *(ptr + 8);
  func_ = std::string(ptr + 9, method_name_length);
  // 检查包大小是否一致
  if (buff.ByteSize() < packet_size) {
    TRPC_LOG_ERROR("Buffer size is less than packet size.");
    return false;
  }

  req_body = buff.Cut(packet_size - (9 + method_name_length));
  return true;
}

bool DemoRequestProtocol::ZeroCopyEncode(NoncontiguousBuffer& buff) {
  // 计算方法名称的长度
  method_name_length = static_cast<uint8_t>(func_.size());

  // 计算总的数据包大小
  packet_size =
      sizeof(packet_size) + sizeof(packet_id) + sizeof(method_name_length) + method_name_length + req_body.ByteSize();

  // 创建一个缓冲区构建器
  NoncontiguousBufferBuilder builder;

  // 预留空间并编码固定头部
  char header_buffer[sizeof(packet_size) + sizeof(packet_id) + sizeof(method_name_length)] = {0};
  char* ptr = header_buffer;
  memcpy(ptr, &packet_size, sizeof(packet_size));
  ptr += sizeof(packet_size);
  memcpy(ptr, &packet_id, sizeof(packet_id));
  ptr += sizeof(packet_id);
  memcpy(ptr, &method_name_length, sizeof(method_name_length));

  // 将编码后的固定头部添加到构建器中
  builder.Append(header_buffer, sizeof(header_buffer));

  // 将方法名称添加到构建器中
  builder.Append(std::move(func_));

  // 将请求体数据追加到构建器中
  builder.Append(std::move(req_body));
  // 获取并返回构建好的缓冲区
  buff = builder.DestructiveGet();

  return true;
}

// DemoResponseProtocol implementation
bool DemoResponseProtocol::ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) {
  if (buff.ByteSize() < 9) {  // 4 字节 packet_size + 4 字节 packet_id + 1 字节 error_code
    TRPC_LOG_ERROR("Buffer size is less than required header size.");
    return false;
  }

  // 解码固定头部和 packet_id
  const char* ptr = buff.FirstContiguous().data();
  memcpy(&packet_size, ptr, sizeof(packet_size));
  memcpy(&packet_id, ptr + 4, sizeof(packet_id));

  error_code = *(ptr + 8);

  // 检查包大小是否一致
  if (buff.ByteSize() < packet_size) {
    TRPC_LOG_ERROR("Buffer size is less than packet size.");
    return false;
  }

  // 剩余数据作为响应体
  rsp_body = buff.Cut(packet_size - 9);  // 9 = 4字节包长度 + 4字节 packet_id + 1字节错误码
  return true;
}

// 这里的rsp没内容
bool DemoResponseProtocol::ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) {
  // 编码固定头部和 packet_id
  packet_size = 9 + rsp_body.ByteSize();
  ::trpc::NoncontiguousBufferBuilder builder;
  auto* unaligned_header = builder.Reserve(9);

  memcpy(unaligned_header, &packet_size, 4);
  unaligned_header += 4;

  memcpy(unaligned_header, &packet_id, 4);
  unaligned_header += 4;

  memcpy(unaligned_header, &error_code, 1);

  builder.Append(std::move(rsp_body));

  buff = builder.DestructiveGet();

  return true;
}

}  // namespace examples::demo
