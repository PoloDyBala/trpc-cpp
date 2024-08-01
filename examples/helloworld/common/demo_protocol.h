#pragma once

#include <cstring>
#include <memory>
#include <string>

#include "trpc/codec/protocol.h"
#include "trpc/codec/trpc/trpc.pb.h"
#include "trpc/util/buffer/noncontiguous_buffer.h"
namespace examples::demo {
using namespace trpc;
class DemoRequestProtocol : public ::trpc::Protocol {
 public:
  ~DemoRequestProtocol() override = default;

  bool ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) override;

  bool ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) override;

  bool GetRequestId(uint32_t& req_id) const override {
    req_id = packet_id;
    return true;
  }
  bool SetRequestId(uint32_t req_id) override {
    packet_id = req_id;
    return true;
  }

  void SetFuncName(std::string func_name) override {
    // std::cout << "调用DemoRequestProtocol::SetFuncName" << std::endl;
    func_ = func_name;
  }
  const std::string& GetFuncName() const override {
    // std::cout << "调用DemoRequestProtocol::GetFuncName" << std::endl;
    // std::cout << "func_name::" << func_ << std::endl;
    return func_;
  }

  void SetNonContiguousProtocolBody(NoncontiguousBuffer&& buff) override {
    req_body = std::move(buff);
    // std::cout << "调用DemoRequestProtocol::SetNonContiguousProtocolBody" << std::endl;
  }
  NoncontiguousBuffer GetNonContiguousProtocolBody() override {
    // std::cout << "调用DemoRequestProtocol::GetNonContiguousProtocolBody" << std::endl;
    // std::cout << "这里的得到的是什么大小" << static_cast<size_t>(req_body.ByteSize()) << std::endl;
    return std::move(req_body);
  }

 public:
  uint32_t packet_size{0};
  uint32_t packet_id{0};
  uint8_t method_name_length{0};
  std::string func_;
  NoncontiguousBuffer req_body;
};

class DemoResponseProtocol : public ::trpc::Protocol {
 public:
  ~DemoResponseProtocol() override = default;

  bool ZeroCopyDecode(::trpc::NoncontiguousBuffer& buff) override;

  bool ZeroCopyEncode(::trpc::NoncontiguousBuffer& buff) override;

  bool GetRequestId(uint32_t& req_id) const override {
    req_id = packet_id;
    return true;
  }
  bool SetRequestId(uint32_t req_id) override {
    packet_id = req_id;
    return true;
  }

  void SetNonContiguousProtocolBody(NoncontiguousBuffer&& buff) override { rsp_body = std::move(buff); }
  NoncontiguousBuffer GetNonContiguousProtocolBody() override { return std::move(rsp_body); }

 public:
  uint32_t packet_size{0};
  uint32_t packet_id{0};
  uint8_t error_code{0};
  NoncontiguousBuffer rsp_body;
};

using DemoRequestProtocolPtr = std::shared_ptr<DemoRequestProtocol>;
using DemoResponseProtocolPtr = std::shared_ptr<DemoResponseProtocol>;

}  // namespace examples::demo
