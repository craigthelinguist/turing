

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"
#include "map.h"
#include "program.h"


#define ERR_MSG(...) do {\
   fprintf(stderr, __VA_ARGS__);\
   fprintf(stderr, "\n");\
} while(0)



// Data structures.
// ======================================================================

   /**
      This is the representation of a program that can be executed by an
      interpreter. The members are:
         states : a mapping from state names to an array of clauses.
         name : the name of the program.
         init_state : state the program should start in.
         num_inputs : number of inputs to the program. 
         finalised : whether the program has been correctly set up.   
            If a program has been set up it is an error to try and modify
            its members.
   **/
struct program {
   Map *states; // Str -> Array of Clauses
   Str *name;
   Str *init_state;
   int num_inputs;
   int finalised;
};

   /**
      A clause specifies what the machine should do when it reads a given
      input. The members for a clause are:
         input : the input for which the clause applies.
         action : what the machine should do if the clause applies.
         end_state : the state the machine should end in after applying
            the clause.
   **/
struct clause {
   char input;
   Instruction instruction;
   Str *end_state;
};


// Public function declarations.
// ======================================================================

Program *Prog_Make(void);
int Prog_SizeOf(void);
void Prog_Free (struct program *prog);

// Private function declarations.
// ======================================================================

struct clause *Clause_Make (char input, Instruction instr, Str *end_state);
void Clause_Free (struct clause *clause);
int Clause_SizeOf();  
void Map_FreeStr (void *s);
int Map_CmpStr (void *v1, void *v2);
void Map_FreeClauses (void *arr_clauses);



// Modifying functions.
// ======================================================================

void Prog_SetName (struct program *prog, Str *str)
{

   // Error check.
   if (prog->finalised)
      ERR_MSG("Error setting program name:\
               program metadata cannot be modified after it has been finalised.");

   // Copy name and set string.
   prog->name = malloc(Str_SizeOf());
   memcpy(prog->name, str, Str_SizeOf());

}

void Prog_SetInitState (struct program *prog, Str *state_name)
{

   // Error check.
   if (prog->finalised)
      ERR_MSG("Error setting initial state:\
               program metadata cannot be modified after it has been finalised.");

   // Copy name and set string.
   prog->init_state = malloc(Str_SizeOf());
   memcpy(prog->init_state, state_name, Str_SizeOf());

}

void Prog_SetNumInputs (struct program *prog, int inputs)
{
   
   // Error check.
   if (prog->finalised)
      ERR_MSG("Error setting number of inputs to program:\
               program metadata cannot be modified after it has been finalised.");

   // Set inputs.
   prog->num_inputs = inputs;

}

void Prog_AddState (Program *prog, Str *state_name, int num_clauses,
                    char *inputs, struct instruction *instrs, Str **end_states)
{

   // Error checking.
   if (prog->finalised)
      ERR_MSG("Error adding state to program:\
               program cannot be modified after it has been finalised.");
   if (num_clauses <= 0)
      ERR_MSG("Need at least 1 clause per state.");

   // Note the use of calloc: this is a null-terminated array.
   struct clause **arr_clauses = calloc(num_clauses + 1, Clause_SizeOf());

   // Allocate and build clauses.
   int i;
   for (i=0 ; i < num_clauses; i++) {
      arr_clauses[i] = Clause_Make(inputs[i], instrs[i], end_states[i]);
   }

   // Put into map.
   Str *s = malloc(Str_SizeOf());
   memcpy(s, state_name, Str_SizeOf());
   Map_Put(prog->states, s, arr_clauses);

}

void Prog_Finalise (Program *prog)
{

   // Check user is calling at the right time.
   if (prog->finalised)
      ERR_MSG("Error finalising: Program is already finalised.");
   
   // Check everything has been defined.
   if (prog->states == NULL || Prog_NumInputs(prog) <= 0)
      ERR_MSG("Error finalising: program has no states.");

   if (prog->name == NULL)
      ERR_MSG("Error finalising: program has no name.");

   if (prog->init_state == NULL)
      ERR_MSG("Error finalising: program has no initial state.");

   // Check those definitions are sensible.
   if (!Prog_IsStateDefined(prog, prog->init_state))
      ERR_MSG("Error finalising: could not find the specified initial state.");

   if (prog->num_inputs < 0)
      ERR_MSG("Error finalising: program must have non-negative number of inputs.");
   
   // Everything looks fine; mark program as finalised.
   prog->finalised = 1;

}



// Public functions for accessing, using programs.
// ======================================================================

int Prog_IsStateDefined (struct program *prog, Str *s)
{
   return Map_Contains (prog->states, s);
}

Str *Prog_Name (struct program *prog)
{
   Str *s = malloc(Str_SizeOf());
   memcpy(s, prog->name, Str_SizeOf());
   return s;
}

int Prog_NumInputs (struct program *prog)
{
   return prog->num_inputs;
}

Str *Prog_InitState (struct program *prog)
{
   Str *s = malloc(Str_SizeOf());
   memcpy(s, prog->init_state, Str_SizeOf());
   return s;
}

int Prog_NumStates (struct program *prog)
{
   return Map_Size(prog->states);
}

Instruction Prog_NextInstruction (Program *prog, Str *state, char input)
{

   // Check the state exists.
   if (!Map_Contains(prog->states, state)) {
      Instruction instr = { M_ERR, '\0' };
      return instr;
   }

   // Look through clauses and return the appropriate instruction.
   struct clause **clauses = Map_Get(prog->states, state);
   int i;
   for (i=0; clauses[i] != '\0'; i++) {
      if (clauses[i]->input == input) {
         return clauses[i]->instruction;      
      }   
   }

   // If no clause matches the input then return ERR.
   Instruction instr = { M_ERR, '\0' };
   return instr;

}

Str *Prog_NextTransition (Program *prog, Str *state, char input)
{
   
   // Check the state exists.
   if (!Map_Contains(prog->states, state))
      return NULL;

   // Look through clauses and return the appropriate instruction.
   struct clause **clauses = Map_Get(prog->states, state);
   int i;
   for (i=0; clauses[i] != '\0'; i++) {
      if (clauses[i]->input == input) {
         return clauses[i]->end_state;      
      }   
   }

   // If no clause matches the input then return ERR.
   return NULL;

}


// Public functions for allocating, deleting programs.
// ======================================================================

Program *Prog_Make (void)
{
   struct program *prog = malloc(Prog_SizeOf());
   prog->states = Map_Make (2,
                            Str_SizeOf(), sizeof(struct clause **),
                            NULL, // hash function
                            Map_FreeStr, Map_CmpStr,
                            NULL, NULL);
   prog->name = NULL;
   prog->init_state = NULL;
   prog->num_inputs = -1;
   prog->finalised = 0;
   return prog;
}

void Prog_Free (struct program *prog)
{
   Map_Free(prog->states);
   //Str_Free(prog->name);
   //Str_Free(prog->init_state);
   //free(prog);
}

int Prog_SizeOf()
{
   return sizeof(struct program);
}



// Private functions.
// ======================================================================

struct clause *Clause_Make (char input, Instruction instr, Str *end_state)
{
   struct clause *cl = malloc(Clause_SizeOf());
   cl->input = input;
   cl->instruction = instr;
   cl->end_state = malloc(Str_SizeOf());
   memcpy(cl->end_state, end_state, Str_SizeOf());
   return cl;
}

void Clause_Free (struct clause *clause)
{
   Str_Free(clause->end_state);
   free(clause);
}

int Clause_SizeOf()
{
   return sizeof(struct clause);
}



// Free/compare functions for map.
// ======================================================================

void Map_FreeStr (void *s)
{
   Str *str = s;
   Str_Free(str);
}

int Map_CmpStr (void *v1, void *v2)
{
   Str *s1 = v1;
   Str *s2 = v2;
   if (Str_Len(s1) != Str_Len(s2))
      return 1;
   return Str_Cmp(s1, s2);
}

void Map_FreeClauses (void *arr_clauses)
{
   struct clause**clauses = (struct clause **)arr_clauses;
   int i;
   for (i=0; clauses[i] != NULL; i++) {
      Clause_Free(clauses[i]);
   }
   free(arr_clauses);
}











