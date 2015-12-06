

/* This module is for parsing text files describing programs and turning them into
   some intermediate form that an interpreter can execute on a Turing machine. The
   input is a string of text describing a program and the output is the actual
   program. Here is the grammar:

Here is the grammar.

PROGRAM     ::= HEADER [DEFINITION]+

HEADER      ::= NAME INPUTS INITIAL [IMPORTS]?
NAME        ::= Routine: IDEN.
INPUTS      ::= Inputs: NUMBER.
INITIAL     ::= Init: IDEN.
IMPORTS     ::= Imports: [IDEN]+ [,IDEN]*.

DEFINITION  ::= IDEN: [CLAUSE]+ 
CLAUSE      ::= [NUMBER | LETTER | blank] -> ACTION, IDEN.
ACTION      ::= PRIMITIVE | INVOCATION
PRIMITIVE   ::= left | right | [NUMBER | LETTER | blank]
INVOCATION  ::= IDEN.IDEN([ARGLIST]?)

ARGLIST     ::= SYMBOL | SYMBOL,ARGLIST

SYMBOL      ::= NUMBER | LETTER | BLANK
IDEN        ::= [a-zA-Z0-9]+
WORD        ::= [a-zA-Z]+
NUMBER      ::= [0-9]+

**/



// Headers.
// ======================================================================

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "str.h"
#include "map.h"
#include "list.h"

#include "program.h"


// Some handy macros.
// ======================================================================

#define SKIP skip_whitespace(data)

#define ERR(...) do {\
   fprintf(stderr, __VA_ARGS__);\
   fprintf(stderr, "\n");\
   abort();\
} while(0)

#define TERMINATOR do {\
   if (!gobble_char(data, '.'))\
      ERR("Missing '.' terminator");\
} while(0)

#define COLON do {\
   if (!gobble_char(data, ':'))\
      ERR("Missing ':'");\
} while(0)

#define ARROW do {\
   if (!gobble_token(data, "->"))\
      ERR("Missing '->'");\
} while(0)

#define COMMA do {\
   if (!gobble_char(data, ','))\
      ERR("Missing ','");\
} while(0)

#define DONE done(data)


// Definitions.
// ======================================================================

struct parse_data {
   char *text;
   int index;
   int len;
   int line_num;
   Program *prog;
};

typedef struct parse_data DATA;



// Function definitions.
// ======================================================================

// Parsing helpers.
static inline int is_delim (char c);
static inline int is_number (Str *string);
static inline int done (DATA *data);
static inline void skip_whitespace (DATA *data);
static inline int gobble_char (DATA *data, char c);
static inline int gobble_str (DATA *data, char *s);
static inline int gobble_str_insensitive(DATA *data, char *s);

static inline Str *parse_line (DATA *);
static inline Str *parse_string (DATA *);
static inline Str *peek_string (DATA *);
static inline int parse_number (DATA *);

// Parsing grammar rules.
static inline void Parse_Header (DATA *);
static inline void Parse_Name (DATA *);
static inline void Parse_Inputs (DATA *);
static inline void Parse_InitState (DATA *);
static inline void Parse_States (DATA *);
static inline void Parse_State (DATA *);
static inline void Parse_Clause (DATA *data, int index, char *inputs,
                                 Instruction *instrs, Str **end_states);

// Static analysis - should this move to a separate module?
static inline int count_states (DATA *);
static inline int count_clauses (DATA *);

// For the map.
void Map_FreeStr (void *s);
int Map_CmpStr (void *v1, void *v2);
void Map_FreeClauses (void *arr_clauses);

// Parsing helpers.
// ======================================================================

static inline int is_delim (char c)
{
   return isspace(c) || c == ':' || c == '.' || c == ',';
}

static inline int done(DATA * data)
{
   SKIP;
   return data->index >= data->len;
}

static inline void skip_whitespace (DATA * data)
{
   while (data->index < data->len && isspace(data->text[data->index])) {
      char c = data->text[data->index];
      if (c == '\r' || c == '\n')
         data->line_num++;
      data->index++;      
   }
}

static inline Str *parse_line (DATA *data)
{
   SKIP;
   int i = 0;
   while (!isspace(data->text[data->index+i])) i++;
   char contents[i + 1];
   memcpy(contents, data->text + data->index, i);
   contents[i] = '\0';
   data->index += i;
   return Str_Make(contents);
}

static inline Str *parse_string (DATA * data)
{
   SKIP;
   int i = 0;
   while (!is_delim(data->text[data->index+i])) i++;
   char contents[i + 1];
   memcpy(contents, data->text + data->index, i);
   contents[i] = '\0';
   data->index += i;
   return Str_Make(contents);
}

static inline Str *peek_string (DATA * data)
{
   SKIP;
   int i = 0;
   while (!is_delim(data->text[data->index+i])) i++;
   char contents[i + 1];
   memcpy(contents, data->text + data->index, i);
   contents[i] = '\0';
   data->index += i;
   return Str_Make(contents);
}

static inline int parse_number (DATA * data)
{
   SKIP;
   Str *s = parse_string(data);
   if (!is_number(s))
      ERR("Num of inputs to program should be numeric value.");
   int i = Str_ToInt(s);
   free(s);
   return i;
}

static inline int gobble_char (DATA * data, char c)
{
   SKIP;
   if (data->text[data->index] != c)
      return 0;
   data->index++;
   return 1;
}

static inline int gobble_str (DATA * data, char *s)
{
   SKIP;
   int i;
   for (i=0; s[i] != '\0' && isalnum(data->text[data->index+i]); i++) {
      if (s[i] != data->text[data->index+i]) return 0;
   }
   if (isalnum(data->text[data->index+i])) return 0;
   data->index += i;
   return 1;
}

static inline int gobble_token (DATA *data, char *s)
{
   SKIP;
   int i;
   for (i=0; s[i] != '\0' && !isspace(data->text[data->index+i]); i++) {
      if (s[i] != data->text[data->index+i]) return 0;
   }
   data->index += i;
   return 1;
}

static inline int gobble_str_insensitive (DATA * data, char *s)
{
   SKIP;
   int i;
   for (i=0; s[i] != '\0' && isalnum(data->text[data->index+i]); i++) {
      if (tolower(s[i]) != tolower(data->text[data->index+i])) return 0;
   }
   if (isalnum(data->text[data->index+i])) return 0;
   data->index += i;
   return 1;
}

static inline int is_number (Str *string)
{
   int i = 0;
   char *chars = Str_Guts(string);
   for (; i < Str_Len(string); i++) {
      if (!isdigit(chars[i])) return 0;
   }
   return 1;
}

// Parsing program header.
// ======================================================================

   /**
      HEADER ::= NAME INPUTS INITIAL [IMPORTS]?
   **/
static inline void Parse_Header (DATA * data)
{

   // TODO: imports.

   // The things in the header to parse.
   // Note you can specify the header info in any order.
   int name, inputs, init;
   name = inputs = init = 0;
   int maxiters = 3; // number of things in header to parse.
   int i = 0;   

   // Loop around checking for the stuff in the header.
   // Throw an error if something is defined more than once.
   // MaxIters is an upper bound; avoids parser running to end of file.
   while (!(name && inputs && init) || i++ >= maxiters) {

      // Lookahead.
      Str *s = peek_string(data);

      // Case: parsing name of program.
      if (Str_EqIgnoreCase(s, "name")) {
         if (name) ERR("Name defined twice.");
         Parse_Name(data);
         name = 1;
      }

      // Case: parsing number of inputs to program.
      else if (Str_EqIgnoreCase(s, "inputs")) {
         if (inputs) ERR("Number of inputs defined twice.");
         Parse_Inputs(data);
         if (Prog_NumInputs(data->prog) < 0)
            ERR("Specified number of inputs as %d: must be a non-negative amount.",
                Prog_NumInputs(data->prog));
         inputs = 1;      
      }

      // Case: parsing name of initial state.
      else if (Str_EqIgnoreCase(s, "init")) {
         if (init) ERR("Initial state defined twice.");
         Parse_InitState(data);
         init = 1;      
      }

      // case: Unknown, throw your hands in the air.
      else {
         ERR("Unknown keyword in header file: '%s'", Str_Guts(s));
      }
   }

   // Throw an error if you're missing header info
   if (!name) ERR("Missing name declaration in header.");
   if (!inputs) ERR("Missing number of inputs in header.");
   if (!init) ERR("Missing initial state declaration in header.");

}

   /**
      NAME ::= Name: IDEN.
   **/
static inline void Parse_Name (DATA * data)
{
   if (!gobble_str_insensitive(data, "Name"))
      ERR("Expected name declaration.");
   COLON;
   Str *s = parse_string(data);
   Prog_SetName(data->prog, s);
   free(s);
   TERMINATOR;
}

   /**
      INPUTS ::= Inputs: NUMBER.
   **/
static inline void Parse_Inputs (DATA * data)
{
   if (!gobble_str_insensitive(data, "Inputs"))
      ERR("Expected inputs declaration.");
   COLON;
   int num_inputs = parse_number(data);
   Prog_SetNumInputs(data->prog, num_inputs);
   TERMINATOR;
}

   /**
      INITIAL ::= Init: IDEN.
   **/
static inline void Parse_InitState (DATA * data)
{
   if (!gobble_str_insensitive(data, "Init"))
      ERR("Expected declaration of initial state.");
   COLON;
   Str *init_state = parse_string(data);
   Prog_SetInitState(data->prog, init_state);
   free(init_state);
   TERMINATOR;
}



// Parsing state declarations.
// ======================================================================

static inline void Parse_States (DATA * data)
{

   // Figure out how many states in this program.
   int num_states = count_states(data);
   if (num_states == 0)
      ERR("No states found!");

   // Parse the states.
   while (!DONE) Parse_State (data);
   
}

static inline void Parse_State (DATA *data)
{

   // Parse name of state.
   Str *state_name = parse_string(data);

   if (Prog_IsStateDefined(data->prog, state_name))
      ERR("Duplicate state found.");
   COLON;

   // Count number of clauses.
   int num_clauses = count_clauses(data);
   if (num_clauses < 1)
      ERR("Need at least one clause.");

   char *clauses_inputs = calloc(num_clauses + 1, sizeof(char));
   Instruction *clauses_instrs = calloc(num_clauses + 1, sizeof(Instruction));
   Str **clauses_strs = calloc(num_clauses + 1, sizeof(Str *));

   // Parse the clauses.
   int i;
   for (i=0; i < num_clauses; i++) {
      Parse_Clause (data, i, clauses_inputs, clauses_instrs, clauses_strs);
   }

   // Put the state -> clauses pair into the program.
   Prog_AddState (data->prog, state_name, num_clauses,
                  clauses_inputs, clauses_instrs, clauses_strs);
   
   // This is the price of freedom.
   free(state_name);
   free(clauses_inputs);
   free(clauses_instrs);
   for (i=0; i < num_clauses; i++) {
      Str_Free(clauses_strs[i]);
   }
   free(clauses_strs);

}

static inline void Parse_Clause (DATA *data, int index, char *inputs,
                                 Instruction *instructions, Str **end_states)
{
   
   // Parse the clause.
   Str *input_s = parse_string(data);
   ARROW;
   Str *action = parse_string(data);
   COMMA;
   Str *transition = parse_string(data);
   TERMINATOR;

   // Convert input to appropriate char.
   char input;
   if (Str_Eq(input_s, "blank"))
      input = BLANK;
   else if (Str_Len(input_s) != 1)
      ERR("Input for clause must be a single character or blank.");
   input = Str_CharAt(input_s, 0);

   // Convert action to an instruction.
   Action act; char output;
   if (Str_Eq(action, "right"))        { act = M_RIGHT; output = '\0'; }
   else if (Str_Eq(action, "left"))    { act = M_LEFT;  output = '\0'; }
   else if (Str_Eq(input_s, "blank"))  { act = M_PRINT; output = ' '; }
   else if (Str_Len(input_s) == 1)     { act = M_PRINT; output = Str_CharAt(input_s, 0); }
   else ERR ("Unknown action for clause.");
   Instruction instr = { act, output };

   // Put data at current index.
   inputs[index] = input;
   instructions[index] = instr;
   end_states[index] = Str_Make(Str_Guts(transition));

   // Free all the stuff we've used.
   free(input_s);
   free(action);
   free(transition);

}

static inline int count_clauses (DATA *data)
{
   int ogIndex = data->index;
   int num_clauses = 0;

   Str *s = NULL;

   while (!done(data)) {
      s = parse_string(data);
      if (!gobble_token(data, "->"))
         break;
      s = parse_string(data);
      COMMA;
      s = parse_string(data);
      TERMINATOR;
      num_clauses++;
   }

   if (s != NULL) Str_Free(s);
   data->index = ogIndex;
   return num_clauses;

}

static inline int count_states (DATA * data)
{

   int ogIndex = data->index;
   int num_states = 0;

   Str *s;

   while (!done(data)) {

      // Parse state identifier and colon.
      s = parse_string(data); // identifier
      if (!gobble_char(data, ':'))
         ERR("Expected state, got something unknown.");
      num_states++;

      // Looking through a state. Skip over the clauses.
      int num_clauses = 0;
      while (1) {

         // Check if you're at the end.
         if (done(data)) break;

         // Check if you're parsing another clause, or are inside
         // an entirely new state definition.
         int indexb4 = data->index;
         s = parse_string(data);
         if (!gobble_token(data, "->")) {
            data->index = indexb4;
            break;
         }
           
         // Parse the rest of the clause.
         s = parse_string(data);
         COMMA;
         s = parse_string(data);
         TERMINATOR;
         num_clauses++;

      }

      // Done parsing state. Check number of clauses.
      if (num_clauses == 0)
         ERR("No clauses given for state definition."); 

   }

   // Clean up, reset parser, return count.
   if (s != NULL) Str_Free(s);
   data->index = ogIndex;
   return num_states;

}


// Public functions.
// ======================================================================

Program *Parse_Program (Str *string)
{

   // Ready the parser.
   struct parse_data *data = malloc(sizeof(struct parse_data));
   data->index = 0;
   data->len = Str_Len(string);
   data->text = Str_Guts(string);
   data->line_num = 1;
   data->prog = Prog_Make();

   // Parse meta info.
   Parse_Header(data);

   // Parse states.
   Parse_States(data);

   // Check the program is a good one.
   // validate_program(data->prog);
   
   // Free stuff.
   free(data);

   return data->prog;

}



// Main.
// ======================================================================

int main (int argc, char **argv)
{

   // Check cmd args.
   if (argc < 2) {
      printf("Usage: ./parser <fpath>\n");
      return 0;
   }
   
   // Check file exists.
   if (access(argv[1], F_OK) == -1)
      goto IOerr;
      
   // Open file, figure out length of buffer.
   char *buffer = NULL;
   long length;
   FILE *f = fopen (argv[1], "rb");
   if (!f) goto IOerr;
   fseek(f, 0, SEEK_END);
   length = ftell(f);
   fseek(f, 0, SEEK_SET);
   buffer = malloc (length + 1);
   if (!buffer) goto IOerr;
   
   // Read file contents into buffer. Make string.
   fread (buffer, sizeof(char), length, f);
   buffer[length] = '\0';
   Str *prog_text = Str_Make(buffer);
  
   // Do parsing.
   Program *prog = Parse_Program(prog_text);
  
   // Extract parsing details.
   Str *name_s = Prog_Name(prog);
   Str *init_state_s = Prog_InitState(prog);
   char *name = Str_Guts(name_s);
   char *init_state = Str_Guts(init_state_s);
   int num_states = Prog_NumStates(prog);
   int num_inputs = Prog_NumInputs(prog);
   
   // Print info.
   printf("\n");
   printf("%s parsed.\n\n", argv[1]);
   printf("DETAILS\n");
   printf("==============\n");
   printf("Program name: %s\n", name);
   printf("Number of inputs: %d\n", num_inputs);
   printf("Initial state: %s\n", init_state);
   printf("Number of states: %d\n", num_states);
   printf("\n");   

   // Tear down everything.
   free(name_s); free(init_state_s);
   free(name); free(init_state);
   Str_Free(prog_text);
   // Prog_Free(prog);
   // TODO: something fucks up when freeing this.
   return 0;

   IOerr:
      printf("Error reading file: %s\n", argv[1]);
      return 1;

}

















