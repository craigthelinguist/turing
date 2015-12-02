

#ifndef MAP_H
   #include "map.h"
#endif

#ifndef STR_H
   #include "str.h"
#endif

#define BLANK 0

typedef enum action { LEFT, RIGHT, PRINT } Action;

typedef struct program Program;
typedef struct clause Clause;

int SizeOf_Program ();
int SizeOf_Clause ();
void Program_Free (Program *pr);
void Clause_Free (Clause *cl);

   /**
      Each file describes one program.
    **/
struct program {
   Map *states; // Str -> Array of Clauses
   Str *name;
   Str *init_state;
   int num_inputs;
};

struct clause {
   char input;
   Action action;
   Str *end_state;
};

