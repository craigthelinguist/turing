
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

struct Pair {
   void *key;
   void *val;
};

struct HashMap {
   int capacity;
   int numElems; // number of things in the map.
   int szKey;
   int szVal;
   HashFunc hash;
   FreeFunc keyFree;
   FreeFunc valFree;
   CmpFunc keyCmp;
   CmpFunc valCmp;
   List **table; // array of list ptrs.
};



// Internal function declarations.
// ======================================================================

unsigned int _hash (struct HashMap *map, void *key);
List *_list (struct HashMap *map);
int _should_resize (struct HashMap *map);
void _rebuild (struct HashMap *map);




// Internal functions.
// ======================================================================

   /**
      Make a list for storing values in.
         map : map the list will be apart of.
    **/
List *_list (struct HashMap *map)
{
   return ListMake(sizeof(struct Pair), NULL, NULL);
}


   /**
      Whether the hash-table needs to be resized. Result is non-zero if it
      needs to resize, or zero otherwise.
         map : map to check.
    **/
int _should_resize (struct HashMap *map)
{
   float density = map->numElems / map->capacity;
   return density >= 0.75;
}

   /**
      Apply the hash function of the map to the specified item.
      Return not guaranteed to be within the bounds of the table.
         key : pointer to thing serving as a key.
    **/
unsigned int _hash (struct HashMap *map, void *key)
{
   if (map->hash != NULL)
      return (*map->hash)(key);
   return 0;
}

   /**
      Create a new hashtable for the given map and copy all the values
      over from the old hashtable. After this function, map's table will
      refer to the new updated table.
         map : map whose table shall be rebuilt.
         new_capacity : how big the new table will be.
    **/
void _rebuild (struct HashMap *map)
{

   // Allocate a new array with twice the space.
   // We use calloc to zero out memory, since the first byte of each bucket
   // is going to signify whether that bucket is occupied.
   int new_capacity = map->capacity * 2;
   List **new_table = calloc(new_capacity, sizeof(List *));

   // Copy values across from old array.
   int i;
   for (i=0 ; i < map->capacity; i++) {

      // Check if there is anything at this bucket in the table.
      List *list = (List *)(map->table[i]);
      if (list == NULL) continue;

      // Copy list ptr across to new table.
      void *key;
      ListHead(key, list);
      unsigned int index = _hash(map, key) % new_capacity;
      memcpy(new_table[index], key, sizeof(List *));
   }

   // Free old array, update hash map.
   free(map->table);
   map->table = new_table;
   map->capacity = new_capacity;

}



// Publicly visible functions.
// ======================================================================

struct HashMap *
HashMapMake (int szKey, int szVal, HashFunc hashFunc,
             FreeFunc freeKeys, FreeFunc freeVals,
             CmpFunc cmpKeys, CmpFunc cmpVals)
{
   struct HashMap *map = malloc(sizeof (struct HashMap));
   map->capacity = INIT_CAPACITY;
   map->numElems = 0;
   map->szKey = szKey;
   map->szVal = szVal;
   map->hash = hashFunc;
   map->keyFree = freeKeys;
   map->valFree = freeVals;
   map->keyCmp = cmpKeys;
   map->valCmp = cmpVals;
   map->table = calloc(map->capacity, sizeof (List *));
   return map;
}



   /**
      Tear down the given HashMap. It will free each key-value pair
      that it has stored.
         map : map to free.
    **/
void HashMapFree (HashMap *map)
{
   int i;
   for (i=0; i < map->capacity; i++) {
      List *list = (List *)(map->table[i]);
      if (list == NULL) continue;
      ListFree(list);
   }
   free(map);
}

   /**
      Return the number of key-value pairs in the map.
         map : map to check.
    **/
int HashMapSize (HashMap *map) {
   return map->numElems;
}

   /**
      Check whether the given key is in the map.
         map : map to check.
         key : key you're searching for.
    **/
int HashMapContains (HashMap *map, void *key) {
   return HashMapGet(map, key) != NULL;
}

   /**
      Get the value associated with the specified key.
         map : map to check.
         key : key to check for.
    **/
void *HashMapGet (HashMap *map, void *key) {
   
   // Hash key and get appropriate list.
   unsigned int index = _hash(map, key) % map->capacity;
   List *list = (List *)(map->table[index]);
   if (list == NULL) return NULL;

   // Check the list for the key. The list of pairs has its comparator
   // something which checks for keys, and it will return the pair as a struct.
   void *ptr;
   ListGet (ptr, list, key);
   if (ptr == NULL) return NULL;
   struct Pair *pair = (struct Pair *)ptr;
   return pair->val;

}

   /**
      Put the given key-value pair into the map.
         map : map to put the key-value pair into.
         key : the address of the value under the map.
         val : the image of the key under the map.
    **/
void HashMapPut (HashMap *map, void *key, void *val) {
   
   unsigned int index = _hash(map, key) % map->capacity;
   List *list = (List *)map->table[index];

   // Calloc a struct which we will use to check if our element is in this
   // bucket. Use calloc to zero out padding bytes so that memcmp will work.
   struct Pair *pair;
   memset(pair, 0, sizeof(pair));
   pair->key = key;
   pair->val = val;

   // Need to make a list.   
   if (list == NULL) {
      list = _list(map);
      ListPrepend(list, pair); // List makes a copy of pair.
      return;
   }

   // Remove key-val pair if it already exists. Insert new one.
   ListDel(list, pair);
   ListPrepend(list, pair);

}












