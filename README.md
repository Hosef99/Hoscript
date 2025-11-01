# Hoscript

## Description
This project is a calculator that is capable of doing basic operations. 

## Features
- able to calculate the basic operations like addition, subtraction, multiplication, division and exponentiation
- supports common math functions like: sin, abs (absolute value), exp, log, sqrt, etc...
- supports matrix operations like addition, subtraction, multiplication, scalar multiplication and exponentiation
- supports matrix functions like determinant, inverse, zero, identity matrix
- supports strings, string concatenation
- supports lists
- added if else
- added for loops

## What it couldn't do
- append a list, finding the size of a list, delete an element from a list
- finding the size of a string, replacing characters in a string
- iterate through a matrix
- break out of a for loop

## Grammar
```
parse   -> stmt (NEWLINE stmt)* EOF

stmt    -> assign | if_stmt | for_stmt | block

block   -> LBRACE (stmt (NEWLINE stmt)*)? RBRACE

if_stmt -> IF LPAREN assign RPAREN stmt (ELSE stmt)?
for_stmt -> FOR LPAREN IDENT IN expr RPAREN stmt

assign  -> comp (ASSIGN assign)*
comp    -> expr ((GREATER | LESSER | GEQ | LEQ | EQ) expr)*
expr    -> term ((PLUS | MINUS) term)* | (IDENT | postfix) ASSIGN expr
term    -> mul ((MUL | DIV) mul)*
mul     -> factor (IMPLICIT_MUL factor)*
factor  -> (PLUS | MINUS)* power 
power   -> postfix (POW factor)?
postfix -> base (LBRACK slice RBRACK)*
slice   -> expr COLON expr
base    -> NUM | IDENT | func | LPAREN expr RPAREN | list
func    -> IDENT LPAREN (expr (COMMA expr)*)? RPAREN
list    -> LBRACK assign (COMMA assign)* RBRACK
IMPLICIT_MUL -> is triggered when the next factor can logically follow without an operator
```

## Install
Just clone the repo and build it by typing `make` in cmd, the `hos.exe` file is located in the `bin` folder. If you do not have make installed, please google how to get it.

## Usage
### REPL
For Windows systems, run `bin\hos.exe` in the command prompt.
For Linux systems, run `./bin/hos` in terminal.
Type `help()` to look for help.

### Running a file
To run a file, put the filename in the argument.
In Windows, run `bin\hos.exe filename.hos` in the command prompt.
In Linux, run `./bin/hos filename.hos` in terminal.

## Acknowledgements
Special thanks to [@hongttisme](https://github.com/hongttisme) for introducing me to the main ideas of compiler and being the man to discuss when I have questions. Appreciate all the help provided from [@sym111222333](https://github.com/sym111222333), which helped a lot on indexing and expression comparisons. 
