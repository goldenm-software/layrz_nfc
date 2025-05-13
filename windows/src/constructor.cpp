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
  std::unique_ptr<LayrzNfcCallbackChannel> LayrzNfcPlugin::callbackChannel;

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
}  // namespace layrz_nfc
