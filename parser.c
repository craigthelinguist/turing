

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



// Function definitions.
// ======================================================================

// Parsing helpers.
static inline int done ();
static inline void skip_whitespace ();
static inline void err (char *msg);
static inline Str *parse_string ();
static inline int gobble_char (char c);
static inline int gobble_str (char *s);
static inline void validate_program (struct program *program);

// Parsing grammar rules.
static inline Str *Parse_String();
static inline Str *Parse_Name();
static inline int Parse_Inputs();
static inline Str *Parse_InitState();
static inline struct state Parse_State();
static inline int Count_States();


// Variables.
// ======================================================================

static int index = 0;
static char *data = 0;
static int len = 0;
static int line_num = 1;



// Parsing helpers.
// ======================================================================

static inline int done()
{
   skip_whitespace();
   return index >= len;
}

static inline void skip_whitespace ()
{
   int i = 0;
   while (index < len && isspace(data[index+i])) {
      char c = data[index+i];
      if (c == '\r' || c == '\n')
         line_num++;
      i++;
   }
}

static inline void err(char *msg)
{
   fprintf(stderr, "Line %d: %s\n", line_num, msg);
   abort();
}

static inline Str *Parse_String ()
{
   int i = 0;
   while (!isalpha(data[index+i])) i++;
   char contents[i];
   memcpy(contents, data + index, i);
   index += i;
   return Str_Make(contents);
}

static inline int gobble_char (char c)
{
   if (data[index] != c) return 0;;
   index++;
   return 1;
}

static inline int gobble_str (char *s)
{
   int i;
   for (i=0; s[i] != '\0' && isalnum(data[index+i]); i++) {
      if (s[i] != data[index+i]) return 0;
   }
   if (isalnum(data[index+i])) return 0;
   index += i;
   return 1;
}

static inline int gobble_str_sensitive (char *s)
{
   int i;
   for (i=0; s[i] != '\0' && isalnum(data[index+i]); i++) {
      if (tolower(s[i]) != tolower(data[index+i])) return 0;
   }
   if (isalnum(data[index+i])) return 0;
   index += i;
   return 1;
}

static inline void validate_program (struct program *program)
{
   if (program->num_inputs < 0)
      err("%s has %d inputs - should have a non-negative amount.");//, program->name, program->num_inputs);
   if (!Map_Contains(program->states, program->init_state))
      err("initial state of %s is %s, which hasn't been defined.");//, program->name, program->init_state);
}


// Parsing grammar rules.
// ======================================================================

static inline Str *Parse_Name()
{
   if (!gobble_str_insensitive("Name"))
      err("Expected name declaration.");
   skip_whitespace();
   if (!gobble_char(':'))
      err("Missing ':' in name declaration.");
   skip_whitespace();
   Str *name = Parse_String();
   if (!gobble_char('.'))
      err("Missing '.' terminator in name declaration (Name:%s)");//, name);
   skip_whitespace();
   return name;
}

static inline int Parse_Inputs()
{
   return -1;
}

static inline Str *Parse_InitState()
{
   return NULL;
}

static inline struct state Parse_State()
{
   struct state state;
   return state;
}

static inline int Count_States()
{
   return -1;
}

// Public functions.
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

struct program *Parse_Program (Str *string)
{

   // Ready the parser.
   index = 0;
   len = Str_Len(string);
   data = Str_Guts(string);

   // Parse meta info.
   Str *routine_name = Parse_Name();
   int num_inputs = Parse_Inputs();
   if (num_inputs < 0)
      err("Must have non-negative number of inputs.");
   Str *init_state = Parse_InitState();

   // Create and initialise the program.
   struct program *program = malloc(sizeof program);
   program->num_inputs = num_inputs;
   program->init_state = init_state;
   program->name = routine_name;
   int num_states = Count_States();
   if (num_states == 0)
      err("Need at least one state.");
   program->states = Map_Make(num_states, sizeof(Str *), sizeof(struct state), NULL,
                              Map_FreeStr, Map_CmpStr, NULL, NULL);

   // Parse definitions.
   int i;
   for (i=0; i < num_states; i++) {
      struct state state = Parse_State();
      if (Map_Contains(program->states, state.name))
         err("state being defined again.");
      Map_Put(program->states, state.name, &state);
   }

   // Check the program is a good one.
   validate_program(program);
   return program;

}

