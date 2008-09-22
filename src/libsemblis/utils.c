#include <malloc.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>

#include "utils.h"

void *util_grow_buffer_to_size(void *buffer,
                               unsigned int *cur_capacity,
                               unsigned int desired_capacity,
                               unsigned int bytes_per_block)
{
    unsigned int capacity;
    void *new_buffer;

    if(desired_capacity < *cur_capacity)
        return buffer;

    capacity = *cur_capacity;
    new_buffer = buffer;

    while(capacity < desired_capacity) {
        if(capacity > 0)
            capacity *= 2;
        else
            capacity = 1;

        new_buffer = realloc(new_buffer, capacity * bytes_per_block);
        *cur_capacity = capacity;
    }

    return new_buffer;
}

static char* tmp_mbs_buffer;
static unsigned int tmp_mbs_buffer_cap;

char *util_to_mbs(String x)
{
    int len = wcslen(x) + 1;
    
    tmp_mbs_buffer = util_grow_buffer_to_size(tmp_mbs_buffer,
                                              &tmp_mbs_buffer_cap,
                                              len,
                                              sizeof(*tmp_mbs_buffer));

    wcstombs(tmp_mbs_buffer, x, len);
    return tmp_mbs_buffer;
}

static String tmp_wcs_buffer;
static unsigned int tmp_wcs_buffer_cap;

String util_to_wcs(char *x)
{
    int len = strlen(x) + 1;
    
    tmp_wcs_buffer = util_grow_buffer_to_size(tmp_wcs_buffer,
                                              &tmp_wcs_buffer_cap,
                                              len,
                                              sizeof(*tmp_wcs_buffer));

    mbstowcs(tmp_wcs_buffer, x, len);
    return tmp_wcs_buffer;
}

bool util_wcs_eq(String arg1, String arg2)
{
    if(arg1 == NULL || arg2 == NULL)
        return false;

    if( wcscmp(arg1, arg2) == 0)
        return true;

    return false;
}

bool util_mbs_eq(char *arg1, char *arg2)
{
    if( strcmp(arg1, arg2) == 0)
        return true;

    return false;
}

DLL_INFO data_t *reader_to_data(reader_node_t *code, bool in_pair, bool is_gc)
{
    int fn_index = code->filename_index;
    int line_num = code->line_num;
    uint8 type;
    void *arg1, *arg2, *arg3;
    data_t *result;

    arg1 = arg2 = arg3 = NULL;

    switch(code->token_type) {
    case SRT_PAIR:
        arg1 = reader_to_data(code->child, true, is_gc);
        if(code->next)
            arg2 = reader_to_data(code->next, true, is_gc);
        else
            arg2 = DataNIL;
        type = DT_PAIR;
        break;
    case SRT_STRING: 
    {
        String str = util_to_wcs(code->data);
        int len = wcslen(str);
        
        if(str[len-1] == L'"')
            str[len-1] = 0;

        arg1 = &str[1];          /* skip the '"' chars on each end */
        type = DT_STRING;
        break;
    }
    case SRT_SYMBOL:
    {
        String str = util_to_wcs(code->data);
        arg1 = &str[1]; /* skip the '\'' char */
        type = DT_SYMBOL;
        break;
    }
    case SRT_VARIABLE:
        arg1 = util_to_wcs(code->data);
        type = DT_VARIABLE;
        break;
    case SRT_NUMBER:
        arg1 = code->data;
        type = DT_NUMBER;
        break;
    default:
        assert(false);
    }

    /* special case of ")" */
    if(type == DT_VARIABLE && util_wcs_eq(arg1, L")"))
        return DataNIL;

    if(is_gc)
        result = data_create(fn_index, line_num, type, arg1, arg2, arg3);
    else
        result = data_create_no_gc(fn_index, line_num, type, arg1, arg2, arg3);

    if(result->type != DT_PAIR && in_pair) {
        data_t *data_next;
        if(code->next)
            data_next = reader_to_data(code->next, true, is_gc);
        else
            data_next = DataNIL;
        
        if(is_gc)
            result = data_create(result->filename,
                                 result->line_num,
                                 DT_PAIR,
                                 result,
                                 data_next,
                                 result);
        else
            result = data_create_no_gc(result->filename,
                                       result->line_num,
                                       DT_PAIR,
                                       result,
                                       data_next,
                                       result);
        
    }

    return result;
}
