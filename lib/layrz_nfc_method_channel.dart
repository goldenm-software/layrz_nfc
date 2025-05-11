import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'layrz_nfc_platform_interface.dart';

/// An implementation of [LayrzNfcPlatform] that uses method channels.
class MethodChannelLayrzNfc extends LayrzNfcPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('layrz_nfc');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
