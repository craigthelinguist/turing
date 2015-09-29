
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "hashmap.h"


const int INIT_CAPACITY = 10;

struct HashMap {
   int capacity;
   int size; // current size of buckets.
   int szKey;
   int szVal;
   int szBucket;
   void (*hash)(void *);
   void (*keyFree)(void *);
   void (*valFree)(void *);
   int (*keyCmp)(void *, void *);
   int (*valCmp)(void *, void *);
   void **buckets;
};

int _hash (void *key);
int _shouldresize (struct HashMap *map);
int _szBucket (struct HashMap *map);
void _rebuild (struct HashMap *map, int newCapacity);


// Publicly visible functions.
// ======================================================================



// Internal functions.
// ======================================================================

   /**
      Whether the hash-table needs to be resized. Result is non-zero if it
      needs to resize, or zero otherwise.
         map : map to check.
    **/
int _shouldresize (struct HashMap *map)
{
   float density = map->size / map->capacity;
   return density >= 0.75;
}

   /**
      Default hash.
         key : address of item to hash.
    **/
int _hash (void *key)
{
   return 0;
}

int _szBucket (struct HashMap *map)
{
   return 1 + sizeof(List *) + map->szKey;
}

void _insert (void **buckets, void *key, void *val)
{
   int hash = _hash(key);
   // to do
}

void _rebuild (struct HashMap *map, int new_capacity)
{

   /**
    This is what a bucket is:
    =========================================
    | SIGNAL | POINTER TO | POINTER TO LIST |
    |  BYTE  |     KEY    |     VALUES      |
    =========================================
    **/
   int szBucket = _szBucket(map);

   // Allocate a new array with twice the space.
   // We use calloc to zero out memory, since the first byte of each bucket
   // is going to signify whether that bucket is occupied.
   void **new_table = calloc(new_capacity, szBucket);
  
   // Copy values across from old array.
   int i;
   for (i=0 ; i < map->capacity; i++){
   
      // Figure out offset. Check signal byte.
      // If signal byte is zero, nothing in this bucket to copy.
      int offset = szBucket * i;
      int signalByte = *((int *)map->buckets[offset]);
      if (!signalByte) continue; // No entry in this bucket.
      
      // Otherwise extract key and list * from the bucket.
      void *key = map->buckets[offset+1];
      List **list = map->buckets[offset+1+map->szKey];
      
      // Compute hash for new table.
      // Copy across the signal byte, key, and list ptr.
      int index = _hash(key);
      *((int *)new_table[index]) = 1;
      memcpy(new_table + index + 1, key, map->szKey);
      memcpy(new_table + index + 1 + map->szKey, list, sizeof(List *));
      
   }
   
   // Free old array, update hash map.
   free(map->buckets);
   map->buckets = new_table;
   map->capacity = new_capacity;
   
}











