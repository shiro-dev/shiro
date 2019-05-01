#ifndef HELPER_PHP_H
#define HELPER_PHP_H

#include "echo.class.h"

template <typename Any>
void echo(Any content) {
    PHP::Echo echo;
    echo.Call(content);
}

template <typename Any, typename... Rest>
void echo(Any content, Rest... rest) 
{
    PHP::Echo echo;
    echo.Call(content, rest...);
}

#endif