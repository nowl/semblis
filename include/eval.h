#ifndef __EVAL_H__
#define __EVAL_H__

#include "reader.h"
#include "data.h"
#include "environment.h"
#include "windll.h"

extern DLL_INFO environment_t *GlobalEnv;

/*
typedef enum {
    RT_VAL,
    RT_EXP,
    RT_ARGL,
    RT_UNEV,
    RT_PROC,
    RT_ENV,
    RT_CODE_POINTER,
    RT_TOP_CODE_POINTER,
    RT_CONT,
    RT_PROC_PRIM,
    RT_LAST_ARG_POINTER
} RegType;

void eval_set_register(RegType type, void *setting);
*/

int eval_init();
void eval_destroy();

void eval_set_error(data_t *error);

void eval_gather_gc(void (*add_data_cb)(data_t *),
                    void (*add_env_cb)(environment_t *));

data_t *eval(reader_node_t *code);

DLL_INFO void eval_set_new_pointers(environment_t **old_envs, environment_t **new_envs, int num);

DLL_INFO short eval_get_current_val_filename(void);
DLL_INFO short eval_get_current_val_line_num(void);

#endif	/* __EVAL_H__ */
