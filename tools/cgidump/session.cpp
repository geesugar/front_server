#include "session.h"
Session::Session(const std::string& client) {
  m_input = new HttpBuf(client, true);
  m_output = new HttpBuf(client, false);
}

Session::~Session() {
  delete m_input;
  m_input = NULL;
  delete m_output;
  m_output = NULL;
}


bool Session::InputTcpPkg(const uint8_t* data, const int& len, const bool& input) {
  if (input) {
    return m_input->InputTcpPkg(data, len);
  } else {
    return m_output->InputTcpPkg(data, len);
  }
}

