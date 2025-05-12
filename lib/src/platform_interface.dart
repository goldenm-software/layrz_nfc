import 'package:ndef/ndef.dart';

abstract class LayrzNfcPlatformInterface {
  Stream<NDEFRecord> get onRead => throw UnimplementedError('onRead has not been implemented.');
  Future<bool> checkCapabilities() => throw UnimplementedError('checkCapabilities() has not been implemented.');
  Future<bool> canRead() => throw UnimplementedError('canRead() has not been implemented.');
  Future<bool> canWrite() => throw UnimplementedError('canWrite() has not been implemented.');
  Future<bool> canSimulate() => throw UnimplementedError('canSimulate() has not been implemented.');
  Future<bool> startReading() => throw UnimplementedError('startReading() has not been implemented.');
  Future<bool> stopReading() => throw UnimplementedError('stopReading() has not been implemented.');
}
