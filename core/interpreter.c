
#include <stdlib.h>
#include "map.h"
#include "interpreter.h"

int
Prog_Halted (struct machine *m, Program *prog, Str *state)
{
   return state == NULL || Str_EqIgnoreCase(state, "halt");
}

void
Prog_Step (Machine *m, Program *prog)
{

   // Check if you are in the halting state.
   Str *state =  M_State(m);
   if (Prog_Halted(m, prog, state)) return;
   
   // Read input. Look up the appropriate instruction.
   char input = M_Read(m);
   Instruction instr = Prog_NextInstruction(prog, state, input);

   // Perform instruction.
   switch (instr.action) {
      case M_ERR:
         state = NULL;
         break;
      case M_LEFT:
         M_MvLeft(m);
         break;
      case M_RIGHT:
         M_MvRight(m);
         break;
      case M_PRINT:
         M_Write(m, instr.output);
         break;
   }

   // Look up and perform transition. Free memory.
   if (state != NULL) M_NextState(m, prog, input); 
   Str_Free(state);

}
