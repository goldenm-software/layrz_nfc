// ignore_for_file: use_build_context_synchronously

import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:layrz_nfc/layrz_nfc.dart';
import 'package:layrz_icons/layrz_icons.dart';
import 'package:layrz_models/layrz_models.dart';
import 'package:layrz_theme/layrz_theme.dart';

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

  @override
  void initState() {
    super.initState();
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
                      await plugin.bindScanners();

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
                        await plugin.startReading();
                        setState(() => _isReading = true);
                      },
                    ),
                  ],
                  const SizedBox(width: 10),
                  ThemedButton(
                    labelText: 'Test parsers',
                    color: Colors.orange,
                    onTap: _parse,
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }

  void _parse() {
    final payload = Uint8List.fromList([
      0x03,
      0xa0,
      0x0c,
      0x34,
      0x03,
      0x1f,
      0x91,
      0x01,
      0x0d,
      0x54,
      0x02,
      0x65,
      0x6e,
      0x48,
      0x6f,
      0x6c,
      0x61,
      0x20,
      0x6d,
      0x75,
      0x6e,
      0x64,
      0x6f,
      0x51,
      0x01,
      0x0a,
      0x55,
      0x04,
      0x6c,
      0x61,
      0x79,
      0x72,
      0x7a,
      0x2e,
      0x63,
      0x6f,
      0x6d,
      0xfe,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
    ]);

    parseNfcForumType2(payload);
  }
}
