#pragma once

#include <memory>
#include <string>
#include <cstring>

#include "trpc/codec/protocol.h"

namespace examples::demo {

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

 public:
  uint32_t packet_size{0};
  uint32_t packet_id{0};
  uint8_t method_name_length{0};
  std::string method_name;
  std::string req_data;
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

 public:
  uint32_t packet_size{0};
  uint32_t packet_id{0};
  uint8_t error_code{0};
  std::string rsp_data;
};

using DemoRequestProtocolPtr = std::shared_ptr<DemoRequestProtocol>;
using DemoResponseProtocolPtr = std::shared_ptr<DemoResponseProtocol>;

} 
