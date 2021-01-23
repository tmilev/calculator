# Examples

The calculator is a substitution engine and does not distinguish between "operators" and "regular functions", except for the different syntax with which these are entered. In the calculator, the expression "a+b" is equivalent to "add(a,b)". 

Every operator/function in the calculator corresponds to one or more internal function, which we call handlers. For example, the function/operator "a+b" can be handled by more than 40 different handlers. 

Each handler can apply by substituting the expression "a+b" with a new one, or leave the expression intact. If multiple handlers are applicable, the one that is listed first in the list below is applied.

The calculator has built-in syntax for allowing the user to add their own substitution rules to the built-in handlers, as well as to turn off any of the built-in handlers.


## Detecting "infinite" substutions.
When substituting expressions, is it possible to get an "infinite" substitution cycle? The built-in calculator rules are designed so as to never generate such cycles, with the reader's understanding that in practice this is subject to software bugs. 


However, since we allow arbitrary user modifications of our rules, it is guaranteed - both in practice and in theory - that the user can trigger an "infinite" substitution cycle. In fact, there are examples of infinite substitution cycles in the documentation of the function handlers. It is also possible to code a non-cyclical substitution that generates an ever-growing ("infinite") expression. The engine deals with this in two ways. 

- The engine caches all expressions generated in the past, up to a computational threshold, and, using the cache, can detect cycles. This can detect both types of cycles: ever-growing and finite ones, where we note that every infinite substitution falls into one of these two categories. The cache is also used to speed up substitutions of expressions that have previously been "reduced".

- The engine has computational timeouts and RAM memory limits.

-- If a first, "soft" timeout is hit (less than 10 seconds, depending on configuration), the calculator will continue the computation, but will return to the user with an explanation and a handle to monitor the so queued long computation.

-- If a larger "hard" timeout is hit (usually between 30-100 seconds, depending on configuration), the calculator will crash gracefully, with a reasonable progress report of how far it got. This timeout can be turned off entirely for the purpose of carrying out scientific computations. Of course, turning off the calculator hard is not recommended for a public server. 

-- There are various RAM memory and stack depth limits. Hitting any of these will cause the calculator to crash with a reasonable explanation of its internal state.


## List of handlers
Below, we list all built-in calculator commands. The command is used to tell the calculator to carry out a given computation, with unspecified syntax. 

The next item in [] brackets is the calculator name of the handler, distinguishing between different overloaded commands bound to the same symbol. This can be used to turn on/turn off the handler (use TurnOnRules/TurnOffRules).

The entry in {} braces is a name of the C++ function where the handler is implemented. This can be used to easily search the source code of the calculator.

### Syntax is not fixed in stone 
In this section, we use the term "syntax" to refer to the translation of a sequence of characters to an internal tree-based Expression data structure.

- In the terminology of our handlers, the string "a+b" corresponds to the command "+" with inputs "a" and "b". These are in turn handled by one of 40+ handlers, of which the first applicable one is chosen. Each of these handlers is implemented by a C++ function that takes as input the calculator expressions "+", "a" and "b" (note the command name is passed to the C++ handler as part of the inputs).

- Likewise, the string "Polynomialize((x+1)^2)" corresponds to the command "Polynomialize" with input "(x+1)^2". 

Note that the two examples given here have different syntax; this is so by design. Our syntax is not fixed in stone but in practice is very stable for commonly used expressions, as it is built on top of a subset of LaTeX. 


### Auto-generated list of handlers.

${content-inserted-by-calculator-in-Calculator::Examples::writeExamplesReadme}
