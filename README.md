# Interpreter
Simple interpreter for the BASIC programming language,
created in c++.

Interpreter supports so far following commands:
PRINT,
LET,
INPUT,
END,
GOTO,
RANDOM (0 - 1),
PI,
IF,
FOR,
TO,
NEXT,
INT,
DEC,
STR,
int,
str,
dec,
DEFINE,
RETURN,
ENDDEF

Expression ends with a newline.
Can calculate INT & FLOAT.
typecast between INT -> FLOAT and the other way around.
Supported datatypes are INT, FLOAT & STRING.
Compare operatos are <, > and =.
Support comparison between INT to INT, FLOAT to FLOAT,
FLOAT to INT and STR to STR.
undefined datatypes is treated as an INT
datatype keywords with lowercases uses in datatype definition
for function parameters. In all other cases uses upercases.
variables and function names can only contains letters.
Funktion body is initialized in function calls such as
LET var = myFunction().
