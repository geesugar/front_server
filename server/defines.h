/*
 * Copyright [2013-2017] <geesugar>
 * file: defines.h
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#ifndef SERVER_DEFINES_H
#define SERVER_DEFINES_H

#include "thirdparty/glog/logging.h"

#ifndef FUNC_NAME
#define FUNC_NAME \
  "[" << __func__ << "] " \

#endif  // FUNC_NAME

inline std::string MethodName(const std::string& pretty_function) {
  size_t colons = pretty_function.find("(");
  size_t begin = pretty_function.substr(0, colons).rfind(" ") + 1;
  size_t end = pretty_function.rfind("(") - begin;
  return pretty_function.substr(begin, end);
}

#define __METHOD_NAME__ MethodName(__PRETTY_FUNCTION__)

#ifndef EPSILON
#define EPSILON 0.00000001
#endif  // EPSILON

#ifndef DOUBLE_NE
#define DOUBLE_NE(a, b) \
  ((std::abs((a)-(b)) >= (EPSILON)) ? true : false)
#endif  // DOUBLE_NE

#ifndef DOUBLE_GT
#define DOUBLE_GT(a, b) \
  ((((a) - (b)) > (EPSILON)) ? true : false)
#endif  // DOUBLE_GT

#endif  // SERVER_DEFINES_H
