

#ifndef STR_H
   #include "str.h"
#endif

#ifndef MACHINE_H
   #include "machine.h"
#endif

#ifndef PROGRAM_H
   #include "program.h"
#endif

   /**
      Check to see if the given state is the halting state.
   **/
int Prog_Halted (Machine *m,
                 Program *prog,
                 Str *state);

   /**
      Check to see which clause should be executed by the machine
      running the program in the given state.
   **/
struct clause *Prog_ResolveClause (Machine *m,
                                    Program *prog,
                                    Str *state,
                                    struct clause **clauses);

   /**
      Perform one step of the program on the machine in the given state.
   **/
void Prog_Step (Machine *m,
                Program *prog,
                Str *state);

