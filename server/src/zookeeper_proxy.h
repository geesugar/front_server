/*
 * Copyright [2013-2017] <geesugar>
 * file: zookeeper_proxy.h
 * author: longtao@geesugar.com
 * date: 2017-12-14
 */
#ifndef SERVER_SRC_ZOOKEEPER_PROXY_H
#define SERVER_SRC_ZOOKEEPER_PROXY_H

#include "thirdparty/glog/logging.h"

namespace Front { namespace Server {
class ZookeeperProxy {
public:
  ZookeeperProxy();
  virtual ~ZookeeperProxy();
  // add your public interface here:

private:
  // add your private function, members here:
  //
};  // class ZookeeperProxy
}  // namespace Server
}  // namespace Front
#endif  // SERVER_SRC_ZOOKEEPER_PROXY_H
