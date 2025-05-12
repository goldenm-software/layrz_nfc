library;

import 'package:flutter/foundation.dart';
import 'package:flutter/rendering.dart';
import 'package:ndef/ndef.dart' as ndef;

part 'src/nfc_forum_type_2.dart';
part 'src/mifare_classic.dart';

extension on Uint8List {
  String get humanized {
    return map((e) => '0x${e.toRadixString(16).padLeft(2, '0')}').join(', ');
  }
}
