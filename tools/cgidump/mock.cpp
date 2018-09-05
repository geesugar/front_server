#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <libpcap/include/pcap.h>

#include "utils.h"
#include "parse_pkg.h"

#define DEFAULT_SNAPLEN 262144  // same as tcpdump
#define PCAP_ERRBUF_SIZE 256

int main (int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: mock file" << std::endl;
    return -1;
  }

  ParsePkg::GetInstance()->Init("10.197.136.192", 11135);

  char ebuf[PCAP_ERRBUF_SIZE] = {0};
  pcap_t * pc = pcap_open_offline(argv[1], ebuf);
  struct pcap_pkthdr pkthdr;
  int count = 0;
  while (true) {
    const uint8_t *pkt_str = pcap_next(pc, &pkthdr);
    if (NULL == pkt_str) {
      break;
    }
    ParsePkg::GetInstance()->InputPkg(pkt_str, pkthdr.caplen);
    if (20 < count) {
      break;
    }
    count++;
  }

  pcap_close(pc);
  return 0;
}
