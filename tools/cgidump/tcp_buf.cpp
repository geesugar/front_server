#include "tcp_buf.h"
#include <iostream>

TcpBuf::TcpBuf(const std::string& client) {
  m_buf = new uint8_t[MAX_BUF_SIZE];
  memset(m_buf, 0, MAX_BUF_SIZE);
  m_start = 0;
  m_end = 0;
  m_client = client;
}

TcpBuf::~TcpBuf() {
  delete[] m_buf;
}

bool TcpBuf::InputTcpPkg(const uint8_t* data, const int& len) {
  if (len > RemainSize()) {
    std::cout << "client[" << m_client << "]'s buffer not enough. remain["
      << RemainSize() << "] len[" << len << "]" << std::endl;
    return false;
  }

  if (m_end + len >= MAX_BUF_SIZE) {
    int first_len =  MAX_BUF_SIZE - m_end;
    int second_len = len - first_len;
    memcpy(m_buf + m_end, data, first_len);
    if (0 != second_len) {
      memcpy(m_buf, data + first_len, second_len);
    }
    m_end = second_len;
  } else {
    memcpy(m_buf + m_end, data, len);
    m_end += len;
  }
  m_buf[m_end] = '\0';
  return true;
}

int32_t TcpBuf::PreOutputTcpPkg(uint8_t* data, int len) {
  len = len  > GetDataLen() ? GetDataLen() : len;
  if (len + m_start >= MAX_BUF_SIZE) {
    int first_len =  MAX_BUF_SIZE - m_start;
    int second_len = len - first_len;
    memcpy(data, m_buf + m_start, first_len);
    if (0 != second_len) {
      memcpy(data + first_len, m_buf, second_len);
    }
  } else {
    memcpy(data, m_buf + m_start, len);
  }
  return len;
}

std::string TcpBuf::GetClientStr() {
  return m_client;
}

int32_t TcpBuf::OutputTcpPkg(uint8_t* data, int len) {
  len = len  > GetDataLen() ? GetDataLen() : len;
  if (len + m_start >= MAX_BUF_SIZE) {
    int first_len =  MAX_BUF_SIZE - m_start;
    int second_len = len - first_len;
    memcpy(data, m_buf + m_start, first_len);
    if (0 != second_len) {
      memcpy(data + first_len, m_buf, second_len);
    }
    m_start = second_len;
  } else {
    memcpy(data, m_buf + m_start, len);
    m_start += len;
  }
  return len;
}

int32_t TcpBuf::GetDataLen() {
  if (m_end == m_start) {
    return 0;
  } else if (m_end > m_start) {
    return m_end - m_start;
  } else {
    return MAX_BUF_SIZE - m_start + m_end;
  }
}

int32_t TcpBuf::RemainSize() {
  return MAX_BUF_SIZE - GetDataLen()  - 2;
}
