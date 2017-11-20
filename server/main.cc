/*
 * Copyright [2013-2017] <geesugar>
 * file: main.cc
 * author: longtao@geesugar.com
 * date: 2017-11-16
 */

#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server/config.h"
#include "server/defines.h"
#include "server/server.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

int32_t main(int32_t argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);
  FLAGS_log_dir = ".";

  int32_t listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (0 != listen(listen_sock, FLAGS_tcp_backlog)) {
    LOG(ERROR) << FUNC_NAME << "listen error. errno[" << errno << "]";
    return -1;
  }

  LOG(INFO) << __func__ << " server port[" << FLAGS_port << "]";
  return 0;
}
