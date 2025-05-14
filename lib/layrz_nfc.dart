import 'package:layrz_nfc/src/layrz_nfc_pigeon/pigeon_channel.dart';
import 'package:layrz_nfc/src/platform_interface.dart';
import 'package:ndef/ndef.dart';

/// The `LayrzNfc` class defines an interface for interacting with NFC capabilities.
/// The methods in this class should be implemented to provide specific functionality
/// related to reading, writing, and simulating NFC tags.
class LayrzNfc {
  static final LayrzNfcPlatformInterface _channel = _getPlatformChannel();
  static LayrzNfcPlatformInterface _getPlatformChannel() {
    return LayrzNfcPigeonChannel.instance;
  }

  /// Listens for NFC tag read events.
  ///
  /// You need to call [startReading] to start receiving events.
  /// This stream emits [NDEFRecord] objects when a tag is read, you can receive multiple [NDEFRecord]s per tag
  /// depending on the tag payload.
  Stream<NDEFRecord> get onRead => _channel.onRead;

  /// Checks if the device has the necessary capabilities to use NFC.
  ///
  /// This validation is only for check if the device has NFC capabilities, aka, device has NFC hardware.
  Future<bool> checkCapabilities() => _channel.checkCapabilities();

  /// Checks if the device can read NFC tags.
  ///
  /// Returns `true` if the device can read NFC tags, `false` otherwise.
  Future<bool> canRead() => _channel.canRead();

  /// Checks if the device can write to NFC tags.
  ///
  /// Returns `true` if the device can write to NFC tags, `false` otherwise.
  Future<bool> canWrite() => _channel.canWrite();

  /// Checks if the device can simulate NFC tags.
  ///
  /// Returns `true` if the device can simulate NFC tags, `false` otherwise.
  Future<bool> canSimulate() => _channel.canSimulate();

  /// Starts reading NFC tags.
  ///
  /// This method should be called to initiate the NFC reading process.
  Future<bool> startReading() => _channel.startReading();

  /// Stops reading NFC tags.
  ///
  /// This method should be called to stop the NFC reading process.
  Future<bool> stopReading() => _channel.stopReading();
}
