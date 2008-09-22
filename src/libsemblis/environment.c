#include <assert.h>
#include <wchar.h>
#include <stdio.h>

#include "environment.h"
#include "data.h"
#include "macros.h"
#include "gc.h"

/* global definitions */

environment_t **EnvironmentList;
int MaxNumEnvironments;
int EnvironmentsInUse;

void env_init(int max_num_environments)
{
    int i;

    MaxNumEnvironments = max_num_environments;

    EnvironmentList = malloc(sizeof(*EnvironmentList) * MaxNumEnvironments);
   
    for(i=0; i<MaxNumEnvironments; i++)
        EnvironmentList[i] = NULL;

    EnvironmentsInUse = 0;
}

void env_close(void)
{
    free(EnvironmentList);
}

//hashtable_t *env_binding;

/** Size function for the hashtable used in the environment bindings.
 * Must return the number of bytes to malloc, this is also counting
 * the ending null character */
 unsigned int env_str_size_func(void *info)
{
    return (unsigned int)sizeof(wchar_t) * (wcslen(info) + 1);
}

/** Copy function for the hashtable used in the environment bindings.
 * This is the number of wide-characters to copy over, size is the
 * result of the length function above and so must be divided by the
 * size of a wchar. */
void* env_str_copy_func(void *dest, void *src, size_t size)
{
#ifdef WIN32
	return memcpy(dest, src, size);
#else
	return wmemcpy(dest, src, size/(sizeof(wchar_t)));
#endif
}

/** Key equal function for the hashtable used in the environment
 * bindings.  This is the wchar equivalent of the char equal function,
 * similar to wcscmp. */
bool env_str_equal_func(void *key1, void *key2)
{
    String str1;
    String str2;
    
    if(key1 == NULL || key2 == NULL)
	return false;

    str1 = key1;
    str2 = key2;

    if(wcscmp(str1, str2) == 0)
	return true;

    return false;
}

static unsigned int data_size_func(void *info)
{
    return (unsigned int)sizeof(data_t);
}

#if 0
static void* recursive_data_copy_func(void *dest, void *src, size_t size)
{
    data_t* dest_d = dest;

    memcpy(dest_d, src, size);

    /* copy string if necessary */
    if(data_is_string_type(dest_d)) {
        String s = data_get_string(dest_d);
        String s2 = malloc(sizeof(*s2) * (wcslen(s) + 1));
        wcscpy(s2, s);
        data_set_string(dest_d, s2);
    }
    
    /* do recursive copy */
    if(data_is_cons_type(dest_d)) {
        data_t *d1 = eng_get_new_block();
        data_t *d2 = eng_get_new_block();

        recursive_data_copy_func(d1, data_get_car(src), size);
        data_set_car(dest_d, d1);

        recursive_data_copy_func(d2, data_get_cdr(src), size);
        data_set_cdr(dest_d, d2);
    }

    return dest_d;
}
#endif

static environment_t *create(environment_t *parent) {
    environment_t *env = malloc(sizeof(*env));

    env->bindings = hashtable_create(INITIAL_ENV_BINDING_SIZE,
                                      HT_COPY_KEY,
                                      hashtable_jenkins_hash_wchar,
                                      env_str_size_func,
                                      env_str_copy_func,
                                      env_str_equal_func,
                                      NULL,
                                      NULL);
    env->marked = false;
    env->parent = parent;

    return env;
}
        
environment_t *env_create(environment_t *parent)
{
    static int next_env_spot = 0;
    
    environment_t *env = create(parent);

    if(EnvironmentsInUse == MaxNumEnvironments) {
        /* hit max, trigger garbage collection */
        gc_run_with_env(env);

        if(EnvironmentsInUse == MaxNumEnvironments) {
            printf("max number of environments hit\n");
            exit(1);
        }
    }

    /* find spot in env list to insert this */
    do {
        if(EnvironmentList[next_env_spot] == NULL) {
            EnvironmentList[next_env_spot++] = env;
            next_env_spot %= MaxNumEnvironments;
            break;
        }
        ++next_env_spot;
        next_env_spot %= MaxNumEnvironments;
    } while(1);

    ++EnvironmentsInUse;

    return env;
}

environment_t *env_create_no_gc(environment_t *parent)
{
    return create(parent);
}

static void destroy(environment_t *env) {
    assert(env);

    hashtable_destroy(env->bindings);

    free(env);
}

void env_destroy(environment_t *env)
{
    destroy(env);
    
    --EnvironmentsInUse;
}

void env_destroy_no_gc(environment_t *env)
{
    destroy(env);
}

environment_t* env_getparent(environment_t *env)
{
    assert(env);

    return env->parent;
}

void env_setparent(environment_t *env, environment_t *parent)
{
    assert(env);
    
    env->parent = parent;
}

data_t* env_get(environment_t *env, String name, bool recurse)
{
    data_t *possible_data;

    assert(env);
    
    possible_data = hashtable_get(env->bindings, name);

    if(possible_data)
        return possible_data;

    if(recurse && env->parent)
        return env_get(env->parent, name, true);

    return NULL;
}

void env_set(environment_t *env, String name, data_t* value)
{
	data_t *old_slot;

    assert(env);
    assert(value);
    assert(name);

    /* TODO: do a recursive copy here of the data tree and name */

    /* Only look for an old slot in the _current_ environment, not
     * parent environments or this will break the set in that
     * case. For instance if "i" were a variable inside of a new
     * environment we wish to set, then if "i" is found in a higher
     * parent environment it will be destroyed first. */
//    old_slot = hashtable_get(env->bindings, name);
//    if(old_slot)
//        data_destroy(old_slot);

    hashtable_put(&env->bindings, name, value);
}

void env_set_marked(environment_t *env, bool type)
{
    assert(env);
    
    if(env->marked != type) {
        env->marked = type;
        env_mark_bindings(env, type);
    }

    if(env->parent && env->parent->marked != type) {
        env_set_marked(env->parent, type);
        env_mark_bindings(env->parent, type);
    }        
}

bool env_get_marked(environment_t *env)
{
    assert(env);

    return env->marked;
}

void env_mark_bindings(environment_t *env, bool type)
{
    unsigned int i;
    hashtable_entry_t *entry;
    
    if(! env)
        return;

    for(i=0; i<env->bindings->capacity; ++i) {
        entry = env->bindings->elements[i];
        
        if(entry->key) {
            data_t *slot = (data_t *)entry->value;
            data_set_marked_recursive(slot, type);
        }
    }
}

int env_get_size(environment_t *env)
{
    assert(env);

    return env->bindings->length;
}
