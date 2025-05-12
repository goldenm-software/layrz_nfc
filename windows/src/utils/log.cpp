#include "utils.h"

namespace layrz_nfc::Utils {
  void Log(const char* format, ...) {
    // Append to the 
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    std::cout << "LayrzNfcPlugin/Windows: " << buffer << std::endl; 
  }
} // namespace layrz_nfc::Utils