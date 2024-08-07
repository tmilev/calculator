# Cpp code style

The code in the calculator has been written for more than a decade
and its code style has evolved. 

Code snippets that do no follow the present style guide can be refactored on 
sight, as the contributors see fit. Since the project is mostly self-contained, 
this mostly boils down to string replacement.

All code style guides not specified here are subject to change and 
uniformization.

## Code formatting automation tools


As of February 28, 2022, we have a working end-to-end self-contained source 
code formatter. To format every `.cpp` and `.h` file in the src/ directory, 
use the command:

```
calculator format
```

If the source code formatter fails to parse one of your files, it will not 
overwrite your old code, but write a `.fail` file with a partially parsed file.
Our source code formatter is semantic and will parse your source code into an 
abstract code tree, so it will fail to format your code if you have unbalanced 
parentheses, etc.
The source code formatter is located in
[source_code_formatter.cpp](src/source_code_formatter.cpp). If you think you 
can improve it, you are welcome to do so.

## Variable and class names

### CammelCase
Class names use UpperCamelCase (java style). Methods, functions, variables use 
lowerCamelCase (java style).

### Abbreviations

The rules for abbreviations are very simple. 

Do not use abbreviations.

Try to follow this rule down to common abbreviations: for example, 
prefer maximum over max. Common sense exceptions are allowed, but discouraged, 
and may be refactored away. Older code may use abbreviations; these are being 
refactored away.

Avoid use of non-meaningful variable names:

```
int x;
int cc;
int j
```

Exceptions are made for simplest of use cases. A somewhat random guideline: if 
a variable's use takes more than 10 lines of code, or is used in different 
syntactic scopes, then it need a proper descriptive name.

OK:
```
int sum = 0;
for (int i = 0; i < 11; i ++) {
  sum += i * i;
}
```

Not OK (the variable s participates in non-nested syntax scopes):
``` 
int s = 0;
for (int i = 0; i < 11; i ++) {
  s += i * i;
}
for (int i = 30; i < 50; i ++) {
  s += i * i;
}
```

OK:
``` 
int sumOfSquares = 0;
for (int i = 0; i < 11; i ++) {
  sumOfSquares += i * i;
}
for (int i = 30; i < 50; i ++) {
  sumOfSquares += i * i;
}
```
## Braces, blocks, tabs

### Tabs
Tabs are two characters. Do not use the tab character, use spacebar instead.

### Curly braces
To make matters simple, use go-style / java style / javascript style braces 
everywhere:
```
void someFunction() {
  // ...
}
```

### No implied blocks
Do not use:
```
if (condition)
  doSomething();
```
Use instead:
```
if (condition) {
  doSomething();
}
```
Originally the project used the first style exclusively, and required that 
one-line blocks be converted to the no-brace style. It worked without causing 
any problems. However, according to the present author's personal unconfirmed 
opinion at the time of writing, most code style guides "out there" require putting 
braces as above. 

The present rule was therefore introduced solely in the name of conformity.

## Operators and spacing.
Put white space around every mathematical operator:

``` 
a = b * c + d;
x |= y;
c ++;
a = b;
```
To keep it simple, follow this rule everywhere, including in for loops, default 
function arguments, etc.

## Pointers
Use nullptr for null pointers, else a number of IDEs complain.


