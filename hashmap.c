

#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 100

// Struct declarations.
// ============================================================

struct Pair {
   char *key;
   int value;
};

struct ListNode {
   struct Pair pair;
   struct ListNode *next;
};

struct List {
   struct ListNode *head;
   int size;
};

// Functions declarations.
// ============================================================

// For string comparison.
int StrEquals (char *s1, char *s2);

// For list.
int ListContains (struct List *list, char *key);
struct Pair ListGet (struct List *list, char *key);
int ListAdd (struct List *list, struct Pair pair);
struct ListNode *MakeNode (struct Pair pair);
void DelNode (struct ListNode *node);
struct List *MakeList ();
void DelList (struct List *list);

int
StrEquals (char *s1, char *s2)
{
   return !strcmp(s1, s2);
}

int
ListContains (struct List *list, char *key)
{
   struct ListNode *node = list->head;
   while (node != NULL) {
      struct Pair pair = node->pair;
      if (StrEquals(pair.key, key)) return 1;
      node = node->next;
   }
   return 0;
}

struct Pair
ListGet (struct List *list, char *key)
{
   struct ListNode *node = list->head;
   while (node != NULL) {
      struct Pair pair = node->pair;
      if (StrEquals(pair.key, key)) return pair;
      node = node->next;
   }
}

int
ListAdd (struct List *list, struct Pair pair) {
   return 0;
}

struct ListNode *
MakeNode (struct Pair pair)
{
   struct ListNode *node = malloc(sizeof (struct ListNode));
   node->next = NULL;
   node->pair = pair;
   return node;
}

void
DelNode (struct ListNode *node)
{
   node->next = NULL;
   free(node);
}

struct List *
MakeList ()
{
   struct List *list = malloc(sizeof (struct List));
   list->head = NULL;
   list->size = 0;
   return list;
}

void
DelList (struct List *list)
{
   struct ListNode *node = list->head;
   while (node != NULL) {
      struct ListNode *next = node->next;
      DelNode(node);
      node = next;
   }
   list->head = NULL;
   free(list);
}

