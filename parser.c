

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
#ifndef UNISTD_H
   #include <unistd.h>
#endif

#include "str.h"
#include "map.h"
#include "list.h"


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
   if (!gobble_str(data, "->"))\
      ERR("Missing '->'");\
} while(0)

#define COMMA do {\
   if (!gobble_char(data, ','))\
      ERR("Missing ','");\
} while(0)

#define DONE done(data)


#include "program.h"

// Definitions.
// ======================================================================

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
static inline void Parse_State (DATA *, Map *);

// Static analysis.
static inline int count_states (DATA *);
static inline int count_clauses (DATA *);
static inline void validate_program (struct program *program);

// For the map.
void Map_FreeStr (void *s);
int Map_CmpStr (void *v1, void *v2);

// Parsing helpers.
// ======================================================================

static inline int is_delim (char c)
{
   return isspace(c) || c == ':' || c == '.';
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
         if (data->prog->num_inputs < 0)
            ERR("Specified number of inputs as %d: must be a non-negative amount.", data->prog->num_inputs);
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
   data->prog->name = malloc(Str_SizeOf());
   Str *s = parse_string(data);
   memcpy(data->prog->name, s, Str_SizeOf());
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
   data->prog->num_inputs = parse_number(data);
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
   data->prog->init_state = malloc(Str_SizeOf());
   memcpy(data->prog->init_state, init_state, Str_SizeOf());
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

   // Mapping of Str -> State
   Map *map = Map_Make(num_states,                  // size of map
                       Str_SizeOf(), sizeof(struct clause *),
                       NULL,                        // the hash function
                       Map_FreeStr, Map_CmpStr,     // key functions
                       NULL, NULL);

   // Parse the states.
   while (!DONE) Parse_State (data, map);
   data->prog->states = map;
   
}

static inline struct clause *Parse_Clause (DATA *data)
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

   // Convert action to appropriate thing.
   Action act;
   if (Str_Eq(action, "right"))
      act = RIGHT;
   else if (Str_Eq(action, "left"))
      act = LEFT;
   else if (Str_Eq(input_s, "blank"))
      act = PRINT;
   else if (Str_Len(input_s) == 1)
      act = PRINT;
   else
      ERR("Unknown action for clause.");

   // Construct and return clause.
   struct clause *cl = malloc(sizeof (struct clause));
   cl->input = input;
   cl->action = act;
   cl->end_state = Str_Make(Str_Guts(transition));
   free(input_s);
   free(action);
   free(transition);
   return cl;

}

static inline void Parse_State (DATA *data, Map *map)
{

   // Parse name of state.
   Str *str = parse_string(data);
   if (Map_Contains (map, str))
      ERR("Duplicate state found.");
   COLON;

   // Count number of clauses.
   int num_clauses = count_clauses(data);
   if (num_clauses < 1)
      ERR("Need at least one clause.");
   struct clause **clauses = calloc(num_clauses, sizeof (struct clause));
   
   // Parse the clauses.
   int i;
   for (i=0; i < num_clauses; i++){
      clauses[i] = Parse_Clause (data);
   }

   // Put into map.
   Map_Put (map, str, clauses);
   free(str);

}

static inline int count_clauses (DATA *data)
{
   int ogIndex = data->index;
   int count = 0;

   Str *s;

   while (!done(data)) {
      s = parse_string(data);
      s = parse_string(data);
      if (Str_Eq(s, ":"))
         break;
      if (!Str_Eq(s, "->"))
         ERR("Unknown string while parsing clauses.");
      s = parse_string(data);
      COMMA;
      s = parse_string(data);
      TERMINATOR;
   }

   free(s);
   data->index = ogIndex;
   return count;

}

static inline int count_states(DATA * data)
{
   // TO IMPLEMENT
   return -1;
}


// Static analysis functions.
// ======================================================================


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
   struct parse_data *data = malloc(sizeof(struct parse_data));
   data->index = 0;
   data->len = Str_Len(string);
   data->text = Str_Guts(string);
   data->line_num = 1;
   data->prog = malloc(sizeof (struct program));

   // Parse meta info.
   Parse_Header(data);

   // Parse states.
   //Parse_States(data);

   // Check the program is a good one.
   //validate_program(data->prog);
   
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
   struct program *prog = Parse_Program(prog_text);
   
   // Display parsing details.
   printf("%s parsed.\n\n", argv[1]);
   printf("DETAILS\n");
   printf("==============\n");
   printf("Program name: %s\n", Str_Guts(prog->name));
   printf("Num inputs: %d\n", prog->num_inputs);
   printf("Initial state: %s\n\n", Str_Guts(prog->init_state));
   
   // Tear down everything.
   Str_Free(prog_text);
   // .. should also free the struct program here.
   return 0;
   
   IOerr:
      printf("Error reading file: %s\n", argv[1]);
      return 1;
   
}

















