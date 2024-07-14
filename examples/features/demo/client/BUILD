load("//trpc:trpc.bzl", "trpc_proto_library")

licenses(["notice"])
package(default_visibility = ["//visibility:public"])
cc_binary(
    name = "client",
    srcs = ["client.cc"],
    deps = [
        "//examples/features/demo/server:helloworld_proto",
        "//examples/features/demo/common:demo_client_codec",
        "//examples/features/demo/common:demo_protocol",
        "@com_github_gflags_gflags//:gflags",
        "@trpc_cpp//trpc/client:make_client_context",
        "@trpc_cpp//trpc/client:non_rpc_service_proxy",
        "@trpc_cpp//trpc/common:runtime_manager",
        "@trpc_cpp//trpc/client:trpc_client",
        "@trpc_cpp//trpc/common:trpc_plugin",
    ],
)

