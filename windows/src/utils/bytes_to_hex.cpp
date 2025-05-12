#include "utils.h"

namespace layrz_nfc::Utils {
  std::stringstream BytesToHex(const std::vector<BYTE> &bytes) {
    std::stringstream ss;
    ss << std::hex << std::uppercase;
    for (const auto &byte : bytes) {
      ss << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    return ss;
  }

  std::stringstream BytesToHex(const BYTE* data, size_t size) {
    std::stringstream ss;
    ss << std::hex << std::uppercase;
    for (size_t i = 0; i < size; ++i) {
      ss << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
    }
    return ss;
  }
} // namespace layrz_nfc::Utils