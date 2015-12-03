
#ifndef MACHINE_H
#define MACHINE_H

   typedef struct machine Machine;

   typedef enum { M_LEFT, M_RIGHT, M_PRINT, M_ERR } Action;

   // Constructing and destructing machines.
   Machine *MakeMachine (void);
   void DelMachine (Machine *m);

   // Machine instructions.
   void M_Write (Machine *m, char c);
   char M_Read (Machine *m);
   void M_MvRight (Machine *m);
   void M_MvLeft (Machine *m);

#endif
