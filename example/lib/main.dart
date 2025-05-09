import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:layrz_nfc/layrz_nfc.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _layrzNfcPlugin = LayrzNfc();
  bool canRead = false;
  bool canWrite = false;
  bool canSimulate = false;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(title: const Text('Plugin example app')),
        body: Center(
          child: Column(
            spacing: 24,
            children: [
              Text('Can Nfc Read: $canRead'),
              Text('Can Nfc Write: $canWrite'),
              Text('Can Nfc Simulate: $canSimulate'),

              ElevatedButton(
                onPressed: () => _callAll(),
                child: const Text('Check Capabilities'),
              ),
              ElevatedButton(
                onPressed: () => _readNfc(),
                child: const Text('Can Read'),
              ),
            ],
          ),
        ),
      ),
    );
  }

  void _callAll() async {
    canRead = await _layrzNfcPlugin.canRead();
    canWrite = await _layrzNfcPlugin.canWrite();
    canSimulate = await _layrzNfcPlugin.canSimulate();
    setState(() {});
  }

  void _readNfc() async {
    final bool success = await _layrzNfcPlugin.read();
    debugPrint("Read NFC Success: $success");
    setState(() {});
  }
}
