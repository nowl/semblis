#ifndef __PRIMS_H__
#define __PRIMS_H__

#include <stdlib.h>

#include "data.h"
#include "hashtable.h"

typedef data_t *PrimFunc(data_t* args);

#define PRIM_FUNCTION(name) data_t *(name)(data_t *args)

DLL_INFO void prims_init(void);
DLL_INFO void prims_destroy(void);
DLL_INFO PrimFunc *prims_get(String name);

#define REGISTER_FUNC(type, name, func)                                 \
    {                                                                   \
        char msg[256];                                                  \
        char *msg_p = (char *)msg;                                      \
        snprintf(msg_p, 256, "registering "type" function: %ls", (name)); \
        logger_log((type), LL_INFO, msg_p);                             \
        hashtable_put(&reg_table, (wchar_t*)(name), (void*)(func));             \
    }                                                                   

#if 0

DLL_INFO extern String default_error_message;

#define MAX_STRING_LEN (1<<16)

/* checks if the given Data* is an error object and if so returns
 * immediately to the caller */
#define ERROR_CHECK_AND_RETURN(data)                                 \
    if(! (data)) {                                                   \
        return eng_create_error(default_error_message);              \
    }                                                                \
    if( data_is_error(data) ) {                                      \
        if(Engine->kill_if_error) {                                  \
            char tmp[MAX_STRING_LEN];                                \
            wcstombs(tmp, data_get_string(data), MAX_STRING_LEN);    \
            OUTPUT_ERR("%s", tmp);                                   \
            OUTPUT_ERR(" > error at %s:%d\n",                        \
                       data_get_filename(data),                      \
                       data_get_line_num(data));                     \
                                                                     \
            exit(1);                                                 \
        } else                                                       \
            return (data);                                           \
    }
#endif

#endif	/* __PRIMS_H__ */
