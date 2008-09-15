#ifndef __MACROS_H__
#define __MACROS_H__

#include "output.h"

extern OutputFunc output_std_func, output_err_func;

#define OUTPUT_REG        output_std_func
#define OUTPUT_ERR        output_err_func

/* debug macros */

#define _START(name)

/* data macros */

#define IS_PROCEDURE(x)                         \
    ( (x)->type == DT_PROCEDURE )
#define IS_MACRO(x)                             \
    ( (x)->type == DT_MACRO )
#define IS_POINTER(x)                           \
    ( (x)->type == DT_POINTER )
#define IS_NIL(x)                               \
    ( (x)->type == DT_NIL )
#define IS_PAIR(x)                              \
    ( (x)->type == DT_PAIR || (x)->type == DT_NIL )
#define IS_NUMBER(x)                            \
    ( (x)->type == DT_NUMBER )
#define IS_SYMBOL(x)                            \
    ( (x)->type == DT_SYMBOL )
#define IS_DATAERROR(x)                         \
    ( (x)->type == DT_DATAERROR )
#define IS_STRING_TYPE(x)                       \
    ( (x)->type == DT_STRING ||                 \
      (x)->type == DT_VARIABLE ||               \
      (x)->type == DT_SYMBOL ||                 \
      (x)->type == DT_DATAERROR )
#define CAR(x)                                  \
    ( (x)->data.pair.car )
#define CDR(x)                                  \
    ( (x)->data.pair.cdr )

#define RETURN_ERROR(arg, message)                              \
    {                                                           \
        return data_create((arg)->filename,                     \
                           (arg)->line_num,                     \
                           DT_DATAERROR,                        \
                           (void*)(message),                    \
                           NULL,                                \
                           NULL);                               \
    }

#define CHECK_ARITY(x)                                          \
    {                                                           \
        int count = 0;                                          \
        data_t *tmp = args;                                     \
        while(! IS_NIL(tmp)) {                                  \
            ++count;                                            \
            tmp = CDR(tmp);                                     \
        }                                                       \
        if( (x) != count ) {                                    \
            RETURN_ERROR(args, L"wrong number of arguments");    \
        }                                                       \
    }

#define MIN_ARITY(x)                                            \
    {                                                           \
        int count = 0;                                          \
        data_t *tmp = args;                                     \
        while(! IS_NIL(tmp)) {                                  \
            ++count;                                            \
            tmp = CDR(tmp);                                     \
        }                                                       \
        if( (x) > count ) {                                     \
            RETURN_ERROR(args, L"wrong number of arguments");    \
        }                                                       \
    }

#define RETURN_NUMBER(origin, val)                               \
    {                                                            \
        return data_create((origin)->filename,                   \
                           (origin)->line_num,                   \
                           DT_NUMBER,                            \
                           &(val),                               \
                           NULL,                                 \
                           NULL);                                \
    }

#define RETURN_STRING(origin, val)                               \
    {                                                            \
        return data_create((origin)->filename,                   \
                           (origin)->line_num,                   \
                           DT_STRING,                            \
                           val,                                  \
                           NULL,                                 \
                           NULL);                                \
    }

#define EMPTY_PAIR(orig)                        \
    data_create((orig)->filename,               \
                (orig)->line_num,               \
                DT_PAIR,                        \
                DataNIL,                        \
                DataNIL,                        \
                NULL);
        
#endif  /* __MACROS_H__ */
