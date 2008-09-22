#ifndef __TYPES_H__
#define __TYPES_H__

#include <stddef.h>

#ifndef WIN32
#include <stdbool.h>
#else
#include <windows.h>
#define snprintf _snprintf
# ifndef __cplusplus
    typedef char bool;
#  define false 0
#  define true 1
# endif /* __cplusplus */
#endif /* WIN32 */


#include "hashtable.h"

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef signed char  int8;
typedef signed short int16;
typedef signed long  int32;

typedef wchar_t* String;

#endif	/* __TYPES_H__ */
