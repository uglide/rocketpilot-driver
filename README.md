## RocketPilot-Driver aka *RocketPilot Qt Testability Driver*

### What is this?

This is Qt testability driver which allows [RocketPilot](https://github.com/uglide/RocketPilot) (Autopilot fork) to inspect the
internals of Qt5 / QML-based applications.

### Goals of fork
- Improve introspection :white_check_mark:
- Add OSX support :white_check_mark:
- Add Windows support :white_check_mark:


### How does it work?

Qt loads a 'qt_testability' library, if *either* the ``-testability`` command
line argument is passed to ``QCoreApplication``, *or* if the
``QT_LOAD_TESTABILITY`` environment variable is set. This codebase provides that
library, along with several others.

Upon being loaded, it connects to the system bus, and exposes an interface that
the RocketPilot knows how to interact with.


### Build

#### Ubuntu
```
sudo apt-get install libxpathselect-dev
qmake
make -j 2

```

#### macOS
```
brew install boost
cd 3rdparty/xpathselect
qmake
make -j 2
cd ../../
qmake
make -j 2
```


#### Windows
1. Install nuget https://www.nuget.org/downloads
2. Install boost lib:
```
cd 3rdparty/xpathselect
nuget install boost -Version 1.71.0
```
3. Build xpathselect (3rdparty/xpathselect) using QtCreator
4. Build rocketpilot-driver
5. Install Msys2 to C:\msys2 & update core packages
6. Install dbus:
```
pacman -S mingw64/mingw-w64-x86_64-dbus
```
7. Update C:\msys64\mingw64\share\dbus-1\session.conf :
```
<listen>tcp:host=localhost,port=54321,family=ipv4</listen>
```
8. Add `C:\msys64\mingw64\bin` to PATH
9. Add `DBUS_SESSION_BUS_ADDRESS` env variable with value `tcp:host=localhost,port=54321,family=ipv4`
10. Run `dbus-daemon.exe --session`