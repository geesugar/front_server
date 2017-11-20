/*
 * Copyright [2013-2017] <geesugar>
 * file: main.cc
 * author: longtao@geesugar.com
 * date: 2017-11-16
 */

#include <iostream>

#include "server/config.h"
#include "server/defines.h"
#include "server/server.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

int32_t main(int32_t argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_log_dir = ".";

  return 0;
}
