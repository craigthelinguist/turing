
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

struct HashMap {
   int capacity;
   int numBuckets; // current size of buckets.
   int szKey;
   int szVal;
   int szBucket;
   HashFunc hash;
   FreeFunc keyFree;
   FreeFunc valFree;
   CmpFunc keyCmp;
   CmpFunc valCmp;
   void **buckets;
};



// Internal function declarations.
// ======================================================================

unsigned int _hash (struct HashMap *map, void *key);
List *_list (struct HashMap *map);
int _should_resize (struct HashMap *map);
int _szBucket (struct HashMap *map);
void _rebuild (struct HashMap *map);




// Internal functions.
// ======================================================================

   /**
      Make a chain for storing values in.
         map : map the list will be apart of.
    **/
List *_list (struct HashMap *map)
{
   return ListMake(map->szVal, map->valFree, map->valCmp);
}

   /**
      Whether the hash-table needs to be resized. Result is non-zero if it
      needs to resize, or zero otherwise.
         map : map to check.
    **/
int _should_resize (struct HashMap *map)
{
   float density = map->numBuckets / map->capacity;
   return density >= 0.75;
}

   /**
      Return an index specifying where the key should be hashed to.
      The index will be modulo the bounds of the map's hashtable.
         map : the map that the key is being hashed into.
         key : pointer to thing serving as a key.
    **/
unsigned int _hash (struct HashMap *map, void *key)
{
   if (map->hash != NULL)
      return (*map->hash)(key) % map->capacity;
   return 0;
}

int _szBucket (struct HashMap *map)
{
   /**
    This is what a bucket is:
    =========================================
    | SIGNAL | POINTER TO |       LIST      |
    |  BYTE  |     KEY    |    OF VALUES    |
    =========================================
    **/
   return 1 + sizeof(List *) + map->szKey;
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

   /**
    This is what a bucket is:
    =========================================
    | SIGNAL | POINTER TO |       LIST      |
    |  BYTE  |     KEY    |    OF VALUES    |
    =========================================
    **/
   int szBucket = _szBucket(map);

   // Allocate a new array with twice the space.
   // We use calloc to zero out memory, since the first byte of each bucket
   // is going to signify whether that bucket is occupied.
   int new_capacity = map->capacity * 2;
   void **new_table = calloc(new_capacity, szBucket);
  
   // Copy values across from old array.
   int i;
   for (i=0 ; i < map->capacity; i++){
   
      // Figure out offset. Check signal byte.
      // If signal byte is zero, nothing in this bucket to copy.
      // Cast to char * because char = 1 byte on all platforms.
      int offset = szBucket * i;
      int signalByte = *((char *)map->buckets[offset]);
      if (!signalByte) continue; // No entry in this bucket.
      
      // Otherwise extract key and list * from the bucket.
      void *key = map->buckets[offset+1];
      List *list = map->buckets[offset+1+map->szKey];
      
      // Compute hash for new table.
      // Copy across the signal byte, key, and list ptr.
      int index = _hash(map, key);
      *((int *)new_table[index]) = 1;
      memcpy(new_table + index + 1, key, map->szKey);
      memcpy(new_table + index + 1 + map->szKey, list, sizeof(List *));
      
   }
   
   // Free old array, update hash map.
   free(map->buckets);
   map->buckets = new_table;
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
   map->numBuckets = 0;
   map->szKey = szKey;
   map->szVal = szVal;
   map->hash = hashFunc;
   map->keyFree = freeKeys;
   map->valFree = freeVals;
   map->keyCmp = cmpKeys;
   map->valCmp = cmpVals;
   map->szBucket = 1 + sizeof(List *) + szKey;
   map->buckets = calloc(map->capacity, map->szBucket);
   return map;
}



   /**
      Tear down the given HashMap. It will free each key-value pair
      that it has stored.
         map : map to free.
    **/
void HashMapDel (HashMap *map)
{
   int szBucket = _szBucket(map);
   int i;
   for (i=0; i < map->capacity; i++) {
      int offset = szBucket * i;
      int signalByte = *((char *)map->buckets[offset]);
      if (!signalByte) continue; // No entry in this bucket
      
      // Get the key and list. Free each.
      void *key = map->buckets + 1;
      List *list = (List *)(map->buckets + map->szKey );
      if (!map->keyFree) free(key);
      else (*map->keyFree)(key);
      ListDel(list);
   }
   free(map);
}

   /**
      Return the number of key-value pairs in the map.
         map : map to check.
    **/
int HashMapSize (HashMap *map) {
   return map->numBuckets;
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
   
   // Hash key to get its index.
   unsigned int index = _hash(map, key);
   
   // Check first byte at index to see where key should be.
   char *signal_byte = (char *)map->buckets[index];
   if (!signal_byte) return NULL;
   
   // Get the list, check the first element.
   List *list = (List *)(map->buckets[index + 1 + map->szKey]);
   return ListHead(list);
   
}

   /**
      Put the given key-value pair into the map.
         map : map to put the key-value pair into.
         key : the address of the value under the map.
         val : the image of the key under the map.
    **/
void *HashMapPut (HashMap *map, void *key, void *val) {
   
   // Get index and check signal byte.
   unsigned int index = _hash(map, key);
   char *signal_byte = (char *)map->buckets[index];

   // If signal byte is false, then you need to construct
   // a list for this bucket.
   if (!signal_byte) {
      List *list = ListMake(map->szVal, map->valFree, map->valCmp);
      memcpy(map->buckets[index + 1], key, map->szKey);
      memcpy(map->buckets[index + 1 + map->szKey], list, sizeof(List *));
   }
   
   // hmm... what if two distinct keys hash to the same list?
   // i'm boned - the table just need to hold a signal byte
   // and a list.
   // Maybe can pack signal byte to show if there has been a collision
   // here.
   
}












