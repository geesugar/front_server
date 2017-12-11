/*
 * Copyright [2013-2017] <geesugar>
 * file: main.cc
 * author: longtao@geesugar.com
 * date: 2017-11-16
 */

#include <iostream>
#include <memory>

#include "server/src/config.h"
#include "server/src/defines.h"
#include "server/src/server.h"

using Front::Server::Server;

int32_t main(int32_t argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  std::shared_ptr<Server> svr(new Server());
  return 0;
}
