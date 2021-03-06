This is a Turing machine simulator. It has its own language for specifying machines. Machines have doubly infinite tape in both directions and may call previously defined Turing machines as subroutines. The interactive interpreter visualises their step-by-step computation.

Grammar
=======
```Java
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
```

Building
========

Build and run the simulation like so:

```bash
make sim
./sim <prog-location> <prog-args>
```

For example, the program successor.tm takes one argument, so you can run it like this:
```bash
./sim programs/succesor.tm 4
```

To build the tests, type:
```bash
make tests
```

Libraries
=========
This project uses the MU unit testing library:
https://github.com/siu/minunit

Copyright
=========
This project is licenced under the [GPL3](http://www.gnu.org/licenses/gpl-3.0.en.html).
