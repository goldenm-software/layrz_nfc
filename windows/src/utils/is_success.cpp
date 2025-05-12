#include "utils.h"

namespace layrz_nfc::Utils {
  bool IsSuccess(IBuffer const& buffer) {
    if (buffer == nullptr || buffer.Length() < 2) return false;

    auto reader = DataReader::FromBuffer(buffer);
    reader.ByteOrder(ByteOrder::BigEndian);
    reader.UnicodeEncoding(UnicodeEncoding::Utf8);

    auto length = buffer.Length();

    std::vector<uint8_t> bytes(length);
    reader.ReadBytes(bytes);

    return bytes[length - 2] == 0x90 && bytes[length - 1] == 0x00;
  }

  std::vector<uint8_t> GetPayload(IBuffer const& buffer) {
    uint32_t len = buffer.Length();
    if (len < 2) return {};
    
    std::vector<uint8_t> bytes(len);
    winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer).ReadBytes(bytes);
    return std::vector<uint8_t>(bytes.begin(), bytes.end() - 2);
  }
}
