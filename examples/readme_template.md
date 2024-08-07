# Examples

The calculator is a substitution engine and does not distinguish between "operators" and "regular functions" except for the different syntax with which these are entered. For example, the expressions "a+b" and "add(a,b)" are equivalent. 

Calculator functions come in two flavors: user-defined expression substitutions and built-in functions. Every built in function/operator corresponds to one or more internal C++ functions ("handlers"). For example, the function/operator "a+b" can be handled by more than 40 different such functions. Each handler either substitutes the expression "a+b" with a new one, or leaves it intact. When multiple handlers are available, the first applicable one is used (here, applicable = actually changes the expression). The order in which the handlers are applied is the same as the order in the documentation below.

In addition to the built-in functions, the users can dynamically add their own substitution rules. Users can also turn on and off any of the built-in handlers.


## Detecting "infinite" substutions.
When substituting expressions, is it possible to get an "infinite" substitution cycle? The built-in calculator rules are designed so as to never generate such cycles, with the reader's understanding that in practice this is subject to software bugs. 


However, since we allow arbitrary user modifications of our rules, it is guaranteed - both in practice and in theory - that the user can trigger an "infinite" substitution cycle. In fact, there are examples of infinite substitution cycles in the documentation of the function handlers. It is also possible to code a non-cyclical substitution that generates an ever-growing ("infinite") expression. The engine deals with this in two ways. 

- The engine caches all expressions generated in the past, up to a computational threshold, and, using the cache, can detect cycles. This can detect both types of cycles: ever-growing and finite ones, where we note that every infinite substitution falls into one of these two categories. The cache is also used to speed up substitutions of expressions that have previously been "reduced".

- The engine has computational timeouts and RAM memory limits.

    -- If a first "soft" timeout is hit (less than 10 seconds, depending on configuration), the calculator will continue the computation, but will return to the user with an explanation and a handle to monitor the so queued long computation.

    -- If a larger "hard" timeout is hit (usually between 30-100 seconds, depending on configuration), the calculator will crash gracefully, with a reasonable progress report of how far it got. This timeout can be turned off entirely for the purpose of carrying out scientific computations. Of course, turning off the hard timeout is not recommended for a public server. 

    -- There are various RAM memory and stack depth limits. Hitting any of these will cause the calculator to crash with a reasonable explanation of its internal state.


## Syntax is not fixed in stone 
By "syntax", we mean the translation of a sequence of characters to an internal tree-based Expression data structure.

- In the terminology of our handlers, the string "a+b" corresponds to the command "+" with inputs "a" and "b". These are in turn handled by one of 40+ handlers, of which the first applicable one is chosen. Each of these handlers is implemented by a C++ function that takes as input the calculator expressions "+", "a" and "b" (note the command name is passed to the C++ handler as part of the inputs).

- Likewise, the string "Polynomialize((x+1)^2)" corresponds to the command "Polynomialize" with input "(x+1)^2". The actual C++ handler takes as input both the expression "Polynomialize" and the expression "(x+1)^2".

Note that these two examples have different syntax, but very similar internal representations. This is so by design. Our syntax is designed to follow a small subset of LaTeX, which in turn follows mathematical conventions rather than computer science ones. In practice our syntax is very stable for commonly used expressions (fractions, integrals, exponents, etc.). 

## List of handlers
Below, we list all built-in calculator handlers ("commands", "operators"). For each handler, the first entry is the name of the command.

The next item, shown in [] brackets is the calculator name of the handler, distinguishing between different overloaded commands bound to the same symbol. This can be used to turn on/turn off the handler (use TurnOnRules/TurnOffRules).

The entry in {} braces is the name of the C++ function where the handler is implemented. This can be used to easily search the source code of the calculator.

Some functions are marked as "admin only". These are only available to administrator accounts: you will need to install the calculator on your own machine to have the permission to run them. Some functions are marked as "invisible". These are experimental functions not intended for public consumption which may or may not require admin access.

### Auto-generated list of handlers.

${content-inserted-by-calculator-in-Calculator::Examples::writeExamplesReadme}
