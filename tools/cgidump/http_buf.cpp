#include <iostream>
#include "http_buf.h"

HttpBuf::HttpBuf(const std::string& client, const bool& input) : m_input(input) {
  m_tcp_buf = new TcpBuf(client);
}

HttpBuf::~HttpBuf() {
  delete m_tcp_buf;
  m_tcp_buf = NULL;
}

bool HttpBuf::IsHttpHeader(const uint8_t* data, const int& len) {
  if (0 == len) return false;

  // TODO(towerlong) regex
  const char* tmp = (const char*)data;
  if (0 == strncmp(tmp, "POST", strlen("POST")) ||
    0 == strncmp(tmp, "GET", strlen("GET")) ||
    0 == strncmp(tmp, "HTTP", strlen("HTTP"))) {
    return true;
  }
  return false;
}

bool HttpBuf::GetHttpHeaders(const uint8_t* data, const int& len,
  std::map<std::string, std::string>* headers, std::string* first_line, int* remain_pos) {
  if (0 == len) return false;

  int pos = 0;
  int i = 0;
  for (; i < len; ++i) {
    if ('\n' == data[i]) {
      if (i > 0 || '\r' == data[i - 1]) {
        if (i > 1) {
          // \r\n
          if (0 == i - pos - 1 && i > 3 && data[i - 2] == '\n' && data[i - 3] == '\r') {
            // \r\n\r\n
            break;
          }
          std::string head((const char*)data + pos, i - pos - 1);
          std::vector<std::string> kv;
          Utils::SplitByDelimiter(head, ":", &kv);
          if (kv.size() != 2) {
            if (headers->size() == 0) {
              *first_line = Utils::TrimStr(head);
            } else {
              std::cerr << "illegal head[" << head << "]" << std::endl;
            }
          } else {
            (*headers)[Utils::TrimStr(kv[0])] = Utils::TrimStr(kv[1]);
          }
        }
        pos = i + 1;
      }
    }
  }
  *remain_pos = i + 1;
  return true;
}

bool HttpBuf::ParseHttpPkg() {
  uint8_t read_buf[4096] = {0};
  int32_t len = m_tcp_buf->PreOutputTcpPkg(read_buf, sizeof(read_buf) / sizeof(uint8_t));
  if (len <= 0) {
    return false;
  }

  std::map<std::string, std::string> headers;
  int remain_pos = 0;
  std::string first_line;
  bool ret = GetHttpHeaders(read_buf, len, &headers, &first_line, &remain_pos);
  if (!ret) {
    std::cout << "get http headers failed. client[" << m_tcp_buf->GetClientStr()
      << "]" << std::endl;
    return false;
  }
  std::cout << first_line << std::endl;

  auto find_it = headers.find("Content-Length");
  if (find_it == headers.end()) {
    std::cerr << "can not find Content-Length in http header. client["
      << m_tcp_buf->GetClientStr() << "]" << std::endl;
    return false;
  }
  std::string content_length = find_it->second.c_str();

  int length = atoi(content_length.c_str()) + remain_pos;
  if (m_tcp_buf->GetDataLen() >= length) {
    memset(read_buf, 0, sizeof(read_buf));
    len = m_tcp_buf->OutputTcpPkg(read_buf, length);
    if (len != length) {
      std::cerr << "get tcp buf faild" << std::endl;
      return false;
    }
    std::cout << "Content-Length: " << content_length << " pos:" << remain_pos << std::endl;
    // Utils::PrintHex(read_buf, len);
  } else {
    std::cout << "tcp has been segmented. Content-Length[" << content_length << "]" << std::endl;
  }
  return true;
}

bool HttpBuf::InputTcpPkg(const uint8_t* data, const int& len) {
  if (!IsHttpHeader(data, len) && 0 == m_tcp_buf->GetDataLen()) {
    std::cerr << "drop client[" << m_tcp_buf->GetClientStr() << "] pkg, len[" << len << "]" << std::endl;
    return false;
  }

  bool ret = m_tcp_buf->InputTcpPkg(data, len);
  if (!ret) {
    std::cerr << "input tcp pkg to tcp failed. client[" << m_tcp_buf->GetClientStr() << "] len["
      << len << "]" << std::endl;
    return false;
  }

  ParseHttpPkg();
  return true;
}
