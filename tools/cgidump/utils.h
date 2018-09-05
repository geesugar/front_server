#pragma once
#include <stdint.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <vector>

class Utils {
public:
  Utils();
  virtual ~Utils();

  static void PrintHex(const uint8_t* ctx, const int& size, std::ostream& os = std::cout);
  static std::string Long2IP(const uint32_t& ip);
  static std::string TcpFlags2String(const int& flags);
  static void SplitByDelimiter(const std::string& str, const std::string& delimiter,
    std::vector<std::string>* vec_str);
  static std::string TrimStr(const std::string& str);

private:
};  // class Utils
