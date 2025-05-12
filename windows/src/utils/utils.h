#pragma once

#ifndef __LAYRZ_NFC_UTILS_H__
#define __LAYRZ_NFC_UTILS_H__

#include <iostream>
#include <sstream>
#include <iomanip>

#include <Windows.h>
#include <winrt/Windows.Storage.Streams.h>

namespace layrz_nfc::Utils {
  /// @brief Logs a message to the console.
  /// @param format 
  /// @param ... The variable arguments to be formatted.
  /// @note This function is used to log messages to the console.
  void Log(const char* format, ...);

  /// @brief Converts a vector of bytes to a hex string.
  /// @param bytes The vector of bytes to be converted.
  /// @return The hex string representation of the bytes.
  std::stringstream BytesToHex(const std::vector<BYTE> &bytes);

  /// @brief Converts a vector of bytes to a hex string.
  /// @param data The pointer to the bytes to be converted.
  /// @param size The size of the bytes to be converted.
  /// @return The hex string representation of the bytes.
  std::stringstream BytesToHex(const BYTE* data, size_t size);
}

#endif // __LAYRZ_NFC_UTILS_H__