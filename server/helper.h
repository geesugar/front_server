/*
 * Copyright [2013-2017] <geesugar>
 * file: helper.h
 * author: longtao@geesugar.com
 * date: 2017-12-06
 */
#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <string>

#include "thirdparty/glog/logging.h"

namespace Front { namespace Server {

std::string GetAddressBySocket(const int32_t sock,
  std::string* ip = NULL, int32_t* port = NULL);

}  // namespace Server
}  // namespace Front
#endif  // SERVER_HELPER_H
