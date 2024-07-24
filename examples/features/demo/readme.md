# demo Protocol Demo.

The server invocation relationship is as follows:

client <---> server

In common dir code, we implement thirdparty protocol.

## Usage

We can use the following command to view the directory tree.
```shell
$ tree examples/features/thirdparty_protocol/
examples/features/thirdparty_protocol/
├── client
│   ├── BUILD
│   ├── client.cc
│   └── trpc_cpp_fiber.yaml
├── CMakeLists.txt
├── common
│   ├── BUILD
│   ├── demo_client_codec.cc
│   ├── demo_client_codec.h
│   ├── demo_protocol.cc
│   ├── demo_protocol.h
│   ├── demo_server_codec.cc
│   └── demo_server_codec.h
├── README.md
├── run.sh
└── server
    ├── BUILD
    ├── demo_server.cc
    └── trpc_cpp_fiber.yaml
    ├── greeter_server.cc
    └── gretter_server.h
    └── helloworld.proto

```

We can use the following script to quickly compile and run a program.
```shell
sh ./examples/features/demo/run.sh
```


The content of the output from the client program is as follows:
``` text
请求协议开始封装
请求协议封装完成
request size: 20
request id: 1
request method_name: SayHello
request method_name_length: 8
request body: 
Tom
请求开始发送
开始处理
开始调用RPC方法
RPC调用成功
响应协议开始封装
响应协议封装完成
response size: 20
response id: 1
response error_code: 0
response: 
Hello Tom. It is my own protocol.
```
