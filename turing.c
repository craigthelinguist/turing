
#include <stdlib.h>
#include <ncurses.h>

#include "list.h"
#include "hashmap.h"
#include "word.h"

#define TAPE_SIZE 100

typedef enum instr { HALT = -3, LEFT, RIGHT } Instruction;


// Function declarations.
// ============================================================

struct Tape *MakeTape (void);
void DelTape (struct Tape *);

struct Machine *MakeMachine (void);
void DelMachine (struct Machine *);

// Struct definitions.
// ============================================================

struct Tape {
   char cells[TAPE_SIZE];
   struct Tape *prev;
   struct Tape *next;
};

struct Machine {
   int head;
   int state;
   HashMap *trans_lookup;
   HashMap *input_lookup;
   HashMap *output_lookup;
   struct Tape *leftmost;
   struct Tape *current;
};



// Memory allocation/freeing.
// ============================================================

struct Machine *
MakeMachine (void)
{
   struct Machine *m = malloc(sizeof (struct Machine));
   m->head = (int) (TAPE_SIZE/2); // Put in middle of tape.
   m->leftmost = MakeTape();
   m->current = m->leftmost;
   return m;
}

void
DelMachine (struct Machine *m)
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
Write (struct Machine *m, char c) {
   int head = m->head;
   (m->current->cells)[head] = c;
}

int
Read (struct Machine *m, char c) {
   int head = m->head;
   return (m->current->cells)[head] == c;
}

void
MvRight (struct Machine *m) {
   int head = (m->head + 1) % TAPE_SIZE;
   if (head == 0) {
      struct Tape *next_tape = MakeTape();
      m->current->next = next_tape;
      m->current = next_tape;
   }
}

void
MvLeft (struct Machine *m) {
   int head = (m->head - 1) % TAPE_SIZE;
   if (head == -1) {
      head = TAPE_SIZE - 1; // Wrap around to end.
      struct Tape *prev_tape = MakeTape();
      m->current->prev = prev_tape;
      m->current = prev_tape;
   }
}

void
Step (struct Machine *m) {
   if (m->state == HALT) return;
   void *ptr = HashMapGet (m->output_lookup, &(m->state));
   if (ptr == NULL) abort(); // No such state exists.
   int instr = *((int *)ptr);   
   switch (instr) {
   
      case LEFT:
         MvLeft(m);
         break;

      case RIGHT:
         MvRight(m);
         break;

      default:
         // should actually print char here         
         abort();

   }
}

int main (int argc, char *argv[]){}

















