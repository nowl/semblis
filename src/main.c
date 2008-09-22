#include <stdio.h>
#include <string.h>

#include "semblis.h"

int main(int argc, char* argv[])
{
    reader_node_t *root;
    data_t *result;
    char *filename;

    if(argc == 1)
    {
        printf("\nusage: semblis <filename>\n\n");
        return 1;
    }

    /* initialization */
    semblis_init();

    /* set search dir */
    eng_add_search_dir(".");

    /* reading */
    filename = argv[1];
    root = eng_read_file(filename);
    
    /* evaluating */
    result = eng_eval(root);

//    pretty_print_env(GlobalEnv);

    /* display result */
    printf(" -->\n");
    pretty_print(result);

    /* shutdown */
    semblis_shutdown();

    return 0;
}
