#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include "types.h"
#include "hashtable.h"
#include "windll.h"
#include "data.h"

/* maps strings to Slots */
typedef hashtable_t *EnvBinding;

#define INITIAL_ENV_BINDING_SIZE 8

/* These are for copying the wide character strings for the
 * environment bindings. Needed by the underlying hashtable key
 * implementation. */
extern unsigned int	 env_str_size_func(void *info);
extern void         *env_str_copy_func(void *dest, void *src, size_t size);
extern bool          env_str_equal_func(void *key1, void *key2);

typedef struct environment_type {
    struct environment_type *parent;
    EnvBinding bindings;
    bool marked;
} environment_t;

/* API Start */

/** Global list of all environments in use. As environments are
 * created they aree added to the next non NULL location in this
 * global list. It is up to the garbage collector to maintain the list
 * and to destroy environments. */
extern environment_t **EnvironmentList;
extern int MaxNumEnvironments;
extern int EnvironmentsInUse;

void env_init(int max_num_environments);
void env_close(void);

/**
 * Create an environment.
 * @param parent is a pointer to the parent environment
 * @return a pointer to the new environment
 */
DLL_INFO environment_t *env_create(environment_t *parent);

/**
 * Create an environment that is not garbage collected.
 * @param parent is a pointer to the parent environment
 * @return a pointer to the new environment
 */
DLL_INFO environment_t *env_create_no_gc(environment_t *parent);

/**
 * Destroys an environment.
 * @param env is a pointer to the environment to be destroyed
 */
DLL_INFO void env_destroy(environment_t *env);

/**
 * Destroys an environment that is not garbage collected.
 * @param env is a pointer to the environment to be destroyed
 */
DLL_INFO void env_destroy_no_gc(environment_t *env);

/**
 * Gets the parent environment.
 * @param env is the environment of interest
 * @return the parent of the environment of interest
 */
DLL_INFO environment_t* env_get_parent(environment_t *env);

/**
 * Set the parent environment of the environment of interest.
 * @param env is the environment of interest
 * @param parent is the parent environment
 */
DLL_INFO void env_set_parent(environment_t *env, environment_t *parent);

/**
 * Attempts to find the slot with the given name in the
 * environment. If recurse is true then this will look at the parent
 * environment if not found in current environment.
 * @param env is the environment of interest
 * @param name is the slot name to look for
 * @param recurse to set whether to look at parent environments also
 * @return the matching slot or NULL
 */
DLL_INFO data_t* env_get(environment_t *env, String name, bool recurse);

/**
 * Sets the slot with the given name to slot in environment.
 * @param env is the environment of interest
 * @param name is the slot name to set
 * @param value is the slot to set
 */
DLL_INFO void env_set(environment_t *env, String name, data_t* value);

/**
 * Sets the environment to not be garbage collected.
 * @param env is the environment of interest
 * @param type is whether to set on or off
 */
void env_set_marked(environment_t *env, bool type);

/**
 * Gets whether the environment is to be garbage collected.
 * @param env is the environment of interest
 * @return whether environment is set
 */
bool env_get_marked(environment_t *env);

/**
 * Sets the bindings in the environment to not be garbage collected.
 * @param env is the environment of interest
 * @param type is whether to set on or off
 */
void env_mark_bindings(environment_t *env, bool type);

/**
 * Finds the number of bindings in the environment
 * @param env is the environment of interest
 * @return the number of bindings
 */
int env_get_size(environment_t *env);

/* API End */

#endif	/* __ENVIRONMENT_H__ */
