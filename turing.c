
#include <stdlib.h>

#include "list.h"
#include "map.h"
#include "program.h"
#include "str.h"

#define TAPE_SIZE 100

// Function declarations.
// ============================================================

// For the machine.
struct Tape *MakeTape (void);
void DelTape (struct Tape *);
struct machine *MakeMachine (void);
void DelMachine (struct machine *);

// For running the machine.


struct clause *Prog_ResolveClause (struct machine *m,
                                   Str *state,
                                   struct clause **clauses);
void Prog_Step (struct machine *m, Str *state);
int Prog_Halted (struct machine *m, Str *state);



// Struct definitions.
// ============================================================

struct Tape {
   char cells[TAPE_SIZE];
   struct Tape *prev;
   struct Tape *next;
};

struct machine {
   int head;
   int state;
   struct Tape *leftmost;
   struct Tape *current;
   struct program *prog;
};



// Memory allocation/freeing.
// ============================================================

struct machine *
MakeMachine (void)
{
   struct machine *m = malloc(sizeof (struct machine));
   m->head = (int) (TAPE_SIZE/2); // Put in middle of tape.
   m->leftmost = MakeTape();
   m->current = m->leftmost;
   return m;
}

void
DelMachine (struct machine *m)
{
  
   // First free all the tapes in this machine.
   struct Tape *tape = m->leftmost;
   while (tape != NULL) {
      struct Tape *next = tape->next;
      DelTape(tape);
      tape = next;
   }

   // Then free the machine.
   m->leftmost = NULL;
   m->current = NULL;
   free(m);


}

struct Tape *
MakeTape () {
   struct Tape *tape = malloc(sizeof (struct Tape));
   tape->prev = NULL;
   tape->next = NULL;
   return tape;
}

void
DelTape (struct Tape *tape) {
   tape->prev = NULL;
   tape->next = NULL;
   free(tape);
}



// Machine instructions.
// ============================================================

void
Write (struct machine *m, char c) {
   int head = m->head;
   (m->current->cells)[head] = c;
}

int
Read (struct machine *m, char c) {
   int head = m->head;
   return (m->current->cells)[head] == c;
}

void
MvRight (struct machine *m) {
   int head = (m->head + 1) % TAPE_SIZE;
   if (head == 0) {
      struct Tape *next_tape = MakeTape();
      m->current->next = next_tape;
      m->current = next_tape;
   }
}

void
MvLeft (struct machine *m) {
   int head = (m->head - 1) % TAPE_SIZE;
   if (head == -1) {
      head = TAPE_SIZE - 1; // Wrap around to end.
      struct Tape *prev_tape = MakeTape();
      m->current->prev = prev_tape;
      m->current = prev_tape;
   }
}



// Mapping program to machine instructions.
// ============================================================

void
Prog_Step (struct machine *m, Str *state)
{

   // Check if you are in the halting state.
   if (Prog_Halted(m, state)) return;
   
   // Look up clauses for current state.
   struct clause **clauses = Map_Get (m->prog->states, state);

   // If no such state defined, halt.
   if (clauses == NULL) {
      state = NULL;
      return;
   }
   
   // Clause resolution.
   struct clause *cl = Prog_ResolveClause(m, state, clauses);

   // If could not resolve clause, halt.
   if (cl == NULL) {
      state = NULL;
      return;
   }

   // Perform clause.
   switch (cl->action) {
   
      case LEFT:
         MvLeft(m);
         break;
      case RIGHT:
         MvRight(m);
         break;
      case PRINT:
         // should print shit here
         break;

   }

   // Transition.
   state = cl->end_state;

}

struct clause *
Prog_ResolveClause (struct machine *m, Str *state, struct clause **clauses)
{
   struct clause *cl = NULL;
   int i;
   for (i=0; clauses[i] != NULL; i++) {
      if (Read(m, clauses[i]->input)) {
         cl = clauses[i];
         break;      
      }
   }
   return cl;
}

int
Prog_Halted (struct machine *m, Str *state)
{
   return state == NULL;
}

int main (int argc, char **argv) {
    return 1;
}

















