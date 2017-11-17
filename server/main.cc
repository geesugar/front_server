/*
 * Copyright [2013-2017] <geesugar>
 * file: main.cc
 * author: longtao@geesugar.com
 * date: 2017-11-16
 */

#include <iostream>

#include "server/config.h"
#include "server/server.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_log_dir = ".";

  LOG(INFO) << __func__ << " server port[" << FLAGS_port << "]";
  return 0;
}
