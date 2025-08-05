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



*=* [CheckAutoEquality] {CalculatorBasics::checkRule}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25LogEvaluation%5cn%28%7b%7ba%7d%7d%2a%7b%7bb%7d%7d%29%2a%7b%7bc%7d%7d%3da%2a%28b%2ac%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%LogEvaluation
({{a}}*{{b}})*{{c}}=a*(b*c);
```
Checks whether the rule is of the form A = A, and substitutes the expression with an error if that is the case. This usually happens when a general rule is entered twice. In the following example, we try to redefine the associative rule of the calculator. This fails because the associative rule is already implemented: the left hand side of the below expression is substituted with a*(b*c), and thus the rule becomes a*(b*c)=a*(b*c), which clearly is infinite substitution.

*;* [MeltBrackets] {CalculatorBasics::outerMeltBrackets}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22c%20%3d%20%28a%20%3db%29%3b%5cna%3b%5cnc%3b%5cna%3b%5cnd%20%3d%20%28e%20%3d%20f%3b%20g%3dh%29%3b%5cnd%3b%5cne%3b%5cnMelt%7b%7dd%3b%5cne%3b%5cng%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
c = (a =b);
a;
c;
a;
d = (e = f; g=h);
d;
e;
Melt{}d;
e;
g;
```
Melts down a layer of parenthesis around a list of commands prepended with the Melt operation.
 More precisely, cycles through all immediate children of the expression.
If the k^th child X is a list of two elements starting with Melt, then the operation does the following. If the second child Y of X is a list starting with EndStatement(;), then X is replaced with the second, third, ... children of Y. If Y is not a list starting with EndStatement, X is replaced with Y.

Operator or function \+ is overloaded with 45 total handlers.

*\+* [TrigSumToTrigProduct] {CalculatorFunctionsTrigonometry::trigonometricSumToTrigonometricProduct}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22TrigSumToTrigProduct%5c%22%29%3b%5cnsin%28a%29%20%2b%20sin%28b%29%3b%5cnsin%28b%29%20-%20sin%28a%29%3b%5cn-%20sin%28b%29%20%2b%20sin%28a%29%3b%5cn-%20sin%28b%29%20-%20sin%28a%29%3b%5cncos%28a%29%20%2b%20cos%28b%29%3b%5cncos%28a%29%20-%20cos%28b%29%3b%5cn-%20cos%28a%29%20-%20cos%28b%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("TrigSumToTrigProduct");
sin(a) + sin(b);
sin(b) - sin(a);
- sin(b) + sin(a);
- sin(b) - sin(a);
cos(a) + cos(b);
cos(a) - cos(b);
- cos(a) - cos(b)
```
Implementation of sum-to-product trig rules. 

*\+* [AddStrings] {CalculatorFunctionsBinaryOps::addStringToString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%22hello%20%5c%22%2b%20%5c%22world%5c%22%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
"hello "+ "world"
```
Concatenates strings. 

*\+* [AddRationals] {CalculatorFunctionsBinaryOps::addRationalToRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%20%2b%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2 + 3
```
Adds two rational numbers. 

*\+* [AddRationalToElementZmodP] {CalculatorFunctionsBinaryOps::addElementZModPOrRationalToElementZModPOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%20mod%207%29%20%2b%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2 mod 7) + 3
```
Adds elements of Z_p. 

*\+* [AddElementZmodPToRational] {CalculatorFunctionsBinaryOps::addElementZModPOrRationalToElementZModPOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%20mod%207%29%20%2b%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2 mod 7) + 3
```
Adds elements of Z_p. 

*\+* [AddElementZmodPToElementZmodP] {CalculatorFunctionsBinaryOps::addElementZModPOrRationalToElementZModPOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%20mod%207%29%20%2b%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2 mod 7) + 3
```
Adds elements of Z_p. 

*\+* [AddPolynomialZmodPToPolynomialZmodP] {CalculatorFunctionsBinaryOps::addElementZModPOrRationalToElementZModPOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20PolynomialModP%28x%5e2%20%2b%20x%20%2b%207%2c%205%29%3b%5cnb%20%3d%20PolynomialModP%28x%5e2%20%2b%201%2c%205%29%3b%5cna%20%2b%20b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = PolynomialModP(x^2 + x + 7, 5);
b = PolynomialModP(x^2 + 1, 5);
a + b
```
Adds polynomials modulo integers. 

*\+* [AddPolynomialZmodPToInteger] {CalculatorFunctionsBinaryOps::addPolynomialModuloIntegerToInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20PolynomialModP%28x%5e2%20%2b%20x%20%2b%207%2c%205%29%3b%5cnb%20%3d%203%3b%5cna%20%2b%20b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = PolynomialModP(x^2 + x + 7, 5);
b = 3;
a + b
```
Adds polynomials modulo integers. 

*\+* [AddPolynomialModPolynomialModPToPolynomialModPolynomialModP] {CalculatorFunctionsBinaryOps::addPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22p%20%3d%20PolynomialModP%28x%5e2%20%2b%20x%20%2b%207%2c%205%29%3b%5cna%20%3d%20PolynomialModP%28x%5e2%20%2b%201%2c%205%29%3b%5cnb%20%3d%20PolynomialModP%28x%5e2%2c%205%29%3b%5cna%20mod%20p%3b%5cnb%20mod%20p%3b%5cna%2bb%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
p = PolynomialModP(x^2 + x + 7, 5);
a = PolynomialModP(x^2 + 1, 5);
b = PolynomialModP(x^2, 5);
a mod p;
b mod p;
a+b
```
Adds polynomials modulo integers. 

*\+* [AddAlgebraicNumberToAlgebraicNumber] {CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%20%7b2%7d%2b%20%5c%5csqrt%20%7b3%7d%20%2b%20%5c%5csqrt%7b6%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt {2}+ \sqrt {3} + \sqrt{6}
```
Adds two algebraic numbers. 

*\+* [AddAlgebraicNumberToRational] {CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2f%28%5c%5csqrt%20%7b2%7d%2b%201%2b%5c%5csqrt%7b3%7d%2b%5c%5csqrt%7b6%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1/(\sqrt {2}+ 1+\sqrt{3}+\sqrt{6})
```
Adds algebraic number to rational. 

*\+* [AddRationalToAlgebraicNumber] {CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2f%281%2b%5c%5csqrt%20%7b2%7d%2b%5c%5csqrt%7b%7d6%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1/(1+\sqrt {2}+\sqrt{}6)
```
Adds rational to algebraic number. 

*\+* [AddDoubleToRational] {CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%283.14159265358979323846%29%20%2b%201%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(3.14159265358979323846) + 1;

```
Adds double or rational to a double or rational approximately using the built-in cpp addition, returning double. 

*\+* [AddPlots] {CalculatorFunctionsBinaryOps::addPlotToPlot}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Plot2D%7b%7d%28sin%7b%7d%28x%29%2c%20-5%2c%205%29%20%2b%20Plot2D%7b%7d%281%2fsin%7b%7d%28x%20%29%2c%200.1%2c%203.041592654%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Plot2D{}(sin{}(x), -5, 5) + Plot2D{}(1/sin{}(x ), 0.1, 3.041592654)
```
Superimposes two plots. 

*\+* [SortTerms] {CalculatorFunctions::sortTerms}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%2b3%2ba%2b2a%2bb%2b1%2ba%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2+3+a+2a+b+1+a
```
Sorts terms (over the rationals). Similar to AddTerms but doesn't combine monomial coefficients or drop zeroes. 

*\+* [AddTerms] {CalculatorFunctions::addTerms}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2ba-2a_1%2b%201%2f2%2ba_1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1+a-2a_1+ 1/2+a_1
```
Collects all terms (over the rationals), adding up terms proportional up to a rational number. Zero summands are removed, unless zero is the only term left. 

*\+* [CommonDenominator] {CalculatorFunctionsPolynomial::combineFractionsCommutativeWithInternalLibrary}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%2fb%2bc%2fd%3b%5cnz%3d%28x-2%29%28x%2b1%29%3b%5cnw%3d%28x-3%29%28x%2b1%29%3b%5cn1%2fz%2b1%2fw%3b%5cn1%2f%28x%2bsqrt%2820%29%29%2b%201%2f%28x%2bsqrt%283%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a/b+c/d;
z=(x-2)(x+1);
w=(x-3)(x+1);
1/z+1/w;
1/(x+sqrt(20))+ 1/(x+sqrt(3));
```
Combines fractions under the assumption that all participants commute. Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= (a * lcm(b,d)/b+c*lcm(b,d)/d)/(lcm(b,d)); Please note that this transformation is not correct if b and d do not commute. 

*\+* [CommonDenominatorSimple] {CalculatorFunctions::combineFractionsCommutative}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22CommonDenominator%5c%22%29%3b%5cna%2fb%2bc%2fd%3b%5cnz%3d%28x-2%29%28x%2b1%29%3b%5cnw%3d%28x-3%29%28x%2b1%29%3b%5cn1%2fz%2b1%2fw%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("CommonDenominator");
a/b+c/d;
z=(x-2)(x+1);
w=(x-3)(x+1);
1/z+1/w
```
Combines fractions on condition that all participants commute. Similar to the CommonDenominator rule but does not compute least common multiples.Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= (a *d+c*b)/(d*b); Please note that this transformation is not correct if b and d do not commute. 

*\+* [CommonDenominatorOneNonFraction] {CalculatorBasics::combineFractions}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22f%7b%7d%7b%7bx%7d%7d%3d%20%282x%20%2b3%29%2f%282x%20%2b%201%29%3b%5cng%7b%7d%7b%7by%7d%7d%3d%20%28y-2%29%2f%28y%2b3%29%3b%5cng%7b%7df%7b%7dz%3b%5cnf%7b%7dg%7b%7dz%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
f{}{{x}}= (2x +3)/(2x + 1);
g{}{{y}}= (y-2)/(y+3);
g{}f{}z;
f{}g{}z
```
Combines fractions. Equivalent to {{a}}/{{b}}+{{c}}= (a +c*b)/b; 

*\+* [AddRationalToPolynomial] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2bPolynomial%7b%7d%5c%5clambda%3b%5cnPolynomial%7b%7d%5c%5clambda%20%2b%201%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1+Polynomial{}\lambda;
Polynomial{}\lambda + 1
```
Adds a rational to a polynomial. 

*\+* [AddPolynomialToRational] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2bPolynomial%7b%7d%5c%5clambda%3b%5cnPolynomial%7b%7d%5c%5clambda%20%2b%201%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1+Polynomial{}\lambda;
Polynomial{}\lambda + 1
```
Adds a polynomial to a rational. 

*\+* [AddPolynomialToPolynomial] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%201%20%2b%20Polynomial%7b%7d%20%5c%5clambda%3b%5cnx%20%2b%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = 1 + Polynomial{} \lambda;
x + x
```
Adds a polynomial to a polynomial. 

*\+* [AddPolynomialOverAlgebraicNumbersToPolynomialOverAlgebraicNumbers] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%28%5c%5csqrt%7b12%7dx%29%20%2b%20PolynomialAlgebraicNumbers%28-%5c%5csqrt%7b3%7dx%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers(\sqrt{12}x) + PolynomialAlgebraicNumbers(-\sqrt{3}x)
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddAlgebraicNumberToPolynomialOverAlgebraicNumbers] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%28%5c%5csqrt%7b12%7d%29%20%2b%20PolynomialAlgebraicNumbers%28%5c%5csqrt%7b3%7dx%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers(\sqrt{12}) + PolynomialAlgebraicNumbers(\sqrt{3}x)
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddPolynomialOverAlgebraicNumbersToAlgebraicNumber] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%28%5c%5csqrt%7b12%7dx%29%20%2bPolynomialAlgebraicNumbers%28-%5c%5csqrt%7b3%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers(\sqrt{12}x) +PolynomialAlgebraicNumbers(-\sqrt{3})
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddRationalToPolynomialAlgebraicNumbers] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%20%2b%20PolynomialAlgebraicNumbers%28%5c%5csqrt%7b12%7dx%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1 + PolynomialAlgebraicNumbers(\sqrt{12}x)
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddPolynomialAlgebraicNumbersToRational] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%28%5c%5csqrt%7b12%7dx%29%20%2b%201%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers(\sqrt{12}x) + 1
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddPolynomialToAlgebraicNumber] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%28x%29%20%2b%5c%5csqrt%7b2%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers(x) +\sqrt{2}
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddPolynomialToPolynomialAlgebraicNumbers] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7dx%20%2b%20PolynomialAlgebraicNumbers%28%5c%5csqrt%7b2%7dx%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}x + PolynomialAlgebraicNumbers(\sqrt{2}x)
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddPolynomialAlgebraicNumbersToPolynomial] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%28%5c%5csqrt%7b2%7dx%29%20%2b%20Polynomial%7b%7dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers(\sqrt{2}x) + Polynomial{}x
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddAlgebraicNumberToPolynomial] {CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%7b2%7d%2bPolynomialAlgebraicNumbers%28x%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt{2}+PolynomialAlgebraicNumbers(x)
```
Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. 

*\+* [AddRationalFunctionToRationalFunction] {CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrRationalFunctionToRationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylDimFormula%7b%7d%28a_2%2c%20%280%2c3%29%29%20%2b%20WeylDimFormula%7b%7d%28a_2%2c%20%283%2c0%29%29%20%2b%204%20WeylDimFormula%7b%7d%28a_2%2c%20%281%2c1%29%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylDimFormula{}(a_2, (0,3)) + WeylDimFormula{}(a_2, (3,0)) + 4 WeylDimFormula{}(a_2, (1,1)) 
```
Adds a rational function to a rational function. 

*\+* [AddRationalToDouble] {CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%283.14159265358979323846%29%20%2b%201%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(3.14159265358979323846) + 1
```
Adds double or rational to a double or rational approximately using the built-in cpp addition, returning double. 

*\+* [AddDoubleToDouble] {CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%283.14159265358979323846%29%20%2b%201%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(3.14159265358979323846) + 1
```
Adds double or rational to a double or rational approximately using the built-in cpp addition, returning double. 

*\+* [AddMatrices] {CalculatorFunctionsBinaryOps::addMatrixToMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cbegin%7bpmatrix%7d%201%26%202%20%5c%5c%5c%5c%202%20%26%203%5c%5cend%7bpmatrix%7d%20%2b%20%5cn%5c%5cbegin%7bpmatrix%7d%201%26%203%20%5c%5c%5c%5c%202%20%26%203%5c%5cend%7bpmatrix%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\begin{pmatrix} 1& 2 \\ 2 & 3\end{pmatrix} + 
\begin{pmatrix} 1& 3 \\ 2 & 3\end{pmatrix}
```
Adds two matrices.

*\+* [AddSequences] {CalculatorFunctionsBinaryOps::addSequenceToSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22v_%7b1%7d%3d%20%281%2c%202%2c%203%29%3b%5cnv_%7b2%7d%3d%20%281%2c%203%2c%202%29%3b%5cnv_%7b3%7d%3d%20%283%2c%201%2c%201%29%3b%5cnv_%7b4%7d%3d%20%28-2%2c%202%2c%202%29%3b%5cn1%2f2v_%7b1%7d%2b%201%2f2v_%7b2%7d%2b7%2f8v_%7b3%7d%2b%2013%2f16v_%7b4%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
v_{1}= (1, 2, 3);
v_{2}= (1, 3, 2);
v_{3}= (3, 1, 1);
v_{4}= (-2, 2, 2);
1/2v_{1}+ 1/2v_{2}+7/8v_{3}+ 13/16v_{4}
```
Adds two sequences (termwise), provided the sequences have the same number of entries. In case the entries of the sequences are elements of a base field, corresponds to vector addition.

*\+* [AddMatrixTensorToMatrixTensor] {CalculatorFunctionsBinaryOps::addMatrixTensorToMatrixTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20MakeMatrixTensorForm%7b%7d%28%285%2c%208%29%2c%20%283%2c%205%29%29%3b%5cn3A%20%2a%20A%20-%20A%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = MakeMatrixTensorForm{}((5, 8), (3, 5));
3A * A - A;
```
Adds two matrices.

*\+* [AddRationalToElementWeylAlgebra] {CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%20%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cnx_i%5c%5cpartial_i-%5c%5cpartial_i%20x_i-%5bx_i%2c%20%5c%5cpartial_i%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
 \partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
x_i\partial_i-\partial_i x_i-[x_i, \partial_i]
```
Adds a rational or polynomial to element Weyl algebra. 

*\+* [AddPolynomialToElementWeylAlgebra] {CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cnx_i%20%2b%5c%5cpartial_i%20%2bx_i%5c%5cpartial_i-%5c%5cpartial_i%20x_i-%5bx_i%2c%20%5c%5cpartial_i%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
x_i +\partial_i +x_i\partial_i-\partial_i x_i-[x_i, \partial_i]
```
Adds a rational or polynomial to element Weyl algebra. 

*\+* [AddElementWeylAlgebraToPolynomial] {CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%20%5c%5cpartial_%7b%7bi%7d%7d%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cnx_i%20%2bx_i%5c%5cpartial_i-%5c%5cpartial_i%20x_i-%5bx_i%2c%20%5c%5cpartial_i%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
 \partial_{{i}} = ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
x_i +x_i\partial_i-\partial_i x_i-[x_i, \partial_i]
```
Adds a rational or polynomial to element Weyl algebra. 

*\+* [AddElementWeylAlgebraToElementWeylAlgebra] {CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%20%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cnx_i%5c%5cpartial_i-%5c%5cpartial_i%20x_i-%5bx_i%2c%20%5c%5cpartial_i%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
 \partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
x_i\partial_i-\partial_i x_i-[x_i, \partial_i]
```
Adds a rational or polynomial to element Weyl algebra. 

*\+* [AddElementTensorGeneralizedVermaModuleToElementTensorGeneralizedVermaModule] {CalculatorFunctionsBinaryOps::addElementTensorToElementTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22v%3dHeighestWeightVector%7b%7d%28G_2%2c%20%281%2c0%29%2c%280%2c0%29%29%3b%5cn%283%2f4%20v%29%5c%5cotimes%20v-3%2f4%20%28v%5c%5cotimes%20v%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
v=HeighestWeightVector{}(G_2, (1,0),(0,0));
(3/4 v)\otimes v-3/4 (v\otimes v)
```
Adds two elements of tensor products of generalized Verma modules. 

*\+* [AddWeightLieAlgebraPolynomialToWeightLieAlgebraPolynomial] {CalculatorFunctionsBinaryOps::addWeightToWeight}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5clambda%20%3d%20Polynomial%7b%7d%5c%5clambda%3b%20%5c%5cvarepsilon_%7b%7ba%7d%7d%20%3d%20MakeWeight%7b%7d%28B_3%2c%20a%2c%20epsilon%29%3b%20%281%20%2f%202%20%2b%20%5c%5clambda%29%20%5c%5cvarepsilon_1%20%2b%201%20%2f%202%20%5c%5cvarepsilon_2%2b%201%20%2f%202%20%5c%5cvarepsilon_3%20%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\lambda = Polynomial{}\lambda; \varepsilon_{{a}} = MakeWeight{}(B_3, a, epsilon); (1 / 2 + \lambda) \varepsilon_1 + 1 / 2 \varepsilon_2+ 1 / 2 \varepsilon_3  
```
Adds two weights. 

*\+* [AddElementUniversalEnvelopingRationalFunctionCoefficientsToElementUniversalEnvelopingRationalFunctionCoefficients] {CalculatorFunctionsBinaryOps::addUniversalEnvelopingAlgebraElementToAny}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7b%7bi%7d%7d%7d%20%3d%20ChevalleyGenerator%7b%7d%28F_%7b1%7d%2c%20%7b%7bi%7d%7d%29%3b%5cnh_%7b%7b%7bi%7d%7d%7d%20%3d%20CartanGenerator%7b%7d%28F_%7b1%7d%2c%20%7b%7bi%7d%7d%29%3b%5cn%5bg_%7b22%7d%2bg_%7b20%7d%2bg_%7b14%7d%2cg_%7b-%2014%7d%2bg_%7b-20%7d%2bg_%7b-22%7d%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{{i}}} = ChevalleyGenerator{}(F_{1}, {{i}});
h_{{{i}}} = CartanGenerator{}(F_{1}, {{i}});
[g_{22}+g_{20}+g_{14},g_{- 14}+g_{-20}+g_{-22}]
```
Adds an element of UE (Universal Enveloping algebra) to an element of UE.

*\+* (_composite_) [CompositeArithmeticOperationEvaluatedOnArgumentAddition] {CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%20%2bb%29%7b%7dx%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a +b){}x;
```
Equivalent to (a +b){}x = (a{}x) +(b{}x) 

*\-* [Minus] {CalculatorBasics::minus}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22-%201%20%2b%20%28-%205%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
- 1 + (- 5)
```
Transforms a - b to a +(- 1) * b and - b to (- 1) * b. Equivalent to a rule -{{b}}=MinusOne * b;
{{a}}-{{b}}=a + MinusOne * b;


Operator or function / is overloaded with 27 total handlers.

*/* [SumProductNotationToOperatorRelativeToDivision] {CalculatorFunctions::sumTimesExpressionToSumOf}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotExpressionTree%28%20%5c%5csum%5c%5climits_%7bb%7d%5ec%29%3b%5cnPlotExpressionTree%28%20%5c%5csum%5c%5climits_%7bb%7d%5ec%2aa%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotExpressionTree( \sum\limits_{b}^c);
PlotExpressionTree( \sum\limits_{b}^c*a) 
```
Transforms \sum\limits_{b}^c* a to (\sum\limits_b^c){} a. 

*/* [IntegralOperatorFromQuotient] {CalculatorFunctionsIntegration::integralOperator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20x%20dx%3b%5cn%5c%5cint%20x%20%281%2bx%29%20dx%3b%5cn%5c%5cint_2%5e3%20x%20dx%3b%5cn%5c%5cint_2%5e3%20x%281%2bx%29dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int x dx;
\int x (1+x) dx;
\int_2^3 x dx;
\int_2^3 x(1+x)dx
```
Transforms integral notation into an integral expression. 

*/* [DivideElementZmodPByElementZModP] {CalculatorFunctionsBinaryOps::divideElementZmodPorRationalByElementZmodPorRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%20mod%20%207%29%20%2f%20%283%20mod%207%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2 mod  7) / (3 mod 7)
```
Divides elements of Z/pZ. 

*/* [LeibnizDifferentialOperatorToDifferentialOperator] {CalculatorFunctionsDifferentiation::diffdivDiffxToDifferentiation}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differential%20%2fDifferential%20%7b%7dx%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differential /Differential {}x x
```
Replaces Differential / Differential {}x by Differentiate{}(x). Should also work on the notation (Differential y)/(Differential x). 

*/* [LeibnizDifferentialOperatorNotation] {CalculatorFunctionsDifferentiation::ddivDxToDiffDivDiffx}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22d%2fdx%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
d/dx x
```
Replaces d/dx by Differential /Differential{}x. Note that the variable of differentiation is expected to be the string following the d letter. 

*/* [DivideAlgebraicByAlgebraic] {CalculatorFunctionsBinaryOps::divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2f%281%2b%5c%5csqrt%7b%7d2%2b%5c%5csqrt%7b%7d3%2b%5c%5csqrt%7b%7d5%2b%5c%5csqrt%7b%7d7%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1/(1+\sqrt{}2+\sqrt{}3+\sqrt{}5+\sqrt{}7)
```
Divides algebraic numbers. 

*/* [DivideRationalByAlgebraic] {CalculatorFunctionsBinaryOps::divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%221%2f%28%5c%5csqrt%7b%7d2%2b%5c%5csqrt%7b%7d3%2b%5c%5csqrt%7b%7d5%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
1/(\sqrt{}2+\sqrt{}3+\sqrt{}5)
```
Divides rational by algebraic number. 

*/* [DivideAlgebraicByRational] {CalculatorFunctionsBinaryOps::divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28%5c%5csqrt%7b%7d2%2b%5c%5csqrt%7b%7d3%2b%5c%5csqrt%7b%7d5%29%2f5%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(\sqrt{}2+\sqrt{}3+\sqrt{}5)/5
```
Divides algebraic number by rational. 

*/* [DivideByNumber] {CalculatorFunctions::divideByNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%226%2f15%2b%28a%20%2bb%29%2f5%3b%5cna%2f%5c%5csqrt%7b%7d2%3b%5cnx%2fDoubleValue%7b%7d10%5e10%3b%5cnx%2fDoubleValue%7b%7d5%3b%5cn6%2f4%2b3%2f0%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
6/15+(a +b)/5;
a/\sqrt{}2;
x/DoubleValue{}10^10;
x/DoubleValue{}5;
6/4+3/0
```
If b is rational, algebraic, or double, substitutes (anything)/b with anything* (1/b).

*/* [DivideByNumberTrivial] {CalculatorEducationalFunctions::divideByNumberTrivial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25LogEvaluation%5cnTurnOnRules%28DivideByNumberTrivial%29%3b%5cnTurnOffRules%28DivideByNumber%2c%20ConvertShortDenominatorToNegativePower%2c%20DivideRationalByRational%29%3b%5cn4%2f6%3b%5cn2%2f3%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%LogEvaluation
TurnOnRules(DivideByNumberTrivial);
TurnOffRules(DivideByNumber, ConvertShortDenominatorToNegativePower, DivideRationalByRational);
4/6;
2/3
```
Intended for educational purposes. Divides rationals only if the operation is trivial, such as in(2) /(3) --> (2/3). The rule is turned off by default.

*/* [DivideZeroDivideByAnything] {CalculatorBasics::subZeroDivAnythingWithZero}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%220%2fb%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
0/b; 
```
Provided that x is not equal to zero, substitutes 0/x with 0. 

*/* [DivideCancellations] {CalculatorFunctions::divisionCancellations}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2fb%29%2f%28a%2fd%29%3b%5cn%28a%2fb%29%2f%28c%2fb%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a/b)/(a/d);
(a/b)/(c/b)
```
Division cancellations. Substitutes (a/b)/(a/d) with d/a and (a/b)/(c/b) with a/c. 

*/* [AssociateDivisionDivision] {CalculatorFunctions::associateDivisionDivision}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2fb%29%2fc%3b%5cna%2f%28b%2fc%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a/b)/c;
a/(b/c);
```
Substitutes (a/b)/c =a/(c*b); a/(b/c)=a*c/b. Note the order of multiplication in the rules: this operation is safe and correct for non-commutative rings as well.

*/* [AssociateAdividedByBdividedByCpowerD] {CalculatorFunctions::associateAdivBdivCPowerD}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%2f%28b%2fc%29%5ed%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a/(b/c)^d;
```
Substitutes a/(b/c)^d =a c^d/b^d.Note the order of multiplication in the rules: this operation is safe and correct for non-commutative rings as well.

*/* [ConvertShortDenominatorToNegativePower] {CalculatorFunctions::atimesBpowerJplusEtcDivBpowerI}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%20x%5e%7b1%2f2%7d%20%2b%20b%20x%20%29%2fx%3b%5cn%28a%20x%5e%7b1%2f2%7d%20%2b%20b%20x%20%29%2fx%5e2%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a x^{1/2} + b x )/x;
(a x^{1/2} + b x )/x^2;
```
Rule: (a_0 + a_1 x^{c_1}+ ... + a_n x^{c_n}) /x^t =a_0 x^{-t}+ a_1 x^{c_1-t}+...+a_n x^{c_n-t} 

*/* [DivideRationalByRational] {CalculatorFunctionsBinaryOps::divideRationalByRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%224%2f6%3b%5cn2%2f0%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
4/6;
2/0;
```
Divides two rational numbers. 

*/* [DivideRationalByDouble] {CalculatorFunctionsBinaryOps::divideDoubleByDouble}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%200.5%3b%5cnb%3d%200.5%3b%5cnc%20%3d%20DoubleValue%7b%7d3.3%3b%5cna%2fc%3b%5cnc%2fa%3b%5cnc%2fc%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = 0.5;
b= 0.5;
c = DoubleValue{}3.3;
a/c;
c/a;
c/c
```
Divides doubles. 

*/* [DivideDoubleByDouble] {CalculatorFunctionsBinaryOps::divideDoubleByDouble}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%200.5%3b%5cnb%20%3d%200.5%3b%5cnc%20%3d%20DoubleValue%7b%7d3.3%3b%5cna%2fc%3b%5cnc%2fa%3b%5cnc%2fc%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = 0.5;
b = 0.5;
c = DoubleValue{}3.3;
a/c;
c/a;
c/c
```
Divides doubles. 

*/* [DivideRationalByPolynomial] {CalculatorFunctionsBinaryOps::divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22z%20%3d%20Polynomial%7b%7d%28x%5e2%2by%5e2%29%3b%5cn1%2fz%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
z = Polynomial{}(x^2+y^2);
1/z
```
Divides rational by polynomial (to get a rational function).

*/* [DividePolynomialByPolynomial] {CalculatorFunctionsBinaryOps::divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7d%28-x_%7b1%7d%5e%7b2%7dx_%7b2%7dx_%7b3%7d-x_%7b1%7d%5e%7b2%7dx_%7b3%7d%2bx_%7b2%7d%2b%201%29%2f%5cnPolynomial%7b%7d%28x_%7b1%7d%5e%7b2%7dx_%7b2%7dx_%7b3%7d-x_%7b1%7d%5e%7b2%7dx_%7b3%7d-x_%7b2%7d%2b%201%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}(-x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}+x_{2}+ 1)/
Polynomial{}(x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}-x_{2}+ 1) 
```
Divides polynomial by polynomial (to get a rational function). 

*/* [DivideRationalFractionByPolynomial] {CalculatorFunctionsBinaryOps::divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeRationalFunction%28x%29%2fPolynomial%7b%7d%28y%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeRationalFunction(x)/Polynomial{}(y) 
```
Divides rational fraction by polynomial (to get a rational fraction). 

*/* [DividePolynomialByRationalFraction] {CalculatorFunctionsBinaryOps::divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%28x%29%20%2f%5cnMakeRationalFunction%7b%7d%28y%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial(x) /
MakeRationalFunction{}(y) 
```
Divides rational fraction by polynomial (to get a rational fraction). 

*/* [DivideRationalFractionByRationalFraction] {CalculatorFunctionsBinaryOps::divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeRationalFunction%7b%7d%28x%29%20%2f%20MakeRationalFunction%7b%7d%28y%29%3b%5cnPolynomial%7b%7d%28x%29%20%2f%20MakeRationalFunction%7b%7d%28y%29%3b%5cnMakeRationalFunction%7b%7d%28x%29%20%2f%20Polynomial%7b%7d%28y%29%3b%5cnMakeRationalFunction%7b%7d%28x%29%20%2f%202%3b%5cn2%20%2f%20MakeRationalFunction%7b%7d%28y%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeRationalFunction{}(x) / MakeRationalFunction{}(y);
Polynomial{}(x) / MakeRationalFunction{}(y);
MakeRationalFunction{}(x) / Polynomial{}(y);
MakeRationalFunction{}(x) / 2;
2 / MakeRationalFunction{}(y)
```
Divides rational fraction by a rational fraction. 

*/* [DividePolynomialModPByPolynomialModP] {CalculatorFunctionsBinaryOps::dividePolynomialModuloIntegerByPolynomialModuloInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialModP%7b%7d%28x%5e5%2b5x%5e3%2bx%5e2%2b4x%2b1%2c%207%29%20%2f%20PolynomialModP%7b%7d%28x%5e5%2b6x%5e3%2bx%5e2%2b5x%2b1%2c%207%29%3b%5cnPolynomialModP%7b%7d%28x%5e3-x%2b1%2c%203%29%20%2f%20PolynomialModP%7b%7d%28x%5e2%2b2%2c%203%29%3b%5cnPolynomialModP%7b%7d%28x%5e3-x%2b1%2c%203%29%20%2f%20PolynomialModP%7b%7d%28x%5e2%2b2%2c%207%29%3b%5cnPolynomialModP%7b%7d%28x%5e3-x%2b1%2c%206%29%20%2f%20PolynomialModP%7b%7d%282x%5e2%2b2%2c%206%29%3b%5cnPolynomialModP%7b%7d%28x%5e3-x%2b1%2c%2021%29%20%2f%20PolynomialModP%7b%7d%282x%5e2%2b2%2c%2021%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialModP{}(x^5+5x^3+x^2+4x+1, 7) / PolynomialModP{}(x^5+6x^3+x^2+5x+1, 7);
PolynomialModP{}(x^3-x+1, 3) / PolynomialModP{}(x^2+2, 3);
PolynomialModP{}(x^3-x+1, 3) / PolynomialModP{}(x^2+2, 7);
PolynomialModP{}(x^3-x+1, 6) / PolynomialModP{}(2x^2+2, 6);
PolynomialModP{}(x^3-x+1, 21) / PolynomialModP{}(2x^2+2, 21);

```
Divides polynomial modulo integer by another polynomial modulo integer. 

*/* [DivideExpressionsAsIfPolynomial] {CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282x%5e2%2b3x-5%29%2f%285x%5e4%2bx%5e2%2b2x-8%29%3b%5cnx%5e2%2fx%3b%5cn%28x%2b1%29%5e2%2f%28x%2b1%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2x^2+3x-5)/(5x^4+x^2+2x-8);
x^2/x;
(x+1)^2/(x+1)
```
Assumes that the numerator and denominator of a fraction commute. Divides the two expressions under the assumption that both can be converted topolynomials with rational coefficients.

*/* [DivideSequenceByScalar] {CalculatorFunctionsBinaryOps::divideSequenceByScalar}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%281%2c2%29%2fx%3b%5cn%282%2c3%29%2fMakeInputBox%7b%7d%28name%3dx%2c%20value%3d2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(1,2)/x;
(2,3)/MakeInputBox{}(name=x, value=2)
```
Divides a sequence by a scalar. 

*/* (_composite_) [CompositeArithmeticOperationEvaluatedOnArgumentDivision] {CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2fb%29%7b%7dx%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a/b){}x;
```
Equivalent to (a/b){}x = (a{}x)/(b{}x) 

Operator or function \* is overloaded with 72 total handlers.

*\** [MultiplyMatrixByMatrix] {CalculatorFunctionsBinaryOps::multiplyMatrixByMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cbegin%7bpmatrix%7d%201%26%20%202%20%5c%5c%5c%5c%203%26%205%5c%5cend%7bpmatrix%7d%5c%5cbegin%7bpmatrix%7d%201%26%20-2%20%5c%5c%5c%5c%203%26%205%5c%5cend%7bpmatrix%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\begin{pmatrix} 1&  2 \\ 3& 5\end{pmatrix}\begin{pmatrix} 1& -2 \\ 3& 5\end{pmatrix}
```
Multiplies matrices.

*\** [MultiplySequenceByMatrix] {CalculatorFunctionsBinaryOps::multiplySequenceByMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%281%2c2%29%5c%5cbegin%7bpmatrix%7d%201%26%202%20%5c%5c%5c%5c%203%26%205%5c%5cend%7bpmatrix%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(1,2)\begin{pmatrix} 1& 2 \\ 3& 5\end{pmatrix}
```
Multiplies matrices.

*\** [MultiplySequenceByScalarLike] {CalculatorFunctionsBinaryOps::multiplySequenceByScalarLike}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%281%2c2%29sqrt%28x%2by%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(1,2)sqrt(x+y)
```
Multiplies a sequence standing on the left by a constant. 

*\** [MultiplyMatrixBySequence] {CalculatorFunctionsBinaryOps::multiplySequenceByMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%281%2c2%29%5c%5cbegin%7bpmatrix%7d%201%26%202%20%5c%5c%5c%5c%203%26%205%5c%5cend%7bpmatrix%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(1,2)\begin{pmatrix} 1& 2 \\ 3& 5\end{pmatrix}
```
Multiplies a matrix standing on the left by a sequence/list (matrix-row) standing on the right.
This operation is valid only if the the matrix on the right is a column-vector.

*\** [InterpretAsDifferential] {CalculatorFunctionsDifferentiation::interpretAsDifferential}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20x%20dx%3b%20%5c%5cint%20x%20%281%20%2b%20x%29%20dx%3b%20%5c%5cint_2%5e3%20x%20dx%3b%20%5c%5cint_2%5e3%20x%20%281%20%2b%20x%29%20dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int x dx; \int x (1 + x) dx; \int_2^3 x dx; \int_2^3 x (1 + x) dx
```
If contexts implies it, interprets an atom of the form dx as Differential {} x. 

*\** [IntegralOperatorFromProduct] {CalculatorFunctionsIntegration::integralOperator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20x%20dx%3b%5cn%5c%5cint%20x%20d%20x%3b%5cn%28%5c%5cint%20x%29%20dx%3b%5cn%5c%5cint%20x%20%281%2bx%29%20dx%3b%5cn%5c%5cint_2%5e3%20x%20dx%3b%5cn%5c%5cint_2%5e3%20x%281%2bx%29dx%3b%5cn%5c%5cint%5c%5ctheta%20d%5c%5ctheta%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int x dx;
\int x d x;
(\int x) dx;
\int x (1+x) dx;
\int_2^3 x dx;
\int_2^3 x(1+x)dx;
\int\theta d\theta
```
Transforms integral notation into an integral expression. 

*\** [SumProductNotationToOperator] {CalculatorFunctions::sumTimesExpressionToSumOf}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotExpressionTree%28%20%5c%5csum%5c%5climits_%7bb%7d%5ec%29%3b%5cnPlotExpressionTree%28%20%5c%5csum%5c%5climits_%7bb%7d%5ec%2aa%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotExpressionTree( \sum\limits_{b}^c);
PlotExpressionTree( \sum\limits_{b}^c*a) 
```
Transforms \sum\limits_{b}^c* a to (\sum\limits_b^c){} a. 

*\** [DifferentiateWithRespectToXTimeAny] {CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22d%2fdx%20%281%2fx%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
d/dx (1/x) 
```
Replaces Differentiate{}(x)*a by Differentiate{}(x, a).

*\** [MultiplyRationalByElementZmodP] {CalculatorFunctionsBinaryOps::multiplyElementZmodPorRationalByElementZmodPorRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%20mod%207%29%20%2a%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2 mod 7) * 3
```
Multiplies elements of Z_p. 

*\** [MultiplyElementZmodPByElementZModP] {CalculatorFunctionsBinaryOps::multiplyElementZmodPorRationalByElementZmodPorRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%20mod%207%29%20%2a%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2 mod 7) * 3
```
Multiplies elements of Z_p. 

*\** [MultiplyAlgebraicNumberByAlgebraicNumber] {CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%7b%7d2%28%5c%5csqrt%20%7b2%7d%2a%20%5c%5csqrt%20%7b3%7d%20%2b%5c%5csqrt%7b%7d2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt{}2(\sqrt {2}* \sqrt {3} +\sqrt{}2)
```
Multiplies two algebraic numbers. 

*\** [MultiplyRationalByAlgebraicNumber] {CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%28%5c%5csqrt%20%7b2%7d%2b%5c%5csqrt%7b%7d3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2(\sqrt {2}+\sqrt{}3)
```
Multiplies two algebraic number by rational. 

*\** [MultiplyRationals] {CalculatorFunctionsBinaryOps::multiplyRationalByRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%20%2a%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2 * 3
```
Multiplies two rationals. 

*\** [MultiplyRationalByDouble] {CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%281%2f3%29%2a3%3b%5cnz%3d101%5e20%3b%5cnDoubleValue%7b%7d%28z%29%3b%5cnDoubleValue%7b%7d%28z%29%20%2bDoubleValue%7b%7d%281%29-DoubleValue%7b%7d%28z%29%3b%5cn%28z%2b%201%29-z%3b%5cny%20%3d%20101%5e200%3b%5cnDoubleValue%28y%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(1/3)*3;
z=101^20;
DoubleValue{}(z);
DoubleValue{}(z) +DoubleValue{}(1)-DoubleValue{}(z);
(z+ 1)-z;
y = 101^200;
DoubleValue(y)
```
Multiplies rational by a double approximately using the built-in cpp multiplication returning double. The cpp multiplication is supposed to call the system's hardware double multiplication routine. 

*\** [MultiplyDoubleByRational] {CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%281%2f3%29%2a3%3b%5cnDoubleValue%7b%7d%28%28101%29%5e%7b20%7d%29%3b%5cnDoubleValue%7b%7d%28DoubleValue%7b%7d%28%28101%29%5e%7b20%7d%29%20%2b%20DoubleValue%7b%7d%281%29%29%20-%20DoubleValue%7b%7d%28101%5e%7b20%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(1/3)*3;
DoubleValue{}((101)^{20});
DoubleValue{}(DoubleValue{}((101)^{20}) + DoubleValue{}(1)) - DoubleValue{}(101^{20})
```
Multiplies rational by a double approximately using the built-in cpp multiplication returning double. The cpp multiplication is supposed to call the system's hardware double multiplication routine. 

*\** [MultiplyDoubleByDouble] {CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%281%2f3%29%2a3%3b%5cnDoubleValue%7b%7d%28%28101%29%5e%7b20%7d%29%3b%5cnDoubleValue%7b%7d%28DoubleValue%7b%7d%28%28101%29%5e%7b20%7d%29%20%2bDoubleValue%7b%7d%281%29%29-DoubleValue%7b%7d%28101%5e%7b20%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(1/3)*3;
DoubleValue{}((101)^{20});
DoubleValue{}(DoubleValue{}((101)^{20}) +DoubleValue{}(1))-DoubleValue{}(101^{20})
```
Multiplies rational by a double approximately using the built-in cpp multiplication returning double. The cpp multiplication is supposed to call the system's hardware double multiplication routine. 

*\** [MultiplyPolynomialByRational] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%2aPolynomial%7b%7d%28a%20%2bb%29%3b%5cnPolynomial%7b%7d%28a%20%2bb%29%2f2%3b%5cnPolynomial%7b%7d%28%28a%20%2bb%29%5e3%29%2aPolynomial%7b%7d%28%28a%20%2bc%29%5e3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2*Polynomial{}(a +b);
Polynomial{}(a +b)/2;
Polynomial{}((a +b)^3)*Polynomial{}((a +c)^3);
```
Multiplies polynomial by a rational (polynomial comes first). 

*\** [MultiplyRationalByPolynomial] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%2aPolynomial%7b%7d%28a%20%2b%20b%29%3b%5cnPolynomial%7b%7d%28a%20%2b%20b%29%2f2%3b%5cnPolynomial%7b%7d%28%28a%20%2b%20b%29%5e3%29%20%2a%20Polynomial%7b%7d%28%28a%20%2b%20c%29%5e3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2*Polynomial{}(a + b);
Polynomial{}(a + b)/2;
Polynomial{}((a + b)^3) * Polynomial{}((a + c)^3);
```
Multiplies rational by a polynomial (rational comes first). 

*\** [MultiplyPolynomialByPolynomial] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%2aPolynomial%7b%7d%28a%20%2bb%29%3b%5cnPolynomial%7b%7d%28a%20%2bb%29%2f2%3b%5cnPolynomial%7b%7d%28%28a%20%2bb%29%5e3%29%20%2a%20Polynomial%7b%7d%28%28a%20%2bc%29%5e3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2*Polynomial{}(a +b);
Polynomial{}(a +b)/2;
Polynomial{}((a +b)^3) * Polynomial{}((a +c)^3);
```
Multiplies two polynomials. 

*\** [MultiplyPolynomialAlgebraicNumbersByPolynomialAlgebraicNumbers] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%7b%7d%28%5c%5csqrt%7b3%7dx%29%2aPolynomialAlgebraicNumbers%7b%7d%28%5c%5csqrt%7b6%7dy%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers{}(\sqrt{3}x)*PolynomialAlgebraicNumbers{}(\sqrt{6}y);
```
Multiplies two polynomials over the algebraic numbers. 

*\** [MultiplyPolynomialModPByPolynomialModP] {CalculatorFunctionsBinaryOps::multiplyPolynomialModPByPolynomialModP}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20PolynomialModP%28%20x%5e2%20%2b%20x%20%2b%201%2c%207%20%29%3b%5cnb%20%3d%20PolynomialModP%28%20x%5e3%20%2b%20x%20%2b%201%2c%207%20%29%3b%5cna%20%2a%20b%3b%5cna%20%2a%20PolynomialModP%28%20x%5e3%20%2b%20x%20%2b%201%2c%205%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = PolynomialModP( x^2 + x + 1, 7 );
b = PolynomialModP( x^3 + x + 1, 7 );
a * b;
a * PolynomialModP( x^3 + x + 1, 5 );
```
Multiply two polynomials modulo an integer. 

*\** [MultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP] {CalculatorFunctionsBinaryOps::multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22p%20%3d%20PolynomialModP%28%20x%5e4%20%2b%20x%20%2b%201%2c%207%20%29%3b%5cna%20%3d%20PolynomialModP%28%20x%5e3%20%2b%20x%20%2b%201%2c%207%20%29%3b%5cnb%20%3d%20PolynomialModP%28%20x%20%2b%202%2c%207%20%29%3b%5cnf%20%3d%20a%20mod%20p%3b%5cng%20%3d%20b%20mod%20p%3b%5cnf%20%2a%20g%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
p = PolynomialModP( x^4 + x + 1, 7 );
a = PolynomialModP( x^3 + x + 1, 7 );
b = PolynomialModP( x + 2, 7 );
f = a mod p;
g = b mod p;
f * g
```
Multiply two polynomials modulo an integer. 

*\** [MultiplyPolynomialAlgebraicNumbersByAlgebraicNumber] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%7b%7d%28%5c%5csqrt%7b3%7dx%29%2a%5c%5csqrt%7b6%7d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers{}(\sqrt{3}x)*\sqrt{6};
```
Multiplies two polynomials over the algebraic numbers. 

*\** [MultiplyAlgebraicNumberByPolynomialAlgebraicNumbers] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%7b6%7d%2aPolynomialAlgebraicNumbers%7b%7d%28%5c%5csqrt%7b3%7dx%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt{6}*PolynomialAlgebraicNumbers{}(\sqrt{3}x);
```
Multiplies two polynomials over the algebraic numbers. 

*\** [MultiplyRationalByPolynomialAlgebraicNumbers] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%7b%7d%28%5c%5csqrt%7b3%7dx%29%2a2%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers{}(\sqrt{3}x)*2;
```
Multiplies two polynomials over the algebraic numbers. 

*\** [MultiplyPolynomialByAlgebraicNumber] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7d%28x%29%2a%5c%5csqrt%7b2%7d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}(x)*\sqrt{2};
```
Multiplies two polynomials over the algebraic numbers. 

*\** [MultiplyAlgebraicNumberByPolynomial] {CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%7b3%7d%20%2a%20PolynomialAlgebraicNumbers%7b%7d%28x%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt{3} * PolynomialAlgebraicNumbers{}(x);
```
Multiplies two polynomials over the algebraic numbers. 

*\** [MultiplyMatrixTensorByMatrixTensor] {CalculatorFunctionsBinaryOps::multiplyMatrixTensorOrRationalByMatrixTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22M%20%3d%20MakeMatrixTensorForm%7b%7d%28%281%2c%201%29%2c%20%280%2c%201%29%29%3b%5cnM%3b%5cnM%20%2a%20M%3b%5cnM%20%2a%20M%20%2a%20M%3b%5cnM%20%2a%20M%20%2a%20M%20%2a%20M%3b%5cn2%20%2a%20M%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
M = MakeMatrixTensorForm{}((1, 1), (0, 1));
M;
M * M;
M * M * M;
M * M * M * M;
2 * M
```
Multiplies matrix rational by matrix tensor. 

*\** [MultiplyRationalByMatrixTensor] {CalculatorFunctionsBinaryOps::multiplyMatrixTensorOrRationalByMatrixTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22M%20%3d%20MakeMatrixTensorForm%7b%7d%28%281%2c%203%2c%205%29%2c%20%28-%201%2c%201%2c%200%29%2c%20%282%2c%202%2c%207%29%29%3b%5cnM%3b%5cnM%20%2a%20M%3b%5cnM%20%2a%20M%20%2a%20M%3b%5cnM%20%2a%20M%20%2a%20M%20%2a%20M%3b%5cn2%20%2a%20M%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
M = MakeMatrixTensorForm{}((1, 3, 5), (- 1, 1, 0), (2, 2, 7));
M;
M * M;
M * M * M;
M * M * M * M;
2 * M 
```
Multiplies rational by matrix tensor form. 

*\** [CombineExponents] {CalculatorBasics::multiplyAtoXtimesAtoYequalsAtoXplusY}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%2a%28x%2ay%29%2ax%2a%28x%2ax%5e3%2ax%29%3b%5cnx%5e%7b2%2f3%7dx%5e%7b1%2f2%7d%3b%5cn%28x%3e0%29%3d1%3b%5cnResetBuiltInHandlerCache%28%29%3b%5cnx%5e%7b2%2f3%7dx%5e%7b1%2f2%7d%3b%5cn%28x%5e%7b1%2f2%7d%29%5e2%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x*(x*y)*x*(x*x^3*x);
x^{2/3}x^{1/2};
(x>0)=1;
ResetBuiltInHandlerCache();
x^{2/3}x^{1/2};
(x^{1/2})^2;
```
Collects multiplicand exponents. 

*\** [MultiplyByOne] {CalculatorBasics::multiplyByOne}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%2a1%3b%5cnx%2a1-x%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x*1;
x*1-x 
```
Rule 1*{{anything}} = anything.

*\** [CommuteConstants] {CalculatorFunctions::outerCommuteConstants}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%206%5e%7b1%2f3%7d%3b%5cn%28x%2010%5e%7b1%2f2%7d%29%5e%7b1%2f3%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x 6^{1/3};
(x 10^{1/2})^{1/3}
```
Rule that commutes constants to the left-most positions. Provided that a is a constant number (built in) and b is not, replaces b*a by a*b. 

*\** [MergeConstantRadicals] {CalculatorFunctions::outerMergeConstantRadicals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%7b%7d2%20%5c%5csqrt%7b%7d3%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt{}2 \sqrt{}3
```
If a and b are constants, replaces a^{c}b^c by (a b)^c.

*\** [AssociativeRule] {CalculatorBasics::associate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2ab%29%2a%28c%2a%28d%2ae%29%2af%29%20-%20a%2ab%2ac%2ad%2a%20e%20%2af%3b%5cn%28a%2ab%29%2a%28c%2a%28e%20%2a%20d%29%2af%29%20-%20a%2ab%2ac%2ad%2a%20e%20%2af%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a*b)*(c*(d*e)*f) - a*b*c*d* e *f;
(a*b)*(c*(e * d)*f) - a*b*c*d* e *f
```
Associative law: reorders the multiplicative tree in standard form. 

*\** [CommuteAtimesBtimesCifUnivariate] {CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%28x%20%2b%201%29%5e%7b-%201%7dx%3b%5cnx%28y%2b%201%29%5e%7b-%201%7dx%3b%5cn%28%5c%5csin%20x%20%29%20x%20%28%5c%5ccos%20x%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x(x + 1)^{- 1}x;
x(y+ 1)^{- 1}x;
(\sin x ) x (\cos x)
```
Commutative law: replaces a*b by b*a provided that 1) a and and b depend on exactly one user-defined variable, 2) a is not a constant and 3) a>b as an expression. 

*\** [CommuteIfUnivariate] {CalculatorFunctions::outerCommuteAtimesBifUnivariate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%28x%20%2b%201%29%5e%7b-%201%7dx%3b%5cnx%28y%2b%201%29%5e%7b-%201%7dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x(x + 1)^{- 1}x;
x(y+ 1)^{- 1}x
```
Commutative law: replaces a*b by b*a provided that 1) a and and b depend on exactly one user-defined variable, 2) a is not a constant and 3) a>b as an expression. 

*\** [ConstantExtraction] {CalculatorBasics::extractBaseMultiplication}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%2a%28%283%2ac%29%2a%284%2ad%29%29%3b%5cn3%2a%28%28a%2a%28d-d%29%29b%2ac%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2*((3*c)*(4*d));
3*((a*(d-d))b*c)
```
Pulls rationals in the front of multiplicative terms.

*\** [AssociateTimesDivision] {CalculatorBasics::associateTimesDivision}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%2a%28b%2fc%29%3b%5cn%28a%2ab%29%2fc-a%2a%28b%2fc%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a*(b/c);
(a*b)/c-a*(b/c)
```
Associative law w.r.t. division. 

*\** [CancelMultiplicativeInverse] {CalculatorBasics::cancelMultiplicativeInverse}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2ab%29%2fb%3b%5cn%28a%2fb%29%2ab%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a*b)/b;
(a/b)*b
```
Cancels multiplicative inverse. 

*\** [DistributeMultiplication] {CalculatorBasics::distributeTimes}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%20%2b%20b%29%20%2a%20c%3b%5cna%20%2a%20%28b%20%2b%20c%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a + b) * c;
a * (b + c)
```
Distributive law (left and right).

*\** [DistributeMultiplicationConstants] {CalculatorBasics::distributeTimesConstant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%28b%2bc%29%3b%5cnTurnOffRules%28DistributeMultiplication%29%3b%5cna%28b%2bc%29%3b%20-5%28b%2bc%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a(b+c);
TurnOffRules(DistributeMultiplication);
a(b+c); -5(b+c) 
```
Distributive law (left and right), acts only if the multiplicand is a constant. This rule is overridden by DistributeMultiplication, unless the DistributeMultiplication rule is turned off (which is the intended use case of this rule).

*\** [DivideAdivBpowerItimesBpowerJ] {CalculatorFunctions::outerDivideReplaceAdivBpowerItimesBpowerJ}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2fx%29%20x%5e%7b-%201%7d%3b%5cn%28a%2fx%5e2%29%20x%5e%7b3%7d%3b%5cn%28a%2fx%29%20x%5e%7b3%7d%3b%5cn%28a%2fx%5e2%29%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a/x) x^{- 1};
(a/x^2) x^{3};
(a/x) x^{3};
(a/x^2) x
```
Rule: (a/x^t)x^s =a x^{s-t}.

*\** [MultiplyRationalByRationalFunction] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylDimFormula%7b%7d%28a_2%2c%20%280%2c3%29%29%20%2b%20WeylDimFormula%7b%7d%28a_2%2c%20%283%2c0%29%29%20%2b%204%20WeylDimFormula%7b%7d%28a_2%2c%20%281%2c1%29%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylDimFormula{}(a_2, (0,3)) + WeylDimFormula{}(a_2, (3,0)) + 4 WeylDimFormula{}(a_2, (1,1)) 
```
Multiplies rational number by a rational function.

*\** [MultiplyPolynomialByRationalFraction] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7d%28x%20%2b1%29MakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2%2bx%20%7d%29%3b%5cnMakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2-x%20%7d%29MakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2%2bx%20%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}(x +1)MakeRationalFunction{}(\frac{-2x -2}{x^2+x });
MakeRationalFunction{}(\frac{-2x -2}{x^2-x })MakeRationalFunction{}(\frac{-2x -2}{x^2+x })
```
Multiplies polynomial by a rational fraction.

*\** [MultiplyRationalFractionByPolynomial] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7d%28x%20%2b1%29MakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2%2bx%20%7d%29%3b%5cnMakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2-x%20%7d%29MakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2%2bx%20%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}(x +1)MakeRationalFunction{}(\frac{-2x -2}{x^2+x });
MakeRationalFunction{}(\frac{-2x -2}{x^2-x })MakeRationalFunction{}(\frac{-2x -2}{x^2+x })
```
Multiplies rational fraction by polynomial.

*\** [MultiplyRationalFunctionByRationalFunction] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7d%28x%20%2b1%29MakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2%2bx%20%7d%29%3b%5cnMakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2-x%20%7d%29MakeRationalFunction%7b%7d%28%5c%5cfrac%7b-2x%20-2%7d%7bx%5e2%2bx%20%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}(x +1)MakeRationalFunction{}(\frac{-2x -2}{x^2+x });
MakeRationalFunction{}(\frac{-2x -2}{x^2-x })MakeRationalFunction{}(\frac{-2x -2}{x^2+x })
```
Multiplies rational function by a rational function.

*\** [ScalarTimesSequence] {CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22v_%7b1%7d%3d%20%281%2c%202%2c%203%29%3b%5cnv_%7b2%7d%3d%20%281%2c%203%2c%202%29%3b%5cnv_%7b3%7d%3d%20%283%2c%201%2c%201%29%3b%5cnv_%7b4%7d%3d%20%28-2%2c%202%2c%202%29%3b%5cn1%2f2v_%7b1%7d%2b%201%2f2v_%7b2%7d%2b7%2f8v_%7b3%7d%2b%2013%2f16v_%7b4%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
v_{1}= (1, 2, 3);
v_{2}= (1, 3, 2);
v_{3}= (3, 1, 1);
v_{4}= (-2, 2, 2);
1/2v_{1}+ 1/2v_{2}+7/8v_{3}+ 13/16v_{4}
```
Carries out multiplication between a rational number on left and sequence on the right. Corresponds to multiplying a vector by a scalar (however please note a sequence does not necessarily consist of elements of a field, so the latter interpretation might not be applicable).

*\** [ScalarTimesMatrix] {CalculatorFunctionsBinaryOps::multiplyAnyScalarByMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%5c%5cbegin%7bpmatrix%7d%20%201%26%202%26%203%5c%5c%5c%5c2%26%201%26%200%20%5c%5cend%7bpmatrix%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2\begin{pmatrix}  1& 2& 3\\2& 1& 0 \end{pmatrix}
```
Multiplies a rational number by a matrix. 

*\** [MultiplyDoubleBySequence] {CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25NumberColors%5cn%281%2c%202%29%20-%20DoubleValue%7b%7d%201%20%282%2c3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%NumberColors
(1, 2) - DoubleValue{} 1 (2,3)
```
Carries out multiplication between a double number on left and sequence on the right. Corresponds to multiplying a vector by a scalar.

*\** [MultiplyAlgebraicNumberBySequence] {CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%281%20%2c2%29-%20DoubleValue%7b%7d%201%20%282%2c3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(1 ,2)- DoubleValue{} 1 (2,3)
```
Multiplies a double number on left and a sequence on the right.scalar.

*\** [MultiplyPolynomialBySequence] {CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20Polynomial%7b%7dx%3b%5cnv%3dx%2a%281%2c%202%2c%203%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = Polynomial{}x;
v=x*(1, 2, 3);
```
Carries out multiplication between a rational number on left and sequence on the right.

*\** [MultiplyRationalByElementWeylAlgebra] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cn3%5c%5cpartial_i%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
3\partial_i
```
Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. 

*\** [MultiplyPolynomialByElementWeylAlgebra] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cnx_i%5c%5cpartial_i-%5c%5cpartial_i%20x_i-%5bx_i%2c%20%5c%5cpartial_i%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
x_i\partial_i-\partial_i x_i-[x_i, \partial_i]
```
Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. 

*\** [MultiplyElementWeylAlgebraByElementWeylAlgebra] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cna%20%3d%20x_1%20x_2%3b%5cnb%20%3d%20%5c%5cpartial_1%20%5c%5cpartial_2%3b%20a%20b%20-%20b%20a%20-%5ba%2cb%5d%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
a = x_1 x_2;
b = \partial_1 \partial_2; a b - b a -[a,b] 
```
Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. 

*\** [MultiplyElementWeylAlgebraByPolynomial] {CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%5cnx_i%5c%5cpartial_i-%5c%5cpartial_i%20x_i-%5bx_i%2c%20%5c%5cpartial_i%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i;
x_i\partial_i-\partial_i x_i-[x_i, \partial_i]
```
Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. 

*\** [MultiplyEllipticCurveElementsRational] {CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g%20%3d%20ElementEllipticCurveNormalForm%28y%5e2%20%3d%20x%5e3%20-%20x%20%2b1%2c%20x%20%3d%203%2c%20y%20%3d%205%29%3b%5cnh%20%3d%20g%5e2%3b%5cnh%20%2a%20g%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);
h = g^2;
h * g

```
Multiplies two elements of elliptic curves.

*\** [MultiplyEllipticCurveElementsOverZmodP] {CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g%20%3d%20ElementEllipticCurveNormalForm%28y%5e2%20%3d%20x%5e3%20%2bx%20-%205%2c%20x%20%3d%203%20mod%2017%2c%20y%20%3d%205%20mod%2017%29%3b%5cnh%20%3d%20g%5e2%3b%5cnh%20%2a%20g%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g = ElementEllipticCurveNormalForm(y^2 = x^3 +x - 5, x = 3 mod 17, y = 5 mod 17);
h = g^2;
h * g

```
Multiplies two elements of elliptic curves.

*\** [MultiplyElementHyperOctahedralByElementHyperOctahedral] {CalculatorFunctionsBinaryOps::multiplyElementHypOctByElementHypOct}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s%20%3d%20MakeElementHyperOctahedral%7b%7d%28%281%2c%202%29%2c%201%2c%200%2c%200%29%3b%20t%20%3d%20MakeElementHyperOctahedral%7b%7d%28%281%2c%203%29%2c%200%2c%200%2c%200%29%3b%20s%20%2a%20t%20%2a%20s%20%2a%20t%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0); t = MakeElementHyperOctahedral{}((1, 3), 0, 0, 0); s * t * s * t
```
Multiplies two elements of hyperoctahedral groups. 

*\** [MultiplyCharacterSemisimpleLieAlgebraModuleBy] {CalculatorFunctionsBinaryOps::multiplyCharSSLieAlgByCharSSLieAlg}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20MakeCharacterLieAlg%7b%7d%28G_2%2c%20%281%2c0%29%29%3b%5cny%20%3d%20MakeCharacterLieAlg%7b%7d%28G_2%2c%20%280%2c1%29%29%3b%5cnx%20%2a%20y%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = MakeCharacterLieAlg{}(G_2, (1,0));
y = MakeCharacterLieAlg{}(G_2, (0,1));
x * y
```
Multiplies two semisimple Lie algebra finite dimensional characters and decomposes using the Brauer-Klimyk formula, Humphreys J. Introduction to Lie algebras and representation theory, page 142, exercise 9. 

*\** [MultiplyWeylGroupElementByWeylGroupElement] {CalculatorFunctionsBinaryOps::multiplyCoxeterElementByCoxeterElement}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20MakeElementWeylGroup%7b%7d%28A_2%2c%201%29%3b%20x%2ax%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = MakeElementWeylGroup{}(A_2, 1); x*x
```
Multiplies two Weyl group elements if possible. 

*\** [TensorAndDecomposeWeylGroupRepresentations] {CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylReps}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22W%20%3d%20WeylGroupNaturalRep%7b%7d%28B_3%29%3b%5cnV%20%3d%20MakeVirtualWeylGroupRepresentation%7b%7dW%3b%5cnW%5c%5cotimes%20W%3b%5cnV%20%2a%20V%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
W = WeylGroupNaturalRep{}(B_3);
V = MakeVirtualWeylGroupRepresentation{}W;
W\otimes W;
V * V
```
Tensor and decompose two virtual Weyl group representations. 

*\** [MultiplyRationalByUE] {CalculatorFunctionsBinaryOps::multiplyAnyByUE}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28F_1%2c%20i%29%3b%20h_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28F_1%2c%20i%29%3b%5cn%5bg_%7b22%7d%2bg_%7b20%7d%2bg_%7b14%7d%2cg_%7b17%7d-6%2f5g_%7b14%7d%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(F_1, i); h_{{i}}= CartanGenerator{}(F_1, i);
[g_{22}+g_{20}+g_{14},g_{17}-6/5g_{14}]
```
Multiplies rational number by an element universal enveloping algebra.

*\** [MultiplyUEByUE] {CalculatorFunctionsBinaryOps::multiplyAnyByElementUniversalEnveloping}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28F_1%2c%20i%29%3bh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28F_1%2c%20i%29%20%3b%5cn%5bg_%7b22%7d%2bg_%7b20%7d%2bg_%7b14%7d%2cg_%7b17%7d-6%2f5g_%7b14%7d%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(F_1, i);h_{{i}}= CartanGenerator{}(F_1, i) ;
[g_{22}+g_{20}+g_{14},g_{17}-6/5g_{14}]
```
Multiplies elment Universal enveloping by element universal enveloping algebra.

*\** [MultiplyAnyByElementTensor] {CalculatorFunctionsBinaryOps::multiplyAnyByElementTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20G_2%3b%5cng_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28X%2c%20i%29%3b%5cnv%3dHeighestWeightVector%7b%7d%28G_2%2c%20%281%2c0%29%2c%280%2c0%29%29%3b%5cn2%2f5%20v%3b%5cn%283%2f4%20v%29%5c%5cotimes%20v%3b%5cn3%2f4%20%28v%5c%5cotimes%20v%29%3b%5cn%283%2f4%20v%29%5c%5cotimes%20v-3%2f4%20%28v%5c%5cotimes%20v%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = G_2;
g_{{i}}= ChevalleyGenerator{}(X,i);
h_{{i}}= CartanGenerator{}(X, i);
v=HeighestWeightVector{}(G_2, (1,0),(0,0));
2/5 v;
(3/4 v)\otimes v;
3/4 (v\otimes v);
(3/4 v)\otimes v-3/4 (v\otimes v)
```
Handles multiplying rational number by an element of tensor product of generalized Verma modules. Not fully tested and documented at the moment. Will get more documented in the future. 

*\** [MultiplyPolynomialByElementTensorGeneralVermaModule] {CalculatorFunctionsBinaryOps::multiplyAnyByElementTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20G_2%3b%5cng_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28X%2c%20i%29%3b%5cnz%20%3d%20Polynomial%7b%7dy%3b%5cnv%20%3d%20HeighestWeightVector%7b%7d%28G_2%2c%20%28z%2c1%29%2c%281%2c0%29%29%3b%5cn%282%2az%29%20v%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = G_2;
g_{{i}}= ChevalleyGenerator{}(X,i);
h_{{i}}= CartanGenerator{}(X, i);
z = Polynomial{}y;
v = HeighestWeightVector{}(G_2, (z,1),(1,0));
(2*z) v;

```
Handles multiplying polynomial by an element of tensor product of generalized Verma modules. Not fully tested and documented at the moment. Will get more documented in the future. 

*\** [MultiplyAnyByTensor] {CalculatorFunctionsBinaryOps::multiplyAnyByElementTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20G_2%3b%5cng_%7b%7bi%7d%7d%20%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%20%3d%20CartanGenerator%7b%7d%28X%2c%20i%29%3b%5cnz%20%3d%20Polynomial%7b%7dy%3b%5cnv%20%3d%20HeighestWeightVector%7b%7d%28G_2%2c%20%28z%2c1%29%2c%281%2c0%29%29%3b%5cn1%2fz%20v%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = G_2;
g_{{i}} = ChevalleyGenerator{}(X,i);
h_{{i}} = CartanGenerator{}(X, i);
z = Polynomial{}y;
v = HeighestWeightVector{}(G_2, (z,1),(1,0));
1/z v
```
Handles multiplying rational function number by an element of tensor product of generalized Verma modules. Not fully tested and documented at the moment. Will get more documented in the future. 

*\** [MultiplyElementUniversalEnvelopingRationalFunctionByElementTensorGeneralizedVermaModule] {CalculatorFunctionsBinaryOps::multiplyAnyByElementTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20G_2%3b%5cng_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28X%2c%20i%29%3b%5cnz%3d%20Polynomial%7b%7dy%3b%5cnv%3dHeighestWeightVector%7b%7d%28G_2%2c%20%28z%2c1%29%2c%281%2c0%29%29%3b%5cnh_1%20v%3b%20%5cnh_2%20v%3b%5cn%20g_1%20g_%7b-%201%7d%20v%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = G_2;
g_{{i}}= ChevalleyGenerator{}(X,i);
h_{{i}}= CartanGenerator{}(X, i);
z= Polynomial{}y;
v=HeighestWeightVector{}(G_2, (z,1),(1,0));
h_1 v; 
h_2 v;
 g_1 g_{- 1} v 
```
Handles acting by element Universal enveloping on an element of tensor product of generalized Verma modules. Not fully tested and documented at the moment. Will get more documented in the future. 

*\** [MultiplyWeylGroupElementByWeightLieAlgebraPolynomial] {CalculatorFunctionsBinaryOps::multiplyWeylGroupElementByWeightPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s_%7b%7ba%7d%7d%3dMakeElementWeylGroup%28B_3%2c%20a%29%3b%5cn%5c%5cvarepsilon_%7b%7ba%7d%7d%20%3d%20MakeWeight%7b%7d%28B_3%2c%20a%2c%20epsilon%29%3b%5cnx%20%3d%20Polynomial%7b%7dx%3b%5cn%5c%5cmu%20%3d%20x%5c%5cvarepsilon_1%3b%5cns_1s_2s_3s_2s_1%20%5c%5cmu%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s_{{a}}=MakeElementWeylGroup(B_3, a);
\varepsilon_{{a}} = MakeWeight{}(B_3, a, epsilon);
x = Polynomial{}x;
\mu = x\varepsilon_1;
s_1s_2s_3s_2s_1 \mu
```
Element of Weyl group action on a weight. 

*\** [MultiplyPolynomialByWeightLieAlgebraPolynomial] {CalculatorFunctionsBinaryOps::multiplyRatOrPolyByWeightPoly}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cvarepsilon_%7b%7ba%7d%7d%3dMakeWeight%7b%7d%28B_3%2c%20a%2c%20epsilon%29%3b%20x%20%3d%20Polynomial%7b%7dx%3b%20x%5c%5cvarepsilon_1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon); x = Polynomial{}x; x\varepsilon_1
```
Carries out multiplication between a rational or polynomial on left and a weight on the right.

*\** [MultiplyRaitonalByWeightLieAlgebraPolynomial] {CalculatorFunctionsBinaryOps::multiplyRatOrPolyByWeightPoly}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cvarepsilon_%7b%7ba%7d%7d%3dMakeWeight%7b%7d%28B_3%2c%20a%2c%20epsilon%29%3b%20x%20%3d%20Polynomial%7b%7dx%3b%20x%5c%5cvarepsilon_1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon); x = Polynomial{}x; x\varepsilon_1
```
Carries out multiplication between a rational or polynomial on left and a weight on the right.

*\** (_composite_) [CompositeArithmeticOperationEvaluatedOnArgumentMultiplication] {CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2ab%29%7b%7dx%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a*b){}x;
```
Equivalent to (a*b){}x = (a{}x)*(b{}x) 

*\** (_composite_) [ConstTimesAnyAction] {CalculatorFunctions::compositeConstTimesAnyActOn}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%282%5c%5csin%29%7b%7dx-2%28%5c%5csin%20x%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(2\sin){}x-2(\sin x) 
```
Rule (a*f){}x = a*(f{}x), provided a is a constant. 

*\!* [Factorial] {CalculatorFunctionsBasic::factorial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%225%21%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
5!
```
Factorial function. 

Operator or function mod is overloaded with 2 total handlers.

*mod* [mod] {CalculatorFunctions::zModP}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%227%20mod%203%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
7 mod 3
```
Same as ModP but uses the mod notation.

*mod* [moduloPolynomial] {CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22p%20%3d%20PolynomialModP%28x%5e2%2c%207%29%3b%5cnq%20%3d%20PolynomialModP%28x%5e9%2bx%2b1%2c%207%29%3b%5cnr%20%3d%20PolynomialModP%28y%5e2%2c%207%29%3b%5cns%20%3d%20PolynomialModP%28x%5e5%2c%205%29%3b%5cna%20%3d%20p%20mod%20q%3b%5cna%5e10%3b%5cnr%20mod%20q%3b%5cns%20mod%20q%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
p = PolynomialModP(x^2, 7);
q = PolynomialModP(x^9+x+1, 7);
r = PolynomialModP(y^2, 7);
s = PolynomialModP(x^5, 5);
a = p mod q;
a^10;
r mod q;
s mod q
```
A polynomial modulo another polynomial. Returns the element given by the first argument of a quotient ring formed modulo the ideal generated by the second argument. 

*and* [and] {CalculatorFunctions::andFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%220%20and%200%3b%5cn0%20and%201%3b%5cn1%20and%200%3b%5cn1%20and%201%3b%5cna%20and%201%3b%5cna%20and%200%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
0 and 0;
0 and 1;
1 and 0;
1 and 1;
a and 1;
a and 0;
```
Logical and. 

Operator or function or is overloaded with 2 total handlers.

*or* [or] {CalculatorFunctions::or}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%220%20or%200%3b%5cn0%20or%201%3b%5cn1%20or%200%3b%5cn1%20or%201%3b%5cna%20or%201%3b%5cna%20or%200%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
0 or 0;
0 or 1;
1 or 0;
1 or 1;
a or 1;
a or 0;
```
Logical or.

*or* [orIdentical] {CalculatorFunctions::orIdentical}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20or%20b%3b%5cna%20or%20a%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a or b;
a or a
```
If the two arguments of or are identical, replaces the expression with the argument. Works even if the individual expression cannot be evaluated to 1 or 0. Please note that mathematically equal objects may fail to be identical, for example a rational number 5 and an algebraic number 5. 

*\times* [crossProduct] {CalculatorFunctions::crossProduct}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%281%2c%20-%202%2c%203%29%20%5c%5ctimes%20%28%201%2c%20-%201%2c%20-%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(1, - 2, 3) \times ( 1, - 1, - 1)
```
Cross product in IIId. Returns error if given two non-3d vectors.

Operator or function \otimes is overloaded with 5 total handlers.

*\otimes* [MatrixTensorMatrix] {CalculatorFunctionsBinaryOps::tensorMatrixByMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22P%20%3d%20%28%280%2c%202%20%29%2c%20%281%2c%200%29%29%3b%5cnQ%20%3d%20%28%280%2c%203%20%29%2c%20%281%2c%200%29%29%3b%5cnX%20%3d%20MakeMatrix%7b%7dP%3b%5cnY%20%3d%20MakeMatrix%7b%7dQ%3b%5cnZ%20%3d%20MakeMatrixTensorForm%7b%7dP%3b%5cnW%20%3d%20MakeMatrixTensorForm%7b%7dQ%3b%5cnX%5c%5cotimes%20Y%3b%5cnZ%5c%5cotimes%20W%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
P = ((0, 2 ), (1, 0));
Q = ((0, 3 ), (1, 0));
X = MakeMatrix{}P;
Y = MakeMatrix{}Q;
Z = MakeMatrixTensorForm{}P;
W = MakeMatrixTensorForm{}Q;
X\otimes Y;
Z\otimes W
```
Tensor product of two matrices.

*\otimes* [TensorMatrixTensorByMatrixTensor] {CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22P%20%3d%20%28%280%20%2c%202%20%29%2c%281%20%2c%200%29%29%3b%5cnQ%20%3d%20%28%280%20%2c%203%20%29%2c%281%20%2c%200%29%29%3b%5cnX%20%3d%20MakeMatrix%7b%7dP%3b%5cnY%20%3d%20MakeMatrix%7b%7dQ%3b%5cnZ%20%3d%20MakeMatrixTensorForm%7b%7dP%3b%5cnW%20%3d%20MakeMatrixTensorForm%7b%7dQ%3b%5cnX%20%5c%5cotimes%20Y%3b%5cnZ%5c%5cotimes%20W%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
P = ((0 , 2 ),(1 , 0));
Q = ((0 , 3 ),(1 , 0));
X = MakeMatrix{}P;
Y = MakeMatrix{}Q;
Z = MakeMatrixTensorForm{}P;
W = MakeMatrixTensorForm{}Q;
X \otimes Y;
Z\otimes W
```
Same as tensor product of matrices but uses class MatrixTensor instead of class Matrix.

*\otimes* [TensorProductStandard] {CalculatorBasics::tensorProductStandard}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20G_2%3b%5cng_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28X%2ci%29%3b%5cnv%3dHeighestWeightVector%7b%7d%28X%2c%20%281%2c0%29%2c%280%2c0%29%29%3b%5cng_%7b-%201%7d%28v%5c%5cotimes%20v%29%3b%5cng_%7b-%201%7dg_%7b-%201%7d%28v%5c%5cotimes%20v%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = G_2;
g_{{i}}= ChevalleyGenerator{}(X,i);
h_{{i}}= CartanGenerator{}(X,i);
v=HeighestWeightVector{}(X, (1,0),(0,0));
g_{- 1}(v\otimes v);
g_{- 1}g_{- 1}(v\otimes v); 
```
Please do note use (or use at your own risk): this is work-in-progress. Will be documented when implemented and tested. Tensor product of generalized Verma modules. 

*\otimes* [TensorElementGeneralizedVermaModuleByElementGeneralizedVermaModule] {CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20G_2%3b%5cng_%7b%7bi%7d%7d%20%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28X%2c%20i%29%3b%5cnz%3d%20Polynomial%7b%7dy%3b%5cnv%3dHeighestWeightVector%7b%7d%28G_2%2c%20%28z%2c1%29%2c%20%281%2c0%29%29%3b%5cng_%7b-%201%7d%28v%5c%5cotimes%20v%29%3b%5cng_%7b-%201%7dg_%7b-%201%7d%28v%5c%5cotimes%20v%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = G_2;
g_{{i}} = ChevalleyGenerator{}(X,i);
h_{{i}}= CartanGenerator{}(X, i);
z= Polynomial{}y;
v=HeighestWeightVector{}(G_2, (z,1), (1,0));
g_{- 1}(v\otimes v);
g_{- 1}g_{- 1}(v\otimes v)
```
Tensor product of two generalized Verma modules. Not fully tested and documented at the moment. Will get more documented in the future. 

*\otimes* [TensorWeylGroupRepresentationByWeylGroupRepresentation] {CalculatorFunctionsWeylGroup::tensorWeylReps}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22V%20%3d%20WeylGroupNaturalRep%7b%7d%28B_3%29%3b%20V%20%5c%5cotimes%20V%3b%20V%20%2a%20V%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
V = WeylGroupNaturalRep{}(B_3); V \otimes V; V * V
```
Tensor product of two Weyl group reps. Does not decompose the tensor product. If you want decomposition, use V*V instead. 

*\oplus* [DirectSumMatrices] {CalculatorFunctionsBinaryOps::directSumMatrixWithMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cbegin%7bpmatrix%7d%201%26%202%5c%5c%5c%5c%202%26%203%5c%5cend%7bpmatrix%7d%5c%5coplus%5cn%5c%5cbegin%7bpmatrix%7d%20-%201%26%203%5c%5c%5c%5c%20-2%26%20-3%5c%5cend%7bpmatrix%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\begin{pmatrix} 1& 2\\ 2& 3\end{pmatrix}\oplus
\begin{pmatrix} - 1& 3\\ -2& -3\end{pmatrix}
```
Direct sum of two matrices, non-commutative.

*\binom* [Binom] {CalculatorFunctionsBinaryOps::binomialCoefficient}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cbinom%7b8%7d%7b3%7d%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\binom{8}{3} 
```
Evaluates the binomial coefficient if possible.

Operator or function \sqrt is overloaded with 3 total handlers.

*\sqrt* [sqrtApproximate] {CalculatorFunctions::sqrtApproximate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%202%2b%5c%5csqrt%203%3b%5cn%28%5c%5csqrt%7b%7d2%2b%5c%5csqrt%7b%7d3%2b%5c%5csqrt%7b%7d6%29%5e2%3b%5cnTurnApproximations%280%29%3b%5cn%5c%5csqrt%7b2%7d%2b%5c%5csqrt%7b3%7d%3b%5cn%5c%5csqrt%5b-2%5d%7b0%7d%3b%5cn%5c%5csqrt%5b3%5d%7b2%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt 2+\sqrt 3;
(\sqrt{}2+\sqrt{}3+\sqrt{}6)^2;
TurnApproximations(0);
\sqrt{2}+\sqrt{3};
\sqrt[-2]{0};
\sqrt[3]{2}
```
Approximate the square root of a number over the reals. 

*\sqrt* [sqrt] {CalculatorFunctions::sqrt}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%202%2b%5c%5csqrt%203%3b%5cn%28%5c%5csqrt%7b%7d2%2b%5c%5csqrt%7b%7d3%2b%5c%5csqrt%7b%7d6%29%5e2%3bTurnApproximations%280%29%3b%5c%5csqrt%7b2%7d%2b%5c%5csqrt%7b3%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt 2+\sqrt 3;
(\sqrt{}2+\sqrt{}3+\sqrt{}6)^2;TurnApproximations(0);\sqrt{2}+\sqrt{3}
```
Square root of a rational, implemented as algebraic extension of the rationals. 

*\sqrt* [DistributeSqrt] {CalculatorFunctions::distributeSqrt}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffApproximations%200%3b%5cn%5c%5csqrt%7b%5c%5cfrac%7b676%7d%7b25%7d%20ln%203%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffApproximations 0;
\sqrt{\frac{676}{25} ln 3}
```
If a is a positive constant, substitutes sqrt(a b) by sqrt(a) sqrt(b).

Operator or function [] is overloaded with 7 total handlers.

*[]* [LieBracketDistribute] {CalculatorFunctionsBinaryOps::lieBracketDistribute}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5ba%2bb%2c%20c%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
[a+b, c]
```
Implementation of the rules [{{a}}+{{b}},{{c}}] = [a, c] +[b, c] and [{{c}},{{a}}+{{b}}] =[c,a] +[c,b]

*[]* [LieBracketOfMatrices] {CalculatorFunctionsBinaryOps::lieBracketOfMatrices}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22h%3d%5c%5cbegin%7bpmatrix%7d3%20%26%200%20%26%200%20%26%200%5c%5c%5c%5c%5cn0%20%26%201%20%26%200%20%26%200%5c%5c%5c%5c0%20%26%200%20%26%20-1%20%26%200%5c%5c%5c%5c0%20%26%200%20%26%200%20%26%20-3%5c%5c%5c%5c%5c%5cend%7bpmatrix%7d%3be%3d%5c%5cbegin%7bpmatrix%7d0%20%26%203%20%26%200%20%26%200%5c%5c%5c%5c0%20%26%200%20%26%202%20%26%200%5c%5c%5c%5c0%20%26%200%20%26%200%20%26%203%2f5%5c%5c%5c%5c0%20%26%200%20%26%200%20%26%200%5c%5c%5c%5c%5c%5cend%7bpmatrix%7d%3bf%3d%5c%5cbegin%7bpmatrix%7d0%20%26%200%20%26%200%20%26%200%5c%5c%5c%5c1%20%26%200%20%26%200%20%26%200%5c%5c%5c%5c0%20%26%202%20%26%200%20%26%200%5c%5c%5c%5c0%20%26%200%20%26%205%20%26%200%5c%5c%5c%5c%5c%5cend%7bpmatrix%7d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
h=\begin{pmatrix}3 & 0 & 0 & 0\\
0 & 1 & 0 & 0\\0 & 0 & -1 & 0\\0 & 0 & 0 & -3\\\end{pmatrix};e=\begin{pmatrix}0 & 3 & 0 & 0\\0 & 0 & 2 & 0\\0 & 0 & 0 & 3/5\\0 & 0 & 0 & 0\\\end{pmatrix};f=\begin{pmatrix}0 & 0 & 0 & 0\\1 & 0 & 0 & 0\\0 & 2 & 0 & 0\\0 & 0 & 5 & 0\\\end{pmatrix};
```
Matrix lie bracket. 

*[]* [LieBracketConstants] {CalculatorFunctionsBinaryOps::lieBracketExtractConstant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5b2a%2c%203b%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
[2a, 3b]
```
Extracts constants from Lie brackets. 

*[]* [LieBracketWeylAlgebraElements] {CalculatorFunctionsBinaryOps::lieBracketRatPolyOrEWAWithRatPolyOrEWA}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%3d%20Polynomial%7b%7dx_i%3b%20%5cn%5b%5c%5cpartial_1%2c%20x_1%5d%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}}= ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}}= Polynomial{}x_i; 
[\partial_1, x_1]; 
```
Lie bracket of elements of Weyl algebras = differential operators with polynomial coefficients. 

*[]* [LieBracketSemisimpleLieAlgebras] {CalculatorFunctionsBinaryOps::lieBracketRatOrUEWithRatOrUE}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3dA_1%3b%5cng_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28X%2ci%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28X%2ci%29%3b%5cn%5bg_1%2cg_%7b-%201%7d%5d%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X =A_1;
g_{{i}}= ChevalleyGenerator{}(X,i);
h_{{i}}= CartanGenerator{}(X,i);
[g_1,g_{- 1}] 
```
Lie bracket of elements of semisimple Lie algebra. 

*[]* [LieBracketSwapTermsIfNeeded] {CalculatorFunctionsBinaryOps::lieBracketSwapTermsIfNeeded}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5bb%2c%20a%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
[b, a]
```
Swaps terms in Lie brackets if needed. 

*[]* [LieBracketJacobiIfNeeded] {CalculatorFunctionsBinaryOps::lieBracketJacobiIdentityIfNeeded}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5b2a%2c3b%5d%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
[2a,3b] 
```
Extracts constants from Lie brackets. 

*=:* [standardIsDenotedBy] {CalculatorBasics::standardIsDenotedBy}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%3ay%3b%5cny%3b%5cnz%3b%5cnz%3dy%3b%5cnz%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x =:y;
y;
z;
z=y;
z
```
The operation =: is the "is denoted by" operation. The expression a =:b always reduces to a =b. In addition to the transformation, the pair of expressions a, b is registered in a special global "registry". This has the following effect. Every time the expression b is met, it is displayed on the screen as a. We note that subsequent occurrences of the expression a will first be replaced by b (as mandated by the a =b command), but then displayed on the screen as a.

Operator or function ^ is overloaded with 35 total handlers.

*^* [EulerFormula] {CalculatorFunctionsTrigonometry::eulerFormulaAsLaw}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22EulerFormula%5c%22%29%3b%5cne%5e%7bi%20x%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("EulerFormula");
e^{i x}
```
Trigonometrizes an exponential expression using Euler's formula.

*^* [PowerImaginaryUnit] {CalculatorFunctions::powerImaginaryUnit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22PowerImaginaryUnit%5c%22%29%3b%5cni%5e%7b-50%21%2b%201%7d%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("PowerImaginaryUnit");
i^{-50!+ 1}; 
```
Raises imaginary unit to an integer power. 

*^* [DegreesToRadians] {CalculatorFunctions::degreesToRadians}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%2230%5e%5c%5ccirc%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
30^\circ
```
Converts degrees to radians

*^* [LimitBoundaryNotationPower] {CalculatorFunctions::handleUnderscorePowerLimits}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5climits_a%5eb%3b%5cn%5c%5climits%5eb_a%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\limits_a^b;
\limits^b_a
```
Handles expressions of the form \limits_a^b

*^* [PowerIntegerByInteger] {CalculatorFunctionsBinaryOps::powerRationalByInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%7b3%5e3%7d%5e3%3b%5cn3%5e%7b3%5e3%7d%3b%5cn3%5e3%5e3%3b%5cn0%5e3%3b%5cn0%5e%7b-3%7d%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
{3^3}^3;
3^{3^3};
3^3^3;
0^3;
0^{-3}; 
```
Raises rational to power, provided the power is a small integer. 

*^* [PowerRationalByRational] {CalculatorFunctionsBinaryOps::powerRationalByRationalReducePrimeFactors}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%2832%29%5e%7b-4%2f5%7d%3b%5cn%284%2f9%29%5e%7b17%2f3%7d%3b%5cn%2812%2f7%29%5e%7b7%2f2%7d%3b%5cn12%5e%7b2%2f3%7d%3b%5cn12%5e%7b-2%2f3%7d%3b%5cn5%5e%7b-%201%2f3%7d%3b%5cn5%5e%7b-%201%2f3%7d-%5c%5csqrt%5b3%5d%7b5%5e-%201%7d%3b%5cn%5c%5csqrt%5b20%5d%7b200%21%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(32)^{-4/5};
(4/9)^{17/3};
(12/7)^{7/2};
12^{2/3};
12^{-2/3};
5^{- 1/3};
5^{- 1/3}-\sqrt[3]{5^- 1};
\sqrt[20]{200!}
```
If a rational number is small enough to factor, reduces the rational exponents of the rational number. 

*^* [PowerRationalByRationalOutputAlgebraic] {CalculatorFunctionsBinaryOps::powerRationalByRationalOutputAlgebraic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%2254%5e%7b1%2f2%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
54^{1/2}
```
If the rational power is small enough, converts to an algebraic number. At the moment works with rational powers whose denominator is 2, may be extended to larger powers in the future. 

*^* [PowerDoubleToDouble] {CalculatorFunctionsBinaryOps::powerDoubleOrRationalToDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22f%7b%7d%7b%7bx%7d%7d%3dx%5e3%2bp%20x%20%2bq%3b%5cnXcardano%20%3d%20%28%20-q%2f2%2b%20%28q%5e2%2f4%2bp%5e3%2f27%29%5e%281%2f2%29%29%5e%281%2f3%29%20%2b%28%20-q%2f2-%20%28q%5e2%2f4%2bp%5e3%2f27%29%5e%281%2f2%29%29%5e%281%2f3%29%3b%5cnq%20%3d%20DoubleValue%7b%7d1%3b%5cnp%20%3d%20DoubleValue%7b%7d1%3b%5cnXcardano%3b%5cnf%7b%7dx%3b%5cnf%7b%7dXcardano%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
f{}{{x}}=x^3+p x +q;
Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) +( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);
q = DoubleValue{}1;
p = DoubleValue{}1;
Xcardano;
f{}x;
f{}Xcardano 
```
Calls the built-in cpp functions to approximately raise a double to a power, provided either the base or the exponent is a double, and provided that the base is non-negative. 

*^* [TransposePowerNotation] {CalculatorFunctionsBinaryOps::powerSequenceOrMatrixByT}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20%281%2c2%29%5et%3b%5cnX-Transpose%7b%7d%281%2c2%29%3b%5cn%5c%5cbegin%7bpmatrix%7d1%262%5c%5c%5c%5c%203%26%205%5c%5cend%7bpmatrix%7d%5et%20%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = (1,2)^t;
X-Transpose{}(1,2);
\begin{pmatrix}1&2\\ 3& 5\end{pmatrix}^t  
```
Provided the exponent is t or T, calls the Transpose function on the base.

*^* [PowerMatrixBuiltInByInteger] {CalculatorFunctionsBinaryOps::powerMatBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20%5c%5cbegin%7bpmatrix%7d%200%20%26%201%20%5c%5c%5c%5c%201%20%261%5c%5cend%7bpmatrix%7d%3b%5cnX%5e5%3b%5cnX%5e%7b-5%7d%3b%5cnf%7b%7d%28%7b%7ba%7d%7d%2c%20%7b%7bb%7d%7d%29%3da%20Polynomial%28%20x%20%29%2bb%3b%5cna%3dFunctionToMatrix%28f%2c2%2c2%29%3b%5cnb%3da%5e%7b-1%7d%3b%5cna%2ab%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = \begin{pmatrix} 0 & 1 \\ 1 &1\end{pmatrix};
X^5;
X^{-5};
f{}({{a}}, {{b}})=a Polynomial( x )+b;
a=FunctionToMatrix(f,2,2);
b=a^{-1};
a*b
```
If the base is a matrix and the exponent is a rational number: 1. If the base is not square, returns error. 2. If the base is (a) square, (b) has a supported built-in coefficient type and (c) the exponent is a small integer, raises the base to the corresponding power. If the power is 0 or negative and the determinant of the matrix is zero, returns error. As of writing, the supported built-in coefficient types are: (1) rational numbers;(2) algebraic numbers; (3) polynomial and rational functions in one variable.

*^* [PowerMatrixNumbersByLargeIntegerIfPossible] {CalculatorFunctionsBinaryOps::powerMatrixNumbersByLargeIntegerIfPossible}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%5c%5cbegin%7bpmatrix%7d%201%20%26%20-1%20%5c%5c%5c%5c%204%20%26%20-3%5c%5cend%7bpmatrix%7d%3b%20X%5e%7b-55%21-%201%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X =\begin{pmatrix} 1 & -1 \\ 4 & -3\end{pmatrix}; X^{-55!- 1}
```
Attempts to exponentiate matrix by a large power. 

*^* [PowerMatrixNumbersBySmallIntegerIfPossible] {CalculatorFunctionsBinaryOps::powerMatrixExpressionsBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%5c%5cbegin%7bpmatrix%7d%20a%20%26%20b%20%5c%5c%5c%5c%20t%20%2bq%20%26r%5c%5cend%7bpmatrix%7d%3b%5cnX%5e%7b2%7d%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X =\begin{pmatrix} a & b \\ t +q &r\end{pmatrix};
X^{2} 
```
Attempts to exponentiate a matrix of expressions, if the exponent is small. 

*^* [PowerElementZmodPToInteger] {CalculatorFunctionsBinaryOps::powerElementZmodPByInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%203%20mod%207%3b%5cna%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = 3 mod 7;
a^2
```
Raises an element of Z mod p to an integer power. 

*^* [PowerToLog] {CalculatorFunctions::powerExponentToLog}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22e%5e%7b%5c%5cln%20x%7d%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
e^{\ln x}; 
```
Replaces e^{\ln x} by x.

*^* [PowerAnyToZero] {CalculatorFunctions::powerAnyToZero}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20x%5e0%3b%5cnx%20%3d%200%3b%5cnA%3b%5cnB%3dx%5e0%3b%5cn0%5e0%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = x^0;
x = 0;
A;
B=x^0;
0^0; 
```
Replaces p^0 by 1. Notice that 0^0 is defined to be 1.An explanation of that definition was given, among others, by Donald Knuth; the argument boils down to making the Newton binomial formula work for (a+0)^n. 

*^* [PowerPolynomialBySmallInteger] {CalculatorFunctionsBinaryOps::powerPolynomialBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20Polynomial%7b%7dx%3b%5cny%20%3d%20Polynomial%7b%7dy%3b%5cn%28x%20%2b2y%2bx%20y%2bx%5e2%2b3y%5e2%29%5e3%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = Polynomial{}x;
y = Polynomial{}y;
(x +2y+x y+x^2+3y^2)^3
```
Raises polynomial to small integer power. 

*^* [PowerPolynomialAlgebraicNumbersBySmallInteger] {CalculatorFunctionsBinaryOps::powerAlgebraicNumberPolynomialBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20Polynomial%7b%7dx%3b%5cny%20%3d%20Polynomial%7b%7dy%3b%5cn%28x%20%2b%5c%5csqrt%7b2%7dy%2bx%20y%2bx%5e2%2b%5c%5csqrt%7b3%7dy%5e2%29%5e3%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = Polynomial{}x;
y = Polynomial{}y;
(x +\sqrt{2}y+x y+x^2+\sqrt{3}y^2)^3
```
Raises a polynomial over the algebraic numbers to small integer power. 

*^* [PowerRationalFractionByInteger] {CalculatorFunctionsBinaryOps::powerRationalFractionBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20MakeRationalFunction%7b%7dx%3b%5cny%20%3d%20MakeRationalFunction%7b%7dy%3b%5cn%28x%2fy%29%5e3%3b%5cn%28x%2fy%29%5e-3%3b%5cn%28x%2fy%29%5e3-%28x%2fy%29%5e-3%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = MakeRationalFunction{}x;
y = MakeRationalFunction{}y;
(x/y)^3;
(x/y)^-3;
(x/y)^3-(x/y)^-3
```
Raises rational function to small integer power. 

*^* [PowerPolynomialModuloIntegerBySmallInteger] {CalculatorFunctionsBinaryOps::powerPolynomialModuloIntegerBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20PolynomialModP%7b%7d%28x%5e2%20%2b%20x%20%2b%201%2c%207%29%3b%5cna%5e20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = PolynomialModP{}(x^2 + x + 1, 7);
a^20
```
Raises polynomial modulo an integer to a small integer power. 

*^* [PowerPolynomialModPModuloPolynomialModPBySmallInteger] {CalculatorFunctionsBinaryOps::powerPolynomialModPModuloPolynomialModPBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22p%20%3d%20x%5e2%20%2b%203x%20%2b%201%3b%5cnq%20%3d%20x%5e3%20%2bx%5e2%20%2b%203%20x%20%2b%205%3b%5cnmodulusComposite%20%3d%20p%20%2a%20q%3b%5cnmodulo%20%3d%20PolynomialModP%28modulusComposite%2c%207%29%3b%5cna%20%3d%20PolynomialModP%7b%7d%28x%2b2%2c%207%29%20mod%20modulo%3b%5cna%5e%287%20%20%29%3b%5cna%5e%2849%29%3b%5cna%5e%287%5e3%20%29%3b%5cna%5e%287%5e4%20%29%3b%5cna%5e%287%5e5%20%29%3b%5cnr%20%3d%20a%5e%28%20%287%5e6%20-%201%29%20%2f2%29%3b%5cns%20%3d%20PolynomialModP%28%20r%20-%201%2c%207%29%3b%5cnt%20%3d%20PolynomialModP%28r%2c%207%29%3b%5cnu%20%3d%20PolynomialModP%28r%20%2b%201%2c%207%29%3b%5cnGCDPoly%28s%2c%20modulo%29%3b%5cnGCDPoly%28t%2c%20modulo%29%3b%5cnGCDPoly%28u%2c%20modulo%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
p = x^2 + 3x + 1;
q = x^3 +x^2 + 3 x + 5;
modulusComposite = p * q;
modulo = PolynomialModP(modulusComposite, 7);
a = PolynomialModP{}(x+2, 7) mod modulo;
a^(7  );
a^(49);
a^(7^3 );
a^(7^4 );
a^(7^5 );
r = a^( (7^6 - 1) /2);
s = PolynomialModP( r - 1, 7);
t = PolynomialModP(r, 7);
u = PolynomialModP(r + 1, 7);
GCDPoly(s, modulo);
GCDPoly(t, modulo);
GCDPoly(u, modulo);
```
Raises polynomial modulo an integer to a small integer power. 

*^* [PowerAlgebraicNumberBySmallInteger] {CalculatorFunctionsBinaryOps::powerAlgebraicNumberBySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%203%2f2%3b%5cnb%20%3d%20-%2015%2f2%3b%5cnc%20%3d%2033%2f4%3b%5cnx%20%3d%20%28-b%2b%5c%5csqrt%7b%7d%28b%5e2-4a%20c%29%29%2f%282a%29%3b%5cnB%3d%20c%2ba%20x%5e%7b2%7d%2bb%20x%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = 3/2;
b = - 15/2;
c = 33/4;
x = (-b+\sqrt{}(b^2-4a c))/(2a);
B= c+a x^{2}+b x;
```
Raises algebraic number to small integer or half-integer power. 

*^* [RadicalAlgebraicNumberPositiveDefault] {CalculatorFunctionsBinaryOps::radicalAlgebraicNumberPositiveDefault}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csqrt%7b3%2b2%5c%5csqrt%5b2%5d%7b2%7d%7d%3b%5cn%287-5%5c%5csqrt%7b2%7d%29%5e%7b7%2f3%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sqrt{3+2\sqrt[2]{2}};
(7-5\sqrt{2})^{7/3}
```
Takes the radical of an algebraic number, if the algebraic number is a positive number that whose radical lies in the underlying algebraic closure.

*^* [PowerFloatingToPower] {CalculatorFunctionsBinaryOps::powerDoubleOrRationalToDoubleOrRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22f%7b%7d%7b%7bx%7d%7d%3dx%5e3%2bp%20x%20%2bq%3b%5cnXcardano%20%3d%20%28%20-q%2f2%2b%20%28q%5e2%2f4%2bp%5e3%2f27%29%5e%281%2f2%29%29%5e%281%2f3%29%20%2b%28%20-q%2f2-%20%28q%5e2%2f4%2bp%5e3%2f27%29%5e%281%2f2%29%29%5e%281%2f3%29%3b%5cnq%3d%20DoubleValue%7b%7d1%3b%5cnp%20%3d%20DoubleValue%7b%7d1%3b%5cnXcardano%3b%5cnf%7b%7dx%3b%5cnf%7b%7dXcardano%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
f{}{{x}}=x^3+p x +q;
Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) +( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);
q= DoubleValue{}1;
p = DoubleValue{}1;
Xcardano;
f{}x;
f{}Xcardano 
```
Calls the built-in cpp functions to approximately raise a double to a power, provided either the base or the exponent is a double. If the base is negative and the exponent is rational with odd denominator, the exponent is evaluated to the corresponding real negative root. 

*^* [PowerPowerToPower] {CalculatorBasics::associateExponentExponent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%5em%29%5en%3b%5cn%28%28ln%283%29%29%5em%29%5en%3b%5cn%28%28ln%280.5%29%29%5em%29%5en%3b%5cn%28a%5em%29%5e2%3b%5cn%28a%5e2%29%5em%3b%5cn%28a%5e%7b1%2f2%7d%29%5e2%3b%5cn%28a%5e%7b2%7d%29%5e%7b1%2f2%7d%3b%5cn%28a%3e0%29%3d1%3b%5cnResetBuiltInHandlerCache%28%29%3b%5cn%28a%5e%7b2%7d%29%5e%7b1%2f2%7d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a^m)^n;
((ln(3))^m)^n;
((ln(0.5))^m)^n;
(a^m)^2;
(a^2)^m;
(a^{1/2})^2;
(a^{2})^{1/2};
(a>0)=1;
ResetBuiltInHandlerCache();
(a^{2})^{1/2};
```
If the rule doesn't break over the complex numbers, substitutes (a^b)^c with a^{b*c}. The rule acts when one of the following holds:
1) a is real and positive;
2) a>0 or a\geq 0 equals 1 (those can be user-defined, for example via (a>0)=1;)
3) c is an integer;
4) a evaluates to real (double), b and c are rational and b*c is an even integer. 

*^* [DistributeExponent] {CalculatorFunctions::distributeExponent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2ab%29%5en%3b%5cn%28%5c%5csqrt%282%29%2ab%29%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a*b)^n;
(\sqrt(2)*b)^2
```
If a is a positive constant, substitutes (a*b)^c with a^c b^c.

*^* [PowerToOne] {Calculator::outerPowerRaiseToFirst}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%5e1%20%2b%20x%5e2%3b%5cnA%5e1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x^1 + x^2;
A^1
```
Realizes the tranformation {{anything}}^1=a. 

*^* [OperatorBoundsSuperscript] {CalculatorFunctions::operatorBounds}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%5c%5csum_a%5eb%3b%5cnLispify%28A%29%3b%5cnPlotExpressionTree%28A%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A =\sum_a^b;
Lispify(A);
PlotExpressionTree(A);
```
Replaces \sum_a^b by (\sum, a, b) .

*^* [PowerEllipticCurveElementRationalByInteger] {CalculatorFunctionsBinaryOps::powerEllipticCuveElementByInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g%20%3d%20ElementEllipticCurveNormalForm%28y%5e2%20%3d%20x%5e3%20-%20x%20%2b1%2c%20x%20%3d%203%2c%20y%20%3d%205%29%3b%5cng%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);
g^2
```
Exponentiates an elliptic curve element by an integer. 

*^* [PowerEllipticCurveElementZmodPByInteger] {CalculatorFunctionsBinaryOps::powerEllipticCuveElementByInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g%20%3d%20ElementEllipticCurveNormalForm%28y%5e2%20%3d%20x%5e3%20-%20x%20%2b1%2c%20x%20%3d%203%2c%20y%20%3d%205%29%3b%5cng%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);
g^2
```
Exponentiates an elliptic curve element by an integer. 

*^* [PowerElementWeylAlgebraBySmallInteger] {CalculatorFunctionsBinaryOps::powerEWABySmallInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial%2c%20x%29%3b%5cnx%20%3d%20ElementWeylAlgebraPoly%7b%7d%28%5c%5cpartial%2c%20x%29%3b%20%5cna%20%3dx%5c%5cpartial%3b%20%5cna%5e10%3b%5cn%5c%5cpartial%20x%5e%7b3%2f2%7d%3b%5cn%5c%5cpartial%5e%7b3%2f2%7d%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial = ElementWeylAlgebraDO{}(\partial, x);
x = ElementWeylAlgebraPoly{}(\partial, x); 
a =x\partial; 
a^10;
\partial x^{3/2};
\partial^{3/2} x
```
Raises element of Weyl algebra to integer power. 

*^* [PowerWeylGroupElementByInteger] {CalculatorFunctionsBinaryOps::powerWeylGroupElementByInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20MakeElementWeylGroup%7b%7d%28f_4%2c%201%29%3b%5cnb%20%3d%20MakeElementWeylGroup%7b%7d%28f_4%2c%202%29%3b%5cnc%20%3d%20MakeElementWeylGroup%7b%7d%28f_4%2c%203%29%3b%5cnd%20%3d%20MakeElementWeylGroup%7b%7d%28f_4%2c%204%29%3b%5cnx%20%3d%20a%20b%20c%20d%20%20b%20a%3b%5cnx%5e1000001%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = MakeElementWeylGroup{}(f_4, 1);
b = MakeElementWeylGroup{}(f_4, 2);
c = MakeElementWeylGroup{}(f_4, 3);
d = MakeElementWeylGroup{}(f_4, 4);
x = a b c d  b a;
x^1000001
```
Exponentiates element of a Weyl group to an integer power. 

*^* [PowerElementUniversalEnvelopingBySmallInteger] {CalculatorFunctionsBinaryOps::powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28G_2%2c%20i%29%3b%20h_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28G_2%2c%20i%29%20%3b%5cn%20%28g_1%2bg_2%29%5e2%2b%20g_1%5e%7b1%2f2%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(G_2, i); h_{{i}}= CartanGenerator{}(G_2, i) ;
 (g_1+g_2)^2+ g_1^{1/2}
```
Raises element of universal enveloping to integer power. If the exponent is non-positive integer but the element of the UE is a single generator with coefficient 1, the exponent will be carried out formally. 

*^* [PowerUEelementToPolyPower] {CalculatorFunctionsBinaryOps::powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28G_2%2c%20i%29%3b%20h_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28G_2%2c%20i%29%20%3b%5cn%28%28%28%28g_1%29%5e%7bPolynomial%7b%7dx%7d%29%5e%7bPolynomial%7b%7dy%7d%29%20%2bg_2%29%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(G_2, i); h_{{i}}= CartanGenerator{}(G_2, i) ;
((((g_1)^{Polynomial{}x})^{Polynomial{}y}) +g_2)^2
```
Provided that an element of Universal Enveloping algebra is a single generator (raised to arbitrary formal polynomial power) with coefficient 1, raises (formally) the element of the UE to arbitrary polynomial power. 

*^* [PowerUEelementToRFPower] {CalculatorFunctionsBinaryOps::powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28G_2%2c%20i%29%3b%20h_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28G_2%2c%20i%29%3b%5cn%20%28%28%28%28g_1%29%5e%7bPolynomial%7b%7dx%7d%29%5e%7bPolynomial%7b%7dy%7d%29%20%2bg_2%29%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(G_2, i); h_{{i}}= CartanGenerator{}(G_2, i);
 ((((g_1)^{Polynomial{}x})^{Polynomial{}y}) +g_2)^2
```
Provided that an element of Universal Enveloping algebra is a single generator (raised to arbitrary formal RF power) with coefficient 1, raises (formally) the element of the UE to arbitrary RF power. 

*^* (_composite_) [ApowerBevaluatedAtC] {CalculatorFunctions::compositeAPowerBEvaluatedAtC}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5ctan%5e2%20x%3b%5cn%28f%5e-2%29%20%7b%7dx%3b%5cn%28f%5e-%201%29%7b%7dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\tan^2 x;
(f^-2) {}x;
(f^- 1){}x 
```
Provided that n is not equal to - 1 and f is not a sequence, use the rule ({{f}}^{{n}}){}{{x}}= (f{}x)^n.

*\geq* [greaterThanOrEqualTo] {CalculatorFunctions::greaterThanOrEqualTo}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20%282%3e%3dx%29%3b%5cnx%20%3d1%3b%5cnA%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = (2>=x);
x =1;
A
```
Greater than or equal to operation. 

*\leq* [lessThanOrEqualTo] {CalculatorFunctions::lessThanOrEqualTo}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%5c%5cleq%20b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a \leq b
```
Transforms \(a\leq b\) to \(b\geq a\).

*\>* [GreaterThan] {CalculatorFunctions::greaterThan}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x_%7b%7bi%7d%7d%20%2a%20x_%7b%7bj%7d%7d%20%2a%20%7b%7ba%7d%7d%20%3aif%28i%20%3e%20j%29%20%3d%20x_j%20%2a%20x_i%20%2a%20a%3b%5cnx_%7b%7bi%7d%7d%2ax_%7b%7bj%7d%7d%3a%20if%20%28i%20%3e%20j%29%20%3d%20x_j%20%2a%20x_i%3b%5cn%28x_2%20%2a%20x_1%20-%20x_1%20%2a%20x_3%29%28x_1%20x_5%20%2b%20x_5%20x_4%20x_2%29%3b%5cnx_5x_4x_3x_2x_1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x_{{i}} * x_{{j}} * {{a}} :if(i > j) = x_j * x_i * a;
x_{{i}}*x_{{j}}: if (i > j) = x_j * x_i;
(x_2 * x_1 - x_1 * x_3)(x_1 x_5 + x_5 x_4 x_2);
x_5x_4x_3x_2x_1
```
Greater than: has similar action to the less than sign. The following example shows an implementation of commutativity. 

*\<* [LessThan] {CalculatorFunctions::lessThan}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%223%20%3c%204%3b%5cn5%20%3c%204%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
3 < 4;
5 < 4
```
If both the left hand side and the right hand side are rational, replaces the expression by 1 if the left number is less than the right, else replaces the expression by 0.

*==* [IsEqualTo] {CalculatorFunctions::outerEqualEqual}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25NumberColors%5cn%20%20A%20%3d%20%28a%20%3d3%2c%20b%3d4%29%3b%5cnB%20%3d%20%28a%20%3d%20%28sqrt%283%29%29%5e2%2c%20b%20%3d%204%29%3b%5cnA%20%3d%3d%20B%3b%5cnx%20%3d%3d%20y%3b%5cnx%20%3d%3d%201%3b%5cnIsEqualToX%7b%7d%7b%7ba%7d%7d%20%3d%20%28a%20%3d%3d%20x%29%3b%5cnIsEqualToX%7b%7dy%3b%5cnIsEqualToX%7b%7dx%3b%5cnIsEqualToX%7b%7d1%3b%5cnx%20%3d1%3b%5cnIsEqualToX%7b%7d1%3b%5cnz%3d%20%5c%5csqrt%7b%7d1%3b%5cnz%3d%3d1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%NumberColors
  A = (a =3, b=4);
B = (a = (sqrt(3))^2, b = 4);
A == B;
x == y;
x == 1;
IsEqualToX{}{{a}} = (a == x);
IsEqualToX{}y;
IsEqualToX{}x;
IsEqualToX{}1;
x =1;
IsEqualToX{}1;
z= \sqrt{}1;
z==1
```
If either the left or the right argument contains a bound variable does nothing. Else evaluates to 1 if the left argument equals mathematically the right argument.

*===* [IsEqualExpressionTree] {CalculatorFunctions::equalEqualEqual}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%222%2b3%20%3d%3d%205%3b%5cnFreeze%7b%7d%282%20%2b%203%29%20%3d%3d%20Freeze%7b%7d5%3b%5cnFreeze%7b%7d%282%20%2b%203%29%20%3d%3d%3d%20Freeze%7b%7d5%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
2+3 == 5;
Freeze{}(2 + 3) == Freeze{}5;
Freeze{}(2 + 3) === Freeze{}5
```
Returns 1 if both sides have identical expression trees, 0 otherwise. 

Operator or function \cup is overloaded with 4 total handlers.

*\cup* [UnionUnionIntervals] {CalculatorFunctionsIntervals::unionUnionIntervals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%5b3%2c%203%5d%20%5c%5ccup%20%28%20%5b3%2c%207%5d%20%5c%5ccup%20%5b6%2c%208%5d%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
[3, 3] \cup ( [3, 7] \cup [6, 8] );
```
In for the expression a \cup (b\cup c) the expression a\cup b can be reduced to d, replaces a\cup(b\cup c) by a\cup d. 

*\cup* [UnionIntervals] {CalculatorFunctionsIntervals::unionIntervals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%5b3%2c7%5d%20%5c%5ccup%20%5b6%2c8%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
[3,7] \cup [6,8);
```
If the union of two intervals is one interval, replaces the interval. 

*\cup* [UnionEmptySet] {CalculatorFunctionsListsAndSets::unionEmptySet}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%5b3%2c%207%29%20%5c%5ccup%20%5c%5cemptyset%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
[3, 7) \cup \emptyset;
```
Takes union with the empty set. 

*\cup* [\cup] {CalculatorFunctionsListsAndSets::union}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%5c%5ccup%20%28MakeSequence%7b%7d%20x%20%5c%5ccup%20MakeSequence%7b%7dx%20%5c%5ccup%20%28a%2cb%2cx%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x\cup (MakeSequence{} x \cup MakeSequence{}x \cup (a,b,x))
```
If all arguments of \cup are of type list, substitutes the expression with a list containing the union of all members (with repetition). If a flag is set requesting that (a,b) is interpreted as an interval, does nothing if either of the two sequences has two elements.

*\sqcup* [\sqcup] {CalculatorFunctionsListsAndSets::unionNoRepetition}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28x%2cy%2cx%29%5c%5csqcup%281%2cx%2cy%2c2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(x,y,x)\sqcup(1,x,y,2)
```
If all arguments of \sqcup are of type list, substitutes the expression with a list containing the union of all members; all repeating members are discarded.

Operator or function \cap is overloaded with 4 total handlers.

*\cap* [IntersectUnion] {CalculatorFunctionsListsAndSets::intersectUnion}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%28%5b3%2c%207%29%20%5c%5ccup%20%289%2c%2010%29%29%20%5c%5ccap%20%284%2c%208.5%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
([3, 7) \cup (9, 10)) \cap (4, 8.5);
```
Intersects with the empty set. 

*\cap* [InersectEmptySet] {CalculatorFunctions::intersectEmptySet}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%20%20%5b3%2c7%29%5c%5ccap%20%5c%5cemptyset%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
  [3,7)\cap \emptyset;
```
Intersects with the empty set. 

*\cap* [InersectIntervals] {CalculatorFunctionsIntervals::intersectIntervals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%5b3%2c%207%29%20%5c%5ccup%20%5b6%2c%208%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
[3, 7) \cup [6, 8);
```
Intersects two intervals. 

*\cap* [\cap] {CalculatorFunctionsListsAndSets::intersection}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28a%2cb%2cc%29%5c%5ccap%20%28c%2c%20d%2c%20e%29%3b%5cnx%20%3d%20%28a%2cb%2cc%29%5c%5ccap%20%28c%2c%20d%2c%20e%29%3b%5cna%20%3d%201%3b%5cnd%20%3d1%3b%5cn%28a%2c%20b%2c%20c%29%5c%5ccap%20%28c%2c%20d%2c%20e%29%3b%5cnx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(a,b,c)\cap (c, d, e);
x = (a,b,c)\cap (c, d, e);
a = 1;
d =1;
(a, b, c)\cap (c, d, e);
x
```
Intersects lists. For the time being, the output order is not specified (will be fixed in the future).

*Matrix* (_composite_) [MatrixTypeDeduction] {CalculatorConversions::outerMatrixExpressionsToMatrixOfType}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25LogEvaluation%5cnA%20%3d%20%5c%5cbegin%7bpmatrix%7d%5cn1%20%26%202%5c%5c%5c%5c%5cn3%20%26%204%5cn%5c%5cend%7bpmatrix%7d%3b%5cnB%20%3d%20%5c%5cbegin%7bpmatrix%7d%5cna%20%26%202%5c%5c%5c%5c%5cn3%20%26%204%5cn%5c%5cend%7bpmatrix%7d%3b%5cnPlotExpressionTree%20A%3b%5cnPlotExpressionTree%20B%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%LogEvaluation
A = \begin{pmatrix}
1 & 2\\
3 & 4
\end{pmatrix};
B = \begin{pmatrix}
a & 2\\
3 & 4
\end{pmatrix};
PlotExpressionTree A;
PlotExpressionTree B
```
If A is a non-typed matrix of expressions that can be converted to a typed matrix, carries out the type specialization. 

*\|* [AbsoluteValue] {CalculatorFunctionsBasic::absoluteValue}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%7c-%201%7c%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
|- 1|
```
Absolute value function 

*"* [QuoteToString] {CalculatorFunctions::quoteToString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20dog.%5c%22%3b%5cn%5c%22%5c%5cu00B0%5c%22%3b%5cn%5c%22%5c%5cu00b0%5c%22%3b%5cnM%3d1%3b%5cn%5c%22M%5c%22%3b%5cn%5c%221%2b1%5c%22%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
"The quick brown fox jumps over the lazy dog.";
"\u00B0";
"\u00b0";
M=1;
"M";
"1+1"
```
Creates a string. Transforms escape sequences to their byte representations. These may be escaped back when returned to the frontend.

*if* [if] {CalculatorFunctions::ifFrozen}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22if%20%281%2c%20x%2c%20y%29%3b%5cnif%20%281%2c%20x%2c%201%20%2f%200%29%3b%5cnif%20%282%20-%201%2c%20x%2c%201%20%2f%200%29%3b%5cnif%20%281%20%2b%201%2c%201%20%2f%200%2c%201%20%2f%200%29%3b%5cnif%20%280%2c%20x%2c%201%20%2f%200%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
if (1, x, y);
if (1, x, 1 / 0);
if (2 - 1, x, 1 / 0);
if (1 + 1, 1 / 0, 1 / 0);
if (0, x, 1 / 0);

```
Frozen if function. Similar to the IfStandard function but does not evaluate the second and third argument unless expected. More precisely, this function evaluates the first argument, keeping the second and third argument frozen. Then the first argument is compared to zero or one. 1) If it equals neither, the expression is not reduced further. 2) If the first argument equals one, the expression is replaced with its second argument. 3) If the first argument equals zero, the expression is replaced with its third argument.


Operator or function LogBase is overloaded with 3 total handlers.

*LogBase* [LogBaseNaturalToLn] {CalculatorFunctionsBasic::logarithmBaseNaturalToLn}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5clog_e%20A%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\log_e A
```
Converts \log_e A to \ln A. 

*LogBase* [LogBaseSimpleCases] {CalculatorFunctionsBasic::logarithmBaseSimpleCases}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5clog_5%2025%3b%5cnlog_5%201%3b%5cnlog_5%20%281%2f5%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\log_5 25;
log_5 1;
log_5 (1/5)
```
If the argument is a power of the base does the computation. 

*LogBase* [LogBase] {CalculatorFunctionsBasic::logarithmBase}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5clog_10%205%3b%5cn%5c%5clog%20_e%2010%3b%5cn%5c%5cln%2010%3b%5cn%5c%5clog_a%20b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\log_10 5;
\log _e 10;
\ln 10;
\log_a b
```
Logarithm function (arbitrary base). Equivalent to \log_{{a}} {{b}}= (\log b)/(\log a). 

Operator or function \int is overloaded with 19 total handlers.

*\int* [DefiniteIntegral] {CalculatorFunctionsIntegration::integrateDefiniteIntegral}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint_%7b1%7d%5e2%20%28x%5e2%2bx%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int_{1}^2 (x^2+x) dx 
```
Integrates a definite integral.

*\int* [IntegrateAfterPolynomialization] {CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%5c%5cleft%28%20%5c%5cfrac%7bx%28x%20%2b%201%29%7d%7b2%7d%20%5c%5cright%29%5e2%20dx%20%3b%5cn%5c%5cint%20x%5e-2%20dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int \left( \frac{x(x + 1)}{2} \right)^2 dx ;
\int x^-2 dx
```
Attempts to rearrange into standard polynomial form and then integrate.

*\int* [IntegratePowerRule] {CalculatorFunctionsIntegration::integrateXPowerN}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20x%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int x dx 
```
Integrates x^n dx. 

*\int* [IntegrateEpowerX] {CalculatorFunctionsIntegration::integrateEpowerAxDiffX}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20e%5e%7b5x%7d%20dx%3b%5cn%5c%5cint%20e%5e%7b-x%7d%20dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int e^{5x} dx;
\int e^{-x} dx
```
If a is a number, integrates e^{a x} dx. 

*\int* [IntegratePullConstant] {CalculatorFunctionsIntegration::integratePullConstant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%202%5c%5csqrt%7b2-x%5e2%7d%20dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int 2\sqrt{2-x^2} dx
```
Pulls out constants from integrals.

*\int* [IntegrateSqrtOneminusXsquared] {CalculatorFunctionsIntegration::integrateSqrtOneMinusXsquared}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%202%5c%5csqrt%7b2-x%5e2%7d%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int 2\sqrt{2-x^2} dx 
```
Integrates \int \sqrt{a-x^2}dx, a > 0.

*\int* [IntegrateSqrtXsquaredMinusOne] {CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%202%5c%5csqrt%7b3x%5e2-5%7d%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int 2\sqrt{3x^2-5} dx 
```
Integrates \int \sqrt{x^2-a}dx, a > 0.

*\int* [IntegrateXpowerNePowerAx] {CalculatorFunctionsIntegration::integrateXpowerNePowerAx}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%202%20x%5e%7b3%7d%20e%5e%7b5x%7d%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int 2 x^{3} e^{5x} dx 
```
Integrates by parts \int x^n e^{a x} dx, where n is a positive integer.

*\int* [IntegrateXPowerNTrigonometryAxPlusB] {CalculatorFunctionsIntegration::integrateXPowerNSineOrCosineAxPlusB}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%202%20x%5e%7b3%7d%20sin%285x%29%20dx%3b%20%5c%5cint%202%20x%5e%7b5%7d%20cos%28-5x%29%20dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int 2 x^{3} sin(5x) dx; \int 2 x^{5} cos(-5x) dx
```
Integrates by parts \int x^n cos(a x) dx and \int x^n sin(a x) dx where n is a positive integer.

*\int* [IntegrateSinPowerNCosPowerM] {CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%5c%5csin%5e%7b4%7dx%20%5c%5ccos%5e%7b6%7dx%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int \sin^{4}x \cos^{6}x dx 
```
Solves/transforms an integral of the form \int \sin^n x \cos^m x. 

*\int* [IntegrateTanPowerNSecPowerM] {CalculatorFunctionsIntegration::integrateTanPowerNSecPowerM}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22ExpressTanViaSineCosine%5c%22%2c%20%5c%22ExpressSecViaCosine%5c%22%29%3b%5cn%5c%5cint%20%5c%5ctan%5e2%20x%20%5c%5csec%5e%7b2%7dx%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("ExpressTanViaSineCosine", "ExpressSecViaCosine");
\int \tan^2 x \sec^{2}x dx 
```
Transforms an integral of the form \int \tan^n x \sec^m x.

*\int* [IntegrateSum] {CalculatorFunctionsIntegration::integrateSum}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%28x%20%2b%201%2b%5c%5csqrt%7b%7d2%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int (x + 1+\sqrt{}2) dx 
```
If the integral is of the form \int (A+B )dx, tries recursively to integrate A and B. If successful, integrates the sum in the obvious way.

*\int* [IntegratePullOutImaginaryUnit] {CalculatorFunctionsIntegration::integratePullImaginaryUnit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20i%203%20e%5e%7bi%20x%7d%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int i 3 e^{i x} dx 
```
Pulls out the imaginary unit in front of the integral. 

*\int* [IntegrateBlockIa] {CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%28%5c%5cfrac%7b3%7d%7b%28x%20%2f%202%20-%201%29%7d%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int (\frac{3}{(x / 2 - 1)}) dx 
```
Integrates building block Ia.

*\int* [IntegrateBlockIb] {CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%28%5c%5cfrac%7b2%7d%7b%283x-%201%29%5e2%7d%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int (\frac{2}{(3x- 1)^2}) dx 
```
Integrates building block Ib.

*\int* [IntegrateBlockIIaIIIa] {CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIaandIIIa}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%28%5c%5cfrac%7b3x%20%2b%202%7d%7bx%5e2%20%2b%20x%20%2b%201%7d%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int (\frac{3x + 2}{x^2 + x + 1}) dx 
```
Integrates building blocks IIa and IIIa.

*\int* [IntegrateBlockIIIb] {CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%28%5c%5cfrac%7b3x%20%2b%202%7d%7b%28x%5e2%20%2b%20x%20%2b%201%29%5e2%7d%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int (\frac{3x + 2}{(x^2 + x + 1)^2}) dx 
```
Integrates building blocks IIIb.

*\int* [IntegrateBlockIIb] {CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%28%5c%5cfrac%7b3x%20%2b%202%7d%7b%28x%5e2%20%2b%20x%20%2b%201%29%5e2%7d%29%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int (\frac{3x + 2}{(x^2 + x + 1)^2}) dx 
```
Integrates building blocks IIb.

*\int* [IntegratePartialFractions] {CalculatorFunctionsIntegration::integrateRationalFunctionSplitToBuidingBlocks}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%5c%5cfrac%7b1%7d%7bx%28x%20%2b%201%29%7d%20dx%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int \frac{1}{x(x + 1)} dx 
```
Attempts to split an integral of a rational function into building block integrals. 

*Rational* (_composite_) [ConstantFunction] {CalculatorFunctions::constantFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%220%7b%7d3%3b%5cn2%7b%7dy%3b%5cn%28%5c%5csqrt%7b%7d2%29%7b%7dx%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
0{}3;
2{}y;
(\sqrt{}2){}x;
```
If x is a constant, replaces x{}({{anything}})=x; 

*AlgebraicNumber* (_composite_) [ConstantFunctionAlgebraicNumber] {CalculatorFunctions::constantFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%220%7b%7d3%3b%5cn2%7b%7dy%3b%5cn%28%5c%5csqrt%7b%7d2%29%7b%7dx%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
0{}3;
2{}y;
(\sqrt{}2){}x;
```
If x is a constant, replaces x{}({{anything}})=x; 

*RationalFunction* (_composite_) [RationalFunctionSubstitution] {CalculatorFunctions::rationalFunctionSubstitution}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%220%7b%7d3%3b%5cn2%7b%7dy%3b%5cn%28%5c%5csqrt%7b%7d2%29%7b%7dx%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
0{}3;
2{}y;
(\sqrt{}2){}x;
```
If x is a constant, replaces x{}({{anything}})=x; 

*VPF* (_composite_) [ApplyVectorPartitionFunction] {CalculatorFunctionsVectorPartitionFunction::applyVectorPartitionFunctionFormula}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22vectors%20%3d%20%28%5cn%281%2c0%29%2c%280%2c1%29%2c%281%2c1%29%2c%281%2c2%29%2c%281%2c3%29%2c%282%2c3%29%20%29%3b%5cnf%3dVectorPartitionFunction%7b%7d%20%28vectors%29%3b%5cntarget%20%3d%20%284%2c5%29%3b%5cnAllVectorPartitions%20%28target%2c%20vectors%29%3b%5cnf%7b%7dtarget%3b%5cng%3dVectorPartitionFunction%28Sequence%7b%7d1%2cSequence%7b%7d2%2cSequence%7b%7d3%29%3b%5cng%7b%7d%2810%29%3b%5cnAllVectorPartitions%28Sequence%7b%7d10%2c%20%28Sequence%7b%7d1%2cSequence%7b%7d2%2cSequence%7b%7d3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
vectors = (
(1,0),(0,1),(1,1),(1,2),(1,3),(2,3) );
f=VectorPartitionFunction{} (vectors);
target = (4,5);
AllVectorPartitions (target, vectors);
f{}target;
g=VectorPartitionFunction(Sequence{}1,Sequence{}2,Sequence{}3);
g{}(10);
AllVectorPartitions(Sequence{}10, (Sequence{}1,Sequence{}2,Sequence{}3))
```
Applies the vector partition function formula. 

*ElementWeylAlgebra* (_composite_) [EWAactOnPoly] {CalculatorFunctions::compositeElementWeylAlgebraActOnPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20ElementWeylAlgebraPoly%7b%7d%28%5c%5cpartial%2c%20x%29%3b%5c%5cpartial%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial%2c%20x%29%3b%5cn%5c%5cpartial%7b%7d%28x%29%3b%20%5c%5cpartial%5e%7b2%7d%7b%7d%28x%5e3%2bx%5e2%29%3b%20x%7b%7d%28x%5e2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = ElementWeylAlgebraPoly{}(\partial, x);\partial = ElementWeylAlgebraDO{}(\partial, x);
\partial{}(x); \partial^{2}{}(x^3+x^2); x{}(x^2)
```
Differential operation acting on a polynomial. 

*[)* [[)] {CalculatorFunctions::intervalLeftClosedFromSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%20PlotExpressionTree%5b1%2c2%29%3b%5cnPlotExpressionTree%281%2c2%5d%3b%5cnPlotExpressionTree%5b1%2c2%5d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals PlotExpressionTree[1,2);
PlotExpressionTree(1,2];
PlotExpressionTree[1,2];
```
Internal data structure transformation: sequence ->left-closed interval.

*(]* [(]] {CalculatorFunctions::intervalRightClosedFromSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%20PlotExpressionTree%5b1%2c2%29%3b%5cnPlotExpressionTree%281%2c2%5d%3b%5cnPlotExpressionTree%5b1%2c2%5d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals PlotExpressionTree[1,2);
PlotExpressionTree(1,2];
PlotExpressionTree[1,2];
```
Internal data structure transformation: sequence ->right-closed interval.

*IntervalClosed* [IntervalClosed] {CalculatorFunctions::intervalClosedFromSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%20PlotExpressionTree%5b1%2c2%29%3b%5cnPlotExpressionTree%281%2c2%5d%3b%5cnPlotExpressionTree%5b1%2c2%5d%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals PlotExpressionTree[1,2);
PlotExpressionTree(1,2];
PlotExpressionTree[1,2];
```
Internal data structure transformation: sequence ->closed interval.

*IntervalOpen* [IntervalOpen] {CalculatorFunctions::intervalOpenFromSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cnPlotExpressionTree%20%281%2c2%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
PlotExpressionTree (1,2); 
```
Internal data structure transformation: sequence ->open interval.

*\log* [\log] {CalculatorFunctionsBasic::logarithm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5clog%7b%7d%28e%29%3b%5cn%5c%5clog%2010%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\log{}(e);
\log 10
```
Logarithm function. Gives a decimal approximation of the natural logarithm provided the input is a double number. 

Operator or function \arctan is overloaded with 2 total handlers.

*\arctan* [ArctanExact] {CalculatorFunctionsTrigonometry::arctanExact}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5carctan%280%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\arctan(0)
```
Arctan function. Tries to evaluate the arctan function. 

*\arctan* [ArctanApproximate] {CalculatorFunctionsTrigonometry::arctanApproximate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5carctan%283%2f4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\arctan(3/4)
```
Arctan function. Tries to evaluate the arctan function. 

*SolveJSON* [SolveJSON] {CalculatorEducationalFunctions::solveJSON}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SolveJSON%28x%5e2%2b2x-3%3d0%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SolveJSON(x^2+2x-3=0);

```
Tries to interpret the input as a high-school or Calculus problem and solve it. Returns its result in JSON format. Freezes its inputs.

*CompareExpressionsJSON* [CompareExpressionsJSON] {CalculatorEducationalFunctions::CompareExpressionsJSON}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CompareExpressionsJSON%28%28x%2b1%29%28x%2b2%29%2cx%5e2%2b3x%2b2%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CompareExpressionsJSON((x+1)(x+2),x^2+3x+2);

```
Compares two expressions and returns the result in JSON format. Freezes its inputs.

*SetRandomSeed* [SetRandomSeed] {CalculatorFunctions::setRandomSeed}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SetRandomSeed%28123%29%3b%5cnRandomInteger%28-%20100%2c%20100%29%3b%5cnRandomInteger%28-%20100%2c%20100%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SetRandomSeed(123);
RandomInteger(- 100, 100);
RandomInteger(- 100, 100)
```
Sets the random seed of the calculator to the given integer value

*SelectAtRandom* [SelectAtRandom] {CalculatorFunctions::selectAtRandom}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22f%3dSelectAtRandom%28%5c%5csin%2c%20%5c%5ccos%29%3b%5cng%3dSelectAtRandom%28%5c%5ctan%2c%20%5c%5ccot%2c%20%5c%5csec%2c%20%5c%5ccsc%29%3b%5cnf%7b%7dg%7b%7dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
f=SelectAtRandom(\sin, \cos);
g=SelectAtRandom(\tan, \cot, \sec, \csc);
f{}g{}x
```
Selects at random an object from a sequence.

*RandomInteger* [RandomInteger] {CalculatorFunctions::randomInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22RandomInteger%28%28-2%2c-%201%29%2c%20%282%2c5%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
RandomInteger((-2,- 1), (2,5));
```
Generates a random integer. The random integer lives in intervals given by pairs of integers. The example code generates a random number in the union of the intervals [-2, - 1], [2,5]. If the input intervals overlap the overlapped integers will be generated with higher probability. For non-overlapping intervals, the random number distribution should be approximately uniform.

*TurnOffRules* [TurnOffRules] {CalculatorFunctions::turnOffRules}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22sqrt%5c%22%29%3b%5cna%20%3d%20%5c%5csqrt%5b4%5d%7bt%7d%3b%5cnTurnOnRules%28%5c%22sqrt%5c%22%29%3b%5cna%3b%5cnTurnOffRules%28%5c%22ConvertShortDenominatorToNegativePower%5c%22%29%3b%5cnb%3d1%2ft%5e3%3bTurnOnRules%28%5c%22ConvertShortDenominatorToNegativePower%5c%22%29%3bb%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("sqrt");
a = \sqrt[4]{t};
TurnOnRules("sqrt");
a;
TurnOffRules("ConvertShortDenominatorToNegativePower");
b=1/t^3;TurnOnRules("ConvertShortDenominatorToNegativePower");b
```
Turns off computational rules.

*TurnOnRules* [TurnOnRules] {CalculatorFunctions::turnOnRules}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22sqrt%5c%22%29%3b%5cna%20%3d%20%5c%5csqrt%5b4%5d%7bt%7d%3b%5cnTurnOnRules%28%5c%22sqrt%5c%22%29%3b%5cna%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("sqrt");
a = \sqrt[4]{t};
TurnOnRules("sqrt");
a
```
Turns on computational rules.

*ResetBuiltInHandlerCache* [ResetBuiltInHandlerCache] {CalculatorFunctions::resetBuiltInHandlerCache}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22sqrt%28a%5e2%29%3b%5cna%5e%7b1%2f2%7da%5e%7b1%2f3%7d%3b%5cn%28a%3e0%29%3d1%3b%5cnsqrt%28a%5e2%29%3b%5cna%5e%7b1%2f2%7da%5e%7b1%2f3%7d%3b%5cnResetBuiltInHandlerCache%28%29%3b%5cnsqrt%28a%5e2%29%3b%5cna%5e%7b1%2f2%7da%5e%7b1%2f3%7d%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
sqrt(a^2);
a^{1/2}a^{1/3};
(a>0)=1;
sqrt(a^2);
a^{1/2}a^{1/3};
ResetBuiltInHandlerCache();
sqrt(a^2);
a^{1/2}a^{1/3};

```
Some built in handlers are affected by user-defined substitutions. This resets the cached evaluation of all built handlers. 

*TurnOnApproximations* [TurnOnApproximations] {CalculatorFunctions::turnOnRules}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffApproximations%280%29%3b%5cnln%282%29%3b%5cnTurnOnApproximations%280%29%3b%5cnln%282%29%3b%5cn%28TurnOffApproximations%200%3b%20ln%282%29%29%3b%5cnln%282%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffApproximations(0);
ln(2);
TurnOnApproximations(0);
ln(2);
(TurnOffApproximations 0; ln(2));
ln(2)
```
Turns on numerical approximations. Will wipe out the expression cache. Takes as input dummy argument. 

*TurnOffApproximations* [TurnOffApproximations] {CalculatorFunctions::turnOffApproximations}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffApproximations%280%29%3b%5cnln%282%29%2bsin%282%29%3b%5cnTurnOnApproximations%280%29%3b%5cnln%282%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffApproximations(0);
ln(2)+sin(2);
TurnOnApproximations(0);
ln(2)
```
Turns on numerical approximations. Takes as input dummy argument. 

*EvaluateSymbols* [EvaluateSymbols] {CalculatorConversions::evaluateSymbols}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%205%3b%5cnleft%20%3d%20a%3b%5cnEvaluateSymbols%28%5c%22x%5ex%20%2bax%20%2ba%20x%20%2b%5c%5cleft%28left%20%5c%5cright%29%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = 5;
left = a;
EvaluateSymbols("x^x +ax +a x +\left(left \right)")
```
Evaluates and replaces individual symbols/variables in LaTeX string. Leaves the rest of the string intact.

*IfStandard* [IfStandard] {CalculatorFunctions::ifStandard}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IfStandard%20%281%2c%20x%2c%20y%29%3b%5cnIfStandard%20%280%2c%20x%2c%20y%29%3b%5cnIfStandard%20%282%2c%20x%2c%20y%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IfStandard (1, x, y);
IfStandard (0, x, y);
IfStandard (2, x, y)
```
Standard if function. Takes 3 arguments. If first argument is true (equal to 1) then returns the second argument. If the first argument is false (equal to 0) returns the third argument. In any other situation the expression is not reduced. Important note: both outcome subexpression of IfStandard. are evaluated, as illustrated by the built-in example. 

*ConvertElementZmodPToInteger* [ConvertElementZmodPToInteger] {CalculatorFunctions::convertElementZmodPToInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%225%5e%28ConvertElementZmodPToInteger%28%209%20mod%207%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
5^(ConvertElementZmodPToInteger( 9 mod 7))
```
Converts element of Z mod p to an integer between 0 and p - 1. 

*MakeInputBox* [MakeInputBox] {CalculatorHtmlFunctions::userInputBox}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeInputBox%28name%20%3d%20a%2c%20value%20%3d%203%29%3b%5cnMakeInputBox%28name%20%3d%20b%2c%20value%20%3d%203%2c%20min%20%3d%201%2c%20max%20%3d%2011%29%3b%5cnMakeInputBox%28name%20%3d%20a%2c%20value%20%3d%205%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeInputBox(name = a, value = 3);
MakeInputBox(name = b, value = 3, min = 1, max = 11);
MakeInputBox(name = a, value = 5);

```
Creates an user input text box. 

*Thaw* [Thaw] {CalculatorFunctions::thaw}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3dFreeze%7b%7d%281%20%2b%201%29%3b%5cnThaw%20a%3b%5cnc%20%3dThaw%28Freeze%28a%2cb%29%29%3b%5cnPlotExpressionTree%20c%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a =Freeze{}(1 + 1);
Thaw a;
c =Thaw(Freeze(a,b));
PlotExpressionTree c
```
If the argument is frozen, removes the top freeze command and returns the argument, else returns the argument unchanged.

*LogEvaluationStepsDebug* [LogEvaluationStepsDebug] {CalculatorSteps::logEvaluationStepsDebug}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LogEvaluationStepsDebug%28%28x%20%2b2%29%28x%20%2b3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LogEvaluationStepsDebug((x +2)(x +3))
```
Creates an expression evaluation tree. Intended for debugging the system. 

*LogEvaluationSteps* [LogEvaluationSteps] {CalculatorSteps::logEvaluationSteps}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LogEvaluationSteps%28%28x%20%2b2%29%28x%20%2b3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LogEvaluationSteps((x +2)(x +3))
```
Show a step-by step evaluation of the calculator. Some steps my be combined or omitted for improved human readability. 

*FlattenCommandEnclosuresOneLayer* [FlattenCommandEnclosuresOneLayer] {CalculatorBasics::flattenCommandEnclosuresOneLayer}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FlattenCommandEnclosuresOneLayer%28CommandEnclosure%7b%7d%28x%20%3d%205%3b%20x%29%3b%20CommandEnclosure%7b%7d%28y%3b%20x%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FlattenCommandEnclosuresOneLayer(CommandEnclosure{}(x = 5; x); CommandEnclosure{}(y; x))
```
Flattens command enclosures. 

*ChildExpression* [ChildExpression] {CalculatorFunctions::childExpression}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ChildExpression%28e%5ex%2c%201%29%3b%5cnChildExpression%28e%5ex%2c%202%29%3b%5cnChildExpression%28e%5ex%2c%203%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ChildExpression(e^x, 1);
ChildExpression(e^x, 2);
ChildExpression(e^x, 3)
```
If defined, returns the nth child of an expression.

*ToString* [ToString] {CalculatorFunctions::expressionToString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ToString%28%20e%5ex%29%3b%5cn%5c%22e%5ex%5c%22%3b%5cn%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20dog.%5c%22%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ToString( e^x);
"e^x";
"The quick brown fox jumps over the lazy dog."
```
Transforms an arbitrary expression to its string representation. The string representation is (supposed to be) LaTeX-compatible. 

*ToUTF8String* [ToUTF8String] {CalculatorFunctions::expressiontoUTF8String}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22f%20%3d%20%5c%5csin%20%7b%7dx%20%2f%20%5c%5ccos%20%7b%7dx%3b%5cnPlot2D%28f%2c%20-%5c%5cpi%2f4%2c%20%5c%5cpi%2f4%29%20%2b%20PlotLabel%7b%7d%28%281%2c%201%29%2c%20%5c%22y%20%3d%5c%22%20%2b%20ToUTF8String%28f%29%29%3b%5cnToUTF8String%28%5c%5cpi%2f4%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
f = \sin {}x / \cos {}x;
Plot2D(f, -\pi/4, \pi/4) + PlotLabel{}((1, 1), "y =" + ToUTF8String(f));
ToUTF8String(\pi/4);
```
Transforms an arbitrary expression to a utf8-string representation. The string is supposed to look reasonable when drawn on a javascript canvas. 

*StringToAtom* [StringToAtom] {CalculatorFunctions::stringToAtom}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22StringToAtom%28%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20dog.%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
StringToAtom("The quick brown fox jumps over the lazy dog.")
```
Transforms a string to an atom. 

*Slice* [Slice] {CalculatorFunctions::sliceString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%3d%20%5c%22abcd%5c%22%3b%5cnSlice%28a%2c%200%2c2%29%3b%5cnSlice%28a%2c0%2c-%201%29%3b%5cnSlice%28a%2c1%29%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a= "abcd";
Slice(a, 0,2);
Slice(a,0,- 1);
Slice(a,1)

```
Slices a string in an interval [a,b), where a is the first included index and b the first excluded index. If b is omitted, then the entire string is sliced. If b is negative, it is replaced by its sum with the string size. 

*LogarithmBaseNCeiling* [LogarithmBaseNCeiling] {CalculatorFunctionsBasic::logarithmBaseNCeiling}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LogarithmBaseNCeiling%282%2c%20951891382051282682454493%29%3b%5cnlog_2%20951891382051282682454493%3b%5cnx%20%3d%20LogarithmBaseNCeiling%282%2c%20100%21%20%2b%201%29%3b%5cn2%5ex%20-%20%28100%21%2b1%29%3b%5cn2%5e%7bx%2b1%7d%20-%20%28100%21%2b1%29%3b%5cny%20%3d%20LogarithmBaseNCeiling%2810%2c%201000%21%20%2b%201%29%3b%5cn10%5ey%20-%201000%21%20-%201%3b%5cn10%5e%7by%20%2b%201%7d%20-%201000%21%20-%201%3b%5cnDoubleValue%28log_2%281000%21%20%2b%201%29%29%20%2b%201%3b%5cnLogarithmBaseNCeiling%282%2c%201%29%3b%5cnLogarithmBaseNCeiling%28-%202%2c%201%29%3b%5cnLogarithmBaseNCeiling%282%2c%20-%201%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LogarithmBaseNCeiling(2, 951891382051282682454493);
log_2 951891382051282682454493;
x = LogarithmBaseNCeiling(2, 100! + 1);
2^x - (100!+1);
2^{x+1} - (100!+1);
y = LogarithmBaseNCeiling(10, 1000! + 1);
10^y - 1000! - 1;
10^{y + 1} - 1000! - 1;
DoubleValue(log_2(1000! + 1)) + 1;
LogarithmBaseNCeiling(2, 1);
LogarithmBaseNCeiling(- 2, 1);
LogarithmBaseNCeiling(2, - 1);

```
Returns ceiling(log_N (input)), i.e., the smallest integer X for which input <= N^X. First argument = N = base, second argument = input.

*Numerator* [Numerator] {CalculatorFunctions::numerator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Numerator%28a%2fb%29%3b%20Numerator%281%2b%201%2fx%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Numerator(a/b); Numerator(1+ 1/x)
```
If the expression is a fraction, returns the numerator, else returns the entire expression. 

*ApplyToSubexpressionsRecurseThroughCalculusFunctions* [ApplyToSubexpressionsRecurseThroughCalculusFunctions] {CalculatorFunctions::applyToSubexpressionsRecurseThroughCalculusFunctions}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ApplyToSubexpressionsRecurseThroughCalculusFunctions%28Polynomialize%2c%20sin%20%28%28x%20%2b%201%29%5e3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ApplyToSubexpressionsRecurseThroughCalculusFunctions(Polynomialize, sin ((x + 1)^3))
```
Applies a transformation, recursively to all subexpressions of a list of known functions. 

*SubList* [SubList] {CalculatorFunctions::subList}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SubList%7b%7d%28%281%2c%202%2c%203%2c%204%29%2c%20%7b%7ba%7d%7d%20%3e%202%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SubList{}((1, 2, 3, 4), {{a}} > 2)
```
Extracts a sub-list that contains all elements that satisfy a condition given by the second argument. Use the bound variable syntax {{a}} to address the current element of the list.

*ApplyToList* [ApplyToList] {CalculatorFunctions::applyToList}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SolveForX%7b%7d%7b%7ba%7d%7d%20%3d%20SolveFor%28x%2c%20%7b%7ba%7d%7d%29%3b%5cnSolveForX%28x%5e2%20%2b%202%29%3b%5cnApplyToList%28SolveForX%2c%28x%5e2%20%2b%201%2c%20x%5e2%20-%20x%20-%202%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SolveForX{}{{a}} = SolveFor(x, {{a}});
SolveForX(x^2 + 2);
ApplyToList(SolveForX,(x^2 + 1, x^2 - x - 2))
```
Applies a transformation to each element of a list. 

*Denominator* [Denominator] {CalculatorFunctions::denominator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Denominator%28a%2fb%29%3b%5cnDenominator%281%2b%201%2fx%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Denominator(a/b);
Denominator(1+ 1/x)
```
If the expression is a fraction, returns the denominator, else returns 1. 

*Product* [Product] {CalculatorFunctions::multiplySequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Product%28a%2c%20b%29%3b%5cnProduct%7b%7d%28Sequence%7b%7da%29%3b%5cnProduct%28Sequence%7b%7da%29%3b%5cnProduct%28a%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Product(a, b);
Product{}(Sequence{}a);
Product(Sequence{}a);
Product(a)
```
Returns the product of the elements in a sequence. When used with zero arguments, returns 1.

Operator or function \sum is overloaded with 2 total handlers.

*\sum* [\sum] {CalculatorFunctions::sumSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csum%28a%2c%20b%29%3b%5cn%5c%5csum%28%29%3b%5cn%5c%5csum%7b%7d%28Sequence%7b%7da%29%3b%5cn%5c%5csum%28Sequence%7b%7da%29%3b%5cn%5c%5csum%28a%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sum(a, b);
\sum();
\sum{}(Sequence{}a);
\sum(Sequence{}a);
\sum(a)
```
Returns the sum of the elements in a sequence. When used with one element, will return that element. 

*\sum* (_composite_) [SumAsOperator] {CalculatorFunctions::sumAsOperatorToSumInternalNotation}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotExpressionTree%28Freeze%28%5c%5csum_a%5eb%20%29%7b%7d%20n%29%3b%5cnPlotExpressionTree%28%28%5c%5csum_a%5eb%20%29%7b%7d%20n%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotExpressionTree(Freeze(\sum_a^b ){} n);
PlotExpressionTree((\sum_a^b ){} n) 
```
Transforms (\sum_a^b ){} n to \sum_a^b n (internal notation). 

*MakeExpression* [MakeExpression] {CalculatorConversions::expressionFromBuiltInType}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeExpression%28Polynomial%7b%7d%28%28x%20-%202y%20%2b%20z%20-%201%29%5e2%28x%20%2b%20y%20-%20z%29%29%29%3b%5cnA%20%3d%20%5c%5cfrac%7b64%20x%5e%7b2%7d%20y%20x%20y%20%2b%2016%20x%20y%20-%20y%7d%7b-%2032%20x%5e%7b2%7d%20y%20x%5e%7b2%7d%20-%208%20x%5e%7b2%7d%20%2b%20x%7d%3b%5cnB%20%3d%20MakeRationalFunction%28A%29%3b%5cnC%20%3d%20MakeExpression%20B%3b%5cnD%20%3d%20MakeRationalFunction%20%7b%7d%28%5c%5cfrac%7b-%202x%5e%7b3%7dy%5e%7b2%7d%20-%20%5c%5cfrac%7bx%20y%7d%7b2%7d%20%2b%20%5c%5cfrac%7by%20%7d%7b32%7d%7d%7bx%5e%7b4%7dy%20%2b%20%5c%5cfrac%7bx%5e%7b2%7d%7d%7b4%7d%20-%20%5c%5cfrac%7bx%7d%7b32%7d%7d%29%3b%5cnMakeExpression%20%7b%7dD%20-%20C%3b%5cnD%20-%20B%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeExpression(Polynomial{}((x - 2y + z - 1)^2(x + y - z)));
A = \frac{64 x^{2} y x y + 16 x y - y}{- 32 x^{2} y x^{2} - 8 x^{2} + x};
B = MakeRationalFunction(A);
C = MakeExpression B;
D = MakeRationalFunction {}(\frac{- 2x^{3}y^{2} - \frac{x y}{2} + \frac{y }{32}}{x^{4}y + \frac{x^{2}}{4} - \frac{x}{32}});
MakeExpression {}D - C;
D - B;
```
Creates expression from built-in polynomial. 

*Polynomial* [Polynomial] {CalculatorConversions::getPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Polynomial%7b%7d%28%28x-2y%2bz-%201%29%5e2%28x%20%2by-z%29%29%3b%5cnPolynomial%7b%7d%28y%5e2%29-%28Polynomial%7b%7dy%29%5e2%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Polynomial{}((x-2y+z- 1)^2(x +y-z));
Polynomial{}(y^2)-(Polynomial{}y)^2
```
Creates a polynomial expression with rational coefficients. 

*PolynomialAlgebraicNumbers* [PolynomialAlgebraicNumbers] {CalculatorConversions::getPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialAlgebraicNumbers%7b%7d%28%28x%20%2b%20%5c%5csqrt%7b2%7d%29%5e2%20%28%5c%5csqrt%7b3%7dx%20-%20%5c%5csqrt%7b5%7d%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialAlgebraicNumbers{}((x + \sqrt{2})^2 (\sqrt{3}x - \sqrt{5}));
```
Creates a polynomial expression with algebraic number coefficients. 

*PolynomialModP* [PolynomialModP] {CalculatorConversions::polynomialModuloInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialModP%7b%7d%28x%5e2%2bx%2b1%2c%202%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialModP{}(x^2+x+1, 2);
```
Creates a polynomial, with modular arithmetic coefficients, i.e., coefficients in Z / p Z. First argument = expression to convert to polynomial. Second argument = the number mod which we are computing. 

*IsDifferentialOneFormOneVariable* [IsDifferentialOneFormOneVariable] {CalculatorFunctions::isDifferentialOneFormOneVariable}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsDifferentialOneFormOneVariable%28Differential%20x%29%3b%5cnIsDifferentialOneFormOneVariable%28%7b%5c%5ctext%7bd%7d%7d%20x%29%3b%5cnIsDifferentialOneFormOneVariable%28x%20%7b%5c%5ctext%7bd%7d%7d%20y%29%3b%5cnIsDifferentialOneFormOneVariable%28%5c%5cfrac%7b%5c%5ctext%7bd%7d%20y%7d%7by%7d%29%3b%5cnIsDifferentialOneFormOneVariable%281%2f%28%5c%5ctext%7bd%7d%20y%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsDifferentialOneFormOneVariable(Differential x);
IsDifferentialOneFormOneVariable({\text{d}} x);
IsDifferentialOneFormOneVariable(x {\text{d}} y);
IsDifferentialOneFormOneVariable(\frac{\text{d} y}{y});
IsDifferentialOneFormOneVariable(1/(\text{d} y));
```
Tests whether the expression is a differential form in one variable. 

*Polynomialize* [Polynomialize] {CalculatorFunctions::polynomialize}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22C%20%3d%20%28c%20a%20%2b%20a%20b%20%2b%20b%20c%20%2b%201%29%5e3%3b%5cnA%20%3d%20Polynomialize%28C%29%3b%5cnB%20%3d%20MakeExpression%28Polynomial%28C%29%29%3b%5cnA%20-%20B%3b%5cnPolynomialize%28%28a%2ba%5e-1%2bsqrt%28-1%29%29%5e2%29%3bPolynomialize%28x%5e-2%29%3b%5cnPolynomialize%28%28x%5e-%7b1%2f3%7d%2b1%29%5e2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
C = (c a + a b + b c + 1)^3;
A = Polynomialize(C);
B = MakeExpression(Polynomial(C));
A - B;
Polynomialize((a+a^-1+sqrt(-1))^2);Polynomialize(x^-2);
Polynomialize((x^-{1/3}+1)^2)
```
Polynomialize(a) is equivalent to MakeExpression(Polynomial(a)).

*IsAlgebraicRadical* [IsAlgebraicRadical] {CalculatorFunctionsAlgebraic::isAlgebraicRadical}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsAlgebraicRadical%28%5c%5csqrt%7b5%20%2b%20%5c%5csqrt%7b2%7d%7d%29%3b%5cnIsAlgebraicRadical%28%5c%5csqrt%7bx%7d%29%3b%5cnIsAlgebraicRadical%28%5c%5csqrt%7b%5c%5cpi%20%2b%20e%20%2b%201%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsAlgebraicRadical(\sqrt{5 + \sqrt{2}});
IsAlgebraicRadical(\sqrt{x});
IsAlgebraicRadical(\sqrt{\pi + e + 1})
```
Tests whether the expression is an algebraic expression obtained using radicals and the four arithmetic operations.

*IsReal* [IsReal] {CalculatorFunctions::isReal}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsReal%28%5c%5csqrt%7b5%20%2b%20%5c%5csqrt%7b-%201%7d%7d%29%3b%5cnIsReal%28%5c%5csqrt%7b%5c%5csqrt%7b5%7d%20-%201%7d%29%3b%5cnIsReal%28sqrt%28%5c%5csqrt%7b%5c%5cpi%7d%20-%202%29%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsReal(\sqrt{5 + \sqrt{- 1}});
IsReal(\sqrt{\sqrt{5} - 1});
IsReal(sqrt(\sqrt{\pi} - 2) )
```
Tests whether the expression is a real constant. 

*IsConstant* [IsConstant] {CalculatorFunctions::isConstant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsConstant%28%5c%5cpi%5e2%29%3b%5cnIsConstant%281%29%3b%5cnIsConstant%28x%29%3b%5cnIsConstant%28e%5e%7b%5c%5csin%28%5c%5cpi%5e2%20%2b%20e%20%2b%5c%5csqrt%7b2%7d%20%2b%203%29%7d%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsConstant(\pi^2);
IsConstant(1);
IsConstant(x);
IsConstant(e^{\sin(\pi^2 + e +\sqrt{2} + 3)});
```
Tests whether the expression is a constant.  

Operator or function \sin is overloaded with 5 total handlers.

*\sin* [SineOfAngleSum] {CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22SineOfAngleSum%5c%22%29%3b%5cn%5c%5csin%28a%20%2b%20b%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("SineOfAngleSum");
\sin(a + b)
```
Implementation of sin (a + b) = sin(a) cos(b) + cos(a) sin(b).

*\sin* [ExpressSinViaExponent] {CalculatorFunctionsTrigonometry::convertSineToExponent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22ExpressSinViaExponent%5c%22%2c%20%5c%22ExpressCosViaExponent%5c%22%29%3b%5cn%5c%5csin%20x%3b%5cncos%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("ExpressSinViaExponent", "ExpressCosViaExponent");
\sin x;
cos x
```
Converts sine to exponent by sin(x) = (e^{i x} - e^{- i x}) / (2 i) 

*\sin* [ExploitSinOddness] {CalculatorFunctionsTrigonometry::exploitSineOddness}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22ExploitSinOddness%5c%22%29%3b%5cnsin%28-%205x%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("ExploitSinOddness");
sin(- 5x)
```
If a is negative, converts sin (a) to -sin(-a).

*\sin* [SineExact] {CalculatorFunctionsTrigonometry::sinExact}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffApproximations%7b%7d%28%29%3b%5cn%5c%5csin%7b%7d%283.1415%29%20-%20%5c%5csin%20%5c%5cpi%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffApproximations{}();
\sin{}(3.1415) - \sin \pi
```
Sine function, evaluates only if exact. 

*\sin* [SineApproximate] {CalculatorFunctionsTrigonometry::sinApproximate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csin%7b%7d%283.1415%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sin{}(3.1415)
```
Sine function: evaluates to a decimal approximation if the input is a double number. 

Operator or function \cos is overloaded with 5 total handlers.

*\cos* [CosineOfAngleSum] {CalculatorFunctionsTrigonometry::cosineOfAngleSumToTrigonometry}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22CosineOfAngleSum%5c%22%29%3b%5cn%5c%5ccos%28a%20%2b%20b%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("CosineOfAngleSum");
\cos(a + b)
```
Implementation of cos (a + b) = cos(a)cos(b) - sin(a)sin(b).

*\cos* [ExpressCosViaExponent] {CalculatorFunctionsTrigonometry::convertCosineToExponent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22ExpressSinViaExponent%5c%22%2c%20%5c%22ExpressCosViaExponent%5c%22%29%3b%5cn%5c%5csin%20x%3b%5cncos%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("ExpressSinViaExponent", "ExpressCosViaExponent");
\sin x;
cos x
```
Converts cosine to exponent by cos(x) = (e^{i x} + e^{- i x}) / 2 

*\cos* [ExploitCosEvenness] {CalculatorFunctionsTrigonometry::exploitCosineEvenness}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOnRules%28%5c%22ExploitCosEvenness%5c%22%29%3b%5cncos%28-%205x%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOnRules("ExploitCosEvenness");
cos(- 5x)
```
If a is negative, converts cos (a) to cos(- a).

*\cos* [CosineExact] {CalculatorFunctionsTrigonometry::cosExact}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffApproximations%7b%7d%28%29%3b%5cn%5c%5ccos%20%5c%5cpi-%20%5c%5ccos%7b%7d%283.1415%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffApproximations{}();
\cos \pi- \cos{}(3.1415)
```
Cosine function. Evaluates only if exact. 

*\cos* [CosineApproximate] {CalculatorFunctionsTrigonometry::cosApproximate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffApproximations%7b%7d%28%29%3b%5cn%5c%5ccos%20%5c%5cpi-%20%5c%5ccos%7b%7d%283.1415%29%3b%5cnTurnOnApproximations%7b%7d%28%29%3b%5cn%5c%5ccos%20%5c%5cpi-%20%5c%5ccos%7b%7d%283.1415%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffApproximations{}();
\cos \pi- \cos{}(3.1415);
TurnOnApproximations{}();
\cos \pi- \cos{}(3.1415);
```
Cosine function. Evaluates to a decimal approximation if the input is a double number. 

*FourierFractionForm* [FourierFractionForm] {CalculatorFunctionsTrigonometry::fourierFractionForm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20cos%282x%29%2f%28cos%5e3%20x%20-%20sin%5e3%20x%29%3b%5cnFourierFractionForm%20A%3b%5cnFourierFractionForm%28%20%281%2btan%28theta%29%29%2f%281-tan%28theta%29%29-%20tan%20%282theta%29-%20sec%282theta%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = cos(2x)/(cos^3 x - sin^3 x);
FourierFractionForm A;
FourierFractionForm( (1+tan(theta))/(1-tan(theta))- tan (2theta)- sec(2theta))
```
Converts a trigonometric expression in one variable to fourier fraction form. 

Operator or function Differentiate is overloaded with 11 total handlers.

*Differentiate* [DifferentiateSqrt] {CalculatorFunctionsDifferentiation::differentiateSqrt}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22d%2fdx%28sqrt%28x%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
d/dx(sqrt(x));
```
Differentiation - square root function. 

*Differentiate* [DifferentiateTrigAndInverseTrig] {CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20%28%5c%5csin%20x%29%20%5c%5ccos%20x%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, (\sin x) \cos x )
```
Differentiation - product rule. 

*Differentiate* [DifferentiateChainRule] {CalculatorFunctionsDifferentiation::differentiateChainRule}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20%5c%5csin%20x%5e2%20%5c%5ccos%20%28%5c%5csin%20x%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, \sin x^2 \cos (\sin x))
```
Differentiation - chain rule.  

*Differentiate* [DifferentiateProductRule] {CalculatorFunctionsDifferentiation::differentiateAtimesB}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20f%20%2a%20g%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, f * g)
```
Differentiation - product rule.  

*Differentiate* [DifferentiateQuotientRule] {CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20f%20%2f%20g%29%3b%5cnDifferentiate%28x%2c%20f%20%2f%20g%5e5%29%3b%5cnDifferentiate%28x%2c%20f%20%2f%20g%5en%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, f / g);
Differentiate(x, f / g^5);
Differentiate(x, f / g^n)
```
Differentiation - division rule, commutative. For the time being differentiation is assumed to be over commutative rings. This might change in the future. The commutative division rule is d/dx (f/g^n) = ((d/dx f) g - n f (d/dx g))/g^{n + 1}.

*Differentiate* [DifferentiateSum] {CalculatorFunctionsDifferentiation::differentiateAplusB}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20f%20%2b%20g%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, f + g)
```
Differentiation - sum rule.

*Differentiate* [DifferentiateConstant] {CalculatorFunctionsDifferentiation::differentiateConstant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%205%20%29%3b%5cnd%2fdx%20%5c%5csqrt%7b5%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, 5 );
d/dx \sqrt{5}
```
Differentiation - constant rule. 

*Differentiate* [DifferentiateIdentity] {CalculatorFunctionsDifferentiation::differentiateX}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20x%20%29%5cn%3bd%2fdx%20x%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, x )
;d/dx x
```
Differentiation - d/dx x = 1.

*Differentiate* [DifferentiateConstPower] {CalculatorFunctionsDifferentiation::differentiateConstPower}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20x%5e2%20%29%3b%5cnd%2fdx%20x%5e-%201%3b%5cnd%2fdt%20t%5e%7b%5c%5csqrt%7b2%7d%7d%3b%5cnd%2fdc%20c%5e%7bDoubleValue%203.14%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, x^2 );
d/dx x^- 1;
d/dt t^{\sqrt{2}};
d/dc c^{DoubleValue 3.14}
```
Differentiation - d/dx x^n = n x^{n - 1}.

*Differentiate* [DifferentiateApowerB] {CalculatorFunctionsDifferentiation::differentiateAPowerB}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differentiate%28x%2c%20x%5e2%29%3b%5cnd%2fdx%20x%5e-%201%3b%5cnd%2fdt%20t%5e%7b%5c%5csqrt%7b2%7d%7d%3b%5cnd%2fdc%20c%5e%7bDoubleValue%203.14%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differentiate(x, x^2);
d/dx x^- 1;
d/dt t^{\sqrt{2}};
d/dc c^{DoubleValue 3.14}
```
Differentiation: d/dx a^b = d/dx(e^{b\log a}) = a^b d/dx(b\log a) .

*Differentiate* (_composite_) [DifferentiateLog] {CalculatorFunctions::compositeDifferentiateLog}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22d%2fdx%20%28%5c%5clog%20x%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
d/dx (\log x)
```
Differentiates log.

*MakeRationalFunction* [MakeRationalFunction] {CalculatorConversions::rationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeRationalFunction%7b%7d%28x_1%20%2b%20MakeRationalFunction%7b%7dx_1%20%2b%20x_2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeRationalFunction{}(x_1 + MakeRationalFunction{}x_1 + x_2)
```
Creates a built-in rational function.

*MakeMatrix* [MakeMatrix] {CalculatorConversions::makeMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s_1%20%3d%20MakeMatrix%7b%7d%28%28-%201%2c-%201%2c%200%2c%200%29%2c%20%280%2c%201%2c%200%2c%200%29%2c%20%280%2c%200%2c%201%2c%200%29%2c%20%280%2c%200%2c%200%2c%201%29%29%3b%5cns_2%20%3d%20MakeMatrix%7b%7d%28%281%2c%200%2c%200%2c%200%29%2c%20%28-%201%2c%20-%201%2c%20-%201%2c%200%29%2c%20%280%2c%200%2c%201%2c%200%29%2c%20%280%2c%200%2c%200%2c%201%29%29%3b%5cns_3%20%3d%20MakeMatrix%7b%7d%28%281%2c%200%2c%200%2c%200%29%2c%20%280%2c%201%2c%200%2c%200%29%2c%20%280%2c%20-%202%2c%20-%201%2c%20-%201%29%2c%20%280%2c%200%2c%200%2c%201%29%29%3b%5cns_4%20%3d%20MakeMatrix%7b%7d%28%281%2c%200%2c%200%2c%200%29%2c%20%280%2c%201%2c%200%2c%200%29%2c%20%280%2c%200%2c%201%2c%200%29%2c%20%280%2c%200%2c%20-%201%2c%20-%201%29%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s_1 = MakeMatrix{}((- 1,- 1, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1));
s_2 = MakeMatrix{}((1, 0, 0, 0), (- 1, - 1, - 1, 0), (0, 0, 1, 0), (0, 0, 0, 1));
s_3 = MakeMatrix{}((1, 0, 0, 0), (0, 1, 0, 0), (0, - 2, - 1, - 1), (0, 0, 0, 1));
s_4 = MakeMatrix{}((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, - 1, - 1)); 
```
Creates an internal c++ matrix structure from double list of rationals. 

*MakeMatrixTensorForm* [MakeMatrixTensorForm] {CalculatorConversions::matrixRationalTensorForm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s_1%20%3d%20MakeMatrixTensorForm%7b%7d%28%5cn%28-%201%2c-%201%2c%200%2c%200%29%2c%5cn%280%2c%201%2c%200%2c%200%29%2c%5cn%280%2c%200%2c%201%2c%200%29%2c%5cn%280%2c%200%2c%200%2c%201%29%5cn%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s_1 = MakeMatrixTensorForm{}(
(- 1,- 1, 0, 0),
(0, 1, 0, 0),
(0, 0, 1, 0),
(0, 0, 0, 1)
); 
```
Same as MakeMatrix but uses different c++ implementation (class MatrixTensor instead of class Matrix). 

*MakeMatrixRFs* [MakeMatrixRFs] {CalculatorConversions::matrixRationalFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s_1%20%3d%20MakeMatrixRFs%7b%7d%28%281%20-%20t%2c%202%29%2c%20%283%2c%202%20-%20t%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s_1 = MakeMatrixRFs{}((1 - t, 2), (3, 2 - t))
```
Creates an internal C++ matrix structure from double list of polynomial functions. 

*EnsureExpressionDependsOnlyOn* [EnsureExpressionDependsOnlyOn] {CalculatorFunctions::ensureExpressionDependsOnlyOnStandard}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22EnsureExpressionDependsOnlyOn%28%5c%5csin%20%28%5c%5cln%20x%29%20x%20y%2c%20x%2c%20y%29%3b%5cnEnsureExpressionDependsOnlyOn%28%5c%5csin%20%28%5c%5cln%20x%29%20x%20y%2c%20x%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
EnsureExpressionDependsOnlyOn(\sin (\ln x) x y, x, y);
EnsureExpressionDependsOnlyOn(\sin (\ln x) x y, x);
```
Ensures the expression in the first argument depends only on the free variables given in the remaining arguments. Returns a warning string if that is not the case, else returns an empty string. Intended to warn users of potentially mistyped expressions. 

*EnsureExpressionDependsMandatoryVariables* [EnsureExpressionDependsMandatoryVariables] {CalculatorFunctions::ensureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22EnsureExpressionDependsMandatoryVariables%28%5c%5csin%20%28%5c%5cln%20x%29%20x%20y%2c%20x%2c%20y%29%3b%5cnEnsureExpressionDependsMandatoryVariables%28%5c%5csin%20%28%5c%5cln%20x%29%20x%20y%2c%20%20%28x%2cy%2cz%29%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
EnsureExpressionDependsMandatoryVariables(\sin (\ln x) x y, x, y);
EnsureExpressionDependsMandatoryVariables(\sin (\ln x) x y,  (x,y,z) );
```
Similar to EnsureExpressionDependsOnlyOn, but requests that the expression depend on two lists, first with mandatory variables, second with non-mandatory. 

*EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants* [EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants] {CalculatorFunctions::ensureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants%282%5c%5cpi%2c%20%5c%5cpi%2c%20y%29%3b%5cnEnsureExpressionDependsMandatoryVariables%282%2c%20%5c%5cpi%2c%20none%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants(2\pi, \pi, y);
EnsureExpressionDependsMandatoryVariables(2, \pi, none );
```
Same as EnsureExpressionDependsMandatoryVariables including named constants such as \pi, e. 

*Arctan2* [Arctan2] {CalculatorFunctionsTrigonometry::arctan2}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Arctan2%281%2csqrt%283%29%20%20%29%3b%5cnArctan2%28-1%2csqrt%283%29%20%29%3b%5cnArctan2%28-1%2c-sqrt%283%29%29%3b%5cnArctan2%281%2c-sqrt%283%29%29%3b%5cnArctan2%281%2csqrt%283%29%29-%20DoubleValue%5c%5cpi%2f6%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Arctan2(1,sqrt(3)  );
Arctan2(-1,sqrt(3) );
Arctan2(-1,-sqrt(3));
Arctan2(1,-sqrt(3));
Arctan2(1,sqrt(3))- DoubleValue\pi/6;
```
Arctan2 function: takes as input y and x coordinate and returns the polar angle. The first argument is y, the second is x.

Operator or function \arcsin is overloaded with 2 total handlers.

*\arcsin* [arcsinAlgebraic] {CalculatorFunctionsTrigonometry::arcsinAlgebraic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5carcsin%281%2f2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\arcsin(1/2)
```
Arcsin function for special angles. 

*\arcsin* [\arcsin] {CalculatorFunctionsTrigonometry::arcsinApproximate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5carcsin%283%2f4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\arcsin(3/4)
```
Arcsin function, evaluated approximately. 

Operator or function \arccos is overloaded with 2 total handlers.

*\arccos* [arccosAlgebraic] {CalculatorFunctionsTrigonometry::arccosAlgebraic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5carccos%28%5c%5csqrt%7b2%7d%2f2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\arccos(\sqrt{2}/2)
```
Arccos function for special angles. 

*\arccos* [ArccosApproximate] {CalculatorFunctionsTrigonometry::arccosApproximate}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5carccos%283%2f4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\arccos(3/4)
```
Arccos function. Tries to evaluate the arccos function approximately. 

*\tan* [ExpressTanViaSineCosine] {CalculatorFunctionsTrigonometry::tan}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5ctan%7b%7d%283.1415%29%3b%5cn%5c%5ctan%201.570796327%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\tan{}(3.1415);
\tan 1.570796327
```
Tangent function. Substitutes \tan{}{{x}}=\sin{}x/\cos x. 

*\cot* [ExpressCotViaSineCosine] {CalculatorFunctionsTrigonometry::cotangent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5ccot%7b%7d%283.1415%29%3b%5cn%5c%5ccot%201.570796327%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\cot{}(3.1415);
\cot 1.570796327
```
Tangent function. Substitutes \tan{}{{x}}=\cos{}x/\sin x. 

*\csc* [ExpressCscViaSine] {CalculatorFunctionsTrigonometry::csc}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5ccsc%7b%7d%283.1415%29%3b%5cn%5c%5ccsc%201.570796327%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\csc{}(3.1415);
\csc 1.570796327
```
Cosecant function. Substitutes \csc{}{{x}}=1/\sin x. 

*\sec* [ExpressSecViaCosine] {CalculatorFunctionsTrigonometry::sec}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5csec%7b%7d%283.1415%29%3b%5cn%5c%5csec%201.570796327%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\sec{}(3.1415);
\sec 1.570796327
```
Secant function. Substitutes \sec{}{{x}}=1/\sec x. 

*Floor* [Floor] {CalculatorFunctionsBasic::floor}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Floor%283%2f7%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Floor(3/7)
```
Floor function.

*Round* [Round] {CalculatorFunctionsBasic::round}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Round%283%2f7%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Round(3/7)
```
Round function.

Operator or function DoubleValue is overloaded with 2 total handlers.

*DoubleValue* [DoubleValue] {CalculatorFunctions::evaluateToDouble}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%283%2f7%29%3b%5cnDoubleValue%7b%7d%2813%21%2b0.1%2c0%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(3/7);
DoubleValue{}(13!+0.1,0);
```
Floating point value of a number. Uses the built-in C++ floating point library.

*DoubleValue* [DoubleValueWithRounding] {CalculatorFunctions::evaluateToDoubleWithRounding}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValue%7b%7d%283%2f7%2c3%29%3b%5cnDoubleValue%7b%7d%283%2f7777777%2c2%29%3b%5cnDoubleValue%7b%7d%280.0030%2c2%29%3b%5cnDoubleValue%7b%7d%283.0030%2c2%29%3b%5cnDoubleValue%7b%7d%283%2f7777777%2c0%29%3b%5cnDoubleValue%7b%7d%2813%21%2b0.1%2c0%29%3b%5cnDoubleValue%7b%7d%283%2f7%2c100%29%3b%5cnDoubleValue%7b%7d%283%2f7%2c-5%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValue{}(3/7,3);
DoubleValue{}(3/7777777,2);
DoubleValue{}(0.0030,2);
DoubleValue{}(3.0030,2);
DoubleValue{}(3/7777777,0);
DoubleValue{}(13!+0.1,0);
DoubleValue{}(3/7,100);
DoubleValue{}(3/7,-5);
```
Round the floating point value of a number up to k <= 10 significant digits after the decimal dot. The first argument is the number to round. The second argument is the number of significant digits to round to. Using two significant digits, 0.00269 rounds to 0.0027. Not to be confused with fixed digits: rounding 0.00269 to two fixed digits rounds to 0.00. Corner cases such as rounding of 0.0030 vs 0.003 are left unspecified/specified-by-example. Integers are not rounded further unless they have very large absolute values. Negatives are multiplied by -1, rounded, and then multiplied back by -1. Handles up to 100 zeroes after the decimal dot, does nothing beyond that. 

*DoubleValueFixed* [DoubleValueFixed] {CalculatorFunctions::evaluateToDoubleWithRounding}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DoubleValueFixed%7b%7d%283%2f7%2c3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DoubleValueFixed{}(3/7,3)
```
Round the floating point value of a number up to k fixed digits.The first argument is the number to round. The second argument is the number of digits to round to.

*ModP* [ModP] {CalculatorFunctions::zModP}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ModP%7b%7d%287%2c%203%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ModP{}(7, 3)
```
Number modulo P. First argument = number, second argument = modulo.

*GCD* [GCD] {CalculatorFunctions::greatestCommonDivisorInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GCD%28100%21%2b%201%2c%20101%20%2a%20103%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GCD(100!+ 1, 101 * 103)
```
Greatest common divisor of two integers.

*LCM* [LCM] {CalculatorFunctions::leastCommonMultipleInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LCM%2891%2c%201001%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LCM(91, 1001)
```
Least common multiple of two integers.

*GCDPoly* [GCDPoly] {CalculatorFunctionsPolynomial::greatestCommonDivisorPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GCDPoly%7b%7d%28x%5e3-1%2c%20x%5e2-1%29%5cn%3bGCDPoly%7b%7d%282%2f3x%5e3-2%2f3%2c%2010x%5e2-10%29%5cn%3bGCDPoly%28%20%28a%5e2-b%5e2%29%28a%5e3-b%5e3%29%2c%20%28a%5e4-b%5e4%29%28a%5e5-b%5e5%29%29%3b%5cnGCDPoly%28PolynomialModP%28%28x%5e2%2b3%29%28x%5e3%2b1%29%2c%207%29%2c%20PolynomialModP%28%28x%5e2%2b3%29%28x%5e3%2bx%2b1%29%2c%207%29%29%3b%5cnf%3dPolynomialModP%7b%7d%28x%5e%7b5%7d%2bx%5e%7b4%7d%2b3x%5e%7b3%7d%2bx%5e%7b2%7d%2bx%20%2b3%2c7%29%3b%5cng%3dPolynomialModP%7b%7d%283x%5e%7b4%7d%2b2x%5e%7b3%7d%2b3x%20%2b3%2c7%29%3b%5cnGCDPoly%28f%2c%20g%29%3b%5cnGCDPoly%7b%7d%28x%20y%2c%20y%2bx%20z%29%3b%5cnGCDPoly%28x%5e9-31x%20-1%2c%20x%5e11-31x-1%29%3b%5cnGCDPoly%28x%5e9-31%2a37x%20-31%2c%20x%5e11-31%2a37x-31%29%3b%5cna%3d151782%20x%5e%7b70%7d-1383143%20x%5e%7b69%7d%2b131971%20x%5e%7b68%7d%2b14514%20x%5e%7b67%7d%2b14750%20x%5e%7b66%7d%5cn-14042%20x%5e%7b65%7d-35786%20x%5e%7b64%7d%2b17375%20x%5e%7b63%7d-3422%20x%5e%7b61%7d-3567%20x%5e%7b60%7d%5cn%2b32161%20x%5e%7b59%7d%2b2952%20x%5e%7b58%7d%2b225120%20x%5e%7b57%7d-27698%20x%5e%7b56%7d-167529792%20x%5e%7b55%7d%5cn%2b150535156%20x%5e%7b54%7d-867304%20x%5e%7b53%7d-14579095%20x%5e%7b52%7d-14601596%20x%5e%7b51%7d%5cn%2b129978%20x%5e%7b50%7d%2b3641327%20x%5e%7b49%7d-75977%20x%5e%7b48%7d-4854%20x%5e%7b47%7d%2b5192%20x%5e%7b46%7d%5cn%2b3452139%20x%5e%7b45%7d%2b152799%20x%5e%7b44%7d-2966830%20x%5e%7b43%7d-25133900%20x%5e%7b42%7d%5cn%2b111031%20x%5e%7b41%7d%2b15241383675%20x%5e%7b40%7d%2b79868%20x%5e%7b39%7d-12080640%20x%5e%7b38%7d%5cn%2b163947%20x%5e%7b37%7d-8286516%20x%5e%7b36%7d-868172%20x%5e%7b35%7d-11138693%20x%5e%7b34%7d%5cn-3751%20x%5e%7b33%7d-14939%20x%5e%7b32%7d%2b2771%20x%5e%7b31%7d%2b25066400%20x%5e%7b30%7d%2b10167%20x%5e%7b29%7d%5cn-5572%20x%5e%7b28%7d-15801492%20x%5e%7b27%7d%2b2571%20x%5e%7b26%7d-5427%20x%5e%7b25%7d-12475983%20x%5e%7b24%7d%5cn%2b11855683%20x%5e%7b23%7d-2952474%20x%5e%7b22%7d%2b2470608%20x%5e%7b21%7d-6775360%20x%5e%7b20%7d%5cn-2720%20x%5e%7b19%7d%2b7368%20x%5e%7b18%7d-17886%20x%5e%7b17%7d%2b2809%20x%5e%7b16%7d-685%20x%5e%7b15%7d%5cn-3680%20x%5e%7b14%7d%2b9744%20x%5e%7b13%7d-2436%20x%5e%7b12%7d%2b8494%20x%5e%7b11%7d-11726%20x%5e%7b10%7d%5cn%2b1536%20x%5e%7b9%7d%2b1270%20x%5e%7b8%7d-1328%20x%5e%7b7%7d%2b3516%20x%5e%7b6%7d-2162%20x%5e%7b5%7d%5cn%2b3881%20x%5e%7b4%7d-2880%20x%5e%7b3%7d%2b760%20x%5e%7b2%7d-550%20x%2b756%3b%5cnb%3d1484502%20x%5e%7b70%7d-1388263%20x%5e%7b69%7d%2b120071%20x%5e%7b68%7d%2b141954%20x%5e%7b67%7d%5cn%2b38890%20x%5e%7b66%7d-15232%20x%5e%7b65%7d-20682%20x%5e%7b63%7d-2464549%20x%5e%7b62%7d%2b238000%20x%5e%7b61%7d%5cn-38309%20x%5e%7b60%7d-206739%20x%5e%7b59%7d%2b28872%20x%5e%7b58%7d%2b228754%20x%5e%7b57%7d-28107%20x%5e%7b56%7d%5cn-163752532%20x%5e%7b55%7d%2b115071321%20x%5e%7b54%7d%2b69662%20x%5e%7b53%7d-1388237%20x%5e%7b52%7d%5cn-16885046%20x%5e%7b51%7d%2b47552%20x%5e%7b50%7d%2b111745%20x%5e%7b49%7d%2b3537370%20x%5e%7b48%7d%5cn%2b246876370%20x%5e%7b47%7d-1968818%20x%5e%7b46%7d%2b121343%20x%5e%7b45%7d%2b400277%20x%5e%7b44%7d%5cn-293694%20x%5e%7b43%7d-25399499%20x%5e%7b42%7d%2b23017%20x%5e%7b41%7d%2b1524188037%20x%5e%7b40%7d%5cn%2b1206453807%20x%5e%7b39%7d%2b1083542%20x%5e%7b38%7d%2b198420%20x%5e%7b37%7d-8294451%20x%5e%7b36%7d%5cn%2b889658%20x%5e%7b35%7d-6236754%20x%5e%7b34%7d-401738%20x%5e%7b33%7d-62365%20x%5e%7b32%7d-95219%20x%5e%7b31%7d%5cn%2b13939694%20x%5e%7b30%7d%2b883735%20x%5e%7b29%7d%2b31623%20x%5e%7b28%7d-8867956%20x%5e%7b27%7d%5cn-564904%20x%5e%7b26%7d-6117%20x%5e%7b25%7d-6673559%20x%5e%7b24%7d%2b6068877%20x%5e%7b23%7d%5cn-1202796%20x%5e%7b22%7d%2b1261501%20x%5e%7b21%7d%2b34335752%20x%5e%7b20%7d-545675%20x%5e%7b19%7d%5cn%2b3336%20x%5e%7b18%7d-18203%20x%5e%7b17%7d%2b2989%20x%5e%7b16%7d-863%20x%5e%7b15%7d-17274%20x%5e%7b14%7d%5cn%2b9744%20x%5e%7b13%7d-2436%20x%5e%7b12%7d%2b8416%20x%5e%7b11%7d%2b19334%20x%5e%7b10%7d%2b1548%20x%5e%7b9%7d%5cn%2b1160%20x%5e%7b8%7d-20917%20x%5e%7b7%7d%2b3492%20x%5e%7b6%7d-2142%20x%5e%7b5%7d-11573%20x%5e%7b4%7d%5cn%2b11904%20x%5e%7b3%7d-2936%20x%5e%7b2%7d%2b2530%20x-7560%3b%5cnGCDPoly%28a%2cb%29%3b%5cnGCDPoly%7b%7d%28-x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b3%7dx_%7b21%7d%5e%7b2%7dx_%7b22%7d-2x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b15%7dx_%7b17%7dx_%7b21%7d%5e%7b3%7d%2b%202x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b2%7dx_%7b16%7dx_%7b21%7d%5e%7b3%7d%2b2x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b2%7dx_%7b17%7dx_%7b20%7dx_%7b21%7d%5e%7b2%7d%2b%20x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b17%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d-x_%7b13%7d%5e%7b2%7dx_%7b15%7dx_%7b16%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d%2b%20x_%7b13%7d%5e%7b2%7dx_%7b15%7dx_%7b17%7d%5e%7b2%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d%2c%20x_%7b13%7d%5e%7b3%7dx_%7b15%7dx_%7b18%7dx_%7b21%7d%5e%7b2%7dx_%7b22%7d-2x_%7b13%7d%5e%7b3%7dx_%7b17%7d%5e%7b2%7dx_%7b21%7d%5e%7b3%7d-%204x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b15%7dx_%7b17%7dx_%7b21%7d%5e%7b3%7d%2b2x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b2%7dx_%7b16%7dx_%7b21%7d%5e%7b3%7d-%202x_%7b13%7d%5e%7b3%7dx_%7b17%7dx_%7b18%7dx_%7b20%7dx_%7b21%7d%5e%7b2%7d%2b2x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b17%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d-%20x_%7b13%7d%5e%7b2%7dx_%7b15%7dx_%7b16%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GCDPoly{}(x^3-1, x^2-1)
;GCDPoly{}(2/3x^3-2/3, 10x^2-10)
;GCDPoly( (a^2-b^2)(a^3-b^3), (a^4-b^4)(a^5-b^5));
GCDPoly(PolynomialModP((x^2+3)(x^3+1), 7), PolynomialModP((x^2+3)(x^3+x+1), 7));
f=PolynomialModP{}(x^{5}+x^{4}+3x^{3}+x^{2}+x +3,7);
g=PolynomialModP{}(3x^{4}+2x^{3}+3x +3,7);
GCDPoly(f, g);
GCDPoly{}(x y, y+x z);
GCDPoly(x^9-31x -1, x^11-31x-1);
GCDPoly(x^9-31*37x -31, x^11-31*37x-31);
a=151782 x^{70}-1383143 x^{69}+131971 x^{68}+14514 x^{67}+14750 x^{66}
-14042 x^{65}-35786 x^{64}+17375 x^{63}-3422 x^{61}-3567 x^{60}
+32161 x^{59}+2952 x^{58}+225120 x^{57}-27698 x^{56}-167529792 x^{55}
+150535156 x^{54}-867304 x^{53}-14579095 x^{52}-14601596 x^{51}
+129978 x^{50}+3641327 x^{49}-75977 x^{48}-4854 x^{47}+5192 x^{46}
+3452139 x^{45}+152799 x^{44}-2966830 x^{43}-25133900 x^{42}
+111031 x^{41}+15241383675 x^{40}+79868 x^{39}-12080640 x^{38}
+163947 x^{37}-8286516 x^{36}-868172 x^{35}-11138693 x^{34}
-3751 x^{33}-14939 x^{32}+2771 x^{31}+25066400 x^{30}+10167 x^{29}
-5572 x^{28}-15801492 x^{27}+2571 x^{26}-5427 x^{25}-12475983 x^{24}
+11855683 x^{23}-2952474 x^{22}+2470608 x^{21}-6775360 x^{20}
-2720 x^{19}+7368 x^{18}-17886 x^{17}+2809 x^{16}-685 x^{15}
-3680 x^{14}+9744 x^{13}-2436 x^{12}+8494 x^{11}-11726 x^{10}
+1536 x^{9}+1270 x^{8}-1328 x^{7}+3516 x^{6}-2162 x^{5}
+3881 x^{4}-2880 x^{3}+760 x^{2}-550 x+756;
b=1484502 x^{70}-1388263 x^{69}+120071 x^{68}+141954 x^{67}
+38890 x^{66}-15232 x^{65}-20682 x^{63}-2464549 x^{62}+238000 x^{61}
-38309 x^{60}-206739 x^{59}+28872 x^{58}+228754 x^{57}-28107 x^{56}
-163752532 x^{55}+115071321 x^{54}+69662 x^{53}-1388237 x^{52}
-16885046 x^{51}+47552 x^{50}+111745 x^{49}+3537370 x^{48}
+246876370 x^{47}-1968818 x^{46}+121343 x^{45}+400277 x^{44}
-293694 x^{43}-25399499 x^{42}+23017 x^{41}+1524188037 x^{40}
+1206453807 x^{39}+1083542 x^{38}+198420 x^{37}-8294451 x^{36}
+889658 x^{35}-6236754 x^{34}-401738 x^{33}-62365 x^{32}-95219 x^{31}
+13939694 x^{30}+883735 x^{29}+31623 x^{28}-8867956 x^{27}
-564904 x^{26}-6117 x^{25}-6673559 x^{24}+6068877 x^{23}
-1202796 x^{22}+1261501 x^{21}+34335752 x^{20}-545675 x^{19}
+3336 x^{18}-18203 x^{17}+2989 x^{16}-863 x^{15}-17274 x^{14}
+9744 x^{13}-2436 x^{12}+8416 x^{11}+19334 x^{10}+1548 x^{9}
+1160 x^{8}-20917 x^{7}+3492 x^{6}-2142 x^{5}-11573 x^{4}
+11904 x^{3}-2936 x^{2}+2530 x-7560;
GCDPoly(a,b);
GCDPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ 2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}- 4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- 2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2})
```
Greatest common divisor polynomial of two polynomials. The divisor is scaled so that all coefficients are relatively prime integers, and so that the leading monomial under the graded lexicographic order (x_2>x_1, etc.) has positive coefficient. If the polynomial is rational and univariate, will attempt to compute the GCD quicker.

*LCMPoly* [LCMPoly] {CalculatorFunctionsPolynomial::leastCommonMultiplePolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LCMPoly%7b%7d%28-x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b3%7dx_%7b21%7d%5e%7b2%7dx_%7b22%7d-2x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b15%7dx_%7b17%7dx_%7b21%7d%5e%7b3%7d%2b%202x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b2%7dx_%7b16%7dx_%7b21%7d%5e%7b3%7d%2b2x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b2%7dx_%7b17%7dx_%7b20%7dx_%7b21%7d%5e%7b2%7d%2b%20x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b17%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d-x_%7b13%7d%5e%7b2%7dx_%7b15%7dx_%7b16%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d%2b%20x_%7b13%7d%5e%7b2%7dx_%7b15%7dx_%7b17%7d%5e%7b2%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d%2c%20x_%7b13%7d%5e%7b3%7dx_%7b15%7dx_%7b18%7dx_%7b21%7d%5e%7b2%7dx_%7b22%7d-2x_%7b13%7d%5e%7b3%7dx_%7b17%7d%5e%7b2%7dx_%7b21%7d%5e%7b3%7d-%204x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b15%7dx_%7b17%7dx_%7b21%7d%5e%7b3%7d%2b2x_%7b13%7d%5e%7b2%7dx_%7b15%7d%5e%7b2%7dx_%7b16%7dx_%7b21%7d%5e%7b3%7d-%202x_%7b13%7d%5e%7b3%7dx_%7b17%7dx_%7b18%7dx_%7b20%7dx_%7b21%7d%5e%7b2%7d%2b2x_%7b13%7d%5e%7b2%7dx_%7b14%7dx_%7b17%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d-%20x_%7b13%7d%5e%7b2%7dx_%7b15%7dx_%7b16%7dx_%7b18%7dx_%7b19%7dx_%7b21%7d%5e%7b2%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LCMPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ 2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}- 4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- 2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2})
```
Least common multiple of two polynomials. The output is scaled so that all coefficients are relatively prime integers, and so that the leading monomial under the graded lexicographic order (x_2>x_1, etc.) has positive coefficient.

*Interpolate* [Interpolate] {CalculatorFunctions::interpolatePolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Interpolate%7b%7d%281%2c0%29%20%3b%20Interpolate%7b%7d%28%281%2c0%29%2c%282%2c3%29%29%3b%5cnInterpolate%7b%7d%28%281%2c1%29%2c%20%282%2c2%29%2c%20%283%2c%204%29%2c%20%284%2c%208%29%2c%20%285%2c%2016%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Interpolate{}(1,0) ; Interpolate{}((1,0),(2,3));
Interpolate{}((1,1), (2,2), (3, 4), (4, 8), (5, 16))
```
Constructs the one-variable polynomial of minimal degree that passes through the points. Points are given in the form ((x_1, y_1),(x_2, y_2), ...,(x_n, y_n))

Operator or function PolyDivRemainder is overloaded with 2 total handlers.

*PolyDivRemainder* [PolyDivRemainderAlgebraic] {CalculatorFunctionsPolynomial::polynomialDivisionRemainderAlgebraic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivRemainder%7b%7d%28x%5e7%2b6x%20y%2b5x%20y%5e8%2by%5e5%2c%20x%20%2by%5e2-%201%2c%20y%5e3-x%20y%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivRemainder{}(x^7+6x y+5x y^8+y^5, x +y^2- 1, y^3-x y);
```
Returns the remainder after taking quotient of a polynomial divided by a set of polynomials using the default monomial order (lexicographic).

*PolyDivRemainder* [PolyDivRemainder] {CalculatorFunctionsPolynomial::polynomialDivisionRemainderBuiltIn}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%3dPolynomialModP%28%20x%5e%7b4%7d%2bx%5e%7b3%7d%2b6x%5e%7b2%7d%2bx%2b5%2c3%29%3b%5cnb%3dPolynomialModP%28%202x%5e3%2b2x%2b2%2c3%29%3b%5cnc%3dPolyDivRemainder%28a%2cb%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a=PolynomialModP( x^{4}+x^{3}+6x^{2}+x+5,3);
b=PolynomialModP( 2x^3+2x+2,3);
c=PolyDivRemainder(a,b);
```
Returns the remainder after taking quotient of a polynomial divided by a set of polynomials using the default monomial order (lexicographic). Uses modular arithmetic.

*PolyDivQuotient* [PolyDivQuotient] {CalculatorFunctionsPolynomial::polynomialDivisionQuotient}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivQuotient%7b%7d%28x%5e7%2b6x%20y%2b5x%20y%5e8%2by%5e5%2c%20x%20%2by%5e2-%201%2c%20y%5e3-x%20y%29%20%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivQuotient{}(x^7+6x y+5x y^8+y^5, x +y^2- 1, y^3-x y) ;
```
Returns the quotients of a polynomial divided by a set of polynomials using the default monomial order (lexicographic).

*PolyDivStringGrLex* [PolyDivStringGrLex] {CalculatorFunctionsPolynomial::polynomialDivisionVerboseGrLex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivStringGrLex%7b%7d%28x%5e7%2b6x%20y%2b5x%20y%5e8%2by%5e5%2c%20x%5e2%2b2%2c%20y%5e3-%201%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivStringGrLex{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1);
```
Prints a string representing division of a polynomial by a set of polynomials using gr lex order, for example, x^2 y^3 >x y^4, y^11>x^10. 

*PolyDivStringGrLexRev* [PolyDivStringGrLexRev] {CalculatorFunctionsPolynomial::polynomialDivisionVerboseGradedReverseLexicographic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivStringGrLexRev%7b%7d%28x%5e7%2b6x%20y%2b5x%20y%5e8%2by%5e5%2c%20x%5e2%2b2%2c%20y%5e3-%201%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivStringGrLexRev{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1);
```
String that presents the division of a polynomial by a set of polynomials. Uses the graded reverse lexicographic order. More precisely, compares monomials by total degree. Ties are broken by comparing powers right to left. First **smaller** power on the right is declared the winner ('larger').For example,  x^2 y^3 > x y^4, x^11 > y^10. 

*PolyDivStringLex* [PolyDivStringLex] {CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivStringLex%7b%7d%28x%5e7%20%2b%206%20x%20y%20%2b%205x%20y%5e8%20%2b%20y%5e5%2c%20x%5e2%20%2b%202%2c%20y%5e3%20-%201%29%3b%5cnPolyDivStringLex%7b%7d%28x%5e7%20%2b%20%5c%5csqrt%7b2%7d%20x%20%2b%201%2c%20%5c%5csqrt%7b3%7dx%5e2%20%2b%202x%20%2b%201%29%3b%5cnPolyDivStringLex%7b%7d%28PolynomialModP%28x%5e5%2b2x%5e3%2bx%5e2%2b2%2c7%29%2cPolynomialModP%28x%5e2%2c%207%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivStringLex{}(x^7 + 6 x y + 5x y^8 + y^5, x^2 + 2, y^3 - 1);
PolyDivStringLex{}(x^7 + \sqrt{2} x + 1, \sqrt{3}x^2 + 2x + 1);
PolyDivStringLex{}(PolynomialModP(x^5+2x^3+x^2+2,7),PolynomialModP(x^2, 7));
```
Prints a string representing division of a polynomial by a set of polynomials using the lexicographic order on the monomial exponents. For example, x^2 y^4 > x y^1000 > x y^2.Please do note that the lexicographic order is not related to the lexicographic order on the words 'expansions' (x^2y^3->xxyyy) of the monomials. 

*PolyDivStringLexRev* [PolyDivStringLexRev] {CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographicOpposite}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivStringLexRev%7b%7d%28x%5e7%20%2b%206x%20y%20%2b%205x%20y%5e8%20%2b%20y%5e5%2c%20x%5e2%20%2b%202%2c%20y%5e3%20-%201%29%20%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivStringLexRev{}(x^7 + 6x y + 5x y^8 + y^5, x^2 + 2, y^3 - 1) ;
```
Prints a string representing division of a polynomial by a set of polynomials using the lexicographic order on the monimial exponents with reversed order of variables. For example, y^2 > x^1000 y > x y. Please do note that the lexicographic order is not related to the lexicographic order on the words 'expansions' (x^2y^3->xxyyy) of the monomials. 

*DFQEuler* [DFQEuler] {CalculatorFunctions::differentialEquationsEulersMethod}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DFQEuler%28x%5e2%20%2b%20y%5e2%20-%201%2c%200%2c%200%2c%201000%2c%20-%202.5%2c%202.5%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DFQEuler(x^2 + y^2 - 1, 0, 0, 1000, - 2.5, 2.5)
```
<b>Calculus teaching function.</b> Iterates Euler's method to approximate solutions of first order ordinary DFQ's. First argument = expression for y', second and third argument = x and y initial values, fourth argument = number of approximating points, fifth and sixth argument = left and right endpoints.

*NewtonsMethod* [NewtonsMethod] {CalculatorFunctions::newtonsMethod}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22NewtonsMethod%28e%5ex%20-%20%28%20200%20sin%7b%7d%20x%20%2b%20x%5e7%29%2c%200%2c%2010%29%3b%5cnNewtonsMethod%28e%5ex%20%3d%20%28%20200%20sin%7b%7d%20x%20%2b%20x%5e7%29%2c%200%2c%2010%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
NewtonsMethod(e^x - ( 200 sin{} x + x^7), 0, 10);
NewtonsMethod(e^x = ( 200 sin{} x + x^7), 0, 10);
```
Applies Newton's method with a given starting point and given number of iterations.The first argument gives the function whose zeroes we are trying to find. The second argument gives the starting point. The last argument gives the number of iterations of the method. 

*PlotDirectionField* [PlotDirectionField] {CalculatorFunctionsPlot::plotDirectionField}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotDirectionField%28%28-%20y%2c%20x%29%2c%28-%202%2c%20-%202%29%2c%282%2c%202%29%2c%20%2820%2c%2020%29%2c%200.2%2c%20blue%2c%201%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotDirectionField((- y, x),(- 2, - 2),(2, 2), (20, 20), 0.2, blue, 1);
```
Plots a direction field (in 2d for the time being, 3d coming soon). Direction field is like a vector field except that all vectors are normalized to have the same length. First argument = the vector field. Second, third arguments: bottom left and top right corner of the viewing rectangle. Next argument: (numX,numY,...), where numX is the number of segments along the x axis and so on. Next argument: length of each direction line. Next arguments: color, line width. 

*PlotPolar* [PlotPolar] {CalculatorFunctionsPlot::plotPolarRfunctionTheta}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotPolar%281%2bsin%20%20t%2c%200%2c%20%5c%5cpi%29%3b%5cnPlotPolar%28%281%20%2b%209%2f10%20cos%288%20t%29%20%29%20%281%20%2b%201%2f10%20cos%20%2824%20t%29%29%289%2f10%20%2b%205%2f100%20cos%20%28200%20t%29%29%20%281%20%2b%20sin%20t%29%2c%200%2c%202%5c%5cpi%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotPolar(1+sin  t, 0, \pi);
PlotPolar((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t))(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\pi)
```
<b>Calculus teaching function.</b> Draws polar curve given in polar coordinates in the form r = f(t), where t is the angle variable. The angle variable is measured in degrees. The first argument gives the function, the second and third argument give the upper and lower bounds of the angle. 

*PlotPolarExtended* [PlotPolarExtended] {CalculatorFunctionsPlot::plotPolarRfunctionThetaExtended}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotPolarExtended%281%20%2b%20sin%20%20t%2c%200%2c%20%5c%5cpi%29%3b%5cnPlotPolarExtended%28%281%20%2b%209%2f10%20cos%288%20t%29%20%29%20%281%20%2b%201%2f10%20cos%20%2824%20t%29%20%29%5cn%289%2f10%20%2b%205%2f100%20cos%20%28200%20t%29%29%20%281%20%2b%20sin%20t%29%2c%200%2c%202%5c%5cpi%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotPolarExtended(1 + sin  t, 0, \pi);
PlotPolarExtended((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t) )
(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\pi)
```
<b>Calculus teaching function.</b> Same as PlotPolar but also produces a graph in the (rho,theta)-plane. 

*GaussianElimination* [GaussianElimination] {CalculatorFunctions::gaussianEliminationMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GaussianElimination%28%5c%5cbegin%7barray%7d%7bcccccc%7d%20%5c%5csqrt%201%20%26%20%5c%5csqrt%202%20%26%20%5c%5csqrt%203%20%26%201%20%26%200%20%26%200%5c%5c%5c%5c%20%5c%5csqrt%204%20%26%20%5c%5csqrt%205%20%26%20%5c%5csqrt%206%20%26%200%20%26%201%20%26%200%5c%5c%5c%5c%20%5c%5csqrt%207%20%26%20%5c%5csqrt%208%20%26%20%5c%5csqrt%209%20%26%200%20%26%200%20%26%201%5c%5cend%7barray%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GaussianElimination(\begin{array}{cccccc} \sqrt 1 & \sqrt 2 & \sqrt 3 & 1 & 0 & 0\\ \sqrt 4 & \sqrt 5 & \sqrt 6 & 0 & 1 & 0\\ \sqrt 7 & \sqrt 8 & \sqrt 9 & 0 & 0 & 1\end{array})
```
Gaussian elimination of a matrix. Prints a detailed string that shows the Gaussian elimination of a matrix.

*UnivariatePartialFractionAlgrithm* [UnivariatePartialFractionAlgrithm] {CalculatorFunctions::splitToPartialFractionsOverAlgebraicRealsAlgorithm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22UnivariatePartialFractionAlgrithm%28%5c%5cfrac%7bx%5e11%7d%7bx%5e%7b8%7d-2x%5e%7b6%7d-2x%5e%7b5%7d%2b4x%5e%7b3%7d%2bx%5e%7b2%7d-2%7d%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
UnivariatePartialFractionAlgrithm(\frac{x^11}{x^{8}-2x^{6}-2x^{5}+4x^{3}+x^{2}-2});
```
Attempts to splits a univariate rational function into partial fractions over the real algebraic numbers. At the moment, the function factors the denominator over the rationals. If this factorization yields quadratic and linear terms at the function succeeds, else fails.

*UnivariatePartialFractions* [UnivariatePartialFractions] {CalculatorFunctions::splitToPartialFractionsOverAlgebraicReals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%5c%5cfrac%7bx%5e3%7d%7bx%5e%7b3%7d-x%5e2%2b2x%5e2-2%7d%3b%5cnB%3dUnivariatePartialFractions%28A%29%3b%5cnA-%5c%5csum%28B%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A =\frac{x^3}{x^{3}-x^2+2x^2-2};
B=UnivariatePartialFractions(A);
A-\sum(B)
```
Attempts to splits a univariate rational function into partial fractions over the real algebraic numbers. If successful, produces a sequence containing the partial fractions. Else does nothing.

*EqualityToArithmeticExpression* [EqualityToArithmeticExpression] {CalculatorFunctions::equalityToArithmeticExpression}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22EqualityToArithmeticExpression%28a%20%3d%20b%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
EqualityToArithmeticExpression(a = b)
```
Transforms the equality a = b to the arithmetic expression a - b.

*PointsImplicitly* [PointsImplicitly] {CalculatorFunctions::getPointsImplicitly}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PointsImplicitly%28%28x%20-%201%29%20%28y%20-%201%29%20-%20%28%28x%20-%201%29%20%5e%202%20%28y%20-%201%29%20%2b%201%29%20%5e%202%2c%20%28-%202%2c%20-%202%29%2c%20%282%2c%202%29%2c%20%2810%2c%2010%29%2c%202000%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PointsImplicitly((x - 1) (y - 1) - ((x - 1) ^ 2 (y - 1) + 1) ^ 2, (- 2, - 2), (2, 2), (10, 10), 2000)
```
Returns points on or close to the curve in two dimensions. Same as plotImplicit but rather than plotting the curve returns the (x,y) pairs of points found. The output given as a nx2 matrix of floating point numbers. 

*PlotImplicit* [PlotImplicit] {CalculatorFunctionsPlot::plotImplicitFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotImplicit%28%28x%20-%201%29%20%28y%20-%201%29%20-%20%28%28x%20-%201%29%5e2%20%28y%20-%201%29%20%2b%201%29%5e2%2c%20%28-%202%2c%20-%202%29%2c%20%282%2c%202%29%2c%20%2810%2c%2010%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotImplicit((x - 1) (y - 1) - ((x - 1)^2 (y - 1) + 1)^2, (- 2, - 2), (2, 2), (10, 10))
```
Plots implicitly a curve given by the zeros of an expression in the letters x and y. The relation between x and y is assumed continuous at the points where it is defined. The function has not been optimized for speed, please use with care. The first argument gives the relation between x and y, the next two arguments give the lower left and upper right corners of the viewing screen in the format (lowLeftX, lowLeftY), (upperRightX, upperRightY). The next argument gives the initial grid precision in the form (numIntervalsVertical, numIntervalsHorizontal). The next two arguments give the width and height of the image in the format (width, height). The last argument is optional and gives an upper limit for the number of triangles to use (max =20000, default =2000). The triangle used to generate the implicit plot is algorithmically chosen.

*PlotImplicitShowGrid* [PlotImplicitShowGrid] {CalculatorFunctionsPlot::plotImplicitShowGridFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotImplicitShowGrid%28%28x-%201%29%20%28y-%201%29-%28%28x-%201%29%5e2%28y-%201%29%20%2b%201%29%5e2%2c%28-2%2c%20-2%29%2c%20%282%2c%202%29%2c%20%2810%2c10%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotImplicitShowGrid((x- 1) (y- 1)-((x- 1)^2(y- 1) + 1)^2,(-2, -2), (2, 2), (10,10))
```
Same as plotImplicit but shows the underlying grid. The yellow grid is the initial one (specified by the user), and the gray grid is obtained by a subdivision which depends on the concrete function.

*PlotCoordinateSystem* [PlotCoordinateSystem] {CalculatorFunctionsPlot::plotCoordinateSystem}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotCoordinateSystem%28%28-%203%2c%20-%202%2c%20-%203%29%2c%20%281%2c%205%2c%204%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4));
```
Plots a 3d coordinate system, fitting in a box given by two opposite corners.

*PlotSetProjectionScreen* [PlotSetProjectionScreen] {CalculatorFunctionsPlot::plotSetProjectionScreenBasis}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotCoordinateSystem%28%28-%202%2c%20-%202%2c%20-%202%29%2c%20%282%2c%202%2c%202%29%29%5cn%2b%20PlotSetProjectionScreen%28%281%2c%200%2c%20-%200.1%29%2c%20%280%2c%201%2c%20-%200.2%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotCoordinateSystem((- 2, - 2, - 2), (2, 2, 2))
+ PlotSetProjectionScreen((1, 0, - 0.1), (0, 1, - 0.2))
```
Set the projection screen. Input: two 3d vectors that give the 2d-basis of the viewing screen. 

*PlotSurface* [PlotSurface] {CalculatorFunctionsPlot::plotSurface}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%20x%20%3d%20%28R%2bv%2acos%28u%2f2%29%29%2acos%28u%29%3b%5cny%20%3d%20%28R%2bv%2acos%28u%2f2%29%29%2asin%28u%29%3b%5cnz%20%3d%20v%2asin%28u%2f2%29%3b%5cnR%3dMakeInputBox%28name%20%3d%20radiusBig%2c%20value%20%3d%202%2c%20min%20%3d%202%2c%20max%20%3d%205%29%3b%5cnr%3dMakeInputBox%28name%20%3d%20radiusSmall%2c%20value%20%3d%200.6%2c%20min%20%3d%200.2%2c%20max%20%3d%201%2c%20step%20%3d%200.2%29%3b%5cnuSegments%20%3d%20MakeInputBox%28name%20%3d%20uSegments%2c%20value%20%3d%2022%2c%20min%20%3d%208%2c%20max%20%3d%2040%29%3b%5cnvSegments%20%3d%20MakeInputBox%28name%20%3d%20vSegments%2c%20value%20%3d%204%2c%20min%20%3d%202%2c%20max%20%3d%2010%29%3b%5cnPlotSurface%28%5cn%28x%2c%20y%2c%20z%29%2c%20u%20%5c%5cin%20%280%2c%202%5c%5cpi%29%2c%20v%20%5c%5cin%20%28-%20r%2c%20r%29%2c%20%5cncolor1%20%3d%20blue%2c%20color2%20%3d%20cyan%2c%20numSegments1%20%3d%20uSegments%2c%20numSegments2%20%3d%20vSegments%5cn%29%20%2b%20PlotSurface%28%5cn%28%20x%20%2b%202%2c%20z%2c%20y%29%2c%20u%20%5c%5cin%20%280%2c%202%20%5c%5cpi%29%2c%20v%20%5c%5cin%20%28-%20r%2cr%29%2c%20%5cncolor1%20%3d%20red%2c%20color2%20%3d%20pink%2c%20numSegments1%20%3d%20uSegments%2c%20numSegments2%20%3d%20vSegments%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS x = (R+v*cos(u/2))*cos(u);
y = (R+v*cos(u/2))*sin(u);
z = v*sin(u/2);
R=MakeInputBox(name = radiusBig, value = 2, min = 2, max = 5);
r=MakeInputBox(name = radiusSmall, value = 0.6, min = 0.2, max = 1, step = 0.2);
uSegments = MakeInputBox(name = uSegments, value = 22, min = 8, max = 40);
vSegments = MakeInputBox(name = vSegments, value = 4, min = 2, max = 10);
PlotSurface(
(x, y, z), u \in (0, 2\pi), v \in (- r, r), 
color1 = blue, color2 = cyan, numSegments1 = uSegments, numSegments2 = vSegments
) + PlotSurface(
( x + 2, z, y), u \in (0, 2 \pi), v \in (- r,r), 
color1 = red, color2 = pink, numSegments1 = uSegments, numSegments2 = vSegments); 
```
 Plots a surface. 

*PlotCurve* [PlotCurve] {CalculatorFunctionsPlot::plotParametricCurve}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20MakeInputBox%28name%20%3d%20%5c%22a%5c%22%2c%20value%20%3d%2012%2c%20min%20%3d%201%2c%20max%20%3d%2025%29%3b%5cnb%20%3d%20MakeInputBox%28name%20%3d%20%5c%22b%5c%22%2c%20value%20%3d%2013%2c%20min%20%3d%201%2c%20max%20%3d%2025%29%3b%5cnPlotFill%28PlotCurve%28%28sin%28a%20t%29%2c%20cos%28b%20t%29%29%2c%200%2c%202%5c%5cpi%2c%20blue%2c%202%2c%202000%29%2c%20pink%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = MakeInputBox(name = "a", value = 12, min = 1, max = 25);
b = MakeInputBox(name = "b", value = 13, min = 1, max = 25);
PlotFill(PlotCurve((sin(a t), cos(b t)), 0, 2\pi, blue, 2, 2000), pink)
```
Plots a curve sitting in 2-dimensional space. The first and second argument give the x and y coordinate functions; the curve parameter must be t.The third and fourth argument give the start/finish range for t. The next argument gives the curve color. The next argument gives the curve width. The next argument gives the number of points used to draw the curve. 

Operator or function PlotSegment is overloaded with 2 total handlers.

*PlotSegment* [PlotSegment] {CalculatorFunctionsPlot::plotSegment}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotSegment%28%281%2c2%29%2c%20%283%2c4%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotSegment((1,2), (3,4))
```
Plots a segment connecting two points. 

*PlotSegment* [PlotSegmentParametric] {CalculatorFunctionsPlot::plotSegmentParametric}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%3dMakeInputBox%28name%3dx%2c%20value%3d1%29%3b%5cny%3dMakeInputBox%28name%3dy%2c%20value%3d2%29%3b%5cnPlotSegment%28%28x%2cy%29%2c%20%283%2c4%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x=MakeInputBox(name=x, value=1);
y=MakeInputBox(name=y, value=2);
PlotSegment((x,y), (3,4))
```
Plots a segment connecting two points; works with parameters. 

*PlotMarkSegment* [PlotMarkSegment] {CalculatorFunctionsPlot::plotMarkSegment}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotSegment%28%281%2c%202%29%2c%20%283%2c4%29%29%20%2b%20PlotMarkSegment%28%281%2c%202%29%2c%20%283%2c%204%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotSegment((1, 2), (3,4)) + PlotMarkSegment((1, 2), (3, 4))
```
Plots a segment mark. 

Operator or function PlotPath is overloaded with 2 total handlers.

*PlotPath* [PlotPathParametric] {CalculatorFunctionsPlot::plotPathParametric}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%3dMakeInputBox%28name%3dx%2c%20value%20%3d%202%29%3b%5cny%3dMakeInputBox%28name%3dy%2c%20value%20%3d%203%29%3b%5cnPlotPath%28%28%280%2c%200%29%2c%20%28x%2cy%29%29%2c%20blue%29%20%2b%20PlotSelectablePoint%28x%2cy%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x=MakeInputBox(name=x, value = 2);
y=MakeInputBox(name=y, value = 3);
PlotPath(((0, 0), (x,y)), blue) + PlotSelectablePoint(x,y);

```
Plots a straight segment path with parametric points. 

*PlotPath* [PlotPath] {CalculatorFunctionsPlot::plotPath}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotPath%28%28%280%2c%200%29%2c%20%283%2c%200%29%2c%20%283%2c%204%29%2c%20%280%2c%200%29%29%2c%20blue%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotPath(((0, 0), (3, 0), (3, 4), (0, 0)), blue)
```
Plots a straight segment path. The path should be enclosed in parentheses, and color must be indicated. 

*PlotSelectablePoint* [PlotSelectablePoint] {CalculatorFunctionsPlot::plotSelectablePoint}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%3dMakeInputBox%28name%3dx%2c%20value%3d2%29%3b%5cny%3dMakeInputBox%28name%3dy%2c%20value%20%3d%203%29%3b%5cnPlotSelectablePoint%28x%2cy%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x=MakeInputBox(name=x, value=2);
y=MakeInputBox(name=y, value = 3);
PlotSelectablePoint(x,y)
```
Plots a point that can be moved with the mouse. The first two inputs are the x and y arguments and must beinput boxes.

*PlotSetId* [PlotSetId] {CalculatorFunctionsPlot::plotSetId}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotSetId%28myId%29%20%2b%20PlotCoordinateSystem%28%28-%203%2c%20-%202%2c%20-%203%29%2c%20%281%2c%205%2c%204%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotSetId(myId) + PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4))
```
Creates an empty plot with a given canvas id. If you add a nameless plot to a named one the the resulting plot acquires the canvas id. 

*MatchesPattern* [MatchesPattern] {CalculatorFunctions::matchesPattern}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MatchesPattern%7b%7d%28a%20%3d%20b%20%2a%2010%2c%20a%20%3d%20%7b%7bc%7d%7d%20%2a%20b%29%3b%5cnMatchesPattern%7b%7d%28a%20%3d%20b%20%2a%2010%2c%20a%20%3d%20b%20%2a%20%7b%7bc%7d%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MatchesPattern{}(a = b * 10, a = {{c}} * b);
MatchesPattern{}(a = b * 10, a = b * {{c}})
```
Checks whether the first argument matches the pattern of the second argument. If no, returns 0. Pattern parametric entries are indicated with the bound variable notation {{a}}. If the expression matches the pattern, the return is a command enclosure in which each of the variables is freed and assigned the matched value. 

*GetVariablesExcludeNamedConstants* [GetVariablesExcludeNamedConstants] {CalculatorFunctions::getFreeVariablesExcludeNamedConstants}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetVariablesExcludeNamedConstants%7b%7d%28e%5ex%20%2b%20x%20%2b%205%20%2b%5c%5carctan%20x%20%2b%20x%20%2ay%20%2b%20x%5ey%20%2b%20x%5e%7by%5ez%7d%29%3b%5cnGetVariablesIncludeNamedConstants%7b%7d%28e%5ex%20%2b%20x%20%2b%205%20%2b%5c%5carctan%20x%20%2b%20x%20%2ay%20%2b%20x%5ey%20%2b%20x%5e%7by%5ez%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetVariablesExcludeNamedConstants{}(e^x + x + 5 +\arctan x + x *y + x^y + x^{y^z});
GetVariablesIncludeNamedConstants{}(e^x + x + 5 +\arctan x + x *y + x^y + x^{y^z})
```
Gets the variables on which the expression depends. Excludes the named constants. Here, the word ``variables'' is to be thought of as ``free variables'' but the possibility for small distinctions is reserved (to allow dealing with named constants, reserved keywords, etc.). 

*GetVariablesIncludeNamedConstants* [GetVariablesIncludeNamedConstants] {CalculatorFunctions::getFreeVariablesIncludeNamedConstants}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetVariablesExcludeNamedConstants%7b%7d%28e%5ex%20%2b%20x%20%2b5%20%2b%5c%5carctan%20x%20%2b%20x%20%2ay%20%2bx%5ey%2bx%5e%7by%5ez%7d%29%3b%5cnGetVariablesIncludeNamedConstants%7b%7d%28e%5ex%20%2b%20x%20%2b5%20%2b%5c%5carctan%20x%20%2b%20x%20%2ay%20%2bx%5ey%2bx%5e%7by%5ez%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetVariablesExcludeNamedConstants{}(e^x + x +5 +\arctan x + x *y +x^y+x^{y^z});
GetVariablesIncludeNamedConstants{}(e^x + x +5 +\arctan x + x *y +x^y+x^{y^z})
```
Gets the variables on which the expression depends. Includes the named constants. Here, the word ``variables'' is to be thought of as ``free variables'' but the possibility for small distinctions is reserved (to allow dealing with named constants, reserved keywords, etc.). 

*PlotPoint* [PlotPoint] {CalculatorFunctionsPlot::plotPoint}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotPoint%7b%7d%28%281%2c%202%5c%5cpi%29%2c%20blue%29%3b%5cnPlotPoint%7b%7d%28%28%281%2c%202%29%2c%20%282%2c3%29%29%2c%20blue%29%3b%5cna%20%3d%20MakeInputBox%28name%20%3d%20a%2c%20value%20%3d%203%29%3b%5cnb%20%3d%20MakeInputBox%28name%20%3d%20b%2c%20value%20%3d%205%29%3b%5cnPlotPoint%28%28a%2c%20b%29%2c%20red%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotPoint{}((1, 2\pi), blue);
PlotPoint{}(((1, 2), (2,3)), blue);
a = MakeInputBox(name = a, value = 3);
b = MakeInputBox(name = b, value = 5);
PlotPoint((a, b), red)
```
<b>Calculus teaching function.</b> Plots a point from x and y coordinate. Can also be used for multiple points. See the example for how to use for a single and multiple points.

Operator or function Plot2D is overloaded with 2 total handlers.

*Plot2D* [Plot2DoverIntervals] {CalculatorFunctionsPlot::plot2DOverIntervals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cnPlot2D%7b%7d%28%5c%5csin%7b%7dx%20%2bcos%7b%7dx%2c%20%5b0%2c%20%5c%5cpi%5d%5c%5ccup%20%5b2%5c%5cpi%2c%203%5c%5cpi%29%2c%20%5c%22blue%5c%22%2c2%2c8%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
Plot2D{}(\sin{}x +cos{}x, [0, \pi]\cup [2\pi, 3\pi), "blue",2,8)
```
If the second argument is a union of intervals, replaces the plot command with a sum of Plot2d's in which the second and third argument are extracted from each of the intervals.

*Plot2D* [Plot2D] {CalculatorFunctionsPlot::plot2D}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Plot2D%7b%7d%28%5c%5csin%7b%7dx%20%2bcos%7b%7dx%2c%200%2c%205%2c%20%5c%22blue%5c%22%2c2%2c8%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Plot2D{}(\sin{}x +cos{}x, 0, 5, "blue",2,8)
```
<b>Calculus teaching function.</b> Makes a 2d plot of a function given in the form y = f(x). The the second and third argument give the upper and lower bounds of x. The remaining arguments are optional. The next argument specifies color. Next argument gives line width. Next argument gives number of sample points. Plots may be added together: adding plots superimposes the plots. 

*IsPlot* [IsPlot] {CalculatorFunctionsPlot::isPlot}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%20%5cnIsPlot%28%20Plot2D%7b%7d%28%5c%5csin%7b%7dx%20%2bcos%7b%7dx%2c%20%5b0%2c%20%5c%5cpi%5d%5c%5ccup%20%5b2%5c%5cpi%2c%203%5c%5cpi%29%2c%20%5c%22blue%5c%22%2c2%2c8%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals 
IsPlot( Plot2D{}(\sin{}x +cos{}x, [0, \pi]\cup [2\pi, 3\pi), "blue",2,8))
```
Returns 1 if the argument is a plot, 0 otherwise.

*PlotFill* [PlotFill] {CalculatorFunctionsPlot::plotFill}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotFill%28Plot2D%7b%7d%28sqrt%281%20-%20x%5e2%29%2c%20-%201%2c%201%2c%20%5c%22blue%5c%22%2c%202%29%20%2b%20Plot2D%28-%20sqrt%281%20-%20x%5e2%29%2c%20-%201%2c%201%29%2c%20%5c%22blue%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotFill(Plot2D{}(sqrt(1 - x^2), - 1, 1, "blue", 2) + Plot2D(- sqrt(1 - x^2), - 1, 1), "blue")
```
Fills a plot with color. 

*PlotRectangle* [PlotRectangle] {CalculatorFunctionsPlot::plotRectangle}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotRectangle%7b%7d%28%281%2c%202%29%2c%20%282%2c%201%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotRectangle{}((1, 2), (2, 1))
```
Plots a rectangle. Arguments format: PlotRectangle{}((lowerCornerLeftXcoord, lowerCornerLeftXcoord), (width, height)).

*PlotGrid* [PlotGrid] {CalculatorFunctionsPlot::plotGrid}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotGrid%7b%7d%280%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotGrid{}(0)
```
Tells the plot to show axes ticks. Takes as input dummy (non-used) argument. 

*PlotRemoveCoordinateAxes* [PlotRemoveCoordinateAxes] {CalculatorFunctionsPlot::plotRemoveCoordinateAxes}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotRemoveCoordinateAxes%7b%7d%280%29%20%2b%20Plot2D%28x%2c-1%2c1%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotRemoveCoordinateAxes{}(0) + Plot2D(x,-1,1);
```
Removes the coordinate axes from a plot. 

*PlotLabel* [PlotLabel] {CalculatorFunctionsPlot::plotLabel}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotLabel%7b%7d%28%281%2c1%29%2c%20%5c%22%281%2c1%29%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotLabel{}((1,1), "(1,1)")
```
Plots a label at a given position. Arguments format: PlotLabel((Xcoord, Ycoord), "Label"). 

*LayerLabel* [LayerLabel] {CalculatorFunctionsPlot::plotLabel}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LayerLabel%28PlotCurve%28%28cos%20t%2c%20sin%20t%29%2c%200%2c%202pi%29%2c%20%5c%22Circle%5c%22%29%2b%5cnLayerLabel%28%5cnPlotSegment%28%28-sqrt%282%29%2c-sqrt%282%29%29%2f2%2c%28sqrt%282%29%2csqrt%282%29%29%2f2%2c%20green%29%5cn%2c%20%5c%22diameter%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LayerLabel(PlotCurve((cos t, sin t), 0, 2pi), "Circle")+
LayerLabel(
PlotSegment((-sqrt(2),-sqrt(2))/2,(sqrt(2),sqrt(2))/2, green)
, "diameter");
```
Labels a given plot object; use to provide a layer to the graphics. First argument must be a plot, second argument must be the label. 

*PlotLatex* [PlotLatex] {CalculatorFunctionsPlot::plotLatex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotLatex%7b%7d%28%281%2f2%2c%5c%5csqrt%7b3%7d%2f2%29%2c%20%5c%22%5c%5cfrac%7b%5c%5csqrt%7b3%7d%7d%7b2%7d%5c%22%29%2bPlotPoint%7b%7d%28%281%2f2%2c%5c%5csqrt%7b3%7d%2f2%29%2c%20blue%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotLatex{}((1/2,\sqrt{3}/2), "\frac{\sqrt{3}}{2}")+PlotPoint{}((1/2,\sqrt{3}/2), blue)
```
Plots a latex label at a given position. Arguments format: PlotLatex((Xcoord, Ycoord), "label"). 

*PlotViewRectangle* [PlotViewRectangle] {CalculatorFunctionsPlot::PlotViewRectangle}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Plot2D%7b%7d%281%2fx%2c%20-30%2c%2030%2c%20%5c%22red%5c%22%29%20%2b%20PlotViewRectangle%28%28-5%2c-5%29%2c%20%285%2c5%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Plot2D{}(1/x, -30, 30, "red") + PlotViewRectangle((-5,-5), (5,5))
```
Creates an empty plot whose sole purpose is to fix the view rectangle of another plot. To restrict/expand the view rectangle of another plot, simply add PlotViewRectangle to the other plot. Takes as input two pairs of numbers: first pair gives the lower left corner of the viewing rectangle, the second argument gives the upper right corner.

*PlotWindow* [PlotWindow] {CalculatorFunctionsPlot::plotViewWindow}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Plot2D%7b%7d%281%2fx%2c%20-30%2c%2030%2c%20%5c%22red%5c%22%29%20%2b%20PlotWindow%28400%2c400%29%20%2b%20PlotViewRectangle%28%28-5%2c-5%29%2c%20%285%2c5%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Plot2D{}(1/x, -30, 30, "red") + PlotWindow(400,400) + PlotViewRectangle((-5,-5), (5,5))
```
Creates an empty plot whose sole purpose is to fix the dimensions (in pixels) of the image in the format (pixelwidth, pixelHeight). To modify the dimensions (in pixels) of another plot, add to it the PlotWindow ``plot''. 

*Plot2DWithBars* [Plot2DWithBars] {CalculatorFunctionsPlot::plot2DWithBars}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5cnA%20%3d3%2f2-%20%28%28-3%2f4%2b%201%2f4%20%28x%29%29%5e%7b2%7d%29%3b%5cnB%3d%20%281%2f4%20%28x%29%29%5e%7b2%7d%2b2%3b%5cnPlot2DWithBars%7b%7d%28A%2c%20B%2c%200%2c%205%2c%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```

A =3/2- ((-3/4+ 1/4 (x))^{2});
B= (1/4 (x))^{2}+2;
Plot2DWithBars{}(A, B, 0, 5, 1)
```
<b>Calculus teaching function.</b> Same as plot2D but plots two functions with bars locked between the two functions, used to illustrate approximations to definite integrals.

*CompareFunctionsNumerically* [CompareFunctionsNumerically] {CalculatorFunctions::compareFunctionsNumerically}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CompareFunctionsNumerically%7b%7d%28arctan%20%28x%29%2c%20arcsin%28x%2f%5c%5csqrt%7bx%5e2%2b%201%7d%29%2c0%2c%205%2c50%2c0.0001%29%3b%5cnCompareFunctionsNumerically%7b%7d%28e%5ex%2c%201%2bx%20%2bx%5e2%2f2%2bx%5e3%2f3%21%2bx%5e4%2f4%21%2bx%5e5%2f5%21%2bx%5e6%2f6%21%2c0%2c%205%2c50%2c0.0001%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CompareFunctionsNumerically{}(arctan (x), arcsin(x/\sqrt{x^2+ 1}),0, 5,50,0.0001);
CompareFunctionsNumerically{}(e^x, 1+x +x^2/2+x^3/3!+x^4/4!+x^5/5!+x^6/6!,0, 5,50,0.0001)
```
<b>Calculus teaching function.</b> Compares two one-variable functions numerically. First two arguments give the two functions. Third and fourth arguments give the interval [a,b] over which to compare. Fifth argument is optional (default: 50) and gives the number of sampling points. Sixth argument is optional (default: 0.0001) and gives the numerical tolerance eps: if two numbers are at a distance less than eps they are considered equal. 

*CompareExpressionsNumerically* [CompareExpressionsNumerically] {CalculatorFunctions::compareExpressionsNumerically}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CompareExpressionsNumerically%7b%7d%28%5c%5carctan%20x%20%2b%20%5c%5carctan%20y%2c%20%5c%5carctan%28%28x%20%2by%29%2f%281-%20x%20y%29%29%2c%200.0001%2c%20x%5c%5cin%280%2c0.5%29%2c%2020%2c%20y%5c%5cin%280%2c0.5%29%2c%2020%20%29%3b%5cnCompareExpressionsNumerically%7b%7d%28%5c%5carctan%20x%20%2b%20%5c%5carctan%20y%2c%5c%5carctan%28%28x%20%2by%29%2f%281-%20x%20y%29%29%2c%200.0001%2c%20x%5c%5cin%280%2c2%29%2c%2020%2c%20y%5c%5cin%280%2c2%29%2c%2020%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CompareExpressionsNumerically{}(\arctan x + \arctan y, \arctan((x +y)/(1- x y)), 0.0001, x\in(0,0.5), 20, y\in(0,0.5), 20 );
CompareExpressionsNumerically{}(\arctan x + \arctan y,\arctan((x +y)/(1- x y)), 0.0001, x\in(0,2), 20, y\in(0,2), 20 );
```
Compares two expressions numerically. First two arguments give the two functions. The third argument gives the precision. All remaining arguments come in pairs, in the form x\in (2,3), 50. 

*CompareExpressionsNumericallyAtPoints* [CompareExpressionsNumericallyAtPoints] {CalculatorFunctions::compareExpressionsNumericallyAtPoints}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CompareExpressionsNumericallyAtPoints%7b%7d%28%5c%5carctan%20x%20%2b%20%5c%5carctan%20y%2c%20%5c%5carctan%28%28x%20%2by%29%2f%281-%20x%20y%29%29%2c%200.001%2c%20%28x%2cy%29%5c%5cin%20%28%280.5%2c0.5%29%2c%280.3%2c0.3%29%29%29%3b%5cntheFun%20%3d%5c%5csqrt%7bx%20y%7d-%284%20x%5e%7b2%7d%20y-%201%29%3b%5cna%20%3d%5c%5cfrac%7b-64%20x%5e%7b3%7d%20y%5e%7b2%7d%2b%2016%20x%20y%2by%7d%7b32%20x%5e%7b4%7d%20y-8%20x%5e%7b2%7d-%20x%7d%3b%5cntheDiff%3d%20d%2fdx%28theFun%29%3b%5cnnum%3d%20%28d%2fdx%20y%20%3d%200%3b%20theDiff%29_2%3b%5cnden%20%3d%20CoefficientOf%20%28d%2fdx%20y%2c%20theDiff%29%3b%5cnb%3d%20-num%2fden%3b%5cnCompareExpressionsNumerically%28a%2cb%2c%200.001%2c%20x%5c%5cin%280%2c5%29%2c10%2c%20y%5c%5cin%20%280%2c5%29%2c10%29%3b%5cnA%20%3d%20PointsImplicitly%28theFun%2c%20%280%2c0%29%2c%284%2c4%29%2c%20%2810%2c10%29%2c%20%28400%2c400%29%29%3b%5cnCompareExpressionsNumericallyAtPoints%28a%2cb%2c%200.01%2c%20%28x%2cy%29%5c%5cin%20A%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CompareExpressionsNumericallyAtPoints{}(\arctan x + \arctan y, \arctan((x +y)/(1- x y)), 0.001, (x,y)\in ((0.5,0.5),(0.3,0.3)));
theFun =\sqrt{x y}-(4 x^{2} y- 1);
a =\frac{-64 x^{3} y^{2}+ 16 x y+y}{32 x^{4} y-8 x^{2}- x};
theDiff= d/dx(theFun);
num= (d/dx y = 0; theDiff)_2;
den = CoefficientOf (d/dx y, theDiff);
b= -num/den;
CompareExpressionsNumerically(a,b, 0.001, x\in(0,5),10, y\in (0,5),10);
A = PointsImplicitly(theFun, (0,0),(4,4), (10,10), (400,400));
CompareExpressionsNumericallyAtPoints(a,b, 0.01, (x,y)\in A);
```
<b>Calculus teaching function.</b> Compares two expressions numerically. First two arguments give the two functions. The third argument gives the precision. All remaining arguments come in pairs, in the form x\in (2,3), 50. 

*IsInteger* [IsInteger] {CalculatorFunctions::isInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsInteger%7b%7da%3b%5cnIsInteger%7b%7d1%3b%5cnf%7b%7d%7b%7ba%7d%7d%3dIsInteger%7b%7da%3b%5cnf%7b%7d1%3b%5cnf%7b%7db%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsInteger{}a;
IsInteger{}1;
f{}{{a}}=IsInteger{}a;
f{}1;
f{}b
```
If the argument has no bound variables, returns 1 if the argument is an integer, 0 otherwise. 

*IsEven* [IsEven] {CalculatorFunctions::isEven}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22i%5e%7b%7bn%7d%7d%20%3a%20if%20IsEven%20n%20%3d%20%28-%201%29%5e%28n%2f2%29%3b%5cni%5e100%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
i^{{n}} : if IsEven n = (- 1)^(n/2);
i^100 
```
If the argument has no bound variables, returns 1 if the argument is an even integer, 0 otherwise. 

*IsSquareFree* [IsSquareFree] {CalculatorFunctions::isSquareFree}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsSquareFree%286%29%3b%5cnIsSquareFree%2812%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsSquareFree(6);
IsSquareFree(12)
```
If the argument is an integer, returns 1 if the integer is square-free (no primes in the decomposition of the integer appear with power greater than one) 

*IsPower* [IsPower] {CalculatorFunctions::isPower}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsPower%286%29%3b%5cnIsPower%288%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsPower(6);
IsPower(8)
```
If the argument is an integer, returns 1 if the integer is plus or minus the power of a prime (no primes in the decomposition of the integer appear with power greater than one) 

*IsSquare* [IsSquare] {CalculatorFunctions::isSquare}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsSquare%288%29%3b%5cnIsSquare%2816%29%3b%5cnIsSquare%28100%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsSquare(8);
IsSquare(16);
IsSquare(100);
```
If the argument is an integer, returns 1 if the number is the square of an integer. 

*IsNonEmptySequence* [IsNonEmptySequence] {CalculatorFunctions::isNonEmptySequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsNonEmptySequence%282%2c3%29%3bIsNonEmptySequence%7b%7d%28%282%2c3%29%29%3b%5cnIsNonEmptySequence%7b%7d1%3b%5cna%7b%7d%7b%7bx%7d%7d%20%3d%20IsNonEmptySequence%28x%29%3b%5cna%7b%7d%28%282%2c3%29%29%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsNonEmptySequence(2,3);IsNonEmptySequence{}((2,3));
IsNonEmptySequence{}1;
a{}{{x}} = IsNonEmptySequence(x);
a{}((2,3))

```
Returns 1 if the input is a non-empty sequence; 0 otherwise. Does not reduce the expression if it has bound variables.

*IsRational* [IsRational] {CalculatorFunctions::isRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsRational%7b%7da%3b%5cnIsRational%7b%7d-%201%3b%5cnf%7b%7d%7b%7ba%7d%7d%3dIsRational%7b%7da%3b%5cnIsRational%7b%7d1%3b%5cnIsRational%7b%7db%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsRational{}a;
IsRational{}- 1;
f{}{{a}}=IsRational{}a;
IsRational{}1;
IsRational{}b
```
If the argument has no bound variables, returns 1 if the argument is an rational, 0 otherwise. 

*not* [not] {CalculatorFunctions::notFunction}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22not%7b%7d1%3b%5cnnot%7b%7da%3b%5cnnot%7b%7d0%3b%5cnnot%7b%7d%283%3d%3d4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
not{}1;
not{}a;
not{}0;
not{}(3==4)
```
If the argument is a small integer, returns 1 if the argument is 0 and 1 the argument is non-zero. If the argument is not a small integer, does nothing. 

*DiagonalizeMatrix* [DiagonalizeMatrix] {CalculatorFunctionsLinearAlgebra::diagonalizeMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%3d%5c%5cbegin%7bpmatrix%7d1%261%5c%5c%5c%5c%201%260%5c%5cend%7bpmatrix%7d%3b%5cnDiagonalizeMatrix%20a%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a=\begin{pmatrix}1&1\\ 1&0\end{pmatrix};
DiagonalizeMatrix a
```
Tries to diagonalize a matrix (Jordan normal form). As of writing, will succeed only when the eigenvalues are rational or use quadratic radicals and the Jordan normal form is diagonal. When successful, returns a list of three matrices, X, A and X^-1, such that XAX^-1 equals the orignal matrix and A is in Jordan normal form.

*FunctionToMatrix* [FunctionToMatrix] {CalculatorFunctionsLinearAlgebra::functionToMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3dFunctionToMatrix%7b%7d%28A%2c5%2c5%29%3b%5cnA%7b%7d%28%7b%7ba%7d%7d%2c%7b%7bb%7d%7d%29%3da%2fb%3b%5cnX%3b%5cn%5c%5cdet%20%7b%7d%20X%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X =FunctionToMatrix{}(A,5,5);
A{}({{a}},{{b}})=a/b;
X;
\det {} X
```
Creates a matrix from a function. The first argument gives the function, the second argument the number of rows, the third- the number of columns.

*Transpose* [Transpose] {CalculatorFunctions::transpose}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Transpose%7b%7d%281%2c2%29%3b%5cn%281%2c2%29%5et%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Transpose{}(1,2);
(1,2)^t
```
Transposes a matrix of expressions. 

*\det* [Determinant] {CalculatorFunctions::determinant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3dFunctionToMatrix%7b%7d%28A%2c5%2c5%29%3b%5cn%20A%7b%7d%28%7b%7ba%7d%7d%2c%7b%7bb%7d%7d%29%3d1%2f%28a%20%2bb%29%3b%5cnX%3b%5cn%5c%5cdet%20%7b%7d%20X%3b%5cnf%7b%7d%28%7b%7ba%7d%7d%2c%20%7b%7bb%7d%7d%29%20%3d%20x_%7b10a%2bb%7d%3b%5cnA%3dFunctionToMatrix%28f%2c%203%2c3%29%3b%5cn%5c%5cdet%20A%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X =FunctionToMatrix{}(A,5,5);
 A{}({{a}},{{b}})=1/(a +b);
X;
\det {} X;
f{}({{a}}, {{b}}) = x_{10a+b};
A=FunctionToMatrix(f, 3,3);
\det A
```
Tries to convert to a matrix of rationals or matrix of rational functions and computes the determinant if not too large.  

*DeterminantPolynomial* [DeterminantPolynomial] {CalculatorFunctions::determinantPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DeterminantPolynomial%7b%7d%5c%5cleft%28%20%5c%5cbegin%7barray%7d%7bccc%7dx_%7b11%7d%20%26%20x_%7b12%7d%20%26%20x_%7b13%7d%20%5c%5c%5c%5c%20x_%7b21%7d%20%26%20x_%7b22%7d%20%26%20x_%7b23%7d%20%5c%5c%5c%5c%20x_%7b31%7d%20%26%20x_%7b32%7d%20%26%20x_%7b33%7d%20%5c%5cend%7barray%7d%20%5c%5cright%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DeterminantPolynomial{}\left( \begin{array}{ccc}x_{11} & x_{12} & x_{13} \\ x_{21} & x_{22} & x_{23} \\ x_{31} & x_{32} & x_{33} \end{array} \right) 
```
Attempts to convert the entries of the matrix to polynomials and computes the determinant polynomial. The matrix must not be larger than 8x8. Note that this function does not perform Gaussian elimination, but rather sums the n! summands of the matrix polynomial. This function is meant to be used with multivariate polynomials with large number of variables (in such cases summing n! summands may very well be faster). 

*Length* [Length] {CalculatorFunctionsListsAndSets::length}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Length%7b%7d%28%29%3b%5cnLength%28a%2c%20b%29%3b%5cnLength%7b%7d%28%28a%2cb%29%29%3b%5cnLength%7b%7d%28a%2cb%29%3b%5cnLength%7b%7d%28a%29%3b%5cnLength%7b%7d%28Sequence%7b%7da%29%3b%5cnLength%7b%7d%28Sequence%7b%7d%28a%2cb%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Length{}();
Length(a, b);
Length{}((a,b));
Length{}(a,b);
Length{}(a);
Length{}(Sequence{}a);
Length{}(Sequence{}(a,b));

```
Returns the length of a sequence. 

*GetMultiplicandList* [GetMultiplicandList] {CalculatorFunctions::collectMultiplicands}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetMultiplicandList%28a%2ab%2ac%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetMultiplicandList(a*b*c) 
```
Converts a sum to a sequence containing the summands. 

*NormalizeIntervals* [NormalizeIntervals] {CalculatorFunctionsIntervals::normalizeIntervals}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cnNormalizeIntervals%28%5b2%2c3%5d%20%5c%5ccup%20%5b5%2c%207%5d%20%5c%5ccup%20%5b-%201%2c-%201%2f2%5d%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
NormalizeIntervals([2,3] \cup [5, 7] \cup [- 1,- 1/2]);
```
Sorts interval union. 

*BelongsTo* [BelongsTo] {CalculatorFunctionsListsAndSets::belongsTo}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22BelongsTo%28x%2c%28x%2cy%2cz%29%29%3b%5cnBelongsTo%28x%2c%28w%2cy%2cz%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
BelongsTo(x,(x,y,z));
BelongsTo(x,(w,y,z));

```
Returns 1 if an element belongs to a list, 0 otherwise. 

*CompareIntervalsNumerically* [CompareIntervalsNumerically] {CalculatorFunctions::compareIntervalsNumerically}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cnCompareIntervalsNumerically%28%5b3%2c7%29%5c%5ccup%20%5b8%2c9%5d%2c%20%5b3%2c7.00000001%29%5c%5ccup%20%5b7.9999999%2c%209%5d%2c%200.0001%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
CompareIntervalsNumerically([3,7)\cup [8,9], [3,7.00000001)\cup [7.9999999, 9], 0.0001);
```
Compares unions of intervals numerically. First and second arguments: two unions of intervals to compare. Third argument: precisions to compare with. 

*GetOpandList* [GetOpandList] {CalculatorFunctions::collectOpands}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cnGetOpandList%5cn%28StringToAtom%28%5c%22%5c%5cotimes%5c%22%29%2c%20a%5c%5cotimes%20b%20%5c%5cotimes%20c%20%29%3b%5cnGetOpandList%28StringToAtom%28%5c%22%5c%5ccup%5c%22%29%2c%20%5ba%2cb%5d%5c%5ccup%20%5bc%2cd%5d%20%5c%5ccup%20%5be%2cf%5d%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
GetOpandList
(StringToAtom("\otimes"), a\otimes b \otimes c );
GetOpandList(StringToAtom("\cup"), [a,b]\cup [c,d] \cup [e,f] );
```
Converts a chain of operations to a list. First argument: operation name. You can expressions such as StringToAtom("+") to make an arbitrary expresssion. 

*GetSummandList* [GetSummandList] {CalculatorFunctions::collectSummands}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetSummandList%281%2ba%20%2bb%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetSummandList(1+a +b) 
```
Converts a sum to a sequence containing the summands. 

*GetSummand* [GetSummand] {CalculatorFunctions::getSummand}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetSummand%28%5c%5csum_%7bn%20%3d1%7d%5e%5c%5cinfty%20%28-%201%29%5e%7b2n%20%2b%201%7d%20x%5e%7b2n%20%2b%201%7d%2f%282n%20%2b%201%29%21%2c%205%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetSummand(\sum_{n =1}^\infty (- 1)^{2n + 1} x^{2n + 1}/(2n + 1)!, 5 )
```
Extracts the nth summand from a sum, <b>starts with the ZEROETH summand</b>. 

*GetFirstSummandContaining* [GetFirstSummandContaining] {CalculatorFunctions::getFirstSummandContaining}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetFirstSummandContaining%28a%20%2bb%2b%5c%5csum_%7bn%20%3d1%7d%5e%5c%5cinfty%28%28-%201%29%5e%7b2n%20%2b%201%7d%20x%5e%7b2n%20%2b%201%7d%2f%282n%20%2b%201%29%21%29%2c%20%5c%5csum%20%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetFirstSummandContaining(a +b+\sum_{n =1}^\infty((- 1)^{2n + 1} x^{2n + 1}/(2n + 1)!), \sum ) 
```
Extracts the first summand containing a subexpression. 

*RemoveDuplicates* [RemoveDuplicates] {CalculatorFunctions::removeDuplicates}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22RemoveDuplicates%28a%2ca%2cc%2ca%2cb%2ca%2cb%2cd%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
RemoveDuplicates(a,a,c,a,b,a,b,d) 
```
Removes duplicates. 

*Sort* [Sort] {CalculatorFunctions::sort}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Sort%28x%5e2%2c%20x%5e3%2c%20x%5e1%2c%20x%5e%7b-%201%7d%29%3b%5cnSort%283%2c2%2c1%2c4%2c3%29%3b%5cnSort%28%283%2c2%2c3%2c1%29%29%3b%5cnSort%7b%7d%28%283%2c2%2c3%2c1%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Sort(x^2, x^3, x^1, x^{- 1});
Sort(3,2,1,4,3);
Sort((3,2,3,1));
Sort{}((3,2,3,1));

```
Sorts a sequence. 

*SortDescending* [SortDescending] {CalculatorFunctions::sortDescending}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SortDescending%28x%5e2%2c%20x%5e3%2c%20x%5e1%2c%20x%5e%7b-%201%7d%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SortDescending(x^2, x^3, x^1, x^{- 1}) 
```
Sorts a sequence in descending order.  

*BlocksOfCommutativity* [BlocksOfCommutativity] {CalculatorFunctions::getUserDefinedSubExpressions}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22BlocksOfCommutativity%28%5c%5csin%20x%20%2b%20x%5e2%2b%203x%20y%20%2b%2018x%5e%7b3%2f4%20y%7d%2b%5c%5csqrt%7b2%7d%5e%7b%5c%5csqrt%7b2%7dc%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
BlocksOfCommutativity(\sin x + x^2+ 3x y + 18x^{3/4 y}+\sqrt{2}^{\sqrt{2}c})
```
Returns subexpression blocks of commutativity. 

*InvertMatrixVerbose* [InvertMatrixVerbose] {CalculatorFunctions::invertMatrixVerbose}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22InvertMatrixVerbose%28%281%2c%202%29%2c%20%282%2c%203%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
InvertMatrixVerbose((1, 2), (2, 3))
```
<b>Calculus teaching function.</b> Inverts a matrix of rationals if invertible, in any other case generates an error. Makes a detailed printout of all Gaussian elimantion steps. 

*InvertMatrix* [InvertMatrix] {CalculatorFunctions::invertMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20MakeMatrix%28%281%2c2%2c1%29%2c%20%281%2c0%2c1%29%2c%20%28-%201%2c1%2c0%29%29%3b%5cnInvertMatrix%20X-%20X%5e%7b-%201%7d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0));
InvertMatrix X- X^{- 1}
```
Inverts a matrix of rationals or algebraic numbers if invertible. 

*Trace* [Trace] {CalculatorFunctions::matrixTrace}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20MakeMatrix%28%281%2c2%2c1%29%2c%20%281%2c0%2c1%29%2c%20%28-%201%2c1%2c0%29%29%3b%5cnTrace%20X%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0));
Trace X
```
Gets trace of a square matrix. 

*ReverseBytes* [ReverseBytes] {CalculatorFunctions::reverseBytes}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ReverseBytes%7b%7d%28%5c%22abc%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ReverseBytes{}("abc")
```
Reverses the bytes of a string. Does not respect utf-8 encoding. 

*Reverse* [Reverse] {CalculatorFunctions::reverseOrder}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Reverse%7b%7d%28s_1%20s_2%20s_3%20s_4%20s_2%20s_3%20s_1%20s_2%20s_3%20s_4%20s_1%20s_2%20s_3%20s_2%20s_1%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Reverse{}(s_1 s_2 s_3 s_4 s_2 s_3 s_1 s_2 s_3 s_4 s_1 s_2 s_3 s_2 s_1)
```
Reverses order of elements. This operation will reverse products, lists, etc. More precisely, the command leaves the fist child in the internal representation of the object in place and flips the order of all other children.

*ReverseRecursively* [ReverseRecursively] {CalculatorFunctions::reverseOrderRecursively}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ReverseRecursively%7b%7d%28s_1%20s_2%20s_3s_4s_2s_3s_1s_2s_3s_4s_1s_2s_3s_2s_1%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ReverseRecursively{}(s_1 s_2 s_3s_4s_2s_3s_1s_2s_3s_4s_1s_2s_3s_2s_1)
```
Same as Reverse but will apply recursively to the children expressions as well.

*SolveFor* [SolveFor] {CalculatorFunctions::solveUnivariatePolynomialWithRadicalsWithRespectTo}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SolveFor%28x%2c%20a%20x%5e2%20%2b%20b%20x%20%2b%20c%29%3b%5cnSolveFor%28x%2c%20a%20x%20%2b%20b%29%3b%5cnSolveFor%28x%2c%20x%5e3%20-%203x%5e2%20-%20x%2f2%20%2b%205%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SolveFor(x, a x^2 + b x + c);
SolveFor(x, a x + b);
SolveFor(x, x^3 - 3x^2 - x/2 + 5);
```
Tries to solve a polynomial equation with respect to a variable. The first argument gives the variable expression (arbitrary expressions allowed) and the second argument gives the polynomial. At the moment the calculator only knows the quadratic formula but Cardano's formula and the fourth degree formula will be implemented when need arises. The solutions of the equation are returned in a list (empty list for no solution). Solutions with multiplicity higher than one are repeated. 

*CardanoFormula* [CardanoFormula] {CalculatorFunctions::cardanoFormula}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CardanoFormula%28x%5e3%20%2b%20p%20x%20%2b%20q%3d0%29%3b%5cnCardanoFormula%28x%5e3%20%2b%202%20x%20%2b%203%29%3b%5cnCardanoFormula%28a%20x%5e3%20%2b%20b%20x%5e2%20%2b%20c%20x%20%2b%20d%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CardanoFormula(x^3 + p x + q=0);
CardanoFormula(x^3 + 2 x + 3);
CardanoFormula(a x^3 + b x^2 + c x + d);

```
Applies the cardano formula to a cubic.

*FactorOutNumberContent* [FactorOutNumberContent] {CalculatorFunctions::factorOutNumberContent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22DistributeMultiplication%5c%22%2c%5c%22DistributeMultiplicationConstants%5c%22%29%3b%5cnFactorOutNumberContent%7b%7d%283x%20%2b%209t%20%2b%2027%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("DistributeMultiplication","DistributeMultiplicationConstants");
FactorOutNumberContent{}(3x + 9t + 27);
```
Factors out the rational number content of an additive expression. The expression part of the result must have integer relatively prime coefficients, with leading coefficient positive. 

*FactorUnivariatePolynomialOverRationalsKronecker* [FactorUnivariatePolynomialOverRationalsKronecker] {CalculatorFunctionsPolynomial::factorPolynomialKronecker}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FactorUnivariatePolynomialOverRationalsKronecker%7b%7d%28x%5e%7b8%7d-44x%5e%7b6%7d%2b438x%5e%7b4%7d-%201292x%5e%7b2%7d%2b529%29%3b%5cnFactorUnivariatePolynomialOverRationalsKronecker%7b%7d%28x%5e%7b8%7d%2b2x%5e%7b7%7d-3x%5e%7b6%7d-4x%5e%7b5%7d%2b6x%5e%7b4%7d%2b2x%5e%7b3%7d-%2013x%5e%7b2%7d%2b%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FactorUnivariatePolynomialOverRationalsKronecker{}(x^{8}-44x^{6}+438x^{4}- 1292x^{2}+529);
FactorUnivariatePolynomialOverRationalsKronecker{}(x^{8}+2x^{7}-3x^{6}-4x^{5}+6x^{4}+2x^{3}- 13x^{2}+ 1)
```
Factors a one variable polynomial over the rationals using Kroenecker's method. After clearing denominators, assume the poly has integer coefficients. If looking for a divisor of degree k, plug in k+1 different integer values of the poly to find k+1 integer values. Factor them. Each selection of k+1 factors of these integer values determines the potential values of a divisor polynomial; the polynomial of degree k can be reconstructed from k+1 values through Lagrange interpolation. Try all possible divisors found in this way.

*FactorUnivariatePolynomialOverRationals* [FactorUnivariatePolynomialOverRationals] {CalculatorFunctionsPolynomial::factorPolynomialFiniteFields}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnFactorUnivariatePolynomialOverRationals%7b%7d%28182903%20x%5e%7b11%7d%2b101813%20x%5e%7b10%7d-68963%20x%5e%7b9%7d%2b32574%20x%5e%7b8%7d%2b11015%20x%5e%7b7%7d%2b453344%20x%5e%7b6%7d%2b106241%20x%5e%7b5%7d%2b115598%20x%5e%7b4%7d%2b102%20x%5e%7b3%7d%2b145%20x%5e%7b2%7d%2b12276%20x%2b261632%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
FactorUnivariatePolynomialOverRationals{}(182903 x^{11}+101813 x^{10}-68963 x^{9}+32574 x^{8}+11015 x^{7}+453344 x^{6}+106241 x^{5}+115598 x^{4}+102 x^{3}+145 x^{2}+12276 x+261632);

```
Factors a one variable polynomial over the rationals using finite field methods. At the time of writing, the method used is probabilistic but with high chance of success, and pretty fast. 

*FactorUnivariatePolynomialOverRationalsDetailed* [FactorUnivariatePolynomialOverRationalsDetailed] {CalculatorFunctionsPolynomial::factorPolynomialFiniteFieldsDetailed}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnFactorUnivariatePolynomialOverRationalsDetailed%7b%7d%28325x%5e%7b2%7d-38x-312%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
FactorUnivariatePolynomialOverRationalsDetailed{}(325x^{2}-38x-312);

```
Same as FactorUnivariatePolynomialOverRationals but prints out a summary of internal computations.

*FactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields* [FactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields] {CalculatorFunctionsPolynomial::factorPolynomialKroneckerThenFiniteFields}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnFactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields%7b%7d%28%5cn169%20x%5e%7b11%7d-312%20x%5e%7b10%7d%2b430%20x%5e%7b9%7d-524%20x%5e%7b8%7d%2b595%20x%5e%7b7%7d-644%20x%5e%7b6%7d%2b581%20x%5e%7b5%7d-440%20x%5e%7b4%7d%2b318%20x%5e%7b3%7d-214%20x%5e%7b2%7d%2b127%20x-56%5cn%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
FactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields{}(
169 x^{11}-312 x^{10}+430 x^{9}-524 x^{8}+595 x^{7}-644 x^{6}+581 x^{5}-440 x^{4}+318 x^{3}-214 x^{2}+127 x-56
);

```
Runs the Kronecker algorithm with a computation throttle; if unsuccessful, runs finite field algorithms. This can be considerably slower than directly running the finite fieldalgorithm.

*FactorOneVariablePolynomialModPrime* [FactorOneVariablePolynomialModPrime] {CalculatorFunctionsPolynomial::factorPolynomialModPrime}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FactorOneVariablePolynomialModPrime%7b%7d%28x%5e5%2bx%5e4%2b2x%5e3-x%5e2-x-1%2c%201009%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FactorOneVariablePolynomialModPrime{}(x^5+x^4+2x^3-x^2-x-1, 1009);

```
Factors a one variable polynomial over a given prime field Z/pZ using the Cantor-Zassenhaus algorithm. First argument = polynomial. Second argument = prime number.

*FactorInteger* [FactorInteger] {CalculatorFunctions::factorInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FactorInteger%2810001011%29%3b%5cnFactorInteger%281%29%3b%5cnFactorInteger%28-%2010%29%3b%5cnFactorInteger%280%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FactorInteger(10001011);
FactorInteger(1);
FactorInteger(- 10);
FactorInteger(0);
```
Factors an integer, assuming the integer is small enough. 

*NumberOfIntegerDivisors* [NumberOfIntegerDivisors] {CalculatorFunctions::numberOfIntegerDivisors}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22NumberOfIntegerDivisors%286%29%3b%5cnf%7b%7d1%3dSequence%7b%7d%282%29%3b%5cnf%7b%7d%7b%7bx%7d%7d%3df%7b%7d%28x-1%29%20%5c%5ccup%20Sequence%7b%7d%28%20NumberOfIntegerDivisors%28x%29%29%3b%5cna%3df%7b%7d100%3b%5cnMax%20a%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
NumberOfIntegerDivisors(6);
f{}1=Sequence{}(2);
f{}{{x}}=f{}(x-1) \cup Sequence{}( NumberOfIntegerDivisors(x));
a=f{}100;
Max a
```
Computes the number of integer divisors (positive and negative) of a given number. For example, 6 has 8 divisors: 1,2,3,6 and their negatives.

*CoefficientsPowersOf* [CoefficientsPowersOf] {CalculatorFunctions::coefficientsPowersOf}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CoefficientsPowersOf%28x%2c%20a%20x%5e2%2b%20b%20%2a3%20x%20%2bc%20%2b%5c%5cpi%20%2b3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CoefficientsPowersOf(x, a x^2+ b *3 x +c +\pi +3)
```
Extracts the coefficients of the powers of the first argument in the second argument.

*ConstantTerm* [ConstantTerm] {CalculatorFunctions::constantTermRelative}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConstantTerm%28y%2c%20%20x%20y%20x%20%2b3%20%2b2z%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConstantTerm(y,  x y x +3 +2z)
```
Extracts term constant relative to the variable in the first argument. 

*CoefficientOf* [CoefficientOf] {CalculatorFunctions::coefficientOf}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CoefficientOf%28y%2c%20x%20%2a%20x%20%2a%20y%20%2b%20x%20%2a%20z%20%2a%20y%20%2a%20z%20%2b%20x%20%2a%20y%20%2a%20x%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CoefficientOf(y, x * x * y + x * z * y * z + x * y * x)
```
Gets the coefficient of the first argument in the second. 

*IsLinearOrConstantIn* [IsLinearOrConstantIn] {CalculatorFunctions::isLinearOrConstantIn}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsLinearOrConstantIn%28x%2c%20x%5e2%20%2b%201%29%3b%5cnIsLinearOrConstantIn%28x%2c%20x%20y%20%2b%201%29%3b%5cnIsLinearOrConstantIn%28x%2c%20x%20%2b%201%29%3b%5cnIsLinearOrConstantIn%28x%2c%20x%20%5c%5cpi%20%2b%201%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsLinearOrConstantIn(x, x^2 + 1);
IsLinearOrConstantIn(x, x y + 1);
IsLinearOrConstantIn(x, x + 1);
IsLinearOrConstantIn(x, x \pi + 1);
```
Returns one if the second argument is linear in the first. All variables are treated as non-constants. 

*IsProductLinearOrConstantTermsIn* [IsProductLinearOrConstantTermsIn] {CalculatorFunctions::isProductLinearOrConstantTermsIn}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22DistributeMultiplication%5c%22%29%3b%5cnIsProductLinearOrConstantTermsIn%28x%2c%20x%5e2%2b%201%29%3b%5cnIsProductLinearOrConstantTermsIn%28x%2c%20x%5e2-%201%29%3b%5cnIsProductLinearOrConstantTermsIn%28x%2c%20%28x-%201%29%28x%20%2b%201%29%29%3b%5cnIsProductLinearOrConstantTermsIn%28x%2c%20%282x%20%2b%201%29%28x%20%5c%5cpi%20%2b%201%29%29%3b%5cnIsProductLinearOrConstantTermsIn%28x%2c%20%282x%20%2by%29%28x%20%5c%5cpi%20%2b%201%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("DistributeMultiplication");
IsProductLinearOrConstantTermsIn(x, x^2+ 1);
IsProductLinearOrConstantTermsIn(x, x^2- 1);
IsProductLinearOrConstantTermsIn(x, (x- 1)(x + 1));
IsProductLinearOrConstantTermsIn(x, (2x + 1)(x \pi + 1));
IsProductLinearOrConstantTermsIn(x, (2x +y)(x \pi + 1));

```
Returns true if the expression is a product of linear or constant terms. Although this is subject to change, at the moment powers of linear terms are not considered linear. 

*IsProductTermsUpToPower* [IsProductTermsUpToPower] {CalculatorFunctions::isProductTermsUpToPower}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TurnOffRules%28%5c%22DistributeMultiplication%5c%22%29%3b%5cnIsProductTermsUpToPower%28x%5e2%2c%20x%5e2%2b%201%29%3b%5cnIsProductTermsUpToPower%28x%5e3%2cx%28%20x%5e2-%201%29%29%3b%5cnIsProductTermsUpToPower%28x%5e2%2cx%28%20x%5e2-%201%29%29%3b%5cnIsProductTermsUpToPower%28x%2c%20%28x-%201%29%28x%20%2b%201%29%29%3b%5cnIsProductTermsUpToPower%28x%2c%20%282x%20%2b%201%29%28x%20%5c%5cpi%20%2b%201%29%29%3b%5cnIsProductTermsUpToPower%28x%2c%20%282x%20%2by%29%28x%20%5c%5cpi%20%2b%201%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TurnOffRules("DistributeMultiplication");
IsProductTermsUpToPower(x^2, x^2+ 1);
IsProductTermsUpToPower(x^3,x( x^2- 1));
IsProductTermsUpToPower(x^2,x( x^2- 1));
IsProductTermsUpToPower(x, (x- 1)(x + 1));
IsProductTermsUpToPower(x, (2x + 1)(x \pi + 1));
IsProductTermsUpToPower(x, (2x +y)(x \pi + 1));

```
Returns true if the expression is a product of terms of power up to the given power. Although this is subject to change, at the moment powers of linear terms are not considered linear. 

*ScaleToLeadingUnit* [ScaleToLeadingUnit] {CalculatorFunctions::scaleToLeadingUnit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ScaleToLeadingUnit%281%2f2%20x%20%2b%201%2f3%20y%2b%201%2f7%20a%20b%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ScaleToLeadingUnit(1/2 x + 1/3 y+ 1/7 a b)
```
Rescales an expression over the rationals so that the leading term has coefficient 1. 

*AugmentMatrixToTheRight* [AugmentMatrixToTheRight] {CalculatorFunctionsBinaryOps::augmentMatrixToTheRight}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AugmentMatrixToTheRight%28%20%28%281%2c1%29%2c%282%2c2%29%29%2c%20%28%280%2c0%29%2c%281%2c1%29%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AugmentMatrixToTheRight( ((1,1),(2,2)), ((0,0),(1,1)))
```
Augments matrix to the right with another matrix. Pastes the content of the second matrix to the right of the first matrix.The matrices must have the same number of rows. 

*AugmentMatrixBelow* [AugmentMatrixBelow] {CalculatorFunctionsBinaryOps::augmentMatrixBelow}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AugmentMatrixBelow%28%20%28%281%2c1%29%2c%282%2c2%29%29%2c%20%28%280%2c0%29%2c%281%2c1%29%29%29%3b%5cna%20%3d%20FunctionToMatrix%283%2c%202%2c2%29%3b%5cnb%20%3d%20FunctionToMatrix%284%2c%202%2c3%29%3b%5cnAugmentMatrixBelow%28b%2ca%29%3b%5cnAugmentMatrixBelow%28a%2ca%29%3b%5cnAugmentMatrixBelow%28b%2cb%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AugmentMatrixBelow( ((1,1),(2,2)), ((0,0),(1,1)));
a = FunctionToMatrix(3, 2,2);
b = FunctionToMatrix(4, 2,3);
AugmentMatrixBelow(b,a);
AugmentMatrixBelow(a,a);
AugmentMatrixBelow(b,b)
```
Augments matrix below by another matrix. Pastes the content of the second matrix below the first matrix.The matrices must have the same number of columns. 

Operator or function \_ is overloaded with 5 total handlers.

*\_* [LimitBoundaryNotationUnderscore] {CalculatorFunctions::handleUnderscorePowerLimits}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5climits_a%5eb%3b%5cn%5c%5climits%5eb_a%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\limits_a^b;
\limits^b_a
```
Handles expressions of the form \limits_a^b

*\_* [OperatorBoundsUnderscore] {CalculatorFunctions::operatorBounds}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%5ea_b%20f%20dx%3b%5cn%5c%5cint_a%5eb%20f%20dx%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int^a_b f dx;
\int_a^b f dx
```
Takes care of the integral superscript notation \int^a 

*\_* [DereferenceSequenceOrMatrix] {CalculatorFunctions::dereferenceSequenceOrMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20%28a%2c%20b%2c%20c%29%3b%5cnX_1%3b%5cnX_2%3b%5cnX_3%3b%5cnX_4%3b%5cnX_j%3b%5cnj%20%3d%203%3b%5cnX_j%3b%5cnDenominations%20%3d%20%281%2c%205%2c%2010%2c%2025%2c50%2c%20100%2c200%2c%20500%2c%201000%2c%202000%2c%205000%29%3b%5cnp%20%280%2c%200%20%29%3d1%3b%5cnp%20%28%7b%7ba%7d%7d%2c%20%7b%7bx%7d%7d%29%3aif%20%28x%20%3c%200%29%20%3d%200%3b%5cnp%20%280%2c%20%7b%7bx%7d%7d%29%3d%200%3b%5cnp%20%28%7b%7ba%7d%7d%2c%20%7b%7bx%7d%7d%20%29%3d%20p%28a-%201%2cx%29%20%2b%20p%28a%2c%20x-Denominations_a%29%3b%5cnp%20%2811%2c%20100%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = (a, b, c);
X_1;
X_2;
X_3;
X_4;
X_j;
j = 3;
X_j;
Denominations = (1, 5, 10, 25,50, 100,200, 500, 1000, 2000, 5000);
p (0, 0 )=1;
p ({{a}}, {{x}}):if (x < 0) = 0;
p (0, {{x}})= 0;
p ({{a}}, {{x}} )= p(a- 1,x) + p(a, x-Denominations_a);
p (11, 100)
```
Dereferences a sequence or a matrix. The syntax is as illustrated by the example. 

*\_* [DereferenceInterval] {CalculatorFunctions::dereferenceInterval}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25UseBracketForIntervals%5cn%5ba%2c%20b%29_1%3b%5cn%28a%2c%20b%5d_2%3b%5cn%5ba%2c%20b%5d_1%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%UseBracketForIntervals
[a, b)_1;
(a, b]_2;
[a, b]_1; 
```
Dereferences an interval, as illustrated by the example. 

*\_* [DereferenceSequenceStatements] {CalculatorFunctions::dereferenceSequenceStatements}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20d%2fdx%28%20%5c%5csqrt%28x%20%2b%20y%29%20-%204x%5e2y%5e2%29%3b%5cn%28d%2fdx%28y%29%3d%200%3b%20A%29_2%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = d/dx( \sqrt(x + y) - 4x^2y^2);
(d/dx(y)= 0; A)_2;
```
Dereferences a sequence of rules. The syntax is as illustrated by the example. 

*Value* [Value] {CalculatorFunctions::valueOfModularExpression}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Value%283%20mod%207%29%3b%5cnValue%289%20mod%207%29%3b%5cna%20%3d%20Value%28%28PolynomialModP%28x%5e9%2c%207%29%29%20mod%20%28PolynomialModP%28x%5e5%2bx%2b1%2c%207%29%29%29%3b%5cnValue%20a%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Value(3 mod 7);
Value(9 mod 7);
a = Value((PolynomialModP(x^9, 7)) mod (PolynomialModP(x^5+x+1, 7)));
Value a
```
Given an expression of the form a mod b, returns the element a. When the input is a modular polynomial, converts the polynomial to a rational. 

*\setminus* [\setminus] {CalculatorFunctionsBinaryOps::setMinus}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28x%2cy%2ct%29%20%5c%5csetminus%20Sequence%7b%7dx%3b%5cn%28x%2cy%29%5c%5csetminus%20%28z%2cy%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(x,y,t) \setminus Sequence{}x;
(x,y)\setminus (z,y)
```
Removes the elements of the second set from the elements of the first set. The outputs will be sorted in ascending order. 

Operator or function Differential is overloaded with 2 total handlers.

*Differential* [DifferentialStandardHandler] {CalculatorFunctions::differentialStandardHandler}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cint%20%5c%5ctheta%20%7b%5c%5ctext%20d%7d%20%5c%5ctheta%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\int \theta {\text d} \theta
```
Transforms Differential{}a to the standard internal form Differential {}(a, 1).

*Differential* [DifferentialOfPolynomial] {CalculatorFunctions::differentialOfPolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Differential%7b%7d%28Polynomial%7b%7d%28x%2by%29%29%3b%5cnDifferential%7b%7d%28Polynomial%7b%7d%28x%5e3y%20z%2by%20z%20%2b%20x%20y%20%2b%20x%5e5%20y%5e2%20z%20%2b%20x%20y%5e2%20z%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Differential{}(Polynomial{}(x+y));
Differential{}(Polynomial{}(x^3y z+y z + x y + x^5 y^2 z + x y^2 z));

```
Differential of a polynomial.

*Max* [Max] {CalculatorFunctions::maximum}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Max%28-%204%2c2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Max(- 4,2)
```
Maximum function.

*Min* [Min] {CalculatorFunctions::minimum}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Min%28-%204%2c2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Min(- 4,2)
```
Maximum function.

*Contains* [Contains] {CalculatorFunctions::contains}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Contains%20%28%28%5c%5carcsin%20x%20%2b%5c%5ccos%20x%2c5%29%2c%20%5c%5carcsin%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Contains ((\arcsin x +\cos x,5), \arcsin )
```
Returns 1 if the first argument contains the second as a sub-expression, else returns 0. Function has not been optimized for speed, use with caution. 

*ExpressionLeafs* [ExpressionLeafs] {CalculatorFunctions::expressionLeafs}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ExpressionLeafs%20%28%28%5c%5carcsin%20x%20%2b%5c%5ccos%20x%2c5%29%2c%20%5c%5carcsin%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ExpressionLeafs ((\arcsin x +\cos x,5), \arcsin )
```
Returns a sequence without repetition of all leafs making up an expression. Here, a ``leaf'' means either an atomic expression or a built-in type. Built-in types are not broken into atoms. 

*Last* [Last] {CalculatorFunctionsListsAndSets::lastElement}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Last%281%2c2%29%3b%5cnLast%28%281%2c%202%29%29%3b%5cnLast%7b%7d%28%29%3b%5cnLast%7b%7d%28%28%29%29%3b%5cna%20%3d%20%281%2c2%29%3b%5cnLast%20a%3b%5cnLast%20Sequence%7b%7d%28a%29%3b%5cnp%7b%7d%28%28%29%2c%200%29%20%3d%201%3b%5cnp%7b%7d%28%7b%7bx%7d%7d%2c%7b%7bn%7d%7d%29%3a%20if%20%28n%20%3c%200%29%20%3d%200%3b%5cnp%7b%7d%28%28%29%2c%20%7b%7bn%7d%7d%29%20%3d%200%3b%5cnp%7b%7d%28%7b%7bx%7d%7d%2c%20%7b%7bn%7d%7d%29%20%3d%20p%7b%7d%28x%2c%20n%20-%20Last%20x%29%20%2bp%7b%7d%28RemoveLast%20x%2c%20n%29%3b%5cnp%7b%7d%28%281%2c%202%2c%205%2c%2010%2c%2025%2c%20100%29%2c%20100%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Last(1,2);
Last((1, 2));
Last{}();
Last{}(());
a = (1,2);
Last a;
Last Sequence{}(a);
p{}((), 0) = 1;
p{}({{x}},{{n}}): if (n < 0) = 0;
p{}((), {{n}}) = 0;
p{}({{x}}, {{n}}) = p{}(x, n - Last x) +p{}(RemoveLast x, n);
p{}((1, 2, 5, 10, 25, 100), 100);
```
Returns the last element of the expression, provided the argument has no bound variables. If the expression has bound variables does nothing.

*RemoveLast* [RemoveLast] {CalculatorFunctionsListsAndSets::removeLastElement}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X%20%3d%20%28a%2cb%2cc%29%3b%5cnY%20%3d%20%28RemoveLast%20X%29%5c%5ccup%20Sequence%7b%7d%28Last%20X%29%20-%20X%3b%5cnRemoveLast%7b%7d%28%28%29%29%3b%5cnRemoveLast%7b%7d%28%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X = (a,b,c);
Y = (RemoveLast X)\cup Sequence{}(Last X) - X;
RemoveLast{}(());
RemoveLast{}();

```
Returns a list with the last element removed, provided the argument has no bound variables. If the expression has bound variables does nothing.

*ConvertBase64ToHex* [ConvertBase64ToHex] {CalculatorFunctionsEncoding::convertBase64ToHex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertBase64ToHex%28%5c%22AQAB%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertBase64ToHex("AQAB");
```
Converts base64 string to hexadecimal string. 

*ConvertStringToHex* [ConvertStringToHex] {CalculatorFunctionsEncoding::convertStringToHex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertStringToHex%28Sha256%28Sha256%28%5c%22hello%5c%22%29%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertStringToHex(Sha256(Sha256("hello")));
```
Converts a bitstream (not necessarily UTF-8 encoded) to hex. 

*ConvertHexToInteger* [ConvertHexToInteger] {CalculatorFunctionsEncoding::hexToInteger}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertHexToInteger%28Base64ToHex%28%5c%22AQAB%5c%22%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertHexToInteger(Base64ToHex("AQAB"));
```
Converts a hex string to an integer. 

*ConvertIntegerToHex* [ConvertIntegerToHex] {CalculatorFunctionsEncoding::integerToHex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertIntegerToHex%2865537%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertIntegerToHex(65537);
```
Converts an integer to hex string. 

*ConvertHexToString* [ConvertHexToString] {CalculatorFunctionsEncoding::hexToString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertHexToString%28%5c%223031300d060960864801650304020105000420%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertHexToString("3031300d060960864801650304020105000420");
```
Converts a hex string to a string. 

*URLStringToNormalString* [URLStringToNormalString] {CalculatorFunctions::urlStringToNormalString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22URLStringToNormalString%28%5c%22randomSeed%253d92742048%2526submissionsAlgebraAnswer%253%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
URLStringToNormalString("randomSeed%3d92742048%26submissionsAlgebraAnswer%3")
```
Converts an url-encoded string to a normal string. 

*ConvertStringToURL* [ConvertStringToURL] {CalculatorFunctions::stringToURL}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertStringToURL%28%5c%22%2b%20%25%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertStringToURL("+ %")
```
Converts a normal string to a url-encoded one. 

*URLKeyValuePairsDecode* [URLKeyValuePairsDecode] {CalculatorFunctions::urlStringToNormalString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22URLKeyValuePairsDecode%28%5c%22Problems%252fFunctions%252dcomposing%252dfractional%252dlinear%252d1.html%3dweight%253d3%2526deadlines%253d%2526%26Problems%252fLimits%252dbasic%252dsubstitution%252d1.html%3dweight%253d3%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dinfinity%252dRF%252dequal%252ddeg%252d1.html%3dweight%253d2%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dminus%252dinfinity%252dRF%252dequal%252ddeg%252d1.html%3dweight%253d1%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dpm%252dinfinity%252dRF%252dnum%252ddeg%252dsmaller%252d1.html%20%3dweight%253d1%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dinfinity%252dRF%252ddeg%252dden%252dsmaller%252d1.html%20%3dweight%253d1%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dminus%252dinfinity%252dRF%252ddeg%252dden%252dsmaller%252d1.html%20%3dweight%253d5%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dminus%252dinfinity%252dRF%252ddeg%252dden%252dsmaller%252d2.html%20%3dweight%253d6%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dpm%252dinfinity%252dquotient%252dradical%252deven%252dpower%252dbasic%20%25%202d1.html%20%3dweight%253d1%2526deadlines%253d%2526%26Problems%252fLimits%252dx%252dtends%252dto%252dminus%252dinfinity%252dquotient%252dradical%252dodd%252dpower%252dbasic%20%25%202d1.html%20%3dweight%253d1%2526deadlines%253d%2526%26Homework%252fPrecalculus%252dPrerequisites%252dUMB%252d1.html%20%3d%20deadlines%253d%2526%26%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
URLKeyValuePairsDecode("Problems%2fFunctions%2dcomposing%2dfractional%2dlinear%2d1.html=weight%3d3%26deadlines%3d%26&Problems%2fLimits%2dbasic%2dsubstitution%2d1.html=weight%3d3%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dinfinity%2dRF%2dequal%2ddeg%2d1.html=weight%3d2%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2dequal%2ddeg%2d1.html=weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dpm%2dinfinity%2dRF%2dnum%2ddeg%2dsmaller%2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d1.html =weight%3d5%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d2.html =weight%3d6%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dpm%2dinfinity%2dquotient%2dradical%2deven%2dpower%2dbasic % 2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dquotient%2dradical%2dodd%2dpower%2dbasic % 2d1.html =weight%3d1%26deadlines%3d%26&Homework%2fPrecalculus%2dPrerequisites%2dUMB%2d1.html = deadlines%3d%26&")
```
Converts an url-encoded string to a normal string. All % signs are interpreted recursively as double, triple, ... url encoded strings and decoded accordingly.

*ConvertIntegerToBase58* [ConvertIntegerToBase58] {CalculatorFunctionsEncoding::convertIntegerUnsignedToBase58}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22theInt%20%3d%20ConvertHexToInteger%28ConvertBase58ToHex%28%5c%221Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK%5c%22%29%29%3bConvertIntegerToBase58%28theInt%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
theInt = ConvertHexToInteger(ConvertBase58ToHex("1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK"));ConvertIntegerToBase58(theInt)
```
Converts an unsigned integer to base58. 

*ConvertBase58ToHex* [ConvertBase58ToHex] {CalculatorFunctionsEncoding::convertBase58ToHex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertBase58ToHex%28%5c%221Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertBase58ToHex("1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK");
```
Converts Base58 to hex. 

*AESCBCEncrypt* [AESCBCEncrypt] {CalculatorFunctionsCrypto::aes_cbc_256_encrypt}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22text%20%3d%20ConvertHexToString%20%5c%226bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710%5c%22%3b%5cnkey%20%3d%20ConvertHexToString%20%5c%22603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4%5c%22%3b%5cnConvertStringToHex%20AESCBCEncrypt%28key%2c%20text%29%3b%5cn%5c%22f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b%5c%22%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
text = ConvertHexToString "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710";
key = ConvertHexToString "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4";
ConvertStringToHex AESCBCEncrypt(key, text);
"f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b";

```
Encodes using aes 256 bit in cbc (cipher block chain) mode. First argument = key. Second argument = text. Reference: NIST SP 800-38A.

*AESCBCDecrypt* [AESCBCDecrypt] {CalculatorFunctionsCrypto::aes_cbc_256_decrypt}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22text%20%3d%20%5c%226bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710%5c%22%3b%5cnkey%20%3d%20%5c%22603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4%5c%22%3b%5cncipherText%20%3d%20ConvertStringToHex%20AESCBCEncrypt%28ConvertHexToString%20key%2c%20ConvertHexToString%20text%29%3b%5cncipherText%3b%5cn%5c%22f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b%5c%22%3b%5cnConvertStringToHex%20AESCBCDecrypt%28ConvertHexToString%20key%2c%20ConvertHexToString%20cipherText%29%3b%5cntext%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
text = "6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710";
key = "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4";
cipherText = ConvertStringToHex AESCBCEncrypt(ConvertHexToString key, ConvertHexToString text);
cipherText;
"f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b";
ConvertStringToHex AESCBCDecrypt(ConvertHexToString key, ConvertHexToString cipherText);
text
```
AES decryption. First argument key, second argument - text. 

*AppendDoubleSha256Check* [AppendDoubleSha256Check] {CalculatorFunctionsCrypto::appendDoubleSha256Check}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%3d%20%5c%2280aad3f1f5de25ff67a4fd3d7808d58510e00ec08a55c10ad5751facf35411509701%5c%22%3b%5cnB%3d%20ConvertHexToString%28A%29%3b%5cnC%3d%20AppendDoubleSha256Check%28B%29%3b%5cnD%3d%20ConvertStringToHex%28C%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A= "80aad3f1f5de25ff67a4fd3d7808d58510e00ec08a55c10ad5751facf35411509701";
B= ConvertHexToString(A);
C= AppendDoubleSha256Check(B);
D= ConvertStringToHex(C)
```
Appends a sha 256 checksum to a string. More precisely, appends the first 4 bytes of sha256 of the string to the string. 

*ConvertHexToBase58* [ConvertHexToBase58] {CalculatorFunctionsEncoding::convertHexToBase58}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertHexToBase58%28%5c%2203aaf2d5530b1a5cbf80c248ca44635ac265f4104ffc5b76ef48f361c03b7f536f%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertHexToBase58("03aaf2d5530b1a5cbf80c248ca44635ac265f4104ffc5b76ef48f361c03b7f536f");
```
Converts hex to base58. 

*CharToBase64* [CharToBase64] {CalculatorFunctionsEncoding::charToBase64}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CharToBase64%28%5c%22pleasure.%5c%22%29%3b%5cnCharToBase64%28%5c%22leasure.%5c%22%29%3b%5cnCharToBase64%28%5c%22easure.%5c%22%29%3b%5cnCharToBase64%28%5c%22asure.%5c%22%29%3b%5cnCharToBase64%28%5c%22sure.%5c%22%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CharToBase64("pleasure.");
CharToBase64("leasure.");
CharToBase64("easure.");
CharToBase64("asure.");
CharToBase64("sure.");

```
Converts characters to bit stream and the bitstream to base64. The character to bit stream conversion is not fixed at the moment and may be system/compiler dependent. I believe that the character to bit stream conversion should be standard for the standard letters in the alphabet. Fancy UTF8 will probably be not read correctly from the CGI input, and furthermore will not be converted in a standard fashion to bit stream. The examples below are taken from Wikipedia. 

*ConvertBase64ToString* [ConvertBase64ToString] {CalculatorFunctionsEncoding::convertBase64ToString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertBase64ToString%28%5c%22k7qTF1hLeOdihfKG5IRnlb7us2FVo1pSC2r0DVLkYwRAQHMs4XatvGcdG81S64uoaqG4fZ9IHJNpZjqokojuX5VIwl6utBO9%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertBase64ToString("k7qTF1hLeOdihfKG5IRnlb7us2FVo1pSC2r0DVLkYwRAQHMs4XatvGcdG81S64uoaqG4fZ9IHJNpZjqokojuX5VIwl6utBO9");
```
Converts base64 to string

*loadKnownCertificates* [loadKnownCertificates] {CalculatorFunctionsCrypto::loadKnownCertificates}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22loadKnownCertificates%280%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
loadKnownCertificates(0);
```
Loads known security certificates from the <a href="/certificates-public/">certificates-public/</a> folder. 

*TestLoadPEMCertificate* [TestLoadPEMCertificate] {CalculatorFunctionsCrypto::testLoadPEMCertificates}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestLoadPEMCertificate%28ConvertBase64ToString%28LoadFileIntoString%28%5c%22test%2fcertificate_self_signed.base64%5c%22%29%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestLoadPEMCertificate(ConvertBase64ToString(LoadFileIntoString("test/certificate_self_signed.base64")));

```
Tests the pem parsing functions. 

*TestLoadPEMPrivateKey* [TestLoadPEMPrivateKey] {CalculatorFunctionsCrypto::testLoadPEMPrivateKey}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestLoadPEMPrivateKey%28ConvertBase64ToString%28LoadFileIntoString%28%5c%22test%2fprivate_key.base64%5c%22%29%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestLoadPEMPrivateKey(ConvertBase64ToString(LoadFileIntoString("test/private_key.base64")));

```
Tests the pem private key functions. 

*TestTLSMessageSequence* [TestTLSMessageSequence] {CalculatorFunctionsCrypto::testTLSMessageSequence}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnTestTLSMessageSequence%28%5cnLoadFileIntoString%28%5c%22test%2fprivate_key.pem%5c%22%29%2c%5cnLoadFileIntoString%28%5c%22test%2fcertificate_self_signed.pem%5c%22%29%2c%5cnConvertHexToString%28LoadFileIntoString%28%5c%22test%2fmessage_client_ssl_0.hex%5c%22%29%29%5cn%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
TestTLSMessageSequence(
LoadFileIntoString("test/private_key.pem"),
LoadFileIntoString("test/certificate_self_signed.pem"),
ConvertHexToString(LoadFileIntoString("test/message_client_ssl_0.hex"))
);

```
Creates a TLS server and test-sends messages to it starting with the client hello. The first argument will be the server's private key. The second argument will be the server's certificate. The next argument will be the client hello. 

*TestTLSDecodeSSLRecord* [TestTLSDecodeSSLRecord] {CalculatorFunctionsCrypto::testTLSDecodeSSLRecord}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnTestTLSDecodeSSLRecord%28%5cnLoadFileIntoString%28%5c%22test%2fmessage_client_ssl_0.hex%5c%22%29%5cn%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
TestTLSDecodeSSLRecord(
LoadFileIntoString("test/message_client_ssl_0.hex")
)
```
Decodes a client hello. 

*TestASN1Decode* [TestASN1Decode] {CalculatorFunctionsCrypto::testASN1Decode}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnTestASN1Decode%28ConvertBase64ToString%28LoadFileIntoString%28%5c%22test%2fcertificate_self_signed.base64%5c%22%29%29%29%3b%5cnTestASN1Decode%28ConvertBase64ToString%28LoadFileIntoString%28%5c%22test%2fprivate_key.base64%5c%22%29%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
TestASN1Decode(ConvertBase64ToString(LoadFileIntoString("test/certificate_self_signed.base64")));
TestASN1Decode(ConvertBase64ToString(LoadFileIntoString("test/private_key.base64")));

```
Tests decoding of abstract syntax one. 

*X509CertificateServer* [X509CertificateServer] {CalculatorFunctionsCrypto::X509CertificateServer}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22X509CertificateServer%200%3b%5cnX509CertificateDecode%20X509CertificateServer%200%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
X509CertificateServer 0;
X509CertificateDecode X509CertificateServer 0
```
Returns the source ofo the X509 certificate of this server. 

*X509CertificateDecode* [X509CertificateDecode] {CalculatorFunctionsCrypto::x509CertificateDecode}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnX509CertificateDecode%5cnLoadFileIntoString%28%5c%22test%2fcertificate_self_signed.pem%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
X509CertificateDecode
LoadFileIntoString("test/certificate_self_signed.pem")
```
Decodes a X509 certificate (the content of a .crt file, starts with -----BEGIN CERTIFICATE-----) to a string. 

*ShowKnownASNObjectIds* [ShowKnownASNObjectIds] {CalculatorFunctionsCrypto::showKnownObjectIds}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ShowKnownASNObjectIds%200%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ShowKnownASNObjectIds 0
```
Lists all abstract syntax one object ids hard-coded in the calculator.

*JWTVerifyAgainstKnownKeys* [JWTVerifyAgainstKnownKeys] {CalculatorFunctionsCrypto::jwtVerifyAgainstKnownKeys}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%20JWTVerifyAgainstKnownKeys%28%5c%22%5c%22%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS JWTVerifyAgainstKnownKeys(""); 
```
Tries to verify a Json Web Token.

*JWTVerifyRSA256* [JWTVerifyRSA256] {CalculatorFunctionsCrypto::jwtVerifyAgainstRSA256}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%20token%20%3d%5c%22eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_dO--xi12jzDwusC-eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AXLIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw%5c%22%3b%5cnmodulus%20%3d%20%5c%22ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ%5c%22%3b%5cnexponent%20%3d%5c%22AQAB%5c%22%3b%5cnJWTVerifyRSA256%28token%2cmodulus%2cexponent%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS token ="eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_dO--xi12jzDwusC-eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AXLIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw";
modulus = "ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ";
exponent ="AQAB";
JWTVerifyRSA256(token,modulus,exponent);
```
Tries to verify a Json Web Token with respect to a given rsa modulus and exponent. The reference JWT token was taken from: https://tools.ietf.org/html/rfc7515#page-38, Appendix A.2.

*Sha1* [Sha1] {CalculatorFunctionsCrypto::sha1OfString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Sha1%28%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20dog%5c%22%29%3b%5cnSha1%28%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20cog%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Sha1("The quick brown fox jumps over the lazy dog");
Sha1("The quick brown fox jumps over the lazy cog");
```
Converts characters to a sequence of bits and computes the sha1 hash value of those bits. The examples below are taken from Wikipedia. 

*Sha224* [Sha224] {CalculatorFunctionsCrypto::sha224OfString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Sha224%28%5c%22%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Sha224("");
```
Converts characters to a sequence of bits and computes the sha224 hash value of those bits. Reference: Wikipedia. 

*TestRSASign* [TestRSASign] {CalculatorFunctionsCrypto::testRSASign}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestRSASign%28%5c%22asdf%5c%22%2c%20426783863749219482096610996314660012394857818566077035178241209143920182457158933390658353396537264757052464334913365120085767481521352901499860151171619220023060480464866565560676440226021303%2c%20352815577859200421265693524055335110516168016791939066619744890606270245172389068191436591061733904616135797438741188808884326847918800555526459128765065042606467292598531117932066982164093023%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestRSASign("asdf", 426783863749219482096610996314660012394857818566077035178241209143920182457158933390658353396537264757052464334913365120085767481521352901499860151171619220023060480464866565560676440226021303, 352815577859200421265693524055335110516168016791939066619744890606270245172389068191436591061733904616135797438741188808884326847918800555526459128765065042606467292598531117932066982164093023)
```
Tests the RSA signature function. Input has three arguments: 1) message to sign 2) private key first prime, 3) private key second prime. Will set hard-coded defaults for all non-specified parameters. 

*GenerateRandomPrime* [GenerateRandomPrime] {CalculatorFunctionsCrypto::generateRandomPrime}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GenerateRandomPrime%2810%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GenerateRandomPrime(10)
```
Generate random prime. Argument = desired number of bytes, max 128. 

*RSAEncrypt* [RSAEncrypt] {CalculatorFunctionsCrypto::RSAEncrypt}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22RSAEncrypt%283233%2c%2017%2c%2065%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
RSAEncrypt(3233, 17, 65)
```
Encrypts with RSA. First argument: modulus. Second argument: (public) exponent. Third argument: message given as a large integer.

*Ripemd160* [Ripemd160] {CalculatorFunctionsCrypto::ripemd160OfString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertStringToHex%20Ripemd160%28%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20dog%5c%22%29%3b%5cn%5c%2237f332f68db77bd9d7edd4969571ad671cf9dd3b%5c%22%3b%5cnConvertStringToHex%20Ripemd160%28%5c%22The%20quick%20brown%20fox%20jumps%20over%20the%20lazy%20cog%5c%22%29%3b%5cn%5c%22132072df690933835eb8b6ad0b77e7b6f14acad7%5c%22%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertStringToHex Ripemd160("The quick brown fox jumps over the lazy dog");
"37f332f68db77bd9d7edd4969571ad671cf9dd3b";
ConvertStringToHex Ripemd160("The quick brown fox jumps over the lazy cog");
"132072df690933835eb8b6ad0b77e7b6f14acad7";

```
Ripemd160 hash function. See wikipedia page. 

*ShaThree256* [ShaThree256] {CalculatorFunctionsCrypto::sha3_256OfString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnConvertStringToHex%20ShaThree256%28%5c%22%5c%22%29%3b%5cn%5c%22a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a%5c%22%3b%5cnConvertStringToHex%20ShaThree256%28%5c%22abc%5c%22%29%3b%5cn%5c%223a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532%5c%22%3b%5cnConvertStringToHex%20ShaThree256%28%5c%22testing%5c%22%29%3b%5cn%5c%227f5979fb78f082e8b1c676635db8795c4ac6faba03525fb708cb5fd68fd40c5e%5c%22%3b%5cnConvertStringToHex%20ShaThree256%5c%22abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq%5c%22%3b%5cn%5c%2241c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376%5c%22%3b%5cnConvertStringToHex%20ShaThree256%5c%22abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu%5c%22%3b%5cn%5c%22916f6061fe879741ca6469b43971dfdb28b1a32dc36cb3254e812be27aad1d18%5c%22%3b%5cnConvertStringToHex%20Keccak256%28%5c%22testing%5c%22%29%3b%5cn%5c%225f16f4c7f149ac4f9510d9cf8cf384038ad348b3bcdc01915f95de12df9d1b02%5c%22%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
ConvertStringToHex ShaThree256("");
"a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a";
ConvertStringToHex ShaThree256("abc");
"3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532";
ConvertStringToHex ShaThree256("testing");
"7f5979fb78f082e8b1c676635db8795c4ac6faba03525fb708cb5fd68fd40c5e";
ConvertStringToHex ShaThree256"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
"41c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376";
ConvertStringToHex ShaThree256"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
"916f6061fe879741ca6469b43971dfdb28b1a32dc36cb3254e812be27aad1d18";
ConvertStringToHex Keccak256("testing");
"5f16f4c7f149ac4f9510d9cf8cf384038ad348b3bcdc01915f95de12df9d1b02";

```
SHA3 of input string, 256 bit version. See the wikipedia page on SHA3. 

*Keccak256* [Keccak256] {CalculatorFunctionsCrypto::keccak256OfString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConvertStringToHex%20ShaThree256%28%5c%22abc%5c%22%29%3b%5cn%5c%223a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532%5c%22%3b%5cnConvertStringToHex%20ShaThree256%28%5c%22%5c%22%29%3b%5cn%5c%22a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a%5c%22%3b%5cnConvertStringToHex%20ShaThree256%28%5c%22testing%5c%22%29%3b%5cn%5c%227f5979fb78f082e8b1c676635db8795c4ac6faba03525fb708cb5fd68fd40c5e%5c%22%3b%5cnConvertStringToHex%20Keccak256%28%5c%22testing%5c%22%29%3b%5cn%5c%225f16f4c7f149ac4f9510d9cf8cf384038ad348b3bcdc01915f95de12df9d1b02%5c%22%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConvertStringToHex ShaThree256("abc");
"3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532";
ConvertStringToHex ShaThree256("");
"a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a";
ConvertStringToHex ShaThree256("testing");
"7f5979fb78f082e8b1c676635db8795c4ac6faba03525fb708cb5fd68fd40c5e";
ConvertStringToHex Keccak256("testing");
"5f16f4c7f149ac4f9510d9cf8cf384038ad348b3bcdc01915f95de12df9d1b02";

```
Keccak256 of input string, 256 bit version. This is ``non-stardard sha3'' and is different from the sha3. See the wikipedia page on SHA3/Keccak. 

*Sha256* [Sha256] {CalculatorFunctionsCrypto::sha256OfString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnConvertStringToHex%20Sha256%28%5c%22%5c%22%29%3b%5cn%5c%22e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855%5c%22%3b%5cnConvertStringToHex%20Sha256%28%5c%22abc%5c%22%29%3b%5cn%5c%22ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad%5c%22%3b%5cnConvertStringToHex%20Sha256%28%5c%22abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu%5c%22%29%3b%5cn%5c%22cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1%5c%22%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
ConvertStringToHex Sha256("");
"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
ConvertStringToHex Sha256("abc");
"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
ConvertStringToHex Sha256("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");
"cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1";

```
Converts characters to a sequence of bits and computes the sha256 hash value of those bits. Reference: Wikipedia. 

*Sha512* [Sha512] {CalculatorFunctionsCrypto::sha512}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%25HideLHS%5cnConvertStringToHex%20Sha512%28%5c%22%5c%22%29%3b%5cn%5c%22cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e%5c%22%3b%5cnConvertStringToHex%20Sha512%28%5c%22abc%5c%22%29%3b%5cn%5c%22ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f%5c%22%3b%5cnConvertStringToHex%20Sha512%28%5c%22abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu%5c%22%29%3b%5cn%5c%228e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909%5c%22%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
%HideLHS
ConvertStringToHex Sha512("");
"cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e";
ConvertStringToHex Sha512("abc");
"ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f";
ConvertStringToHex Sha512("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");
"8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909";

```
Converts characters to a sequence of bits and computes SHA512. UTF8 encoding is used for example space bar is converted to 0x32. Reference: Wikipedia. 

*Sha256Verbose* [Sha256Verbose] {CalculatorFunctionsCrypto::sha256OfStringVerbose}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Sha256Verbose%28%5c%22%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Sha256Verbose("");
```
Same as Sha256 but more verbose. 

*FetchKnownPublicKeys* [FetchKnownPublicKeys] {CalculatorFunctions::fetchKnownPublicKeys}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FetchKnownPublicKeys%280%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FetchKnownPublicKeys(0)
```
Updates known public keys. Requires administrator privileges. At the moment, works for google public keys only. 

*EllipticCurveOrderNIST* [EllipticCurveOrderNIST] {CalculatorFunctions::nistEllipticCurveOrder}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g%20%3d%20EllipticCurveGeneratorNIST%28%5c%22secp256k1%5c%22%29%3b%5cnorder%20%3d%20EllipticCurveOrderNIST%28%5c%22secp256k1%5c%22%29%3b%5cng%5eorder%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g = EllipticCurveGeneratorNIST("secp256k1");
order = EllipticCurveOrderNIST("secp256k1");
g^order
```
Get a NIST curve order. At present implemented for secp256k1 only. 

*EllipticCurveGeneratorNIST* [EllipticCurveGeneratorNIST] {CalculatorFunctions::nistEllipticCurveGenerator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g%3dEllipticCurveGeneratorNIST%28%5c%22secp256k1%5c%22%29%3b%5cng%5e3%3b%20g%5e115792089237316195423570985008687907852837564279074904382605163141518161494337%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g=EllipticCurveGeneratorNIST("secp256k1");
g^3; g^115792089237316195423570985008687907852837564279074904382605163141518161494337
```
Makes generator of a NIST curve. At present implemented for secp256k1 only. 

*SylvesterMatrix* [SylvesterMatrix] {CalculatorConversions::sylvesterMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SylvesterMatrix%282x%5e2%2b2x%2b2%2c%203x%2b3%29%3b%5cnSylvesterMatrix%28Polynomial%285x%5e2%2b4x%2b3%29%2c%20Polynomial%282x%2b1%29%29%3b%5cnSylvesterMatrix%282%2c%203x%2b3%29%3b%5cnSylvesterMatrix%28PolynomialModP%282x%5e2%2b2x%2b2%2c%205%29%2c%20PolynomialModP%283x%2b3%2c%205%29%29%3b%5cnSylvesterMatrix%28x%5e2%2b2x%2b3%2c%204x%2b5%2c%206x%2b7%29%3b%5cnSylvesterMatrix%28PolynomialModP%282x%5e2%2b2x%2b2%2c%205%29%2c%20PolynomialModP%283x%2b3%2c%207%29%29%3b%5cnSylvesterMatrix%280%2c%20x%5e2%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SylvesterMatrix(2x^2+2x+2, 3x+3);
SylvesterMatrix(Polynomial(5x^2+4x+3), Polynomial(2x+1));
SylvesterMatrix(2, 3x+3);
SylvesterMatrix(PolynomialModP(2x^2+2x+2, 5), PolynomialModP(3x+3, 5));
SylvesterMatrix(x^2+2x+3, 4x+5, 6x+7);
SylvesterMatrix(PolynomialModP(2x^2+2x+2, 5), PolynomialModP(3x+3, 7));
SylvesterMatrix(0, x^2);

```
Constructs the transpose Sylvester matrix of two univariate polynomials.

*ConvertAlgebraicNumberToMatrix* [ConvertAlgebraicNumberToMatrix] {CalculatorFunctions::convertAlgebraicNumberToMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20%5c%5csqrt%7b2%7d%3b%5cnA%20%3d%20ConvertAlgebraicNumberToMatrix%28a%29%3b%5cnb%20%3d%20%5c%5csqrt%7b3%7d%3b%5cnB%20%3d%20ConvertAlgebraicNumberToMatrix%28b%29%3b%5cnc%20%3d%20%5c%5csqrt%7b6%7d%3b%5cnC%20%3d%20ConvertAlgebraicNumberToMatrix%28c%29%3b%5cnA%20%5c%5cotimes%20B%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = \sqrt{2};
A = ConvertAlgebraicNumberToMatrix(a);
b = \sqrt{3};
B = ConvertAlgebraicNumberToMatrix(b);
c = \sqrt{6};
C = ConvertAlgebraicNumberToMatrix(c);
A \otimes B
```
Converts the algebraic number to its internal matrix representation. 

*PrintAlgebraicClosureStatus* [PrintAlgebraicClosureStatus] {CalculatorFunctions::printAlgebraicClosureStatus}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AlgebraicNumberFromPolynomial%28x%5e2%20-%206%29%3b%5cnPrintAlgebraicClosureStatus%200%3b%5cnAlgebraicNumberFromPolynomial%28x%5e2%20-%202%29%3b%5cnPrintAlgebraicClosureStatus%200%3b%5cnAlgebraicNumberFromPolynomial%28x%5e2%20-%203%29%3b%5cnPrintAlgebraicClosureStatus%200%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AlgebraicNumberFromPolynomial(x^2 - 6);
PrintAlgebraicClosureStatus 0;
AlgebraicNumberFromPolynomial(x^2 - 2);
PrintAlgebraicClosureStatus 0;
AlgebraicNumberFromPolynomial(x^2 - 3);
PrintAlgebraicClosureStatus 0;

```
Prints in human-redable form the state of the ambient algebraic closure. 

*AlgebraicNumberFromPolynomial* [AlgebraicNumberFromPolynomial] {CalculatorFunctions::getAlgebraicNumberFromMinPoly}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AlgebraicNumberFromPolynomial%7b%7d%28x%5e2%20-%204%29%3b%5cnAlgebraicNumberFromPolynomial%7b%7d%28x%5e2%20-%20%283%20%2b%202sqrt%282%29%29%29%3b%5cnAlgebraicNumberFromPolynomial%7b%7d%28x%5e3%20-%20%287%20%2b%205sqrt%282%29%29%29%3b%5cnAlgebraicNumberFromPolynomial%7b%7d%28x%5e3%20%2b%20%5c%5csqrt%7b2%7dx%20%2b%201%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AlgebraicNumberFromPolynomial{}(x^2 - 4);
AlgebraicNumberFromPolynomial{}(x^2 - (3 + 2sqrt(2)));
AlgebraicNumberFromPolynomial{}(x^3 - (7 + 5sqrt(2)));
AlgebraicNumberFromPolynomial{}(x^3 + \sqrt{2}x + 1);

```
Creates an algebraic number that is a root of a polynomial with algebraic number coefficients. 

*ElementWeylAlgebraDO* [ElementWeylAlgebraDO] {Calculator::elementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%20%3d%20Polynomial%7b%7dx_i%3b%5cn%5b%5c%5cpartial_1%2c%20x_1%5d%3b%5cn%5c%5cpartial_1%20x_1%3b%5cnx_1%5c%5cpartial_1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}} = ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}} = Polynomial{}x_i;
[\partial_1, x_1];
\partial_1 x_1;
x_1\partial_1
```
Creates element of a Weyl algebra = polynomial coefficient differential operator. First argument denotes differential operator letter, second argument - the dual polynomial expression. 

*ElementWeylAlgebraPoly* [ElementWeylAlgebraPoly] {Calculator::polynomialWithEWA}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x_%7b%7bi%7d%7d%20%3d%20ElementWeylAlgebraPoly%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cn%5c%5cpartial_%7b%7bi%7d%7d%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cn%5c%5cpartial_1%20x_1%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x_{{i}} = ElementWeylAlgebraPoly{}(\partial_i, x_i);
\partial_{{i}} = ElementWeylAlgebraDO{}(\partial_i, x_i);
\partial_1 x_1
```
Creates a monomial from the second argument whose differential operator letter is the first argument. 

*FourierTransformDO* [FourierTransformDO] {CalculatorFunctions::fourierTransformElementWeylAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20ElementWeylAlgebraPoly%7b%7d%28%5c%5cpartial%2c%20x%29%3b%5cn%5c%5cpartial%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial%2c%20x%29%3b%5cna%20%3dx%5e3%20%2b%20x%5c%5cpartial%3b%5cnb%20%3d%20%5c%5cpartial%20x%20%2b%20%5c%5cpartial%5e3%20%2b%20%5c%5cpartial%3b%5cn%5bFourierTransformDO%7b%7da%2c%20FourierTransformDO%7b%7db%5d-%20FourierTransformDO%7b%7d%5ba%2c%20b%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = ElementWeylAlgebraPoly{}(\partial, x);
\partial = ElementWeylAlgebraDO{}(\partial, x);
a =x^3 + x\partial;
b = \partial x + \partial^3 + \partial;
[FourierTransformDO{}a, FourierTransformDO{}b]- FourierTransformDO{}[a, b]
```
Fourier-transforms an element of a Weyl algebra. Multiplies each monomial term of odd total degree by - 1 (total degree = sum of degrees in the polynomial variables plus the degrees of the differential variables. 

*MinPolyMatrix* [MinPolyMatrix] {CalculatorFunctions::minimalPolynomialMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20MakeMatrix%7b%7d%28%280%2c%201%29%2c%20%28-%201%2c%200%29%29%3b%5cnp%20%3d%20MinPolyMatrix%7b%7dA%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = MakeMatrix{}((0, 1), (- 1, 0));
p = MinPolyMatrix{}A
```
Computes the minimal polynomial of a matrix, provided that the matrix is not too large.

*IsPrimeMillerRabin* [IsPrimeMillerRabin] {CalculatorFunctions::isPrimeMillerRabin}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d100%21%2b%201%3b%20IsPrimeMillerRabin%28A%29%3b%5cnIsPrimeMillerRabin%284256233%29%3b%5cnIsPrimeMillerRabin%2849979687%29%3b%5cnIsPrimeMillerRabin%284256233%20%2a%2049979687%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A =100!+ 1; IsPrimeMillerRabin(A);
IsPrimeMillerRabin(4256233);
IsPrimeMillerRabin(49979687);
IsPrimeMillerRabin(4256233 * 49979687)
```
Checks whether the number is prime by the Miller-Rabin test. 

*IsPossiblyPrime* [IsPossiblyPrime] {CalculatorFunctions::isPossiblyPrime}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d100%21%2b%201%3b%5cnIsPrimeMillerRabin%28A%29%3b%5cnIsPossiblyPrime%284256233%29%3b%5cnIsPossiblyPrime%2849979687%29%3b%5cnIsPossiblyPrime%284256233%20%2a%2049979687%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A =100!+ 1;
IsPrimeMillerRabin(A);
IsPossiblyPrime(4256233);
IsPossiblyPrime(49979687);
IsPossiblyPrime(4256233 * 49979687)
```
Checks whether the number is prime by trial division first and by the Miller-Rabin test next.

*IsSquareFreePolynomial* [IsSquareFreePolynomial] {CalculatorFunctions::isSquareFreePolynomial}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22IsSquareFreePolynomial%28%28x%5e2-3y%5e2%20x%20%29%28x%2by%29%29%3b%5cnIsSquareFreePolynomial%28%20%28x-3x%20y%20%2b5%20x%20y%5e2%29%5e2%20%283%2bx%20%2by%5e2%29%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
IsSquareFreePolynomial((x^2-3y^2 x )(x+y));
IsSquareFreePolynomial( (x-3x y +5 x y^2)^2 (3+x +y^2) )
```
Computes whether a polynomial is square-free by computing the greatest common divisors of the partial derivatives with the original polynomial.If the greatest common divisor is constant, then the polynomial is square-free. 

*AllPartitions* [AllPartitions] {CalculatorFunctions::allPartitions}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AllPartitions%2810%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AllPartitions(10) 
```
Prints all partitions of a positive number into a sum of positive integers. 

*AllVectorPartitions* [AllVectorPartitions] {CalculatorFunctions::allVectorPartitions}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AllVectorPartitions%28%2810%2c%2011%29%2c%28%281%2c2%29%2c%282%2c3%29%2c%284%2c5%29%2c%282%2c1%29%2c%283%2c2%29%2c%285%2c4%29%29%29%3b%5cnAllVectorPartitions%28%284%2c5%29%2c%28%281%2c0%29%2c%280%2c1%29%2c%281%2c1%29%2c%281%2c2%29%2c%281%2c3%29%2c%282%2c3%29%29%29%3b%5cnAllVectorPartitions%2820%2c%28Sequence%7b%7d1%2cSequence%7b%7d2%2cSequence%7b%7d3%2cSequence%7b%7d4%2cSequence%7b%7d5%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AllVectorPartitions((10, 11),((1,2),(2,3),(4,5),(2,1),(3,2),(5,4)));
AllVectorPartitions((4,5),((1,0),(0,1),(1,1),(1,2),(1,3),(2,3)));
AllVectorPartitions(20,(Sequence{}1,Sequence{}2,Sequence{}3,Sequence{}4,Sequence{}5));
```
Prints all partitions of the vector (first argument) using a given list of vectors (second argument). All partitioning vectors should have positive coordinates. 

*AllSelectionsFixedRank* [AllSelectionsFixedRank] {CalculatorFunctions::allSelectionsFixedRank}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AllSelectionsFixedRank%284%2c%20%281%2c2%2c3%2c4%29%29%3b%5cnAllSelectionsFixedRank%285%2c%20%283%2c1%2c1%29%29%3b%5cnAllSelectionsFixedRank%285%2c%20%283%2c0%2c1%29%29%3b%5cnAllSelectionsFixedRank%280%2c%20%283%2c5%2c7%29%29%3b%5cnAllSelectionsFixedRank%280%2c%20%280%2c0%2c0%29%29%3b%5cnAllSelectionsFixedRank%282%2c%205%29%3b%5cnAllSelectionsFixedRank%282%2c%201%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AllSelectionsFixedRank(4, (1,2,3,4));
AllSelectionsFixedRank(5, (3,1,1));
AllSelectionsFixedRank(5, (3,0,1));
AllSelectionsFixedRank(0, (3,5,7));
AllSelectionsFixedRank(0, (0,0,0));
AllSelectionsFixedRank(2, 5);
AllSelectionsFixedRank(2, 1);
```
Prints all selections of fixed size (given by first argument) from a collection of objects with multiplicities (given as a non-negative integer vector by the second argument). 

*KostkaNumber* [KostkaNumber] {CalculatorFunctionsWeylGroup::kostkaNumber}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22KostkaNumber%28%283%2c2%2c1%29%2c%20%284%2c2%29%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
KostkaNumber((3,2,1), (4,2)) 
```
Computes a Kostka number. First argument = partition given as a tuple a_1, ..., a_n with a_1>a_2> ...> a_n. Second argument = tableaux content = arbitrary tuple of positive integers. 

*PrintNonNegativeVectorsLevel* [PrintNonNegativeVectorsLevel] {Calculator::printZnEnumeration}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintNonNegativeVectorsLevel%7b%7d%284%2c%205%29%3bPrintNonNegativeVectorsLevel%7b%7d%284%2c%200%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintNonNegativeVectorsLevel{}(4, 5);PrintNonNegativeVectorsLevel{}(4, 0); 
```
Prints all vectors of grade level d with n coordinates lying in Z_{>= 0}. Function meant for debugging purposes. First argument = dimension, second argument =grading level. 

Operator or function PolyDivSlidesGrLex is overloaded with 2 total handlers.

*PolyDivSlidesGrLex* [PolyDivSlidesGrLex] {CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolyDivSlidesGrLex%7b%7d%281%2c%20x%5e3%20%2b%20x%20%2b%2010%2c%20x%20%2b2%29%3b%5cnPolyDivSlidesGrLex%7b%7d%281%2cx%20%2b%20y%20%2b%2010%2c%20x%20%2b%201%2c%20y%20-%201%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolyDivSlidesGrLex{}(1, x^3 + x + 10, x +2);
PolyDivSlidesGrLex{}(1,x + y + 10, x + 1, y - 1) 
```
Creates a slide with the polynomial disivion algorithm. First element = starting slide number.

*PolyDivSlidesGrLex* [PolyDivSlidesGrLexModular] {CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%3dPolynomialModP%28%20x%5e%7b4%7d%2bx%5e%7b3%7d%2b6x%5e%7b2%7d%2bx%2b5%2c3%29%3b%5cnb%3dPolynomialModP%28%202x%5e3%2b2x%2b2%2c3%29%3b%5cnPolyDivSlidesGrLex%285%2ca%2cb%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a=PolynomialModP( x^{4}+x^{3}+6x^{2}+x+5,3);
b=PolynomialModP( 2x^3+2x+2,3);
PolyDivSlidesGrLex(5,a,b)
```
Creates a slide with the polynomial disivion algorithm. First element = starting slide number. Uses modular arithmetic

*IsNilpotent* [IsNilpotent] {CalculatorFunctions::isNilpotent}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22isNilpotent%7b%7d%28%280%2c%201%29%2c%20%280%2c%200%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
isNilpotent{}((0, 1), (0, 0))
```
Computes whether a matrix is nilpotent. Recall an object X is nilpotent if there's a positive N with X^N=0. May/will be extended to work for an arbitrary object for which the term "nilpotent" makes sense. 

*SolvePolynomialSystem* [SolvePolynomialSystem] {CalculatorFunctions::SolvePolynomialSystem}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SolvePolynomialSystem%28xy%2bz%3d40%2cxz%2by%3d51%2cx%2by%2bz%3d19%29%20%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SolvePolynomialSystem(xy+z=40,xz+y=51,x+y+z=19) ;
```
Attempts to heuristically find a polynomial system solution. 

*Solve* [Solve] {CalculatorFunctions::solve}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Solve%28xy%2bz%3d40%2cxz%2by%3d51%2cx%2by%2bz%3d19%29%3b%5cnSolve%28xy%2bz%3d40%3bxz%2by%3d51%3bx%2by%2bz%3d19%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Solve(xy+z=40,xz+y=51,x+y+z=19);
Solve(xy+z=40;xz+y=51;x+y+z=19);
```
Attempts to heuristically find a polynomial system solution. Same as SolvePolynomialSystem but may eventually be extended to more types of equations/expressions.

*FindOneSolutionSerreLikePolynomialSystem* [FindOneSolutionSerreLikePolynomialSystem] {CalculatorFunctions::solveSerreLikeSystemNoUpperLimit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FindOneSolutionSerreLikePolynomialSystem%7b%7d%28%5cnx_%7b12%7dx_%7b24%7d-x_%7b10%7dx_%7b22%7d-2x_%7b8%7dx_%7b20%7d-x_%7b7%7dx_%7b19%7d%2b%201%2c%20x_%7b11%7dx_%7b24%7d-x_%7b10%7dx_%7b23%7d-x_%7b8%7dx_%7b21%7d%2c%20x_%7b9%7dx_%7b24%7d-x_%7b8%7dx_%7b23%7d%2bx_%7b7%7dx_%7b21%7d%2c%20x_%7b6%7dx_%7b24%7d%2b2x_%7b5%7dx_%7b23%7d-x_%7b4%7dx_%7b22%7d%2b2x_%7b3%7dx_%7b21%7d-2x_%7b2%7dx_%7b20%7d-x_%7b1%7dx_%7b19%7d%2c%20x_%7b12%7dx_%7b23%7d-x_%7b11%7dx_%7b22%7d-x_%7b9%7dx_%7b20%7d%2c%20x_%7b11%7dx_%7b23%7d%2bx_%7b10%7dx_%7b22%7d%2bx_%7b8%7dx_%7b20%7d-%201%2c%20x_%7b9%7dx_%7b23%7d%2bx_%7b8%7dx_%7b22%7d-x_%7b7%7dx_%7b20%7d%2c%20x_%7b12%7dx_%7b21%7d-x_%7b11%7dx_%7b20%7d%2bx_%7b9%7dx_%7b19%7d%2c%20x_%7b11%7dx_%7b21%7d%2bx_%7b10%7dx_%7b20%7d-x_%7b8%7dx_%7b19%7d%2c%20x_%7b9%7dx_%7b21%7d%2bx_%7b8%7dx_%7b20%7d%2bx_%7b7%7dx_%7b19%7d-%201%2c%20x_%7b12%7dx_%7b18%7d%2b2x_%7b11%7dx_%7b17%7d-x_%7b10%7dx_%7b16%7d%2b2x_%7b9%7dx_%7b15%7d-2x_%7b8%7dx_%7b14%7d-x_%7b7%7dx_%7b13%7d%2c%20x_%7b6%7dx_%7b18%7d-x_%7b4%7dx_%7b16%7d-2x_%7b2%7dx_%7b14%7d%20-%20x_%7b1%7dx_%7b13%7d%20%2b%201%2c%20x_%7b5%7dx_%7b18%7d%2bx_%7b4%7dx_%7b17%7d%2bx_%7b2%7dx_%7b15%7d%2c%20x_%7b3%7dx_%7b18%7d%2bx_%7b2%7dx_%7b17%7d-x_%7b1%7dx_%7b15%7d%2c%20x_%7b6%7dx_%7b17%7d%2bx_%7b5%7dx_%7b16%7d%2bx_%7b3%7dx_%7b14%7d%2c%20x_%7b5%7dx_%7b17%7d%2bx_%7b4%7dx_%7b16%7d%2bx_%7b2%7dx_%7b14%7d%20-%201%2c%20x_%7b3%7dx_%7b17%7d%2bx_%7b2%7dx_%7b16%7d-x_%7b1%7dx_%7b14%7d%2c%20x_%7b6%7dx_%7b15%7d%2bx_%7b5%7dx_%7b14%7d-x_%7b3%7dx_%7b13%7d%2c%20x_%7b5%7dx_%7b15%7d%2bx_%7b4%7dx_%7b14%7d-x_%7b2%7dx_%7b13%7d%2c%20x_%7b3%7dx_%7b15%7d%2bx_%7b2%7dx_%7b14%7d%2bx_%7b1%7dx_%7b13%7d%20-%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FindOneSolutionSerreLikePolynomialSystem{}(
x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14} - x_{1}x_{13} + 1, x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)
```
Attempts to heuristically solve a system of polynomial of equations over the rationals. The input system of equations can be arbitrary, however it is assumed that the system is Serre-like, i.e., comes from the Serre relations governing the embedding of a semisimple Lie algebra in a semisimple Lie algebra. This suggests a certain heuristic solution strategy (will be documented as the code matures). To emphasize, the algorithm is heuristic and not guaranteed to work. The result of the function is a printout with one of the possible outcomes.<br>Outcome 1. While processing the polynomial system, the computation limit was hit. The computation was aborted. No information on the system can be given (except that it is large). <br>Outcome 2. The reduced Groebner basis of the system is {1}. Therefore the system is contradictory - no solution exists. <br>Outcome 3. The reduced Groebner basis was found and is not equal to {1}. Therefore a solution over the complex numbers exists. However, no such solution was found. <br>Outcome 4. A solution was found and is presented to the user.

*FindOneSolutionSerreLikePolynomialSystemUpperLimit* [FindOneSolutionSerreLikePolynomialSystemUpperLimit] {CalculatorFunctions::solveSerreLikeSystemUpperLimit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FindOneSolutionSerreLikePolynomialSystemUpperLimit%7b%7d%28%5cn100%2c10000%2c%5cnx_%7b12%7dx_%7b24%7d-x_%7b10%7dx_%7b22%7d-2x_%7b8%7dx_%7b20%7d-x_%7b7%7dx_%7b19%7d%2b%201%2c%20x_%7b11%7dx_%7b24%7d-x_%7b10%7dx_%7b23%7d-x_%7b8%7dx_%7b21%7d%2c%20x_%7b9%7dx_%7b24%7d-x_%7b8%7dx_%7b23%7d%2bx_%7b7%7dx_%7b21%7d%2c%20x_%7b6%7dx_%7b24%7d%2b2x_%7b5%7dx_%7b23%7d-x_%7b4%7dx_%7b22%7d%2b2x_%7b3%7dx_%7b21%7d-2x_%7b2%7dx_%7b20%7d-x_%7b1%7dx_%7b19%7d%2c%20x_%7b12%7dx_%7b23%7d-x_%7b11%7dx_%7b22%7d-x_%7b9%7dx_%7b20%7d%2c%20x_%7b11%7dx_%7b23%7d%2bx_%7b10%7dx_%7b22%7d%2bx_%7b8%7dx_%7b20%7d-%201%2c%20x_%7b9%7dx_%7b23%7d%2bx_%7b8%7dx_%7b22%7d-x_%7b7%7dx_%7b20%7d%2c%20x_%7b12%7dx_%7b21%7d-x_%7b11%7dx_%7b20%7d%2bx_%7b9%7dx_%7b19%7d%2c%20x_%7b11%7dx_%7b21%7d%2bx_%7b10%7dx_%7b20%7d-x_%7b8%7dx_%7b19%7d%2c%20x_%7b9%7dx_%7b21%7d%2bx_%7b8%7dx_%7b20%7d%2bx_%7b7%7dx_%7b19%7d-%201%2c%20x_%7b12%7dx_%7b18%7d%2b2x_%7b11%7dx_%7b17%7d-x_%7b10%7dx_%7b16%7d%2b2x_%7b9%7dx_%7b15%7d-2x_%7b8%7dx_%7b14%7d-x_%7b7%7dx_%7b13%7d%2c%20x_%7b6%7dx_%7b18%7d-x_%7b4%7dx_%7b16%7d-2x_%7b2%7dx_%7b14%7d-x_%7b1%7dx_%7b13%7d%2b%201%2c%20x_%7b5%7dx_%7b18%7d%2bx_%7b4%7dx_%7b17%7d%2bx_%7b2%7dx_%7b15%7d%2c%20x_%7b3%7dx_%7b18%7d%2bx_%7b2%7dx_%7b17%7d-x_%7b1%7dx_%7b15%7d%2c%20x_%7b6%7dx_%7b17%7d%2bx_%7b5%7dx_%7b16%7d%2bx_%7b3%7dx_%7b14%7d%2c%20x_%7b5%7dx_%7b17%7d%2bx_%7b4%7dx_%7b16%7d%2bx_%7b2%7dx_%7b14%7d%20-%201%2c%20x_%7b3%7dx_%7b17%7d%2bx_%7b2%7dx_%7b16%7d-x_%7b1%7dx_%7b14%7d%2c%20x_%7b6%7dx_%7b15%7d%2bx_%7b5%7dx_%7b14%7d-x_%7b3%7dx_%7b13%7d%2c%20x_%7b5%7dx_%7b15%7d%2bx_%7b4%7dx_%7b14%7d-x_%7b2%7dx_%7b13%7d%2c%20x_%7b3%7dx_%7b15%7d%2bx_%7b2%7dx_%7b14%7d%2bx_%7b1%7dx_%7b13%7d%20-%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FindOneSolutionSerreLikePolynomialSystemUpperLimit{}(
100,10000,
x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14}-x_{1}x_{13}+ 1, x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)
```
Same as FindOneSolutionSerreLikePolynomialSystem but the first argument gives upper limits to the number of polynomial computations that can be carried out, and the second number the number of monomial computations. 

*FindOneSolutionModPUpperLimit* [FindOneSolutionModPUpperLimit] {CalculatorFunctions::findOneSolutionModPUpperLimit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FindOneSolutionModPUpperLimit%7b%7d%28%5cn100%2c%5cn10000%2c%5cn3%2c%5cnx_%7b12%7dx_%7b24%7d-x_%7b10%7dx_%7b22%7d-2x_%7b8%7dx_%7b20%7d-x_%7b7%7dx_%7b19%7d%2b%201%2c%20x_%7b11%7dx_%7b24%7d-x_%7b10%7dx_%7b23%7d-x_%7b8%7dx_%7b21%7d%2c%20x_%7b9%7dx_%7b24%7d-x_%7b8%7dx_%7b23%7d%2bx_%7b7%7dx_%7b21%7d%2c%20x_%7b6%7dx_%7b24%7d%2b2x_%7b5%7dx_%7b23%7d-x_%7b4%7dx_%7b22%7d%2b2x_%7b3%7dx_%7b21%7d-2x_%7b2%7dx_%7b20%7d-x_%7b1%7dx_%7b19%7d%2c%20x_%7b12%7dx_%7b23%7d-x_%7b11%7dx_%7b22%7d-x_%7b9%7dx_%7b20%7d%2c%20x_%7b11%7dx_%7b23%7d%2bx_%7b10%7dx_%7b22%7d%2bx_%7b8%7dx_%7b20%7d-%201%2c%20x_%7b9%7dx_%7b23%7d%2bx_%7b8%7dx_%7b22%7d-x_%7b7%7dx_%7b20%7d%2c%20x_%7b12%7dx_%7b21%7d-x_%7b11%7dx_%7b20%7d%2bx_%7b9%7dx_%7b19%7d%2c%20x_%7b11%7dx_%7b21%7d%2bx_%7b10%7dx_%7b20%7d-x_%7b8%7dx_%7b19%7d%2c%20x_%7b9%7dx_%7b21%7d%2bx_%7b8%7dx_%7b20%7d%2bx_%7b7%7dx_%7b19%7d-%201%2c%20x_%7b12%7dx_%7b18%7d%2b2x_%7b11%7dx_%7b17%7d-x_%7b10%7dx_%7b16%7d%2b2x_%7b9%7dx_%7b15%7d-2x_%7b8%7dx_%7b14%7d-x_%7b7%7dx_%7b13%7d%2c%20x_%7b6%7dx_%7b18%7d-x_%7b4%7dx_%7b16%7d-2x_%7b2%7dx_%7b14%7d-x_%7b1%7dx_%7b13%7d%2b%201%2c%20x_%7b5%7dx_%7b18%7d%2bx_%7b4%7dx_%7b17%7d%2bx_%7b2%7dx_%7b15%7d%2c%20x_%7b3%7dx_%7b18%7d%2bx_%7b2%7dx_%7b17%7d-x_%7b1%7dx_%7b15%7d%2c%20x_%7b6%7dx_%7b17%7d%2bx_%7b5%7dx_%7b16%7d%2bx_%7b3%7dx_%7b14%7d%2c%20x_%7b5%7dx_%7b17%7d%2bx_%7b4%7dx_%7b16%7d%2bx_%7b2%7dx_%7b14%7d%20-%201%2c%20x_%7b3%7dx_%7b17%7d%2bx_%7b2%7dx_%7b16%7d-x_%7b1%7dx_%7b14%7d%2c%20x_%7b6%7dx_%7b15%7d%2bx_%7b5%7dx_%7b14%7d-x_%7b3%7dx_%7b13%7d%2c%20x_%7b5%7dx_%7b15%7d%2bx_%7b4%7dx_%7b14%7d-x_%7b2%7dx_%7b13%7d%2c%20x_%7b3%7dx_%7b15%7d%2bx_%7b2%7dx_%7b14%7d%2bx_%7b1%7dx_%7b13%7d%20-%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FindOneSolutionModPUpperLimit{}(
100,
10000,
3,
x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14}-x_{1}x_{13}+ 1, x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)
```
Attempts to find one solution mod p with a computation limit. The algorithm is heuristic. The first number is the limit of polynomial divisions.The second number is the limit of monomial operations.The third number is the prime number p over which we are solving.

*FindOneSolutionSerreLikePolynomialSystemAlgebraic* [FindOneSolutionSerreLikePolynomialSystemAlgebraic] {CalculatorFunctions::solveSerreLikeSystemAlgebraic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FindOneSolutionSerreLikePolynomialSystemAlgebraic%28x%5e2%20%2b%201%2c%20y%20x%20z%20-%201%2c%20z%5e2%20x%20%2b%20y%20-%201%2c%20w%20u%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FindOneSolutionSerreLikePolynomialSystemAlgebraic(x^2 + 1, y x z - 1, z^2 x + y - 1, w u)
```
Same as FindOneSolutionSerreLikePolynomialSystem but starts directly over the algebraic closure. 

*FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit* [FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit] {CalculatorFunctions::solveSerreLikeSystemAlgebraicUpperLimit}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit%28%5cn10000%2c10000%2c%5cnx%5e2%2b%201%2c%20y%20x%20z%20-%201%2c%20z%5e2%20x%20%2by%20-%201%2c%20w%20u%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit(
10000,10000,
x^2+ 1, y x z - 1, z^2 x +y - 1, w u)
```
Same as FindOneSolutionSerreLikePolynomialSystemAlgebraic but the first argument gives upper limits to the number of polynomial computations that can be carried out, and the second argument gives upper limit to the number of monomial computations. 

*GroebnerLex* [GroebnerLex] {Calculator::groebnerLexicographic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GroebnerLex%7b%7d%28s%5e2%2bc%5e2%2b%201%2c%20a-s%5e4%2c%20b-c%5e4%2c%20upperLimit%3d10000%20%29%3b%5cnGroebnerLex%7b%7d%28s%5e2%2bc%5e2%2b%201%2c%20a-s%5e4%2c%20b-c%5e4%2c%20upperLimit%3d5%20%20%29%3b%5cnGroebnerLex%7b%7d%28s%5e2%2bc%5e2%2b%201%2c%20a-s%5e4%2c%20b-c%5e4%2c%20order%20%3d%20%28s%2cc%2cb%2ca%29%29%3b%5cnGroebnerLex%7b%7d%28s%5e2%2bc%5e2%2b%201%2c%20a-s%5e4%2c%20b-c%5e4%2c%20order%20%3d%20%28a%2cb%2cc%2cs%29%29%3b%5cnThaw%20GroebnerLex%7b%7d%28s%5e2%2bc%5e2%2b%201%2c%20a-s%5e4%2c%20b-c%5e4%2c%20order%20%3d%20%28a%2cb%2cc%2cs%29%29%3b%5cnGroebnerLex%28x%20y%2bz%3d40%2c%20x%20z%2by%3d51%2c%20x%2by%2bz%3d19%29%3b%5cnGroebnerLex%28x%20y%2bz%3d40%2c%20x%20z%2by%3d51%2c%20x%2by%2bz%3d19%2c%20modulus%20%3d%207%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, upperLimit=10000 );
GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, upperLimit=5  );
GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, order = (s,c,b,a));
GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, order = (a,b,c,s));
Thaw GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, order = (a,b,c,s));
GroebnerLex(x y+z=40, x z+y=51, x+y+z=19);
GroebnerLex(x y+z=40, x z+y=51, x+y+z=19, modulus = 7);

```
Transforms to a reduced Groebner basis using the lexicographic order. The lexicographic order is inherited from the comparison of the underlying expressions. The upper limit for polynomial computations is by default:10000. You can change that to, say, 100, by adding upperLimit = 100 to the list of arguments. If you set upperLimit = -1 or another negative, this will run the algorithm with no computation upper limits. Please do not do run with upperLimit=-1 on our public web server since multiple instances of a large computation might hog it up. The resulting printout will let your know whether the upper limit was hit or not. If you want to customize the order of variables, add an argument such as order=(y,x), to signify that the variable y is stronger than variable x. If you want the computation carried out mod p for some prime p, set modulus = p. The result comes out frozen to prevent the calculator from reordering the outputs. Unfreeze it with the Thaw command (see the examples)<br><b>Description of the algorithm.</b> Let f_1 and f_2 be two polynomials. Following Cox, Little, O'Shea, "Ideals, Varieties, Algorithms", page 81, denote by S(f_1, f_2) the symmetric difference of f_1 and f_2. More precisely, let x^{\gamma_1} be the leading monomial of f_1 and x^{\gamma_2} be the leading monomial of f_2, with leading coefficients c_1 and c_2. Then define S(f_1, f_2)= c_2* f_1* lcm (x^\gamma_1, \gamma_2) / x^\gamma_1 - c_1* f_2 *lcm (x^\gamma_1, x^\gamma_2)/x^\gamma_2. Here lcm stands for least common multiple of monomials, defined in the obvious way. <br>
1. Allocate two buckets of polynomials - one "main" bucket and additional bucket. At any given moment in the algorithm, the union of main bucket and the additional bucket should give a basis of the ideal. <br>
2. Move all input elements into the additional bucket. <br>
3. Call the <b>reduce additional bucket</b> subroutine. In the c++ implementation the function is called GroebnerBasisComputation::addPolyAndReduceBasis. <br>
4. Set changed to be true. <br>
5. While (changed)<br>
5.1 For all elements f_1, f_2 in the main bucket form the symmetric difference S(f_1, f_2) and add it to the additional bucket. <br>
5.2 Call the <b>reduce additional bucket</b> subroutine and set changed to be equal to the result of the subroutine. <br><b>Reduce additional bucket</b> subroutine. <br>
1. Set changedMainBucket to be false. <br>
2.While (additional bucket is not empty)<br>
2.1. While (additional bucket is not empty)<br>
2.1.1 Remove the top element from the additional bucket; call that element currentElement. <br>
2.1.2 Divide currentElement by the elements of the main bucket, and record the resulting remainder element in currentRemainder. Here we use the multivariate polynomial division algorithm, page 62 of Cox, Little, O'Shea, "Ideals, Varieties, Algorithms". <br>
2.1.3 If currentRemainder is non-zero, add it to the main bucket and set changedMainBucket to be true.<br>
2.2 For each element in the main bucket <br>
2.2.1 Call the considered element currentElement. <br>
2.2.2 Divide currentElement by the remaining elements (excluding currentElement itself) in the main bucket. Call the remainder of the division currentRemainder. <br>
2.2.3 If currentRemainder is not equal to currentElement, remove currentElement from the main bucket and add currentRemainder to the additional bucket. Note that this operation modifies the main bucket: each element of the main bucket must be traversed exactly once by the current cycle, but the division is carried with the modified state of the main bucket. <br>
3. Return changedMainBucket.  <br><b>End of algorithm description.</b>

*GroebnerGrLex* [GroebnerGrLex] {CalculatorFunctionsPolynomial::groebnerGradedLexicographic}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GroebnerGrLex%7b%7d%28%20a%5e2%2bb%5e2%2b%201%2c%20x-a%5e4%2c%20y-b%5e4%2cupperLimit%3d10000%20%29%3b%5cn%20GroebnerGrLex%7b%7d%28a%5e2%2bb%5e2%2b%201%2c%20x-a%5e4%2c%20y-b%5e4%2c%20upperLimit%3d5%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GroebnerGrLex{}( a^2+b^2+ 1, x-a^4, y-b^4,upperLimit=10000 );
 GroebnerGrLex{}(a^2+b^2+ 1, x-a^4, y-b^4, upperLimit=5 )
```
Transforms to a reduced Groebner basis relative to the graded lexicographic order. In the graded lexicographic order, monomials are first compared by total degree, then by lexicographic order. The lexicographic order is inherited from the comparison of the underlying expressions. All other function setup is as in GroebnberLex. 

*PolynomialRelationsUpperLimit* [PolynomialRelationsUpperLimit] {CalculatorFunctionsPolynomial::polynomialRelations}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PolynomialRelationsUpperLimit%7b%7d%2810000%2c%20s%5e2%2bc%5e2%2b%201%2c%20s%5e4%2c%20c%5e4%20%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PolynomialRelationsUpperLimit{}(10000, s^2+c^2+ 1, s^4, c^4 );
```
Finds the relations between the polynomials.

*ElementEllipticCurveNormalForm* [ElementEllipticCurveNormalForm] {CalculatorFunctions::elementEllipticCurveNormalForm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ElementEllipticCurveNormalForm%28y%5e2%20%3d%20x%5e3%20%2b%20x%20%2b%207%2c%20x%20%3d%203%20mod%20101%2c%20y%20%3d%2021%20mod%20101%29%3bElementEllipticCurveNormalForm%28y%5e2%20%3d%20x%5e3%20-%20x%20%2b%201%2c%20x%20%3d%203%2c%20y%20%3d%205%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ElementEllipticCurveNormalForm(y^2 = x^3 + x + 7, x = 3 mod 101, y = 21 mod 101);ElementEllipticCurveNormalForm(y^2 = x^3 - x + 1, x = 3, y = 5)
```
Makes an elliptic curve from a cubic in normal form, generator letter and base point.

*ConvertPolynomialModularToPolynomialRational* [ConvertPolynomialModularToPolynomialRational] {CalculatorFunctions::convertPolynomialModularToPolynomialRational}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20100x-55%3b%5cnB%20%3d%20PolynomialModP%28A%2c%20101%29%3b%5cnConvertPolynomialModularToPolynomialRational%20B%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = 100x-55;
B = PolynomialModP(A, 101);
ConvertPolynomialModularToPolynomialRational B
```
Converts a modular polynomial to a rational one. 

*PlotMandelbrotSet* [PlotMandelbrotSet] {CalculatorFunctionsComplexDynamics::plotMandelbrotSet}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotMandelbrotSet%28z%5e2%2bC%2c%20C%3d0%29%3b%5cnPlotMandelbrotSet%28z%5e3%2bC%2c%20C%3d0%29%3b%5cnPlotMandelbrotSet%28%20z%5e2%2bz%2bC%2c%20C%3d0%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotMandelbrotSet(z^2+C, C=0);
PlotMandelbrotSet(z^3+C, C=0);
PlotMandelbrotSet( z^2+z+C, C=0);

```
Plots the Mandelbrot set of a complex function. The first argument is the map we are iterating. The second argument is which of the letters of your function is the parameter that is governed by the (x,y) position of the picture, together with the initial value to set for it. For classical Mandelbrot set, the first argument should be z^2+C. The second argument should be C=0. The map works as follows. Start with a given value of the complex parameter C. The value of the parameter is given by the x,y-coordinate of the plotting surface. Now, iterate the map: f(...f(C)). If the point escapes to infinity, color the point with a color that matches the escape speed. Else, color the point black. You are not limited to the function z^2+C only. Feel free to try other functions as well.

*PlotJuliaSet* [PlotJuliaSet] {CalculatorFunctionsComplexDynamics::plotJuliaSet}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotJuliaSet%28%20z%5e2%2b0.7i%2bC%2c%20C%3d0%29%3b%5cnPlotJuliaSet%281%2f%28b%29%28z%5e2%2bz%2b1%2fz%2b1%2fz%5e2%29%2c%20b%3d3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotJuliaSet( z^2+0.7i+C, C=0);
PlotJuliaSet(1/(b)(z^2+z+1/z+1/z^2), b=3);
```
Plots the Julia set of a complex function. The first argument is the map we are iterating. The remaining parameters are complex parameters of the formula. The complex parameters can be adjusted dynamically using the mouse. To compute the plot: we start with the x,y-coordinate of the plotting surface and the number x+iy. Now, iterate the map: f(...f(x+iy)). If the point escapes to infinity, color the point with a color that matches the escape speed. Else, color the point black. You are not limited to the function z^2+C only. Feel free to try other functions as well.

*VectorPartitionFunction* [VectorPartitionFunction] {CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22VectorPartitionFunction%28%5cn%281%2c0%2c0%29%2c%5cn%280%2c1%2c0%29%2c%5cn%280%2c0%2c1%29%2c%5cn%281%2c1%2c0%29%2c%5cn%280%2c1%2c1%29%2c%5cn%281%2c1%2c1%29%5cn%29%3b%5cnVectorPartitionFunction%28%5cn%281%2c0%29%2c%280%2c1%29%2c%281%2c1%29%2c%281%2c2%29%2c%281%2c3%29%2c%282%2c3%29%20%29%3b%5cnVectorPartitionFunction%28%5cnSequence%7b%7d1%2cSequence%7b%7d2%2cSequence%7b%7d3%2cSequence%7b%7d4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
VectorPartitionFunction(
(1,0,0),
(0,1,0),
(0,0,1),
(1,1,0),
(0,1,1),
(1,1,1)
);
VectorPartitionFunction(
(1,0),(0,1),(1,1),(1,2),(1,3),(2,3) );
VectorPartitionFunction(
Sequence{}1,Sequence{}2,Sequence{}3,Sequence{}4)
```
Work in progress. Vector partition function formula. 

*VectorPartitionFunctionWithDetails* [VectorPartitionFunctionWithDetails] {CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22VectorPartitionFunctionWithDetails%28%5cn%281%2c0%2c0%29%2c%5cn%280%2c1%2c0%29%2c%5cn%280%2c0%2c1%29%2c%5cn%281%2c1%2c0%29%2c%5cn%280%2c1%2c1%29%2c%5cn%281%2c1%2c1%29%5cn%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
VectorPartitionFunctionWithDetails(
(1,0,0),
(0,1,0),
(0,0,1),
(1,1,0),
(0,1,1),
(1,1,1)
);

```
Computes the vector partition function. Shows all the internals of the computation, if those are not too long. 

*VectorPartitionFunctionElementary* [VectorPartitionFunctionElementary] {CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormulaElementary}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22VectorPartitionFunctionElementary%28%5cn%281%2c1%29%2c%5cn%281%2c0%29%2c%5cn%280%2c1%29%5cn%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
VectorPartitionFunctionElementary(
(1,1),
(1,0),
(0,1)
);
```
Work in progress. Vector partition function formula, elementary method. 

*ConeDecompositionWithHistory* [ConeDecompositionWithHistory] {CalculatorFunctionsVectorPartitionFunction::coneDecompositionSpannedSlicesWithHistory}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConeDecompositionWithHistory%28%5cn%281%2c0%2c0%29%2c%5cn%280%2c1%2c0%29%2c%5cn%280%2c0%2c1%29%2c%5cn%281%2c1%2c0%29%2c%5cn%280%2c1%2c1%29%2c%5cn%281%2c1%2c1%29%5cn%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConeDecompositionWithHistory(
(1,0,0),
(0,1,0),
(0,0,1),
(1,1,0),
(0,1,1),
(1,1,1)
);

```
Same as ConeDecomposition but shows the history of which slices were made to get the decomposition.

*ConeDecomposition* [ConeDecomposition] {CalculatorFunctionsVectorPartitionFunction::coneDecompositionSpannedSlicesNoHistory}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConeDecomposition%28%5cn%281%2c0%2c0%29%2c%5cn%280%2c1%2c0%29%2c%5cn%280%2c0%2c1%29%2c%5cn%281%2c1%2c0%29%2c%5cn%280%2c1%2c1%29%2c%5cn%281%2c1%2c1%29%5cn%29%3b%5cnConeDecomposition%28G_2%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConeDecomposition(
(1,0,0),
(0,1,0),
(0,0,1),
(1,1,0),
(0,1,1),
(1,1,1)
);
ConeDecomposition(G_2);

```
Cone decomposition related to a vector partition function. Uses only slicing plances spanned by n-1 vectors. If you use a single argument that is a Dynkin type, this function automatically computes the cone decomposition with respect to the positve roots of the type. Works up to type B_4. Larger types will be rejected as their computation run time is measured in hours or worse.

*ConeDecompositionWithoutAmalgamation* [ConeDecompositionWithoutAmalgamation] {CalculatorFunctionsVectorPartitionFunction::coneDecompositionSpannedSlicesNoHistoryNoAmalgamation}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConeDecomposition%28D_4%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConeDecomposition(D_4);
```
A faster version of ConeDecomposition that produces possibly finer (more) chambers than are actually needed. Runs considerably faster.

*ConeDecompositionArbitrarySlices* [ConeDecompositionArbitrarySlices] {CalculatorFunctionsVectorPartitionFunction::coneDecompositionArbitrarySlices}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConeDecompositionArbitrarySlices%28%5cn%281%2c0%2c0%29%2c%5cn%280%2c1%2c0%29%2c%5cn%280%2c0%2c1%29%2c%5cn%281%2c1%2c0%29%2c%5cn%280%2c1%2c1%29%2c%5cn%281%2c1%2c1%29%5cn%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConeDecompositionArbitrarySlices(
(1,0,0),
(0,1,0),
(0,0,1),
(1,1,0),
(0,1,1),
(1,1,1)
);

```
Cone decomposition related to a vector partition function. Unlike ConeDecomposition, this decomposition uses arbitrary combinatorial chamber walls.

*ConeDecompositionArbitrarySlicesWithHistory* [ConeDecompositionArbitrarySlicesWithHistory] {CalculatorFunctionsVectorPartitionFunction::coneDecompositionArbitrarySlicesWithHistory}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ConeDecompositionArbitrarySlicesWithHistory%28%5cn%281%2c0%2c0%29%2c%5cn%280%2c1%2c0%29%2c%5cn%280%2c0%2c1%29%2c%5cn%281%2c1%2c0%29%2c%5cn%280%2c1%2c1%29%2c%5cn%281%2c1%2c1%29%5cn%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ConeDecompositionArbitrarySlicesWithHistory(
(1,0,0),
(0,1,0),
(0,0,1),
(1,1,0),
(0,1,1),
(1,1,1)
);

```
Same as ConeDecompositionArbitrarySlices but prints out the intermediate steps. 

*BernoulliSum* [BernoulliSum] {CalculatorFunctionsVectorPartitionFunction::bernouliSum}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22p%3d3%3b%5cnN%3d100%3b%5cnB%3dBernoulliSum%28p%2cn%29%3b%5cnf%7b%7d0%3d%200%3b%5cnf%7b%7d%7b%7bn%7d%7d%20%3d%20n%5ep%2bf%7b%7d%28n-1%29%3b%5cnf%7b%7dN%3b%5cn%28n%3dN%3bB%29_2%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
p=3;
N=100;
B=BernoulliSum(p,n);
f{}0= 0;
f{}{{n}} = n^p+f{}(n-1);
f{}N;
(n=N;B)_2;

```
Returns the Bernoulli sum formula / Faulhaber's formula, for the sum of the powers of 1^k+2^k+...+n^k as a polynomial of n of k+1'st degree. 

*Lattice* [Lattice] {CalculatorFunctionsVectorPartitionFunction::lattice}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22l%20%3d%20Lattice%28%281%2c2%2c3%29%2c%20%282%2c2%2c2%29%2c%20%283%2c3%2c9%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
l = Lattice((1,2,3), (2,2,2), (3,3,9));

```
Generates a built-in lattice from the given input vectors. 

*SubLatticeScalarProductTimesDirectionInLattice* [SubLatticeScalarProductTimesDirectionInLattice] {CalculatorFunctionsVectorPartitionFunction::subLatticeScalarProductTimesDirectionInLattice}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22L%20%3d%20Lattice%28%282%2c0%29%2c%20%280%2c3%29%29%3b%5cnT%3dLattice%28%287%2c0%29%2c%280%2c9%29%29%3b%5cnSubLatticeScalarProductTimesDirectionInLattice%28L%2c%20T%2c%20%281%2c2%29%2c%20%282%2c3%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
L = Lattice((2,0), (0,3));
T=Lattice((7,0),(0,9));
SubLatticeScalarProductTimesDirectionInLattice(L, T, (1,2), (2,3));
```
We are given a lattices L and T, a vector v and d. We are looking for the lattice X given by: X= {a\in L: &lt; a, v&gt; d \in T }. This function is a useful tool when computing with vector partition functions. First argument: L. Second argument: T. Both must be lattices. Third argument: v. Fourth argument: d. of vectors w in L for which the scalar product of w and v is integer. 

*SubLatticeWithIntegralScalarProducts* [SubLatticeWithIntegralScalarProducts] {CalculatorFunctionsVectorPartitionFunction::subLatticeWithIntegralScalarProducts}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22l%20%3d%20Lattice%28%281%2c2%2c3%29%2c%20%282%2c2%2c2%29%2c%20%283%2c3%2c9%29%29%3b%5cnSubLatticeWithIntegralScalarProducts%28%281%2f2%2c1%2f3%2c1%2f7%29%2c%20l%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
l = Lattice((1,2,3), (2,2,2), (3,3,9));
SubLatticeWithIntegralScalarProducts((1/2,1/3,1/7), l);
```
Given a lattice L and a vector v, generates the sub-lattice W of vectors w in L for which the scalar product of w and v is integer. 

*DualLattice* [DualLattice] {CalculatorFunctionsVectorPartitionFunction::subLatticeWithIntegralScalarProducts}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22l%20%3d%20Lattice%28%281%2c2%2c3%29%2c%20%282%2c2%2c2%29%2c%20%283%2c3%2c9%29%29%3b%5cnDualLattice%28l%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
l = Lattice((1,2,3), (2,2,2), (3,3,9));
DualLattice(l)
```
Computes the dual lattice of a given lattice. The dual of the lattice is the lattice of vectors with integral scalar products with the given vectors. 

*ReduceModLattice* [ReduceModLattice] {CalculatorFunctionsVectorPartitionFunction::reduceModuloLattice}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22l%20%3d%20Lattice%28%281%2c2%2c3%29%2c%20%282%2c2%2c2%29%2c%20%283%2c3%2c9%29%29%3b%5cnReduceModLattice%28%281%2c1%2c1%29%2cl%29%3b%5cnReduceModLattice%28%281%2f2%2c1%2f3%2c1%2f4%29%2cl%29%3b%5cnReduceModLattice%28%281%2c2%29%2cl%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
l = Lattice((1,2,3), (2,2,2), (3,3,9));
ReduceModLattice((1,1,1),l);
ReduceModLattice((1/2,1/3,1/4),l);
ReduceModLattice((1,2),l);

```
Reduces a vector modulo a lattice. First argument: vector. Second argument: lattice. 

*QuotientLatticeRepresentatives* [QuotientLatticeRepresentatives] {CalculatorFunctionsVectorPartitionFunction::quotientLatticeRepresentatives}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22l%20%3d%20Lattice%28%281%2c0%2c0%29%2c%20%280%2c1%2c0%29%2c%20%280%2c0%2c1%29%29%3b%5cnm%20%3d%20Lattice%28%281%2c2%2c3%29%2c%20%282%2c2%2c2%29%2c%20%283%2c3%2c9%29%29%3b%5cnQuotientLatticeRepresentatives%28l%2cm%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
l = Lattice((1,0,0), (0,1,0), (0,0,1));
m = Lattice((1,2,3), (2,2,2), (3,3,9));
QuotientLatticeRepresentatives(l,m)
```
Computes all representatives of a quotient lattice L/M. Both arguments must be lattices. 

*PlotLatticeRestricted* [PlotLatticeRestricted] {CalculatorFunctionsVectorPartitionFunction::plotLatticeRestricted}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22l%20%3d%20Lattice%28%281%2c1%2c0%29%2c%20%280%2c2%2c0%29%2c%20%280%2c0%2c1%29%29%3b%5cnPlotLatticeRestricted%28l%29%3b%5cnm%20%3d%20Lattice%28%281%2c1%29%2c%20%280%2c2%29%29%3b%5cnPlotLatticeRestricted%28m%2c%28%281%2c0%29%2c%20%280%2c1%29%29%20%29%3b%5cnPlotLatticeRestricted%28m%2c%28%28-1%2c1%29%2c%20%281%2c0%29%29%20%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
l = Lattice((1,1,0), (0,2,0), (0,0,1));
PlotLatticeRestricted(l);
m = Lattice((1,1), (0,2));
PlotLatticeRestricted(m,((1,0), (0,1)) );
PlotLatticeRestricted(m,((-1,1), (1,0)) );

```
Plots a lattice. The first argument must be a lattice. The second optional argument is a list of the normals of a restricting cone. All points in the lattice whose scalar product with any of the restricting vectors is zero will be omitted. 

*KostantPartitionFunctionsLatex* [KostantPartitionFunctionsLatex] {CalculatorFunctionsVectorPartitionFunction::kostantPartitionFunctionLatex}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22KostantPartitionFunctionsLatex%28%5cna_2%2c%5cnb_2%2c%5cnc_2%2c%5cng_2%2c%5cna_3%2c%5cnb_3%2c%5cnc_3%2c%5cna_4%5cn%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
KostantPartitionFunctionsLatex(
a_2,
b_2,
c_2,
g_2,
a_3,
b_3,
c_3,
a_4
)
```
Requires a logged-in admin to use. Acts similarly to the VectorPartitionFunction but 1) displays latex only, 2) acts on Dynkin types only and 3) allows multiple dynkin type inputs producing a single chunk of latex output. This function is needed for the internal documentation of our algorithms, use VectorPartitionFunction directly instead. 

*SemisimpleLieAlgebra* [SemisimpleLieAlgebra] {CalculatorConversions::semisimpleLieAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28SemisimpleLieAlgebra%7b%7dG_2%2c%20i%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28SemisimpleLieAlgebra%7b%7dG_2%2c%20i%29%3b%5cn%5bg_1%2cg_%7b-%201%7d%5d%3b%20%5cn%5bg_2%2c%20g_%7b-2%7d%5d%3b%20%5cn%5bh_%7b1%7d%2c%20g_6%5d%3b%20%5cn%5bh_2%2c%20g_%7b-6%7d%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(SemisimpleLieAlgebra{}G_2, i);
h_{{i}}= CartanGenerator{}(SemisimpleLieAlgebra{}G_2, i);
[g_1,g_{- 1}]; 
[g_2, g_{-2}]; 
[h_{1}, g_6]; 
[h_2, g_{-6}]
```
Creates a semisimple Lie algebra. The semisimple Lie algebra is given via its Dynkin type. A simple Dynkin type is given by type letter with upper index equal to the inverse of the scale of the symmetric Cartan matrix and lower index equal to the rank of the subalgebra. For example A^2_3 stands for type A_3 (sl (4)) with symmetric Cartan matrix scale equal to 1/2. If the upper index is omitted, it is implied to be 1.<hr> We define the symmetric Cartan matrix as the matrix whose (i, j)^{th} entry is the scalar product of the i^{th} and j^{th} roots. We assume the roots to be ordered in the order implied by the (non-symmetric) Cartan matrices on page 59 in Humphreys, Introduction to Lie algebras and representation theory. If the upper index of the Dynkin type is 1, the corresponding symmetric Cartan matrix is assigned the default value. The default values are chosen so that the long root has squared length 2 in types A_n, B_n, D_n, E_6, E_7, E_8, and F_4, squared length 4 in C_n, and squared length 6 in type G_2. <br>If upper index is not equal to 1, the symmetric Cartan matrix is given by dividing the default symmetric Cartan matrix by the upper index. 

*WriteSemisimpleLieAlgebra* [WriteSemisimpleLieAlgebra] {Calculator::writeToHardDiskOrPrintSemisimpleLieAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WriteSemisimpleLieAlgebra%28F_4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WriteSemisimpleLieAlgebra(F_4)
```
Writes semisimple Lie algebra structure constants to the output folder of the calculator. Available to logged-in admins only. 

*PrintSemisimpleLieAlgebra* [PrintSemisimpleLieAlgebra] {Calculator::printSemisimpleLieAlgebraVerbose}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintSemisimpleLieAlgebra%7b%7d%28F_4%29%3b%5cnPrintSemisimpleLieAlgebra%7b%7d%282%20G%5e5_2%20%2b%20B_3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintSemisimpleLieAlgebra{}(F_4);
PrintSemisimpleLieAlgebra{}(2 G^5_2 + B_3);
```
Creates a printout with information about the semisimple Lie algebra, including the Lie bracket pairing table. In addition, this function creates a graphics of the root system. 

*PrecomputeSemisimpleLieAlgebraStructureEverything* [PrecomputeSemisimpleLieAlgebraStructureEverything] {CalculatorLieTheory::precomputeSemisimpleLieAlgebraStructureEverything}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrecomputeSemisimpleLieAlgebraStructureEverything%200%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrecomputeSemisimpleLieAlgebraStructureEverything 0
```
Function available to logged-in admins only. Precomputes all built-in semisimple Lie algebra information: root subalgebras, sl(2) subalgebras and semisimple Lie subalgebras, over the complex numbers. Real forms are not included, although there is code for that in the works.Argument gives a starting point (0 or negative to start at the beginning). Turn process monitoring on when using this function.

*PrecomputeSemisimpleLieAlgebraStructureAllButSubalgebras* [PrecomputeSemisimpleLieAlgebraStructureAllButSubalgebras] {CalculatorLieTheory::precomputeSemisimpleLieAlgebraStructureAllButSubalgebras}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrecomputeSemisimpleLieAlgebraStructureAllButSubalgebras%200%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrecomputeSemisimpleLieAlgebraStructureAllButSubalgebras 0
```
Function available to logged-in admins only. Precomputes all built-in semisimple Lie algebra information over the complex numbers, except the semisimple Lie subalgebras. Similar to the precomputeSemisimpleLieAlgebraStructureEverything except that it omits the computation of all semisimple subalgebras, which is by far the slowest computation.Turn process monitoring on when using this function.

*ChevalleyGenerator* [ChevalleyGenerator] {Calculator::chevalleyGenerator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5bChevalleyGenerator%7b%7d%28G_2%2c%206%29%2c%20ChevalleyGenerator%7b%7d%28G_2%2c%20-6%29%5d%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
[ChevalleyGenerator{}(G_2, 6), ChevalleyGenerator{}(G_2, -6)]
```
First argument must be a semisimple Lie algebra, second argument must be an integer from -N to N, where N is the number of positive roots of the Lie algebra. The function returns the Chevalley-Weyl generator labeled by the root corresponding to the integer index. The roots are indexed in the ordered displayed by the PrintSemisimpleLieAlgebra function. 

*CartanGenerator* [CartanGenerator] {Calculator::cartanGenerator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CartanGenerator%7b%7d%28G_2%2c%201%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CartanGenerator{}(G_2, 1)
```
First argument must be a semisimple Lie algebra, second argument must be a number between 1 and K, where K is the rank of the Lie algebra. In this case the function returns the element of the Cartan subalgebra that is dual to the simple root with the same index. Note that this element of the Cartan subalgebra is proportional to a Chevalley-Weyl generator with a coefficient of proportionality equal to 2/(simple root length squared) ).

*StandardRepresentationMatrix* [StandardRepresentationMatrix] {CalculatorLieTheory::standardRepresentationMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Type%20%3d%20A_4%3b%5cnStandardRepresentationMatrix%7b%7d%28Type%2c%20g_1%29%3b%5cng_%7b%7bj%7d%7d%3dChevalleyGenerator%28Type%2c%20j%29%3b%5cnh_%7b%7bj%7d%7d%3dCartanGenerator%28Type%2c%20j%29%3b%5cnStandardRepresentationMatrix%28g_1%29%3b%5cnStandardRepresentationMatrix%28g_1-g_-1%29%3b%5cnStandardRepresentationMatrix%28%20%28g_1%2bg_2%20-%20g_-1-g_-2%29%5e2%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Type = A_4;
StandardRepresentationMatrix{}(Type, g_1);
g_{{j}}=ChevalleyGenerator(Type, j);
h_{{j}}=CartanGenerator(Type, j);
StandardRepresentationMatrix(g_1);
StandardRepresentationMatrix(g_1-g_-1);
StandardRepresentationMatrix( (g_1+g_2 - g_-1-g_-2)^2);
```
Computes matrix of an element of a semisimple Lie algebra (or its universal enveloping algebra) in a 'natural' representation of the Lie algebra. Implemented for type A only at the moment.The argument should be a Chevalley generator or an element of the Universal enveloping algebra.

*AdjointMatrix* [AdjointMatrix] {CalculatorLieTheory::adjointMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Type%20%3d%20A_4%3b%5cng_%7b%7bj%7d%7d%3dChevalleyGenerator%28Type%2c%20j%29%3b%5cnh_%7b%7bj%7d%7d%3dCartanGenerator%28Type%2c%20j%29%3b%5cnAdjointMatrix%28g_2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Type = A_4;
g_{{j}}=ChevalleyGenerator(Type, j);
h_{{j}}=CartanGenerator(Type, j);
AdjointMatrix(g_2)
```
Computes the adjoint matrix of element g. The adjoint matrix is the matrix of the linear map ad(g):x -> [g,x].First element g = element of a semisimple Lie algebra. Extended to a map on the universal enveloping algebra via: ad(g_1, g_2) = ad(g_1)ad(g_2)

*CartanInvolution* [CartanInvolution] {CalculatorLieTheory::cartanInvolution}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CartanInvolution%28AI%2c%204%29%3b%5cnCartanInvolution%28AI%2c%205%29%3b%5cnCartanInvolution%28AII%2c%204%29%3b%5cnCartanInvolution%28AII%2c%205%29%3b%5cnCartanInvolution%28AIII%2c%204%29%3b%5cnCartanInvolution%28AIII%2c%204%2c%200%29%3b%5cnCartanInvolution%28AIII%2c%204%2c%201%29%3b%5cnCartanInvolution%28AIII%2c%204%2c%202%29%3b%5cnCartanInvolution%28AIII%2c%205%2c%202%29%3b%5cnCartanInvolution%28AIII%2c%205%2c%203%29%3b%5cnCartanInvolution%28BI%2c%204%29%3b%5cnCartanInvolution%28BI%2c%204%2c%200%29%3b%5cnCartanInvolution%28BI%2c%204%2c%201%29%3b%5cnCartanInvolution%28BI%2c%204%2c%202%29%3b%5cnCartanInvolution%28BI%2c%204%2c%203%29%3b%5cnCartanInvolution%28BI%2c%205%2c%201%29%3b%5cnCartanInvolution%28BI%2c%205%2c%202%29%3b%5cnCartanInvolution%28BI%2c%205%2c%203%29%3b%5cnCartanInvolution%28BI%2c%205%2c%204%29%3b%5cnCartanInvolution%28CI%2c%205%29%3b%5cnCartanInvolution%28CI%2c%205%2c1%29%3b%5cnCartanInvolution%28CI%2c%205%2c2%29%3b%5cnCartanInvolution%28CI%2c%205%2c%203%29%3b%5cnCartanInvolution%28CI%2c%205%2c%204%29%3b%5cnCartanInvolution%28DI%2c%205%29%3b%5cnCartanInvolution%28DI%2c%205%2c1%29%3b%5cnCartanInvolution%28DI%2c%205%2c2%29%3b%5cnCartanInvolution%28DI%2c%205%2c3%29%3b%5cnCartanInvolution%28DI%2c%205%2c4%29%3b%5cnCartanInvolution%28DI%2c%205%2c5%29%3b%5cnCartanInvolution%28DII%2c%205%29%3b%5cnCartanInvolution%28DII%2c%205%2c1%29%3b%5cnCartanInvolution%28DII%2c%205%2c2%29%3b%5cnCartanInvolution%28DII%2c%205%2c3%29%3b%5cnCartanInvolution%28DII%2c%205%2c4%29%3b%5cnCartanInvolution%28DII%2c%205%2c5%29%3b%5cnCartanInvolution%28DII%2c%203%2c1%29%3b%5cnCartanInvolution%28E_6%2c%20EI%29%3b%5cnCartanInvolution%28E_6%2c%20EII%29%3b%5cnCartanInvolution%28E_6%2c%20EIII%29%3b%5cnCartanInvolution%28E_6%2c%20EIV%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CartanInvolution(AI, 4);
CartanInvolution(AI, 5);
CartanInvolution(AII, 4);
CartanInvolution(AII, 5);
CartanInvolution(AIII, 4);
CartanInvolution(AIII, 4, 0);
CartanInvolution(AIII, 4, 1);
CartanInvolution(AIII, 4, 2);
CartanInvolution(AIII, 5, 2);
CartanInvolution(AIII, 5, 3);
CartanInvolution(BI, 4);
CartanInvolution(BI, 4, 0);
CartanInvolution(BI, 4, 1);
CartanInvolution(BI, 4, 2);
CartanInvolution(BI, 4, 3);
CartanInvolution(BI, 5, 1);
CartanInvolution(BI, 5, 2);
CartanInvolution(BI, 5, 3);
CartanInvolution(BI, 5, 4);
CartanInvolution(CI, 5);
CartanInvolution(CI, 5,1);
CartanInvolution(CI, 5,2);
CartanInvolution(CI, 5, 3);
CartanInvolution(CI, 5, 4);
CartanInvolution(DI, 5);
CartanInvolution(DI, 5,1);
CartanInvolution(DI, 5,2);
CartanInvolution(DI, 5,3);
CartanInvolution(DI, 5,4);
CartanInvolution(DI, 5,5);
CartanInvolution(DII, 5);
CartanInvolution(DII, 5,1);
CartanInvolution(DII, 5,2);
CartanInvolution(DII, 5,3);
CartanInvolution(DII, 5,4);
CartanInvolution(DII, 5,5);
CartanInvolution(DII, 3,1);
CartanInvolution(E_6, EI);
CartanInvolution(E_6, EII);
CartanInvolution(E_6, EIII);
CartanInvolution(E_6, EIV);

```
Computes the Cartan involution from a Vogan diagram. The Vogan diagrams are listed on page 695, Knapp, Lie groups beyond an introduction, 2nd edition.However, these were given no labels. We merged various labeling sytems found in: Lie Groups and Lie Algebras III: Structure of Lie Groups and Lie Algebras, Vinberg, Onishchik, page 229;Shôrô Araki, On root systems and an infinitesimal classification of irreducible symmetric spaces, 1962, the tables in section 5 and page 695 in Knapp's book quoted above. The possible labels are: AI, n, matching the real form k_0=so(n) in sl(n, R).
AII, n, n-odd, matching the real form k_0=sp(n) in sl(n, H).
AIII, n, k, matching the real form k_0=s(u(n)+u(n-k)) in su(n, n-k).The first argument is one of the labels above. The second argument n - the rank of the ambient Lie algebra. The third parameter - enter it only if it applies - is the extra parameter required for the following types:AIII, .This is work in progress.

*AdCommonEigenspace* [AdCommonEigenspace] {CalculatorLieTheory::adCommonEigenSpaces}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AdCommonEigenspace%7b%7d%28F_4%2c%20-5%20%28g_%7b9%7d%29%20%2b3%20%28g_%7b13%7d%29%20%2b5%20%28g_%7b16%7d%29%20%2b4%20%28g_%7b10%7d%29%2c%20g_%7b14%7d%2bg_%7b22%7d%2bg_%7b20%7d%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AdCommonEigenspace{}(F_4, -5 (g_{9}) +3 (g_{13}) +5 (g_{16}) +4 (g_{10}), g_{14}+g_{22}+g_{20})
```
Computes common eigenspace of the adjoint action of semisimple Lie algebra elements inside the semisimple Lie algebra. 

*AttemptExtendingEtoHEFwithHinCartan* [AttemptExtendingEtoHEFwithHinCartan] {Calculator::attemptExtendingEtoHEFwithHinCartan}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AttemptExtendingEtoHEFwithHinCartan%7b%7d%28F_4%2c%20g_1%2b2g_2%2b3g_3%2b4g_4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AttemptExtendingEtoHEFwithHinCartan{}(F_4, g_1+2g_2+3g_3+4g_4)
```
Attempts to embed an element E into an sl(2)-triple over the rationals, such that the element H is in the ambient Cartan algebra. 

*PrintMacdonaldPolys* [PrintMacdonaldPolys] {CalculatorFunctionsWeylGroup::macdonaldPolys}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintMacdonaldPolys%7b%7d%28B_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintMacdonaldPolys{}(B_3)
```
Prints macdonald polynomials from a semisimple type. 

*CharPoly* [CharPoly] {CalculatorFunctions::characteristicPolynomialMatrix}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%5c%5cbegin%7bpmatrix%7d2%20%26%203%20%20%26%205%20%5c%5c%5c%5c%207%20%26%2011%20%26%2013%5c%5c%5c%5c%2017%26%2019%20%26%2023%5c%5cend%7bpmatrix%7d%3b%20CharPoly%7b%7dA%3bMinPolyMatrix%7b%7dA%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A =\begin{pmatrix}2 & 3  & 5 \\ 7 & 11 & 13\\ 17& 19 & 23\end{pmatrix}; CharPoly{}A;MinPolyMatrix{}A;
```
Computes the characteristic polynomial of a matrix (= det(A-q*Id)), provided that the matrix is not too large.

*MakeCharacterLieAlg* [MakeCharacterLieAlg] {CalculatorLieTheory::characterSemisimpleLieAlgebraFiniteDimensional}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22x%20%3d%20MakeCharacterLieAlg%7b%7d%28G_2%2c%20%281%2c%200%29%29%3b%5cny%20%3d%20MakeCharacterLieAlg%7b%7d%28G_2%2c%20%280%2c%201%29%29%3b%5cnx%20%2a%20y%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
x = MakeCharacterLieAlg{}(G_2, (1, 0));
y = MakeCharacterLieAlg{}(G_2, (0, 1));
x * y
```
Creates character of a semisimple Lie algebra finite dimensional irreducible module. First argument gives type, second argument gives highest weight in fundamental coordinates.

*GetLinksToSimpleLieAlgebraPrintouts* [GetLinksToSimpleLieAlgebraPrintouts] {Calculator::getLinksToSimpleLieAlgebras}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GetLinksToSimpleLieAlgebraPrintouts%7b%7d0%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GetLinksToSimpleLieAlgebraPrintouts{}0
```
Gets simple Lie algebra links to the calculator.

*SlTwoRealFormStructure* [SlTwoRealFormStructure] {CalculatorLieTheory::slTwoRealFormStructureComputeOnDemand}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SlTwoRealFormStructure%7b%7d%28a_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SlTwoRealFormStructure{}(a_3)
```
Computes the Lie algebra structure over Kostant-Sekiguchi sl twos with respect to maximally compact real form.

*SlTwoRealFormStructureRecompute* [SlTwoRealFormStructureRecompute] {CalculatorLieTheory::slTwoRealFormStructureForceRecompute}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SlTwoRealFormStructureForceRecompute%7b%7d%28a_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SlTwoRealFormStructureForceRecompute{}(a_3)
```
Same as SlTwoRealFormStructure but forces a recompute. 

*IsReductiveLieSubalgebra* [IsReductiveLieSubalgebra] {CalculatorLieTheory::isReductiveLieSubalgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%28F_4%2c%5cng_%7b-1%7d%2c%20g_%7b6%7d%20%20%20%2c%20g_%7b9%7d%2bg_%7b2%7d%2c%5cng_%7b10%7d%2c%20g_%7b13%7d%20%20%2c%20g_%7b16%7d%2bg_%7b2%7d%2c%5cng_%7b17%7d-6%2f5g_%7b14%7d%2c%20g_%7b19%7d%2b8%2f5g_%7b14%7d%2c%5cng_%7b20%7d%2bg_%7b14%7d%20%20%20%2c%20g_%7b21%7d%2c%5cng_%7b22%7d%2c%20g_%7b23%7d%20%20%2c%5cng_%7b24%7d%5cn%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
(F_4,
g_{-1}, g_{6}   , g_{9}+g_{2},
g_{10}, g_{13}  , g_{16}+g_{2},
g_{17}-6/5g_{14}, g_{19}+8/5g_{14},
g_{20}+g_{14}   , g_{21},
g_{22}, g_{23}  ,
g_{24}
)
```
Not implemented yet. Given a set of elements of a semisimple lie algebra, tries to determine whether the Lie subalgebra they generate is reductive and what is its type.

*ConesIntersection* [ConesIntersection] {CalculatorFunctions::conesIntersect}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22v_1%20%3d%20%281%2c%202%2c%203%29%3b%5cnv_2%20%3d%20%281%2c%203%2c%202%29%3b%5cnv_3%20%3d%20%283%2c%201%2c%201%29%3b%5cnv_4%20%3d%20%28-%202%2c%202%2c%202%29%3b%5cnConesIntersection%7b%7d%28%28v_1%2c%20v_2%29%2c%20%28v_3%2c%20v_4%20%29%29%3b%5cnConesIntersection%7b%7d%28%28v_1%2c%20v_2%29%2c%20%28v_3%2c%20-%20v_4%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
v_1 = (1, 2, 3);
v_2 = (1, 3, 2);
v_3 = (3, 1, 1);
v_4 = (- 2, 2, 2);
ConesIntersection{}((v_1, v_2), (v_3, v_4 ));
ConesIntersection{}((v_1, v_2), (v_3, - v_4));
```
Takes as input two sequences of vectors, generates two cones, and intersects them using the simplex algorithm. The output is a string report of the operation. The first cone is generated over Z_{&gt;} (``strict cone'') the second cone is generated over Z_{&gt;= 0} (``non-strict cone'').

*HeighestWeightVector* [HeighestWeightVector] {CalculatorLieTheory::highestWeightVector}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%3d%20ChevalleyGenerator%7b%7d%28B_3%2c%20i%29%3b%5cnh_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28B_3%2c%20i%29%3b%5cnv_%5c%5cmu%3dHeighestWeightVector%7b%7d%20%28A_3%2c%20%281%2c0%2c1%29%2c%280%2c0%2c0%29%29%3b%5cnv_%5c%5clambda%20%3dHeighestWeightVector%7b%7d%28B_3%2c%20%28x_1%2c0%2c1%29%2c%281%2c0%2c0%29%29%3b%5cnh_1g_%7b-%201%7dv_%5c%5clambda%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}}= ChevalleyGenerator{}(B_3, i);
h_{{i}}= CartanGenerator{}(B_3, i);
v_\mu=HeighestWeightVector{} (A_3, (1,0,1),(0,0,0));
v_\lambda =HeighestWeightVector{}(B_3, (x_1,0,1),(1,0,0));
h_1g_{- 1}v_\lambda
```
Highest weight vector in a generalized Verma module. The first argument gives the semisimple Lie algebra. The second argument gives the highest weight in fundamental coordinates. The third argument parametrizes the parabolic subalgebra, e.g. (1,0,0) stands for a parabolic subalgebra with first simple root crossed-out. The second argument is allowed to have entries that are not non-negative integers in the positions in which the third argument has 1's. 

*PrintModule* [PrintModule] {CalculatorLieTheory::printGeneralizedVermaModule}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintModule%7b%7d%28G_2%2c%20%282%2c%200%29%2c%20%280%2c%200%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintModule{}(G_2, (2, 0), (0, 0))
```
Makes a report on a finite dimensional Lie algebra module, or more generally, on a generalized Verma module (irreducible finite dimensional semisimple Lie algebra modules are a partial case of generalized Verma modules). The first argument gives the semisimple Lie algebra. The second argument gives the highest weight in fundamental coordinates. The third argument parametrizes the parabolic subalgebra, e.g. (1,0) stands for a parabolic subalgebra (lying in algebra of rank 2) with first simple root crossed-out. The second argument is allowed to have entries that are not non-negative integers in the positions in which the third argument has 1's. 

*HighestWeightTAAbf* [HighestWeightTAAbf] {CalculatorFunctions::highestWeightTransposeAntiAutomorphismBilinearForm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7ba%7d%7d%3d%20ChevalleyGenerator%7b%7d%20%28G_2%2c%20a%29%3b%5cnHighestWeightTAAbf%7b%7d%28g_%7b-%201%7d%20g_%7b-2%7d%2c%20g_%7b-%201%7dg_%7b-2%7d%2c%20%282%2c2%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{a}}= ChevalleyGenerator{} (G_2, a);
HighestWeightTAAbf{}(g_{- 1} g_{-2}, g_{- 1}g_{-2}, (2,2))
```
Highest weight transpose anti-automorphism bilinear form, a.k.a. Shapovalov form. Let M be a Verma module with highest weight vector v given in fundamental coordinates. Let P:M->M be a projection map onto Cv that maps every weight vector of M of weight different from the highest to 0. Let u_1, u_2 be two words in the universal enveloping algebra. Then define HighestWeightTAAbf(u_1,u_2)= Tr_M (P ( taa(u_1) u_2 ), where taa() is the transpose anti-automorphism of g. 

*WeylDimFormula* [WeylDimFormula] {CalculatorFunctions::weylDimFormula}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylDimFormula%7b%7d%28G_2%2c%20%28x%2c0%29%29%3b%5cnWeylDimFormula%7b%7d%28B_3%2c%20%28x%2c0%2c0%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylDimFormula{}(G_2, (x,0));
WeylDimFormula{}(B_3, (x,0,0));
```
Weyl dimension formula. First argument gives the type of the Weyl group of the simple Lie algebra in the form Type_Rank (e.g. E_6). The second argument gives the highest weight in fundamental coordinates. 

*AnimateLittelmannPaths* [AnimateLittelmannPaths] {CalculatorLieTheory::animateLittelmannPaths}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22AnimateLittelmannPaths%7b%7d%28G_2%2c%20%282%2c%200%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
AnimateLittelmannPaths{}(G_2, (2, 0));
```
Generates all Littelmann-Lakshmibai-Seshadri paths, draws them and animates them. Presented first on the seminar in Charles University Prague. The first argument gives the type of the semisimple Lie algebra, the second gives the highest weight. 

*DecomposeInducingRepGenVermaModule* [DecomposeInducingRepGenVermaModule] {CalculatorLieTheory::decomposeFDPartGeneralizedVermaModuleOverLeviPart}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DecomposeInducingRepGenVermaModule%7b%7d%28B_3%2c%20%280%2c%201%2c%201%29%2c%20%281%2c%200%2c%200%29%2c%20%281%2c%200%2c%201%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DecomposeInducingRepGenVermaModule{}(B_3, (0, 1, 1), (1, 0, 0), (1, 0, 1))
```
Decomposes the inducing module of a generalized Verma module over the Levi part of a parabolic smaller than the inducing one. The first argument gives the type of the algebra. The second argument gives the highest weight of the module in fundamental coordinates. The third argument gives the parabolic subalgebra with respect to which we induce. The last argument gives the parabolic subalgebra with respect to whose Levi part we decompose.

*Casimir* [Casimir] {Calculator::casimir}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Casimir%7b%7d%28G_2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Casimir{}(G_2)
```
Gives the Casimir element. 

*CasimirWRTLevi* [CasimirWRTLevi] {CalculatorFunctions::casimirWithRespectToLevi}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CasimirWRTLevi%7b%7d%28B_3%2c%20%281%2c0%2c0%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CasimirWRTLevi{}(B_3, (1,0,0))
```
Gives the Casimir element of a Levi part of a parabolic subalgebra. First argument = ambient Lie algebra. Second argument = parabolic selection = 1 stands for simple root outside of the Levi component, 0 stands for simple root of the Levi component.

*HmmG2inB3* [HmmG2inB3] {Calculator::embedG2InB3}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7ba%7d%7d%3d%20ChevalleyGenerator%7b%7d%20%28G_2%2c%20a%29%3b%20HmmG2inB3%7b%7d%28g_1%29%3b%5cnHmmG2inB3%7b%7d%28g_2%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{a}}= ChevalleyGenerator{} (G_2, a); HmmG2inB3{}(g_1);
HmmG2inB3{}(g_2) 
```
Embeds elements of the Universal enveloping of G_2 in B_3, following an embedding found in a paper by McGovern.

*DrawRootSystem* [DrawRootSystem] {CalculatorFunctions::drawRootSystem}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DrawRootSystem%7b%7d%28A_7%2c%20%281%2c%200%20%2c%202%2c%202%2c%202%2c%200%2c%201%29%2c%20%281%2c%203%2c%202%2c%202%2c%202%2c%203%2c%201%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DrawRootSystem{}(A_7, (1, 0 , 2, 2, 2, 0, 1), (1, 3, 2, 2, 2, 3, 1))
```
Draws the root system of a semisimple Lie algebra. Takes one or three arguments: if one argument is provided, that must be a semisimple Lie algebra or a semisimple Lie algebra type; if three arguments are provided, the first must be a semisimple Lie algebra or a semisimple Lie algebra type, and the second and third argument must be two rational vectors of dimension equal to the rank of the semisimple Lie algebra. The root system is drawn in a Coxeter plane. If the extra two vectors are given, they are used to initialize a preferred projection plane in the change-to-basis entries below the graphics. Clicking the obvious button creates a basis change animations ideal for presentations.

*DrawWeightSupportWithMults* [DrawWeightSupportWithMults] {CalculatorFunctions::drawWeightSupportWithMults}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DrawWeightSupportWithMults%7b%7d%28B_3%2c%20%280%2c%201%2c%201%29%29%3b%5cnDrawWeightSupportWithMults%7b%7d%28G_2%2c%20%281%2c%200%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DrawWeightSupportWithMults{}(B_3, (0, 1, 1));
DrawWeightSupportWithMults{}(G_2, (1, 0))
```
Draws the weight support of an irreducible finite-dimensional highest weight module. The first argument gives the type and the second gives the highest weight given in fundamental weight basis. The maximum number of drawn weights allowed is (hard-code) limited to 10 000. If the number of weights in the weight support exceeds this number, only the first 10 000 weights will be drawn. <b>Warning. Drawing text (i.e. multiplicities) is very javascript-processor-intensive. Use only for *small* examples, else you might hang your browser. </b>

*DrawWeightSupport* [DrawWeightSupport] {CalculatorFunctions::drawWeightSupport}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DrawWeightSupport%7b%7d%28B_3%2c%20%281%2c%201%2c%201%29%29%3b%5cnDrawWeightSupport%7b%7d%28G_2%2c%20%281%2c%202%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DrawWeightSupport{}(B_3, (1, 1, 1));
DrawWeightSupport{}(G_2, (1, 2))
```
Same as DrawWeightSupportWithMults but displays no multiplicities. Same warning for hanging up your browser with javascript holds.

*SplitFDpartB3overG2CharsOnly* [SplitFDpartB3overG2CharsOnly] {CalculatorLieTheory::splitFDpartB3overG2CharsOnly}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SplitFDpartB3overG2CharsOnly%28x_1%2c%202%2c%200%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SplitFDpartB3overG2CharsOnly(x_1, 2, 0)
```
Splits the finite dimensional part of the inducing module of the generalized Verma module of B_3(so(7)) into G_2-components. The argument is gives the highest weight of the generalized Verma module in fundamental coordinates with respect to so(7). The arguments which are not small integers indicate the non-selected roots of the inducing parabolic subalgebra of B_3. 

*SplitFDpartB3overG2* [SplitFDpartB3overG2] {CalculatorFunctions::splitFDpartB3overG2}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SplitFDpartB3overG2%7b%7d%28x_1%2c%201%2c%200%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SplitFDpartB3overG2{}(x_1, 1, 0)
```
Splits the finite dimensional part of the inducing module of the generalized Verma module of B_3(so(7)) into G_2-components. The argument is gives the highest weight of the generalized Verma module in fundamental coordinates with respect to so(7). The arguments which are not small integers indicate the non-selected roots of the inducing parabolic subalgebra of B_3. 

*PrintB3G2branchingTableCharsOnly* [PrintB3G2branchingTableCharsOnly] {CalculatorLieTheory::printB3G2branchingTableCharsOnly}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintB3G2branchingTableCharsOnly%7b%7d%282%2c%20%280%2c0%2c0%29%29%3b%20PrintB3G2branchingTableCharsOnly%7b%7d%282%2c%20%28x_1%2c0%2c0%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintB3G2branchingTableCharsOnly{}(2, (0,0,0)); PrintB3G2branchingTableCharsOnly{}(2, (x_1,0,0))
```
Creates a table of branching of finite dimensional B_3-modules over G_2. The argument of the function gives the maximum height of the B_3-weight. The second argument indicates the parabolic subalgebra of B_3- zero entry stands for the corresponding root space lying in the Levi part of the parabolic. Non-zero entry means the corresponding negative root space is not in the parabolic. The expression given in that coordinate is used as the corresponding highest weight. 

*PrintB3G2branchingTable* [PrintB3G2branchingTable] {Calculator::printB3G2branchingTable}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintB3G2branchingTable%7b%7d%281%2c%20%280%2c0%2c0%29%29%3b%20PrintB3G2branchingTable%7b%7d%281%2c%20%28x_1%2c0%2c0%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintB3G2branchingTable{}(1, (0,0,0)); PrintB3G2branchingTable{}(1, (x_1,0,0))
```
Creates a table of branching of finite dimensional B_3-modules over G_2. The argument of the function gives the maximum height of the B_3-weight. The function works with arguments 0 or 1; values of 2 or more must be run off-line.

*SplitFDTensorGenericGeneralizedVerma* [SplitFDTensorGenericGeneralizedVerma] {CalculatorLieTheory::splitGenericGeneralizedVermaTensorFiniteDimensional}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SplitFDTensorGenericGeneralizedVerma%7b%7d%28G_2%2c%20%281%2c%200%29%2c%20%28x_1%2c%20x_2%29%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SplitFDTensorGenericGeneralizedVerma{}(G_2, (1, 0), (x_1, x_2)); 
```
Experimental, please don't use. Splits generic generalized Verma module tensor finite dimensional module. 

*WriteGenVermaAsDiffOperatorsUpToWeightLevel* [WriteGenVermaAsDiffOperatorsUpToWeightLevel] {CalculatorLieTheory::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WriteGenVermaAsDiffOperatorsUpToWeightLevel%7b%7d%28B_3%2c%201%2c%20%280%2c%200%2c%20y%29%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WriteGenVermaAsDiffOperatorsUpToWeightLevel{}(B_3, 1, (0, 0, y)); 
```
<b>Experimental, please don't use</b>. Embeds a Lie algebra in the Weyl algebra with matrix coefficients. The third argument gives the highest weight. The non-zero entries of the highest weight give the root spaces outside of the Levi part of the parabolic. The second argument gives the weight level to which the computation should be carried out

*MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder* [MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder] {CalculatorLieTheory::writeGenVermaModAsDiffOperatorsGeneratorOrder}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder%7b%7d%28B_3%2c%20%280%2c0%2c0%29%2c%20%280%2c%200%2c%201%29%2c%20x%2c%20%5c%5cpartial%2c%20a%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder{}(B_3, (0,0,0), (0, 0, 1), x, \partial, a); 
```
Constructs a parabolically induced map from a semisimple Lie algebra to a Weyl algebra with matrix coefficients. More precisely, this function constructs a generalized Verma module and writes it using differential operators with matrix coefficients. The first argument gives the type of the semisimple Lie algebra, the second argument gives the highest weight in fundamental coordinates. The third argument gives the parabolic subalgebra selection with 0 standing for non-crossed out roots and 1 for crossed-out roots. In all non crossed-out root positions, the coordinates of the highest weight vector must be small integers (because the highest weight must be dominant and integral with respect to the Levi part of the inducing parabolic subalgebra). In the crossed-out root positions, the coordinates of the highest weight vector can be arbitrary polynomial expressions. The algorithm depends on an order chosen on the Chevalley-Weyl generators of the nilradical. Here, we sort the nilradical elements in descending order relative to the order of their roots. In addition to the first 3 arguments, this function accepts further optional arguments, customizing the notation of the final printout.

*MapSemisimpleLieAlgebraInWeylAlgebra* [MapSemisimpleLieAlgebraInWeylAlgebra] {Calculator::writeGenVermaModAsDiffOperatorsNilOrderDescending}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MapSemisimpleLieAlgebraInWeylAlgebra%7b%7d%28B_3%2c%20%280%2c%200%2c%200%29%2c%20%280%2c%200%2c%201%29%2c%20x%2c%20%5c%5cpartial%2c%20a%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MapSemisimpleLieAlgebraInWeylAlgebra{}(B_3, (0, 0, 0), (0, 0, 1), x, \partial, a); 
```
Same as MapSemisimpleLieAlgebraInWeylAlgebra, but with a different order on the elements of the nilradical. We sort the elements of the nilradical as follows. Consider the partial order given by the graded lex order on the coordinates that correspond to crossed-out roots. Then extend this partial order by ``breaking the ties'' via the graded lexicographic order on the non-crossed out roots. This extension we call the nil-order. Now this function sorts the elements of the nilradical in descending nil-order, that is, elements with higher nil-order come first.

*MapSemisimpleLieAlgebraInWeylAlgebraAllGens* [MapSemisimpleLieAlgebraInWeylAlgebraAllGens] {CalculatorLieTheory::writeGenVermaModAsDiffOperatorsAllGensNilOrderDescending}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MapSemisimpleLieAlgebraInWeylAlgebraAllGens%7b%7d%28B_3%2c%20%280%2c0%2c0%29%2c%20%280%2c%200%2c%201%29%2c%20x%2c%20%5c%5cpartial%2c%20a%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MapSemisimpleLieAlgebraInWeylAlgebraAllGens{}(B_3, (0,0,0), (0, 0, 1), x, \partial, a); 
```
Constructs a parabolically induced map from a semisimple Lie algebra to a Weyl algebra with matrix coefficients. More precisely, this function constructs a generalized Verma module and writes it using differential operators with matrix coefficients. The first argument gives the type of the semisimple Lie algebra, the second argument gives the highest weight in fundamental coordinates. The third argument gives the parabolic subalgebra selection with 0 standing for non-crossed out roots and 1 for crossed-out roots. In all non crossed-out root positions, the coordinates of the highest weight vector must be small integers (because the highest weight must be dominant and integral with respect to the Levi part of the inducing parabolic subalgebra). In the crossed-out root positions, the coordinates of the highest weight vector can be arbitrary polynomial expressions. In addition to the first 3 arguments, this function accepts further optional arguments, customizing the notation of the final printout. The differential operator part of the algorithm uses the nil-order given by the function MapSemisimpleLieAlgebraInWeylAlgebra.

*KLcoeffs* [KLcoeffs] {CalculatorLieTheory::kazhdanLuzstigCoeffificents}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22KLcoeffs%7b%7d%28B_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
KLcoeffs{}(B_3)
```
Computes the n by n tables of 1) Kazhdan-Lusztig polynomials, 2) R polynomials and 3) Kazhdan-Lusztig polynomials evaluated at one, where n<=192 is the size of the Weyl group (i.e. no larger than D_4(so(8)). The notation follows the original paper by Kazhdan and Lusztig, "Representations of Coxeter Groups and Hecke Algebras". The algorithm is directly derived from formulas (2.0a-c) there, as explained in the Wikipedia page on Kazhdan-Lusztig polynomials. Please note that the 192 by 192 element table takes almost 3 minutes to compute. Faster implementations of the KL polynomials are available from programs by Fokko du Cloux and others (our simple implementation stores the full table of R-polynomials and KL-polynomials in RAM memory at all times, unlike the other more efficient implementations).

*RootSubsystem* [RootSubsystem] {CalculatorLieTheory::rootSubsystem}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22RootSubsystem%28F_4%2c%20%280%2c%201%2c%200%2c%200%29%2c%20%280%2c%200%2c%201%2c%200%29%2c%20%281%2c%201%2c%202%2c%202%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
RootSubsystem(F_4, (0, 1, 0, 0), (0, 0, 1, 0), (1, 1, 2, 2))
```
Generates a root subsystem of a simple type. First argument indicates simple type, second, third, ... arguments give the generating roots. 

*PrintRootSubalgebrasAndSlTwos* [PrintRootSubalgebrasAndSlTwos] {CalculatorLieTheory::printRootSubalgebrasAndSlTwos}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintRootSubalgebrasAndSlTwos%28E_6%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintRootSubalgebrasAndSlTwos(E_6)
```
Prints sl(2) subalgebras and root subalgebras. The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).

*PrintRootSubalgebrasAndSlTwosRecompute* [PrintRootSubalgebrasAndSlTwosRecompute] {CalculatorLieTheory::printRootSubalgebrasAndSlTwosForceRecompute}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintRootSubalgebrasAndSlTwosRecompute%28b_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintRootSubalgebrasAndSlTwosRecompute(b_3)
```
Same as PrintRootSubalgebrasAndSlTwos but forces recomputation. Use to recompute obsolete or interrupted output files.

*ParabolicsInfoBruhatGraph* [ParabolicsInfoBruhatGraph] {CalculatorLieTheory::parabolicWeylGroupsBruhatGraph}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22parabolicsInfoBruhatGraph%7b%7d%28B_3%2c%281%2c0%2c0%29%2c%281%2c0%2c0%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
parabolicsInfoBruhatGraph{}(B_3,(1,0,0),(1,0,0))
```
Makes a table with information about the Weyl group of a parabolic subalgebra of the ambient Lie algebra, as well as the cosets (given by minimal coset representatives) of the Weyl subgroup in question. The input must have as many integers as there are simple roots in the ambient Lie algebra. If the root is crossed out (i.e. not a root space of the Levi part), one should put a 1 in the corresponding coordinate. Otherwise, one should put 0. For example, for Lie algebra B3(so(7)), calling parabolicsInfoBruhatGraph(0, 0, 0) gives you the Weyl group info for the entire algebra; calling parabolicsInfoBruhatGraph(1,0,0) gives you info for the Weyl subgroup generated by the last two simple root. In the produced graph, the element s_{\eta_i} corresponds to a reflection with respect to the i^th simple root. You will get your output as a .png file link, you must click onto the link to see the end result. <b>Please do not use for subalgebras larger than B_4 (so(9)). The vpf program has no problem handling this function up to E_6 but LaTeX crashes trying to process the output. </b>

*TestMonomialBasisConjecture* [TestMonomialBasisConjecture] {CalculatorLieTheory::testMonomialBaseConjecture}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestMonomialBasisConjecture%7b%7d%282%2c%2050%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestMonomialBasisConjecture{}(2, 50)
```
Tests the monomial basis conjecture from a common work by Jackson and Milev paper. First argument gives rank bound. Second argument gives dimension bound. 

*LSpath* [LSpath] {CalculatorLieTheory::LSPath}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LSpath%7b%7d%28G_2%2c%20%280%2c0%29%2c%20%282%2c1%29%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LSpath{}(G_2, (0,0), (2,1) )
```
Lakshmibai-Seshadri path starting at 0. The first argument gives the semisimple Lie algebra, the next arguments give the way-points of the path.

*LROdefine* [LROdefine] {CalculatorLieTheory::littelmannOperator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22e_%7b%7bi%7d%7d%3d%20LROdefine_i%3b%20e_%7b-%201%7d%20e_%7b-%201%7d%20LSpath%7b%7d%28G_2%2c%20%280%2c0%29%2c%20%282%2c1%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
e_{{i}}= LROdefine_i; e_{- 1} e_{- 1} LSpath{}(G_2, (0,0), (2,1))
```
Littelmann root operator e_i, where e_i is the Littelmann root operator with respect to root of index i. If i is negative then the e_i root operator is defined to be the f_\alpha operator.

*PrintProductDistancesModN* [PrintProductDistancesModN] {CalculatorFunctions::findProductDistanceModN}. (admin only) (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintProductDistancesModN%2865537%2c%20%2897%2c%2098%2c%2099%2c%20100%2c%20101%2c%20102%2c%20103%2c%20104%2c%20105%2c%20106%2c%20107%2c%20108%2c%20109%2c%20110%2c%20111%2c%20112%2c%20113%2c%20114%2c%20115%2c%20116%2c%20117%2c%20118%2c%20119%2c%20120%2c%20121%2c%20122%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintProductDistancesModN(65537, (97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122))
```
First argument = number N. Second argument = list of positive numbers. This function finds, modulo N, the product distance between 1 and each number mod N. We define the product distance between 1 and a number X as the minimal sum of elements whose product equals X mod N, where the elements are taken from the predefined list of positive integers given by the second argument.

*SolveProductSumEquationOverSetModN* [SolveProductSumEquationOverSetModN] {CalculatorFunctions::solveProductSumEquationOverSetModN}. (admin only) (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SolveProductSumEquationOverSetModN%28theMod%20%3d65537%3b%20theProduct%20%3d16628%3b%20theSum%3d1286%3b%20theSet%20%3d%20%2897%2c%2098%2c%2099%2c%20100%2c%20101%2c%20102%2c%20103%2c%20104%2c%20105%2c%20106%2c%20107%2c%20108%2c%20109%2c%20110%2c%20111%2c%20112%2c%20113%2c%20114%2c%20115%2c%20116%2c%20117%2c%20118%2c%20119%2c%20120%2c%20121%2c%20122%29%29%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SolveProductSumEquationOverSetModN(theMod =65537; theProduct =16628; theSum=1286; theSet = (97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122)) 
```
Tries to find one solution of the system a_1*a_2* ...= X mod N a_1+a_2+...=Y where the a_i's belong to a predefined set of positive numbers. 

*ComputeFKFT* [ComputeFKFT] {CalculatorLieTheory::computePairingTablesAndFKFTsubalgebras}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ComputeFKFT%28%20ComputeSemisimpleSubalgebras%28c_3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ComputeFKFT( ComputeSemisimpleSubalgebras(c_3))
```
Attempts to compute all Fernando-Kac subalgebras according to the most experimental, latest and greatest algorithm. Argument must be of type semisimple Lie subalgebras. 

*ComputeSemisimpleSubalgebras* [ComputeSemisimpleSubalgebras] {CalculatorLieTheory::computeSemisimpleSubalgebras}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ComputeSemisimpleSubalgebras%28A_2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ComputeSemisimpleSubalgebras(A_2)
```
Computes the semisimple subalgebras of a semisimple Lie algebra and creates a data structure containing them. 

*CentralizerChains* [CentralizerChains] {CalculatorLieTheory::getCentralizerChainsSemisimpleSubalgebras}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CentralizerChains%20%28ComputeSemisimpleSubalgebras%7b%7d%28B_3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CentralizerChains (ComputeSemisimpleSubalgebras{}(B_3))
```
This function is disabled by default (takes too long): you have to first call TurnRulesOn(CentralizerChains). Please use this function only if running the calculator on your own machine; don't use it on a public server. Creates a printout with centralizer chains of semisimple Lie subalgebras. 

*PrintSemisimpleSubalgebras* [PrintSemisimpleSubalgebras] {CalculatorLieTheory::printSemisimpleSubalgebrasRegular}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintSemisimpleSubalgebras%28B_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintSemisimpleSubalgebras(B_3)
```
<b>This function is being developed and is not implemented fully yet.</b> Prints the semisimple subalgebras of a semisimple Lie algebra. 

*PrintSemisimpleSubalgebrasRecompute* [PrintSemisimpleSubalgebrasRecompute] {CalculatorLieTheory::printSemisimpleSubalgebrasRecompute}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintSemisimpleSubalgebrasRecompute%28C_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintSemisimpleSubalgebrasRecompute(C_3)
```
<b>This function is being developed and is not implemented fully yet.</b>Prints the semisimple subalgebras of a semisimple Lie algebra. 

*PrintSemisimpleSubalgebrasNoCentralizers* [PrintSemisimpleSubalgebrasNoCentralizers] {CalculatorLieTheory::printSemisimpleSubalgebrasNoCentralizers}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintSemisimpleSubalgebrasNoCentralizers%28A_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintSemisimpleSubalgebrasNoCentralizers(A_3)
```
<b>This function is being developed and is not implemented fully yet.</b>Prints the semisimple subalgebra candidates of a Lie algebra. 

*PrintSemisimpleSubalgebrasFull* [PrintSemisimpleSubalgebrasFull] {CalculatorLieTheory::printSemisimpleSubalgebrasNilradicals}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrintSemisimpleSubalgebrasFull%7b%7d%28A_2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrintSemisimpleSubalgebrasFull{}(A_2)
```
<b>This function is being developed and is not implemented fully yet.</b>Prints the semisimple subalgebras of a semisimple Lie algebra. 

*CanBeExtendedParabolicallyTo* [CanBeExtendedParabolicallyTo] {CalculatorLieTheory::canBeExtendedParabolicallyTo}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CanBeExtendedParabolicallyTo%28A%5e3_1%2c%20A_5%29%3bCanBeExtendedParabolicallyTo%28a_2%2c%20b_3%29%3bCanBeExtendedParabolicallyTo%28g_2%2c%20b_3%29%3bCanBeExtendedParabolicallyTo%28d_5%2c%20e_6%29%3bCanBeExtendedParabolicallyTo%28b_3%2c%20a_10%29%3bCanBeExtendedParabolicallyTo%28c_2%2c%20c_10%29%3bCanBeExtendedParabolicallyTo%28b_3%2be_6%2ba_3%2c%20d_4%2be_8%2bf_4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CanBeExtendedParabolicallyTo(A^3_1, A_5);CanBeExtendedParabolicallyTo(a_2, b_3);CanBeExtendedParabolicallyTo(g_2, b_3);CanBeExtendedParabolicallyTo(d_5, e_6);CanBeExtendedParabolicallyTo(b_3, a_10);CanBeExtendedParabolicallyTo(c_2, c_10);CanBeExtendedParabolicallyTo(b_3+e_6+a_3, d_4+e_8+f_4)
```
Finds whether a Dynkin type extends via the standard parabolic extension to another. 

*EmbedSemisimpleInSemisimple* [EmbedSemisimpleInSemisimple] {CalculatorLieTheory::embedSemisimpleAlgebraInSemisimpleAlgebra}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22EmbedSemisimpleInSemisimple%7b%7d%28G%5e3_2%2c%20B_3%29%3bEmbedSemisimpleInSemisimple%7b%7d%28G_2%2c%20B_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
EmbedSemisimpleInSemisimple{}(G^3_2, B_3);EmbedSemisimpleInSemisimple{}(G_2, B_3)
```
Tries to find all embeddings of the first semisimple type into the second. Records all intermediate subalgebras. 

*LoadSemisimpleSubalgebras* [LoadSemisimpleSubalgebras] {CalculatorConversions::loadSemisimpleSubalgebras}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LoadSemisimpleSubalgebras%20%7b%7d%28EmbedSemisimpleInSemisimple%7b%7d%28G_2%2c%20B_3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LoadSemisimpleSubalgebras {}(EmbedSemisimpleInSemisimple{}(G_2, B_3))
```
<b>This function is being developed and is not implemented fully yet.</b>Loads a semisimpleSubalgebra from expression. 

*SltwoSubalgebra* [SltwoSubalgebra] {CalculatorConversions::slTwoSubalgebraPrecomputed}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SltwoSubalgebra%28%5cnB_%7b3%7d%2c%5cn3%20ChevalleyGenerator%28%28B%29_%7b3%7d%2c%20-2%29%2c%5cn3%20ChevalleyGenerator%28%20%28B%29_%7b3%7d%2c%203%29%5cn%2b%206%20ChevalleyGenerator%28%20%28B%29_%7b3%7d%2c%201%29%5cn%2b%2010%20%2f%203%20ChevalleyGenerator%28%20%28B%29_%7b3%7d%2c%202%29%5cn%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SltwoSubalgebra(
B_{3},
3 ChevalleyGenerator((B)_{3}, -2),
3 ChevalleyGenerator( (B)_{3}, 3)
+ 6 ChevalleyGenerator( (B)_{3}, 1)
+ 10 / 3 ChevalleyGenerator( (B)_{3}, 2)
)
```
<b>This function is being developed and is not implemented fully yet. </b>Loads an sl(2) subalgebra from expression. 

*Freudenthal* [Freudenthal] {CalculatorFunctions::freudenthalFormula}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Freudenthal%7b%7d%28B_3%2c%20%282%2c2%2c2%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Freudenthal{}(B_3, (2,2,2))
```
Computes the dominant weights with multiplicities of a finite dimensional module of a highest weight given in fundamental coordinates. The first argument gives the semisimple Lie algebra type, the second argument gives the highest weight in fundamental coordinates. 

*Killing* [Killing] {CalculatorLieTheory::killingForm}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22h_%7b%7bi%7d%7d%3d%20CartanGenerator%7b%7d%28F_1%2c%20i%29%3bKF%7b%7d%28%7b%7bi%7d%7d%2c%7b%7bj%7d%7d%29%3dKilling%7b%7d%28h_i%2c%20h_j%29%3bFunctionToMatrix%28KF%2c%204%2c%204%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
h_{{i}}= CartanGenerator{}(F_1, i);KF{}({{i}},{{j}})=Killing{}(h_i, h_j);FunctionToMatrix(KF, 4, 4)
```
Computes the Killing form product of two elements of semisimple Lie algebra. 

*FreudenthalFull* [FreudenthalFull] {CalculatorFunctions::freudenthalFull}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FreudenthalFull%7b%7d%28G_2%2c%20%282%2c%201%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FreudenthalFull{}(G_2, (2, 1))
```
Computes the full character and prints it to screen. Argument format same as the Freudenthal function. 

*PerturbSplittingNormal* [PerturbSplittingNormal] {CalculatorFunctions::perturbSplittingNormal}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PerturbSplittingNormal%7b%7d%28%280%2c0%2c0%2c0%29%2c%20%28%28-%201%2c%20-2%2c%20-2%2c%20-2%29%2c%20%28-%201%2c%20-%201%2c%20-2%2c%20-2%29%2c%20%280%2c%20-%201%2c%20-2%2c%20-2%29%2c%20%28-%201%2c%20-%201%2c%20-%201%2c%20-%201%29%2c%20%280%2c%20-%201%2c%20-%201%2c%20-%201%29%2c%280%2c%200%2c%20-%201%2c%20-%201%29%2c%20%28-%201%2c%20-%201%2c%200%2c%200%29%2c%20%280%2c%20-%201%2c%200%2c%200%29%2c%20%28-%201%2c%200%2c%200%2c%200%29%2c%20%281%2c%200%2c%200%2c%200%29%2c%20%280%2c%201%2c%200%2c%200%29%2c%20%281%2c%201%2c%200%2c%200%29%2c%280%2c%200%2c%201%2c%201%29%2c%20%280%2c%201%2c%201%2c%201%29%2c%20%281%2c%201%2c%201%2c%201%29%2c%20%280%2c%201%2c%202%2c%202%29%2c%20%281%2c%201%2c%202%2c%202%29%2c%20%281%2c%202%2c%202%2c%202%29%29%2c%28%28-2%2c%20-3%2c%20-4%2c%20-2%29%2c%20%28-%201%2c%20-3%2c%20-4%2c%20-2%29%2c%20%28-%201%2c%20-2%2c%20-4%2c%20-2%29%2c%20%28-%201%2c%20-2%2c%20-3%2c%20-2%29%2c%20%28-%201%2c%20-2%2c%20-2%2c%20-2%29%2c%20%28-%201%2c%20-2%2c%20-3%2c%20-%201%29%2c%28-%201%2c%20-%201%2c%20-2%2c%20-2%29%2c%20%28-%201%2c%20-2%2c%20-2%2c%20-%201%29%2c%20%280%2c%20-%201%2c%20-2%2c%20-2%29%2c%20%28-%201%2c%20-%201%2c%20-2%2c%20-%201%29%2c%20%28-%201%2c%20-2%2c%20-2%2c%200%29%2c%280%2c%20-%201%2c%20-2%2c%20-%201%29%2c%20%28-%201%2c%20-%201%2c%20-%201%2c%20-%201%29%2c%20%28-%201%2c%20-%201%2c%20-2%2c%200%29%2c%20%280%2c%20-%201%2c%20-%201%2c%20-%201%29%2c%20%280%2c%20-%201%2c%20-2%2c%200%29%2c%20%28-%201%2c%20-%201%2c%20-%201%2c%200%29%2c%280%2c%200%2c%20-%201%2c%20-%201%29%2c%20%280%2c%20-%201%2c%20-%201%2c%200%29%2c%20%28-%201%2c%20-%201%2c%200%2c%200%29%2c%20%280%2c%200%2c%200%2c%20-%201%29%2c%20%280%2c%200%2c%20-%201%2c%200%29%2c%20%280%2c%20-%201%2c%200%2c%200%29%2c%20%28-%201%2c%200%2c%200%2c%200%29%2c%281%2c%200%2c%200%2c%200%29%2c%20%280%2c%201%2c%200%2c%200%29%2c%20%280%2c%200%2c%201%2c%200%29%2c%20%280%2c%200%2c%200%2c%201%29%2c%20%281%2c%201%2c%200%2c%200%29%2c%20%280%2c%201%2c%201%2c%200%29%2c%20%280%2c%200%2c%201%2c%201%29%2c%20%281%2c%201%2c%201%2c%200%29%2c%280%2c%201%2c%202%2c%200%29%2c%20%280%2c%201%2c%201%2c%201%29%2c%20%281%2c%201%2c%202%2c%200%29%2c%20%281%2c%201%2c%201%2c%201%29%2c%20%280%2c%201%2c%202%2c%201%29%2c%20%281%2c%202%2c%202%2c%200%29%2c%20%281%2c%201%2c%202%2c%201%29%2c%20%280%2c%201%2c%202%2c%202%29%2c%20%281%2c%202%2c%202%2c%201%29%2c%281%2c%201%2c%202%2c%202%29%2c%20%281%2c%202%2c%203%2c%201%29%2c%20%281%2c%202%2c%202%2c%202%29%2c%20%281%2c%202%2c%203%2c%202%29%2c%20%281%2c%202%2c%204%2c%202%29%2c%20%281%2c%203%2c%204%2c%202%29%2c%20%282%2c%203%2c%204%2c%202%29%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PerturbSplittingNormal{}((0,0,0,0), ((- 1, -2, -2, -2), (- 1, - 1, -2, -2), (0, - 1, -2, -2), (- 1, - 1, - 1, - 1), (0, - 1, - 1, - 1),(0, 0, - 1, - 1), (- 1, - 1, 0, 0), (0, - 1, 0, 0), (- 1, 0, 0, 0), (1, 0, 0, 0), (0, 1, 0, 0), (1, 1, 0, 0),(0, 0, 1, 1), (0, 1, 1, 1), (1, 1, 1, 1), (0, 1, 2, 2), (1, 1, 2, 2), (1, 2, 2, 2)),((-2, -3, -4, -2), (- 1, -3, -4, -2), (- 1, -2, -4, -2), (- 1, -2, -3, -2), (- 1, -2, -2, -2), (- 1, -2, -3, - 1),(- 1, - 1, -2, -2), (- 1, -2, -2, - 1), (0, - 1, -2, -2), (- 1, - 1, -2, - 1), (- 1, -2, -2, 0),(0, - 1, -2, - 1), (- 1, - 1, - 1, - 1), (- 1, - 1, -2, 0), (0, - 1, - 1, - 1), (0, - 1, -2, 0), (- 1, - 1, - 1, 0),(0, 0, - 1, - 1), (0, - 1, - 1, 0), (- 1, - 1, 0, 0), (0, 0, 0, - 1), (0, 0, - 1, 0), (0, - 1, 0, 0), (- 1, 0, 0, 0),(1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1), (1, 1, 0, 0), (0, 1, 1, 0), (0, 0, 1, 1), (1, 1, 1, 0),(0, 1, 2, 0), (0, 1, 1, 1), (1, 1, 2, 0), (1, 1, 1, 1), (0, 1, 2, 1), (1, 2, 2, 0), (1, 1, 2, 1), (0, 1, 2, 2), (1, 2, 2, 1),(1, 1, 2, 2), (1, 2, 3, 1), (1, 2, 2, 2), (1, 2, 3, 2), (1, 2, 4, 2), (1, 3, 4, 2), (2, 3, 4, 2)))
```
Takes 3 arguments: normal, cone and general vectors. Attempts to perturb the normal so that the cone stays on one side of the normal and so that the general vectors have as little zero scalar products with the normal as possible. 

*TestSpechtModules* [TestSpechtModules] {CalculatorFunctionsWeylGroup::testSpechtModules}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestSpechtModules%284%29%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestSpechtModules(4); 
```
Tests all Specht modules of S_n, where n is the only argument taken by the function.

*MakeElementWeylGroup* [MakeElementWeylGroup] {CalculatorFunctionsWeylGroup::weylGroupElement}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s_%7b%7bi%7d%7d%20%3d%20MakeElementWeylGroup%7b%7d%28A_2%2c%20i%29%3b%5cn%28s_1%20%2b%20s_2%29%282s_1%20%2b%20s_2%29%283s_1%20%2b%20s_2%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s_{{i}} = MakeElementWeylGroup{}(A_2, i);
(s_1 + s_2)(2s_1 + s_2)(3s_1 + s_2)
```
Needs a group name and a list of generators

*MakeElementHyperOctahedral* [MakeElementHyperOctahedral] {CalculatorConversions::makeElementHyperOctahedral}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s%20%3d%20MakeElementHyperOctahedral%7b%7d%28%281%2c%202%29%2c%201%2c%200%2c%200%29%3b%5cnt%20%3d%20MakeElementHyperOctahedral%7b%7d%28%281%2c%203%29%2c%200%2c%200%2c%200%29%3b%5cns%20%2a%20t%20%2a%20s%20%2a%20t%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0);
t = MakeElementHyperOctahedral{}((1, 3), 0, 0, 0);
s * t * s * t
```
Needs a group name and a list of generators

*WeylGroupClassicalSignMultiplicities* [WeylGroupClassicalSignMultiplicities] {CalculatorFunctionsWeylGroup::signSignatureRootSubsystemsFromKostkaNumbers}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupClassicalSignMultiplicities%7b%7d%28b_3%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28b_4%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28b_5%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28d_4%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28d_5%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28d_6%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28a_2%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28a_3%29%3b%5cnWeylGroupClassicalSignMultiplicities%7b%7d%28a_4%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupClassicalSignMultiplicities{}(b_3);
WeylGroupClassicalSignMultiplicities{}(b_4);
WeylGroupClassicalSignMultiplicities{}(b_5);
WeylGroupClassicalSignMultiplicities{}(d_4);
WeylGroupClassicalSignMultiplicities{}(d_5);
WeylGroupClassicalSignMultiplicities{}(d_6);
WeylGroupClassicalSignMultiplicities{}(a_2);
WeylGroupClassicalSignMultiplicities{}(a_3);
WeylGroupClassicalSignMultiplicities{}(a_4);

```
Prints the sign multiplicities of a simple Weyl group of classical type. See a common article by T. Folz-Donahue, S. Jackson, T. Milev, A. Noel. 

*WeylGroupTauSignatures* [WeylGroupTauSignatures] {CalculatorFunctionsWeylGroup::signSignatureRootSubsystems}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupTauSignatures%7b%7d%28b_3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupTauSignatures{}(b_3);
```
Prints the tau signatures of a Weyl group. See a common article by T. Folz-Donahue, S. Jackson, T. Milev, A. Noel. 

*WeylGroupConjugacyClassesFromAllElements* [WeylGroupConjugacyClassesFromAllElements] {CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupConjugacyClassesFromAllElements%7b%7d%28A_2%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupConjugacyClassesFromAllElements{}(A_2);
```
For small ranks, computes the conjugacy classes of a Weyl group by enumerating all elements of the group. 

*WeylGroupOuterConjugacyClassesFromAllElements* [WeylGroupOuterConjugacyClassesFromAllElements] {CalculatorFunctionsWeylGroup::weylGroupOuterConjugacyClassesFromAllElements}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupOuterConjugacyClassesFromAllElements%7b%7d%28D_4%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupOuterConjugacyClassesFromAllElements{}(D_4);
```
Computes conjugacy classes, identifying classes that are conjugate using outer automorphisms. 

*WeylGroupConjugacyClassesRepresentatives* [WeylGroupConjugacyClassesRepresentatives] {CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupConjugacyClassesRepresentatives%7b%7d%28A_2%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupConjugacyClassesRepresentatives{}(A_2);
```
Computes a representative in each conjugacy classes of a Weyl group by comparing conjugacy classes invariants and by enumerating conjugacy class orbits. 

*WeylGroupConjugacyClasses* [WeylGroupConjugacyClasses] {CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupConjugacyClasses%7b%7d%28f_4%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupConjugacyClasses{}(f_4);
```
Loads the conjugacy classes of a Weyl group (hard-coded), or computes them if rank<=6. 

*WeylGroupCharTable* [WeylGroupCharTable] {CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupCharTable%7b%7d%28b_3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupCharTable{}(b_3);
```
Loads the character table of a Weyl group. The character tables are hard-coded.

*WeylGroupIrrepsAndCharTableComputeFromScratch* [WeylGroupIrrepsAndCharTableComputeFromScratch] {CalculatorFunctionsWeylGroup::weylGroupIrrepsAndCharTableComputeFromScratch}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupIrrepsAndCharTableComputeFromScratch%7b%7d%28b_3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupIrrepsAndCharTableComputeFromScratch{}(b_3);
```
<b>Work in progress. Please do not use.</b> Computes from scratch the irreducible representations and the character table of a Weyl group.

*WeylOrbitSize* [WeylOrbitSize] {CalculatorFunctionsWeylGroup::weylGroupOrbitSize}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylOrbitSize%28E_6%2c%20%283%2c%203%2c%203%2c%207%2c%207%2c%2011%29%29%3b%5cnWeylOrbitSize%28E_8%2c%20%283%2c%203%2c%203%2c%207%2c%207%2c%2011%2c%2013%2c%2013%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylOrbitSize(E_6, (3, 3, 3, 7, 7, 11));
WeylOrbitSize(E_8, (3, 3, 3, 7, 7, 11, 13, 13));
```
Computes the size of a Weyl group orbit of a weight

*WeylOrbitSimpleCoords* [WeylOrbitSimpleCoords] {CalculatorFunctionsWeylGroup::weylGroupOrbitSimple}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylOrbitSimpleCoords%7b%7d%28B_2%2c%20%28y%2c%20y%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylOrbitSimpleCoords{}(B_2, (y, y));
```
Generates a Weyl orbit printout from simple coords. First argument = type. Second argument = weight in simple coords. The orbit size is cut off at max 1920 elements (type D_5).

*WeylGroupOrbitOuterSimple* [WeylGroupOrbitOuterSimple] {CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupOrbitOuterSimple%7b%7d%28D_4%2c%20%281%2c%200%2c%200%2c%200%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupOrbitOuterSimple{}(D_4, (1, 0, 0, 0))
```
Generates a Weyl outer orbit printout from simple coords. The outer orbit is the orbit under the Weyl group extended with the outer automoprhisms of the Weyl group. First argument = type. Second argument = weight in simple coords. The orbit size is cut off at 1921*2 elements. 

*WeylOrbitFundCoords* [WeylOrbitFundCoords] {CalculatorFunctionsWeylGroup::weylGroupOrbitFund}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylOrbitFundCoords%7b%7d%28B_2%2c%20%28y%2c%200%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylOrbitFundCoords{}(B_2, (y, 0));
```
Generates a Weyl orbit printout from fundamental coords. First argument = type. Second argument = weight in fundamental coords. 

*WeylOrbitFundRho* [WeylOrbitFundRho] {CalculatorFunctionsWeylGroup::weylGroupOrbitFundRho}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylOrbitFundRho%7b%7d%28B_2%2c%20%28y%2c%200%29%20%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylOrbitFundRho{}(B_2, (y, 0) )
```
Generates a Weyl orbit printout from fundamental coords. First argument = type. Second argument = weight in fundamental coords. Doing the rho-modified action. 

*WeylRaiseToMaximallyDominant* [WeylRaiseToMaximallyDominant] {CalculatorFunctionsWeylGroup::weylNoOuterRaiseToMaximallyDominant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylRaiseToMaximallyDominant%7b%7d%28F_4%2c%20%283%2c%206%2c%208%2c%204%29%2c%20%28-%201%2c%20-%203%2c%20-%202%2c%200%29%29%3b%5cnWeylRaiseToMaximallyDominant%7b%7d%28F_4%2c%20%283%2c%206%2c%208%2c%204%29%2c%20%280%2c%20-%203%2c%20-%204%2c%20-%202%29%29%3b%5cnWeylRaiseToMaximallyDominant%7b%7d%28F_4%2c%20%280%2c%20-%203%2c%20-%204%2c%20-%202%29%20%2c%20%283%2c%206%2c%208%2c%204%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylRaiseToMaximallyDominant{}(F_4, (3, 6, 8, 4), (- 1, - 3, - 2, 0));
WeylRaiseToMaximallyDominant{}(F_4, (3, 6, 8, 4), (0, - 3, - 4, - 2));
WeylRaiseToMaximallyDominant{}(F_4, (0, - 3, - 4, - 2) , (3, 6, 8, 4));
```
Raises a set of rational vectors simultaneously to maximally dominant: it raises the first vector to dominant, then each consecutive vector is raised as much as possible without changing the preceding, alrady raised,  vectors. First argument = type. Second argument = weight in simple coords. 

*MakeWeight* [MakeWeight] {CalculatorFunctionsWeylGroup::lieAlgebraWeight}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cvarepsilon_%7b%7ba%7d%7d%3dMakeWeight%7b%7d%28B_3%2c%20a%2c%20epsilon%29%3b%5cn%5c%5cpsi_%7b%7ba%7d%7d%3dMakeWeight%7b%7d%28B_3%2c%20a%2c%20fundamental%29%3b%5cn%5c%5ceta_%7b%7ba%7d%7d%3d%20MakeWeight%7b%7d%28B_3%2c%20a%2c%20simple%29%3b%5cn%5c%5cvarepsilon_1%3b%5cn%5c%5cpsi_1%3b%5cn%5c%5ceta_3%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon);
\psi_{{a}}=MakeWeight{}(B_3, a, fundamental);
\eta_{{a}}= MakeWeight{}(B_3, a, simple);
\varepsilon_1;
\psi_1;
\eta_3
```
Makes a weight. First argument = type. Second argument = coordinate. Third argument = one of the keywords epsilon, simple, fundamental, standing for the coordinate system. 

*MakeRho* [MakeRho] {CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5crho%20%3d%20MakeRho%28B_3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\rho = MakeRho(B_3);
```
Makes the half-sum of the positive roots.

*IsOuterAuto* [IsOuterAuto] {CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22A%20%3d%20MakeMatrix%28%281%2c%200%2c%200%2c%200%2c%200%2c%20-%201%29%2c%280%2c%200%2c%200%2c%200%2c%201%2c%20-%202%29%2c%280%2c%200%2c%201%2c%200%2c%200%2c%20-%202%29%2c%280%2c%200%2c%200%2c%201%2c%200%2c%20-%203%29%2c%280%2c%201%2c%200%2c%200%2c%200%2c%20-%202%29%2c%280%2c%200%2c%200%2c%200%2c%200%2c%20-%201%29%29%3bIsOuterAuto%7b%7d%28e_6%2c%20A%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
A = MakeMatrix((1, 0, 0, 0, 0, - 1),(0, 0, 0, 0, 1, - 2),(0, 0, 1, 0, 0, - 2),(0, 0, 0, 1, 0, - 3),(0, 1, 0, 0, 0, - 2),(0, 0, 0, 0, 0, - 1));IsOuterAuto{}(e_6, A);
```
Checks whether the element is outer automorphism of a root system. First argument = type. Second argument = matrix linear operator corresponding written in simple basis. 

*WeylOuterRaiseToMaximallyDominant* [WeylOuterRaiseToMaximallyDominant] {CalculatorFunctionsWeylGroup::weylOuterRaiseToMaximallyDominant}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylOuterRaiseToMaximallyDominant%7b%7d%28D_4%2c%20%281%2c%200%2c%200%2c%200%29%2c%20%280%2c%200%2c%200%2c%201%29%29%3b%5cnWeylOuterRaiseToMaximallyDominant%7b%7d%28D_4%2c%20%280%2c%200%2c%200%2c%201%29%2c%20%281%2c%200%2c%200%2c%200%29%29%3b%5cnWeylOuterRaiseToMaximallyDominant%7b%7d%28D_4%2c%20%281%2c%200%2c%200%2c%200%29%2c%20%280%2c%200%2c%201%2c%200%29%29%3b%5cnWeylOuterRaiseToMaximallyDominant%7b%7d%28D_4%2c%20%280%2c%200%2c%201%2c%200%29%2c%20%281%2c%200%2c%200%2c%200%29%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylOuterRaiseToMaximallyDominant{}(D_4, (1, 0, 0, 0), (0, 0, 0, 1));
WeylOuterRaiseToMaximallyDominant{}(D_4, (0, 0, 0, 1), (1, 0, 0, 0));
WeylOuterRaiseToMaximallyDominant{}(D_4, (1, 0, 0, 0), (0, 0, 1, 0));
WeylOuterRaiseToMaximallyDominant{}(D_4, (0, 0, 1, 0), (1, 0, 0, 0));

```
Same as WeylRaiseToMaximallyDominant but uses outer Weyl group automorphisms as well.

*GenerateFiniteMultiplicativelyClosedSet* [GenerateFiniteMultiplicativelyClosedSet] {Calculator::generateMultiplicativelyClosedSet}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22s_1%3dMakeMatrix%7b%7d%28%28-%201%2c%201%2c%200%29%2c%20%280%2c%201%2c%200%29%2c%20%280%2c%200%2c%201%29%29%3b%5cns_2%3dMakeMatrix%7b%7d%28%281%2c%200%2c%200%29%2c%20%281%2c%20-%201%2c%201%29%2c%20%280%2c%200%2c%201%29%29%3b%5cns_3%3dMakeMatrix%7b%7d%28%281%2c%200%2c%200%29%2c%20%280%2c%201%2c%200%29%2c%20%280%2c%202%2c%20-%201%29%29%3b%5cnGenerateFiniteMultiplicativelyClosedSet%7b%7d%2848%2c%20s_1%2c%20s_2%2c%20s_3%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
s_1=MakeMatrix{}((- 1, 1, 0), (0, 1, 0), (0, 0, 1));
s_2=MakeMatrix{}((1, 0, 0), (1, - 1, 1), (0, 0, 1));
s_3=MakeMatrix{}((1, 0, 0), (0, 1, 0), (0, 2, - 1));
GenerateFiniteMultiplicativelyClosedSet{}(48, s_1, s_2, s_3);
```
The first argument gives upper_bound to the number of elements of the set. Generates a finite multiplicatively closed set of at most upper_bound elements, or returns error indicating the multiplicatively closed set is larger than the upper bound.

*CartanSA* [CartanSA] {CalculatorFunctions::constructCartanSubalgebra}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22g_%7b%7bi%7d%7d%20%3d%20ChevalleyGenerator%7b%7d%28b_3%2c%20i%29%3b%5cnh_%7b%7bi%7d%7d%20%3d%20CartanGenerator%7b%7d%28b_3%2c%20i%29%3b%5cnCartanSA%28g_1%20%2b%20g_3%2c%20g_-1%20%2b%20g_-3%2c%20g_2%2c%20g_-2%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
g_{{i}} = ChevalleyGenerator{}(b_3, i);
h_{{i}} = CartanGenerator{}(b_3, i);
CartanSA(g_1 + g_3, g_-1 + g_-3, g_2, g_-2);
```
Generates a Cartan subalgebra, code is still experimental.

*GenerateVectorSpaceClosedWithRespectToLieBracket* [GenerateVectorSpaceClosedWithRespectToLieBracket] {CalculatorLieTheory::generateVectorSpaceClosedWithRespectToLieBracket}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22%5c%5cpartial_%7b%7bi%7d%7d%20%3d%20ElementWeylAlgebraDO%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%5cnx_%7b%7bi%7d%7d%20%3d%20ElementWeylAlgebraPoly%7b%7d%28%5c%5cpartial_i%2c%20x_i%29%3b%20g_1%20%3d%20%28-%20x_%7b5%7d%20%5c%5cpartial_%7b5%7d%5e%7b2%7d%20-%20x_%7b4%7d%20%5c%5cpartial_%7b4%7d%20%5c%5cpartial_%7b5%7d%20%20-%20x_%7b3%7d%20%5c%5cpartial_%7b3%7d%20%5c%5cpartial_%7b5%7d%20-%20x_%7b2%7d%20%5c%5cpartial_%7b2%7d%20%5c%5cpartial_%7b5%7d%20%2b%20x_%7b1%7d%20%5c%5cpartial_%7b2%7d%20%5c%5cpartial_%7b4%7d%20%2bx_%7b1%7d%20%5c%5cpartial_%7b3%7d%5e%7b2%7d%29%3b%5cng_-%201%20%3d%20x_5%3b%5cng_2%20%3d%20x_%7b5%7d%20%5c%5cpartial_%7b4%7d%20-%20x_%7b2%7d%20%5c%5cpartial_%7b1%7d%3b%5cng_-2%20%3d%20%28x_%7b4%7d%20%5c%5cpartial_%7b5%7d%20-%20x_%7b1%7d%20%5c%5cpartial_%7b2%7d%29%3b%5cng_3%20%3d%20%282x_%7b4%7d%20%5c%5cpartial_%7b3%7d%20-%20x_%7b3%7d%20%5c%5cpartial_%7b2%7d%29%3b%5cng_-3%20%3d%20%28x_%7b3%7d%20%5c%5cpartial_%7b4%7d%20-%202x_%7b2%7d%20%5c%5cpartial_%7b3%7d%29%3b%5cnGenerateVectorSpaceClosedWithRespectToLieBracket%7b%7d%2850%2c%20g_1%2c%20g_-%201%2c%20g_2%2c%20g_-2%2c%20g_3%2c%20g_-3%29%3b%5cnGenerateVectorSpaceClosedWithRespectToLieBracket%7b%7d%2850%2c%20g_1%2c%20g_2%2c%20g_-2%2c%20g_3%2c%20g_-3%29%3b%5cnGenerateVectorSpaceClosedWithRespectToLieBracket%7b%7d%2850%2c%20g_1%2c%20g_-%201%2c%20g_2%2c%20g_-2%2c%20g_3%29%3b%5cnq_%7b%7bi%7d%7d%20%3d%20ChevalleyGenerator%7b%7d%28F_4%2c%20i%29%3b%5cns_2%20%3d%20-%20q_%7b-%205%7d%20-%20q_%7b-6%7d%3b%5cns_1%20%3d%20q_%7b20%7d%20%2b%20q_%7b19%7d%3b%5cns_-%201%20%3d%20-%20q_%7b-%2019%7d%20-%20q_%7b-%2020%7d%3b%5cns_-2%20%3d%202q_%7b6%7d%20%2b%202q_%7b5%7d%3b%5cnGenerateVectorSpaceClosedWithRespectToLieBracket%2852%2c%20s_1%2c%20s_2%2c%20s_-%201%2c%20s_-2%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
\partial_{{i}} = ElementWeylAlgebraDO{}(\partial_i, x_i);
x_{{i}} = ElementWeylAlgebraPoly{}(\partial_i, x_i); g_1 = (- x_{5} \partial_{5}^{2} - x_{4} \partial_{4} \partial_{5}  - x_{3} \partial_{3} \partial_{5} - x_{2} \partial_{2} \partial_{5} + x_{1} \partial_{2} \partial_{4} +x_{1} \partial_{3}^{2});
g_- 1 = x_5;
g_2 = x_{5} \partial_{4} - x_{2} \partial_{1};
g_-2 = (x_{4} \partial_{5} - x_{1} \partial_{2});
g_3 = (2x_{4} \partial_{3} - x_{3} \partial_{2});
g_-3 = (x_{3} \partial_{4} - 2x_{2} \partial_{3});
GenerateVectorSpaceClosedWithRespectToLieBracket{}(50, g_1, g_- 1, g_2, g_-2, g_3, g_-3);
GenerateVectorSpaceClosedWithRespectToLieBracket{}(50, g_1, g_2, g_-2, g_3, g_-3);
GenerateVectorSpaceClosedWithRespectToLieBracket{}(50, g_1, g_- 1, g_2, g_-2, g_3);
q_{{i}} = ChevalleyGenerator{}(F_4, i);
s_2 = - q_{- 5} - q_{-6};
s_1 = q_{20} + q_{19};
s_- 1 = - q_{- 19} - q_{- 20};
s_-2 = 2q_{6} + 2q_{5};
GenerateVectorSpaceClosedWithRespectToLieBracket(52, s_1, s_2, s_- 1, s_-2);
```
Generates a vector space closed with respect to the Lie bracket from input of type elements of Weyl algebra or Lie algebra. The first argument of the input is an upper bound for the dimension of the vector space. The remaining arguments must be differential operators. The output is a vector space basis printout. Fails if the dimension of the vector space is larger than the upper bound.

*WeylGroupNaturalRep* [WeylGroupNaturalRep] {CalculatorFunctionsWeylGroup::weylGroupNaturalRep}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupNaturalRep%7b%7d%28B_3%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupNaturalRep{}(B_3)
```
Gets the natural representation of the Weyl group.

*WeylGroupGetOuterAutoGenerators* [WeylGroupGetOuterAutoGenerators] {CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22WeylGroupGetOuterAutoGenerators%7b%7d%282D_4%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
WeylGroupGetOuterAutoGenerators{}(2D_4)
```
Gets the generators of the outer automorphism group (the group generated by the graph automorphisms of the Dynkin diagram.

*MakeVirtualWeylGroupRepresentation* [MakeVirtualWeylGroupRepresentation] {CalculatorFunctionsWeylGroup::makeVirtualWeylRep}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeVirtualWeylGroupRepresentation%7b%7d%28WeylGroupNaturalRep%7b%7d%28B_3%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeVirtualWeylGroupRepresentation{}(WeylGroupNaturalRep{}(B_3))
```
Convert a representation into virtual one.

*RepresentElementHyperoctahedral* [RepresentElementHyperoctahedral] {CalculatorFunctionsWeylGroup::representElementHyperOctahedral}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22V%20%3d%20HyperOctahedralRepresentation%28%281%2c%201%29%2c%201%29%3b%5cns%20%3d%20MakeElementHyperOctahedral%7b%7d%28%281%2c%202%29%2c%201%2c%200%2c%200%29%3b%5cnRepresentElementHyperoctahedral%28s%2c%20V%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
V = HyperOctahedralRepresentation((1, 1), 1);
s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0);
RepresentElementHyperoctahedral(s, V)
```
Represents element of hyperoctahedral into a representation. 

*HyperOctahedralIrreps* [HyperOctahedralIrreps] {CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules}. (admin only) (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22HyperOctahedralIrreps%283%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
HyperOctahedralIrreps(3)
```
Prints all modules induced from Specht modules. 

*SpechtModule* [SpechtModule] {CalculatorFunctionsWeylGroup::spechtModule}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22SpechtModule%28%283%2c%202%2c%201%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
SpechtModule((3, 2, 1))
```
Gets the Specht module of the partition. 

*HyperOctahedralRepresentation* [HyperOctahedralRepresentation] {CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules}. (admin only) (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22HyperOctahedralRepresentation%28%281%2c%201%29%2c%20%281%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
HyperOctahedralRepresentation((1, 1), (1))
```
Gets one hyperoctahedral representation from two partitions. 

*HyperOctahedralGeneratorPrint* [HyperOctahedralGeneratorPrint] {CalculatorFunctionsWeylGroup::hyperOctahedralPrintGeneratorCommutationRelations}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22HyperOctahedralGeneratorPrint%283%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
HyperOctahedralGeneratorPrint(3)
```
Prints the generator commutation relations of a hyperoctahedral group. 

*PrincipalSlTwoIndex* [PrincipalSlTwoIndex] {CalculatorFunctions::getPrincipalSl2Index}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PrincipalSlTwoIndex%28G_2%5e3%20%2b%20D_4%5e2%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PrincipalSlTwoIndex(G_2^3 + D_4^2);
```
Gives the symmetric Cartan corresponding to a given dynkin type.

*DynkinIndicesSlTwoSubalgebras* [DynkinIndicesSlTwoSubalgebras] {CalculatorFunctions::getDynkinIndicesSlTwoSubalgebras}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22DynkinIndicesSlTwoSubalgebras%282A_2%29%3b%5cn%20DynkinIndicesSlTwoSubalgebras%282A_2%2bA_2%5e2%29%3b%5cnDynkinIndicesSlTwoSubalgebras%282A_2%2bA_2%5e7%29%3b%5cnDynkinIndicesSlTwoSubalgebras%282G_2%2bG_2%5e7%2bF_4%2bF_4%5e19%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
DynkinIndicesSlTwoSubalgebras(2A_2);
 DynkinIndicesSlTwoSubalgebras(2A_2+A_2^2);
DynkinIndicesSlTwoSubalgebras(2A_2+A_2^7);
DynkinIndicesSlTwoSubalgebras(2G_2+G_2^7+F_4+F_4^19);
```
Fetches the absolute Dynkin indices of the sl(2) subalgebras of the input Dynkin type. The indices of each simple component are computed by actually constructing the sl(2)-subalgebras; clearly this can be greatly optimized but we postpone that for a future version. 

*CartanSymmetric* [CartanSymmetric] {CalculatorFunctions::getSymmetricCartan}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CartanSymmetric%28e%5e7_6%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CartanSymmetric(e^7_6);
```
Gives the symmetric Cartan corresponding to a given dynkin type.

*GrowDynkinType* [GrowDynkinType] {CalculatorFunctions::growDynkinType}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22GrowDynkinType%28A%5e30_1%2bd%5e30_4%2c%20e_6%29%3b%5cnGrowDynkinType%28g%5e35_2%2bB%5e30_2%2c%20e_6%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
GrowDynkinType(A^30_1+d^30_4, e_6);
GrowDynkinType(g^35_2+B^30_2, e_6);
```
This is a calculator testing function. Grows a Dynkin type inside an ambient Dynkin type. 

*LoadFileIntoString* [LoadFileIntoString] {CalculatorConversions::loadFileIntoString}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LoadFileIntoString%28%5c%22test%2fcertificate_self_signed.base64%5c%22%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LoadFileIntoString("test/certificate_self_signed.base64");

```
Loads a file into a string. The file must be given its relative file name displayed when browsing the web server. There are two exceptions. 1) The file can be located in a folder in the project base that is otherwise not visible by the webserver **provided that** the folder is white-listed via the FileOperations class within the C++ source. Example: folder problems/ is white-listed. To access this file simply start your file name with the foldername. Do not start the folder name with the / character.2) The file can be located in a folder in a level parallel to the project base - such folders are outside of the project folder - **provided that** the folder is white listed in the C++ code. 

*InterpretProblem* [InterpretProblem] {CalculatorHtmlFunctions::interpretProblem}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22InterpretProblem%28LoadFileIntoString%28%5c%22test%2fproblems%2ffunctions_composing_fractional_linear_1.html%5c%22%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
InterpretProblem(LoadFileIntoString("test/problems/functions_composing_fractional_linear_1.html"))
```
Does as ExtractCalculatorExpressionFromHtml but in addition interprets the calculator commands. 

*ProblemGiveUp* [ProblemGiveUp] {CalculatorHtmlFunctions::interpretProblemGiveUp}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ProblemGiveUp%28%5cn%5c%22test%2fproblems%2ffunctions_composing_fractional_linear_1.html%5c%22%2c%5cn%5c%22AlgebraAnswer%5c%22%2c%5cn%5c%22123%5c%22%5cn%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ProblemGiveUp(
"test/problems/functions_composing_fractional_linear_1.html",
"AlgebraAnswer",
"123"
)
```
Gives the predefined answer to a problem. First argument must be a string with the problem. The second argument must be the id of the answer. Third argument must be the random seed integer.

*ExtractCalculatorExpressionFromHtml* [ExtractCalculatorExpressionFromHtml] {CalculatorHtmlFunctions::extractCalculatorExpressionFromHtml}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22ExtractCalculatorExpressionFromHtml%28LoadFileIntoString%28%5c%22test%2fproblems%2ffunctions_composing_fractional_linear_1.html%5c%22%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
ExtractCalculatorExpressionFromHtml(LoadFileIntoString("test/problems/functions_composing_fractional_linear_1.html"))
```
Reads html and extracts embedded calculator commands. Content enclosed in spans with appropriate class names is interpreted; all other content is copied without any change. At the moment of writing, the planned span class names are: "calculator", "calculatorHidden", "calculatorAnswer".

*TestCalculatorIndicator* [TestCalculatorIndicator] {CalculatorFunctions::testIndicator}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestCalculatorIndicator%281000%2c%20200000%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestCalculatorIndicator(1000, 200000)
```
(This is not a mathematical function). Tests the calculator indicator mechanism.First argument times number of iterations. Second argument = length of dummy comment appended to the calculator comments. Use a large dummy comment to test that large outputs are piped correctly to the monitoring process. 

*TestTopCommand* [TestTopCommand] {CalculatorFunctions::testTopCommand}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestTopCommand%281000%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestTopCommand(1000)
```
(This is not a mathematical function). Tests the top linux command. 

*Crash* [Crash] {CalculatorFunctions::crash}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Crash%280%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Crash(0)
```
Crashes the calculator: tests the crashing mechanism (are crash logs properly created, etc.). 

*CrashListOutOfBounds* [CrashListOutOfBounds] {CalculatorFunctions::crashByListOutOfBounds}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CrashListOutOfBounds%280%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CrashListOutOfBounds(0)
```
Crashes the calculator by attempting to use data out-of-bounds in a List data structure. 

*CrashVectorOutOfBounds* [CrashVectorOutOfBounds] {CalculatorFunctions::crashByVectorOutOfBounds}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22CrashVectorOutOfBounds%280%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
CrashVectorOutOfBounds(0)
```
Crashes the calculator by attempting to use data out-of-bounds in a std::vector.

*PlotExpressionTree* [PlotExpressionTree] {CalculatorFunctionsPlot::drawExpressionGraph}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotExpressionTree%28e%5ex%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotExpressionTree(e^x)
```
Draws the internal tree structure of an expression. Does not unfold built-in types.

*PlotExpressionTreeFull* [PlotExpressionTreeFull] {CalculatorFunctionsPlot::drawExpressionGraphFull}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22PlotExpressionTreeFull%281%29%3b%5cnPlotExpressionTree%281%20%2b%201%29%3b%5cnPlotExpressionTree%28Freeze%7b%7d%281%20%2b%201%29%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
PlotExpressionTreeFull(1);
PlotExpressionTree(1 + 1);
PlotExpressionTree(Freeze{}(1 + 1));
```
Draws the internal tree structure of an expression. Unfolds built-in types. 

*Lispify* [Lispify] {CalculatorFunctions::lispify}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Lispify%28e%5ex%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Lispify(e^x)
```
Shows the internal tree structure of an expression, without completely unfolding the tree structure of expressions that represent a single mathematical entity.

*LispifyFull* [LispifyFull] {CalculatorFunctions::lispifyFull}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22LispifyFull%28e%5ex%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
LispifyFull(e^x)
```
Shows the complete internal tree structure of an expression (replacing the expression with a string).

*TestBase64* [TestBase64] {CalculatorFunctionsEncoding::base64ToCharToBase64Test}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestBase64%28%5c%22TheQuickBrownFoxJumpsOverTheLazyDog%3d%5c%22%29%3b%5cnTestBase64%28%5c%22TheQuickBrownFoxJumpsOverTheLazyDog%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestBase64("TheQuickBrownFoxJumpsOverTheLazyDog=");
TestBase64("TheQuickBrownFoxJumpsOverTheLazyDog")
```
Test function: converts a base64 string to bitstream and back to base64. Output must be identical to input. 

*StringDifference* [StringDifference] {CalculatorFunctions::stringDifference}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22StringDifference%28%5c%22XMJYAUZ%5c%22%2c%5c%22MZJAWXU%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
StringDifference("XMJYAUZ","MZJAWXU")
```
Computes the difference between two strings, provided that the strings are small enough. The algorithm consumes O(leftStringSize * rightStringSize) RAM. For more information, see https://en.wikipedia.org/wiki/Longest_common_subsequence_problem.

*TestJSON* [TestJSON] {CalculatorFunctionsEncoding::testJSON}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestJSON%28%5c%22%7ba%3a1%2c%20b%3a%20false%2c%7d%5c%22%29%3b%5cnTestJSON%28%5c%22%7b%5c%5c%5c%22a%5c%5c%5c%22%3a%5c%5c%5c%22%5c%5c%5c%5c%5c%5c%5c%22%5c%5c%5c%22%7d%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestJSON("{a:1, b: false,}");
TestJSON("{\"a\":\"\\\"\"}");
```
Tests the JSON parsing mechanism. Input: json string, use backslash escapes for backslashes and quotes.

*FormatCPPCode* [FormatCPPCode] {CalculatorFunctions::formatCPPSourceCode}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FormatCPPCode%7b%7d%28%5c%22src%2fsource_code_formatter.cpp%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FormatCPPCode{}("src/source_code_formatter.cpp")
```
Format cpp code. 

*FormatCPPCodeDebug* [FormatCPPCodeDebug] {CalculatorFunctions::formatCPPSourceCodeDebug}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FormatCPPCodeDebug%7b%7d%28%5c%22src%2fsource_code_formatter.cpp%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FormatCPPCodeDebug{}("src/source_code_formatter.cpp")
```
Format cpp code, debug version that shows the code formatter internals.

*FormatCPPDirectory* [FormatCPPDirectory] {CalculatorFunctions::formatCPPDirectory}. (invisible) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22FormatCPPDirectory%7b%7d%28%5c%22src%2f%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
FormatCPPDirectory{}("src/")
```
Format cpp directory. 

*TestCalculatorAll* [TestCalculatorAll] {Calculator::automatedTest}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestCalculatorAll%7b%7d%280%2c%200%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestCalculatorAll{}(0, 0)
```
Runs an automated test of built in functions against a set of known good results. First argument = index of first test to run (0 = run all).If positive, the second argument gives the number of tests to run. Else, when the second argument is negative or zero, we run all tests. 

*TestTopicListProblems* [TestTopicListProblems] {CalculatorFunctions::testTopicListProblems}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestTopicListProblems%200%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestTopicListProblems 0
```
Tests whether all files given in the default topic lists open properly. 

*TestProblemInterpretation* [TestProblemInterpretation] {CalculatorFunctions::testProblemInterpretation}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22TestProblemInterpretation%7b%7d%280%2c%200%2c%200%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
TestProblemInterpretation{}(0, 0, 0)
```
Runs an automated test of built in problems located in the problems/default/ folder. First argument = index of first problem to test. Second argument = number of problems to test. Third argument = starting random seed. To test a particular problem, set the index of the first problem to that of your problem, and set the number of problems to test to 1. To find out the index of your problem, run the entire test array once and the index of each problem file will be listed in the output. Until first error is seen, every test will be run a number of times, 3 at the time of writing. The random seed will be incremented for each run. 

*PrintRuleStack* [PrintRuleStack] {CalculatorFunctions::printRuleStack}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22a%20%3d%20b%3b%5cnPrintRuleStack%7b%7d0%3b%5cnc%20%3d%20d%3b%20PrintRuleStack%7b%7d0%3b%20%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
a = b;
PrintRuleStack{}0;
c = d; PrintRuleStack{}0; 
```
Prints the current rule stack

*Crawl* [Crawl] {CalculatorFunctionsFreecalc::crawlTexFile}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22Crawl%28%5c%22freecalc%2fhomework%2freferenceallproblemsbycourse%2fcalculusimasterproblemsheet.tex%5c%22%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
Crawl("freecalc/homework/referenceallproblemsbycourse/calculusimasterproblemsheet.tex")
```
Crawls a latex file collecting all local style files and all \input commands to produce a single latex file. The function was originally designed for the purposes of the freecalc project as not been tested on any other projects, please use only for freecalc.

*BuildFreecalc* [BuildFreecalc] {CalculatorFunctionsFreecalc::buildFreecalc}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22BuildFreecalc%28%5c%22freecalc%2flectures%2freferencelectures%2fprecalculus.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2flectures%2freferencelectures%2fcalculusi.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2flectures%2freferencelectures%2fcalculusii.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2flectures%2freferencelectures%2fcalculusiiimultivariable.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2fhomework%2freferenceallproblemsbycourse%2fprecalculus.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2fhomework%2freferenceallproblemsbycourse%2fcalculusi.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2fhomework%2freferenceallproblemsbycourse%2fcalculusii.tex%5c%22%29%3b%5cnBuildFreecalc%28%5c%22freecalc%2fhomework%2freferenceallproblemsbycourse%2fcalculusiii.tex%5c%22%29%3b%5cn%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
BuildFreecalc("freecalc/lectures/referencelectures/precalculus.tex");
BuildFreecalc("freecalc/lectures/referencelectures/calculusi.tex");
BuildFreecalc("freecalc/lectures/referencelectures/calculusii.tex");
BuildFreecalc("freecalc/lectures/referencelectures/calculusiiimultivariable.tex");
BuildFreecalc("freecalc/homework/referenceallproblemsbycourse/precalculus.tex");
BuildFreecalc("freecalc/homework/referenceallproblemsbycourse/calculusi.tex");
BuildFreecalc("freecalc/homework/referenceallproblemsbycourse/calculusii.tex");
BuildFreecalc("freecalc/homework/referenceallproblemsbycourse/calculusiii.tex");

```
Builds the freecalc lecture pdfs. This function performs file operations and is allowed only to logged-in administrator accounts. Takes as arguments the lecture folder (within the freecalc project) and the file name. The function assumes the freecalc project is installed in a folder called freecalc, parallel to the calculator project folder (i.e, we have folders /vectorpartition and /freecalc next to one another). The folders are given relative to the /freecalc base.

*BuildFreecalcWithSlides* [BuildFreecalcWithSlides] {CalculatorFunctionsFreecalc::BuildFreecalcWithSlides}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22BuildFreecalcWithSlides%28%5c%22freecalc%2fhomework%2freferenceallproblemsbycourse%2fcalculusi.tex%5c%22%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
BuildFreecalcWithSlides("freecalc/homework/referenceallproblemsbycourse/calculusi.tex");
```
Same as BuildFreeCalc but attempts to also build individual slides. 

*BuildSlidesInTopicList* [BuildSlidesInTopicList] {CalculatorFunctionsFreecalc::buildFreecalcSlidesOnTopic}. (admin only) 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22BuildSlidesInTopicList%20%280%29%3b%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
BuildSlidesInTopicList (0);
```
Builds all slides in the topic list. Available to logged-in admins only. Use the topic number as input, or 0 for the current topic. 

*SuffixNotationForPostScript* [SuffixNotationForPostScript] {CalculatorFunctions::suffixNotationForPostScript}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22suffixNotationForPostScript%7b%7d%28%281%2f3%20%2b%20a%20%2b%20b%29%20%2a%20c%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
suffixNotationForPostScript{}((1/3 + a + b) * c)
```
Suffix notation. for postscript, used to quickly generate pstricks drawings in LaTeX.

*MakeJavascriptExpression* [MakeJavascriptExpression] {CalculatorFunctionsPlot::makeJavascriptExpression}. 
[Example](https://calculator-algebra.org/app#%7b%22calculatorInput%22%3a%22MakeJavascriptExpression%28a%28b%2bc%29%29%22%2c%22currentPage%22%3a%22calculator%22%7d)
```
MakeJavascriptExpression(a(b+c))
```
Attempts to construct a javascript translation of the input. If not successful leaves the expression unchanged.
