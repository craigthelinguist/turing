
typedef struct List List;

int ListContains (List *list, void *value);
void *ListGet (List *list, void *value);
void ListPrepend (List *list, void *value);
List *ListMake (int elemSize, void (*FreeFunc)(void *), int (*Compare)(void *, void *));
void ListDel (List *list);
int ListSize (List *list);


