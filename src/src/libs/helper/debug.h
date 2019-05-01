#ifndef HELPER_DEBUG_H
#define HELPER_DEBUG_H

#include "debug.class.h"

void log(const char* value)
{
    System::Debug debug;
    debug.Log(value);
}

void log(int value)
{
    System::Debug debug;
    debug.Log(value);
}

#endif