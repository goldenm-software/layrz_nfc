import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'layrz_nfc_method_channel.dart';

abstract class LayrzNfcPlatform extends PlatformInterface {
  /// Constructs a LayrzNfcPlatform.
  LayrzNfcPlatform() : super(token: _token);

  static final Object _token = Object();

  static LayrzNfcPlatform _instance = MethodChannelLayrzNfc();

  /// The default instance of [LayrzNfcPlatform] to use.
  ///
  /// Defaults to [MethodChannelLayrzNfc].
  static LayrzNfcPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [LayrzNfcPlatform] when
  /// they register themselves.
  static set instance(LayrzNfcPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
