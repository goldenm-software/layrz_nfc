#include "layrz_nfc_plugin.h"

namespace layrz_nfc {
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
      Utils::Log("No NFC reader assigned.");
      result(ErrorOr<bool>(false));
      return;
    }

    StartReadingAsync();

    result(ErrorOr<bool>(true));
  }

  void LayrzNfcPlugin::StopReading(std::function<void(ErrorOr<bool> reply)> result) {
    if (reader_ != nullptr) {
      // reader_.CardAdded(nullptr);
      // reader_.CardRemoved(nullptr);
      reader_ = nullptr;
    }

    result(ErrorOr<bool>(true));
  }
}