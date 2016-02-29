

#include "machine.h"

#define TAPE_SIZE 100

#define CHAR_CODE_BLANK 32
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
   Str *state;
   struct tape *leftmost;
   struct tape *current;
};

struct tape *Tape_Make (void);
void Tape_Del (struct tape *);



// Memory allocation/freeing.
// ============================================================

struct machine *
M_Make (Program *prog, int *inputs)
{

   // Make the struct representing the machine.
   struct machine *m = malloc(sizeof (struct machine));
   m->head = (int) (TAPE_SIZE/2); // Put in middle of tape.
   m->leftmost = Tape_Make();
   m->current = m->leftmost;
   m->state = Prog_InitState(prog);

   // Write the inputs to the tape.
   int num_inputs = Prog_NumInputs(prog);
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
   tape->cells = malloc(sizeof(char) * TAPE_SIZE);
   int i;
   for (i = 0; i < TAPE_SIZE; i++) {
      tape->cells[i] = CHAR_CODE_BLANK;
   }
   return tape;
}

void
Tape_Del (struct tape *tape) {
   tape->prev = NULL;
   tape->next = NULL;
   free(tape->cells);
   free(tape);
}

char
M_CharAtHead (struct machine *m, int offset)
{
   struct tape *current = m->current;
   int myhead = m->head;

   while (offset != 0) {

      // Symbol is left of head.
      if (offset < 0) {
         
         // Move onto next tape to the left.
         if (myhead == 0) {
            if (current->prev == NULL) {
               return CHAR_CODE_BLANK;
            }
            else {
               current = current->prev;
               myhead = TAPE_SIZE - 1;      
               offset--;      
            }       
         }
  
         // Else move left one.
         else {
            myhead--;
            offset--;
         }
      }

      // Symbol is right of head.
      else {
         
         // Move onto next tape to the right.
         if (myhead == TAPE_SIZE - 1) {
            if (current->next == NULL) {
               return CHAR_CODE_BLANK;
            }
            else {
               current = current->next;
               myhead = 0;
               offset--;
            }         
         }

         // Else move right one.
         else {
            myhead++;
            offset--;
         }

      }
   }

   return current->cells[myhead];

}

void
M_NextState (Machine *m, Program *prog, char input)
{
   if (m->state == NULL) return;
   Str_Free(m->state);
   m->state = Prog_NextTransition(prog, m->state, input);
}

Str *
M_State (Machine *m)
{
   return Str_Copy(m->state);
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

