# Farert 経路運賃営業キロ計算アプリ

## なんなのか
- https://farert.blogspot.com/p/main.html

## 使い方
- https://farert.blogspot.com/2017/03/blog-post_54.html

## build & make

### Windows
+ Open file app/win_mfc/fjr_mfc/alps_mfc.sln in Microsoft Visual Studio 2022 Express.
+ Run [build]-[batch build]-[release]

### Android
+ Open folder 'app/Farert.android/' in Android studio 4.
+ build project.

### iOS
+ Open file 'farert.ios/Farert.xcodeproj' in Xcode.
+ [build] or [archive]

### POSIX(CLI)
```
$ cd test/unix/all
$ make
$ source start.sh
$ ./farert <駅1> <路線1> <駅2> ...
```
### How to edit and make database
- See db/ directory.

## license
- GPL(v3)

## Author
- sutezo

