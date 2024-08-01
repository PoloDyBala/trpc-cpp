//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "trpc/common/trpc_app.h"
#include "trpc/common/trpc_plugin.h"
#include "trpc/log/trpc_log.h"
#include "trpc/server/method.h"
#include "trpc/server/non_rpc_method_handler.h"
#include "trpc/server/non_rpc_service_impl.h"
#include "trpc/server/non_rpc_service_method.h"

#include "examples/features/demo/common/demo_protocol.h"
#include "examples/features/demo/common/demo_server_codec.h"

#include "examples/features/demo/server/greeter_service.h"
#include "examples/features/demo/server/helloworld.trpc.pb.h"

namespace examples::demo {

class DemoServer : public ::trpc::TrpcApp {
 public:
  int RegisterPlugins() override {
    ::trpc::TrpcPlugin::GetInstance()->RegisterServerCodec(std::make_shared<DemoServerCodec>());
    return 0;
  }

  int Initialize() override {
    const auto& config = ::trpc::TrpcConfig::GetInstance()->GetServerConfig();
    // Set the service name, which must be the same as the value of the `server:service:name` configuration item
    // in the framework configuration file, otherwise the framework cannot receive requests normally
    std::string service_name = fmt::format("{}.{}.{}.{}", "trpc", config.app, config.server, "demo_service");

    TRPC_FMT_INFO("service name:{}", service_name);

    RegisterService(service_name, std::make_shared<GreeterServiceImpl>());
    return 0;
  }

  void Destroy() override {}
};

}  // namespace examples::demo

int main(int argc, char** argv) {
  examples::demo::DemoServer demo_server;
  demo_server.Main(argc, argv);
  demo_server.Wait();

  return 0;
}