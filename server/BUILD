cc_binary(
    name = 'front_server',
    srcs = [
      'main.cc',
      'server.cc',
      'config.cc'
    ],
    deps = [
      "//thirdparty/glog:glog",
      "//thirdparty/gflags:gflags",
      "#pthread",
    ]
)
