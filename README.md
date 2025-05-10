# layrz_nfc

[![Pub version](https://img.shields.io/pub/v/layrz_nfc?logo=flutter)](https://pub.dev/packages/layrz_nfc)
[![Pub Points](https://img.shields.io/pub/points/layrz_nfc)](https://pub.dev/packages/layrz_nfc/score)
[![likes](https://img.shields.io/pub/likes/layrz_nfc?logo=flutter)](https://pub.dev/packages/layrz_nfc/score)
[![GitHub license](https://img.shields.io/github/license/goldenm-software/layrz_nfc?logo=github)](https://github.com/goldenm-software/layrz_nfc)

A simple way to interact with NFC cards in Flutter.

## Methods and streams

| Method or stream | Android | iOS | macOS | Windows | Description | 
| --- | --- | --- | --- | --- | --- |
| `checkCapabilities` | ✅ | ❌ | ❌ | ❌ | Check if the device has NFC capabilities. |
| `canRead` | ✅ | ❌ | ❌ | ❌ | Check if the device can read NFC cards. |
| `canWrite` | ✅ | ❌ | ❌ | ❌ | Check if the device can write NFC cards. |
| `canSimulate` | ✅ | ❌ | ❌ | ❌ | Check if the device can simulate NFC cards. |
| `startReading` | ❌ | ❌ | ❌ | ❌ | Start reading NFC cards. |
| `stopReading` | ❌ | ❌ | ❌ | ❌ | Stop reading NFC cards. |
| `write` | ❌ | ❌ | ❌ | ❌ | Write NFC cards. |
| `simulate` | ❌ | ❌ | ❌ | ❌ | Simulate NFC cards. |  
| `onRead` | ❌ | ❌ | ❌ | ❌ | Stream that emits when a NFC card is read, requires `startReading` and `stopReading` to work. | 


### Languages used
| Platform | Language |
| --- | --- |
| Android | Kotlin |
| iOS | Swift |
| macOS | Swift |
| Windows | C++ |

## Minimum requirements
### Android

6.0 Marshmallow (API Level 23) or later. Be careful with the permissions!.

### iOS

iOS 14.0 or later.

### macOS

11.0 Big Sur or later.

### Windows

Windows 10.0 or later (Like as the versions supported by [Flutter](https://docs.flutter.dev/reference/supported-platforms)).

## Usage
To use this plugin, add `layrz_nfc` as a [dependency in your pubspec.yaml file](https://flutter.dev/docs/development/packages-and-plugins/using-packages).

```yaml
dependencies:
  flutter:
    sdk: flutter
  layrz_nfc: ^latest_version
```

Then you can import the package in your Dart code:

```dart
import 'package:layrz_nfc/layrz_nfc.dart';

/// ...

final nfc = LayrzNfc();

/// Listen for read events
///
/// `NfcPayload` is from this package
nfc.onRead.listen((NfcPayload payload) {
  debugPrint(payload);
});

/// Remaining methods are TO DO
```

## Permissions and requirements

Before getting into the platform specific permissions, always raises the question "How can I handle the permissions on my Flutter app?". Well, you can use the [`permission_handler`](https://pub.dev/packages/permission_handler) package to handle the permissions on your Flutter app, or you can handle them manually with native code, the choice is yours.

### Android
```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android">
  <!-- Declares the permission to use NFC on your android project -->
  <uses-permission android:name="android.permission.NFC" />

  <!-- This feature declares that the project requires NFC capabilities, and the Play Store will be display it as well -->
  <uses-feature android:name="android.hardware.nfc" android:required="true" />

  <!-- ... -->
</manifest>
```

### iOS

TBD

### macOS

TBD

### Windows

TBD 

## FAQ

### Why is this package called `layrz_nfc`?
All packages developed by [Layrz](https://layrz.com) are prefixed with `layrz_`, check out our other packages on [pub.dev](https://pub.dev/publishers/goldenm.com/packages).

### I need to pay to use this package?
<b>No!</b> This library is free and open source, you can use it in your projects without any cost, but if you want to support us, give us a thumbs up here in [pub.dev](https://pub.dev/packages/layrz_nfc) and star our [Repository](https://github.com/goldenm-software/layrz_nfc)!

### Can I contribute to this package?
<b>Yes!</b> We are open to contributions, feel free to open a pull request or an issue on the [Repository](https://github.com/goldenm-software/layrz_nfc)!

### I have a question, how can I contact you?
If you need more assistance, you open an issue on the [Repository](https://github.com/goldenm-software/layrz_nfc) and we're happy to help you :)

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

This project is maintained by [Golden M](https://goldenm.com) with authorization of [Layrz LTD](https://layrz.com).

## Who are you? / Want to work with us?
<b>Golden M</b> is a software and hardware development company what is working on a new, innovative and disruptive technologies. For more information, contact us at [sales@goldenm.com](mailto:sales@goldenm.com) or via WhatsApp at [+(507)-6979-3073](https://wa.me/50769793073?text="From%20layrz_nfc%20flutter%20library.%20Hello").
