#pragma once

#ifndef __LAYRZ_NFC_PARSERS_H__
#define __LAYRZ_NFC_PARSERS_H__

#include <iostream>
#include <sstream>
#include <iomanip>

#include <Windows.h>
#include <winrt/Windows.Storage.Streams.h>

namespace layrz_nfc::Parsers {
  // Define an enum for NFC card types
  enum class CardType {
    MIFARE_MINI,
    MIFARE_CLASSIC_1K,
    MIFARE_CLASSIC_4K,
    UNKNOWN
  };

  CardType GetCardType(const std::vector<BYTE>& data);
}

#endif // __LAYRZ_NFC_PARSERS_H__