#ifndef __LIBLOADER_H__
#define __LIBLOADER_H__

#include "types.h"
#include "hashtable.h"
#include "windll.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef WIN32
# define LIBRARY_HANDLE HMODULE
#else
# define LIBRARY_HANDLE void*
#endif

#ifdef WIN32
DLL_INFO hashtable_t *reg_table;  /* declared in core_prims.c */
#else
DLL_INFO extern hashtable_t *reg_table;  /* declared in core_prims.c */
#endif

typedef struct {
    char *_mod_name;
    LIBRARY_HANDLE _prev_handle;
} LibLoader;


LibLoader *ll_create(void);
void ll_destroy(LibLoader *ll);

bool ll_set_library(LibLoader *ll, char *filename);
void ll_set_module_name(LibLoader *ll, char *mod_name);
bool ll_register_foreign_func(LibLoader *ll,
                              char *lisp_func,
                              char *c_func);

#endif	/* __LIBLOADER_H__ */
