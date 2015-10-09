
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "map.h"

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

// Block of all zeroes.
// Used to see if a bucket has been initialised at an index.
static const char emptyblock[sizeof (struct bucket)];


// Internal function declarations.
// ======================================================================

static unsigned int djb2 (struct map *map, void *key);
static unsigned int hash (struct map *map, void *key);
static int should_resize (struct map *map);
static void rebuild (struct map *map);
static inline void init_bucket (struct map *map, unsigned int index);
static inline struct bucket free_bucket (struct bucket bucket);
static inline int bucket_exists (struct map *map, unsigned int index);


// Internal functions.
// ======================================================================

static unsigned int djb2 (struct map *map, void *key)
{
   unsigned int hash = 5381;
   char *ptr = (char *)key;
   int i;
   
   for (i=0; i < map->szkey; i++) {
      char c = *(ptr + i);
      hash = ((hash << 5) + hash) + c;
   }
   
   return hash;
}

   /**
      Run an item through the hash function of a map. This is NOT guaranteed
      to return a valid index to the map's table (you should mod the length of
      the table), but it will return a non-negative value.
         map : map you're performing the hash for.
         key : thing to be hashed.
    **/
static unsigned int hash (struct map *map, void *key)
{
   unsigned int r;
   if (map->hash) return map->hash(key);
   else return djb2(map, key);
}

   /**
      Whether the table should be rebuilt. This is based on the ratio of
      items in the table to the capacity of the table.
         map : map to check.
    **/
static int should_resize (struct map *map)
{
   float density = (float)map->items / (float)map->capacity;
   return density > 0.75;
}

   /**
      Initialise a new bucket in the map's table at the given index. This
      involves allocating memory for the two lists that the bucket refers to.
         map : map to initialise bucket for.
         index : index where the bucket should be created.
    **/
static inline void init_bucket (struct map *map, unsigned int index)
{
   map->table[index].keys = List_Make(10, map->szkey, map->keycmp, map->keyfree);
   map->table[index].vals = List_Make(10, map->szval, map->valcmp, map->valfree);
}

   /**
      Free memory referred to by a given bucket.
         bucket : bucket whose members should be freed.
    **/
static inline struct bucket free_bucket (struct bucket bucket)
{
   List_Free(bucket.keys);
   List_Free(bucket.vals);
}

   /**
      Check if a bucket at a specific index has been initialised.
         map : map to check.
         index : index of bucket to check.
    **/
static inline int bucket_exists (struct map *map, unsigned int index)
{
   return memcmp(emptyblock, map->table + index, sizeof (struct bucket));
}

   /**
      Rebuild the map's table. This extends the capacity of the table and copy
      everything across by rehashing 
    **/
static void rebuild (struct map *map)
{

   // Allocate memory for new table.
   int new_capacity = map->capacity * 2;
   struct bucket *new_table = calloc(sizeof (struct bucket), new_capacity);
   struct bucket *old_table = map->table;

   // Copy all buckets across, rehashing each as you go.
   int i;
   for (i=0; i < map->capacity; i++) {
      if (!bucket_exists(map, i)) continue;
      struct bucket bucket = map->table[i];
      void *key = List_Get(bucket.keys, 0);
      unsigned int k = hash(map, key) % new_capacity;
      memcpy(new_table + k, old_table + i, sizeof (struct bucket));
   }

   // Update map.
   map->table = new_table;
   map->capacity = new_capacity;
   free(old_table);
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
   map->table = calloc(init_capacity, sizeof (struct bucket));
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
      if (!bucket_exists(map, i)) continue;
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
   unsigned int hash_index = hash(map, key) % map->capacity;
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
   unsigned int hash_index = hash(map, key) % map->capacity;
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
   if (should_resize(map))
      rebuild(map);
      
   // Find bucket where (key, val) should go.
   unsigned int hash_index = hash(map, key) % map->capacity;
   if (!bucket_exists(map, hash_index))
      init_bucket(map, hash_index);
   struct bucket *buck = map->table + hash_index;  

   // Check if that key is already here.
   int key_index = List_IndexOf(buck->keys, key);
   
   // Append (key, val) if it does not exist.
   if (key_index == -1) {
      List_Append(buck->keys, key);
      List_Append(buck->vals, val);   
      map->items += 1;
   }

   // Overwrite (key, val) if it already exists.
   else {
      List_Insert(buck->keys, key, key_index);
      List_Insert(buck->vals, val, key_index);   
   }

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
   unsigned int hash_index = hash(map, key) & map->capacity;
   if (!bucket_exists(map, hash_index))
      return 0;
   struct bucket *buck = map->table + hash_index;  

   // Check if key is in this bucket.
   int in_map = List_Contains(buck->keys, key);
   if (in_map) map->items -= 1;
   return in_map;

}











