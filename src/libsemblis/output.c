#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

int console_output(const char *format, ...)
{
    va_list va;
    
    va_start(va, format);

    return vprintf(format, va);
}

OutputFuncType *output_func_create(char *func_name, OutputFunc func)
{
    OutputFuncType *oft = malloc(sizeof(*oft));
    oft->func_name = malloc(strlen(func_name) + 1);
    strcpy(oft->func_name, func_name);
    
    oft->func = func;

    return oft;
}

void output_func_destroy(OutputFuncType *of)
{
    free(of->func_name);
    free(of);
}
