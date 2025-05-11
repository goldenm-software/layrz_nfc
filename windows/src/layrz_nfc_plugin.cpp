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
  void LayrzNfcPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar) {
    auto plugin = std::make_unique<LayrzNfcPlugin>(registrar);
    LayrzNfcPlatformChannel::SetUp(registrar->messenger(), plugin.get());
    registrar->AddPlugin(std::move(plugin));
  }

  LayrzNfcPlugin::LayrzNfcPlugin(flutter::PluginRegistrarWindows *registrar) : uiThreadHandler_(registrar) {
    GetProximityDeviceAsync();
  }

  LayrzNfcPlugin::~LayrzNfcPlugin() {}

  winrt::fire_and_forget LayrzNfcPlugin::GetProximityDeviceAsync() {
    auto device = ProximityDevice::GetDefault();
    if (device) {
      
    }
    co_return;
  }

  void LayrzNfcPlugin::CheckCapabilities(std::function<void(ErrorOr<bool> reply)> result) {
    // pass
  }
  void LayrzNfcPlugin::CanRead(std::function<void(ErrorOr<bool> reply)> result) {
    // pass
  }
  void LayrzNfcPlugin::CanWrite(std::function<void(ErrorOr<bool> reply)> result) {
    // pass
  }
  void LayrzNfcPlugin::CanSimulate(std::function<void(ErrorOr<bool> reply)> result) {
    // pass
  }
  void LayrzNfcPlugin::StartReading(std::function<void(ErrorOr<bool> reply)> result) {
    // pass
  }
  void LayrzNfcPlugin::StopReading(std::function<void(ErrorOr<bool> reply)> result) {
    // pass
  }
}  // namespace layrz_nfc
