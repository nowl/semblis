#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <stdlib.h>

#include "windll.h"
#include "types.h"

typedef unsigned long (*HashFunc)(void *);
typedef bool (*HashEqualFunc)(void *key1, void *key2);
typedef unsigned int (*SizeFunc)(void *info);
typedef void* (*CopyFunc)(void *dest, void *src, size_t size);

typedef struct {
    void *key;
    void *value;
} hashtable_entry_t;

typedef struct {
    HashFunc hash;
    HashEqualFunc equal;
    SizeFunc key_size_func;
    CopyFunc key_copy_func;
    SizeFunc val_size_func;
    CopyFunc val_copy_func;

    unsigned int capacity;
    unsigned int length;

    hashtable_entry_t **elements;

    char flags;

    bool _internal_put;
} hashtable_t;

unsigned long hashtable_sdbm_hash_char(void *string);
unsigned long hashtable_sdbm_hash_wchar(void *string);

unsigned long hashtable_jenkins_hash_char(void *key);
unsigned long hashtable_jenkins_hash_wchar(void *key);

hashtable_t	*hashtable_create_str_str(int size,
					  char flags);

hashtable_t     *hashtable_create(int size,
                                  char flags,
                                  HashFunc hash_func,
                                  SizeFunc key_size,
                                  CopyFunc key_copy,
                                  HashEqualFunc key_equal,
                                  SizeFunc val_size,
                                  CopyFunc val_copy);

#define HT_COPY_KEY           0x01
#define HT_COPY_VALUE         0x02

DLL_INFO bool		 hashtable_put(hashtable_t **hashtable_ptr,
			       void *key,
			       void *value);

void		*hashtable_get(hashtable_t *hashtable,
			       void *key);

bool		 hashtable_remove(hashtable_t *hashtable,
				  void *key);

bool		 hashtable_destroy(hashtable_t *hashtable);


void *hashtable_debug_get(hashtable_t *hashtable, void *key);
long hashtable_debug_get_misses(void);

#endif	/* __HASHTABLE_H__ */
