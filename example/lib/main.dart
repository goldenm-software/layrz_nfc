// ignore_for_file: use_build_context_synchronously
import 'dart:async';

import 'package:flutter/material.dart';
import 'package:layrz_nfc/layrz_nfc.dart';
import 'package:layrz_icons/layrz_icons.dart';
import 'package:layrz_models/layrz_models.dart';
import 'package:layrz_theme/layrz_theme.dart';
import 'package:ndef/ndef.dart' as ndef;

const kFont = AppFont(source: FontSource.google, name: 'Ubuntu');

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await ThemedFontHandler.preloadFont(kFont);
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: generateLightTheme(),
      debugShowCheckedModeBanner: false,
      builder: (context, child) {
        return ThemedSnackbarMessenger(
          child: child ?? const SizedBox(),
        );
      },
      home: const HomePage(),
    );
  }
}

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  AppThemedAsset get logo => const AppThemedAsset(
        normal: 'https://cdn.layrz.com/resources/layrz/logo/normal.png',
        white: 'https://cdn.layrz.com/resources/layrz/logo/white.png',
      );
  AppThemedAsset get favicon => const AppThemedAsset(
        normal: 'https://cdn.layrz.com/resources/layrz/favicon/normal.png',
        white: 'https://cdn.layrz.com/resources/layrz/favicon/white.png',
      );

  final plugin = LayrzNfc();
  bool _isReading = false;
  String? payload;

  @override
  void initState() {
    super.initState();
    plugin.onRead.listen((record) {
      if (record is ndef.TextRecord) {
        debugPrint('TextRecord: ${record.text}');
      } else if (record is ndef.UriRecord) {
        debugPrint('UriRecord: ${record.uri}');
      } else {
        debugPrint('Unknown record type: ${record.runtimeType}');
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return ThemedLayout(
      isBackEnabled: false,
      logo: logo,
      favicon: favicon,
      appTitle: 'Layrz NFC',
      enableNotifications: false,
      userDynamicAvatar: Avatar(
        type: AvatarType.icon,
        icon: LayrzIconsClasses.solarOutlineUser,
      ),
      body: SizedBox(
        width: double.infinity,
        child: Column(
          children: [
            Text(
              "Layrz NFC",
              style: Theme.of(context).textTheme.titleMedium,
            ),
            const SizedBox(height: 10),
            SingleChildScrollView(
              scrollDirection: Axis.horizontal,
              child: Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  ThemedButton(
                    labelText: 'Check capabilities',
                    color: Colors.blue,
                    onTap: () async {
                      bool result = await plugin.checkCapabilities();
                      ThemedSnackbarMessenger.of(context).showSnackbar(ThemedSnackbar(
                        message: 'Capabilities: $result',
                        color: Colors.blue,
                        icon: LayrzIcons.solarOutlineBluetoothSquare,
                        maxLines: 5,
                      ));

                      await Future.delayed(const Duration(milliseconds: 100));

                      result = await plugin.canRead();
                      ThemedSnackbarMessenger.of(context).showSnackbar(ThemedSnackbar(
                        message: 'Can read: $result',
                        color: Colors.blue,
                        icon: LayrzIcons.solarOutlineBluetoothSquare,
                        maxLines: 5,
                      ));

                      await Future.delayed(const Duration(milliseconds: 100));

                      result = await plugin.canWrite();
                      ThemedSnackbarMessenger.of(context).showSnackbar(ThemedSnackbar(
                        message: 'Can write: $result',
                        color: Colors.blue,
                        icon: LayrzIcons.solarOutlineBluetoothSquare,
                        maxLines: 5,
                      ));

                      await Future.delayed(const Duration(milliseconds: 100));

                      result = await plugin.canSimulate();
                      ThemedSnackbarMessenger.of(context).showSnackbar(ThemedSnackbar(
                        message: 'Can simulate: $result',
                        color: Colors.blue,
                        icon: LayrzIcons.solarOutlineBluetoothSquare,
                        maxLines: 5,
                      ));
                    },
                  ),
                  if (_isReading) ...[
                    const SizedBox(width: 10),
                    ThemedButton(
                      labelText: 'Stop reading',
                      color: Colors.red,
                      onTap: () async {
                        await plugin.stopReading();
                        setState(() => _isReading = false);
                      },
                    ),
                  ] else ...[
                    const SizedBox(width: 10),
                    ThemedButton(
                      labelText: 'Start reading',
                      color: Colors.green,
                      onTap: () async {
                        debugPrint(' Flutter call starting reading');
                        await plugin.startReading();
                        setState(() => _isReading = true);
                      },
                    ),
                  ],
                ],
              ),
            ),
            Text(
              "Payload: $payload",
              style: Theme.of(context).textTheme.titleLarge,
            ),
          ],
        ),
      ),
    );
  }
}
