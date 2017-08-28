/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "qttestability.h"

#ifdef Q_OS_LINUX
    #include <link.h>
#else
    #include <dlfcn.h>
#endif

#include <string>
#include <iostream>


void qt_testability_init(void)
{
    std::string driver_name;
    #ifdef Q_OS_LINUX
        driver_name = "libautopilot_driver_qt5.so.1";
    #else
        driver_name = "libautopilot_driver_qt5.dylib";
    #endif

    void* driver = dlopen(driver_name.c_str(), RTLD_LAZY);
    if (!driver)
    {
        std::cerr << "Cannot load library: " << dlerror() << std::endl
            << "Autopilot introspection will not be available for this process." << std::endl;
            return;
    }

    // load the entry point function for the actual driver:
    typedef void (*entry_t)();
    // clear errors:
    dlerror();
    entry_t entry_point = (entry_t) dlsym(driver, "qt_testability_init");
    const char* err = dlerror();
    if (err)
    {
        std::cerr << "Cannot load library entry point symbol: " << err << std::endl
        << "Autopilot introspection will not be available for this process." << std::endl;
            return;
    }
    entry_point();
}
