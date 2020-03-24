#include "qttestability.h"
#include <windows.h>
#include <string>
#include <iostream>

void qt_testability_init(void)
{    
    HINSTANCE driver = LoadLibrary(L"rocketpilot_driver_qt5.dll");
    if (!driver)
    {
        std::cerr << "Cannot load library" << std::endl
            << "Autopilot introspection will not be available for this process." << std::endl;
            return;
    }

    // load the entry point function for the actual driver:
    typedef void (*entry_t)();
    entry_t entry_point = (entry_t)GetProcAddress(driver, "qt_testability_init");
    if (!entry_point)
    {
        std::cerr << "Cannot load library entry point symbol" << std::endl
        << "Autopilot introspection will not be available for this process." << std::endl;
            return;
    }
    entry_point();
}
