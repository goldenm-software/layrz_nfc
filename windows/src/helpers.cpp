#include "layrz_nfc_plugin.h"

namespace layrz_nfc {
  winrt::fire_and_forget LayrzNfcPlugin::GetScannersAsync() {
    auto selector = SmartCardReader::GetDeviceSelector();
    IVectorView<DeviceInformation> devices = co_await DeviceInformation::FindAllAsync(selector);
    if (devices.Size() == 0) {
      Utils::Log("No NFC devices found.");
      nfc_ = nullptr;
      co_return;
    }

    auto firstDevice = devices.GetAt(0);
    auto name = winrt::to_string(firstDevice.Name());
    auto id = winrt::to_string(firstDevice.Id());
    Utils::Log("Selecting %s#%s device", name.c_str(), id.c_str());

    nfc_ = firstDevice;
    co_return;
  }

  winrt::fire_and_forget LayrzNfcPlugin::StartReadingAsync() {
    Utils::Log("Starting NFC reader...");
    reader_ = co_await SmartCardReader::FromIdAsync(nfc_.Id());

    if (reader_ == nullptr) {
      Utils::Log("No NFC reader found.");
      co_return;
    }
    
    reader_.CardRemoved([this](SmartCardReader const&, CardRemovedEventArgs const& args) {
      Utils::Log("Card removed...");
    });

    reader_.CardAdded([this](SmartCardReader const&, CardAddedEventArgs const& args) -> winrt::fire_and_forget {
      Utils::Log("Card added...");
      SmartCard card = args.SmartCard();

      Utils::Log("Connecting card...");
      SmartCardConnection conn = co_await card.ConnectAsync();
      if (conn == nullptr) {
        Utils::Log("Failed to connect to NFC card.");
        co_return;
      }

      Utils::Log("Connected to NFC card!");

      auto atr = co_await card.GetAnswerToResetAsync();
      if (atr == nullptr) {
        Utils::Log("Failed to get ATR.");
        conn.Close();
        co_return;
      }

      auto hex = CryptographicBuffer::EncodeToHexString(atr);
      std::string atrHex = winrt::to_string(hex);
      // Utils::Log("ATR: %s", atrHex.c_str());

      std::vector<uint8_t> dataFromCard;

      auto format = DetectTagFormat(atr);
      if (format == TagFormat::kMifareClassic) {
        ReadMifareClassicAsync(conn, dataFromCard);
      } else if (format == TagFormat::kNfcForumType2) {
        ReadForumType2Async(conn, dataFromCard);
      } else {
        Utils::Log("Unsupported tag format.");
      }

      
      if (dataFromCard.size() > 0) {
        // std::string ascii(dataFromCard.begin(), dataFromCard.end());
        // ascii.erase(std::remove(ascii.begin(), ascii.end(), '\0'), ascii.end());
        // Utils::Log("Data from card: %s", ascii.c_str());

        uiThreadHandler_.Post([this, dataFromCard, format]() {
          TagPayload obj(dataFromCard, format);

          callbackChannel->OnRead(obj, SuccessCallback, ErrorCallback);
        });
      }

      conn.Close();
      Utils::Log("Closed connection to NFC card.");

      co_return;
    });

    co_return;
  }

  winrt::fire_and_forget LayrzNfcPlugin::ReadMifareClassicAsync(SmartCardConnection conn, std::vector<uint8_t> &dataFromCard) {
    IBuffer loadKey = CryptographicBuffer::DecodeFromHexString(L"FF82000006FFFFFFFFFFFF");
    auto loadResponse = co_await conn.TransmitAsync(loadKey);

    winrt::hstring hex = CryptographicBuffer::EncodeToHexString(loadResponse);
    std::string loadResponseHex = winrt::to_string(hex);
    // Utils::Log("Load response: %s", loadResponseHex.c_str());

    if (!Utils::IsSuccess(loadResponse)) {
      Utils::Log("Failed to load key.");
      co_return;
    }

    for (int block = 0; block < 64; ++block) {
      if ((block + 1) % 4 == 0) continue;

      std::stringstream ss;
      ss << "FF860000050100" << std::setfill('0') << std::setw(2) << std::hex << block << "6100";
      auto authKey = CryptographicBuffer::DecodeFromHexString(winrt::to_hstring(ss.str()));
      auto authResponse = co_await conn.TransmitAsync(authKey);
      if (!Utils::IsSuccess(authResponse)) {
        Utils::Log("Failed to authenticate block %d.", block);
        continue;
      }

      std::stringstream rs;
      rs << "FFB000" << std::setfill('0') << std::setw(2) << std::hex << block << "10";
      auto readKey = CryptographicBuffer::DecodeFromHexString(winrt::to_hstring(rs.str()));
      auto readResponse = co_await conn.TransmitAsync(readKey);
      if (!Utils::IsSuccess(readResponse)) {
        Utils::Log("Failed to read block %d.", block);
        continue;
      }

      std::vector<uint8_t> payload = Utils::GetPayload(readResponse);
      if (!payload.empty()) {
        dataFromCard.insert(dataFromCard.end(), payload.begin(), payload.end());
      }
    }
  }

  winrt::fire_and_forget LayrzNfcPlugin::ReadForumType2Async(SmartCardConnection conn, std::vector<uint8_t> &dataFromCard) {
    for (int page = 4; page < 36; page += 4) { // read 4 pages (16 bytes) per APDU
      std::stringstream ss;
      ss << "FFB000" << std::setfill('0') << std::setw(2) << std::hex << page << "10";

      auto readApdu = CryptographicBuffer::DecodeFromHexString(winrt::to_hstring(ss.str()));
      auto response = co_await conn.TransmitAsync(readApdu);

      if (!Utils::IsSuccess(response)) {
        Utils::Log("Failed to read page %d (Forum Type 2).", page);
        continue;
      }

      std::vector<uint8_t> payload = Utils::GetPayload(response);
      if (!payload.empty()) {
        dataFromCard.insert(dataFromCard.end(), payload.begin(), payload.end());
      }
    }

    co_return;
  }

  std::string LayrzNfcPlugin::GetAtrHex(IBuffer atr) {
    return winrt::to_string(CryptographicBuffer::EncodeToHexString(atr));
  }

  TagFormat LayrzNfcPlugin::DetectTagFormat(IBuffer atr) {
    auto hex = GetAtrHex(atr);
    if (hex.find("030001") != std::string::npos) {
      return TagFormat::kMifareClassic;
    } else if (hex.find("030003") != std::string::npos) {
      return TagFormat::kNfcForumType2;
    }
    return TagFormat::kUnknown;
  }
}