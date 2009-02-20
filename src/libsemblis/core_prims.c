#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "prims.h"
#include "macros.h"
#include "reader.h"
#include "logger.h"
#include "environment.h"

#define INITIAL_TABLE_SIZE 32
hashtable_t *reg_table;

#define MAX_STRING_LEN 512
static data_t *display_func_helper(data_t *arg)
{
    switch(arg->type) {
    case DT_STRING:
    case DT_SYMBOL:
    case DT_VARIABLE:
    case DT_DATAERROR:
    {
        char tmp[MAX_STRING_LEN];
        wcstombs(tmp, arg->data.text, MAX_STRING_LEN);
        OUTPUT_ERR("%s", tmp);

        if(arg->type == DT_DATAERROR) {
            OUTPUT_ERR(" > error at %s:%d\n",
                       get_filename_by_index(arg->filename),
                       arg->line_num);
        }

        break;
    }
    case DT_PROCEDURE:
        OUTPUT_REG("{compound procedure}%d", (int)arg);
        break;
    case DT_NUMBER:
        OUTPUT_REG("%f", arg->data.number);
        break;
    case DT_PAIR:
        OUTPUT_REG("(");
        
        while(arg != DataNIL) {
            data_t *car = CAR(arg);
            display_func_helper(car);
            arg = CDR(arg);
            OUTPUT_REG(" ");
        }

        OUTPUT_REG(" )");
        break;
    case DT_NIL:
        OUTPUT_REG("NIL");
        break;
    case DT_T:
        OUTPUT_REG("#T");
        break;
    default:
        OUTPUT_ERR("error in display");
        return DataNIL;
    }

    return DataTrue;
}

PRIM_FUNCTION(display_func) {
    data_t *arg;


    if(IS_PAIR(args)) 
	{
        arg = CAR(args);
        args = CDR(args);
    } else
	{
        arg = args;
	}

    return display_func_helper(arg);
}

PRIM_FUNCTION(newline_func) {
    OUTPUT_REG("\n");
    return DataTrue;
}

PRIM_FUNCTION(list_func) {
    data_t *root, *list, *prev;

    MIN_ARITY(1);

    root = EMPTY_PAIR(args);
    list = root;
    prev = NULL;

    while(args != DataNIL) {
        data_t *result = CAR(args);
        
        CAR(list) = result;
        CDR(list) = EMPTY_PAIR(result);
        prev = list;
        list = CDR(list);
        args = CDR(args);
    }

    if(prev)
        CDR(prev) = DataNIL;
    
    return root;
}

PRIM_FUNCTION(first_func) {
    data_t *result;

    MIN_ARITY(1);

    result = CAR(args);

    if(! IS_PAIR(result))
        RETURN_ERROR(result, L"argument to first must be a pair");
    
    return CAR(result);
}

PRIM_FUNCTION(rest_func) {
	data_t *result;
    
    MIN_ARITY(1);

    result = CAR(args);

    if(! IS_PAIR(result))
        RETURN_ERROR(result, L"argument to first must be a consable-type");

    return CDR(result);
}

PRIM_FUNCTION(cons_func) {
	data_t *value, *list;

    MIN_ARITY(2);

    value = CAR(args);
    list = CAR(CDR(args));
    
    return data_create(value->filename,
                       value->line_num,
                       DT_PAIR,
                       value,
                       list,
                       NULL);
}

PRIM_FUNCTION(or_func) {
    MIN_ARITY(1);

    while(args != DataNIL) {
        data_t *val = CAR(args);

        if(val != DataNIL)
            return val;
        
        args = CDR(args);
    }

    return DataNIL;
}

PRIM_FUNCTION(and_func) {
    MIN_ARITY(1);

    data_t * val = DataNIL;
    while(args != DataNIL) {
        val = CAR(args);

        if( val == DataNIL )
            return DataNIL;

        args = CDR(args);
    }

    return val;
}


PRIM_FUNCTION(not_func) {
	data_t* arg1;

    CHECK_ARITY(1);

    arg1 = CAR(args);

    if(arg1 == DataNIL)
        return DataTrue;

    return DataNIL;
}

/* math-related functions */

PRIM_FUNCTION(plus_func) {
    double total = 0.0;
    data_t *cur_data;

    MIN_ARITY(1);

    while(args != DataNIL) {
        cur_data = CAR(args);
        if(!IS_NUMBER(cur_data))
            RETURN_ERROR(cur_data, L"args to + must be numbers");
       
        total += cur_data->data.number;

        args = CDR(args);
    }

    RETURN_NUMBER(cur_data, total);
}

PRIM_FUNCTION(subtract_func) {
    double total = 0.0;
    data_t *cur_data;

    MIN_ARITY(1);

    cur_data = CAR(args);
    if(!IS_NUMBER(cur_data))
        RETURN_ERROR(cur_data, L"args to - must be numbers");
    args = CDR(args);
    
    if(args == DataNIL)
        total = -cur_data->data.number;
    else
        total = cur_data->data.number;

    while(args != DataNIL) {
        cur_data = CAR(args);
        if(!IS_NUMBER(cur_data))
            RETURN_ERROR(cur_data, L"args to - must be numbers");
       
        total -= cur_data->data.number;

        args = CDR(args);
    }

    RETURN_NUMBER(cur_data, total);
}

PRIM_FUNCTION(mult_func) {
    double total = 1.0;
    data_t *cur_data;

    while(args != DataNIL) {
        cur_data = CAR(args);
        if(!IS_NUMBER(cur_data))
            RETURN_ERROR(cur_data, L"args * must be numbers");
       
        total *= cur_data->data.number;

        args = CDR(args);
    }

    RETURN_NUMBER(cur_data, total);
}

PRIM_FUNCTION(divide_func) {
    double total = 0.0;
    data_t *arg;

    MIN_ARITY(2);

    arg = CAR(args);
    if(!IS_NUMBER(arg))
        RETURN_ERROR(arg, L"args to / must be numbers");

    total = arg->data.number;
    
    args = CDR(args);
    
    while(args != DataNIL) {
        arg = CAR(args);
        if(!IS_NUMBER(arg))
            RETURN_ERROR(arg, L"args to / must be numbers");
       
        total /= arg->data.number;

        args = CDR(args);
    }

    RETURN_NUMBER(arg, total);
}


PRIM_FUNCTION(equals_func) {
    data_t *arg1, *arg2;

    CHECK_ARITY(2);

	arg1 = CAR(args);
    arg2 = CAR(CDR(args));

    /* first check for special case of comparison to true and nil */
    if(arg1 == DataNIL) {
        if(arg2 != DataNIL)
            return DataNIL;
        else
            return DataTrue;
    } else if(arg2 == DataNIL) {
        if(arg1 != DataNIL)
            return DataNIL;
        else
            return DataTrue;
    }

    if(arg1 == DataTrue) {
        if(arg2 != DataTrue)
            return DataNIL;
        else
            return DataTrue;
    } else if(arg2 == DataTrue) {
        if(arg1 != DataTrue)
            return DataNIL;
        else
            return DataTrue;
    }

    if(IS_STRING_TYPE(arg1)) {
        if(! IS_STRING_TYPE(arg2))
            RETURN_ERROR(arg1, L"first arg to = is a string but second is not");
        
        if( util_wcs_eq(arg1->data.text, arg2->data.text) )
            return DataTrue;
    } else if(IS_NUMBER(arg1)) {
        if(! IS_NUMBER(arg2))
            RETURN_ERROR(arg1, L"first arg to = is a real but second arg is not a number");
        
        if( arg1->data.number == arg2->data.number )
            return DataTrue;
    } else {
        RETURN_ERROR(arg1, L"equal not implemented for given arguments");
    }

    return DataNIL;
}

PRIM_FUNCTION(lt_func) {
    data_t *arg1, *arg2;

    CHECK_ARITY(2);

    arg1 = CAR(args);
    arg2 = CAR(CDR(args));

    if(! IS_NUMBER(arg1) || ! IS_NUMBER(arg2))
        RETURN_ERROR(args, L"arguments to < must be numbers");

	if(arg1->data.number < arg2->data.number)
	    return DataTrue;

    return DataNIL;
}

PRIM_FUNCTION(gt_func) {
    data_t *arg1, *arg2;

    CHECK_ARITY(2);

    arg1 = CAR(args);
    arg2 = CAR(CDR(args));

    if(! IS_NUMBER(arg1) || ! IS_NUMBER(arg2))
        RETURN_ERROR(args, L"arguments to > must be numbers");

	if(arg1->data.number > arg2->data.number)
	    return DataTrue;

    return DataNIL;
}

PRIM_FUNCTION(ltoe_func) {
    data_t *arg1, *arg2;

    CHECK_ARITY(2);

    arg1 = CAR(args);
    arg2 = CAR(CDR(args));

    if(! IS_NUMBER(arg1) || ! IS_NUMBER(arg2))
        RETURN_ERROR(args, L"arguments to <= must be numbers");

	if(arg1->data.number <= arg2->data.number)
	    return DataTrue;

    return DataNIL;
}

PRIM_FUNCTION(gtoe_func) {
    data_t *arg1, *arg2;

    CHECK_ARITY(2);

    arg1 = CAR(args);
    arg2 = CAR(CDR(args));

    if(! IS_NUMBER(arg1) || ! IS_NUMBER(arg2))
        RETURN_ERROR(args, L"arguments to >= must be numbers");

	if(arg1->data.number >= arg2->data.number)
	    return DataTrue;

    return DataNIL;
}

PRIM_FUNCTION(rand_func) {
    CHECK_ARITY(0);
    
    double result = rand();
    result /= RAND_MAX;

    RETURN_NUMBER(args, result);
}

PRIM_FUNCTION(mod_func) {
    data_t *arg1, *arg2;
    int a1, a2;
    double r1;

    CHECK_ARITY(2);

    arg1 = CAR(args);
    arg2 = CAR(CDR(args));

    if(! IS_NUMBER(arg1) || ! IS_NUMBER(arg2))
        RETURN_ERROR(args, L"arguments to mod must be numbers");

    a1 = (int)arg1->data.number;
    a2 = (int)arg2->data.number;
    
    r1 = a1 % a2;

	RETURN_NUMBER(args, r1);
}

PRIM_FUNCTION(int_func) {
    data_t *arg1;
    double a1;

    CHECK_ARITY(1);

    arg1 = CAR(args);

    if(! IS_NUMBER(arg1))
        RETURN_ERROR(arg1, L"argument to \"int\" must be a number");
    
    a1 = (int)arg1->data.number;

    RETURN_NUMBER(arg1, a1);
}

static void register_core_prims(void)
{
    REGISTER_FUNC("core", L"display", display_func);
    REGISTER_FUNC("core", L"newline", newline_func);
    REGISTER_FUNC("core", L"first", first_func);
    REGISTER_FUNC("core", L"rest", rest_func);
    REGISTER_FUNC("core", L"list", list_func);
    REGISTER_FUNC("core", L"push", cons_func);
    REGISTER_FUNC("core", L"not", not_func);
    REGISTER_FUNC("core", L"or", or_func);
    REGISTER_FUNC("core", L"and", and_func);
/*     REGISTER_FUNC(L"str", str_func, 1); */
/*     REGISTER_FUNC(L"ptr", ptr_func, 1); */

    REGISTER_FUNC("core math", L"+", plus_func);
    REGISTER_FUNC("core math", L"-", subtract_func);
    REGISTER_FUNC("core math", L"*", mult_func);
    REGISTER_FUNC("core math", L"/", divide_func);
    REGISTER_FUNC("core math", L"=", equals_func);
    REGISTER_FUNC("core math", L"<", lt_func);
    REGISTER_FUNC("core math", L">", gt_func);
    REGISTER_FUNC("core math", L"<=", ltoe_func);
    REGISTER_FUNC("core math", L">=", gtoe_func);
    REGISTER_FUNC("core math", L"random", rand_func);
    REGISTER_FUNC("core math", L"mod", mod_func);
    REGISTER_FUNC("core math", L"int", int_func);
}


DLL_INFO void prims_init(void)
{
    reg_table = hashtable_create(INITIAL_TABLE_SIZE,
                                 HT_COPY_KEY,
                                 hashtable_jenkins_hash_wchar,
                                 env_str_size_func,
                                 env_str_copy_func,
                                 env_str_equal_func,
                                 NULL,
                                 NULL);

    srand(time(NULL));

    register_core_prims();
}

DLL_INFO void prims_destroy(void)
{
    hashtable_destroy(reg_table);
}

DLL_INFO PrimFunc *prims_get(String name)
{
    return hashtable_get(reg_table, name);
}

/*

PRIM_FUNCTION(str_func) {
    Data *str_object;
    String c_str;
    Data *result;

	_START("str_func");

	result = data_arg_next(&args, env);

    ERROR_CHECK_AND_RETURN(result);

    if(! data_is_integer(result))
        return eng_create_error(L"argument to str must be an integer (pointer)");

    c_str = (String)data_get_integer(result);
    str_object = eng_create_string(c_str);

    return str_object;
}


PRIM_FUNCTION(ptr_func) {
    Data *ptr_object;
    String ptr;
    Data *result;

	_START("ptr_func");

	result = data_arg_next(&args, env);

    ERROR_CHECK_AND_RETURN(result);

    if(! data_is_string(result))
        return eng_create_error(L"argument to str must be a string");

    ptr = data_get_string(result);
    ptr_object = eng_create_int((int32)ptr);

    return ptr_object;
}
*/
