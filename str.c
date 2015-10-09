

// Headers.
// ============================================================

#ifndef STDIO_H
   #include <stdio.h>
#endif

#ifndef STDLIB_H
   #include <stdlib.h>
#endif

#ifndef STRING_H
   #include <string.h>
#endif

#include "str.h"


// Definitions.
// ============================================================

struct str {
   int len;
   char *chars;
};

static inline void out_of_bounds (void);



// Internal functions.
// ============================================================

static inline void out_of_bounds ()
{
   fprintf(stderr, "Trying to access list at illegal index.");
   abort();
}



// Public functions.
// ============================================================

char Str_CharAt (Str *str, int index)
{
   if (index < 0 || index >= str->len)
      out_of_bounds();
   return str->chars[index];
}

int Str_Len (Str *str)
{
   return str->len;
}

Str *Str_Make (char *contents)
{
   int count = 0;
   while (contents+count != NULL)
      count++;
   struct str *str = malloc(sizeof (struct str));
   str->len = count;
   str->chars = malloc(sizeof (char) * count);
   memcpy(str->chars, contents, sizeof (char) * count);
   return str;
}

void Str_Free (Str *str)
{
   free(str->chars);
   str->chars = NULL;
   free(str);
}

int Str_Cmp (Str *str1, Str *str2)
{
   int minimum = str1->len < str2->len ? str1->len : str2->len;
   int maximum = str1->len < str2->len ? str2->len : str1->len;
   int difference = maximum - minimum;
   int i;
   for (i=0; i < minimum; i++) {
      if (str1->chars[i] != str2->chars[i]) difference++;
   }
   return difference;
}

char *guts (Str *str)
{
   return str.chars;
}



