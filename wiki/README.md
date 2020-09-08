# The calculator substitution engine

## Examples first please!

### Arithmetics
```
1+1
```
[1+1](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%221%2B1%22%7D)

Default computations are over the rational numbers.
```
1+2-3*4/5^{6}
```
[1+2-3*4/5^{6}](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%221%2B2-3*4%2F5%5E%7B6%7D%22%7D)

Large number arithmetic works without approximations.
```
101!/(101!+1)
```
[101!/(101!+1)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22101!%20%2F%20(101!%2B1)%22%7D)

Floating point conversions need to be invoked explicitly.
```
DoubleValue( 100! / (101!+1))
```
[DoubleValue( 100! / (101!+1))](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22DoubleValue(%20100!%20%2F%20(101!%2B1))%22%7D)

... but as usual, careful with approximations.
```
DoubleValue( 1000! / (1001!+1))
```
[DoubleValue( 1000! / (1001!+1))](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22DoubleValue(%201000!%20%2F%20(1001!%2B1))%22%7D)

### Algebra
Basic symbolic computation.
```
(x-1)(x+2)(x-3)(x+4)
```
[(x-1)(x+2)(x-3)(x+4)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22(x-1)(x%2B2)(x-3)(x%2B4)%22%7D).

Rational functions.
```
(x-1)/(x-2)+(x-2)/(x-3)+(x-3)/(x-1)
```
[(x-1)/(x-2)+(x-2)/(x-3)+(x-3)/(x-1)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22(x-1)%2F(x-2)%20%2B%20(x-2)%2F(x-3)%20%2B%20(x-3)%2F(x-1)%20%22%7D)

### Computations with radicals
Standard radical operations.
```
sqrt(24)- sqrt(96);
\sqrt[3]{27}
```
[sqrt(24)- sqrt(96);\sqrt[3]{27}](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22sqrt(24)-%20sqrt(96)%3B%5Cn%5C%5Csqrt%5B3%5D%7B27%7D%22%2C%22monitoring%22%3A%22true%22%7D)

Radical rationalization.
```
1/(1+sqrt(2)+sqrt(3)+sqrt(5)+sqrt(7))
```
[1/(1+sqrt(2)+sqrt(3)+sqrt(5)+sqrt(7))](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%221%2F(1%2Bsqrt(2)%2Bsqrt(3)%2Bsqrt(5)%2Bsqrt(7))%22%7D)

No special treatment of sqrt(-1).
```
1/(1+sqrt(-1)+sqrt(2))
```

[1/(1+sqrt(-1)+sqrt(2))](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%221%2F(1%2Bsqrt(-1)%2Bsqrt(2))%22%7D)

### Trigonometry, inverse trig, logarithms

Trig works with algebraic numbers if available; pi is built-in.
```
\sin (pi/3);
\cos (\pi/5);
```

[\sin (pi/3); \cos (\pi/5);](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22%5C%5Csin%20(pi%2F3)%3B%5Cn%5C%5Ccos%20(%5C%5Cpi%2F5)%3B%22%2C%22monitoring%22%3A%22true%22%7D)

Degrees work as expectedzz.
```
\tan 30^\circ
```
[\tan 30^\circ](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22%5C%5Ctan%2030%5E%5C%5Ccirc%22%2C%22monitoring%22%3A%22true%22%2C%22inputFocus%22%3Atrue%7D)

Inverse trigonometry and logarithms evaluate as expected.
```
\ln 12;
\arctan 13;
```
[\ln 12;\arctan 13;](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22%5C%5Cln%2012%3B%5Cn%5C%5Carctan%2013%3B%22%2C%22monitoring%22%3A%22true%22%7D)

Logarithms are clever.
```
log_{8} 128
```
[log_{8} 128](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22log_%7B8%7D%20128%22%2C%22monitoring%22%3A%22true%22%7D)
### Computations with matrices
Use LaTeX syntax to enter matrices. All basic operations work.
```
A=\begin{pmatrix}1&2\\ 3&4\end{pmatrix}; 
A+A; 
A-A; 
2*A; 
A^5; 
A^t;
A^-1; 
A \otimes A; 
A \oplus A; 
\det A
```
[A=\begin{pmatrix}1&2\\ 3&4\end{pmatrix}; A+A; A-A; 2*A; A^5; A^t; A^-1; A \otimes A; A \oplus A; \det A](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22A%3D%5C%5Cbegin%7Bpmatrix%7D1%262%5C%5C%5C%5C%203%264%5C%5Cend%7Bpmatrix%7D%3B%5CnA%2BA%3B%5CnA-A%3B%5Cn2*A%3B%5CnA%5E5%3B%5CnA%5Et%3B%5CnA%5E-1%3B%5CnA%20%5C%5Cotimes%20A%3B%5CnA%20%5C%5Coplus%20A%3B%5Cn%5C%5Cdet%20A%22%2C%22monitoring%22%3A%22true%22%7D)


Works as well with radicals.
```
A=\begin{pmatrix}sqrt(1)&sqrt(2)\\ sqrt(3)&sqrt(4)\end{pmatrix}; 
A+A; 
A-A; 
2*A; 
A^5; 
A^-1;
```
[A=\begin{pmatrix}sqrt(1)&sqrt(2)\\ sqrt(3)&sqrt(4)\end{pmatrix}; A+A; A-A; 2*A; A^5; A^-1;](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22A%3D%5C%5Cbegin%7Bpmatrix%7Dsqrt(1)%26sqrt(2)%5C%5C%5C%5C%20sqrt(3)%26sqrt(4)%5C%5Cend%7Bpmatrix%7D%3B%5CnA%2BA%3B%5CnA-A%3B%5Cn2*A%3B%5CnA%5E5%3B%5CnA%5E-1%3B%5Cn%5Cn%20%22%7D)

### Define your own function
Define a two-variable function and evaluate it. Evaluation/substitution need not be numerical. {} Syntax is used to denote function application - carefully chosen so it has no visible artifacts in LaTeX (recall that in math texts, function application vs multiplication is implied from context, not from the notation).
```
f{}({{a}}, {{b}}) = 1/(a+b); 
f{}(2,3); 
f{}(x+3, 1/(x^2-1))
```
[f{}({{a}}, {{b}}) = 1/(a+b); f{}(2,3); f{}(x+3, 1/(x^2-1))](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22f%7B%7D(%7B%7Ba%7D%7D%2C%20%7B%7Bb%7D%7D)%20%3D%201%2F(a%2Bb)%3B%20f%7B%7D(2%2C3)%3B%20f%7B%7D(x%2B3%2C%201%2F(x%5E2-1))%22%7D)

Make a matrix from a two-variable function.
```
f{}({{a}}, {{b}}) = 1/(a+b);
A= FunctionToMatrix(f,10,10);
\det A
```
[f{}({{a}}, {{b}}) = 1/(a+b);A= FunctionToMatrix(f,10,10);\det A](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22f%7B%7D(%7B%7Ba%7D%7D%2C%20%7B%7Bb%7D%7D)%20%3D%201%2F(a%2Bb)%3B%5CnA%3D%20FunctionToMatrix(f%2C10%2C10)%3B%5Cn%5C%5Cdet%20A%22%7D)

Recursive function just work. Here's the Fibonacci sequence.
```
Fib{}0=1; 
Fib{}1=1; 
Fib{}{{n}} = Fib{}(n-1)+Fib{}(n-2);
Fib{}49; 
Fib{}50; 
g{}{{n}} = 1/sqrt(5) (((1+sqrt(5))/2)^n - ((1- sqrt(5))/2)^n); 
g{}50; 
g{}51;
```
[Fib{}0=1; Fib{}1=1; Fib{}{{n}} = Fib{}(n-1)+Fib{}(n-2);Fib{}49; Fib{}50; g{}{{n}} = 1/sqrt(5) (((1+sqrt(5))/2)^n - ((1- sqrt(5))/2)^n); g{}50; g{}51;](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22Fib%7B%7D0%3D1%3B%20Fib%7B%7D1%3D1%3B%20Fib%7B%7D%7B%7Bn%7D%7D%20%3D%20Fib%7B%7D(n-1)%2BFib%7B%7D(n-2)%3BFib%7B%7D49%3B%20Fib%7B%7D50%3B%5Cng%7B%7D%7B%7Bn%7D%7D%20%3D%201%2F%20sqrt(5)%20(((1%2B%20sqrt(5))%20%2F2%20)%5En%20-%20((1-%20sqrt(5))%20%2F2%20)%5En)%3B%5Cng%7B%7D50%3B%5Cng%7B%7D51%3B%5Cn%22%7D)

Notice the engine does not discard previously computed values of the `Fib` function defined above, and is thus able to compute the expression efficiently.
```
Fib{}0=1; 
Fib{}1=1; 
Fib{}{{n}} = Fib{}(n-1)+Fib{}(n-2);
LogEvaluationSteps(Fib{}10)
```
[Fib{}0=1; Fib{}1=1; Fib{}{{n}} = Fib{}(n-1)+Fib{}(n-2);LogEvaluationSteps(Fib{}10)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22Fib%7B%7D0%3D1%3B%20Fib%7B%7D1%3D1%3B%20Fib%7B%7D%7B%7Bn%7D%7D%20%3D%20Fib%7B%7D(n-1)%2BFib%7B%7D(n-2)%3B%5Cn%5CnLogEvaluationSteps(Fib%7B%7D10)%22%7D)

### Differentiate and integrate
Differentiate basic functions.
```
d/dx( e^{3x} \sin (2x) \ln x)
```
[d/dx( e^{3x} \sin (2x) \ln x)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22d%2Fdx(%20e%5E%7B3x%7D%20%5C%5Csin%20(2x)%20%5C%5Cln%20x)%22%7D)

Integrate rational functions (the lack of +C in the integral is deliberate).
```
\int (x+3) / (x^2+x+1)^2 dx
```
[\int (x+3) / (x^2+x+1)^2 dx](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22%5C%5Cint%20(x%2B3)%20%2F%20(x%5E2%2Bx%2B1)%5E2%20dx%22%7D)

Some trig integrals work too.
```
\int \sin^5 x \cos^7 x dx
```
[\int \sin^5 x \cos^7 x dx](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22%5C%5Cint%20%5C%5Csin%5E5%20x%20%5C%5Ccos%5E7%20x%20dx%22%7D)

### Graphics
Plot two-d graphics:
```
Plot2D(x^3, -2, 2, blue)
```

Superimpose two-d graphics by adding them.
```
Plot2D(x^3, -2, 2, blue)+Plot2D(x, -2, 2, green)+PlotPoint((1,1), red)+PlotPoint((-1,-1), red)
```

[Plot2D(x^3, -2, 2, blue)+Plot2D(x, -2, 2, green)+PlotPoint((1,1), red)+PlotPoint((-1,-1), red)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22Plot2D(x%5E3%2C%20-2%2C%202%2C%20blue)%20%2B%5CnPlot2D(x%2C%20-2%2C%202%2C%20green)%2BPlotPoint((1%2C1)%2C%20red)%2BPlotPoint((-1%2C-1)%2C%20red)%22%2C%22monitoring%22%3A%22true%22%7D)


Fill a plot.
```
p{}{{x}} = (\cos (2pi x), \sin (2pi x));
PlotFill( PlotPath( (p{}0, p{}(4/7), p{}(8/7), p{}(12/7), p{}(16/7), p{}(20/7), p{}(24/7), p{}(28/7)), blue), blue)
```
[p{}{{x}} = (\cos (2pi x), \sin (2pi x));PlotFill( PlotPath( (p{}0, p{}(4/7), p{}(8/7), p{}(12/7), p{}(16/7), p{}(20/7), p{}(24/7), p{}(28/7)), blue), blue)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22p%7B%7D%7B%7Bx%7D%7D%20%3D%20(%5C%5Ccos%20(2pi%20x)%2C%20%5C%5Csin%20(2pi%20x))%3B%5CnPlotFill(%20PlotPath(%20(p%7B%7D0%2C%20p%7B%7D(4%2F7)%2C%20p%7B%7D(8%2F7)%2C%20p%7B%7D(12%2F7)%2C%20p%7B%7D(16%2F7)%2C%20p%7B%7D(20%2F7)%2C%20p%7B%7D(24%2F7)%2C%20p%7B%7D(28%2F7))%2C%20blue)%2C%20blue)%22%2C%22monitoring%22%3A%22true%22%7D)

Create slidebars that allow to explore parameter changes.

```
a = MakeInputBox(name = "a", value = 12, min = 1, max = 25);
b = MakeInputBox(name = "b", value = 13, min = 1, max = 25);
PlotFill(PlotCurve((sin(a t), cos(b t)), 0, 2\pi, blue, 2, 2000), pink) 
```
[a = MakeInputBox(name = "a", value = 12, min = 1, max = 25);b = MakeInputBox(name = "b", value = 13, min = 1, max = 25);PlotFill(PlotCurve((sin(a t), cos(b t)), 0, 2\pi, blue, 2, 2000), pink) ](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22a%20%3D%20MakeInputBox(name%20%3D%20%5C%22a%5C%22%2C%20value%20%3D%2012%2C%20min%20%3D%201%2C%20max%20%3D%2025)%3B%5Cnb%20%3D%20MakeInputBox(name%20%3D%20%5C%22b%5C%22%2C%20value%20%3D%2013%2C%20min%20%3D%201%2C%20max%20%3D%2025)%3B%5CnPlotFill(PlotCurve((sin(a%20t)%2C%20cos(b%20t))%2C%200%2C%202%5C%5Cpi%2C%20blue%2C%202%2C%202000)%2C%20pink)%20%22%7D)
### Extras
Factor one-variable polynomials:
```
FactorPolynomialOverRationals(169 x^{11}-312 x^{10}+430 x^{9}-524 x^{8}+595 x^{7}-644 x^{6}+581 x^{5}-440 x^{4}+318 x^{3}-214 x^{2}+127 x-56)
```

[FactorPolynomialOverRationals(169 x^{11}-312 x^{10}+430 x^{9}-524 x^{8}+595 x^{7}-644 x^{6}+581 x^{5}-440 x^{4}+318 x^{3}-214 x^{2}+127 x-56)](https://calculator-algebra.org:8166/#%7B%22currentPage%22%3A%22calculator%22%2C%22calculatorInput%22%3A%22FactorPolynomialOverRationals(169%20x%5E%7B11%7D-312%20x%5E%7B10%7D%2B430%20x%5E%7B9%7D-524%20x%5E%7B8%7D%2B595%20x%5E%7B7%7D-644%20x%5E%7B6%7D%2B581%20x%5E%7B5%7D-440%20x%5E%7B4%7D%2B318%20x%5E%7B3%7D-214%20x%5E%7B2%7D%2B127%20x-56)%22%2C%22monitoring%22%3A%22true%22%7D)

... to be continued.

## How does it work?
All computations handled by the calculator are held in a universal and arbitrarily nested data structure:

`Expression`

The fundamental math `Expression` is very simple: it contains a data entry, a single signed 32-bit or 64-bit integer, and an arbitrary array (allowed to be empty) of other `Expression`s (sub-expressions). Expressions that contain no sub-expressions are called atoms / leafs.

The calculator engine math substitution engine - i.e., an engine designed to make one of the most powerful mathematical techniques - the mathematical substitution - as straightforward as possible.

It keeps on applying a set of rules - mathematical transformations - to the starting expression, until no more rules can be applied. Thus, it is able to model mathematical operations such as uncovering parenthesis and substitutions. 

Calculator programs - a sequence of mathematical expressions with earlier expressions affecting the interpretation of later expressions - are expressions by themselves.

