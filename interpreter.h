

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "str.h"
#include "program.h"
#include "machine.h"

   /**
      Check to see if the given state is the halting state.
   **/
int Prog_Halted (Machine *m,
                 Program *prog,
                 Str *state);

   /**
      Perform one step of the program on the machine in the given state.
   **/
void Prog_Step (Machine *m,
                Program *prog,
                Str *state);

#endif
