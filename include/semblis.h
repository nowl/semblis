#ifndef __SEMBLIS_H__
#define __SEMBLIS_H__

#include <stdlib.h>
#include <stdio.h>

#include "eval.h"
#include "types.h"
#include "environment.h"
#include "data.h"
#include "windll.h"
#include "config.h"
#include "lib_loader.h"
#include "reader.h"
#include "logger.h"
#include "output.h"
#include "macros.h"
#include "utils.h"
#include "classifier.h"
#include "prims.h"

//#define DO_TRACE

#ifdef WIN32
//# include "use_ansi.h"
#endif

DLL_INFO bool semblis_init(void);
DLL_INFO void semblis_shutdown(void);

DLL_INFO void eng_read_default_config();
DLL_INFO void eng_read_config_file(char *filename);

DLL_INFO void eng_add_output_func(char *name, OutputFunc func);

DLL_INFO void eng_print_engine_stats(void);
DLL_INFO int  eng_get_blocks_in_use(void);
DLL_INFO int  eng_get_envs_in_use(void);

DLL_INFO void eng_set_root_directory(char *dir);

void eng_process_config(Config *config);

/* abstract reader and eval functions */
#define ERF_DEFAULTS          0x0
#define ERF_SET_ROOT_DIR      0x1
#define ERF_NO_BYTECODE_WRITE 0x2

DLL_INFO reader_node_t *eng_read_file(char *filename);
DLL_INFO reader_node_t *eng_read_string(char *string);
DLL_INFO data_t *eng_eval(reader_node_t *node);

DLL_INFO void *eng_get_prop(char *prop);

DLL_INFO void eng_add_search_dir(char *dir);
DLL_INFO bool eng_rem_search_dir(char *dir);
DLL_INFO char **eng_get_search_dirs(void);
DLL_INFO int eng_get_num_search_dirs(void);
void search_dir_destroy(void);

#endif	/* __SEMBLIS_H___ */
