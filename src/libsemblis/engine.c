#include <stdio.h>
#include <assert.h>
#include <wctype.h>
#include <string.h>

#include "semblis.h"

static OutputFuncType **output_function_list;
static unsigned int output_function_list_cap;
static int output_function_list_size;

static int max_num_blocks;
static int max_num_environments;
static bool show_engine_stats;
static bool show_garbage_collection_stats;
OutputFunc output_std_func;
OutputFunc output_err_func;
static bool kill_if_error;

/* split and trim result variables */
static char **split_result_frags;
static int split_result_num;
static unsigned int _split_result_frags_cap;

static LibLoader *lib_loader;
static Config *config_main;

DLL_INFO bool semblis_init(void)
{
    /* engine vars */
    max_num_blocks = 50000;
    max_num_environments = 2048;
    show_engine_stats = true;
    show_garbage_collection_stats = false;
    output_std_func = output_err_func = console_output;
    kill_if_error = true;

    /* logger */
    logger_init("semblis.log");

    /* lib loader */
    lib_loader = ll_create();

    /* output functions */
    eng_add_output_func("console", console_output);

    /* primitives */
    prims_init();

    /* config */
    config_main = config_create("main config");
    eng_read_default_config();

    /* eval */
    eval_init();

    return true;
}

DLL_INFO void semblis_shutdown(void)
{
    int i;

    if(show_engine_stats)
        eng_print_engine_stats();

    /* search directories */
    search_dir_destroy();

    /* eval */
    eval_destroy();
    destroy_filenames();

    /* config */
    config_destroy(config_main);

    /* primitives */
    prims_destroy();

    /* lib loader */
    ll_destroy(lib_loader);

    /* logger */
    logger_close();

    /* output functions */
    if(output_function_list)
        free(output_function_list);

    /* split functions */
    for(i=0; i<split_result_num; i++)
        free(split_result_frags[i]);
    free(split_result_frags);
}

DLL_INFO void eng_print_engine_stats(void)
{
    OUTPUT_REG("---------- Engine Stats ----------\n");
    OUTPUT_REG("total blocks in use: %d\n", BlocksInUse);
    OUTPUT_REG("size per block: %d\n", sizeof(data_t));
    OUTPUT_REG("extra environments in use: %d\n", EnvironmentsInUse);
    OUTPUT_REG("size per environment: %d\n", sizeof(environment_t));
    OUTPUT_REG("----------------------------------\n");
}

static bool true_accept(char *ls) {
    switch(strlen(ls)) {
    case 4:
        if(towlower(ls[0]) == 't'
           && towlower(ls[1]) == 'r'
           && towlower(ls[2]) == 'u'
           && towlower(ls[3]) == 'e')
            return true;
    case 3:
        if(towlower(ls[0]) == 'y'
           && towlower(ls[1]) == 'e'
           && towlower(ls[2]) == 's')
            return true;
    case 1:
        if(towlower(ls[0]) == 't'
           || towlower(ls[0]) == 'y')
            return true;
    default:
        break;
    }
    
    return false;
}

DLL_INFO void eng_add_output_func(char *name, OutputFunc func)
{
    OutputFuncType *oft = output_func_create(name, func);

    output_function_list = util_grow_buffer_to_size(output_function_list,
                                                    &output_function_list_cap,
                                                    output_function_list_size+1,
                                                    sizeof(*output_function_list));

    output_function_list[output_function_list_size++] = oft;
}

static OutputFunc get_output_func(char *func_name)
{
    /* search for the given function in the known output function
     * list */
    int i;

    for(i=0; i<output_function_list_size; i++) {
        OutputFuncType *oft = output_function_list[i];
        if(strcmp(oft->func_name, func_name) == 0) {
            return oft->func;
        }
    }

    return console_output;
}

static void process_core_property(Property *prop)
{
    char *key = prop->key;

    if(strcmp(key, "BlocksUntilGarbageCollection") == 0) {
        int num_blocks = atoi(prop->value);
        num_blocks = (num_blocks < 50000) ? 50000 : num_blocks;
        max_num_blocks = num_blocks;
    } else if(strcmp(key, "EnvironmentsUntilGarbageCollection") == 0) {
        int num_envs = atoi(prop->value);
        num_envs = (num_envs < 2048) ? 2048 : num_envs;
        max_num_environments = num_envs;
    } else if(strcmp(key, "ShowEngineStats") == 0) {
        show_engine_stats = true_accept(prop->value);
    } else if(strcmp(key, "ShowGarbageCollectionStats") == 0) {
        show_garbage_collection_stats = true_accept(prop->value);
    } else if(strcmp(key, "StandardOutputFunction") == 0) {
        output_std_func = get_output_func(prop->value);
    } else if(strcmp(key, "ErrorOutputFunction") == 0) {
        output_err_func = get_output_func(prop->value);
    } else if(strcmp(key, "KillWhenErrorHit") == 0) {
        kill_if_error = true_accept(prop->value);
    } else {
        char error[512];
        snprintf(error, 512, "unable to handle property: %s\n", (char *)key);
        logger_log("config", LL_WARNING, error);
    }
}

static char *trim(char *str) {
    char *trimmed_str = malloc(sizeof(char) * (strlen(str) + 1));

    int str_ind = 0;
    char c = -1;

    /* find beginning char */
    while(c != 0) {
        c = str[str_ind++];
	
        if(c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != 0) {
            strcpy(trimmed_str, &str[str_ind-1]);
            break;
        }
    }
	
    /* find ending point */
    str_ind = (int)strlen(str) - str_ind;
    while(true) {
        c = trimmed_str[str_ind--];
	
        if(c != ' ' && c != '\n' && c != '\r' && c != '\t' && c != 0) {
            trimmed_str[str_ind+2] = 0;
            break;
        }
    }

    return trimmed_str;
}

static void split_and_trim(char *str, char split_char)
{
    int start = 0;
    unsigned int start_ind = 0;
    char c = -1;
    int i;

    for(i=0; i<split_result_num; i++)
        free(split_result_frags[i]);
    split_result_num = 0;

    while(c != 0) {
        c = str[start++];

        if(c == split_char || c == 0) {
            char *new_string, *result;
            int result_size;

            str[start-1] = 0;

            result = trim(&str[start_ind]);
            result_size = strlen(result) + 1;
            new_string = malloc(sizeof(*new_string) * result_size);
            memcpy(new_string, result, result_size);

            split_result_frags = util_grow_buffer_to_size(split_result_frags,
                                                          &_split_result_frags_cap,
                                                          split_result_num+1,
                                                          sizeof(*split_result_frags));

            split_result_frags[split_result_num] = new_string;
            split_result_num++;

            start_ind = start;
        }
    }
}

static void process_library_property(Property *prop)
{
    char *key, *val;

    key = prop->key;
    val = prop->value;

    if(strcmp(key, "Include") == 0)
        eng_read_config_file(val);
    else if(strcmp(key, "File") == 0)
        ll_set_library(lib_loader, val);
    else if(strcmp(key, "Module") == 0)
        ll_set_module_name(lib_loader, val);
    else if(strcmp(key, "Definition") == 0) {
        split_and_trim(val, ',');
        if(split_result_num == 2) {
            ll_register_foreign_func(lib_loader,
                                     split_result_frags[0],
                                     split_result_frags[1]);
                                
        }
    } else {
        char error[512];
        snprintf(error, 512, "unable to handle property: %s\n", key);
        logger_log("config", LL_WARNING, error);
    }
}

static void process_semblis_core(Section *section) {
    int i;

    for(i=0; i<section->_properties_num; ++i) {
        Property *prop = section->_properties[i];
        process_core_property(prop);
    }
}

static void process_library_definition(Section *section) {
    int i;

    for(i=0; i<section->_properties_num; ++i) {
        Property *prop = section->_properties[i];
        process_library_property(prop);
    }
}

static void process_section(Section *section)
{
    if(strcmp(section_name(section), "LibraryDefinition") == 0)
        process_library_definition(section);
    else if(strcmp(section_name(section), "SemblisCore") == 0)
        process_semblis_core(section);
}

void eng_process_config(Config *config)
{
    int i;

    for(i=0; i<config->_num_sections; i++)
        process_section(config->_sections[i]);
}

DLL_INFO void eng_read_default_config()
{
    config_read_default_config(config_main);
    eng_process_config(config_main);
}

DLL_INFO void eng_read_config_file(char *filename)
{
    Config *conf = config_create(filename);
    config_read_config(conf, filename);
    eng_process_config(conf);
    config_destroy(conf);
}

static reader_node_t *run_post_read(reader_node_t *reader) {
    if(!reader)
        return NULL;

    classifier_run(reader);

    char *error_stream = get_output_stream_error();
    if(error_stream)
        OUTPUT_REG("ERRORS:\n%s", error_stream);

    char *output_stream = get_output_stream();
    if(output_stream)
        OUTPUT_ERR("%s", output_stream);

    return reader;
}

DLL_INFO reader_node_t *eng_read_file(char *filename)
{
    reader_node_t *reader;

    reader = reader_read_file(filename);

    return run_post_read(reader);
}

DLL_INFO reader_node_t *eng_read_string(char *string)
{
    reader_node_t *reader = reader_read_string(string);

    return run_post_read(reader);
}

DLL_INFO data_t *eng_eval(reader_node_t *node)
{
    return eval(node);
}

DLL_INFO int eng_get_blocks_in_use(void)
{
    return BlocksInUse;
}

DLL_INFO int eng_get_envs_in_use(void)
{
    return EnvironmentsInUse;
}

/*
DLL_INFO void eng_set_root_directory(char *dir)
{
    if(Engine->root_directory)
        free(Engine->root_directory);

        // make a copy of the directory
    Engine->root_directory = malloc(strlen(dir) + 1);

    memcpy(Engine->root_directory, dir, strlen(dir) + 1);
}
*/

DLL_INFO void *eng_get_prop(char *prop)
{
    if(strcmp(prop, "show_garbage_collection_stats") == 0)
        return (void *)show_garbage_collection_stats;
    else if(strcmp(prop, "show_engine_stats") == 0)
        return (void *)show_engine_stats;
    else if(strcmp(prop, "kill_if_error") == 0)
        return (void *)kill_if_error;
    else if(strcmp(prop, "max_num_blocks") == 0)
        return (void *)max_num_blocks;
    else if(strcmp(prop, "max_num_environments") == 0)
        return (void *)max_num_environments;

    return NULL;
}
