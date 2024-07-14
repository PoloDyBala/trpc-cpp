load("//trpc:trpc.bzl", "trpc_proto_library")

licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "demo_server",
    srcs = ["demo_server.cc"],
    linkopts = [
        "-ldl",
    ],
    deps = [
        ":greeter_service",
        ":helloworld_proto",
        "//examples/features/demo/common:demo_protocol",
        "//examples/features/demo/common:demo_server_codec",
        "@trpc_cpp//trpc/common:trpc_app",
        "@trpc_cpp//trpc/common:trpc_plugin",
        "@trpc_cpp//trpc/server:method",
        "@trpc_cpp//trpc/server:non_rpc_method_handler",
        "@trpc_cpp//trpc/server:non_rpc_service_impl",
        "@trpc_cpp//trpc/server:non_rpc_service_method",
    ],
)

trpc_proto_library(
    name = "helloworld_proto",
    srcs = ["helloworld.proto"],
    generate_mock_code = True,
    use_trpc_plugin = True,
)

cc_library(
    name = "greeter_service",
    srcs = ["greeter_service.cc"],
    hdrs = ["greeter_service.h"],
    deps = [
        ":helloworld_proto",
        "@trpc_cpp//trpc/common:status",
        "@trpc_cpp//trpc/common:trpc_app",
        "@trpc_cpp//trpc/log:trpc_log",
    ],
)


