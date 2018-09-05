#pragma once
#include <map>
#include <vector>
#include "utils.h"
#include "tcp_buf.h"

class HttpBuf {
public:
  HttpBuf(const std::string& client, const bool& input);
  virtual ~HttpBuf();
  bool InputTcpPkg(const uint8_t* data, const int& len);

private:
  bool IsHttpHeader(const uint8_t* data, const int& len);
  bool ParseHttpPkg();
  bool GetHttpHeaders(const uint8_t* data, const int& len,
    std::map<std::string, std::string>* headers, std::string* first_line, int* remain_pos);

  TcpBuf* m_tcp_buf;
  bool m_input;
};
