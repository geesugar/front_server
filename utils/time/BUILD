cc_library (
  name = "time",
  srcs = [
    "time.cc",
  ],
  deps = [
    "//thirdparty/glog:glog",
  ],
)

cc_test(
    name = 'time_test',
    srcs = 'time_test.cc',
    deps = [
      ':time',
      '#pthread',
    ]
)
