/*
 * Copyright [2013-2017] <geesugar>
 * file: ding_common.cc
 * author: longtao@geesugar.com
 * date: 2017-12-27
 */
#include "utils/curl_op/curl_op.h"
#include "utils/ding_talk/src/ding_common.h"
namespace utils {

DingMsgSender::DingMsgSender() : m_stop(false) {
  m_send_thread.reset(new std::thread(&DingMsgSender::SendDingMsgThread, this));
}

DingMsgSender::~DingMsgSender() {
  m_stop = true;
  m_msgs_cv.notify_one();
  m_send_thread->join();
}

void DingMsgSender::PushMsg(const DingMsg& msg) {
  int queue_size = 0;
  {
    std::unique_lock<std::mutex> lock(m_msgs_mtx);
    m_msgs.push_back(msg);
    queue_size = m_msgs.size();
  }

  if (1 == queue_size) {
    m_msgs_cv.notify_one();
  }
}

std::string DingMsgSender::GetHostName() {
  static std::string result = "";
  if (result.empty()) {
    char hostname[256] = {0};
    if (0 != gethostname(hostname, sizeof(hostname))) {
      result = "UNKOWN";
    } else {
      result = hostname;
    }
  }
  return result;
}

std::string DingMsgSender::GetProcessName() {
  static std::string result = "";
  if (result.empty()) {
    char process_path[1024] = {0};
    if (readlink("/proc/self/exe", process_path, sizeof(process_path)) <= 0) {
      result = "UNKOWN";
    } else {
      char* process_name = strrchr(process_path, '/');
      if (process_name) {
        result = ++process_name;
      } else {
        result = "UNKOWN";
      }
    }
  }
  return result;
}

void DingMsgSender::SendDingMsg(const DingMsg& msg) {
  std::string msg_str = msg.msg;
  if (!msg_str.empty() && '\n' == msg_str.at(msg_str.length() - 1)) {
    msg_str = msg_str.substr(0, msg_str.length() - 1);
  }
  if (msg.token.empty()) {
    return;
  }

  std::ostringstream oss;
  oss << "{\"msgtype\": \"text\", \"text\": {\"content\":\"" << "["
    << GetHostName() << "] [" << GetProcessName() << "] " << msg_str
    << "\"}, \"at\": {\"atMobiles\": [], \"isAtAll\": false}}";
  std::string url =
    "https://oapi.dingtalk.com/robot/send?access_token=" + msg.token;

  std::string http_return;
  CurlOp::GetHttpReturn(url, oss.str(), JSON_TYPE, 0, 0, &http_return);
}

void DingMsgSender::SendDingMsgThread() {
  LOG(INFO) << __func__ << " send ding message thread start";
  while (!m_stop) {
    DingMsg msg;
    bool has_msg = false;
    {
      std::unique_lock<std::mutex> lock(m_msgs_mtx);
      m_msgs_cv.wait_for(lock, std::chrono::seconds(1), [&] {
        return !m_msgs.empty() || m_stop;
        });
      if (!m_msgs.empty()) {
        msg = m_msgs.front();
        m_msgs.pop_front();
        has_msg = true;
      }
    }

    if (has_msg) {
      SendDingMsg(msg);
    }
  }
}

DingStreamBuf::DingStreamBuf(const std::string& ding_talk_token,
  char* buf, int len) : m_ding_talk_token(ding_talk_token) {
  setp(buf, buf + len - 2);
}

DingStreamBuf::~DingStreamBuf() {
}

int DingStreamBuf::sync() {
  int buf_len = static_cast<int>(epptr() - pbase());
  std::string msg_str(pbase());
  LOG(INFO) << "[DING_MSG] " << msg_str;
  DingMsg msg(m_ding_talk_token, msg_str);
  DingMsgSender::GetInstance()->PushMsg(msg);

  memset(pbase(), 0, buf_len + 2);
  setp(pbase(), epptr());
  return 0;
}

}  // namespace utils
