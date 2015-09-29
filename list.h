
   /**
      Generic list. Has copy-by-value semantics.
   **/
typedef struct List List;

   /**
      Check whether the list contains the specified value.
      Equality is checked based on the custom comparator function you gave to the list.
         list : list to check.
         value : pointer to value to be checked.
    **/
int ListContains (List *list,
                  void *value);

   /**
      Get the item associated with the specified value.
      Equality is checked based on the custom comparator function you gave to the list.
         ptr : location to some memory. The return value will be copied into here.
         list : list to check.
         value : pointer to value to be checked.
    **/
void ListGet (void *ptr,
              List *list,
              void *value);

   /**
      Prepend the item onto the list. The value at the given pointer is copied to a new
      location in memory accessible only to the list, so the list has "copy-by-value"
      semantics.
         list : list to check.
         value : item to prepend.
    **/
void ListPrepend (List *list,
                  void *value);

   /**
      Allocate memory for a new List of values and construct it.
         elemSize : amount of memory each item in the List takes.
         FreeFunc : a function that can be applied to items in the List in
            order to free them. Specifies how memory is to be freed when
            the List destroys itself.
         Compare : a function which can be applied to two items in the List.
            The function should return 0 if two items are equal.
    **/
List *ListMake (int elemSize,
                void (*FreeFunc)(void *),
                int (*Compare)(void *, void *));
                
   /**
      Tear down the list and free its memory. Each value in the list will
      be freed also using the custom FreeFUnc that you specified when the list
      was created.
         list : List to delete.
    **/
void ListDel (List *list);

   /**
      Return the number of elements in the list.
         list : list to check.
    **/
int ListSize (List *list);


