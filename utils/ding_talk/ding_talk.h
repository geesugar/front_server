/*
 * Copyright [2013-2017] <geesugar>
 * file: ding_talk.h
 * author: longtao@geesugar.com
 * date: 2017-12-26
 */
#ifndef UTILS_DING_TALK_DING_TALK_H
#define UTILS_DING_TALK_DING_TALK_H

#include "utils/ding_talk/ding_common.h"
#include "thirdparty/glog/logging.h"

namespace utils {

#define DING_TALK_BUFF_LENGTH 4096

class DingTalk : public std::ostream {
public:
  DingTalk(const std::string& ding_talk_token);
  virtual ~DingTalk();

private:
  DingStreamBuf* m_streambuf;
  char* m_buff;
  int m_buff_len;
};  // class DingTalk
}  // namespace utils
#endif  // UTILS_DING_TALK_DING_TALK_H
