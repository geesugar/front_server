/*
 * Copyright [2013-2017] <geesugar>
 * file: helper.cc
 * author: longtao@geesugar.com
 * date: 2017-12-06
 */
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "server/src/defines.h"
#include "server/src/helper.h"
namespace Front { namespace Server {

std::string GetAddressBySocket(
  const int32_t sock, std::string* ip, int32_t* port) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  if (getpeername(sock, (struct sockaddr*) &addr, &addr_len) < 0) {
    LOG(ERROR) << FUNC_NAME << "get peer name failed. sock[" << sock << "]";
    return "";
  }

  int32_t port_value = ntohs(addr.sin_port);
  if (port) *port = port_value;
  std::string ip_value = inet_ntoa(addr.sin_addr);
  if (ip) *ip = ip_value;
  return ip_value + std::string(":") + std::to_string(port_value);
}

}  // namespace Server
}  // namespace Front
