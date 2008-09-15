#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <wchar.h>
#include <string.h>

#include "reader.h"
#include "utils.h"

#define ADD_TO_ERROR_STREAM(output)             \
    add_to_stream(&output_stream_error,         \
                  &output_stream_error_cap,     \
                  &output_stream_error_loc,     \
                  ( output ) );

#define ADD_TO_OUTPUT_STREAM(output)             \
    add_to_stream(&output_stream,                \
                  &output_stream_cap,            \
                  &output_stream_loc,            \
                  ( output ) );

/* the root and current reader node we are on */
static reader_node_t *root;
static reader_node_t *curnode;

static char *stream;
static int stream_len;

static char *cur_token;
static int cur_token_len;

/* vars to maintain user output */
static char *output_stream_error;
static unsigned int output_stream_error_cap;
static int output_stream_error_loc;
static char *output_stream;
static unsigned int output_stream_cap;
static int output_stream_loc;

static char **cur_filename;
static int cur_filename_index;
static int cur_filename_cap;
static int cur_filename_size;
static int line_num;

static int find_filename_index_by_name(char *filename)
{    
    int i=0;
    int filename_len;
    char mod_filename[33];

    /* limit filenames and strings to 32 chars for debugging */
    if(strlen(filename) > 32)
        memcpy(mod_filename, filename, 32);
    else
        strcpy(mod_filename, filename);
    mod_filename[32] = 0;

    for(i=0; i<cur_filename_size; i++)
        if(strcmp(mod_filename, cur_filename[i]) == 0)
            return i;

    cur_filename = util_grow_buffer_to_size(cur_filename,
                                            &cur_filename_cap,
                                            cur_filename_size+1,
                                            sizeof(*cur_filename));
    filename_len = strlen(mod_filename) + 1;
    cur_filename[cur_filename_size] = malloc(sizeof(*mod_filename) * filename_len);
    memcpy(cur_filename[cur_filename_size++], mod_filename, filename_len);

    return cur_filename_size-1;
}

char *get_filename_by_index(int index)
{
    if(index < 0)
        return "none";

    if(cur_filename)
        return cur_filename[index];
    
    return NULL;
}

void destroy_filenames(void)
{
    int i=0;
    for(i=0; i<cur_filename_size; i++)
        free(cur_filename[i]);

    free(cur_filename);

    cur_filename_cap = 0;
    cur_filename_size = 0;
    cur_filename_index = 0;
    cur_filename = NULL;
}

static void inc_stream() {
    --stream_len;
    stream++;
}

static void dec_stream() {
    ++stream_len;
    stream--;
}

static void add_to_stream(char **stream,
                          unsigned int *cap,
                          int *cur_location,
                          char *text)
{
    int size = strlen(text) + 1;
    *stream = util_grow_buffer_to_size(*stream, cap, size, sizeof(**stream));
    memcpy(*stream + *cur_location, text, size);
    *cur_location += size-1;
}

static void print_line_num()
{
    char output[128];
    snprintf(output, 128, " line number %d\n", line_num);

    ADD_TO_ERROR_STREAM(output);
}

static reader_node_t *create_node(reader_node_t *parent,
                                  char* token_string,
                                  int token_string_len)
{
    reader_node_t *node = malloc(sizeof(*node));
    node->child = NULL;
    node->parent = parent;
    node->next = NULL;

    node->token = malloc(sizeof(*node->token) * (token_string_len+1));
    memcpy(node->token, token_string, token_string_len);
    node->token[token_string_len] = 0;

    node->token_type = SRT_UNCLASSIFIED;
    node->data = NULL;

    node->line_num = line_num;
    node->filename_index = cur_filename_index;

    //printf("read token: %s, length = %d\n", node->token, token_string_len);
    
    return node;
}

static void destroy_node(reader_node_t *node)
{
    if(node->data)
        free(node->data);

    free(node->token);
    free(node);
}

static void consume_whitespace()
{
    while(stream_len)
    {
        uint8 cc = *stream;

        if(cc == '\n')
            line_num++;
	
        if(cc != ' ' && cc != '\t' && cc != '\n')
            return;

        inc_stream();
    }
}

static bool consume_comment()
{
    bool first_char = true;

    while(stream_len)
    {
        uint8 cc = *stream;
        
        if(first_char && cc != ';')
            return false;

        first_char = false;
	
        if(cc == '\n')
            return true;

        inc_stream();
    }

    return false;
}


static void consume_token()
{
    bool in_string = false;
    bool in_paren_symbol = false;
    int paren_count = 0;

    cur_token = stream;
    cur_token_len = 1;    

    uint8 prev_char = '\0'; 
    uint8 cc = '\0';
    
    while(stream_len)
    {
        prev_char = cc;
        cc = *stream;

        inc_stream();

        if(in_paren_symbol) {
            if(cc == '(')
                ++paren_count;
            else if(cc ==')')
                --paren_count;
            
            if(paren_count == 0)
                return;

            cur_token_len++;

            continue;
        } else if(in_string) {
            if(cc != '"') {
                cur_token_len++;
                continue;
            } else if(cc == '"' && prev_char == '\\') {
                /* check for an escaped quote character */
                cur_token_len++;
                continue;
            } else
                return;
        }            

        if(cc == '(' || cc == ')') {
            if(cur_token_len > 1) {
                dec_stream();
                cur_token_len--;                
            }
            
            return;
        } else if(cc == ' ' || cc == '\t' || 
                  cc == '\n' || cc == ')' || 
                  cc == '(' || cc == ';')
        {
            cur_token_len--;
            dec_stream();
            return;
        } else if(cc == '"') {
            in_string = true;
        } else if(cc == '\'') {
            /* look ahead to see if paren */
            if(*stream == '(')
                in_paren_symbol = true;
        }

        cur_token_len++;
    }
}


char *get_output_stream(void) { return output_stream; }
char *get_output_stream_error(void) { return output_stream_error; }

static reader_node_t *invoke(reader_node_t *parent)
{
    reader_node_t *prev = NULL, *start_node = NULL, *node;

    while(stream_len) {
        consume_whitespace();
        while(consume_comment())
            consume_whitespace();

        if(!stream_len)
            break;

        consume_token();

        node = create_node(parent, cur_token, cur_token_len);

        if(!start_node)
            start_node = node;

        if(strcmp(node->token, "(") == 0)
            node->child = invoke(node);
        else if(strcmp(node->token, ")") == 0)
            return start_node;
        
        if(prev)
            prev->next = node;
        
        prev = node;

        consume_whitespace();
        
        if(!stream_len)
            return start_node;
    }

    return start_node;
}

#define READ_CHUNK_SIZE 64

reader_node_t *reader_read_file(char *filename)
{
    FILE *fin;
    unsigned int buffer_cap, buffer_len;
    char *buffer;

    fin = fopen(filename, "r");

    if(!fin)
        return NULL;

    cur_filename_index = find_filename_index_by_name(filename);
    line_num = 1;

    buffer = malloc(sizeof(*buffer) * READ_CHUNK_SIZE);
    buffer_cap = READ_CHUNK_SIZE;
    buffer_len = 0;

    while(!feof(fin)) {
        int num_read = (int)fread(&buffer[buffer_len], 1, READ_CHUNK_SIZE, fin);
        buffer_len += num_read;

        if(buffer_len + READ_CHUNK_SIZE >= buffer_cap)
            buffer = util_grow_buffer_to_size(buffer,
                                              &buffer_cap,
                                              (buffer_cap+1) + READ_CHUNK_SIZE,
                                              sizeof(*buffer));
    }

    fclose(fin);

    /* now invoke the reader */
    stream = buffer;
    stream_len = buffer_len;

    root = invoke(NULL);
    
    free(buffer);
    return root;
}

reader_node_t *reader_read_string(char *string)
{
    cur_filename_index = find_filename_index_by_name(string);
    line_num = 1;
    
    stream = string;
    stream_len = (int)strlen(string);
    
    root = invoke(NULL);

    return root;
}

static void reader_disp_node_token(reader_node_t *node)
{
    if(node->token)
        printf("%s", node->token);
}

static void reader_disp_node_filename(reader_node_t *node)
{
    printf("\t\t%s:%d",
           get_filename_by_index(node->filename_index),
           node->line_num);
}


static void reader_disp_node(reader_node_t *node, int level)
{
    int indent = level;
    
    while(indent > 0) {
        printf("  ");
        --indent;
    }

    reader_disp_node_token(node);
    reader_disp_node_filename(node);
    if(node->child) {
        printf("\n");
        reader_disp_node(node->child, level+1);
    }
    if(node->next) {
        printf("\n");
        reader_disp_node(node->next, level);
    }
}

void reader_disp(reader_node_t *node)
{
    printf("---------------------------------------------------------------------\n");
    printf("<<root>>\n");
    
    reader_disp_node(node, 0);

    printf("\n");
    printf("---------------------------------------------------------------------\n");
}
