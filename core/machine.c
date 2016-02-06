

#include <stdlib.h>

#include "machine.h"


#define TAPE_SIZE 100

#define CHAR_CODE_BLANK 0
#define CHAR_CODE_1 49

struct tape {
   char *cells;
   struct tape *prev;
   struct tape *next;
};

/** This is the internal representation of a machine.
    It's basically a linked list that grows in size
    when it needs to. **/
struct machine {
   int head;
   int state;
   struct tape *leftmost;
   struct tape *current;
};

struct tape *Tape_Make (void);
void Tape_Del (struct tape *);



// Memory allocation/freeing.
// ============================================================

struct machine *
M_Make (int *inputs, int num_inputs)
{

   // Make the struct representing the machine.
   struct machine *m = malloc(sizeof (struct machine));
   m->head = (int) (TAPE_SIZE/2); // Put in middle of tape.
   m->leftmost = Tape_Make();
   m->current = m->leftmost;

   // Write the inputs to the tape.
   int i;
   for (i=0; i < num_inputs; i++) {
      int k;
      for (k=0; k < inputs[i]; k++) {
         M_Write(m, CHAR_CODE_1);
         M_MvRight(m);
      }
      M_MvRight(m);
   }

   // Reset the head, clean up.
   m->current = m->leftmost;
   m->head = (int) (TAPE_SIZE/2);
   return m;
}

void
M_Del (struct machine *m)
{
  
   // First free all the tapes in this machine.
   struct tape *tape = m->leftmost;
   while (tape != NULL) {
      struct tape *next = tape->next;
      Tape_Del(tape);
      tape = next;
   }

   // Then free the machine.
   m->leftmost = NULL;
   m->current = NULL;
   free(m);

}

struct tape *
Tape_Make () {
   struct tape *tape = malloc(sizeof (struct tape));
   tape->prev = NULL;
   tape->next = NULL;
   tape->cells = calloc(sizeof(char), TAPE_SIZE);
   return tape;
}

void
Tape_Del (struct tape *tape) {
   tape->prev = NULL;
   tape->next = NULL;
   free(tape->cells);
   free(tape);
}



// Machine instructions.
// ============================================================

void
M_Write (struct machine *m, char c) {
   int head = m->head;
   (m->current->cells)[head] = c;
}

char
M_Read (struct machine *m) {
   int head = m->head;
   return (m->current->cells)[head];
}

void
M_MvRight (struct machine *m) {
   int head = (m->head + 1) % TAPE_SIZE;
   if (head == 0) {
      struct tape *next_tape = Tape_Make();
      m->current->next = next_tape;
      m->current = next_tape;
   }
   m->head = head;
}

void
M_MvLeft (struct machine *m) {
   int head = (m->head - 1) % TAPE_SIZE;
   if (head == -1) {
      head = TAPE_SIZE - 1; // Wrap around to end.
      struct tape *prev_tape = Tape_Make();
      m->current->prev = prev_tape;
      m->current = prev_tape;
   }
   m->head = head;
}







