
/* This module represents a Turing machine. A Turing machine has doubly infinite
   tape in both directions and a read-write head. It has a very basic instruction
   set for manipulating the tape. It basically serves as the memory/state for an
   executing program. */

#ifndef MACHINE_H
#define MACHINE_H

   #include "action.h"
   #include "program.h"
   #include "stdlib.h"

   #define BLANK 32

   typedef struct machine Machine;

      /** These are basic instructions that a machine can execute. They map to the
          below functions. M_ERR should be used to signify instructions that don't
          make sense (e.g. instructions issued after a program has halted, or when
          the program has not halted and there are no further instructions). **/

      /** Functions for allocating/freeing machines. **/
   Machine *M_Make (Program *prog, int *inputs);
   void M_Del (Machine *m);

      /** Write the specified character onto the cell underneath the head of
          the machine. **/
   void M_Write (Machine *m, char c);

      /** Returns the character in the cell under the head of the machine. **/
   char M_Read (Machine *m);

      /** Move the head of the machine to the right by one cell. **/
   void M_MvRight (Machine *m);

      /** Move the head of the machine to the left by one cell. **/
   void M_MvLeft (Machine *m);

      /** Get the symbol on the tape at the head, with the specified offset. **/
   char M_CharAtHead (Machine *m, int offset);









#endif
