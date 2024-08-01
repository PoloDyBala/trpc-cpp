#include "examples/helloworld/common/demo_server_codec.h"

#include <iostream>
#include <memory>
#include <utility>
#include "examples/helloworld/common/demo_protocol.h"

namespace examples::demo {

// ZeroDecode接口的输入参数
int DemoServerCodec::ZeroCopyCheck(const ::trpc::ConnectionPtr& conn, ::trpc::NoncontiguousBuffer& in,
                                   std::deque<std::any>& out) {
  uint32_t total_buff_size = in.ByteSize();
  // Checks buffer contains a full fixed header.
  if (total_buff_size < 9) {  // Updated to 9 bytes to include method_name_length
    TRPC_FMT_TRACE("protocol checker less, buff size:{} is less than 9", total_buff_size);
    // break;
  }

  const char* ptr{nullptr};
  char header_buffer[9] = {0};

  if (in.FirstContiguous().size() >= 9) {
    ptr = in.FirstContiguous().data();
  } else {
    FlattenToSlow(in, static_cast<void*>(header_buffer), 9);
    ptr = header_buffer;
  }

  uint32_t packet_size = 0;
  uint32_t packet_id = 1;
  uint8_t method_name_length = 1;
  std::string func_;
  memcpy(&packet_size, ptr, sizeof(packet_size));
  memcpy(&packet_id, ptr + 4, sizeof(packet_id));
  memcpy(&method_name_length, ptr + 8, sizeof(method_name_length));
  func_ = std::string(ptr + 9, method_name_length);
  out.emplace_back(in.Cut(packet_size));
  // while (true) {

  // }

  return !out.empty() ? ::trpc::PacketChecker::PACKET_FULL : ::trpc::PacketChecker::PACKET_LESS;
}

bool DemoServerCodec::ZeroCopyDecode(const ::trpc::ServerContextPtr& ctx, std::any&& in, ::trpc::ProtocolPtr& out) {
  auto buff = std::any_cast<::trpc::NoncontiguousBuffer&&>(std::move(in));
  auto* req = static_cast<DemoRequestProtocol*>(out.get());
  return req->ZeroCopyDecode(buff);
}

bool DemoServerCodec::ZeroCopyEncode(const ::trpc::ServerContextPtr& ctx, ::trpc::ProtocolPtr& in,
                                     ::trpc::NoncontiguousBuffer& out) {
  auto* rsp = static_cast<DemoResponseProtocol*>(in.get());
  return rsp->ZeroCopyEncode(out);
}

::trpc::ProtocolPtr DemoServerCodec::CreateRequestObject() { return std::make_shared<DemoRequestProtocol>(); }

::trpc::ProtocolPtr DemoServerCodec::CreateResponseObject() { return std::make_shared<DemoResponseProtocol>(); }

}  // namespace examples::demo
