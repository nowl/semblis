#ifndef __DATA_H__
#define __DATA_H__

#include "types.h"
#include "windll.h"

typedef struct data_type {
    union {
        String text;
        
        double number;
        
        struct {
            struct data_type *car;
            struct data_type *cdr;
        } pair;
        
        struct {
            struct data_type *args;
            struct data_type *code;
            void *env;              /* should point to a environment_t */
        } procedure;

        void *ptr;
    } data;

    short filename;
    short line_num;    

    uint8 type;		/* data type */
    bool  marked; 		/* marked for GC? */
} data_t;

typedef enum {
    DT_STRING = 0,	    	/* 0 */
    DT_SYMBOL,       		/* 1 */
    DT_VARIABLE,     		/* 2 */
    DT_PROCEDURE,    		/* 3 */
    DT_NUMBER,      		/* 4 */
    DT_PAIR,         		/* 5 */
    DT_NIL,          		/* 6 */
    DT_T,            		/* 7 */
    DT_DATAERROR,    		/* 8 */
    DT_MACRO,    		    /* 9 */
    DT_POINTER        		/* 10 */
} DataType;

/** Global list of all blocks in use. As blocks are created they aree
 * added to the next non NULL location in this global list. It is up
 * to the garbage collector to maintain the list and to destroy
 * blocks. */
extern data_t **BlockList;
extern int MaxNumBlocks;
extern int BlocksInUse;

DLL_INFO void data_init(int max_number_blocks);
DLL_INFO void data_close();

DLL_INFO data_t *data_create(int filename_index,
                             int line_num,
                             uint8 type,
                             void *arg1,
                             void *arg2,
                             void *arg3);
DLL_INFO void data_destroy(data_t *data);

DLL_INFO DataType data_get_type(data_t *data);
DLL_INFO void data_set_type(data_t *data, DataType type);
DLL_INFO bool data_is_marked(data_t *data);
DLL_INFO bool data_is_string(data_t *data);
DLL_INFO bool data_is_string_type(data_t *data);
DLL_INFO bool data_is_symbol(data_t *data);
DLL_INFO bool data_is_variable(data_t *data);
DLL_INFO bool data_is_lambda(data_t *data);
DLL_INFO bool data_is_procedure(data_t *data);
DLL_INFO bool data_is_real(data_t *data);
DLL_INFO bool data_is_integer(data_t *data);
DLL_INFO bool data_is_pair(data_t *data);
DLL_INFO bool data_is_error(data_t *data);
DLL_INFO bool data_is_comb(data_t *data);
DLL_INFO bool data_is_nil(data_t *data);
DLL_INFO bool data_is_t(data_t *data);
DLL_INFO bool data_is_data(data_t *data);

bool data_is_cons_type(data_t *data);

DLL_INFO bool data_set_string(data_t *data, String text);
DLL_INFO bool data_set_real(data_t *data, double value);
DLL_INFO bool data_set_integer(data_t *data, uint32 value);
DLL_INFO bool data_set_car(data_t *data, data_t *car);
DLL_INFO bool data_set_cdr(data_t *data, data_t *cdr);
DLL_INFO data_t *data_get_car(data_t *data);
DLL_INFO data_t *data_get_cdr(data_t *data);
DLL_INFO double data_get_real(data_t *data);
DLL_INFO int32 data_get_integer(data_t *data);
DLL_INFO String data_get_string(data_t *data);

DLL_INFO struct Environment_t* data_get_environment(data_t *data);
DLL_INFO void data_set_environment(data_t *data, struct Environment_t* env);

void data_set_marked(data_t *data, bool mark);
void data_set_marked_recursive(data_t *data, bool mark);
int data_get_recursive_size(data_t *data);

DLL_INFO data_t *data_arg_next(data_t **args, struct Environment_t *env);
DLL_INFO data_t *data_arg_next_no_eval(data_t **args);
DLL_INFO bool data_arg_has_next(data_t *args);

DLL_INFO char *data_get_filename(data_t *data);
DLL_INFO int data_get_line_num(data_t *data);

void data_cleanup(void);

/* global data_t objects */
extern data_t *DataNIL, *DataTrue;

#endif	/* __DATA_H__ */
