#include "examples/helloworld/common/demo_client_codec.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>
#include "examples/helloworld/common/demo_protocol.h"
#include "trpc/codec/codec_helper.h"
#include "trpc/compressor/trpc_compressor.h"
#include "trpc/serialization/serialization_factory.h"
#include "trpc/util/buffer/zero_copy_stream.h"
#include "trpc/util/likely.h"
#include "trpc/util/log/logging.h"

namespace examples::demo {

using namespace trpc;
int DemoClientCodec::ZeroCopyCheck(const ::trpc::ConnectionPtr& conn, ::trpc::NoncontiguousBuffer& in,
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

    if (total_buff_size < packet_size) {
      break;
    }

    out.emplace_back(in.Cut(packet_size));
  }

  return !out.empty() ? ::trpc::PacketChecker::PACKET_FULL : ::trpc::PacketChecker::PACKET_LESS;
}

bool DemoClientCodec::ZeroCopyDecode(const ::trpc::ClientContextPtr& ctx, std::any&& in, ::trpc::ProtocolPtr& out) {
  auto buff = std::any_cast<::trpc::NoncontiguousBuffer&&>(std::move(in));
  return out->ZeroCopyDecode(buff);
}

bool DemoClientCodec::ZeroCopyEncode(const ::trpc::ClientContextPtr& ctx, const ::trpc::ProtocolPtr& in,
                                     ::trpc::NoncontiguousBuffer& out) {
  auto* req = static_cast<DemoRequestProtocol*>(in.get());
  return req->ZeroCopyEncode(out);
}

::trpc::ProtocolPtr DemoClientCodec::CreateRequestPtr() { return std::make_shared<DemoRequestProtocol>(); }

::trpc::ProtocolPtr DemoClientCodec::CreateResponsePtr() { return std::make_shared<DemoResponseProtocol>(); }

uint32_t DemoClientCodec::GetSequenceId(const ::trpc::ProtocolPtr& rsp) const {
  auto* rsp_msg = static_cast<DemoResponseProtocol*>(rsp.get());
  return rsp_msg->packet_id;
}

bool DemoClientCodec::FillRequest(const ::trpc::ClientContextPtr& context, const ::trpc::ProtocolPtr& in, void* body) {
  TRPC_ASSERT(body);

  auto* demo_req_protocol = static_cast<DemoRequestProtocol*>(in.get());
  TRPC_ASSERT(demo_req_protocol);

  if (TRPC_UNLIKELY(context->IsTransparent())) {
    return ProcessTransparentReq(demo_req_protocol, body);
  }

  // 序列化传入的pb对象
  serialization::SerializationType serialization_type = context->GetReqEncodeType();
  serialization::SerializationFactory* serializationfactory = serialization::SerializationFactory::GetInstance();
  auto serialization = serializationfactory->Get(serialization_type);
  // std::cout << "这里序列化的类型是什么" << serialization_type << std::endl;
  if (serialization == nullptr) {
    std::string error_msg = "not support serialization_type:";
    error_msg += std::to_string(serialization_type);

    TRPC_LOG_ERROR(error_msg);

    context->SetStatus(Status(GetDefaultClientRetCode(codec::ClientRetCode::ENCODE_ERROR), "unknown encode type"));
    return false;
  }

  serialization::DataType type = context->GetReqEncodeDataType();

  NoncontiguousBuffer data;
  bool encode_ret = serialization->Serialize(type, body, &data);
  if (TRPC_UNLIKELY(!encode_ret)) {
    context->SetStatus(Status(GetDefaultClientRetCode(codec::ClientRetCode::ENCODE_ERROR), "encode failed."));
    return encode_ret;
  }

  auto compress_type = context->GetReqCompressType();
  bool compress_ret = compressor::CompressIfNeeded(compress_type, data, context->GetReqCompressLevel());
  if (TRPC_UNLIKELY(!compress_ret)) {
    context->SetStatus(Status(GetDefaultClientRetCode(codec::ClientRetCode::ENCODE_ERROR), "compress failed."));
    return compress_ret;
  }

  // 设置总体的pack_size, packet_id, method_name_length
  std::size_t serialized_size = data.ByteSize();
  demo_req_protocol->packet_size = 9 + serialized_size + demo_req_protocol->GetFuncName().size();
  demo_req_protocol->packet_id = 0;
  demo_req_protocol->method_name_length = demo_req_protocol->GetFuncName().size();
  demo_req_protocol->SetNonContiguousProtocolBody(std::move(data));
  return true;
}

bool DemoClientCodec::ProcessTransparentReq(DemoRequestProtocol* req_protocol, void* body) {
  auto buf = *reinterpret_cast<NoncontiguousBuffer*>(body);
  req_protocol->SetNonContiguousProtocolBody(std::move(buf));
  return true;
}

bool DemoClientCodec::FillResponse(const ::trpc::ClientContextPtr& ctx, const ::trpc::ProtocolPtr& in, void* body) {
  TRPC_ASSERT(body);

  auto* demo_rsp_protocol = static_cast<DemoResponseProtocol*>(in.get());
  TRPC_ASSERT(demo_rsp_protocol);

  auto rsp_body_data = demo_rsp_protocol->GetNonContiguousProtocolBody();

  serialization::SerializationType serialization_type = ctx->GetRspEncodeType();
  serialization::SerializationFactory* serializationfactory = serialization::SerializationFactory::GetInstance();
  auto serialization = serializationfactory->Get(serialization_type);

  serialization::DataType type = ctx->GetRspEncodeDataType();

  bool decode_ret = serialization->Deserialize(&rsp_body_data, type, body);
  if (!decode_ret) {
    std::string error("trpc input pb response parese from array failed");
    ctx->SetStatus(Status(GetDefaultClientRetCode(codec::ClientRetCode::DECODE_ERROR), 0, error));
    return decode_ret;
  }

  return true;
}

}  // namespace examples::demo
