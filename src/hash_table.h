#ifndef HASHTABLE_HASH_TABLE_H
#define HASHTABLE_HASH_TABLE_H
#include<stdlib.h>
typedef struct {
    char* key;
    char* value;
} Item;

typedef struct {
    size_t size;
    size_t count;
    size_t base_size;
    Item** items;
} HashMap;

HashMap* HashMap_new();

void ht_hash_table_HashMap_destroy(HashMap* hashMap);

const char* ht_hash_table_HashMap_search(HashMap* map, const char *k);

// Store the pair k:v in the associative array a.
void ht_hash_table_HashMap_insert(HashMap* map, const char *key, const char *value);

void ht_hash_table_HashMap_remove(HashMap* map, const char *key);

/// This is exposed due to testing, but should be static and private.
int ht_hash_table_HashMap_single_hash(const char *s, const int prime_number, const size_t bucket_array_length);

#endif //HASHTABLE_HASH_TABLE_H
