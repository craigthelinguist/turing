
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hashmap.h"

// Constants.
// ======================================================================

const int INIT_CAPACITY = 10;


// Typedefs.
// ======================================================================

typedef void (*FreeFunc)(void *);
typedef int (*CmpFunc)(void *, void *);
typedef unsigned int (*HashFunc)(void *);


// Struct declarations.
// ======================================================================

struct bucket {
   List *keys;
   List *vals;
};

static const char emptyblock[sizeof (struct bucket)];

struct map {
   int capacity;
   int items; // number of things in the map.
   int szkey;
   int szval;
   HashFunc hash;
   FreeFunc keyfree;
   FreeFunc valfree;
   CmpFunc keycmp;
   CmpFunc valcmp;
   struct bucket *table; // array of list ptrs.
};



// Internal function declarations.
// ======================================================================

static unsigned int hash (struct map *map, void *key);
static int should_resize (struct map *map);
static void rebuild (struct map *map);
static inline void init_bucket (struct map *map, int index);
static inline struct bucket free_bucket (struct bucket bucket);


// Internal functions.
// ======================================================================

static unsigned int hash (struct map *map, void *key)
{
   if (map->hash) return map->hash(key);
   else return 0;
}

static int should_resize (struct map *map)
{
   float density = map->items / map->capacity;
   return density > 0.75;
}

static inline void init_bucket (struct map *map, int index)
{
   map->table[index].keys = List_Make(10, map->szkey, map->keycmp, map->keyfree);
   map->table[index].vals = List_Make(10, map->szval, map->valcmp, map->valfree);
}

static inline struct bucket free_bucket (struct bucket bucket)
{
   List_Free(bucket.keys);
   List_Free(bucket.vals);
}

static inline int bucket_exists (struct map *map, int index)
{
   return memcmp(emptyblock, map->table + index, sizeof (struct bucket));
}

static void rebuild (struct map *map)
{

   // Allocate memory for new table.
   int new_capacity = map->capacity * 2;
   struct bucket *new_table = calloc(sizeof (struct bucket), new_capacity);
   struct bucket *old_table = map->table;

   // Copy all buckets across.
   int i;
   for (i=0; i < map->capacity; i++) {
      if (!bucket_exists(map, i)) continue;
      memcpy(new_table + i, old_table + i, sizeof (struct bucket));
   }

   // Update map.
   map->capacity = new_capacity;

}




   /**
      Construct a new mapping of key-value pairs.
         szKey : amount of memory needed to specify keys.
         szVal : amount of memory needed to specify values.
         hash : custom function used to hash keys. If none is
            specified, a default will be provided. The hash function
            should produce the same value on two items which are
            equivalent according to your comparison function.
         freeKeys : custom function used to free keys.
         freeVals : custom function used to free vals.
         cmpKeys : custom function used to compare keys. Should
            return 0 if two keys are equal.
         cmpVals : custom function used to compare vals. Should
            return 0 if two vals are equal.
   **/
Map *Map_Make (int init_capacity,
               int szKey,
               int szVal,
               unsigned int (*hash)(void *),
               void (*freeKeys)(void *),
               int (*cmpKeys)(void *, void *),
               void (*freeVals)(void *), 
               int (*cmpVals)(void *, void *))
{
   struct map *map = malloc(sizeof (struct map));
   map->capacity = init_capacity;
   map->szkey = szKey;
   map->szval = szVal;
   map->hash = hash;
   map->keyfree = freeKeys;
   map->valfree = freeVals;
   map->valcmp = cmpVals;
   map->keycmp = cmpKeys;
   struct bucket *table = calloc(sizeof (struct bucket), init_capacity);
   return map;
}

   /**
      Tear down the given Map. It will free each key-value pair
      that it has stored.
         map : map to free.
    **/
void Map_Free (Map *map)
{
   int i;
   for (i=0 ; i < map->capacity; i++) {
      struct bucket buck = map->table[i];
      List_Free(buck.keys);
      List_Free(buck.vals);
   }
   free(map->table);
   free(map);
}

   /**
      Return the number of key-value pairs in the map.
         map : map to check.
    **/
int Map_Size (Map *map){
   return map->items;
}

   /**
      Check whether the given key is in the map.
         map : map to check.
         key : key you're searching for.
    **/
int Map_Contains (Map *map,
                  void *key)
{
   int hash_index = hash(map, key) % map->capacity;
   struct bucket buck = map->table[hash_index];
   return List_Contains(buck.keys, key);
}

   /**
      Get the value associated with the specified key.
         map : map to check.
         key : key to check for.
    **/
void *Map_Get (Map *map,
                  void *key)
{

   // Find appropriate bucket.
   int hash_index = hash(map, key) % map->capacity;
   if (!bucket_exists(map, hash_index))
      return NULL;

   // Check list of keys in the bucket.
   struct bucket buck = map->table[hash_index];
   int key_index = List_IndexOf(buck.keys, key);
   if (key_index == -1) return NULL;
   return List_Get(buck.vals, key_index);

}

   /**
      Put the given key-value pair into the map.
         map : map to put the key-value pair into.
         key : the address of the value under the map.
         val : the image of the key under the map.
    **/
void Map_Put (Map *map, void *key, void *val)
{

   // Check if it's time to resize.
   if (should_resize)
      rebuild(map);

   // Find bucket where (key, val) should go.
   int hash_index = hash(map, key) & map->capacity;
   if (!bucket_exists(map, hash_index))
      init_bucket(map, hash_index);
   struct bucket *buck = map->table + hash_index;  

   // Check if that key is already here.
   int key_index = List_IndexOf(buck->keys, key);
   
   // Append (key, val) if it does not exist.
   if (key_index == -1) {
      List_Append(buck->keys, key);
      List_Append(buck->vals, val);   
   }

   // Overwrite (key, val) if it already exists.
   else {
      List_Insert(buck->keys, key, key_index);
      List_Insert(buck->vals, val, key_index);   
   }

   // Update map.
   map->items += 1;

}

   /**
      Delete the pair associated with the gievn key.
      Returns non-negative value if something was deleted.
         map : map the key-value pair is in.
         key : the key indexing the pair to be deleted.
   **/
int Map_Del (Map *map,
             void *key)
{

   // Find bucket where (key, val) should go.
   int hash_index = hash(map, key) & map->capacity;
   if (!bucket_exists(map, hash_index))
      return 0;
   struct bucket *buck = map->table + hash_index;  

   // Check if key is in this bucket.
   int in_map = List_Contains(buck->keys, key);
   if (in_map) map->items -= 1;
   return in_map;

}











