#include <iostream>

#include "glog/logging.h"

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_log_dir = ".";

  LOG(INFO) << __func__ << " HELLO";
  return 0;
}
