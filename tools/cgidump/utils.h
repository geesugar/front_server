#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <iomanip>

class Utils {
public:
  Utils();
  virtual ~Utils();

  static void PrintHex(const u_char* ctx, const int& size, std::ostream& os = std::cout);
  static std::string Long2IP(const uint32_t& ip);
  static std::string TcpFlags2String(const int& flags);

private:
};  // class Utils
