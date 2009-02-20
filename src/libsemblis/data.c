#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

#include "semblis.h"

data_t **BlockList;
int MaxNumBlocks;
int BlocksInUse;


void data_init(int max_num_blocks)
{
    int i;

    MaxNumBlocks = max_num_blocks;

    BlockList = malloc(sizeof(*BlockList) * MaxNumBlocks);

    for(i=0; i<MaxNumBlocks; i++)
        BlockList[i] = NULL;

    BlocksInUse = 0;
}

void data_close(void)
{
    free(BlockList);
}

static bool insert_block(data_t *new_data) {
    static int next_block_spot = 0;

    if(BlocksInUse == MaxNumBlocks) {
        /* hit max, trigger garbage collection */
//        gc_run_with_block(new_data);
        gc_run();

        if(BlocksInUse == MaxNumBlocks) {
            printf("max number of blocks hit\n");
            exit(1);
        }
    }

    /* find spot in block list to insert this */
    do {
        if(BlockList[next_block_spot] == NULL) {
            BlockList[next_block_spot++] = new_data;
            next_block_spot %= MaxNumBlocks;
            break;
        }
        ++next_block_spot;
        next_block_spot %= MaxNumBlocks;

    } while(1);

    ++BlocksInUse;

    return true;
}

static data_t *create(int filename_index,
                    int line_num,
                    uint8 type,
                    void *arg1,
                    void *arg2,
                    void *arg3)
{
    data_t *new_data = malloc(sizeof(*new_data));

    new_data->marked = false;
    new_data->filename = filename_index;
    new_data->line_num = line_num;
    new_data->type = type;

    switch(type) {
    case DT_STRING:
    case DT_SYMBOL:
    case DT_VARIABLE:
    case DT_DATAERROR:
    {
        wchar_t *val = arg1;
        int len = wcslen(val) + 1;
        new_data->data.text = malloc(len * sizeof(wchar_t));
        wcscpy(new_data->data.text, val);
        break;
    }
    case DT_PROCEDURE:
        new_data->data.procedure.env = arg3;
    case DT_MACRO:
        new_data->data.procedure.args = arg1;
        new_data->data.procedure.code = arg2;
        break;
    case DT_NUMBER:
        new_data->data.number = *(double *)arg1;
        break;
    case DT_PAIR:
        new_data->data.pair.car = arg1;
        new_data->data.pair.cdr = arg2;
        break;
    case DT_POINTER:
        new_data->data.ptr = arg1;
        break;
    default:
        printf("unknown type in data object creation\n");
        break;
    }

    return new_data;
}

data_t *data_create(int filename_index,
                    int line_num,
                    uint8 type,
                    void *arg1,
                    void *arg2,
                    void *arg3)
{
    data_t *new_data = create(filename_index,
                              line_num,
                              type,
                              arg1, arg2, arg3);
    insert_block(new_data);

    if(IS_DATAERROR(new_data))
        eval_set_error(new_data);

    return new_data;
}

data_t *data_create_no_gc(int filename_index,
                          int line_num,
                          uint8 type,
                          void *arg1,
                          void *arg2,
                          void *arg3)
{
    data_t *new_data = create(filename_index,
                              line_num,
                              type,
                              arg1, arg2, arg3);

    if(IS_DATAERROR(new_data))
        eval_set_error(new_data);

    return new_data;
}

static void destroy(data_t *data)
{
    if(IS_STRING_TYPE(data))
        free(data->data.text);

    free(data);
}

void data_destroy(data_t *data)
{
    destroy(data);

    --BlocksInUse;
}

void data_destroy_no_gc(data_t *data)
{
    destroy(data);
}

void data_set_marked(data_t *data, bool mark)
{
    data->marked = mark;

#ifdef DEBUGGING_GC
    Logger *log = logger_get("GC DEBUG");

    {
        char tmp[2000];
        snprintf(tmp, 2000, "%s%s",
                 ( (mark) ? "marking block: " : "unmarking block: " ),
                 util_disp_data(data));
        logger_log(log, LL_DEBUG, tmp);
    }
#endif

}

void data_set_marked_recursive(data_t *data, bool mark)
{
    if(data->type != DT_NIL) {
        switch(data->type) {
        case DT_PAIR:
            if(CDR(data)->marked != mark)
                data_set_marked_recursive(CDR(data), mark);
            if(CAR(data)->marked != mark)
                data_set_marked_recursive(CAR(data), mark);
            break;
        case DT_PROCEDURE:
        {
            environment_t *env = data->data.procedure.env;
            if(env && env->marked != mark)
                env_set_marked(env, mark);
        }
        case DT_MACRO:
        {
            data_t *tmp;

            tmp = data->data.procedure.args;
            if(tmp->marked != mark)
                data_set_marked_recursive(tmp, mark);

            tmp = data->data.procedure.code;
            if(tmp->marked != mark)
                data_set_marked_recursive(tmp, mark);

            break;
        }
        default:
            break;
        }
    }

    data_set_marked(data, mark);
}
