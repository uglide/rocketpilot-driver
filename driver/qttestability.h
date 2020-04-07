/*
Copyright 2012 Canonical

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.
*/

#ifndef QTTESTABILITY_H
#define QTTESTABILITY_H

extern "C"
#if (defined(WIN32) | defined(WIN64))
__declspec(dllexport)
#endif
void qt_testability_init(void);


#endif
