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
#include <memory>
#include <string>
#include <iostream>

#include "trpc/common/trpc_app.h"
#include "trpc/common/trpc_plugin.h"
#include "trpc/log/trpc_log.h"
#include "trpc/server/method.h"
#include "trpc/server/non_rpc_method_handler.h"
#include "trpc/server/non_rpc_service_impl.h"
#include "trpc/server/non_rpc_service_method.h"

#include "examples/features/demo/common/demo_protocol.h"
#include "examples/features/demo/common/demo_server_codec.h"

#include "examples/features/demo/server/helloworld.trpc.pb.h"
#include "examples/features/demo/server/greeter_service.h"

namespace examples::demo {

class DemoServiceImpl : public ::trpc::NonRpcServiceImpl {
 public:
  DemoServiceImpl() {
    auto handler = new ::trpc::NonRpcMethodHandler<DemoRequestProtocol, DemoResponseProtocol>(
        std::bind(&DemoServiceImpl::Execute, this, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
    AddNonRpcServiceMethod(
        new ::trpc::NonRpcServiceMethod(::trpc::kNonRpcName, ::trpc::MethodType::UNARY, handler));
  }

  ::trpc::Status Execute(const ::trpc::ServerContextPtr& context,
                         const DemoRequestProtocol* in,
                         DemoResponseProtocol* out) {
    TRPC_FMT_INFO("request data: {}", in->req_data);
    // 反序列化为 req_data 为 HelloRequest 对象
    std::cout << "开始处理" << std::endl;
    std::cout << in->req_data << std::endl;
    ::trpc::examples::demo::HelloRequest request;
    request.set_msg(in->req_data);
    ::trpc::examples::demo::HelloReply reply;
    
    examples::demo::GreeterServiceImpl service;
    std::cout << "开始调用RPC方法" << std::endl;
    service.SayHello(context, &request, &reply);
    std::cout << "RPC调用成功" << std::endl;
    std::cout << reply.msg() << std::endl;
    
    // if(!request.ParseFromString(in->req_data)) {
    //     std::cerr << "Failed to parse request data." << std::endl;
    // }

    // // 处理请求并准备响应
    
    // trpc::examples::demo::HelloReply reply;
    // reply.set_msg(in->req_data);

    std::cout << "响应协议开始封装" << std::endl; 
    out->packet_size = 100;
    out->packet_id = 1;
    out->error_code = 1;

    // 序列化 HelloReply 对象为字符串并存储在 rsp_data 中
    if (!reply.SerializeToString(&out->rsp_data)) {
        std::cerr << "Failed to serialize response data." << std::endl;
    }

    std::cout << "响应协议封装完成" << std::endl;
    // std::cout << out->rsp_data << std::endl;

    // return ::trpc::kSuccStatus;
    return ::trpc::kSuccStatus;




  }



};




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

    RegisterService(service_name, std::make_shared<DemoServiceImpl>());
    return 0;
  }

  void Destroy() override {}
};

}  // namespace examples::demo_protocol

int main(int argc, char** argv) {
  std::cout << "222" << std::endl;
  examples::demo::DemoServer demo_server;
  demo_server.Main(argc, argv);
  demo_server.Wait();

  return 0;
}