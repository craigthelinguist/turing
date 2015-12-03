
#ifndef PROGRAM_H
#define PROGRAM_H

   #include "map.h"
   #include "str.h"
   #include "machine.h"

   #define BLANK 0

   typedef struct program Program;
   typedef struct clause Clause;

   typedef struct instruction {
      Action action;
      char output;
   } Instruction;


   // Public functions for using and accessing a program.
   // ============================================================

   int Prog_NumInputs (Program *prog);
   Str *Prog_Name (Program *prog);
   Str *Prog_InitState (Program *prog);

   int Prog_IsStateDefined (Program *prog, Str *s);
   int Prog_NumStates (Program *prog);

   Instruction Prog_NextInstruction (Program *prog, Str *state, char input);
   Str *Prog_NextTransition (Program *prog, Str *state, char input);

   // Public functions for creating/setting up a new program.
   // ============================================================

   void Prog_SetName (Program *prog, Str *str);
   void Prog_SetInitState (Program *prog, Str *state_name);
   void Prog_SetNumInputs (Program *prog, int inputs);

   void Prog_AddState (Program *prog, Str *state_name, int num_clauses,
                       char *inputs, Instruction *instrs, Str **end_states);

   void Prog_Free (Program *pr);
   Program *Prog_Make (void);
   int Prog_SizeOf (void);

#endif
