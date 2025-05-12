#include "parsers.h"

namespace layrz_nfc::Parsers {
  CardType GetCardType(const std::vector<BYTE>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
      if (data[i] == 0xA0 && data[i + 1] == 0x00 && data[i + 2] == 0x00) {
        if (i + 7 < data.size()) {
          BYTE typeByte = data[i + 7];
          switch (typeByte) {
            case 0x01:
              return CardType::MIFARE_MINI;
            case 0x02:
              return CardType::MIFARE_CLASSIC_1K;
            case 0x03:
              return CardType::MIFARE_CLASSIC_4K;
            default:
              return CardType::UNKNOWN;
          }
        }
      }
    }

    return CardType::UNKNOWN;
  }
}