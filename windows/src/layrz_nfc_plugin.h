#pragma once

#ifndef __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__
#define __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Proximity.h>
#include <winrt/Windows.Storage.Streams.h>

#include "thread_handler.hpp"
#include "generated/layrz_nfc.g.h"

namespace layrz_nfc {
  using namespace winrt;
  using namespace winrt::Windows::Foundation;
  using namespace winrt::Windows::Networking::Proximity;

  using layrz_nfc::ErrorOr;
  using layrz_nfc::LayrzNfcPlatformChannel;
  // using layrz_nfc::LayrzNfcCallbackChannel;

  class LayrzNfcPlugin : public flutter::Plugin, public LayrzNfcPlatformChannel {
    public:
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
      winrt::fire_and_forget GetProximityDeviceAsync();

      /// @brief Checks if the device has NFC capabilities.
      /// @param result The result callback to be called with the result.
      void CheckCapabilities(std::function<void(ErrorOr<bool> reply)> result);
      void CanRead(std::function<void(ErrorOr<bool> reply)> result);
      void CanWrite(std::function<void(ErrorOr<bool> reply)> result);
      void CanSimulate(std::function<void(ErrorOr<bool> reply)> result);
      void StartReading(std::function<void(ErrorOr<bool> reply)> result);
      void StopReading(std::function<void(ErrorOr<bool> reply)> result);

    // Disallow copy and assign.
    LayrzNfcPlugin(const LayrzNfcPlugin&) = delete;
    LayrzNfcPlugin& operator=(const LayrzNfcPlugin&) = delete;
  };

}  // namespace layrz_nfc

#endif  // __FLUTTER_PLUGIN_LAYRZ_NFC_PLUGIN_H__
