#ifndef __READER_H__
#define __READER_H__

#include "types.h"
#include "windll.h"


#define SRT_UNCLASSIFIED -1
#define SRT_PAIR         0
#define SRT_STRING       1
#define SRT_SYMBOL       2
#define SRT_VARIABLE     3
#define SRT_NUMBER       4

typedef struct reader_node_type {
    struct reader_node_type *child;
    struct reader_node_type *parent;
    struct reader_node_type *next;

    char *token;
    int filename_index;
    int line_num;

    /* classifier info */
    char token_type;
    void *data;
} reader_node_t;

reader_node_t *reader_read_file(char *filename);
reader_node_t *reader_read_string(char *string);

char *get_output_stream(void);
char *get_output_stream_error(void);

void reader_disp(reader_node_t *node);

DLL_INFO char *get_filename_by_index(int index);
void destroy_filenames(void);

#endif	/* __READER_H__ */
