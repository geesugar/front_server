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
};  // class Server
}  // namespace Server
}  // namespace Front
#endif  // SERVER_SERVER_H
