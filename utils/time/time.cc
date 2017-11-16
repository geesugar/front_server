/*
 * Copyright [2013-2017] <geesugar>
 * file: time.cc
 * author: longtao@geesugar.com
 * date: 2017-11-16
 */
#include <ctime>
#include <iomanip>
#include <iostream>

#include "utils/time/time.h"
namespace utils { namespace time {
int64_t CurUTCTimeStamp() {
  std::time_t t = std::time(nullptr);
  return t;
}

}  // namespace time
}  // namespace utils
