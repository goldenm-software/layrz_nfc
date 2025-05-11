#include "include/layrz_nfc/layrz_nfc_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "src/layrz_nfc_plugin.h"

void LayrzNfcPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  layrz_nfc::LayrzNfcPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
