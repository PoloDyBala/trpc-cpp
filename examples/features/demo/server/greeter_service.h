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

#pragma once

#include "examples/features/demo/server/helloworld.trpc.pb.h"

namespace examples {
namespace demo {

class GreeterServiceImpl : public ::trpc::examples::demo::Greeter {
public:
  ::trpc::Status SayHello(::trpc::ServerContextPtr context,
                          const ::trpc::examples::demo::HelloRequest* request,
                          ::trpc::examples::demo::HelloReply* reply) override;

  ::trpc::Status TestSelfProtocol(::trpc::ServerContextPtr context,
                          const ::trpc::examples::demo::HelloRequest* request,
                          ::trpc::examples::demo::HelloReply* reply) override;
};

}  // namespace helloworld
}  // namespace test
