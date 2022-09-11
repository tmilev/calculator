#include "calculator.h"
#include "calculator_lie_theory.h"
#include "calculator_weyl_group_characters.h"
#include "calculator_inner_functions.h"
#include "calculator_functions_polynomial.h"
#include "calculator_inner_typed_functions.h"

void Calculator::initializeFunctionsScientificBasic() {
  STACK_TRACE("Calculator::initializeFunctionsScientificBasic");
  Function::Options innerStandard = Function::Options::standard();
  Function::Options innerFreezesArguments =
  Function::Options::innerFreezesArguments();
  Function::Options compositeStandard = Function::Options::compositeStandard();
  this->addOperationHandler(
    "EllipticCurveOrderNIST",
    CalculatorFunctions::nistEllipticCurveOrder,
    "",
    "Get a NIST curve order. At present implemented for secp256k1 only. ",
    "g = EllipticCurveGeneratorNIST(\"secp256k1\");\n"
    "order = EllipticCurveOrderNIST(\"secp256k1\");\n"
    "g^order",
    "CalculatorFunctions::nistEllipticCurveOrder",
    "EllipticCurveOrderNIST",
    innerStandard
  );
  this->addOperationHandler(
    "EllipticCurveGeneratorNIST",
    CalculatorFunctions::nistEllipticCurveGenerator,
    "",
    "Makes generator of a NIST curve. "
    "At present implemented for secp256k1 only. ",
    "g=EllipticCurveGeneratorNIST(\"secp256k1\");\n"
    "g^3; "
    "g^11579208923731619542357098500868790785"
    "2837564279074904382605163141518161494337",
    "CalculatorFunctions::nistEllipticCurveGenerator",
    "EllipticCurveGeneratorNIST",
    innerStandard
  );
  this->addOperationHandler(
    "SylvesterMatrix",
    CalculatorFunctionsPolynomial::sylvesterMatrix,
    "",
    "Constructs the transpose Sylvester matrix of two univariate polynomials.",
    "SylvesterMatrix(2x^2+2x+2, 3x+3);\n"
    "SylvesterMatrix(Polynomial(5x^2+4x+3), Polynomial(2x+1));\n"
    "SylvesterMatrix(2, 3x+3);\n"
    "SylvesterMatrix(PolynomialModP(2x^2+2x+2, 5), PolynomialModP(3x+3, 5));\n"
    "SylvesterMatrix(x^2+2x+3, 4x+5, 6x+7);\n"
    "SylvesterMatrix(PolynomialModP(2x^2+2x+2, 5), PolynomialModP(3x+3, 7));\n"
    "SylvesterMatrix(0, x^2);\n",
    "CalculatorConversions::sylvesterMatrix",
    "SylvesterMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertAlgebraicNumberToMatrix",
    CalculatorFunctionsAlgebraic::convertAlgebraicNumberToMatrix,
    "",
    "Converts the algebraic number to its internal matrix representation. ",
    "a = \\sqrt{2};\n"
    "A = ConvertAlgebraicNumberToMatrix(a);\n"
    "b = \\sqrt{3};\n"
    "B = ConvertAlgebraicNumberToMatrix(b);\n"
    "c = \\sqrt{6};\n"
    "C = ConvertAlgebraicNumberToMatrix(c);\n"
    "A \\otimes B",
    "CalculatorFunctions::convertAlgebraicNumberToMatrix",
    "ConvertAlgebraicNumberToMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "PrintAlgebraicClosureStatus",
    CalculatorFunctionsAlgebraic::printAlgebraicClosureStatus,
    "",
    "Prints in human-redable form the "
    "state of the ambient algebraic closure. ",
    "AlgebraicNumberFromPolynomial(x^2 - 6);\n"
    "PrintAlgebraicClosureStatus 0;\n"
    "AlgebraicNumberFromPolynomial(x^2 - 2);\n"
    "PrintAlgebraicClosureStatus 0;\n"
    "AlgebraicNumberFromPolynomial(x^2 - 3);\n"
    "PrintAlgebraicClosureStatus 0;\n",
    "CalculatorFunctions::printAlgebraicClosureStatus",
    "PrintAlgebraicClosureStatus",
    innerStandard
  );
  this->addOperationHandler(
    "AlgebraicNumberFromPolynomial",
    CalculatorFunctionsAlgebraic::getAlgebraicNumberFromMinimalPolynomial,
    "",
    "Creates an algebraic number that is "
    "a root of a polynomial with algebraic number coefficients. ",
    "AlgebraicNumberFromPolynomial{}(x^2 - 4);\n"
    "AlgebraicNumberFromPolynomial{}(x^2 - (3 + 2sqrt(2)));\n"
    "AlgebraicNumberFromPolynomial{}(x^3 - (7 + 5sqrt(2)));\n"
    "AlgebraicNumberFromPolynomial{}(x^3 + \\sqrt{2}x + 1);\n",
    "CalculatorFunctions::getAlgebraicNumberFromMinPoly",
    "AlgebraicNumberFromPolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "ElementWeylAlgebraDO",
    CalculatorFunctions::elementWeylAlgebra,
    "",
    "Creates element of a Weyl algebra = "
    "polynomial coefficient differential operator. "
    "First argument denotes differential operator letter, "
    "second argument - the dual polynomial expression. ",
    "\\partial_{{i}} = ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}} = Polynomial{}x_i;\n"
    "[\\partial_1, x_1];\n"
    "\\partial_1 x_1;\n"
    "x_1\\partial_1",
    "Calculator::elementWeylAlgebra",
    "ElementWeylAlgebraDO",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "ElementWeylAlgebraPoly",
    CalculatorFunctions::polynomialWithEWA,
    "",
    "Creates a monomial from the second argument whose differential "
    "operator letter is the first argument. ",
    "x_{{i}} = ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n"
    "\\partial_{{i}} = ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "\\partial_1 x_1",
    "Calculator::polynomialWithEWA",
    "ElementWeylAlgebraPoly",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "FourierTransformDO",
    CalculatorFunctions::fourierTransformElementWeylAlgebra,
    "",
    "Fourier-transforms an element of a Weyl algebra. "
    "Multiplies each monomial "
    "term of odd total degree by - 1 "
    "(total degree = sum of degrees in the polynomial "
    "variables plus the degrees of the differential variables. ",
    "x = ElementWeylAlgebraPoly{}(\\partial, x);\n"
    "\\partial = ElementWeylAlgebraDO{}(\\partial, x);\n"
    "a =x^3 + x\\partial;\n"
    "b = \\partial x + \\partial^3 + \\partial;\n"
    "[FourierTransformDO{}a, FourierTransformDO{}b]"
    "- FourierTransformDO{}[a, b]",
    "CalculatorFunctions::fourierTransformElementWeylAlgebra",
    "FourierTransformDO",
    innerStandard
  );
  this->addOperationHandler(
    "MinPolyMatrix",
    CalculatorFunctionsLinearAlgebra::minimalPolynomialMatrix,
    "",
    "Computes the minimal polynomial of a matrix, "
    "provided that the matrix is not too large.",
    "A = MakeMatrix{}((0, 1), (- 1, 0));\n"
    "p = MinPolyMatrix{}A",
    "CalculatorFunctions::minimalPolynomialMatrix",
    "MinPolyMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "IsPrimeMillerRabin",
    CalculatorFunctions::isPrimeMillerRabin,
    "",
    "Checks whether the number is prime by the Miller-Rabin test. ",
    "A =100!+ 1; IsPrimeMillerRabin(A);\n"
    "IsPrimeMillerRabin(4256233);\n"
    "IsPrimeMillerRabin(49979687);\n"
    "IsPrimeMillerRabin(4256233 * 49979687)",
    "CalculatorFunctions::isPrimeMillerRabin",
    "IsPrimeMillerRabin",
    innerStandard
  );
  this->addOperationHandler(
    "IsPossiblyPrime",
    CalculatorFunctions::isPossiblyPrime,
    "",
    "Checks whether the number is "
    "prime by trial division first "
    "and by the Miller-Rabin test next.",
    "A =100!+ 1;\n"
    "IsPrimeMillerRabin(A);\n"
    "IsPossiblyPrime(4256233);\n"
    "IsPossiblyPrime(49979687);\n"
    "IsPossiblyPrime(4256233 * 49979687)",
    "CalculatorFunctions::isPossiblyPrime",
    "IsPossiblyPrime",
    innerStandard
  );
  this->addOperationHandler(
    "IsSquareFreePolynomial",
    CalculatorFunctions::isSquareFreePolynomial,
    "",
    "Computes whether a polynomial is square-free by "
    "computing the greatest common divisors of "
    "the partial derivatives with the original polynomial."
    "If the greatest common divisor is constant, "
    "then the polynomial is square-free. ",
    "IsSquareFreePolynomial((x^2-3y^2 x )(x+y));\n"
    "IsSquareFreePolynomial( (x-3x y +5 x y^2)^2 (3+x +y^2) )",
    "CalculatorFunctions::isSquareFreePolynomial",
    "IsSquareFreePolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "AllPartitions",
    CalculatorFunctions::allPartitions,
    "",
    "Prints all partitions of a positive number "
    "into a sum of positive integers. ",
    "AllPartitions(10) ",
    "CalculatorFunctions::allPartitions",
    "AllPartitions",
    innerStandard
  );
  this->addOperationHandler(
    "AllVectorPartitions",
    CalculatorFunctions::allVectorPartitions,
    "",
    "Prints all partitions of the vector "
    "(first argument) using a given list of "
    "vectors (second argument). All partitioning "
    "vectors should have positive coordinates. ",
    "AllVectorPartitions((10, 11),("
    "(1,2),"
    "(2,3),"
    "(4,5),"
    "(2,1),"
    "(3,2),"
    "(5,4)));\n"
    "AllVectorPartitions((4,5),((1,0),(0,1),(1,1),(1,2),(1,3),(2,3)));\n"
    "AllVectorPartitions(20,"
    "(Sequence{}1,"
    "Sequence{}2,"
    "Sequence{}3,"
    "Sequence{}4,"
    "Sequence{}5));",
    "CalculatorFunctions::allVectorPartitions",
    "AllVectorPartitions",
    innerStandard
  );
  this->addOperationHandler(
    "AllSelectionsFixedRank",
    CalculatorFunctionsWeylGroup::allSelectionsFixedRank,
    "",
    "Prints all selections of fixed size (given by first argument) "
    "from a collection of objects with multiplicities "
    "(given as a non-negative integer vector by the second argument). ",
    "AllSelectionsFixedRank(4, (1,2,3,4));\n"
    "AllSelectionsFixedRank(5, (3,1,1));\n"
    "AllSelectionsFixedRank(5, (3,0,1));\n"
    "AllSelectionsFixedRank(0, (3,5,7));\n"
    "AllSelectionsFixedRank(0, (0,0,0));\n"
    "AllSelectionsFixedRank(2, 5);\n"
    "AllSelectionsFixedRank(2, 1);",
    "CalculatorFunctions::allSelectionsFixedRank",
    "AllSelectionsFixedRank",
    innerStandard
  );
  this->addOperationHandler(
    "KostkaNumber",
    CalculatorFunctionsWeylGroup::kostkaNumber,
    "",
    "Computes a Kostka number. "
    "First argument = partition given as a tuple "
    "a_1, ..., a_n with a_1>a_2> ...> a_n. "
    "Second argument = tableaux content = "
    "arbitrary tuple of positive integers. ",
    "KostkaNumber((3,2,1), (4,2)) ",
    "CalculatorFunctionsWeylGroup::kostkaNumber",
    "KostkaNumber",
    innerStandard
  );
  this->addOperationHandler(
    "PrintNonNegativeVectorsLevel",
    CalculatorFunctions::printZnEnumeration,
    "",
    "Prints all vectors of grade level d with n "
    "coordinates lying in Z_{>= 0}. "
    "Function meant for debugging purposes. "
    "First argument = dimension, second argument =grading level. ",
    "PrintNonNegativeVectorsLevel{}(4, 5);"
    "PrintNonNegativeVectorsLevel{}(4, 0); ",
    "Calculator::printZnEnumeration",
    "PrintNonNegativeVectorsLevel",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivSlidesGrLex",
    CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex<
      AlgebraicNumber
    >,
    "",
    "Creates a slide with the polynomial disivion algorithm. "
    "First element = starting slide number.",
    "PolyDivSlidesGrLex{}(1, x^3 + x + 10, x +2);\n"
    "PolyDivSlidesGrLex{}(1,x + y + 10, x + 1, y - 1) ",
    "CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex",
    "PolyDivSlidesGrLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivSlidesGrLex",
    CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex<ElementZmodP>,
    "",
    "Creates a slide with the polynomial disivion algorithm. "
    "First element = starting slide number. "
    "Uses modular arithmetic",
    "a=PolynomialModP( x^{4}+x^{3}+6x^{2}+x+5,3);\n"
    "b=PolynomialModP( 2x^3+2x+2,3);\n"
    "PolyDivSlidesGrLex(5,a,b)",
    "CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex",
    "PolyDivSlidesGrLexModular",
    innerStandard
  );
  this->addOperationHandler(
    "IsNilpotent",
    CalculatorFunctions::isNilpotent,
    "",
    "Computes whether a matrix is nilpotent. "
    "Recall an object X is nilpotent "
    "if there's a positive N with X^N=0. "
    "May/will be extended to work for an arbitrary object "
    "for which the term \"nilpotent\" makes sense. ",
    "isNilpotent{}((0, 1), (0, 0))",
    "CalculatorFunctions::isNilpotent",
    "IsNilpotent",
    innerStandard
  );
  this->addOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystem",
    CalculatorFunctions::solveSerreLikeSystemNoUpperLimit,
    "",
    "Attempts to heuristically solve a system "
    "of polynomial of equations over the rationals. "
    "The input system of equations can be arbitrary, "
    "however it is assumed that the system is Serre-like, i.e., comes "
    "from the Serre relations governing the embedding "
    "of a semisimple Lie algebra in a semisimple Lie algebra. "
    "This suggests a certain heuristic solution strategy "
    "(will be documented as the code matures). "
    "To emphasize, the algorithm is heuristic "
    "and not guaranteed to work. "
    "The result of the function is a printout with one of "
    "the possible outcomes.<br>"
    "Outcome 1. While processing the polynomial system, "
    "the computation limit was hit. "
    "The computation was aborted. "
    "No information on the system can be given (except that it is large). <br>"
    "Outcome 2. The reduced Groebner basis of the system is {1}. "
    "Therefore the system is contradictory - no solution exists. <br>"
    "Outcome 3. The reduced Groebner basis was found and is not equal to {1}. "
    "Therefore a solution over the complex numbers "
    "exists. However, no such solution was found. <br>"
    "Outcome 4. A solution was found and is presented to the user.",
    "FindOneSolutionSerreLikePolynomialSystem{}(\n"
    "x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, "
    "x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, "
    "x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, "
    "x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+"
    "2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, "
    "x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, "
    "x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, "
    "x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, "
    "x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, "
    "x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, "
    "x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, "
    "x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+"
    "2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, "
    "x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14} - x_{1}x_{13} + 1, "
    "x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, "
    "x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, "
    "x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, "
    "x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, "
    "x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, "
    "x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, "
    "x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, "
    "x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)",
    "CalculatorFunctions::solveSerreLikeSystemNoUpperLimit",
    "FindOneSolutionSerreLikePolynomialSystem",
    innerStandard
  );
  this->addOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystemUpperLimit",
    CalculatorFunctions::solveSerreLikeSystemUpperLimit,
    "",
    "Same as FindOneSolutionSerreLikePolynomialSystem "
    "but the first argument gives upper limits "
    "to the number of polynomial computations that can be carried out, "
    "and the second number the number of monomial computations. ",
    "FindOneSolutionSerreLikePolynomialSystemUpperLimit{}(\n"
    "100,"
    "10000,\n"
    "x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, "
    "x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, "
    "x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, "
    "x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+"
    "2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, "
    "x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, "
    "x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, "
    "x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, "
    "x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, "
    "x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, "
    "x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, "
    "x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+"
    "2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, "
    "x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14}-x_{1}x_{13}+ 1, "
    "x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, "
    "x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, "
    "x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, "
    "x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, "
    "x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, "
    "x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, "
    "x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, "
    "x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)",
    "CalculatorFunctions::solveSerreLikeSystemUpperLimit",
    "FindOneSolutionSerreLikePolynomialSystemUpperLimit",
    innerStandard
  );
  this->addOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystemAlgebraic",
    CalculatorFunctions::solveSerreLikeSystemAlgebraic,
    "",
    "Same as FindOneSolutionSerreLikePolynomialSystem "
    "but starts directly over algebraic closure. ",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraic"
    "(x^2 + 1, y x z - 1, z^2 x + y - 1, w u)",
    "CalculatorFunctions::solveSerreLikeSystemAlgebraic",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraic",
    innerStandard
  );
  this->addOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit",
    CalculatorFunctions::solveSerreLikeSystemAlgebraicUpperLimit,
    "",
    "Same as FindOneSolutionSerreLikePolynomialSystemAlgebraic "
    "but the first argument gives upper limits to the number of "
    "polynomial computations that can be "
    "carried out, and the second argument "
    "gives upper limit to the number of monomial computations. ",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit(\n"
    "10000,10000,\n"
    "x^2+ 1, y x z - 1, z^2 x +y - 1, w u)",
    "CalculatorFunctions::solveSerreLikeSystemAlgebraicUpperLimit",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit",
    innerStandard
  );
  this->addOperationHandler(
    "GroebnerLex",
    CalculatorFunctionsPolynomial::groebnerLexicographic,
    "",
    "Transforms to a reduced Groebner basis using the "
    "lexicographic order. The lexicographic order is "
    "inherited from the comparison of the underlying expressions. "
    "The upper limit for polynomial computations is by default:"
    "10000. You can change that to, say, 100, "
    "by adding upperLimit=100 to the list of arguments. "
    "If you set upperLimit = -1 or another negative, this will run "
    "the algorithm with no computation upper limits. "
    "Please do not do run with upperLimit=-1 on our public "
    "web server since multiple instances of a large computation "
    "might hog it up. The resulting printout will "
    "let your know whether the upper limit was hit or not. "
    "If you want to customize the order of variables, add an argument such "
    "as order=(y,x), to signify that the "
    "variable y is stronger than variable x. "
    "The result comes out frozen to prevent the calculator "
    "from reordering the outputs. Unfreeze it with the "
    "Thaw command (see the examples)"
    "<br>"
    "<b>Description of the algorithm.</b> "
    "Let f_1 and f_2 be two polynomials. Following Cox, Little, O'Shea, "
    "\"Ideals, Varieties, Algorithms\", page 81, "
    "denote by S(f_1, f_2) the symmetric difference of f_1 and f_2. "
    "More precisely, let x^{\\gamma_1} be the "
    "leading monomial of f_1 and x^{\\gamma_2} "
    "be the leading monomial of f_2, with leading "
    "coefficients c_1 and c_2. Then define "
    "S(f_1, f_2)= c_2* f_1* lcm (x^\\gamma_1, \\gamma_2) / "
    "x^\\gamma_1 - c_1* f_2 *lcm (x^\\gamma_1, x^\\gamma_2)/x^\\gamma_2. "
    "Here lcm stands for least common multiple of "
    "monomials, defined in the obvious way. "
    "<br>\n"
    "1. Allocate two buckets of polynomials - one \"main\" "
    "bucket and additional bucket. At any given moment in the "
    "algorithm, the union of main bucket and the "
    "additional bucket should give a basis of the ideal. "
    "<br>\n2. Move all input elements into the additional bucket. <br>\n"
    "3. Call the <b>reduce additional bucket</b> subroutine. "
    "In the c++ implementation the function is "
    "called GroebnerBasisComputation::addPolyAndReduceBasis. <br>\n"
    "4. Set changed to be true. <br>\n"
    "5. While (changed)<br>\n"
    "5.1 For all elements f_1, f_2 in the main bucket "
    "form the symmetric difference S(f_1, f_2) and add "
    "it to the additional bucket. <br>\n"
    "5.2 Call the <b>reduce additional bucket</b> subroutine "
    "and set changed to be equal to the result of the subroutine. "
    "<br><b>Reduce additional bucket</b> subroutine. <br>\n"
    "1. Set changedMainBucket to be false. <br>\n"
    "2.While (additional bucket is not empty)<br>\n"
    "2.1. While (additional bucket is not empty)<br>\n"
    "2.1.1 Remove the top element from the additional bucket; "
    "call that element currentElement. <br>\n"
    "2.1.2 Divide currentElement by the elements of "
    "the main bucket, and record the resulting remainder "
    "element in currentRemainder. Here we use the "
    "multivariate polynomial division algorithm, "
    "page 62 of Cox, Little, O'Shea, "
    "\"Ideals, Varieties, Algorithms\". <br>\n"
    "2.1.3 If currentRemainder is non-zero, add "
    "it to the main bucket and set changedMainBucket to be true."
    "<br>\n2.2 For each element in the main bucket <br>\n"
    "2.2.1 Call the considered element currentElement. <br>\n"
    "2.2.2 Divide currentElement by the remaining elements "
    "(excluding currentElement itself) in the main bucket. "
    "Call the remainder of the division currentRemainder. <br>\n"
    "2.2.3 If currentRemainder is not equal to currentElement, "
    "remove currentElement from the main bucket "
    "and add currentRemainder to the additional bucket. "
    "Note that this operation modifies the main "
    "bucket: each element of the main bucket must be "
    "traversed exactly once by the current cycle, "
    "but the division is carried with "
    "the modified state of the main bucket. <br>\n"
    "3. Return changedMainBucket.  <br><b>End of algorithm description.</b>",
    "GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, upperLimit=10000 );\n"
    "GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, upperLimit=5  );\n"
    "GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, order = (s,c,b,a));\n"
    "GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, order = (a,b,c,s));\n"
    "Thaw GroebnerLex{}(s^2+c^2+ 1, a-s^4, b-c^4, order = (a,b,c,s));",
    "Calculator::groebnerLex",
    "GroebnerLex",
    innerStandard
  );
  this->addOperationHandler(
    "GroebnerGrLex",
    CalculatorFunctionsPolynomial::groebnerGradedLexicographic,
    "",
    "Transforms to a reduced Groebner basis relative to the graded "
    "lexicographic order. In the graded lexicographic order, "
    "monomials are first compared by total degree, "
    "then by lexicographic order. The lexicographic order "
    "is inherited from the comparison of the underlying expressions. "
    "All other function setup is as in GroebnberLex. ",
    "GroebnerGrLex{}( a^2+b^2+ 1, x-a^4, y-b^4,upperLimit=10000 );\n "
    "GroebnerGrLex{}(a^2+b^2+ 1, x-a^4, y-b^4, upperLimit=5 )",
    "CalculatorFunctionsPolynomial::groebnerGradedLexicographic",
    "GroebnerGrLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolynomialRelationsUpperLimit",
    CalculatorFunctionsPolynomial::polynomialRelations,
    "",
    "Finds the relations between the polynomials.",
    "PolynomialRelationsUpperLimit{}(10000, s^2+c^2+ 1, s^4, c^4 );",
    "CalculatorFunctionsPolynomial::polynomialRelations",
    "PolynomialRelationsUpperLimit",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra
    ,
    this->opRational(),
    this->opElementWeylAlgebra(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    " \\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\nx_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]"
    ,
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrElementWeylAlgebra"
    "ToRattionalOrPolynomialOrElementWeylAlgebra",
    "AddRationalToElementWeylAlgebra",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra
    ,
    this->opPolynomialRational(),
    this->opElementWeylAlgebra(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i +\\partial_i +x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrElementWeylAlgebra"
    "ToRattionalOrPolynomialOrElementWeylAlgebra",
    "AddPolynomialToElementWeylAlgebra",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra
    ,
    this->opElementWeylAlgebra(),
    this->opPolynomialRational(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    " \\partial_{{i}} = ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i +x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrElementWeylAlgebra"
    "ToRattionalOrPolynomialOrElementWeylAlgebra",
    "AddElementWeylAlgebraToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra
    ,
    this->opElementWeylAlgebra(),
    this->opElementWeylAlgebra(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    " \\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrElementWeylAlgebra"
    "ToRattionalOrPolynomialOrElementWeylAlgebra",
    "AddElementWeylAlgebraToElementWeylAlgebra",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra
    ,
    this->opRational(),
    this->opElementWeylAlgebra(),
    "Multiplies rational or polynomial or "
    "element Weyl algebra by rational "
    "or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "3\\partial_i",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrElementWeylAlgebraBy"
    "RationalOrPolynomialOrElementWeylAlgebra",
    "MultiplyRationalByElementWeylAlgebra",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra
    ,
    this->opPolynomialRational(),
    this->opElementWeylAlgebra(),
    "Multiplies rational or polynomial or element "
    "Weyl algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrElementWeylAlgebraBy"
    "RationalOrPolynomialOrElementWeylAlgebra",
    "MultiplyPolynomialByElementWeylAlgebra",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra
    ,
    this->opElementWeylAlgebra(),
    this->opElementWeylAlgebra(),
    "Multiplies rational or polynomial or element Weyl "
    "algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "a = x_1 x_2;\n"
    "b = \\partial_1 \\partial_2; a b - b a -[a,b] ",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrElementWeylAlgebraBy"
    "RationalOrPolynomialOrElementWeylAlgebra",
    "MultiplyElementWeylAlgebraByElementWeylAlgebra",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra
    ,
    this->opElementWeylAlgebra(),
    this->opPolynomialRational(),
    "Multiplies rational or polynomial or element Weyl "
    "algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrElementWeylAlgebraBy"
    "RationalOrPolynomialOrElementWeylAlgebra",
    "MultiplyElementWeylAlgebraByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements,
    this->opEllipticCurveElementsRational(),
    this->opEllipticCurveElementsRational(),
    "Multiplies two elements of elliptic curves.",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);\n"
    "h = g^2;\n"
    "h * g\n",
    "CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements",
    "MultiplyEllipticCurveElementsRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyEllipticCurveElementsZmodP,
    this->opEllipticCurveElementsZmodP(),
    this->opEllipticCurveElementsZmodP(),
    "Multiplies two elements of elliptic curves.",
    "g = ElementEllipticCurveNormalForm"
    "(y^2 = x^3 +x - 5, x = 3 mod 17, y = 5 mod 17);\n"
    "h = g^2;\n"
    "h * g\n",
    "CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements",
    "MultiplyEllipticCurveElementsOverZmodP",
    innerStandard
  );
  this->addOperationHandler(
    "ElementEllipticCurveNormalForm",
    CalculatorFunctions::elementEllipticCurveNormalForm,
    "",
    "Makes an elliptic curve from a cubic "
    "in normal form, generator letter and base point.",
    "ElementEllipticCurveNormalForm"
    "(y^2 = x^3 + x + 7, x = 3 mod 101, y = 21 mod 101);"
    "ElementEllipticCurveNormalForm(y^2 = x^3 - x + 1, x = 3, y = 5)",
    "CalculatorFunctions::elementEllipticCurveNormalForm",
    "ElementEllipticCurveNormalForm",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerEllipticCurveRationalElementByInteger,
    this->opEllipticCurveElementsRational(),
    this->opRational(),
    "Exponentiates an elliptic curve element by an integer. ",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);\n"
    "g^2",
    "CalculatorFunctionsBinaryOps::powerEllipticCuveElementByInteger",
    "PowerEllipticCurveElementRationalByInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerEllipticCurveZmodPElementByInteger,
    this->opEllipticCurveElementsZmodP(),
    this->opRational(),
    "Exponentiates an elliptic curve element by an integer. ",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);\n"
    "g^2",
    "CalculatorFunctionsBinaryOps::powerEllipticCuveElementByInteger",
    "PowerEllipticCurveElementZmodPByInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerElementWeylAlgebraBySmallInteger,
    this->opElementWeylAlgebra(),
    this->opRational(),
    "Raises element of Weyl algebra to integer power. ",
    "\\partial = ElementWeylAlgebraDO{}(\\partial, x);\n"
    "x = ElementWeylAlgebraPoly{}(\\partial, x); \n"
    "a =x\\partial; \n"
    "a^10;\n"
    "\\partial x^{3/2};\n"
    "\\partial^{3/2} x",
    "CalculatorFunctionsBinaryOps::powerEWABySmallInteger",
    "PowerElementWeylAlgebraBySmallInteger",
    innerStandard
  );
  this->addOperationHandler(
    "ElementWeylAlgebra",
    CalculatorFunctions::compositeElementWeylAlgebraActOnPolynomial,
    "",
    "Differential operation acting on a polynomial. ",
    "x = ElementWeylAlgebraPoly{}(\\partial, x);"
    "\\partial = ElementWeylAlgebraDO{}(\\partial, x);\n"
    "\\partial{}(x); \\partial^{2}{}(x^3+x^2); x{}(x^2)",
    "CalculatorFunctions::compositeElementWeylAlgebraActOnPolynomial",
    "EWAactOnPoly",
    compositeStandard
  );
  this->addOperationHandler(
    "ConvertPolynomialModularToPolynomialRational",
    CalculatorFunctions::convertPolynomialModularToPolynomialRational,
    "",
    "Converts polynomial  ",
    "A = 100x-55;\n"
    "B = PolynomialModP(A, 101);\n"
    "ConvertPolynomialModularToPolynomialRational B",
    "CalculatorFunctions::convertPolynomialModularToPolynomialRational",
    "ConvertPolynomialModularToPolynomialRational",
    innerStandard
  );
  this->addOperationHandler(
    "PlotMandelbrotSet",
    CalculatorFunctionsComplexDynamics::plotMandelbrotSet,
    "",
    "Plots the Mandelbrot set of a complex function. "
    "The first argument is the map we are iterating. "
    "The second argument is which of the letters of your function is the "
    "parameter that is governed by the (x,y) position of the picture, "
    "together with the initial value to set for it. "
    "For classical Mandelbrot set, the first argument should be z^2+C. "
    "The second argument should be C=0. "
    "The map works as follows. "
    "Start with a given value of the complex parameter C. "
    "The value of the parameter is given by the x,y-coordinate of "
    "the plotting surface."
    "Now, iterate the map: f(...f(C)). "
    "If the point escapes to infinity, color the point with a color"
    "that matches the escape speed. "
    "Else, color the point black. "
    "You are not limited to the function z^2+C only. "
    "Feel free to try other functions as well.",
    "PlotMandelbrotSet(z^2+C, C=0);\n"
    "PlotMandelbrotSet(z^3+C, C=0);\n"
    "PlotMandelbrotSet( z^2+z+C, C=0);\n",
    "CalculatorFunctionsComplexDynamics::plotMandelbrotSet",
    "PlotMandelbrotSet",
    innerStandard
  );
  this->addOperationHandler(
    "PlotJuliaSet",
    CalculatorFunctionsComplexDynamics::plotJuliaSet,
    "",
    "Plots the Julia set of a complex function. "
    "The first argument is the map we are iterating. "
    "The remaining parameters are complex parameters of the formula. "
    "The complex parameters can be adjusted dynamically using the "
    "drag and drop."
    "To compute the plot: we start with the x,y-coordinate of "
    "the plotting surface and the number x+iy. "
    "Now, iterate the map: f(...f(x+iy)). "
    "If the point escapes to infinity, color the point with a color"
    "that matches the escape speed. "
    "Else, color the point black. "
    "You are not limited to the function z^2+C only. "
    "Feel free to try other functions as well.",
    "PlotJuliaSet( z^2+0.7i+C, C=0);\n"
    "PlotJuliaSet(1/(b)(z^2+z+1/z+1/z^2), b=3);",
    "CalculatorFunctionsComplexDynamics::plotJuliaSet",
    "PlotJuliaSet",
    innerStandard
  );
}
