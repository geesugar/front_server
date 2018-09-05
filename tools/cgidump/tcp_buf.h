#pragma once
#include <string.h>
#include <stdint.h>
#include <string>

#define MAX_BUF_SIZE 8192

class TcpBuf {
public:
  TcpBuf(const std::string& client);
  virtual ~TcpBuf();

  bool InputTcpPkg(const uint8_t* data, const int& len);
  int32_t OutputTcpPkg(uint8_t* data, int len);
  int32_t PreOutputTcpPkg(uint8_t* data, int len);
  int32_t GetDataLen();
  std::string GetClientStr();

private:
  bool PreProcessPkg();

  int32_t RemainSize();

  std::string m_client;

  uint8_t* m_buf;
  int32_t m_start;
  int32_t m_end;
};  // class TcpBuf
