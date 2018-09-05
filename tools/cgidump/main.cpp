#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <vector>

#include "parse_pkg.h"
#include "utils.h"

#define DEFAULT_SNAPLEN 262144  // same as tcpdump
#define PCAP_ERRBUF_SIZE 256


void PrintHex(const uint8_t* ctx, const int& size) {
    std::ostringstream oss;
    const int cnt_per_line = 16;
    for (int i = 0; i * cnt_per_line < size; ++i) {
        int j = 0;
        for (j = i * cnt_per_line; j < size && j < (i + 1) * cnt_per_line; ++j) {
            int value = *(ctx + j);
            oss << "0x" << std::setfill('0') << std::hex << std::setw (2) << value << " ";
        }

        int remain = (i + 1) * cnt_per_line - j;
        for (int k = 0; k < remain; ++k) {
            oss << "     ";
        }
        oss << "     ";
        for (j = i * cnt_per_line; j < size && j < (i + 1) * cnt_per_line; ++j) {
            char value = *(ctx + j);
            if ((value >= '0' && value <= '9') ||
                (value >= 'a' && value <= 'z') ||
                (value >= 'A' && value <= 'Z')) {
                oss << value;
            } else {
                oss << ".";
            }
        }
        oss << "\n";
    }
    std::cout << oss.str() << std::endl;
}


void Usage() {
    std::cout << "cgidump eth0 port cgi-name [dumpfile]" << std::endl;
}

bool CheckPort(char* port_str) {
    int port = atoi(port_str);
    return port > 0;
}

bool GetInterfaceIP(const char* interface, std::string* ip) {
    pcap_if_t* alldevs = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    bool result = false;
    int ret = pcap_findalldevs(&alldevs, errbuf);
    if (0 == ret) {
        for (pcap_if_t* p = alldevs; p != NULL; p = p->next) {
            if (strlen(interface) == strlen(p->name) && 0 == strcmp(p->name, interface)) {
                struct pcap_addr* addr = p->addresses;
                while (addr) {
                  struct sockaddr_in* address = (struct sockaddr_in*)(addr->addr);
                  unsigned long s_addr = ntohl(address->sin_addr.s_addr);
                  *ip = Utils::Long2IP(s_addr);
                  addr = addr->next;
                }
                result = true;
                break;
            }
        }
    } else {
      std::cout << "pcap_findalldevs failed." << std::endl;
    }
    pcap_freealldevs(alldevs);
    return result;
}

std::string PktPrint(const struct pcap_pkthdr* h, const uint8_t *bytes) {
  char date_time[64] = {0};
  struct tm t;
  strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", localtime_r(&h->ts.tv_sec, &t));

  std::ostringstream oss;
  oss << "ts[" << date_time << "." << h->ts.tv_usec / 1000 << "] caplen[" << h->caplen << "] len[" << h->len << "]";
  return oss.str();
}

std::string Long2IP(const uint32_t& ip) {
  std::ostringstream oss;
  oss << ((ip & 0xFF000000) >> 24) << "." << ((ip & 0x00FF0000) >> 16) << "." << ((ip & 0x0000FF00) >> 8) << "." << (ip & 0x000000FF);
  return oss.str();
}

std::string TcpFlags2String(const int& flags) {
  std::ostringstream oss;
  if (flags & TH_FIN) {
    oss << "FIN, ";
  }

  if (flags & TH_SYN) {
    oss << "SYN, ";
  }

  if (flags & TH_RST) {
    oss << "RST, ";
  }

  if (flags & TH_PUSH) {
    oss << "PUSH, ";
  }

  if (flags & TH_ACK) {
    oss << "ACK, ";
  }

  if (flags & TH_URG) {
    oss << "URG, ";
  }
  std::string str_flags = oss.str();
  if (str_flags.empty()) {
    return "";
  }
  return str_flags.substr(0, str_flags.length() - 2);
}

void PrintTcpHeader(const struct TcpHeader* tcp_header) {
  int port_src = ntohs(tcp_header->port_src);
  int port_dst = ntohs(tcp_header->port_dst);
  uint32_t sn = ntohl(tcp_header->sn);
  uint32_t ack = ntohl(tcp_header->ack);
  int hlen = TCP_HL(tcp_header);;
  int flags = TCP_FLAGS(tcp_header);;
  int win_size = ntohs(tcp_header->win_size);
  std::cout << "src_port[" << port_src << "] dst_port[" << port_dst << "] sn[" << sn << "] ack[" << ack << "] hlen[" << hlen << "] flags[" << TcpFlags2String(flags) << "] win_size[" << win_size << "]" << std::endl;
}

void PrintIpHeader(const struct IpHeader* ip_header) {
  int version = IP_V(ip_header);
  int ip_header_length = IP_HL(ip_header) * 4;
  int ip_len = ntohs(ip_header->ip_len);
  int ip_p = ip_header->ip_p; // 6
  std::string src = Long2IP(ntohl(ip_header->ip_src));
  std::string dst = Long2IP(ntohl(ip_header->ip_dst));
  std::cout << "version[" << version << "] ip_header_length[" << ip_header_length << "] total_length[" << ip_len << "] protocal[" << ip_p << "] src[" << src << "] dst[" << dst << "]" << std::endl;
}

void PrintEtherHeader(const struct EtherHeader* eth_hdr) {
  std::ostringstream oss;
  oss << "DST[";
  for (int i = 0; i < ETHER_ADDR_LEN; ++i) {
    int value = eth_hdr->ether_dhost[i];
    if (i < ETHER_ADDR_LEN - 1) {
      oss << std::setfill('0') << std::hex << std::setw(2) << value << ":";
    } else {
      oss << std::setfill('0') << std::hex << std::setw(2) << value << "] ";
    }
  }

  oss << "SRC[";
  for (int i = 0; i < ETHER_ADDR_LEN; ++i) {
    int value = eth_hdr->ether_shost[i];
    if (i < ETHER_ADDR_LEN - 1) {
      oss << std::setfill('0') << std::hex << std::setw(2) << value << ":";
    } else {
      oss << std::setfill('0') << std::hex << std::setw(2) << value << "] ";
    }
  }
  oss << "TYPE[0x" << std::setw(4) << std::setfill('0') << std::hex << htons(eth_hdr->ether_length_type) << "]\n";
  std::cout << oss.str();
}

bool ParseHttp(const uint8_t* data, const int& len, int* remain_pos) {
  if (0 == len) return false;

  int pos = 0;
  int i = 0;
  for (; i < len; ++i) {
    if ('\n' == data[i]) {
      if (i > 0 || '\r' == data[i - 1]) {
        if (i > 1) {
          // \r\n
          if (0 == i - pos - 1 && i > 3 && data[i - 2] == '\n' && data[i - 3] == '\r') {
            // \r\n\r\n
            break;
          }
          std::string head((const char*)data + pos, i - pos - 1);
          std::cout << head << std::endl;
        }
        pos = i + 1;
      }
    }
  }
  *remain_pos = i + 1;
  return true;
}

void PcapHandler(uint8_t *dumper, const struct pcap_pkthdr* h, const uint8_t* bytes) {
  if (NULL != dumper) {
    // 保存数据报文到文件中
    pcap_dump(dumper, h, bytes);
  }

  ParsePkg::GetInstance()->InputPkg(bytes, h->caplen);

  const uint8_t* p = bytes;
  const struct EtherHeader* ep = (const struct EtherHeader*)(p);
  p += ETHER_HDRLEN;
  uint16_t length_type = htons(ep->ether_length_type);
  bool is_vlan = false;
  if (ETHERTYPE_8021Q == length_type) {
    p += VLAN_LEN;
    is_vlan = true;
    const struct VlanHeader* vh = (const struct VlanHeader*)(p);
    uint16_t length_type = htons(vh->length_type);
    if (IPV4TYPE == length_type) {
      std::cout << "unsuport length type[" << length_type << "]" << std::endl;
      return;
    }
  } else if (IPV4TYPE == length_type) {
    // normal
  } else {
    std::cout << "unsuport length type[" << length_type << "]" << std::endl;
    return;
  }
  const struct IpHeader* ip_header = (const struct IpHeader*)(p);
  int version = IP_V(ip_header);
  int ip_header_length = IP_HL(ip_header) * 4;
  int ip_len = ntohs(ip_header->ip_len);
  int ip_p = ip_header->ip_p;
  if (IP_VERSION_4 != version || IP_HDRLEN != ip_header_length || IP_PROTOCAL_TCP != ip_p) {
    std::cout << "unsuport version[" << version << "] length[" << ip_header_length << "] protocol[" << ip_p << "]" << std::endl;
    PrintHex(bytes, 40);
    return;
  }
  p += IP_HDRLEN;
  const struct TcpHeader* tcp_header = (const struct TcpHeader*)(p);
  p += TCP_HL(tcp_header);
  int content_len = ip_len - IP_HDRLEN - TCP_HL(tcp_header);

  std::cout << PktPrint(h, bytes) << std::endl;
  PrintEtherHeader(ep);
  PrintIpHeader(ip_header);
  PrintTcpHeader(tcp_header);

  int next_pos = 0;
  ParseHttp(p, content_len, &next_pos);
  p += next_pos;

  PrintHex(p, content_len);
}

int main(int argc, char** argv) {
  if (argc < 4) {
    Usage();
    return -1;
  }

  std::string ip = "";
  if (!GetInterfaceIP(argv[1], &ip)) {
    std::cout << "does not find interface [" << argv[1] << "]" << std::endl;
    return -1;
  }

  if (!CheckPort(argv[2])) {
    std::cout << "port param [" << argv[2] << "] illegal" << std::endl;
    return -1;
  }

  std::string dev = argv[1];
  std::string port = argv[2];
  std::string cgi_name = argv[3];
  std::string dumpfile = "";
  if (argc >= 5) {
    dumpfile = argv[4];
  }

  std::cout << "creating a sniffing session with dev[" << dev << "]" << std::endl;

  struct bpf_program fp;
  bpf_u_int32 mask;
  bpf_u_int32 net;
  char ebuf[PCAP_ERRBUF_SIZE] = {0};
  pcap_t *pc;

  if (pcap_lookupnet(dev.c_str(), &net, &mask, ebuf) == -1) {
    std::cout << "can't get netmask for device " << dev << std::endl;
    net = 0;
    mask = 0;
  }

  pc = pcap_open_live(dev.c_str(), DEFAULT_SNAPLEN, 1, 1000, ebuf);
  if (NULL == pc) {
    std::cout << "couldn't open device " << dev << ": " << ebuf <<  std::endl;
    return -1;
  }

  char filter_exp[512] = {0};
  snprintf(filter_exp, sizeof(filter_exp), "port %s", port.c_str());
  if (pcap_compile(pc, &fp, filter_exp, 0, net) == -1) {
    std::cout << "couldn't parse filter[" << filter_exp << "]: " << ebuf << std::endl;
    return -1;
  }

  if (pcap_setfilter(pc, &fp) == -1) {
    std::cout << "couldn't install filter[" << filter_exp << "]: " << ebuf << std::endl;
    return -1;
  }

  pcap_dumper_t* dumper = NULL;
  if (!dumpfile.empty()) {
    dumper = pcap_dump_open(pc, dumpfile.c_str());
    if (NULL == dumper) {
      std::cout << "error open dumpfile[" << dumpfile << "]" << std::endl;
      return -1;
    }
  }

  pcap_loop(pc, -1, PcapHandler, (unsigned char*)dumper);

  pcap_close(pc);
  return 0;
}
