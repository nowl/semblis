#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "eval.h"
#include "environment.h"
#include "macros.h"
#include "utils.h"
#include "prims.h"

//#define DEBUG
#define DEBUG_TRACE
//#define DEBUG_MEDIUM
//#define DEBUG_FULL

//#define DEBUG_MACRO
//#define DEBUG_STACK
//#define DEBUG_GC


/***********/
/* globals */
/***********/

data_t *DataNIL, *DataTrue;
environment_t *GlobalEnv;

/*************/
/* registers */
/*************/

static data_t *val, *exp, *argl, *unev, *proc;
static environment_t *env;
static data_t *code_pointer, *top_code_pointer;
static int cont;

static PrimFunc *proc_prim;
static data_t *last_arg_pointer;

/******************/
/* register types */
/******************/

#define RT_DATA 0
#define RT_SLOT 1

/**************/
/* structures */
/**************/

typedef struct {
    data_t* args;
    data_t* code;
    environment_t* env;
} procedure_t;

/******************/
/* continue jumps */
/******************/

enum {
    JUMP_CODE,                  /* 0 */
    JUMP_MACRO_FINISH,          /* 1 */
    JUMP_EV_DEFINITION_1,       /* 2 */
    JUMP_EV_ASSIGNMENT_1,       /* 3 */
    JUMP_EV_APPL_DID_OPERATOR,  /* 4 */
    JUMP_EV_APPL_ACCUMULATE_ARG, /* 5 */
    JUMP_EV_APPL_ACCUM_LAST_ARG, /* 6 */
    JUMP_EV_SEQUENCE_CONTINUE,   /* 7 */
    JUMP_EV_IF_DECIDE,           /* 8 */
    JUMP_EV_SEQUENCE,            /* 9 */
    JUMP_EV_IMPORT_AUX,          /*10 */
};

/*******************/
/* stack functions */
/*******************/

#define MAX_STACK_DEPTH 10000

typedef enum {
    ST_DATA,
    ST_ENV,
    ST_CONT,
    ST_ARG_POINTER,
} StackType;

static struct {
    void *data[MAX_STACK_DEPTH];
    StackType stack_type[MAX_STACK_DEPTH];
    int next;
} stack;

static StackType popped_stacktype;

static void* pop_stack() {
    if(stack.next == 0) {
        printf("trying to pop empty stack!\n");
        return NULL;
    }

    --stack.next;
    
#ifdef DEBUG_STACK
    printf("(pop ) stack size = %d\n", stack.next);
#endif

    popped_stacktype = stack.stack_type[stack.next];

    return stack.data[stack.next];
}

static void push_stack(void *entry, StackType type) {
    if(stack.next == MAX_STACK_DEPTH) {
        printf("trying to push to full stack!\n");
        return;
    }
    
    stack.data[stack.next] = entry;
    stack.stack_type[stack.next++] = type;

#ifdef DEBUG_STACK
    printf("(push) stack size = %d\n", stack.next);
#endif
}

static void save_regs(void) {
    /* val, proc_prim are dropped */

    push_stack(exp, ST_DATA);
    push_stack(argl, ST_DATA);
    push_stack(unev, ST_DATA);
    push_stack(proc, ST_DATA);
    push_stack(code_pointer, ST_DATA);
    push_stack(top_code_pointer, ST_DATA);
    push_stack(env, ST_ENV);
    push_stack(cont, ST_CONT);
    push_stack(last_arg_pointer, ST_ARG_POINTER);

    cont = JUMP_CODE;
}

static void restore_regs(void) {
    last_arg_pointer = pop_stack();
    cont = pop_stack();
    env = pop_stack();
    top_code_pointer = pop_stack();
    code_pointer = pop_stack();
    proc = pop_stack();
    unev = pop_stack();
    argl = pop_stack();
    exp = pop_stack();
}

/*********************/
/* utility functions */
/*********************/

static void append_to_last(data_t **list, data_t *val) {
    data_t *tmp = data_create(val->filename, val->line_num,
                              DT_PAIR, val, DataNIL, NULL);

    if(last_arg_pointer) {
        last_arg_pointer->data.pair.cdr = tmp;
        last_arg_pointer = CDR(last_arg_pointer);
    } else {
        *list = tmp;
        last_arg_pointer = *list;
    }
}

#ifdef DEBUG
static void dump_regs(char *header) {
    printf("\n>>>>>>>>>>>> ");

    if(header)
        printf("%s <<<<<<<<<<<<\n", header);

    printf("exp:\n");
    pretty_print(exp);

# ifdef DEBUG_MEDIUM
    printf("val: ");
    semi_print(val);
    printf("\n");
    printf("argl: ");
    semi_print(argl);
    printf("\n");
    printf("unev: ");
    semi_print(unev);
    printf("\n");
    printf("proc: ");
    semi_print(proc);
    printf("\n");
    printf("cont: %d\n", cont);
# endif  /* DEBUG_MEDIUM */

# ifdef DEBUG_FULL
    printf("val:\n");
    pretty_print(val);
    printf("argl:\n");
    pretty_print(argl);
    printf("unev:\n");
    pretty_print(unev);
    printf("proc:\n");
    pretty_print(proc);
    //printf("code pointer:\n");
    //pretty_print(code_pointer);
    //printf("top code pointer:\n");
    //pretty_print(top_code_pointer);
    printf("cont: %d\n", cont);
# endif  /* DEBUG_FULL */

# ifdef DEBUG_STACK
    printf("....stack dump....\n");
    int i;
    for(i=0; i<stack.next; i++) {
        //stack.data[stack.next] = entry;
        switch(stack.stack_type[i]) {
        case ST_DATA:
            printf("(stack) data\n");
            break;
        case ST_ENV:
            printf("(stack) env\n");
            break;
        case ST_CONT:
            printf("(stack) continuation: %d\n", stack.data[i]);
            break;
        case ST_ARG_POINTER:
            printf("(stack) arg pointer\n");
            break;
        }
    }
# endif
}
#else
static void dump_regs(char *header) {};
#endif  /* DEBUG */

static bool is_primitive_proc(data_t *val) {
    if(! IS_STRING_TYPE(val))
        return false;

    proc_prim = prims_get(val->data.text);

    if(proc_prim)
        return true;

    return false;
}

static bool is_compound_proc(data_t *val) {
    if(val->type == DT_PROCEDURE)
        return true;
    
    return false;
}

static bool is_macro(data_t *val) {
    if(val->type == DT_MACRO)
        return true;

    return false;
}

static data_t *make_procedure(data_t *unev,
                              data_t *exp, 
                              environment_t *env) {
    return data_create(exp->filename, 
                       exp->line_num,
                       DT_PROCEDURE,
                       unev,
                       exp,
                       env);
}

/* TODO: macro shouldn't carry the environment */
static data_t *make_macro(data_t *unev,
                          data_t *exp) {
    return data_create(exp->filename, 
                       exp->line_num,
                       DT_MACRO,
                       unev,
                       exp,
                       NULL);
}

static environment_t *extend_environment(data_t *formal_params, 
                                         data_t *actual_params,
                                         environment_t *env) {

    environment_t *new_env = env_create(env);

    /* TODO: check arg counts are equal here */
    while(CAR(formal_params) != DataNIL) {
        data_t *arg1 = CAR(formal_params);
        data_t *arg2 = CAR(actual_params);

        assert(IS_STRING_TYPE(arg1));

        if(util_wcs_eq(arg1->data.text, L"&rest")) {
            /* If this is a parameter defined as "&rest" then assign
             * the rest of the parameters to this actual parameter
             */
            arg2 = actual_params;

            env_set(new_env, L"&rest", arg2);
            break;
        }

        env_set(new_env, arg1->data.text, arg2);

        formal_params = CDR(formal_params);
        actual_params = CDR(actual_params);
    }

    return new_env;
}

static data_t *symbol_to_data(data_t *obj) {
    char *tmp_str = util_to_mbs(obj->data.text);
    reader_node_t *tmp = reader_read_string(tmp_str);
    classifier_run(tmp);
    
    return CAR(reader_to_data(tmp, false, true));
}

static data_t *possible_symbol_to_data(data_t *obj) {
    if(obj->data.text[0] == L'(')
        return symbol_to_data(obj);
    else
        return obj;
}

/*****************/
/* gc operations */
/*****************/

#ifdef DEBUG_GC
void
eval_gather_gc(void (*add_data_cb)(data_t *),
               void (*add_env_cb)(environment_t *))
{
    int i;
    
    printf(".................val:\n");
    pretty_print(val);
    printf(".................exp:\n");
    pretty_print(exp);
    printf(".................argl:\n");
    pretty_print(argl);
    printf(".................unev:\n");
    pretty_print(unev);
    printf(".................proc:\n");
    pretty_print(proc);

    printf(".................env bindings\n");

    for(i=0; i<env->bindings->capacity; i++) {
        hashtable_entry_t *he = env->bindings->elements[i];

        if(he->key) {
            pretty_print(he->value);
        }
    }

    printf(".................GlobalEnv bindings\n");

    for(i=0; i<GlobalEnv->bindings->capacity; i++) {
        hashtable_entry_t *he = env->bindings->elements[i];

        if(he->key) {
            pretty_print(he->value);
        }
    }

    /* add registers and core stuff */

    add_data_cb(DataNIL);
    add_data_cb(DataTrue);
    add_env_cb(GlobalEnv);
    add_data_cb(val);
    add_data_cb(exp);
    add_data_cb(argl);
    add_data_cb(unev);
    add_data_cb(proc);
    add_env_cb(env);
    //add_data_cb(code_pointer);
//    add_data_cb(top_code_pointer);
    //add_data_cb(last_arg_pointer); /* may not need this */
    
    /* add stack elements */
    printf(".................stack data\n");
    for(i=0; i<stack.next; i++) {
        if(stack.stack_type[i] == ST_DATA) {
            add_data_cb(stack.data[i]);
            pretty_print(stack.data[i]);
        } else if(stack.stack_type[i] == ST_ENV) {
            int j;
            add_env_cb(stack.data[i]);

            printf(".................stack env bindings\n");
            
            for(j=0; j<((environment_t*)stack.data[i])->bindings->capacity; j++) {
                hashtable_entry_t *he = env->bindings->elements[j];
                
                if(he->key) {
                    pretty_print(he->value);
                }
            }
        }
    }
}
#else  /* DEBUG_GC */
void
eval_gather_gc(void (*add_data_cb)(data_t *),
               void (*add_env_cb)(environment_t *))
{
    int i;

    /* add registers and core stuff */

    add_data_cb(DataNIL);
    add_data_cb(DataTrue);
    add_env_cb(GlobalEnv);
    add_data_cb(val);
    add_data_cb(exp);
    add_data_cb(argl);
    add_data_cb(unev);
    add_data_cb(proc);
    add_env_cb(env);
    //add_data_cb(code_pointer);
//    add_data_cb(top_code_pointer);
    //add_data_cb(last_arg_pointer);
    
    /* add stack elements */
    for(i=0; i<stack.next; i++) {
        if(stack.stack_type[i] == ST_DATA)
            add_data_cb(stack.data[i]);
        else if(stack.stack_type[i] == ST_ENV)
            add_env_cb(stack.data[i]);
    }
}

#endif  /* DEBUG_GC */

/***********************/
/* register operations */
/***********************/

static data_t *lookup_variable_value(data_t *value, environment_t *env) {
    String actual_text;

    assert(IS_STRING_TYPE(value));
    
    actual_text = value->data.text;

    data_t *tmp = env_get(env, actual_text, true);

    if(! tmp) {
        if(prims_get(actual_text)) {
            tmp = value;
        } else {
            wchar_t error[256];
            swprintf(error, 256, L"unknown variable: %ls",
                     actual_text);
            tmp = data_create(value->filename, value->line_num,
                              DT_DATAERROR, error, NULL, NULL);
        }
    }

    return tmp;
}

static void define_variable_value(data_t *variable,
                                  data_t *value,
                                  environment_t *env) 
{
    assert(IS_STRING_TYPE(variable));

//    env_set(env, variable->data.text, data_clone(value));
    env_set(env, variable->data.text, value);
}

static void set_variable_value(data_t *variable,
                               data_t *value,
                               environment_t *env) 
{
    assert(IS_STRING_TYPE(variable));

    data_t *original_env = env;
    data_t *possible_data = env_get(env, 
                                    variable->data.text, 
                                    false);

    while(!possible_data && env->parent) {
        env = env->parent;
        possible_data = env_get(env, variable->data.text, false);
    }

    if(!possible_data)
        env = original_env;

    env_set(env, variable->data.text, value);
}

static void run_eval_on_string(data_t *input) {
    char *tmp_str;
    reader_node_t *tmp;
    String string;

    save_regs();

    string = input->data.text;
    tmp_str = util_to_mbs(string);
    tmp = reader_read_string(tmp_str);
    if(tmp) {
        classifier_run(tmp);
        val = eval(tmp);
    } else {
        val = data_create(input->filename, input->line_num, DT_DATAERROR,
                          L"problem reading string for eval", NULL, NULL);
    }

    restore_regs();
}

#define MAX_FILENAME_LENGTH  1024

static void run_eval_on_file(char *filename) {
    reader_node_t *tmp;
    int i;
    char **search_dirs;
    char mod_filename[MAX_FILENAME_LENGTH];
    bool found_valid_filename = false;

    save_regs();

    /* test the various search directories */
    search_dirs = eng_get_search_dirs();    
    for(i=eng_get_num_search_dirs()-1; i>=0; i--) {
        if(!search_dirs[i])
            continue;

        int search_dirs_len = strlen(search_dirs[i]);
        bool has_ending_paren;
        has_ending_paren = (search_dirs[search_dirs_len-1] == '/');

        snprintf(mod_filename,
                 MAX_FILENAME_LENGTH,
                 has_ending_paren ? ("%s%s") : ("%s/%s"),
                 search_dirs[i],
                 filename);

        FILE *fin = fopen(mod_filename, "r");
        if(fin) {
            fclose(fin);
            found_valid_filename = true;
            break;
        }
    }


    if(found_valid_filename) {
        tmp = reader_read_file(mod_filename);
        if(tmp) {
            classifier_run(tmp);
            val = eval(tmp);
        } else {
            wchar_t error[256];
            swprintf(error, 256, L"problem reading file: %s", mod_filename);
            val = data_create(val->filename, val->line_num,
                              DT_DATAERROR, error, NULL, NULL);
        }
    } else {
        wchar_t error[256];
        swprintf(error, 256, L"problem finding file: %s", filename);
        val = data_create(val->filename, val->line_num,
                          DT_DATAERROR, error, NULL, NULL);
    }
    
    restore_regs();
}


/***************************/
/* main eval api functions */
/***************************/
static bool have_error;
static bool kill_if_error;
static data_t *error_value;

void eval_set_error(data_t *error)
{
    have_error = true;
    
    if(kill_if_error)
        error_value = error;
}

static void rec_set_new_pointers_data(data_t *arg,
                                      environment_t **old, 
                                      environment_t **new,
                                      int num);

static void rec_set_new_pointers_env(environment_t **envp,
                                     environment_t **old, 
                                     environment_t **new,
                                     int num)
{
    if(!envp)
        return;

    environment_t *env = *envp;

    if(!env)
        return;

//    printf("looking for environment matching %x\n", env);

    /* actually do the switch */
    int i;
    for(i=0; i<num; i++)
        if(old[i] == env) {
            *envp = new[i];
            return;
        }

    /* we should recursively go through the unknown environment */
    for(i=0; i<env->bindings->capacity; ++i) {
        hashtable_entry_t *entry = env->bindings->elements[i];
        
        if(entry->key) {
            data_t *slot = (data_t *)entry->value;
            rec_set_new_pointers_data(slot, old, new, num);
        }
    }

}                                      

static void rec_set_new_pointers_data(data_t *arg,
                                      environment_t **old, 
                                      environment_t **new,
                                      int num)
{
    if(!arg)
        return;

    data_t *car = NULL;
    data_t *cdr = NULL;
    
    switch(arg->type) {
    case DT_PROCEDURE:
        
        rec_set_new_pointers_env(&arg->data.procedure.env, old, new, num);
    case DT_MACRO:
        car = arg->data.procedure.args;
        cdr = arg->data.procedure.code;
        break;
    case DT_PAIR:
        car = CAR(arg);
        cdr = CDR(arg);
        break;
    default:
        break;
    }
    
    if(car)
        rec_set_new_pointers_data(car, old, new, num);
    if(cdr)
        rec_set_new_pointers_data(cdr, old, new, num);
}

void eval_set_new_pointers(environment_t **old_envs, 
                           environment_t **new_envs,
                           int num)
{
    /* set registers */
    rec_set_new_pointers_data(val, old_envs, new_envs, num);
    rec_set_new_pointers_data(exp, old_envs, new_envs, num);
    rec_set_new_pointers_data(argl, old_envs, new_envs, num);
    rec_set_new_pointers_data(unev, old_envs, new_envs, num);
    rec_set_new_pointers_data(proc, old_envs, new_envs, num);
//    rec_set_new_pointers_data(code_pointer, old_envs, new_envs, num);
//    rec_set_new_pointers_data(top_code_pointer, old_envs, new_envs, num);
//    rec_set_new_pointers_data(last_arg_pointer, old_envs, new_envs, num);

    rec_set_new_pointers_env(&env, old_envs, new_envs, num);

    /* set stack elements */
    int i;
    for(i=0; i<stack.next; i++) {
        switch(stack.stack_type[i]) {
        case ST_DATA:
            rec_set_new_pointers_data(stack.data[i], old_envs, new_envs, num);
            break;
        case ST_ENV:
            rec_set_new_pointers_env(&stack.data[i], old_envs, new_envs, num);
            break;
        default:
            break;
        }
    }
}

/*
void eval_set_register(RegType type, void *setting)
{
    switch(type) {
    case RT_VAL:
        val = setting;
        break;
    case RT_EXP:
        exp = setting;
        break;
    case RT_ARGL:
        argl = setting;
        break;
    case RT_UNEV:
        unev = setting;
        break;
    case RT_PROC:
        proc = setting;
        break;
    case RT_ENV:
        env = setting;
        break;
    case RT_CODE_POINTER:
        code_pointer = setting;
        break;
    case RT_TOP_CODE_POINTER:
        top_code_pointer = setting;
        break;
    case RT_CONT:
        cont = *(int *)setting;
        break;
    case RT_PROC_PRIM:
        proc_prim = setting;
        break;
    case RT_LAST_ARG_POINTER:
        last_arg_pointer = setting;
        break;
    }
}
*/

int eval_init()
{
    int max_num_environments = (int)eng_get_prop("max_num_environments");
    int max_num_blocks = (int)eng_get_prop("max_num_blocks");

    stack.next = 0;
    cont = JUMP_CODE;

    env = NULL;
    val = exp = argl = unev = proc = NULL;
    code_pointer = top_code_pointer = NULL;

    env_init(max_num_environments);
    data_init(max_num_blocks);

    GlobalEnv = env_create(NULL);

    DataNIL = malloc(sizeof(*DataNIL));
    DataNIL->marked = false;
    DataNIL->filename = -1;
    DataNIL->line_num = 0;
    DataNIL->type = DT_NIL;
    DataNIL->data.pair.car = DataNIL;
    DataNIL->data.pair.cdr = DataNIL;

    DataTrue = malloc(sizeof(*DataTrue));
    DataTrue->marked = false;
    DataTrue->filename = -1;
    DataTrue->line_num = 0;
    DataTrue->type = DT_T;

    /* define special "nil" variable */
    data_t *tmp = data_create(-1, 
                              0,
                              DT_VARIABLE,
                              L"nil",
                              NULL,
                              NULL);
    define_variable_value(tmp, DataNIL, GlobalEnv);

    have_error = false;

    kill_if_error = (bool)eng_get_prop("kill_if_error");

    return 0;
}

void eval_destroy()
{
    env_destroy(GlobalEnv);

    data_close();
    env_close();
}

data_t *eval(reader_node_t *code)
{
#ifdef DEBUG
    reader_disp(code);
#endif

    char et;
    data_t *val_tmp;

    save_regs();

EVAL_ENTRY:
    dump_regs("EVAL_ENTRY pre");

    if(! code)
        goto EXIT_EVAL;

    code_pointer = reader_to_data(code, false, false);
    
#ifdef DEBUG
    pretty_print(code_pointer);
#endif

    top_code_pointer = code_pointer;
    env = GlobalEnv;

    exp = code_pointer;

//    pretty_print(exp);

    dump_regs("EVAL_ENTRY post");

    if(IS_PAIR(exp))
        goto EVAL_BEGIN;

EVAL_DISPATCH_NEXT_CODE:
    dump_regs("EVAL_DISPATCH_NEXT_CODE pre");

    exp = code_pointer;

//    if(! code_pointer)
//        goto EXIT_EVAL;

    dump_regs("EVAL_DISPATCH_NEXT_CODE post");

EVAL_DISPATCH:
    dump_regs("EVAL_DISPATCH pre");

    et = exp->type;

    switch(et) {
    case DT_STRING:
    case DT_NUMBER:
    case DT_NIL:
    case DT_T:
        goto EVAL_SELF_EVAL;
    case DT_VARIABLE:
        goto EVAL_VARIABLE;
    case DT_SYMBOL:
        goto EVAL_QUOTED;
    case DT_PAIR:
        if(util_wcs_eq(CAR(exp)->data.text, L"define") )
            if(IS_PAIR(CAR(CDR(exp))))
                goto EVAL_DEFINE_LAMBDA;
            else
                goto EVAL_DEFINITION;
        else if(util_wcs_eq(CAR(exp)->data.text, L"macro") )
            goto EVAL_MACRO_LAMBDA;
        else if(util_wcs_eq(CAR(exp)->data.text, L"set") )
            goto EVAL_ASSIGNMENT;
        else if(util_wcs_eq(CAR(exp)->data.text, L"begin") )
            goto EVAL_BEGIN_STATEMENT;
        else if(util_wcs_eq(CAR(exp)->data.text, L"lambda") )
            goto EVAL_LAMBDA;
        else if(util_wcs_eq(CAR(exp)->data.text, L"if") )
            goto EVAL_IF;
        else if(util_wcs_eq(CAR(exp)->data.text, L"eval") )
            goto EVAL_EVAL;
        else if(util_wcs_eq(CAR(exp)->data.text, L"import") )
            goto EVAL_IMPORT;
        else
            goto EVAL_APPLICATION;        
    default:
        printf("problem in EVAL_DISPATCH\n");
        pretty_print(exp);
        return;
    }

EVAL_SELF_EVAL:
    dump_regs("EVAL_SELF_EVAL pre");

    val = exp;

    dump_regs("EVAL_SELF_EVAL post");
    goto JUMP_TO_CONTINUE;

EVAL_VARIABLE:
    dump_regs("EVAL_VARIABLE pre");

    val = lookup_variable_value(exp, env);

    dump_regs("EVAL_VARIABLE post");
    goto JUMP_TO_CONTINUE;

EVAL_QUOTED:
    dump_regs("EVAL_QUOTED pre");
    
    val = possible_symbol_to_data(exp);
    
    dump_regs("EVAL_QUOTED post");
    goto JUMP_TO_CONTINUE;

EVAL_LAMBDA:
    dump_regs("EVAL_LAMBDA pre");

    unev = CAR(CDR(exp));
    exp = CDR(CDR(exp));
    val = make_procedure(unev, exp, env);

    dump_regs("EVAL_LAMBDA post");
    goto JUMP_TO_CONTINUE;

EVAL_DEFINE_LAMBDA:
    dump_regs("EVAL_DEFINE_LAMBDA pre");

    unev = CDR(CAR(CDR(exp)));
    //exp = CDR(CDR(exp));
    val = make_procedure(unev, CDR(CDR(exp)), env);
    unev = CAR(CAR(CDR(exp)));
    define_variable_value(unev, val, env);
    val = DataTrue;

    dump_regs("EVAL_DEFINE_LAMBDA post");
    goto JUMP_TO_CONTINUE;

EVAL_MACRO_LAMBDA:
    dump_regs("EVAL_MACRO_LAMBDA pre");

    unev = CDR(CAR(CDR(exp)));
    //exp = CDR(CDR(exp));
    val = make_macro(unev, CDR(CDR(exp)));
    unev = CAR(CAR(CDR(exp)));
    define_variable_value(unev, val, env);
    val = DataTrue;

    dump_regs("EVAL_MACRO_LAMBDA post");
    goto JUMP_TO_CONTINUE;

EVAL_EVAL:
    dump_regs("EVAL_EVAL pre");
    //push_stack(cont, ST_CONT);
    //cont = JUMP_EVAL_EVAL_CONT;
    exp = CAR(CDR(exp));

    if(IS_STRING_TYPE(exp)) {
        run_eval_on_string(exp);
        /* val holds the result of the eval */
    } else {
        val = data_create(exp->filename, exp->line_num,
                          DT_DATAERROR,
                          L"argument to eval is not a string type",
                          NULL, NULL);        
    }
    
    dump_regs("EVAL_EVAL post");
    goto JUMP_TO_CONTINUE;

EVAL_IMPORT:
    dump_regs("EVAL_IMPORT pre");
    exp = CAR(CDR(exp));

    if(IS_STRING(exp)) {
        char * filename = util_to_mbs(exp->data.text);
    
        run_eval_on_file(filename);
    } else if(IS_STRING_TYPE(exp)) {
        //run_eval_on_string(exp);
        push_stack(cont, ST_CONT);

        cont = JUMP_EV_IMPORT_AUX;

        goto EVAL_DISPATCH;

        /* val holds the result of the eval */
        
        //char * filename = util_to_mbs(val->data.text);
    
        //run_eval_on_file(filename);

    } else {	
        val = data_create(exp->filename, exp->line_num,
                          DT_DATAERROR,
                          L"argument to import is not a string type",
                          NULL, NULL);
    }
    
    dump_regs("EVAL_IMPORT post");

    goto JUMP_TO_CONTINUE;

EVAL_IMPORT_AUX:
    dump_regs("EVAL_IMPORT pre");

    if(IS_STRING(val)) {
        char * filename = util_to_mbs(val->data.text);
    
        run_eval_on_file(filename);
    } else {	
        val = data_create(val->filename, val->line_num,
                          DT_DATAERROR,
                          L"argument to import is not a string type",
                          NULL, NULL);
    }
    
    dump_regs("EVAL_IMPORT post");

    cont = pop_stack();

    goto JUMP_TO_CONTINUE;

EVAL_IF:
    dump_regs("EVAL_IF pre");
    push_stack(exp, ST_DATA);
    push_stack(env, ST_ENV);
    push_stack(cont, ST_CONT);
    cont = JUMP_EV_IF_DECIDE;
    exp = CAR(CDR(exp));
    dump_regs("EVAL_IF post");
    goto EVAL_DISPATCH;

EV_IF_DECIDE:
    dump_regs("EV_IF_DECIDE pre");
    cont = pop_stack();
    env = pop_stack();
    exp = pop_stack();
    if(val != DataNIL)
        goto EV_IF_CONSEQUENT;

    /* fall-through */

EV_IF_ALTERNATIVE:
    exp = CAR(CDR(CDR(CDR(exp))));
    goto EVAL_DISPATCH;

EV_IF_CONSEQUENT:
    exp = CAR(CDR(CDR(exp)));
    goto EVAL_DISPATCH;

EVAL_ASSIGNMENT:
    dump_regs("EVAL_ASSIGNMENT pre");

    unev = CAR(CDR(exp));
    push_stack(unev, ST_DATA);
    exp = CAR(CDR(CDR(exp)));
    push_stack(env, ST_ENV);
    push_stack(cont, ST_CONT);
    cont = JUMP_EV_ASSIGNMENT_1;

    dump_regs("EVAL_ASSIGNMENT post");
    goto EVAL_DISPATCH;

EV_ASSIGNMENT_1:
    dump_regs("EV_ASSIGNMENT_1 pre");

    cont = pop_stack();
    env = pop_stack();
    unev = pop_stack();
    set_variable_value(unev, val, env);
    val = DataTrue;

    dump_regs("EV_ASSIGNMENT_1 post");
    goto JUMP_TO_CONTINUE;


EVAL_DEFINITION:
    dump_regs("EVAL_DEFINITION pre");

    unev = CAR(CDR(exp));
    push_stack(unev, ST_DATA);
    exp = CAR(CDR(CDR(exp)));
    push_stack(env, ST_ENV);
    push_stack(cont, ST_CONT);
    cont = JUMP_EV_DEFINITION_1;

    dump_regs("EVAL_DEFINITION post");
    goto EVAL_DISPATCH;

EV_DEFINITION_1:
    dump_regs("EV_DEFINITION_1 pre");

    cont = pop_stack();
    env = pop_stack();
    unev = pop_stack();
    define_variable_value(unev, val, env);
    val = DataTrue;

    dump_regs("EV_DEFINITION_1 post");
    goto JUMP_TO_CONTINUE;

EVAL_APPLICATION:
    dump_regs("EVAL_APPLICATION pre");

    push_stack(cont, ST_CONT);
    push_stack(env, ST_ENV);
    unev = CDR(exp);
    push_stack(unev, ST_DATA);
    exp = CAR(exp);
    cont = JUMP_EV_APPL_DID_OPERATOR;

    dump_regs("EVAL_APPLICATION post");
    goto EVAL_DISPATCH;

EV_APPL_DID_OPERATOR:
    dump_regs("EV_APPL_DID_OPERATOR pre");

    unev = pop_stack();
    env = pop_stack();
    argl = DataNIL;
    last_arg_pointer = NULL;
    proc = val;

    /* if we ended up with a string type in the procedure name, then
     * attempt to look that up in the environment  */
    if(IS_STRING_TYPE(proc))
        proc = lookup_variable_value(proc, env);

    if(is_macro(proc)) {
        //cont = pop_stack();
        cont = JUMP_MACRO_FINISH;
        push_stack(cont, ST_CONT);
        goto MACRO_APPLY;
    }


    if(unev == DataNIL)
        goto APPLY_DISPATCH;
    push_stack(proc, ST_DATA);

    /* fall through */

EV_APPL_OPERAND_LOOP:
    dump_regs("EV_APPL_OPERAND_LOOP pre");

    push_stack(argl, ST_DATA);
    push_stack(last_arg_pointer, ST_ARG_POINTER);
    exp = CAR(unev);
    if(CDR(unev) == DataNIL)
        goto EV_APPL_LAST_ARG;
    push_stack(env, ST_ENV);
    push_stack(unev, ST_DATA);
    cont = JUMP_EV_APPL_ACCUMULATE_ARG;
    goto EVAL_DISPATCH;

EV_APPL_ACCUMULATE_ARG:
    dump_regs("EV_APPL_ACCUMULATE_ARG pre");

    unev = pop_stack();
    env = pop_stack();
    last_arg_pointer = pop_stack();
    argl = pop_stack();
    append_to_last(&argl, val);
    unev = CDR(unev);

    dump_regs("EV_APPL_ACCUMULATE_ARG post");
    goto EV_APPL_OPERAND_LOOP;

EV_APPL_LAST_ARG:
    dump_regs("EV_APPL_LAST_ARG pre");

    cont = JUMP_EV_APPL_ACCUM_LAST_ARG;

    dump_regs("EV_APPL_LAST_ARG post");
    goto EVAL_DISPATCH;

EV_APPL_ACCUM_LAST_ARG:
    dump_regs("EV_APPL_ACCUM_LAST_ARG pre");

    last_arg_pointer = pop_stack();
    argl = pop_stack();

    append_to_last(&argl, val);

    proc = pop_stack();

    dump_regs("EV_APPL_ACCUM_LAST_ARG post");
    goto APPLY_DISPATCH;

APPLY_DISPATCH:
    dump_regs("APPLY_DISPATCH pre");
    if(is_primitive_proc(proc))
        goto PRIMITIVE_APPLY;
    if(is_compound_proc(proc))
        goto COMPOUND_APPLY;
    goto UNKNOWN_PROCEDURE_TYPE;
    
PRIMITIVE_APPLY:
    dump_regs("PRIMITIVE_APPLY pre");

    val = proc_prim(argl);
    cont = pop_stack();

    dump_regs("PRIMITIVE_APPLY post");
    goto JUMP_TO_CONTINUE;

COMPOUND_APPLY:
    dump_regs("COMPOUND_APPLY pre");
    assert(IS_PROCEDURE(proc));
    unev = proc->data.procedure.args;
    env = proc->data.procedure.env;
    env = extend_environment(unev, argl, env);
    unev = proc->data.procedure.code;

    dump_regs("COMPOUND_APPLY post");
    goto EV_SEQUENCE;

MACRO_APPLY:
    dump_regs("MACRO_APPLY pre");
    assert(IS_MACRO(proc));
    argl = unev;
    unev = proc->data.procedure.args;
    env = extend_environment(unev, argl, env);
    unev = proc->data.procedure.code;
    /* do a separate eval on the result of the macro expansion */
    dump_regs("MACRO_APPLY post");
    goto EV_SEQUENCE;

MACRO_FINISH:
    dump_regs("MACRO_FINISH pre");
    exp = val;
#ifdef DEBUG_MACRO
    pretty_print(exp);
#endif
    cont = pop_stack();
    dump_regs("MACRO_FINISH post");
    goto EVAL_DISPATCH;
    
UNKNOWN_PROCEDURE_TYPE:
    dump_regs("UNKNOWN_PROCEDURE_TYPE");    

    cont = pop_stack();
    {
        wchar_t error[256];
        swprintf(error, 256, L"unknown procedure, line:%d", val->line_num);
        val = data_create(-1, 0, DT_DATAERROR, error, NULL, NULL);
    }
    goto EXIT_EVAL;

EVAL_BEGIN_STATEMENT:
    exp = CDR(exp);

    /* fall-through */

EVAL_BEGIN:
    dump_regs("EVAL_BEGIN pre");
    
    unev = exp;
    push_stack(cont, ST_CONT);

    dump_regs("EVAL_BEGIN post");
    goto EV_SEQUENCE;

EV_SEQUENCE:
    dump_regs("EV_SEQUENCE pre");

    exp = CAR(unev);
    if(CDR(unev) == DataNIL)
        goto EV_SEQUENCE_LAST_EXP;
    push_stack(unev, ST_DATA);
    push_stack(env, ST_ENV);
    cont = JUMP_EV_SEQUENCE_CONTINUE;
    goto EVAL_DISPATCH;    
    
EV_SEQUENCE_CONTINUE:
    dump_regs("EV_SEQUENCE_CONTINUE pre");

    env = pop_stack();
    unev = pop_stack();
    unev = CDR(unev);

    dump_regs("EV_SEQUENCE_CONTINUE post");
    goto EV_SEQUENCE;

EV_SEQUENCE_LAST_EXP:
    dump_regs("EV_SEQUENCE_LAST_EXP pre");

    cont = pop_stack();

    dump_regs("EV_SEQUENCE_LAST_EXP post");
    goto EVAL_DISPATCH;

JUMP_TO_CONTINUE:

    /* if we hit an error and we're killing after errors than return
     * this and reset the error flag */
    if(have_error && kill_if_error) {
        val = error_value;
        have_error = false;
    }

    if(IS_DATAERROR(val))
        goto EXIT_EVAL;

    //if(val->type == DT_DATAERROR)
    //goto EXIT_EVAL;
   
    switch(cont) {
    case JUMP_CODE:
        /*
        if(code_pointer->type != DT_PAIR)
            goto EXIT_EVAL;

        code_pointer = CDR(code_pointer);
        goto EVAL_DISPATCH_NEXT_CODE;
        */
        goto EXIT_EVAL;
    case JUMP_EV_DEFINITION_1:
        goto EV_DEFINITION_1;
    case JUMP_EV_ASSIGNMENT_1:
        goto EV_ASSIGNMENT_1;
    case JUMP_EV_APPL_DID_OPERATOR:
        goto EV_APPL_DID_OPERATOR;
    case JUMP_EV_APPL_ACCUMULATE_ARG:
        goto EV_APPL_ACCUMULATE_ARG;
    case JUMP_EV_APPL_ACCUM_LAST_ARG:
        goto EV_APPL_ACCUM_LAST_ARG;
    case JUMP_EV_SEQUENCE_CONTINUE:
        goto EV_SEQUENCE_CONTINUE;
    case JUMP_EV_SEQUENCE:
        goto EV_SEQUENCE;
    case JUMP_EV_IF_DECIDE:
        goto EV_IF_DECIDE;
    case JUMP_MACRO_FINISH:
        goto MACRO_FINISH;
    case JUMP_EV_IMPORT_AUX:
        goto EVAL_IMPORT_AUX;
    }

EXIT_EVAL:

    free(top_code_pointer);

    val_tmp = val;

    restore_regs();

    return val_tmp;
}

DLL_INFO short eval_get_current_val_filename(void)
{
   if(val)
     //return get_filename_by_index(val->filename);
     return val->filename;
   else
     return -1;
}

DLL_INFO short eval_get_current_val_line_num(void)
{
   if(val)
     return val->line_num;
   else
     return 0;
}
