import 'dart:convert';

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
  void onRead(Uint8List payload) {
    debugPrint('Received payload: ${payload.humanized}');
    final i = payload.indexOf(0x03); // TLV Type
    if (i == -1 || i + 1 >= payload.length) {
      debugPrint('Invalid payload: ${payload.humanized}');
      return;
    }

    final length = payload[i + 1];
    final start = i + 2;
    final end = start + length;

    if (end > payload.length) {
      debugPrint('Invalid payload length: ${payload.humanized}');
      debugPrint('\tStart: $start, End: $end, Length: $length');
      return;
    }

    final ndefMessage = payload.sublist(start, end);
    debugPrint('Trimmed payload: ${ndefMessage.humanized}');

    parseNdef(ndefMessage);
  }

  void parseNdef(Uint8List bytes) {
    int i = 0;
    while (i < bytes.length) {
      final header = bytes[i];
      final isME = (header & 0x40) != 0;
      final isSR = (header & 0x10) != 0;
      final tnf = header & 0x07;

      if (!isSR) {
        debugPrint('❌ Only short records (SR=1) supported in this parser.');
        return;
      }

      final typeLength = bytes[i + 1];
      final payloadLength = bytes[i + 2];
      final typeStart = i + 3;
      final payloadStart = typeStart + typeLength;
      final payloadEnd = payloadStart + payloadLength;

      if (payloadEnd > bytes.length) {
        debugPrint('❌ Invalid length in NDEF record.');
        return;
      }

      final type = String.fromCharCodes(bytes.sublist(typeStart, payloadStart));
      final payload = bytes.sublist(payloadStart, payloadEnd);

      // Print record
      debugPrint('📦 TNF: $tnf, Type: $type, Payload: ${payload.humanized}');

      if (tnf == 1 && type == 'T') {
        final langLen = payload[0];
        final text = String.fromCharCodes(payload.sublist(1 + langLen));
        debugPrint('📝 Text: $text');
      } else if (tnf == 1 && type == 'U') {
        final uriId = payload[0];
        String uriSuffix = payload.sublist(1).map((e) => String.fromCharCode(e)).join();
        String uriPrefix = decodeUriPrefix(uriId);
        String uri = uriPrefix + uriSuffix;
        debugPrint('🌐 URI: $uri');
      } else {
        debugPrint('📦 Unsupported TNF: $tnf, Type: $type');
      }

      i = payloadEnd;
      if (isME) break;
    }
  }

  String decodeUriPrefix(int id) {
    const prefixes = [
      '',
      'http://www.',
      'https://www.',
      'http://',
      'https://',
      'tel:',
      'mailto:',
      'ftp://anonymous:anonymous@',
      'ftp://ftp.',
      'ftps://',
      'sftp://',
      'smb://',
      'nfs://',
      'ftp://',
      'dav://',
      'news:',
      'telnet://',
      'imap:',
      'rtsp://',
      'urn:',
      'pop:',
      'sip:',
      'sips:',
      'tftp:',
      'btspp://',
      'btl2cap://',
      'btgoep://',
      'tcpobex://',
      'irdaobex://',
      'file://',
      'urn:epc:id:',
      'urn:epc:tag:',
      'urn:epc:pat:',
      'urn:epc:raw:',
      'urn:epc:',
      'urn:nfc:'
    ];
    return (id < prefixes.length) ? prefixes[id] : '';
  }
}

extension on Uint8List {
  String get humanized {
    return map((e) => e.toRadixString(16).padLeft(2, '0')).join(' ');
  }
}
