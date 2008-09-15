#include <stdio.h>

#include "environment.h"
#include "data.h"
#include "macros.h"
#include "eval.h"
#include "gc.h"
#include "semblis.h"

/* This can be absolutely turned off for speed, so the checks for
 * printing won't even happen at runtime. */
#define ENABLE_GC_STATS
//#define DEBUG_BLOCKLIST

static void mark_data(data_t *data) {
    if(data)
        data_set_marked_recursive(data, true);
}

static void mark_env(environment_t *env) {
    if(env)
        env_set_marked(env, true);
}

void gc_run(void)
{
    int i;

#ifdef ENABLE_GC_STATS
    if((bool)eng_get_prop("show_garbage_collection_stats")) {
        OUTPUT_REG("---------- before garbage collection ----------\n");
        OUTPUT_REG("blocks in use: %d\n", BlocksInUse);
        OUTPUT_REG("environments in use: %d\n", EnvironmentsInUse);
    }
#endif

#ifdef DEBUG_BLOCKLIST
    for(i=0; i<MaxNumBlocks; i++)
        if(BlockList[i]) {
            printf("block............................%d, %d\n", i,
                   BlockList[i]->type);
  
            //if(i==333 || i==334 || i==336 || i==337||i==364||i==367)
            //pretty_print(BlockList[i]);
        }
#endif

    /* unmark everything */
    for(i=0; i<MaxNumBlocks; i++)
        if(BlockList[i])
            data_set_marked_recursive(BlockList[i], false);
    
    for(i=0; i<MaxNumEnvironments; i++)
        if(EnvironmentList[i])
            env_set_marked(EnvironmentList[i], false);
       
    /* let the evaluator mark the necessary blocks and environments */

    eval_gather_gc(mark_data, mark_env);

    /* go through and remove unmarked blocks and environments */

    for(i=0; i<MaxNumBlocks; i++)
        if(BlockList[i] && !BlockList[i]->marked) {
            data_destroy(BlockList[i]);
            BlockList[i] = NULL;
        }

    for(i=0; i<MaxNumEnvironments; i++)
        if(EnvironmentList[i] && !EnvironmentList[i]->marked) {
            env_destroy(EnvironmentList[i]);
            EnvironmentList[i] = NULL;
        }

#ifdef ENABLE_GC_STATS
    if((bool)eng_get_prop("show_garbage_collection_stats")) {
        OUTPUT_REG("---------- after garbage collection ----------\n");
        OUTPUT_REG("blocks in use: %d\n", BlocksInUse);
        OUTPUT_REG("environments in use: %d\n", EnvironmentsInUse);
        OUTPUT_REG("----------------------------------------------\n");
    }
#endif

#ifdef DEBUG_BLOCKLIST
    for(i=0; i<MaxNumBlocks; i++)
        if(BlockList[i]) {
            //printf("block............................%d, %d\n", i,
            //       BlockList[i]->type);
            pretty_print(BlockList[i]);
        } else {
            printf("block............................%d, freed\n", i);
        }
#endif

}

void gc_run_with_env(environment_t *env) {
    mark_env(env);
    gc_run();
}
