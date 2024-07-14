#include "examples/features/demo/common/demo_server_codec.h"

#include <memory>
#include <utility>

#include "examples/features/demo/common/demo_protocol.h"

namespace examples::demo {

int DemoServerCodec::ZeroCopyCheck(const ::trpc::ConnectionPtr& conn, ::trpc::NoncontiguousBuffer& in,
                                   std::deque<std::any>& out) {
  while (true) {
    uint32_t total_buff_size = in.ByteSize();
    // Checks buffer contains a full fixed header.
    if (total_buff_size < 9) {  // Updated to 9 bytes to include method_name_length
      TRPC_FMT_TRACE("protocol checker less, buff size:{} is less than 9", total_buff_size);
      break;
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
    memcpy(&packet_size, ptr, sizeof(packet_size));
    // 网络为大端编码, 转换为主机字节序
    packet_size = ntohs(packet_size);



    if (total_buff_size < packet_size) {
      break;
    }

    out.emplace_back(in.Cut(packet_size));
  }

  return !out.empty() ? ::trpc::PacketChecker::PACKET_FULL : ::trpc::PacketChecker::PACKET_LESS;
}

bool DemoServerCodec::ZeroCopyDecode(const ::trpc::ServerContextPtr& ctx, std::any&& in, ::trpc::ProtocolPtr& out) {
  ctx->SetFuncName(::trpc::kNonRpcName);

  auto buff = std::any_cast<::trpc::NoncontiguousBuffer&&>(std::move(in));
  auto* req = static_cast<DemoRequestProtocol*>(out.get());

  return req->ZeroCopyDecode(buff);
}

bool DemoServerCodec::ZeroCopyEncode(const ::trpc::ServerContextPtr& ctx, ::trpc::ProtocolPtr& in,
                                     ::trpc::NoncontiguousBuffer& out) {
  auto* rsp = static_cast<DemoResponseProtocol*>(in.get());

  return rsp->ZeroCopyEncode(out);
}

::trpc::ProtocolPtr DemoServerCodec::CreateRequestObject() {
  return std::make_shared<DemoRequestProtocol>();
}

::trpc::ProtocolPtr DemoServerCodec::CreateResponseObject() {
  return std::make_shared<DemoResponseProtocol>();
}

}  // namespace examples::demo
