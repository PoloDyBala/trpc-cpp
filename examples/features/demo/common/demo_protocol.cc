#include "examples/features/demo/common/demo_protocol.h"
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
  std::cout << "调用DemoRequestProtocol::ZeroCopyDecode" << std::endl;
  if (buff.ByteSize() < 9) {
    TRPC_LOG_ERROR("Buffer size is less than required header size.");
    return false;
  }

  // 解码packet_size以及packet_id
  const char* ptr = buff.FirstContiguous().data();
  memcpy(&packet_size, ptr, sizeof(packet_size));
  packet_size = ntohl(packet_size);

  memcpy(&packet_id, ptr + 4, sizeof(packet_id));
  packet_id = ntohl(packet_id);

  method_name_length = *(ptr + 8);
  func_ = std::string(ptr + 9, method_name_length);

  // 检查包大小是否一致
  std::cout << "buff size" << buff.ByteSize() << std::endl;
  if (buff.ByteSize() < packet_size) {
    TRPC_LOG_ERROR("Buffer size is less than packet size.");
    return false;
  }

  req_body = buff.Cut(packet_size - (9 + method_name_length));
  return true;
}

bool DemoRequestProtocol::ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) {
  packet_size = 9 + req_body.size() + method_name_length;
  ::trpc::NoncontiguousBufferBuilder builder;
  auto* unaligned_header = builder.Reserve(9);

  uint32_t tmp_packet_size = htons(packet_size);
  memcpy(unaligned_header, &tmp_packet_size, 4);
  unaligned_header += 4;

  uint32_t tmp_packet_id = htons(packet_id);
  memcpy(unaligned_header, &tmp_packet_id, 4);
  unaligned_header += 4;

  uint8_t tmp_method_name_length = method_name_length;
  memcpy(unaligned_header, &tmp_method_name_length, 1);
  unaligned_header += 1;

  std::string tmp_method_name = func_;
  memcpy(unaligned_header, &tmp_method_name, func_.size());
  unaligned_header += func_.size();

  builder.Append(std::move(req_body));

  buff = builder.DestructiveGet();

  return true;
}

// DemoResponseProtocol implementation
bool DemoResponseProtocol::ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) {
  // 假设固定头部大小为 5 字节，用于存储 packet_size 和 error_code
  std::cout << "调用DemoResponseProtocol::ZeroCopyDecode" << std::endl;
  if (buff.ByteSize() < 9) {  // 4 字节 packet_size + 4 字节 packet_id + 1 字节 error_code
    TRPC_LOG_ERROR("Buffer size is less than required header size.");
    return false;
  }

  // 解码固定头部和 packet_id
  const char* ptr = buff.FirstContiguous().data();
  memcpy(&packet_size, ptr, sizeof(packet_size));
  packet_size = ntohl(packet_size);

  memcpy(&packet_id, ptr + 4, sizeof(packet_id));
  packet_id = ntohl(packet_id);

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

bool DemoResponseProtocol::ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) {
  // 编码固定头部和 packet_id
  std::cout << "调用DemoResponseProtocol::ZeroCopyEncode" << std::endl;
  packet_size = 9 + rsp_body.ByteSize();

  ::trpc::NoncontiguousBufferBuilder builder;
  auto* unaligned_header = builder.Reserve(9);

  uint32_t tmp_packet_size = htons(packet_size);
  memcpy(unaligned_header, &tmp_packet_size, 4);
  unaligned_header += 4;

  uint32_t tmp_packet_id = htons(packet_id);
  memcpy(unaligned_header, &tmp_packet_id, 4);
  unaligned_header += 4;

  uint8_t tmp_error_code = htons(error_code);
  memcpy(unaligned_header, &tmp_error_code, 1);

  builder.Append(std::move(rsp_body));

  buff = builder.DestructiveGet();

  return true;
}

}  // namespace examples::demo
