
typedef struct list List;

   /**
      Construct a new list. Returns a pointer to the list.
      It must be freed using ListFree.
         initial_capacity : starting size of the list.
         elem_size : size of the elements (in bytes) that will
            be in the list.
         cmp : a function which tells you how to compare two
            elements in the list. This should return '0' if
            two elements are identical.
         free : a function telling you how to free the items
            in the list. Will be called when you use List_Free. **/ 
List * List_Make (int initial_capacity,
                 int elem_size,
                 int (*cmp)(void *, void*),
                 void (*free)(void *));

   /**
      Free the given list.
         list : list to free. **/
void List_Free (List *list);
     
   /**
      Get the size of the list.
         list : list to check.
    **/
int List_Size (List *list);
                    
   /**
      Append an item to the given list. If there is no size in
      the list, it will be automatically resized.
         list : list to append to.
         item : item to append onto the list. **/
void List_Append (List *list,
                 void *item);

   /**
      Insert an item into a list at a specified position. The
      thing at that position will be overwritten.
         list : list to insert into.
         item : item to be inserted.
         index : index where item should be inserted. **/
void List_Insert (List *list,
                  void *item,
                  int index);

   /**
      Check if the item is in the list and return the first index
      where it is located. Your input is checked against each
      item in the list using the comparator function you passed
      in when constructing the list; otherwise it uses memcmp.
         list : list to check through.
         item : item you're searching for. **/
int List_IndexOf (List *list,
                  void *item);

   /**
      Check if the item is in the list. Returns a non-zero value
      if it is. Your input is checked against each item in the
      list using the comparator function you passed in when
      constructing the list; otherwise it uses memcmp.
         list : list to check.
         item : item you're searching for. **/
int List_Contains (List *list,
                   void *item);

   /**
      Get the item in the list at the specified index. The item
      is copied into a new portion of memory and given back to you,
      so it is your responsibility to free it. Returns a NULL
      pointer if there is no item at that index. Aborts if the
      index is out of bounds.
         list : list to check.
         index : location to get the item from. **/
void *List_Get (List *list,
                int index);
                
   /**
      Delete the item at the specified index. Does nothing if
      there is no item at that index. Aborts if the index is out
      of bounds. Returns non-zero value if something was deleted.
         list : list to delete form.
         index : index to delete. **/
int List_Del (List *list,
               int index);

   /**
      Find and remove the item from the list, if it exists. If
      something was deleted the function returns non-zero value.
      Your input is checked against each item in the list using
      the comparator function you passed in when constructing the
      list; otherwise it uses memcmp.
         list : list to check.
         item : item you're searching for. */
int List_Remove (List *list,
                 void *item);
   
