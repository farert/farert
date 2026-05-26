# Farert 経路運賃営業キロ計算アプリ

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](license.txt)
![Android](https://img.shields.io/badge/Android-24%2B-3DDC84?logo=android&logoColor=white)
![iOS](https://img.shields.io/badge/iOS-supported-000000?logo=apple&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-supported-0078D4?logo=windows&logoColor=white)
![POSIX CLI](https://img.shields.io/badge/POSIX-CLI-4EAA25?logo=gnubash&logoColor=white)

## Quick Try

```
git clone https://github.com/farert/farert.git
cd farert
cd test/unix/azusa
make
source ../all/start.sh
./farecli 東京 東海道新幹線 新大阪 山陽新幹線 博多
```

## なんなのか
- https://farert.blogspot.com/p/main.html

## 使い方
- https://farert.blogspot.com/2017/03/blog-post_54.html

## build & make

### Windows
+ Open file app/win_mfc/fjr_mfc/alps_mfc.sln in Microsoft Visual Studio 2022 Express.
+ Run [build]-[batch build]-[release]

### Android
+ Open folder 'app/Farert.android/' in Android studio.

#### build project.

##### APK

```

./gradlew assembleRelease
## output to app/build/outputs/apk/release/app-release.apk
```

##### AAB

```

./gradlew bundleRelease
## output to app/build/outputs/bundle/release/app-release.aab
```

### iOS

+ Open file 'farert.ios/Farert.xcodeproj' in Xcode.
+ [build] or [archive]

### POSIX(CLI)

```
$ cd test/unix/azusa
$ make
$ source ../all/start.sh
$ ./farecli <駅1> <路線1> <駅2> ...

```

### Debugging core logic

- VSCode Debug (POSIX)

`test/unix/all` をワークスペースとして開く場合:

```
cd test/unix/all
make
mkdir -p .vscode
cp dot-vscode_launch.json .vscode/launch.json
code .
```

- Use VSCode Plugin: CodeLLDB（`vadimcn.vscode-lldb`）
- Can you edit the `.vscode/launch.json` `args`

### Test code in Java

- See `./javatest.md`

### How to edit and make database

- See db/ directory.

## license

- GPL(v3)
- See db/ directory for database license agreement.

## Author

- Copyright(c) sutezo 2026
