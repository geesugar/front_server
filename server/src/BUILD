cc_binary(
    name = 'front_server',
    srcs = [
      'main.cc',
      'server.cc',
      'config.cc',
      'helper.cc',
      'zookeeper_proxy.cc',
    ],
    deps = [
      "//thirdparty/glog:glog",
      "//thirdparty/gflags:gflags",
      "//thirdparty/event2:event",
      "//thirdparty/zookeeper:zookeeper_mt",
      "#pthread",
    ]
)
