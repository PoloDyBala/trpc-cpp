#include "examples/features/demo/common/demo_client_codec.h"

#include <memory>
#include <utility>

#include "examples/features/demo/common/demo_protocol.h"

namespace examples::demo {

int DemoClientCodec::ZeroCopyCheck(const ::trpc::ConnectionPtr& conn, ::trpc::NoncontiguousBuffer& in,
                                   std::deque<std::any>& out) {
  while (true) {
    uint32_t total_buff_size = in.ByteSize();
    // Checks buffer contains a full fixed header.
    if (total_buff_size < 9) { // Updated to 9 bytes to include method_name_length
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
    packet_size = ntohs(packet_size);

    if (total_buff_size < packet_size) {
      break;
    }

    out.emplace_back(in.Cut(packet_size));
  }

  return !out.empty() ? ::trpc::PacketChecker::PACKET_FULL : ::trpc::PacketChecker::PACKET_LESS;
}

bool DemoClientCodec::ZeroCopyDecode(const ::trpc::ClientContextPtr& ctx, std::any&& in, ::trpc::ProtocolPtr& out) {
  auto buff = std::any_cast<::trpc::NoncontiguousBuffer&&>(std::move(in));
  auto* rsp = static_cast<DemoResponseProtocol*>(out.get());

  return rsp->ZeroCopyDecode(buff);
}

bool DemoClientCodec::ZeroCopyEncode(const ::trpc::ClientContextPtr& ctx, const ::trpc::ProtocolPtr& in,
                                     ::trpc::NoncontiguousBuffer& out) {
  auto* req = static_cast<DemoRequestProtocol*>(in.get());

  return req->ZeroCopyEncode(out);
}

::trpc::ProtocolPtr DemoClientCodec::CreateRequestPtr() {
  return std::make_shared<DemoRequestProtocol>();
}

::trpc::ProtocolPtr DemoClientCodec::CreateResponsePtr() {
  return std::make_shared<DemoResponseProtocol>();
}

uint32_t DemoClientCodec::GetSequenceId(const ::trpc::ProtocolPtr& rsp) const {
  auto* rsp_msg = static_cast<DemoResponseProtocol*>(rsp.get());
  return rsp_msg->packet_id;
}

}  // namespace examples::demo
