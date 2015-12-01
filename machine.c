

#include "machine.h"
#include <stdlib.h>

#define TAPE_SIZE 100


/** This is the internal representation of a machine.
    It's basically a linked list. **/
struct Tape *MakeTape (void);
void DelTape (struct Tape *);

struct Tape {
   char cells[TAPE_SIZE];
   struct Tape *prev;
   struct Tape *next;
};

/** This is the actual implementation of a machine. **/
struct machine {
   int head;
   int state;
   struct Tape *leftmost;
   struct Tape *current;
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



