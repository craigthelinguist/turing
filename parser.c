

/**

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

#ifndef CTYPE_H
   #include <ctype.h>
#endif
#ifndef STRCMP_H
   #include <string.h>
#endif
#ifndef STDIO_H
   #include <stdio.h>
#endif
#ifndef STDLIB_H
   #include <stdlib.h>
#endif

#include "str.h"
#include "map.h"
#include "list.h"
#include "program.h"


// Some handy macros.
// ======================================================================

#define SKIP skip_whitespace(data)

#define ERR(...) do {\
   fprintf(stderr, __VA_ARGS__);\
   abort();\
} while(0)

#define TERMINATOR err(data, "Missing '.' terminator")
#define COLON err(data, "Missing ':'")
#define ARROW err(data, "Missing '->'")
#define COMMA err(data, "Missing ','")

// Definitions.
// ======================================================================

typedef enum stmt_type { PRINT, MOVEMENT, INVOCATION } StmtType;

   /**
      Each file describes one program.
    **/
struct program {
   Map *states; // Str -> State
   Str *name;
   Str *init_state;
   int num_inputs;
};

   /**
      An individual state within a program.
   **/
struct state {
   Str *name;
   int num_cases;
   char *inputs;
   struct stmt *actions;
   Str *transitions;
};

   /**
      A particular kind of instructions that involves calling another
      program.
   **/
struct invocation {
   Str *name;
   int num_args;
   char *args;
   Str *transition;
};

   /**
      Some instruction the state carries out.
   **/
struct stmt {
   StmtType type;
   union {
      struct invocation invocation;
      Str *primitive;
   };
};

struct parse_data {
   char *text;
   int index;
   int len;
   int line_num;
   struct program *prog;
};

typedef struct parse_data DATA;



// Function definitions.
// ======================================================================

// Parsing helpers.
static inline int done (DATA data);
static inline void skip_whitespace (DATA data);
static inline void err (DATA data, char *msg);
static inline int gobble_char (DATA data, char c);
static inline int gobble_str (DATA data, char *s);
static inline int gobble_str_insensitive(DATA data, char *s);
static inline int is_number (Str *s);

static inline Str *Parse_String (DATA);
static inline Str *Peek_String (DATA);
static inline int Parse_Number (DATA);

// Parsing grammar rules.
static inline void Parse_Header (DATA);
static inline void Parse_Name (DATA);
static inline void Parse_Inputs (DATA);
static inline void Parse_InitState (DATA);
static inline void Parse_States (DATA);

// Static analysis.
static inline int Count_States (DATA);
static inline void validate_program (struct program *program);



// Parsing helpers.
// ======================================================================

static inline int done(DATA data)
{
   skip_whitespace(data);
   return data.index >= data.len;
}

static inline void skip_whitespace (DATA data)
{
   int i = 0;
   while (data.index < data.len && isspace(data.text[data.index+i])) {
      char c = data.text[data.index+i];
      if (c == '\r' || c == '\n')
         data.line_num++;
      i++;
   }
}

static inline void err (DATA data, char *msg)
{

}

static inline Str *Parse_String (DATA data)
{
   int i = 0;
   while (!isalpha(data.text[data.index+i])) i++;
   char contents[i];
   memcpy(contents, data.text + data.index, i);
   data.index += i;
   return Str_Make(contents);
}

static inline Str *Peek_String (DATA data)
{
   Str *s = Parse_String(data);
   data.index -= Str_Len(s);
   return s;
}

static inline int Parse_Number (DATA data)
{
   Str *s = Parse_String(data);
   if (!is_number(s))
      ERR("Num of inputs to program should be numeric value.");
   return Str_ToInt(s);
}

static inline int gobble_char (DATA data, char c)
{
   if (data.text[data.index] != c) return 0;;
   data.index++;
   return 1;
}

static inline int gobble_str (DATA data, char *s)
{
   int i;
   for (i=0; s[i] != '\0' && isalnum(data.text[data.index+i]); i++) {
      if (s[i] != data.text[data.index+i]) return 0;
   }
   if (isalnum(data.text[data.index+i])) return 0;
   data.index += i;
   return 1;
}

static inline int gobble_str_insensitive (DATA data, char *s)
{
   int i;
   for (i=0; s[i] != '\0' && isalnum(data.text[data.index+i]); i++) {
      if (tolower(s[i]) != tolower(data.text[data.index+i])) return 0;
   }
   if (isalnum(data.text[data.index+i])) return 0;
   data.index += i;
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
static inline void Parse_Header (DATA data)
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

      // Lookahed.
      Str *s = Peek_String(data);

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
         if (data.prog->num_inputs < 0)
            ERR("Specified number of inputs as %d: must be a non-negative amount.", data.prog->num_inputs);
         inputs = 1;      
      }

      // Case: parsing name of initial state..
      else if (Str_EqIgnoreCase(s, "init")) {
         if (init) ERR("Initial state defined twice.");
         Parse_InitState(data);
         init = 1;      
      }

      // case: Unknown, throw your hands in the air.
      else {
         ERR("Unknown keyword in header file: %s.", Str_Guts(s));
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
static inline void Parse_Name (DATA data)
{
   if (!gobble_str_insensitive(data, "Name"))
      ERR("Expected name declaration.");
   SKIP; COLON; SKIP;
   data.prog->name = Parse_String(data);
   TERMINATOR; SKIP;
}

   /**
      INPUTS ::= Inputs: NUMBER.
   **/
static inline void Parse_Inputs (DATA data)
{
   if (!gobble_str_insensitive(data, "Inputs"))
      ERR("Expected inputs declaration.");
   SKIP; COLON; SKIP;
   data.prog->num_inputs = Parse_Number(data);
   TERMINATOR; SKIP;
}

   /**
      INITIAL ::= Init: IDEN.
   **/
static inline void Parse_InitState (DATA data)
{
   if (!gobble_str_insensitive(data, "Init"))
      ERR("Expected declaration of initial state.");
   SKIP; COLON; SKIP;
   data.prog->init_state = Parse_String(data);
   TERMINATOR; SKIP;
}



// Parsing state declarations.
// ======================================================================

static inline void Parse_States (DATA data)
{
   struct state state;
}



// Static analysis functions.
// ======================================================================

static inline int Count_States(DATA data)
{
   return -1;
}

static inline void validate_program (struct program *program)
{
   if (program->num_inputs < 0)
      fprintf(stderr, "Program has %d inputs - should have a non-negative amount.", program->num_inputs);
   if (!Map_Contains(program->states, program->init_state))
      fprintf(stderr, "Initial state is %s, which hasn't been defined.", Str_Guts(program->init_state));
}



// Used for map lookups.
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



// Public functions.
// ======================================================================

struct program *Parse_Program (Str *string)
{

   // Ready the parser.
   struct parse_data data;
   data.index = 0;
   data.len = Str_Len(string);
   data.text = Str_Guts(string);
   data.line_num = 1;
   data.prog = malloc(sizeof (struct program));

   // Parse meta info.
   Parse_Header(data);

   // Parse states.
   Parse_States(data);

   // Check the program is a good one.
   validate_program(data.prog);
   return data.prog;

}

