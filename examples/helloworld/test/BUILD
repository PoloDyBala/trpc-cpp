cc_binary(
    name = "future_client",
    srcs = ["future_client.cc"],
    deps = [
        "//examples/helloworld:helloworld_proto",
        "//examples/helloworld/common:demo_client_codec",
        "//examples/helloworld/common:demo_protocol",
        "@com_github_gflags_gflags//:gflags",
        "@trpc_cpp//trpc/client:make_client_context",
        "@trpc_cpp//trpc/client:trpc_client",
        "@trpc_cpp//trpc/common:runtime_manager",
        "@trpc_cpp//trpc/future:future_utility",
        "@trpc_cpp//trpc/log:trpc_log",
    ],
)

cc_binary(
    name = "fiber_client",
    srcs = ["fiber_client.cc"],
    deps = [
        "//examples/helloworld:helloworld_proto",
        "//examples/helloworld/common:demo_client_codec",
        "//examples/helloworld/common:demo_protocol",
        "@com_github_gflags_gflags//:gflags",
        "@trpc_cpp//trpc/client:make_client_context",
        "@trpc_cpp//trpc/client:trpc_client",
        "@trpc_cpp//trpc/common:runtime_manager",
        "@trpc_cpp//trpc/log:trpc_log",
        "@trpc_cpp//trpc/common:trpc_plugin",
    ],
)
