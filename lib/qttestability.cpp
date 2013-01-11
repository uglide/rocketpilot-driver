/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#include "qttestability.h"

#include <link.h>
#include <string>
#include <iostream>

typedef enum
{
    QT_VERSION_4,
    QT_VERSION_5,
    QT_VERSION_UNKNOWN
} QtVersion;

static int
callback(struct dl_phdr_info *info, size_t size, void *data)
{
    (void) size;
    QtVersion *v = (QtVersion*) data;

    if (*v != QT_VERSION_UNKNOWN)
    {
        std::string lib_path(info->dlpi_name);
        if (lib_path.rfind("libQtCore.so.4") != std::string::npos)
        {
            *v = QT_VERSION_4;
        }
        else if (lib_path.rfind("libQtCore.so.5") != std::string::npos)
        {
            *v = QT_VERSION_5;
        }
    }

    return 0;
}

void qt_testability_init(void)
{
    QtVersion version = QT_VERSION_UNKNOWN;
    dl_iterate_phdr(callback, &version);

    std::string driver_name;
    if (version == QT_VERSION_4)
    {
        driver_name = "libqt_driver.so.1";
    }
    else if (version == QT_VERSION_5)
    {
        driver_name = "libqt_driver.so.1";
    }
    else
    {
        std::cerr << "We don't seem to link to version 4 or 5 of QtCore." << std::endl
            << "Unable to determine which autopilot driver to load." << std::endl
            << "Autopilot introspection will not be available for this process." << std::endl;
            return;
    }

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
