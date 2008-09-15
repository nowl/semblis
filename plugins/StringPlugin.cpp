#include <wchar.h>
#include <stdio.h>

extern "C" {
#include "ForeignMacros.h"
}

#define MAX_STRING_LEN 1024

FOREIGN_FUNCTION(string_plugin_add) {
    wchar_t result_string[MAX_STRING_LEN];
    wchar_t tmp_string[MAX_STRING_LEN];
    result_string[0] = 0;

    MIN_ARITY(2);
    
    while(! IS_NIL(args)) {
        data_t *arg = CAR(args);

        if(! IS_STRING_TYPE(arg) && ! IS_NUMBER(arg) )
            RETURN_ERROR(arg, "args to string_plugin_add must be strings or numbers");

        wchar_t new_string[MAX_STRING_LEN];

        if(IS_STRING_TYPE(arg)) {
            String new_string_tmp = arg->data.text;
            wcscpy(new_string, new_string_tmp);
        } else {
            swprintf(new_string,
                     MAX_STRING_LEN,
                     L"%f",
                     arg->data.number);
        }
        wcscpy(tmp_string, result_string);
        
        swprintf(result_string,
                 MAX_STRING_LEN,
                 L"%ls%ls",
                 tmp_string,
                 new_string);

        args = CDR(args);
    }

    RETURN_STRING(args, result_string);
}

FOREIGN_FUNCTION(string_plugin_num) {
    CHECK_ARITY(1);

    data_t *arg = CAR(args);

    if(! IS_STRING_TYPE(arg))
        RETURN_ERROR(arg, L"args to string_plugin_num must be string");

    const wchar_t* new_string = arg->data.text;
    char new_string_char[MAX_STRING_LEN];
    wcsrtombs(new_string_char, &new_string, MAX_STRING_LEN, NULL);

    double value = atof(new_string_char);

    if(value)
        RETURN_NUMBER(arg, value);

    RETURN_ERROR(arg, L"couldn't convert string to double");
}

FOREIGN_FUNCTION(string_plugin_split) {
    CHECK_ARITY(2);

    data_t *arg = CAR(args);

    if(! IS_STRING_TYPE(arg))
        RETURN_ERROR(arg, L"arguments string_plugin_split must be strings");

    String orig_str = arg->data.text;

    String str = (String)malloc(sizeof(*str) * (wcslen(orig_str) + 1));
    wcscpy(str, orig_str);

    args = CDR(args);
    arg = CAR(args);

    if(! IS_STRING_TYPE(arg)) {
        free(str);
        RETURN_ERROR(arg, L"arguments string_plugin_split must be strings");
    }

    String split_char_str = arg->data.text;

    if(wcslen(split_char_str) != 1) {
        free(str);
        RETURN_ERROR(arg, L"the second argument to string_plugin_split must be a single character in length");
    }

    wchar_t split_char = split_char_str[0];

    data_t *result = NULL;
    data_t *place_holder = NULL;

    // begin split operation
    int start = 0;
    unsigned int start_ind = 0;
    wchar_t c = -1;

    while(c != 0) {
        c = str[start++];

        if(c == split_char || c == 0) {
            str[start-1] = 0;

            data_t *tmp = data_create(arg->filename,
                                      arg->line_num,
                                      DT_STRING,
                                      &str[start_ind],
                                      NULL,
                                      NULL);

            tmp = data_create(arg->filename, arg->line_num,
                              DT_PAIR, tmp,
                              DataNIL, NULL);

            if(!result) {
                result = tmp;
                place_holder = result;
            } else {
                CDR(place_holder) = tmp;
                place_holder = tmp;
            }

            start_ind = start;
        }
    }

    free(str);

    if(result)
        return result;

    return DataNIL;
}

FOREIGN_FUNCTION(string_plugin_find) {
    CHECK_ARITY(2);

    data_t *arg = CAR(args);

    if(! IS_STRING_TYPE(arg))
        RETURN_ERROR(arg, L"arguments to string_plugin_find must be strings");

    String str = arg->data.text;

    args = CDR(args);
    arg = CAR(args);

    if(! IS_STRING_TYPE(arg))
        RETURN_ERROR(arg, L"arguments to string_plugin_find must be strings");

    String str_to_find = arg->data.text;

    int str_i = wcslen(str);
    int str_to_find_i = wcslen(str_to_find);

    double index = -1;

    if(str_to_find_i > str_i) RETURN_NUMBER(arg, index);

    int i;
    for(i = 0; i <= str_i - str_to_find_i; i++) {
        int j;
        bool match = true;
        for(j = 0; j < str_to_find_i; j++) {
            if(str_to_find[j] != str[j+i]) {
                match = false;
                break;
            }
        }

        if(match) {
            index = i;
            break;
        }
    }

    RETURN_NUMBER(arg, index);
}


FOREIGN_FUNCTION(string_plugin_trim) {
    CHECK_ARITY(1);

    data_t *arg = CAR(args);

    if(! IS_STRING_TYPE(arg))
        RETURN_ERROR(arg, L"argument to string_plugin_trim must be a string");

    String str = arg->data.text;

    wchar_t trimmed_str[MAX_STRING_LEN];

    int str_ind = 0;
    wchar_t c = -1;

    /* find beginning char */
    while(c != 0) {
        c = str[str_ind++];

        if(c != L' ' && c != L'\n' && c != L'\r' && c != L'\t' && c != 0) {
            wcscpy(trimmed_str, &str[str_ind-1]);
            break;
        }
    }

    /* find ending point */
    str_ind = wcslen(str) - str_ind;
    while(true) {
        c = trimmed_str[str_ind--];

        if(c != L' ' && c != L'\n' && c != L'\r' && c != L'\t' && c != 0) {
            trimmed_str[str_ind+2] = 0;
            break;
        }
    }

    RETURN_STRING(arg, trimmed_str);
}
