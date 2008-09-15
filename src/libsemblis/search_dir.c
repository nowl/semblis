#include <string.h>

#include "semblis.h"

static char **search_dirs;
static unsigned int search_dirs_cap;
static int search_dirs_num;

DLL_INFO void eng_add_search_dir(char *dir)
{
    int i;
    int found_ind = -1;

    for(i=0; i<search_dirs_num; i++)
        if(! search_dirs[i]) {
            found_ind = i;
            break;
        }

    if(found_ind < 0) {
        search_dirs = util_grow_buffer_to_size(search_dirs,
                                               &search_dirs_cap,
                                               search_dirs_num+1,
                                               sizeof(*search_dirs));
        found_ind = search_dirs_num++;
    }

    i = strlen(dir) + 1;

    search_dirs[found_ind] = malloc(i);
    strcpy(search_dirs[found_ind], dir);
}

DLL_INFO bool eng_rem_search_dir(char *dir)
{
    int i;
    bool removed = false;
    
    for(i=0; i<search_dirs_num; i++)
        if(search_dirs[i] && strcmp(search_dirs[i], dir) == 0) {
            free(search_dirs[i]);
            search_dirs[i] = NULL;
            removed = true;
            i++;
            break;
        }

    /* move others back */
    for(; i<search_dirs_num; i++) {
        search_dirs[i-1] = search_dirs[i];

        if(i == search_dirs_num-1)
            search_dirs[i] = NULL;
    }

    return removed;
}

DLL_INFO char **eng_get_search_dirs(void)
{
    return search_dirs;
}

DLL_INFO int eng_get_num_search_dirs(void)
{
    return search_dirs_num;
}

void search_dir_destroy(void)
{
    int i;

    for(i=0; i<search_dirs_num; i++)
        if(search_dirs[i])
            free(search_dirs[i]);

    free(search_dirs);
}
