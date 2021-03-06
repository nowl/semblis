extern "C" {
#include "ForeignMacros.h"
}

#include <wchar.h>
#include <vector>
#include <map>

using namespace std;

static environment_t *main_env;
static vector<environment_t*> stored_envs;
static vector<data_t*> data_objs;
static map<environment_t *, environment_t *> store_visited;
static map<environment_t *, environment_t *> restore_visited;

static void destroy_objs() {
    env_destroy_no_gc(main_env);
    for(unsigned int i=0; i<data_objs.size(); i++)
        free(data_objs[i]);
    data_objs.clear();
    for(unsigned int i=0; i<stored_envs.size(); i++)
        env_destroy_no_gc(stored_envs[i]);    
    stored_envs.clear();

    main_env = NULL;
}

static data_t *data_store_recursive(data_t *data);
static data_t *data_restore_recursive(data_t *data);

static environment_t *env_store_recursive(environment_t *env) {
    environment_t *new_env = env_create_no_gc(env->parent);
    unsigned int i;
    hashtable_entry_t *entry;

    store_visited[env] = new_env;

    for(i=0; i<env->bindings->capacity; ++i) {
        entry = env->bindings->elements[i];
        
        if(entry->key) {
//            printf("storing %ls\n", entry->key);
            data_t *slot = (data_t *)entry->value;
            data_t *new_slot = NULL;

            new_slot = data_store_recursive(slot);
            
            env_set(new_env,
                    (String)entry->key,
                    new_slot);
        }
    }

    return new_env;
}

static environment_t *env_restore_recursive(environment_t *env, bool global_env) {
    environment_t *new_env = env_create(env->parent);
    
    if(global_env)
        GlobalEnv = new_env;
    
    unsigned int i;
    hashtable_entry_t *entry;

    restore_visited[env] = new_env;
//    printf(">>mapping new environment %x to old environment %x\n", env, new_env);

    
    for(i=0; i<env->bindings->capacity; ++i) {
        entry = env->bindings->elements[i];
        
        if(entry->key) {
//            printf("restoring: %ls\n", entry->key);
            data_t *slot = (data_t *)entry->value;
            data_t *new_slot = data_restore_recursive(slot);
            
            env_set(new_env,
                    (String)entry->key,
                    new_slot);
        }
    }

    return new_env;
}

static data_t *data_store_recursive(data_t *data) {
    /* Just make a copy of all data blocks. If any happen to be pairs
     * then change the address pointed to by those blocks along with
     * the copy.
     */
    
    data_t *new_data = NULL, *arg1, *arg2;
    environment_t *env = NULL;

    new_data = (data_t*)malloc(sizeof(*new_data));

    new_data->marked = false;
    new_data->filename = data->filename;
    new_data->line_num = data->line_num;
    new_data->type = data->type;
        
    switch(data->type) {
    case DT_STRING:
    case DT_SYMBOL:
    case DT_VARIABLE:
    case DT_DATAERROR:
    {
        wchar_t *val = data->data.text;
        int len = wcslen(val) + 1;
        new_data->data.text = (wchar_t*)malloc(len * sizeof(wchar_t));
        wcscpy(new_data->data.text, val);
        break;
    }
    case DT_PROCEDURE:
        if(store_visited.count((environment_t*)data->data.procedure.env) == 0) {
            env = env_store_recursive((environment_t*)data->data.procedure.env);
            stored_envs.push_back(env);
        } else {
            env = store_visited[(environment_t*)data->data.procedure.env];
        }
        new_data->data.procedure.env = env;        
    case DT_MACRO:
        arg1 = data_store_recursive(data->data.procedure.code);
        arg2 = data_store_recursive(data->data.procedure.args);
        new_data->data.procedure.args = arg2;
        new_data->data.procedure.code = arg1;
        break;
    case DT_NUMBER:
        new_data->data.number = data->data.number;
        break;
    case DT_PAIR:
        arg1 = data_store_recursive(CAR(data));
        arg2 = data_store_recursive(CDR(data));
        CAR(new_data) = arg1;
        CDR(new_data) = arg2;
        break;
    case DT_NIL:
    case DT_T:
        break;
    case DT_POINTER:
        new_data->data.ptr = data->data.ptr;
    default:
        break;
    }

    data_objs.push_back(new_data);

    return new_data;
}

static data_t *data_restore_recursive(data_t *data) {
    data_t *new_data = NULL, *car, *cdr;
    environment_t *env;

    switch(data->type) {
    case DT_STRING:
    case DT_SYMBOL:
    case DT_VARIABLE:
    case DT_DATAERROR:
        new_data = data_create(data->filename, data->line_num, data->type, data->data.text, NULL, NULL);
        break;
    case DT_PROCEDURE: 
//        printf("checking for %x\n", data->data.procedure.env);
        if(restore_visited.count((environment_t*)data->data.procedure.env) == 0) {
            env = env_restore_recursive((environment_t*)data->data.procedure.env, false);
//            printf("  not found, creating and setting mapping to %x\n", env);
        } else {
            env = restore_visited[(environment_t*)data->data.procedure.env];
//            printf("  found, setting mapping to %x\n", env);
        }
        car = data_restore_recursive(data->data.procedure.args);
        cdr = data_restore_recursive(data->data.procedure.code);
        new_data = data_create(data->filename, data->line_num, DT_PROCEDURE, car, cdr, env);
        break;
    case DT_MACRO:
        car = data_restore_recursive(data->data.procedure.args);
        cdr = data_restore_recursive(data->data.procedure.code);
        new_data = data_create(data->filename, data->line_num, DT_MACRO, car, cdr, NULL);
        break;
    case DT_NUMBER:
        new_data = data_create(data->filename, data->line_num, DT_NUMBER, &data->data.number, NULL, NULL);
        break;
    case DT_PAIR:
        car = data_restore_recursive(CAR(data));
        cdr = data_restore_recursive(CDR(data));
        new_data = data_create(data->filename, data->line_num, DT_PAIR, car, cdr, NULL);
        break;
    case DT_NIL:
        new_data = DataNIL;
        break;
    case DT_T:
        new_data = DataTrue;
        break;
    case DT_POINTER:
        new_data = data_create(data->filename, data->line_num, DT_POINTER, data->data.ptr, NULL, NULL);
        break;
    }

    return new_data;
}

FOREIGN_FUNCTION(env_restore_plugin_save) {
    _START("env_restore_plugin_save");
    FF_START;

    if(main_env)
        destroy_objs();

    store_visited.clear();

    main_env = env_store_recursive(GlobalEnv);

    FF_RET_TRUE;
}

FOREIGN_FUNCTION(env_restore_plugin_restore) {
    _START("env_restore_plugin_restore");
    FF_START;

    if(!main_env)
        FF_RET_FALSE;

    // remove old environment
//    env_destroy(GlobalEnv);
    
    // restore old environment
    restore_visited.clear();

//    printf("global env: %x\n", GlobalEnv);

    env_restore_recursive(main_env, true);

//    printf("global env: %x\n", GlobalEnv);

    environment_t **new_envs = (environment_t **)malloc(sizeof(*new_envs) * store_visited.size());
    environment_t **old_envs = (environment_t **)malloc(sizeof(*old_envs) * store_visited.size());

    int i = 0;
    map<environment_t *, environment_t *>::iterator iter = store_visited.begin();
    for(; iter != store_visited.end(); iter++, i++) {
        old_envs[i] = iter->first;
        new_envs[i] = restore_visited[iter->second];

//        printf("mapping: %x to %x\n", old_envs[i], new_envs[i]);
    }

//    printf("total of %d entries\n", i);

    eval_set_new_pointers(old_envs, new_envs, i);

    free(new_envs);
    free(old_envs);

    // set environment in eval
//    eval_set_register(RT_ENV, GlobalEnv);

    // free up used space
//    destroy_objs();

    FF_RET_TRUE;
}
