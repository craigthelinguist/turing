

#include <stdlib.h>
#include <string.h>

#include "list.h"

// Struct declarations.
// ============================================================
struct ListNode *MakeNode (void *value);

// Functions declarations. ============================================================

int
Comparison (struct List *list, void *ptr1, void *ptr2)
{
   if (list->Compare == NULL)
      return memcmp(ptr1, ptr2, list->elemSize);   
   else
      return list->Compare(ptr1, ptr2);   
}

struct List *
MakeList (int elemSize, void (*FreeFunc)(void *), int (*Compare)(void *, void *))
{
   List *list = malloc(sizeof (struct List));
   list->elemSize = elemSize;
   list->FreeFunc = FreeFunc;
   list->Compare = Compare;
   list->length = 0;
   list->head = NULL;
}

void
DelList (struct List *list)
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

void *
ListGet (struct List *list, void *value)
{
   struct ListNode *node = list->head;
   while (node != NULL) {
      if (!Comparison(list, node->dataPtr, value))
         return node->dataPtr;
      else
         node = node->next;
   }
   return NULL;
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

