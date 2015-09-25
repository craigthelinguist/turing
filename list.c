

#include <stdlib.h>
#include <string.h>

// Struct declarations.
// ============================================================

typedef struct ListNode {
   void *dataPtr;
   struct ListNode *next;
} ListNode;

typedef struct List {
   struct ListNode *head;
   int elemSize;
   int length;
   void (*FreeFunc)(void *);
   int (*Compare)(void *, void *);
} List;

// Function declarations.
// ============================================================
struct ListNode *MakeNode (void *value);
#include "list.h"


// Function implementations.
// ============================================================

int
Comparison (struct List *list, void *ptr1, void *ptr2)
{
   if (list->Compare == NULL)
      return memcmp(ptr1, ptr2, list->elemSize);   
   else
      return list->Compare(ptr1, ptr2);   
}

struct List *
ListMake (int elemSize, void (*FreeFunc)(void *), int (*Compare)(void *, void *))
{
   List *list = malloc(sizeof (struct List));
   list->elemSize = elemSize;
   list->FreeFunc = FreeFunc;
   list->Compare = Compare;
   list->length = 0;
   list->head = NULL;
}

int
ListSize (struct List *list)
{
   return list->length;
}

void
ListDel (struct List *list)
{
   struct ListNode *node = list->head;
   void (*FreeFunc)(void *) = list->FreeFunc;
   while (node != NULL) {
      if (FreeFunc != NULL) FreeFunc(node->dataPtr);
      else free(node->dataPtr);
      struct ListNode *next = node->next;
      node->next = NULL;
      free(node);
      node = next;
   }
   free(list);
}

int
ListContains (struct List *list, void *value)
{
   struct ListNode *node = list->head;
   while (node != NULL) {
      if (!Comparison(list, node->dataPtr, value))
         return 1;
      node = node->next;   
   }
   return 0;
}


  /*
    Check through the list for a specified value. An item in the list is
    matched to the value according to your custom comparator function (or
    using memcmp, if your comparator function is NULL).

    You should allocate memory for the first parameter, returnPtr. It is
    your responsibility to free it.

    If no such element exists, returnPtr will be zeroed.   

    returnPtr:
      A pointer to some value. If there is a match in the list then it will
      get copied into here.
    list:
      Pointer to the list you want to look through.
    value:
      Pointer to something to look for. It will be compared with items in the list
      using memcmp, or your custom comparator function
  */
void
ListGet (void *returnPtr, struct List *list, void *value)
{
   struct ListNode *node = list->head;
   while (node != NULL) {
      if (!Comparison(list, node->dataPtr, value)) {
         memcpy(returnPtr, node->dataPtr, list->elemSize);
         return;
      }
      else {
         node = node->next;
      }
   }

   // Zero memory to signify value was not found.
   memset(returnPtr, 0, list->elemSize);
}

void
ListPrepend (struct List *list, void *value)
{

   // Allocate memory for node. Copy value across.
   ListNode *node = malloc(sizeof (struct ListNode));
   node->dataPtr = malloc(list->elemSize);
   memcpy(node->dataPtr, value, list->elemSize);

   // Prepend onto list.
   node->next = list->head;
   list->head = node;
   list->length = list->length + 1;

}

