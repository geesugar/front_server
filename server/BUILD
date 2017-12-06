cc_binary(
    name = 'front_server',
    srcs = [
      'main.cc',
      'server.cc',
      'config.cc',
      'helper.cc',
    ],
    deps = [
      "//thirdparty/glog:glog",
      "//thirdparty/gflags:gflags",
      "//thirdparty/event2:event",
      "#pthread",
    ]
)
