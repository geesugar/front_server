/*
 * Copyright [2013-2017] <geesugar>
 * file: ding_talk.cc
 * author: longtao@geesugar.com
 * date: 2017-12-26
 */
#include "utils/ding_talk/ding_talk.h"
namespace utils {
DingTalk::DingTalk(const std::string& ding_talk_token) : std::ostream(NULL) {
  m_buff = new char[DING_TALK_BUFF_LENGTH];
  m_buff_len = DING_TALK_BUFF_LENGTH;
  m_streambuf = new DingStreamBuf(ding_talk_token, m_buff, m_buff_len);
  rdbuf(m_streambuf);
}

DingTalk::~DingTalk() {
  if (m_streambuf) {
    delete m_streambuf;
  }
  if (m_buff) {
    delete[] m_buff;
  }
}

}  // namespace utils
