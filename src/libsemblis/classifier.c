#include <stdio.h>
#include <string.h>

#include "classifier.h"

static bool consume_number(reader_node_t *node)
{
    double	 number = 0.0f;
    double	 multiplier;
    bool     decimal_found;
    bool     first_char;
    bool     negativeFound;

    char *stream;
    int stream_len;

    stream = node->token;
    stream_len = strlen(stream);
    
    decimal_found     = false;
    first_char	      = true;
    multiplier	      = 0.1;
    negativeFound     = false;

    while(stream_len)
    {
        char cc;
        int digit;

        cc = *stream++;
        --stream_len;

        if(first_char)
        {
            first_char = false;
            if(cc == '-')
            {
                if(!stream_len)
                    return false;

                negativeFound = true;
                continue;
            }
        }

        if(cc == '.')
        {
            decimal_found = true;
            continue;
        }
        else if(cc < '0' || cc > '9')
        {
            return false;
        }
        else
        {
            digit = cc - 48;
	    
            if(! decimal_found)
            {
                number = number*10 + digit;
            }
            else
            {
                number += digit * multiplier;
                multiplier *= 0.1;
            }
        }
    }
    
    if(negativeFound)
        number = -number;
    
    //printf("number = %f\n", number);

    node->token_type = SRT_NUMBER;
    node->data = malloc(sizeof(double));
    *(double *)node->data = number;

    return true;
}

static bool consume_string(reader_node_t *node)
{
    char *stream;
    int stream_len;

    stream = node->token;
    stream_len = strlen(stream);

    if(stream[0] == '"' &&
       stream[stream_len-1] == '"') {
        ++stream_len;

        node->token_type = SRT_STRING;
        node->data = malloc(sizeof(*stream) * stream_len);
        memcpy(node->data, stream, stream_len);

        return true;
    }

    return false;
}

static bool consume_symbol(reader_node_t *node)
{
    char *stream;
    int stream_len;

    stream = node->token;
    stream_len = strlen(stream);

    if(stream[0] == '\'') {
        ++stream_len;

        node->token_type = SRT_SYMBOL;
        node->data = malloc(sizeof(*stream) * stream_len);
        memcpy(node->data, stream, stream_len);

        return true;
    }

    return false;
}

static bool consume_variable(reader_node_t *node)
{
    char *stream;
    int stream_len;

    stream = node->token;
    stream_len = strlen(stream);

    ++stream_len;

    node->token_type = SRT_VARIABLE;
    node->data = malloc(sizeof(*stream) * stream_len);
    memcpy(node->data, stream, stream_len);

    return true;    
}

static bool consume_pair(reader_node_t *node)
{
    char *stream;
    int stream_len;

    stream = node->token;
    stream_len = strlen(stream);

    if(stream[0] == '(' && stream_len == 1) {
        node->token_type = SRT_PAIR;
        
        return true;
    }

    return false;
}

static void classify(reader_node_t *node)
{
    if(node->token) {
        if( consume_number(node) ) {
            //printf("number found\n");
        } else if( consume_string(node) ) {
            //printf("string found\n");
        } else if( consume_symbol(node) ) {
            //printf("symbol found\n");
        } else if( consume_pair(node) ) {
            //printf("pair found\n");
        } else if( consume_variable(node) ) {
            //printf("variable found\n");
        } else {
            //printf("problem classifying\n");
        }
    } else {
        printf("problem classifying: no token!\n");
    }

    if(node->child)
        classify(node->child);
    if(node->next)
        classify(node->next);
}

void classifier_run(reader_node_t *node)
{
    classify(node);
}
