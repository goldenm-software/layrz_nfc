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
    if (context != 0) {
      SCardReleaseContext(context);
      context = 0;
    }

    if (!createNfcContext()) {
      Utils::Log("Failed to establish context.");
      nfc_ = nullptr;
      return;
    }

    char readers[2048];
    DWORD readersSize = sizeof(readers);
    auto status = SCardListReadersA(context, nullptr, readers, &readersSize);
    if (status == SCARD_S_SUCCESS && readersSize > 1) {
      strcpy_s(readerName_, readers);
      nfc_ = readerName_;

      Utils::Log("NFC reader found: " + std::string(nfc_));
      return;
    }
    
    Utils::Log("Failed to list NFC readers. Error: " + std::to_string(status));
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
      Utils::Log("Failed to establish context.");
      result(ErrorOr<bool>(false));
      return;
    }

    read_thread_ = std::thread([this]() {
      runNfcThread(context, nfc_);
    });

    result(ErrorOr<bool>(true));
  }

  void LayrzNfcPlugin::StopReading(std::function<void(ErrorOr<bool> reply)> result) {
    should_stop_ = true;

    if (read_thread_.joinable()) read_thread_.join();

    result(ErrorOr<bool>(true));
  }

  bool LayrzNfcPlugin::createNfcContext() {
    if (context != 0) { return true; }

    if (SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &context) != SCARD_S_SUCCESS) {
      Utils::Log("Failed to establish context.");
      return false;
    }

    if (context == 0) {
      Utils::Log("No NFC context found.");
      return false;
    }

    Utils::Log("NFC context established successfully.");
    return true;
  }

  void LayrzNfcPlugin::runNfcThread(SCARDCONTEXT cxt, const char* nfcReader) {
    if (nfcReader == nullptr) {
      Utils::Log("No NFC device found.");
      return;
    }

    std::cout << "Context: 0x" << std::hex << static_cast<uintptr_t>(cxt) << std::dec << std::endl;
    std::cout << "NFC device: " << nfcReader << std::endl;

    should_stop_ = false;
    while (!should_stop_) {
      SCARD_READERSTATEA state{};
      state.szReader = nfcReader;
      state.dwCurrentState = SCARD_STATE_UNAWARE;
  
      LONG status = SCardGetStatusChangeA(cxt, INFINITE, &state, 1);
      if (status != SCARD_S_SUCCESS) {
        Utils::Log("SCardGetStatusChangeA failed: " + std::to_string(status) + " (" + std::to_string(GetLastError()) + ")");
        Sleep(100);
        continue;
      }
      
      SCARDHANDLE card;
      DWORD protocol;

      auto res = SCardConnectA(
        cxt,
        nfcReader,
        SCARD_SHARE_SHARED,
        SCARD_PROTOCOL_T1,
        &card,
        &protocol
      );

      if (res != SCARD_S_SUCCESS) {
        // Utils::Log("Failed to connect to NFC device.");
        Sleep(100);
        continue;
      }

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
            Utils::Log("Timeout reading sector " + std::to_string(sector));
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
          callbackChannel->OnRead(data, SuccessCallback, ErrorCallback);
        });
      }/*  else {
        Utils::Log("Empty sector " + std::to_string(sector));
      } */

      SCardDisconnect(card, SCARD_UNPOWER_CARD);

      state.dwCurrentState = state.dwEventState;
      while (true) {
        status = SCardGetStatusChangeA(cxt, 1000, &state, 1);
        if (status == SCARD_S_SUCCESS && !(state.dwEventState & SCARD_STATE_PRESENT)) {
          break;
        }
      }
    }
  }
}  // namespace layrz_nfc
