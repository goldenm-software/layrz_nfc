import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(
  PigeonOptions(
    dartPackageName: 'layrz_nfc',
    dartOptions: DartOptions(),
    dartOut: 'lib/src/layrz_nfc_pigeon/layrz_nfc.g.dart',
    kotlinOptions: KotlinOptions(package: 'com.layrz.layrz_nfc'),
    kotlinOut: 'android/src/main/kotlin/com/layrz/layrz_nfc/LayrzNfc.g.kt',
    // swiftOptions: SwiftOptions(),
    // swiftOut: 'darwin/layrz_nfc/Sources/layrz_nfc/LayrzNfc.g.swift',
    // cppOptions: CppOptions(namespace: 'layrz_nfc'),
    // cppHeaderOut: 'windows/src/generated/layrz_nfc.g.h',
    // cppSourceOut: 'windows/src/generated/layrz_nfc.g.cpp',
    debugGenerators: true,
  ),
)

// Host API from Flutter to Native
@HostApi()
abstract class LayrzNfcPlatformChannel {
  @async
  bool checkCapabilities();

  @async
  bool canRead();

  @async
  bool canWrite();

  @async
  bool canSimulate();

  @async
  bool read();
}

// Flutter API from Native to Flutter
// @FlutterApi()
// abstract class LayrzNfcCallbackChannel {}
