import 'package:layrz_nfc/src/layrz_nfc_pigeon/pigeon_channel.dart';
import 'package:layrz_nfc/src/platform_interface.dart';

/// The `LayrzNfc` class defines an interface for interacting with NFC capabilities.
/// The methods in this class should be implemented to provide specific functionality
/// related to reading, writing, and simulating NFC tags.
class LayrzNfc {
  static LayrzNfcPlatformInterface _channel = _getPlatformChannel();
  static LayrzNfcPlatformInterface _getPlatformChannel() {
    return LayrzNfcPigeonChannel.instance;
  }

  /// Checks if the device has the necessary capabilities to use NFC.
  ///
  /// Returns a `Future<bool>` indicating whether the capabilities are available.
  /// Currently not implemented.
  Future<bool> checkCapabilities() => _channel.checkCapabilities();

  /// Checks if the device can read NFC tags.
  ///
  /// Returns a `Future<bool>` indicating whether reading is possible.
  /// Currently not implemented.
  Future<bool> canRead() => _channel.canRead();

  /// Checks if the device can write to NFC tags.
  ///
  /// Returns a `Future<bool>` indicating whether writing is possible.
  /// Currently not implemented.
  Future<bool> canWrite() => _channel.canWrite();

  /// Checks if the device can simulate NFC tags.
  ///
  /// Returns a `Future<bool>` indicating whether simulation is possible.
  /// Currently not implemented.
  Future<bool> canSimulate() => _channel.canSimulate();

  /// Reads data from an NFC tag.
  Future<bool> read() => _channel.read();
}
