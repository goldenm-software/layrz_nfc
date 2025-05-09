import 'package:flutter/foundation.dart';
import 'package:layrz_nfc/src/layrz_nfc_pigeon/layrz_nfc.g.dart';
import 'package:layrz_nfc/src/platform_interface.dart';

class LayrzNfcPigeonChannel extends LayrzNfcPlatformInterface {
  static LayrzNfcPigeonChannel? _instance;
  static LayrzNfcPigeonChannel get instance {
    _instance ??= LayrzNfcPigeonChannel._();
    return _instance!;
  }

  LayrzNfcPigeonChannel._() {
    debugPrint("TODO");
  }
  final _channel = LayrzNfcPlatformChannel();

  @override
  Future<bool> checkCapabilities() => _channel.checkCapabilities();
  @override
  Future<bool> canRead() => _channel.canRead();
  @override
  Future<bool> canWrite() => _channel.canWrite();
  @override
  Future<bool> canSimulate() => _channel.canSimulate();
}
