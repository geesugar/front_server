/*
 * Copyright [2013-2017] <geesugar>
 * file: ding_common.h
 * author: longtao@geesugar.com
 * date: 2017-12-27
 */
#ifndef UTILS_DING_TALK_DING_COMMON_H
#define UTILS_DING_TALK_DING_COMMON_H

#include <string.h>
#include <condition_variable>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "thirdparty/glog/logging.h"

namespace utils {

class DingMsg {
 public:
  DingMsg(const std::string token, const std::string msg)
    : token(token), msg(msg) {}
  DingMsg() : token(""), msg("") {}
  std::string token;
  std::string msg;
};

class DingMsgSender {
 public:
  static  DingMsgSender* GetInstance() {
    static DingMsgSender ins;
    return &ins;
  }

  void PushMsg(const DingMsg& msg);

 private:
  DingMsgSender();
  virtual ~DingMsgSender();
  void SendDingMsgThread();
  void SendDingMsg(const DingMsg& mgs);

 private:
  std::list<DingMsg> m_msgs;
  std::mutex m_msgs_mtx;
  std::condition_variable m_msgs_cv;

  std::shared_ptr<std::thread> m_send_thread;
  bool m_stop;
};

class DingStreamBuf : public std::streambuf {
 public:
  DingStreamBuf(const std::string& ding_talk_token, char* buf, int len);
  virtual ~DingStreamBuf();

  int sync();

 private:
  std::string m_ding_talk_token;
};  // class DingCommon
}  // namespace utils
#endif  // UTILS_DING_TALK_DING_COMMON_H
