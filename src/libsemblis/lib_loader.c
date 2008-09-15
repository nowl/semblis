#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "lib_loader.h"
#include "logger.h"
#include "prims.h"
#include "utils.h"
#include "macros.h"

#ifdef WIN32
# define LIBRARY_OPEN(lib)            LoadLibrary( (lib) )
# define LIBRARY_GETSYM(handle, sym)  GetProcAddress( (handle), (sym) )
# define LIBRARY_CLOSE(handle)        FreeLibrary( (handle) )
//# define LIBRARY_ERROR                GetLastError
#else
# define LIBRARY_OPEN(lib)            dlopen( (lib), RTLD_LAZY | RTLD_GLOBAL)
# define LIBRARY_GETSYM(handle, sym)  dlsym( (handle), (sym) )
# define LIBRARY_CLOSE(handle)        dlclose( (handle) )
//# define LIBRARY_ERROR                dlerror
#endif


static LIBRARY_HANDLE *loaded_libs;
static unsigned int loaded_libs_cap;
static int loaded_libs_size;

LibLoader *ll_create(void)
{
    LibLoader *lib;

    lib = malloc(sizeof(*lib));

    return lib;
}

void ll_destroy(LibLoader *lib)
{
    unsigned int i;

    /* close all the libraries */
    for(i=0; i<loaded_libs_size; i++) {
        LIBRARY_HANDLE handle = loaded_libs[i];
        LIBRARY_CLOSE(handle);            
    }
	
    free(loaded_libs);
    
    free(lib);
}

bool ll_set_library(LibLoader *ll, char *filename)
{
    LIBRARY_HANDLE handle;

    handle = LIBRARY_OPEN(filename);
    
    if( !handle) {
#ifdef WIN32
        OUTPUT_ERR("problem opening library\n");
#else 
        OUTPUT_ERR("problem opening library: %s\n", dlerror());
#endif
        return false;
    }

    loaded_libs = util_grow_buffer_to_size(loaded_libs,
                                           &loaded_libs_cap,
                                           loaded_libs_size+1,
                                           sizeof(*loaded_libs));

    loaded_libs[loaded_libs_size++] = handle;

    ll->_prev_handle = handle;

    return true;
}

void ll_set_module_name(LibLoader *ll, char *mod_name)
{
    ll->_mod_name = mod_name;
}

#define MAX_FUNCTION_LEN 256

bool ll_register_foreign_func(LibLoader *ll,
                              char *lisp_func,
                              char *c_func)
{
    PrimFunc *func;

    func = (PrimFunc *)LIBRARY_GETSYM(ll->_prev_handle, c_func);

#ifndef WIN32
    {
        char *error;

        if((error = dlerror()) != NULL) {
            OUTPUT_ERR("problem finding symbol: %s\n", error);
            return false;
        }
    }
#else
    if(func == NULL) {
        OUTPUT_ERR("problem opening function: %s in library\n", c_func);
        return false;
    }
#endif

    if(strcmp(ll->_mod_name, "None") == 0) {
        /* need to first convert to String */
        String tmp_w[MAX_FUNCTION_LEN];
        mbsrtowcs((wchar_t *)tmp_w,
                  (const char **)&lisp_func,
                  MAX_FUNCTION_LEN,
                  NULL);

        REGISTER_FUNC("foreign", (wchar_t*)tmp_w, func);
    } else {
        char tmp[MAX_FUNCTION_LEN];
        char *tmp_p = tmp;
        String tmp_w[MAX_FUNCTION_LEN];

        snprintf(tmp_p, MAX_FUNCTION_LEN,
                 "%s:%s", ll->_mod_name, lisp_func);

        /* need to first convert to String */
        mbsrtowcs((wchar_t*)tmp_w,
                  (const char **)&tmp_p,
                  MAX_FUNCTION_LEN,
                  NULL);

        REGISTER_FUNC("foreign", (wchar_t*)tmp_w, func);
    }
    
    return true;
}
