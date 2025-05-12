import 'package:flutter/foundation.dart';
import 'package:layrz_nfc/src/layrz_nfc_pigeon/layrz_nfc.g.dart';
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
    final bytes = payload.payload;
    // debugPrint("Received bytes: ${bytes.humanized}");
    int index = -1;
    int length = 0;

    for (int i = 0; i < bytes.length - 2; i++) {
      if (bytes[i] == 0x03) {
        final len = bytes[i + 1];
        if (len > 0 && i + 2 + len <= bytes.length) {
          index = i;
          length = len;
        }
      }
    }

    if (index == -1) {
      // debugPrint("No TLV found");
      return;
    }

    final data = bytes.sublist(index + 2, index + 2 + length);
    // debugPrint("Trimmed data: ${data.humanized}");

    final output = ndef.decodeRawNdefMessage(data);
    debugPrint("Decoded NDEF message for type ${payload.format}: $output");
  }
}

extension HumanizedUint8List on Uint8List {
  String get humanized {
    return map((e) => '0x${e.toRadixString(16).padLeft(2, '0')}').join(', ');
  }
}
