bazel build //examples/features/demo/...

echo "begin"
./bazel-bin/examples/features/demo/server/demo_server --config=./examples/features/demo/server/trpc_cpp_fiber.yaml &
sleep 1
./bazel-bin/examples/features/demo/client/client --client_config=./examples/features/demo/client/trpc_cpp_fiber.yaml
killall demo_server
