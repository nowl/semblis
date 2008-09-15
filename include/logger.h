#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "windll.h"
#include "stdbool.h"

typedef enum {
    LL_DEBUG = 0,
    LL_INFO,
    LL_WARNING,
    LL_ERROR,
    LL_CRITICAL
} LogLevel;

DLL_INFO void logger_init(char *filename);
DLL_INFO void logger_close();

DLL_INFO void logger_set_level(LogLevel level);
DLL_INFO void logger_set_console_output(bool setting);
DLL_INFO void logger_log(const char *logger_name,
                         LogLevel level,
                         char *message);

#endif // __LOGGER_H__
