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
#include "utils/ding_talk/ding_common.h"

namespace utils {

#define DING_TALK_BUFF_LENGTH 4096

/*
 * DingTalk could send message to dingding chat group
 * Usage: DingTalk(access_token) << msg << std::endl;
 * the output stream must end with std::endl, otherwise will not send dingding message
 * Reference:
 * https://open-doc.dingtalk.com/docs/doc.htm?spm=a219a.7629140.0.0.qaR1XD&treeId=257&articleId=105735&docType=1
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
