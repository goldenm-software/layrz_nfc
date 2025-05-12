#include "layrz_nfc_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <chrono> // Include chrono for time measurement

namespace layrz_nfc {
  static std::unique_ptr<LayrzNfcCallbackChannel> callbackChannel;

  void LayrzNfcPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar) {
    auto plugin = std::make_unique<LayrzNfcPlugin>(registrar);
    LayrzNfcPlatformChannel::SetUp(registrar->messenger(), plugin.get());
    registrar->AddPlugin(std::move(plugin));

    callbackChannel = std::make_unique<LayrzNfcCallbackChannel>(registrar->messenger());
  }

  LayrzNfcPlugin::LayrzNfcPlugin(flutter::PluginRegistrarWindows *registrar) : uiThreadHandler_(registrar) {
    GetScannersAsync();
  }

  LayrzNfcPlugin::~LayrzNfcPlugin() {}

  void LayrzNfcPlugin::GetScannersAsync() {
    if (context_ != 0) {
      SCardReleaseContext(context_);
      context_ = 0;
    }

    if (!createNfcContext()) {
      Utils::Log("Failed to establish context_.");
      nfc_ = nullptr;
      return;
    }

    char readers[2048];
    DWORD readersSize = sizeof(readers);
    auto status = SCardListReadersA(context_, nullptr, readers, &readersSize);
    if (status == SCARD_S_SUCCESS && readersSize > 1) {
      strcpy_s(readerName_, readers);
      nfc_ = readerName_;

      Utils::Log("NFC reader found: %s", nfc_);
      return;
    }
    
    Utils::Log("Failed to list NFC readers. Error: %d", status);
    nfc_ = nullptr;
  }

  void LayrzNfcPlugin::CheckCapabilities(std::function<void(ErrorOr<bool> reply)> result) {
    if (nfc_ != nullptr) {
      result(ErrorOr<bool>(true));
      return;
    }

    Utils::Log("No NFC capabilities found.");
    result(ErrorOr<bool>(false));
  }

  void LayrzNfcPlugin::CanRead(std::function<void(ErrorOr<bool> reply)> result) {
    if (nfc_ != nullptr) {
      result(ErrorOr<bool>(true));
      return;
    }

    Utils::Log("Cannot read NFC tags.");
    result(ErrorOr<bool>(false));
  }
  
  void LayrzNfcPlugin::CanWrite(std::function<void(ErrorOr<bool> reply)> result) {
    if (nfc_ != nullptr) {
      result(ErrorOr<bool>(true));
      return;
    }

    Utils::Log("Cannot write NFC tags.");
    result(ErrorOr<bool>(false));
  }

  void LayrzNfcPlugin::CanSimulate(std::function<void(ErrorOr<bool> reply)> result) {
    if (nfc_ != nullptr) {
      result(ErrorOr<bool>(true));
      return;
    }

    Utils::Log("Cannot simulate NFC tags.");
    result(ErrorOr<bool>(false));
  }

  void LayrzNfcPlugin::BindScanners(std::function<void(std::optional<FlutterError> reply)> result) {
    GetScannersAsync();
    result(std::nullopt);
  }

  void LayrzNfcPlugin::StartReading(std::function<void(ErrorOr<bool> reply)> result) {
    if (nfc_ == nullptr) {
      Utils::Log("No NFC capabilities found.");
      result(ErrorOr<bool>(false));
      return;
    }

    if (!createNfcContext()) {
      Utils::Log("Failed to establish context_.");
      result(ErrorOr<bool>(false));
      return;
    }

    readThread_ = std::thread([this]() {
      runNfcThread(context_, nfc_);
    });

    result(ErrorOr<bool>(true));
  }

  void LayrzNfcPlugin::StopReading(std::function<void(ErrorOr<bool> reply)> result) {
    shouldStop_ = true;

    if (context_ != 0) {
      SCardCancel(context_);
    }

    if (readThread_.joinable()) readThread_.join();

    result(ErrorOr<bool>(true));
  }

  bool LayrzNfcPlugin::createNfcContext() {
    if (context_ != 0) { return true; }

    if (SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &context_) != SCARD_S_SUCCESS) {
      Utils::Log("Failed to establish context_.");
      return false;
    }

    if (context_ == 0) {
      Utils::Log("No NFC context_ found.");
      return false;
    }

    Utils::Log("NFC context_ established successfully.");
    return true;
  }

  void LayrzNfcPlugin::runNfcThread(SCARDCONTEXT cxt, const char* nfcReader) {
    if (nfcReader == nullptr) {
      Utils::Log("No NFC reader setted.");
      return;
    }

    // std::cout << "Context: 0x" << std::hex << static_cast<uintptr_t>(cxt) << std::dec << std::endl;
    // std::cout << "NFC device: " << nfcReader << std::endl;

    shouldStop_ = false;
    while (!shouldStop_) {
      try {
        SCARD_READERSTATEA state{};
        state.szReader = nfcReader;
        state.dwCurrentState = SCARD_STATE_UNAWARE;
    
        LONG status = SCardGetStatusChangeA(cxt, 500, &state, 1);
        Utils::Log("Status: %s", getStatusOfReaderState(state).c_str());
  
        if (status != SCARD_S_SUCCESS) {
          // Utils::Log("SCardGetStatusChangeA failed: " + std::to_string(status) + " (" + std::to_string(GetLastError()) + ")");
          Sleep(NFC_LOOP_SLEEP);
          continue;
        }
  
        if (state.dwEventState & SCARD_STATE_EMPTY) {
          // Utils::Log("NFC device removed or not present.");
          Sleep(NFC_LOOP_SLEEP);
          continue;
        }
  
        Utils::Log("NFC device detected, reading it.");
        
        SCARDHANDLE card;
        DWORD protocol;
  
        auto res = SCardConnectA(
          cxt,
          nfcReader,
          SCARD_SHARE_SHARED,
          SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
          &card,
          &protocol
        );
  
        if (res != SCARD_S_SUCCESS) {
          Utils::Log("Failed to connect to NFC device.");
          Sleep(NFC_LOOP_SLEEP);
          SCardDisconnect(card, SCARD_UNPOWER_CARD);
          continue;
        }

        BYTE atr[32];
        char readerName[256];
        DWORD atrLen = sizeof(atr);
        DWORD readerLen = sizeof(readerName);
        DWORD proto, pdwState;

        LONG cardStatus = SCardStatusA(
          card,           // handle from SCardConnectA
          readerName,
          &readerLen,
          &pdwState,
          &proto,
          atr,
          &atrLen
        );

        if (cardStatus != SCARD_S_SUCCESS) {
          std::cerr << "SCardStatus failed\n";
          SCardDisconnect(card, SCARD_UNPOWER_CARD);
          Sleep(NFC_LOOP_SLEEP);
          continue;
        }

        // Print ATR
        Utils::Log("ATR: %s (%d bytes)",  Utils::BytesToHex(atr, atrLen).str().c_str(), atrLen);

        auto type = Parsers::GetCardType(std::vector<BYTE>(atr, atr + atrLen));
        switch (type) {
          case Parsers::CardType::MIFARE_MINI:
            Utils::Log("Mifare Mini card detected.");
            break;
          case Parsers::CardType::MIFARE_CLASSIC_1K:
            Utils::Log("Mifare Classic 1K card detected.");
            break;
          case Parsers::CardType::MIFARE_CLASSIC_4K:
            Utils::Log("Mifare Classic 4K card detected.");
            break;
          case Parsers::CardType::UNKNOWN:
            Utils::Log("Unknown card type detected.");
            break;
        }

        readMifareMiniCard(card, protocol);

        SCardDisconnect(card, SCARD_UNPOWER_CARD);
        state.dwCurrentState = state.dwEventState;
        while (true) {
          status = SCardGetStatusChangeA(cxt, 1000, &state, 1);
          if (status == SCARD_E_CANCELLED) {
            Utils::Log("Thread cancelled.");
            break;
          }
  
          if (state.dwEventState & SCARD_STATE_EMPTY) {
            Utils::Log("NFC device removed.");
            break;
          }
        }
      } catch (const std::exception& e) {
        Utils::Log("Exception: %s", std::string(e.what()));
      } catch (...) {
        Utils::Log("Unknown exception occurred.");
      }
    }
  }

  void LayrzNfcPlugin::readMifareMiniCard(SCARDHANDLE card, DWORD protocol) {
    Utils::Log("Reading Mifare Mini card...");
    BYTE loadKey[] = {
      0xFF, 0x82, 0x00, 0x00, 0x06,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    Utils::Log("Load key command: %s (%d bytes)", Utils::BytesToHex(loadKey, sizeof(loadKey)).str().c_str(), sizeof(loadKey));
    auto loadKeyResult = send(card, loadKey, sizeof(loadKey), protocol);
    Utils::Log("Load key result: %s (%d bytes)", Utils::BytesToHex(loadKeyResult).str().c_str(), loadKeyResult.size());

    for (int block = 4; block < 20; ++block) {
      Utils::Log("Reading block %d", block);
      // Skip trailer blocks (3, 7, 11, 15, 19)
      if ((block + 1) % 4 == 0) {
          Utils::Log("Block %02d: [skipped - trailer]", block);
          continue;
      }
      try {
        auto authCmd = buildAuthAPDUCommand(static_cast<BYTE>(block));
        Utils::Log("Auth command: %s (%d bytes)", Utils::BytesToHex(authCmd).str().c_str(), authCmd.size());
        auto authResult = send(card, authCmd.data(), static_cast<DWORD>(authCmd.size()), protocol);
        Utils::Log("Auth result: %s (%d bytes)", Utils::BytesToHex(authResult).str().c_str(), authResult.size());
        if (authResult.size() < 2 || authResult[authResult.size() - 2] != 0x90) {
          Utils::Log("Failed to authenticate block %d", block);
          return;
        }
        
        auto readCmd = buildReadAPDUCommand(static_cast<BYTE>(block));
        Utils::Log("Read command: %s (%d bytes)", Utils::BytesToHex(readCmd).str().c_str(), readCmd.size());
        auto readResult = send(card, readCmd.data(), static_cast<DWORD>(readCmd.size()), protocol);
        Utils::Log("Read result: %s (%d bytes)", Utils::BytesToHex(readResult).str().c_str(), readResult.size());
        if (readResult.size() < 18 || readResult[readResult.size() - 2] != 0x90) {
          Utils::Log("Failed to read block %d", block);
          return;
        }

        readResult.resize(readResult.size() - 2); // Remove status bytes
        Utils::Log("Block %d data: %s (%d bytes)", block, Utils::BytesToHex(readResult).str().c_str(), readResult.size());
      } catch (const std::exception& e) {
        Utils::Log("Exception: %s", std::string(e.what()).c_str());
      } catch (...) {
        Utils::Log("Unknown exception occurred.");
      }
    }
  }

  void LayrzNfcPlugin::readMifareClassicCard(SCARDHANDLE card) {
    Utils::Log("Reading Mifare Classic card...");
    auto send = [&](SCARDHANDLE card, BYTE* cmd, DWORD len, int sector) -> std::optional<std::vector<uint8_t>> {
      BYTE resp[256];
      DWORD respLen = sizeof(resp);
      LONG res = SCardTransmit(card, SCARD_PCI_T1, cmd, len, nullptr, resp, &respLen);

      if (res == SCARD_S_SUCCESS && respLen >= 2 && resp[respLen - 2] == 0x90 && resp[respLen - 1] == 0x00) {
        std::vector<uint8_t> blockData(resp, resp + respLen - 2);
        return blockData;
      }
      // Utils::Log("Failed to send command: " + std::to_string(res) + " (" + std::to_string(GetLastError()) + ")");
      return std::nullopt;
    };

    auto authenticate = [&](SCARDHANDLE card, int block) {
      BYTE auth[] = {
        0xFF, 0x86, 0x00, 0x00, 0x05,
        0x01,              // Number of structures
        0x00,              // Reserved
        static_cast<BYTE>(block),  // Block number
        0x60,              // 0x60 = Key A
        0x00               // Key slot 0
      };

      BYTE resp[256];
      DWORD respLen = sizeof(resp);

      LONG res = SCardTransmit(card, SCARD_PCI_T1, auth, sizeof(auth), nullptr, resp, &respLen);
      return res == SCARD_S_SUCCESS &&
            respLen >= 2 &&
            resp[respLen - 2] == 0x90 &&
            resp[respLen - 1] == 0x00;
    };

    std::vector<uint8_t> data;
    for (int sector = 0; sector < 16; ++sector) {
      int baseBlock = sector * 4;
      
      auto start = std::chrono::steady_clock::now();
      if (!authenticate(card, baseBlock)) { continue; }
      
      // Utils::Log("Reading sector " + std::to_string(sector));
      
      bool timeout = false;
      for (int i = 0; i < 4; ++i) {
        if (std::chrono::steady_clock::now() - start > std::chrono::seconds(2)) {
          Utils::Log("Timeout reading sector %d", sector);
          timeout = true;
          break;
        }

        BYTE read[] = { 0xFF, 0xB0, 0x00, static_cast<BYTE>(baseBlock + i), 0x10 };
        auto result = send(card, read, sizeof(read), sector);
        if (result.has_value()) {
          const auto& blockData = result.value();
          data.insert(data.end(), blockData.begin(), blockData.end());
        }

        if (timeout) continue;
      }
    }
    if (!data.empty()) {
      // Utils::Log("Sector " + std::to_string(sector) + " data: " + std::to_string(sectorData.size()) + " bytes");
      uiThreadHandler_.Post([this, data]() {
        TagPayload payload(data, TagFormat::kMifareClassic);
        callbackChannel->OnRead(payload, SuccessCallback, ErrorCallback);
      });
    } else {
      Utils::Log("Empty data.");
    }
  }

  void LayrzNfcPlugin::readNfcForumType2Card(SCARDHANDLE card) {
    Utils::Log("Reading NFC Forum Type 2 card...");
    auto send = [&](SCARDHANDLE card, BYTE* cmd, DWORD len, int sector) -> std::optional<std::vector<uint8_t>> {
      BYTE resp[256];
      DWORD respLen = sizeof(resp);
      LONG res = SCardTransmit(card, SCARD_PCI_T1, cmd, len, nullptr, resp, &respLen);

      if (res == SCARD_S_SUCCESS && respLen >= 2 && resp[respLen - 2] == 0x90 && resp[respLen - 1] == 0x00) {
        std::vector<uint8_t> blockData(resp, resp + respLen - 2);
        return blockData;
      }
      // Utils::Log("Failed to send command: " + std::to_string(res) + " (" + std::to_string(GetLastError()) + ")");
      return std::nullopt;
    };

    BYTE readCCCmd[] = {0xFF, 0xB0, 0x00, 0x03, 0x04}; // Read 4 bytes from page 3
    auto ccResult = send(card, readCCCmd, sizeof(readCCCmd), 3);
    if (!ccResult.has_value()) {
      Utils::Log("Failed to read Capability Container (CC) from page 3.");
      return;
    }

    const auto& ccData = ccResult.value();
    if (ccData.size() < 4) {
      Utils::Log("Invalid CC data.");
      return;
    }

    std::ostringstream ccStream;
    ccStream << "";
    for (const auto& byte : ccData) {
      ccStream << std::hex << std::uppercase << static_cast<int>(byte) << " ";
    }
    Utils::Log("Output CC: %s", ccStream.str().c_str());

    // Calculate the total number of pages
    int totalMemoryBytes = ccData[2] * 8; // Memory size in bytes (from CC byte 2)
    int totalPages = totalMemoryBytes / 4; // Each page is 4 bytes
    Utils::Log("Total memory: %d bytes, Total pages: %d", totalMemoryBytes, totalPages);

    std::vector<uint8_t> data;

    // Start measuring time
    auto startTime = std::chrono::steady_clock::now();

    for (int page = 0; page < totalPages && !shouldStop_; ++page) {
      // Utils::Log("Reading page " + std::to_string(page));

      // Check if 2 seconds have passed
      auto currentTime = std::chrono::steady_clock::now();
      auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
      if (elapsedTime.count() >= NFC_READ_TIMEOUT) {
        Utils::Log("Maximum reading time of 2 seconds reached.");
        break;
      }

      // 0xFF, 0xB0 => PC/SC "READ BINARY" for 4 bytes of a Type 2 block
      BYTE readCmd[] = {0xFF, 0xB0, 0x00, static_cast<BYTE>(page), 0x04};
      auto result = send(card, readCmd, sizeof(readCmd), page);
      if (!result.has_value()) {
        Utils::Log("Failed to read page %d", page);
        continue;
      }

      const auto& blockData = result.value();
      data.insert(data.end(), blockData.begin(), blockData.end());
    }

    // After reading all pages, if we got anything:
    if (!data.empty()) {
      Utils::Log("Sending %d bytes", data.size());
      uiThreadHandler_.Post([this, data]() {
        TagPayload payload(data, TagFormat::kNfcForumType2);
        callbackChannel->OnRead(payload, SuccessCallback, ErrorCallback);
      });
    } else {
      Utils::Log("Empty data.");
    }
  }

  std::vector<BYTE> LayrzNfcPlugin::send(SCARDHANDLE hCard, BYTE* cmd, DWORD len, DWORD protocol) {
    std::vector<BYTE> resp(256);
    DWORD respLen = static_cast<DWORD>(resp.size());
    const SCARD_IO_REQUEST* pioSendPci = (protocol == SCARD_PROTOCOL_T0) ? SCARD_PCI_T0 : SCARD_PCI_T1;

    LONG res = SCardTransmit(hCard, pioSendPci, cmd, len, nullptr, resp.data(), &respLen);
    if (res != SCARD_S_SUCCESS)
        throw std::runtime_error("Transmit failed");

    resp.resize(respLen);
    return resp;
  }

  std::string LayrzNfcPlugin::getStatusOfReaderState(SCARD_READERSTATEA state) {
    std::string status = "Status: ";
    if (state.dwEventState & SCARD_STATE_PRESENT) {
      status += "Present";
    } else if (state.dwEventState & SCARD_STATE_EMPTY) {
      status += "Empty";
    } else {
      status += "Unknown";
    }

    status += " (Code: " + std::to_string(state.dwEventState) + ")";
    return status;
  }

  std::vector<BYTE> LayrzNfcPlugin::buildAuthAPDUCommand(BYTE block) {
    return {0xFF, 0x86, 0x00, 0x00, 0x05, 0x01, 0x00, block, 0x60, 0x00};
  }

  std::vector<BYTE> LayrzNfcPlugin::buildReadAPDUCommand(BYTE block) {
    return {0xFF, 0xB0, 0x00, block, 0x10};
  }
}  // namespace layrz_nfc
