/*
 * Copyright [2013-2017] <geesugar>
 * file: send.cc
 * author: longtao@geesugar.com
 * date: 2017-12-28
 */
#include <string>

#include "utils/ding_talk/ding_talk.h"

int main(int argc, char* argv[]) {
  std::string token =
    "38b1137b19202f2f02c6594877f9457387f48a3b1062f28a5c5ea6ab9b9eb2b5";
  utils::DingTalk(token) << "测试\n1234567890";
  std::this_thread::sleep_for(std::chrono::seconds(20000));
  return 0;
}
