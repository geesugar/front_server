/*
 * Copyright [2013-2017] <geesugar>
 * file: server.cc
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server/config.h"
#include "server/defines.h"
#include "server/server.h"
namespace Front { namespace Server {
Server::Server() {
  CHECK(InitServer()) << "init server failed.";
}

Server::~Server() {
}

bool Server::InitServer() {
  int32_t sock = -1;
  if (!CreateSocket(&sock)) {
    LOG(ERROR) << FUNC_NAME << "create socket failed.";
    return false;
  }

  if (!Listen(sock)) {
    LOG(ERROR) << FUNC_NAME << "server listen failed.";
    return false;
  }

  pid_t pid = getpid();
  LOG(INFO) << FUNC_NAME << "start server. pid[" << pid << "] port["
    << FLAGS_port << "]";
  return true;
}

bool Server::CreateSocket(int32_t* fd) {
  int32_t sock = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sock) {
    LOG(ERROR) << FUNC_NAME << "create socket failed. error["
      << strerror(errno) << "]";
    return false;
  }

  if (!SetReUseAddr(sock)) {
    LOG(ERROR) << FUNC_NAME << "set socket reuse address failed.";
    return false;
  }

  *fd = sock;
  return true;
}

/*
 * DESC: The SO_REUSEADDR socket option allows a socket to forcibly
 *       bind to a port in use by another socket.
 */
bool Server::SetReUseAddr(const int32_t& fd) {
  int32_t yes = 1;
  if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    LOG(ERROR) << FUNC_NAME << "setsockopt SO_REUSEADDR failed. error["
      << strerror(errno) << "]";
    return false;
  }
  return true;
}

bool Server::Listen(const int32_t& sock) {
  // bind
  struct sockaddr_in sock_addr;
  memset(&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_addr.sin_port = htons(FLAGS_port);
  if (-1 == bind(sock, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr))) {
    LOG(ERROR) << FUNC_NAME << "bind sock[" << sock << "] on[INADDR_ANY:"
      << FLAGS_port << "] failed. error[" << strerror(errno) << "]";
    return false;
  }

  // listen
  if (-1 == listen(sock, FLAGS_tcp_backlog)) {
    LOG(ERROR) << FUNC_NAME << "listen error. error[" << strerror(errno) << "]";
    return false;
  }
  return true;
}

}  // namespace Server
}  // namespace Front
