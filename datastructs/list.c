
// Header files.
// ======================================================================


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "list.h"



// Definitions.
// ======================================================================

typedef int (*CmpFunc)(void *, void*);
typedef void (*FreeFunc)(void *);

struct list {
   int len;
   int capacity;
   int szitem;
   void *arr;
   CmpFunc cmp;
   FreeFunc free_item;
};

static inline int item_eq (struct list *list, void *item1, void *item2);
static inline int should_resize (struct list *list);
static void resize (struct list *list);



// Internal functions.
// ============================================================

static inline int item_eq (struct list *list, void *item1, void *item2)
{
   if (!list->cmp)
      return !memcmp(item1, item2, list->szitem);
   else
      return !(*(list->cmp))(item1, item2);
}

static inline void item_free (struct list *list, void *item)
{
   if (list->free_item)
      (*(list->free_item))(item);
}

static inline int should_resize (struct list *list)
{
   return list->capacity == list->len;
}

static void resize (struct list *list)
{

   // Allocate memory for new array. Use char * so that pointer
   // arithmetic moves by one byte each time.
   int new_capacity = list->capacity * 2;
   char *new_arr = calloc(list->szitem, new_capacity);
   char *old_arr = list->arr;
   int szitem = list->szitem;
   
   // Copy everything across to the new table.
   int i;
   for (i=0; i < list->len; i++) {
      int offset = i*szitem;
      memcpy(new_arr + offset, old_arr + offset, szitem);
   }
   
   // Free old table, update the struct.
   free(old_arr);
   list->capacity = new_capacity;
   list->arr = (void *)new_arr;
   
}

static inline void *offset (struct list *list, int index)
{
   int offset = list->szitem * index;
   char *arr = list->arr;
   return arr + offset;
}

static inline void out_of_bounds ()
{
   fprintf(stderr, "Trying to access list at illegal index.");
   abort();
}



// Public functions.
// ============================================================

List * List_Make (int initial_capacity,
                 int elem_size,
                 CmpFunc cmp,
                 FreeFunc free)
{
   struct list *list = malloc(sizeof (struct list));
   list->arr = calloc(elem_size, initial_capacity);
   list->capacity = initial_capacity;
   list->szitem = elem_size;
   list->cmp = cmp;
   list->len = 0;
   list->free_item = free;
   return list;
}

void List_Free (List *list)
{

   // Go through list freeing everything.
   int i;
   for (i=0; i < list->len; i++) {
      void *item = offset(list, i);
      item_free(list, item);
      memset(item, 0, list->szitem);
   }

   free(list->arr);
   list->arr = NULL;
}

int List_Size (List *list)
{
   return list->len;
}

void List_Append (List *list,
                 void *item)
{

   // Some preliminary checks.
   if (item == NULL)
      return;
   if (should_resize(list))
      resize(list);
   
   // Smash it on the end.
   void *last_slot = offset(list, list->len);
   memcpy(last_slot, item, list->szitem);
   list->len++;
   
}

void List_Insert (List *list,
                  void *item,
                  int index)
{

   // Error checking.
   if (item == NULL)
      return;
   if (index > list->len || index < 0)
      out_of_bounds();

   // Inserting at end is appending.
   if (index == list->len) {
      List_Append(list, item);
      return;
   }

   // Overwrite thing.
   void *slot = offset(list, index);
   memcpy(slot, item, list->szitem);
}

int List_IndexOf (List *list,
                  void *item)
{
   
   // Some preliminary checks.
   if (item == NULL)
      return -1;
      
   // Check each element in array.
   int i;
   for (i=0; i < list->len; i++) {
      void *list_item = offset(list, i);
      if (item_eq(list, list_item, item)) return i;
   }
   
   // If you're down here, item not in the List.
   return -1;
   
}

int List_Contains (List *list,
                   void *item)
{
   int idx = List_IndexOf(list, item);
   return idx >= 0 && idx < list->len;
}

void *List_Get (List *list,
                int index)
{

   // Preliminary checks.
   if (index < 0)
      out_of_bounds();
   if (index >= list->len)
      return NULL;
   if (list == NULL)
      abort();
      
   // Get item, copy it into a new thing, return it.
   void *item = offset(list, index);
   void *to_return = malloc(list->szitem);
   memcpy(to_return, item, list->szitem);
   return to_return;

}

int List_Del (List *list,
               int index)
{
   
   // Preliminary checks.
   if (index < 0)
      out_of_bounds();
   if (index >= list->len)
      return 0;
   
   // Get offset, free stuff at that position.
   void *curr = offset(list, index);
   item_free(list, offset);
   
   // Move everything down.
   int i;
   for (i=index+1; i < list->len; i++) {
      void *prev = curr;
      curr = offset(list, i);
      memcpy(prev, curr, list->szitem);
   }

   // Zero out stuff at last position.
   curr = offset(list, i);
   memset(curr, 0, list->szitem);
   
   // Update list.
   list->len--;
   return 1;
   
}

int List_Remove (List *list,
                 void *item)
{
   int index = List_IndexOf(list, item);
   return List_Del(list, index);
}
         
