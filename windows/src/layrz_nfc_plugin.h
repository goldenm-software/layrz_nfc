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
#include <winrt/Windows.Networking.Proximity.h>
#include <winrt/Windows.Storage.Streams.h>

#include <winscard.h>

#include "thread_handler.hpp"
#include "utils/utils.h"
#include "parsers/parsers.h"
#include "generated/layrz_nfc.g.h"

namespace layrz_nfc {
  using namespace winrt;
  using namespace winrt::Windows::Foundation;
  using namespace winrt::Windows::Networking::Proximity;
  using namespace Windows::Storage::Streams;

  using layrz_nfc::ErrorOr;
  using layrz_nfc::LayrzNfcPlatformChannel;
  using layrz_nfc::LayrzNfcCallbackChannel;

  
  class LayrzNfcPlugin : public flutter::Plugin, public LayrzNfcPlatformChannel {
    public:
      /// @brief This is the NFC device used for communication.
      /// @note This device can be null if no NFC device is found.
      const char* nfc_ = nullptr;

      /// @brief This is the NFC reader name used for communication.
      char readerName_[256] = {};

      /// @brief This is the UI thread handler used for posting messages to the UI thread.
      /// @note This handler is used to post messages to the UI thread from the background thread.
      LayrzNfcPluginUiThreadHandler uiThreadHandler_;

      /// @brief This is the context used for establishing a connection to the NFC device.
      SCARDCONTEXT context_ = 0;

      /// @brief This is the flag used to stop the background thread.
      std::atomic<bool> shouldStop_ = false;

      /// @brief This is the background thread used for reading NFC tags.
      std::thread readThread_;

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
      void GetScannersAsync();

      /// @brief Binds the NFC scanners to the plugin.
      /// @param result The result callback to be called with the result.
      /// @note This function is called to bind the NFC scanners to the plugin.
      void BindScanners(std::function<void(std::optional<FlutterError> reply)> result);

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
      /// @brief Creates a new NFC context.
      /// @return true if the context was created successfully, false otherwise.
      bool createNfcContext();

      /// @brief Runs the NFC thread.
      /// @param cxt The context to be used for the NFC device.
      /// @param nfcReader The NFC reader name to be used for the NFC device.
      /// @note This function is called to run the NFC thread.
      void runNfcThread(SCARDCONTEXT cxt, const char* nfcReader);

      static void SuccessCallback() {}
      static void ErrorCallback(const FlutterError &error) {
        // Ignore ChannelConnection Error, This might occur because of HotReload
        if (error.code() != "channel-error") {
          std::cout << "ErrorCode: " << error.code() << " Message: " << error.message() << std::endl;
        }
      }

      /// @brief Gets the status of the reader state.
      /// @param state The reader state to be checked.
      /// @return The status of the reader state.
      std::string getStatusOfReaderState(SCARD_READERSTATEA state);

      /// @brief Reads the Mifare Mini card.
      /// @param card The card handle to be used for reading the card.
      /// @param protocol The protocol to be used for reading the card.
      /// @note This function is called to read the Mifare Mini card.
      void readMifareMiniCard(SCARDHANDLE card, DWORD protocol);

      /// @brief Reads the Mifare Classic card.
      /// @param card The card handle to be used for reading the card.
      /// @note This function is called to read the Mifare Classic card.
      void readMifareClassicCard(SCARDHANDLE card);

      /// @brief Reads the NFC Forum Type 2 card.
      /// @param card The card handle to be used for reading the card.
      /// @note This function is called to read the NFC Forum Type 2 card.
      void readNfcForumType2Card(SCARDHANDLE card);

      /// @brief Authenticates the card.
      /// @param card The card handle to be used for authentication.
      /// @param block The block number to be authenticated.
      /// @return the vector to be send to the card.
      std::vector<BYTE> buildAuthAPDUCommand(BYTE block);

      /// @brief Builds the read APDU command.
      /// @param block The block number to be read.
      /// @return The read APDU command to be sent to the card.
      std::vector<BYTE> buildReadAPDUCommand(BYTE block);

      /// @brief Sends an APDU command to the card.
      /// @param hCard The card handle to be used for sending the command.
      /// @param cmd The command to be sent to the card.
      /// @param response The response to be received from the card.
      /// @return The response from the card.
      std::vector<BYTE> send(SCARDHANDLE hCard, BYTE* cmd, DWORD len, DWORD protocol);
  };

}  // namespace layrz_nfc

#endif  // __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__
