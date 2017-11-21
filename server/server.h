/*
 * Copyright [2013-2017] <geesugar>
 * file: server.h
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "thirdparty/glog/logging.h"

namespace Front { namespace Server {
class Server {
public:
  Server();
  virtual ~Server();
  // add your public interface here:

private:
  bool InitServer();

  bool CreateSocket(int32_t* fd);
  bool SetReUseAddr(const int32_t& fd);
  bool Listen(const int32_t& sock);
};  // class Server
}  // namespace Server
}  // namespace Front
#endif  // SERVER_SERVER_H
