

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

struct parse_data {
   char *text;
   int index;
   int len;
   int line_num;
};

typedef struct parse_data DATA;



// Function definitions.
// ======================================================================

// Parsing helpers.
static inline int done (DATA data);
static inline void skip_whitespace (DATA data);
static inline void err (DATA data, char *msg);
static inline Str *parse_string (DATA data);
static inline int gobble_char (DATA data, char c);
static inline int gobble_str (DATA data, char *s);
static inline void validate_program (struct program *program);

// Parsing grammar rules.
static inline Str *Parse_String(DATA);
static inline Str *Parse_Name(DATA);
static inline int Parse_Inputs(DATA);
static inline Str *Parse_InitState(DATA);
static inline struct state Parse_State(DATA);
static inline int Count_States(DATA);



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
   fprintf(stderr, "Line %d: %s\n", data.line_num, msg);
   abort();
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

static inline int gobble_str_sensitive (DATA data, char *s)
{
   int i;
   for (i=0; s[i] != '\0' && isalnum(data.text[data.index+i]); i++) {
      if (tolower(s[i]) != tolower(data.text[data.index+i])) return 0;
   }
   if (isalnum(data.text[data.index+i])) return 0;
   data.index += i;
   return 1;
}

static inline void validate_program (struct program *program)
{
   if (program->num_inputs < 0)
      fprintf(stderr, "Program has %d inputs - should have a non-negative amount.", program->num_inputs);
   if (!Map_Contains(program->states, program->init_state))
      fprintf(stderr, "Initial state is %s, which hasn't been defined.", Str_Guts(program->init_state));
}


// Parsing grammar rules.
// ======================================================================

static inline Str *Parse_Name(DATA data)
{
   if (!gobble_str_insensitive(data, "Name"))
      err(data, "Expected name declaration.");
   skip_whitespace(data);
   if (!gobble_char(data, ':'))
      err(data, "Missing ':' in name declaration.");
   skip_whitespace(data);
   Str *name = Parse_String(data);
   if (!gobble_char(data, '.'))
      err(data, "Missing '.' terminator in name declaration (Name:%s)");//, name);
   skip_whitespace(data);
   return name;
}

static inline int Parse_Inputs(DATA data)
{
   return -1;
}

static inline Str *Parse_InitState(DATA data)
{
   return NULL;
}

static inline struct state Parse_State(DATA data)
{
   struct state state;
   return state;
}

static inline int Count_States(DATA data)
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
   struct parse_data data;
   data.index = 0;
   data.len = Str_Len(string);
   data.text = Str_Guts(string);
   data.line_num = 1;

   // Parse meta info.
   Str *routine_name = Parse_Name(data);
   int num_inputs = Parse_Inputs(data);
   if (num_inputs < 0)
      err(data, "Must have non-negative number of inputs.");
   Str *init_state = Parse_InitState(data);

   // Create and initialise the program.
   struct program *program = malloc(sizeof program);
   program->num_inputs = num_inputs;
   program->init_state = init_state;
   program->name = routine_name;
   int num_states = Count_States(data);
   if (num_states == 0)
      err(data, "Need at least one state.");
   program->states = Map_Make(num_states, sizeof(Str *), sizeof(struct state), NULL,
                              Map_FreeStr, Map_CmpStr, NULL, NULL);

   // Parse definitions.
   int i;
   for (i=0; i < num_states; i++) {
      struct state state = Parse_State(data);
      if (Map_Contains(program->states, state.name))
         err(data, "state being defined again.");
      Map_Put(program->states, state.name, &state);
   }

   // Check the program is a good one.
   validate_program(program);
   return program;

}

