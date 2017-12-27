#include <iostream>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include "utils/ding_talk/ding_talk.h"

int main(int argc, char* argv[]) {
  std::vector<std::thread> ths;
  for (int i = 0 ; i < 10; ++i) {
    ths.push_back(std::thread([=] {
        int index = i + 1;
        while (1) {
        utils::DingTalk(std::to_string(index)) << "ABCDEFGHIJKLMN" << std::endl;
        int v1 = rand() % 3 + 1;
        std::this_thread::sleep_for(std::chrono::seconds(v1));
        }}));
  }
  std::this_thread::sleep_for(std::chrono::seconds(20000));
  return 0;
}
