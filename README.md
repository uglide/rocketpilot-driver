## Unofficial Autopilot Qt Driver

### What is this?


This is modified the Qt driver for autopilot. It allows autopilot to inspect the
internals of Qt5 / QMl-based applications.

### Goals of fork
- Improve introspection
- Add Windows support


###H ow does it work?

Qt loads a 'qt_testability' library, if *either* the ``-testability`` command
line argument is passed to ``QCoreApplication``, *or* if the
``QT_LOAD_TESTABILITY`` environment variable is set. This codebase provides that
library, along with several others.

Upon being loaded, it connects to the system bus, and exposes an interface that
the autopilot test runner knows how to interact with.


### Build

#### Ubuntu
##### Dependencies
```
sudo apt-get install libxpathselect-dev

```

