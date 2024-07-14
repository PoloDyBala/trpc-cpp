#include "examples/features/demo/common/demo_protocol.h"

#include <arpa/inet.h>

//   uint32_t packet_size{0};
//   uint32_t packet_id{0};
//   uint8_t method_name_length{0};
//   std::string method_name;
//   std::string req_data;

namespace examples::demo {

// DemoRequestProtocol implementation
bool DemoRequestProtocol::ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) {
  const char* ptr{nullptr};
  char header_buffer[9] = {0};

  if (buff.FirstContiguous().size() >= 9) {
    ptr = buff.FirstContiguous().data();
  } else {
    ::trpc::FlattenToSlow(buff, static_cast<void*>(header_buffer), 9);
    ptr = header_buffer;
  }

  memcpy(&packet_size, ptr, sizeof(packet_size));
  packet_size = ntohs(packet_size);

  memcpy(&packet_id, ptr + 4, sizeof(packet_id));
  packet_id = ntohs(packet_id);

  memcpy(&method_name_length, ptr + 8, sizeof(method_name_length));
  method_name_length = ntohs(method_name_length);

  buff.Skip(9);

  // 读取方法名
  method_name.resize(method_name_length);
  ::trpc::FlattenToSlow(buff, const_cast<char*>(method_name.data()), method_name_length);

  buff.Skip(9 + method_name_length);

  req_data = ::trpc::FlattenSlow(buff);

  return true;
}

bool DemoRequestProtocol::ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) {
  // 计算总的 packet_size: 9 字节的固定头部（packet_size + packet_id + method_name_length）加上 method_name 和 req_data 的大小
  packet_size = 9 + static_cast<uint32_t>(method_name.size()) + static_cast<uint32_t>(req_data.size());

  // 创建 NoncontiguousBufferBuilder 对象
  ::trpc::NoncontiguousBufferBuilder builder;
  
  // 预留头部空间
  auto* unaligned_header = builder.Reserve(9 + method_name.size());

  // 编码 packet_size
  uint32_t tmp_packet_size = htons(packet_size);
  memcpy(unaligned_header, &tmp_packet_size, 4);
  unaligned_header += 4;

  // 编码 packet_id
  uint32_t tmp_packet_id = htons(packet_id);
  memcpy(unaligned_header, &tmp_packet_id, 4);
  unaligned_header += 4;

  // 编码 method_name_length
  uint8_t tmp_method_name_length = htons(method_name_length);
  memcpy(unaligned_header, &tmp_method_name_length, 1);
  unaligned_header += 1;

  // 编码 method_name
  memcpy(unaligned_header, method_name.data(), method_name_length);


  // 追加请求数据
  builder.Append(std::move(req_data));

  // 生成最终的缓冲区
  buff = builder.DestructiveGet();

  return true;
}


// DemoResponseProtocol implementation
bool DemoResponseProtocol::ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) {
  const char* ptr{nullptr};
  char header_buffer[9] = {0};

  if (buff.FirstContiguous().size() >= 9) {
    ptr = buff.FirstContiguous().data();
  } else {
    ::trpc::FlattenToSlow(buff, static_cast<void*>(header_buffer), 9);
    ptr = header_buffer;
  }

  memcpy(&packet_size, ptr, sizeof(packet_size));
  packet_size = ntohs(packet_size);

  memcpy(&packet_id, ptr + 4, sizeof(packet_id));
  packet_id = ntohs(packet_id);

  memcpy(&error_code, ptr + 4, sizeof(error_code));
  error_code = ntohs(error_code);

  buff.Skip(9);


  rsp_data = ::trpc::FlattenSlow(buff);

  return true;
}

bool DemoResponseProtocol::ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) {
  packet_size = 9 + rsp_data.size();

  ::trpc::NoncontiguousBufferBuilder builder;
  auto* unaligned_header = builder.Reserve(9);

  uint32_t tmp_packet_size = htons(packet_size);
  memcpy(unaligned_header, &tmp_packet_size, 4);
  unaligned_header += 4;

  uint32_t tmp_packet_id = htons(packet_id);
  memcpy(unaligned_header, &tmp_packet_id, 4);
  unaligned_header += 4;

  uint32_t tmp_error_code = htons(error_code);
  memcpy(unaligned_header, &tmp_error_code, 1);

  builder.Append(std::move(rsp_data));

  buff = builder.DestructiveGet();

  return true;
}

}  // namespace examples::demo
