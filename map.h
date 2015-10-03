
   /**
      A generic Map with copy-by-value semantics.
    **/
typedef struct map Map;

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
               int (*cmpVals)(void *, void *));

   /**
      Tear down the given Map. It will free each key-value pair
      that it has stored.
         map : map to free.
    **/
void Map_Free (Map *map);

   /**
      Return the number of key-value pairs in the map.
         map : map to check.
    **/
int Map_Size (Map *map);

   /**
      Check whether the given key is in the map.
         map : map to check.
         key : key you're searching for.
    **/
int Map_Contains (Map *map,
                  void *key);

   /**
      Get the value associated with the specified key.
         map : map to check.
         key : key to check for.
    **/
void *Map_Get (Map *map,
                  void *key);

   /**
      Put the given key-value pair into the map.
         map : map to put the key-value pair into.
         key : the address of the value under the map.
         val : the image of the key under the map.
    **/
void Map_Put (Map *map, void *key, void *val);

   /**
      Delete the pair associated with the gievn key.
      Returns non-negative value if something was deleted.
         map : map the key-value pair is in.
         key : the key indexing the pair to be deleted.
   **/
int Map_Del (Map *map,
             void *key);













