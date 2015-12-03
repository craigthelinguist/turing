
/* A program defines a series of instructions that may be executed on a Turing
   machine.

   You can allocate and free programs using Prog_Make, Prog_Free, and
   Prog_SizeOf().

   After allocating you can use the below modifying functions to change a
   program's metadata and add clauses to it. You can query and use the program
   by using the below accessing functions.

   Once you have set up a program's metadata and added clauses/states to it,
   call Prog_Finalise. This signifies that the program is now well-formed and
   ready for execution on a Turing machine. After finalising you may no longer
   call the modifying functions (doing so causes an error). */



#ifndef PROGRAM_H
#define PROGRAM_H

   #include "map.h"
   #include "str.h"
   #include "machine.h"

   // Public functions for using and accessing a program.
   // ============================================================

   typedef struct program Program;

   typedef struct instruction {
      Action action;
      char output;
   } Instruction;


   // Accessing functions.
   // ============================================================

      /**
         These functions return metadata about the program. The Str
         pointers returned are freshly allocated, so you should
         free them when you're done.
      **/
   int Prog_NumInputs (Program *prog);
   Str *Prog_Name (Program *prog);
   Str *Prog_InitState (Program *prog);

      /**
         Check if the program has a state with the given name.
      **/
   int Prog_IsStateDefined (Program *prog, Str *s);

      /**
         Return the number of states the program has.
      **/
   int Prog_NumStates (Program *prog);

      /**
         Return the instruction that should be executed next. If there isn't
         one the instruction returned is M_ERR.
            prog : the program being executed.
            state : name of the state the machine is currently in.
            input : the character being read on the tape.
      **/
   Instruction Prog_NextInstruction (Program *prog, Str *state, char input);

      /**
         Return the state that the machine should transition into after
         executing the next instruction. If there isn't one NULL is returned.
            prog : the program being executed.
            state : name of the state the machine is currently in.
            input : the character being read on the tape.
      **/
   Str *Prog_NextTransition (Program *prog, Str *state, char input);



   // Allocation functions.
   // ============================================================

      /**
         These functions are used for making, deleting, and allocating
         memory for programs.
      **/
   void Prog_Free (Program *pr);
   Program *Prog_Make (void);
   int Prog_SizeOf (void);



   // Modifying functions.
   // ============================================================

      /**
         This finalises the program. Before finalising the contents of
         a program can be modified but they cannot be read. After
         finalising the contents of the program can no longer be
         modified but become readable. If the program is not well-formed
         then this will cause an error.
      **/
   void Prog_Finalise (Program *pr);

      /**
         These functions are used for setting metadata about a given
         program. They may only be used before a program is finalised.
         They all have copy-by-value semantics so the program won't be
         storing the same Str * that you pass in.
      **/
   void Prog_SetName (Program *prog, Str *str);
   void Prog_SetInitState (Program *prog, Str *state_name);
   void Prog_SetNumInputs (Program *prog, int inputs);

      /**
         Add a state to the program. This is done by passing in three arrays.
         The length of the arrays should equal num_clauses.
            prog : program you're adding states to.
            state_name : the name of the state.
            num_clauses : number of clauses in the state definition.
            inputs : the inputs for each clause.
            instrs : the instructions for each clause.
            end_states : the transition states for each clause.
      **/
   void Prog_AddState (Program *prog, Str *state_name, int num_clauses,
                       char *inputs, Instruction *instrs, Str **end_states);

#endif
