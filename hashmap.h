
   /**
      A generic hashmap with copy-by-value semantics.
    **/
typedef struct hashmap HashMap;

   /**
      Construct a new HashMap, which associates keys to values.
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
HashMap *HashMapMake (int szKey,
                      int szVal,
                      int hash(void *),
                      void freeKeys(void *),
                      void freeVals(void *),
                      int cmpKeys(void *, void *),
                      int cmpVals(void *, void *));

   /**
      Tear down the given HashMap. It will free each key-value pair
      that it has stored.
         map : 
    **/
void HashMapDel (HashMap *map);

   /**
      Return the number of key-value pairs in the map.
         map : map to check.
    **/
int HashMapSize (HashMap *map);

   /**
      Check whether the given key is in the map.
         map : map to check.
         key : key you're searching for.
    **/
int HashMapContains (HashMap *map,
                     void *key);

   /**
      Get the value associated with the specified key.
         map : map to check.
         key : key to check for.
    **/
void *HashMapGet (HashMap *map,
                  void *key);

   /**
      Put the given key-value pair into the map.
         map : map to put the key-value pair into.
         key : the address of the value under the map.
         val : the image of the key under the map.
    **/
void *HashMapPut (HashMap *map, void *key, void *val);
