#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

#include "data.h"
#include "reader.h"
#include "types.h"

void *util_grow_buffer_to_size(void *buffer,
                               unsigned int *cur_capacity,
                               unsigned int desired_capacity,
                               unsigned int bytes_per_block);

char *util_to_mbs(String x);
String util_to_wcs(char *x);
bool util_wcs_eq(String arg1, String arg2);
bool util_mbs_eq(char *arg1, char *arg2);


void pretty_print(data_t *data);

DLL_INFO data_t *reader_to_data(reader_node_t *code, bool in_pair);

#endif  /* __UTILS_H__ */
