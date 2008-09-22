#include <stdio.h>
#include <wchar.h>

#include "utils.h"
#include "macros.h"

//#define PRETTY_PRINT_HEADER
#define PRETTY_PRINT_ENV_HEADER
#define PRETTY_PRINT_FOOTER

static void rec_pretty_print(data_t *data,
                             int indent_level)
{
    int i;

    if(!data) {
        OUTPUT_REG("NULL pointer\n");
        return;
    }

    for(i=0; i<indent_level; i++)
        OUTPUT_REG("    ");
    
    switch(data->type)
    {
    case DT_PAIR:
        OUTPUT_REG("| pair\n");
        
        if(CAR(data))
            rec_pretty_print(CAR(data), indent_level+1);
        else
            OUTPUT_REG("car null\n");

        if(CDR(data))
            rec_pretty_print(CDR(data), indent_level);
        else
            OUTPUT_REG("cdr null\n");

        break;
    case DT_NUMBER:
        OUTPUT_REG("| number: %f\n", data->data.number);
        break;
    case DT_PROCEDURE:
        OUTPUT_REG("| procedure: \n");
        rec_pretty_print(data->data.procedure.code, indent_level+1);
        break;
    case DT_MACRO:
        OUTPUT_REG("| macro: \n");
        break;
    case DT_VARIABLE:
        OUTPUT_REG("| variable: %s\n", util_to_mbs(data->data.text));
        break;
    case DT_SYMBOL:
        OUTPUT_REG("| symbol: %s\n", util_to_mbs(data->data.text));
        break;
    case DT_STRING:
        OUTPUT_REG("| string: %s\n", util_to_mbs(data->data.text));
        break;
    case DT_NIL:
        OUTPUT_REG("| nil\n");
        break;
    case DT_T:
        OUTPUT_REG("| #T\n");
        break;
    case DT_DATAERROR:
        OUTPUT_REG("| file = %s:%d, error: %ls\n", 
                   get_filename_by_index(data->filename),
                   data->line_num,
                   data->data.text);
        break;
    default:
        OUTPUT_REG("| unknown type\n");
        break;
    }
}

void pretty_print(data_t *data)
{

#ifdef PRETTY_PRINT_HEADER
    OUTPUT_REG("\n---------- start of pretty print ----------\n");
#endif

    rec_pretty_print(data, 0);

#ifdef PRETTY_PRINT_HEADER
    OUTPUT_REG("---------- end of pretty print ----------\n\n");
#endif

#ifdef PRETTY_PRINT_FOOTER
    OUTPUT_REG("\n");
#endif
}

void semi_print(data_t *data)
{
    if(!data) {
        OUTPUT_REG("NULL pointer");
        return;
    }

    switch(data->type)
    {
    case DT_PAIR:
        OUTPUT_REG("pair: car=%d(0x%x) cdr=%d(0x%x)",
                   CAR(data), CAR(data),
                   CDR(data), CDR(data));
        break;
    case DT_NUMBER:
        OUTPUT_REG("number: %f", data->data.number);
        break;
    case DT_PROCEDURE:
        OUTPUT_REG("procedure: args=%d(0x%x) code=%d(0x%x) env=%d(0x%x)",
                   data->data.procedure.args,
                   data->data.procedure.args,
                   data->data.procedure.code,
                   data->data.procedure.code,
                   data->data.procedure.env,
                   data->data.procedure.env);
        break;
    case DT_MACRO:
        OUTPUT_REG("macro: args=%d(0x%x) code=%d(0x%x)",
                   data->data.procedure.args,
                   data->data.procedure.args,
                   data->data.procedure.code,
                   data->data.procedure.code);
        break;
    case DT_VARIABLE:
        OUTPUT_REG("variable: %s", util_to_mbs(data->data.text));
        break;
    case DT_SYMBOL:
        OUTPUT_REG("symbol: %s", util_to_mbs(data->data.text));
        break;
    case DT_STRING:
        OUTPUT_REG("string: %s", util_to_mbs(data->data.text));
        break;
    case DT_NIL:
        OUTPUT_REG("NIL");
        break;
    case DT_T:
        OUTPUT_REG("#T");
        break;
    case DT_DATAERROR:
        OUTPUT_REG("error: file = %s:%d, error: %ls\n", 
                   get_filename_by_index(data->filename),
                   data->line_num,
                   data->data.text);
        break;
    default:
        OUTPUT_REG("warning: unknown type!!");
        break;
    }
}

static void rec_pretty_print_env(environment_t *env)
{
    int i;
    hashtable_entry_t* entry;

    for(i=0; i<env->bindings->capacity; ++i) {
        entry = env->bindings->elements[i];
        
        if(entry->key) {
            data_t *slot = (data_t *)entry->value;
            OUTPUT_REG("%ls:\n", entry->key);
            rec_pretty_print(slot, 0);

            if(slot->type == DT_PROCEDURE && slot->data.procedure.env != env) {
                OUTPUT_REG("printing sub environment\n");
                rec_pretty_print_env(slot->data.procedure.env);
                OUTPUT_REG("finished printing sub environment\n");
            }
        }
    }
}

void pretty_print_env(environment_t *env)
{
#ifdef PRETTY_PRINT_ENV_HEADER
    OUTPUT_REG("---------- printing environment -----------\n");
#endif

    rec_pretty_print_env(env);

#ifdef PRETTY_PRINT_ENV_HEADER
    OUTPUT_REG("---------- end printing environment -----------\n");
#endif
}
