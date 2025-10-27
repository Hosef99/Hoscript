# Hoscript

## Description
This project is a calculator that is capable of doing basic operations. 

## Features
- able to calculate the basic operations like addition, subtraction, multiplication, division and exponentiation
- supports common math functions like: sin, abs (absolute value), exp, log, sqrt, etc...
- supports matrix operations like addition, subtraction, multiplication, scalar multiplication and exponentiation
- supports matrix functions like determinant, inverse, zero, identity matrix

## Grammar
```
parse -> assign EOF
expr -> term ((PLUS | MINUS) term)*| (IDENT | postfix) ASSIGN expr
term -> mul ((MUL | DIV) mul)*
mul -> factor (IMPLICIT_MUL factor)*
factor -> (PLUS | MINUS)* power 
power -> postfix (POW factor)?
postfix  → base (LBRACK expr RBRACK)*
base -> NUM | IDENT | func | LPAREN expr RPARAN | mat
func -> IDENT LPARAN (expr (COMMA expr)*)? RPARAN
mat -> LBRACK row (SEMICOLON row)* RBRACK
row -> expr (COMMA expr)*
IMPLICIT_MUL -> is triggered when the next factor can logically follow without an operator
```

## Install
Just clone the repo and build it by typing `make` in cmd, the `cal.exe` file is located in the `bin` folder. If you do not have make installed, please google it.

## Usage
Run `cal.exe` in cmd. Type `help` to look for help.