// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_lie_theory.h"
#include "math_general_implementation.h"
#include "calculator_weyl_group_characters.h"
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
// Hard-coded functions and operators.
std::string Calculator::Functions::Names::setRandomSeed = "SetRandomSeed";
std::string Calculator::Functions::Names::commandEnclosure =
"CommandEnclosure";
std::string Calculator::Functions::Names::setInputBox = "SetInputBox";
std::string Calculator::Functions::Names::sort = "Sort";
std::string Calculator::Functions::Names::transpose = "Transpose";
std::string Calculator::Functions::Names::approximations = "Approximations";
std::string Calculator::Functions::Names::vectorPartitionFunction =
"VectorPartitionFunction";
std::string Calculator::Functions::Names::turnOnRules = "TurnOnRules";
std::string Calculator::Functions::Names::turnOffRules = "TurnOffRules";
std::string Calculator::Functions::Names::Trigonometry::sine = "\\sin";
std::string Calculator::Functions::Names::Trigonometry::cosine = "\\cos";
// Built-in types.
std::string Calculator::BuiltInTypes::Names::elementTensorsGeneralizedVermas =
"ETGVM";
std::string Calculator::BuiltInTypes::Names::vectorPartitionFunction = "VPF";
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
    CalculatorDatabaseFunctions::executeMongoQuery,
    "",
    "Executes a mongoDB query. Requires administator rights. "
    "The database name is calculator (can't be modified). "
    "First argument: collection name. Second argument: query. ",
    "MongoFind(\"users\", \"{}\")",
    "CalculatorDatabaseFunctions::executeMongoQuery",
    "MongoFind",
    adminDefault
  );
}

// Naming conventions: please start all built-in calculator functions with
// capital letter.
// Exceptions are made for the following.
// 1) Functions that have mathematical names
// starting with small letters (example: \\sin, logical operations, etc.)
// Those typically start with a backslash.
// 2) Exceptions (desired or otherwise) are made for functions that have
// already
// been assigned keywords starting with non-capital letter
// and have been used too often.
// IMPORTANT.
// The order of registration of operation handlers for the same operation name
// defines the order in which operation
// handlers are called. Operations handlers registered first are executed
// first.
// The order only matters for different handlers of the same operation.
// This order is very important, as some of the
// handlers will act properly only if the preceding ones have been carried
// through
// (example: outerExtractBaseMultiplication requires outerAssociate).
// Note that one can easily transform the code so that the order does not
// matter.
// One can do that by ``packing'' the correct order of operations into a
// super-function,
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
  Function::Options innerFreezesArguments =
  Function::Options::innerFreezesArguments();
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
    Calculator::Functions::Names::setRandomSeed,
    CalculatorFunctions::setRandomSeed,
    "",
    "Sets the random seed of the calculator to the given integer value",
    "SetRandomSeed(123);\n"
    "RandomInteger(- 100, 100);\n"
    "RandomInteger(- 100, 100)",
    "CalculatorFunctions::setRandomSeed",
    Calculator::Functions::Names::setRandomSeed,
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
    "CalculatorFunctions::randomInteger",
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
    "CalculatorFunctions::turnOffRules",
    "TurnOffRules",
    innerStandard
  );
  this->addOperationHandler(
    "TurnOnRules",
    CalculatorFunctions::turnOnRules,
    "",
    "Turns on computational rules.",
    "TurnOffRules(\"sqrt\"); a = \\sqrt[4]{t}; TurnOnRules(\"sqrt\"); a",
    "CalculatorFunctions::turnOnRules",
    "TurnOnRules",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Functions::Names::approximations,
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
    Calculator::Functions::Names::approximations,
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
    "Turns on numerical approximations. "
    "Takes as input dummy argument. ",
    "TurnOffApproximations(0);\n"
    "ln(2);\n"
    "TurnOnApproximations(0);\n"
    "ln(2)",
    "CalculatorFunctions::turnOffApproximations",
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
    CalculatorFunctions::orFunction,
    "",
    "Logical or.",
    "0 or 0;\n"
    "0 or 1;\n"
    "1 or 0;\n"
    "1 or 1;\n"
    "a or 1;\n"
    "a or 0;",
    "CalculatorFunctions::or",
    "or",
    innerStandard
  );
  this->addOperationHandler(
    "or",
    CalculatorFunctions::orIdentical,
    "",
    "If the two arguments of or are identical, "
    "replaces the expression with the argument. "
    "Works even if the individual expression "
    "cannot be evaluated to 1 or 0. "
    "Please note that mathematically equal objects "
    "may fail to be identical, for example "
    "a rational number 5 and an algebraic number 5. ",
    "a or b; a or a",
    "CalculatorFunctions::orIdentical",
    "orIdentical",
    innerStandard
  );
  this->addOperationHandler(
    "IfStandard",
    CalculatorFunctions::ifStandard,
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
    "CalculatorFunctions::ifStandard",
    "IfStandard",
    innerStandard
  );
  this->addOperationHandler(
    "if",
    CalculatorFunctions::ifFrozen,
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
    "if (0, x, 1 / 0);\n",
    "CalculatorFunctions::ifFrozen",
    "if",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "and",
    CalculatorFunctions::andFunction,
    "",
    "Logical and. ",
    "0 and 0; 0 and 1; 1 and 0; 1 and 1; a and 1; a and 0;",
    "CalculatorFunctions::and",
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
    Calculator::Functions::Names::setInputBox,
    CalculatorHtmlFunctions::setInputBox,
    "",
    "Sets value for input box that overrides "
    "the input box (no box is displayed). ",
    "SetInputBox(name = a, value = 3); "
    "MakeInputBox(name = a)",
    "CalculatorHtmlFunctions::setInputBox",
    Calculator::Functions::Names::setInputBox,
    innerStandard
  );
  this->addOperationHandler(
    "Thaw",
    CalculatorFunctions::thaw,
    "",
    "If the argument is frozen, removes the top "
    "freeze command and returns the argument, "
    "else returns the argument unchanged.",
    "a =Freeze{}(1 + 1);\n"
    "Thaw a;\n"
    "c =Thaw(Freeze(a,b));\n"
    "PlotExpressionTree c",
    "CalculatorFunctions::thaw",
    "Thaw",
    innerStandard
  );
  this->addOperationHandler(
    "LogEvaluationStepsDebug",
    CalculatorSteps::logEvaluationStepsDebug,
    "",
    "Creates an expression evaluation tree. "
    "Intended for debugging the system. ",
    "LogEvaluationStepsDebug((x +2)(x +3))",
    "CalculatorSteps::logEvaluationStepsDebug",
    "LogEvaluationStepsDebug",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "LogEvaluationSteps",
    CalculatorSteps::logEvaluationSteps,
    "",
    "Show a step-by step evaluation of the calculator. "
    "Some steps my be combined or omitted for improved human readability. ",
    "LogEvaluationSteps((x +2)(x +3))",
    "CalculatorSteps::logEvaluationSteps",
    "LogEvaluationSteps",
    innerFreezesArguments
  );
  this->addOperationHandler(
    "FlattenCommandEnclosuresOneLayer",
    CalculatorBasics::flattenCommandEnclosuresOneLayer,
    "",
    "Flattens command enclosures. ",
    "FlattenCommandEnclosuresOneLayer(CommandEnclosure{}(x = 5; x); "
    "CommandEnclosure{}(y; x))",
    "CalculatorBasics::flattenCommandEnclosuresOneLayer",
    "FlattenCommandEnclosuresOneLayer",
    innerStandard
  );
  this->addOperationHandler(
    "ChildExpression",
    CalculatorFunctions::childExpression,
    "",
    "If defined, returns the nth child of an expression.",
    "ChildExpression(e^x, 1);\n"
    "ChildExpression(e^x, 2);\n"
    "ChildExpression(e^x, 3)",
    "CalculatorFunctions::childExpression",
    "ChildExpression",
    innerStandard
  );
  this->addOperationHandler(
    "ToString",
    CalculatorFunctions::expressionToString,
    "",
    "Transforms an arbitrary expression to its string representation. "
    "The string representation is (supposed to be) LaTeX-compatible. ",
    "ToString( e^x);\n"
    "\"e^x\";\"The quick brown fox jumps over the lazy dog.\"",
    "CalculatorFunctions::expressionToString",
    "ToString",
    innerStandard
  );
  this->addOperationHandler(
    "ToUTF8String",
    CalculatorFunctions::expressionToUTF8String,
    "",
    "Transforms an arbitrary expression "
    "to a utf8-string representation. "
    "The string is supposed to look reasonable "
    "when drawn on a javascript canvas. ",
    "f = \\sin {}x / \\cos {}x;\n"
    "Plot2D(f, -\\pi/4, \\pi/4) + "
    "PlotLabel{}((1, 1), \"y =\" + ToUTF8String(f));\n"
    "ToUTF8String(\\pi/4);",
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
    "Creates a string. "
    "Transforms escape sequences to their byte representations. "
    "These may be escaped back when returned to the frontend.",
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
    "CalculatorFunctionsBasic::logarithmBaseNCeiling",
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
    CalculatorFunctions::numerator,
    "",
    "If the expression is a fraction, returns the numerator, "
    "else returns the entire expression. ",
    "Numerator(a/b); Numerator(1+ 1/x)",
    "CalculatorFunctions::numerator",
    "Numerator",
    innerStandard
  );
  this->addOperationHandler(
    "ApplyToSubexpressionsRecurseThroughCalculusFunctions",
    CalculatorFunctions::applyToSubexpressionsRecurseThroughCalculusFunctions,
    "",
    "Applies a transformation, recursively to all subexpressions of "
    "a list of known functions. ",
    "ApplyToSubexpressionsRecurseThroughCalculusFunctions"
    "(Polynomialize, sin ((x + 1)^3))",
    "CalculatorFunctions::applyToSubexpressionsRecurseThroughCalculusFunctions"
    ,
    "ApplyToSubexpressionsRecurseThroughCalculusFunctions",
    innerStandard
  );
  this->addOperationHandler(
    "SubList",
    CalculatorFunctions::subList,
    "",
    "Extracts a sub-list that contains all elements that satisfy a "
    "condition given by the second argument. Use the bound "
    "variable syntax {{a}} to address the current element of the list.",
    "SubList{}((1, 2, 3, 4), {{a}} > 2)",
    "CalculatorFunctions::subList",
    "SubList",
    innerStandard
  );
  this->addOperationHandler(
    "ApplyToList",
    CalculatorFunctions::applyToList,
    "",
    "Applies a transformation to each element of a list. ",
    "SolveForX{}{{a}} = SolveFor(x, {{a}});\n"
    "SolveForX(x^2 + 2);\n"
    "ApplyToList(SolveForX,(x^2 + 1, x^2 - x - 2))",
    "CalculatorFunctions::applyToList",
    "ApplyToList",
    innerStandard
  );
  this->addOperationHandler(
    "Denominator",
    CalculatorFunctions::denominator,
    "",
    "If the expression is a fraction, returns the denominator, "
    "else returns 1. ",
    "Denominator(a/b);\n"
    "Denominator(1+ 1/x)",
    "CalculatorFunctions::denominator",
    "Denominator",
    innerStandard
  );
  this->addOperationHandler(
    "Product",
    CalculatorFunctions::multiplySequence,
    "",
    "Returns the product of the elements in a sequence. "
    "When used with zero arguments, returns 1.",
    "Product(a, b);\n"
    "Product{}(Sequence{}a);\n"
    "Product(Sequence{}a);\n"
    "Product(a)",
    "CalculatorFunctions::multiplySequence",
    "Product",
    innerStandard
  );
  this->addOperationHandler(
    "\\sum",
    CalculatorFunctions::sumSequence,
    "",
    "Returns the sum of the elements in a sequence. "
    "When used with one element, will return that element. ",
    "\\sum(a, b);\n"
    "\\sum();\n"
    "\\sum{}(Sequence{}a);\n"
    "\\sum(Sequence{}a);\n"
    "\\sum(a)",
    "CalculatorFunctions::sumSequence",
    "\\sum",
    innerStandard
  );
  this->addOperationHandler(
    "MakeExpression",
    CalculatorConversions::expressionFromBuiltInType,
    "",
    "Creates expression from built-in polynomial. ",
    "MakeExpression(Polynomial{}((x - 2y + z - 1)^2(x + y - z)));\n"
    "A = \\frac{64 x^{2} y x y + 16 x y - y}{- 32 x^{2} y x^{2} "
    "- 8 x^{2} + x};\n"
    "B = MakeRationalFunction(A);\n"
    "C = MakeExpression B;\n"
    "D = MakeRationalFunction {}("
    "\\frac{- 2x^{3}y^{2} - \\frac{x y}{2} + \\frac{y }{32}}"
    "{x^{4}y + \\frac{x^{2}}{4} - \\frac{x}{32}}"
    ");\n"
    "MakeExpression {}D - C;\n"
    "D - B;",
    "CalculatorConversions::expressionFromBuiltInType",
    "MakeExpression",
    innerStandard
  );
  this->addOperationHandler(
    "Polynomial",
    CalculatorConversions::getPolynomial<Rational>,
    "",
    "Creates a polynomial expression with rational coefficients. ",
    "Polynomial{}((x-2y+z- 1)^2(x +y-z));"
    "\nPolynomial{}(y^2)-(Polynomial{}y)^2",
    "CalculatorConversions::getPolynomial",
    "Polynomial",
    innerStandard
  );
  this->addOperationHandler(
    "PolynomialAlgebraicNumbers",
    CalculatorConversions::getPolynomial<AlgebraicNumber>,
    "",
    "Creates a polynomial expression with algebraic number coefficients. ",
    "PolynomialAlgebraicNumbers{}((x + \\sqrt{2})^2 (\\sqrt{3}x - \\sqrt{5}));"
    ,
    "CalculatorConversions::getPolynomial",
    "PolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationHandler(
    "PolynomialModP",
    CalculatorConversions::polynomialModuloInteger,
    "",
    "Creates a polynomial, with modular "
    "arithmetic coefficients, i.e., coefficients in Z / p Z. "
    "First argument = expression to convert to polynomial. "
    "Second argument = the number mod which we are computing. ",
    "PolynomialModP{}(x^2+x+1, 2);",
    "CalculatorConversions::polynomialModuloInteger",
    "PolynomialModP",
    innerStandard
  );
  this->addOperationHandler(
    "IsDifferentialOneFormOneVariable",
    CalculatorFunctions::isDifferentialOneFormOneVariable,
    "",
    "Tests whether the expression is a differential form in one variable. ",
    "IsDifferentialOneFormOneVariable(Differential x);\n"
    "IsDifferentialOneFormOneVariable({\\text{d}} x);\n"
    "IsDifferentialOneFormOneVariable(x {\\text{d}} y);\n"
    "IsDifferentialOneFormOneVariable(\\frac{\\text{d} y}{y});\n"
    "IsDifferentialOneFormOneVariable(1/(\\text{d} y));",
    "CalculatorFunctions::isDifferentialOneFormOneVariable",
    "IsDifferentialOneFormOneVariable",
    innerStandard
  );
  this->addOperationHandler(
    "Polynomialize",
    CalculatorFunctions::polynomialize,
    "",
    "Polynomialize(a) is equivalent to MakeExpression(Polynomial(a)).",
    "C = (c a + a b + b c + 1)^3;\n"
    "A = Polynomialize(C);\n"
    "B = MakeExpression(Polynomial(C));\n"
    "A - B;\n"
    "Polynomialize((a+a^-1+sqrt(-1))^2)",
    "CalculatorFunctions::polynomialize",
    "Polynomialize",
    innerStandard
  );
  this->addOperationHandler(
    "IsAlgebraicRadical",
    CalculatorFunctionsAlgebraic::isAlgebraicRadical,
    "",
    "Tests whether the expression is an algebraic expression "
    "obtained using radicals and the four arithmetic operations.  ",
    "IsAlgebraicRadical(\\sqrt{5 + \\sqrt{2}});\n"
    "IsAlgebraicRadical(\\sqrt{x});\n"
    "IsAlgebraicRadical(\\sqrt{\\pi + e + 1})",
    "CalculatorFunctionsAlgebraic::isAlgebraicRadical",
    "IsAlgebraicRadical",
    innerStandard
  );
  this->addOperationHandler(
    "IsReal",
    CalculatorFunctions::isReal,
    "",
    "Tests whether the expression is a real constant. ",
    "IsReal(\\sqrt{5 + \\sqrt{- 1}}); IsReal(\\sqrt{\\sqrt{5} - 1});\n"
    "IsReal(sqrt(\\sqrt{\\pi} - 2) )",
    "CalculatorFunctions::isReal",
    "IsReal",
    innerStandard
  );
  this->addOperationHandler(
    "IsConstant",
    CalculatorFunctions::isConstant,
    "",
    "Tests whether the expression is a constant.  ",
    "IsConstant(\\pi^2); IsConstant(1);IsConstant(x);\n"
    "IsConstant(e^{\\sin(\\pi^2 + e +\\sqrt{2} + 3)});",
    "CalculatorFunctions::isConstant",
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
    "Attempts to rearrange into standard "
    "polynomial form and then integrate.",
    "\\int \\left( \\frac{x(x + 1)}{2} \\right)^2 dx ",
    "CalculatorFunctionsIntegration::"
    "integratePowerByUncoveringParenthesisFirst",
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
    "\\int e^{5x} dx;\n"
    "\\int e^{-x} dx",
    "CalculatorFunctionsIntegration::integrateEpowerAxDiffX",
    "IntegrateEpowerX",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integratePullConstant,
    "",
    "Pulls out constants from integrals  ",
    "\\int 2\\sqrt{2-x^2} dx",
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
    "CalculatorFunctionsIntegration::integrateSqrtOneminusXsquared",
    "IntegrateSqrtOneminusXsquared",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne,
    "",
    "Integrates \\int \\sqrt{x^2-a}dx, a > 0.",
    "\\int 2\\sqrt{3x^2-5} dx ",
    "CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne",
    "IntegrateSqrtXsquaredMinusOne",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateXpowerNePowerAx,
    "",
    "Integrates by parts \\int x^n e^{a x} dx, "
    "where n is a positive integer.",
    "\\int 2 x^{3} e^{5x} dx ",
    "CalculatorFunctionsIntegration::integrateXpowerNePowerAx",
    "IntegrateXpowerNePowerAx",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsTrigonometry::eulerFormulaAsLaw,
    "",
    "Trigonometrizes an exponential expression using Euler's formula.",
    "TurnOnRules(\"EulerFormula\");\n"
    "e^{i x}",
    "CalculatorFunctionsTrigonometry::eulerFormulaAsLaw",
    "EulerFormula",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::powerImaginaryUnit,
    "",
    "Raises imaginary unit to an integer power. ",
    "TurnOnRules(\"PowerImaginaryUnit\");\n"
    "i^{-50!+ 1}; ",
    "CalculatorFunctions::powerImaginaryUnit",
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
    "CalculatorFunctionsTrigonometry::trigonometricSumToTrigonometricProduct",
    "TrigSumToTrigProduct",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\sin",
    CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry,
    "",
    "Implementation of sin (a + b) = sin(a) cos(b) + cos(a) sin(b).",
    "TurnOnRules(\"SineOfAngleSum\"); \\sin(a + b)",
    "CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry",
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
    "CalculatorFunctionsTrigonometry::cosineOfAngleSumToTrigonometry",
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
    "CalculatorFunctionsTrigonometry::convertSineToExponent",
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
    "CalculatorFunctionsTrigonometry::convertCosineToExponent",
    "ExpressCosViaExponent",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "FourierFractionForm",
    CalculatorFunctionsTrigonometry::fourierFractionForm,
    "",
    "Converts a trigonometric expression "
    "in one variable to fourier fraction form. ",
    "A = cos(2x)/(cos^3 x - sin^3 x);\n"
    "FourierFractionForm A;\n"
    "",
    "CalculatorFunctionsTrigonometry::fourierFractionForm",
    "FourierFractionForm",
    innerStandard
  );
  this->addOperationHandler(
    "\\cos",
    CalculatorFunctionsTrigonometry::exploitCosineEvenness,
    "",
    "If a is negative, converts cos (a) to cos(- a).",
    "TurnOnRules(\"ExploitCosEvenness\");\n"
    "cos(- 5x)",
    "CalculatorFunctionsTrigonometry::exploitCosineEvenness",
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
    "CalculatorFunctionsTrigonometry::exploitSineOddness",
    "ExploitSinOddness",
    innerStandardOffByDefault
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM,
    "",
    "Solves/transforms an integral of the form "
    "\\int \\sin^n x \\cos^m x. ",
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
    "CalculatorFunctionsIntegration::integrateTanPowerNSecPowerM",
    "IntegrateTanPowerNSecPowerM",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateSum,
    "",
    "If the integral is of the form \\int (A+B )dx, "
    "tries recursively to integrate A and B. "
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
    "CalculatorFunctionsDifferentiation::differentiateSqrt",
    "DifferentiateSqrt",
    innerStandard
  );
  // The function integrating the building
  // blocks must come in the exact order
  // below:
  // else we risk infinite substitution cycle. The reasons are
  // implementation-specific.
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuildingBlockIa,
    "",
    "Integrates building block Ia.",
    "\\int (\\frac{3}{(x / 2 - 1)}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa",
    "IntegrateBlockIa",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb,
    "",
    "Integrates building block Ib.",
    "\\int (\\frac{2}{(3x- 1)^2}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb",
    "IntegrateBlockIb",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::
    integrateRationalFunctionBuidingBlockIIaandIIIa,
    "",
    "Integrates building blocks IIa and IIIa.",
    "\\int (\\frac{3x + 2}{x^2 + x + 1}) dx ",
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIaandIIIa"
    ,
    "IntegrateBlockIIaIIIa",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb,
    "",
    "Integrates building blocks IIIb.",
    "\\int (\\frac{3x + 2}{(x^2 + x + 1)^2}) dx ",
    "CalculatorFunctionsIntegration::"
    "integrateRationalFunctionBuidingBlockIIIb",
    "IntegrateBlockIIIb",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb,
    "",
    "Integrates building blocks IIb.",
    "\\int (\\frac{3x + 2}{(x^2 + x + 1)^2}) dx ",
    "CalculatorFunctionsIntegration::"
    "integrateRationalFunctionBuidingBlockIIb",
    "IntegrateBlockIIb",
    innerStandard
  );
  this->addOperationHandler(
    "\\int",
    CalculatorFunctionsIntegration::
    integrateRationalFunctionSplitToBuidingBlocks,
    "",
    "Attempts to split an integral of a "
    "rational function into building block integrals. ",
    "\\int \\frac{1}{x(x + 1)} dx ",
    "CalculatorFunctionsIntegration::"
    "integrateRationalFunctionSplitToBuidingBlocks",
    "IntegratePartialFractions",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig,
    "",
    "Differentiation - product rule. ",
    "Differentiate(x, (\\sin x) \\cos x )",
    "CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig",
    "DifferentiateTrigAndInverseTrig",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateChainRule,
    "",
    "Differentiation - chain rule.  ",
    "Differentiate(x, \\sin x^2 \\cos (\\sin x))",
    "CalculatorFunctionsDifferentiation::differentiateChainRule",
    "DifferentiateChainRule",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAtimesB,
    "",
    "Differentiation - product rule.  ",
    "Differentiate(x, f * g)",
    "CalculatorFunctionsDifferentiation::differentiateAtimesB",
    "DifferentiateProductRule",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative,
    "",
    "Differentiation - division rule, commutative. "
    "For the time being differentiation "
    "is assumed to be over commutative rings. "
    "This might change in the future. "
    "The commutative division rule is "
    "d/dx (f/g^n) = ((d/dx f) g - n f (d/dx g))/g^{n + 1}.",
    "Differentiate(x, f / g);\n"
    "Differentiate(x, f / g^5);\n"
    "Differentiate(x, f / g^n)",
    "CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative",
    "DifferentiateQuotientRule",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAplusB,
    "",
    "Differentiation - sum rule.",
    "Differentiate(x, f + g)",
    "CalculatorFunctionsDifferentiation::differentiateAplusB",
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
    "CalculatorFunctionsDifferentiation::differentiateConstant",
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
    "CalculatorFunctionsDifferentiation::differentiateX",
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
    "CalculatorFunctionsDifferentiation::differentiateConstPower",
    "DifferentiateConstPower",
    innerStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctionsDifferentiation::differentiateAPowerB,
    "",
    "Differentiation: d/dx a^b = d/dx(e^{b\\log a}) = a^b d/dx(b\\log a) .",
    "Differentiate(x, x^2);\n"
    "d/dx x^- 1;\n"
    "d/dt t^{\\sqrt{2}};\n"
    "d/dc c^{DoubleValue 3.14}",
    "CalculatorFunctionsDifferentiation::differentiateAPowerB",
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
    CalculatorConversions::rationalFunction,
    "",
    "Creates a built-in rational function.",
    "MakeRationalFunction{}(x_1 + MakeRationalFunction{}x_1 + x_2)",
    "CalculatorConversions::rationalFunction",
    "MakeRationalFunction",
    innerStandard
  );
  this->addOperationHandler(
    "MakeMatrix",
    CalculatorConversions::makeMatrix,
    "",
    "Creates an internal c++ matrix structure from double list of rationals. ",
    "s_1 = MakeMatrix{}"
    "((- 1,- 1, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1));\n"
    "s_2 = MakeMatrix{}"
    "((1, 0, 0, 0), (- 1, - 1, - 1, 0), (0, 0, 1, 0), (0, 0, 0, 1));\n"
    "s_3 = MakeMatrix{}"
    "((1, 0, 0, 0), (0, 1, 0, 0), (0, - 2, - 1, - 1), (0, 0, 0, 1));\n"
    "s_4 = MakeMatrix{}"
    "((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, - 1, - 1)); ",
    "CalculatorConversions::makeMatrix",
    "MakeMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "MakeMatrixTensorForm",
    CalculatorConversions::matrixRationalTensorForm,
    "",
    "Same as MakeMatrix but uses different c++ implementation "
    "(class MatrixTensor instead of class Matrix). ",
    "s_1 = MakeMatrixTensorForm{}(\n"
    "(- 1,- 1, 0, 0),\n"
    "(0, 1, 0, 0),\n"
    "(0, 0, 1, 0),\n"
    "(0, 0, 0, 1)\n"
    "); ",
    "CalculatorConversions::matrixRationalTensorForm",
    "MakeMatrixTensorForm",
    innerStandard
  );
  this->addOperationHandler(
    "MakeMatrixRFs",
    CalculatorConversions::matrixRationalFunction,
    "",
    "Creates an internal C++ matrix structure "
    "from double list of polynomial functions. ",
    "s_1 = MakeMatrixRFs{}((1 - t, 2), (3, 2 - t))",
    "CalculatorConversions::matrixRationalFunction",
    "MakeMatrixRFs",
    innerStandard
  );
  this->addOperationHandler(
    "EnsureExpressionDependsOnlyOn",
    CalculatorFunctions::ensureExpressionDependsOnlyOnStandard,
    "",
    "Ensures the expression in the first argument depends "
    "only on the free variables given in the remaining arguments. "
    "Returns a warning string if "
    "that is not the case, else returns an empty string. "
    "Intended to warn users of potentially mistyped expressions. ",
    "EnsureExpressionDependsOnlyOn(\\sin (\\ln x) x y, x, y);\n"
    "EnsureExpressionDependsOnlyOn(\\sin (\\ln x) x y, x);",
    "CalculatorFunctions::ensureExpressionDependsOnlyOnStandard",
    "EnsureExpressionDependsOnlyOn",
    innerStandard
  );
  this->addOperationHandler(
    "EnsureExpressionDependsMandatoryVariables",
    CalculatorFunctions::
    ensureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants,
    "",
    "Similar to EnsureExpressionDependsOnlyOn, "
    "but requests that the expression "
    "depend on two lists, first "
    "with mandatory variables, second with non-mandatory. ",
    "EnsureExpressionDependsMandatoryVariables"
    "(\\sin (\\ln x) x y, x, y);\n"
    "EnsureExpressionDependsMandatoryVariables"
    "(\\sin (\\ln x) x y,  (x,y,z) );",
    "CalculatorFunctions::"
    "ensureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants",
    "EnsureExpressionDependsMandatoryVariables",
    innerStandard
  );
  this->addOperationHandler(
    "EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants",
    CalculatorFunctions::
    ensureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants,
    "",
    "Same as EnsureExpressionDependsMandatoryVariables "
    "including named constants such as \\pi, e. ",
    "EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants"
    "(2\\pi, \\pi, y);\n"
    "EnsureExpressionDependsMandatoryVariables"
    "(2, \\pi, none );",
    "CalculatorFunctions::"
    "ensureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants",
    "EnsureExpressionDependsMandatoryVariablesIncludeNamedConstants",
    innerStandard
  );
  this->addOperationHandler(
    "\\log",
    CalculatorFunctionsBasic::logarithm,
    "",
    "Logarithm function. "
    "Gives a decimal approximation of the natural "
    "logarithm provided the input is a double number. ",
    "\\log{}(e);\n"
    "\\log 10",
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
    "\\log_5 25;\n"
    "log_5 1;\n"
    "log_5 (1/5)",
    "CalculatorFunctionsBasic::logarithmBaseSimpleCases",
    "LogBaseSimpleCases",
    innerStandard
  );
  this->addOperationHandler(
    "LogBase",
    CalculatorFunctionsBasic::logarithmBase,
    "",
    "Logarithm function (arbitrary base). Equivalent to "
    "\\log_{{a}} {{b}}= (\\log b)/(\\log a). ",
    "\\log_10 5;\n"
    "\\log _e 10;\n"
    "\\ln 10;\n"
    "\\log_a b",
    "CalculatorFunctionsBasic::logarithmBase",
    "LogBase",
    innerStandard
  );
  this->addOperationHandler(
    "\\arctan",
    CalculatorFunctionsTrigonometry::arctan,
    "",
    "Arctan function. Tries to evaluate the arctan function. ",
    "\\arctan(3/4)",
    "CalculatorFunctionsTrigonometry::arctan",
    "\\arctan",
    innerStandard
  );
  this->addOperationHandler(
    "\\arcsin",
    CalculatorFunctionsTrigonometry::arcsinAlgebraic,
    "",
    "Arcsin function for special angles. ",
    "\\arcsin(1/2)",
    "CalculatorFunctionsTrigonometry::arcsinAlgebraic",
    "arcsinAlgebraic",
    innerStandard
  );
  this->addOperationHandler(
    "\\arccos",
    CalculatorFunctionsTrigonometry::arccosAlgebraic,
    "",
    "Arccos function for special angles. ",
    "\\arccos(\\sqrt{2}/2)",
    "CalculatorFunctionsTrigonometry::arccosAlgebraic",
    "arccosAlgebraic",
    innerStandard
  );
  this->addOperationHandler(
    "\\arccos",
    CalculatorFunctionsTrigonometry::arccos,
    "",
    "Arccos function. "
    "Tries to evaluate the arccos function. ",
    "\\arccos(3/4)",
    "CalculatorFunctionsTrigonometry::arccos",
    "\\arccos",
    innerStandard
  );
  this->addOperationHandler(
    "\\arcsin",
    CalculatorFunctionsTrigonometry::arcsin,
    "",
    "Arcsin function. "
    "Tries to evaluate the arcsin function. ",
    "\\arcsin(3/4)",
    "CalculatorFunctionsTrigonometry::arcsin",
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
    "Sine function. Evaluates to a decimal "
    "approximation if the input is a double number. ",
    "\\sin{}(3.1415)",
    "CalculatorFunctionsTrigonometry::sin",
    "Sine",
    innerStandard
  );
  this->addOperationHandler(
    "\\cos",
    CalculatorFunctionsTrigonometry::cos,
    "",
    "Cosine function. Evaluates to a decimal approximation "
    "if the input is a double number. ",
    "\\cos{}(3.1415)",
    "CalculatorFunctionsTrigonometry::cos",
    "Cosine",
    innerStandard
  );
  this->addOperationHandler(
    "\\tan",
    CalculatorFunctionsTrigonometry::tan,
    "",
    "Tangent function. Substitutes \\tan{}{{x}}=\\sin{}x/\\cos x. ",
    "\\tan{}(3.1415);\n"
    "\\tan 1.570796327",
    "CalculatorFunctionsTrigonometry::tan",
    "ExpressTanViaSineCosine",
    innerStandard
  );
  this->addOperationHandler(
    "\\cot",
    CalculatorFunctionsTrigonometry::cotangent,
    "",
    "Tangent function. Substitutes \\tan{}{{x}}=\\cos{}x/\\sin x. ",
    "\\cot{}(3.1415);\n"
    "\\cot 1.570796327",
    "CalculatorFunctionsTrigonometry::cotangent",
    "ExpressCotViaSineCosine",
    innerStandard
  );
  this->addOperationHandler(
    "\\csc",
    CalculatorFunctionsTrigonometry::csc,
    "",
    "Cosecant function. Substitutes \\csc{}{{x}}=1/\\sin x. ",
    "\\csc{}(3.1415);\n"
    "\\csc 1.570796327",
    "CalculatorFunctionsTrigonometry::csc",
    "ExpressCscViaSine",
    innerStandard
  );
  this->addOperationHandler(
    "\\sec",
    CalculatorFunctionsTrigonometry::sec,
    "",
    "Secant function. Substitutes \\sec{}{{x}}=1/\\sec x. ",
    "\\sec{}(3.1415);\n"
    "\\sec 1.570796327",
    "CalculatorFunctionsTrigonometry::sec",
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
    CalculatorFunctions::evaluateToDouble,
    "",
    "Double value of a rational number.",
    "DoubleValue{}(3/7)",
    "CalculatorFunctions::evaluateToDouble",
    "DoubleValue",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::degreesToRadians,
    "",
    "Converts degrees to radians",
    "30^\\circ",
    "CalculatorFunctions::degreesToRadians",
    "DegreesToRadians",
    innerStandard
  );
  this->addOperationHandler(
    "ModP",
    CalculatorFunctions::zModP,
    "",
    "Number modulo P. "
    "First argument = number, second argument = modulo.",
    "ModP{}(7, 3)",
    "CalculatorFunctions::zModP",
    "ModP",
    innerStandard
  );
  this->addOperationHandler(
    "GCD",
    CalculatorFunctions::greatestCommonDivisorInteger,
    "",
    "Greatest common divisor of two integers.",
    "GCD(100!+ 1, 101 * 103)",
    "CalculatorFunctions::greatestCommonDivisorInteger",
    "GCD",
    innerStandard
  );
  this->addOperationHandler(
    "LCM",
    CalculatorFunctions::leastCommonMultipleInteger,
    "",
    "Least common multiple of two integers.",
    "LCM(91, 1001)",
    "CalculatorFunctions::leastCommonMultipleInteger",
    "LCM",
    innerStandard
  );
  this->addOperationHandler(
    "GCDPoly",
    CalculatorFunctionsPolynomial::greatestCommonDivisorPolynomial,
    "",
    "Greatest common divisor polynomial of two polynomials. "
    "The divisor is scaled so that all coefficients "
    "are relatively prime integers, "
    "and so that the leading monomial under the "
    "graded lexicographic order (x_2>x_1, etc.) "
    "has positive coefficient.",
    "GCDPoly{}(x^3-1, x^2-1)\n;"
    "GCDPoly{}(2/3x^3-2/3, 10x^2-10)\n;"
    "GCDPoly( (a^2-b^2)(a^3-b^3), (a^4-b^4)(a^5-b^5));\n"
    "GCDPoly(PolynomialModP((x^2+3)(x^3+1), 7), "
    "PolynomialModP((x^2+3)(x^3+x+1), 7));\n"
    "f=PolynomialModP{}(x^{5}+x^{4}+3x^{3}+x^{2}+x +3,7);\n"
    "y=PolynomialModP{}(3x^{4}+2x^{3}+3x +3,7);\n"
    "GCDPoly(f, y);\n"
    "GCDPoly{}(x y, y+x z);\n"
    "GCDPoly{}("
    "-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}"
    "-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ "
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+"
    "2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ "
    "x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-"
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ "
    "x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, "
    "x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-"
    "2x_{13}^{3}x_{17}^{2}x_{21}^{3}- "
    "4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+"
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- "
    "2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+"
    "2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- "
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2})",
    "CalculatorFunctionsPolynomial::greatestCommonDivisorPolynomial",
    "GCDPoly",
    innerStandard
  );
  this->addOperationHandler(
    "LCMPoly",
    CalculatorFunctionsPolynomial::leastCommonMultiplePolynomial,
    "",
    "Least common multiple of two polynomials. "
    "The output is scaled so that all "
    "coefficients are relatively prime integers, "
    "and so that the leading monomial under "
    "the graded lexicographic order "
    "(x_2>x_1, etc.) has positive coefficient.",
    "LCMPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-"
    "2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ "
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+"
    "2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ "
    "x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-"
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ "
    "x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, "
    "x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-"
    "2x_{13}^{3}x_{17}^{2}x_{21}^{3}- "
    "4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+"
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- "
    "2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+"
    "2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- "
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2})",
    "CalculatorFunctionsPolynomial::leastCommonMultiplePolynomial",
    "LCMPoly",
    innerStandard
  );
  this->addOperationHandler(
    "Interpolate",
    CalculatorFunctions::interpolatePolynomial,
    "",
    "Constructs the one-variable polynomial of minimal degree that passes through "
    "the points. Points are given in the form "
    "((x_1, y_1),(x_2, y_2), ...,(x_n, y_n))",
    "Interpolate{}(1,0) ; Interpolate{}((1,0),(2,3));\n"
    "Interpolate{}((1,1), (2,2), (3, 4), (4, 8), (5, 16))",
    "CalculatorFunctions::interpolatePolynomial",
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
    "CalculatorFunctionsPolynomial::polynomialDivisionRemainder",
    "PolyDivRemainder",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivQuotient",
    CalculatorFunctionsPolynomial::polynomialDivisionQuotient,
    "",
    "Returns the quotients of a "
    "polynomial divided by a set of polynomials using "
    "the default monomial order (lexicographic).",
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
    "PolyDivSlidesGrLex{}(1, x^3 + x + 10, x +2);\n"
    "PolyDivSlidesGrLex{}(1,x + y + 10, x + 1, y - 1) ",
    "CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex",
    "PolyDivSlidesGrLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringGrLex",
    CalculatorFunctionsPolynomial::polynomialDivisionVerboseGrLex,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using "
    "gr lex order, for example, x^2 y^3 >x y^4, y^11>x^10. ",
    "PolyDivStringGrLex{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1);",
    "CalculatorFunctionsPolynomial::polynomialDivisionVerboseGrLex",
    "PolyDivStringGrLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringGrLexRev",
    CalculatorFunctionsPolynomial::
    polynomialDivisionVerboseGradedReverseLexicographic,
    "",
    "String that presents the division of a polynomial "
    "by a set of polynomials. "
    "Uses the graded reverse lexicographic order. "
    "More precisely, compares monomials by total degree. "
    "Ties are broken by comparing powers right to left. "
    "First **smaller** power on the right "
    "is declared the winner ('larger')."
    "For example,  x^2 y^3 > x y^4, x^11 > y^10. ",
    "PolyDivStringGrLexRev{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1);",
    "CalculatorFunctionsPolynomial::"
    "polynomialDivisionVerboseGradedReverseLexicographic",
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
    "PolyDivStringLex{}(x^7 + \\sqrt{2} x + 1, \\sqrt{3}x^2 + 2x + 1);\n"
    "PolyDivStringLex{}(PolynomialModP(x^5+2x^3+x^2+2,7),"
    "PolynomialModP(x^2, 7));",
    "CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic",
    "PolyDivStringLex",
    innerStandard
  );
  this->addOperationHandler(
    "PolyDivStringLexRev",
    CalculatorFunctionsPolynomial::
    polynomialDivisionVerboseLexicographicOpposite,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using the "
    "lexicographic order on the monimial exponents "
    "with reversed order of variables. "
    "For example, y^2 > x^1000 y > x y. "
    "Please do note that the lexicographic order "
    "is not related to the lexicographic order on the "
    "words 'expansions' (x^2y^3->xxyyy) of the monomials. ",
    "PolyDivStringLexRev{}(x^7 + 6x y + 5x y^8 + y^5, x^2 + 2, y^3 - 1) ;",
    "CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexRev",
    "PolyDivStringLexRev",
    innerStandard
  );
  this->addOperationHandler(
    "DFQEuler",
    CalculatorFunctions::differentialEquationsEulersMethod,
    "",
    "<b>Calculus teaching function.</b> Iterates Euler's method "
    "to approximate solutions of first order ordinary DFQ's. "
    "First argument = expression for y', "
    "second and third argument = x and y initial values, "
    "fourth argument = number of approximating points, "
    "fifth and sixth argument = left and right endpoints.",
    "DFQEuler(x^2 + y^2 - 1, 0, 0, 1000, - 2.5, 2.5)",
    "CalculatorFunctions::differentialEquationsEulersMethod",
    "DFQEuler",
    innerStandard
  );
  this->addOperationHandler(
    "NewtonsMethod",
    CalculatorFunctions::newtonsMethod,
    "",
    "Applies Newton's method with a given "
    "starting point and given number of iterations."
    "The first argument gives the "
    "function whose zeroes we are trying to find. "
    "The second argument gives the starting point. "
    "The last argument gives "
    "the number of iterations of the method. ",
    "NewtonsMethod(e^x - ( 200 sin{} x + x^7), 0, 10);\n"
    "NewtonsMethod(e^x = ( 200 sin{} x + x^7), 0, 10);",
    "CalculatorFunctions::newtonsMethod",
    "NewtonsMethod",
    innerStandard
  );
  this->addOperationHandler(
    "PlotDirectionField",
    CalculatorFunctionsPlot::plotDirectionField,
    "",
    "Plots a direction field (in 2d for the time being, 3d coming soon). "
    "Direction field is like a vector field except that all vectors are "
    "normalized to have the same length. "
    "First argument = the vector field. "
    "Second, third arguments: bottom left and "
    "top right corner of the "
    "viewing rectangle. "
    "Next argument: (numX,numY,...), where numX is the number "
    "of segments along the x axis and so on. "
    "Next argument: length of "
    "each direction line. "
    "Next arguments: color, line width. ",
    "PlotDirectionField((- y, x),(- 2, - 2),(2, 2), (20, 20), 0.2, blue, 1);",
    "CalculatorFunctionsPlot::plotDirectionField",
    "PlotDirectionField",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPolar",
    CalculatorFunctionsPlot::plotPolarRfunctionTheta,
    "",
    "<b>Calculus teaching function.</b> Draws polar curve given in "
    "polar coordinates in the form r = f(t), where t is the angle variable. "
    "The angle variable is measured in degrees. "
    "The first argument gives the function, "
    "the second and third argument "
    "give the upper and lower bounds of the angle. ",
    "PlotPolar(1+sin  t, 0, \\pi);\n"
    "PlotPolar((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t))"
    "(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\\pi)",
    "CalculatorFunctionsPlot::plotPolarRfunctionTheta",
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
    "CalculatorFunctionsPlot::plotPolarRfunctionThetaExtended",
    "PlotPolarExtended",
    innerStandard
  );
  this->addOperationHandler(
    "GaussianElimination",
    CalculatorFunctions::gaussianEliminationMatrix,
    "",
    "Gaussian elimination of a matrix. Prints a detailed string "
    "that shows the Gaussian elimination of a matrix.",
    "GaussianElimination(\\begin{array}{cccccc} "
    "\\sqrt 1 & \\sqrt 2 & \\sqrt 3 & 1 & 0 & 0\\\\ "
    "\\sqrt 4 & \\sqrt 5 & \\sqrt 6 & 0 & 1 & 0\\\\ "
    "\\sqrt 7 & \\sqrt 8 & \\sqrt 9 & 0 & 0 & 1\\end{array})",
    "CalculatorFunctions::gaussianEliminationMatrix",
    "GaussianElimination",
    innerStandard
  );
  this->addOperationHandler(
    "UnivariatePartialFractionAlgrithm",
    CalculatorFunctions::splitToPartialFractionsOverAlgebraicRealsAlgorithm,
    "",
    "Attempts to splits a univariate "
    "rational function into partial fractions "
    "over the real algebraic numbers. At the moment, "
    "the function factors the denominator "
    "over the rationals. If this factorization "
    "yields quadratic and linear terms at "
    "the function succeeds, else fails.",
    "UnivariatePartialFractionAlgrithm"
    "(\\frac{x^11}{x^{8}-2x^{6}-2x^{5}+4x^{3}+x^{2}-2});",
    "CalculatorFunctions::splitToPartialFractionsOverAlgebraicRealsAlgorithm",
    "UnivariatePartialFractionAlgrithm",
    innerStandard
  );
  this->addOperationHandler(
    "UnivariatePartialFractions",
    CalculatorFunctions::splitToPartialFractionsOverAlgebraicReals,
    "",
    "Attempts to splits a univariate rational function into partial "
    "fractions over the real algebraic numbers. "
    "If successful, produces a sequence containing the partial fractions. "
    "Else does nothing.",
    "A =\\frac{x^3}{x^{3}-x^2+2x^2-2};\n"
    "B=UnivariatePartialFractions(A);\n"
    "A-\\sum(B)",
    "CalculatorFunctions::splitToPartialFractionsOverAlgebraicReals",
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
    CalculatorFunctions::getPointsImplicitly,
    "",
    "Returns points on or close to the curve in two dimensions. "
    "Same as plotImplicit but rather than plotting "
    "the curve returns the (x,y) pairs of points found. "
    "The output given as a nx2 matrix of floating point numbers. ",
    "PointsImplicitly"
    "((x - 1) (y - 1) - ((x - 1) ^ 2 (y - 1) + 1) ^ 2, "
    "(- 2, - 2), (2, 2), (10, 10), 2000)",
    "CalculatorFunctions::getPointsImplicitly",
    "PointsImplicitly",
    innerStandard
  );
  this->addOperationHandler(
    "PlotImplicit",
    CalculatorFunctionsPlot::plotImplicitFunction,
    "",
    "Plots implicitly a curve given by the zeros "
    "of an expression in the letters "
    "x and y. The relation between x and y is "
    "assumed continuous at the points where it is defined. "
    "The function has not been optimized for speed, "
    "please use with care. "
    "The first argument gives the "
    "relation between x and y, the next two arguments give "
    "the lower left and upper right corners of "
    "the viewing screen in the format "
    "(lowLeftX, lowLeftY), (upperRightX, upperRightY). "
    "The next argument gives the initial grid "
    "precision in the form (numIntervalsVertical, numIntervalsHorizontal). "
    "The next two arguments give the width "
    "and height of the image in the format (width, height). "
    "The last argument is optional and "
    "gives an upper limit for the number "
    "of triangles to use (max =20000, default =2000). "
    "The triangle used to generate the "
    "implicit plot is algorithmically chosen.",
    "PlotImplicit"
    "((x - 1) (y - 1) - ((x - 1)^2 (y - 1) + 1)^2, "
    "(- 2, - 2), (2, 2), (10, 10))",
    "CalculatorFunctionsPlot::plotImplicitFunction",
    "PlotImplicit",
    innerStandard
  );
  this->addOperationHandler(
    "PlotImplicitShowGrid",
    CalculatorFunctionsPlot::plotImplicitShowGridFunction,
    "",
    "Same as plotImplicit but shows the underlying grid. "
    "The yellow grid is the initial one (specified by the user), "
    "and the gray grid is obtained by a "
    "subdivision which depends on the concrete function.",
    "PlotImplicitShowGrid"
    "((x- 1) (y- 1)-((x- 1)^2(y- 1) + 1)^2,"
    "(-2, -2), (2, 2), (10,10))",
    "CalculatorFunctionsPlot::plotImplicitShowGridFunction",
    "PlotImplicitShowGrid",
    innerStandard
  );
  this->addOperationHandler(
    "PlotCoordinateSystem",
    CalculatorFunctionsPlot::plotCoordinateSystem,
    "",
    "Plots a 3d coordinate system, "
    "fitting in a box given by two opposite corners.",
    "PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4));",
    "CalculatorFunctionsPlot::plotCoordinateSystem",
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
    "CalculatorFunctionsPlot::plotSetProjectionScreenBasis",
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
    "R=MakeInputBox(name = radiusBig, "
    "value = 2, min = 2, max = 5);\n"
    "r=MakeInputBox(name = radiusSmall, "
    "value = 0.6, min = 0.2, max = 1, step = 0.2);\n"
    "uSegments = MakeInputBox(name = uSegments, "
    "value = 22, min = 8, max = 40);\n"
    "vSegments = MakeInputBox(name = vSegments, "
    "value = 4, min = 2, max = 10);\n"
    "PlotSurface(\n"
    "(x, y, z), u \\in (0, 2\\pi), v \\in (- r, r), \n"
    "color1 = blue, color2 = cyan, "
    "numSegments1 = uSegments, numSegments2 = vSegments\n"
    ") + "
    "PlotSurface(\n"
    "( x + 2, z, y), u \\in (0, 2 \\pi), v \\in (- r,r), \n"
    "color1 = red, color2 = pink, "
    "numSegments1 = uSegments, numSegments2 = vSegments); ",
    "CalculatorFunctionsPlot::plotSurface",
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
    "PlotFill(PlotCurve"
    "((sin(a t), cos(b t)), 0, 2\\pi, blue, 2, 2000"
    "), pink)",
    "CalculatorFunctionsPlot::plotParametricCurve",
    "PlotCurve",
    innerStandard
  );
  this->addOperationHandler(
    "PlotSegment",
    CalculatorFunctionsPlot::plotSegment,
    "",
    "Plots a segment connecting two points. ",
    "PlotSegment((1,2), (3,4))",
    "CalculatorFunctionsPlot::plotSegment",
    "PlotSegment",
    innerStandard
  );
  this->addOperationHandler(
    "PlotMarkSegment",
    CalculatorFunctionsPlot::plotMarkSegment,
    "",
    "Plots a segment mark. ",
    "PlotSegment((1, 2), (3,4)) + PlotMarkSegment((1, 2), (3, 4))",
    "CalculatorFunctionsPlot::plotMarkSegment",
    "PlotMarkSegment",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPath",
    CalculatorFunctionsPlot::plotPath,
    "",
    "Plots a straight segment path. "
    "The path should be enclosed in parentheses, "
    "and color must be indicated. ",
    "PlotPath(((0, 0), (3, 0), (3, 4), (0, 0)), blue)",
    "CalculatorFunctionsPlot::plotPath",
    "PlotPath",
    innerStandard
  );
  this->addOperationHandler(
    "PlotSetId",
    CalculatorFunctionsPlot::plotSetId,
    "",
    "Creates an empty plot with a given canvas id. "
    "If you add a nameless plot to a named one "
    "the the resulting plot acquires the canvas id. ",
    "PlotSetId(myId) + PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4))",
    "CalculatorFunctionsPlot::plotSetId",
    "PlotSetId",
    innerStandard
  );
  this->addOperationHandler(
    "MatchesPattern",
    CalculatorFunctions::matchesPattern,
    "",
    "Checks whether the first argument matches "
    "the pattern of the second argument. If no, returns 0. "
    "Pattern parametric entries are indicated "
    "with the bound variable notation {{a}}. "
    "If the expression matches "
    "the pattern, the return is a command enclosure "
    "in which each of the variables is "
    "freed and assigned the matched value. ",
    "MatchesPattern{}(a = b * 10, a = {{c}} * b); "
    "MatchesPattern{}(a = b * 10, a = b * {{c}})",
    "CalculatorFunctions::matchesPattern",
    "MatchesPattern",
    innerStandard
  );
  this->addOperationHandler(
    "GetVariablesExcludeNamedConstants",
    CalculatorFunctions::getFreeVariablesExcludeNamedConstants,
    "",
    "Gets the variables on which the expression depends. "
    "Excludes the named constants. "
    "Here, the word ``variables'' is to be thought of as "
    "``free variables'' but the possibility for small distinctions is "
    "reserved (to allow dealing with named constants, "
    "reserved keywords, etc.). ",
    "GetVariablesExcludeNamedConstants{}"
    "(e^x + x + 5 +\\arctan x + x *y + x^y + x^{y^z});\n"
    "GetVariablesIncludeNamedConstants{}"
    "(e^x + x + 5 +\\arctan x + x *y + x^y + x^{y^z})",
    "CalculatorFunctions::getFreeVariablesExcludeNamedConstants",
    "GetVariablesExcludeNamedConstants",
    innerStandard
  );
  this->addOperationHandler(
    "GetVariablesIncludeNamedConstants",
    CalculatorFunctions::getFreeVariablesIncludeNamedConstants,
    "",
    "Gets the variables on which the expression depends. "
    "Includes the named constants. "
    "Here, the word ``variables'' is to be thought of as "
    "``free variables'' but the possibility for small distinctions is "
    "reserved (to allow dealing with named constants, reserved keywords, etc.). "
    ,
    "GetVariablesExcludeNamedConstants{}"
    "(e^x + x +5 +\\arctan x + x *y +x^y+x^{y^z});\n"
    "GetVariablesIncludeNamedConstants{}"
    "(e^x + x +5 +\\arctan x + x *y +x^y+x^{y^z})",
    "CalculatorFunctions::getFreeVariablesIncludeNamedConstants",
    "GetVariablesIncludeNamedConstants",
    innerStandard
  );
  this->addOperationHandler(
    "PlotPoint",
    CalculatorFunctionsPlot::plotPoint,
    "",
    "<b>Calculus teaching function.</b> "
    "Plots a point from x and y coordinate. "
    "Can also be used for multiple points. "
    "See the example for how to use for a single and multiple points.",
    "PlotPoint{}((1, 2\\pi), blue);\n"
    "PlotPoint{}(((1, 2), (2,3)), blue);\n"
    "a = MakeInputBox(name = a, value = 3);\n"
    "b = MakeInputBox(name = b, value = 5);\n"
    "PlotPoint((a, b), red)",
    "CalculatorFunctionsPlot::plotPoint",
    "PlotPoint",
    innerStandard
  );
  this->addOperationHandler(
    "Plot2D",
    CalculatorFunctionsPlot::plot2DOverIntervals,
    "",
    "If the second argument is a union of intervals, "
    "replaces the plot command with a sum of Plot2d's in which "
    "the second and third argument are extracted from each of the intervals.",
    "%UseBracketForIntervals\n"
    "Plot2D{}(\\sin{}x +cos{}x, "
    "[0, \\pi]\\cup [2\\pi, 3\\pi), \"blue\",2,8)",
    "CalculatorFunctionsPlot::plot2DOverIntervals",
    "Plot2DoverIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "Plot2D",
    CalculatorFunctionsPlot::plot2D,
    "",
    "<b>Calculus teaching function.</b> "
    "Makes a 2d plot of a function given in the form "
    "y = f(x). The the second and third argument give the upper and "
    "lower bounds of x. The remaining arguments are optional. "
    "The next argument specifies color. "
    "Next argument gives line width. "
    "Next argument gives number of sample points. "
    "Plots may be added together: "
    "adding plots superimposes the plots. ",
    "Plot2D{}(\\sin{}x +cos{}x, 0, 5, \"blue\",2,8)",
    "CalculatorFunctionsPlot::plot2D",
    "Plot2D",
    innerStandard
  );
  this->addOperationHandler(
    "IsPlot",
    CalculatorFunctionsPlot::isPlot,
    "",
    "Returns 1 if the argument is a plot, 0 otherwise.",
    "%UseBracketForIntervals \n"
    "IsPlot( Plot2D{}(\\sin{}x +cos{}x, "
    "[0, \\pi]\\cup [2\\pi, 3\\pi), \"blue\",2,8))",
    "CalculatorFunctionsPlot::isPlot",
    "IsPlot",
    innerStandard
  );
  this->addOperationHandler(
    "PlotFill",
    CalculatorFunctionsPlot::plotFill,
    "",
    "Fills a plot with color. ",
    "PlotFill(Plot2D{}(sqrt(1 - x^2), - 1, 1, \"blue\", 2) + "
    "Plot2D(- sqrt(1 - x^2), - 1, 1), \"blue\")",
    "CalculatorFunctionsPlot::plotFill",
    "PlotFill",
    innerStandard
  );
  this->addOperationHandler(
    "PlotRectangle",
    CalculatorFunctionsPlot::plotRectangle,
    "",
    "Plots a rectangle. "
    "Arguments format: "
    "PlotRectangle{}"
    "((lowerCornerLeftXcoord, lowerCornerLeftXcoord), (width, height)).",
    "PlotRectangle{}((1, 2), (2, 1))",
    "CalculatorFunctionsPlot::plotRectangle",
    "PlotRectangle",
    innerStandard
  );
  this->addOperationHandler(
    "PlotGrid",
    CalculatorFunctionsPlot::plotGrid,
    "",
    "Tells the plot to show axes ticks. "
    "Takes as input dummy (non-used) argument. ",
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
    "Plots a label at a given position. "
    "Arguments format: "
    "PlotLabel((Xcoord, Ycoord), \"Label\"). ",
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
    "Plot2D{}(1/x, -30, 30, \"red\") + "
    "PlotViewRectangle((-5,-5), (5,5))",
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
    "Plot2D{}(1/x, -30, 30, \"red\") + "
    "PlotWindow(400,400) + PlotViewRectangle((-5,-5), (5,5))",
    "CalculatorFunctionsPlot::plotViewWindow",
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
    "\nA =3/2- ((-3/4+ 1/4 (x))^{2});\n"
    "B= (1/4 (x))^{2}+2;\nPlot2DWithBars{}(A, B, 0, 5, 1)",
    "CalculatorFunctionsPlot::plot2DWithBars",
    "Plot2DWithBars",
    innerStandard
  );
  this->addOperationHandler(
    "CompareFunctionsNumerically",
    CalculatorFunctions::compareFunctionsNumerically,
    "",
    "<b>Calculus teaching function.</b> "
    "Compares two one-variable functions numerically. "
    "First two arguments give the two functions. "
    "Third and fourth arguments "
    "give the interval [a,b] over which to compare. "
    "Fifth argument is optional (default: 50) "
    "and gives the number of sampling points. "
    "Sixth argument is optional (default: 0.0001) "
    "and gives the numerical tolerance eps: if two numbers are "
    "at a distance less than eps they are considered equal. ",
    "CompareFunctionsNumerically{}(arctan (x), "
    "arcsin(x/\\sqrt{x^2+ 1}),0, 5,50,0.0001);\n"
    "CompareFunctionsNumerically{}(e^x, "
    "1+x +x^2/2+x^3/3!+x^4/4!+x^5/5!+x^6/6!,0, 5,50,0.0001)",
    "CalculatorFunctions::compareFunctionsNumerically",
    "CompareFunctionsNumerically",
    innerStandard
  );
  this->addOperationHandler(
    "CompareExpressionsNumerically",
    CalculatorFunctions::compareExpressionsNumerically,
    "",
    "Compares two expressions numerically. "
    "First two arguments give the two functions. "
    "The third argument gives the precision. "
    "All remaining arguments come in pairs, "
    "in the form x\\in (2,3), 50. ",
    "CompareExpressionsNumerically{}("
    "\\arctan x + \\arctan y, \\arctan((x +y)/(1- x y)), "
    "0.0001, x\\in(0,0.5), 20, y\\in(0,0.5), 20 );\n"
    "CompareExpressionsNumerically{}(\\arctan x + \\arctan y,"
    "\\arctan((x +y)/(1- x y)), 0.0001, x\\in(0,2), 20, y\\in(0,2), 20 );",
    "CalculatorFunctions::compareExpressionsNumerically",
    "CompareExpressionsNumerically",
    innerStandard
  );
  this->addOperationHandler(
    "CompareExpressionsNumericallyAtPoints",
    CalculatorFunctions::compareExpressionsNumericallyAtPoints,
    "",
    "<b>Calculus teaching function.</b> Compares "
    "two expressions numerically. First two arguments "
    "give the two functions. "
    "The third argument gives the precision. "
    "All remaining arguments come "
    "in pairs, in the form x\\in (2,3), 50. ",
    "CompareExpressionsNumericallyAtPoints{}("
    "\\arctan x + \\arctan y, \\arctan((x +y)/(1- x y)), 0.001, "
    "(x,y)\\in ((0.5,0.5),(0.3,0.3)));\n"
    "theFun =\\sqrt{x y}-(4 x^{2} y- 1);\n"
    "a =\\frac{-64 x^{3} y^{2}+ 16 x y+y}{32 x^{4} y-8 x^{2}- x};\n"
    "theDiff= d/dx(theFun);\n"
    "num= (d/dx y = 0; theDiff)_2;\n"
    "den = CoefficientOf (d/dx y, theDiff);\n"
    "b= -num/den;\n"
    "CompareExpressionsNumerically"
    "(a,b, 0.001, x\\in(0,5),10, y\\in (0,5),10);\n"
    "A = PointsImplicitly(theFun, (0,0),(4,4), (10,10), (400,400));\n"
    "CompareExpressionsNumericallyAtPoints(a,b, 0.01, (x,y)\\in A);",
    "CalculatorFunctions::compareExpressionsNumericallyAtPoints",
    "CompareExpressionsNumericallyAtPoints",
    innerStandard
  );
  this->addOperationHandler(
    "IsInteger",
    CalculatorFunctions::isInteger,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an integer, 0 otherwise. ",
    "IsInteger{}a;\n"
    "IsInteger{}1;\n"
    "f{}{{a}}=IsInteger{}a;\n"
    "f{}1;\nf{}b",
    "CalculatorFunctions::isInteger",
    "IsInteger",
    innerStandard
  );
  this->addOperationHandler(
    "IsEven",
    CalculatorFunctions::isEven,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an even integer, 0 otherwise. ",
    "i^{{n}} : if IsEven n = (- 1)^(n/2);\n"
    "i^100 ",
    "CalculatorFunctions::isEven",
    "IsEven",
    innerStandard
  );
  this->addOperationHandler(
    "IsSquareFree",
    CalculatorFunctions::isSquareFree,
    "",
    "If the argument is an integer, returns 1 if the "
    "integer is square-free "
    "(no primes in the decomposition of the integer "
    "appear with power greater than one) ",
    "IsSquareFree(6);\n"
    "IsSquareFree(12)",
    "CalculatorFunctions::isSquareFree",
    "IsSquareFree",
    innerStandard
  );
  this->addOperationHandler(
    "IsPower",
    CalculatorFunctions::isPower,
    "",
    "If the argument is an integer, returns 1 if the integer "
    "is plus or minus the power of a prime "
    "(no primes in the decomposition of the integer "
    "appear with power greater than one) ",
    "IsPower(6);\n"
    "IsPower(8)",
    "CalculatorFunctions::isPower",
    "IsPower",
    innerStandard
  );
  this->addOperationHandler(
    "IsSquare",
    CalculatorFunctions::isSquare,
    "",
    "If the argument is an integer, "
    "returns 1 if the number is the square of an integer. ",
    "IsSquare(8);\n"
    "IsSquare(16);\n"
    "IsSquare(100);",
    "CalculatorFunctions::isSquare",
    "IsSquare",
    innerStandard
  );
  this->addOperationHandler(
    "[)",
    CalculatorFunctions::intervalLeftClosedFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->left-closed interval.",
    "%UseBracketForIntervals PlotExpressionTree[1,2);\n"
    "PlotExpressionTree(1,2];\n"
    "PlotExpressionTree[1,2];",
    "CalculatorFunctions::intervalLeftClosedFromSequence",
    "[)",
    innerStandard
  );
  this->addOperationHandler(
    "(]",
    CalculatorFunctions::intervalRightClosedFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->right-closed interval.",
    "%UseBracketForIntervals "
    "PlotExpressionTree[1,2);\n"
    "PlotExpressionTree(1,2];\n"
    "PlotExpressionTree[1,2];",
    "CalculatorFunctions::intervalRightClosedFromSequence",
    "(]",
    innerStandard
  );
  this->addOperationHandler(
    "IntervalClosed",
    CalculatorFunctions::intervalClosedFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->closed interval.",
    "%UseBracketForIntervals PlotExpressionTree[1,2);\n"
    "PlotExpressionTree(1,2];\n"
    "PlotExpressionTree[1,2];",
    "CalculatorFunctions::intervalClosedFromSequence",
    "IntervalClosed",
    innerStandard
  );
  this->addOperationHandler(
    "IntervalOpen",
    CalculatorFunctions::intervalOpenFromSequence,
    "",
    "Internal data structure transformation: "
    "sequence ->open interval.",
    "%UseBracketForIntervals PlotExpressionTree (1,2); ",
    "CalculatorFunctions::intervalOpenFromSequence",
    "IntervalOpen",
    innerStandard
  );
  this->addOperationHandler(
    "IsNonEmptySequence",
    CalculatorFunctions::isNonEmptySequence,
    "",
    "Returns 1 if the input is a non-empty sequence; 0 otherwise. "
    "Does not reduce the expression if it has bound variables.",
    "IsNonEmptySequence(2,3);"
    "IsNonEmptySequence{}((2,3));\n"
    "IsNonEmptySequence{}1;\n"
    "a{}{{x}} = IsNonEmptySequence(x);\n"
    "a{}((2,3))\n",
    "CalculatorFunctions::isNonEmptySequence",
    "IsNonEmptySequence",
    innerStandard
  );
  this->addOperationHandler(
    "IsRational",
    CalculatorFunctions::isRational,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an rational, 0 otherwise. ",
    "IsRational{}a;"
    "IsRational{}- 1;\n"
    "f{}{{a}}=IsRational{}a;\n"
    "IsRational{}1;\n"
    "IsRational{}b",
    "CalculatorFunctions::isRational",
    "IsRational",
    innerStandard
  );
  this->addOperationHandler(
    "not",
    CalculatorFunctions::notFunction,
    "",
    "If the argument is a small integer, "
    "returns 1 if the argument is 0 and 1 the argument is non-zero. "
    "If the argument is not a small integer, does nothing. ",
    "not{}1;\n"
    "not{}a;\n"
    "not{}0;\n"
    "not{}(3==4)",
    "CalculatorFunctions::notFunction",
    "not",
    innerStandard
  );
  this->addOperationHandler(
    "DiagonalizeMatrix",
    CalculatorFunctionsLinearAlgebra::diagonalizeMatrix,
    "",
    "Tries to diagonalize a matrix (Jordan normal form). "
    "As of writing, will succeed only when the eigenvalues "
    "are rational or use quadratic radicals "
    "and the Jordan normal form is diagonal. "
    "When successful, returns a "
    "list of three matrices, X, A and X^-1, "
    "such that XAX^-1 equals the orignal "
    "matrix and A is in Jordan normal form.",
    "a=\\begin{pmatrix}1&1\\\\ 1&0\\end{pmatrix};\n"
    "DiagonalizeMatrix a",
    "CalculatorFunctionsLinearAlgebra::diagonalizeMatrix",
    "DiagonalizeMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "FunctionToMatrix",
    CalculatorFunctionsLinearAlgebra::functionToMatrix,
    "",
    "Creates a matrix from a function. "
    "The first argument gives the function, "
    "the second argument the number of rows, "
    "the third- the number of columns.",
    "X =FunctionToMatrix{}(A,5,5);\n"
    "A{}({{a}},{{b}})=a/b;\n"
    "X;\n"
    "\\det {} X",
    "CalculatorFunctionsLinearAlgebra::functionToMatrix",
    "FunctionToMatrix",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Functions::Names::transpose,
    CalculatorFunctions::transpose,
    "",
    "Transposes a matrix of expressions. ",
    "Transpose{}(1,2);\n"
    "(1,2)^t",
    "CalculatorFunctions::transpose",
    Calculator::Functions::Names::transpose,
    innerStandard
  );
  this->addOperationHandler(
    "\\det",
    CalculatorFunctions::determinant,
    "",
    "Tries to convert to a matrix of rationals or matrix of rational "
    "functions and computes the determinant if "
    "not too large.  ",
    "X =FunctionToMatrix{}(A,5,5);\n "
    "A{}({{a}},{{b}})=1/(a +b);\n"
    "X;\n"
    "\\det {} X;\n"
    "f{}({{a}}, {{b}}) = x_{10a+b};\n"
    "A=FunctionToMatrix(f, 3,3);\n"
    "\\det A",
    "CalculatorFunctions::determinant",
    "Determinant",
    innerStandard
  );
  this->addOperationHandler(
    "DeterminantPolynomial",
    CalculatorFunctions::determinantPolynomial,
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
    "CalculatorFunctions::determinantPolynomial",
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
    CalculatorFunctions::collectMultiplicands,
    "",
    "Converts a sum to a sequence containing the summands. ",
    "GetMultiplicandList(a*b*c) ",
    "CalculatorFunctions::collectMultiplicands",
    "GetMultiplicandList",
    innerStandard
  );
  this->addOperationHandler(
    "NormalizeIntervals",
    CalculatorFunctionsIntervals::normalizeIntervals,
    "",
    "Sorts interval union. ",
    "%UseBracketForIntervals\n"
    "NormalizeIntervals([2,3] \\cup [5, 7] \\cup [- 1,- 1/2]);",
    "CalculatorFunctionsIntervals::normalizeIntervals",
    "NormalizeIntervals",
    innerStandard
  );
  this->addOperationHandler(
    "\\cup",
    CalculatorFunctionsIntervals::unionUnionIntervals,
    "",
    "In for the expression "
    "a \\cup (b\\cup c) the expression a\\cup b "
    "can be reduced to d, "
    "replaces a\\cup(b\\cup c) by a\\cup d. ",
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
    "If the union of two intervals is one interval, "
    "replaces the interval. ",
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
    "CalculatorFunctionsListsAndSets::unionEmptySet",
    "UnionEmptySet",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctionsListsAndSets::intersectUnion,
    "",
    "Intersects with the empty set. ",
    "%UseBracketForIntervals\n([3, 7) \\cup (9, 10)) \\cap (4, 8.5);",
    "CalculatorFunctionsListsAndSets::intersectUnion",
    "IntersectUnion",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctions::intersectEmptySet,
    "",
    "Intersects with the empty set. ",
    "%UseBracketForIntervals\n  [3,7)\\cap \\emptyset;",
    "CalculatorFunctions::intersectEmptySet",
    "InersectEmptySet",
    innerStandard
  );
  this->addOperationHandler(
    "\\cap",
    CalculatorFunctionsIntervals::intersectIntervals,
    "",
    "Intersects two intervals. ",
    "%UseBracketForIntervals\n[3, 7) \\cup [6, 8);",
    "CalculatorFunctionsIntervals::intersectIntervals",
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
    CalculatorFunctions::compareIntervalsNumerically,
    "",
    "Compares unions of intervals numerically. "
    "First and second arguments: two unions of "
    "intervals to compare. "
    "Third argument: precisions to compare with. ",
    "%UseBracketForIntervals\n"
    "CompareIntervalsNumerically"
    "([3,7)\\cup [8,9], [3,7.00000001)\\cup [7.9999999, 9], 0.0001);",
    "CalculatorFunctions::compareIntervalsNumerically",
    "CompareIntervalsNumerically",
    innerStandard
  );
  this->addOperationHandler(
    "GetOpandList",
    CalculatorFunctions::collectOpands,
    "",
    "Converts a chain of operations to a list. "
    "First argument: operation name. "
    "You can expressions such as StringToAtom(\"+\") "
    "to make an arbitrary expresssion. ",
    "%UseBracketForIntervals\nGetOpandList"
    "(StringToAtom(\"\\otimes\"), a\\otimes b \\otimes c ); "
    "GetOpandList(StringToAtom(\"\\cup\"), [a,b]\\cup [c,d] \\cup [e,f] );",
    "CalculatorFunctions::collectOpands",
    "GetOpandList",
    innerStandard
  );
  this->addOperationHandler(
    "GetSummandList",
    CalculatorFunctions::collectSummands,
    "",
    "Converts a sum to a sequence containing the summands. ",
    "GetSummandList(1+a +b) ",
    "CalculatorFunctions::collectSummands",
    "GetSummandList",
    innerStandard
  );
  this->addOperationHandler(
    "GetSummand",
    CalculatorFunctions::getSummand,
    "",
    "Extracts the nth summand from a sum, "
    "<b>starts with the ZEROETH summand</b>. ",
    "GetSummand"
    "(\\sum_{n =1}^\\infty (- 1)^{2n + 1} x^{2n + 1}/(2n + 1)!, 5 )",
    "CalculatorFunctions::getSummand",
    "GetSummand",
    innerStandard
  );
  this->addOperationHandler(
    "GetFirstSummandContaining",
    CalculatorFunctions::getFirstSummandContaining,
    "",
    "Extracts the first summand containing a subexpression. ",
    "GetFirstSummandContaining(a +b+\\sum_{n =1}^\\infty"
    "((- 1)^{2n + 1} x^{2n + 1}/(2n + 1)!), \\sum ) ",
    "CalculatorFunctions::getFirstSummandContaining",
    "GetFirstSummandContaining",
    innerStandard
  );
  this->addOperationHandler(
    "RemoveDuplicates",
    CalculatorFunctions::removeDuplicates,
    "",
    "Removes duplicates. ",
    "RemoveDuplicates(a,a,c,a,b,a,b,d) ",
    "CalculatorFunctions::removeDuplicates",
    "RemoveDuplicates",
    innerStandard
  );
  this->addOperationHandler(
    Calculator::Functions::Names::sort,
    CalculatorFunctions::sort,
    "",
    "Sorts a sequence. ",
    "Sort(x^2, x^3, x^1, x^{- 1});\n"
    "Sort(3,2,1,4,3);\n"
    "Sort((3,2,3,1));\n"
    "Sort{}((3,2,3,1));\n",
    "CalculatorFunctions::sort",
    Calculator::Functions::Names::sort,
    innerStandard
  );
  this->addOperationHandler(
    "SortDescending",
    CalculatorFunctions::sortDescending,
    "",
    "Sorts a sequence in descending order.  ",
    "SortDescending(x^2, x^3, x^1, x^{- 1}) ",
    "CalculatorFunctions::sortDescending",
    "SortDescending",
    innerStandard
  );
  this->addOperationHandler(
    "BlocksOfCommutativity",
    CalculatorFunctions::getUserDefinedSubExpressions,
    "",
    "Returns subexpression blocks of commutativity. ",
    "BlocksOfCommutativity"
    "(\\sin x + x^2+ 3x y + 18x^{3/4 y}+\\sqrt{2}^{\\sqrt{2}c})",
    "CalculatorFunctions::getUserDefinedSubExpressions",
    "BlocksOfCommutativity",
    innerStandard
  );
  this->addOperationHandler(
    "InvertMatrixVerbose",
    CalculatorFunctions::invertMatrixVerbose,
    "",
    "<b>Calculus teaching function.</b> "
    "Inverts a matrix of rationals if invertible, "
    "in any other case generates an error. Makes a detailed "
    "printout of all Gaussian elimantion steps. ",
    "InvertMatrixVerbose((1, 2), (2, 3))",
    "CalculatorFunctions::invertMatrixVerbose",
    "InvertMatrixVerbose",
    innerStandard
  );
  this->addOperationHandler(
    "InvertMatrix",
    CalculatorFunctions::invertMatrix,
    "",
    "Inverts a matrix of rationals or "
    "algebraic numbers if invertible. ",
    "X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0));\n"
    "InvertMatrix X- X^{- 1}",
    "CalculatorFunctions::invertMatrix",
    "InvertMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "Trace",
    CalculatorFunctions::matrixTrace,
    "",
    "Gets trace of a square matrix. ",
    "X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0));\n"
    "Trace X",
    "CalculatorFunctions::matrixTrace",
    "Trace",
    innerStandard
  );
  this->addOperationHandler(
    "ReverseBytes",
    CalculatorFunctions::reverseBytes,
    "",
    "Reverses the bytes of a string. "
    "Does not respect utf-8 encoding. ",
    "ReverseBytes{}(\"abc\")",
    "CalculatorFunctions::reverseBytes",
    "ReverseBytes",
    innerStandard
  );
  this->addOperationHandler(
    "Reverse",
    CalculatorFunctions::reverseOrder,
    "",
    "Reverses order of elements. "
    "This operation will reverse products, lists, etc. "
    "More precisely, the command leaves the fist child in "
    "the internal representation of the object in place "
    "and flips the order of all other children.",
    "Reverse{}(s_1 s_2 s_3 s_4 s_2 s_3 s_1 s_2 s_3 s_4 s_1 s_2 s_3 s_2 s_1)",
    "CalculatorFunctions::reverseOrder",
    "Reverse",
    innerStandard
  );
  this->addOperationHandler(
    "ReverseRecursively",
    CalculatorFunctions::reverseOrderRecursively,
    "",
    "Same as Reverse but will apply recursively "
    "to the children expressions as well.",
    "ReverseRecursively{}(s_1 s_2 s_3s_4s_2s_3s_1s_2s_3s_4s_1s_2s_3s_2s_1)",
    "CalculatorFunctions::reverseOrderRecursively",
    "ReverseRecursively",
    innerStandard
  );
  this->addOperationHandler(
    "SolveFor",
    CalculatorFunctions::solveUnivariatePolynomialWithRadicalsWithRespectTo,
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
    "CalculatorFunctions::"
    "solveUnivariatePolynomialWithRadicalsWithRespectTo",
    "SolveFor",
    innerStandard
  );
  this->addOperationHandler(
    "CardanoFormula",
    CalculatorFunctions::cardanoFormula,
    "",
    "Applies the cardano formula to a cubic.",
    "CardanoFormula(x^3 + p x + q=0);\n"
    "CardanoFormula(x^3 + 2 x + 3);\n"
    "CardanoFormula(a x^3 + b x^2 + c x + d);\n",
    "CalculatorFunctions::cardanoFormula",
    "CardanoFormula",
    innerStandard
  );
  this->addOperationHandler(
    "\\sqrt",
    CalculatorFunctions::sqrt,
    "",
    "Square root of a rational, "
    "implemented as algebraic extension of the rationals. ",
    "\\sqrt 2+\\sqrt 3;(\\sqrt{}2+\\sqrt{}3+\\sqrt{}6)^2",
    "CalculatorFunctions::sqrt",
    "sqrt",
    innerStandard
  );
  this->addOperationHandler(
    "FactorOutNumberContent",
    CalculatorFunctions::factorOutNumberContent,
    "",
    "Factors out the rational number content of an additive expression. "
    "The expression part of the result "
    "must have integer relatively prime coefficients, "
    "with leading coefficient positive. ",
    "TurnOffRules(\"DistributeMultiplication\","
    "\"DistributeMultiplicationConstants\"); "
    "FactorOutNumberContent{}(3x + 9t + 27);",
    "CalculatorFunctions::factorOutNumberContent",
    "FactorOutNumberContent",
    innerStandard
  );
  this->addOperationHandler(
    "FactorUnivariatePolynomialOverRationalsKronecker",
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
    "FactorUnivariatePolynomialOverRationalsKronecker{}"
    "(x^{8}-44x^{6}+438x^{4}- 1292x^{2}+529);\n"
    "FactorUnivariatePolynomialOverRationalsKronecker{}"
    "(x^{8}+2x^{7}-3x^{6}-4x^{5}+6x^{4}+2x^{3}- 13x^{2}+ 1)",
    "CalculatorFunctionsPolynomial::factorPolynomialKronecker",
    "FactorUnivariatePolynomialOverRationalsKronecker",
    innerStandard
  );
  this->addOperationHandler(
    "FactorUnivariatePolynomialOverRationals",
    CalculatorFunctionsPolynomial::factorPolynomialFiniteFields,
    "",
    "Factors a one variable polynomial over the rationals "
    "using finite field methods. "
    "At the time of writing, the method used is probabilistic "
    "but with high chance of success, and pretty fast. ",
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
    "FactorUnivariatePolynomialOverRationalsDetailed",
    CalculatorFunctionsPolynomial::factorPolynomialFiniteFieldsDetailed,
    "",
    "Same as FactorUnivariatePolynomialOverRationals but "
    "prints out a summary of internal computations.",
    "%HideLHS\n"
    "FactorUnivariatePolynomialOverRationalsDetailed{}("
    "325x^{2}-38x-312"
    ");\n",
    "CalculatorFunctionsPolynomial::factorPolynomialFiniteFieldsDetailed",
    "FactorUnivariatePolynomialOverRationalsDetailed",
    innerStandard
  );
  this->addOperationHandler(
    "FactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields",
    CalculatorFunctionsPolynomial::factorPolynomialKroneckerThenFiniteFields,
    "",
    "Runs the Kronecker algorithm with a computation throttle; "
    "if unsuccessful, runs finite field algorithms. "
    "This can be considerably slower than directly running the finite field"
    "algorithm.",
    "%HideLHS\n"
    "FactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields{}(\n"
    "169 x^{11}-312 x^{10}+430 x^{9}-524 x^{8}+595 x^{7}"
    "-644 x^{6}+581 x^{5}-440 x^{4}+318 x^{3}-214 x^{2}+127 x-56\n"
    ");\n",
    "CalculatorFunctionsPolynomial::factorPolynomialKroneckerThenFiniteFields",
    "FactorUnivariatePolynomialOverRationalsKroneckerThenFiniteFields",
    innerNoTestInvisibleExperimental
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
    "CalculatorFunctions::factorInteger",
    "FactorInteger",
    innerStandard
  );
  this->addOperationHandler(
    "NumberOfIntegerDivisors",
    CalculatorFunctions::numberOfIntegerDivisors,
    "",
    "Computes the number of integer divisors "
    "(positive and negative) of a given number. "
    "For example, 6 has 8 divisors: 1,2,3,6 "
    "and their negatives.",
    "NumberOfIntegerDivisors(6);\n"
    "f{}1=Sequence{}(2);\n"
    "f{}{{x}}=f{}(x-1) \\cup Sequence{}( NumberOfIntegerDivisors(x));\n"
    "a=f{}100;\n"
    "Max a",
    "CalculatorFunctions::numberOfIntegerDivisors",
    "NumberOfIntegerDivisors",
    innerStandard
  );
  this->addOperationHandler(
    "CoefficientsPowersOf",
    CalculatorFunctions::coefficientsPowersOf,
    "",
    "Extracts the coefficients of the powers "
    "of the first argument in the second argument.",
    "CoefficientsPowersOf(x, a x^2+ b *3 x +c +\\pi +3)",
    "CalculatorFunctions::coefficientsPowersOf",
    "CoefficientsPowersOf",
    innerStandard
  );
  this->addOperationHandler(
    "ConstantTerm",
    CalculatorFunctions::constantTermRelative,
    "",
    "Extracts term constant relative to the variable in the first argument. ",
    "ConstantTerm(y,  x y x +3 +2z)",
    "CalculatorFunctions::constantTermRelative",
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
    CalculatorFunctions::isLinearOrConstantIn,
    "",
    "Returns one if the second argument is linear in the first. "
    "All variables are treated as non-constants. ",
    "IsLinearOrConstantIn(x, x^2 + 1);\n"
    "IsLinearOrConstantIn(x, x y + 1);\n"
    "IsLinearOrConstantIn(x, x + 1);\n"
    "IsLinearOrConstantIn(x, x \\pi + 1);",
    "CalculatorFunctions::isLinearOrConstantIn",
    "IsLinearOrConstantIn",
    innerStandard
  );
  this->addOperationHandler(
    "IsProductLinearOrConstantTermsIn",
    CalculatorFunctions::isProductLinearOrConstantTermsIn,
    "",
    "Returns true if the expression is "
    "a product of linear or constant terms. "
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
    CalculatorFunctions::isProductTermsUpToPower,
    "",
    "Returns true if the expression is a product of terms "
    "of power up to the given power. "
    "Although this is subject to change, at the moment "
    "powers of linear terms are not considered linear. ",
    "TurnOffRules(\"DistributeMultiplication\");\n"
    "IsProductTermsUpToPower(x^2, x^2+ 1);\n"
    "IsProductTermsUpToPower(x^3,x( x^2- 1));\n"
    "IsProductTermsUpToPower(x^2,x( x^2- 1));\n"
    "IsProductTermsUpToPower(x, (x- 1)(x + 1));\n"
    "IsProductTermsUpToPower(x, (2x + 1)(x \\pi + 1));\n"
    "IsProductTermsUpToPower(x, (2x +y)(x \\pi + 1));\n",
    "CalculatorFunctions::isProductTermsUpToPower",
    "IsProductTermsUpToPower",
    innerStandard
  );
  this->addOperationHandler(
    ";",
    CalculatorBasics::meltBrackets,
    "",
    "Melts down a layer of parenthesis around a list of "
    "commands prepended with the Melt operation.\n "
    "More precisely, cycles through "
    "all immediate children of the expression.\n"
    "If the k^th child X is a list "
    "of two elements starting with Melt, "
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
    "CalculatorBasics::outerMeltBrackets",
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
    "of the calculator. "
    "This fails because the associative rule is already implemented: "
    "the left hand side of the below expression is "
    "substituted with a*(b*c), and thus the rule becomes "
    "a*(b*c)=a*(b*c), which clearly is infinite substitution.",
    "%LogEvaluation\n"
    "({{a}}*{{b}})*{{c}}=a*(b*c);",
    "CalculatorBasics::checkRule",
    "CheckAutoEquality",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addStringToString,
    this->opString(),
    this->opString(),
    "Concatenates strings. ",
    "\"hello \"+ \"world\"",
    "CalculatorFunctionsBinaryOps::addStringToString",
    "AddStrings",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addRationalToRational,
    this->opRational(),
    this->opRational(),
    "Adds two rational numbers. ",
    "2 + 3",
    "CalculatorFunctionsBinaryOps::addRationalToRational",
    "AddRationals",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addElementZModPOrRationalToElementZModPOrRational,
    this->opRational(),
    this->opEltZmodP(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::"
    "addElementZModPOrRationalToElementZModPOrRational",
    "AddRationalToElementZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addElementZModPOrRationalToElementZModPOrRational,
    this->opEltZmodP(),
    this->opRational(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::"
    "addElementZModPOrRationalToElementZModPOrRational",
    "AddElementZmodPToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addElementZModPOrRationalToElementZModPOrRational,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::"
    "addElementZModPOrRationalToElementZModPOrRational",
    "AddElementZmodPToElementZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addPolynomialModuloIntegerToPolynomialModuloInteger,
    this->opPolynomialModuloInteger(),
    this->opPolynomialModuloInteger(),
    "Adds polynomials modulo integers. ",
    "a = PolynomialModP(x^2 + x + 7, 5);\n"
    "b = PolynomialModP(x^2 + 1, 5);\n"
    "a + b",
    "CalculatorFunctionsBinaryOps::"
    "addElementZModPOrRationalToElementZModPOrRational",
    "AddPolynomialZmodPToPolynomialZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addPolynomialModuloIntegerToInteger,
    this->opPolynomialModuloInteger(),
    this->opRational(),
    "Adds polynomials modulo integers. ",
    "a = PolynomialModP(x^2 + x + 7, 5);\n"
    "b = 3;\n"
    "a + b",
    "CalculatorFunctionsBinaryOps::addPolynomialModuloIntegerToInteger",
    "AddPolynomialZmodPToInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger
    ,
    this->opPolynomialModuloPolynomialModuloInteger(),
    this->opPolynomialModuloPolynomialModuloInteger(),
    "Adds polynomials modulo integers. ",
    "p = PolynomialModP(x^2 + x + 7, 5);\n"
    "a = PolynomialModP(x^2 + 1, 5);\n"
    "b = PolynomialModP(x^2, 5);\n"
    "a mod p;\n"
    "b mod p;\n"
    "a+b",
    "CalculatorFunctionsBinaryOps::"
    "addPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger"
    ,
    "AddPolynomialModPolynomialModPToPolynomialModPolynomialModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber,
    this->opAlgebraicNumber(),
    this->opAlgebraicNumber(),
    "Adds two algebraic numbers. ",
    "\\sqrt {2}+ \\sqrt {3} + \\sqrt{6}",
    "CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber",
    "AddAlgebraicNumberToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Adds algebraic number to rational. ",
    "1/(\\sqrt {2}+ 1+\\sqrt{3}+\\sqrt{6})",
    "CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber",
    "AddAlgebraicNumberToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber,
    this->opRational(),
    this->opAlgebraicNumber(),
    "Adds rational to algebraic number. ",
    "1/(1+\\sqrt {2}+\\sqrt{}6)",
    "CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber",
    "AddRationalToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational,
    this->opDouble(),
    this->opRational(),
    "Adds double or rational to a double or rational approximately "
    "using the built-in cpp addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational",
    "AddDoubleToRational",
    innerStandard
  );
  // must come before outer plus:
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addPlotToPlot,
    this->opCalculusPlot(),
    this->opCalculusPlot(),
    "Superimposes two plots. ",
    "Plot2D{}(sin{}(x), -5, 5) + Plot2D{}(1/sin{}(x ), 0.1, 3.041592654)",
    "CalculatorFunctionsBinaryOps::addPlotToPlot",
    "AddPlots",
    innerStandard
  );
  this->addOperationHandler(
    "ScaleToLeadingUnit",
    CalculatorFunctions::scaleToLeadingUnit,
    "",
    "Rescales an expression over the rationals "
    "so that the leading term has coefficient 1. ",
    "ScaleToLeadingUnit(1/2 x + 1/3 y+ 1/7 a b)",
    "CalculatorFunctions::scaleToLeadingUnit",
    "ScaleToLeadingUnit",
    innerStandard
  );
  List<std::string> additiveOperations = List<std::string>({"+", "-"});
  this->addOperationHandler(
    "+",
    CalculatorFunctions::sortTerms,
    "",
    "Sorts terms (over the rationals). "
    "Similar to AddTerms but doesn't combine "
    "monomial coefficients or drop zeroes. ",
    "2+3+a+2a+b+1+a",
    "CalculatorFunctions::sortTerms",
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
    "CalculatorFunctions::addTerms",
    "AddTerms",
    outerStandard,
    &additiveOperations
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctionsPolynomial::
    combineFractionsCommutativeWithInternalLibrary,
    "",
    "Combines fractions under the assumption "
    "that all participants commute. "
    "Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= "
    "(a * lcm(b,d)/b+c*lcm(b,d)/d)/(lcm(b,d)); "
    "Please note that this transformation "
    "is not correct if b and d do not commute. ",
    "a/b+c/d;\n"
    "z=(x-2)(x+1);\n"
    "w=(x-3)(x+1);\n"
    "1/z+1/w;\n"
    "1/(x+sqrt(20))+ 1/(x+sqrt(3));",
    "CalculatorFunctionsPolynomial::"
    "combineFractionsCommutativeWithInternalLibrary",
    "CommonDenominator",
    outerStandard
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctions::combineFractionsCommutative,
    "",
    "Combines fractions on condition that all participants commute. "
    "Similar to the CommonDenominator rule "
    "but does not compute least common multiples."
    "Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= (a *d+c*b)/(d*b); "
    "Please note that this transformation "
    "is not correct if b and d do not commute. ",
    "TurnOffRules(\"CommonDenominator\");\n"
    "a/b+c/d;\n"
    "z=(x-2)(x+1);\n"
    "w=(x-3)(x+1);\n"
    "1/z+1/w",
    "CalculatorFunctions::combineFractionsCommutative",
    "CommonDenominatorSimple",
    outerStandard
  );
  this->addOperationHandler(
    "+",
    CalculatorBasics::combineFractions,
    "",
    "Combines fractions. "
    "Equivalent to {{a}}/{{b}}+{{c}}= (a +c*b)/b; ",
    "f{}{{x}}= (2x +3)/(2x + 1);\ng{}{{y}}= (y-2)/(y+3);\ng{}f{}z;\nf{}g{}z",
    "CalculatorBasics::combineFractions",
    "CommonDenominatorOneNonFraction",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialRational(),
    "Adds a rational to a polynomial. ",
    "1+Polynomial{}\\lambda;\n"
    "Polynomial{}\\lambda + 1",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddRationalToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opRational(),
    "Adds a polynomial to a rational. ",
    "1+Polynomial{}\\lambda;\n"
    "Polynomial{}\\lambda + 1",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialRational(),
    "Adds a polynomial to a polynomial. ",
    "x = 1 + Polynomial{} \\lambda;\n"
    "x + x",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) + "
    "PolynomialAlgebraicNumbers(-\\sqrt{3}x)",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialOverAlgebraicNumbersToPolynomialOverAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}) + "
    "PolynomialAlgebraicNumbers(\\sqrt{3}x)",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddAlgebraicNumberToPolynomialOverAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opAlgebraicNumber(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) +"
    "PolynomialAlgebraicNumbers(-\\sqrt{3})",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialOverAlgebraicNumbersToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "1 + PolynomialAlgebraicNumbers(\\sqrt{12}x)",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddRationalToPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opRational(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) + 1",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialAlgebraicNumbersToRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opAlgebraicNumber(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(x) +\\sqrt{2}",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialAlgebraicNumbers(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "Polynomial{}x + PolynomialAlgebraicNumbers(\\sqrt{2}x)",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialToPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opPolynomialRational(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{2}x) + Polynomial{}x",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddPolynomialAlgebraicNumbersToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialRational(),
    "Adds a polynomial over the algebraic numbers "
    "to a polynomial over the algebraic numbers. ",
    "\\sqrt{2}+PolynomialAlgebraicNumbers(x)",
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial",
    "AddAlgebraicNumberToPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::
    addRationalOrPolynomialOrRationalFunctionToRationalFunction,
    this->opRationalFraction(),
    this->opRationalFraction(),
    "Adds a rational function to a rational function. ",
    "WeylDimFormula{}(a_2, (0,3)) + "
    "WeylDimFormula{}(a_2, (3,0)) + "
    "4 WeylDimFormula{}(a_2, (1,1)) ",
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrRationalFunctionToRationalFunction",
    "AddRationalFunctionToRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational,
    this->opRational(),
    this->opDouble(),
    "Adds double or rational to a double "
    "or rational approximately using the "
    "built-in cpp addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational",
    "AddRationalToDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational,
    this->opDouble(),
    this->opDouble(),
    "Adds double or rational to a double or "
    "rational approximately using the built-in cpp "
    "addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational",
    "AddDoubleToDouble",
    innerStandard
  );
  this->addOperationHandler(
    "+",
    CalculatorFunctionsBinaryOps::addMatrixToMatrix,
    "",
    "Adds two matrices.",
    "\\begin{pmatrix} 1& 2 \\\\ 2 & 3\\end{pmatrix} + \n"
    "\\begin{pmatrix} 1& 3 \\\\ 2 & 3\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::addMatrixToMatrix",
    "AddMatrices",
    innerStandard
  );
  this->addOperationHandler(
    "AugmentMatrixToTheRight",
    CalculatorFunctionsBinaryOps::augmentMatrixToTheRight,
    "",
    "Augments matrix to the right with another matrix. "
    "Pastes the content of the second matrix "
    "to the right of the first matrix."
    "The matrices must have the same number of rows. ",
    "AugmentMatrixToTheRight( ((1,1),(2,2)), ((0,0),(1,1)))",
    "CalculatorFunctionsBinaryOps::augmentMatrixToTheRight",
    "AugmentMatrixToTheRight",
    innerStandard
  );
  this->addOperationHandler(
    "AugmentMatrixBelow",
    CalculatorFunctionsBinaryOps::augmentMatrixBelow,
    "",
    "Augments matrix below by another matrix. "
    "Pastes the content of the second matrix below the first matrix."
    "The matrices must have the same number of columns. ",
    "AugmentMatrixBelow( ((1,1),(2,2)), ((0,0),(1,1)));\n"
    "a = FunctionToMatrix(3, 2,2);\n"
    "b = FunctionToMatrix(4, 2,3);\n"
    "AugmentMatrixBelow(b,a);\n"
    "AugmentMatrixBelow(a,a);\n"
    "AugmentMatrixBelow(b,b)",
    "CalculatorFunctionsBinaryOps::augmentMatrixBelow",
    "AugmentMatrixBelow",
    innerStandard
  );
  this->addOperationHandler(
    "\\oplus",
    CalculatorFunctionsBinaryOps::directSumMatrixWithMatrix,
    "",
    "Direct sum of two matrices, non-commutative.",
    "\\begin{pmatrix} 1& 2\\\\ 2& 3\\end{pmatrix}\\oplus\n"
    "\\begin{pmatrix} - 1& 3\\\\ -2& -3\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::directSumMatrixWithMatrix",
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
    "CalculatorFunctionsBinaryOps::multiplyMatrixByMatrix",
    "MultiplyMatrixByMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::multiplySequenceByMatrix,
    "",
    "Multiplies matrices.",
    "(1,2)\\begin{pmatrix} 1& 2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::multiplySequenceByMatrix",
    "MultiplySequenceByMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::multiplyMatrixBySequence,
    "",
    "Multiplies a matrix standing on the left by a "
    "sequence/list (matrix-row) standing on the right.\n"
    "This operation is valid only if "
    "the the matrix on the right is a column-vector.",
    "(1,2)\\begin{pmatrix} 1& 2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::multiplySequenceByMatrix",
    "MultiplyMatrixBySequence",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addSequenceToSequence,
    this->opSequence(),
    this->opSequence(),
    "Adds two sequences (termwise), provided the sequences "
    "have the same number of entries. In case the entries "
    "of the sequences are elements of a base field, "
    "corresponds to vector addition.",
    "v_{1}= (1, 2, 3);\n"
    "v_{2}= (1, 3, 2);\n"
    "v_{3}= (3, 1, 1);\n"
    "v_{4}= (-2, 2, 2);\n"
    "1/2v_{1}+ 1/2v_{2}+7/8v_{3}+ 13/16v_{4}",
    "CalculatorFunctionsBinaryOps::addSequenceToSequence",
    "AddSequences",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::addMatrixTensorToMatrixTensor,
    this->opMatrixTensorRational(),
    this->opMatrixTensorRational(),
    "Adds two matrices.",
    "A = MakeMatrixTensorForm{}((5, 8), (3, 5));\n"
    "3A * A - A;",
    "CalculatorFunctionsBinaryOps::addMatrixTensorToMatrixTensor",
    "AddMatrixTensorToMatrixTensor",
    innerStandard
  );
  this->addOperationHandler(
    "-",
    CalculatorBasics::minus,
    "",
    "Transforms a - b to a +(- 1) * b and - b to (- 1) * b. "
    "Equivalent to a rule "
    "-{{b}}=MinusOne * b; {{a}}-{{b}}=a + MinusOne * b",
    "- 1 + (- 5)",
    "CalculatorBasics::minus",
    "Minus",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsDifferentiation::interpretAsDifferential,
    "",
    "If contexts implies it, interprets an atom of "
    "the form dx as Differential {} x. ",
    "\\int x dx;"
    " \\int x (1 + x) dx; "
    "\\int_2^3 x dx; "
    "\\int_2^3 x (1 + x) dx",
    "CalculatorFunctionsDifferentiation::interpretAsDifferential",
    "InterpretAsDifferential",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsIntegration::integralOperator,
    "",
    "Transforms integral notation into an integral expression. ",
    "\\int x dx;\n"
    "\\int x d x;\n"
    "(\\int x) dx;\n"
    "\\int x (1+x) dx;\n"
    "\\int_2^3 x dx;\n"
    "\\int_2^3 x(1+x)dx;\n"
    "\\int\\theta d\\theta",
    "CalculatorFunctionsIntegration::integralOperator",
    "IntegralOperatorFromProduct",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::sumTimesExpressionToSumOf,
    "",
    "Transforms \\sum\\limits_{b}^c* a to (\\sum\\limits_b^c){} a. ",
    "PlotExpressionTree( \\sum\\limits_{b}^c);\n"
    "PlotExpressionTree( \\sum\\limits_{b}^c*a) ",
    "CalculatorFunctions::sumTimesExpressionToSumOf",
    "SumProductNotationToOperator",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::sumTimesExpressionToSumOf,
    "",
    "Transforms \\sum\\limits_{b}^c* a to (\\sum\\limits_b^c){} a. ",
    "PlotExpressionTree( \\sum\\limits_{b}^c);\n"
    "PlotExpressionTree( \\sum\\limits_{b}^c*a) ",
    "CalculatorFunctions::sumTimesExpressionToSumOf",
    "SumProductNotationToOperatorRelativeToDivision",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::handleUnderscorePowerLimits,
    "",
    "Handles expressions of the form \\limits_a^b",
    "\\limits_a^b; \\limits^b_a",
    "CalculatorFunctions::handleUnderscorePowerLimits",
    "LimitBoundaryNotationPower",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::handleUnderscorePowerLimits,
    "",
    "Handles expressions of the form \\limits_a^b",
    "\\limits_a^b; \\limits^b_a",
    "CalculatorFunctions::handleUnderscorePowerLimits",
    "LimitBoundaryNotationUnderscore",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny,
    "",
    "Replaces Differentiate{}(x)*a by Differentiate{}(x, a).",
    "d/dx (1/x) ",
    "CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny",
    "DifferentiateWithRespectToXTimeAny",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyEltZmodPorRatByEltZmodPorRat,
    this->opRational(),
    this->opEltZmodP(),
    "Multiplies elements of Z_p. ",
    "(2 mod 7) * 3",
    "CalculatorFunctionsBinaryOps::multiplyEltZmodPorRatByEltZmodPorRat",
    "MultiplyRationalByElementZmodP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyEltZmodPorRatByEltZmodPorRat,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Multiplies elements of Z_p. ",
    "(2 mod 7) * 3",
    "CalculatorFunctionsBinaryOps::multiplyEltZmodPorRatByEltZmodPorRat",
    "MultiplyElementZmodPByElementZModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber,
    this->opAlgebraicNumber(),
    this->opAlgebraicNumber(),
    "Multiplies two algebraic numbers. ",
    "\\sqrt{}2(\\sqrt {2}* \\sqrt {3} +\\sqrt{}2)",
    "CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber",
    "MultiplyAlgebraicNumberByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber,
    this->opRational(),
    this->opAlgebraicNumber(),
    "Multiplies two algebraic number by rational. ",
    "2(\\sqrt {2}+\\sqrt{}3)",
    "CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber",
    "MultiplyRationalByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyRationalByRational,
    this->opRational(),
    this->opRational(),
    "Multiplies two rationals. ",
    "2 * 3",
    "CalculatorFunctionsBinaryOps::multiplyRationalByRational",
    "MultiplyRationals",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational,
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
    "CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational",
    "MultiplyRationalByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational,
    this->opDouble(),
    this->opRational(),
    "Multiplies rational by a double "
    "approximately using the built-in cpp "
    "multiplication returning double. "
    "The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "DoubleValue{}((101)^{20});\n"
    "DoubleValue{}(DoubleValue{}((101)^{20}) + "
    "DoubleValue{}(1)) - "
    "DoubleValue{}(101^{20})",
    "CalculatorFunctionsBinaryOps::"
    "multiplyDoubleOrRationalByDoubleOrRational",
    "MultiplyDoubleByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational,
    this->opDouble(),
    this->opDouble(),
    "Multiplies rational by a double approximately "
    "using the built-in cpp multiplication "
    "returning double. "
    "The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "DoubleValue{}((101)^{20});\n"
    "DoubleValue{}(DoubleValue{}((101)^{20}) +"
    "DoubleValue{}(1))-"
    "DoubleValue{}(101^{20})",
    "CalculatorFunctionsBinaryOps::"
    "multiplyDoubleOrRationalByDoubleOrRational",
    "MultiplyDoubleByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opRational(),
    "Multiplies polynomial by a rational (polynomial comes first). ",
    "2*Polynomial{}(a +b);\n"
    "Polynomial{}(a +b)/2;\n"
    "Polynomial{}((a +b)^3)*Polynomial{}((a +c)^3);",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialRational(),
    "Multiplies rational by a polynomial (rational comes first). ",
    "2*Polynomial{}(a + b);\n"
    "Polynomial{}(a + b)/2;\n"
    "Polynomial{}((a + b)^3) * Polynomial{}((a + c)^3);",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyRationalByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialRational(),
    "Multiplies two polynomials. ",
    "2*Polynomial{}(a +b);\n"
    "Polynomial{}(a +b)/2;\n"
    "Polynomial{}((a +b)^3) * Polynomial{}((a +c)^3);",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opPolynomialAlgebraicNumbers(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*"
    "PolynomialAlgebraicNumbers{}(\\sqrt{6}y);",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialAlgebraicNumbersByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyPolynomialModPByPolynomialModP,
    this->opPolynomialModuloInteger(),
    this->opPolynomialModuloInteger(),
    "Multiply two polynomials modulo an integer. ",
    "a = PolynomialModP( x^2 + x + 1, 7 );\n"
    "b = PolynomialModP( x^3 + x + 1, 7 );\n"
    "a * b;\n"
    "a * PolynomialModP( x^3 + x + 1, 5 );",
    "CalculatorFunctionsBinaryOps::multiplyPolynomialModPByPolynomialModP",
    "MultiplyPolynomialModPByPolynomialModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP,
    this->opPolynomialModuloPolynomialModuloInteger(),
    this->opPolynomialModuloPolynomialModuloInteger(),
    "Multiply two polynomials modulo an integer. ",
    "p = PolynomialModP( x^4 + x + 1, 7 );\n"
    "a = PolynomialModP( x^3 + x + 1, 7 );\n"
    "b = PolynomialModP( x + 2, 7 );\n"
    "f = a mod p;\n"
    "g = b mod p;\n"
    "f * g",
    "CalculatorFunctionsBinaryOps::"
    "multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP",
    "MultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialAlgebraicNumbers(),
    this->opAlgebraicNumber(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*\\sqrt{6};",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialAlgebraicNumbersByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialAlgebraicNumbers(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "\\sqrt{6}*PolynomialAlgebraicNumbers{}(\\sqrt{3}x);",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyAlgebraicNumberByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opRational(),
    this->opPolynomialAlgebraicNumbers(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*2;",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyRationalByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opPolynomialRational(),
    this->opAlgebraicNumber(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "Polynomial{}(x)*\\sqrt{2};",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyPolynomialByAlgebraicNumber",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyNumberOrPolynomialByNumberOrPolynomial,
    this->opAlgebraicNumber(),
    this->opPolynomialRational(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "\\sqrt{3} * PolynomialAlgebraicNumbers{}(x);",
    "CalculatorFunctionsBinaryOps::"
    "multiplyNumberOrPolynomialByNumberOrPolynomial",
    "MultiplyAlgebraicNumberByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyMatrixTensorOrRationalByMatrixTensor,
    this->opMatrixTensorRational(),
    this->opMatrixTensorRational(),
    "Multiplies matrix rational by matrix tensor. ",
    "M = MakeMatrixTensorForm{}((1, 1), (0, 1));\n"
    "M;\n"
    "M * M;\n"
    "M * M * M;\n"
    "M * M * M * M;\n"
    "2 * M",
    "CalculatorFunctionsBinaryOps::"
    "multiplyMatrixTensorOrRationalByMatrixTensor",
    "MultiplyMatrixTensorByMatrixTensor",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyMatrixTensorOrRationalByMatrixTensor,
    this->opRational(),
    this->opMatrixTensorRational(),
    "Multiplies rational by matrix tensor form. ",
    "M = MakeMatrixTensorForm{}((1, 3, 5), (- 1, 1, 0), (2, 2, 7));\n"
    "M;\n"
    "M * M;\n"
    "M * M * M;\n"
    "M * M * M * M;\n"
    "2 * M ",
    "CalculatorFunctionsBinaryOps::"
    "multiplyMatrixTensorOrRationalByMatrixTensor",
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
    "CalculatorBasics::multiplyAtoXtimesAtoYequalsAtoXplusY",
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
    "CalculatorBasics::multiplyByOne",
    "MultiplyByOne",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::outerCommuteConstants,
    "",
    "Rule that commutes constants to the left-most positions. "
    "Provided that a is a constant number "
    "(built in) and b is not, replaces b*a by a*b. ",
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
    "(a*b)*(c*(d*e)*f) - a*b*c*d* e *f;\n"
    "(a*b)*(c*(e * d)*f) - a*b*c*d* e *f",
    "CalculatorBasics::associate",
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
    "x(x + 1)^{- 1}x;\n"
    "x(y+ 1)^{- 1}x",
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
    "CalculatorBasics::extractBaseMultiplication",
    "ConstantExtraction",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::associateTimesDivision,
    "",
    "Associative law w.r.t. division. ",
    "a*(b/c);\n"
    "(a*b)/c-a*(b/c)",
    "CalculatorBasics::associateTimesDivision",
    "AssociateTimesDivision",
    outerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorBasics::cancelMultiplicativeInverse,
    "",
    "Cancels multiplicative inverse. ",
    "(a*b)/b; (a/b)*b",
    "CalculatorBasics::cancelMultiplicativeInverse",
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
    "CalculatorBasics::distributeTimes",
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
    "CalculatorBasics::distributeTimesConstant",
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
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction,
    this->opRational(),
    this->opRationalFraction(),
    "Multiplies rational number by a rational function.",
    "WeylDimFormula{}(a_2, (0,3)) + "
    "WeylDimFormula{}(a_2, (3,0)) + "
    "4 WeylDimFormula{}(a_2, (1,1)) ",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyRationalByRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction,
    this->opPolynomialRational(),
    this->opRationalFraction(),
    "Multiplies polynomial by a rational fraction.",
    "Polynomial{}(x +1)MakeRationalFunction{}(\\frac{-2x -2}{x^2+x });\n"
    "MakeRationalFunction{}(\\frac{-2x -2}{x^2-x })"
    "MakeRationalFunction{}(\\frac{-2x -2}{x^2+x })",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyPolynomialByRationalFraction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction,
    this->opRationalFraction(),
    this->opPolynomialRational(),
    "Multiplies rational fraction by polynomial.",
    "Polynomial{}(x +1)MakeRationalFunction{}(\\frac{-2x -2}{x^2+x });\n"
    "MakeRationalFunction{}(\\frac{-2x -2}{x^2-x })"
    "MakeRationalFunction{}(\\frac{-2x -2}{x^2+x })",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyRationalFractionByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::
    multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction,
    this->opRationalFraction(),
    this->opRationalFraction(),
    "Multiplies rational function by a rational function.",
    "Polynomial{}(x +1)MakeRationalFunction{}(\\frac{-2x -2}{x^2+x });\n"
    "MakeRationalFunction{}(\\frac{-2x -2}{x^2-x })"
    "MakeRationalFunction{}(\\frac{-2x -2}{x^2+x })",
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction",
    "MultiplyRationalFunctionByRationalFunction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence,
    this->opRational(),
    this->opSequence(),
    "Carries out multiplication between a rational number on left "
    "and sequence on the right. "
    "Corresponds to multiplying a vector by a scalar "
    "(however please note a sequence does not necessarily consist "
    "of elements of a field, so the latter "
    "interpretation might not be applicable).",
    "v_{1}= (1, 2, 3);\n"
    "v_{2}= (1, 3, 2);\n"
    "v_{3}= (3, 1, 1);\n"
    "v_{4}= (-2, 2, 2);\n"
    "1/2v_{1}+ 1/2v_{2}+7/8v_{3}+ 13/16v_{4}",
    "CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence",
    "ScalarTimesSequence",
    innerStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAnyScalarByMatrix,
    "",
    "Multiplies a rational number by a matrix",
    "2\\begin{pmatrix}  1& 2& 3\\\\2& 1& 0 \\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::multiplyAnyScalarByMatrix",
    "ScalarTimesMatrix",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence,
    this->opDouble(),
    this->opSequence(),
    "Carries out multiplication between a double number on left "
    "and sequence on the right. "
    "Corresponds to multiplying a vector by a scalar.",
    "%NumberColors\n"
    "(1, 2) - DoubleValue{} 1 (2,3)",
    "CalculatorFunctionsBinaryOps::"
    "multiplyAnyScalarBySequence",
    "MultiplyDoubleBySequence",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence,
    this->opAlgebraicNumber(),
    this->opSequence(),
    "Multiplies a double number on left "
    "and a sequence on the right.scalar.",
    "(1 ,2)- DoubleValue{} 1 (2,3)",
    "CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence",
    "MultiplyAlgebraicNumberBySequence",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence,
    this->opPolynomialRational(),
    this->opSequence(),
    "Carries out multiplication between a rational number on left "
    "and sequence on the right.",
    "x = Polynomial{}x; v=x*(1, 2, 3);",
    "CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence",
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
    CalculatorFunctions::valueOfModularExpression,
    "",
    "Given an expression of the form a mod b, returns the element a. "
    "When the input is a modular polynomial, "
    "converts the polynomial to a rational. ",
    "Value(3 mod 7);\n"
    "Value(9 mod 7);\n"
    "a = Value((PolynomialModP(x^9, 7)) mod (PolynomialModP(x^5+x+1, 7)));\n"
    "Value a",
    "CalculatorFunctions::valueOfModularExpression",
    "Value",
    innerStandard
  );
  this->addOperationHandler(
    "mod",
    CalculatorFunctions::zModP,
    "",
    "Same as ModP but uses the mod notation.",
    "7 mod 3",
    "CalculatorFunctions::zModP",
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
    "CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP",
    "moduloPolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "\\setminus",
    CalculatorFunctionsBinaryOps::setMinus,
    "",
    "Removes the elements of the second set "
    "from the elements of the first set. "
    "The outputs will be sorted in ascending order.",
    " (x,y,t) \\setminus Sequence{}x; (x,y)\\setminus (z,y)",
    "CalculatorFunctionsBinaryOps::setMinus",
    "\\setminus",
    innerStandard
  );
  this->addOperationHandler(
    "Differential",
    CalculatorFunctions::differentialStandardHandler,
    "",
    "Transforms Differential{}a to the standard "
    "internal form Differential {}(a, 1).",
    "\\int \\theta {\\text d} \\theta",
    "CalculatorFunctions::differentialStandardHandler",
    "DifferentialStandardHandler",
    innerStandard
  );
  this->addOperationHandler(
    "Differential",
    CalculatorFunctions::differentialOfPolynomial,
    "",
    "Differential of a polynomial.",
    "Differential{}(Polynomial{}(x+y));\n"
    "Differential{}(Polynomial{}("
    "x^3y z+y z + x y + x^5 y^2 z + x y^2 z"
    "));\n",
    "CalculatorFunctions::differentialOfPolynomial",
    "DifferentialOfPolynomial",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsIntegration::integralOperator,
    "",
    "Transforms integral notation into an integral expression. ",
    "\\int x dx;\n"
    "\\int x (1+x) dx;\n"
    "\\int_2^3 x dx;\n"
    "\\int_2^3 x(1+x)dx",
    "CalculatorFunctionsIntegration::integralOperator",
    "IntegralOperatorFromQuotient",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::divideEltZmodPorRatByEltZmodPorRat,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Divides elements of Z/pZ. ",
    "(2 mod  7) / (3 mod 7)",
    "CalculatorFunctionsBinaryOps::divideEltZmodPorRatByEltZmodPorRat",
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
    "CalculatorFunctionsDifferentiation::"
    "diffdivDiffxToDifferentiation",
    "LeibnizDifferentialOperatorToDifferentialOperator",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsDifferentiation::ddivDxToDiffDivDiffx,
    "",
    "Replaces d/dx by Differential /Differential{}x. "
    "Note that the variable of differentiation "
    "is expected to be the string following the d letter. ",
    "d/dx x",
    "CalculatorFunctionsDifferentiation::ddivDxToDiffDivDiffx",
    "LeibnizDifferentialOperatorNotation",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational,
    this->opAlgebraicNumber(),
    this->opAlgebraicNumber(),
    "Divides algebraic numbers. ",
    "1/(1+\\sqrt{}2+\\sqrt{}3+\\sqrt{}5+\\sqrt{}7)",
    "CalculatorFunctionsBinaryOps::"
    "divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational",
    "DivideAlgebraicByAlgebraic",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational,
    this->opRational(),
    this->opAlgebraicNumber(),
    "Divides rational by algebraic number. ",
    "1/(\\sqrt{}2+\\sqrt{}3+\\sqrt{}5)",
    "CalculatorFunctionsBinaryOps::"
    "divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational",
    "DivideRationalByAlgebraic",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Divides algebraic number by rational. ",
    "(\\sqrt{}2+\\sqrt{}3+\\sqrt{}5)/5",
    "CalculatorFunctionsBinaryOps::"
    "divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational",
    "DivideAlgebraicByRational",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::divideByNumber,
    "",
    "If b is rational, algebraic, or double, "
    "substitutes (anything)/b with anything* (1/b).",
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
    "CalculatorBasics::subZeroDivAnythingWithZero",
    "DivideZeroDivideByAnything",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::divisionCancellations,
    "",
    "Division cancellations. "
    "Substitutes (a/b)/(a/d) with d/a and (a/b)/(c/b) with a/c. ",
    "(a/b)/(a/d); (a/b)/(c/b)",
    "CalculatorFunctions::divisionCancellations",
    "DivideCancellations",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::associateDivisionDivision,
    "",
    "Substitutes (a/b)/c =a/(c*b); a/(b/c)=a*c/b. "
    "Note the order of multiplication in the rules: this operation is safe "
    "and correct for non-commutative rings as well.",
    "(a/b)/c;\n"
    "a/(b/c);",
    "CalculatorFunctions::associateDivisionDivision",
    "AssociateDivisionDivision",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::associateAdivBdivCPowerD,
    "",
    "Substitutes a/(b/c)^d =a c^d/b^d."
    "Note the order of multiplication in the rules: "
    "this operation is safe and correct for "
    "non-commutative rings as well.",
    "a/(b/c)^d;",
    "CalculatorFunctions::associateAdivBdivCPowerD",
    "AssociateAdividedByBdividedByCpowerD",
    outerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::atimesBpowerJplusEtcDivBpowerI,
    "",
    "Rule: (a_0 + a_1 x^{c_1}+ ... + a_n x^{c_n}) /x^t "
    "=a_0 x^{-t}+ a_1 x^{c_1-t}+...+a_n x^{c_n-t} ",
    "(a x^{1/2} + b x )/x;\n"
    "(a x^{1/2} + b x )/x^2;",
    "CalculatorFunctions::atimesBpowerJplusEtcDivBpowerI",
    "ConvertShortDenominatorToNegativePower",
    outerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::divideRationalByRational,
    this->opRational(),
    this->opRational(),
    "Divides two rational numbers. ",
    "4/6; 2/0;",
    "CalculatorFunctionsBinaryOps::divideRationalByRational",
    "DivideRationalByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::divideDoubleByDouble,
    this->opRational(),
    this->opDouble(),
    "Divides doubles. ",
    "a = 0.5;\n"
    "b= 0.5;\n"
    "c = DoubleValue{}3.3;\n"
    "a/c;\n"
    "c/a;\n"
    "c/c",
    "CalculatorFunctionsBinaryOps::divideDoubleByDouble",
    "DivideRationalByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::divideDoubleByDouble,
    this->opDouble(),
    this->opDouble(),
    "Divides doubles. ",
    "a = 0.5;\n"
    "b = 0.5;\n"
    "c = DoubleValue{}3.3;\n"
    "a/c;\n"
    "c/a;\n"
    "c/c",
    "CalculatorFunctionsBinaryOps::divideDoubleByDouble",
    "DivideDoubleByDouble",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial,
    this->opRational(),
    this->opPolynomialRational(),
    "Divides rational by polynomial (to get a rational function).",
    "z = Polynomial{}(x^2+y^2);\n"
    "1/z",
    "CalculatorFunctionsBinaryOps::"
    "divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial"
    ,
    "DivideRationalByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial,
    this->opPolynomialRational(),
    this->opPolynomialRational(),
    "Divides polynomial by polynomial (to get a rational function). ",
    "Polynomial{}(-x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}+x_{2}+ 1)/\n"
    "Polynomial{}(x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}-x_{2}+ 1) ",
    "CalculatorFunctionsBinaryOps::"
    "divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial"
    ,
    "DividePolynomialByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial,
    this->opRationalFraction(),
    this->opPolynomialRational(),
    "Divides rational fraction by polynomial (to get a rational fraction). ",
    "MakeRationalFunction(x)/Polynomial(y) ",
    "CalculatorFunctionsBinaryOps::"
    "divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial"
    ,
    "DivideRationalFractionByPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial,
    this->opPolynomialRational(),
    this->opRationalFraction(),
    "Divides rational fraction by polynomial (to get a rational fraction). ",
    "Polynomial(x) /\n"
    "MakeRationalFunction(y) ",
    "CalculatorFunctionsBinaryOps::"
    "divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial"
    ,
    "DividePolynomialByRationalFraction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial,
    this->opRationalFraction(),
    this->opRationalFraction(),
    "Divides rational fraction by a rational fraction. ",
    "MakeRationalFunction{}(x) / MakeRationalFunction{}(y);\n"
    "Polynomial{}(x) / MakeRationalFunction{}(y);\n"
    "MakeRationalFunction{}(x) / Polynomial{}(y);\n"
    "MakeRationalFunction{}(x) / 2;\n"
    "2 / MakeRationalFunction{}(y)",
    "CalculatorFunctionsBinaryOps::"
    "divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial"
    ,
    "DivideRationalFractionByRationalFraction",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::
    dividePolynomialModuloIntegerByPolynomialModuloInteger,
    this->opPolynomialModuloInteger(),
    this->opPolynomialModuloInteger(),
    "Divides polynomial modulo integer by another polynomial modulo integer. ",
    "PolynomialModP{}(x^5+5x^3+x^2+4x+1, 7) / "
    "PolynomialModP{}(x^5+6x^3+x^2+5x+1, 7);\n"
    "PolynomialModP{}(x^3-x+1, 3) / PolynomialModP{}(x^2+2, 3);\n"
    "PolynomialModP{}(x^3-x+1, 3) / PolynomialModP{}(x^2+2, 7);\n"
    "PolynomialModP{}(x^3-x+1, 6) / PolynomialModP{}(2x^2+2, 6);\n"
    "PolynomialModP{}(x^3-x+1, 21) / PolynomialModP{}(2x^2+2, 21);\n",
    "CalculatorFunctionsBinaryOps::"
    "dividePolynomialModuloIntegerByPolynomialModuloInteger",
    "DividePolynomialModPByPolynomialModP",
    innerStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial,
    "",
    "Assumes that the numerator and denominator of a fraction commute. "
    "Divides the two expressions "
    "under the assumption that both can be converted to"
    "polynomials with rational coefficients.",
    "(2x^2+3x-5)/(5x^4+x^2+2x-8);\n"
    "x^2/x;\n"
    "(x+1)^2/(x+1)",
    "CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial",
    "DivideExpressionsAsIfPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerRationalByInteger,
    this->opRational(),
    this->opRational(),
    "Raises rational to power, provided the power is a small integer. ",
    "{3^3}^3; 3^{3^3}; 3^3^3; 0^3; 0^{-3}; ",
    "CalculatorFunctionsBinaryOps::powerRationalByInteger",
    "PowerIntegerByInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerRationalByRationalReducePrimeFactors,
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
    "CalculatorFunctionsBinaryOps::powerRationalByRationalReducePrimeFactors",
    "PowerRationalByRational",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerRationalByRationalOutputAlgebraic,
    this->opRational(),
    this->opRational(),
    "If the rational power is small enough, "
    "converts to an algebraic number. "
    "At the moment works with rational powers whose denominator is 2, "
    "may be extended to larger powers in the future. ",
    "54^{1/2}",
    "CalculatorFunctionsBinaryOps::"
    "powerRationalByRationalOutputAlgebraic",
    "PowerRationalByRationalOutputAlgebraic",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerDoubleOrRationalToDoubleOrRational,
    this->opRational(),
    this->opDouble(),
    "Calls the built-in cpp functions "
    "to approximately raise a double to a power, "
    "provided either the base or the exponent "
    "is a double, and provided that "
    "the base is non-negative. ",
    "f{}{{x}}=x^3+p x +q;\n"
    "Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) "
    "+( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);\n"
    "q = DoubleValue{}1;\n"
    "p = DoubleValue{}1;\n"
    "Xcardano;\n"
    "f{}x;\n"
    "f{}Xcardano ",
    "CalculatorFunctionsBinaryOps::"
    "powerDoubleOrRationalToDoubleOrRational",
    "PowerDoubleToDouble",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::powerSequenceOrMatrixByT,
    "",
    "Provided the exponent is t or T, calls the Transpose "
    "function on the base.",
    "X = (1,2)^t;\n"
    "X-Transpose{}(1,2);\n"
    "\\begin{pmatrix}1&2\\\\ 3& 5\\end{pmatrix}^t  ",
    "CalculatorFunctionsBinaryOps::powerSequenceOrMatrixByT",
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
    "As of writing, the supported built-in "
    "coefficient types are: (1) rational numbers;"
    "(2) algebraic numbers; "
    "(3) polynomial and rational functions in one variable.",
    "X = \\begin{pmatrix} 0 & 1 \\\\ 1 &1\\end{pmatrix};\n"
    "X^5;\n"
    "X^{-5};\n"
    "f{}({{a}}, {{b}})=a Polynomial( x )+b;\n"
    "a=FunctionToMatrix(f,2,2);\n"
    "b=a^{-1};\n"
    "a*b",
    "CalculatorFunctionsBinaryOps::powerMatBySmallInteger",
    "PowerMatrixBuiltInByInteger",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::powerMatrixNumbersByLargeIntegerIfPossible,
    "",
    "Attempts to exponentiate matrix by a large power. ",
    "X =\\begin{pmatrix} 1 & -1 \\\\ 4 & -3\\end{pmatrix}; X^{-55!- 1}",
    "CalculatorFunctionsBinaryOps::"
    "powerMatrixNumbersByLargeIntegerIfPossible",
    "PowerMatrixNumbersByLargeIntegerIfPossible",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::powerMatrixExpressionsBySmallInteger,
    "",
    "Attempts to exponentiate a matrix of expressions, "
    "if the exponent is small. ",
    "X =\\begin{pmatrix} a & b \\\\ t +q &r\\end{pmatrix}; X^{2}  ",
    "CalculatorFunctionsBinaryOps::powerMatrixExpressionsBySmallInteger",
    "PowerMatrixNumbersBySmallIntegerIfPossible",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::powerElementZmodPByInteger,
    "",
    "Raises an element of Z mod p to an integer power. ",
    "a = 3 mod 7; a^2",
    "CalculatorFunctionsBinaryOps::powerElementZmodPByInteger",
    "PowerElementZmodPToInteger",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::powerExponentToLog,
    "",
    "Replaces e^{\\ln x} by x.",
    "e^{\\ln x}; ",
    "CalculatorFunctions::powerExponentToLog",
    "PowerToLog",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::powerAnyToZero,
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
    "CalculatorFunctions::powerAnyToZero",
    "PowerAnyToZero",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerPolynomialBySmallInteger,
    this->opPolynomialRational(),
    this->opRational(),
    "Raises polynomial to small integer power. ",
    "x = Polynomial{}x; y = Polynomial{}y;(x +2y+x y+x^2+3y^2)^3",
    "CalculatorFunctionsBinaryOps::powerPolynomialBySmallInteger",
    "PowerPolynomialBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerAlgebraicNumberPolynomialBySmallInteger,
    this->opPolynomialAlgebraicNumbers(),
    this->opRational(),
    "Raises a polynomial over the algebraic numbers to small integer power. ",
    "x = Polynomial{}x;\n"
    "y = Polynomial{}y;\n"
    "(x +\\sqrt{2}y+x y+x^2+\\sqrt{3}y^2)^3",
    "CalculatorFunctionsBinaryOps::"
    "powerAlgebraicNumberPolynomialBySmallInteger",
    "PowerPolynomialAlgebraicNumbersBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerRationalFractionBySmallInteger,
    this->opRationalFraction(),
    this->opRational(),
    "Raises rational function to small integer power. ",
    "x = MakeRationalFunction{}x;\n"
    "y = MakeRationalFunction{}y;\n"
    "(x/y)^3;\n"
    "(x/y)^-3;\n"
    "(x/y)^3-(x/y)^-3",
    "CalculatorFunctionsBinaryOps::powerRationalFractionBySmallInteger",
    "PowerRationalFractionByInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerPolynomialModuloIntegerBySmallInteger,
    this->opPolynomialModuloInteger(),
    this->opRational(),
    "Raises polynomial modulo an integer to a small integer power. ",
    "a = PolynomialModP{}(x^2 + x + 1, 7);\n"
    "a^20",
    "CalculatorFunctionsBinaryOps::"
    "powerPolynomialModuloIntegerBySmallInteger",
    "PowerPolynomialModuloIntegerBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::
    powerPolynomialModPModuloPolynomialModPBySmallInteger,
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
    "s = PolynomialModP( r - 1, 7);\n"
    "t = PolynomialModP(r, 7);\n"
    "u = PolynomialModP(r + 1, 7);\n"
    "GCDPoly(s, modulo);\n"
    "GCDPoly(t, modulo);\n"
    "GCDPoly(u, modulo);",
    "CalculatorFunctionsBinaryOps::"
    "powerPolynomialModPModuloPolynomialModPBySmallInteger",
    "PowerPolynomialModPModuloPolynomialModPBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerAlgebraicNumberBySmallInteger,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Raises algebraic number to small integer or half-integer power. ",
    "a = 3/2;\n"
    "b = - 15/2;\n"
    "c = 33/4;\n"
    "x = (-b+\\sqrt{}(b^2-4a c))/(2a);\n"
    "B= c+a x^{2}+b x;",
    "CalculatorFunctionsBinaryOps::"
    "powerAlgebraicNumberBySmallInteger",
    "PowerAlgebraicNumberBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::radicalAlgebraicNumberPositiveDefault,
    this->opAlgebraicNumber(),
    this->opRational(),
    "Takes the radical of an algebraic number, "
    "if the algebraic number is a positive number"
    "that whose radical lies in the underlying algebraic closure.",
    "\\sqrt{3+2\\sqrt[2]{2}};\n"
    "(7-5\\sqrt{2})^{7/3}",
    "CalculatorFunctionsBinaryOps::radicalAlgebraicNumberPositiveDefault",
    "RadicalAlgebraicNumberPositiveDefault",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::powerDoubleOrRationalToDoubleOrRational,
    this->opDouble(),
    this->opRational(),
    "Calls the built-in cpp functions "
    "to approximately raise a double to a power, "
    "provided either the base or the exponent is a double. "
    "If the base is negative and "
    "the exponent is rational with odd denominator, "
    "the exponent is evaluated to the corresponding "
    "real negative root. ",
    "f{}{{x}}=x^3+p x +q;\n"
    "Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) "
    "+( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);\n"
    "q= DoubleValue{}1;\n"
    "p = DoubleValue{}1;\n"
    "Xcardano;\n"
    "f{}x;\n"
    "f{}Xcardano   ",
    "CalculatorFunctionsBinaryOps::"
    "powerDoubleOrRationalToDoubleOrRational",
    "PowerFloatingToPower",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorBasics::associateExponentExponent,
    "",
    "If the rule doesn't break over the complex numbers, "
    "substitutes (a^b)^c with a^{b*c}. "
    "The rule acts when one of the following holds:\n"
    "1) a is real and positive;\n"
    "2) a>0 or a\\geq 0 equals 1 "
    "(those can be user-defined, for example via (a>0)=1;)\n"
    "3) c is an integer;\n"
    "4) a evaluates to real (double), b and c "
    "are rational and b*c is an even integer. ",
    "(a^m)^n;\n"
    "((ln(3))^m)^n;\n"
    "((ln(0.5))^m)^n;\n"
    "(a^m)^2;\n"
    "(a^2)^m;\n"
    "(a^{1/2})^2;\n"
    "(a^{2})^{1/2};\n"
    "(a>0)=1;\n"
    "(a^{2})^{1/2};",
    "CalculatorBasics::associateExponentExponent",
    "PowerPowerToPower",
    innerStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::distributeExponent,
    "",
    "If a is a positive constant, substitutes (a*b)^c with a^c b^c.",
    "(a*b)^n; (\\sqrt(2)*b)^2",
    "CalculatorFunctions::distributeExponent",
    "DistributeExponent",
    innerStandard
  );
  this->addOperationHandler(
    "\\sqrt",
    CalculatorFunctions::distributeSqrt,
    "",
    "If a is a positive constant, substitutes sqrt(a b) by sqrt(a) sqrt(b).",
    "TurnOffApproximations 0;\n"
    "\\sqrt{\\frac{676}{25} ln 3}",
    "CalculatorFunctions::distributeSqrt",
    "DistributeSqrt",
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
    CalculatorFunctions::operatorBounds,
    "",
    "Replaces \\sum_a^b by (\\sum, a, b) .",
    "A =\\sum_a^b;\n"
    "Lispify(A);\n"
    "PlotExpressionTree(A);",
    "CalculatorFunctions::operatorBounds",
    "OperatorBoundsSuperscript",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::operatorBounds,
    "",
    "Takes care of the integral superscript notation \\int^a ",
    "\\int^a_b f dx; \\int_a^b f dx",
    "CalculatorFunctions::operatorBounds",
    "OperatorBoundsUnderscore",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::dereferenceSequenceOrMatrix,
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
    "CalculatorFunctions::dereferenceSequenceOrMatrix",
    "DereferenceSequenceOrMatrix",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::dereferenceInterval,
    "",
    "Dereferences an interval, as illustrated by the example. ",
    "%UseBracketForIntervals\n"
    "[a, b)_1;\n"
    "(a, b]_2;\n"
    "[a, b]_1; ",
    "CalculatorFunctions::dereferenceInterval",
    "DereferenceInterval",
    innerStandard
  );
  this->addOperationHandler(
    "_",
    CalculatorFunctions::dereferenceSequenceStatements,
    "",
    "Dereferences a sequence of rules. "
    "The syntax is as illustrated by the example. ",
    "A = d/dx( \\sqrt(x + y) - 4x^2y^2);\n"
    "(d/dx(y)= 0; A)_2;  ",
    "CalculatorFunctions::dereferenceSequenceStatements",
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
    "Same as tensor product of matrices but uses "
    "class MatrixTensor instead of class Matrix.",
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
    "Please do note use (or use at your own risk): "
    "this is work-in-progress. "
    "Will be documented when implemented and tested. "
    "Tensor product of "
    "generalized Verma modules. ",
    "X = G_2;\n"
    "g_{{i}}= ChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= CartanGenerator{}(X,i);\n"
    "v=HeighestWeightVector{}(X, (1,0),(0,0));\n"
    "g_{- 1}(v\\otimes v);\n"
    "g_{- 1}g_{- 1}(v\\otimes v); ",
    "CalculatorBasics::outerTensor",
    "TensorProductStandard",
    innerNoTestInvisibleExperimental
  );
  this->addOperationHandler(
    "=:",
    CalculatorBasics::standardIsDenotedBy,
    "",
    "The operation =: is the \"is denoted by\" operation. "
    "The expression a =:b always reduces to a =b. "
    "In addition to the transformation, "
    "the pair of expressions a, b is registered "
    "in a special global \"registry\". "
    "This has the following effect. Every time "
    "the expression b is met, it is displayed on the screen as a. "
    "We note that subsequent occurrences of the expression a "
    "will first be replaced by b (as mandated by the a =b command), "
    "but then displayed on the screen as a.",
    "x =:y;\n"
    "y;\n"
    "z;\n"
    "z=y;\n"
    "z",
    "CalculatorBasics::standardIsDenotedBy",
    "standardIsDenotedBy",
    innerStandard
  );
  this->addOperationHandler(
    "<",
    CalculatorFunctions::lessThan,
    "",
    "If both the left hand side and the right hand side are rational, "
    "replaces the expression by 1 if the left number "
    "is less than the right, else replaces the expression by 0.",
    "3 < 4; 5 < 4",
    "CalculatorFunctions::lessThan",
    "LessThan",
    innerStandard
  );
  this->addOperationHandler(
    ">",
    CalculatorFunctions::greaterThan,
    "",
    "Greater than: has similar action to the less than sign. "
    "The following example shows "
    "an implementation of commutativity. ",
    "x_{{i}} * x_{{j}} * {{a}} :if(i > j) = x_j * x_i * a;\n"
    "x_{{i}}*x_{{j}}: if (i > j) = x_j * x_i;\n"
    "(x_2 * x_1 - x_1 * x_3)(x_1 x_5 + x_5 x_4 x_2);\n"
    "x_5x_4x_3x_2x_1",
    "CalculatorFunctions::greaterThan",
    "GreaterThan",
    innerStandard
  );
  this->addOperationHandler(
    "Max",
    CalculatorFunctions::maximum,
    "",
    "Maximum function.",
    "Max(- 4,2)",
    "CalculatorFunctions::maximum",
    "Max",
    innerStandard
  );
  this->addOperationHandler(
    "Min",
    CalculatorFunctions::minimum,
    "",
    "Maximum function.",
    "Min(- 4,2)",
    "CalculatorFunctions::minimum",
    "Min",
    innerStandard
  );
  this->addOperationHandler(
    "\\geq",
    CalculatorFunctions::greaterThanOrEqualTo,
    "",
    "Greater than or equal to operation. ",
    "A = (2>=x);\n"
    "x =1;\n"
    "A",
    "CalculatorFunctions::greaterThanOrEqualTo",
    "greaterThanOrEqualTo",
    innerStandard
  );
  this->addOperationHandler(
    "\\leq",
    CalculatorFunctions::lessThanOrEqualTo,
    "",
    "Transforms \\(a\\leq b\\) to \\(b\\geq a\\).",
    "a \\leq b",
    "CalculatorFunctions::lessThanOrEqualTo",
    "lessThanOrEqualTo",
    innerStandard
  );
  this->addOperationHandler(
    "==",
    CalculatorFunctions::outerEqualEqual,
    "",
    "If either the left or the right argument "
    "contains a bound variable does nothing. "
    "Else evaluates to 1 if the left argument "
    "equals mathematically the right argument.",
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
    CalculatorFunctions::equalEqualEqual,
    "",
    "Returns 1 if both sides have identical expression trees, "
    "0 otherwise. ",
    "2+3 == 5;\n"
    "Freeze{}(2 + 3) == Freeze{}5;\n"
    "Freeze{}(2 + 3) === Freeze{}5",
    "CalculatorFunctions::equalEqualEqual",
    "IsEqualExpressionTree",
    innerStandard
  );
  this->addOperationHandler(
    "Contains",
    CalculatorFunctions::contains,
    "",
    "Returns 1 if the first argument contains the second "
    "as a sub-expression, else returns 0. Function "
    "has not been optimized for speed, use with caution. ",
    "Contains ((\\arcsin x +\\cos x,5), \\arcsin )",
    "CalculatorFunctions::contains",
    "Contains",
    innerStandard
  );
  this->addOperationHandler(
    "ExpressionLeafs",
    CalculatorFunctions::expressionLeafs,
    "",
    "Returns a sequence without repetition of all leafs "
    "making up an expression. "
    "Here, a ``leaf'' means either an atomic expression "
    "or a built-in type. "
    "Built-in types are not broken into atoms. ",
    "ExpressionLeafs ((\\arcsin x +\\cos x,5), \\arcsin )",
    "CalculatorFunctions::expressionLeafs",
    "ExpressionLeafs",
    innerStandard
  );
  this->addOperationHandler(
    "Last",
    CalculatorFunctionsListsAndSets::lastElement,
    "",
    "Returns the last element of the expression, "
    "provided the argument has "
    "no bound variables. If the expression has bound "
    "variables does nothing.",
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
    "CalculatorFunctionsListsAndSets::lastElement",
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
    "CalculatorFunctionsListsAndSets::intersection",
    "\\cap",
    innerStandard
  );
  this->addOperationHandler(
    "\\cup",
    CalculatorFunctionsListsAndSets::listUnion,
    "",
    "If all arguments of \\cup are of type list, "
    "substitutes the expression with "
    "a list containing the union of all members (with repetition). "
    "If a flag is set requesting that (a,b) is "
    "interpreted as an interval, does nothing if "
    "either of the two sequences has two elements.",
    "x\\cup (MakeSequence{} x \\cup MakeSequence{}x \\cup (a,b,x))",
    "CalculatorFunctionsListsAndSets::union",
    "\\cup",
    innerStandard
  );
  this->addOperationHandler(
    "\\sqcup",
    CalculatorFunctionsListsAndSets::unionNoRepetition,
    "",
    "If all arguments of \\sqcup are of type list, "
    "substitutes the expression "
    "with a list containing the union of all members; "
    "all repeating members are discarded.",
    "(x,y,x)\\sqcup(1,x,y,2)",
    "CalculatorFunctionsListsAndSets::unionNoRepetition",
    "\\sqcup",
    innerStandard
  );
  this->addOperationHandler(
    "\\sum",
    CalculatorFunctions::sumAsOperatorToSumInternalNotation,
    "",
    "Transforms (\\sum_a^b ){} n to \\sum_a^b n (internal notation). ",
    "PlotExpressionTree(Freeze(\\sum_a^b ){} n);\n"
    "PlotExpressionTree((\\sum_a^b ){} n) ",
    "CalculatorFunctions::sumAsOperatorToSumInternalNotation",
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
    CalculatorFunctions::constantFunction,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;\n"
    "2{}y;\n"
    "(\\sqrt{}2){}x;",
    "CalculatorFunctions::constantFunction",
    "ConstantFunction",
    compositeStandard
  );
  this->addOperationHandler(
    "RationalFunction",
    CalculatorFunctions::rationalFunctionSubstitution,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;\n"
    "2{}y;\n"
    "(\\sqrt{}2){}x;",
    "CalculatorFunctions::rationalFunctionSubstitution",
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
    "CalculatorFunctions::"
    "compositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentMultiplication",
    compositeStandard
  );
  this->addOperationHandler(
    "/",
    CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a/b){}x = (a{}x)/(b{}x) ",
    "(a/b){}x;",
    "CalculatorFunctions::"
    "compositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentDivision",
    compositeStandard
  );
  this->addOperationHandler(
    "AlgebraicNumber",
    CalculatorFunctions::constantFunction,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;\n"
    "2{}y;\n"
    "(\\sqrt{}2){}x;",
    "CalculatorFunctions::constantFunction",
    "ConstantFunctionAlgebraicNumber",
    compositeStandard
  );
  this->addOperationHandler(
    "*",
    CalculatorFunctions::compositeConstTimesAnyActOn,
    "",
    "Rule (a*f){}x = a*(f{}x), provided a is a constant. ",
    "(2\\sin){}x-2(\\sin x) ",
    "CalculatorFunctions::compositeConstTimesAnyActOn",
    "ConstTimesAnyAction",
    compositeStandard
  );
  this->addOperationHandler(
    "^",
    CalculatorFunctions::compositeAPowerBEvaluatedAtC,
    "",
    "Provided that n is not equal to - 1 and "
    "f is not a sequence, use the rule ({{f}}^{{n}}){}{{x}}= (f{}x)^n.",
    "\\tan^2 x;\n"
    "(f^-2) {}x ; (f^- 1){}x ",
    "CalculatorFunctions::compositeAPowerBEvaluatedAtC",
    "ApowerBevaluatedAtC",
    compositeStandard
  );
  this->addOperationHandler(
    "Differentiate",
    CalculatorFunctions::compositeDifferentiateLog,
    "",
    "Differentiates log.",
    "d/dx (\\log x)",
    "CalculatorFunctions::compositeDifferentiateLog",
    "DifferentiateLog",
    compositeStandard
  );
  this->addOperationHandler(
    "SolveJSON",
    CalculatorEducationalFunctions::solveJSON,
    "",
    "Tries to interpret the input as a high-school or Calculus problem "
    "and solve it. "
    "Returns its result in JSON format. "
    "Freezes its inputs.",
    "SolveJSON(x^2+2x-3=0);\n",
    "CalculatorEducationalFunctions::solveJSON",
    "SolveJSON",
    innerFreezesArguments
  );
}

void Calculator::initializePredefinedStandardOperationsWithoutHandler() {
  STACK_TRACE(
    "Calculator::initializePredefinedStandardOperationsWithoutHandler"
  );
  // additional operations treated like function
  // names but otherwise not parsed
  // as syntactic elements.
  this->addOperationNoRepetitionAllowed("RulesOff");
  this->addOperationNoRepetitionAllowed("RulesOn");
  this->addOperationNoRepetitionAllowed("Freeze");
  this->addOperationNoRepetitionAllowed("\\infty");
  this->addOperationNoRepetitionAllowed("\\phantom");
  this->addOperationNoRepetitionAllowed(
    Calculator::Functions::Names::commandEnclosure
  );
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
  // additional operations with the same status as user-input expressions.
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
  STACK_TRACE("Calculator::initializeAtomsNonCacheable");
  this->atomsThatMustNotBeCached.setExpectedSize(30);
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew(
    "RandomInteger"
  );
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew(
    "SelectAtRandom"
  );
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew(
    "GenerateRandomPrime"
  );
  this->atomsThatMustNotBeCached.addOnTopNoRepetitionMustBeNew(
    "PrintAlgebraicClosureStatus"
  );
}

void Calculator::initializeAtomsNotGoodForChainRule() {
  STACK_TRACE("Calculator::initializeAtomsNotGoodForChainRule");
  this->atomsNotAllowingChainRule.addOnTopNoRepetitionMustBeNew("Bind");
}

void Calculator::initializeAtomsThatAllowCommutingOfArguments() {
  STACK_TRACE("Calculator::initializeAtomsThatAllowCommutingOfArguments");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.setExpectedSize(
    30
  );
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("+");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("*");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("/");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("^");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("|");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("\\sin");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("\\cos");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.
  addOnTopNoRepetitionMustBeNew("\\log");
}

void Calculator::initializeArithmeticOperations() {
  STACK_TRACE("Calculator::initializeArithmeticOperations");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("+");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("-");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("*");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("/");
  this->arithmeticOperations.addOnTopNoRepetitionMustBeNew("^");
}

void Calculator::initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions() {
  STACK_TRACE(
    "Calculator::initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions"
  );
  // Related heavily to
  // initializeOperationsInterpretedAsFunctionsMultiplicatively
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\sin");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\cos");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\tan");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\cot");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\sec");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\csc");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.
  addOnTopNoRepetitionMustBeNew("\\log");
}

void Calculator::initializeOperationsInterpretedAsFunctionsMultiplicatively() {
  STACK_TRACE(
    "Calculator::initializeOperationsInterpretedAsFunctionsMultiplicatively"
  );
  // Related heavily to
  // initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\sin");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\cos");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\tan");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\cot");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\sec");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\csc");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\arctan");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\arcsin");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\arccos");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.
  addOnTopNoRepetitionMustBeNew("\\log");
}

void Calculator::initializeOperationsThatAreKnownFunctions() {
  STACK_TRACE("Calculator::initializeOperationsThatAreKnownFunctions");
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
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew(
    "\\arctan"
  );
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew(
    "\\arcsin"
  );
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew(
    "\\arccos"
  );
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew("\\log");
  this->knownFunctionsWithComplexRange.addOnTopNoRepetitionMustBeNew(
    "LogBase"
  );
}

void Calculator::addKnownDoubleConstant(
  const std::string& constantName, double value
) {
  this->atomsNotInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew(
    constantName
  );
  Expression constant;
  constant.makeAtom(*this, constantName);
  this->knownDoubleConstants.addOnTopNoRepetitionMustBeNew(constant);
  this->knownDoubleConstantValues.addOnTop(value);
}

void Calculator::initializeBuiltInAtomsNotInterpretedAsFunctions() {
  STACK_TRACE("Calculator::initializeBuiltInAtomsNotInterpretedAsFunctions");
  this->atomsNotInterpretedAsFunctions.setExpectedSize(30);
  this->addKnownDoubleConstant("\\pi", MathRoutines::pi());
  this->addKnownDoubleConstant("e", MathRoutines::E());
  this->atomsNotInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("\\int");
  this->atomsNotInterpretedAsFunctions.addOnTopNoRepetitionMustBeNew("i");
}

void CalculatorParser::addTrigonometricSplit(
  const std::string& trigonometricFunction,
  const List<std::string>& variables
) {
  STACK_TRACE("Calculator::addTrigonometricSplit");
  List<std::string> split;
  for (int i = 0; i < variables.size; i ++) {
    const std::string& variable = variables[i];
    split.setSize(0);
    split.addOnTop("\\" + trigonometricFunction);
    split.addOnTop(variable);
    this->predefinedWordSplits.setKeyValue(
      trigonometricFunction + variable, split
    );
    this->predefinedWordSplits.setKeyValue(
      "\\" + trigonometricFunction + variable, split
    );
    split.setSize(0);
    split.addOnTop(variable);
    split.addOnTop("\\" + trigonometricFunction);
    this->predefinedWordSplits.setKeyValue(
      variable + trigonometricFunction, split
    );
    this->predefinedWordSplits.setKeyValue(
      variable + "\\" + trigonometricFunction, split
    );
  }
  for (int i = 0; i < variables.size; i ++) {
    for (int j = 0; j < variables.size; j ++) {
      split.setSize(0);
      split.addOnTop(variables[i]);
      split.addOnTop("\\" + trigonometricFunction);
      split.addOnTop(variables[j]);
      this->predefinedWordSplits.setKeyValue(
        variables[i] + trigonometricFunction + variables[j], split
      );
    }
  }
}

void Calculator::addOneStringAtomHandler(
  int atom, Expression::ToStringHandler handler
) {
  this->addOneStringHandler(atom, handler, this->toStringHandlersAtoms);
}

void Calculator::addOneStringCompositeHandler(
  int atom, Expression::ToStringHandler handler
) {
  this->addOneStringHandler(atom, handler, this->toStringHandlersComposite);
}

void Calculator::addOneStringHandler(
  int atom,
  Expression::ToStringHandler handler,
  MapList<int, Expression::ToStringHandler, HashFunctions::hashFunction>&
  handlerCollection
) {
  if (handlerCollection.contains(atom)) {
    if (handlerCollection.getValueNoFail(atom) != handler) {
      global.fatal
      << "More than one toStringHandler for atom "
      << this->operations.keys[atom]
      << "."
      << global.fatal;
    }
  } else {
    handlerCollection.setKeyValue(atom, handler);
  }
}

void Calculator::initializeToStringHandlers() {
  STACK_TRACE("Calculator::initializeToStringHandlers");
  this->addOneStringAtomHandler(
    this->opDefine(), Expression::toStringDefine
  );
  this->addOneStringAtomHandler(
    this->opIsDenotedBy(), Expression::toStringIsDenotedBy
  );
  this->addOneStringAtomHandler(
    this->opLog(), Expression::toStringLnAbsoluteInsteadOfLogarithm
  );
  this->addOneStringAtomHandler(
    this->opLogBase(), Expression::toStringLogBase
  );
  this->addOneStringAtomHandler(
    this->opIntervalOpen(), Expression::toStringIntervalOpen
  );
  this->addOneStringAtomHandler(
    this->opIntervalRightClosed(),
    Expression::toStringIntervalRightClosed
  );
  this->addOneStringAtomHandler(
    this->opIntervalLeftClosed(), Expression::toStringIntervalLeftClosed
  );
  this->addOneStringAtomHandler(
    this->opIntervalClosed(), Expression::toStringIntervalClosed
  );
  this->addOneStringAtomHandler(
    this->opQuote(), Expression::toStringQuote
  );
  this->addOneStringAtomHandler(
    this->opDefineConditional(), Expression::toStringDefineConditional
  );
  this->addOneStringAtomHandler(
    this->opDivide(), Expression::toStringDivide
  );
  this->addOneStringAtomHandler(
    this->opTensor(), Expression::toStringTensor
  );
  this->addOneStringAtomHandler(this->opIn(), Expression::toStringIn);
  this->addOneStringAtomHandler(this->opOr(), Expression::toStringOr);
  this->addOneStringAtomHandler(this->opAnd(), Expression::toStringAnd);
  this->addOneStringAtomHandler(
    this->opBinom(), Expression::toStringBinom
  );
  this->addOneStringAtomHandler(
    this->opUnderscore(), Expression::toStringUnderscore
  );
  this->addOneStringAtomHandler(
    this->opSetMinus(), Expression::toStringSetMinus
  );
  this->addOneStringAtomHandler(
    this->opLimitBoundary(), Expression::toStringLimitBoundary
  );
  this->addOneStringAtomHandler(
    this->opTimes(), Expression::toStringTimes
  );
  this->addOneStringAtomHandler(
    this->opCrossProduct(), Expression::toStringCrossProduct
  );
  this->addOneStringAtomHandler(
    this->opSqrt(), Expression::toStringSqrt
  );
  this->addOneStringAtomHandler(
    this->opFactorial(), Expression::toStringFactorial
  );
  this->addOneStringAtomHandler(
    this->opAbsoluteValue(), Expression::toStringAbsoluteValue
  );
  this->addOneStringAtomHandler(
    this->opPower(), Expression::toStringPower
  );
  this->addOneStringAtomHandler(
    this->opPlus(), Expression::toStringPlus
  );
  this->addOneStringAtomHandler(
    this->opDirectSum(), Expression::toStringDirectSum
  );
  this->addOneStringAtomHandler(
    this->opMinus(), Expression::toStringMinus
  );
  this->addOneStringAtomHandler(
    this->opBind(), Expression::toStringBind
  );
  this->addOneStringAtomHandler(
    this->opEqualEqual(), Expression::toStringEqualEqual
  );
  this->addOneStringAtomHandler(
    this->opEqualEqualEqual(), Expression::toStringEqualEqualEqual
  );
  this->addOneStringAtomHandler(
    this->opDifferentiate(), Expression::toStringDifferentiate
  );
  this->addOneStringAtomHandler(
    this->opDifferential(), Expression::toStringDifferential
  );
  this->addOneStringAtomHandler(
    this->opSum(), Expression::toStringSumOrIntegral
  );
  this->addOneStringAtomHandler(
    this->opIntegral(), Expression::toStringSumOrIntegral
  );
  this->addOneStringAtomHandler(
    this->opGreaterThan(), Expression::toStringGreaterThan
  );
  this->addOneStringAtomHandler(
    this->opGreaterThanOrEqualTo(),
    Expression::toStringGreaterThanOrEqualTo
  );
  this->addOneStringAtomHandler(
    this->opLessThanOrEqualTo(), Expression::toStringLessThanOrEqualTo
  );
  this->addOneStringAtomHandler(
    this->opLimit(), Expression::toStringLimit
  );
  this->addOneStringAtomHandler(
    this->opLimitProcess(), Expression::toStringLimitProcess
  );
  this->addOneStringAtomHandler(
    this->opLessThan(), Expression::toStringLessThan
  );
  this->addOneStringAtomHandler(
    this->opSequence(), Expression::toStringSequence
  );
  this->addOneStringAtomHandler(
    this->opLieBracket(), Expression::toStringLieBracket
  );
  this->addOneStringAtomHandler(this->opMod(), Expression::toStringMod);
  this->addOneStringAtomHandler(
    this->opUnion(), Expression::toStringUnion
  );
  this->addOneStringAtomHandler(
    this->opIntersection(), Expression::toStringIntersection
  );
  this->addOneStringAtomHandler(
    this->opUnionNoRepetition(), Expression::toStringUnionNoRepetition
  );
  this->addOneStringAtomHandler(
    this->opError(), Expression::toStringError
  );
  this->addOneStringCompositeHandler(
    this->opMatrix(), Expression::toStringMatrix
  );
  // To add a new type here, you need to define
  // Expression::toStringBuiltIn<your_new_type>
  this->addOneBuiltInHandler<std::string>();
  this->addOneBuiltInHandler<JSData>();
  this->addOneBuiltInHandler<Rational>();
  this->addOneBuiltInHandler<ElementEllipticCurve<Rational> >();
  this->addOneBuiltInHandler<ElementEllipticCurve<ElementZmodP> >();
  this->addOneBuiltInHandler<ElementZmodP>();
  this->addOneBuiltInHandler<InputBox>();
  this->addOneBuiltInHandler<
    GroupRepresentation<
      FiniteGroup<ElementHyperoctahedralGroupR2>, Rational
    >
  >();
  this->addOneBuiltInHandler<ElementHyperoctahedralGroupR2>();
  this->addOneBuiltInHandler<Polynomial<Rational> >();
  this->addOneBuiltInHandler<Polynomial<ElementZmodP> >();
  this->addOneBuiltInHandler<Polynomial<AlgebraicNumber> >();
  this->addOneBuiltInHandler<PolynomialModuloPolynomial<ElementZmodP> >();
  this->addOneBuiltInHandler<AlgebraicNumber>();
  this->addOneBuiltInHandler<RationalFraction<Rational> >();
  this->addOneBuiltInHandler<RationalFraction<AlgebraicNumber> >();
  this->addOneBuiltInHandler<RationalFraction<ElementZmodP> >();
  this->addOneBuiltInHandler<Weight<Polynomial<Rational> > >();
  this->addOneBuiltInHandler<SemisimpleLieAlgebra*>();
  this->addOneBuiltInHandler<
    ElementUniversalEnveloping<RationalFraction<Rational> >
  >();
  this->addOneBuiltInHandler<MatrixTensor<Rational> >();
  this->addOneBuiltInHandler<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >();
  this->addOneBuiltInHandler<Plot>();
  this->addOneBuiltInHandler<WeylGroupData>();
  this->addOneBuiltInHandler<ElementWeylGroup>();
  this->addOneBuiltInHandler<
    GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  this->addOneBuiltInHandler<
    VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  this->addOneBuiltInHandler<CharacterSemisimpleLieAlgebraModule<Rational> >();
  this->addOneBuiltInHandler<SemisimpleSubalgebras>();
  this->addOneBuiltInHandler<double>();
  this->addOneBuiltInHandler<AlgebraicNumber>();
  this->addOneBuiltInHandler<LittelmannPath>();
  this->addOneBuiltInHandler<ElementWeylAlgebra<Rational> >();
  this->addOneBuiltInHandler<PartialFractions>();
}

void Calculator::initializeBuiltInsFreezeArguments() {
  STACK_TRACE("Calculator::initializeBuiltInsFreezeArguments");
  this->atomsThatFreezeArguments.setExpectedSize(
    this->builtInTypes.all.size + 100
  );
  this->atomsThatFreezeArguments.addListOnTop(this->builtInTypes.all);
}

void Calculator::initializeAtomsThatFreezeArguments() {
  STACK_TRACE("Calculator::initializeAtomsThatFreezeArguments");
  this->atomsThatFreezeArguments.addOnTopNoRepetitionMustBeNew("Freeze");
  this->atomsThatFreezeArguments.addOnTopNoRepetitionMustBeNew("Bind");
}
