# Interpreter
Simple interpreter for the BASIC programming language,
created in c++.

Interpreter supports so far following commands:
PRINT,
LET,
INPUT,
END,
GOTO,
RANDOM,
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

Interpreter specification:
Expression ends with a newline.

Can calculate INT & FLOAT.

typecast between INT -> FLOAT and the other way around.

Supported datatypes are INT, FLOAT & STRING.
No support for negative numbers.

Compare operatos are <, > and =.
Support comparison between INT to INT, FLOAT to FLOAT,
FLOAT to INT and STR to STR.
< and > operators for STR compare the string length.

undefined datatypes is treated as an INT

RANDOM generates a random number between 0 and 1,

datatype keywords with lowercases uses in datatype definition
for function parameters. In all other cases uses upercases.
variables and function names can only contains letters.

Funktion body is initialized in function calls such as
LET var = myFunction().
Supports from zero to std::vector_maxsize arguments in function head.

Not ending a file with END will cause a crash.

Error handling:
has support for 31 build in error messages these can be find inside Interpreter/Interpreter/instructions/errorMessages.txt
Whene interpreter recalls an error in the input file it will end program and
print error message and linenumber inside Interpreter/Interpreter/instructions/errorLog.txt
Error message will be deleted on program startup.

Setup:
Clone project https://github.com/syvjohan/Interpreter.git
If you have Visual studio go to Interpreter/Interpreter/Interpreter.vcxproj and "double click".
To change input file goto Manager.cpp line 42. There is four existing input files in Interpreter/Interpreter/instructions.
In Manager.cpp line 85 in the else if condition for END command there has been set an sleep for 5 seconds this is used for debugging,
delete the line or set time to a desired value.

Enjoy!!!