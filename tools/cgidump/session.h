#pragma once

#include "http_buf.h"

class Session {
public:
  Session(const std::string& client);
  bool InputTcpPkg(const uint8_t* data, const int& len, const bool& input);
  virtual ~Session();
private:
  HttpBuf* m_input;
  HttpBuf* m_output;
};
