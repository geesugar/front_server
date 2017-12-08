/*
 * Copyright [2013-2017] <geesugar>
 * file: config.h
 * author: longtao@geesugar.com
 * date: 2017-11-17
 */
#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DECLARE_int32(tcp_backlog);

DECLARE_int32(port);

#endif  // SERVER_CONFIG_H
