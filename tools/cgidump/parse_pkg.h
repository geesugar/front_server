#pragma once
#include "defines.h"

class ParsePkg {
 public:
  static ParsePkg* GetInstance();

  void Init(const std::string& ip, const int& port);

  bool InputPkg(const u_char* data, const int& len);

 private:
  ParsePkg();
  ~ParsePkg();

  void PrintPkgInfo(const EtherHeader* eh, const IpHeader* ih, const TcpHeader* th);
  bool ParseHttp(const u_char* data, const int& len, int* remain_pos);
  bool GetClientAddr(const IpHeader* ih, const TcpHeader* th,
    std::string* client_ip, int* client_port);

 private:
  std::string m_ip;
  int m_port;
  bool m_inited;
  // std::map<std::string, > m_buffers;
};
