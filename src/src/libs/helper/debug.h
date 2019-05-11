#ifndef HELPER_DEBUG_H
#define HELPER_DEBUG_H

#include "debug.class.h"

template <typename Any>
void log(Any content) {
    System::Debug debug;
    debug.Log(content);
}

template <typename Any, typename... Rest>
void log(Any content, Rest... rest) 
{
    System::Debug debug;
    debug.Log(content, rest...);
}

#endif