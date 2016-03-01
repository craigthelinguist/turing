
/* This module executes programs on Turing machines. It translates the structure
   of a program into the underlying Turing machine instructions.

   You can execute a program by repeatedly calling I_Step. A program has finished
   executing when I_Halted returns a non-negative value. */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdlib.h>
#include "str.h"
#include "map.h"
#include "program.h"
#include "machine.h"

   /**
      Check if the program has finished executing.
   **/
int I_Halted (Machine *m, Program *prog);

   /**
      Perform the next step in the program execution. If there is no next
      step, or the program has halted, then this does nothing.
   **/
void I_Step (Machine *m, Program *prog);

#endif
