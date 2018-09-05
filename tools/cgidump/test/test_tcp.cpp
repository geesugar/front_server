#include "gtest/gtest.h"
#include "../tcp_buf.h"
#include "../utils.h"


int main(int argc, char **argv) {
  unsigned char data[128] = {0};
  for (unsigned int i = 0; i < sizeof(data); ++i) {
    data[i] = i;
  }
  Utils::PrintHex(data, 128);
  std::cout << "------------"<< std::endl;

  TcpBuf buf("ABC");
  std::cout << buf.InputTcpPkg(data, 9) << std::endl;
  std::cout << buf.InputTcpPkg(data, 4) << std::endl;
  std::cout << buf.InputTcpPkg(data + 4, 2) << std::endl;
  unsigned char rd_buf[128] = {0};
  int rd_len = buf.OutputTcpPkg(rd_buf, 3) ;
  std::cout << "read: "<< rd_len << " data:" << std::endl;
  Utils::PrintHex(rd_buf, rd_len);

  std::cout << "write: "<< buf.InputTcpPkg(data + 3, 3) << std::endl;

  memset(rd_buf, 0, 128);
  rd_len = buf.OutputTcpPkg(rd_buf, 5) ;
  std::cout << "read: "<< rd_len << " data:" << std::endl;
  Utils::PrintHex(rd_buf, rd_len);

  std::cout << "write: "<< buf.InputTcpPkg(data + 5, 5) << std::endl;

  memset(rd_buf, 0, 128);
  rd_len = buf.OutputTcpPkg(rd_buf, 128) ;
  std::cout << "read: "<< rd_len << " data:" << std::endl;
  Utils::PrintHex(rd_buf, rd_len);
  return 0;
}

