/*
 * Copyright [2013-2017] <geesugar>
 * file: ding_talk.h
 * author: longtao@geesugar.com
 * date: 2017-12-26
 */
#ifndef UTILS_DING_TALK_DING_TALK_H
#define UTILS_DING_TALK_DING_TALK_H

#include <string>

#include "thirdparty/glog/logging.h"
#include "utils/ding_talk/src/ding_common.h"

namespace utils {

#define DING_TALK_BUFF_LENGTH 4096

/*
 * Desc:
 *   DingTalk make sendind message to dingding chat group easily
 *
 * Usage:
 *   DingTalk(access_token) << msg << std::endl;
 *
 * NOte:
 *   The output stream must be ended with std::endl, otherwise it does
 *   not send dingding message, because the buffer's flush function
 *   in which realize the send dingding message action would be invoked
 *   when outputing std::endl.
 *
 * Reference:
 *   https://open-doc.dingtalk.com/docs/doc.htm?spm=a219a.7629140.0.0.qaR1XD&treeId=257&articleId=105735&docType=1
 */
class DingTalk : public std::ostream {
 public:
  explicit DingTalk(const std::string& ding_talk_token);
  virtual ~DingTalk();

 private:
  DingStreamBuf* m_streambuf;
  char* m_buff;
  int m_buff_len;
};  // class DingTalk
}  // namespace utils
#endif  // UTILS_DING_TALK_DING_TALK_H
