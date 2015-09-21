


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

int ListContains (List *list, void *value);
void *ListGet (List *list, void *value);
void ListPrepend (List *list, void *value);
List *MakeList (int elemSize, void (*FreeFunc)(void *), int (*Compare)(void *, void *));
void DelList (List *list);


