#pragma once

#ifndef __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__
#define __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__

#ifndef NFC_READ_TIMEOUT
#define NFC_READ_TIMEOUT 2 // seconds
#endif // NFC_READ_TIMEOUT

#ifndef NFC_LOOP_SLEEP
#define NFC_LOOP_SLEEP 500 // milliseconds
#endif // NFC_LOOP_SLEEP

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Proximity.h>
#include <winrt/Windows.Devices.SmartCards.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Security.Cryptography.h>

#include "thread_handler.hpp"
#include "utils/utils.h"
#include "parsers/parsers.h"
#include "generated/layrz_nfc.g.h"

namespace layrz_nfc {
  using namespace winrt;
  using namespace winrt::Windows::Foundation;
  using namespace winrt::Windows::Foundation::Collections;
  using namespace winrt::Windows::Devices::SmartCards;
  using namespace winrt::Windows::Devices::Enumeration;
  using namespace winrt::Windows::Networking::Proximity;
  using namespace Windows::Storage::Streams;
  using namespace Windows::Security::Cryptography;

  using layrz_nfc::ErrorOr;
  using layrz_nfc::LayrzNfcPlatformChannel;
  using layrz_nfc::LayrzNfcCallbackChannel;

  
  class LayrzNfcPlugin : public flutter::Plugin, public LayrzNfcPlatformChannel {
    public:
      /// @brief This is the NFC callback channel used for communication with the Flutter side.
      /// @note This channel is used to send messages to the Flutter side.
      static std::unique_ptr<LayrzNfcCallbackChannel> callbackChannel;

      /// @brief This is the NFC device used for communication.
      /// @note This device can be null if no NFC device is found.
      DeviceInformation nfc_{nullptr};

      /// @brief This is the NFC reader used for communication.
      /// @note This reader can be null if is not scanning.
      SmartCardReader reader_{nullptr};

      /// @brief This is the UI thread handler used for posting messages to the UI thread.
      /// @note This handler is used to post messages to the UI thread from the background thread.
      LayrzNfcPluginUiThreadHandler uiThreadHandler_;

      /// @brief Registers the plugin with the Flutter engine.
      /// @param registrar The plugin registrar for the plugin.
      /// @note This function is called by the Flutter engine to register the plugin.
      static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

      /// @brief Constructs a new LayrzNfcPlugin instance.
      /// @param registrar The plugin registrar for the plugin.
      /// @note This constructor is called by the Flutter engine to create an instance of the plugin with the thread handler.
      LayrzNfcPlugin(flutter::PluginRegistrarWindows *registrar);

      /// @brief Destroys the LayrzNfcPlugin instance.
      /// @note This destructor is called by the Flutter engine to destroy the plugin instance.
      ~LayrzNfcPlugin();

      /// @brief Gets the proximity device asynchronously.
      /// @note This function is called to get the proximity device asynchronously.
      winrt::fire_and_forget GetScannersAsync();

      /// @brief Checks if the device has NFC capabilities.
      /// @param result The result callback to be called with the result.
      void CheckCapabilities(std::function<void(ErrorOr<bool> reply)> result);

      /// @brief Checks if the device can read NFC tags.
      /// @param result The result callback to be called with the result.
      void CanRead(std::function<void(ErrorOr<bool> reply)> result);

      /// @brief Checks if the device can write NFC tags.
      /// @param result The result callback to be called with the result.
      void CanWrite(std::function<void(ErrorOr<bool> reply)> result);

      /// @brief Checks if the device can simulate NFC tags.
      /// @param result The result callback to be called with the result.
      void CanSimulate(std::function<void(ErrorOr<bool> reply)> result);

      /// @brief Starts reading NFC tags.
      /// @param result The result callback to be called with the result.
      void StartReading(std::function<void(ErrorOr<bool> reply)> result);

      /// @brief Stops reading NFC tags.
      /// @param result The result callback to be called with the result.
      void StopReading(std::function<void(ErrorOr<bool> reply)> result);

    // Disallow copy and assign.
    LayrzNfcPlugin(const LayrzNfcPlugin&) = delete;
    LayrzNfcPlugin& operator=(const LayrzNfcPlugin&) = delete;

    private:
      static void SuccessCallback() {}
      static void ErrorCallback(const FlutterError &error) {
        // Ignore ChannelConnection Error, This might occur because of HotReload
        if (error.code() != "channel-error") {
          std::cout << "ErrorCode: " << error.code() << " Message: " << error.message() << std::endl;
        }
      }

      /// @brief This function is used to run the NFC reading procedure.
      winrt::fire_and_forget StartReadingAsync();

      /// @brief This function is used to read the Mifare Classic card asynchronously.
      /// @param conn The smart card connection to be used for reading.
      /// @param dataFromCard The data read from the card as a vector of bytes pointer.
      winrt::fire_and_forget ReadMifareClassicAsync(SmartCardConnection conn, std::vector<uint8_t> &dataFromCard);

      /// @brief This function is used to read the NFC Forum Type 2 card asynchronously.
      /// @param conn The smart card connection to be used for reading.
      /// @param dataFromCard The data read from the card as a vector of bytes pointer.
      winrt::fire_and_forget ReadForumType2Async(SmartCardConnection conn, std::vector<uint8_t> &dataFromCard);

      /// @brief This function is used to get the ATR (Answer to Reset) of the card.
      /// @param conn The smart card connection to be used for reading.
      /// @param atr The ATR of the card as a buffer.
      std::string GetAtrHex(IBuffer atr);

      /// @brief This function is used to detect the tag format of the card.
      /// @param atr The ATR of the card as a buffer.
      /// @return The tag format of the card as a TagFormat enum.
      TagFormat DetectTagFormat(IBuffer atr);
  };

}  // namespace layrz_nfc

#endif  // __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__
