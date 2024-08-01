package(default_visibility = ["//visibility:public"])

cc_library(
    name = "demo_protocol",
    srcs = ["demo_protocol.cc"],
    hdrs = ["demo_protocol.h"],
    deps = [
        "@trpc_cpp//trpc/codec:protocol",
        "@trpc_cpp//trpc/util:likely",
        "@trpc_cpp//trpc/util:ref_ptr",
        "@trpc_cpp//trpc/util/buffer:noncontiguous_buffer",
        "@trpc_cpp//trpc/util/buffer:zero_copy_stream",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)

cc_library(
    name = "demo_server_codec",
    srcs = ["demo_server_codec.cc"],
    hdrs = ["demo_server_codec.h"],
    deps = [
        ":demo_protocol",
        "@trpc_cpp//trpc/codec:server_codec",
        "@trpc_cpp//trpc/server:server_context",
    ],
)

cc_library(
    name = "demo_client_codec",
    srcs = ["demo_client_codec.cc"],
    hdrs = ["demo_client_codec.h"],
    deps = [
        ":demo_protocol",
        "@trpc_cpp//trpc/client:client_context",
        "@trpc_cpp//trpc/codec:client_codec",
        "@trpc_cpp//trpc/codec:codec_helper",
        "@trpc_cpp//trpc/common:status",
        "@trpc_cpp//trpc/compressor:trpc_compressor",
        "@trpc_cpp//trpc/runtime/iomodel/reactor/common:connection",
        "@trpc_cpp//trpc/serialization:serialization_factory",
        "@trpc_cpp//trpc/util/buffer:noncontiguous_buffer",
        "@trpc_cpp//trpc/util/buffer:zero_copy_stream",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)
