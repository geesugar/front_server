#include <arpa/inet.h>
#include <string>

#include "parse_pkg.h"
#include "utils.h"

ParsePkg* ParsePkg::GetInstance() {
  static ParsePkg parse_pkg;
  return &parse_pkg;
}

ParsePkg::ParsePkg() : m_inited(false) {
}

ParsePkg::~ParsePkg() {
}

void ParsePkg::Init(const std::string& ip, const int& port) {
  m_ip = ip;
  m_port = port;
  m_inited = true;
}

bool ParsePkg::InputPkg(const u_char* data, const int& len) {
  if (!m_inited) {
    std::cout << "ParsePkg has not initilized" << std::endl;
    return false;
  }

  const u_char* p = data;
  const struct EtherHeader* eh = (const struct EtherHeader*)(p);
  p += ETHER_HDRLEN;
  uint16_t length_type = ntohs(eh->ether_length_type);

  // vlan
  bool is_vlan = false;
  if (ETHERTYPE_8021Q == length_type) {
    is_vlan = true;
    const struct VlanHeader* vh = (const struct VlanHeader*)(p);
    p += VLAN_LEN;
    uint16_t length_type = ntohs(vh->length_type);
    if (IPV4TYPE != length_type) {
      std::cout << "unsupport length_type[0x" << std::setfill('0') << std::hex
        << std::setw (2) << length_type << "]" << std::endl;
      return false;
    }
  } else if (IPV4TYPE != length_type) {
    std::cout << "unsupport length_type[0x" << std::setfill('0') << std::hex
      << std::setw (2) << length_type << "]" << std::endl;
    return false;
  }
  const struct IpHeader* ih = (const struct IpHeader*)(p);
  if (IP_VERSION_4 != IP_V(ih) || IP_HDRLEN != IP_HL(ih) * 4 || IP_PROTOCAL_TCP != ih->ip_p) {
    std::cout << "unsupprot version[" << IP_V(ih) << "] length[" << IP_HL(ih)
      << "] protocol[" << ih->ip_p << "]" << std::endl;
    return false;
  }
  p += IP_HDRLEN;
  const struct TcpHeader* th = (const struct TcpHeader*)(p);
  p += TCP_HL(th);

  // print package info
  PrintPkgInfo(eh, ih, th);

  // get client address
  std::string client_ip;
  int client_port;
  if (!GetClientAddr(ih, th, &client_ip, &client_port)) {
    std::cout << "get client address failed"<< std::endl;
    return false;
  }
  std::cout << "client[" << client_ip << ":" << client_port << "]"<< std::endl;

  int data_len = ntohs(ih->ip_len) - IP_HDRLEN - TCP_HL(th);;
  if (0 == data_len) {
    return true;
  }

  // int remain_pos = 0;
  // bool ret = ParseHttp(p, data_len, &remain_pos);
  // if (!ret) {
  // std::cerr << "parse http failed" << std::endl;
  // return false;
  // }
  // Utils::PrintHex(p, data_len);
  return true;
}

bool ParsePkg::GetClientAddr(const IpHeader* ih, const TcpHeader* th,
  std::string* client_ip, int* client_port) {
  std::string src_ip = Utils::Long2IP(ntohl(ih->ip_src));
  std::string dst_ip = Utils::Long2IP(ntohl(ih->ip_dst));
  int src_port = ntohs(th->port_src);
  int dst_port = ntohs(th->port_dst);
  if (src_port == m_port && src_ip == m_ip) {
    *client_ip = dst_ip;
    *client_port = dst_port;
  } else if (dst_port == m_port && dst_ip == m_ip) {
    *client_ip = src_ip;
    *client_port = src_port;
  } else {
    return false;
  }
  return true;
}

void ParsePkg::PrintPkgInfo(const EtherHeader* eh, const IpHeader* ih, const TcpHeader* th) {
  std::ostringstream oss;
  oss << std::uppercase;
  for (int i = 0; i < ETHER_ADDR_LEN; ++i) {
    int value = eh->ether_dhost[i];
    if (i < ETHER_ADDR_LEN - 1) {
      oss << std::setfill('0') << std::hex << std::setw(2) << value << ":";
    } else {
      oss << std::setfill('0') << std::hex << std::setw(2) << value;
    }
  }

  std::string dst_mac = oss.str();
  oss.str("");

  for (int i = 0; i < ETHER_ADDR_LEN; ++i) {
    int value = eh->ether_shost[i];
    if (i < ETHER_ADDR_LEN - 1) {
      oss << std::setfill('0') << std::hex << std::setw(2) << value << ":";
    } else {
      oss << std::setfill('0') << std::hex << std::setw(2) << value;
    }
  }


  std::string src_mac = oss.str();
  oss.str("");

  std::string src_ip = Utils::Long2IP(ntohl(ih->ip_src));
  std::string dst_ip = Utils::Long2IP(ntohl(ih->ip_dst));

  int src_port = ntohs(th->port_src);
  int dst_port = ntohs(th->port_dst);
  std::string flags = Utils::TcpFlags2String(TCP_FLAGS(th));

  int data_len = ntohs(ih->ip_len) - IP_HDRLEN - TCP_HL(th);;

  std::cout << src_ip << ":" << src_port << " -> " << dst_ip << ":" << dst_port << " [" << flags << "] ";
  if (data_len > 0) {
    std::cout << data_len;
  }
  std::cout << std::endl;
}

bool ParsePkg::ParseHttp(const u_char* data, const int& len, int* remain_pos) {
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
