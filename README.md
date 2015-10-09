This is a Turing machine simulator. It has its own language for specifying machines. These Turing machines are unbounded in the amount of tape they can use and are also able to call other Turing machines as subroutines. The interactive interpreter lets you watch their step-by-step computation.

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

Libraries
=========
This project uses the MU unit testing library:
https://github.com/siu/minunit

Copyright
=========
This project is licenced under the [GPL3](http://www.gnu.org/licenses/gpl-3.0.en.html).
