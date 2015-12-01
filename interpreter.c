

#ifndef STDLIB_H
   #include <stdlib.h>
#endif

#ifndef INTERPRETER_H
   #include "interpreter.h"
#endif

#ifndef PROGRAM_H
   #include "program.h"
#endif

#ifndef MACHINE_H
   #include "machine.h"
#endif

#ifndef STR_H
   #include "str.h"
#endif

int
Prog_Halted (struct machine *m, Program *prog, Str *state)
{
   return Str_EqIgnoreCase(state, "halt");
}

   /**
      Execute one step of the machine.
   **/
void
Prog_Step (Machine *m, Program *prog, Str *state)
{

   // Check if you are in the halting state.
   if (Prog_Halted(m, prog, state)) return;
   
   // Look up clauses for current state.
   struct clause **clauses = Map_Get (prog->states, state);

   // If no such state defined, halt.
   if (clauses == NULL) {
      state = NULL;
      return;
   }
   
   // Clause resolution.
   struct clause *cl = Prog_ResolveClause(m, prog, state, clauses);

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
Prog_ResolveClause (Machine *m, Program *prog, Str *state, struct clause **clauses)
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

