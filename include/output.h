#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <stdarg.h>

typedef int (*OutputFunc)(const char *format, ...);

int console_output(const char *format, ...);

typedef struct {
    char *func_name;
    OutputFunc func;
} OutputFuncType;

OutputFuncType *output_func_create(char *func_name, OutputFunc func);
void output_func_destroy(OutputFuncType *of);

#endif  /* __OUTPUT_H__ */
