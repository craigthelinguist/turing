

#ifndef PARSER_H
#define PARSER_H

   #include <ctype.h>
   #include <string.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <unistd.h>

   #include "str.h"
   #include "map.h"
   #include "list.h"

   #include "program.h"

   /**
      Return the program described by the specified input string.
   **/
   Program *Parser_ProgFromString (Str *string);

   /**
      Return the program described by the contents of the file.
      Returns a null pointer if the file does not exist.
   **/
   Program *Parser_ProgFromFile (Str *fname);

#endif
