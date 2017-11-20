/*
 * Copyright [2013-2017] <geesugar>
 * file: server.cc
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server/config.h"
#include "server/defines.h"
#include "server/server.h"
namespace Front { namespace Server {
Server::Server() {
  InitServer();
}

Server::~Server() {
}

bool Server::InitServer() {
  int32_t listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (0 != listen(listen_sock, FLAGS_tcp_backlog)) {
    LOG(ERROR) << FUNC_NAME << "listen error. errno[" << errno << "]";
    return -1;
  }

  pid_t pid = getpid();
  LOG(INFO) << FUNC_NAME << "start server. pid[" << pid << "e port["
    << FLAGS_port << "]";
  return true;
}

}  // namespace Server
}  // namespace Front
