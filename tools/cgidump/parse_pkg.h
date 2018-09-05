#pragma once
#include <string.h>
#include <stdint.h>
#include <map>
#include "defines.h"
#include "session.h"

class ParsePkg {
 public:
  static ParsePkg* GetInstance();

  void Init(const std::string& ip, const int& port);

  bool InputPkg(const uint8_t* data, const int& len);

 private:
  ParsePkg();
  ~ParsePkg();

  void PrintPkgInfo(const EtherHeader* eh, const IpHeader* ih, const TcpHeader* th);
  bool GetClientAddr(const IpHeader* ih, const TcpHeader* th,
    std::string* client_ip, int* client_port, bool* input);
  bool ParseHttpPkg(TcpBuf* buf);
  bool GetHttpHeaders(const uint8_t* data, const int& len,
    std::map<std::string, std::string>* headers, int* remain_pos);
  bool InputPkg2Session(const std::string& c_ip, const int& c_port, const bool& input, const uint8_t* data, const int& len);

 private:
  std::string m_ip;
  int m_port;
  bool m_inited;
  std::map<std::string, Session*> m_sessions;
};
