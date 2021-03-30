// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_lie_theory.h"
#include "math_general_implementation.h"
#include "calculator_Weyl_group_characters.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_inner_functions.h"
#include "calculator_functions_polynomial.h"
#include "calculator_educational_functions_1.h"
#include "calculator_html_functions.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "database.h"
#include "calculator_database_mongo.h"
#include "string_constants.h"

// This file lists calculator functions and various hard-coded rules.

std::string Calculator::Atoms::setRandomSeed = "SetRandomSeed";
std::string Calculator::Atoms::commandEnclosure = "CommandEnclosure";
std::string Calculator::Atoms::setInputBox = "SetInputBox";
std::string Calculator::Atoms::sort = "Sort";
std::string Calculator::Atoms::transpose = "Transpose";
std::string Calculator::Atoms::approximations = "Approximations";
std::string Calculator::Atoms::turnOnRules = "TurnOnRules";
std::string Calculator::Atoms::turnOffRules = "TurnOffRules";

void Calculator::initializeAdminFunctions() {
  Function::Options adminDefault, adminDisabled;
  adminDefault.dontTestAutomatically = true;
  adminDefault.adminOnly = true;
  adminDefault.flagIsInner = true;
  adminDisabled = adminDefault;
  adminDisabled.disabledByUserDefault = true;
  adminDisabled.disabledByUser = true;
  if (!global.flagDatabaseCompiled || global.flagRunningConsoleTest) {
    return;
  }
  this->addOperationHandler(
    "MongoFind",
    CalculatorDatabaseFunctions::innerExecuteMongoQuery,
    "",
    "Executes a mongoDB query. Requires administator rights. "
    "The database name is calculator (can't be modified). "
    "First argument: collection name. Second argument: query. ",
    "MongoFind(\"users\", \"{}\")",
    "CalculatorDatabaseFunctions::innerExecuteMongoQuery",
    "MongoFind",
    adminDefault
  );
}

// Naming conventions: please start all built-in calculator functions with capital letter.
// Exceptions are made for the following.
// 1) Functions that have mathematical names
// starting with small letters (example: \\sin, logical operations, etc.)
// Those typically start with a backslash.
// 2) Exceptions (desired or otherwise) are made for functions that have already
// been assigned keywords starting with non-capital letter
// and have been used too often.
// IMPORTANT.
// The order of registration of operation handlers for the same operation name
// defines the order in which operation
// handlers are called. Operations handlers registered first are executed first.
// The order only matters for different handlers of the same operation.
// This order is very important, as some of the
// handlers will act properly only if the preceding ones have been carried through
// (example: outerExtractBaseMultiplication requires outerAssociate).
// Note that one can easily transform the code so that the order does not matter.
// One can do that by ``packing'' the correct order of operations into a super-function,
// or by making the corresponding handlers call each other as needed.
// A combination of these two was indeed the original design approach,
// but experience showed that life is easier if we
// take advantage of the order of operations to write less code.
void Calculator::initializeFunctionsStandard() {
  Function::Options outerStandard;
  outerStandard.flagIsInner = false;
  Function::Options outerAdminInvisibleNoTest;
  outerAdminInvisibleNoTest.flagIsInner = false;
  outerAdminInvisibleNoTest.adminOnly = true;
  outerAdminInvisibleNoTest.dontTestAutomatically = true;
  Function::Options innerNoTestInvisibleExperimental;
  innerNoTestInvisibleExperimental.flagIsInner = true;
  innerNoTestInvisibleExperimental.flagIsExperimental = true;
  innerNoTestInvisibleExperimental.dontTestAutomatically = true;
  innerNoTestInvisibleExperimental.visible = false;
  Function::Options innerStandard;
  innerStandard.flagIsInner = true;

  Function::Options innerFreezesArguments = Function::Options::innerFreezesArguments();

  Function::Options innerInvisible;
  innerInvisible.flagIsInner = true;
  innerInvisible.visible = false;

  Function::Options innerStandardOffByDefault;
  innerStandardOffByDefault.flagIsInner = true;
  innerStandardOffByDefault.disabledByUser = true;
  innerStandardOffByDefault.disabledByUserDefault = true;

  Function::Options innerNoTest = Function::Options::innerNoTest();

  Function::Options innerNoTestExperimental;
  innerNoTestExperimental = innerNoTest;
  innerNoTestExperimental.flagIsExperimental = true;

  Function::Options innerAdminNoTest;
  innerAdminNoTest.flagIsInner = true;
  innerAdminNoTest.dontTestAutomatically = true;
  innerAdminNoTest.adminOnly = true;

  Function::Options innerAdminNoTestInvisibleOffByDefault;
  innerAdminNoTestInvisibleOffByDefault.flagIsInner = true;
  innerAdminNoTestInvisibleOffByDefault.dontTestAutomatically = true;
  innerAdminNoTestInvisibleOffByDefault.adminOnly = true;
  innerAdminNoTestInvisibleOffByDefault.visible = false;
  innerAdminNoTestInvisibleOffByDefault.disabledByUser = true;
  innerAdminNoTestInvisibleOffByDefault.disabledByUserDefault = true;

  Function::Options compositeStandard = Function::Options::compositeStandard();
  compositeStandard.flagIsCompositeHandler = true;
  compositeStandard.flagIsInner = true;

  this->addOperationHandler(
    Calculator::Atoms::setRandomSeed,
    CalculatorFunctions::setRandomSeed,
    "",
    "Sets the random seed of the calculator to the given integer value",
    "SetRandomSeed(123);\n"
    "RandomInteger(- 100, 100);\n"
    "RandomInteger(- 100, 100)",
    "CalculatorConversions::setRandomSeed",
    Calculator::Atoms::setRandomSeed,
    innerStandard
  );
  this->addOperationHandler(
    "SelectAtRandom",
    CalculatorFunctions::selectAtRandom,
    "",
    "Selects at random an object from a sequence.",
    "f=SelectAtRandom(\\sin, \\cos); "
    "g=SelectAtRandom(\\tan, \\cot, \\sec, \\csc);"
    "f{}g{}x",
    "CalculatorFunctions::SelectAtRandom",
    "SelectAtRandom",
    innerNoTest
  );
  this->addOperationHandler(
    "RandomInteger",
    CalculatorFunctions::randomInteger,
    "",
    "Generates a random integer. "
    "The random integer lives in intervals given "
    "by pairs of integers. The example code "
    "generates a random number in the union of the "
    "intervals [-2, - 1], [2,5]. "
    "If the input intervals overlap "
    "the overlapped integers will be "
    "generated with higher probability. "
    "For non-overlapping intervals, the random "
    "number distribution should be approximately uniform.",
    "RandomInteger((-2,- 1), (2,5));",
    "CalculatorConversions::randomInteger",
    "RandomInteger",
    innerNoTest
  );
  this->addOperationHandler(
    "TurnOffRules",
    CalculatorFunctions::turnOffRules,
    "",
    "Turns off computational rules.",
    "TurnOffRules(\"sqrt\");\n"
    "a = \\sqrt[4]{t};\n"
    "TurnOnRules(\"sqrt\");\n"
    "a;\n"
    "TurnOffRules(\"ConvertShortDenominatorToNegativePower\");\n"
    "b=1/t^3;"
    "TurnOnRules(\"ConvertShortDenominatorToNegativePower\");"
    "b",
    "CalculatorConversions::turnOffRules",
    "TurnOffRules",
    innerStandard
  );
  this->addOperationHandler(
    "TurnOnRules",
    CalculatorFunctions::turnOnRules,
    "",
    "Turns on computational rules.",
    "TurnOffRules(\"sqrt\"); a = \\sqrt[4]{t}; TurnOnRules(\"sqrt\"); a",
    "CalculatorConversions::turnOnRules",
    "TurnOnRules",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Atoms::approximations,
    CalculatorFunctions::approximationsDummy,
    "",
    "A dummy handler, used to make the implementation of"
    "TurnOnApproximations easier. ",
    "TurnOffApproximations(0);\n"
    "ln(2);\n"
    "TurnOnApproximations(0);\n"
    "ln(2);\n"
    "(TurnOffApproximations 0; ln(2));\n"
    "ln(2)",
    "CalculatorFunctions::approximationsDummy",
    Calculator::Atoms::approximations,
    innerInvisible
  );
  this->addOperationHandler(
    "TurnOnApproximations",
    CalculatorFunctions::turnOnApproximations,
    "",
    "Turns on numerical approximations. "
    "Will wipe out the expression cache. "
    "Takes as input dummy argument. ",
    "TurnOffApproximations(0);\n"
    "ln(2);\n"
    "TurnOnApproximations(0);\n"
    "ln(2);\n"
    "(TurnOffApproximations 0; ln(2));\n"
    "ln(2)",
    "CalculatorFunctions::turnOnRules",
    "TurnOnApproximations",
    innerStandard
  );
  this->addOperationHandler(
    "TurnOffApproximations",
    CalculatorFunctions::turnOffApproximations,
    "",
    "Turns on numerical approximations. Takes as input dummy argument. ",
    "TurnOffApproximations(0); ln(2); TurnOnApproximations(0); ln(2)",
    "CalculatorConversions::turnOnRules",
    "TurnOffApproximations",
    innerStandard
  );
  this->addOperationHandler(
    "EvaluateSymbols",
    CalculatorHtmlFunctions::evaluateSymbols,
    "",
    "Evaluates and replaces individual symbols/variables in LaTeX string. "
    "Leaves the rest of the string intact.",
    "x = 5;\n"
    "left = a;\n"
    "EvaluateSymbols(\"x^x +ax +a x +\\left(left \\right)\")",
    "CalculatorConversions::evaluateSymbols",
    "EvaluateSymbols",
    innerStandard
  );
  this->addOperationHandler(
    "or",
    CalculatorFunctions::innerOr,
    "",
    "Logical or.",
    "0 or 0; 0 or 1; 1 or 0; 1 or 1; a or 1; a or 0;",
    "CalculatorConversions::innerOr",
    "or",
    innerStandard
  );
  this->addOperationHandler(
    "or",
    CalculatorFunctions::innerOrIdentical,
    "",
    "If the two arguments of or are identical, "
    "replaces the expression with the argument. "
    "Works even if the individual expression "
    "cannot be evaluated to 1 or 0. "
    "Please note that mathematically equal objects "
    "may fail to be identical, for example "
    "a rational number 5 and an algebraic number 5. ",
    "a or b; a or a",
    "CalculatorConversions::innerOrIdentical",
    "orIdentical",
    innerStandard
  );
  this->addOperationHandler(
    "IfStandard",
    CalculatorFunctions::innerIfStandard,
    "",
    "Standard if function. Takes 3 arguments. "
    "If first argument is true (equal to 1) "
    "then returns the second argument. "
    "If the first argument is false (equal to 0) "
    "returns the third argument. "
    "In any other situation the expression is not reduced. "
    "Important note: both outcome subexpression of IfStandard. "
    "are evaluated, as illustrated by the built-in example. ",
    "IfStandard (1, x, y);\n"
    "IfStandard (0, x, y);\n"
    "IfStandard (2, x, y)",
    "CalculatorConversions::innerIfStandard",
    "IfStandard",
    innerStandard
  );
  this->addOperationHandler(
    "if",
    CalculatorFunctions::innerIfFrozen,
    "",
    "Frozen if function. "
    "Similar to the IfStandard function but does not "
    "evaluate the second and third argument unless expected. "
    "More precisely, "
    "this function evaluates the first argument, "
    "keeping the second and third argument frozen. "
    "Then the first argument is compared to zero or one. "
    "1) If it equals neither, the expression is not reduced further. "
    "2) If the first argument equals one, "
    "the expression is replaced with its second argument. "
    "3) If the first argument equals zero, "
    "the expression is replaced with its third argument.\n",
    "if (1, x, y);\n"
    "if (1, x, 1 / 0);\n"
    "if (2 - 1, x, 1 / 0);\n"
    "if (1 + 1, 1 / 0, 1 / 0);\n"
    "if (0, x, 1 / 0);\n"
    ,
    "CalculatorConversions::innerIfFrozen",
    "if",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "and",
    CalculatorFunctions::innerAnd,
    "",
    "Logical and. ",
    "0 and 0; 0 and 1; 1 and 0; 1 and 1; a and 1; a and 0;",
    "CalculatorConversions::innerAnd",
    "and",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertElementZmodPToInteger",
    CalculatorFunctions::convertElementZmodPToInteger,
    "",
    "Converts element of Z mod p to an integer between 0 and p - 1. ",
    "5^(ConvertElementZmodPToInteger( 9 mod 7))",
    "CalculatorFunctions::convertElementZmodPToInteger",
    "ConvertElementZmodPToInteger",
    innerStandard
  );
  this->addOperationHandler(
    "MakeInputBox",
    CalculatorHtmlFunctions::userInputBox,
    "",
    "Creates an user input text box. ",
    "MakeInputBox(name = a, value = 3);",
    "CalculatorHtmlFunctions::userInputBox",
    "MakeInputBox",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Atoms::setInputBox,
    CalculatorHtmlFunctions::setInputBox,
    "",
    "Sets value for input box that overrides the input box (no box is displayed). ",
    "SetInputBox(name = a, value = 3); "
    "MakeInputBox(name = a)",
    "CalculatorHtmlFunctions::setInputBox",
    Calculator::Atoms::setInputBox,
    innerStandard
  );
  this->addOperationHandler(
    "Thaw",
    CalculatorFunctions::innerThaw,
    "",
    "If the argument is frozen, removes the top "
    "freeze command and returns the argument, "
    "else returns the argument unchanged.",
    "a =Freeze{}(1 + 1); Thaw a; c =Thaw(Freeze(a,b)); PlotExpressionTree c",
    "CalculatorFunctions::innerThaw",
    "Thaw",
    innerStandard
  );
  this->addOperationHandler(
    "LogEvaluationStepsDebug",
    CalculatorSteps::innerLogEvaluationStepsDebug,
    "",
    "Creates an expression evaluation tree. "
    "Intended for debugging the system. ",
    "LogEvaluationStepsDebug((x +2)(x +3))",
    "CalculatorFunctions::innerLogEvaluationStepsDebug",
    "LogEvaluationStepsDebug",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "LogEvaluationSteps",
    CalculatorSteps::innerLogEvaluationSteps,
    "",
    "Show a step-by step evaluation of the calculator. "
    "Some steps my be combined or omitted for improved human readability. ",
    "LogEvaluationSteps((x +2)(x +3))",
    "CalculatorFunctions::innerLogEvaluationSteps",
    "LogEvaluationSteps",
    innerFreezesArguments
  );

  this->addOperationHandler(
    "FlattenCommandEnclosuresOneLayer",
    CalculatorBasics::flattenCommandEnclosuresOneLayeR,
    "",
    "Flattens command enclosures. ",
    "FlattenCommandEnclosuresOneLayer(CommandEnclosure{}(x = 5; x); "
    "CommandEnclosure{}(y; x))",
    "Calculator::innerFlattenCommandEnclosuresOneLayeR",
    "FlattenCommandEnclosuresOneLayer",
    innerStandard
  );

  this->addOperationHandler(
    "ChildExpression",
    CalculatorFunctions::innerChildExpression,
    "",
    "If defined, returns the nth child of an expression.",
    "ChildExpression(e^x, 1); ChildExpression(e^x, 2); ChildExpression(e^x, 3)",
    "CalculatorFunctions::innerChildExpression",
    "ChildExpression",
    innerStandard
  );
  this->addOperationHandler(
    "ToString",
    CalculatorFunctions::expressionToString,
    "",
    "Transforms an arbitrary expression to its string representation. "
    "The string representation is (supposed to be) LaTeX-compatible. ",
    "ToString( e^x); \n \"e^x\";\"The quick brown fox jumps over the lazy dog.\"",
    "CalculatorFunctions::expressionToString",
    "ToString",
    innerStandard
  );
  this->addOperationHandler(
    "ToUTF8String",
    CalculatorFunctions::expressiontoUTF8String,
    "",
    "Transforms an arbitrary expression to a utf8-string representation. "
    "The string is supposed to look reasonable when drawn on a javascript canvas. ",
    "f = \\sin {}x / \\cos {}x;\n"
    "Plot2D(f, -\\pi/4, \\pi/4) + PlotLabel{}((1, 1), \"y =\" + ToUTF8String(f))",
    "CalculatorFunctions::expressiontoUTF8String",
    "ToUTF8String",
    innerStandard
  );
  this->addOperationHandler(
    "StringToAtom",
    CalculatorFunctions::stringToAtom,
    "",
    "Transforms a string to an atom. ",
    "StringToAtom(\"The quick brown fox jumps over the lazy dog.\")",
    "CalculatorFunctions::stringToAtom",
    "StringToAtom",
    innerStandard
  );
  this->addOperationHandler(
    "\"",
    CalculatorFunctions::quoteToString,
    "",
    "Creates a string. ",
    "\"The quick brown fox jumps over the lazy dog.\";\n"
    "\"\\u00B0\";\n"
    "\"\\u00b0\"",
    "CalculatorFunctions::quoteToString",
    "QuoteToString",
    innerStandard
  );
  this->addOperationHandler(
    "Slice",
    CalculatorFunctions::sliceString,
    "",
    "Slices a string in an interval [a,b), "
    "where a is the first included index and b the first excluded index. "
    "If b is omitted, then the entire string is sliced. "
    "If b is negative, it is replaced by its sum with the string size. ",
    "a= \"abcd\";\n"
    "Slice(a, 0,2);\n"
    "Slice(a,0,- 1);\n"
    "Slice(a,1)\n",
    "CalculatorFunctions::sliceString",
    "Slice",
    innerStandard
  );
  this->addOperationHandler(
    "LogarithmBaseNCeiling",
    CalculatorFunctionsBasic::logarithmBaseNCeiling,
    "",
    "Returns ceiling(log_N (input)), i.e., the smallest integer "
    "X for which input <= N^X. First argument = N = base, "
    "second argument = input.",
    "LogarithmBaseNCeiling(2, 951891382051282682454493);\n"
    "log_2 951891382051282682454493;\n"
    "x = LogarithmBaseNCeiling(2, 100! + 1);\n"
    "2^x - (100!+1);\n"
    "2^{x+1} - (100!+1);\n"
    "y = LogarithmBaseNCeiling(10, 1000! + 1);\n"
    "10^y - 1000! - 1;\n"
    "10^{y + 1} - 1000! - 1;\n"
    "DoubleValue(log_2(1000! + 1)) + 1;\n"
    "LogarithmBaseNCeiling(2, 1);\n"
    "LogarithmBaseNCeiling(- 2, 1);\n"
    "LogarithmBaseNCeiling(2, - 1);\n",
    "CalculatorFunctions::logarithmBaseNCeiling",
    "LogarithmBaseNCeiling",
    innerStandard
  );

  this->addOperationHandler(
    "!",
    CalculatorFunctionsBasic::factorial,
    "",
    "Factorial function. ",
    "5!",
    "CalculatorFunctionsBasic::factorial",
    "Factorial",
    innerStandard
  );
  this->addOperationHandler(
    "Numerator",
    CalculatorFunctions::innerNumerator,
    "",
    "If the expression is a fraction, returns the numerator, "
    "else returns the entire expression. ",
    "Numerator(a/b); Numerator(1+ 1/x)",
    "CalculatorFunctions::innerNumerator",
    "Numerator",
    innerStandard
  );
  this->addOperationHandler(
    "ApplyToSubexpressionsRecurseThroughCalculusFunctions",
    CalculatorFunctions::innerApplyToSubexpressionsRecurseThroughCalculusFunctions,
    "",
    "Applies a transformation, recursively to all subexpressions of "
    "a list of known functions. ",
    "ApplyToSubexpressionsRecurseThroughCalculusFunctions(Polynomialize, sin ((x + 1)^3))",
    "CalculatorFunctions::innerApplyToSubexpressionsRecurseThroughCalculusFunctions",
    "ApplyToSubexpressionsRecurseThroughCalculusFunctions",
    innerStandard
  );
  this->addOperationHandler(
    "SubList",
    CalculatorFunctions::innerSubList,
    "",
    "Extracts a sub-list that contains all elements that satisfy a "
    "condition given by the second argument. Use the bound "
    "variable syntax {{a}} to address the current element of the list.",
    "SubList{}((1, 2, 3, 4), {{a}} > 2)",
    "CalculatorFunctions::innerSubList",
    "SubList",
    innerStandard
  );
  this->addOperationHandler(
    "ApplyToList",
    CalculatorFunctions::innerApplyToList,
    "",
    "Applies a transformation to each element of a list. ",
    "SolveForX{}{{a}} = SolveFor(x, {{a}});\n"
    "SolveForX(x^2 + 2);\n"
    "ApplyToList(SolveForX,(x^2 + 1, x^2 - x - 2))",
    "CalculatorFunctions::innerApplyToList",
    "ApplyToList",
    innerStandard
  );
  this->addOperationHandler(
    "Denominator",
    CalculatorFunctions::innerDenominator,
    "",
    "If the expression is a fraction, returns the denominator, "
    "else returns 1. ",
    "Denominator(a/b); Denominator(1+ 1/x)",
    "CalculatorFunctions::innerDenominator",
    "Denominator",
    innerStandard
  );
  this->addOperationHandler(
    "Product",
    CalculatorFunctions::innerMultiplySequence,
    "",
    "Returns the product of the elements in a sequence. "
    "When used with zero arguments, returns 1.",
    "Product(a, b);\n"
    "Product{}(Sequence{}a);\n"
    "Product(Sequence{}a);\n"
    "Product(a)",
    "CalculatorFunctions::innerMultiplySequence",
    "Product",
    innerStandard
  );
  this->addOperationHandler(
    "\\sum",
    CalculatorFunctions::innerSumSequence,
    "",
    "Returns the sum of the elements in a sequence. "
    "When used with one element, will return that element. ",
    "\\sum(a, b);\n"
    "\\sum();\n"
    "\\sum{}(Sequence{}a);\n"
    "\\sum(Sequence{}a);\n"
    "\\sum(a)",
    "CalculatorFunctions::innerSumSequence",
    "\\sum",
    innerStandard
  );
  this->addOperationHandler(
    "MakeExpression",
    CalculatorConversions::innerExpressionFromBuiltInType,
    "",
    "Creates expression from built-in polynomial. ",
    "MakeExpression(Polynomial{}((x - 2y + z - 1)^2(x + y - z)));\n"
    "A = \\frac{64 x^{2} y x y + 16 x y - y}{- 32 x^{2} y x^{2} - 8 x^{2} + x};\n"
    "B = MakeRationalFunction(A);\n"
    "C = MakeExpression B;\n"
    "D = MakeRationalFunction {}("
    "\\frac{- 2x^{3}y^{2} - \\frac{x y}{2} + \\frac{y }{32}}{x^{4}y + \\frac{x^{2}}{4} - \\frac{x}{32}}"
    ");\n"
    "MakeExpression {}D - C;\n"
    "D - B;",
    "CalculatorConversions::innerExpressionFromBuiltInTypE",
    "MakeExpression",
    innerStandard
  );
  this->addOperationHandler(
    "Polynomial",
    CalculatorConversions::innerPolynomial<Rational>,
    "",
    "Creates a polynomial expression with rational coefficients. ",
    "Polynomial{}((x-2y+z- 1)^2(x +y-z));"
    "\nPolynomial{}(y^2)-(Polynomial{}y)^2",
    "CalculatorConversions::innerPolynomial",
    "Polynomial",
    innerStandard
  );
  this->addOperationHandler(
    "PolynomialAlgebraicNumbers",
    CalculatorConversions::innerPolynomial<AlgebraicNumber>,
    "",
    "Creates a polynomial expression with algebraic number coefficients. ",
    "PolynomialAlgebraicNumbers{}((x + \\sqrt{2})^2 (\\sqrt{3}x - \\sqrt{5}));",
    "CalculatorConversions::innerPolynomial",
    "PolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationHandler(
    "PolynomialModP",
    CalculatorConversions::innerPolynomialModuloInteger,
    "",
    "Creates a polynomial, with modular "
    "arithmetic coefficients, i.e., coefficients in Z / p Z. "
    "First argument = expression to convert to polynomial. "
    "Second argument = the number mod which we are computing. ",
    "PolynomialModP{}(x^2+x+1, 2);",
    "CalculatorConversions::innerPolynomialModuloInteger",
    "PolynomialModP",
    innerStandard
  );

  this->addOperationHandler(
    "IsDifferentialOneFormOneVariable",
    CalculatorFunctions::innerIsDifferentialOneFormOneVariable,
    "",
    "Tests whether the expression is a differential form in one variable.  ",
    "IsDifferentialOneFormOneVariable(Differential x);\n"
    "IsDifferentialOneFormOneVariable({\\text{d}} x);\n"
    "IsDifferentialOneFormOneVariable(x {\\text{d}} y);\n"
    "IsDifferentialOneFormOneVariable(\\frac{\\text{d} y}{y});\n"
    "IsDifferentialOneFormOneVariable(1/(\\text{d} y));",
    "CalculatorFunctions::innerIsDifferentialOneFormOneVariable",
    "IsDifferentialOneFormOneVariable",
    innerStandard
  );
  this->addOperationHandler(
    "Polynomialize",
    CalculatorFunctions::outerPolynomialize,
    "",
    "Polynomialize(a) is equivalent to MakeExpression(Polynomial(a)).",
    "C = (c a + a b + b c + 1)^3;\n"
    "A = Polynomialize(C); "
    "B = MakeExpression(Polynomial(C)); "
    "A - B",
    "CalculatorFunctions::outerPolynomializE",
    "Polynomialize",
    innerStandard
  );
  this->addOperationHandler(
    "IsAlgebraicRadical",
    CalculatorFunctionsAlgebraic::isAlgebraicRadical,
    "",
    "Tests whether the expression is an algebraic expression "
    "obtained using radicals and the four arithmetic operations.  ",
    "IsAlgebraicRadical(\\sqrt{5 + \\sqrt{2}}); "
    "IsAlgebraicRadical(\\sqrt{x}); "
    "IsAlgebraicRadical(\\sqrt{\\pi + e + 1})",
    "CalculatorFunctionsAlgebraic::isAlgebraicRadical",
    "IsAlgebraicRadical",
    innerStandard
  );
  this->addOperationHandler(
    "IsReal",
    CalculatorFunctions::innerIsReal,
    "",
    "Tests whether the expression is a real constant.  ",
    "IsReal(\\sqrt{5 + \\sqrt{- 1}}); IsReal(\\sqrt{\\sqrt{5} - 1});\n"
    "IsReal(sqrt(\\sqrt{\\pi} - 2) )",
    "CalculatorFunctions::innerIsReal",
    "IsReal",
    innerStandard
  );
  this->addOperationHandler(
    "IsConstant",
    CalculatorFunctions::innerIsConstant,
    "",
    "Tests whether the expression is a constant.  ",
    "IsConstant(\\pi^2); IsConstant(1);IsConstant(x);\n"
    "IsConstant(e^{\\sin(\\pi^2 + e +\\sqrt{2} + 3)}); ",
    "CalculatorFunctions::innerIsConstant",
    "IsConstant",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateDefiniteIntegral,
    "",
    "Integrates a definite integral.  ",
    "\\int_{1}^2 (x^2+x) dx ",
    "CalculatorFunctionsIntegration::integrateDefiniteIntegral",
    "DefiniteIntegral",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst,
    "",
    "Attempts to rearrange into standard polynomial form and then integrate.  ",
    "\\int \\left( \\frac{x(x + 1)}{2} \\right)^2 dx ",
    "CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst",
    "IntegrateAfterPolynomialization",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateXnDiffX,
    "",
    "Integrates x^n dx. ",
    "\\int x dx ",
    "CalculatorFunctionsIntegration::integrateXnDiffX",
    "IntegratePowerRule",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateEpowerAxDiffX,
    "",
    "If a is a number, integrates e^{a x} dx. ",
    "\\int x dx ",
    "CalculatorFunctionsIntegration::integrateEpowerAxDiffX",
    "IntegrateEpowerX",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integratePullConstant,
    "",
    "Pulls out constants from integrals  ",
    "\\int 2\\sqrt{2-x^2} dx ",
    "CalculatorFunctionsIntegration::integratePullConstant",
    "IntegratePullConstant",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSqrtOneMinusXsquared,
    "",
    "Integrates \\int \\sqrt{a-x^2}dx, a > 0.",
    "\\int 2\\sqrt{2-x^2} dx ",
    "CalculatorFunctionsIntegration::innerIntegrateSqrtOneminusXsquared",
    "IntegrateSqrtOneminusXsquared",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne,
    "",
    "Integrates \\int \\sqrt{x^2-a}dx, a > 0.  ",
    "\\int 2\\sqrt{3x^2-5} dx ",
    "CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne",
    "IntegrateSqrtXsquaredMinusOne",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateXpowerNePowerAx,
    "",
    "Integrates by parts \\int x^n e^{a x} dx, where n is a positive integer.  ",
    "\\int 2 x^{3} e^{5x} dx ",
    "CalculatorFunctionsIntegration::integrateXpowerNePowerAx",
    "IntegrateXpowerNePowerAx",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsTrigonometry::eulerFormulaAsLaw,
    "",
    "Trigonometrizes an exponential expression using Euler's formula.  ",
    "TurnOnRules(\"EulerFormula\");\n"
    "e^{i x}",
    "CalculatorFunctions::eulerFormulaAsLaw",
    "EulerFormula",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerPowerImaginaryUnit,
    "",
    "Raises imaginary unit to an integer power. ",
    "TurnOnRules(\"PowerImaginaryUnit\");\n"
    "i^{-50!+ 1}; ",
    "CalculatorFunctions::innerPowerImaginaryUnit",
    "PowerImaginaryUnit",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctionsTrigonometry::trigonometricSumToTrigonometricProduct,
    "",
    "Implementation of sum-to-product trig rules. ",
    "TurnOnRules(\"TrigSumToTrigProduct\");\n"
    "sin(a) + sin(b);\n"
    "sin(b) - sin(a);\n"
    "- sin(b) + sin(a);\n"
    "- sin(b) - sin(a);\n"
    "cos(a) + cos(b);\n"
    "cos(a) - cos(b);\n"
    "- cos(a) - cos(b)",
    "CalculatorFunctions::trigonometricSumToTrigonometricProduct",
    "TrigSumToTrigProduct",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\sin",
    CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry,
    "",
    "Implementation of sin (a + b) = sin(a) cos(b) + cos(a) sin(b).",
    "TurnOnRules(\"SineOfAngleSum\"); \\sin(a + b)",
    "CalculatorFunctions::sineOfAngleSumToTrigonometry",
    "SineOfAngleSum",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\cos",
    CalculatorFunctionsTrigonometry::cosineOfAngleSumToTrigonometry,
    "",
    "Implementation of cos (a + b) = cos(a)cos(b) - sin(a)sin(b).",
    "TurnOnRules(\"CosineOfAngleSum\");\n"
    "\\cos(a + b)",
    "CalculatorFunctions::cosineOfAngleSumToTrigonometry",
    "CosineOfAngleSum",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\sin",
    CalculatorFunctionsTrigonometry::convertSineToExponent,
    "",
    "Converts sine to exponent by sin(x) = (e^{i x} - e^{- i x}) / (2 i) ",
    "TurnOnRules(\"ExpressSinViaExponent\", \"ExpressCosViaExponent\");\n"
    "\\sin x;\n"
    "cos x",
    "CalculatorFunctions::innerConvertTrigToExponent",
    "ExpressSinViaExponent",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\cos",
    CalculatorFunctionsTrigonometry::convertCosineToExponent,
    "",
    "Converts cosine to exponent by cos(x) = (e^{i x} + e^{- i x}) / 2 ",
    "TurnOnRules(\"ExpressSinViaExponent\", \"ExpressCosViaExponent\");\n"
    "\\sin x;\n"
    "cos x",
    "CalculatorFunctions::innerConvertTrigToExponent",
    "ExpressCosViaExponent",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\cos",
    CalculatorFunctionsTrigonometry::exploitCosineEvenness,
    "",
    "If a is negative, converts cos (a) to cos(- a).",
    "TurnOnRules(\"ExploitCosEvenness\");\n"
    "cos(- 5x)",
    "CalculatorFunctions::exploitCosineEvenness",
    "ExploitCosEvenness",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\sin",
    CalculatorFunctionsTrigonometry::exploitSineOddness,
    "",
    "If a is negative, converts sin (a) to -sin(-a).",
    "TurnOnRules(\"ExploitSinOddness\");\n"
    "sin(- 5x)",
    "CalculatorFunctions::exploitSineOddness",
    "ExploitSinOddness",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM,
    "",
    "Solves/transforms an integral of the form \\int \\sin^n x \\cos^m x. ",
    "\\int \\sin^{4}x \\cos^{6}x dx ",
    "CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM",
    "IntegrateSinPowerNCosPowerM",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateTanPowerNSecPowerM,
    "",
    "Transforms an integral of the form \\int \\tan^n x \\sec^m x.",
    "TurnOffRules(\"ExpressTanViaSineCosine\", \"ExpressSecViaCosine\");\n"
    "\\int \\tan^2 x \\sec^{2}x dx ",
    "CalculatorFunctions::integrateTanPowerNSecPowerM",
    "IntegrateTanPowerNSecPowerM",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSum,
    "",
    "If the integral is of the form \\int (A+B )dx, tries recursively to integrate A and B. "
    "If successful, integrates the sum in the obvious way.",
    "\\int (x + 1+\\sqrt{}2) dx ",
    "CalculatorFunctionsIntegration::integrateSum",
    "IntegrateSum",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integratePullImaginaryUnit,
    "",
    "Pulls out the imaginary unit in front of the integral. ",
    "\\int i 3 e^{i x} dx ",
    "CalculatorFunctionsIntegration::integratePullImaginaryUnit",
    "IntegratePullOutImaginaryUnit",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateSqrt,
    "",
    "Differentiation - square root function. ",
    "d/dx(sqrt(x));",
    "CalculatorFunctions::differentiateSqrt",
    "DifferentiateSqrt",
    innerStandard
  );
  //the function integrating the building blocks must come in the exact order below:
  //else we risk infinite substitution cycle. The reasons are implementation-specific.
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa,
    "",
    "Integrates building block Ia.  ",
    "\\int (\\frac{3}{(x / 2 - 1)}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa",
    "IntegrateBlockIa",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb,
    "",
    "Integrates building block Ib.  ",
    "\\int (\\frac{2}{(3x- 1)^2}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb",
    "IntegrateBlockIb",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIaandIIIa,
    "",
    "Integrates building blocks IIa and IIIa.  ",
    "\\int (\\frac{3x + 2}{x^2 + x + 1}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIaandIIIa",
    "IntegrateBlockIIaIIIa",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb,
    "",
    "Integrates building blocks IIIb.  ",
    "\\int (\\frac{3x + 2}{(x^2 + x + 1)^2}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb",
    "IntegrateBlockIIIb",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb,
    "",
    "Integrates building blocks IIb.  ",
    "\\int (\\frac{3x + 2}{(x^2 + x + 1)^2}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb",
    "IntegrateBlockIIb",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionSplitToBuidingBlocks,
    "",
    "Attempts to split an integral of a rational function into building block integrals. ",
    "\\int \\frac{1}{x(x + 1)} dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionSplitToBuidingBlocks",
    "IntegratePartialFractions",
    innerStandard
  );

  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig,
    "",
    "Differentiation - product rule. ",
    "Differentiate(x, (\\sin x) \\cos x )",
    "CalculatorFunctions::differentiateTrigAndInverseTrig",
    "DifferentiateTrigAndInverseTrig",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateChainRule,
    "",
    "Differentiation - chain rule.  ",
    "Differentiate(x, \\sin x^2 \\cos (\\sin x))",
    "CalculatorFunctions::differentiateChainRule",
    "DifferentiateChainRule",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAtimesB,
    "",
    "Differentiation - product rule.  ",
    "Differentiate(x, f * g)",
    "CalculatorFunctions::differentiateAtimesB",
    "DifferentiateProductRule",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative,
    "",
    "Differentiation - division rule, commutative. "
    "For the time being differentiation is assumed to be over commutative rings. "
    "This might change in the future. "
    "The commutative division rule is "
    "d/dx (f/g^n) = ((d/dx f) g - n f (d/dx g))/g^{n + 1}. ",
    "Differentiate(x, f / g);\n"
    "Differentiate(x, f / g^5);\n"
    "Differentiate(x, f / g^n)",
    "CalculatorFunctions::differentiateAdivideBCommutative",
    "DifferentiateQuotientRule",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAplusB,
    "",
    "Differentiation - sum rule.  ",
    "Differentiate(x, f + g)",
    "CalculatorFunctions::differentiateAplusB",
    "DifferentiateSum",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateConstant,
    "",
    "Differentiation - constant rule. ",
    "Differentiate(x, 5 );\n"
    "d/dx \\sqrt{5}",
    "CalculatorFunctions::differentiateConstant",
    "DifferentiateConstant",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateX,
    "",
    "Differentiation - d/dx x = 1.",
    "Differentiate(x, x )\n;"
    "d/dx x",
    "CalculatorFunctions::differentiateX",
    "DifferentiateIdentity",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateConstPower,
    "",
    "Differentiation - d/dx x^n = n x^{n - 1}.",
    "Differentiate(x, x^2 );\n"
    "d/dx x^- 1;\n"
    "d/dt t^{\\sqrt{2}};\n"
    "d/dc c^{DoubleValue 3.14}",
    "CalculatorFunctions::differentiateConstPower",
    "DifferentiateConstPower",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAPowerB,
    "",
    "Differentiation - d/dx a^b = d/dx(e^{b\\log a}) = a^b d/dx(b\\log a) .",
    "Differentiate(x, x^2);\n"
    "d/dx x^- 1;\n"
    "d/dt t^{\\sqrt{2}};\n"
    "d/dc c^{DoubleValue 3.14}",
    "CalculatorFunctions::differentiateAPowerB",
    "DifferentiateApowerB",
    innerStandard
  );
  this->addOperationHandler(
    "\\times",
    CalculatorFunctions::crossProduct,
    "",
    "Cross product in IIId. Returns error if given two non-3d vectors.",
    "(1, - 2, 3) \\times ( 1, - 1, - 1)",
    "CalculatorFunctions::crossProduct",
    "crossProduct",
    innerStandard
  );

  this->addOperationHandler(
    "MakeRationalFunction",
    CalculatorConversions::innerRationalFunction,
    "",
    "Creates a built-in rational function.",
    "MakeRationalFunction{}(x_1 + MakeRationalFunction{}x_1 + x_2)",
    "CalculatorConversions::innerRationalFunctioN",
    "MakeRationalFunction",
    innerStandard
  );
  this->addOperationHandler(
    "MakeMatrix",
    CalculatorConversions::innerMakeMatrix,
    "",
    "Creates an internal c++ matrix structure from double list of rationals. ",
    "s_1 = MakeMatrix{}((- 1,- 1, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1));\n"
    "s_2 = MakeMatrix{}((1, 0, 0, 0), (- 1, - 1, - 1, 0), (0, 0, 1, 0), (0, 0, 0, 1));\n"
    "s_3 = MakeMatrix{}((1, 0, 0, 0), (0, 1, 0, 0), (0, - 2, - 1, - 1), (0, 0, 0, 1));\n"
    "s_4 = MakeMatrix{}((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, - 1, - 1)); ",
    "CalculatorConversions::innerMatrixRational",
    "MakeMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "MakeMatrixTensorForm",
    CalculatorConversions::innerMatrixRationalTensorForM,
    "",
    "Same as MakeMatrix but uses different c++ implementation "
    "(class MatrixTensor instead of class Matrix). ",
    "s_1 = MakeMatrixTensorForm{}(\n"
    "(- 1,- 1, 0, 0),\n"
    "(0, 1, 0, 0),\n"
    "(0, 0, 1, 0),\n"
    "(0, 0, 0, 1)\n"
    "); ",
    "CalculatorConversions::innerMatrixRationalTensorForM",
    "MakeMatrixTensorForm",
    innerStandard
  );
  this->addOperationHandler(
    "MakeMatrixRFs",
    CalculatorConversions::innerMatrixRationalFunction,
    "",
    "Creates an internal C++ matrix structure "
    "from double list of polynomial functions. ",
    "s_1 = MakeMatrixRFs{}((1 - t, 2), (3, 2 - t))",
    "CalculatorConversions::innerMatrixRationalFunction",
    "MakeMatrixRFs",
    innerStandard
  );

  this->addOperationHandler(
    "EnsureExpressionDependsOnlyOn",
    CalculatorFunctions::innerEnsureExpressionDependsOnlyOnStandard,
    "",
    "Ensures the expression in the first argument depends only on the free variables given in the remaining arguments. "
    "Returns a warning string if that is not the case, else returns an empty string. "
    "Intended to warn users of potentially mistyped expressions. ",
    "EnsureExpressionDependsOnlyOn(\\sin (\\ln x) x y, x, y);\n"
    "EnsureExpressionDependsOnlyOn(\\sin (\\ln x) x y, x);",
    "CalculatorFunctions::innerEnsureExpressionDependsOnlyOnStandard",
    "EnsureExpressionDependsOnlyOn",
    innerStandard
  );
  this->addOperationHandler(
    "EnsureExpressionDependsMandatoryVariables",
    CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants,
    "",
    "Similar to EnsureExpressionDependsOnlyOn, but requests that the expression "
    "depend on two lists, first "
    "with mandatory variables, second with non-mandatory. ",
    "EnsureExpressionDependsMandatoryVariables(\\sin (\\ln x) x y, x, y); "
    "EnsureExpressionDependsMandatoryVariables(\\sin (\\ln x) x y,  (x,y,z) );",
    "CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables",
    "EnsureExpressionDependsMandatoryVariables",
    innerStandard
  );
  this->addOperationHandler(
    "EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants",
    CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants,
    "",
    "Same as EnsureExpressionDependsMandatoryVariables including named constants such as \\pi, e. ",
    "EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants(2\\pi, \\pi, y);\n"
    "EnsureExpressionDependsMandatoryVariables(2, \\pi, none );",
    "CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables",
    "EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants",
    innerStandard
  );
  this->addOperationHandler(
    "\\log",
    CalculatorFunctionsBasic::logarithm,
    "",
    "Logarithm function. Gives a decimal approximation of the natural "
    "logarithm provided the input is a double number. ",
    "\\log{}(e); \\log 10",
    "CalculatorFunctionsBasic::logarithm",
    "\\log",
    innerStandard
  );
  this->addOperationHandler(
    "LogBase",
    CalculatorFunctionsBasic::logarithmBaseNaturalToLn,
    "",
    "Converts \\log_e A to \\ln A. ",
    "\\log_e A",
    "CalculatorFunctionsBasic::logarithmBaseNaturalToLn",
    "LogBaseNaturalToLn",
    innerStandard
  );
  this->addOperationHandler(
    "LogBase",
    CalculatorFunctionsBasic::logarithmBaseSimpleCases,
    "",
    "If the argument is a power of the base does the computation. ",
    "\\log_5 25; log_5 1; log_5 (1/5)",
    "CalculatorFunctions::logarithmBaseSimpleCases",
    "LogBaseSimpleCases",
    innerStandard
  );
  this->addOperationHandler(
    "LogBase",
    CalculatorFunctionsBasic::logarithmBase,
    "",
    "Logarithm function (arbitrary base). Equivalent to "
    "\\log_{{a}} {{b}}= (\\log b)/(\\log a). ",
    "\\log_10 5; \\log _e 10; \\ln 10; \\log_a b",
    "CalculatorFunctions::logarithmBase",
    "LogBase",
    innerStandard
  );
  this->addOperationHandler(
    "\\arctan",
    CalculatorFunctionsTrigonometry::arctan,
    "",
    "Arctan function. Tries to evaluate the arctan function. ",
    "\\arctan(3/4)",
    "CalculatorFunctions::arctan",
    "\\arctan",
    innerStandard
  );
  this->addOperationHandler(
    "\\arcsin",
    CalculatorFunctionsTrigonometry::arcsinAlgebraic,
    "",
    "Arcsin function for special angles. ",
    "\\arcsin(1/2)",
    "CalculatorFunctions::arcsinAlgebraic",
    "arcsinAlgebraic",
    innerStandard
  );
  this->addOperationHandler(
    "\\arccos",
    CalculatorFunctionsTrigonometry::arccosAlgebraic,
    "",
    "Arccos function for special angles. ",
    "\\arccos(\\sqrt{2}/2)",
    "CalculatorFunctions::arccosAlgebraic",
    "arccosAlgebraic",
    innerStandard
  );
  this->addOperationHandler(
    "\\arccos",
    CalculatorFunctionsTrigonometry::arccos,
    "",
    "Arccos function. Tries to evaluate the arccos function. ",
    "\\arccos(3/4)",
    "CalculatorFunctions::arccos",
    "\\arccos",
    innerStandard
  );
  this->addOperationHandler(
    "\\arcsin",
    CalculatorFunctionsTrigonometry::arcsin,
    "",
    "Arcsin function. Tries to evaluate the arcsin function. ",
    "\\arcsin(3/4)",
    "CalculatorFunctions::arcsin",
    "\\arcsin",
    innerStandard
  );
  this->addOperationHandler(
    "|",
    CalculatorFunctionsBasic::absoluteValue,
    "",
    "Absolute value function ",
    "|- 1|",
    "CalculatorFunctionsBasic::absoluteValue",
    "AbsoluteValue",
    innerStandard
  );
  this->addOperationHandler(
    "\\sin",
    CalculatorFunctionsTrigonometry::sin,
    "",
    "Sine function. Evaluates to a decimal approximation if the input is a double number. ",
    "\\sin{}(3.1415)",
    "CalculatorFunctions::sin",
    "Sine",
    innerStandard
  );
  this->addOperationHandler(
    "\\cos",
    CalculatorFunctionsTrigonometry::cos,
    "",
    "Cosine function. Evaluates to a decimal approximation if the input is a double number. ",
    "\\cos{}(3.1415)",
    "CalculatorFunctions::cos",
    "Cosine",
    innerStandard
  );
  this->addOperationHandler(
    "\\tan",
    CalculatorFunctionsTrigonometry::tan,
    "",
    "Tangent function. Substitutes \\tan{}{{x}}=\\sin{}x/\\cos x. ",
    "\\tan{}(3.1415); \\tan 1.570796327",
    "CalculatorFunctions::tan",
    "ExpressTanViaSineCosine",
    innerStandard
  );
  this->addOperationHandler(
    "\\cot",
    CalculatorFunctionsTrigonometry::cotangent,
    "",
    "Tangent function. Substitutes \\tan{}{{x}}=\\cos{}x/\\sin x. ",
    "\\cot{}(3.1415); \\cot 1.570796327",
    "CalculatorFunctions::cotangent",
    "ExpressCotViaSineCosine",
    innerStandard
  );
  this->addOperationHandler(
    "\\csc",
    CalculatorFunctionsTrigonometry::csc,
    "",
    "Cosecant function. Substitutes \\csc{}{{x}}=1/\\sin x. ",
    "\\csc{}(3.1415); \\csc 1.570796327",
    "CalculatorFunctions::csc",
    "ExpressCscViaSine",
    innerStandard
  );
  this->addOperationHandler(
    "\\sec",
    CalculatorFunctionsTrigonometry::sec,
    "",
    "Secant function. Substitutes \\sec{}{{x}}=1/\\sec x. ",
    "\\sec{}(3.1415); \\sec 1.570796327",
    "CalculatorFunctions::sec",
    "ExpressSecViaCosine",
    innerStandard
  );
  this->addOperationHandler(
    "Floor",
    CalculatorFunctionsBasic::floor,
    "",
    "Floor function.",
    "Floor(3/7)",
    "CalculatorFunctionsBasic::floor",
    "Floor",
    innerStandard
  );
  this->addOperationHandler(
    "Round",
    CalculatorFunctionsBasic::round,
    "",
    "Round function.",
    "Round(3/7)",
    "CalculatorFunctionsBasic::round",
    "Round",
    innerStandard
  );
  this->addOperationHandler(
    "DoubleValue",
    CalculatorFunctions::innerEvaluateToDoublE,
    "",
    "Double value of a rational number.",
    "DoubleValue{}(3/7)",
    "CalculatorFunctions::innerEvaluateToDoublE",
    "DoubleValue",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerDegreesToRadians,
    "",
    "Converts degrees to radians",
    "30^\\circ",
    "CalculatorFunctions::innerDegreesToRadians",
    "DegreesToRadians",
    innerStandard
  );
  this->addOperationHandler(
    "ModP",
    CalculatorFunctions::innerZmodP,
    "",
    "Number modulo P. First argument = number, second argument = modulo.",
    "ModP{}(7, 3)",
    "Calculator::innerZmodP",
    "ModP",
    innerStandard
  );

  this->addOperationHandler(
    "GCD",
    CalculatorFunctions::greatestCommonDivisorInteger,
    "",
    "Greatest common divisor of two integers.",
    "GCD(100!+ 1, 101 * 103)",
    "CalculatorFunctions::leastCommonMultipleInteger",
    "GCD",
    innerStandard
  );
  this->addOperationHandler(
    "LCM",
    CalculatorFunctions::leastCommonMultipleInteger,
    "",
    "Least common multiple of two integers.",
    "LCM(91, 1001)",
    "CalculatorFunctions::innerLCM",
    "LCM",
    innerStandard
  );
  this->addOperationHandler(
    "GCDPoly",
    CalculatorFunctionsPolynomial::greatestCommonDivisorPolynomial,
    "",
    "Greatest common divisor polynomial of two polynomials. "
    "The divisor is scaled so that all coefficients are relatively prime integers, "
    "and so that the leading monomial under the graded lexicographic order (x_2>x_1, etc.) "
    "has positive coefficient.",
    "GCDPoly{}(x^3-1, x^2-1)\n;"
    "GCDPoly{}(2/3x^3-2/3, 10x^2-10)\n;"
    "GCDPoly( (a^2-b^2)(a^3-b^3), (a^4-b^4)(a^5-b^5));\n"
    "GCDPoly(PolynomialModP((x^2+3)(x^3+1), 7), PolynomialModP((x^2+3)(x^3+x+1), 7));\n"
    "f=PolynomialModP{}(x^{5}+x^{4}+3x^{3}+x^{2}+x +3,7);\n"
    "y=PolynomialModP{}(3x^{4}+2x^{3}+3x +3,7);\n"
    "GCDPoly(f, y);\n"
    "GCDPoly{}(x y, y+x z);\n"
    "GCDPoly{}("
    "-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}"
    "-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ "
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ "
    "x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ "
    "x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, "
    "x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}- "
    "4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- "
    "2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- "
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}  )",
    "CalculatorFunctionsPolynomial::greatestCommonDivisorPolynomial",
    "GCDPoly",
    innerStandard
  );
  this->addOperationHandler(
    "LCMPoly",
    CalculatorFunctionsPolynomial::leastCommonMultiplePolynomial,
    "",
    "Least common multiple of two polynomials. "
    "The output is scaled so that all coefficients are relatively prime integers, "
    "and so that the leading monomial under the graded lexicographic order "
    "(x_2>x_1, etc.) has positive coefficient.",
    "LCMPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ "
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ "
    "x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ "
    "x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, "
    "x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}- "
    "4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- "
    "2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- "
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}  )",
    "CalculatorFunctionsPolynomial::leastCommonMultiplePolynomial",
    "LCMPoly",
    innerStandard
  );
  this->addOperationHandler(
    "Interpolate",
    CalculatorFunctions::innerInterpolatePoly,
    "",
    "Constructs the one-variable polynomial of minimal degree that passes through "
    "the points. Points are given in the form "
    "((x_1, y_1),(x_2, y_2), ...,(x_n, y_n))",
    "Interpolate{}(1,0) ; Interpolate{}((1,0),(2,3));\n"
    "Interpolate{}((1,1), (2,2), (3, 4), (4, 8), (5, 16))",
    "Calculator::innerInterpolatePoly",
    "Interpolate",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivRemainder",
    CalculatorFunctionsPolynomial::polynomialDivisionRemainder,
    "",
    "Returns the remainder after taking quotient of a "
    "polynomial divided by a set of polynomials "
    "using the default monomial order (lexicographic).",
    "PolyDivRemainder{}(x^7+6x y+5x y^8+y^5, x +y^2- 1, y^3-x y);",
    "CalculatorFunctions::innerPolynomialDivisionRemainder",
    "PolyDivRemainder",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivQuotient",
    CalculatorFunctionsPolynomial::polynomialDivisionQuotient,
    "",
    "Returns the quotients of a "
    "polynomial divided by a set of polynomials using the default monomial order (lexicographic).",
    "PolyDivQuotient{}(x^7+6x y+5x y^8+y^5, x +y^2- 1, y^3-x y) ;",
    "CalculatorFunctionsPolynomial::polynomialDivisionQuotient",
    "PolyDivQuotient",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivSlidesGrLex",
    CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex,
    "",
    "Creates a slide with the polynomial disivion algorithm. "
    "First element = starting slide number.",
    "PolyDivSlidesGrLex{}(1, x^3 + x + 10, x +2) ; PolyDivSlidesGrLex{}(1,x + y + 10, x + 1, y - 1) ",
    "Calculator::innerPolynomialDivisionSlidesGrLex",
    "PolyDivSlidesGrLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringGrLex",
    CalculatorFunctionsPolynomial::polynomialDivisionVerboseGrLex,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using gr lex order, for example, x^2 y^3 >x y^4, y^11>x^10. ",
    "PolyDivStringGrLex{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1) ;",
    "CalculatorFunctions::innerPolynomialDivisionVerboseGrLex",
    "PolyDivStringGrLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringGrLexRev",
    CalculatorFunctionsPolynomial::polynomialDivisionVerboseGradedReverseLexicographic,
    "",
    "String that presents the division of a polynomial "
    "by a set of polynomials. Uses the graded reverse lexicographic order. "
    "More precisely, compares monomials by total degree. "
    "Ties are broken by comparing powers right to left. "
    "First **smaller** power on the right is declared the winner ('larger')."
    "For example,  x^2 y^3 > x y^4, x^11 > y^10. ",
    "PolyDivStringGrLexRev{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1) ;",
    "CalculatorFunctions::innerPolynomialDivisionVerboseGradedReverseLexicographic",
    "PolyDivStringGrLexRev",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringLex",
    CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using the "
    "lexicographic order on the monomial exponents. "
    "For example, x^2 y^4 > x y^1000 > x y^2."
    "Please do note that the lexicographic order "
    "is not related to the lexicographic order on the "
    "words 'expansions' (x^2y^3->xxyyy) of the monomials. ",
    "PolyDivStringLex{}(x^7 + 6 x y + 5x y^8 + y^5, x^2 + 2, y^3 - 1);\n"
    "PolyDivStringLex{}(x^7 + \\sqrt{2} x + 1, \\sqrt{3}x^2 + 2x + 1)",
    "CalculatorFunctions::innerPolynomialDivisionVerboseLex",
    "PolyDivStringLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringLexRev",
    CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographicOpposite,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using the "
    "lexicographic order on the monimial exponents "
    "with reversed order of variables. "
    "For example, y^2 > x^1000 y > x y. "
    "Please do note that the lexicographic order "
    "is not related to the lexicographic order on the "
    "words 'expansions' (x^2y^3->xxyyy) of the monomials. "
    ,
    "PolyDivStringLexRev{}(x^7 + 6x y + 5x y^8 + y^5, x^2 + 2, y^3 - 1) ;",
    "CalculatorFunctions::innerPolynomialDivisionVerboseLexRev",
    "PolyDivStringLexRev",
    innerStandard
  );

  this->addOperationHandler(
    "DFQEuler",
    CalculatorFunctions::innerDFQsEulersMethod,
    "",
    "<b>Calculus teaching function.</b> Iterates Euler's method "
    "to approximate solutions of first order ordinary DFQ's. "
    "First argument = expression for y', "
    "second and third argument = x and y initial values, "
    "fourth argument = number of approximating points, "
    "fifth and sixth argument = left and right endpoints.",
    "DFQEuler(x^2 + y^2 - 1, 0, 0, 1000, - 2.5, 2.5)",
    "CalculatorFunctions::innerDFQsEulersMethod",
    "DFQEuler",
    innerStandard
  );
  this->addOperationHandler(
    "NewtonsMethod",
    CalculatorFunctions::innerNewtonsMethod,
    "",
    "Applies Newton's method with a given starting point and given number of iterations."
    "The first argument gives the function whose zeroes we are trying to find. "
    "The second argument gives the starting point. "
    "The last argument gives the number of iterations of the method. ",
    "NewtonsMethod(e^x - ( 200 sin{} x + x^7), 0, 10);\n"
    "NewtonsMethod(e^x = ( 200 sin{} x + x^7), 0, 10);",
    "CalculatorFunctions::innerNewtonsMethod",
    "NewtonsMethod",
    innerStandard
  );

  this->addOperationHandler(
    "PlotDirectionField",
    CalculatorFunctionsPlot::plotDirectionField,
    "",
    "Plots a direction field (in 2d for the time being, 3d coming soon). "
    "Direction field is like a vector field except that all vectors are "
    "normalized to have the same length. First argument = the vector field. "
    "Second, third arguments: bottom left and top right corner of the "
    "viewing rectangle. Next argument: (numX,numY,...), where numX is the number "
    "of segments along the x axis and so on. Next argument: length of "
    "each direction line. "
    "Next arguments: color, line width. ",
    "PlotDirectionField( (- y, x), (- 2, - 2), (2, 2), (20, 20), 0.2, blue, 1);",
    "CalculatorFunctions::plotDirectionField",
    "PlotDirectionField",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPolar",
    CalculatorFunctionsPlot::plotPolarRfunctionTheta,
    "",
    "<b>Calculus teaching function.</b> Draws polar curve given in "
    "polar coordinates  in the form r = f(t), where t is the angle variable. "
    "The angle variable is measured in degrees. "
    "The first argument gives the function, the second and third argument "
    "give the upper and lower bounds of the angle. ",
    "PlotPolar(1+sin  t, 0, \\pi);\n"
    "PlotPolar((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t) )"
    "(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\\pi)",
    "CalculatorFunctions::plotPolarRfunctionTheta",
    "PlotPolar",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPolarExtended",
    CalculatorFunctionsPlot::plotPolarRfunctionThetaExtended,
    "",
    "<b>Calculus teaching function.</b> Same as PlotPolar "
    "but also produces a graph in the (rho,theta)-plane. ",
    "PlotPolarExtended(1 + sin  t, 0, \\pi);\n"
    "PlotPolarExtended((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t) )\n"
    "(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\\pi)",
    "CalculatorFunctions::plotPolarRfunctionThetaExtended",
    "PlotPolarExtended",
    innerStandard
  );
  this->addOperationHandler(
    "GaussianElimination",
    CalculatorFunctions::innerGaussianEliminationMatrix,
    "",
    "Gaussian elimination of a matrix. Prints a detailed string "
    "that shows the Gaussian elimination of a matrix.",
    "GaussianElimination(\\begin{array}{cccccc} "
    "sqrt 1 & sqrt 2 & sqrt 3 & 1 & 0 & 0\\\\ "
    "sqrt 4 & sqrt 5 & sqrt 6 & 0 & 1 & 0\\\\ "
    "sqrt 7 & sqrt 8 & sqrt 9 & 0 & 0 & 1 \\end{array})",
    "CalculatorFunctions::innerGaussianEliminationMatrix",
    "GaussianElimination",
    innerStandard
  );
  this->addOperationHandler(
    "UnivariatePartialFractionAlgrithm",
    CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm,
    "",
    "Attempts to splits a univariate rational function into partial fractions "
    "over the real algebraic numbers. At the moment, "
    "the function factors the denominator "
    "over the rationals. If this factorization "
    "yields quadratic and linear terms at the function succeeds, else fails.",
    "UnivariatePartialFractionAlgrithm(\\frac{x^11}{x^{8}-2x^{6}-2x^{5}+4x^{3}+x^{2}-2});",
    "CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm",
    "UnivariatePartialFractionAlgrithm",
    innerStandard
  );
  this->addOperationHandler(
    "UnivariatePartialFractions",
    CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealS,
    "",
    "Attempts to splits a univariate rational function into partial "
    "fractions over the real algebraic numbers. "
    "If successful, produces a sequence containing the partial fractions. "
    "Else does nothing.",
    "A =\\frac{x^3}{x^{3}-x^2+2x^2-2};\n"
    "B=UnivariatePartialFractions(A);\n"
    "A-\\sum(B)",
    "CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealS",
    "UnivariatePartialFractions",
    innerStandard
  );
  this->addOperationHandler(
    "EqualityToArithmeticExpression",
    CalculatorFunctions::equalityToArithmeticExpression,
    "",
    "Transforms the equality a = b to the arithmetic expression a - b.",
    "EqualityToArithmeticExpression(a = b)",
    "CalculatorFunctions::equalityToArithmeticExpression",
    "EqualityToArithmeticExpression",
    innerStandard
  );
  this->addOperationHandler(
    "PointsImplicitly",
    CalculatorFunctions::innerGetPointsImplicitly,
    "",
    "Returns points on or close to the curve in two dimensions. "
    "Same as plotImplicit but rather than plotting "
    "the curve returns the (x,y) pairs of points found. "
    "The output given as a nx2 matrix of floating point numbers. ",
    "PointsImplicitly((x - 1) (y - 1) - ((x - 1) ^ 2 (y - 1) + 1) ^ 2, (- 2, - 2), (2, 2), (10, 10), 2000)",
    "CalculatorFunctions::innerGetPointsImplicitly",
    "PointsImplicitly",
    innerStandard
  );
  this->addOperationHandler(
    "PlotImplicit",
    CalculatorFunctionsPlot::plotImplicitFunction,
    "",
    "Plots implicitly a curve given by the zeros of an expression in the letters "
    "x and y. The relation between x and y is "
    "assumed continuous at the points where it is defined. "
    "The function has not been optimized for speed, please use with care. "
    "The first argument gives the relation between x and y, the next two arguments give "
    "the lower left and upper right corners of the viewing screen in the format "
    "(lowLeftX, lowLeftY), (upperRightX, upperRightY). "
    "The next argument gives the initial grid "
    "precision in the form (numIntervalsVertical, numIntervalsHorizontal). "
    "The next two arguments give the width and height of the image in the format (width, height). "
    "The last argument is optional and gives an upper limit for the number "
    "of triangles to use (max =20000, default =2000). "
    "The triangle used to generate the implicit plot is algorithmically chosen.",
    "PlotImplicit((x - 1) (y - 1) - ((x - 1)^2 (y - 1) + 1)^2, (- 2, - 2), (2, 2), (10, 10))",
    "CalculatorFunctions::plotImplicitFunction",
    "PlotImplicit",
    innerStandard
  );
  this->addOperationHandler(
    "PlotImplicitShowGrid",
    CalculatorFunctionsPlot::plotImplicitShowGridFunction,
    "",
    "Same as plotImplicit but shows the underlying grid. "
    "The yellow grid is the initial one (specified by the user), "
    "and the gray grid is obtained by a subdivision which depends on the concrete function.",
    "PlotImplicitShowGrid((x- 1) (y- 1)-((x- 1)^2(y- 1) + 1)^2, (-2, -2), (2, 2), (10,10))",
    "CalculatorFunctions::plotImplicitShowGridFunction",
    "PlotImplicitShowGrid",
    innerStandard
  );
  this->addOperationHandler(
    "PlotCoordinateSystem",
    CalculatorFunctionsPlot::plotCoordinateSystem,
    "",
    "Plots a 3d coordinate system, fitting in a box given by two opposite corners.  ",
    "PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4));",
    "CalculatorFunctions::plotCoordinateSystem",
    "PlotCoordinateSystem",
    innerStandard
  );
  this->addOperationHandler(
    "PlotSetProjectionScreen",
    CalculatorFunctionsPlot::plotSetProjectionScreenBasis,
    "",
    "Set the projection screen. Input: two 3d vectors "
    "that give the 2d-basis of the viewing screen. ",
    "PlotCoordinateSystem((- 2, - 2, - 2), (2, 2, 2))\n"
    "+ PlotSetProjectionScreen((1, 0, - 0.1), (0, 1, - 0.2))",
    "CalculatorFunctions::plotSetProjectionScreenBasis",
    "PlotSetProjectionScreen",
    innerStandard
  );
  this->addOperationHandler(
    "PlotSurface",
    CalculatorFunctionsPlot::plotSurface,
    "",
    " Plots a surface. ",
    "%HideLHS x = (R+v*cos(u/2))*cos(u);\n"
    "y = (R+v*cos(u/2))*sin(u);\n"
    "z = v*sin(u/2);\n"
    "R = MakeInputBox (name = radiusBig, value = 2, min = 2, max = 5);\n"
    "r = MakeInputBox (name = radiusSmall, value = 0.6, min = 0.2, max = 1, step = 0.2);\n"
    "uSegments = MakeInputBox(name = uSegments, value = 22, min = 8, max = 40);\n"
    "vSegments = MakeInputBox(name = vSegments, value = 4, min = 2, max = 10);\n"
    "PlotSurface(\n"
    "(x, y, z), u \\in (0, 2\\pi), v \\in (- r, r), \n"
    "color1 = blue, color2 = cyan, numSegments1 = uSegments, numSegments2 = vSegments\n"
    ") + "
    "PlotSurface(\n"
    "( x + 2, z, y), u \\in (0, 2 \\pi), v \\in (- r,r), \n"
    "color1 = red, color2 = pink, numSegments1 = uSegments, numSegments2 = vSegments); ",
    "CalculatorFunctions::plotSurface",
    "PlotSurface",
    innerStandard
  );
  this->addOperationHandler(
    "PlotCurve",
    CalculatorFunctionsPlot::plotParametricCurve,
    "",
    "Plots a curve sitting in 2-dimensional space. "
    "The first and second argument give the x and y "
    "coordinate functions; the curve parameter must be t."
    "The third and fourth argument give the start/finish range for t. "
    "The next argument gives the curve color. "
    "The next argument gives the curve width. "
    "The next argument gives the number of points used to draw the curve. ",
    "a = MakeInputBox(name = \"a\", value = 12, min = 1, max = 25);\n"
    "b = MakeInputBox(name = \"b\", value = 13, min = 1, max = 25);\n"
    "PlotFill(PlotCurve((sin(a t), cos(b t)), 0, 2\\pi, blue, 2, 2000), pink) ",
    "CalculatorFunctions::plotParametricCurve",
    "PlotCurve",
    innerStandard
  );
  this->addOperationHandler(
    "PlotSegment",
    CalculatorFunctionsPlot::plotSegment,
    "",
    "Plots a segment connecting two points. ",
    "PlotSegment( (1,2), (3,4))",
    "CalculatorFunctions::plotSegment",
    "PlotSegment",
    innerStandard
  );
  this->addOperationHandler(
    "PlotMarkSegment",
    CalculatorFunctionsPlot::plotMarkSegment,
    "",
    "Plots a segment mark. ",
    "PlotSegment((1, 2), (3,4)) + PlotMarkSegment((1, 2), (3, 4))",
    "CalculatorFunctions::plotMarkSegment",
    "PlotMarkSegment",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPath",
    CalculatorFunctionsPlot::plotPath,
    "",
    "Plots a straight segment path. The path should be enclosed in parentheses, and color must be indicated. ",
    "PlotPath(((0, 0), (3, 0), (3, 4), (0, 0)), blue)",
    "CalculatorFunctions::plotPath",
    "PlotPath",
    innerStandard
  );
  this->addOperationHandler(
    "PlotSetId",
    CalculatorFunctionsPlot::plotSetId,
    "",
    "Creates an empty plot with a given canvas id. "
    "If you add a nameless plot to a named one the the resulting plot acquires the canvas id. ",
    "PlotSetId(myId) + PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4))",
    "CalculatorFunctions::plotSetId",
    "PlotSetId",
    innerStandard
  );
  this->addOperationHandler(
    "MatchesPattern",
    CalculatorFunctions::matchesPattern,
    "",
    "Checks whether the first argument matches the pattern of the second argument. If no, returns 0. "
    "Pattern parametric entries are indicated with the bound variable notation {{a}}. "
    "If the expression matches the pattern, the return is a command enclosure "
    "in which each of the variables is freed and assigned the matched value. ",
    "MatchesPattern{}(a = b * 10, a = {{c}} * b); "
    "MatchesPattern{}(a = b * 10, a = b * {{c}})",
    "CalculatorFunctions::matchesPattern",
    "MatchesPattern",
    innerStandard
  );
  this->addOperationHandler(
    "GetVariablesExcludeNamedConstants",
    CalculatorFunctions::innerGetFreeVariablesExcludeNamedConstants,
    "",
    "Gets the variables on which the expression depends. "
    "Excludes the named constants. Here, the word ``variables'' is to be thought of as "
    "``free variables'' but the possibility for small distinctions is "
    "reserved (to allow dealing with named constants, reserved keywords, etc.). ",
    "GetVariablesExcludeNamedConstants{}(e^x + x + 5 +\\arctan x + x *y + x^y + x^{y^z}); "
    "GetVariablesIncludeNamedConstants{}(e^x + x + 5 +\\arctan x + x *y + x^y + x^{y^z})",
    "CalculatorFunctions::innerGetFreeVariablesExcludeNamedConstants",
    "GetVariablesExcludeNamedConstants",
    innerStandard
  );
  this->addOperationHandler(
    "GetVariablesIncludeNamedConstants",
    CalculatorFunctions::innerGetFreeVariablesIncludeNamedConstants,
    "",
    "Gets the variables on which the expression depends. Includes the named constants. "
    "Here, the word ``variables'' is to be thought of as "
    "``free variables'' but the possibility for small distinctions is "
    "reserved (to allow dealing with named constants, reserved keywords, etc.). ",
    "GetVariablesExcludeNamedConstants{}(e^x + x +5 +\\arctan x + x *y +x^y+x^{y^z}); "
    "GetVariablesIncludeNamedConstants{}(e^x + x +5 +\\arctan x + x *y +x^y+x^{y^z})",
    "CalculatorFunctions::innerGetFreeVariablesIncludeNamedConstants",
    "GetVariablesIncludeNamedConstants",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPoint",
    CalculatorFunctionsPlot::plotPoint,
    "",
    "<b>Calculus teaching function.</b> Plots a point from x and y coordinate. ",
    "PlotPoint{}((1,2),blue)",
    "CalculatorFunctions::plotPoint",
    "PlotPoint",
    innerStandard
  );
  this->addOperationHandler(
    "Plot2D",
    CalculatorFunctionsPlot::plot2DOverIntervals,
    "",
    "If the second argument is a union of intervals, "
    "replaces the plot command with a sum of Plot2d's in which "
    "the second and third argument are extracted from each of the intervals. ",
    "%UseBracketForIntervals\n"
    "Plot2D{}(\\sin{}x +cos{}x, [0, \\pi]\\cup [2\\pi, 3\\pi), \"blue\",2,8)",
    "CalculatorFunctions::plot2DOverIntervals",
    "Plot2DoverIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "Plot2D",
    CalculatorFunctionsPlot::plot2D,
    "",
    "<b>Calculus teaching function.</b> Makes a 2d plot of a function given in the form "
    "y = f(x). The the second and third argument give the upper and "
    "lower bounds of x. The remaining arguments are optional. "
    "The next argument specifies color. "
    "Next argument gives line width. "
    "Next argument gives number of sample points. "
    "Plots may be added together- adding plots superimposes the plots. ",
    "Plot2D{}(\\sin{}x +cos{}x, 0, 5, \"blue\",2,8)",
    "CalculatorFunctions::plot2D",
    "Plot2D",
    innerStandard
  );
  this->addOperationHandler(
    "IsPlot",
    CalculatorFunctionsPlot::isPlot,
    "",
    "Returns 1 if the argument is a plot, 0 otherwise.",
    "%UseBracketForIntervals \n"
    "IsPlot( Plot2D{}(\\sin{}x +cos{}x, [0, \\pi]\\cup [2\\pi, 3\\pi), \"blue\",2,8))",
    "CalculatorFunctions::isPlot",
    "IsPlot",
    innerStandard
  );
  this->addOperationHandler(
    "PlotFill",
    CalculatorFunctionsPlot::plotFill,
    "",
    "Fills a plot with color. ",
    "PlotFill(Plot2D{}(sqrt(1 - x^2), - 1, 1, \"blue\", 2) + Plot2D(- sqrt(1 - x^2), - 1, 1), \"blue\")",
    "CalculatorFunctions::plotFill",
    "PlotFill",
    innerStandard
  );
  this->addOperationHandler(
    "PlotRectangle",
    CalculatorFunctionsPlot::plotRectangle,
    "",
    "Plots a rectangle. "
    "Arguments format: "
    "PlotRectangle{}((lowerCornerLeftXcoord,  lowerCornerLeftXcoord), (width, height)).",
    "PlotRectangle{}((1, 2), (2, 1))",
    "CalculatorFunctions::plotRectangle",
    "PlotRectangle",
    innerStandard
  );
  this->addOperationHandler(
    "PlotGrid",
    CalculatorFunctionsPlot::plotGrid,
    "",
    "Tells the plot to show axes ticks. Takes as input dummy (non-used) argument. ",
    "PlotGrid{}(0)",
    "CalculatorFunctionsPlot::plotGrid",
    "PlotGrid",
    innerStandard
  );
  this->addOperationHandler(
    "PlotRemoveCoordinateAxes",
    CalculatorFunctionsPlot::plotRemoveCoordinateAxes,
    "",
    "Removes the coordinate axes from a plot. ",
    "PlotRemoveCoordinateAxes{}(0) + Plot2D(x,-1,1);",
    "CalculatorFunctionsPlot::plotRemoveCoordinateAxes",
    "PlotRemoveCoordinateAxes",
    innerStandard
  );
  this->addOperationHandler(
    "PlotLabel",
    CalculatorFunctionsPlot::plotLabel,
    "",
    "Plots a label at a given position. Arguments format: PlotLabel((Xcoord, Ycoord), \"Label\"). ",
    "PlotLabel{}((1,1), \"(1,1)\")",
    "CalculatorFunctionsPlot::plotLabel",
    "PlotLabel",
    innerStandard
  );
  this->addOperationHandler(
    "PlotViewRectangle",
    CalculatorFunctionsPlot::plotViewRectangle,
    "",
    "Creates an empty plot whose sole purpose is to fix "
    "the view rectangle of another plot. "
    "To restrict/expand the view rectangle of another plot, "
    "simply add PlotViewRectangle to the other plot. "
    "Takes as input two pairs of numbers: first pair gives the lower "
    "left corner of the viewing rectangle, "
    "the second argument gives the upper right corner.",
    "Plot2D{}(1/x, -30, 30, \"red\") + PlotViewRectangle((-5,-5), (5,5))",
    "CalculatorFunctionsPlot::PlotViewRectangle",
    "PlotViewRectangle",
    innerStandard
  );
  this->addOperationHandler(
    "PlotWindow",
    CalculatorFunctionsPlot::plotViewWindow,
    "",
    "Creates an empty plot whose sole purpose is to fix the dimensions "
    "(in pixels) of the image in the format "
    "(pixelwidth, pixelHeight). "
    "To modify the dimensions (in pixels) of another plot, "
    "add to it the PlotWindow ``plot''. ",
    "Plot2D{}(1/x, -30, 30, \"red\") + PlotWindow(400,400) + PlotViewRectangle((-5,-5), (5,5))",
    "CalculatorFunctionsPlot::innerPlotWindow",
    "PlotWindow",
    innerStandard
  );
  this->addOperationHandler(
    "Plot2DWithBars",
    CalculatorFunctionsPlot::plot2DWithBars,
    "",
    "<b>Calculus teaching function.</b> Same as plot2D but plots two "
    "functions with bars locked between the two functions, "
    "used to illustrate approximations to definite integrals.",
    "\nA =3/2- ((-3/4+ 1/4 (x))^{2});\nB= (1/4 (x))^{2}+2;\nPlot2DWithBars{}(A, B, 0, 5, 1)",
    "CalculatorFunctionsPlot::plot2DWithBars",
    "Plot2DWithBars",
    innerStandard
  );
  this->addOperationHandler(
    "CompareFunctionsNumerically",
    CalculatorFunctions::innerCompareFunctionsNumerically,
    "",
    "<b>Calculus teaching function.</b> Compares two one-variable functions numerically. "
    "First two arguments give the two functions. "
    "Third and fourth arguments give the interval [a,b] over which to compare. "
    "Fifth argument is optional (default: 50) "
    "and gives the number of sampling points. Sixth argument is optional (default: 0.0001) "
    "and gives the numerical tolerance eps- if two numbers are "
    "at a distance less than eps they are considered equal. ",
    "CompareFunctionsNumerically{}(arctan (x), arcsin(x/\\sqrt{x^2+ 1}),0, 5,50,0.0001);\n"
    "CompareFunctionsNumerically{}(e^x, 1+x +x^2/2+x^3/3!+x^4/4!+x^5/5!+x^6/6!,0, 5,50,0.0001)",
    "CalculatorFunctions::innerCompareFunctionsNumerically",
    "CompareFunctionsNumerically",
    innerStandard
  );
  this->addOperationHandler(
    "CompareExpressionsNumerically",
    CalculatorFunctions::innerCompareExpressionsNumerically,
    "",
    "Compares two expressions numerically. "
    "First two arguments give the two functions. The third argument gives the precision. "
    "All remaining arguments come in pairs, in the form x\\in (2,3), 50. ",
    "CompareExpressionsNumerically{}(\\arctan x + \\arctan y, \\arctan((x +y)/(1- x y)), "
    "0.0001, x\\in(0,0.5), 20, y\\in(0,0.5), 20 );\n"
    "CompareExpressionsNumerically{}(\\arctan x + \\arctan y,"
    "\\arctan((x +y)/(1- x y)), 0.0001, x\\in(0,2), 20, y\\in(0,2), 20 );",
    "CalculatorFunctions::innerCompareExpressionsNumerically",
    "CompareExpressionsNumerically",
    innerStandard
  );
  this->addOperationHandler(
    "CompareExpressionsNumericallyAtPoints",
    CalculatorFunctions::innerCompareExpressionsNumericallyAtPoints,
    "",
    "<b>Calculus teaching function.</b> Compares "
    "two expressions numerically. First two arguments "
    "give the two functions. The third argument gives the precision. "
    "All remaining arguments come in pairs, in the form x\\in (2,3), 50. ",
    "CompareExpressionsNumericallyAtPoints{}("
    "\\arctan x + \\arctan y, \\arctan((x +y)/(1- x y)), 0.001, "
    "(x,y)\\in ((0.5,0.5),(0.3,0.3)));\n"
    "theFun =\\sqrt{x y}-(4 x^{2} y- 1);\n"
    "a =\\frac{-64 x^{3} y^{2}+ 16 x y+y}{32 x^{4} y-8 x^{2}- x};\n"
    "theDiff= d/dx(theFun);\n"
    "num= (d/dx y = 0; theDiff)_2;\n"
    "den = CoefficientOf (d/dx y, theDiff);\n"
    "b= -num/den;\n"
    "CompareExpressionsNumerically(a,b, 0.001, x\\in(0,5),10, y\\in (0,5),10);\n"
    "A = PointsImplicitly(theFun, (0,0),(4,4), (10,10), (400,400));\n"
    "CompareExpressionsNumericallyAtPoints(a,b, 0.01, (x,y)\\in A);",
    "CalculatorFunctions::innerCompareExpressionsNumericallyAtPoints",
    "CompareExpressionsNumericallyAtPoints",
    innerStandard
  );
  this->addOperationHandler(
    "IsInteger",
    CalculatorFunctions::innerIsInteger,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an integer, 0 otherwise. ",
    "IsInteger{}a;\nIsInteger{}1;\nf{}{{a}}=IsInteger{}a;\nf{}1;\nf{}b",
    "Calculator::innerIsInteger",
    "IsInteger",
    innerStandard
  );
  this->addOperationHandler(
    "IsEven",
    CalculatorFunctions::innerIsEven,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an even integer, 0 otherwise. ",
    "i^{{n}} : if IsEven n = (- 1)^(n/2); i^100 ",
    "CalculatorFunctions::innerIsEven",
    "IsEven",
    innerStandard
  );
  this->addOperationHandler(
    "IsSquareFree",
    CalculatorFunctions::innerIsSquareFree,
    "",
    "If the argument is an integer, returns 1 if the "
    "integer is square-free "
    "(no primes in the decomposition of the integer "
    "appear with power greater than one) ",
    "IsSquareFree(6);\n"
    "IsSquareFree(12)",
    "CalculatorFunctions::innerIsSquareFree",
    "IsSquareFree",
    innerStandard
  );
  this->addOperationHandler(
    "IsPower",
    CalculatorFunctions::innerIsPower,
    "",
    "If the argument is an integer, returns 1 if the integer "
    "is plus or minus the power of a prime "
    "(no primes in the decomposition of the integer "
    "appear with power greater than one) ",
    "IsPower(6); IsPower(8)",
    "CalculatorFunctions::innerIsPower",
    "IsPower",
    innerStandard
  );
  this->addOperationHandler(
    "IsSquare",
    CalculatorFunctions::innerIsSquare,
    "",
    "If the argument is an integer, returns 1 if the number is the square of an integer. ",
    "IsSquare(8); IsSquare(16); IsSquare(100);",
    "CalculatorFunctions::innerIsSquare",
    "IsSquare",
    innerStandard
  );
  this->addOperationHandler(
    "[)",
    CalculatorFunctions::innerIntervalLeftClosedFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->left-closed interval.",
    "%UseBracketForIntervals PlotExpressionTree[1,2); PlotExpressionTree(1,2]; PlotExpressionTree[1,2];",
    "CalculatorFunctions::innerIntervalToSequence",
    "[)",
    innerStandard
  );
  this->addOperationHandler(
    "(]",
    CalculatorFunctions::innerIntervalRightClosedFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->right-closed interval.",
    "%UseBracketForIntervals PlotExpressionTree[1,2); PlotExpressionTree(1,2]; PlotExpressionTree[1,2];",
    "CalculatorFunctions::innerIntervalToSequence",
    "(]",
    innerStandard
  );
  this->addOperationHandler(
    "IntervalClosed",
    CalculatorFunctions::innerIntervalClosedFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->closed interval.",
    "%UseBracketForIntervals PlotExpressionTree[1,2); PlotExpressionTree(1,2]; PlotExpressionTree[1,2];",
    "CalculatorFunctions::innerIntervalToSequence",
    "IntervalClosed",
    innerStandard
  );
  this->addOperationHandler(
    "IntervalOpen",
    CalculatorFunctions::innerIntervalOpenFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->open interval.",
    "%UseBracketForIntervals PlotExpressionTree (1,2); ",
    "CalculatorFunctions::innerIntervalOpenFromSequence",
    "IntervalOpen",
    innerStandard
  );
  this->addOperationHandler(
    "IsNonEmptySequence",
    CalculatorFunctions::innerIsNonEmptySequence,
    "",
    "Returns 1 if the input is a non-empty sequence; 0 otherwise. "
    "Does not reduce the expression if it has bound variables.",
    "IsNonEmptySequence(2,3);"
    "IsNonEmptySequence{}((2,3));\n"
    "IsNonEmptySequence{}1;\n"
    "a{}{{x}} = IsNonEmptySequence(x);\n"
    "a{}((2,3))\n",
    "CalculatorFunctions::innerIsNonEmptySequence",
    "IsNonEmptySequence",
    innerStandard
  );
  this->addOperationHandler(
    "IsRational",
    CalculatorFunctions::innerIsRational,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an rational, 0 otherwise. ",
    "IsRational{}a;"
    "IsRational{}- 1;\n"
    "f{}{{a}}=IsRational{}a;\n"
    "IsRational{}1;\n"
    "IsRational{}b",
    "Calculator::innerIsRational",
    "IsRational",
    innerStandard
  );
  this->addOperationHandler(
    "not",
    CalculatorFunctions::innerNot,
    "",
    "If the argument is a small integer, "
    "returns 1 if the argument is 0 and 1 the argument is non-zero. "
    "If the argument is not a small integer, does nothing. ",
    "not{}1;not{}a; not{}0; not{}(3==4)",
    "Calculator::innerNot",
    "not",
    innerStandard
  );

  this->addOperationHandler(
    "FunctionToMatrix",
    CalculatorFunctionsLinearAlgebra::functionToMatrix,
    "",
    "Creates a matrix from a function. "
    "The first argument gives the function, "
    "the second argument the number of rows, the third- the number of columns.",
    "X =FunctionToMatrix{}(A,5,5);\n A{}({{a}},{{b}})=a/b;\n X; \\det {} X",
    "Calculator::innerFunctionToMatrix",
    "FunctionToMatrix",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Atoms::transpose,
    CalculatorFunctions::transpose,
    "",
    "Transposes a matrix of expressions. ",
    "Transpose{}(1,2); (1,2)^t",
    "Calculator::innerTranspose",
    Calculator::Atoms::transpose,
    innerStandard
  );
  this->addOperationHandler(
    "\\det",
    CalculatorFunctions::innerDeterminant,
    "",
    "Tries to convert to a matrix of rationals or matrix of rational "
    "functions and computes the determinant if "
    "not too large.  ",
    "X =FunctionToMatrix{}(A,5,5);\n "
    "A{}({{a}},{{b}})=1/(a +b);\n "
    "X; \\det {} X",
    "CalculatorFunctions::innerDeterminant",
    "Determinant",
    innerStandard
  );
  this->addOperationHandler(
    "DeterminantPolynomial",
    CalculatorFunctions::innerDeterminantPolynomial,
    "",
    "Attempts to convert the entries of the matrix "
    "to polynomials and computes the determinant polynomial. "
    "The matrix must not be larger than 8x8. "
    "Note that this function does not perform "
    "Gaussian elimination, but "
    "rather sums the n! summands of the matrix "
    "polynomial. "
    "This function is meant to be used with "
    "multivariate polynomials with large "
    "number of variables "
    "(in such cases summing n! summands may very well be faster). ",
    "DeterminantPolynomial{}\\left( "
    "\\begin{array}{ccc}"
    "x_{11} & x_{12} & x_{13} \\\\ "
    "x_{21} & x_{22} & x_{23} \\\\ "
    "x_{31} & x_{32} & x_{33} \\end{array} \\right) ",
    "Calculator::innerDeterminantPolynomial",
    "DeterminantPolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "Length",
    CalculatorFunctionsListsAndSets::length,
    "",
    "Returns the length of a sequence. ",
    "Length{}();\n"
    "Length(a, b);\n"
    "Length{}((a,b));\n"
    "Length{}(a,b);\n"
    "Length{}(a);\n"
    "Length{}(Sequence{}a);\n"
    "Length{}(Sequence{}(a,b));\n",
    "CalculatorFunctionsListsAndSets::length",
    "Length",
    innerStandard
  );
  this->addOperationHandler(
    "GetMultiplicandList",
    CalculatorFunctions::innerCollectMultiplicands,
    "",
    "Converts a sum to a sequence containing the summands. ",
    "GetMultiplicandList(a*b*c) ",
    "CalculatorFunctions::innerCollectMultiplicands",
    "GetMultiplicandList",
    innerStandard
  );
  this->addOperationHandler(
    "NormalizeIntervals",
    CalculatorFunctionsIntervals::normalizeIntervals,
    "",
    "Sorts interval union. ",
    "%UseBracketForIntervals\n NormalizeIntervals([2,3] \\cup [5, 7] \\cup [- 1,- 1/2]);",
    "CalculatorFunctions::normalizeIntervals",
    "NormalizeIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "\\cup",
    CalculatorFunctionsIntervals::unionUnionIntervals,
    "",
    "In for the expression a \\cup (b\\cup c) the expression a\\cup b "
    "can be reduced to d, replaces a\\cup(b\\cup c) by a\\cup d. ",
    "%UseBracketForIntervals\n"
    "[3, 3] \\cup ( [3, 7] \\cup [6, 8] );",
    "CalculatorFunctionsIntervals::unionUnionIntervals",
    "UnionUnionIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "\\cup",
    CalculatorFunctionsIntervals::unionIntervals,
    "",
    "If the union of two intervals is one interval, replaces the interval. ",
    "%UseBracketForIntervals\n"
    "[3,7] \\cup [6,8);",
    "CalculatorFunctionsIntervals::unionIntervals",
    "UnionIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "\\cup",
    CalculatorFunctionsListsAndSets::unionEmptySet,
    "",
    "Takes union with the empty set. ",
    "%UseBracketForIntervals\n"
    "[3, 7) \\cup \\emptyset;",
    "CalculatorFunctions::unionEmptySet",
    "UnionEmptySet",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctionsListsAndSets::intersectUnion,
    "",
    "Intersects with the empty set. ",
    "%UseBracketForIntervals\n([3, 7) \\cup (9, 10)) \\cap (4, 8.5);",
    "CalculatorFunctions::intersectUnion",
    "IntersectUnion",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctions::innerIntersectEmptySet,
    "",
    "Intersects with the empty set. ",
    "%UseBracketForIntervals\n  [3,7)\\cap \\emptyset;",
    "CalculatorFunctions::innerIntersectEmptySet",
    "InersectEmptySet",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctionsIntervals::intersectIntervals,
    "",
    "Intersects two intervals. ",
    "%UseBracketForIntervals\n[3, 7) \\cup [6, 8);",
    "CalculatorFunctions::intersectIntervals",
    "InersectIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "BelongsTo",
    CalculatorFunctionsListsAndSets::belongsTo,
    "",
    "Returns 1 if an element belongs to a list, 0 otherwise. ",
    "BelongsTo(x,(x,y,z));\n"
    "BelongsTo(x,(w,y,z));\n",
    "CalculatorFunctionsListsAndSets::belongsTo",
    "BelongsTo",
    innerStandard
  );
  this->addOperationHandler(
    "CompareIntervalsNumerically",
    CalculatorFunctions::innerCompareIntervalsNumerically,
    "",
    "Compares unions of intervals numerically. "
    "First and second arguments: two unions of "
    "intervals to compare. "
    "Third argument: precisions to compare with. ",
    "%UseBracketForIntervals\n  CompareIntervalsNumerically"
    "([3,7)\\cup [8,9], [3,7.00000001)\\cup [7.9999999, 9], 0.0001);",
    "CalculatorFunctions::innerCompareIntervalsNumerically",
    "CompareIntervalsNumerically",
    innerStandard
  );
  this->addOperationHandler(
    "GetOpandList",
    CalculatorFunctions::innerCollectOpands,
    "",
    "Converts a chain of operations to a list. "
    "First argument: operation name. "
    "You can expressions such as StringToAtom(\"+\") "
    "to make an arbitrary expresssion. ",
    "%UseBracketForIntervals\nGetOpandList(StringToAtom(\"\\otimes\"), a\\otimes b \\otimes c ); "
    "GetOpandList(StringToAtom(\"\\cup\"), [a,b]\\cup [c,d] \\cup [e,f] );",
    "CalculatorFunctions::innerCollectOpands",
    "GetOpandList",
    innerStandard
  );
  this->addOperationHandler(
    "GetSummandList",
    CalculatorFunctions::innerCollectSummands,
    "",
    "Converts a sum to a sequence containing the summands. ",
    "GetSummandList(1+a +b) ",
    "CalculatorFunctions::outerGetSummands",
    "GetSummandList",
    innerStandard
  );
  this->addOperationHandler(
    "GetSummand",
    CalculatorFunctions::innerGetSummand,
    "",
    "Extracts the nth summand from a sum, "
    "<b>starts with the ZEROETH summand</b>. ",
    "GetSummand(\\sum_{n =1}^\\infty (- 1)^{2n + 1} x^{2n + 1}/(2n + 1)!, 5 ) ",
    "CalculatorFunctions::innerGetSummand",
    "GetSummand",
    innerStandard
  );
  this->addOperationHandler(
    "GetFirstSummandContaining",
    CalculatorFunctions::innerGetFirstSummandContaining,
    "",
    "Extracts the first summand containing a subexpression. ",
    "GetFirstSummandContaining(a +b+\\sum_{n =1}^\\infty"
    "((- 1)^{2n + 1} x^{2n + 1}/(2n + 1)!), \\sum ) ",
    "CalculatorFunctions::innerGetFirstSummandContaining",
    "GetFirstSummandContaining",
    innerStandard
  );
  this->addOperationHandler(
    "RemoveDuplicates",
    CalculatorFunctions::innerRemoveDuplicates,
    "",
    "Removes duplicates. ",
    "RemoveDuplicates(a,a,c,a,b,a,b,d) ",
    "CalculatorFunctions::innerRemoveDuplicates",
    "RemoveDuplicates",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Atoms::sort,
    CalculatorFunctions::innerSort,
    "",
    "Sorts a sequence. ",
    "Sort(x^2, x^3, x^1, x^{- 1});\n"
    "Sort(3,2,1,4,3);\n"
    "Sort((3,2,3,1));\n"
    "Sort{}((3,2,3,1));\n",
    "CalculatorFunctions::innerSort",
    Calculator::Atoms::sort,
    innerStandard
  );
  this->addOperationHandler(
    "SortDescending",
    CalculatorFunctions::innerSortDescending,
    "",
    "Sorts a sequence in descending order.  ",
    "SortDescending(x^2, x^3, x^1, x^{- 1}) ",
    "CalculatorFunctions::innerSortDescending",
    "SortDescending",
    innerStandard
  );
  this->addOperationHandler(
    "BlocksOfCommutativity",
    CalculatorFunctions::innerGetUserDefinedSubExpressions,
    "",
    "Returns subexpression blocks of commutativity. ",
    "BlocksOfCommutativity(\\sin x + x^2+ 3x y + 18x^{3/4 y}+\\sqrt{2}^{\\sqrt{2}c})",
    "CalculatorFunctions::innerGetUserDefinedSubExpressions",
    "BlocksOfCommutativity",
    innerStandard
  );
  this->addOperationHandler(
    "InvertMatrixVerbose",
    CalculatorFunctions::innerInvertMatrixVerbose,
    "",
    "<b>Calculus teaching function.</b> "
    "Inverts a matrix of rationals if invertible, "
    "in any other case generates an error. Makes a detailed "
    "printout of all Gaussian elimantion steps. ",
    "InvertMatrixVerbose((1, 2), (2, 3))",
    "Calculator::innerInvertMatrixVerbose",
    "InvertMatrixVerbose",
    innerStandard
  );
  this->addOperationHandler(
    "InvertMatrix",
    CalculatorFunctions::innerInvertMatrix,
    "",
    "Inverts a matrix of rationals or algebraic numbers if invertible. ",
    "X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0)); InvertMatrix X- X^{- 1}",
    "CalculatorFunctions::innerInvertMatrix",
    "InvertMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "Trace",
    CalculatorFunctions::innerTrace,
    "",
    "Gets trace of a square matrix. ",
    "X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0)); Trace X",
    "CalculatorFunctions::innerTrace",
    "Trace",
    innerStandard
  );
  this->addOperationHandler(
    "ReverseBytes",
    CalculatorFunctions::innerReverseBytes,
    "",
    "Reverses the bytes of a string. Does not respect utf-8 encoding. ",
    "ReverseBytes{}(\"abc\")",
    "Calculator::innerReverseBytes",
    "ReverseBytes",
    innerStandard
  );
  this->addOperationHandler(
    "Reverse",
    CalculatorFunctions::innerReverseOrder,
    "",
    "Reverses order of elements. This operation will reverse products, lists, etc. "
    "More precisely, the command leaves the fist child in "
    "the internal representation of the object in place "
    "and flips the order of all other children.",
    "Reverse{}(s_1 s_2 s_3 s_4 s_2 s_3 s_1 s_2 s_3 s_4 s_1 s_2 s_3 s_2 s_1)",
    "Calculator::innerReverseOrder",
    "Reverse",
    innerStandard
  );
  this->addOperationHandler(
    "ReverseRecursively",
    CalculatorFunctions::innerReverseOrderRecursively,
    "",
    "Same as Reverse but will apply recursively to the children expressions as well.",
    "ReverseRecursively{}(s_1 s_2 s_3s_4s_2s_3s_1s_2s_3s_4s_1s_2s_3s_2s_1)",
    "Calculator::innerReverseOrderRecursively",
    "ReverseRecursively",
    innerStandard
  );
  this->addOperationHandler(
    "SolveFor",
    CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT,
    "",
    "Tries to solve a polynomial equation with respect to a variable. "
    "The first argument gives the variable expression "
    "(arbitrary expressions allowed) and "
    "the second argument gives the polynomial. "
    "At the moment the calculator "
    "only knows the quadratic formula but Cardano's "
    "formula and the fourth degree formula "
    "will be implemented when need arises. "
    "The solutions of the equation are "
    "returned in a list (empty list for no solution). "
    "Solutions with multiplicity higher than one are repeated. ",
    "SolveFor(x, a x^2 + b x + c);\n"
    "SolveFor(x, a x + b);\n"
    "SolveFor(x, x^3 - 3x^2 - x/2 + 5);",
    "CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT",
    "SolveFor",
    innerStandard
  );

  this->addOperationHandler(
    "\\sqrt",
    CalculatorFunctions::innerSqrt,
    "",
    "Square root of a rational, "
    "implemented as algebraic extension of the rationals. ",
    "\\sqrt 2+\\sqrt 3;(\\sqrt{}2+\\sqrt{}3+\\sqrt{}6)^2",
    "Calculator::innerSqrt",
    "sqrt",
    innerStandard
  );
  this->addOperationHandler(
    "FactorOutNumberContent",
    CalculatorFunctions::innerFactorOutNumberContent,
    "",
    "Factors out the rational number content of an additive expression. "
    "The expression part of the result "
    "must have integer relatively prime coefficients, "
    "with leading coefficient positive. ",
    "TurnOffRules(\"DistributeMultiplication\",\"DistributeMultiplicationConstants\"); "
    "FactorOutNumberContent{}(3x + 9t + 27);",
    "CalculatorFunctions::innerFactorOutNumberContent",
    "FactorOutNumberContent",
    innerStandard
  );
  this->addOperationHandler(
    "FactorOneVarPolyOverRationals",
    CalculatorFunctionsPolynomial::factorPolynomialKronecker,
    "",
    "Factors a one variable polynomial over the rationals "
    "using Kroenecker's method. After clearing denominators, "
    "assume the poly has integer coefficients. If looking "
    "for a divisor of degree k, "
    "plug in k+1 different integer values of the poly to find k+1 "
    "integer values. Factor them. "
    "Each selection of k+1 factors of these integer values "
    "determines the potential values of a divisor polynomial; "
    "the polynomial of degree k can be reconstructed from k+1 values "
    "through Lagrange interpolation. "
    "Try all possible divisors found in this way.",
    "FactorOneVarPolyOverRationals{}(x^{8}-44x^{6}+438x^{4}- 1292x^{2}+529);\n"
    "FactorOneVarPolyOverRationals{}(x^{8}+2x^{7}-3x^{6}-4x^{5}+6x^{4}+2x^{3}- 13x^{2}+ 1)",
    "CalculatorFunctionsPolynomial::factorPolynomial",
    "FactorOneVarPolyOverRationals",
    innerStandard
  );
  this->addOperationHandler(
    "FactorUnivariatePolynomialOverRationals",
    CalculatorFunctionsPolynomial::factorPolynomialFiniteFields,
    "",
    "Factors a one variable polynomial over the rationals "
    "using finite field methods. "
    "At the time of writing, the method used is probabilistic "
    "but with high chance of success. ",
    "%HideLHS\n"
    "FactorUnivariatePolynomialOverRationals{}("
    "182903 x^{11}+101813 x^{10}-68963 x^{9}+32574 x^{8}+"
    "11015 x^{7}+453344 x^{6}+106241 x^{5}+115598 x^{4}+"
    "102 x^{3}+145 x^{2}+12276 x+261632"
    ");\n",
    "CalculatorFunctionsPolynomial::factorPolynomialFiniteFields",
    "FactorUnivariatePolynomialOverRationals",
    innerStandard
  );
  this->addOperationHandler(
    "FactorPolynomialOverRationals",
    CalculatorFunctionsPolynomial::factorPolynomialRational,
    "",
    "Runs the Kronecker algorithm with a computation throttle; "
    "if unsuccessful, runs finite field algorithms. ",
    "%HideLHS\n"
    "FactorPolynomialOverRationals{}(\n"
    "169 x^{11}-312 x^{10}+430 x^{9}-524 x^{8}+595 x^{7}"
    "-644 x^{6}+581 x^{5}-440 x^{4}+318 x^{3}-214 x^{2}+127 x-56\n"
    ");\n",
    "CalculatorFunctionsPolynomial::factorPolynomialRational",
    "FactorPolynomialOverRationals",
    innerStandard
  );
  this->addOperationHandler(
    "FactorOneVariablePolynomialModPrime",
    CalculatorFunctionsPolynomial::factorPolynomialModPrime,
    "",
    "Factors a one variable polynomial over a given prime field Z/pZ "
    "using the Cantor-Zassenhaus algorithm. "
    "First argument = polynomial. "
    "Second argument = prime number.",
    "FactorOneVariablePolynomialModPrime{}(x^5+x^4+2x^3-x^2-x-1, 1009);\n",
    "CalculatorFunctionsPolynomial::factorPolynomialModPrime",
    "FactorOneVariablePolynomialModPrime",
    innerStandard
  );
  this->addOperationHandler(
    "FactorInteger",
    CalculatorFunctions::factorInteger,
    "",
    "Factors an integer, assuming the integer is small enough. ",
    "FactorInteger(10001011);\n"
    "FactorInteger(1);\n"
    "FactorInteger(- 10);\n"
    "FactorInteger(0);",
    "Calculator::innerFactorInteger",
    "FactorInteger",
    innerStandard
  );
  this->addOperationHandler(
    "CoefficientsPowersOf",
    CalculatorFunctions::innerCoefficientsPowersOf,
    "",
    "Extracts the coefficients of the powers of the first argument in the second argument.",
    "CoefficientsPowersOf(x, a x^2+ b *3 x +c +\\pi +3)",
    "CalculatorFunctions::innerCoefficientsPowersOf",
    "CoefficientsPowersOf",
    innerStandard
  );
  this->addOperationHandler(
    "ConstantTerm",
    CalculatorFunctions::innerConstTermRelative,
    "",
    "Extracts term constant relative to the variable in the first argument. ",
    "ConstantTerm(y,  x y x +3 +2z)",
    "CalculatorFunctions::innerConstTermRelative",
    "ConstantTerm",
    innerStandard
  );
  this->addOperationHandler(
    "CoefficientOf",
    CalculatorFunctions::coefficientOf,
    "",
    "Gets the coefficient of the first argument in the second. ",
    "CoefficientOf(y, x * x * y + x * z * y * z + x * y * x)",
    "CalculatorFunctions::coefficientOf",
    "CoefficientOf",
    innerStandard
  );
  this->addOperationHandler(
    "IsLinearOrConstantIn",
    CalculatorFunctions::innerIsLinearOrConstantIn,
    "",
    "Returns one if the second argument is linear in the first. "
    "All variables are treated as non-constants. ",
    "IsLinearOrConstantIn(x, x^2 + 1);\n"
    "IsLinearOrConstantIn(x, x y + 1);\n"
    "IsLinearOrConstantIn(x, x + 1);\n"
    "IsLinearOrConstantIn(x, x \\pi + 1);",
    "CalculatorFunctions::innerIsLinearOrConstantIn",
    "IsLinearOrConstantIn",
    innerStandard
  );
  this->addOperationHandler(
    "IsProductLinearOrConstantTermsIn",
    CalculatorFunctions::isProductLinearOrConstantTermsIn,
    "",
    "Returns true if the expression is a product of linear or constant terms. "
    "Although this is subject to change, at the moment "
    "powers of linear terms are not considered linear. ",
    "TurnOffRules(\"DistributeMultiplication\");\n"
    "IsProductLinearOrConstantTermsIn(x, x^2+ 1);\n"
    "IsProductLinearOrConstantTermsIn(x, x^2- 1);\n"
    "IsProductLinearOrConstantTermsIn(x, (x- 1)(x + 1));\n"
    "IsProductLinearOrConstantTermsIn(x, (2x + 1)(x \\pi + 1));\n"
    "IsProductLinearOrConstantTermsIn(x, (2x +y)(x \\pi + 1));\n",
    "CalculatorFunctions::isProductLinearOrConstantTermsIn",
    "IsProductLinearOrConstantTermsIn",
    innerStandard
  );
  this->addOperationHandler(
    "IsProductTermsUpToPower",
    CalculatorFunctions::innerIsProductTermsUpToPower,
    "",
    "Returns true if the expression is a product of terms of power up to the given power. "
    "Although this is subject to change, at the moment "
    "powers of linear terms are not considered linear. ",
    "TurnOffRules(\"DistributeMultiplication\");\n"
    "IsProductTermsUpToPower(x^2, x^2+ 1);\n"
    "IsProductTermsUpToPower(x^3,x( x^2- 1));\n"
    "IsProductTermsUpToPower(x^2,x( x^2- 1));\n"
    "IsProductTermsUpToPower(x, (x- 1)(x + 1));\n"
    "IsProductTermsUpToPower(x, (2x + 1)(x \\pi + 1));\n"
    "IsProductTermsUpToPower(x, (2x +y)(x \\pi + 1));\n",
    "CalculatorFunctions::innerIsProductTermsUpToPower",
    "IsProductTermsUpToPower",
    innerStandard
  );
  this->addOperationHandler(
    ";",
    CalculatorBasics::meltBrackets,
    "",
    "Melts down a layer of parenthesis around a list of "
    "commands prepended with the Melt operation.\n "
    "More precisely, cycles through all immediate children of the expression.\n"
    "If the k^th child X is a list of two elements starting with Melt, "
    "then the operation does the following. "
    "If the second child Y of X is a list starting with EndStatement(;), "
    "then X is replaced with the second, third, ... children of Y. "
    "If Y is not a list starting with EndStatement, X is replaced with Y.",
    "c = (a =b);\n"
    "a;\n"
    "c;\n"
    "a;\n"
    "d = (e = f; g=h);\n"
    "d;\n"
    "e;\n"
    "Melt{}d;\n"
    "e;\n"
    "g;",
    "Calculator::outerMeltBrackets",
    "MeltBrackets",
    outerStandard
  );
  this->addOperationHandler(
    "=",
    CalculatorBasics::checkRule,
    "",
    "Checks whether the rule is of the form A = A, and substitutes "
    "the expression with an error if that is the case. "
    "This usually happens when a general rule is entered twice. "
    "In the following example, we try to redefine the associative rule "
    "of the calculator. This fails because the associative rule is already implemented: "
    "the left hand side of the below expression is substituted with a*(b*c), and thus the rule becomes "
    "a*(b*c)=a*(b*c), which clearly is infinite substitution.",
    "%LogEvaluation\n({{a}}*{{b}})*{{c}}=a*(b*c);  ",
    "Calculator::outerCheckRule",
    "CheckAutoEquality",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddStringToString,
    this->opString(),
    this->opString(),
    "Concatenates strings. ",
    "\"hello \"+ \"world\"",
    "CalculatorFunctionsBinaryOps::innerAddStringToString",
    "AddStrings",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddRatToRat,
    this->opRational(),
    this->opRational(),
    "Adds two rational numbers. ",
    "2 + 3",
    "CalculatorFunctionsBinaryOps::innerAddRatToRat",
    "AddRationals",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational,
    this->opRational(),
    this->opEltZmodP(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational",
    "AddRationalToElementZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational,
    this->opEltZmodP(),
    this->opRational(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational",
    "AddElementZmodPToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational",
    "AddElementZmodPToElementZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToPolynomialModuloInteger,
    this->opPolynomialModuloInteger(),
    this->opPolynomialModuloInteger(),
    "Adds polynomials modulo integers. ",
    "a = PolynomialModP(x^2 + x + 7, 5);\n"
    "b = PolynomialModP(x^2 + 1, 5);\n"
    "a + b",
    "CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToPolynomialModuloInteger",
    "AddPolynomialZmodPToPolynomialZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToInteger,
    this->opPolynomialModuloInteger(),
    this->opRational(),
    "Adds polynomials modulo integers. ",
    "a = PolynomialModP(x^2 + x + 7, 5);\n"
    "b = 3;\n"
    "a + b",
    "CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToInteger",
    "AddPolynomialZmodPToInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger,
    this->opPolynomialModuloPolynomialModuloInteger(),
    this->opPolynomialModuloPolynomialModuloInteger(),
    "Adds polynomials modulo integers. ",
    "p = PolynomialModP(x^2 + x + 7, 5);\n"
    "a = PolynomialModP(x^2 + 1, 5);\n"
    "b = PolynomialModP(x^2, 5);\n"
    "a mod p;\n"
    "b mod p;\n"
    "a+b",
    "CalculatorFunctionsBinaryOps::innerAddPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger",
    "AddPolynomialModPolynomialModPToPolynomialModPolynomialModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber,
    this->opAlgebraicNumber(),
    this->opAlgebraicNumber(),
    "Adds two algebraic numbers. ",
    "\\sqrt {2}+ \\sqrt {3} + \\sqrt{6}",
    "CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber",
    "AddAlgebraicNumberToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Adds algebraic number to rational. ",
    "1/(\\sqrt {2}+ 1+\\sqrt{3}+\\sqrt{6})",
    "CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber",
    "AddAlgebraicNumberToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber,
    this->opRational(),
    this->opAlgebraicNumber(),
    "Adds rational to algebraic number. ",
    "1/(1+\\sqrt {2}+\\sqrt{}6)",
    "CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber",
    "AddRationalToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational,
    this->opDouble(),
    this->opRational(),
    "Adds double or rational to a double or rational approximately "
    "using the built-in cpp addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational",
    "AddDoubleToRational",
    innerStandard
  );
  // must come before outer plus:
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddPlotToPlot,
    this->opCalculusPlot(),
    this->opCalculusPlot(),
    "Superimposes two plots. ",
    "Plot2D{}(sin{}(x), -5, 5) + Plot2D{}(1/sin{}(x ), 0.1, 3.041592654)",
    "CalculatorFunctionsBinaryOps::innerAddPlotToPlot",
    "AddPlots",
    innerStandard
  );
  this->addOperationHandler(
    "ScaleToLeadingUnit",
    CalculatorFunctions::innerScaleToLeadingUnit,
    "",
    "Rescales an expression over the rationals so that the leading term has coefficient 1. ",
    "ScaleToLeadingUnit(1/2 x + 1/3 y+ 1/7 a b)",
    "Calculator::innerScaleToLeadingUnit",
    "ScaleToLeadingUnit",
    innerStandard
  );
  List<std::string> additiveOperations = List<std::string>({"+", "-"});
  this->addOperationHandler(
    "+",
    CalculatorFunctions::sortTerms,
    "",
    "Sorts terms (over the rationals). "
    "Similar to AddTerms but doesn't combine monomial coefficients or drop zeroes. ",
    "2+3+a+2a+b+1+a",
    "Calculator::sortTerms",
    "SortTerms",
    Function::Options::outerOffByDefault(),
    &additiveOperations
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctions::addTerms,
    "",
    "Collects all terms (over the rationals), adding up terms "
    "proportional up to a rational number. "
    "Zero summands are removed, unless zero is the only term left. ",
    "1+a-2a_1+ 1/2+a_1",
    "Calculator::addTerms",
    "AddTerms",
    outerStandard,
    &additiveOperations
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctionsPolynomial::combineFractionsCommutativeWithInternalLibrary,
    "",
    "Combines fractions under the assumption that all participants commute. "
    "Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= (a * lcm(b,d)/b+c*lcm(b,d)/d)/(lcm(b,d)); "
    "Please note that this transformation is not correct if b and d do not commute. ",
    "a/b+c/d;\n"
    "z=(x-2)(x+1);\n"
    "w=(x-3)(x+1);\n"
    "1/z+1/w;\n"
    "1/(x+sqrt(20))+ 1/(x+sqrt(3));",
    "CalculatorFunctionsPolynomial::combineFractionsCommutativeWithInternalLibrary",
    "CommonDenominator",
    outerStandard
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctions::combineFractionsCommutative,
    "",
    "Combines fractions on condition that all participants commute. "
    "Similar to the CommonDenominator rule but does not compute least common multiples."
    "Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= (a *d+c*b)/(d*b); "
    "Please note that this transformation is not correct if b and d do not commute. ",
    "TurnOffRules(\"CommonDenominator\");\n"
    "a/b+c/d;\n"
    "z=(x-2)(x+1);\n"
    "w=(x-3)(x+1);\n"
    "1/z+1/w",
    "CalculatorFunctions::outerCombineFractionsCommutative",
    "CommonDenominatorSimple",
    outerStandard
  );
  this->addOperationHandler(
    "+",
    CalculatorBasics::combineFractions,
    "",
    "Combines fractions. Equivalent to {{a}}/{{b}}+{{c}}= (a +c*b)/b; ",
    "f{}{{x}}= (2x +3)/(2x + 1);\ng{}{{y}}= (y-2)/(y+3);\ng{}f{}z;\nf{}g{}z",
    "Calculator::outerCombineFractions",
    "CommonDenominatorOneNonFraction",
    outerStandard
  );

  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialRational(),
    "Adds a rational to a polynomial. ",
    "1+Polynomial{}\\lambda; Polynomial{}\\lambda + 1",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddRationalToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opRational(),
    "Adds a polynomial to a rational. ",
    "1+Polynomial{}\\lambda; Polynomial{}\\lambda + 1",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialRational(),
    "Adds a polynomial to a polynomial. ",
    "x = 1 + Polynomial{} \\lambda;\n"
    "x + x",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) + PolynomialAlgebraicNumbers(-\\sqrt{3}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialOverAlgebraicNumbersToPolynomialOverAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}) +PolynomialAlgebraicNumbers(\\sqrt{3}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddAlgebraicNumberToPolynomialOverAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opAlgebraicNumber(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) +PolynomialAlgebraicNumbers(-\\sqrt{3})",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialOverAlgebraicNumbersToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "1 + PolynomialAlgebraicNumbers(\\sqrt{12}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddRationalToPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opRational(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) + 1",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialAlgebraicNumbersToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opAlgebraicNumber(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(x) +\\sqrt{2}",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "Polynomial{}x + PolynomialAlgebraicNumbers(\\sqrt{2}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opPolynomialRational(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{2}x) + Polynomial{}x",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialAlgebraicNumbersToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialRational(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "\\sqrt{2}+PolynomialAlgebraicNumbers(x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial",
    "AddAlgebraicNumberToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrRationalFunctionToRationalFunction,
    this->opRationalFunction(),
    this->opRationalFunction(),
    "Adds a rational function to a rational function. ",
    "WeylDimFormula{}(a_2, (0,3)) + WeylDimFormula{}(a_2, (3,0)) + 4 WeylDimFormula{}(a_2, (1,1)) ",
    "CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF",
    "AddRationalFunctionToRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational,
    this->opRational(),
    this->opDouble(),
    "Adds double or rational to a double or rational approximately using the "
    "built-in cpp addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational",
    "AddRationalToDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational,
    this->opDouble(),
    this->opDouble(),
    "Adds double or rational to a double or rational approximately using the built-in cpp "
    "addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational",
    "AddDoubleToDouble",
    innerStandard
  );

  this->addOperationHandler(
    "+",
    CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix,
    "",
    "Adds two matrices.",
    "\\begin{pmatrix} 1& 2 \\\\ 2 & 3\\end{pmatrix} + \n"
    "\\begin{pmatrix} 1& 3 \\\\ 2 & 3\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix",
    "AddMatrices",
    innerStandard
  );

  this->addOperationHandler(
    "AugmentMatrixToTheRight",
    CalculatorFunctionsBinaryOps::innerAugmentMatrixToTheRight,
    "",
    "Augments matrix to the right with another matrix. "
    "Pastes the content of the second matrix to the right of the first matrix."
    "The matrices must have the same number of rows. ",
    "AugmentMatrixToTheRight( ((1,1),(2,2)), ((0,0),(1,1)))",
    "CalculatorFunctionsBinaryOps::innerAugmentMatrixToTheRight",
    "AugmentMatrixToTheRight",
    innerStandard
  );
  this->addOperationHandler(
    "AugmentMatrixBelow",
    CalculatorFunctionsBinaryOps::innerAugmentMatrixBelow,
    "",
    "Augments matrix below by another matrix. "
    "Pastes the content of the second matrix below the first matrix."
    "The matrices must have the same number of columns. ",
    "AugmentMatrixBelow( ((1,1),(2,2)), ((0,0),(1,1)))",
    "CalculatorFunctionsBinaryOps::innerAugmentMatrixBelow",
    "AugmentMatrixBelow",
    innerStandard
  );
  this->addOperationHandler(
    "\\oplus",
    CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix,
    "",
    "Direct sum of two matrices, non-commutative.",
    "\\begin{pmatrix} 1& 2\\\\ 2& 3\\end{pmatrix}\\oplus\n"
    "\\begin{pmatrix} - 1& 3\\\\ -2& -3\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix",
    "DirectSumMatrices",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::multiplyMatrixByMatrix,
    "",
    "Multiplies matrices.",
    "\\begin{pmatrix} 1&  2 \\\\ 3& 5\\end{pmatrix}"
    "\\begin{pmatrix} 1& -2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix",
    "MultiplyMatrixByMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix,
    "",
    "Multiplies matrices.",
    "(1,2)\\begin{pmatrix} 1& 2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix",
    "MultiplySequenceByMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::multiplyMatrixBySequence,
    "",
    "Multiplies a matrix standing on the left by a "
    "sequence/list (matrix-row) standing on the right.\n"
    "This operation is valid only if the the matrix on the right is a column-vector.",
    "(1,2)\\begin{pmatrix} 1& 2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix",
    "MultiplySequenceByMatrix",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddSequenceToSequence,
    this->opSequence(),
    this->opSequence(),
    "Adds two sequences (termwise), provided the sequences "
    "have the same number of entries. In case the entries "
    "of the sequences are elements of a base field, corresponds to vector addition.",
    "v_{1}= (1, 2, 3);\n"
    "v_{2}= (1, 3, 2);\n"
    "v_{3}= (3, 1, 1);\n"
    "v_{4}= (-2, 2, 2);\n"
    "1/2v_{1}+ 1/2v_{2}+7/8v_{3}+ 13/16v_{4}",
    "CalculatorFunctionsBinaryOps::innerAddSequenceToSequence",
    "AddSequences",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor,
    this->opMatrixTensorRational(),
    this->opMatrixTensorRational(),
    "Adds two matrices.",
    "A = MakeMatrixTensorForm{}((5, 8), (3, 5));\n"
    "3A * A - A;",
    "CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor",
    "AddMatrixTensorToMatrixTensor",
    innerStandard
  );
  this->addOperationHandler(
    "-",
    CalculatorBasics::minus,
    "",
    "Transforms a - b to a +(- 1) * b and - b to (- 1) * b. Equivalent to a rule "
    "-{{b}}=MinusOne * b; {{a}}-{{b}}=a + MinusOne * b", "- 1 + (- 5)",
    "CalculatorBasics::minus",
    "Minus",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsDifferentiation::interpretAsDifferential,
    "",
    "If circumstances imply it, interprets an atom of the form dx as Differential {} x. ",
    "(\\int x) dx; \\int x (1 + x) dx; \\int_2^3 x dx; \\int_2^3 x (1 + x) dx",
    "CalculatorFunctions::interpretAsDifferential",
    "InterpretAsDifferential",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsIntegration::integralOperator,
    "",
    "Transforms integral notation into an integral expression. ",
    "(\\int x)dx;\n"
    "\\int x (1+x) dx;\n"
    "\\int_2^3 x dx;\n"
    "\\int_2^3 x(1+x)dx",
    "CalculatorFunctions::integralOperator",
    "IntegralOperatorFromProduct",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::innerSumTimesExpressionToSumOf,
    "",
    "Transforms \\sum\\limits_{b}^c* a to (\\sum\\limits_b^c){} a. ",
    "PlotExpressionTree(  \\sum\\limits_{b}^c);\n"
    "PlotExpressionTree( \\sum\\limits_{b}^c*a) ",
    "CalculatorFunctions::innerSumTimesExpressionToSumOf",
    "SumProductNotationToOperator",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::innerSumTimesExpressionToSumOf,
    "",
    "Transforms \\sum\\limits_{b}^c* a to (\\sum\\limits_b^c){} a. ",
    "PlotExpressionTree(  \\sum\\limits_{b}^c);\n"
    "PlotExpressionTree( \\sum\\limits_{b}^c*a) ",
    "CalculatorFunctions::innerSumTimesExpressionToSumOf",
    "SumProductNotationToOperatorRelativeToDivision",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerHandleUnderscorePowerLimits,
    "",
    "Handles expressions of the form \\limits_a^b",
    "\\limits_a^b; \\limits^b_a",
    "CalculatorFunctions::innerHandleUnderscorePowerLimits",
    "LimitBoundaryNotationPower",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::innerHandleUnderscorePowerLimits,
    "",
    "Handles expressions of the form \\limits_a^b",
    "\\limits_a^b; \\limits^b_a",
    "CalculatorFunctions::innerHandleUnderscorePowerLimits",
    "LimitBoundaryNotationUnderscore",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny,
    "",
    "Replaces Differentiate{}(x)*a by Differentiate{}(x, a).",
    "d/dx (1/x) ",
    "CalculatorFunctions::differentiateWithRespectToXTimesAny",
    "DifferentiateWithRespectToXTimeAny",
    outerStandard
  );

  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat,
    this->opRational(),
    this->opEltZmodP(),
    "Multiplies elements of Z_p. ",
    "(2 mod 7) * 3",
    "CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat",
    "MultiplyRationalByElementZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Multiplies elements of Z_p. ",
    "(2 mod 7) * 3",
    "CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat",
    "MultiplyElementZmodPByElementZModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber,
    this->opAlgebraicNumber(),
    this->opAlgebraicNumber(),
    "Multiplies two algebraic numbers. ",
    "\\sqrt{}2(\\sqrt {2}* \\sqrt {3} +\\sqrt{}2)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber",
    "MultiplyAlgebraicNumberByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber,
    this->opRational(),
    this->opAlgebraicNumber(),
    "Multiplies two algebraic number by rational. ",
    "2(\\sqrt {2}+\\sqrt{}3)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber",
    "MultiplyRationalByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRationalByRational,
    this->opRational(),
    this->opRational(),
    "Multiplies two rationals. ",
    "2 * 3",
    "CalculatorFunctionsBinaryOps::innerMultiplyRationalByRational",
    "MultiplyRationals",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational,
    this->opRational(),
    this->opDouble(),
    "Multiplies rational by a double approximately using the built-in "
    "cpp multiplication returning double. "
    "The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "z=101^20;\n"
    "DoubleValue{}(z);\n"
    "DoubleValue{}(z) +DoubleValue{}(1)-DoubleValue{}(z);\n"
    "(z+ 1)-z;\n"
    "y = 101^200;\n"
    "DoubleValue(y)",
    "CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational",
    "MultiplyRationalByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational,
    this->opDouble(),
    this->opRational(),
    "Multiplies rational by a double approximately using the built-in cpp "
    "multiplication returning double. The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "DoubleValue{}((101)^{20});\n"
    "DoubleValue{}(DoubleValue{}((101)^{20}) + DoubleValue{}(1)) - DoubleValue{}(101^{20})",
    "CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational",
    "MultiplyDoubleByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational,
    this->opDouble(),
    this->opDouble(),
    "Multiplies rational by a double approximately using the built-in cpp multiplication "
    "returning double. The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "DoubleValue{}((101)^{20});\n"
    "DoubleValue{}(DoubleValue{}((101)^{20}) +DoubleValue{}(1))-DoubleValue{}(101^{20})",
    "CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational",
    "MultiplyDoubleByDouble",
    innerStandard
  );

  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opRational(),
    "Multiplies polynomial by a rational (polynomial comes first). ",
    "2*Polynomial{}(a +b);\n"
    "Polynomial{}(a +b)/2;\n"
    "Polynomial{}((a +b)^3)*Polynomial{}((a +c)^3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialRational(),
    "Multiplies rational by a polynomial (rational comes first). ",
    "2*Polynomial{}(a + b);\n"
    "Polynomial{}(a + b)/2;\n"
    "Polynomial{}((a + b)^3) * Polynomial{}((a + c)^3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyRationalByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialRational(),
    "Multiplies two polynomials. ",
    "2*Polynomial{}(a +b);\n"
    "Polynomial{}(a +b)/2;\n"
    "Polynomial{}((a +b)^3) * Polynomial{}((a +c)^3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opPolynomialAlgebraicNumbers(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*PolynomialAlgebraicNumbers{}(\\sqrt{6}y);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialAlgebraicNumbersByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPByPolynomialModP,
    this->opPolynomialModuloInteger(),
    this->opPolynomialModuloInteger(),
    "Multiply two polynomials modulo an integer. ",
    "a = PolynomialModP( x^2 + x + 1, 7 );\n"
    "b = PolynomialModP( x^3 + x + 1, 7 );\n"
    "a * b;\n"
    "a * PolynomialModP( x^3 + x + 1, 5 );",
    "CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPByPolynomialModP",
    "MultiplyPolynomialModPByPolynomialModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP,
    this->opPolynomialModuloPolynomialModuloInteger(),
    this->opPolynomialModuloPolynomialModuloInteger(),
    "Multiply two polynomials modulo an integer. ",
    "p = PolynomialModP( x^4 + x + 1, 7 );\n"
    "a = PolynomialModP( x^3 + x + 1, 7 );\n"
    "b = PolynomialModP( x + 2, 7 );\n"
    "f = a mod p;\n"
    "g = b mod p;\n"
    "f * g",
    "CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP",
    "MultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opAlgebraicNumber(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*\\sqrt{6};",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialAlgebraicNumbersByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialAlgebraicNumbers(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "\\sqrt{6}*PolynomialAlgebraicNumbers{}(\\sqrt{3}x);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyAlgebraicNumberByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialAlgebraicNumbers(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*2;",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyRationalByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opAlgebraicNumber(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "Polynomial{}(x)*\\sqrt{2};",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialRational(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "\\sqrt{3} * PolynomialAlgebraicNumbers{}(x);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyAlgebraicNumberByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor,
    this->opMatrixTensorRational(),
    this->opMatrixTensorRational(),
    "Multiplies matrix rational by matrix tensor. ",
    "M = MakeMatrixTensorForm{}((1, 1), (0, 1));\n"
    "M;\n"
    "M * M;\n"
    "M * M * M;\n"
    "M * M * M * M;\n"
    "2 * M",
    "CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor",
    "MultiplyMatrixTensorByMatrixTensor",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor,
    this->opRational(),
    this->opMatrixTensorRational(),
    "Multiplies rational by matrix tensor form. ",
    "M = MakeMatrixTensorForm{}((1, 3, 5), (- 1, 1, 0), (2, 2, 7));\n"
    "M;\n"
    "M * M;\n"
    "M * M * M;\n"
    "M * M * M * M;\n"
    "2 * M ",
    "CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor",
    "MultiplyRationalByMatrixTensor",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::multiplyAtoXtimesAtoYequalsAtoXplusY,
    "",
    "Collects multiplicand exponents. ",
    "x*(x*y)*x*(x*x^3*x);\n"
    "x^{2/3}x^{1/2};\n"
    "(x>0)=1;\n"
    "x^{2/3}x^{1/2};\n"
    "(x^{1/2})^2;",
    "Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY",
    "CombineExponents",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::multiplyByOne,
    "",
    "Rule 1*{{anything}} = anything.",
    "x*1;\n"
    "x*1-x ",
    "Calculator::innerMultiplyByOne",
    "MultiplyByOne",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::outerCommuteConstants,
    "",
    "Rule that commutes constants to the left-most positions. "
    "Provided that a is a constant number (built in) and b is not, replaces b*a by a*b. ",
    "x 6^{1/3}; (x 10^{1/2})^{1/3}",
    "CalculatorFunctions::outerCommuteConstants",
    "CommuteConstants",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::outerMergeConstantRadicals,
    "",
    "If a and b are constants, replaces a^{c}b^c by (a b)^c.",
    "\\sqrt{}2 \\sqrt{}3",
    "CalculatorFunctions::outerMergeConstantRadicals",
    "MergeConstantRadicals",
    outerStandard
  );
  List<std::string> multiplicativeOperations = List<std::string>({"*"});
  this->addOperationHandler(
    "*",
    CalculatorBasics::associate,
    "",
    "Associative law: reorders the multiplicative tree in standard form. ",
    "(a*b)*(c*(d*e)*f) - a*b*c*d* e *f;(a*b)*(c*(e * d)*f) - a*b*c*d* e *f",
    "Calculator::outerAssociate",
    "AssociativeRule",
    outerStandard,
    &multiplicativeOperations
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate,
    "",
    "Commutative law: replaces a*b by b*a provided that "
    "1) a and and b depend on exactly one user-defined variable, "
    "2) a is not a constant and 3) a>b as an expression. ",
    "x(x + 1)^{- 1}x; x(y+ 1)^{- 1}x; (\\sin x ) x (\\cos x)",
    "CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate",
    "CommuteAtimesBtimesCifUnivariate",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::outerCommuteAtimesBifUnivariate,
    "",
    "Commutative law: replaces a*b by b*a provided that "
    "1) a and and b depend on exactly one user-defined variable, "
    "2) a is not a constant and 3) a>b as an expression. ",
    "x(x + 1)^{- 1}x; x(y+ 1)^{- 1}x",
    "CalculatorFunctions::outerCommuteAtimesBifUnivariate",
    "CommuteIfUnivariate",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::extractBaseMultiplication,
    "",
    "Pulls rationals in the front of multiplicative terms.",
    "2*((3*c)*(4*d)); 3*((a*(d-d))b*c)",
    "Calculator::outerExtractBaseMultiplication",
    "ConstantExtraction",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::associateTimesDivision,
    "",
    "Associative law w.r.t. division. ",
    "a*(b/c); (a*b)/c-a*(b/c)",
    "Calculator::outerAssociateTimesDivision",
    "AssociateTimesDivision",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::cancelMultiplicativeInverse,
    "",
    "Cancels multiplicative inverse. ",
    "(a*b)/b; (a/b)*b",
    "Calculator::innerCancelMultiplicativeInverse",
    "CancelMultiplicativeInverse",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::distributeTimes,
    "",
    "Distributive law (left and right).",
    "(a + b) * c;\n"
    "a * (b + c)",
    "Calculator::outerDistributeTimes",
    "DistributeMultiplication",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::distributeTimesConstant,
    "",
    "Distributive law (left and right), acts only if the "
    "multiplicand is a constant. This rule is "
    "overridden by DistributeMultiplication, unless "
    "the DistributeMultiplication rule is turned off "
    "(which is the intended use case of this rule).",
    "a(b+c); TurnOffRules(DistributeMultiplication); a(b+c); -5(b+c) ",
    "Calculator::outerDistributeTimes",
    "DistributeMultiplicationConstants",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::outerDivideReplaceAdivBpowerItimesBpowerJ,
    "",
    "Rule: (a/x^t)x^s =a x^{s-t}.",
    "(a/x) x^{- 1};\n"
    "(a/x^2) x^{3};\n"
    "(a/x) x^{3};\n"
    "(a/x^2) x",
    "CalculatorFunctions::outerDivideReplaceAdivBpowerItimesBpowerJ",
    "DivideAdivBpowerItimesBpowerJ",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction,
    this->opRational(),
    this->opRationalFunction(),
    "Multiplies rational number by a rational function.",
    "WeylDimFormula{}(a_2, (0,3)) + WeylDimFormula{}(a_2, (3,0)) + 4 WeylDimFormula{}(a_2, (1,1)) ",
    "CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyRationalByRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction,
    this->opPolynomialRational(),
    this->opRationalFunction(),
    "Multiplies rational function by a rational function.",
    "Polynomial{}(x +1)MakeRationalFunction{}(\frac{-2x -2}{x^2+x });\n"
    "MakeRationalFunction{}(\frac{-2x -2}{x^2-x })MakeRationalFunction{}(\frac{-2x -2}{x^2+x })",
    "CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyPolynomialByRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction,
    this->opRationalFunction(),
    this->opRationalFunction(),
    "Multiplies rational function by a rational function.",
    "Polynomial{}(x +1)MakeRationalFunction{}(\frac{-2x -2}{x^2+x });\n"
    "MakeRationalFunction{}(\frac{-2x -2}{x^2-x })MakeRationalFunction{}(\frac{-2x -2}{x^2+x })",
    "CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyRationalFunctionByRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence,
    this->opRational(),
    this->opSequence(),
    "Carries out multiplication between a rational number on left "
    "and sequence on the right. Corresponds to multiplying a vector by a scalar "
    "(however please note a sequence does not necessarily consist "
    "of elements of a field, so the latter interpretation might not be applicable).",
    "v_{1}= (1, 2, 3);\nv_{2}= (1, 3, 2);\nv_{3}= (3, 1, 1);\n"
    "v_{4}= (-2, 2, 2);\n1/2v_{1}+ 1/2v_{2}+7/8v_{3}+ 13/16v_{4}",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence",
    "ScalarTimesSequence",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix,
    "",
    "Multiplies a rational number by a matrix",
    "2\\begin{pmatrix}  1& 2& 3\\\\2& 1& 0 \\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix",
    "ScalarTimesMatrix",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence,
    this->opDouble(),
    this->opSequence(),
    "Carries out multiplication between a double number on left "
    "and sequence on the right. Corresponds to multiplying a vector by a scalar.",
    "%NumberColors\n"
    "(1, 2) - DoubleValue{} 1 (2,3)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence",
    "MultiplyDoubleBySequence",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence,
    this->opAlgebraicNumber(),
    this->opSequence(),
    "Multiplies a double number on left and a sequence on the right.scalar.",
    "(1 ,2)- DoubleValue{} 1 (2,3)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence",
    "MultiplyAlgebraicNumberBySequence",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence,
    this->opPolynomialRational(),
    this->opSequence(),
    "Carries out multiplication between a rational number on left "
    "and sequence on the right.",
    "x = Polynomial{}x; v=x*(1, 2, 3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence",
    "MultiplyPolynomialBySequence",
    innerStandard
  );
  this->addOperationHandler(
    "\\binom",
    CalculatorFunctionsBinaryOps::binomialCoefficient,
    "",
    "Evaluates the binomial coefficient if possible.",
    "\\binom{8}{3} ",
    "CalculatorFunctionsBinaryOps::binomialCoefficient",
    "Binom",
    innerStandard
  );
  this->addOperationHandler(
    "Value",
    CalculatorFunctions::innerValueOfModularExpression,
    "",
    "Given an expression of the form a mod b, returns the element a. "
    "When the input is a modular polynomial, converts the polynomial to a rational. ",
    "Value(3 mod 7);\n"
    "Value(9 mod 7);\n"
    "a = Value((PolynomialModP(x^9, 7)) mod (PolynomialModP(x^5+x+1, 7)));\n"
    "Value a",
    "CalculatorFunctions::innerValueOfModularExpression",
    "Value",
    innerStandard
  );
  this->addOperationHandler(
    "mod",
    CalculatorFunctions::innerZmodP,
    "",
    "Same as ModP but uses the mod notation.",
    "7 mod 3",
    "Calculator::innerZmodP",
    "mod",
    innerStandard
  );
  this->addOperationHandler(
    "mod",
    CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP,
    "",
    "A polynomial modulo another polynomial. "
    "Returns the element given by the first argument "
    "of a quotient ring formed modulo the ideal "
    "generated by the second argument. ",
    "p = PolynomialModP(x^2, 7);\n"
    "q = PolynomialModP(x^9+x+1, 7);\n"
    "r = PolynomialModP(y^2, 7);\n"
    "s = PolynomialModP(x^5, 5);\n"
    "a = p mod q;\n"
    "a^10;\n"
    "r mod q;\n"
    "s mod q",
    "Calculator::innerZmodP",
    "moduloPolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "\\setminus",
    CalculatorFunctionsBinaryOps::setMinus,
    "",
    "Removes the elements of the second set from the elements of the first set. "
    "The outputs will be sorted in ascending order.",
    " (x,y,t) \\setminus Sequence{}x; (x,y)\\setminus (z,y)",
    "CalculatorFunctionsBinaryOps::setMinus",
    "\\setminus",
    innerStandard
  );
  this->addOperationHandler(
    "Differential",
    CalculatorFunctions::innerDifferentialStandardHandler,
    "",
    "Transforms Differential{}a to the standard internal form Differential {}(a, 1).",
    "\\int \\theta {\\text d} \\theta",
    "CalculatorFunctionsBinaryOps::innerDifferentialStandardHandler",
    "DifferentialStandardHandler",
    innerStandard
  );
  this->addOperationHandler(
    "Differential",
    CalculatorFunctions::innerDifferentialOfPolynomial,
    "",
    "Differential of a polynomial.",
    "Differential{}(Polynomial{}(x+y));\n"
    "Differential{}(Polynomial{}(x^3y z+y z + x y + x^5 y^2 z + x y^2 z));\n",
    "CalculatorFunctionsBinaryOps::innerDifferentialOfPolynomial",
    "DifferentialOfPolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsIntegration::integralOperator,
    "",
    "Transforms integral notation into an integral expression. ",
    "(\\int x)dx; \\int x (1+x) dx; \\int_2^3 x dx; \\int_2^3 x(1+x)dx",
    "CalculatorFunctions::integralOperator",
    "IntegralOperatorFromQuotient",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideEltZmodPorRatByEltZmodPorRat,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Divides elements of Z/pZ. ",
    "(2 mod  7) / (3 mod 7)",
    "CalculatorFunctionsBinaryOps::innerDivideEltZmodPorRatByEltZmodPorRat",
    "DivideElementZmodPByElementZModP",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsDifferentiation::diffdivDiffxToDifferentiation,
    "",
    "Replaces Differential / Differential {}x by Differentiate{}(x). "
    "Should also work on the notation (Differential y)/(Differential x). ",
    "Differential /Differential {}x x",
    "CalculatorFunctions::diffdivDiffxToDifferentiation",
    "LeibnizDifferentialOperatorToDifferentialOperator",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsDifferentiation::ddivDxToDiffDivDiffx,
    "",
    "Replaces d/dx by Differential /Differential{}x. "
    "Note that the variable of differentiation is expected to be the string following the d letter. ",
    "d/dx x",
    "CalculatorFunctions::ddivDxToDiffDivDiffx",
    "LeibnizDifferentialOperatorNotation",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat,
    this->opAlgebraicNumber(),
    this->opAlgebraicNumber(),
    "Divides algebraic numbers. ",
    "1/(1+\\sqrt{}2+\\sqrt{}3+\\sqrt{}5+\\sqrt{}7)",
    "CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat",
    "DivideAlgebraicByAlgebraic",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat,
    this->opRational(),
    this->opAlgebraicNumber(),
    "Divides rational by algebraic number. ",
    "1/(\\sqrt{}2+\\sqrt{}3+\\sqrt{}5)",
    "CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat",
    "DivideRationalByAlgebraic",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Divides algebraic number by rational. ",
    "(\\sqrt{}2+\\sqrt{}3+\\sqrt{}5)/5",
    "CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat",
    "DivideAlgebraicByRational",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::divideByNumber,
    "",
    "If b is rational, algebraic, or double, substitutes (anything)/b with anything* (1/b).",
    "6/15+(a +b)/5;\n"
    "a/\\sqrt{}2;\n"
    "x/DoubleValue{}10^10;x/DoubleValue{}5;\n"
    "6/4+3/0",
    "CalculatorFunctions::divideByNumber",
    "DivideByNumber",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorEducationalFunctions::divideByNumberTrivial,
    "",
    "Intended for educational purposes. "
    "Divides rationals only if the operation is trivial, such as in"
    "(2) /(3) --> (2/3). "
    "The rule is turned off by default.",
    "%LogEvaluation\n"
    "TurnOnRules(DivideByNumberTrivial);\n"
    "TurnOffRules("
    "DivideByNumber, "
    "ConvertShortDenominatorToNegativePower, "
    "DivideRationalByRational"
    ");\n"
    "4/6;\n"
    "2/3",
    "CalculatorEducationalFunctions::divideByNumberTrivial",
    "DivideByNumberTrivial",
    Function::Options::outerOffByDefault()
  );
  this->addOperationHandler(
    "/",
    CalculatorBasics::subZeroDivAnythingWithZero,
    "",
    "Provided that x is not equal to zero, substitutes 0/x with 0. ",
    "0/b; ",
    "Calculator::innerSubZeroDivAnythingWithZero",
    "DivideZeroDivideByAnything",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::outerDivCancellations,
    "",
    "Division cancellations. Substitutes (a/b)/(a/d) with d/a and (a/b)/(c/b) with a/c. ",
    "(a/b)/(a/d); (a/b)/(c/b)",
    "CalculatorFunctions::outerDivCancellations",
    "DivideCancellations",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::outerAssociateDivisionDivision,
    "",
    "Substitutes (a/b)/c =a/(c*b); a/(b/c)=a*c/b. "
    "Note the order of multiplication in the rules: this operation is safe "
    "and correct for non-commutative rings as well.",
    "(a/b)/c;\n"
    "a/(b/c);",
    "CalculatorFunctions::outerAssociateDivisionDivision",
    "AssociateDivisionDivision",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::outerAssociateAdivBdivCpowerD,
    "",
    "Substitutes a/(b/c)^d =a c^d/b^d."
    "Note the order of multiplication in the rules: this operation is safe and correct for "
    "non-commutative rings as well.",
    "a/(b/c)^d;",
    "CalculatorFunctions::outerAssociateAdivBdivCpowerD",
    "AssociateAdividedByBdividedByCpowerD",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::outerAtimesBpowerJplusEtcDivBpowerI,
    "",
    "Rule: (a_0 + a_1 x^{c_1}+ ... + a_n x^{c_n}) /x^t =a_0 x^{-t}+ a_1 x^{c_1-t}+...+a_n x^{c_n-t} ",
    "(a x^{1/2} + b x )/x;\n"
    "(a x^{1/2} + b x )/x^2;",
    "CalculatorFunctions::outerAtimesBpowerJplusEtcDivBpowerI",
    "ConvertShortDenominatorToNegativePower",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideRatByRat,
    this->opRational(),
    this->opRational(),
    "Divides two rational numbers. ",
    "4/6; 2/0;",
    "CalculatorFunctionsBinaryOps::innerDivideRatByRat",
    "DivideRationalByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble,
    this->opRational(),
    this->opDouble(),
    "Divides doubles. ",
    "a = 0.5; b= 0.5; c = DoubleValue{}3.3; a/c; c/a; c/c",
    "CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble",
    "DivideRationalByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble,
    this->opDouble(),
    this->opDouble(),
    "Divides doubles. ",
    "a = 0.5;\n"
    "b = 0.5;\n"
    "c = DoubleValue{}3.3;\n"
    "a/c;\n"
    "c/a;\n"
    "c/c",
    "CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble",
    "DivideDoubleByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial,
    this->opRational(),
    this->opPolynomialRational(),
    "Divides rational by polynomial (to get a rational function).",
    "z = Polynomial{}(x^2+y^2);\n1/z",
    "CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial",
    "DivideRationalByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialRational(),
    "Divides polynomial by polynomial (to get a rational function). ",
    "Polynomial{}(-x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}+x_{2}+ 1)/\n"
    "Polynomial{}(x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}-x_{2}+ 1) ",
    "CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial",
    "DividePolynomialByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDividePolynomialModuloIntegerByPolynomialModuloInteger,
    this->opPolynomialModuloInteger(),
    this->opPolynomialModuloInteger(),
    "Divides polynomial modulo integer by another polynomial modulo integer. ",
    "PolynomialModP{}(x^5+5x^3+x^2+4x+1, 7) / PolynomialModP{}(x^5+6x^3+x^2+5x+1, 7);\n"
    "PolynomialModP{}(x^3-x+1, 3) / PolynomialModP{}(x^2+2, 3);\n"
    "PolynomialModP{}(x^3-x+1, 3) / PolynomialModP{}(x^2+2, 7);\n"
    "PolynomialModP{}(x^3-x+1, 6) / PolynomialModP{}(2x^2+2, 6);\n"
    "PolynomialModP{}(x^3-x+1, 21) / PolynomialModP{}(2x^2+2, 21);\n",
    "CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial",
    "DividePolynomialModPByPolynomialModP",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial,
    "",
    "Assumes that the numerator and denominator of a fraction commute. "
    "Divides the two expressions under the assumption that both can be converted to"
    "polynomials with rational coefficients.",
    "(2x^2+3x-5)/(5x^4+x^2+2x-8);\n"
    "x^2/x;\n"
    "(x+1)^2/(x+1)",
    "CalculatorFunctionsPolynomial::innerDivideExpressionsAsIfPolynomial",
    "DivideExpressionsAsIfPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerRationalByInteger,
    this->opRational(),
    this->opRational(),
    "Raises rational to power, provided the power is a small integer. ",
    "{3^3}^3; 3^{3^3}; 3^3^3; 0^3; 0^{-3}; ",
    "CalculatorFunctionsBinaryOps::innerPowerRationalByInteger",
    "PowerIntegerByInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors,
    this->opRational(),
    this->opRational(),
    "If a rational number is small enough to factor, reduces the "
    "rational exponents of the rational number. ",
    "(32)^{-4/5};\n"
    "(4/9)^{17/3};\n"
    "(12/7)^{7/2};\n"
    "12^{2/3};\n"
    "12^{-2/3};\n"
    "5^{- 1/3};\n"
    "5^{- 1/3}-\\sqrt[3]{5^- 1};\n"
    "\\sqrt[20]{200!}",
    "CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors",
    "PowerRationalByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerRationalByRationalOutputAlgebraic,
    this->opRational(),
    this->opRational(),
    "If the rational power is small enough, converts to an algebraic number. "
    "At the moment works with rational powers whose denominator is 2, "
    "may be extended to larger powers in the future. ",
    "54^{1/2}",
    "CalculatorFunctionsBinaryOps::innerPowerRationalByRationalOutputAlgebraic",
    "PowerRationalByRationalOutputAlgebraic",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerDoubleOrRationalToDoubleOrRational,
    this->opRational(),
    this->opDouble(),
    "Calls the built-in cpp functions to approximately raise a double to a power, "
    "provided either the base or the exponent is a double, and provided that "
    "the base is non-negative. ",
    "f{}{{x}}=x^3+p x +q;\n"
    "Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) +( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);\n"
    "q = DoubleValue{}1; \np = DoubleValue{}1; \nXcardano; \nf{}x; \nf{}Xcardano   ",
    "CalculatorFunctionsBinaryOps::innerPowerDoubleOrRationalToDoubleOrRational",
    "PowerDoubleToDouble",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT,
    "",
    "Provided the exponent is t or T, calls the Transpose "
    "function on the base.",
    "X = (1,2)^t; X-Transpose{}(1,2); \\begin{pmatrix}1&2\\\\ 3& 5\\end{pmatrix}^t  ",
    "CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT",
    "TransposePowerNotation",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::powerMatrixBuiltInBySmallInteger,
    "",
    "If the base is a matrix and the exponent is a rational number: "
    "1. If the base is not square, returns error. "
    "2. If the base is "
    "(a) square, "
    "(b) has a supported built-in coefficient type and "
    "(c) the exponent is a "
    "small integer, raises the base to the corresponding power. "
    "If the power is 0 or negative and the determinant of "
    "the matrix is zero, returns error. "
    "As of writing, the supported built-in coefficient types are: (1) rational numbers;"
    "(2) algebraic numbers; "
    "(3) polynomial and rational functions in one variable.",
    "X = \\begin{pmatrix} 0 & 1 \\\\ 1 &1\\end{pmatrix};\n"
    "X^5;\n"
    "X^{-5};\n"
    "f{}({{a}}, {{b}})=a Polynomial( x )+b;\n"
    "a=FunctionToMatrix(f,2,2);\n"
    "b=a^{-1};\n"
    "a*b",
    "CalculatorFunctionsBinaryOps::innerPowerMatBySmallInteger",
    "PowerMatrixBuiltInByInteger",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersByLargeIntegerIfPossible,
    "",
    "Attempts to exponentiate matrix by a large power. ",
    "X =\\begin{pmatrix} 1 & -1 \\\\ 4 & -3\\end{pmatrix}; X^{-55!- 1}  ",
    "CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersByLargeIntegerIfPossible",
    "PowerMatrixNumbersByLargeIntegerIfPossible",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerMatrixExpressionsBySmallInteger,
    "",
    "Attempts to exponentiate a matrix of expressions, if the exponent is small. ",
    "X =\\begin{pmatrix} a & b \\\\ t +q &r\\end{pmatrix}; X^{2}  ",
    "CalculatorFunctionsBinaryOps::innerPowerMatrixExpressionsBySmallInteger",
    "PowerMatrixNumbersBySmallIntegerIfPossible",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementZmodPByInteger,
    "",
    "Raises an element of Z mod p to an integer power. ",
    "a = 3 mod 7; a^2",
    "CalculatorFunctions::innerPowerElementZmodPByInteger",
    "PowerElementZmodPToInteger",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerPowerExponentToLog,
    "",
    "Replaces e^{\\ln x} by x.",
    "e^{\\ln x}; ",
    "CalculatorFunctions::innerPowerExponentToLog",
    "PowerToLog",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerPowerAnyToZero,
    "",
    "Replaces p^0 by 1. "
    "Notice that 0^0 is defined to be 1."
    "An explanation of that definition was given, among others, by "
    "Donald Knuth; the argument boils down to making the "
    "Newton binomial formula work for (a+0)^n. ",
    "A = x^0;\n"
    "x = 0;\n"
    "A;\n"
    "B=x^0;\n"
    "0^0; ",
    "CalculatorFunctions::innerPowerAnyToZero",
    "PowerAnyToZero",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerPolynomialBySmallInteger,
    this->opPolynomialRational(),
    this->opRational(),
    "Raises poly to small integer power. ",
    "x = Polynomial{}x; y = Polynomial{}y;(x +2y+x y+x^2+3y^2)^3",
    "CalculatorFunctionsBinaryOps::innerPowerPolynomialBySmallInteger",
    "PowerPolynomialBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberPolynomialBySmallInteger,
    this->opPolynomialAlgebraicNumbers(),
    this->opRational(),
    "Raises polynomial over algebraic numbers to small integer power. ",
    "x = Polynomial{}x; y = Polynomial{}y;(x +\\sqrt{2}y+x y+x^2+\\sqrt{3}y^2)^3",
    "CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberPolynomialBySmallInteger",
    "PowerPolynomialAlgebraicNumbersBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerPolynomialModuloIntegerBySmallInteger,
    this->opPolynomialModuloInteger(),
    this->opRational(),
    "Raises polynomial modulo an integer to a small integer power. ",
    "a = PolynomialModP{}(x^2 + x + 1, 7);\n"
    "a^20",
    "CalculatorFunctionsBinaryOps::innerPowerPolynomialModuloIntegerBySmallInteger",
    "PowerPolynomialModuloIntegerBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerPolynomialModPModuloPolynomialModPBySmallInteger,
    this->opPolynomialModuloPolynomialModuloInteger(),
    this->opRational(),
    "Raises polynomial modulo an integer to a small integer power. ",
    "p = x^2 + 3x + 1;\n"
    "q = x^3 +x^2 + 3 x + 5;\n"
    "modulusComposite = p * q;\n"
    "modulo = PolynomialModP(modulusComposite, 7);\n"
    "a = PolynomialModP{}(x+2, 7) mod modulo;\n"
    "a^(7  );\n"
    "a^(49);\n"
    "a^(7^3 );\n"
    "a^(7^4 );\n"
    "a^(7^5 );\n"
    "r = a^( (7^6 - 1) /2);\n"
    "s = PolynomialModP(Value Value r - 1, 7);\n"
    "t = PolynomialModP(Value Value r, 7);\n"
    "u = PolynomialModP(Value Value r + 1, 7);\n"
    "GCDPoly(s, modulo);\n"
    "GCDPoly(t, modulo);\n"
    "GCDPoly(u, modulo);",
    "CalculatorFunctionsBinaryOps::innerPowerPolynomialModPModuloPolynomialModPBySmallInteger",
    "PowerPolynomialModPModuloPolynomialModPBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Raises algebraic number to small integer or half-integer power. ",
    "a = 3/2;\n"
    "b = - 15/2;\n"
    "c = 33/4;\n"
    "x = (-b+\\sqrt{}(b^2-4a c))/(2a);\n"
    "B= c+a x^{2}+b x;",
    "CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger",
    "PowerAlgebraicNumberBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Takes the radical of an algebraic number, if the algebraic number is a positive number"
    "that whose radical lies in the underlying algebraic closure.",
    "\\sqrt{3+2\\sqrt[2]{2}};\n"
    "(7-5\\sqrt{2})^{7/3}",
    "CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault",
    "RadicalAlgebraicNumberPositiveDefault",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerDoubleOrRationalToDoubleOrRational,
    this->opDouble(),
    this->opRational(),
    "Calls the built-in cpp functions to approximately raise a double to a power, "
    "provided either the base or the exponent is a double. If the base is negative and "
    "the exponent is rational with odd denominator, the exponent is evaluated to the corresponding "
    "real negative root. ",
    "f{}{{x}}=x^3+p x +q;\n"
    "Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) +( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);\n"
    "q= DoubleValue{}1; \np = DoubleValue{}1; \nXcardano; \nf{}x; \nf{}Xcardano   ",
    "CalculatorFunctionsBinaryOps::innerPowerDoubleOrRationalToDoubleOrRational",
    "PowerFloatingToPower",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorBasics::associateExponentExponent,
    "",
    "If the rule doesn't break over the complex numbers, substitutes (a^b)^c with a^{b*c}. "
    "The rule acts when one of the following holds:\n"
    "1) a is real and positive;\n"
    "2) a>0 or a\\geq 0 equals 1 (those can be user-defined, for example via (a>0)=1;)\n"
    "3) c is an integer;\n"
    "4) a evaluates to real (double), b and c are rational and b*c is an even integer. ",
    "(a^m)^n; ((ln(3))^m)^n; ((ln(0.5))^m)^n; (a^m)^2; (a^2)^m; (a^{1/2})^2; (a^{2})^{1/2}; (a>0)=1; (a^{2})^{1/2};",
    "Calculator::innerAssociateExponentExponent",
    "PowerPowerToPower",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerDistributeExponent,
    "",
    "If a is a positive constant, substitutes (a*b)^c with a^c b^c.",
    "(a*b)^n; (\\sqrt(2)*b)^2",
    "CalculatorFunctions::innerDistributeExponent",
    "DistributeExponent",
    innerStandard
  );
  this->addOperationHandler(
    "\\sqrt",
    CalculatorFunctions::innerDistributeSqrt,
    "",
    "If a is a positive constant, substitutes sqrt(a b) by sqrt(a) sqrt(b).",
    " TurnOffApproximations 0; \\sqrt{\\frac{676}{25} ln 3}",
    "Calculator::innerDistributeSqrt",
    "innerDistributeSqrt",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    Calculator::outerPowerRaiseToFirst,
    "",
    "Realizes the tranformation {{anything}}^1=a. ",
    "x^1 + x^2; A^1",
    "Calculator::outerPowerRaiseToFirst",
    "PowerToOne",
    outerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerOperatorBounds,
    "",
    "Replaces \\int_a^b by (\\int, a, b) .",
    "A =\\int_a^b; Lispify(A); PlotExpressionTree(A); ",
    "CalculatorFunctions::innerIntegralUpperBound",
    "OperatorBoundsSuperscript",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::innerOperatorBounds,
    "",
    "Takes care of the integral superscript notation \\int^a ",
    "\\int^a_b f dx; \\int_a^b f dx",
    "Calculator::innerUnderscoreIntWithAny",
    "OperatorBoundsUnderscore",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::innerDereferenceSequenceOrMatrix,
    "",
    "Dereferences a sequence or a matrix. "
    "The syntax is as illustrated by the example. ",
    "X = (a, b, c);\n"
    "X_1;\n"
    "X_2;\n"
    "X_3;\n"
    "X_4;\n"
    "X_j;\n"
    "j = 3;\n"
    "X_j;\n"
    "Denominations = (1, 5, 10, 25,50, 100,200, 500, 1000, 2000, 5000);\n"
    "p (0, 0 )=1;\n"
    "p ({{a}}, {{x}}):if (x < 0) = 0;\n"
    "p (0, {{x}})= 0;\n"
    "p ({{a}}, {{x}} )= p(a- 1,x) + p(a, x-Denominations_a);\n"
    "p (11, 100)",
    "CalculatorFunctions::innerDereferenceSequenceOrMatrix",
    "DereferenceSequenceOrMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::innerDereferenceInterval,
    "",
    "Dereferences an interval, as illustrated by the example. ",
    "%UseBracketForIntervals\n"
    "[a, b)_1;\n"
    "(a, b]_2;\n"
    "[a, b]_1; ",
    "CalculatorFunctions::innerDereferenceInterval",
    "DereferenceInterval",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::innerDereferenceSequenceStatements,
    "",
    "Dereferences a sequence of rules. "
    "The syntax is as illustrated by the example. ",
    "A = d/dx( \\sqrt(x + y) - 4x^2y^2);\n"
    "(d/dx(y)= 0; A)_2;  ",
    "CalculatorFunctions::innerDereferenceSequenceStatements",
    "DereferenceSequenceStatements",
    innerStandard
  );
  this->addOperationHandler(
    "\\otimes",
    CalculatorFunctionsBinaryOps::tensorMatrixByMatrix,
    "",
    "Tensor product of two matrices.",
    "P = ((0, 2 ), (1, 0));\n"
    "Q = ((0, 3 ), (1, 0));\n"
    "X = MakeMatrix{}P;\n"
    "Y = MakeMatrix{}Q;\n"
    "Z = MakeMatrixTensorForm{}P;\n"
    "W = MakeMatrixTensorForm{}Q;\n"
    "X\\otimes Y;\n"
    "Z\\otimes W",
    "CalculatorFunctionsBinaryOps::tensorMatrixByMatrix",
    "MatrixTensorMatrix",
    innerStandard
  );

  this->addOperationBinaryInnerHandlerWithTypes(
    "\\otimes",
    CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor,
    this->opMatrixTensorRational(),
    this->opMatrixTensorRational(),
    "Same as tensor product of matrices but uses class MatrixTensor instead of class Matrix.",
    "P = ((0 , 2 ),(1 , 0));\n"
    "Q = ((0 , 3 ),(1 , 0));\n"
    "X = MakeMatrix{}P;\n"
    "Y = MakeMatrix{}Q;\n"
    "Z = MakeMatrixTensorForm{}P;\n"
    "W = MakeMatrixTensorForm{}Q;\n"
    "X \\otimes Y;\n"
    "Z\\otimes W",
    "CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor",
    "TensorMatrixTensorByMatrixTensor",
    innerStandard
  );
  this->addOperationHandler(
    "\\otimes",
    CalculatorBasics::tensorProductStandard,
    "",
    "Please do note use (or use at your own risk): this is work-in-progress. "
    "Will be documented when implemented and tested. Tensor product of "
    "generalized Verma modules. ",
    "X = G_2;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X,i);\n"
    "v=HeighestWeightVector{}(X, (1,0),(0,0));\n"
    "g_{- 1}(v\\otimes v);\n"
    "g_{- 1}g_{- 1}(v\\otimes v); ",
    "Calculator::outerTensor",
    "TensorProductStandard",
    innerNoTestInvisibleExperimental
  );
  this->addOperationHandler(
    "=:",
    CalculatorBasics::standardIsDenotedBy,
    "",
    "The operation =: is the \"is denoted by\" operation. "
    "The expression a =:b always reduces to a =b. "
    "In addition to the transformation, the pair of expressions a, b is registered "
    "in a special global \"registry\". This has the following effect. Every time "
    "the expression b is met, it is displayed on the screen as a. "
    "We note that subsequent occurrences of the expression a "
    "will first be replaced by b (as mandated by the a =b command), "
    "but then displayed on the screen as a.",
    "x =:y;\n"
    "y;\n"
    "z;\n"
    "z=y;\n"
    "z",
    "Calculator::standardIsDenotedBy",
    "standardIsDenotedBy",
    innerStandard
  );
  this->addOperationHandler(
    "<",
    CalculatorFunctions::innerLessThan,
    "",
    "If both the left hand side and the right hand side are rational, "
    "replaces the expression by 1 if the left number "
    "is less than the right, else replaces the expression by 0.",
    "3 < 4; 5 < 4",
    "CalculatorFunctions::innerLessThan",
    "LessThan",
    innerStandard
  );
  this->addOperationHandler(
    ">",
    CalculatorFunctions::innerGreaterThan,
    "",
    "Greater than: has similar action to the less than sign. "
    "The following example shows an implementation of commutativity. ",
    "x_{{i}} * x_{{j}} * {{a}} :if(i > j) = x_j * x_i * a;\n"
    "x_{{i}}*x_{{j}}: if (i > j) = x_j * x_i;\n"
    "(x_2 * x_1 - x_1 * x_3)(x_1 x_5 + x_5 x_4 x_2);\n"
    "x_5x_4x_3x_2x_1",
    "CalculatorFunctions::innerGreaterThan",
    "GreaterThan",
    innerStandard
  );
  this->addOperationHandler(
    "Max",
    CalculatorFunctions::innerMax,
    "",
    "Maximum function.",
    "Max(- 4,2)",
    "CalculatorFunctions::innerMax",
    "Max",
    innerStandard
  );
  this->addOperationHandler(
    "Min",
    CalculatorFunctions::innerMin,
    "",
    "Maximum function.",
    "Min(- 4,2)",
    "CalculatorFunctions::innerMin",
    "Min",
    innerStandard
  );
  this->addOperationHandler(
    "\\geq",
    CalculatorFunctions::innerGreaterThanOrEqualTo,
    "",
    "Greater than or equal to operation. ",
    "A = (2>=x);\n"
    "x =1;\n"
    "A",
    "Calculator::innerGreaterThanOrEqualTo",
    "greaterThanOrEqualTo",
    innerStandard
  );
  this->addOperationHandler(
    "\\leq",
    CalculatorFunctions::innerLessThanOrEqualTo,
    "",
    "Transforms \\(a\\leq b\\) to \\(b\\geq a\\).",
    "a \\leq b",
    "Calculator::innerLessThanOrEqualTo",
    "lessThanOrEqualTo",
    innerStandard
  );
  this->addOperationHandler(
    "==",
    CalculatorFunctions::outerEqualEqual,
    "",
    "If either the left or the right argument contains a bound variable does nothing. "
    "Else evaluates to 1 if the left argument equals mathematically the right argument.",
    "%NumberColors\n  "
    "A = (a =3, b=4);\n"
    "B = (a = (sqrt(3))^2, b = 4);\n"
    "A == B;\n"
    "x == y;\n"
    "x == 1;\nIsEqualToX{}{{a}} = (a == x);\n"
    "IsEqualToX{}y;\n"
    "IsEqualToX{}x;\n"
    "IsEqualToX{}1;\n"
    "x =1;\n"
    "IsEqualToX{}1; z= \\sqrt{}1; z==1",
    "CalculatorFunctions::outerEqualEqual",
    "IsEqualTo",
    outerStandard
  );
  this->addOperationHandler(
    "===",
    CalculatorFunctions::innerEqualEqualEqual,
    "",
    "Returns 1 if both sides have identical expression trees, 0 otherwise. ",
    "2+3 == 5;\n"
    "Freeze{}(2 + 3) == Freeze{}5;\n"
    "Freeze{}(2 + 3) === Freeze{}5",
    "CalculatorFunctions::innerEqualEqualEqual",
    "IsEqualExpressionTree",
    innerStandard
  );
  this->addOperationHandler(
    "Contains",
    CalculatorFunctions::innerContains,
    "",
    "Returns 1 if the first argument contains the second "
    "as a sub-expression, else returns 0. Function "
    "has not been optimized for speed, use with caution. ",
    "Contains ((\\arcsin x +\\cos x,5), \\arcsin )",
    "CalculatorFunctions::innerContains",
    "Contains",
    innerStandard
  );
  this->addOperationHandler(
    "ExpressionLeafs",
    CalculatorFunctions::innerExpressionLeafs,
    "",
    "Returns a sequence without repetition of all leafs "
    "making up an expression. "
    "Here, a ``leaf'' means either an atomic expression or a built-in type. "
    "Built-in types are not broken into atoms. ",
    "ExpressionLeafs ((\\arcsin x +\\cos x,5), \\arcsin )",
    "CalculatorFunctions::innerExpressionLeafs",
    "ExpressionLeafs",
    innerStandard
  );
  this->addOperationHandler(
    "Last",
    CalculatorFunctionsListsAndSets::lastElement,
    "",
    "Returns the last element of the expression, provided the argument has "
    "no bound variables. If the expression has bound variables does nothing.",
    "Last(1,2);\n"
    "Last((1, 2));\n"
    "Last{}();\n"
    "Last{}(());\n"
    "a = (1,2);\n"
    "Last a;\n"
    "Last Sequence{}(a);\n"
    "p{}((), 0) = 1;\n"
    "p{}({{x}},{{n}}): if (n < 0) = 0;\n"
    "p{}((), {{n}}) = 0;\n"
    "p{}({{x}}, {{n}}) = p{}(x, n - Last x) +p{}(RemoveLast x, n);\n"
    "p{}((1, 2, 5, 10, 25, 100), 100);",
    "CalculatorFunctions::innerLastElement",
    "Last",
    innerStandard
  );
  this->addOperationHandler(
    "RemoveLast",
    CalculatorFunctionsListsAndSets::removeLastElement,
    "",
    "Returns a list with the last element removed, provided "
    "the argument has no bound variables. If the expression "
    "has bound variables does nothing.",
    "X = (a,b,c);\n"
    "Y = (RemoveLast X)\\cup Sequence{}(Last X) - X;\n"
    "RemoveLast{}(());\n"
    "RemoveLast{}();\n",
    "CalculatorFunctionsListsAndSets::removeLastElement",
    "RemoveLast",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctionsListsAndSets::intersection,
    "",
    "Intersects lists. For the time being, the "
    "output order is not specified (will be fixed in the future).",
    "(a,b,c)\\cap (c, d, e);\n"
    "x = (a,b,c)\\cap (c, d, e);\n"
    "a = 1;\n"
    "d =1;\n"
    "(a, b, c)\\cap (c, d, e); x",
    "CalculatorFunctions::intersection",
    "\\cap",
    innerStandard
  );
  this->addOperationHandler(
    "\\cup",
    CalculatorFunctionsListsAndSets::listUnion,
    "",
    "If all arguments of \\cup are of type list, substitutes the expression with "
    "a list containing the union of all members (with repetition). "
    "If a flag is set requesting that (a,b) is interpreted as an interval, does nothing if "
    "either of the two sequences has two elements.",
    "x\\cup (MakeSequence{} x \\cup MakeSequence{}x \\cup (a,b,x))",
    "CalculatorFunctions::union",
    "\\cup",
    innerStandard
  );
  this->addOperationHandler(
    "\\sqcup",
    CalculatorFunctionsListsAndSets::unionNoRepetition,
    "",
    "If all arguments of \\sqcup are of type list, substitutes the expression "
    "with a list containing the union of all members; "
    "all repeating members are discarded.",
    "(x,y,x)\\sqcup(1,x,y,2)",
    "CalculatorFunctions::unionNoRepetition",
    "\\sqcup",
    innerStandard
  );

  this->addOperationHandler(
    "\\sum",
    CalculatorFunctions::innerSumAsOperatorToSumInternalNotation,
    "",
    "Transforms (\\sum_a^b ){} n to \\sum_a^b n (internal notation). ",
    "PlotExpressionTree(Freeze(\\sum_a^b ){} n);\n"
    "PlotExpressionTree((\\sum_a^b ){} n) ",
    "CalculatorFunctions::innerSumAsOperatorToSumInternalNotation",
    "SumAsOperator",
    compositeStandard
  );
  this->addOperationHandler(
    "Matrix",
    CalculatorConversions::outerMatrixExpressionsToMatrixOfType,
    "",
    "If A is a non-typed matrix of expressions that can "
    "be converted to a typed matrix, carries out the type specialization. ",
    "%LogEvaluation\n"
    "A = \\begin{pmatrix}\n"
    "1 & 2\\\\\n"
    "3 & 4\n"
    "\\end{pmatrix};\n"
    "B = \\begin{pmatrix}\n"
    "a & 2\\\\\n"
    "3 & 4\n"
    "\\end{pmatrix};\n"
    "PlotExpressionTree A;\n"
    "PlotExpressionTree B",
    "CalculatorConversions::outerMatrixExpressionsToMatrixOfType",
    "MatrixTypeDeduction",
    compositeStandard
  );
  this->addOperationHandler(
    "Rational",
    CalculatorFunctions::innerConstantFunction,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;2{}y;(\\sqrt{}2){}x;",
    "CalculatorFunctions::innerConstantFunction",
    "ConstantFunction",
    compositeStandard
  );
  this->addOperationHandler(
    "RationalFunction",
    CalculatorFunctions::innerRationalFunctionSubstitution,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;2{}y;(\\sqrt{}2){}x;",
    "CalculatorFunctions::innerRationalFunctionSubstitution",
    "RationalFunctionSubstitution",
    compositeStandard
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a +b){}x = (a{}x) +(b{}x) ",
    "(a +b){}x;",
    "CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentAddition",
    compositeStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a*b){}x = (a{}x)*(b{}x) ",
    "(a*b){}x;",
    "CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentMultiplication",
    compositeStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a/b){}x = (a{}x)/(b{}x) ",
    "(a/b){}x;",
    "CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentDivision",
    compositeStandard
  );
  this->addOperationHandler(
    "AlgebraicNumber",
    CalculatorFunctions::innerConstantFunction,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;\n"
    "2{}y;\n"
    "(\\sqrt{}2){}x;",
    "CalculatorFunctions::innerConstantFunction",
    "ConstantFunctionAlgebraicNumber",
    compositeStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::innerCompositeConstTimesAnyActOn,
    "",
    "Rule (a*f){}x = a*(f{}x), provided a is a constant. ",
    "(2\\sin){}x-2(\\sin x) ",
    "CalculatorFunctions::innerCompositeConstTimesAnyActOn",
    "ConstTimesAnyAction",
    compositeStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::innerCompositeApowerBevaluatedAtC,
    "",
    "Provided that n is not equal to - 1 and f is not a sequence, use the rule ({{f}}^{{n}}){}{{x}}= (f{}x)^n.",
    "\\tan^2 x; (f^-2) {}x ; (f^- 1){}x ",
    "CalculatorFunctions::innerCompositeApowerBevaluatedAtC",
    "ApowerBevaluatedAtC",
    compositeStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerCompositeDifferentiateLog,
    "",
    "Differentiates log.",
    "d/dx (\\log x)",
    "CalculatorFunctions::innerCompositeDifferentiateLog",
    "DifferentiateLog",
    compositeStandard
  );
  this->addOperationHandler(
    "SolveJSON",
    CalculatorEducationalFunctions::solveJSON,
    "",
    "Tries to interpret the input as a high-school or Calculus problem "
    "and solve it. Returns its result in JSON format. Freezes its inputs.",
    "SolveJSON(x^2+2x-3=0);\n",
    "CalculatorFunctions::solveJSON",
    "SolveJSON",
    innerFreezesArguments
  );
}

void Calculator::initializePredefinedStandardOperationsWithoutHandler() {
  MacroRegisterFunctionWithName("Calculator::initializePredefinedStandardOperationsWithoutHandler");
  //additional operations treated like function names but otherwise not parsed as syntactic elements.

  this->addOperationNoRepetitionAllowed("RulesOff");
  this->addOperationNoRepetitionAllowed("RulesOn");
  this->addOperationNoRepetitionAllowed("Freeze");
  this->addOperationNoRepetitionAllowed("\\infty");
  this->addOperationNoRepetitionAllowed("\\phantom");
  this->addOperationNoRepetitionAllowed("CommandEnclosure");
  this->addOperationNoRepetitionAllowed("MonomialPoly");
  this->addOperationNoRepetitionAllowed("Melt");
  this->addOperationNoRepetitionAllowed("Bind");
  this->addOperationNoRepetitionAllowed("\\limits");
  this->addOperationNoRepetitionAllowed("[)");
  this->addOperationNoRepetitionAllowed("(]");
  this->addOperationNoRepetitionAllowed("IntervalClosed");
  this->addOperationNoRepetitionAllowed("IntervalOpen");
  this->addOperationNoRepetitionAllowed("IndefiniteIndicator");
  this->addOperationNoRepetitionAllowed("\\log");
  //additional operations with the same status as user-input expressions.
  this->addOperationNoRepetitionAllowed("\\emptyset");
  this->addOperationNoRepetitionAllowed("\\pi");
  this->addOperationNoRepetitionAllowed("e");
  this->addOperationNoRepetitionAllowed("i");
  this->addOperationNoRepetitionAllowed("\\arctan");
  this->addOperationNoRepetitionAllowed("\\neq");
  this->addOperationNoRepetitionAllowed("CommandEnclosureStart");
  this->addOperationNoRepetitionAllowed("CommandEnclosureFinish");
  this->addOperationNoRepetitionAllowed("ExpressionHistory");
  this->addOperationNoRepetitionAllowed("ExpressionHistorySet");
  this->addOperationNoRepetitionAllowed("ExpressionHistorySetChild");
  this->addOperationNoRepetitionAllowed("SolveJSON");
  this->addOperationNoRepetitionAllowed("CompareExpressionsJSON");
}

void Calculator::initializeAtomsNonCacheable() {
  MacroRegisterFunctionWithName("Calculator::initializeAtomsNonCacheable");
  this->atomsThatMustNotBeCached.setExpectedSize(30);
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew("RandomInteger");
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew("SelectAtRandom");
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew("GenerateRandomPrime");
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew("PrintAlgebraicClosureStatus");
//  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew("PrintRuleStack");
}

void Calculator::initializeAtomsNotGoodForChainRule() {
  MacroRegisterFunctionWithName("Calculator::initializeAtomsNotGoodForChainRule");
  this->atomsNotAllowingChainRule.addOnTopNoRepetitionMustBeNew("Bind");
}

void Calculator::initializeStringsThatSplitIfFollowedByDigit() {
  MacroRegisterFunctionWithName("Calculator::initializeStringsThatSplitIfFollowedByDigit");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\cdot");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\circ");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\frac");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\ln");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\log");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\sin");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\cos");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\tan");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\cot");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\sec");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\csc");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\arctan");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\arcsin");
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew("\\arccos");
}

void Calculator::initializeAtomsThatAllowCommutingOfArguments() {
  MacroRegisterFunctionWithName("Calculator::initializeAtomsThatAllowCommutingOfArguments");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.setExpectedSize(30);
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("+");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("*");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("/");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("^");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("|");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("\\sin");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("\\cos");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.addOnTopNoRepetitionMustBeNew("\\log");
}

void Calculator::initializeArithmeticOperations() {
  MacroRegisterFunctionWithName("Calculator::initializeArithmeticOperations");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("+");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("-");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("*");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("/");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("^");
}

void Calculator::initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions() {
  MacroRegisterFunctionWithName("Calculator::initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions");
  //Related heavily to initializeOperationsInterpretedAsFunctionsMultiplicatively
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\sin");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\cos");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\tan");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\cot");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\sec");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\csc");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\log");
}

void Calculator::initializeOperationsInterpretedAsFunctionsMultiplicatively() {
  MacroRegisterFunctionWithName("Calculator::initializeOperationsInterpretedAsFunctionsMultiplicatively");
  //Related heavily to initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\sin");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\cos");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\tan");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\cot");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\sec");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\csc");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\arctan");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\arcsin");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\arccos");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.addOnTopNoRepetitionMustBeNew("\\log");
}

void Calculator::initializeOperationsThatAreKnownFunctions() {
  MacroRegisterFunctionWithName("Calculator::initializeOperationsThatAreKnownFunctions");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("+");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("-");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("*");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("/");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("^");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("|");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\sqrt");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\sin");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\cos");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\tan");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\cot");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\sec");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\csc");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\arctan");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\arcsin");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\arccos");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\log");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("LogBase");
}

void Calculator::addKnownDoubleConstant(const std::string& constantName, double value) {
  this->atomsNotInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew(constantName);
  Expression constant;
  constant.makeAtom(constantName, *this);
  this->knownDoubleConstants.addOnTopNoRepetitionMustBeNew(constant);
  this->knownDoubleConstantValues.addOnTop(value);
}

void Calculator::initializeBuiltInAtomsNotInterpretedAsFunctions() {
  MacroRegisterFunctionWithName("Calculator::initializeBuiltInAtomsNotInterpretedAsFunctions");
  this->atomsNotInterpretedAsFunctions.setExpectedSize(30);

  this->addKnownDoubleConstant("\\pi", MathRoutines::pi());
  this->addKnownDoubleConstant("e", MathRoutines::E());
  this->atomsNotInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\int");
  this->atomsNotInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("i");
}

void Calculator::addTrigonometricSplit(
  const std::string& trigonometricFunction, const List<std::string>& variables
) {
  MacroRegisterFunctionWithName("Calculator::addTrigonometricSplit");
  List<std::string> theSplit;
  for (int i = 0; i < variables.size; i ++) {
    const std::string& variable = variables[i];
    theSplit.setSize(0);
    theSplit.addOnTop("\\" + trigonometricFunction);
    theSplit.addOnTop(variable);
    this->predefinedWordSplits.setKeyValue(trigonometricFunction + variable, theSplit);
    this->predefinedWordSplits.setKeyValue("\\" + trigonometricFunction + variable, theSplit);
    theSplit.setSize(0);
    theSplit.addOnTop(variable);
    theSplit.addOnTop("\\" + trigonometricFunction);
    this->predefinedWordSplits.setKeyValue(variable + trigonometricFunction, theSplit);
    this->predefinedWordSplits.setKeyValue(variable + "\\" + trigonometricFunction, theSplit);
  }
  for (int i = 0; i < variables.size; i ++) {
    for (int j = 0; j < variables.size; j ++) {
      theSplit.setSize(0);
      theSplit.addOnTop(variables[i]);
      theSplit.addOnTop("\\" + trigonometricFunction);
      theSplit.addOnTop(variables[j]);
      this->predefinedWordSplits.setKeyValue(variables[i] + trigonometricFunction + variables[j], theSplit);
    }
  }
}

void Calculator::addOneStringAtomHandler(
  int atom,
  Expression::ToStringHandler handler
) {
  this->addOneStringHandler(atom, handler, this->toStringHandlersAtoms);
}

void Calculator::addOneStringCompositeHandler(
  int atom,
  Expression::ToStringHandler handler
) {
  this->addOneStringHandler(atom, handler, this->toStringHandlersComposite);
}

void Calculator::addOneStringHandler(
  int atom,
  Expression::ToStringHandler handler,
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction>& handlerCollection
) {
  if (handlerCollection.contains(atom)) {
    if (handlerCollection.getValueNoFail(
      atom) != handler
    ) {
      global.fatal << "More than one toStringHandler for atom "
      << this->operations.keys[atom] << "." << global.fatal;
    }
  } else {
    handlerCollection.setKeyValue(atom, handler);
  }
}

void Calculator::initializeToStringHandlers() {
  MacroRegisterFunctionWithName("Calculator::initializeToStringHandlers");
  this->addOneStringAtomHandler(this->opDefine()                , Expression::toStringDefine                      );
  this->addOneStringAtomHandler(this->opIsDenotedBy()           , Expression::toStringIsDenotedBy                 );
  this->addOneStringAtomHandler(this->opLog()                   , Expression::toStringLnAbsoluteInsteadOfLogarithm);
  this->addOneStringAtomHandler(this->opLogBase()               , Expression::toStringLogBase                     );
  this->addOneStringAtomHandler(this->opIntervalOpen()          , Expression::toStringIntervalOpen                );
  this->addOneStringAtomHandler(this->opIntervalRightClosed()   , Expression::toStringIntervalRightClosed         );
  this->addOneStringAtomHandler(this->opIntervalLeftClosed()    , Expression::toStringIntervalLeftClosed          );
  this->addOneStringAtomHandler(this->opIntervalClosed()        , Expression::toStringIntervalClosed              );
  this->addOneStringAtomHandler(this->opQuote()                 , Expression::toStringQuote                       );
  this->addOneStringAtomHandler(this->opDefineConditional()     , Expression::toStringDefineConditional           );

  this->addOneStringAtomHandler(this->opDivide()                , Expression::toStringDivide                      );
  this->addOneStringAtomHandler(this->opTensor()                , Expression::toStringTensor                      );
  this->addOneStringAtomHandler(this->opIn()                    , Expression::toStringIn                          );
  this->addOneStringAtomHandler(this->opOr()                    , Expression::toStringOr                          );
  this->addOneStringAtomHandler(this->opAnd()                   , Expression::toStringAnd                         );
  this->addOneStringAtomHandler(this->opBinom()                 , Expression::toStringBinom                       );
  this->addOneStringAtomHandler(this->opUnderscore()            , Expression::toStringUnderscore                  );
  this->addOneStringAtomHandler(this->opSetMinus()              , Expression::toStringSetMinus                    );
  this->addOneStringAtomHandler(this->opLimitBoundary()         , Expression::toStringLimitBoundary               );
  this->addOneStringAtomHandler(this->opTimes()                 , Expression::toStringTimes                       );
  this->addOneStringAtomHandler(this->opCrossProduct()          , Expression::toStringCrossProduct                );
  this->addOneStringAtomHandler(this->opSqrt()                  , Expression::toStringSqrt                        );
  this->addOneStringAtomHandler(this->opFactorial()             , Expression::toStringFactorial                   );
  this->addOneStringAtomHandler(this->opAbsoluteValue()         , Expression::toStringAbsoluteValue               );
  this->addOneStringAtomHandler(this->opThePower()              , Expression::toStringPower                       );
  this->addOneStringAtomHandler(this->opPlus()                  , Expression::toStringPlus                        );
  this->addOneStringAtomHandler(this->opDirectSum()             , Expression::toStringDirectSum                   );
  this->addOneStringAtomHandler(this->opMinus()                 , Expression::toStringMinus                       );
  this->addOneStringAtomHandler(this->opBind()                  , Expression::toStringBind                        );
  this->addOneStringAtomHandler(this->opEqualEqual()            , Expression::toStringEqualEqual                  );
  this->addOneStringAtomHandler(this->opEqualEqualEqual()       , Expression::toStringEqualEqualEqual             );
  this->addOneStringAtomHandler(this->opDifferentiate()         , Expression::toStringDifferentiate               );
  this->addOneStringAtomHandler(this->opDifferential()          , Expression::toStringDifferential                );
  this->addOneStringAtomHandler(this->opSum()                   , Expression::toStringSumOrIntegral               );
  this->addOneStringAtomHandler(this->opIntegral()              , Expression::toStringSumOrIntegral               );
  this->addOneStringAtomHandler(this->opGreaterThan()           , Expression::toStringGreaterThan                 );
  this->addOneStringAtomHandler(this->opGreaterThanOrEqualTo()  , Expression::toStringGreaterThanOrEqualTo        );
  this->addOneStringAtomHandler(this->opLessThanOrEqualTo()     , Expression::toStringLessThanOrEqualTo           );
  this->addOneStringAtomHandler(this->opLimit()                 , Expression::toStringLimit                       );
  this->addOneStringAtomHandler(this->opLimitProcess()          , Expression::toStringLimitProcess                );
  this->addOneStringAtomHandler(this->opLessThan()              , Expression::toStringLessThan                    );
  this->addOneStringAtomHandler(this->opSequence()              , Expression::toStringSequence                    );
  this->addOneStringAtomHandler(this->opLieBracket()            , Expression::toStringLieBracket                  );
  this->addOneStringAtomHandler(this->opMod()                   , Expression::toStringMod                         );
  this->addOneStringAtomHandler(this->opUnion()                 , Expression::toStringUnion                       );
  this->addOneStringAtomHandler(this->opIntersection()          , Expression::toStringIntersection                );
  this->addOneStringAtomHandler(this->opUnionNoRepetition()     , Expression::toStringUnionNoRepetition           );
  this->addOneStringAtomHandler(this->opError()                 , Expression::toStringError                       );

  this->addOneStringCompositeHandler(this->opMatrix()           , Expression::toStringMatrix                      );

  // To add a new type here, you need to define
  // Expression::toStringBuiltIn<your_new_type>
  this->addOneBuiltInHandler<std::string                                                              >();
  this->addOneBuiltInHandler<JSData                                                                   >();
  this->addOneBuiltInHandler<Rational                                                                 >();
  this->addOneBuiltInHandler<ElementEllipticCurve<Rational>                                           >();
  this->addOneBuiltInHandler<ElementEllipticCurve<ElementZmodP>                                       >();
  this->addOneBuiltInHandler<ElementZmodP                                                             >();
  this->addOneBuiltInHandler<InputBox                                                                 >();
  this->addOneBuiltInHandler<GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>>();
  this->addOneBuiltInHandler<ElementHyperoctahedralGroupR2                                            >();
  this->addOneBuiltInHandler<Polynomial<Rational>                                                     >();
  this->addOneBuiltInHandler<Polynomial<ElementZmodP>                                                 >();
  this->addOneBuiltInHandler<Polynomial<AlgebraicNumber>                                              >();
  this->addOneBuiltInHandler<PolynomialModuloPolynomial<ElementZmodP>                                 >();
  this->addOneBuiltInHandler<AlgebraicNumber                                                          >();
  this->addOneBuiltInHandler<RationalFunction<Rational>                                               >();
  this->addOneBuiltInHandler<RationalFunction<AlgebraicNumber>                                        >();
  this->addOneBuiltInHandler<RationalFunction<ElementZmodP>                                           >();
  this->addOneBuiltInHandler<Weight<Polynomial<Rational> >                                            >();
  this->addOneBuiltInHandler<SemisimpleLieAlgebra*                                                    >();
  this->addOneBuiltInHandler<ElementUniversalEnveloping<RationalFunction<Rational> >                  >();
  this->addOneBuiltInHandler<MatrixTensor<Rational>                                                   >();
  this->addOneBuiltInHandler<ElementTensorsGeneralizedVermas<RationalFunction<Rational> >             >();
  this->addOneBuiltInHandler<Plot                                                                     >();
  this->addOneBuiltInHandler<WeylGroupData                                                            >();
  this->addOneBuiltInHandler<ElementWeylGroup                                                         >();
  this->addOneBuiltInHandler<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>             >();
  this->addOneBuiltInHandler<VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>           >();
  this->addOneBuiltInHandler<CharacterSemisimpleLieAlgebraModule<Rational>                            >();
  this->addOneBuiltInHandler<SemisimpleSubalgebras                                                    >();
  this->addOneBuiltInHandler<double                                                                   >();
  this->addOneBuiltInHandler<AlgebraicNumber                                                          >();
  this->addOneBuiltInHandler<LittelmannPath                                                           >();
  this->addOneBuiltInHandler<ElementWeylAlgebra<Rational>                                             >();
}

void Calculator::initializePredefinedWordSplits() {
  MacroRegisterFunctionWithName("Calculator::initializePredefinedWordSplits");
  List<std::string> theSplit;
  List<std::string> theVars;
  theVars.addOnTop("x");
  theVars.addOnTop("y");
  theSplit.setSize(0);
  theSplit.addOnTop("x");
  theSplit.addOnTop("y");
  this->predefinedWordSplits.setKeyValue("xy", theSplit);
  theSplit.setSize(0);
  theSplit.addOnTop("y");
  theSplit.addOnTop("x");
  this->predefinedWordSplits.setKeyValue("yx", theSplit);
  this->addTrigonometricSplit("sin", theVars);
  this->addTrigonometricSplit("cos", theVars);
  this->addTrigonometricSplit("tan", theVars);
  this->addTrigonometricSplit("cot", theVars);
  this->addTrigonometricSplit("sec", theVars);
  this->addTrigonometricSplit("csc", theVars);
}

void Calculator::initializeBuiltInsFreezeArguments() {
  MacroRegisterFunctionWithName("Calculator::initializeBuiltInsFreezeArguments");
  this->atomsThatFreezeArguments.setExpectedSize(this->builtInTypes.size + 100);
  this->atomsThatFreezeArguments.addListOnTop(this->builtInTypes);
}

void Calculator::initializeAtomsThatFreezeArguments() {
  MacroRegisterFunctionWithName("Calculator::initializeAtomsThatFreezeArguments");
  this->atomsThatFreezeArguments.addOnTopNoRepetitionMustBeNew("Freeze");
  this->atomsThatFreezeArguments.addOnTopNoRepetitionMustBeNew("Bind");
}
