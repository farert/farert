# Android 版のアップデート

## 目的

- Migrate to Andrdoid version from C++ version.
- Android は、C++の Git hash 8c366 Versionと同様
- C++ バージョンの  現在 から更新している箇所を、Android版へも反映させる。
- Android Version is Java.
- C++ は、デバック済み、動作確認済みで正しい動作をしている。
- C++ から Android版への更新は、その深い意味を考えず機械的にできるだけ1対1の対応でAndroid版を実装すること。
- C++の実装に忠実に沿うようにAndroid版は更新すること

## Android version

`app/Farert.android/app/src/main/java/org/sutezo/alps`

## C++ version

`app/alps/alpdb.cpp`
`app/alps/alpdb.h`

## Test
`./test/run_java_regression.sh`
