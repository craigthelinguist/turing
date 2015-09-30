

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

void
_FreeData (struct List *list, void *dataPtr)
{
   void (*f)(void *) = list->FreeFunc;
   if (list->FreeFunc == NULL) free(dataPtr);
   else (*f)(dataPtr);
}

void
_FreeNode (struct List *list, struct ListNode *ln)
{
   _FreeData(list, ln->dataPtr);
   ln->next = NULL;
   free(ln);
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
ListFree (struct List *list)
{
   struct ListNode *node = list->head;
   void (*FreeFunc)(void *) = list->FreeFunc;
   while (node != NULL) {
      struct ListNode *next = node->next;
      _FreeNode(list, node);
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
         memset(returnPtr, 0, list->elemSize); // To zero out any padding.
         memcpy(returnPtr, node->dataPtr, list->elemSize);
         return;
      }
      else {
         node = node->next;
      }
   }

   // Zero memory to signify value was not found.
   // Issue: Ints can be all zero, so maybe this needs to return
   // a void ** ?
   memset(returnPtr, 0, list->elemSize);
}

void
ListHead (void *returnPtr, struct List *list)
{
   if (list->head == NULL) {
      returnPtr = NULL;
      return;   
   }
   void *ptr = calloc(1, list->elemSize); // calloc to zero out any padding bytes.
   memcpy(ptr, list->head->dataPtr, list->elemSize);
}

int
ListDel (struct List *list, void *value)
{
   // Nothing in list.
   if (list->head == NULL) return 0;

   // It's in the head of the list.
   if (!Comparison(list, value, list->head->dataPtr)) {
      _FreeNode(list, list->head);
      list->head = NULL;
      list->length = list->length - 1;
      return 1;
   }

   // Otherwise traverse the list keeping track of prev and next node.
   struct ListNode *prev = list->head;
   struct ListNode *next = prev->next;
   
   while (next != NULL) {
      
      // Not at this position. Keep moving along.
      if (Comparison(list, value, next->dataPtr)) {
         ListNode *tmp = next;
         next = next->next;
         prev = tmp;
         continue;
      }
   
      // Found it. Free the node, adjust the list, return 1.
      struct ListNode *next_next = next->next;
      _FreeNode(list, next);
      prev->next = next_next;
      list->length = list->length - 1;
      return 1;

   }

   return 0; // Item not in list.
}

void
ListPrepend (struct List *list, void *value)
{

   // Allocate memory for node. Copy value across.
   ListNode *node = malloc(sizeof (struct ListNode));
   node->dataPtr = calloc(1, list->elemSize); // calloc to zero out any padding bytes.
   memcpy(node->dataPtr, value, list->elemSize);

   // Prepend onto list.
   node->next = list->head;
   list->head = node;
   list->length = list->length + 1;

}

