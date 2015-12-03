
typedef struct machine Machine;

typedef enum { M_LEFT, M_RIGHT, M_PRINT } Action;

// Constructing and destructing machines.
Machine *MakeMachine (void);
void DelMachine (Machine *m);

// Machine instructions.
void Write (Machine *m, char c);
int Read (Machine *m, char c);
void MvRight (Machine *m);
void MvLeft (Machine *m);

