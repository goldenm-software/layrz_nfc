.PHONY: all test build lint clean

build:
	dart run pigeon --input pigeon/layrz_nfc.dart

lint:
	dart fix --dry-run

test:
	flutter test

clean:
	flutter clean
	cd example
	flutter clean
	cd ..
	flutter pub get
