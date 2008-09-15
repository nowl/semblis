#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

#define MAX_TABLE_USAGE (0.7f)
#define MAX_LOOPS 20

/* hash function */
/*
static unsigned long
sdbm(unsigned char *str)
{
    unsigned long hash = 0;
    int c, loop = 0;

    while ((c = *str++) != 0 && (loop++ < MAX_LOOPS))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
*/


static long
jenkins_one_at_a_time_hash(unsigned char *key,
                           size_t key_len)
{
    long hash = 0;
    size_t i;
 
    for (i = 0; i < key_len; i++) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}


unsigned long
hashtable_jenkins_hash_wchar(void* key)
{
    long hash = 0;
    wchar_t *str = key;
    wchar_t c;

    int loop = 0;
    //while (((c = *str++) != 0) && (loop++ < MAX_LOOPS)) {
    while ((c = *str++) != 0) {
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
 
    return hash;
}

unsigned long
hashtable_jenkins_hash_char(void* key)
{
    long hash = 0;
    char *str = key;
    char c;

    int loop = 0;
    //while (((c = *str++) != 0) && (loop++ < MAX_LOOPS)) {
    while ((c = *str++) != 0) {
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
 
    return hash;
}

unsigned long
hashtable_sdbm_hash_char(void *string)
{
    char *str = string;

    unsigned long hash = 0;
    char c;

    while ((c = *str++) != 0)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

unsigned long
hashtable_sdbm_hash_wchar(void *string)
{
    wchar_t *str = string;

    unsigned long hash = 0;
    wchar_t c;

    int loop = 0;
    while (((c = *str++) != 0) && (loop++ < MAX_LOOPS))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

/* custom string compare */
static bool
string_comp(void *key1, void *key2)
{
    unsigned char *str1;
    unsigned char *str2;
    
    if(key1 == NULL || key2 == NULL)
        return false;

    str1 = key1;
    str2 = key2;

    while( (*str1 != 0) && (*str2 != 0) ) {
        if(*str1++ != *str2++)
            return false;
    }

    if(*str1 || *str2)
        return false;

    return true;
}

/* custom string length */

static unsigned int
string_len(void *key)
{
    int i = 0;
    unsigned char *str = key;
    
    while(*str++)
        ++i;

    return i+1;
}

static void *
string_cpy(void *dest, void *src, size_t size)
{
    return memcpy(dest, src, size);
}

hashtable_t *
hashtable_create_str_str(int size, char flags) 
{
    return hashtable_create(size, flags, hashtable_jenkins_hash_char,
                            string_len, string_cpy, string_comp, string_len, string_cpy);
}

hashtable_t *
hashtable_create(int size, char flags, HashFunc hash_func,
                 SizeFunc key_size, CopyFunc key_copy, HashEqualFunc key_equal,
                 SizeFunc val_size, CopyFunc val_copy)
{
    hashtable_t *hashtable;
    unsigned int i;

    hashtable = malloc(sizeof(*hashtable));

    hashtable->hash	     = hash_func;
    hashtable->equal	     = key_equal;
    hashtable->elements	     = malloc(size * sizeof(*hashtable->elements));
    hashtable->capacity	     = size;
    hashtable->length	     = 0;
    hashtable->flags	     = flags;
    hashtable->_internal_put = false;
   
    hashtable->key_size_func = key_size;
    hashtable->key_copy_func = key_copy;
    hashtable->val_size_func = val_size;
    hashtable->val_copy_func = val_copy;

    for(i=0; i<hashtable->capacity; ++i) {
        hashtable->elements[i] = malloc(sizeof(*hashtable->elements[i]));
        hashtable->elements[i]->key = NULL;
        hashtable->elements[i]->value = NULL;
    }

    return hashtable;
}

bool
hashtable_destroy(hashtable_t *hashtable)
{
    unsigned int i;

    for(i=0; i<hashtable->capacity; ++i) {
        if(! hashtable->_internal_put) {
            /* delete the keys and values if this is a real destroy */
            if(hashtable->elements[i]->key) {
                if(hashtable->flags & HT_COPY_KEY)
                    free(hashtable->elements[i]->key);
                if(hashtable->flags & HT_COPY_VALUE)
                    free(hashtable->elements[i]->value);
            }
        }
        
        free(hashtable->elements[i]);
    }
    free(hashtable->elements);
    free(hashtable);
    
    return true;
}

static void
rebuild_hashtable(hashtable_t **hashtable_ptr) {
    unsigned int i;
    hashtable_t *hashtable = *hashtable_ptr;
    hashtable_t *new_hashtable;

    //snprintf(tmp, 128, "hashtable{%d}", (int)hashtable);

    //snprintf(tmp, 128, "rebuilding hashtable with size %d", 2 * hashtable->capacity);

    /* create a new hashtable with double the capacity */
    new_hashtable = 
        hashtable_create(2 * hashtable->capacity,
                         hashtable->flags,
                         hashtable->hash,
                         hashtable->key_size_func,
                         hashtable->key_copy_func,
                         hashtable->equal,
                         hashtable->val_size_func,
                         hashtable->val_copy_func);
    
    /* copy over all the values over to the new hashtable */
    for(i=0; i<hashtable->capacity; i++) {
        hashtable_entry_t *entry = hashtable->elements[i];

        if(entry->key) {
            /* _internal_put insures no extra copy */
            new_hashtable->_internal_put = true;
            hashtable_put(&new_hashtable, entry->key, entry->value);
            new_hashtable->_internal_put = false;
        }
    }

    /* delete the old table */
    hashtable->_internal_put = true;
    hashtable_destroy(hashtable);
    
    /* rereference the new table */
    *hashtable_ptr = new_hashtable;
}

bool
hashtable_put(hashtable_t **hashtable_ptr, void *key, void *value)
{
    int			 mod;    
    unsigned long	 hash;
    hashtable_t         *hashtable;
    hashtable_entry_t	*possible_hit;
    float                table_usage;
    
    hashtable = *hashtable_ptr;
    
    mod = hashtable->capacity - 1;
    hash = hashtable->hash(key) & mod;
    
    possible_hit = hashtable->elements[hash];
    while(possible_hit->key) {
        /* if this is the same key then this is the element we want */
        if(hashtable->equal(possible_hit->key, key)) {
            /* the hashtable will increment after this loop so
             * decrement now to keep it the same length */
            --hashtable->length;

            /* also delete this node's keys/values */
            if(hashtable->flags & HT_COPY_KEY)
                free(possible_hit->key);
            if(hashtable->flags & HT_COPY_VALUE)
                free(possible_hit->value);
            break;
        }
        
        hash = (hash+1) & mod;
        possible_hit = hashtable->elements[hash];
    }
//    printf("filling element %d with %s => %s\n", hash, key, value);
    
    if(hashtable->_internal_put) {
        /* this comes from the rebuild function */
        possible_hit->key = key;
        possible_hit->value = value;
    } else {
        /* this is a real user put */
        
        if(hashtable->flags & HT_COPY_KEY) {
            /* copy key */
            int string_length = hashtable->key_size_func(key);
            possible_hit->key = malloc(string_length);
            hashtable->key_copy_func(possible_hit->key, key, string_length);
        } else {
            /* just copy the pointer */
            possible_hit->key = key;
        }
    
        if(hashtable->flags & HT_COPY_VALUE) {
            /* copy value */
            int string_length = hashtable->val_size_func(value);
            possible_hit->value = malloc(string_length);
            hashtable->val_copy_func(possible_hit->value, value, string_length);
        } else {
            /* just copy the pointer */
            possible_hit->value = value;
        }
    }
    
    ++hashtable->length;

    /* if hashtable gets too large then rebuild as a larger
     * hashtable */
    table_usage = (float)hashtable->length / hashtable->capacity;
    if(table_usage > MAX_TABLE_USAGE) {
        rebuild_hashtable(hashtable_ptr);
    }
    
    return true;
}

void *
hashtable_get(hashtable_t *hashtable, void *key)
{
    int			 mod;
    unsigned long	 hash;
    hashtable_entry_t	*possible_hit;

    mod = hashtable->capacity - 1;
    hash = hashtable->hash(key) & mod;

    possible_hit = hashtable->elements[hash];
    
    while(possible_hit->key) {
        if( hashtable->equal(possible_hit->key, key) )
            return possible_hit->value;
        
        hash = (hash+1) & mod;
        possible_hit = hashtable->elements[hash];
    }

    return NULL;
}

static long get_misses;

void *
hashtable_debug_get(hashtable_t *hashtable, void *key)
{
    int			 mod;
    unsigned long	 hash;
    hashtable_entry_t	*possible_hit;
    
    get_misses = 0;

    mod = hashtable->capacity - 1;
    hash = hashtable->hash(key) & mod;

    possible_hit = hashtable->elements[hash];
    
    while(possible_hit->key) {
        if( hashtable->equal(possible_hit->key, key) )
            return possible_hit->value;

        get_misses++;
        
        hash = (hash+1) & mod;
        possible_hit = hashtable->elements[hash];
    }

    return NULL;
}

long hashtable_debug_get_misses(void)
{
    return get_misses;
}

bool
hashtable_remove(hashtable_t *hashtable, void *key)
{
    /* TODO: implement this, (currently not used) */

    return true;
}
