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

bool ParsePkg::InputPkg(const uint8_t* data, const int& len) {
  if (!m_inited) {
    std::cout << "ParsePkg has not initilized" << std::endl;
    return false;
  }

  const uint8_t* p = data;
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

  static int i = 0;
  std::cout << "[" << ++i << "]" << std::endl;

  // print package info
  PrintPkgInfo(eh, ih, th);

  // get client address
  std::string client_ip;
  int client_port;
  bool is_input = false;
  if (!GetClientAddr(ih, th, &client_ip, &client_port, &is_input)) {
    std::cout << "get client address failed"<< std::endl;
    return false;
  }

  int data_len = ntohs(ih->ip_len) - IP_HDRLEN - TCP_HL(th);;
  if (0 == data_len) {
    return true;
  }

  InputPkg2Session(client_ip, client_port, is_input, p, data_len);
  return true;
}

bool ParsePkg::GetClientAddr(const IpHeader* ih, const TcpHeader* th,
  std::string* client_ip, int* client_port, bool* input) {
  std::string src_ip = Utils::Long2IP(ntohl(ih->ip_src));
  std::string dst_ip = Utils::Long2IP(ntohl(ih->ip_dst));
  int src_port = ntohs(th->port_src);
  int dst_port = ntohs(th->port_dst);
  if (src_port == m_port && src_ip == m_ip) {
    *client_ip = dst_ip;
    *client_port = dst_port;
    *input = false;
  } else if (dst_port == m_port && dst_ip == m_ip) {
    *client_ip = src_ip;
    *client_port = src_port;
    *input = true;
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

bool ParsePkg::GetHttpHeaders(const uint8_t* data, const int& len,
  std::map<std::string, std::string>* headers, int* remain_pos) {
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
          std::vector<std::string> kv;
          Utils::SplitByDelimiter(head, ":", &kv);
          if (kv.size() != 2) {
            std::cout << "illegal header[" << head << "]" << std::endl;
          } else {
            (*headers)[Utils::TrimStr(kv[0])] = Utils::TrimStr(kv[1]);
          }
        }
        pos = i + 1;
      }
    }
  }
  *remain_pos = i + 1;
  return true;
}

bool ParsePkg::ParseHttpPkg(TcpBuf* buf) {
  uint8_t pre_read[1024] = {0};
  int32_t len = buf->PreOutputTcpPkg(pre_read, sizeof(pre_read) / sizeof(uint8_t));
  if (len <= 0) {
    return false;
  }

  std::map<std::string, std::string> headers;
  int remain_pos = 0;
  bool ret = GetHttpHeaders(pre_read, len, &headers, &remain_pos);
  if (!ret) {
    std::cout << "get http headers failed. client[" << buf->GetClientStr()
      << "]" << std::endl;
    return false;
  }

  for (auto it = headers.begin(); it != headers.end(); ++it) {
    std::cout << "KEY:" << it->first << " VALUE:" << it->second << std::endl;
  }

  return true;
}

bool ParsePkg::InputPkg2Session(const std::string& c_ip, const int& c_port,
  const bool& input, const uint8_t* data, const int& len) {
  std::ostringstream oss;
  oss << c_ip << ":" << c_port;
  std::string client = oss.str();
  Session* session = NULL;
  auto find_it = m_sessions.find(client);
  if (m_sessions.end() == find_it) {
    session = new Session(client);
    m_sessions[client] = session;
  } else {
    session = find_it->second;
  }

  // if (!IsHttpHeader(data, len) && 0 == buf->GetDataLen()) {
    // std::cout << "drop client[" << client << "] pkg. len[" << len << "]" << std::endl;
    // return false;
  // }

  bool ret = session->InputTcpPkg(data, len, input);
  if (!ret) {
    std::cout << "input tcp pkg to session failed. client[" << client << "] len["
      << len << "] [" << (input ? "input" : "output") << "]" << std::endl;
    return false;
  }

  // ParseHttpPkg(buf);

  return true;
}
