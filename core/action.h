
#ifndef ACTION_H
#define ACTION_H

      /** These are basic instructions that a machine can execute. They map to the
          below functions. M_ERR should be used to signify instructions that don't
          make sense (e.g. instructions issued after a program has halted, or when
          the program has not halted and there are no further instructions). **/
   typedef enum { M_LEFT, M_RIGHT, M_PRINT, M_ERR } Action;

#endif

