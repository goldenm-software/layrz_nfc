part of '../parsers.dart';

void parseNfcForumType2(Uint8List bytes) {
  int tlvMarker = bytes.indexOf(0x03);
  if (tlvMarker == -1) {
    debugPrint("No TLV marker found");
    return;
  }

  debugPrint("Data: ${bytes.sublist(tlvMarker).humanized}");

  int length = bytes[tlvMarker + 1];
  int startIndex = tlvMarker + 2;
  int endIndex = startIndex + length;

  debugPrint("Length: $length");
  debugPrint("Start index: $startIndex");
  debugPrint("End index: $endIndex");
  if (endIndex > bytes.length) {
    debugPrint("End index is out of bounds");
    return;
  }

  Uint8List payload = bytes.sublist(startIndex, endIndex);
  debugPrint("Payload: ${payload.humanized}");

  int index = 0;
  while (index < payload.length) {
    int header = payload[index++];
    bool mb = (header & 0x80) != 0;
    bool me = (header & 0x40) != 0;
    bool sr = (header & 0x10) != 0;
    int tnf = header & 0x07;

    int typeLength = payload[index++];
    int payloadLength = sr ? payload[index++] : (payload[index++] << 8) | payload[index++];
    Uint8List type = payload.sublist(index, index + typeLength);
    index += typeLength;
    Uint8List recordPayload = payload.sublist(index, index + payloadLength);
    index += payloadLength;

    debugPrint(
        "Record: MB=$mb, ME=$me, TNF=$tnf, Type=${String.fromCharCodes(type)}, Payload=${String.fromCharCodes(recordPayload)}");
  }
}
