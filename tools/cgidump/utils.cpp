#include "defines.h"
#include "utils.h"

Utils::Utils() {
}
Utils::~Utils() {
}

std::string Utils::TcpFlags2String(const int& flags) {
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

std::string Utils::Long2IP(const uint32_t& ip) {
    std::ostringstream oss;
    oss << ((ip & 0xFF000000) >> 24) << "." << ((ip & 0x00FF0000) >> 16) << "." << ((ip & 0x0000FF00) >> 8) << "." << (ip & 0x000000FF);
    return oss.str();
}
void Utils::PrintHex(const u_char* ctx, const int& size, std::ostream& os) {
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
    os << oss.str() << std::endl;
}
