import 'dart:convert';

import 'package:flutter/foundation.dart';
import 'package:layrz_nfc/src/layrz_nfc_pigeon/layrz_nfc.g.dart';
import 'package:layrz_nfc/src/parsers/parsers.dart';
import 'package:layrz_nfc/src/platform_interface.dart';
import 'package:ndef/ndef.dart' as ndef;

class LayrzNfcPigeonChannel extends LayrzNfcPlatformInterface {
  static LayrzNfcPigeonChannel? _instance;
  static LayrzNfcPigeonChannel get instance {
    _instance ??= LayrzNfcPigeonChannel._();
    return _instance!;
  }

  LayrzNfcPigeonChannel._() {
    LayrzNfcCallbackChannel.setUp(_LayrzNfcCallbackHandler());
  }
  final _channel = LayrzNfcPlatformChannel();

  @override
  Future<void> bindScanners() => _channel.bindScanners();

  @override
  Future<bool> checkCapabilities() => _channel.checkCapabilities();

  @override
  Future<bool> canRead() => _channel.canRead();

  @override
  Future<bool> canWrite() => _channel.canWrite();

  @override
  Future<bool> canSimulate() => _channel.canSimulate();

  @override
  Future<bool> startReading() => _channel.startReading();

  @override
  Future<bool> stopReading() => _channel.stopReading();
}

class _LayrzNfcCallbackHandler extends LayrzNfcCallbackChannel {
  @override
  void onRead(TagPayload payload) {
    switch (payload.format) {
      case TagFormat.nfcForumType2:
        parseNfcForumType2(payload.payload);
        break;
      case TagFormat.mifareClassic:
        parseMifareClassic(payload.payload);
        break;
      // ignore: unreachable_switch_default
      default:
        debugPrint('Unknown tag format: ${payload.format}');
    }
  }
}
