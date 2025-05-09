abstract class LayrzNfcPlatformInterface {
  Future<bool> checkCapabilities() =>
      throw UnimplementedError('checkCapabilities() has not been implemented.');

  Future<bool> canRead() =>
      throw UnimplementedError('canRead() has not been implemented.');

  Future<bool> canWrite() =>
      throw UnimplementedError('canWrite() has not been implemented.');

  Future<bool> canSimulate() =>
      throw UnimplementedError('canSimulate() has not been implemented.');

  Future<bool> read() =>
      throw UnimplementedError('read() has not been implemented.');
}
