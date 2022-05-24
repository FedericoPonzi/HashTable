#include "hash_table.h"
#include "prime.h"

#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

/// Sentinel value for a deleted item
static Item HT_DELETED_ITEM = {NULL, NULL};
/// Initial hash table available size
static const size_t HT_INITIAL_BASE_SIZE = 50;
/// Max hashtable load before resizing up
static const size_t MAX_LOAD_UP_PERC = 70;
/// Min hashtable load before resizing down
static const size_t MIN_LOAD_DOWN_PERC = 10;
static const bool IS_RESIZABLE = true;

int ht_hash_table_HashMap_get_load(HashMap *hm);

static Item *ht_hash_table_Item_new(const char *k, const char *v) {
    Item *item = malloc(sizeof(Item));
    item->key = strdup(k);
    item->value = strdup(v);
    return item;
}

static void ht_hash_table_Item_destroy(Item *item) {
    free(item->key);
    free(item->value);
    free(item);
}

/**
 * The variable a should be a prime number larger than the size of the alphabet.
 * We're hashing ASCII strings, which has an alphabet size of 128, so we should choose a prime larger than that.
 * Pathological data: for any hash function, there is a 'pathological' set of inputs, which all hash to the same value
 * o find this set of inputs, run a large set of inputs through the function. All inputs which hash to a particular
 * bucket form a pathological set.
 * Pathological inputs also poses a security issue. If a hash table is fed a set of colliding keys by some malicious user,
 * then searches for those keys will take much longer (O(n)) than normal (O(1)). This can be used as a denial of service
 * attack against systems which are underpinned by hash tables, such as DNS and certain web services.
 * @param s
 * @param prime_number
 * @param bucket_array_length
 * @return
 */
int ht_hash_table_HashMap_single_hash(const char *s, const int prime_number, const size_t bucket_array_length) {
    /// Ascii:
    const int ALPHABET_SIZE = 128;

    assert(prime_number > ALPHABET_SIZE);
    long ret = 0;
    const double length = (double) strlen(s);
    for (int i = 0; i < length; i++) {
        //printf("ret: %ld, ret prior: %ld, ret final: %ld\n", ret, (long)pow(prime_number, length - i + 1) * s[i], (long)pow(prime_number, length - i + 1) * s[i] % bucket_array_length);
        ret += (long) pow((double) prime_number, length - i + 1) * s[i];
        ret = ret % (long) bucket_array_length;
    }
    return (int) ret;
}

/**
 * Double hashing of the key.
 * @param s
 * @param bucket_array_length
 * @param attempt
 * @return
 */
static int ht_hash_table_HashMap_hash(const char *s, const size_t bucket_array_length, const int attempt) {
    const int PRIME_1 = 168;
    const int PRIME_2 = 169;
    const int hash_a = ht_hash_table_HashMap_single_hash(s, PRIME_1, bucket_array_length);
    const int hash_b = ht_hash_table_HashMap_single_hash(s, PRIME_2, bucket_array_length);
    return (hash_a + (attempt * (hash_b + 1))) % (int) bucket_array_length;
}

static HashMap *ht_hash_table_HashMap_new_sized(const size_t base_size) {
    HashMap *hm = malloc(sizeof(HashMap));
    hm->base_size = base_size;
    hm->size = ht_prime_next_prime(hm->base_size);
    hm->count = 0;
    hm->items = calloc(hm->size, sizeof(Item *));
    return hm;
}

inline int ht_hash_table_HashMap_get_load(HashMap *hm) {
    return (int) (((double) hm->count) * 100 / ((double) hm->size));
}


static void ht_hash_table_HashMap_swap(HashMap* old_ht, HashMap* new_ht){
    size_t tmp = old_ht->base_size;
    old_ht->base_size = new_ht->base_size;
    new_ht->base_size = tmp;

    tmp = old_ht->count;
    old_ht->count = new_ht->count;
    new_ht->count = tmp;

    const size_t tmp_size = old_ht->size;
    old_ht->size = new_ht->size;
    new_ht->size = tmp_size;

    Item **tmp_items = old_ht->items;
    old_ht->items = new_ht->items;
    new_ht->items = tmp_items;
}

static void ht_hash_table_HashMap_resize(HashMap *ht, size_t base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    HashMap *new_ht = ht_hash_table_HashMap_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        Item *item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_hash_table_HashMap_insert(new_ht, item->key, item->value);
        }
    }q
    ht_hash_table_HashMap_swap(ht, new_ht);
    ht_hash_table_HashMap_destroy(new_ht);
}

static void ht_hash_table_HashMap_resize_up(HashMap *ht) {
    const size_t new_size = ht->base_size * 2;
    ht_hash_table_HashMap_resize(ht, new_size);
}

static void ht_hash_table_HashMap_resize_down(HashMap *ht) {
    const size_t new_size = ht->base_size / 2;
    ht_hash_table_HashMap_resize(ht, new_size);
}

/*
 *****************************
 * Public APIs:
 *****************************
*/

HashMap *HashMap_new() {
    return ht_hash_table_HashMap_new_sized(HT_INITIAL_BASE_SIZE);
}

void ht_hash_table_HashMap_destroy(HashMap *hashMap) {

    for (int i = 0; i < hashMap->size; i++) {
        Item *item = hashMap->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_hash_table_Item_destroy(item);
        }
    }

    free(hashMap->items);
    free(hashMap);
}

// Return the value v associated with key k from the associative array a, or NULL if the key does not exist.
const char *ht_hash_table_HashMap_search(HashMap *map, const char *k) {
    int attempt = 0;
    int index = ht_hash_table_HashMap_hash(k, map->size, attempt);
    Item *item = map->items[index];
    while (map->items[index] != NULL) {
        if (item != &HT_DELETED_ITEM && strcmp(item->key, k) == 0) {
            return item->value;
        }
        attempt++;
        index = ht_hash_table_HashMap_hash(k, map->size, attempt);
        item = map->items[index];
    }
    return NULL;
}

// Store the pair k:v in the associative array map.
void ht_hash_table_HashMap_insert(HashMap *map, const char *key, const char *value) {
    const int load = ht_hash_table_HashMap_get_load(map);
    if (load > MAX_LOAD_UP_PERC && IS_RESIZABLE) {
            ht_hash_table_HashMap_resize_up(map);
    }
    Item *new_item = ht_hash_table_Item_new(key, value);
    int attempt = 0;
    int index = ht_hash_table_HashMap_hash(key, map->size, attempt);
    while (map->items[index] != NULL && map->items[index] != &HT_DELETED_ITEM && strcmp(map->items[index]->key, key) != 0)  {
        attempt++;
        index = ht_hash_table_HashMap_hash(key, map->size, attempt);
    }
    map->items[index] = new_item;
    map->count++;
}


/**
 * Delete the k:v pair associated with k, or do nothing if k does not exist.
 *
 * The item we wish to delete may be part of a collision chain. Removing it from the table will break that chain,
 * and will make finding items in the tail of the chain impossible.
 * To solve this, instead of deleting the item, we simply mark it as deleted.
 * @param map
 * @param k
 */
void ht_hash_table_HashMap_remove(HashMap *map, const char *key) {
    int attempt = 0;
    int index = ht_hash_table_HashMap_hash(key, map->size, attempt);
    Item *item = map->items[index];
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM && strcmp(item->key, key) == 0) {
            ht_hash_table_Item_destroy(item);
            map->items[index] = &HT_DELETED_ITEM;
            map->count--;
            break;
        }
        attempt++;
        index = ht_hash_table_HashMap_hash(key, map->size, attempt);
        item = map->items[index];
    }
    const int load = ht_hash_table_HashMap_get_load(map);
    if (load < MIN_LOAD_DOWN_PERC && IS_RESIZABLE) {
        ht_hash_table_HashMap_resize_down(map);
    }
}

