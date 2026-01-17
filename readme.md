# Farert 経路運賃営業キロ計算アプリ

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

### How to edit and make database

- See db/ directory.

## license

- GPL(v3)
- See db/ directory for database license agreement.

## Author

- Copyright(c) sutezo 2025

