

#ifndef STDLIB_H
   #include <stdlib.h>
#endif

#ifndef PROGRAM_H
   #include "program.h"
#endif

#ifndef STR_H
   #include "str.h"
#endif

#ifndef MAP_H
   #include "map.h"
#endif

void Program_Free (struct program *prog)
{
   Map_Free(prog->states);
   Str_Free(prog->name);
   Str_Free(prog->init_state);
   free(prog);
}

void Clause_Free (struct clause *clause)
{
   Str_Free(clause->end_state);
   free(clause);
}

int SizeOf_Program()
{
   return sizeof(struct program);
}
