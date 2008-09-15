#ifndef __FOREIGN_MACROS_H__
#define __FOREIGN_MACROS_H__

#include "semblis.h"
#include "prims.h"

//////////////////////////////////////////////////////////////////////
// some helper macros for writing plugins
//////////////////////////////////////////////////////////////////////

#define FOREIGN_FUNCTION(name)			\
    extern "C" DLL_EXPORT PRIM_FUNCTION( (name) )

#define FF_START                                \
    data_t *arg = CAR(args);                    \
    if(IS_DATAERROR(arg))                       \
        return arg;

#define FF_GET_NEXT_ARG                         \
    args = CDR(args);                           \
    arg = CAR(args);                            \
    if(IS_DATAERROR(arg))                       \
        return arg;

#define FF_IS_POINTER   (IS_POINTER(arg))
#define FF_IS_NUMBER    (IS_NUMBER(arg))
#define FF_IS_STRING    (IS_STRING_TYPE(arg))
#define FF_IS_NIL       (IS_NIL(arg))
#define FF_IS_FALSE     (IS_NIL(arg))
#define FF_IS_TRUE      (!IS_NIL(arg))

#define FF_RET_ERROR(err)    RETURN_ERROR(arg, ( err ));
#define FF_RET_NUMBER(x)     RETURN_NUMBER(arg, ( x ));
#define FF_RET_STRING(x)     RETURN_STRING(arg, ( x ));
#define FF_RET_TRUE          return DataTrue;
#define FF_RET_FALSE         return DataNIL;

#define FF_CREATE_PAIR	       	data_create(-1, 0, DT_PAIR, DataNIL, DataNIL, NULL)
#define FF_CREATE_NUMBER(x)    	data_create(-1, 0, DT_NUMBER, (void*) (x), NULL, NULL)
#define FF_CREATE_STRING(x)    	data_create(-1, 0, DT_STRING, (x), NULL, NULL)
#define FF_CREATE_ERROR(x)	data_create(-1, 0, DT_DATAERROR, (void*) (x), NULL, NULL)
#define FF_CREATE_POINTER(x)   	data_create(-1, 0, DT_POINTER, (x), NULL, NULL)

#endif	// __FOREIGN_MACROS_H__
