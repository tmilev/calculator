// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "math_general_implementation.h"
#include "calculator_Weyl_group_characters.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_inner_functions.h"
#include "calculator_html_functions.h"
#include "math_general_polynomial_computations_basic_implementation.h" //undefined reference to Polynomial<AlgebraicNumber>::MakeOne(int)
#include "database.h"
#include "calculator_database_mongo.h"

// This file lists calculator functions and various hard-coded rules.

void Calculator::initAdminFunctions() {
  Function::Options adminDefault, adminDisabled;
  adminDefault.dontTestAutomatically = true;
  adminDefault.adminOnly = true;
  adminDefault.flagIsInner = true;
  adminDisabled = adminDefault;
  adminDisabled.disabledByUserDefault = true;
  adminDisabled.disabledByUser = true;

  this->AddOperationHandler(
    "StopServer",
    CalculatorFunctions::innerStopServer,
    "",
    "Stops the server/kills the calculator for all users. "
    "Requires administator rights. "
    "Disabled by default. "
    "Use to stop server remotely/test server service restart. ",
    "StopServer 0",
    "CalculatorDatabaseFunctions::innerStopServer",
    "StopServer",
    adminDisabled
  );

  if (!global.flagDatabaseCompiled) {
    return;
  }
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "RepairDatabaseEmailRecords",
    CalculatorDatabaseFunctions::innerRepairDatabaseEmailRecords,
    "",
    "Repairs username/email mismatches. Administrator use only. ",
    "TurnOnRules(RepairDatabaseEmailRecords); RepairDatabaseEmailRecords(0)",
    "DatabaseRoutines::innerRepairDatabaseEmailRecords",
    "RepairDatabaseEmailRecords",
    adminDefault
  );
}

void Calculator::initCalculusTestingFunctions() {
  this->initAdminFunctions();
}

///////////////
/// \brief Calculator::initPredefinedInnerFunctions
/// Naming conventions: please start all built-in calculator functions with capital letter.
/// Exceptions are made for
/// 1) functions that have mathematical names
/// starting with small letters (example: \\sin, logical operations, etc.)
/// Those typically start with a backslash.
/// 2) functions that have already been assigned keywords starting with non-capital letter
/// and have been used too often.
///
void Calculator::initPredefinedInnerFunctions() {
  Function::Options innerStandard;
  innerStandard.flagIsInner = true;

  Function::Options innerInvisible;
  innerInvisible.flagIsInner = true;
  innerInvisible.visible = false;

  Function::Options innerStandardOffByDefault;
  innerStandardOffByDefault.flagIsInner = true;
  innerStandardOffByDefault.disabledByUser = true;
  innerStandardOffByDefault.disabledByUserDefault = true;

  Function::Options innerInvisibleNoTest;
  innerInvisibleNoTest.flagIsInner = true;
  innerInvisibleNoTest.visible = false;
  innerInvisibleNoTest.dontTestAutomatically = true;

  Function::Options innerNoTest;
  innerNoTest.flagIsInner = true;
  innerNoTest.dontTestAutomatically = true;


  Function::Options innerExperimental;
  innerExperimental.flagIsInner = true;
  innerExperimental.flagIsExperimental = true;

  Function::Options innerNoTestExperimental;
  innerNoTestExperimental = innerNoTest;
  innerNoTestExperimental.flagIsExperimental = true;

  Function::Options innerInvisibleExperimental;
  innerInvisibleExperimental.flagIsInner = true;
  innerInvisibleExperimental.flagIsExperimental = true;
  innerInvisibleExperimental.visible = false;

  Function::Options innerAdminNoTest;
  innerAdminNoTest.flagIsInner = true;
  innerAdminNoTest.dontTestAutomatically = true;
  innerAdminNoTest.adminOnly = true;

  Function::Options innerAdminNoTestExperimental;
  innerAdminNoTestExperimental = innerAdminNoTest;
  innerAdminNoTestExperimental.flagIsExperimental = true;

  Function::Options innerAdminNoTestInvisibleOffByDefault;
  innerAdminNoTestInvisibleOffByDefault.flagIsInner = true;
  innerAdminNoTestInvisibleOffByDefault.dontTestAutomatically = true;
  innerAdminNoTestInvisibleOffByDefault.adminOnly = true;
  innerAdminNoTestInvisibleOffByDefault.visible = false;
  innerAdminNoTestInvisibleOffByDefault.disabledByUser = true;
  innerAdminNoTestInvisibleOffByDefault.disabledByUserDefault = true;


  this->AddOperationHandler(
    "SetRandomSeed",
    CalculatorFunctions::innerSetRandomSeed,
    "",
    "Sets the random seed of the calculator to the given integer value",
    "SetRandomSeed(123); RandomInteger(- 100,100); RandomInteger(- 100,100)",
    "CalculatorConversions::innerSetRandomSeed",
    "SetRandomSeed",
    innerStandard
  );
  this->AddOperationHandler(
    "SelectAtRandom",
    CalculatorFunctions::innerSelectAtRandom,
    "",
    "Selects at random an object from a sequence.",
    "f=SelectAtRandom(\\sin, \\cos); "
    "g=SelectAtRandom(\\tan, \\cot, \\sec, \\csc);"
    "f{}g{}x",
    "CalculatorConversions::SelectAtRandom",
    "SelectAtRandom",
    innerNoTest
  );
  this->AddOperationHandler(
    "RandomInteger",
    CalculatorFunctions::innerRandomInteger,
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
    "CalculatorConversions::innerRandomInteger",
    "RandomInteger",
    innerNoTest
  );
  this->AddOperationHandler(
    "TurnOffRules",
    CalculatorFunctions::innerTurnOffRules,
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
    "CalculatorConversions::innerTurnOffRules",
    "TurnOffRules",
    innerStandard
  );
  this->AddOperationHandler(
    "TurnOnRules",
    CalculatorFunctions::innerTurnOnRules,
    "",
    "Turns on computational rules.",
    "TurnOffRules(\"sqrt\"); a = \\sqrt[4]{t}; TurnOnRules(\"sqrt\"); a",
    "CalculatorConversions::innerTurnOnRules",
    "TurnOnRules",
    innerStandard
  );
  this->AddOperationHandler(
    "TurnOnApproximations",
    CalculatorFunctions::innerTurnOnApproximations,
    "",
    "Turns on numerical approximations. Takes as input dummy argument. ",
    "TurnOffApproximations(0); ln(2); TurnOnApproximations(0); ln(2)",
    "CalculatorConversions::innerTurnOnRules",
    "TurnOnApproximations",
    innerStandard
  );
  this->AddOperationHandler(
    "TurnOffApproximations",
    CalculatorFunctions::innerTurnOffApproximations,
    "",
    "Turns on numerical approximations. Takes as input dummy argument. ",
    "TurnOffApproximations(0); ln(2); TurnOnApproximations(0); ln(2)",
    "CalculatorConversions::innerTurnOnRules",
    "TurnOffApproximations",
    innerStandard
  );
  this->AddOperationHandler(
    "EvaluateSymbols",
    CalculatorHtmlFunctions::innerEvaluateSymbols,
    "",
    "Evaluates and replaces individual symbols/variables in LaTeX string. Leaves the rest of the string intact.",
    "x = 5;\n"
    "left = a;\n"
    "EvaluateSymbols(\"x^x +ax +a x +\\left(left \\right)\")",
    "CalculatorConversions::innerEvaluateSymbols",
    "EvaluateSymbols",
    innerStandard
  );
  this->AddOperationHandler(
    "or",
    CalculatorFunctions::innerOr,
    "",
    "Logical or.",
    "0 or 0; 0 or 1; 1 or 0; 1 or 1; a or 1; a or 0;",
    "CalculatorConversions::innerOr",
    "or",
    innerStandard
  );
  this->AddOperationHandler(
    "or",
    CalculatorFunctions::innerOrIdentical,
    "",
    "If the two arguments of or are identical, replaces the expression with the argument. "
    "Works even if the individual expression cannot be evaluated to 1 or 0. "
    "Please note that mathematically equal objects may fail to be identical, for example "
    "a rational number 5 and an algebraic number 5. ",
    "a or b; a or a",
    "CalculatorConversions::innerOrIdentical",
    "orIdentical",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
    innerStandard
  );
  this->AddOperationHandler(
    "and",
    CalculatorFunctions::innerAnd,
    "",
    "Logical and. ",
    "0 and 0; 0 and 1; 1 and 0; 1 and 1; a and 1; a and 0;",
    "CalculatorConversions::innerAnd",
    "and",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertElementZmodPToInteger",
    CalculatorFunctions::innerConvertElementZmodPToInteger,
    "",
    "Converts element of Z mod p to an integer between 0 and p - 1. ",
    "5^(ConvertElementZmodPToInteger( 9 mod 7))",
    "CalculatorFunctions::innerConvertElementZmodPToInteger",
    "ConvertElementZmodPToInteger",
    innerStandard
  );

  this->AddOperationHandler(
    "URLStringToNormalString",
    CalculatorFunctions::innerUrlStringToNormalString,
    "",
    "Converts an url-encoded string to a normal string. ",
    "URLStringToNormalString(\"randomSeed%3d92742048%26submissionsAlgebraAnswer%3\")",
    "CalculatorConversions::innerUrlStringToNormalString",
    "URLStringToNormalString",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertStringToURL",
    CalculatorFunctions::innerStringToURL,
    "",
    "Converts a normal string to a url-encoded one. ",
    "ConvertStringToURL(\"+ %\")",
    "CalculatorConversions::innerStringToURL",
    "ConvertStringToURL",
    innerStandard
  );
  this->AddOperationHandler(
    "URLKeyValuePairsDecode",
    CalculatorFunctions::innerURLKeyValuePairsToNormalRecursive,
    "",
    "Converts an url-encoded string to a normal string. "
    "All % signs are interpreted recursively "
    "as double, triple, ... url encoded strings and decoded accordingly.",
    "URLKeyValuePairsDecode(\"Problems%2fFunctions%2dcomposing%2dfractional%2dlinear%2d1.html=weight%3d3%26deadlines%3d%26&Problems%2fLimits%2dbasic%2dsubstitution%2d1.html=weight%3d3%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dinfinity%2dRF%2dequal%2ddeg%2d1.html=weight%3d2%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2dequal%2ddeg%2d1.html=weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dpm%2dinfinity%2dRF%2dnum%2ddeg%2dsmaller%2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d1.html =weight%3d5%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d2.html =weight%3d6%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dpm%2dinfinity%2dquotient%2dradical%2deven%2dpower%2dbasic % 2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dquotient%2dradical%2dodd%2dpower%2dbasic % 2d1.html =weight%3d1%26deadlines%3d%26&Homework%2fPrecalculus%2dPrerequisites%2dUMB%2d1.html = deadlines%3d%26&\")",
    "CalculatorConversions::innerUrlStringToNormalString",
    "URLKeyValuePairsDecode",
    innerStandard
  );
  this->AddOperationHandler(
    "LoadFileIntoString",
    CalculatorConversions::innerLoadFileIntoString,
    "",
    "Loads a file into a string. "
    "The file must be given its relative file name displayed when browsing "
    "the web server. "
    "There are two exceptions. 1) The file can be located in a folder in the project base "
    "that is otherwise not visible by the webserver **provided that** the folder is white-listed "
    "via the FileOperations class within the C++ source. Example: folder problems/ "
    "is white-listed. To access this file simply start your file name with the foldername. "
    "Do not start the folder name with the / character."
    "2) The file can be located in a folder in a level parallel to the project base "
    "- such folders are outside of the project folder - **provided that** "
    "the folder is white listed in the C++ code. ",
    "LoadFileIntoString(\"test/certificate_self_signed.base64\");\n"
    "LoadFileIntoString(\"problems/default/Functions-composing-fractional-linear-1.html\")",
    "CalculatorConversions::innerLoadFileIntoString",
    "LoadFileIntoString",
    innerStandard
  );
  this->AddOperationHandler(
    "InterpretProblem",
    CalculatorHtmlFunctions::innerInterpretProblem,
    "",
    "Does as ExtractCalculatorExpressionFromHtml but in "
    "addition interprets the calculator commands. ",
    "InterpretProblem(LoadFileIntoString("
    "\"problems/default/Functions-composing-fractional-linear-1.html\"))",
    "CalculatorHtmlFunctions::innerInterpretProblem",
    "InterpretProblem",
    innerNoTest
  );
  this->AddOperationHandler(
    "ProblemGiveUp",
    CalculatorHtmlFunctions::innerInterpretProblemGiveUp,
    "",
    "Gives the predefined answer to a problem. "
    "First argument must be a string with the problem. "
    "The second argument must be the id of the answer. "
    "Third argument must be the random seed integer.",
    "ProblemGiveUp(\n"
    "\"problems/Functions-composing-fractional-linear-1.html\",\n"
    "\"AlgebraAnswer\",\n"
    "\"123\"\n"
    ")",
    "CalculatorHtmlFunctions::innerInterpretProblemGiveUp",
    "ProblemGiveUp",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeInputBox",
    CalculatorHtmlFunctions::innerUserInputBox,
    "",
    "Creates an user input text box. ",
    "MakeInputBox(name = a, value = 3);",
    "CalculatorHtmlFunctions::innerUserInputBox",
    "MakeInputBox",
    innerStandard
  );
  this->AddOperationHandler(
    "SetInputBox",
    CalculatorHtmlFunctions::innerSetInputBox,
    "",
    "Sets value for input box that overrides the input box (no box is displayed). ",
    "SetInputBox(name = a, value =  3); "
    "MakeInputBox(name = a)",
    "CalculatorHtmlFunctions::innerSetInputBox",
    "SetInputBox",
    innerStandard
  );
  this->AddOperationHandler(
    "ExtractCalculatorExpressionFromHtml",
    CalculatorHtmlFunctions::innerExtractCalculatorExpressionFromHtml,
    "",
    "Reads html and extracts embedded calculator commands. Content enclosed in "
    "spans with appropriate class names is interpreted; all other content "
    "is copied without any change. At the moment of writing, "
    "the planned span class names are: \"calculator\", "
    "\"calculatorHidden\", \"calculatorAnswer\".",
    "ExtractCalculatorExpressionFromHtml(LoadFileIntoString("
    "\"problems/Functions-composing-fractional-linear-1.html\"))",
    "CalculatorHtmlFunctions::innerExtractCalculatorExpressionFromHtml",
    "ExtractCalculatorExpressionFromHtml",
    innerStandard
  );
  this->AddOperationHandler(
    "TestCalculatorIndicator",
    CalculatorFunctions::innerTestIndicator,
    "",
    "(This is not a mathematical function). Tests the calculator indicator mechanism."
    "First argument times number of iterations. "
    "Second argument = length of dummy comment appended to the calculator comments. "
    "Use a large dummy comment to test that large outputs are piped correctly to the "
    "monitoring process. ",
    "TestCalculatorIndicator(1000, 200000)",
    "CalculatorFunctions::innerTestIndicator",
    "TestCalculatorIndicator",
    innerNoTest,
    ""
  );
  this->AddOperationHandler(
    "TestTopCommand",
    CalculatorFunctions::innerTestTopCommand,
    "",
    "(This is not a mathematical function). Tests the top linux command. ",
    "TestTopOperation(1000)",
    "CalculatorFunctions::innerTestTopCommand",
    "TestTopCommand",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "Crash",
    CalculatorFunctions::innerCrash,
    "",
    "Crashes the calculator: tests the "
    "crashing mechanism (are crash logs properly created, etc.). ",
    "Crash(0)",
    "CalculatorFunctions::innerCrash",
    "Crash",
    innerNoTest
  );
  this->AddOperationHandler(
    "CrashListOutOfBounds",
    CalculatorFunctions::innerCrashByListOutOfBounds,
    "",
    "Crashes the calculator by attempting to use data out-of-bounds in a List data structure. ",
    "CrashListOutOfBounds(0)",
    "CalculatorFunctions::innerCrashByListOutOfBounds",
    "CrashListOutOfBounds",
    innerNoTest
  );
  this->AddOperationHandler(
    "CrashVectorOutOfBounds",
    CalculatorFunctions::innerCrashByVectorOutOfBounds,
    "",
    "Crashes the calculator by attempting to use data "
    "out-of-bounds in a std::vector.",
    "CrashVectorOutOfBounds(0)",
    "CalculatorFunctions::innerCrashByVectorOutOfBounds",
    "CrashVectorOutOfBounds",
    innerNoTest
  );
  this->AddOperationHandler(
    "PlotExpressionTree",
    CalculatorFunctions::innerDrawExpressionGraph,
    "",
    "Draws the internal tree structure of an expression. "
    "Does not unfold built-in types.",
    "PlotExpressionTree(e^x)",
    "CalculatorFunctions::innerDrawExpressionGraph",
    "PlotExpressionTree",
    innerStandard
  );
  this->AddOperationHandler(
    "Thaw",
    CalculatorFunctions::innerThaw,
    "",
    "If the argument is frozen, removes the top freeze command and returns the argument, "
    "else returns the argument unchanged.",
    "a =Freeze{}(1 + 1); Thaw a; c =Thaw(Freeze(a,b)); PlotExpressionTree c",
    "CalculatorFunctions::innerThaw",
    "Thaw",
    innerStandard
  );
  this->AddOperationHandler(
    "LogEvaluationStepsDebug",
    Calculator::innerLogEvaluationStepsDebug,
    "",
    "Creates an expression evaluation tree. Intended for debugging the system. ",
    "LogEvaluationStepsDebug((x +2)(x +3))",
    "CalculatorFunctions::innerLogEvaluationStepsDebug",
    "LogEvaluationStepsDebug",
    innerStandard
  );
  this->AddOperationHandler(
    "LogEvaluationSteps",
    Calculator::innerLogEvaluationSteps,
    "",
    "Show a step-by step evaluation of the calculator. "
    "Some steps my be combined or omitted for improved human readability. ",
    "LogEvaluationSteps((x +2)(x +3))",
    "CalculatorFunctions::innerLogEvaluationSteps",
    "LogEvaluationSteps",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotExpressionTreeFull",
    CalculatorFunctions::innerDrawExpressionGraphFull,
    "",
    "Draws the internal tree structure of an expression. Unfolds built-in types. ",
    "PlotExpressionTreeFull(1);\n"
    "PlotExpressionTree(1 + 1);\n"
    "PlotExpressionTree(Freeze{}(1 + 1));",
    "CalculatorFunctions::innerDrawExpressionGraphFull",
    "PlotExpressionTreeFull",
    innerStandard
  );
  this->AddOperationHandler(
    "Lispify",
    CalculatorFunctions::innerLispify,
    "",
    "Shows the internal tree structure of an expression, "
    "without completely unfolding the tree structure of "
    "expressions that represent a single mathematical entity.",
    "Lispify(e^x)",
    "CalculatorFunctions::innerLispify",
    "Lispify",
    innerStandard
  );
  this->AddOperationHandler(
    "FlattenCommandEnclosuresOneLayer",
    Calculator::innerFlattenCommandEnclosuresOneLayeR,
    "",
    "Flattens command enclosures. ",
    "FlattenCommandEnclosuresOneLayer(CommandEnclosure{}(x = 5; x); "
    "CommandEnclosure{}(y; x))",
    "Calculator::innerFlattenCommandEnclosuresOneLayeR",
    "FlattenCommandEnclosuresOneLayer",
    innerStandard
  );
  this->AddOperationHandler(
    "LispifyFull",
    CalculatorFunctions::innerLispifyFull,
    "",
    "Shows the complete internal tree structure of an expression "
    "(replacing the expression with a string).",
    "LispifyFull(e^x)",
    "CalculatorFunctions::innerLispifyFull",
    "LispifyFull",
    innerStandard
  );
  this->AddOperationHandler(
    "ChildExpression",
    CalculatorFunctions::innerChildExpression,
    "",
    "If defined, returns the nth child of an expression.",
    "ChildExpression(e^x, 1); ChildExpression(e^x, 2); ChildExpression(e^x, 3)",
    "CalculatorFunctions::innerChildExpression",
    "ChildExpression",
    innerStandard
  );
  this->AddOperationHandler(
    "ToString",
    CalculatorFunctions::innerExpressionToString,
    "",
    "Transforms an arbitrary expression to its string representation. "
    "The string representation is (supposed to be) LaTeX-compatible. ",
    "ToString( e^x); \n \"e^x\";\"The quick brown fox jumps over the lazy dog.\"",
    "CalculatorFunctions::innerExpressionToString",
    "ToString",
    innerStandard
  );
  this->AddOperationHandler(
    "ToUTF8String",
    CalculatorFunctions::innerExpressionToUTF8String,
    "",
    "Transforms an arbitrary expression to a utf8-string representation. "
    "The string is supposed to look reasonable when drawn on a javascript canvas. ",
    "f = \\sin {}x / \\cos {}x;\n"
    "Plot(f, -\\pi/4, \\pi/4) + PlotLabel{}((1,1), \"y =\" + ToUTF8String(f))",
    "CalculatorFunctions::innerExpressionToUTF8String",
    "ToUTF8String",
    innerStandard
  );
  this->AddOperationHandler(
    "StringToAtom",
    CalculatorFunctions::innerStringToAtom,
    "",
    "Transforms a string to an atom. ",
    "StringToAtom(\"The quick brown fox jumps over the lazy dog.\")",
    "CalculatorFunctions::innerStringToAtom",
    "StringToAtom",
    innerStandard
  );
  this->AddOperationHandler(
    "\"",
    CalculatorFunctions::innerQuoteToString,
    "",
    "Creates a string. ",
    "\"The quick brown fox jumps over the lazy dog.\"",
    "CalculatorFunctions::innerQuoteToString",
    "QuoteToString",
    innerStandard
  );
  this->AddOperationHandler(
    "TestBase64",
    CalculatorFunctions::innerBase64ToCharToBase64Test,
    "",
    "Test function: converts a base64 string to bitstream and back to base64. "
    "Output must be identical to input. ",
    "TestBase64(\"TheQuickBrownFoxJumpsOverTheLazyDog=\");\n"
    "TestBase64(\"TheQuickBrownFoxJumpsOverTheLazyDog\")",
    "CalculatorFunctions::innerBase64ToCharToBase64Test",
    "TestBase64",
    innerStandard
  );
  this->AddOperationHandler(
    "EllipticCurveOrderNIST",
    CalculatorFunctions::innerNISTEllipticCurveOrder,
    "",
    "Get a NIST curve order. At present implemented for secp256k1 only. ",
    "g = EllipticCurveGeneratorNIST(\"secp256k1\");\n"
    "order = EllipticCurveOrderNIST(\"secp256k1\");\n"
    "g^order",
    "CalculatorFunctions::innerNISTEllipticCurveOrder",
    "EllipticCurveOrderNIST",
    innerStandard
  );
  this->AddOperationHandler(
    "EllipticCurveGeneratorNIST",
    CalculatorFunctions::innerNISTEllipticCurveGenerator,
    "",
    "Makes generator of a NIST curve. At present implemented for secp256k1 only. ",
    "g=EllipticCurveGeneratorNIST(\"secp256k1\");\n"
    "g^3; g^115792089237316195423570985008687907852837564279074904382605163141518161494337",
    "CalculatorFunctions::innerNISTEllipticCurveGenerator",
    "EllipticCurveGeneratorNIST",
    innerStandard
  );

  this->AddOperationHandler(
    "StringDifference",
    CalculatorFunctions::innerStringDifference,
    "",
    "Computes the difference between two strings, "
    "provided that the strings are small enough. "
    "The algorithm consumes O(leftStringSize * rightStringSize) RAM. "
    "For more information, see "
    "https://en.wikipedia.org/wiki/Longest_common_subsequence_problem.",
    "StringDifference(\"XMJYAUZ\",\"MZJAWXU\")",
    "CalculatorFunctions::innerStringDiff",
    "StringDifference",
    innerStandard
  );

  this->AddOperationHandler(
    "Slice",
    CalculatorFunctions::innerSliceString,
    "",
    "Slices a string in an interval [a,b), "
    "where a is the first included index and b the first excluded index. "
    "If b is omitted, then the entire string is sliced. "
    "If b is negative, it is replaced by its sum with the string size. ",
    "a= \"abcd\";\n"
    "Slice(a, 0,2);\n"
    "Slice(a,0,- 1);\n"
    "Slice(a,1)\n",
    "CalculatorFunctions::innerSliceString",
    "Slice",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertIntegerToBase58",
    CalculatorFunctions::innerConvertIntegerUnsignedToBase58,
    "",
    "Converts an unsigned integer to base58. ",
    "theInt = ConvertHexToInteger(ConvertBase58ToHex(\"1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK\"));"
    "ConvertIntegerToBase58(theInt)",
    "CalculatorFunctions::innerConvertIntegerUnsignedToBase58",
    "ConvertIntegerToBase58",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertBase58ToHex",
    CalculatorFunctions::innerConvertBase58ToHex,
    "",
    "Converts Base58 to hex. ",
    "ConvertBase58ToHex(\"1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK\");",
    "CalculatorFunctions::innerConvertBase58ToHex",
    "ConvertBase58ToHex",
    innerStandard
  );
  this->AddOperationHandler(
    "AESCBCEncrypt",
    CalculatorFunctionsCrypto::innerAES_CBC_256_Encrypt,
    "",
    "Encodes using aes 256 bit in cbc (cipher block chain) mode. "
    "First argument = key. Second argument = text. Reference: NIST SP 800-38A.",
    "text = ConvertHexToString \"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710\";\n"
    "key = ConvertHexToString \"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4\";\n"
    "ConvertStringToHex AESCBCEncrypt(key, text);\n"
    "\"f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b\";\n",
    "CalculatorFunctionsCrypto::innerAES_CBC_256_Encrypt",
    "AESCBCEncrypt",
    innerStandard
  );
  this->AddOperationHandler(
    "AESCBCDecrypt",
    CalculatorFunctionsCrypto::innerAES_CBC_256_Decrypt,
    "",
    "AES decryption. First argument key, second argument - text. ",
    "text = \"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710\";\n"
    "key = \"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4\";\n"
    "cipherText = ConvertStringToHex AESCBCEncrypt(ConvertHexToString key, ConvertHexToString text);\n"
    "cipherText;\n"
    "\"f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b\";\n"
    "ConvertStringToHex AESCBCDecrypt(ConvertHexToString key, ConvertHexToString cipherText);\n"
    "text",
    "CalculatorFunctionsCrypto::innerAES_CBC_256_Decrypt",
    "AESCBCDecrypt",
    innerStandard
  );
  this->AddOperationHandler(
    "AppendDoubleSha256Check",
    CalculatorFunctions::innerAppendDoubleSha256Check,
    "",
    "Appends a sha 256 checksum to a string. More precisely, appends the first 4 bytes "
    "of sha256 of the string to the string. ",
    "A= \"80aad3f1f5de25ff67a4fd3d7808d58510e00ec08a55c10ad5751facf35411509701\";\n"
    "B= ConvertHexToString(A);\n"
    "C= AppendDoubleSha256Check(B);\n"
    "D= ConvertStringToHex(C)",
    "CalculatorFunctions::innerAppendDoubleSha256Check",
    "AppendDoubleSha256Check",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertHexToBase58",
    CalculatorFunctions::innerConvertHexToBase58,
    "",
    "Converts hex to base58. ",
    "ConvertHexToBase58(\"03aaf2d5530b1a5cbf80c248ca44635ac265f4104ffc5b76ef48f361c03b7f536f\");",
    "CalculatorFunctions::innerConvertHexToBase58",
    "ConvertHexToBase58",
    innerStandard
  );
  this->AddOperationHandler(
    "CharToBase64",
    CalculatorFunctions::innerCharToBase64,
    "",
    "Converts characters to bit stream and the bitstream to base64. "
    "The character to bit stream conversion is not fixed at the moment "
    "and may be system/compiler dependent. "
    "I believe that the character to bit stream conversion should be standard for "
    "the standard letters in the alphabet. "
    "Fancy UTF8 will probably be not read correctly from the CGI input, "
    "and furthermore will not be converted in a standard fashion to bit stream. "
    "The examples below are taken from Wikipedia. ",
    "CharToBase64(\"pleasure.\");\n"
    "CharToBase64(\"leasure.\");\n"
    "CharToBase64(\"easure.\");\n"
    "CharToBase64(\"asure.\");\n"
    "CharToBase64(\"sure.\");\n",
    "CalculatorFunctions::innerCharToBase64",
    "CharToBase64",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertBase64ToString",
    CalculatorFunctions::innerConvertBase64ToString,
    "",
    "Converts base64 to string",
    "ConvertBase64ToString("
    "\"k7qTF1hLeOdihfKG5IRnlb7us2FVo1pSC2r0DVLkYwRAQHMs4XatvGcdG81S64uoaqG4fZ9IHJNpZjqokojuX5VIwl6utBO9\""
    ");",
    "CalculatorFunctions::innerConvertBase64ToString",
    "ConvertBase64ToString",
    innerStandard
  );
  this->AddOperationHandler(
    "LoadKnownCertificates",
    CalculatorFunctions::innerLoadKnownCertificates,
    "",
    "Loads known security certificates from the <a href=\"/certificates-public/\">certificates-public/</a> folder. ",
    "LoadKnownCertificates(0);",
    "CalculatorFunctions::innerLoadKnownCertificates",
    "LoadKnownCertificates",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "TestLoadPEMCertificate",
    CalculatorFunctions::innerTestLoadPEMCertificates,
    "",
    "Tests the pem parsing functions. ",
    "TestLoadPEMCertificate(ConvertBase64ToString(LoadFileIntoString(\"test/certificate_self_signed.base64\")));\n",
    "CalculatorFunctions::innerTestLoadPEMCertificates",
    "TestLoadPEMCertificate",
    innerStandard
  );
  this->AddOperationHandler(
    "TestLoadPEMPrivateKey",
    CalculatorFunctions::innerTestLoadPEMPrivateKey,
    "",
    "Tests the pem private key functions. ",
    "TestLoadPEMPrivateKey(ConvertBase64ToString(LoadFileIntoString(\"test/private_key.base64\")));\n",
    "CalculatorFunctions::innerTestLoadPEMPrivateKey",
    "TestLoadPEMPrivateKey",
    innerStandard
  );
  this->AddOperationHandler(
    "TestTLSMessageSequence",
    CalculatorFunctions::innerTestTLSMessageSequence,
    "",
    "Creates a TLS server and test-sends "
    "messages to it starting with the client hello. "
    "The first argument will be the server's private key. "
    "The second argument will be the server's certificate. "
    "The next argument will be the client hello. ",
    "%HideLHS\nTestTLSMessageSequence(\n"
    "LoadFileIntoString(\"test/private_key.pem\"),\n"
    "LoadFileIntoString(\"test/certificate_self_signed.pem\"),\n"
    "ConvertHexToString("
    "LoadFileIntoString(\"test/message_client_ssl_0.hex\")"
    ")\n);\n",
    "CalculatorFunctions::innerTestTLSMessageSequence",
    "TestTLSMessageSequence",
    innerNoTestExperimental
  );
  this->AddOperationHandler(
    "TestTLSDecodeSSLRecord",
    CalculatorFunctions::innerTestTLSDecodeSSLRecord,
    "",
    "Decodes a client hello. ",
    "%HideLHS\nTestTLSDecodeSSLRecord(\n"
    "LoadFileIntoString(\"test/message_client_ssl_0.hex\")\n"
    ")",
    "CalculatorFunctions::innerTestTLSDecodeSSLRecord",
    "TestTLSDecodeSSLRecord",
    innerExperimental
  );
  this->AddOperationHandler(
    "TestASN1Decode",
    CalculatorFunctions::innerTestASN1Decode,
    "",
    "Tests decoding of abstract syntax one. ",
    "%HideLHS\n"
    "TestASN1Decode(ConvertBase64ToString(LoadFileIntoString(\"test/certificate_self_signed.base64\")));\n"
    "TestASN1Decode(ConvertBase64ToString(LoadFileIntoString(\"test/private_key.base64\")));\n",
    "CalculatorFunctions::innerTestASN1Decode",
    "TestASN1Decode",
    innerExperimental
  );
  this->AddOperationHandler(
    "X509CertificateServerBase64",
    CalculatorFunctions::innerX509CertificateServer,
    "",
    "Returns the base 64 encoding of the X509 certificate of this server. ",
    "X509CertificateServerBase64 0; ",
    "CalculatorFunctions::innerX509certificateCrunch",
    "X509CertificateServerBase64",
    innerNoTest
  );
  this->AddOperationHandler(
    "X509CertificateDecode",
    CalculatorFunctions::innerX509CertificateDecode,
    "",
    "Decodes raw X509 certificate to a string. ",
    "%HideLHS\n"
    "X509CertificateDecode ConvertBase64ToString\n"
    "LoadFileIntoString(\"test/certificate_self_signed.pem\")",
    "CalculatorFunctions::innerX509CertificateDecode",
    "X509CertificateDecode",
    innerStandard
  );
  this->AddOperationHandler(
    "ShowKnownASNObjectIds",
    CalculatorFunctions::innerShowKnownObjectIds,
    "",
    "Lists all abstract syntax one object ids hard-coded in the calculator.",
    "ShowKnownASNObjectIds 0",
    "CalculatorFunctions::innerShowKnownObjectIds",
    "ShowKnownASNObjectIds",
    innerStandard
  );
  this->AddOperationHandler(
    "JWTVerifyAgainstKnownKeys",
    CalculatorFunctions::innerJWTVerifyAgainstKnownKeys,
    "",
    "Tries to verify a Json Web Token.",
    "%HideLHS JWTverifyAgainstKnownKeys(\"\"); ",
    "CalculatorFunctions::innerJWTverifyAgainstKnownKeys",
    "JWTVerifyAgainstKnownKeys",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "JWTVerifyRSA256",
    CalculatorFunctions::innerJWTVerifyAgainstRSA256,
    "",
    "Tries to verify a Json Web Token with respect to a given rsa modulus and exponent. "
    "The reference JWT token was taken from: https://tools.ietf.org/html/rfc7515#page-38, "
    "Appendix A.2.",
    "%HideLHS "
    "token =\"eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_dO--xi12jzDwusC-eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AXLIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw\";\n"
    "modulus = \"ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ\";\n"
    "exponent =\"AQAB\";\n"
    "JWTVerifyRSA256(token,modulus,exponent);",
    "CalculatorFunctions::innerJWTVerifyAgainstRSA256",
    "JWTVerifyRSA256",
    innerStandard
  );
  this->AddOperationHandler(
    "Sha1",
    CalculatorFunctions::innerSha1OfString,
    "",
    "Converts characters to a sequence of bits and computes the sha1 hash value of those bits. "
    "The examples below are taken from Wikipedia. ",
    "Sha1(\"The quick brown fox jumps over the lazy dog\");\n"
    "Sha1(\"The quick brown fox jumps over the lazy cog\");",
    "CalculatorFunctions::innerSha1OfString",
    "Sha1",
    innerStandard
  );
  this->AddOperationHandler(
    "Sha224",
    CalculatorFunctions::innerSha224OfString,
    "",
    "Converts characters to a sequence of bits and computes the sha224 hash value of those bits. "
    "Reference: Wikipedia. ",
    "Sha224(\"\");",
    "CalculatorFunctions::innerSha224OfString",
    "Sha224",
    innerStandard
  );
  this->AddOperationHandler(
    "LogarithmBaseNCeiling",
    CalculatorFunctions::innerLogarithmBaseNCeiling,
    "",
    "Returns ceiling(log_N (input)), i.e., the smallest integer "
    "X for which input <= N^X. First argument = N = base, "
    "second argument = input.",
    "LogarithmBaseNCeiling(2, 951891382051282682454493);\n"
    "log_2 951891382051282682454493;\n"
    "x = LogarithmBaseNCeiling(2, 100! + 1);\n"
    "2^x - (100!+1);\n"
    "2^{x+1} - (100!+1);\n"
    "y=LogarithmBaseNCeiling(10, 1000! + 1);\n"
    "10^y - 1000! - 1; 10^{y+1} - 1000! - 1;\n"
    "DoubleValue(log_2(1000!+1)) + 1;\n"
    "LogarithmBaseNCeiling(2, 1);\n"
    "LogarithmBaseNCeiling(-2, 1);\n"
    "LogarithmBaseNCeiling(2, -1);\n",
    "CalculatorFunctions::innerLogarithmBaseNCeiling",
    "LogarithmBaseNCeiling",
    innerStandard
  );
  this->AddOperationHandler(
    "TestRSASign",
    CalculatorFunctions::innerTestRSASign,
    "",
    "Tests the RSA signature function. "
    "Input has three arguments: 1) message to sign "
    "2) private key first prime, "
    "3) private key second prime. "
    "Will set hard-coded defaults "
    "for all non-specified parameters. ",
    "TestRSASign(\"asdf\", "
    "426783863749219482096610996314660012394857818566077035"
    "178241209143920182457158933390658353396537264757052464"
    "334913365120085767481521352901499860151171619220023060"
    "480464866565560676440226021303, "
    "352815577859200421265693524055335110516168016791939066"
    "619744890606270245172389068191436591061733904616135797"
    "438741188808884326847918800555526459128765065042606467"
    "292598531117932066982164093023)",
    "CalculatorFunctions::innerTestRSASign",
    "TestRSASign",
    innerStandard
  );
  this->AddOperationHandler(
    "GenerateRandomPrime",
    CalculatorFunctions::innerGenerateRandomPrime,
    "",
    "Generate random prime. Argument = desired number of bytes, max 128. ",
    "GenerateRandomPrime(10)",
    "CalculatorFunctions::innerGenerateRandomPrime",
    "GenerateRandomPrime",
    innerNoTest
  );

  this->AddOperationHandler(
    "RSAEncrypt",
    CalculatorFunctions::innerRSAEncrypt,
    "",
    "Encrypts with RSA. First argument: modulus. Second argument: (public) exponent. "
    "Third argument: message given as a large integer.",
    "RSAEncrypt(3233, 17, 65)",
    "CalculatorFunctions::innerRSAEncrypt",
    "RSAEncrypt",
    innerStandard
  );
  this->AddOperationHandler(
    "Ripemd160",
    CalculatorFunctions::innerRIPEMD160OfString,
    "",
    "Ripemd160 hash function. See wikipedia page. ",
    "ConvertStringToHex Ripemd160(\"The quick brown fox jumps over the lazy dog\");\n"
    "\"37f332f68db77bd9d7edd4969571ad671cf9dd3b\";\n"
    "ConvertStringToHex Ripemd160(\"The quick brown fox jumps over the lazy cog\");\n"
    "\"132072df690933835eb8b6ad0b77e7b6f14acad7\";\n",
    "CalculatorFunctions::innerRIPEMD160OfString",
    "Ripemd160",
    innerStandard
  );
  this->AddOperationHandler(
    "ShaThree256",
    CalculatorFunctions::innerSha3_256OfString,
    "",
    "SHA3 of input string, 256 bit version. See the wikipedia page on SHA3. ",
    "%HideLHS\n"
    "ConvertStringToHex ShaThree256(\"\");\n"
    "\"a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a\";\n"
    "ConvertStringToHex ShaThree256(\"abc\");\n"
    "\"3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532\";\n"
    "ConvertStringToHex ShaThree256(\"testing\");\n"
    "\"7f5979fb78f082e8b1c676635db8795c4ac6faba03525fb708cb5fd68fd40c5e\";\n"
    "ConvertStringToHex ShaThree256\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\";\n"
    "\"41c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376\";\n"
    "ConvertStringToHex ShaThree256\"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu\";\n"
    "\"916f6061fe879741ca6469b43971dfdb28b1a32dc36cb3254e812be27aad1d18\";\n"
    "ConvertStringToHex Keccak256(\"testing\");\n"
    "\"5f16f4c7f149ac4f9510d9cf8cf384038ad348b3bcdc01915f95de12df9d1b02\";\n",
    "CalculatorFunctions::innerSha3_256OfString",
    "ShaThree256",
    innerStandard
  );
  this->AddOperationHandler(
    "Keccak256",
    CalculatorFunctions::innerKeccak256OfString,
    "",
    "Keccak256 of input string, 256 bit version. This is ``non-stardard sha3'' "
    "and is different from the sha3. See the wikipedia page on SHA3/Keccak. ",
    "ConvertStringToHex ShaThree256(\"abc\");\n"
    "\"3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532\";\n"
    "ConvertStringToHex ShaThree256(\"\");\n"
    "\"a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a\";\n"
    "ConvertStringToHex ShaThree256(\"testing\");\n"
    "\"7f5979fb78f082e8b1c676635db8795c4ac6faba03525fb708cb5fd68fd40c5e\";\n"
    "ConvertStringToHex Keccak256(\"testing\");\n"
    "\"5f16f4c7f149ac4f9510d9cf8cf384038ad348b3bcdc01915f95de12df9d1b02\";\n",
    "CalculatorFunctions::innerKeccak256OfString",
    "Keccak256",
    innerStandard
  );
  this->AddOperationHandler(
    "Sha256",
    CalculatorFunctions::innerSha256OfString,
    "",
    "Converts characters to a sequence of bits and computes the sha256 hash value of those bits. "
    "Reference: Wikipedia. ",
    "%HideLHS\n"
    "ConvertStringToHex Sha256(\"\");\n"
    "\"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\";\n"
    "ConvertStringToHex Sha256(\"abc\");\n"
    "\"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\";\n"
    "ConvertStringToHex Sha256(\"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu\");\n"
    "\"cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1\";\n",
    "CalculatorFunctions::innerSha256OfString",
    "Sha256",
    innerStandard
  );
  this->AddOperationHandler(
    "Sha512",
    CalculatorFunctions::innerSha512,
    "",
    "Converts characters to a sequence of bits and computes SHA512. "
    "UTF8 encoding is used for example space bar is converted to 0x32. "
    "Reference: Wikipedia. ",
    "%HideLHS\n"
    "ConvertStringToHex Sha512(\"\");\n"
    "\"cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e\";\n"
    "ConvertStringToHex Sha512(\"abc\");\n"
    "\"ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f\";\n"
    "ConvertStringToHex Sha512(\"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu\");\n"
    "\"8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909\";\n",
    "CalculatorFunctions::innerSha512",
    "Sha512",
    innerStandard
  );
  this->AddOperationHandler(
    "Sha256Verbose",
    CalculatorFunctions::innerSha256OfStringVerbose,
    "",
    "Same as Sha256 but more verbose. ",
    "Sha256Verbose(\"\");",
    "CalculatorFunctions::innerSha256OfStringVerbose",
    "Sha256Verbose",
    innerStandard
  );
  this->AddOperationHandler(
    "TestJSON",
    CalculatorFunctions::innerTestJSON,
    "",
    "Tests the JSON parsing mechanism. Input: json string, use backslash escapes for "
    "backslashes and quotes.",
    "TestJSON(\"{a:1}\"); TestJSON(\"{\\\"a\\\":\\\"\\\\\\\"\\\"}\");",
    "CalculatorFunctions::innerTestJSON",
    "TestJSON",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertBase64ToHex",
    CalculatorFunctions::innerBase64ToHex,
    "",
    "Converts base64 string to hexadecimal string. ",
    "ConvertBase64ToHex(\"AQAB\");",
    "CalculatorFunctions::innerBase64ToHex",
    "ConvertBase64ToHex",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertStringToHex",
    CalculatorFunctions::innerConvertStringToHex,
    "",
    "Converts a bitstream (not necessarily UTF-8 encoded) to hex. ",
    "ConvertStringToHex(Sha256(Sha256(\"hello\")));",
    "CalculatorFunctions::innerConvertStringToHex",
    "ConvertStringToHex",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertHexToInteger",
    CalculatorFunctions::innerHexToInteger,
    "",
    "Converts a hex string to an integer. ",
    "ConvertHexToInteger(Base64ToHex(\"AQAB\"));",
    "CalculatorFunctions::innerHexToInteger",
    "ConvertHexToInteger",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertIntegerToHex",
    CalculatorFunctions::innerIntegerToHex,
    "",
    "Converts an integer to hex string. ",
    "ConvertIntegerToHex(65537);",
    "CalculatorFunctions::innerIntegerToHex",
    "ConvertIntegerToHex",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertHexToString",
    CalculatorFunctions::innerHexToString,
    "",
    "Converts a hex string to a string. ",
    "ConvertHexToString(\"3031300d060960864801650304020105000420\");",
    "CalculatorFunctions::innerHexToString",
    "ConvertHexToString",
    innerStandard
  );
  this->AddOperationHandler(
    "FormatCPPCode",
    CalculatorFunctions::innerFormatCPPSourceCode,
    "",
    "Format cpp code. ",
    "FormatCPPCode{}(\"src/database.cpp\")",
    "Calculator::innerFormatCPPSourceCode",
    "FormatCPPCode",
    innerInvisibleNoTest
  );
  this->AddOperationHandler(
    "FormatCPPDirectory",
    CalculatorFunctions::innerFormatCPPDirectory,
    "",
    "Format cpp directory. ",
    "FormatCPPDirectory{}(\"src/\")",
    "Calculator::innerFormatCPPDirectory",
    "FormatCPPDirectory",
    innerInvisibleNoTest
  );

  this->AddOperationHandler(
    "TestCalculatorAll",
    Calculator::innerAutomatedTest,
    "",
    "Runs an automated test of built in functions "
    "against a set of known good results. "
    "First argument = index of first test to run (0 = run all)."
    "If positive, the second argument gives the number of tests to run. "
    "Else, when the second argument is negative or zero, we run all tests. ",
    "TestCalculatorAll{}(0, 0)",
    "Calculator::innerAutomatedTest",
    "TestCalculatorAll",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "TestTopicListProblems",
    CalculatorFunctions::innerTestTopicListProblems,
    "",
    "Tests whether all files given in the default topic lists open properly. ",
    "TestTopicListProblems 0",
    "CalculatorFunctions::innerTestTopicListProblems",
    "TestTopicListProblems",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "TestProblemInterpretation",
    CalculatorFunctions::innerTestProblemInterpretation,
    "",
    "Runs an automated test of built in "
    "problems located in the problems/default/ folder. "
    "First argument = index of first problem to test. "
    "Second argument = number of problems to test. "
    "Third argument = starting random seed. "
    "To test a particular problem, set the index of the "
    "first problem to that of your problem, and set the "
    "number of problems to test to 1. To find out the index"
    "of your problem, run the entire test array once and "
    "the index of each problem file will be listed in the output. "
    "Until first error is seen, every test will be run a number of times, "
    "3 at the time of writing. "
    "The random seed will be incremented for each run. ",
    "TestProblemInterpretation{}(0, 0, 0)",
    "CalculatorFunctions::innerTestProblemInterpretation",
    "TestProblemInterpretation",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "!",
    CalculatorFunctions::innerFactorial,
    "",
    "Factorial function. ",
    "5!",
    "CalculatorFunctions::innerFactorial",
    "Factorial",
    innerStandard
  );
  this->AddOperationHandler(
    "RepresentElementHyperoctahedral",
    CalculatorFunctionsWeylGroup::innerRepresentElementHyperOctahedral,
    "",
    "Represents element of hyperoctahedral into a representation. ",
    "V= HyperOctahedralRepresentation((1,1),1);\n"
    "s = MakeElementHyperOctahedral{}((1,2),1,0,0);\n"
    "RepresentElementHyperoctahedral(s, V)",
    "CalculatorFunctionsWeylGroup::innerRepresentElementHyperOctahedral",
    "RepresentElementHyperoctahedral",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "HyperOctahedralIrreps",
    CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules,
    "",
    "Prints all modules induced from Specht modules. ",
    "HyperOctahedralIrreps(3)",
    "CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules",
    "HyperOctahedralIrreps",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "SpechtModule",
    CalculatorFunctionsWeylGroup::innerSpechtModule,
    "",
    "Gets the Specht module of the partition. ",
    "SpechtModule((3,2,1))",
    "CalculatorFunctionsWeylGroup::innerSpechtModule",
    "SpechtModule",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "HyperOctahedralRepresentation",
    CalculatorFunctionsWeylGroup::innerHyperOctahedralGetOneRepresentation,
    "",
    "Gets one hyperoctahedral representation from two partitions. ",
    "HyperOctahedralRepresentation((1,1), (1))",
    "CalculatorFunctionsWeylGroup::innerHyperOctahedralAllModulesInducedFromSpechtModules",
    "HyperOctahedralRepresentation",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "HyperOctahedralGeneratorPrint",
    CalculatorFunctionsWeylGroup::innerHyperOctahedralPrintGeneratorCommutationRelations,
    "",
    "Prints the generator commutation relations of a hyperoctahedral group. ",
    "HyperOctahedralGeneratorPrint(3)",
    "CalculatorFunctionsWeylGroup::innerHyperOctahedralPrintGeneratorCommutationRelations",
    "HyperOctahedralGeneratorPrint",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "PrintMacdonaldPolys",
    CalculatorFunctionsWeylGroup::innerMacdonaldPolys,
    "",
    "Prints macdonald polynomials from a semisimple type. ",
    "PrintMacdonaldPolys{}(B_3)",
    "CalculatorFunctionsWeylGroup::innerMacdonaldPolys",
    "PrintMacdonaldPolys",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "SubList",
    CalculatorFunctions::innerSubList,
    "",
    "Extracts a sub-list that contains all elements that satisfy a "
    "condition given by the second argument. Use the bound "
    "variable syntax {{a}} to address the current element of the list.",
    "SubList{}((1,2,3,4), {{a}}>2)",
    "CalculatorFunctions::innerSubList",
    "SubList",
    innerStandard
  );
  this->AddOperationHandler(
    "ApplyToList",
    CalculatorFunctions::innerApplyToList,
    "",
    "Applies a transformation to each element of a list. ",
    "SolveForX{}{{a}}=SolveFor(x,{{a}});\n"
    "SolveForX(x^2+2);\n"
    "ApplyToList(SolveForX,(x^2+ 1,x^2-x-2))",
    "CalculatorFunctions::innerApplyToList",
    "ApplyToList",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "MakeExpression",
    CalculatorConversions::innerExpressionFromBuiltInTypE,
    "",
    "Creates expression from built-in polynomial. ",
    "MakeExpression(Polynomial{}((x-2y+z- 1)^2(x +y-z)));\n"
    "A = \\frac{64 x^{2} y x y+ 16 x y- y}{-32 x^{2} y x^{2}-8 x^{2}+x};"
    "B = MakeRationalFunction(A);"
    "C = MakeExpression B;"
    "D = MakeRationalFunction {}("
    "\\frac{-2x^{3}y^{2}-\\frac{x y }{2}+\\frac{y }{32}}{x^{4}y +\\frac{x^{2}}{4}-\\frac{x }{32}}"
    ");\n"
    "MakeExpression {}D-C;\n"
    "D-B;",
    "CalculatorConversions::innerExpressionFromBuiltInTypE",
    "MakeExpression",
    innerStandard
  );
  this->AddOperationHandler(
    "Polynomial",
    CalculatorConversions::innerPolynomiaL<Rational>,
    "",
    "Creates a polynomial expression with rational coefficients. ",
    "Polynomial{}((x-2y+z- 1)^2(x +y-z));"
    "\nPolynomial{}(y^2)-(Polynomial{}y)^2",
    "CalculatorConversions::innerPolynomiaL",
    "Polynomial",
    innerStandard
  );
  this->AddOperationHandler(
    "PolynomialAlgebraicNumbers",
    CalculatorConversions::innerPolynomiaL<AlgebraicNumber>,
    "",
    "Creates a polynomial expression with algebraic number coefficients. ",
    "PolynomialAlgebraicNumbers{}((x +\\sqrt{2})^2 (\\sqrt{3}x-\\sqrt{5}));",
    "CalculatorConversions::innerPolynomiaL",
    "PolynomialAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationHandler(
    "ConvertAlgebraicNumberToMatrix",
    CalculatorFunctions::innerConvertAlgebraicNumberToMatrix,
    "",
    "Converts the algebraic number to its internal matrix representation. ",
    "a = \\sqrt{2};\n"
    "A = ConvertAlgebraicNumberToMatrix(a);\n"
    "b = \\sqrt{3};\n"
    "B = ConvertAlgebraicNumberToMatrix(b);\n"
    "c = \\sqrt{6};\n"
    "C = ConvertAlgebraicNumberToMatrix(c);\n"
    "A\\otimes B",
    "CalculatorFunctions::innerConvertAlgebraicNumberToMatrix",
    "ConvertAlgebraicNumberToMatrix",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintAlgebraicClosureStatus",
    CalculatorFunctions::innerPrintAlgebraicClosureStatus,
    "",
    "Prints in human-redable form the state of the ambient algebraic closure. ",
    "AlgebraicNumberFromPolynomial(x^2-6);\n"
    "PrintAlgebraicClosureStatus 0;\n"
    "AlgebraicNumberFromPolynomial(x^2-2);\n"
    "PrintAlgebraicClosureStatus 0;\n"
    "AlgebraicNumberFromPolynomial(x^2-3);\n"
    "PrintAlgebraicClosureStatus 0;\n",
    "CalculatorFunctions::innerPrintAlgebraicClosureStatus",
    "PrintAlgebraicClosureStatus",
    innerStandard
  );
  this->AddOperationHandler(
    "AlgebraicNumberFromPolynomial",
    CalculatorFunctions::innerGetAlgebraicNumberFromMinPoly,
    "",
    "Creates an algebraic number that is a root of a polynomial with algebraic number coefficients. ",
    "AlgebraicNumberFromPolynomial{}(x^2 - 4);\n"
    "AlgebraicNumberFromPolynomial{}(x^2 - (3+2sqrt(2)));\n"
    "AlgebraicNumberFromPolynomial{}(x^3 - (7+5sqrt(2)));\n"
    "AlgebraicNumberFromPolynomial{}(x^3+\\sqrt{2}x + 1);\n",
    "CalculatorFunctions::innerGetAlgebraicNumberFromMinPoly",
    "AlgebraicNumberFromPolynomial",
    innerStandard
  );
  this->AddOperationHandler(
    "PrincipalSlTwoIndex",
    CalculatorFunctions::innerGetPrincipalSl2Index,
    "",
    "Gives the symmetric Cartan corresponding to a given dynkin type.",
    "PrincipalSlTwoIndex(G_2^3+D_4^2);",
    "CalculatorFunctions::innerGetPrincipalSl2Index",
    "PrincipalSlTwoIndex",
    innerStandard
  );
  this->AddOperationHandler(
    "DynkinIndicesSlTwoSubalgebras",
    CalculatorFunctions::innerGetDynkinIndicesSlTwoSubalgebras,
    "",
    "Fetches the absolute Dynkin indices of the sl(2) subalgebras of the input Dynkin type. "
    "The indices of each simple component are computed by actually constructing "
    "the sl(2)-subalgebras; clearly this can be greatly optimized "
    "but we postpone that for a future version. ",
    "DynkinIndicesSlTwoSubalgebras(2A_2);\n "
    "DynkinIndicesSlTwoSubalgebras(2A_2+A_2^2);\n"
    "DynkinIndicesSlTwoSubalgebras(2A_2+A_2^7);\n"
    "DynkinIndicesSlTwoSubalgebras(2G_2+G_2^7+F_4+F_4^19);",
    "CalculatorFunctions::innerGetDynkinIndicesSlTwoSubalgebras",
    "DynkinIndicesSlTwoSubalgebras",
    innerStandard
  );
  this->AddOperationHandler(
    "CartanSymmetric",
    CalculatorFunctions::innerGetSymmetricCartan,
    "",
    "Gives the symmetric Cartan corresponding to a given dynkin type.",
    "CartanSymmetric(e^7_6);",
    "CalculatorFunctions::innerGetSymmetricCartan",
    "CartanSymmetric",
    innerStandard
  );
  this->AddOperationHandler(
    "GrowDynkinType",
    CalculatorFunctions::innerGrowDynkinType,
    "",
    "This is a calculator testing function. Grows a Dynkin "
    "type inside an ambient Dynkin type. ",
    "GrowDynkinType(A^30_1+d^30_4, e_6);\n"
    "GrowDynkinType(g^35_2+B^30_2, e_6);",
    "CalculatorFunctions::innerGrowDynkinType",
    "GrowDynkinType",
    innerStandard
  );
  this->AddOperationHandler(
    "IsDifferentialOneFormOneVariable",
    CalculatorFunctions::innerIsDifferentialOneFormOneVariable,
    "",
    "Tests whether the expression is a differential form in one variable.  ",
    "IsDifferentialOneFormOneVariable(Differential x );\n"
    "IsDifferentialOneFormOneVariable({\\text{d}} x );\n"
    "IsDifferentialOneFormOneVariable(x {\\text{d}} y );\n"
    "IsDifferentialOneFormOneVariable(\\frac{\\text{d} y}{y} );\n"
    "IsDifferentialOneFormOneVariable(1/(\\text{d} y) );",
    "CalculatorFunctions::innerIsDifferentialOneFormOneVariable",
    "IsDifferentialOneFormOneVariable",
    innerStandard
  );
  this->AddOperationHandler(
    "Polynomialize",
    CalculatorFunctions::outerPolynomializE,
    "",
    "Polynomialize(a) is equivalent to MakeExpression(Polynomial(a)).",
    "C = (c a + a b +b c + 1 )^3;\n"
    "A = Polynomialize(C); "
    "B=MakeExpression(Polynomial(C)); "
    "A-B",
    "CalculatorFunctions::outerPolynomializE",
    "Polynomialize",
    innerStandard
  );
  this->AddOperationHandler(
    "IsAlgebraicRadical",
    CalculatorFunctions::innerIsAlgebraicRadical,
    "",
    "Tests whether the expression is an algebraic expression "
    "obtained using radicals and the four arithmetic operations.  ",
    "IsAlgebraicRadical(\\sqrt{5+\\sqrt{2}}); "
    "IsAlgebraicRadical(\\sqrt{x}); "
    "IsAlgebraicRadical(\\sqrt{\\pi +e + 1})",
    "CalculatorFunctions::innerIsAlgebraicRadical",
    "IsAlgebraicRadical",
    innerStandard
  );
  this->AddOperationHandler(
    "IsReal",
    CalculatorFunctions::innerIsReal,
    "",
    "Tests whether the expression is a real constant.  ",
    "IsReal(\\sqrt{5+\\sqrt{- 1}}); IsReal(\\sqrt{\\sqrt{5}- 1});\n"
    "IsReal(sqrt(\\sqrt{\\pi}-2) )",
    "CalculatorFunctions::innerIsReal",
    "IsReal",
    innerStandard
  );
  this->AddOperationHandler(
    "IsConstant",
    CalculatorFunctions::innerIsConstant,
    "",
    "Tests whether the expression is a constant.  ",
    "IsConstant(\\pi^2); IsConstant(1);IsConstant(x);\n"
    "IsConstant(e^{\\sin(\\pi^2+e +\\sqrt{2}+3)}  ); ",
    "CalculatorFunctions::innerIsConstant",
    "IsConstant",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateDefiniteIntegral,
    "",
    "Integrates a definite integral.  ",
    "\\int_{1}^2 (x^2+x) dx ",
    "CalculatorFunctions::innerIntegrateDefiniteIntegral",
    "DefiniteIntegral",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegratePowerByUncoveringParenthesisFirst,
    "",
    "Attempts to rearrange into standard polynomial form and then integrate.  ",
    "\\int  \\left( \\frac{x(x + 1) }{ 2} \\right)^2 dx ",
    "CalculatorFunctions::innerIntegratePowerByUncoveringParenthesisFirst",
    "IntegrateAfterPolynomialization",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateXnDiffX,
    "",
    "Integrates x^n dx. ",
    "\\int x dx ",
    "CalculatorFunctions::innerIntegrateXnDiffX",
    "IntegratePowerRule",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateEpowerAxDiffX,
    "",
    "If a is a number, integrates e^{a x} dx. ",
    "\\int x dx ",
    "CalculatorFunctions::innerIntegrateEpowerAxDiffX",
    "IntegrateEpowerX",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegratePullConstant,
    "",
    "Pulls out constants from integrals  ",
    "\\int 2\\sqrt{2-x^2} dx ",
    "CalculatorFunctions::innerIntegratePullConstant",
    "IntegratePullConstant",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateSqrtOneMinusXsquared,
    "",
    "Integrates \\int \\sqrt{a-x^2}dx, a > 0.",
    "\\int 2\\sqrt{2-x^2} dx ",
    "CalculatorFunctions::innerIntegrateSqrtOneminusXsquared",
    "IntegrateSqrtOneminusXsquared",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateSqrtXsquaredMinusOne,
    "",
    "Integrates \\int \\sqrt{x^2-a}dx, a > 0.  ",
    "\\int 2\\sqrt{3x^2-5} dx ",
    "CalculatorFunctions::innerIntegrateSqrtXsquaredMinusOne",
    "IntegrateSqrtXsquaredMinusOne",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateXpowerNePowerAx,
    "",
    "Integrates by parts \\int x^n e^{a x} dx, where n is a positive integer.  ",
    "\\int 2 x^{3} e^{5x} dx ",
    "CalculatorFunctions::innerIntegrateXpowerNePowerAx",
    "IntegrateXpowerNePowerAx",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerEulerFlaAsALaw,
    "",
    "Trigonometrizes an exponential expression using Euler's formula.  ",
    "TurnOnRules(\"EulerFormula\"); e^{i x}",
    "CalculatorFunctions::innerEulerFlaAsALaw",
    "EulerFormula",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "+",
    CalculatorFunctions::innerTrigSumToTrigProduct,
    "",
    "Implementation of sum-to-product trig rules. ",
    "TurnOnRules(\"TrigSumToTrigProduct\");\n"
    "sin(a) +sin(b); sin(b)-sin(a);\n"
    "-sin(b) +sin(a); -sin(b)-sin(a);\n"
    "cos(a) +cos(b); cos(a)-cos(b); -cos(a)-cos(b)",
    "CalculatorFunctions::innerTrigSumToTrigProduct",
    "TrigSumToTrigProduct",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\sin",
    CalculatorFunctions::innerSineOfAngleSumToTrig,
    "",
    "Implementation of sin (a + b) = sin(a) cos(b) + cos(a) sin(b).",
    "TurnOnRules(\"SineOfAngleSum\"); \\sin(a + b)",
    "CalculatorFunctions::innerSineOfAngleSumToTrig",
    "SineOfAngleSum",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\cos",
    CalculatorFunctions::innerCosineOfAngleSumToTrig,
    "",
    "Implementation of cos (a +b)= cos(a)cos(b)-sin(a)sin(b).",
    "TurnOnRules(\"CosineOfAngleSum\"); \\cos(a +b)",
    "CalculatorFunctions::innerCosineOfAngleSumToTrig",
    "CosineOfAngleSum",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\sin",
    CalculatorFunctions::innerConvertSinToExponent,
    "",
    "Converts sine to exponent by sin(x) = (e^{i x}-e^{-i x}) / (2i) ",
    "TurnOnRules(\"ExpressSinViaExponent\", \"ExpressCosViaExponent\");\n"
    "\\sin x;\n"
    "cos x",
    "CalculatorFunctions::innerConvertTrigToExponent",
    "ExpressSinViaExponent",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\cos",
    CalculatorFunctions::innerConvertCosToExponent,
    "",
    "Converts cosine to exponent by cos(x) = (e^{i x}+e^{-i x})/(2) ",
    "TurnOnRules(\"ExpressSinViaExponent\", \"ExpressCosViaExponent\");\n"
    "\\sin x;\n"
    "cos x",
    "CalculatorFunctions::innerConvertTrigToExponent",
    "ExpressCosViaExponent",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\cos",
    CalculatorFunctions::innerExploitCosEvenness,
    "",
    "If a is negative, converts cos (a) to cos(- a).",
    "TurnOnRules(\"ExploitCosEvenness\"); cos(-5x)",
    "CalculatorFunctions::innerExploitCosEvenness",
    "ExploitCosEvenness",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\sin",
    CalculatorFunctions::innerExploitSinOddness,
    "",
    "If a is negative, converts sin (a) to -sin(-a).",
    "TurnOnRules(\"ExploitSinOddness\"); sin(-5x)",
    "CalculatorFunctions::innerExploitSinOddness",
    "ExploitSinOddness",
    innerStandardOffByDefault
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateSinPowerNCosPowerM,
    "",
    "Solves/transforms an integral of the form \\int \\sin^n x \\cos^m x. ",
    "\\int \\sin^{4}x \\cos^{6}x dx ",
    "CalculatorFunctions::innerIntegrateSinPowerNCosPowerM",
    "IntegrateSinPowerNCosPowerM",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateTanPowerNSecPowerM,
    "",
    "Transforms an integral of the form \\int \\tan^n x \\sec^m x.  ",
    "TurnOffRules(\"ExpressTanViaSineCosine\", \"ExpressSecViaCosine\"); \\int \\tan^2 x \\sec^{2}x dx ",
    "CalculatorFunctions::innerIntegrateTanPowerNSecPowerM",
    "IntegrateTanPowerNSecPowerM",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateSum,
    "",
    "If the integral is of the form \\int (A+B )dx, tries recursively to integrate A and B. "
    "If successful, integrates the sum in the obvious way.",
    "\\int (x + 1+\\sqrt{}2) dx ",
    "CalculatorFunctions::innerIntegrateSum",
    "IntegrateSum",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegratePullImaginaryUnit,
    "",
    "Pulls out the imaginary unit in front of the integral. ",
    "\\int i 3 e^{i x} dx ",
    "CalculatorFunctions::innerIntegratePullImaginaryUnit",
    "IntegratePullOutImaginaryUnit",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateSqrt,
    "",
    "Differentiation - square root function. ",
    "d/dx(sqrt(x));",
    "CalculatorFunctions::innerDifferentiateSqrt",
    "DifferentiateSqrt",
    innerStandard
  );
  //the function integrating the building blocks must come in the exact order below:
  //else we risk infinite substitution cycle. The reasons are implementation-specific.
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIa,
    "",
    "Integrates building block Ia.  ",
    "\\int  (\\frac{3}{(x/2- 1)} ) dx ",
    "CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIa",
    "IntegrateBlockIa",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIb,
    "",
    "Integrates building block Ib.  ",
    "\\int  (\\frac{2}{(3x- 1)^2} ) dx ",
    "CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIb",
    "IntegrateBlockIb",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa,
    "",
    "Integrates building blocks IIa and IIIa.  ",
    "\\int  (\\frac{3x +2}{x^2+x + 1} ) dx ",
    "CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa",
    "IntegrateBlockIIaIIIa",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIIb,
    "",
    "Integrates building blocks IIIb.  ",
    "\\int  (\\frac{3x +2}{(x^2+x + 1)^2} ) dx ",
    "CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIIb",
    "IntegrateBlockIIIb",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIb,
    "",
    "Integrates building blocks IIb.  ",
    "\\int  (\\frac{3x +2}{(x^2+x + 1)^2} ) dx ",
    "CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIb",
    "IntegrateBlockIIb",
    innerStandard
  );
  this->AddOperationHandler(
    "\\int",
    CalculatorFunctions::innerIntegrateRationalFunctionSplitToBuidingBlocks,
    "",
    "Attempts to split an integral of a rational function into building block integrals. ",
    "\\int \\frac{1}{x(x + 1)} dx ",
    "CalculatorFunctions::innerIntegrateRationalFunctionSplitToBuidingBlocks",
    "IntegratePartialFractions",
    innerStandard
  );

  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateTrigAndInverseTrig,
    "",
    "Differentiation - product rule. ",
    "Differentiate(x,  (\\sin x) \\cos x )",
    "CalculatorFunctions::innerDifferentiateTrigAndInverseTrig",
    "DifferentiateTrigAndInverseTrig",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateChainRule,
    "",
    "Differentiation - chain rule.  ",
    "Differentiate(x,  \\sin x^2 \\cos (\\sin x))",
    "CalculatorFunctions::innerDifferentiateChainRule",
    "DifferentiateChainRule",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateAtimesB,
    "",
    "Differentiation - product rule.  ",
    "Differentiate(x, f*g )",
    "CalculatorFunctions::innerDifferentiateAtimesB",
    "DifferentiateProductRule",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateAdivideBCommutative,
    "",
    "Differentiation - division rule, commutative. "
    "For the time being differentiation is assumed to be over commutative rings. "
    "This might change in the future. "
    "The commutative division rule is "
    "d/dx (f/g^n) = ((d/dx f) g- n f (d/dx g))/g^{n + 1}. ",
    "Differentiate(x, f/g ); Differentiate(x, f/g^5 ); Differentiate(x, f/g^n )",
    "CalculatorFunctions::innerDifferentiateAdivideBCommutative",
    "DifferentiateQuotientRule",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateAplusB,
    "",
    "Differentiation - sum rule.  ",
    "Differentiate(x, f + g)",
    "CalculatorFunctions::innerDifferentiateAplusB",
    "DifferentiateSum",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateConstant,
    "",
    "Differentiation - constant rule. ",
    "Differentiate(x, 5 ); d/dx \\sqrt{5}",
    "CalculatorFunctions::innerDifferentiateConstant",
    "DifferentiateConstant",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateX,
    "",
    "Differentiation - d/dx x = 1.",
    "Differentiate(x, x ); d/dx x",
    "CalculatorFunctions::innerDifferentiateX",
    "DifferentiateIdentity",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateConstPower,
    "",
    "Differentiation - d/dx x^n = n x^{n- 1}.",
    "Differentiate(x, x^2 ); d/dx x^- 1; d/dt t^{\\sqrt{2}}; d/dc c^{DoubleValue 3.14}",
    "CalculatorFunctions::innerDifferentiateConstPower",
    "DifferentiateConstPower",
    innerStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerDifferentiateAPowerB,
    "",
    "Differentiation - d/dx a^b= d/dx(e^{b\\log a}) = a^b d/dx(b\\log a) .",
    "Differentiate(x, x^2 ); d/dx x^- 1; d/dt t^{\\sqrt{2}}; d/dc c^{DoubleValue 3.14}",
    "CalculatorFunctions::innerDifferentiateAPowerB",
    "DifferentiateApowerB",
    innerStandard
  );
  this->AddOperationHandler(
    "\\times",
    CalculatorFunctions::innerCrossProduct,
    "",
    "Cross product in IIId. Returns error if given two non-3d vectors.",
    "(1,-2,3 ) \\times( 1,- 1,- 1)",
    "CalculatorFunctions::innerCrossProduct",
    "crossProduct",
    innerStandard
  );
  this->AddOperationHandler(
    "ElementWeylAlgebraDO",
    Calculator::innerElementWeylAlgebra,
    "",
    "Creates element of a Weyl algebra = polynomial coefficient differential operator. "
    "First argument denotes differential operator letter, "
    "second argument - the dual polynomial expression. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i; \n[\\partial_1, x_1]; \n\\partial_1 x_1; \nx_1\\partial_1",
    "Calculator::innerElementWeylAlgebra",
    "ElementWeylAlgebraDO",
    innerStandard
  );
  this->AddOperationHandler(
    "ElementWeylAlgebraPoly",
    Calculator::innerPolynomialWithEWA,
    "",
    "Creates a monomial from the second argument whose differential "
    "operator letter is the first argument. ",
    "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n"
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "\\partial_1 x_1",
    "Calculator::innerPolynomialWithEWA",
    "ElementWeylAlgebraPoly",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeRationalFunction",
    CalculatorConversions::innerRationalFunctioN,
    "",
    "Creates a built-in rational function.",
    "MakeRationalFunction{}(x_1+MakeRationalFunction{}x_1+x_2)",
    "CalculatorConversions::innerRationalFunctioN",
    "MakeRationalFunction",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeMatrix",
    CalculatorConversions::innerMakeMatrix,
    "",
    "Creates an internal c++ matrix structure from double list of rationals. ",
    "s_1 = MakeMatrix{}((- 1,- 1, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1));\n"
    "s_2 = MakeMatrix{}((1, 0, 0, 0), (- 1, - 1, - 1,0), (0, 0, 1, 0), (0, 0, 0, 1));\n"
    "s_3 = MakeMatrix{}((1, 0, 0, 0), (0, 1, 0, 0), (0, - 2, - 1, - 1), (0, 0, 0, 1));\n"
    "s_4 = MakeMatrix{}((1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, - 1, - 1)); ",
    "CalculatorConversions::innerMatrixRational",
    "MakeMatrix",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "FourierTransformDO",
    CalculatorFunctions::innerFourierTransformEWA,
    "",
    "Fourier-transforms an element of a Weyl algebra. "
    "Multiplies each monomial "
    "term of odd total degree by - 1 "
    "(total degree = sum of degrees in the polynomial "
    "variables plus the degrees of the differential variables. ",
    "x = ElementWeylAlgebraPoly{}(\\partial, x);\n"
    "\\partial = ElementWeylAlgebraDO{}(\\partial, x);\n"
    "a =x^3 + x\\partial; b = \\partial x + \\partial^3 + \\partial;\n"
    "[FourierTransformDO{}a,FourierTransformDO{}b]"
    "-FourierTransformDO{}[a,b]",
    "CalculatorFunctions::innerFourierTransformEWA",
    "FourierTransformDO",
    innerStandard
  );
  this->AddOperationHandler(
    "PerturbSplittingNormal",
    Calculator::innerPerturbSplittingNormal,
    "",
    "Takes 3 arguments: normal, cone and general vectors. "
    "Attempts to perturb the normal so that the cone stays on "
    "one side of the normal and so that the general vectors "
    "have as little zero scalar products with the normal as possible. ",
    "PerturbSplittingNormal{}((0,0,0,0), ("
    "(- 1, -2, -2, -2), (- 1, - 1, -2, -2), (0, - 1, -2, -2), "
    "(- 1, - 1, - 1, - 1), (0, - 1, - 1, - 1),"
    "(0, 0, - 1, - 1), (- 1, - 1, 0, 0), (0, - 1, 0, 0), "
    "(- 1, 0, 0, 0), (1, 0, 0, 0), (0, 1, 0, 0), (1, 1, 0, 0),"
    "(0, 0, 1, 1), (0, 1, 1, 1), (1, 1, 1, 1), "
    "(0, 1, 2, 2), (1, 1, 2, 2), (1, 2, 2, 2)),"
    "("
    "(-2, -3, -4, -2), (- 1, -3, -4, -2), (- 1, -2, -4, -2), "
    "(- 1, -2, -3, -2), (- 1, -2, -2, -2), (- 1, -2, -3, - 1),"
    "(- 1, - 1, -2, -2), (- 1, -2, -2, - 1), (0, - 1, -2, -2), "
    "(- 1, - 1, -2, - 1), (- 1, -2, -2, 0),"
    "(0, - 1, -2, - 1), (- 1, - 1, - 1, - 1), (- 1, - 1, -2, 0), "
    "(0, - 1, - 1, - 1), (0, - 1, -2, 0), (- 1, - 1, - 1, 0),"
    "(0, 0, - 1, - 1), (0, - 1, - 1, 0), (- 1, - 1, 0, 0), "
    "(0, 0, 0, - 1), (0, 0, - 1, 0), (0, - 1, 0, 0), (- 1, 0, 0, 0),"
    "(1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1), "
    "(1, 1, 0, 0), (0, 1, 1, 0), (0, 0, 1, 1), (1, 1, 1, 0),"
    "(0, 1, 2, 0), (0, 1, 1, 1), (1, 1, 2, 0), (1, 1, 1, 1), "
    "(0, 1, 2, 1), (1, 2, 2, 0), (1, 1, 2, 1), (0, 1, 2, 2), "
    "(1, 2, 2, 1),"
    "(1, 1, 2, 2), (1, 2, 3, 1), (1, 2, 2, 2), (1, 2, 3, 2), "
    "(1, 2, 4, 2), (1, 3, 4, 2), (2, 3, 4, 2)))",
    "Calculator::innerPerturbSplittingNormal",
    "PerturbSplittingNormal",
    innerStandard
  );
  this->AddOperationHandler(
    "TestSpechtModules",
    CalculatorFunctionsWeylGroup::innerTestSpechtModules,
    "",
    "Tests all Specht modules of S_n, where n is the only argument taken by the function.",
    "TestSpechtModules(4); ",
    "CalculatorFunctionsWeylGroup::innerTestSpechtModules",
    "TestSpechtModules",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "MakeElementWeylGroup",
    CalculatorFunctionsWeylGroup::innerWeylGroupElement,
    "",
    "Needs a group name and a list of generators",
    "s_{{i}}=MakeElementWeylGroup{}(A_2, i);\n(s_1+s_2)(2s_1+s_2)(3s_1+s_2)",
    "CalculatorFunctionsWeylGroup::innerWeylGroupElement",
    "MakeElementWeylGroup",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeElementHyperOctahedral",
    CalculatorConversions::innerMakeElementHyperOctahedral,
    "",
    "Needs a group name and a list of generators",
    "s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0);\n"
    "t = MakeElementHyperOctahedral{}((1, 3), 0, 0, 0);\n"
    "s * t * s * t",
    "CalculatorConversions::innerMakeElementHyperOctahedral",
    "MakeElementHyperOctahedral",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupClassicalSignMultiplicities",
    CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystemsFromKostkaNumbers,
    "",
    "Prints the sign multiplicities of a simple Weyl group of classical type. "
    "See a common article by "
    "T. Folz-Donahue, S. Jackson, T. Milev, A. Noel. ",
    "WeylGroupClassicalSignMultiplicities{}(b_3);\n"
    "WeylGroupClassicalSignMultiplicities{}(b_4);\n"
    "WeylGroupClassicalSignMultiplicities{}(b_5);\n"
    "WeylGroupClassicalSignMultiplicities{}(d_4);\n"
    "WeylGroupClassicalSignMultiplicities{}(d_5);\n"
    "WeylGroupClassicalSignMultiplicities{}(d_6);\n"
    "WeylGroupClassicalSignMultiplicities{}(a_2);\n"
    "WeylGroupClassicalSignMultiplicities{}(a_3);\n"
    "WeylGroupClassicalSignMultiplicities{}(a_4);\n",
    "CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystemsFromKostkaNumbers",
    "WeylGroupClassicalSignMultiplicities",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupTauSignatures",
    CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems,
    "",
    "Prints the tau signatures of a Weyl group. See a common article by "
    "T. Folz-Donahue, S. Jackson, T. Milev, A. Noel. ",
    "WeylGroupTauSignatures{}(b_3);",
    "CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems",
    "WeylGroupTauSignatures",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "WeylGroupConjugacyClassesFromAllElements",
    CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements,
    "",
    "For small ranks, computes the conjugacy classes of a Weyl "
    "group by enumerating all elements of the group. ",
    "WeylGroupConjugacyClassesFromAllElements{}(A_2);",
    "CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements",
    "WeylGroupConjugacyClassesFromAllElements",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "WeylGroupOuterConjugacyClassesFromAllElements",
    CalculatorFunctionsWeylGroup::innerWeylGroupOuterConjugacyClassesFromAllElements,
    "",
    "Computes conjugacy classes, identifying classes that "
    "are conjugate using outer automorphisms. ",
    "WeylGroupOuterConjugacyClassesFromAllElements{}(D_4);",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOuterConjugacyClassesFromAllElements",
    "WeylGroupOuterConjugacyClassesFromAllElements",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "WeylGroupConjugacyClassesRepresentatives",
    CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives,
    "",
    "Computes a representative in each conjugacy classes of a "
    "Weyl group by comparing conjugacy classes invariants and by "
    "enumerating conjugacy class orbits. ",
    "WeylGroupConjugacyClassesRepresentatives{}(A_2);",
    "CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives",
    "WeylGroupConjugacyClassesRepresentatives",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "WeylGroupConjugacyClasses",
    CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS,
    "",
    "Loads the conjugacy classes of a Weyl group (hard-coded), "
    "or computes them if rank<=6. ",
    "WeylGroupConjugacyClasses{}(f_4);",
    "CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS",
    "WeylGroupConjugacyClasses",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupCharTable",
    CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable,
    "",
    "Loads the character table of a Weyl group. "
    "The character tables are hard-coded.",
    "WeylGroupCharTable{}(b_3);",
    "CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable",
    "WeylGroupCharTable",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupIrrepsAndCharTableComputeFromScratch",
    CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch,
    "",
    "<b>Work in progress. Please do not use.</b> "
    "Computes from scratch the irreducible representations "
    "and the character table of a Weyl group.",
    "WeylGroupIrrepsAndCharTableComputeFromScratch{}(b_3);",
    "CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch",
    "WeylGroupIrrepsAndCharTableComputeFromScratch",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "WeylOrbitSize",
    CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSize,
    "",
    "Computes the size of a Weyl group orbit of a weight",
    "WeylOrbitSize(E_6, (3,3,3,7,7,11)); WeylOrbitSize(E_8, (3,3,3,7,7,11,13,13));",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSize",
    "WeylOrbitSize",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylOrbitSimpleCoords",
    CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSimple,
    "",
    "Generates a Weyl orbit printout from simple coords. "
    "First argument = type. Second argument = weight in simple coords. "
    "The orbit size is cut off at max 1920 elements (type D_5).",
    "WeylOrbitSimpleCoords{}(B_2, (y, y));",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSimple",
    "WeylOrbitSimpleCoords",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupOrbitOuterSimple",
    CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple,
    "",
    "Generates a Weyl outer orbit printout from simple coords. "
    "The outer orbit is the orbit "
    "under the Weyl group extended with the outer automoprhisms of the Weyl group. "
    "First argument = type. Second argument = weight in simple coords. "
    "The orbit size is cut off at 1921*2 elements. ",
    "WeylGroupOrbitOuterSimple{}(D_4, (1,0,0,0))",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple",
    "WeylGroupOrbitOuterSimple",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylOrbitFundCoords",
    CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFund,
    "",
    "Generates a Weyl orbit printout from fundamental coords. "
    "First argument = type. Second argument = weight in fundamental coords. ",
    "WeylOrbitFundCoords{}(B_2, (y, 0));",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFund",
    "WeylOrbitFundCoords",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylOrbitFundRho",
    CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFundRho,
    "",
    "Generates a Weyl orbit printout from fundamental coords. "
    "First argument = type. Second argument = weight in fundamental coords. "
    "Doing the rho-modified action. ",
    "WeylOrbitFundRho{}(B_2, (y, 0) )",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFundRho",
    "WeylOrbitFundRho",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylRaiseToMaximallyDominant",
    CalculatorFunctionsWeylGroup::innerWeylNoOuterRaiseToMaximallyDominant,
    "",
    "Raises a set of rational vectors simultaneously to maximally dominant: "
    "it raises the first vector to dominant, then each consecutive vector is "
    "raised as much as possible without changing the preceding, "
    "alrady raised,  vectors. First argument = type. "
    "Second argument = weight in simple coords. ",
    "WeylRaiseToMaximallyDominant{}(F_4, (3, 6, 8, 4), (- 1, -3, -2, 0)  );\n"
    "WeylRaiseToMaximallyDominant{}(F_4, (3, 6, 8, 4), (0, -3, -4, -2)  );\n"
    "WeylRaiseToMaximallyDominant{}(F_4, (0, -3, -4, -2) , (3, 6, 8, 4)  );",
    "CalculatorFunctionsWeylGroup::innerWeylNoOuterRaiseToMaximallyDominant",
    "WeylRaiseToMaximallyDominant",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeWeight",
    CalculatorFunctionsWeylGroup::innerLieAlgebraWeight,
    "",
    "Makes a weight. First argument = type. "
    "Second argument = coordinate. "
    "Third argument = one of the keywords epsilon, simple, "
    "fundamental, standing for the coordinate system. ",
    "\\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon);\n"
    "\\psi_{{a}}=MakeWeight{}(B_3, a, fundamental);\n"
    "\\eta_{{a}}= MakeWeight{}(B_3, a, simple);\n"
    "\\varepsilon_1;\n"
    "\\psi_1;\n"
    "\\eta_3",
    "CalculatorFunctionsWeylGroup::innerLieAlgebraWeight",
    "MakeWeight",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeRho",
    CalculatorFunctionsWeylGroup::innerLieAlgebraRhoWeight,
    "",
    "Makes the half-sum of the positive roots.",
    "\n\\rho =MakeRho(B_3);",
    "CalculatorFunctionsWeylGroup::innerLieAlgebraRhoWeight",
    "MakeRho",
    innerStandard
  );
  this->AddOperationHandler(
    "IsOuterAuto",
    CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup,
    "",
    "Checks whether the element is outer automorphism of a root system. "
    "First argument = type. "
    "Second argument = matrix linear operator corresponding written in simple basis. ",
    "A = MakeMatrix"
    "((1, 0, 0, 0, 0, - 1),"
    "(0, 0, 0, 0, 1, -2),"
    "(0, 0, 1, 0, 0, -2),"
    "(0, 0, 0, 1, 0, -3),"
    "(0, 1, 0, 0, 0, -2),"
    "(0, 0, 0, 0, 0, - 1));"
    "IsOuterAuto{}(e_6, A);",
    "CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup",
    "IsOuterAuto",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylOuterRaiseToMaximallyDominant",
    CalculatorFunctionsWeylGroup::innerWeylOuterRaiseToMaximallyDominant,
    "",
    "Same as WeylRaiseToMaximallyDominant but uses outer Weyl group automorphisms as well.",
    "WeylOuterRaiseToMaximallyDominant{}(D_4,  (1, 0, 0, 0), (0, 0, 0, 1)  );\n"
    "WeylOuterRaiseToMaximallyDominant{}(D_4,  (0, 0, 0, 1), (1, 0, 0, 0)  );\n"
    "WeylOuterRaiseToMaximallyDominant{}(D_4,  (1, 0, 0, 0), (0, 0, 1, 0)  );\n"
    "WeylOuterRaiseToMaximallyDominant{}(D_4,  (0, 0, 1, 0), (1, 0, 0, 0)  );\n",
    "CalculatorFunctionsWeylGroup::innerWeylOuterRaiseToMaximallyDominant",
    "WeylOuterRaiseToMaximallyDominant",
    innerStandard
  );
  this->AddOperationHandler(
    "GenerateFiniteMultiplicativelyClosedSet",
    Calculator::innerGenerateMultiplicativelyClosedSet,
    "",
    "The first argument gives upper_bound to the number of elements of the set. "
    "Generates a finite multiplicatively closed set of at most upper_bound elements, "
    "or returns error indicating the multiplicatively closed "
    "set is larger than the upper bound.",
    "s_1=MakeMatrix{}((- 1 , 1 , 0),(0 , 1 , 0),(0 , 0 , 1));\n"
    "s_2=MakeMatrix{}((1 , 0 , 0),(1 , - 1 , 1),(0 , 0 , 1));\n"
    "s_3=MakeMatrix{}((1 , 0 , 0),(0 , 1 , 0),(0 , 2 , - 1));\n"
    "GenerateFiniteMultiplicativelyClosedSet{}(48, s_1, s_2, s_3);",
    "Calculator::innerGenerateMultiplicativelyClosedSet",
    "GenerateFiniteMultiplicativelyClosedSet",
    innerStandard
  );
  this->AddOperationHandler(
    "CartanSA",
    CalculatorFunctions::innerConstructCartanSA,
    "",
    "Generates a Cartan subalgebra, code is still experimental.",
    "g_{{i}}= GetChevalleyGenerator{}(b_3, i);\n"
    "h_{{i}}= GetCartanGenerator{}(b_3, i);"
    "CartanSA(g_1+g_3, g_- 1+g_-3, g_2, g_-2);",
    "CalculatorFunctions::innerConstructCartanSA",
    "CartanSA",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "GenerateVectorSpaceClosedWRTLieBracket",
    CalculatorFunctions::innerGenerateVectorSpaceClosedWRTLieBracket,
    "",
    "Generates a vector space closed with respect to the Lie bracket "
    "from input of type elements of Weyl algebra or Lie algebra. "
    "The first argument of the input is an upper bound for the dimension of the vector space. "
    "The remaining arguments must be differential operators. "
    "The output is a vector space basis printout. "
    "Fails if the dimension of the vector space is larger than the upper bound.",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= ElementWeylAlgebraPoly{}(\\partial_i, x_i); "
    "g_1= (-x_{5} \\partial_{5}^{2}-x_{4} \\partial_{4} \\partial_{5}  -x_{3} \\partial_{3} \\partial_{5} "
    "-x_{2} \\partial_{2} \\partial_{5}  +x_{1} \\partial_{2} \\partial_{4} +x_{1} \\partial_{3}^{2});\n"
    "g_- 1=x_5;\n"
    "g_2=x_{5} \\partial_{4} -x_{2} \\partial_{1} ;\n"
    "g_-2= (x_{4} \\partial_{5} -x_{1} \\partial_{2} );\n"
    "g_3= (2x_{4} \\partial_{3} -x_{3} \\partial_{2} );\n"
    "g_-3= (x_{3} \\partial_{4} -2x_{2} \\partial_{3} );\n"
    "GenerateVectorSpaceClosedWRTLieBracket{}(50, g_1, g_- 1, g_2, g_-2, g_3, g_-3);\n"
    "GenerateVectorSpaceClosedWRTLieBracket{}(50,g_1,g_2, g_-2, g_3, g_-3);\n"
    "GenerateVectorSpaceClosedWRTLieBracket{}(50,g_1, g_- 1, g_2, g_-2, g_3);\n"
    "q_{{i}}= GetChevalleyGenerator{}(F_4, i);\n"
    "s_2= -q_{-5}-q_{-6};\ns_1=q_{20}+q_{19};\n s_- 1= -q_{- 19}-q_{-20}\n; s_-2=2q_{6}+2q_{5};\n"
    "GenerateVectorSpaceClosedWRTLieBracket(52, s_1, s_2, s_- 1, s_-2);",
    "CalculatorFunctions::innerGenerateVectorSpaceClosedWRTLieBracket",
    "GenerateVectorSpaceClosedWRTLieBracket",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupNaturalRep",
    CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep,
    "",
    "Gets the natural representation of the Weyl group.",
    "WeylGroupNaturalRep{}(B_3)",
    "CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep",
    "WeylGroupNaturalRep",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylGroupGetOuterAutoGenerators",
    CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint,
    "",
    "Gets the generators of the outer automorphism group (the group generated by "
    "the graph automorphisms of the Dynkin diagram.",
    "WeylGroupGetOuterAutoGenerators{}(2D_4)",
    "CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint",
    "WeylGroupGetOuterAutoGenerators",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeVirtualWeylGroupRepresentation",
    CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep,
    "",
    "Convert a representation into virtual one.",
    "MakeVirtualWeylGroupRepresentation{}(WeylGroupNaturalRep{}(B_3))",
    "CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep",
    "MakeVirtualWeylGroupRepresentation",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "MinPolyMatrix",
    CalculatorFunctions::innerMinPolyMatrix,
    "",
    "Computes the minimal polynomial of a matrix, provided that the matrix is not too large.",
    "A = MakeMatrix{}((0, 1), (- 1, 0));\n"
    "p = MinPolyMatrix{}A",
    "CalculatorFunctions::innerMinPolyMatrix",
    "MinPolyMatrix",
    innerStandard
  );
  this->AddOperationHandler(
    "CharPoly",
    CalculatorFunctions::innerCharPolyMatrix,
    "",
    "Computes the characteristic polynomial of a matrix (= det(A-q*Id)), "
    "provided that the matrix is not too large.",
    "A =\\begin{pmatrix}2 & 3& 5\\\\ 7& 11& 13\\\\ 17&19 &23\\end{pmatrix}; p =MinPolyMatrix{}A",
    "CalculatorFunctions::innerCharPolyMatrix",
    "CharPoly",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeCharacterLieAlg",
    Calculator::innerCharacterSSLieAlgFD,
    "",
    "Creates character of a semisimple Lie algebra finite dimensional irreducible module. "
    "First argument gives type, second argument gives highest weight in fundamental coordinates.",
    "x = MakeCharacterLieAlg{}(G_2, (1,0));\n"
    "y = MakeCharacterLieAlg{}(G_2, (0,1));\n"
    "x * y",
    "Calculator::innerCharacterSSLieAlgFD",
    "MakeCharacterLieAlg",
    innerStandard
  );
  this->AddOperationHandler(
    "GetLinks",
    Calculator::innerGetLinksToSimpleLieAlgerbas,
    "",
    "Gets simple Lie algebra links to the calculator.",
    "GetLinks{}0",
    "Calculator::innerGetLinksToSimpleLieAlgerbas",
    "GetLinks",
    innerInvisible
  );
  this->AddOperationHandler(
    "ConesIntersection",
    Calculator::innerConesIntersect,
    "",
    "Takes as input two sequences of vectors, generates two cones, "
    "and intersects them using the simplex algorithm. "
    "The output is a string report of the operation. "
    "The first cone is generated over Z_{&gt;} (``strict cone'') "
    "the second cone is generated over Z_{&gt;= 0} (``non-strict cone'').",
    "v_1= (1, 2, 3 ); v_2= (1, 3, 2); v_3= (3,1,1); v_4= (-2,2, 2);\n"
    "ConesIntersection{}((v_1,v_2 ), (v_3,v_4 ));\nConesIntersection{}((v_1,v_2 ), (v_3,-v_4 ));",
    "Calculator::innerConesIntersect",
    "ConesIntersection",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintRuleStack",
    CalculatorFunctions::innerPrintRuleStack,
    "",
    "Prints the current rule stack",
    "a = b;\n"
    "PrintRuleStack{}0;\n"
    "c = d; PrintRuleStack{}0; ",
    "CalculatorFunctions::innerPrintRuleStack",
    "PrintRuleStack",
    innerStandard
  );
  this->AddOperationHandler(
    "Crawl",
    CalculatorFunctions::innerCrawlTexFile,
    "",
    "Crawls a latex file collecting all local style files and all \\input "
    "commands to produce a single latex file. The function was "
    "originally designed for the purposes of the freecalc project "
    "as not been tested on any other projects, please use only for freecalc.",
    "Crawl(\"freecalc/homework/referenceallproblemsbycourse/calculusimasterproblemsheet.tex\")",
    "CalculatorFunctions::innerCrawlTexFile",
    "Crawl",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "BuildFreecalc",
    CalculatorFunctions::innerBuildFreecalc,
    "",
    "Builds the freecalc lecture pdfs. "
    "This function performs file operations and is allowed only to logged-in administrator accounts. "
    "Takes as arguments the lecture folder (within the freecalc project) and the file name. "
    "The function assumes the freecalc project is installed in a folder called freecalc, "
    "parallel to the calculator project folder "
    "(i.e, we have folders /vectorpartition and /freecalc next to one another). "
    "The folders are given relative to the /freecalc base.",
    "BuildFreecalc (\"freecalc/lectures/referencelectures/calculusi.tex\");\n"
    "BuildFreecalc(\"freecalc/lectures/referencelectures/calculusii.tex\");\n"
    "BuildFreecalc(\"freecalc/lectures/referencelectures/calculusiiimultivariable.tex\");\n"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/calculusimasterproblemsheet.tex\");\n"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/calculusiimasterproblemsheet.tex\");"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/calculusiiimasterproblemsheet.tex\");",
    "CalculatorFunctions::innerBuildFreecalc",
    "BuildFreecalc",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "BuildFreecalcWithSlides",
    CalculatorFunctions::innerBuildFreecalcSingleSlides,
    "",
    "Same as BuildFreeCalc but attempts to also build individual slides. ",
    "BuildFreecalcWithSlides (\"freecalc/lectures/referenceallproblemsbycourse/calculusimasterproblemsheet.tex\");",
    "CalculatorFunctions::BuildFreecalcWithSlides",
    "BuildFreecalcWithSlides",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "BuildSlidesInTopicList",
    CalculatorFunctions::innerBuildFreecalcSlidesOnTopic,
    "",
    "Builds all slides in the current topic list. Available to logged-in admins only. ",
    "BuildSlidesInTopicList (0);",
    "CalculatorFunctions::innerBuildFreecalcSlidesOnTopic",
    "BuildSlidesInTopicList",
    innerAdminNoTest
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "\\log",
    CalculatorFunctions::innerLog,
    "",
    "Logarithm function. Gives a decimal approximation of the natural "
    "logarithm provided the input is a double number. ",
    "\\log{}(e); \\log 10",
    "CalculatorFunctions::innerLog",
    "\\log",
    innerStandard
  );
  this->AddOperationHandler(
    "LogBase",
    CalculatorFunctions::innerLogBaseNaturalToLn,
    "",
    "Converts \\log_e A to \\ln A. ",
    "\\log_e A",
    "CalculatorFunctions::innerLogBaseNaturalToLn",
    "LogBaseNaturalToLn",
    innerStandard
  );
  this->AddOperationHandler(
    "LogBase",
    CalculatorFunctions::innerLogBaseSimpleCases,
    "",
    "If the argument is a power of the base does the computation. ",
    "\\log_5 25; log_5 1; log_5 (1/5)",
    "CalculatorFunctions::innerLogBaseSimpleCases",
    "LogBaseSimpleCases",
    innerStandard
  );
  this->AddOperationHandler(
    "LogBase",
    CalculatorFunctions::innerLogBase,
    "",
    "Logarithm function (arbitrary base). Equivalent to "
    "\\log_{{a}} {{b}}= (\\log b)/(\\log a). ",
    "\\log_10 5; \\log _e 10; \\ln 10; \\log_a b",
    "CalculatorFunctions::innerLogBase",
    "LogBase",
    innerStandard
  );
  this->AddOperationHandler(
    "\\arctan",
    CalculatorFunctions::innerArctan,
    "",
    "Arctan function. Tries to evaluate the arctan function. ",
    "\\arctan(3/4)",
    "CalculatorFunctions::innerArctan",
    "\\arctan",
    innerStandard
  );
  this->AddOperationHandler(
    "\\arcsin",
    CalculatorFunctions::innerArcsinAlgebraic,
    "",
    "Arcsin function for special angles. ",
    "\\arcsin(1/2)",
    "CalculatorFunctions::innerArcsinAlgebraic",
    "arcsinAlgebraic",
    innerStandard
  );
  this->AddOperationHandler(
    "\\arccos",
    CalculatorFunctions::innerArccosAlgebraic,
    "",
    "Arccos function for special angles. ",
    "\\arccos(\\sqrt{2}/2)",
    "CalculatorFunctions::innerArccosAlgebraic",
    "arccosAlgebraic",
    innerStandard
  );
  this->AddOperationHandler(
    "\\arccos",
    CalculatorFunctions::innerArccos,
    "",
    "Arccos function. Tries to evaluate the arccos function. ",
    "\\arccos(3/4)",
    "CalculatorFunctions::innerArccos",
    "\\arccos",
    innerStandard
  );
  this->AddOperationHandler(
    "\\arcsin",
    CalculatorFunctions::innerArcsin,
    "",
    "Arcsin function. Tries to evaluate the arcsin function. ",
    "\\arcsin(3/4)",
    "CalculatorFunctions::innerArcsin",
    "\\arcsin",
    innerStandard
  );
  this->AddOperationHandler(
    "|",
    CalculatorFunctions::innerAbs,
    "",
    "Absolute value function ",
    "|- 1|",
    "CalculatorFunctions::innerAbs",
    "AbsoluteValue",
    innerStandard
  );
  this->AddOperationHandler(
    "\\sin",
    CalculatorFunctions::innerSin,
    "",
    "Sine function. Evaluates to a decimal approximation if the input is a double number. ",
    "\\sin{}(3.1415)",
    "CalculatorFunctions::innerSin",
    "Sine",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cos",
    CalculatorFunctions::innerCos,
    "",
    "Cosine function. Evaluates to a decimal approximation if the input is a double number. ",
    "\\cos{}(3.1415)",
    "CalculatorFunctions::innerCos",
    "Cosine",
    innerStandard
  );
  this->AddOperationHandler(
    "\\tan",
    CalculatorFunctions::innerTan,
    "",
    "Tangent function. Substitutes \\tan{}{{x}}=\\sin{}x/\\cos x. ",
    "\\tan{}(3.1415); \\tan 1.570796327",
    "CalculatorFunctions::innerTan",
    "ExpressTanViaSineCosine",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cot",
    CalculatorFunctions::innerCot,
    "",
    "Tangent function. Substitutes \\tan{}{{x}}=\\cos{}x/\\sin x. ",
    "\\cot{}(3.1415); \\cot 1.570796327",
    "CalculatorFunctions::innerCot",
    "ExpressCotViaSineCosine",
    innerStandard
  );
  this->AddOperationHandler(
    "\\csc",
    CalculatorFunctions::innerCsc,
    "",
    "Cosecant function. Substitutes \\csc{}{{x}}=1/\\sin x. ",
    "\\csc{}(3.1415); \\csc 1.570796327",
    "CalculatorFunctions::innerCsc",
    "ExpressCscViaSine",
    innerStandard
  );
  this->AddOperationHandler(
    "\\sec",
    CalculatorFunctions::innerSec,
    "",
    "Secant function. Substitutes \\sec{}{{x}}=1/\\sec x. ",
    "\\sec{}(3.1415); \\sec 1.570796327",
    "CalculatorFunctions::innerSec",
    "ExpressSecViaCosine",
    innerStandard
  );
  this->AddOperationHandler(
    "Floor",
    CalculatorFunctions::innerFloor,
    "",
    "Floor function.",
    "Floor(3/7)",
    "CalculatorFunctions::innerFloor",
    "Floor",
    innerStandard
  );
  this->AddOperationHandler(
    "Round",
    CalculatorFunctions::innerRound,
    "",
    "Round function.",
    "Round(3/7)",
    "CalculatorFunctions::innerRound",
    "Round",
    innerStandard
  );
  this->AddOperationHandler(
    "DoubleValue",
    CalculatorFunctions::innerEvaluateToDoublE,
    "",
    "Double value of a rational number.",
    "DoubleValue{}(3/7)",
    "CalculatorFunctions::innerEvaluateToDoublE",
    "DoubleValue",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerDegreesToRadians,
    "",
    "Converts degrees to radians",
    "30^\\circ",
    "CalculatorFunctions::innerDegreesToRadians",
    "DegreesToRadians",
    innerStandard
  );
  this->AddOperationHandler(
    "ModP",
    Calculator::innerZmodP,
    "",
    "Number modulo P. First argument = number, second argument = modulo.",
    "ModP{}(7, 3)",
    "Calculator::innerZmodP",
    "ModP",
    innerStandard
  );
  this->AddOperationHandler(
    "AdCommonEigenspace",
    Calculator::innerAdCommonEigenSpaces,
    "",
    "Computes common eigenspace of the adjoint action of semisimple Lie "
    "algebra elements inside the semisimple Lie algebra. ",
    "AdCommonEigenspace{}(F_4, -5 (g_{9}) +3 (g_{13}) +5 (g_{16}) +4 (g_{10}), g_{14}+g_{22}+g_{20})",
    "Calculator::innerAdCommonEigenSpaces",
    "AdCommonEigenspace",
    innerStandard
  );
  this->AddOperationHandler(
    "AttemptExtendingEtoHEFwithHinCartan",
    Calculator::innerAttemptExtendingEtoHEFwithHinCartan,
    "",
    "Attempts to embed an element E into an sl(2)-triple over "
    "the rationals, such that the element H is in the "
    "ambient Cartan algebra. ",
    "AttemptExtendingEtoHEFwithHinCartan{}(F_4, g_1+2g_2+3g_3+4g_4)",
    "Calculator::innerAttemptExtendingEtoHEFwithHinCartan",
    "AttemptExtendingEtoHEFwithHinCartan",
    innerStandard
  );
  this->AddOperationHandler(
    "IsPrimeMillerRabin",
    CalculatorFunctions::innerIsPrimeMillerRabin,
    "",
    "Checks whether the number is prime by the Miller-Rabin test. ",
    "A =100!+ 1; IsPrimeMillerRabin(A);\n"
    "IsPrimeMillerRabin(4256233);\n"
    "IsPrimeMillerRabin(49979687);\n"
    "IsPrimeMillerRabin(4256233 * 49979687)",
    "CalculatorFunctions::innerIsPrimeMillerRabin",
    "IsPrimeMillerRabin",
    innerStandard
  );
  this->AddOperationHandler(
    "IsPossiblyPrime",
    CalculatorFunctions::innerIsPossiblyPrime,
    "",
    "Checks whether the number is "
    "prime by trial division first "
    "and by the Miller-Rabin test next.",
    "A =100!+ 1;\n"
    "IsPrimeMillerRabin(A);\n"
    "IsPossiblyPrime(4256233);\n"
    "IsPossiblyPrime(49979687);\n"
    "IsPossiblyPrime(4256233 * 49979687)",
    "CalculatorFunctions::innerIsPossiblyPrime",
    "IsPossiblyPrime",
    innerStandard
  );
  this->AddOperationHandler(
    "GCD",
    CalculatorFunctions::innerGCD,
    "",
    "Greatest common divisor of two integers.",
    "GCD(100!+ 1, 101 * 103)",
    "CalculatorFunctions::innerGCD",
    "GCD",
    innerStandard
  );
  this->AddOperationHandler(
    "LCM",
    CalculatorFunctions::innerLCM,
    "",
    "Least common multiple of two integers.",
    "LCM(91, 1001)",
    "CalculatorFunctions::innerLCM",
    "LCM",
    innerStandard
  );
  this->AddOperationHandler(
    "GCDPoly",
    Calculator::innerGCDPoly,
    "",
    "Greatest common divisor polynomial of two polynomials. "
    "The divisor is scaled so that all coefficients are relatively prime integers, "
    "and so that the leading monomial under the graded lexicographic order (x_2>x_1, etc.) "
    "has positive coefficient.",
    "GCDPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+ "
    "2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+ "
    "x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+ "
    "x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2}, "
    "x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}- "
    "4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}- "
    "2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}- "
    "x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}  )",
    "Calculator::innerGCDPoly",
    "GCDPoly",
    innerStandard
  );
  this->AddOperationHandler(
    "LCMPoly",
    Calculator::innerLCMPoly,
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
    "Calculator::innerLCMPoly",
    "LCMPoly",
    innerStandard
  );
  this->AddOperationHandler(
    "Interpolate",
    Calculator::innerInterpolatePoly,
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
  this->AddOperationHandler(
    "PolyDivRemainder",
    CalculatorFunctions::innerPolynomialDivisionRemainder,
    "",
    "Returns the remainder after taking quotient of a "
    "polynomial divided by a set of polynomials "
    "using the default monomial order (lexicographic).",
    "PolyDivRemainder{}(x^7+6x y+5x y^8+y^5, x +y^2- 1, y^3-x y);",
    "CalculatorFunctions::innerPolynomialDivisionRemainder",
    "PolyDivRemainder",
    innerStandard
  );
  this->AddOperationHandler(
    "PolyDivQuotient",
    CalculatorFunctions::innerPolynomialDivisionQuotient,
    "",
    "Returns the quotients of a "
    "polynomial divided by a set of polynomials using the default monomial order (lexicographic).",
    "PolyDivQuotient{}(x^7+6x y+5x y^8+y^5, x +y^2- 1, y^3-x y) ;",
    "CalculatorFunctions::innerPolynomialDivisionRemainder",
    "PolyDivQuotient",
    innerStandard
  );
  this->AddOperationHandler(
    "PolyDivSlidesGrLex",
    CalculatorFunctions::innerPolynomialDivisionSlidesGrLex,
    "",
    "Creates a slide with the polynomial disivion algorithm. "
    "First element = starting slide number.",
    "PolyDivSlidesGrLex{}(1, x^3 + x + 10, x +2) ; PolyDivSlidesGrLex{}(1,x + y + 10, x + 1, y - 1) ",
    "Calculator::innerPolynomialDivisionSlidesGrLex",
    "PolyDivSlidesGrLex",
    innerStandard
  );
  this->AddOperationHandler(
    "PolyDivStringGrLex",
    CalculatorFunctions::innerPolynomialDivisionVerboseGrLex,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using gr lex order, for example, x^2 y^3 >x y^4, y^11>x^10. ",
    "PolyDivStringGrLex{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1) ;",
    "CalculatorFunctions::innerPolynomialDivisionVerboseGrLex",
    "PolyDivStringGrLex",
    innerStandard
  );
  this->AddOperationHandler(
    "PolyDivStringGrLexRev",
    CalculatorFunctions::innerPolynomialDivisionVerboseGrLexRev,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using total degree (graded) order with "
    "equal total degrees compared by lex order with reversed order of the letters, "
    "for example, x y^4> x^2 y^3 , x^11>y^10. ",
    "PolyDivStringGrLexRev{}(x^7+6x y+5x y^8+y^5, x^2+2, y^3- 1) ;",
    "CalculatorFunctions::innerPolynomialDivisionVerboseGrLexRev",
    "PolyDivStringGrLexRev",
    innerStandard
  );
  this->AddOperationHandler(
    "PolyDivStringLex",
    CalculatorFunctions::innerPolynomialDivisionVerboseLex,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using the lex order, for example, x^2 y^4 > x y^1000 > x y^2. ",
    "PolyDivStringLex{}(x^7 + 6 x y + 5x y^8 + y^5, x^2 + 2, y^3 - 1);",
    "CalculatorFunctions::innerPolynomialDivisionVerboseLex",
    "PolyDivStringLex",
    innerStandard
  );
  this->AddOperationHandler(
    "PolyDivStringLexRev",
    CalculatorFunctions::innerPolynomialDivisionVerboseLexRev,
    "",
    "Prints a string representing division of "
    "a polynomial by a set of polynomials using the lex "
    "order with reversed order of variables, "
    "for example, y^2 > x^1000 y > x y. ",
    "PolyDivStringLexRev{}(x^7 + 6x y + 5x y^8 + y^5, x^2 + 2, y^3- 1) ;",
    "CalculatorFunctions::innerPolynomialDivisionVerboseLexRev",
    "PolyDivStringLexRev",
    innerStandard
  );
  this->AddOperationHandler(
    "SuffixNotationForPostScript",
    Calculator::innerSuffixNotationForPostScript,
    "",
    "Suffix notation. for postscript, used to quickly generate pstricks drawings in LaTeX.",
    "suffixNotationForPostScript{}((1/3 + a + b) * c)",
    "Calculator::innerSuffixNotationForPostScript",
    "SuffixNotationForPostScript",
    innerStandard
  );
  this->AddOperationHandler(
    "MakeJavascriptExpression",
    CalculatorFunctions::innerMakeJavascriptExpressioN,
    "",
    "Attempts to construct a javascript translation of the input. "
    "If not successful leaves the expression unchanged.",
    "MakeJavascriptExpression(a(b+c))",
    "Calculator::innerMakeJavascriptExpressioN",
    "MakeJavascriptExpression",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "FetchKnownPublicKeys",
    CalculatorFunctions::innerFetchKnownPublicKeys,
    "",
    "Updates known public keys. Requires administrator privileges. "
    "At the moment, works for google public keys only. ",
    "FetchKnownPublicKeys(0)",
    "CalculatorFunctions::innerFetchKnownPublicKeys",
    "FetchKnownPublicKeys",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "PlotDirectionField",
    CalculatorFunctions::innerPlotDirectionField,
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
    "CalculatorFunctions::innerPlotDirectionField",
    "PlotDirectionField",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotPolar",
    CalculatorFunctions::innerPlotPolarRfunctionTheta,
    "",
    "<b>Calculus teaching function.</b> Draws polar curve given in "
    "polar coordinates  in the form r = f(t), where t is the angle variable. "
    "The angle variable is measured in degrees. "
    "The first argument gives the function, the second and third argument "
    "give the upper and lower bounds of the angle. ",
    "PlotPolar(1+sin  t, 0, \\pi);\n"
    "PlotPolar((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t) )"
    "(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\\pi)",
    "CalculatorFunctions::innerPlotPolarRfunctionTheta",
    "PlotPolar",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotPolarExtended",
    CalculatorFunctions::innerPlotPolarRfunctionThetaExtended,
    "",
    "<b>Calculus teaching function.</b> Same as PlotPolar "
    "but also produces a graph in the (rho,theta)-plane. ",
    "PlotPolarExtended(1+sin  t, 0, \\pi);\n"
    "PlotPolarExtended((1 + 9/10 cos(8 t) ) (1 + 1/10 cos (24 t) )\n"
    "(9/10 + 5/100 cos (200 t)) (1 + sin t), 0, 2\\pi)",
    "CalculatorFunctions::innerPlotPolarRfunctionThetaExtended",
    "PlotPolarExtended",
    innerStandard
  );
  this->AddOperationHandler(
    "GaussianElimination",
    CalculatorFunctions::innerGaussianEliminationMatrix,
    "",
    "Gaussian elimination of a matrix. Prints a detailed string "
    "that shows the Gaussian elimination of a matrix.",
    "GaussianElimination(\\begin{array}{cccccc} "
    "sqrt 1 & sqrt 2 & sqrt 3 & 1 & 0& 0\\\\ "
    "sqrt 4 & sqrt 5 & sqrt 6 & 0 &1&0\\\\ "
    "sqrt 7 & sqrt 8 & sqrt 9 & 0 & 0 & 1 \\end{array})",
    "CalculatorFunctions::innerGaussianEliminationMatrix",
    "GaussianElimination",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "EqualityToArithmeticExpression",
    CalculatorFunctions::innerEqualityToArithmeticExpressioN,
    "",
    "Transforms the equality a = b to the arithmetic expression a - b.",
    "EqualityToArithmeticExpression(a = b)",
    "CalculatorFunctions::innerEqualityToArithmeticExpression",
    "EqualityToArithmeticExpression",
    innerStandard
  );
  this->AddOperationHandler(
    "PointsImplicitly",
    CalculatorFunctions::innerGetPointsImplicitly,
    "",
    "Returns points on or close to the curve in two dimensions. "
    "Same as plotImplicit but rather than plotting "
    "the curve returns the (x,y) pairs of points found. "
    "The output given as a nx2 matrix of floating point numbers. ",
    "PointsImplicitly((x - 1) (y - 1) - ((x - 1) ^ 2 (y - 1) + 1) ^ 2, (- 2, - 2), (2, 2), (10, 10), (400, 400))",
    "CalculatorFunctions::innerGetPointsImplicitly",
    "PointsImplicitly",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotImplicit",
    CalculatorFunctions::innerPlotImplicitFunction,
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
    "CalculatorFunctions::innerPlotImplicitFunction",
    "PlotImplicit",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotImplicitShowGrid",
    CalculatorFunctions::innerPlotImplicitShowGridFunction,
    "",
    "Same as plotImplicit but shows the underlying grid. "
    "The yellow grid is the initial one (specified by the user), "
    "and the gray grid is obtained by a subdivision which depends on the concrete function.",
    "PlotImplicitShowGrid((x- 1) (y- 1)-((x- 1)^2(y- 1) + 1)^2, (-2, -2), (2, 2), (10,10))",
    "CalculatorFunctions::innerPlotImplicitShowGridFunction",
    "PlotImplicitShowGrid",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotCoordinateSystem",
    CalculatorFunctions::innerPlotCoordinateSystem,
    "",
    "Plots a 3d coordinate system, fitting in a box given by two opposite corners.  ",
    "PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4));",
    "CalculatorFunctions::innerPlotCoordinateSystem",
    "PlotCoordinateSystem",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotSetProjectionScreen",
    CalculatorFunctions::innerPlotSetProjectionScreenBasis,
    "",
    "Set the projection screen. Input: two 3d vectors "
    "that give the 2d-basis of the viewing screen. ",
    "PlotCoordinateSystem((- 2, - 2, - 2), (2, 2, 2))\n"
    "+ PlotSetProjectionScreen((1, 0, - 0.1), (0, 1, - 0.2))",
    "CalculatorFunctions::innerPlotSetProjectionScreenBasis",
    "PlotSetProjectionScreen",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotSurface",
    CalculatorFunctions::innerPlotSurface,
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
    "CalculatorFunctions::innerPlotSurface",
    "PlotSurface",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotCurve",
    CalculatorFunctions::innerPlotParametricCurve,
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
    "CalculatorFunctions::innerPlotParametricCurve",
    "PlotCurve",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotSegment",
    CalculatorFunctions::innerPlotSegment,
    "",
    "Plots a segment connecting two points. ",
    "PlotSegment( (1,2), (3,4))",
    "CalculatorFunctions::innerPlotSegment",
    "PlotSegment",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotMarkSegment",
    CalculatorFunctions::innerPlotMarkSegment,
    "",
    "Plots a segment mark. ",
    "PlotSegment((1, 2), (3,4)) + PlotMarkSegment((1, 2), (3, 4))",
    "CalculatorFunctions::innerPlotMarkSegment",
    "PlotMarkSegment",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotPath",
    CalculatorFunctions::innerPlotPath,
    "",
    "Plots a straight segment path. The path should be enclosed in parentheses, and color must be indicated. ",
    "PlotPath(((0, 0), (3, 0), (3, 4), (0, 0)), blue)",
    "CalculatorFunctions::innerPlotPath",
    "PlotPath",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotSetId",
    CalculatorFunctions::innerPlotSetId,
    "",
    "Creates an empty plot with a given canvas id. "
    "If you add a nameless plot to a named one the the resulting plot acquires the canvas id. ",
    "PlotSetId(myId) + PlotCoordinateSystem((- 3, - 2, - 3), (1, 5, 4))",
    "CalculatorFunctions::innerPlotSetId",
    "PlotSetId",
    innerStandard
  );
  this->AddOperationHandler(
    "MatchesPattern",
    CalculatorFunctions::innerMatchesPattern,
    "",
    "Checks whether the first argument matches the pattern of the second argument. If no, returns 0. "
    "Pattern parametric entries are indicated with the bound variable notation {{a}}. "
    "If the expression matches the pattern, the return is a command enclosure "
    "in which each of the variables is freed and assigned the matched value. ",
    "MatchesPattern{}(a = b * 10, a = {{c}} * b); "
    "MatchesPattern{}(a = b * 10, a = b * {{c}})",
    "CalculatorFunctions::innerMatchesPattern",
    "MatchesPattern",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "PlotPoint",
    CalculatorFunctions::innerPlotPoint,
    "",
    "<b>Calculus teaching function.</b> Plots a point from x and y coordinate. ",
    "PlotPoint{}((1,2),blue)",
    "CalculatorFunctions::innerPlotPoint",
    "PlotPoint",
    innerStandard
  );
  this->AddOperationHandler(
    "Plot2D",
    CalculatorFunctions::innerPlot2DoverIntervals,
    "",
    "If the second argument is a union of intervals, "
    "replaces the plot command with a sum of Plot2d's in which "
    "the second and third argument are extracted from each of the intervals. ",
    "%UseBracketForIntervals\n"
    "Plot2D{}(\\sin{}x +cos{}x, [0, \\pi]\\cup [2\\pi, 3\\pi), \"blue\",2,8)",
    "CalculatorFunctions::innerPlot2DoverIntervals",
    "Plot2DoverIntervals",
    innerStandard
  );
  this->AddOperationHandler(
    "Plot2D",
    CalculatorFunctions::innerPlot2D,
    "",
    "<b>Calculus teaching function.</b> Makes a 2d plot of a function given in the form "
    "y = f(x). The the second and third argument give the upper and "
    "lower bounds of x. The remaining arguments are optional. "
    "The next argument specifies color. "
    "Next argument gives line width. "
    "Next argument gives number of sample points. "
    "Plots may be added together- adding plots superimposes the plots. ",
    "Plot2D{}(\\sin{}x +cos{}x, 0, 5, \"blue\",2,8)",
    "CalculatorFunctions::innerPlot2D",
    "Plot2D",
    innerStandard
  );
  this->AddOperationHandler(
    "IsPlot",
    CalculatorFunctions::innerIsPlot,
    "",
    "Returns 1 if the argument is a plot, 0 otherwise.",
    "%UseBracketForIntervals \n"
    "IsPlot( Plot2D{}(\\sin{}x +cos{}x, [0, \\pi]\\cup [2\\pi, 3\\pi), \"blue\",2,8))",
    "CalculatorFunctions::innerIsPlot",
    "IsPlot",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotFill",
    CalculatorFunctions::innerPlotFill,
    "",
    "Fills a plot with color. ",
    "PlotFill(Plot2D{}(sqrt(1 - x^2), - 1, 1, \"blue\", 2) + Plot2D(- sqrt(1 - x^2), - 1, 1), \"blue\")",
    "CalculatorFunctions::innerPlotFill",
    "PlotFill",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotRectangle",
    CalculatorFunctions::innerPlotRectangle,
    "",
    "Plots a rectangle. "
    "Arguments format: "
    "PlotRectangle{}((lowerCornerLeftXcoord,  lowerCornerLeftXcoord), (width, height)).",
    "PlotRectangle{}((1, 2), (2, 1))",
    "CalculatorFunctions::innerPlotRectangle",
    "PlotRectangle",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotGrid",
    CalculatorFunctions::innerPlotGrid,
    "",
    "Tells the plot to show axes ticks. Takes as input dummy (non-used) argument. ",
    "PlotGrid{}(0)",
    "CalculatorFunctions::innerPlotGrid",
    "PlotGrid",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotRemoveCoordinateAxes",
    CalculatorFunctions::innerPlotRemoveCoordinateAxes,
    "",
    "Removes the coordinate axes from a plot. ",
    "PlotRemoveCoordinateAxes{}(0)",
    "CalculatorFunctions::innerPlotRemoveCoordinateAxes",
    "PlotRemoveCoordinateAxes",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotLabel",
    CalculatorFunctions::innerPlotLabel,
    "",
    "Plots a label at a given position. Arguments format: PlotLabel((Xcoord, Ycoord), \"Label\"). ",
    "PlotLabel{}((1,1), \"(1,1)\")",
    "CalculatorFunctions::innerPlotLabel",
    "PlotLabel",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotViewRectangle",
    CalculatorFunctions::innerPlotViewRectangle,
    "",
    "Creates an empty plot whose sole purpose is to fix "
    "the view rectangle of another plot. "
    "To restrict/expand the view rectangle of another plot, "
    "simply add PlotViewRectangle to the other plot. "
    "Takes as input two pairs of numbers: first pair gives the lower "
    "left corner of the viewing rectangle, "
    "the second argument gives the upper right corner.",
    "Plot2D{}(1/x, -30, 30, \"red\") + PlotViewRectangle((-5,-5), (5,5))",
    "CalculatorFunctions::PlotViewRectangle",
    "PlotViewRectangle",
    innerStandard
  );
  this->AddOperationHandler(
    "PlotWindow",
    CalculatorFunctions::innerPlotViewWindow,
    "",
    "Creates an empty plot whose sole purpose is to fix the dimensions "
    "(in pixels) of the image in the format "
    "(pixelwidth, pixelHeight). "
    "To modify the dimensions (in pixels) of another plot, "
    "add to it the PlotWindow ``plot''. ",
    "Plot2D{}(1/x, -30, 30, \"red\") + PlotWindow(400,400) + PlotViewRectangle((-5,-5), (5,5))",
    "CalculatorFunctions::innerPlotWindow",
    "PlotWindow",
    innerStandard
  );
  this->AddOperationHandler(
    "Plot2DWithBars",
    CalculatorFunctions::innerPlot2DWithBars,
    "",
    "<b>Calculus teaching function.</b> Same as plot2D but plots two "
    "functions with bars locked between the two functions, "
    "used to illustrate approximations to definite integrals.",
    "\nA =3/2- ((-3/4+ 1/4 (x))^{2});\nB= (1/4 (x))^{2}+2;\nPlot2DWithBars{}(A, B, 0, 5, 1)",
    "CalculatorFunctions::innerPlot2DWithBars",
    "Plot2DWithBars",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "IsInteger",
    Calculator::innerIsInteger,
    "",
    "If the argument has no bound variables, returns 1 if "
    "the argument is an integer, 0 otherwise. ",
    "IsInteger{}a;\nIsInteger{}1;\nf{}{{a}}=IsInteger{}a;\nf{}1;\nf{}b",
    "Calculator::innerIsInteger",
    "IsInteger",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "IsSquareFree",
    CalculatorFunctions::innerIsSquareFree,
    "",
    "If the argument is an integer, returns 1 if the "
    "integer is square-free "
    "(no primes in the decomposition of the integer "
    "appear with power greater than one) ",
    "IsSquareFree(6); IsSquareFree(12)",
    "CalculatorFunctions::innerIsSquareFree",
    "IsSquareFree",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "IsSquare",
    CalculatorFunctions::innerIsSquare,
    "",
    "If the argument is an integer, returns 1 if the number is the square of an integer. ",
    "IsSquare(8); IsSquare(16); IsSquare(100);",
    "CalculatorFunctions::innerIsSquare",
    "IsSquare",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "IsNonEmptySequence",
    CalculatorFunctions::innerIsNonEmptySequence,
    "",
    "If the argument has no bound variables, returns 1 "
    "if the argument is an integer, 0 otherwise. ",
    "IsInteger{}a;\n"
    "IsInteger{}1;\n"
    "f{}{{a}}=IsInteger{}a;\n"
    "f{}1;\n"
    "f{}b",
    "CalculatorFunctions::innerIsNonEmptySequence",
    "IsNonEmptySequence",
    innerStandard
  );
  this->AddOperationHandler(
    "IsRational",
    Calculator::innerIsRational,
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
  this->AddOperationHandler(
    "not",
    Calculator::innerNot,
    "",
    "If the argument is a small integer, "
    "returns 1 if the argument is 0 and 1 the argument is non-zero. "
    "If the argument is not a small integer, does nothing. ",
    "not{}1;not{}a; not{}0; not{}(3==4)",
    "Calculator::innerNot",
    "not",
    innerStandard
  );
  this->AddOperationHandler(
    "AllPartitions",
    CalculatorFunctions::innerAllPartitions,
    "",
    "Prints all partitions of a positive number into a sum of positive integers. ",
    "AllPartitions(10) ",
    "CalculatorFunctions::innerAllPartitions",
    "AllPartitions",
    innerStandard
  );
  this->AddOperationHandler(
    "AllVectorPartitions",
    CalculatorFunctions::innerAllVectorPartitions,
    "",
    "Prints all partitions of the vector "
    "(first argument) using a given list of "
    "vectors (second argument). All partitioning "
    "vectors should have positive coordinates. ",
    "AllVectorPartitions((10, 11), ((1,2 ), (2, 3), (4, 5), (2, 1), (3, 2), (5, 4))) ",
    "CalculatorFunctions::innerAllVectorPartitions",
    "AllVectorPartitions",
    innerStandard
  );
  this->AddOperationHandler(
    "AllSelectionsFixedRank",
    CalculatorFunctionsWeylGroup::innerAllSelectionsFixedRank,
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
    "CalculatorFunctions::innerAllSelectionsFixedRank",
    "AllSelectionsFixedRank",
    innerStandard
  );
  this->AddOperationHandler(
    "KostkaNumber",
    CalculatorFunctionsWeylGroup::innerKostkaNumber,
    "",
    "Computes a Kostka number. "
    "First argument = partition given as a tuple "
    "a_1, ..., a_n with a_1>a_2> ...> a_n. "
    "Second argument = tableaux content = "
    "arbitrary tuple of positive integers. ",
    "KostkaNumber((3,2,1), (4,2)) ",
    "CalculatorFunctions::innerKostkaNumbers",
    "KostkaNumber",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintNonNegativeVectorsLevel",
    Calculator::innerPrintZnEnumeration,
    "",
    "Prints all vectors of grade level d with n "
    "coordinates lying in Z_{>= 0}. "
    "Function meant for debugging purposes. "
    "First argument = dimension, second argument =grading level. ",
    "PrintNonNegativeVectorsLevel{}(4, 5);PrintNonNegativeVectorsLevel{}(4, 0); ",
    "Calculator::innerPrintZnEnumeration",
    "PrintNonNegativeVectorsLevel",
    innerStandard
  );
  this->AddOperationHandler(
    "SemisimpleLieAlgebra",
    CalculatorConversions::innerSemisimpleLieAlgebra,
    "",
    "Creates a semisimple Lie algebra. The semisimple Lie algebra "
    "is given via its Dynkin type. A simple Dynkin type is given by "
    "type letter with upper index equal to the "
    "inverse of the scale of the symmetric Cartan "
    "matrix and lower index equal to "
    "the rank of the subalgebra. "
    "For example A^2_3 stands for type "
    "A_3 (sl (4)) with symmetric "
    "Cartan matrix scale equal to 1/2. "
    "If the upper index is omitted, "
    "it is implied to be 1.<hr> "
    "We define the symmetric Cartan matrix "
    "as the matrix whose (i, j)^{th} "
    "entry is the scalar product of the "
    "i^{th} and j^{th} roots. "
    "We assume the roots to be ordered in "
    "the order implied by the "
    "(non-symmetric) Cartan matrices on "
    "page 59 in Humphreys, "
    "Introduction to Lie algebras and representation theory. "
    "If the upper index of the Dynkin type is 1, "
    "the corresponding symmetric Cartan matrix is "
    "assigned the default value. "
    "The default values are chosen "
    "so that the long root has squared "
    "length 2 in types A_n, B_n, D_n, E_6, E_7, E_8, "
    "and F_4, squared length 4 in C_n, "
    "and squared length 6 in type G_2. "
    "<br>If upper index is not equal to 1, "
    "the symmetric "
    "Cartan matrix is given by dividing "
    "the default symmetric "
    "Cartan matrix by the upper index. ",
    "g_{{i}}= GetChevalleyGenerator{}(SemisimpleLieAlgebra{}G_2, i);\n"
    "h_{{i}}= GetCartanGenerator{}(SemisimpleLieAlgebra{}G_2, i);\n"
    "[g_1,g_{- 1}]; \n[g_2, g_{-2}]; \n[h_{1}, g_6]; \n[h_2, g_{-6}]",
    "CalculatorConversions::innerSemisimpleLieAlgebra",
    "SemisimpleLieAlgebra",
    innerStandard
  );
  this->AddOperationHandler(
    "WriteSemisimpleLieAlgebra",
    Calculator::innerWriteSSLieAlgebraToHD,
    "",
    "Writes semisimple Lie algebra structure constants to "
    "the output folder of the calculator. Available to logged-in admins only. ",
    "WriteSemisimpleLieAlgebra(F_4)",
    "Calculator::innerWriteToHDOrPrintSSLieAlgebra",
    "WriteSemisimpleLieAlgebra",
    innerStandard
  );

  this->AddOperationHandler(
    "PrintSemisimpleLieAlgebra",
    Calculator::innerPrintSSLieAlgebraVerbose,
    "",
    "Creates a printout with information about the "
    "semisimple Lie algebra, including "
    "the Lie bracket pairing table. "
    "In addition, this function creates "
    "a graphics of the root system. ",
    "PrintSemisimpleLieAlgebra{}(F_4);\n"
    "PrintSemisimpleLieAlgebra{}(2 G^5_2 + B_3);",
    "Calculator::innerPrintSSLieAlgebraVerbose",
    "PrintSemisimpleLieAlgebra",
    innerStandard
  );
  this->AddOperationHandler(
    "PrecomputeSemisimpleLieAlgebraStructure",
    CalculatorFunctions::innerPrecomputeSemisimpleLieAlgebraStructure,
    "",
    "Function available to logged-in admins only. "
    "Precomputes all built-in semisimple Lie algebra information. "
    "Argument gives a starting point (0 or negative to start at the beginning). "
    "Turn process monitoring on when using this function. ",
    "PrecomputeSemisimpleLieAlgebraStructure 0",
    "CalculatorFunctions::innerPrecomputeSemisimpleLieAlgebraStructure",
    "PrecomputeSemisimpleLieAlgebraStructure",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "GetChevalleyGenerator",
    Calculator::innerGetChevGen,
    "",
    "First argument must be a semisimple "
    "Lie algebra, second argument must "
    "be an integer from -N to N, "
    "where N is the number of positive "
    "roots of the Lie algebra. "
    "The function returns the "
    "Chevalley-Weyl generator labeled by the root "
    "corresponding to the integer index. "
    "The roots are indexed in the "
    "ordered displayed by the "
    "PrintSemisimpleLieAlgebra function. ",
    "[GetChevalleyGenerator{}(G_2, 6), GetChevalleyGenerator{}(G_2, -6)]",
    "Calculator::innerGetChevGen",
    "GetChevalleyGenerator",
    innerStandard
  );
  this->AddOperationHandler(
    "GetCartanGenerator",
    Calculator::innerGetCartanGen,
    "",
    "First argument must be a semisimple Lie algebra, "
    "second argument must "
    "be a number between 1 and K, where K is the rank "
    "of the Lie algebra. "
    "In this case the function returns the element of "
    "the Cartan subalgebra that is dual "
    "to the simple root with the same index. "
    "Note that this element of the Cartan subalgebra "
    "is proportional to a Chevalley-Weyl generator "
    "with a coefficient of proportionality "
    "equal to 2/(simple root length squared) ).",
    "GetCartanGenerator{}(G_2, 1)",
    "Calculator::innerGetCartanGen",
    "GetCartanGenerator",
    innerStandard
  );
  this->AddOperationHandler(
    "FunctionToMatrix",
    Calculator::innerFunctionToMatrix,
    "",
    "Creates a matrix from a function. "
    "The first argument gives the function, "
    "the second argument the number of rows, the third- the number of columns.",
    "X =FunctionToMatrix{}(A,5,5);\n A{}({{a}},{{b}})=a/b;\n X; \\det {} X",
    "Calculator::innerFunctionToMatrix",
    "FunctionToMatrix",
    innerStandard
  );
  this->AddOperationHandler(
    "Transpose",
    Calculator::innerTranspose,
    "",
    "Transposes a matrix of expressions. ",
    "Transpose{}(1,2); (1,2)^t",
    "Calculator::innerTranspose",
    "Transpose",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "DeterminantPolynomial",
    Calculator::innerDeterminantPolynomial,
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
  this->AddOperationHandler(
    "Length",
    CalculatorFunctions::innerLength,
    "",
    "Returns the length of a sequence. ",
    "Length{}();\n"
    "Length(a, b);\n"
    "Length{}((a,b));\n"
    "Length{}(a,b);\n"
    "Length{}(a);\n"
    "Length{}(Sequence{}a);\n"
    "Length{}(Sequence{}(a,b));\n",
    "CalculatorFunctions::innerLength",
    "Length",
    innerStandard
  );
  this->AddOperationHandler(
    "GetMultiplicandList",
    CalculatorFunctions::innerCollectMultiplicands,
    "",
    "Converts a sum to a sequence containing the summands. ",
    "GetMultiplicandList(a*b*c) ",
    "CalculatorFunctions::innerCollectMultiplicands",
    "GetMultiplicandList",
    innerStandard
  );
  this->AddOperationHandler(
    "NormalizeIntervals",
    CalculatorFunctions::innerNormalizeIntervals,
    "",
    "Sorts interval union. ",
    "%UseBracketForIntervals\n NormalizeIntervals([2,3] \\cup [5, 7] \\cup [- 1,- 1/2]);",
    "CalculatorFunctions::innerNormalizeIntervals",
    "NormalizeIntervals",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cup",
    CalculatorFunctions::innerUnionUnionIntervals,
    "",
    "In for the expression a \\cup (b\\cup c) the expression a\\cup b "
    "can be reduced to d, replaces a\\cup(b\\cup c) by a\\cup d. ",
    "%UseBracketForIntervals\n"
    "[3, 3] \\cup ( [3, 7] \\cup [6, 8] );",
    "CalculatorFunctions::innerUnionUnionIntervals",
    "UnionUnionIntervals",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cup",
    CalculatorFunctions::innerUnionIntervals,
    "",
    "If the union of two intervals is one interval, replaces the interval. ",
    "%UseBracketForIntervals\n"
    "[3,7] \\cup [6,8);",
    "CalculatorFunctions::innerUnionIntervals",
    "UnionIntervals",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cup",
    CalculatorFunctions::innerUnionEmptySet,
    "",
    "Takes union with the empty set. ",
    "%UseBracketForIntervals\n"
    "[3, 7) \\cup \\emptyset;",
    "CalculatorFunctions::innerUnionEmptySet",
    "UnionEmptySet",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cap",
    CalculatorFunctions::innerIntersectUnion,
    "",
    "Intersects with the empty set. ",
    "%UseBracketForIntervals\n([3, 7) \\cup (9, 10)) \\cap (4, 8.5);",
    "CalculatorFunctions::innerIntersectUnion",
    "IntersectUnion",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cap",
    CalculatorFunctions::innerIntersectEmptySet,
    "",
    "Intersects with the empty set. ",
    "%UseBracketForIntervals\n  [3,7)\\cap \\emptyset;",
    "CalculatorFunctions::innerIntersectEmptySet",
    "InersectEmptySet",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cap",
    CalculatorFunctions::innerIntersectIntervals,
    "",
    "Intersects two intervals. ",
    "%UseBracketForIntervals\n[3, 7) \\cup [6, 8);",
    "CalculatorFunctions::innerIntersectIntervals",
    "InersectIntervals",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "GetSummandList",
    CalculatorFunctions::innerCollectSummands,
    "",
    "Converts a sum to a sequence containing the summands. ",
    "GetSummandList(1+a +b) ",
    "CalculatorFunctions::outerGetSummands",
    "GetSummandList",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "RemoveDuplicates",
    CalculatorFunctions::innerRemoveDuplicates,
    "",
    "Removes duplicates. ",
    "RemoveDuplicates(a,a,c,a,b,a,b,d) ",
    "CalculatorFunctions::innerRemoveDuplicates",
    "RemoveDuplicates",
    innerStandard
  );
  this->AddOperationHandler(
    "Sort",
    CalculatorFunctions::innerSort,
    "",
    "Sorts a sequence. ",
    "Sort(x^2, x^3, x^1, x^{- 1});\n"
    "Sort(3,2,1,4,3);\n"
    "Sort((3,2,3,1));\n"
    "Sort{}((3,2,3,1));\n",
    "CalculatorFunctions::innerSort",
    "Sort",
    innerStandard
  );
  this->AddOperationHandler(
    "SortDescending",
    CalculatorFunctions::innerSortDescending,
    "",
    "Sorts a sequence in descending order.  ",
    "SortDescending(x^2, x^3, x^1, x^{- 1}) ",
    "CalculatorFunctions::innerSortDescending",
    "SortDescending",
    innerStandard
  );
  this->AddOperationHandler(
    "BlocksOfCommutativity",
    CalculatorFunctions::innerGetUserDefinedSubExpressions,
    "",
    "Returns subexpression blocks of commutativity. ",
    "BlocksOfCommutativity(\\sin x + x^2+ 3x y + 18x^{3/4 y}+\\sqrt{2}^{\\sqrt{2}c})",
    "CalculatorFunctions::innerGetUserDefinedSubExpressions",
    "BlocksOfCommutativity",
    innerStandard
  );
  this->AddOperationHandler(
    "HeighestWeightVector",
    CalculatorFunctions::innerHWV,
    "",
    "Highest weight vector in a generalized Verma module. "
    "The first argument gives the semisimple Lie algebra. "
    "The second argument "
    "gives the highest weight in fundamental coordinates. "
    "The third argument parametrizes the parabolic subalgebra, e.g. "
    "(1,0,0) stands for a "
    "parabolic subalgebra with first simple root crossed-out. "
    "The second argument is allowed to have "
    "entries that are not non-negative integers in the "
    "positions in which the third argument has 1's. ",
    "g_{{i}}= GetChevalleyGenerator{}(B_3, i);\n"
    "h_{{i}}= GetCartanGenerator{}(B_3, i);\n"
    "v_\\mu=HeighestWeightVector{} (A_3, (1,0,1),(0,0,0));\n"
    "v_\\lambda =HeighestWeightVector{}(B_3, (x_1,0,1),(1,0,0));\n"
    "h_1g_{- 1}v_\\lambda",
    "CalculatorFunctions::innerHWV",
    "HeighestWeightVector",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintModule",
    CalculatorFunctions::innerPrintGenVermaModule,
    "",
    "Makes a report on a finite dimensional Lie algebra module, "
    "or more generally, on a generalized Verma module "
    "(irreducible finite dimensional semisimple Lie algebra "
    "modules are a partial case of generalized Verma modules). "
    "The first argument gives the semisimple Lie algebra. "
    "The second argument gives the highest weight "
    "in fundamental coordinates. The third argument "
    "parametrizes the parabolic subalgebra, e.g. "
    "(1,0) stands for a parabolic subalgebra "
    "(lying in algebra of rank 2) "
    "with first simple root crossed-out. "
    "The second argument is allowed to have "
    "entries that are not non-negative integers "
    "in the positions in which the third argument has 1's. ",
    "PrintModule{}(G_2, (2, 0), (0, 0))",
    "CalculatorFunctions::innerPrintGenVermaModule",
    "PrintModule",
    innerStandard
  );
  this->AddOperationHandler(
    "HighestWeightTAAbf",
    CalculatorFunctions::innerHWTAABF,
    "",
    "Highest weight transpose anti-automorphism bilinear form, a.k.a. "
    "Shapovalov form. Let M be a Verma module "
    "with highest weight vector v given in "
    "fundamental coordinates. Let P:M->M "
    "be a projection map onto Cv that maps "
    "every weight vector of M of weight "
    "different from the highest to 0. "
    "Let u_1, u_2 be two words in the "
    "universal enveloping algebra. "
    "Then define HighestWeightTAAbf(u_1,u_2)= Tr_M (P ( taa(u_1) u_2 ), "
    "where taa() is the transpose anti-automorphism of g. ",
    "g_{{a}}= GetChevalleyGenerator{} (G_2, a);\nHighestWeightTAAbf{}(g_{- 1} g_{-2}, g_{- 1}g_{-2}, (2,2))",
    "CalculatorFunctions::innerHWTAABF",
    "HighestWeightTAAbf",
    innerStandard
  );
  this->AddOperationHandler(
    "WeylDimFormula",
    CalculatorFunctions::innerWeylDimFormula,
    "",
    "Weyl dimension formula. First argument gives "
    "the type of the Weyl group of the simple "
    "Lie algebra in the form Type_Rank (e.g. E_6). "
    "The second argument gives the highest weight in fundamental coordinates. ",
    "WeylDimFormula{}(G_2, (x,0));\nWeylDimFormula{}(B_3, (x,0,0));",
    "CalculatorFunctions::innerWeylDimFormula",
    "WeylDimFormula",
    innerStandard
  );
  this->AddOperationHandler(
    "AnimateLittelmannPaths",
    Calculator::innerAnimateLittelmannPaths,
    "",
    "Generates all Littelmann-Lakshmibai-Seshadri paths, draws them and animates them. "
    "Presented first on the seminar in Charles University Prague. "
    "The first argument gives the type of the semisimple Lie algebra, "
    "the second gives the highest weight. ",
    "AnimateLittelmannPaths{}(G_2, (2, 0));",
    "Calculator::innerAnimateLittelmannPaths",
    "AnimateLittelmannPaths",
    innerStandard
  );
  this->AddOperationHandler(
    "DecomposeInducingRepGenVermaModule",
    CalculatorFunctions::innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart,
    "",
    "Decomposes the inducing module of a generalized "
    "Verma module over the Levi part of a parabolic "
    "smaller than the inducing one. "
    "The first argument gives the type of the algebra. "
    "The second argument gives the highest "
    "weight of the module in "
    "fundamental coordinates. "
    "The third argument gives "
    "the parabolic subalgebra with respect "
    "to which we induce. The last argument "
    "gives the parabolic subalgebra with "
    "respect to whose Levi part we decompose.",
    "DecomposeInducingRepGenVermaModule{}(B_3, (0, 1, 1), (1, 0, 0), (1, 0, 1))",
    "CalculatorFunctions::innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart",
    "DecomposeInducingRepGenVermaModule",
    innerStandard
  );
  this->AddOperationHandler(
    "Casimir",
    Calculator::innerCasimir,
    "",
    "Gives the Casimir element. ",
    "Casimir{}(G_2)",
    "Calculator::innerCasimir",
    "Casimir",
    innerStandard
  );
  this->AddOperationHandler(
    "CasimirWRTLevi",
    CalculatorFunctions::innerCasimirWRTlevi,
    "",
    "Gives the Casimir element of a Levi part of "
    "a parabolic subalgebra. First argument = ambient Lie algebra. "
    "Second argument = parabolic selection = 1 "
    "stands for simple root outside of the Levi "
    "component, 0 stands for simple root of the Levi component.",
    "CasimirWRTLevi{}(B_3, (1,0,0))",
    "CalculatorFunctions::innerCasimirWRTlevi",
    "CasimirWRTLevi",
    innerStandard
  );
  this->AddOperationHandler(
    "HmmG2inB3",
    Calculator::innerEmbedG2inB3,
    "",
    "Embeds elements of the Universal enveloping "
    "of G_2 in B_3, following an embedding found "
    "in a paper by McGovern.",
    "g_{{a}}= GetChevalleyGenerator{} (G_2, a); "
    "HmmG2inB3{}(g_1);\nHmmG2inB3{}(g_2) ",
    "Calculator::innerEmbedG2inB3",
    "HmmG2inB3",
    innerStandard
  );
  this->AddOperationHandler(
    "DrawRootSystem",
    CalculatorFunctions::innerDrawRootSystem,
    "",
    "Draws the root system of a semisimple Lie algebra. "
    "Takes one or three arguments: "
    "if one argument is provided, that must "
    "be a semisimple Lie algebra or "
    "a semisimple Lie algebra type; "
    "if three arguments are provided, "
    "the first must be a semisimple "
    "Lie algebra or a semisimple Lie algebra type, "
    "and the second and third argument must be two "
    "rational vectors of dimension "
    "equal to the rank of the semisimple "
    "Lie algebra. The root system is drawn in a "
    "Coxeter plane. If the extra two vectors "
    "are given, they are used to initialize "
    "a preferred projection plane "
    "in the change-to-basis entries "
    "below the graphics. "
    "Clicking the obvious button creates a basis "
    "change animations ideal for presentations.",
    "DrawRootSystem{}(A_7, (1, 0 , 2, 2, 2, 0, 1), (1, 3, 2, 2, 2, 3, 1))",
    "CalculatorFunctions::innerDrawRootSystem",
    "DrawRootSystem",
    innerStandard
  );
  this->AddOperationHandler(
    "DrawWeightSupportWithMults",
    CalculatorFunctions::innerDrawWeightSupportWithMults,
    "",
    "Draws the weight support of an irreducible "
    "finite-dimensional highest weight module. "
    "The first argument gives the type and the second "
    "gives the highest weight given in "
    "fundamental weight basis. The maximum number "
    "of drawn weights allowed is (hard-code) "
    "limited to 10 000. "
    "If the number of weights in the weight "
    "support exceeds this number, only "
    "the first 10 000 weights will be drawn. "
    "<b>Warning. Drawing text (i.e. multiplicities) "
    "is very javascript-processor-intensive. "
    "Use only for *small* examples, "
    "else you might hang your browser. </b>",
    "DrawWeightSupportWithMults{}(B_3, (0, 1, 1));\n"
    "DrawWeightSupportWithMults{}(G_2, (1, 0))",
    "CalculatorFunctions::innerDrawWeightSupportWithMults",
    "DrawWeightSupportWithMults",
    innerStandard
  );
  this->AddOperationHandler(
    "DrawWeightSupport",
    CalculatorFunctions::innerDrawWeightSupport,
    "",
    "Same as DrawWeightSupportWithMults but displays no multiplicities. "
    "Same warning for hanging up your browser "
    "with javascript holds.",
    "DrawWeightSupport{}(B_3, (1, 1, 1));\n"
    "DrawWeightSupport{}(G_2, (1, 2))",
    "CalculatorFunctions::innerDrawWeightSupport",
    "DrawWeightSupport",
    innerStandard
  );
  this->AddOperationHandler(
    "SplitFDpartB3overG2CharsOnly",
    Calculator::innerSplitFDpartB3overG2CharsOnly,
    "",
    "Splits the finite dimensional part of the "
    "inducing module of the generalized "
    "Verma module of B_3(so(7)) into G_2-components. "
    "The argument is gives the highest weight of the "
    "generalized Verma module in fundamental "
    "coordinates with respect to so(7). "
    "The arguments which are not small integers "
    "indicate the non-selected "
    "roots of the inducing parabolic subalgebra of B_3. ",
    "SplitFDpartB3overG2CharsOnly(x_1, 2, 0)",
    "Calculator::innerSplitFDpartB3overG2CharsOnly",
    "SplitFDpartB3overG2CharsOnly",
    innerStandard
  );
  this->AddOperationHandler(
    "SplitFDpartB3overG2",
    CalculatorFunctions::innerSplitFDpartB3overG2,
    "",
    "Splits the finite dimensional part of the inducing "
    "module of the generalized Verma module of "
    "B_3(so(7)) into G_2-components. "
    "The argument is gives the highest weight of the "
    "generalized Verma module in fundamental "
    "coordinates with respect to so(7). "
    "The arguments which are not small integers "
    "indicate the non-selected roots of the inducing "
    "parabolic subalgebra of B_3. ",
    "SplitFDpartB3overG2{}(x_1, 1, 0)",
    "CalculatorFunctions::innerSplitFDpartB3overG2",
    "SplitFDpartB3overG2",
    innerInvisibleExperimental
  );
  this->AddOperationHandler(
    "PrintB3G2branchingTableCharsOnly",
    Calculator::innerPrintB3G2branchingTableCharsOnly,
    "",
    "Creates a table of branching of finite dimensional B_3-modules over G_2. "
    "The argument of the function gives the "
    "maximum height of the B_3-weight. "
    "The second argument indicates the parabolic subalgebra of "
    "B_3- zero entry stands for the corresponding "
    "root space lying in the Levi part of the parabolic. "
    "Non-zero entry means the corresponding "
    "negative root space is not in the parabolic. "
    "The expression given in that coordinate "
    "is used as the corresponding highest weight. ",
    "PrintB3G2branchingTableCharsOnly{}(2, (0,0,0)); PrintB3G2branchingTableCharsOnly{}(2, (x_1,0,0))",
    "Calculator::innerPrintB3G2branchingTableCharsOnly",
    "PrintB3G2branchingTableCharsOnly",
    innerInvisibleExperimental
  );
  this->AddOperationHandler(
    "PrintB3G2branchingTable",
    Calculator::innerPrintB3G2branchingTable,
    "",
    "Creates a table of branching of finite dimensional B_3-modules over G_2. "
    "The argument of the function gives the maximum height "
    "of the B_3-weight. The function works with arguments 0 or 1; "
    "values of 2 or more must be run off-line.",
    "PrintB3G2branchingTable{}(1, (0,0,0)); "
    "PrintB3G2branchingTable{}(1, (x_1,0,0))",
    "Calculator::innerPrintB3G2branchingTable",
    "PrintB3G2branchingTable",
    innerInvisibleExperimental
  );
  this->AddOperationHandler(
    "SplitFDTensorGenericGeneralizedVerma",
    CalculatorFunctions::innerSplitGenericGenVermaTensorFD,
    "",
    "Experimental, please don't use. Splits generic generalized "
    "Verma module tensor finite dimensional module. ",
    "SplitFDTensorGenericGeneralizedVerma{}(G_2, (1, 0), (x_1, x_2)); ",
    "CalculatorFunctions::innerSplitGenericGenVermaTensorFD",
    "SplitFDTensorGenericGeneralizedVerma",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "WriteGenVermaAsDiffOperatorsUpToWeightLevel",
    CalculatorFunctions::innerWriteGenVermaModAsDiffOperatorUpToLevel,
    "",
    "<b>Experimental, please don't use</b>. "
    "Embeds a Lie algebra in the Weyl algebra "
    "with matrix coefficients. The third argument "
    "gives the highest weight. The non-zero entries "
    "of the highest weight give the root spaces "
    "outside of the Levi part of the parabolic. "
    "The second argument gives the weight "
    "level to which the computation should be carried out",
    "WriteGenVermaAsDiffOperatorsUpToWeightLevel{}(B_3, 1, (0, 0, y)); ",
    "CalculatorFunctions::innerWriteGenVermaModAsDiffOperatorUpToLevel",
    "WriteGenVermaAsDiffOperatorsUpToWeightLevel",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder",
    Calculator::innerWriteGenVermaModAsDiffOperatorsGeneratorOrder,
    "",
    "Constructs a parabolically induced map from a "
    "semisimple Lie algebra to a Weyl algebra "
    "with matrix coefficients. "
    "More precisely, this function constructs a "
    "generalized Verma module and writes it using "
    "differential operators with matrix coefficients. "
    "The first argument gives the type of the "
    "semisimple Lie algebra, the second argument "
    "gives the highest weight in fundamental coordinates. "
    "The third argument gives the parabolic subalgebra "
    "selection with 0 standing for non-crossed "
    "out roots and 1 for crossed-out roots. "
    "In all non crossed-out root positions, "
    "the coordinates of the highest weight "
    "vector must be small integers (because the highest "
    "weight must be dominant and integral with respect "
    "to the Levi part of the inducing parabolic subalgebra). "
    "In the crossed-out root positions, the coordinates "
    "of the highest weight vector can be "
    "arbitrary polynomial expressions. "
    "The algorithm depends on an order chosen on "
    "the Chevalley-Weyl generators of the nilradical. "
    "Here, we sort the nilradical elements in descending "
    "order relative to the order of their roots. "
    "In addition to the first 3 arguments, this function "
    "accepts further optional arguments, "
    "customizing the notation of the final printout.",
    "MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder{}(B_3, (0,0,0), (0, 0, 1), x, \\partial, a); ",
    "Calculator::innerWriteGenVermaModAsDiffOperatorsGeneratorOrder",
    "MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder",
    innerStandard
  );
  this->AddOperationHandler(
    "MapSemisimpleLieAlgebraInWeylAlgebra",
    Calculator::innerWriteGenVermaModAsDiffOperatorsNilOrderDescending,
    "",
    "Same as MapSemisimpleLieAlgebraInWeylAlgebra, "
    "but with a different order on the elements "
    "of the nilradical. We sort the elements "
    "of the nilradical as follows. "
    "Consider the partial order given "
    "by the graded lex order on the coordinates that "
    "correspond to crossed-out roots. "
    "Then extend this partial order "
    "by ``breaking the ties'' via the graded "
    "lexicographic order on the non-crossed out roots. "
    "This extension we call "
    "the nil-order. Now this function sorts the "
    "elements of the nilradical in descending nil-order, "
    "that is, elements with higher nil-order come first.",
    "MapSemisimpleLieAlgebraInWeylAlgebra{}(B_3, (0, 0, 0), (0, 0, 1), x, \\partial, a); ",
    "Calculator::innerWriteGenVermaModAsDiffOperatorsNilOrderDescending",
    "MapSemisimpleLieAlgebraInWeylAlgebra",
    innerStandard
  );
  this->AddOperationHandler(
    "MapSemisimpleLieAlgebraInWeylAlgebraAllGens",
    Calculator::innerWriteGenVermaModAsDiffOperatorsAllGensNilOrderDescending,
    "",
    "Constructs a parabolically induced map from "
    "a semisimple Lie algebra "
    "to a Weyl algebra with matrix coefficients. "
    "More precisely, this function constructs a "
    "generalized Verma module and writes it using "
    "differential operators with matrix coefficients. "
    "The first argument gives the type of the "
    "semisimple Lie algebra, the second argument "
    "gives the highest weight in fundamental coordinates. "
    "The third argument gives the parabolic "
    "subalgebra selection with 0 standing for "
    "non-crossed out roots and 1 for crossed-out roots. "
    "In all non crossed-out root positions, "
    "the coordinates of the highest weight "
    "vector must be small integers (because the highest "
    "weight must be dominant and integral with "
    "respect to the Levi part of the inducing parabolic subalgebra). "
    "In the crossed-out root positions, the "
    "coordinates of the highest weight vector can be "
    "arbitrary polynomial expressions. "
    "In addition to the first 3 arguments, this function "
    "accepts further optional arguments, customizing the "
    "notation of the final printout. The differential "
    "operator part of the algorithm uses the "
    "nil-order given by the function "
    "MapSemisimpleLieAlgebraInWeylAlgebra.",
    "MapSemisimpleLieAlgebraInWeylAlgebraAllGens{}(B_3, (0,0,0), (0, 0, 1), x, \\partial, a); ",
    "Calculator::innerWriteGenVermaModAsDiffOperatorsAllGensNilOrderDescending",
    "MapSemisimpleLieAlgebraInWeylAlgebraAllGens",
    innerStandard
  );
  this->AddOperationHandler(
    "KLcoeffs",
    Calculator::innerKLcoeffs,
    "",
    "Computes the n by n tables of "
    "1) Kazhdan-Lusztig polynomials, "
    "2) R polynomials and "
    "3) Kazhdan-Lusztig polynomials evaluated at one, where n<=192 "
    "is the size of the Weyl group (i.e. no larger than D_4(so(8)). "
    "The notation follows the original paper by Kazhdan and Lusztig, "
    "\"Representations of Coxeter Groups and Hecke Algebras\". "
    "The algorithm is directly derived from formulas (2.0a-c) there, "
    "as explained in the Wikipedia page on Kazhdan-Lusztig polynomials. "
    "Please note that the 192 by 192 element table takes "
    "almost 3 minutes to compute. "
    "Faster implementations of the KL polynomials are "
    "available from programs by Fokko du Cloux and others "
    "(our simple implementation stores the full table "
    "of R-polynomials and KL-polynomials in RAM memory at "
    "all times, unlike the other more efficient implementations).",
    "KLcoeffs{}(B_3)",
    "Calculator::innerKLcoeffs",
    "KLcoeffs",
    innerStandard
  );
  this->AddOperationHandler(
    "RootSubsystem",
    Calculator::innerRootSubsystem,
    "",
    "Generates a root subsystem of a simple type. "
    "First argument indicates simple type, second, third,... arguments "
    "give the generating roots. ",
    "RootSubsystem(F_4, (0, 1, 0, 0), (0, 0, 1, 0), (1, 1, 2, 2))",
    "Calculator::innerRootSubsystem",
    "RootSubsystem",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintRootSubalgebras",
    CalculatorFunctions::innerPrintRootSAs,
    "",
    "Prints sl(2) subalgebras and root subalgebras. "
    "The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
    "PrintRootSubalgebras(E_6)",
    "CalculatorFunctions::innerPrintRootSAs",
    "PrintRootSubalgebras",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintRootSubalgebrasRecompute",
    CalculatorFunctions::innerPrintRootSAsForceRecompute,
    "",
    "Same as printRootSubalgebras but forces recomputation. "
    "Use to recompute obsolete or interrupted output files.",
    "PrintRootSubalgebrasRecompute(b_3)",
    "CalculatorFunctions::innerPrintRootSAsForceRecompute",
    "PrintRootSubalgebrasRecompute",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "PrintSlTwoSubalgebras",
    CalculatorFunctions::innerPrintSltwos,
    "",
    "Prints sl(2) subalgebras and root subalgebras. "
    "The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
    "PrintSlTwoSubalgebras(g_2)",
    "CalculatorFunctions::innerPrintSltwos",
    "PrintSlTwoSubalgebras",
    innerStandard
  );
  this->AddOperationHandler(
    "ParabolicsInfoBruhatGraph",
    CalculatorFunctions::innerParabolicWeylGroupsBruhatGraph,
    "",
    "Makes a table with information about the Weyl group of a "
    "parabolic subalgebra of the ambient Lie algebra, "
    "as well as the cosets "
    "(given by minimal coset representatives) of the Weyl subgroup "
    "in question. "
    "The input must have as many integers as there "
    "are simple roots in the ambient Lie algebra. "
    "If the root is crossed out "
    "(i.e. not a root space of the Levi part), "
    "one should put a 1 in the corresponding coordinate. "
    "Otherwise, one should put 0. "
    "For example, for Lie algebra B3(so(7)), calling "
    "parabolicsInfoBruhatGraph(0, 0, 0) gives you the "
    "Weyl group info for the entire algebra; "
    "calling parabolicsInfoBruhatGraph(1,0,0) gives you "
    "info for the Weyl subgroup generated by "
    "the last two simple root. "
    "In the produced graph, the element s_{\\eta_i} "
    "corresponds to a reflection with respect "
    "to the i^th simple root. You will get your "
    "output as a .png file link, you must click onto "
    "the link to see the end result. "
    "<b>Please do not use for subalgebras "
    "larger than B_4 (so(9)). The vpf program has "
    "no problem handling this function up to "
    "E_6 but LaTeX crashes trying to process the output. </b>",
    "parabolicsInfoBruhatGraph{}(B_3,(1,0,0),(1,0,0))",
    "CalculatorFunctions::innerParabolicWeylGroupsBruhatGraph",
    "ParabolicsInfoBruhatGraph",
    innerStandard
  );
  this->AddOperationHandler(
    "TestMonomialBasisConjecture",
    Calculator::innerTestMonomialBaseConjecture,
    "",
    "Tests the monomial basis conjecture from a common work by Jackson and Milev paper. "
    "First argument gives rank bound. "
    "Second argument gives dimension bound. ",
    "TestMonomialBasisConjecture{}(2, 50)",
    "Calculator::innerTestMonomialBaseConjecture",
    "TestMonomialBasisConjecture",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "LSpath",
    Calculator::innerLSPath,
    "",
    "Lakshmibai-Seshadri path starting at 0. "
    "The first argument gives the semisimple Lie algebra, "
    "the next arguments give the way-points of the path.",
    "LSpath{}(G_2, (0,0), (2,1) )",
    "Calculator::innerLSPath",
    "LSpath",
    innerStandard
  );
  this->AddOperationHandler(
    "LROdefine",
    Calculator::innerLittelmannOperator,
    "",
    "Littelmann root operator e_i, where e_i is the Littelmann "
    "root operator with respect to root of index i. "
    "If i is negative then the e_i root operator is defined to be "
    "the f_\\alpha operator.",
    "e_{{i}}= LROdefine_i; e_{- 1} e_{- 1} LSpath{}(G_2, (0,0), (2,1))",
    "Calculator::innerLittelmannOperator",
    "LROdefine",
    innerExperimental
  );
  this->AddOperationHandler(
    "PrintProductDistancesModN",
    CalculatorFunctions::innerFindProductDistanceModN,
    "",
    "First argument = number N. Second argument = list of positive numbers. "
    "This function finds, modulo N, the product "
    "distance between 1 and each number mod N. "
    "We define the product distance between 1 and a "
    "number X as the minimal sum of elements "
    "whose product equals X mod N, where "
    "the elements are taken from the predefined "
    "list of positive integers given by the second argument.",
    "PrintProductDistancesModN(65537, "
    "(97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122)"
    ")",
    "CalculatorFunctions::innerFindProductDistanceModN",
    "PrintProductDistancesModN",
    innerAdminNoTestInvisibleOffByDefault
  );
  this->AddOperationHandler(
    "SolveProductSumEquationOverSetModN",
    CalculatorFunctions::innerSolveProductSumEquationOverSetModN,
    "",
    "Tries to find one solution of the system a_1*a_2* ...= X mod N a_1+a_2+...=Y "
    "where the a_i's belong to a "
    "predefined set of positive numbers. ",
    "SolveProductSumEquationOverSetModN(theMod =65537; theProduct =16628; theSum=1286; "
    "theSet = (97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122)) ",
    "CalculatorFunctions::innerFindProductDistanceModN",
    "SolveProductSumEquationOverSetModN",
    innerAdminNoTestInvisibleOffByDefault
  );
  this->AddOperationHandler(
    "IsNilpotent",
    CalculatorFunctions::innerIsNilpotent,
    "",
    "Computes whether a matrix is nilpotent. "
    "May/will be extended to work for an arbitrary object "
    "for which the term \"nilpotent\" makes sense. ",
    "IsNilpotent{}((0, 1), (0, 0))",
    "CalculatorFunctions::innerIsNilpotent",
    "IsNilpotent",
    innerStandard
  );
  this->AddOperationHandler(
    "InvertMatrixVerbose",
    Calculator::innerInvertMatrixVerbose,
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
  this->AddOperationHandler(
    "InvertMatrix",
    CalculatorFunctions::innerInvertMatrix,
    "",
    "Inverts a matrix of rationals or algebraic numbers if invertible. ",
    "X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0)); InvertMatrix X- X^{- 1}",
    "CalculatorFunctions::innerInvertMatrix",
    "InvertMatrix",
    innerStandard
  );
  //this->AddOperationHandler
  //("GramSchmidtAlgebraicVerbose",
  //  CalculatorFunctions::innerGramSchmidtVerbose, "",
  // "(Attempts to) run Gram-Schmidt (over the algebraic numbers!) on a small matrix . The inputs are vector-rows.",
  // "X = GramSchmidtAlgebraicVerbose((1,2,1), (1,0,1), (- 1,1,0)); InvertMatrix X- X^{- 1}",
  // false, false,
  // "CalculatorFunctions::innerInvertMatrix",
  // "InvertMatrix");
  this->AddOperationHandler(
    "Trace",
    CalculatorFunctions::innerTrace,
    "",
    "Gets trace of a square matrix. ",
    "X = MakeMatrix((1,2,1), (1,0,1), (- 1,1,0)); Trace X",
    "CalculatorFunctions::innerTrace",
    "Trace",
    innerStandard
  );
  this->AddOperationHandler(
    "ReverseBytes",
    CalculatorFunctions::innerReverseBytes,
    "",
    "Reverses the bytes of a string. Does not respect utf-8 encoding. ",
    "ReverseBytes{}(\"abc\")",
    "Calculator::innerReverseBytes",
    "ReverseBytes",
    innerStandard
  );
  this->AddOperationHandler(
    "Reverse",
    Calculator::innerReverseOrder,
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
  this->AddOperationHandler(
    "ReverseRecursively",
    Calculator::innerReverseOrderRecursivelY,
    "",
    "Same as Reverse but will apply recursively to the children expressions as well.",
    "ReverseRecursively{}(s_1 s_2 s_3s_4s_2s_3s_1s_2s_3s_4s_1s_2s_3s_2s_1)",
    "Calculator::innerReverseOrderRecursively",
    "ReverseRecursively",
    innerStandard
  );
  this->AddOperationHandler(
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
    "SolveFor(x, a x^2+b x +c); SolveFor(x, a x +b)",
    "CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT",
    "SolveFor",
    innerStandard
  );
  this->AddOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystem",
    CalculatorFunctions::innerSolveSerreLikeSystemNoUpperLimit,
    "",
    "Attempts to heuristically solve a system "
    "of polynomial of equations over the rationals. "
    "The input system of equations can be arbitrary, "
    "however it is assumed that the system is Serre-like, i.e., comes "
    "from the Serre relations governing the embedding "
    "of a semisimple Lie algebra in a semisimple Lie algebra. "
    "This suggests a certain heuristic solution strategy "
    "(will be documented as the code matures). "
    "To stress it out explicitly, the algorithm is heuristic "
    "- i.e., is not guaranteed to work. "
    "The result of the function is a printout with one of "
    "the possible outcomes. <br>"
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
    "FindOneSolutionSerreLikePolynomialSystem{}("
    "x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, "
    "x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, "
    "x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, "
    "x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, "
    "x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, "
    "x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, "
    "x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, "
    "x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, "
    "x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, "
    "x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, "
    "x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, "
    "x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14} - x_{1}x_{13} + 1, "
    "x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, "
    "x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, "
    "x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, "
    "x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, "
    "x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, "
    "x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, "
    "x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, "
    "x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)",
    "CalculatorFunctions::innerSolveSerreLikeSystemNoUpperLimit",
    "FindOneSolutionSerreLikePolynomialSystem",
    innerStandard
  );
  this->AddOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystemUpperLimit",
    CalculatorFunctions::innerSolveSerreLikeSystemUpperLimit,
    "",
    "Same as FindOneSolutionSerreLikePolynomialSystem "
    "but the first argument gives upper limits "
    "to the number of polynomial computations that can be carried out. ",
    "FindOneSolutionSerreLikePolynomialSystemUpperLimit{}( 2001, "
    "x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+ 1, "
    "x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21}, "
    "x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21}, "
    "x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19}, "
    "x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20}, "
    "x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}- 1, "
    "x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20}, "
    "x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19}, "
    "x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19}, "
    "x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}- 1, "
    "x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13}, "
    "x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14}-x_{1}x_{13}+ 1, "
    "x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15}, "
    "x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15}, "
    "x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14}, "
    "x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14} - 1, "
    "x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14}, "
    "x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13}, "
    "x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13}, "
    "x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13} - 1)",
    "CalculatorFunctions::innerSolveSerreLikeSystemUpperLimit",
    "FindOneSolutionSerreLikePolynomialSystemUpperLimit",
    innerStandard
  );
  this->AddOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystemAlgebraic",
    CalculatorFunctions::innerSolveSerreLikeSystemAlgebraic,
    "",
    "Same as FindOneSolutionSerreLikePolynomialSystem "
    "but starts directly over algebraic closure. ",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraic"
    "(x^2 + 1, y x z - 1, z^2 x + y - 1, w u)",
    "CalculatorFunctions::innerSolveSerreLikeSystemAlgebraic",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraic",
    innerStandard
  );
  this->AddOperationHandler(
    "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit",
    CalculatorFunctions::innerSolveSerreLikeSystemAlgebraicUpperLimit,
    "",
    "Same as FindOneSolutionSerreLikePolynomialSystemAlgebraic "
    "but the first argument gives upper limits to the number of "
    "polynomial computations that can be carried out.",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit"
    "(10000, x^2+ 1, y x z - 1, z^2 x +y - 1, w u)",
    "CalculatorFunctions::innerSolveSerreLikeSystemAlgebraicUpperLimit",
    "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit",
    innerStandard
  );
  this->AddOperationHandler(
    "GroebnerLexUpperLimit",
    Calculator::innerGroebnerLex,
    "",
    "Transforms to a reduced Groebner basis using the "
    "lexicographic order. The lexicographic order is "
    "inherited from the comparison of the underlying expressions. "
    "<b>The first argument gives an upper bound to "
    "the number of polynomial operations allowed.</b> "
    "A non-positive number signifies no upper limit, "
    "however please do not use (this is a public "
    "web server and multiple instances of a large computation "
    "might hog it up). The resulting printout will "
    "let your know whether the upper limit was hit or not. "
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
    "Here lcm stands for least common multiple of monomials, defined in the obvious way. "
    "<br>\n"
    "1. Allocate two buckets of polynomials - one \"main\" "
    "bucket and additional bucket. At any given moment in the "
    "algorithm, the union of main bucket and the "
    "additional bucket should give a basis of the ideal. "
    "<br>\n2. Move all input elements into the additional bucket. <br>\n"
    "3. Call the <b>reduce additional bucket</b> subroutine. "
    "In the c++ implementation the function is "
    "called GroebnerBasisComputation::AddPolyAndReduceBasis. <br>\n"
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
    "GroebnerLexUpperLimit{}(10000, s^2+c^2+ 1, a-s^4, b-c^4 );\n"
    "GroebnerLexUpperLimit{}(5, s^2+c^2+ 1, a-s^4, b-c^4 );",
    "Calculator::innerGroebnerLex",
    "GroebnerLexUpperLimit",
    innerStandard
  );
  this->AddOperationHandler(
    "PolynomialRelationsUpperLimit",
    CalculatorFunctions::innerPolynomialRelations,
    "",
    "Finds the relations between the polynomials.",
    "PolynomialRelationsUpperLimit{}(10000, s^2+c^2+ 1, s^4, c^4 );",
    "CalculatorFunctions::innerPolynomialRelations",
    "PolynomialRelationsUpperLimit",
    innerStandard
  );
  this->AddOperationHandler(
    "GroebnerRevLexUpperLimit",
    Calculator::innerGroebnerRevLex,
    "",
    "Same as GroebnerLexUpperLimit but uses reverse order on the variables (z<x).",
    "GroebnerRevLexUpperLimit{}(10000, s^2+c^2+ 1, a-s^4, b-c^4 );"
    "\nGroebnerRevLexUpperLimit{}(5, s^2+c^2+ 1, a-s^4, b-c^4 );",
    "Calculator::innerGroebnerRevLex",
    "GroebnerRevLexUpperLimit",
    innerStandard
  );
  this->AddOperationHandler(
    "GroebnerGrLexUpperLimit",
    Calculator::innerGroebnerGrLex,
    "",
    "Transforms to a reduced Groebner basis relative to the graded "
    "lexicographic order. In the graded lexicographic order, "
    "monomials are first compared by total degree, "
    "then by lexicographic order. The lexicographic order "
    "is inherited from the comparison of the underlying expressions. "
    "<b>The first argument gives an upper bound "
    "to the number of polynomial operations allowed.</b> "
    "A non-positive number signifies no upper limit, "
    "however please do not use (this is a public "
    "web server and multiple instances of a large "
    "computation might hog it up). "
    "The resulting printout will let your know whether "
    "the upper limit was hit or not. "
    "For a description of the algorithm used see "
    "the description of function GroebnerLexUpperLimit.",
    "GroebnerGrLexUpperLimit{}(10000, a^2+b^2+ 1, x-a^4, y-b^4 );\n "
    "GroebnerGrLexUpperLimit{}(5, a^2+b^2+ 1, x-a^4, y-b^4 )",
    "Calculator::innerGroebnerGrLex",
    "GroebnerGrLexUpperLimit",
    innerStandard
  );
  this->AddOperationHandler(
    "ComputeFKFT",
    CalculatorFunctions::innerComputePairingTablesAndFKFTsubalgebras,
    "",
    "Attempts to compute all Fernando-Kac subalgebras according "
    "to the most experimental, latest and greatest algorithm. "
    "Argument must be of type semisimple Lie subalgebras. ",
    "ComputeFKFT( ComputeSemisimpleSubalgebras(c_3))",
    "CalculatorFunctions::innerComputePairingTablesAndFKFTsubalgebras",
    "ComputeFKFT",
    innerAdminNoTestExperimental
  );
  this->AddOperationHandler(
    "ComputeSemisimpleSubalgebras",
    CalculatorFunctions::innerComputeSemisimpleSubalgebras,
    "",
    "Computes the semisimple subalgebras of a semisimple "
    "Lie algebra and creates a data structure containing them. ",
    "ComputeSemisimpleSubalgebras(A_2)",
    "CalculatorFunctions::innerComputeSemisimpleSubalgebras",
    "ComputeSemisimpleSubalgebras",
    innerInvisibleNoTest
  );
  this->AddOperationHandler(
    "CentralizerChains",
    CalculatorFunctions::innerGetCentralizerChainsSemisimpleSubalgebras,
    "",
    "This function is disabled by default (takes too long): you have to first call "
    "TurnRulesOn(CentralizerChains). "
    "Please use this function only if running the calculator on "
    "your own machine; don't use it on a public server. "
    "Creates a printout with centralizer chains of semisimple Lie subalgebras. ",
    "CentralizerChains (ComputeSemisimpleSubalgebras{}(B_3))",
    "CalculatorFunctions::innerGetCentralizerChainsSemisimpleSubalgebras",
    "CentralizerChains",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "PrintSemisimpleSubalgebras",
    Calculator::innerPrintSSsubalgebrasRegular,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b> "
    "Prints the semisimple subalgebras of a semisimple Lie algebra. ",
    "PrintSemisimpleSubalgebras(B_3)",
    "Calculator::innerPrintSSsubalgebrasRegular",
    "PrintSemisimpleSubalgebras",
    innerStandard
  );
  this->AddOperationHandler(
    "PrintSemisimpleSubalgebrasRecompute",
    Calculator::innerPrintSSsubalgebrasRecompute,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b> "
    "Prints the semisimple subalgebras of a semisimple Lie algebra. ",
    "PrintSemisimpleSubalgebrasRecompute(C_3)",
    "Calculator::innerPrintSSsubalgebrasRecompute",
    "PrintSemisimpleSubalgebrasRecompute",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "PrintSemisimpleSubalgebrasNoCentralizers",
    Calculator::innerPrintSSsubalgebrasNoCentralizers,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b> "
    "Prints the semisimple subalgebra candidates of a Lie algebra. ",
    "PrintSemisimpleSubalgebrasNoCentralizers(A_3)",
    "Calculator::innerPrintSSsubalgebrasNoCentralizers",
    "PrintSemisimpleSubalgebrasNoCentralizers",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "PrintSemisimpleSubalgebrasFull",
    Calculator::innerPrintSSsubalgebrasNilradicals,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b>"
    "Prints the semisimple subalgebras of a semisimple Lie algebra. ",
    "PrintSemisimpleSubalgebrasFull{}(A_2)",
    "Calculator::innerPrintSSsubalgebrasNilradicals",
    "PrintSemisimpleSubalgebrasFull",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "CanBeExtendedParabolicallyTo",
    CalculatorFunctions::innerCanBeExtendedParabolicallyTo,
    "",
    "Finds whether a Dynkin type extends via the standard parabolic extension to another. ",
    "CanBeExtendedParabolicallyTo(A^3_1, A_5);"
    "CanBeExtendedParabolicallyTo(a_2, b_3);"
    "CanBeExtendedParabolicallyTo(g_2, b_3);"
    "CanBeExtendedParabolicallyTo(d_5, e_6);"
    "CanBeExtendedParabolicallyTo(b_3, a_10);"
    "CanBeExtendedParabolicallyTo(c_2, c_10);"
    "CanBeExtendedParabolicallyTo(b_3+e_6+a_3, d_4+e_8+f_4)",
    "CalculatorFunctions::innerCanBeExtendedParabolicallyTo",
    "CanBeExtendedParabolicallyTo",
    innerStandard
  );
  this->AddOperationHandler(
    "EmbedSemisimpleInSemisimple",
    CalculatorFunctions::innerEmbedSSalgInSSalg,
    "",
    "Tries to find all embeddings of the first semisimple type into the second. "
    "Records all intermediate subalgebras. ",
    "EmbedSemisimpleInSemisimple{}(G^3_2, B_3);"
    "EmbedSemisimpleInSemisimple{}(G_2, B_3)",
    "CalculatorFunctions::innerEmbedSSalgInSSalg",
    "EmbedSemisimpleInSemisimple",
    innerNoTest
  );
  this->AddOperationHandler(
    "LoadSemisimpleSubalgebras",
    CalculatorConversions::innerLoadSemisimpleSubalgebras,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b>"
    "Loads a semisimpleSubalgebra from expression. ",
    "LoadSemisimpleSubalgebras {}(EmbedSemisimpleInSemisimple{}(G_2, B_3))",
    "CalculatorConversions::innerLoadSemisimpleSubalgebras",
    "LoadSemisimpleSubalgebras",
    innerAdminNoTest
  );
  this->AddOperationHandler(
    "SltwoSubalgebra",
    CalculatorConversions::innerSlTwoSubalgebraPrecomputed,
    "",
    "<b>This function is being developed and is not implemented fully yet. </b>"
    "Loads an sl(2) subalgebra from expression. ",
    "SltwoSubalgebra(\n"
    "B_{3},\n"
    "3 GetChevalleyGenerator((B)_{3}, -2),\n"
    "3 GetChevalleyGenerator( (B)_{3}, 3)\n"
    "+ 6 GetChevalleyGenerator( (B)_{3}, 1)\n"
    "+ 10 / 3 GetChevalleyGenerator( (B)_{3}, 2)\n"
    ")",
    "CalculatorConversions::innerSlTwoSubalgebraPrecomputed",
    "SltwoSubalgebra",
    innerAdminNoTest
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "FactorOneVarPolyOverRationals",
    Calculator::innerFactorPoly,
    "",
    "Factors a one variable polynomial over the rationals "
    "using Kroenecker's method. After clearing denominators, "
    "assume the poly has integer coefficients. If looking "
    "for an integer coefficient divisor of degree k, "
    "plug in k different integer values of the poly and "
    "finds the set of all possible divisors of "
    "the value of poly at the k points. This gives a "
    "finite set of possibilities for the divisors, "
    "as interpolated by Lagrange polynomials.",
    "FactorOneVarPolyOverRationals{}(x^{8}-44x^{6}+438x^{4}- 1292x^{2}+529);\n"
    "FactorOneVarPolyOverRationals{}(x^{8}+2x^{7}-3x^{6}-4x^{5}+6x^{4}+2x^{3}- 13x^{2}+ 1)",
    "Calculator::innerFactorPoly",
    "FactorOneVarPolyOverRationals",
    innerStandard
  );
  this->AddOperationHandler(
    "FactorInteger",
    CalculatorFunctions::innerFactorIntegeR,
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
  this->AddOperationHandler(
    "Freudenthal",
    Calculator::innerFreudenthalEval,
    "",
    "Computes the dominant weights with multiplicities of a "
    "finite dimensional module of a highest weight "
    "given in fundamental coordinates. The first argument gives "
    "the semisimple Lie algebra type, the second argument gives "
    "the highest weight in fundamental coordinates. ",
    "Freudenthal{}(B_3, (2,2,2))",
    "Calculator::innerFreudenthalEval",
    "Freudenthal",
    innerStandard
  );
  this->AddOperationHandler(
    "Killing",
    Calculator::innerKillingForm,
    "",
    "Computes the Killing form product of two elements of semisimple Lie algebra. ",
    "h_{{i}}= GetCartanGenerator{}(F_1, i);"
    "KF{}({{i}},{{j}})=Killing{}(h_i, h_j);"
    "FunctionToMatrix(KF, 4, 4)",
    "Calculator::innerKillingForm",
    "Killing",
    innerStandard
  );
  this->AddOperationHandler(
    "FreudenthalFull",
    Calculator::innerFreudenthalFull,
    "",
    "Computes the full character and prints it to screen. "
    "Argument format same as the Freudenthal function. ",
    "FreudenthalFull{}(G_2, (2, 1))",
    "Calculator::innerFreudenthalFull",
    "FreudenthalFull",
    innerStandard
  );
  this->AddOperationHandler(
    "CoefficientsPowersOf",
    CalculatorFunctions::innerCoefficientsPowersOf,
    "",
    "Extracts the coefficients of the powers of the first argument in the second argument.",
    "CoefficientsPowersOf(x, a x^2+ b *3 x +c +\\pi +3)",
    "CalculatorFunctions::innerCoefficientsPowersOf",
    "CoefficientsPowersOf",
    innerStandard
  );
  this->AddOperationHandler(
    "ConstantTerm",
    CalculatorFunctions::innerConstTermRelative,
    "",
    "Extracts term constant relative to the variable in the first argument. ",
    "ConstantTerm(y,  x y x +3 +2z)",
    "CalculatorFunctions::innerConstTermRelative",
    "ConstantTerm",
    innerStandard
  );
  this->AddOperationHandler(
    "CoefficientOf",
    CalculatorFunctions::innerCoefficientOf,
    "",
    "Gets the coefficient of the first argument in the second. ",
    "CoefficientOf(y, x * x * y + x * z * y * z + x * y * x)",
    "CalculatorFunctions::innerCoefficientOf",
    "CoefficientOf",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "IsProductLinearOrConstantTermsIn",
    CalculatorFunctions::innerIsProductLinearOrConstTermsIn,
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
    "CalculatorFunctions::innerIsProductLinearOrConstTermsIn",
    "IsProductLinearOrConstantTermsIn",
    innerStandard
  );
  this->AddOperationHandler(
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
}

void Calculator::initPredefinedStandardOperations() {
  // IMPORTANT.
  // The order of registration of operation handlers for the same operation name
  // defines the order in which operation
  // handlers are called. Operations handlers registered first are executed first.
  // The order only matters for different handlers of the same operation.
  // This order is very important, as some of the
  // handlers will act properly only if the preceding ones have been carried through
  // (example: outerExtractBaseMultiplication requires outerAssociate).
  // Note that one can easily transform the code so that the order does not matter.
  // One can do that by ``packing'' the the correct order of operations into a super-function,
  // or by making the corresponding handlers call each other as needed.
  // A combination of these two was indeed the original design approach,
  // and gradually evolved to what is used now.
  Function::Options outerStandard;
  outerStandard.flagIsInner = false;
  Function::Options outerAdminInvisibleNoTest;
  outerAdminInvisibleNoTest.flagIsInner = false;
  outerAdminInvisibleNoTest.adminOnly = true;
  outerAdminInvisibleNoTest.dontTestAutomatically = true;
  Function::Options innerStandard;
  innerStandard.flagIsInner = true;
  Function::Options innerExperimentalNoTest;
  innerExperimentalNoTest.flagIsInner = true;
  innerExperimentalNoTest.flagIsExperimental = true;
  innerExperimentalNoTest.dontTestAutomatically = true;
  Function::Options innerNoTestInvisibleExperimental;
  innerNoTestInvisibleExperimental.flagIsInner = true;
  innerNoTestInvisibleExperimental.flagIsExperimental = true;
  innerNoTestInvisibleExperimental.dontTestAutomatically = true;
  innerNoTestInvisibleExperimental.visible = false;

  this->AddOperationHandler(
    ";",
    Calculator::outerMeltBrackets,
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
  this->AddOperationHandler(
    "=",
    Calculator::outerCheckRule,
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
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat,
    this->opRational(),
    this->opEltZmodP(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat",
    "AddRationalToElementZmodP",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat,
    this->opEltZmodP(),
    this->opRational(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat",
    "AddElementZmodPToRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat,
    this->opEltZmodP(),
    this->opEltZmodP(),
    "Adds elements of Z_p. ",
    "(2 mod 7) + 3",
    "CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat",
    "AddElementZmodPToElementZmodP",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber,
    this->opAlgNumber(),
    this->opAlgNumber(),
    "Adds two algebraic numbers. ",
    "\\sqrt {2}+ \\sqrt {3} + \\sqrt{6}",
    "CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber",
    "AddAlgebraicNumberToAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber,
    this->opAlgNumber(),
    this->opRational(),
    "Adds algebraic number to rational. ",
    "1/(\\sqrt {2}+ 1+\\sqrt{3}+\\sqrt{6})",
    "CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber",
    "AddAlgebraicNumberToRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber,
    this->opRational(),
    this->opAlgNumber(),
    "Adds rational to algebraic number. ",
    "1/(1+\\sqrt {2}+\\sqrt{}6)",
    "CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber",
    "AddRationalToAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat,
    this->opDouble(),
    this->opRational(),
    "Adds double or rational to a double or rational approximately "
    "using the built-in cpp addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat",
    "AddDoubleToRational",
    innerStandard
  );
  // must come before outer plus:
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationHandler(
    "ScaleToLeadingUnit",
    CalculatorFunctions::innerScaleToLeadingUnit,
    "",
    "Rescales an expression over the rationals so that the leading term has coefficient 1. ",
    "ScaleToLeadingUnit(1/2 x + 1/3 y+ 1/7 a b)",
    "Calculator::innerScaleToLeadingUnit",
    "ScaleToLeadingUnit",
    innerStandard
  );
  this->AddOperationHandler(
    "+",
    Calculator::outerPlus,
    "",
    "Collects all terms (over the rationals), adding up terms "
    "proportional up to a rational number. "
    "Zero summands are removed, unless zero is the only term left. ",
    "1+a-2a_1+ 1/2+a_1",
    "Calculator::outerPlus",
    "AddTerms",
    outerStandard,
    "+"
  );
  this->AddOperationHandler(
    "+",
    CalculatorFunctions::outerCombineFractionsCommutative,
    "",
    "Combines fractions on condition that all participants commute. "
    "Equivalent to {{a}}/{{b}}+{{c}}/{{d}}= (a *d+c*b)/(d*b); "
    "Please note that this transformation is not correct if b and d do not commute. ",
    "a / b + c / d",
    "CalculatorFunctions::outerCombineFractionsCommutative",
    "CommonDenominator",
    outerStandard
  );
  this->AddOperationHandler(
    "+",
    this->outerCombineFractions,
    "",
    "Combines fractions. Equivalent to {{a}}/{{b}}+{{c}}= (a +c*b)/b; ",
    "f{}{{x}}= (2x +3)/(2x + 1);\ng{}{{y}}= (y-2)/(y+3);\ng{}f{}z;\nf{}g{}z",
    "Calculator::outerCombineFractions",
    "CommonDenominatorOneNonFraction",
    outerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor,
    this->opElementTensorGVM(),
    this->opElementTensorGVM(),
    "Adds two elements of tensor products of generalized Verma modules. ",
    "v=HeighestWeightVector{}(G_2, (1,0),(0,0));\n"
    "(3/4 v)\\otimes v-3/4 (v\\otimes v)",
    "CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor",
    "AddElementTensorGeneralizedVermaModuleToElementTensorGeneralizedVermaModule",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opRational(),
    this->opPoly(),
    "Adds a rational to a polynomial. ",
    "1+Polynomial{}\\lambda; Polynomial{}\\lambda + 1",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddRationalToPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPoly(),
    this->opRational(),
    "Adds a polynomial to a rational. ",
    "1+Polynomial{}\\lambda; Polynomial{}\\lambda + 1",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialToRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPoly(),
    this->opPoly(),
    "Adds a polynomial to a polynomial. ",
    "x = 1 + Polynomial{} \\lambda;\n"
    "x + x",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialToPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPolyOverANs(),
    this->opPolyOverANs(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) + PolynomialAlgebraicNumbers(-\\sqrt{3}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialOverAlgebraicNumbersToPolynomialOverAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opAlgNumber(),
    this->opPolyOverANs(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}) +PolynomialAlgebraicNumbers(\\sqrt{3}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddAlgebraicNumberToPolynomialOverAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPolyOverANs(),
    this->opAlgNumber(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) +PolynomialAlgebraicNumbers(-\\sqrt{3})",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialOverAlgebraicNumbersToAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opRational(),
    this->opPolyOverANs(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "1 + PolynomialAlgebraicNumbers(\\sqrt{12}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddRationalToPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPolyOverANs(),
    this->opRational(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{12}x) + 1",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialAlgebraicNumbersToRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPoly(),
    this->opAlgNumber(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(x) +\\sqrt{2}",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialToAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPoly(),
    this->opPolyOverANs(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "Polynomial{}x + PolynomialAlgebraicNumbers(\\sqrt{2}x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialToPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opPolyOverANs(),
    this->opPoly(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers(\\sqrt{2}x) + Polynomial{}x",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddPolynomialAlgebraicNumbersToPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly,
    this->opAlgNumber(),
    this->opPoly(),
    "Adds a polynomial over the algebraic numbers to a polynomial over the algebraic numbers. ",
    "\\sqrt{2}+PolynomialAlgebraicNumbers(x)",
    "CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly",
    "AddAlgebraicNumberToPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA,
    this->opRational(),
    this->opElementWeylAlgebra(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    " \\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\nx_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA",
    "AddRationalToElementWeylAlgebra",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA,
    this->opPoly(),
    this->opElementWeylAlgebra(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\nx_i +\\partial_i +x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA",
    "AddPolynomialToElementWeylAlgebra",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA,
    this->opElementWeylAlgebra(),
    this->opPoly(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    " \\partial_{{i}} = ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\nx_i +x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA",
    "AddElementWeylAlgebraToPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA,
    this->opElementWeylAlgebra(),
    this->opElementWeylAlgebra(),
    "Adds a rational or polynomial to element Weyl algebra. ",
    " \\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\nx_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA",
    "AddElementWeylAlgebraToElementWeylAlgebra",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF,
    this->opRationalFunction(),
    this->opRationalFunction(),
    "Adds a rational function to a rational function. ",
    "WeylDimFormula{}(a_2, (0,3)) + WeylDimFormula{}(a_2, (3,0)) + 4 WeylDimFormula{}(a_2, (1,1)) ",
    "CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF",
    "AddRationalFunctionToRationalFunction",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat,
    this->opRational(),
    this->opDouble(),
    "Adds double or rational to a double or rational approximately using the "
    "built-in cpp addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat",
    "AddRationalToDouble",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat,
    this->opDouble(),
    this->opDouble(),
    "Adds double or rational to a double or rational approximately using the built-in cpp "
    "addition, returning double. ",
    "DoubleValue{}(3.14159265358979323846) + 1",
    "CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat",
    "AddDoubleToDouble",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddWeightToWeight,
    this->opWeightLieAlgPoly(),
    this->opWeightLieAlgPoly(),
    "Adds two weights. ",
    "\\lambda = Polynomial{}\\lambda; "
    "\\varepsilon_{{a}} = MakeWeight{}(B_3, a, epsilon); "
    "(1 / 2 + \\lambda) \\varepsilon_1 + 1 / 2 \\varepsilon_2+ 1 / 2 \\varepsilon_3  ",
    "CalculatorFunctionsBinaryOps::innerAddWeightToWeight",
    "AddWeightLieAlgebraPolynomialToWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddUEToAny,
    this->opElementUEoverRF(),
    this->opElementUEoverRF(),
    "Adds an element of UE (Universal Enveloping algebra) to an element of UE.",
    "g_{{{i}}} = GetChevalleyGenerator{}(F_{1}, {{i}});\n"
    "h_{{{i}}} = GetCartanGenerator{}(F_{1}, {{i}});\n"
    "[g_{22}+g_{20}+g_{14},g_{- 14}+g_{-20}+g_{-22}]",
    "CalculatorFunctionsBinaryOps::innerAddUEToAny",
    "AddElementUniversalEnvelopingRationalFunctionCoefficientsToElementUniversalEnvelopingRationalFunctionCoefficients",
    innerStandard
  );
  this->AddOperationHandler(
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

  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix,
    "",
    "Multiplies matrices.",
    "\\begin{pmatrix} 1&  2 \\\\ 3& 5\\end{pmatrix}"
    "\\begin{pmatrix} 1& -2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix",
    "MultiplyMatrixByMatrix",
    innerStandard
  );
  this->AddOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix,
    "",
    "Multiplies matrices.",
    "(1,2)\\begin{pmatrix} 1& 2 \\\\ 3& 5\\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix",
    "MultiplySequenceByMatrix",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor,
    this->opMatTensorRat(),
    this->opMatTensorRat(),
    "Adds two matrices.",
    "A = MakeMatrixTensorForm{}((5, 8), (3, 5));\n"
    "3A * A - A;",
    "CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor",
    "AddMatrixTensorToMatrixTensor",
    innerStandard
  );
  this->AddOperationHandler(
    "-",
    Calculator::outerMinus,
    "",
    "Transforms a - b to a +(- 1) * b and - b to (- 1) * b. Equivalent to a rule "
    "-{{b}}=MinusOne * b; {{a}}-{{b}}=a + MinusOne * b", "- 1 + (- 5)",
    "Calculator::outerMinus",
    "Minus",
    outerStandard
  );
  this->AddOperationHandler(
    "*",
    CalculatorFunctions::innerInterpretAsDifferential,
    "",
    "If circumstances imply it, interprets an atom of the form dx as Differential {} x. ",
    "(\\int x) dx; \\int x (1 + x) dx; \\int_2^3 x dx; \\int_2^3 x (1 + x) dx",
    "CalculatorFunctions::innerInterpretAsDifferential",
    "InterpretAsDifferential",
    innerStandard
  );
  this->AddOperationHandler(
    "*",
    CalculatorFunctions::innerIntegralOperator,
    "",
    "Transforms integral notation into an integral expression. ",
    "(\\int x)dx;\n"
    "\\int x (1+x) dx;\n"
    "\\int_2^3 x dx;\n"
    "\\int_2^3 x(1+x)dx",
    "CalculatorFunctions::innerIntegralOperator",
    "IntegralOperatorFromProduct",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerHandleUnderscorePowerLimits,
    "",
    "Handles expressions of the form \\limits_a^b",
    "\\limits_a^b; \\limits^b_a",
    "CalculatorFunctions::innerHandleUnderscorePowerLimits",
    "LimitBoundaryNotationPower",
    innerStandard
  );
  this->AddOperationHandler(
    "_",
    CalculatorFunctions::innerHandleUnderscorePowerLimits,
    "",
    "Handles expressions of the form \\limits_a^b",
    "\\limits_a^b; \\limits^b_a",
    "CalculatorFunctions::innerHandleUnderscorePowerLimits",
    "LimitBoundaryNotationUnderscore",
    innerStandard
  );
  this->AddOperationHandler(
    "*",
    CalculatorFunctions::outerDifferentiateWRTxTimesAny,
    "",
    "Replaces Differentiate{}(x)*a by Differentiate{}(x, a).",
    "d/dx (1/x) ",
    "CalculatorFunctions::outerDifferentiateWRTxTimesAny",
    "DifferentiateWithRespectToXTimeAny",
    outerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct,
    this->opElementHyperOctahedral(),
    this->opElementHyperOctahedral(),
    "Multiplies two elements of hyperoctahedral groups. ",
    "s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0); "
    "t = MakeElementHyperOctahedral{}((1, 3), 0, 0, 0); "
    "s * t * s * t",
    "CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct",
    "MultiplyElementHyperOctahedralByElementHyperOctahedral",
    innerExperimentalNoTest
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber,
    this->opAlgNumber(),
    this->opAlgNumber(),
    "Multiplies two algebraic numbers. ",
    "\\sqrt{}2(\\sqrt {2}* \\sqrt {3} +\\sqrt{}2)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber",
    "MultiplyAlgebraicNumberByAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber,
    this->opRational(),
    this->opAlgNumber(),
    "Multiplies two algebraic number by rational. ",
    "2(\\sqrt {2}+\\sqrt{}3)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber",
    "MultiplyRationalByAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatByRat,
    this->opRational(),
    this->opRational(),
    "Multiplies two rationals. ",
    "2 * 3",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatByRat",
    "MultiplyRationals",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat,
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
    "CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat",
    "MultiplyRationalByDouble",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat,
    this->opDouble(),
    this->opRational(),
    "Multiplies rational by a double approximately using the built-in cpp "
    "multiplication returning double. The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "DoubleValue{}((101)^{20});\n"
    "DoubleValue{}(DoubleValue{}((101)^{20}) + DoubleValue{}(1)) - DoubleValue{}(101^{20})",
    "CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat",
    "MultiplyDoubleByRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat,
    this->opDouble(),
    this->opDouble(),
    "Multiplies rational by a double approximately using the built-in cpp multiplication "
    "returning double. The cpp multiplication is supposed to call the system's "
    "hardware double multiplication routine. ",
    "DoubleValue{}(1/3)*3;\n"
    "DoubleValue{}((101)^{20});\n"
    "DoubleValue{}(DoubleValue{}((101)^{20}) +DoubleValue{}(1))-DoubleValue{}(101^{20})",
    "CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat",
    "MultiplyDoubleByDouble",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt,
    this->opWeylGroupElement(),
    this->opWeylGroupElement(),
    "Multiplies two Coxeter elements if possible. ",
    "x = MakeElementWeylGroup{}(A_2, 1); x*x",
    "CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt",
    "MultiplyWeylGroupElementByWeylGroupElement",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg,
    this->opCharSSAlgMod(),
    this->opCharSSAlgMod(),
    "Multiplies two semisimple Lie algebra finite dimensional characters and decomposes using the "
    "Brauer-Klimyk formula, Humphreys J. Introduction to Lie algebras and representation theory, "
    "page 142, exercise 9. ",
    "x = MakeCharacterLieAlg{}(G_2, (1,0));\n"
    "y = MakeCharacterLieAlg{}(G_2, (0,1));\n"
    "x * y",
    "CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg",
    "MultiplyCharacterSemisimpleLieAlgebraModuleBy",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA,
    this->opRational(),
    this->opElementWeylAlgebra(),
    "Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "3\\partial_i",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA",
    "MultiplyRationalByElementWeylAlgebra",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA,
    this->opPoly(),
    this->opElementWeylAlgebra(),
    "Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA",
    "MultiplyPolynomialByElementWeylAlgebra",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA,
    this->opElementWeylAlgebra(),
    this->opElementWeylAlgebra(),
    "Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "a = x_1 x_2;\n"
    "b = \\partial_1 \\partial_2; a b - b a -[a,b] ",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA",
    "MultiplyElementWeylAlgebraByElementWeylAlgebra",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA,
    this->opElementWeylAlgebra(),
    this->opPoly(),
    "Multiplies rational or polynomial or element Weyl algebra by rational or polynomial or element Weyl algebra. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i;\n"
    "x_i\\partial_i-\\partial_i x_i-[x_i, \\partial_i]",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA",
    "MultiplyElementWeylAlgebraByPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opPoly(),
    this->opRational(),
    "Multiplies polynomial by a rational (polynomial comes first). ",
    "2*Polynomial{}(a +b);\n"
    "Polynomial{}(a +b)/2;\n"
    "Polynomial{}((a +b)^3)*Polynomial{}((a +c)^3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyPolynomialByRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opRational(),
    this->opPoly(),
    "Multiplies rational by a polynomial (rational comes first). ",
    "2*Polynomial{}(a + b);\n"
    "Polynomial{}(a + b)/2;\n"
    "Polynomial{}((a + b)^3) * Polynomial{}((a + c)^3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyRationalByPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opPoly(),
    this->opPoly(),
    "Multiplies two polynomials. ",
    "2*Polynomial{}(a +b);\n"
    "Polynomial{}(a +b)/2;\n"
    "Polynomial{}((a +b)^3) * Polynomial{}((a +c)^3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyPolynomialByPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opPolyOverANs(),
    this->opPolyOverANs(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*PolynomialAlgebraicNumbers{}(\\sqrt{6}y);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyPolynomialAlgebraicNumbersByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opPolyOverANs(),
    this->opAlgNumber(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*\\sqrt{6};",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyPolynomialAlgebraicNumbersByAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opAlgNumber(),
    this->opPolyOverANs(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "\\sqrt{6}*PolynomialAlgebraicNumbers{}(\\sqrt{3}x);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyAlgebraicNumberByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opRational(),
    this->opPolyOverANs(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "PolynomialAlgebraicNumbers{}(\\sqrt{3}x)*2;",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyRationalByPolynomialAlgebraicNumbers",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opPoly(),
    this->opAlgNumber(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "Polynomial{}(x)*\\sqrt{2};",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyPolynomialByAlgebraicNumber",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly,
    this->opAlgNumber(),
    this->opPoly(),
    "Multiplies two polynomials over the algebraic numbers. ",
    "\\sqrt{3} * PolynomialAlgebraicNumbers{}(x);",
    "CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly",
    "MultiplyAlgebraicNumberByPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor,
    this->opMatTensorRat(),
    this->opMatTensorRat(),
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor,
    this->opRational(),
    this->opMatTensorRat(),
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps,
    this->opWeylGroupVirtualRep(),
    this->opWeylGroupVirtualRep(),
    "Tensor and decompose two virtual Weyl group representations. ",
    "W = WeylGroupNaturalRep{}(B_3);\n"
    "V = MakeVirtualWeylGroupRepresentation{}W;\n"
    "W\\otimes W;\n"
    "V * V",
    "CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps",
    "TensorAndDecomposeWeylGroupRepresentations",
    innerExperimentalNoTest
  );
  this->AddOperationHandler(
    "*",
    Calculator::innerMultiplyAtoXtimesAtoYequalsAtoXplusY,
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
  this->AddOperationHandler(
    "*",
    Calculator::innerMultiplyByOne,
    "",
    "Rule 1*{{anything}} = anything.",
    "x*1;\n"
    "x*1-x ",
    "Calculator::innerMultiplyByOne",
    "MultiplyByOne",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "*",
    CalculatorFunctions::outerMergeConstantRadicals,
    "",
    "If a and b are constants, replaces a^{c}b^c by (a b)^c.",
    "\\sqrt{}2 \\sqrt{}3",
    "CalculatorFunctions::outerMergeConstantRadicals",
    "MergeConstantRadicals",
    outerStandard
  );
  this->AddOperationHandler(
    "*",
    Calculator::outerAssociate,
    "",
    "Associative law: reorders the multiplicative tree in standard form. ",
    "(a*b)*(c*(d*e)*f) - a*b*c*d* e *f;(a*b)*(c*(e * d)*f) - a*b*c*d* e *f",
    "Calculator::outerAssociate",
    "AssociativeRule",
    outerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "*",
    Calculator::outerExtractBaseMultiplication,
    "",
    "Pulls rationals in the front of multiplicative terms.",
    "2*((3*c)*(4*d)); 3*((a*(d-d))b*c)",
    "Calculator::outerExtractBaseMultiplication",
    "ConstantExtraction",
    outerStandard
  );
  this->AddOperationHandler(
    "*",
    Calculator::outerAssociateTimesDivision,
    "",
    "Associative law w.r.t. division. ",
    "a*(b/c); (a*b)/c-a*(b/c)",
    "Calculator::outerAssociateTimesDivision",
    "AssociateTimesDivision",
    outerStandard
  );
  this->AddOperationHandler(
    "*",
    Calculator::innerCancelMultiplicativeInverse,
    "",
    "Cancels multiplicative inverse. ",
    "(a*b)/b; (a/b)*b",
    "Calculator::innerCancelMultiplicativeInverse",
    "CancelMultiplicativeInverse",
    innerStandard
  );
  this->AddOperationHandler(
    "*",
    Calculator::outerDistributeTimes,
    "",
    "Distributive law (left and right).",
    "(a + b) * c;\n"
    "a * (b + c)",
    "Calculator::outerDistributeTimes",
    "DistributeMultiplication",
    outerStandard
  );
  this->AddOperationHandler(
    "*",
    Calculator::outerDistributeTimesConstant,
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
  this->AddOperationHandler(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE,
    this->opRational(),
    this->opElementUEoverRF(),
    "Multiplies rational number by an element universal enveloping algebra.",
    "g_{{i}}= GetChevalleyGenerator{}(F_1, i); h_{{i}}= GetCartanGenerator{}(F_1, i);\n"
    "[g_{22}+g_{20}+g_{14},g_{17}-6/5g_{14}]",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE",
    "MultiplyRationalByUE",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE,
    this->opElementUEoverRF(),
    this->opElementUEoverRF(),
    "Multiplies elment Universal enveloping by element universal enveloping algebra.",
    "g_{{i}}= GetChevalleyGenerator{}(F_1, i);"
    "h_{{i}}= GetCartanGenerator{}(F_1, i) ;\n"
    "[g_{22}+g_{20}+g_{14},g_{17}-6/5g_{14}]",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE",
    "MultiplyUEByUE",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF,
    this->opRational(),
    this->opRationalFunction(),
    "Multiplies rational number by a rational function.",
    "WeylDimFormula{}(a_2, (0,3)) + WeylDimFormula{}(a_2, (3,0)) + 4 WeylDimFormula{}(a_2, (1,1)) ",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF",
    "MultiplyRatOrPolyOrRFByRF",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opRational(),
    this->opElementTensorGVM(),
    "Handles multiplying rational number by an element of "
    "tensor product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\nh_{{i}}= GetCartanGenerator{}(X, i);\n"
    "v=HeighestWeightVector{}(G_2, (1,0),(0,0));\n"
    "2/5 v;\n(3/4 v)\\otimes v;\n3/4 (v\\otimes v);\n(3/4 v)\\otimes v-3/4 (v\\otimes v)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyAnyByEltTensor",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opPoly(),
    this->opElementTensorGVM(),
    "Handles multiplying polynomial by an element of tensor "
    "product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\n"
    "g_{{i}}= GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X, i);\n"
    "z = Polynomial{}y;\n"
    "v = HeighestWeightVector{}(G_2, (z,1),(1,0));\n"
    "(2*z) v;\n",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyPolynomialByElementTensorGeneralVermaModule",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements,
    this->opEllipticCurveElementsRational(),
    this->opEllipticCurveElementsRational(),
    "Multiplies two elements of elliptic curves.",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);\n"
    "h = g^2;\n"
    "h * g\n",
    "CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements",
    "MultiplyEllipticCurveElementsRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElementsZmodP,
    this->opEllipticCurveElementsZmodP(),
    this->opEllipticCurveElementsZmodP(),
    "Multiplies two elements of elliptic curves.",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 +x - 5, x = 3 mod 17, y = 5 mod 17);\n"
    "h = g^2;\n"
    "h * g\n",
    "CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements",
    "MultiplyEllipticCurveElementsOverZmodP",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opRationalFunction(),
    this->opElementTensorGVM(),
    "Handles multiplying rational function number by an element of tensor product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\n"
    "g_{{i}} = GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}} = GetCartanGenerator{}(X, i);\n"
    "z = Polynomial{}y;\n"
    "v = HeighestWeightVector{}(G_2, (z,1),(1,0));\n"
    "1/z v",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyAnyByTensor",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opElementUEoverRF(),
    this->opElementTensorGVM(),
    "Handles acting by element Universal enveloping on an element of "
    "tensor product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\nh_{{i}}= GetCartanGenerator{}(X, i);\n"
    "z= Polynomial{}y;\nv=HeighestWeightVector{}(G_2, (z,1),(1,0));\n"
    "h_1 v; \nh_2 v;\n g_1 g_{- 1} v ",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyElementUniversalEnvelopingRationalFunctionByElementTensorGeneralizedVermaModule",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationHandler(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix,
    "",
    "Multiplies a rational number by a matrix",
    "2\\begin{pmatrix}  1& 2& 3\\\\2& 1& 0 \\end{pmatrix}",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix",
    "ScalarTimesMatrix",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence,
    this->opAlgNumber(),
    this->opSequence(),
    "Multiplies a double number on left and a sequence on the right.scalar.",
    "(1 ,2)- DoubleValue{} 1 (2,3)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence",
    "MultiplyAlgebraicNumberBySequence",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence,
    this->opPoly(),
    this->opSequence(),
    "Carries out multiplication between a rational number on left "
    "and sequence on the right.",
    "x = Polynomial{}x; v=x*(1, 2, 3);",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence",
    "MultiplyPolynomialBySequence",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly,
    this->opWeylGroupElement(),
    this->opWeightLieAlgPoly(),
    "Element of Weyl group action on a weight. ",
    "s_{{a}}=MakeElementWeylGroup(B_3, a);\n"
    "\\varepsilon_{{a}} = MakeWeight{}(B_3, a, epsilon);\n"
    "x = Polynomial{}x;\n"
    "\\mu = x\\varepsilon_1;\n"
    "s_1s_2s_3s_2s_1 \\mu",
    "CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly",
    "MultiplyWeylGroupElementByWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly,
    this->opPoly(),
    this->opWeightLieAlgPoly(),
    "Carries out multiplication between a rational or polynomial "
    "on left and a weight on the right.",
    "\\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon); "
    "x = Polynomial{}x; x\\varepsilon_1",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly",
    "MultiplyPolynomialByWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly,
    this->opRational(),
    this->opWeightLieAlgPoly(),
    "Carries out multiplication between a rational or polynomial "
    "on left and a weight on the right.",
    "\\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon); "
    "x = Polynomial{}x; x\\varepsilon_1",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly",
    "MultiplyRaitonalByWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->AddOperationHandler(
    "\\binom",
    CalculatorFunctionsBinaryOps::innerNChooseK,
    "",
    "Evaluates the binomial coefficient if possible.",
    "\\binom{8}{3} ",
    "CalculatorFunctionsBinaryOps::innerNChooseK",
    "Binom",
    innerStandard
  );
  this->AddOperationHandler(
    "ElementEllipticCurveNormalForm",
    CalculatorFunctions::innerElementEllipticCurveNormalForm,
    "",
    "Makes an elliptic curve from a cubic in normal form, generator letter and base point.",
    "ElementEllipticCurveNormalForm(y^2 = x^3 + x + 7, x = 3 mod 101, y = 21 mod 101);"
    "ElementEllipticCurveNormalForm(y^2 = x^3 - x + 1, x = 3, y = 5)",
    "CalculatorFunctions::innerElementEllipticCurveNormalForm",
    "ElementEllipticCurveNormalForm",
    innerStandard
  );
  this->AddOperationHandler(
    "mod",
    Calculator::innerZmodP,
    "",
    "Same as ModP but uses the mod notation.",
    "7 mod 3",
    "Calculator::innerZmodP",
    "mod",
    innerStandard
  );
  this->AddOperationHandler(
    "\\setminus",
    CalculatorFunctionsBinaryOps::innerSetMinus,
    "",
    "Removes the elements of the second set from the elements of the first set. "
    "The outputs will be sorted in ascending order.",
    " (x,y,t) \\setminus Sequence{}x; (x,y)\\setminus (z,y)",
    "CalculatorFunctionsBinaryOps::innerSetMinus",
    "\\setminus",
    innerStandard
  );
  this->AddOperationHandler(
    "Differential",
    CalculatorFunctions::innerDifferentialStandardHandler,
    "",
    "Transforms Differential{}a to the standard internal form Differential {}(a, 1).",
    "\\int \\theta {\\text d} \\theta",
    "CalculatorFunctionsBinaryOps::innerDifferentialStandardHandler",
    "DifferentialStandardHandler",
    innerStandard
  );
  this->AddOperationHandler(
    "/",
    CalculatorFunctions::innerIntegralOperator,
    "",
    "Transforms integral notation into an integral expression. ",
    "(\\int x)dx; \\int x (1+x) dx; \\int_2^3 x dx; \\int_2^3 x(1+x)dx",
    "CalculatorFunctions::innerIntegralOperator",
    "IntegralOperatorFromQuotient",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationHandler(
    "/",
    CalculatorFunctions::innerDiffdivDiffxToDifferentiation,
    "",
    "Replaces Differential / Differential {}x by Differentiate{}(x). "
    "Should also work on the notation (Differential y)/(Differential x). ",
    "Differential /Differential {}x x",
    "CalculatorFunctions::innerDiffdivDiffxToDifferentiation",
    "LeibnizDifferentialOperatorToDifferentialOperator",
    innerStandard
  );
  this->AddOperationHandler(
    "/",
    CalculatorFunctions::innerDdivDxToDiffDivDiffx,
    "",
    "Replaces d/dx by Differential /Differential{}x. "
    "Note that the variable of differentiation is expected to be the string following the d letter. ",
    "d/dx x",
    "CalculatorFunctions::innerDdivDxToDiffDivDiffx",
    "LeibnizDifferentialOperatorNotation",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat,
    this->opAlgNumber(),
    this->opAlgNumber(),
    "Divides algebraic numbers. ",
    "1/(1+\\sqrt{}2+\\sqrt{}3+\\sqrt{}5+\\sqrt{}7)",
    "CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat",
    "DivideAlgebraicByAlgebraic",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat,
    this->opRational(),
    this->opAlgNumber(),
    "Divides rational by algebraic number. ",
    "1/(\\sqrt{}2+\\sqrt{}3+\\sqrt{}5)",
    "CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat",
    "DivideRationalByAlgebraic",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat,
    this->opAlgNumber(),
    this->opRational(),
    "Divides algebraic number by rational. ",
    "(\\sqrt{}2+\\sqrt{}3+\\sqrt{}5)/5",
    "CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat",
    "DivideAlgebraicByRational",
    innerStandard
  );
  this->AddOperationHandler(
    "/",
    CalculatorFunctions::outerDivideByNumber,
    "",
    "If b is rational, algebraic, or double, substitutes (anything)/b with anything* (1/b).",
    "6/15+(a +b)/5;\n"
    "a/\\sqrt{}2;\n"
    "x/DoubleValue{}10^10;x/DoubleValue{}5;\n"
    "6/4+3/0",
    "CalculatorFunctions::outerDivideByNumber",
    "DivideByNumber",
    outerStandard
  );
  this->AddOperationHandler(
    "/",
    Calculator::innerSubZeroDivAnythingWithZero,
    "",
    "Provided that x is not equal to zero, substitutes 0/x with 0. ",
    "0/b; ",
    "Calculator::innerSubZeroDivAnythingWithZero",
    "DivideZeroDivideByAnything",
    innerStandard
  );
  this->AddOperationHandler(
    "/",
    CalculatorFunctions::outerDivCancellations,
    "",
    "Division cancellations. Substitutes (a/b)/(a/d) with d/a and (a/b)/(c/b) with a/c. ",
    "(a/b)/(a/d); (a/b)/(c/b)",
    "CalculatorFunctions::outerDivCancellations",
    "DivideCancellations",
    outerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly,
    this->opRational(),
    this->opPoly(),
    "Divides rational by polynomial (to get a rational function).",
    "z = Polynomial{}(x^2+y^2);\n1/z",
    "CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly",
    "DivideRationalByPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "/",
    CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly,
    this->opPoly(),
    this->opPoly(),
    "Divides polynomial by polynomial (to get a rational function). ",
    "Polynomial{}(-x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}+x_{2}+ 1)/\n"
    "Polynomial{}(x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}-x_{2}+ 1) ",
    "CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly",
    "DividePolynomialByPolynomial",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors,
    this->opRational(),
    this->opRational(),
    "If a rational number is small enough to factor, reduces the "
    "rational exponents of the rational number. ",
    "\n%LogEvaluation (4/9)^{17/3}; (12/7)^{7/2}; 12^{2/3}; 12^{-2/3}; 5^{- 1/3}; 5^{- 1/3}-\\sqrt[3]{5^- 1} ; \\sqrt[20]{200!}",
    "CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors",
    "PowerRationalByRational",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat,
    this->opRational(),
    this->opDouble(),
    "Calls the built-in cpp functions to approximately raise a double to a power, "
    "provided either the base or the exponent is a double, and provided that "
    "the base is non-negative. ",
    "f{}{{x}}=x^3+p x +q;\n"
    "Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) +( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);\n"
    "q = DoubleValue{}1; \np = DoubleValue{}1; \nXcardano; \nf{}x; \nf{}Xcardano   ",
    "CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat",
    "PowerDoubleToDouble",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerMatNumbersBySmallInteger,
    "",
    "If the base is a matrix and the exponent is a rational number: "
    "1. If the base is not square, returns error. "
    "2. If the base is square and consists of "
    "rational or algebraic numbers and the exponent is a "
    "small integer, raises the base to the corresponding power. "
    "If the power is 0 or negative and the determinant of "
    "the matrix is zero, returns error. ",
    "X =\\begin{pmatrix} 0 & 1 \\\\ 1 &1\\end{pmatrix}; X^5; X^{-5}  ",
    "CalculatorFunctionsBinaryOps::innerPowerMatBySmallInteger",
    "PowerMatrixNumbersByInteger",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerMatNumbersByLargeIntegerIfPossible,
    "",
    "Attempts to exponentiate matrix by a large power. ",
    "X =\\begin{pmatrix} 1 & -1 \\\\ 4 & -3\\end{pmatrix}; X^{-55!- 1}  ",
    "CalculatorFunctionsBinaryOps::innerPowerMatNumbersByLargeIntegerIfPossible",
    "PowerMatrixNumbersByLargeIntegerIfPossible",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerMatExpressionsBySmallInteger,
    "",
    "Attempts to exponentiate a matrix of expressions, if the exponent is small. ",
    "X =\\begin{pmatrix} a & b \\\\ t +q &r\\end{pmatrix}; X^{2}  ",
    "CalculatorFunctionsBinaryOps::innerPowerMatExpressionsBySmallInteger",
    "PowerMatrixNumbersBySmallIntegerIfPossible",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementZmodPByInteger,
    "",
    "Raises an element of Z mod p to an integer power. ",
    "a = 3 mod 7; a^2",
    "CalculatorFunctions::innerPowerElementZmodPByInteger",
    "PowerElementZmodPToInteger",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerPowerExponentToLog,
    "",
    "Replaces e^{\\ln x} by x.",
    "e^{\\ln x}; ",
    "CalculatorFunctions::innerPowerExponentToLog",
    "PowerToLog",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger,
    this->opPoly(),
    this->opRational(),
    "Raises poly to small integer power. ",
    "x = Polynomial{}x; y = Polynomial{}y;(x +2y+x y+x^2+3y^2)^3",
    "CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger",
    "PowerPolynomialBySmallInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerAlgNumPolyBySmallInteger,
    this->opPolyOverANs(),
    this->opRational(),
    "Raises poly over algebraic numbers to small integer power. ",
    "x = Polynomial{}x; y = Polynomial{}y;(x +\\sqrt{2}y+x y+x^2+\\sqrt{3}y^2)^3",
    "CalculatorFunctionsBinaryOps::innerPowerAlgNumPolyBySmallInteger",
    "PowerPolynomialAlgebraicNumbersBySmallInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger,
    this->opEllipticCurveElementsRational(),
    this->opRational(),
    "Exponentiates an elliptic curve element by an integer. ",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);\n"
    "g^2",
    "CalculatorFunctionsBinaryOps::innerPowerEllipticCuveElementByInteger",
    "PowerEllipticCurveElementRationalByInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger,
    this->opEllipticCurveElementsZmodP(),
    this->opRational(),
    "Exponentiates an elliptic curve element by an integer. ",
    "g = ElementEllipticCurveNormalForm(y^2 = x^3 - x +1, x = 3, y = 5);\n"
    "g^2",
    "CalculatorFunctionsBinaryOps::innerPowerEllipticCuveElementByInteger",
    "PowerEllipticCurveElementZmodPByInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger,
    this->opAlgNumber(),
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
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault,
    this->opAlgNumber(),
    this->opRational(),
    "Takes the radical of an algebraic number, if the algebraic number is a positive number"
    "that whose radical lies in the underlying algebraic closure.",
    "\\sqrt{3+2\\sqrt[2]{2}};\n"
    "(7-5\\sqrt{2})^{7/3}",
    "CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger",
    "PowerAlgebraicNumberBySmallInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger,
    this->opElementWeylAlgebra(),
    this->opRational(),
    "Raises element of Weyl algebra to integer power. ",
    "\\partial = ElementWeylAlgebraDO{}(\\partial, x);\n"
    "x = ElementWeylAlgebraPoly{}(\\partial, x); \n"
    "a =x\\partial; \n"
    "a^10;\n"
    "\\partial x^{3/2};\n"
    "\\partial^{3/2} x",
    "CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger",
    "PowerElementWeylAlgebraBySmallInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF,
    this->opElementUEoverRF(),
    this->opRational(),
    "Raises element of universal enveloping to integer power. "
    "If the exponent is non-positive integer but the element of the UE is "
    "a single generator with coefficient 1, the exponent will be carried out formally. ",
    "g_{{i}}= GetChevalleyGenerator{}(G_2, i); h_{{i}}= GetCartanGenerator{}(G_2, i) ;"
    "\n (g_1+g_2)^2+ g_1^{1/2}",
    "CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF",
    "PowerElementUniversalEnvelopingBySmallInteger",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF,
    this->opElementUEoverRF(),
    this->opPoly(),
    "Provided that an element of Universal Enveloping algebra is "
    "a single generator (raised to arbitrary formal polynomial power) with coefficient 1, "
    "raises (formally) the element of the UE to arbitrary polynomial power. ",
    "g_{{i}}= GetChevalleyGenerator{}(G_2, i); h_{{i}}= GetCartanGenerator{}(G_2, i) ;\n"
    "((((g_1)^{Polynomial{}x})^{Polynomial{}y}) +g_2)^2",
    "CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF",
    "PowerUEelementToPolyPower",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF,
    this->opElementUEoverRF(),
    this->opRationalFunction(),
    "Provided that an element of Universal Enveloping algebra is a single generator "
    "(raised to arbitrary formal RF power) with coefficient 1, raises "
    "(formally) the element of the UE to arbitrary RF power. ",
    "g_{{i}}= GetChevalleyGenerator{}(G_2, i); h_{{i}}= GetCartanGenerator{}(G_2, i);\n "
    "((((g_1)^{Polynomial{}x})^{Polynomial{}y}) +g_2)^2",
    "CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF",
    "PowerUEelementToRFPower",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat,
    this->opDouble(),
    this->opRational(),
    "Calls the built-in cpp functions to approximately raise a double to a power, "
    "provided either the base or the exponent is a double. If the base is negative and "
    "the exponent is rational with odd denominator, the exponent is evaluated to the corresponding "
    "real negative root. ",
    "f{}{{x}}=x^3+p x +q;\n"
    "Xcardano = ( -q/2+ (q^2/4+p^3/27)^(1/2))^(1/3) +( -q/2- (q^2/4+p^3/27)^(1/2))^(1/3);\n"
    "q= DoubleValue{}1; \np = DoubleValue{}1; \nXcardano; \nf{}x; \nf{}Xcardano   ",
    "CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat",
    "PowerFloatingToPower",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    Calculator::innerAssociateExponentExponent,
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
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerDistributeExponent,
    "",
    "If a is a positive constant, substitutes (a*b)^c with a^c b^c.",
    "(a*b)^n; (\\sqrt(2)*b)^2",
    "CalculatorFunctions::innerDistributeExponent",
    "DistributeExponent",
    innerStandard
  );
  this->AddOperationHandler(
    "\\sqrt",
    CalculatorFunctions::innerDistributeSqrt,
    "",
    "If a is a positive constant, substitutes sqrt(a b) by sqrt(a) sqrt(b).",
    " TurnOffApproximations 0; \\sqrt{\\frac{676}{25} ln 3}",
    "Calculator::innerDistributeSqrt",
    "innerDistributeSqrt",
    innerStandard
  );
  this->AddOperationHandler(
    "^",
    Calculator::outerPowerRaiseToFirst,
    "",
    "Realizes the tranformation {{anything}}^1=a. ",
    "x^1 + x^2; A^1",
    "Calculator::outerPowerRaiseToFirst",
    "PowerToOne",
    outerStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerOperatorBounds,
    "",
    "Replaces \\int_a^b by (\\int, a, b) .",
    "A =\\int_a^b; Lispify(A); PlotExpressionTree(A); ",
    "CalculatorFunctions::innerIntegralUpperBound",
    "OperatorBoundsSuperscript",
    innerStandard
  );
  this->AddOperationHandler(
    "_",
    CalculatorFunctions::innerOperatorBounds,
    "",
    "Takes care of the integral superscript notation \\int^a ",
    "\\int^a_b f dx; \\int_a^b f dx",
    "Calculator::innerUnderscoreIntWithAny",
    "OperatorBoundsUnderscore",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "\\otimes",
    CalculatorFunctionsBinaryOps::innerTensorMatrixByMatrix,
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
    "CalculatorFunctionsBinaryOps::innerTensorMatrixByMatrix",
    "MatrixTensorMatrix",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "\\otimes",
    CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor,
    this->opElementTensorGVM(),
    this->opElementTensorGVM(),
    "Tensor product of two generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\n"
    "g_{{i}} = GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X, i);\n"
    "z= Polynomial{}y;\n"
    "v=HeighestWeightVector{}(G_2, (z,1), (1,0));\n"
    "g_{- 1}(v\\otimes v);\n"
    "g_{- 1}g_{- 1}(v\\otimes v)",
    "CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor",
    "TensorElementGeneralizedVermaModuleByElementGeneralizedVermaModule",
    innerStandard
  );
  this->AddOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::innerLieBracketDistribute,
    "",
    "Implementation of the rules [{{a}}+{{b}},{{c}}] = [a, c] +[b, c] "
    "and [{{c}},{{a}}+{{b}}] =[c,a] +[c,b]",
    "[a+b, c]",
    "CalculatorFunctionsBinaryOps::innerLieBracketDistribute",
    "LieBracketDistribute",
    innerStandard
  );
  this->AddOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant,
    "",
    "Extracts constants from Lie brackets. ",
    "[2a, 3b]",
    "CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant",
    "LieBracketConstants",
    innerStandard
  );
  this->AddOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA,
    "",
    "Lie bracket of elements of Weyl algebras = differential operators with polynomial coefficients. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i; \n[\\partial_1, x_1]; ",
    "CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA",
    "LieBracketWeylAlgebraElements",
    innerStandard
  );
  this->AddOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE,
    "",
    "Lie bracket of elements of semisimple Lie algebra. ",
    "X =A_1;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X,i);\n"
    "[g_1,g_{- 1}] ",
    "CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE",
    "LieBracketSemisimpleLieAlgebras",
    innerStandard
  );
  this->AddOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded,
    "",
    "Swaps terms in Lie brackets if needed. ",
    "[b, a]",
    "CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded",
    "LieBracketSwapTermsIfNeeded",
    innerStandard
  );
  this->AddOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded,
    "",
    "Extracts constants from Lie brackets. ",
    "[2a,3b] ",
    "CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant",
    "LieBracketJacobiIfNeeded",
    innerStandard
  );

  this->AddOperationBinaryInnerHandlerWithTypes(
    "\\otimes",
    CalculatorFunctionsWeylGroup::innerTensorWeylReps,
    this->opWeylGroupRep(),
    this->opWeylGroupRep(),
    "Tensor product of two Weyl group reps. Does not decompose the tensor product. "
    "If you want decomposition, use V*V instead. ",
    "V = WeylGroupNaturalRep{}(B_3); V \\otimes V; V * V",
    "CalculatorFunctionsWeylGroup::innerTensorWeylReps",
    "TensorWeylGroupRepresentationByWeylGroupRepresentation",
    innerStandard
  );
  this->AddOperationBinaryInnerHandlerWithTypes(
    "\\otimes",
    CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor,
    this->opMatTensorRat(),
    this->opMatTensorRat(),
    "Same as tensor product of matrices but uses class MatrixTensor instead of class Matrix.",
    "P = ((0 , 2 ),(1 , 0));\n"
    "Q = ((0 , 3 ),(1 , 0));\n"
    "X = MakeMatrix{}P;\n"
    "Y = MakeMatrix{}Q;\n"
    "Z = MakeMatrixTensorForm{}P;\n"
    "W = MakeMatrixTensorForm{}Q;\n"
    "X \\otimes Y;\n"
    "Z\\otimes W",
    "CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor",
    "TensorMatrixTensorByMatrixTensor",
    innerStandard
  );
  this->AddOperationHandler(
    "\\otimes",
    Calculator::outerTensorProductStandard,
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
  this->AddOperationHandler(
    "=:",
    Calculator::StandardIsDenotedBy,
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
    "Calculator::StandardIsDenotedBy",
    "StandardIsDenotedBy",
    innerStandard
  );

  std::stringstream StandardPowerStreamInfo, moreInfoOnIntegers;
  moreInfoOnIntegers
  << "LargeIntUnsigned::SquareRootOfCarryOverBound is "
  << "restricted to be smaller than the square root of a "
  << "positive signed int on the system: in this way, multiplying two small integers and "
  << "storing the result in a signed int is guaranteed to produce the correct result. "
  << "Rationals are stored in the computer as 8 bytes+ one pointer (which is another 4 bytes on a 32 bit system). "
  << "The pointer either equals zero, or points to a dynamically resizable structure able to hold "
  << "arbitrary integer sizes (within the system's address space limits). If the pointer is zero, we call  "
  << "such a rational number small. In this case its denominator and numerator are stored in "
  << "the other 8 bytes, and should both be smaller than LargeIntUnsigned::CarryOverBound = "
  << LargeIntegerUnsigned::CarryOverBound
  << ". When requesting an arithmetic operation, if both rationals are small, (i.e. their pointer zero)"
  << " a check is performed whether the denominator and numerator are smaller in absolute value than "
  << "LargeIntUnsigned::SquareRootOfCarryOverBound = "
  << LargeIntegerUnsigned::SquareRootOfCarryOverBound
  << ". If that check passes, the two rationals are multiplied using the "
  << "built-in processor instructions for operations with integers. "
  << "If any of the check fails, both rationals are converted to the larger dynamically "
  << "resizeable structure, sufficient memory is allocated, "
  << "and multiplication/addition is carried using algorithms located in class "
  << "LargeIntUnsigned. The algorithms are O(log(n)^2) (the number of digits squared). "
  << "If the need arises, we may implement "
  << "discrete Fourier transform multiplication algorithm (O(log(n)(log(log n))). ";
  StandardPowerStreamInfo
  << "If the left argument evaluates to atom, "
  << "and if the right argument is a small integer atom, "
  << "tries to carry out the raise-to-power operation. "
  << "If successful, substitutes the expression with the obtained atom. "
  << "A small integer is defined at compile time in the "
  << "variable LargeIntUnsigned::SquareRootOfCarryOverBound (currently equal to "
  << LargeIntegerUnsigned::SquareRootOfCarryOverBound << "). "
  << HtmlRoutines::GetHtmlSpanHidableStartsHiddeN(moreInfoOnIntegers.str());

  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "Max",
    CalculatorFunctions::innerMax,
    "",
    "Maximum function.",
    "Max(- 4,2)",
    "CalculatorFunctions::innerMax",
    "Max",
    innerStandard
  );
  this->AddOperationHandler(
    "Min",
    CalculatorFunctions::innerMin,
    "",
    "Maximum function.",
    "Min(- 4,2)",
    "CalculatorFunctions::innerMin",
    "Min",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "\\leq",
    CalculatorFunctions::innerLessThanOrEqualTo,
    "",
    "Transforms \\(a\\leq b\\) to \\(b\\geq a\\).",
    "a \\leq b",
    "Calculator::innerLessThanOrEqualTo",
    "lessThanOrEqualTo",
    innerStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "Last",
    CalculatorFunctions::innerLastElement,
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
  this->AddOperationHandler(
    "RemoveLast",
    CalculatorFunctions::innerRemoveLastElement,
    "",
    "Returns a list with the last element removed, provided "
    "the argument has no bound variables. If the expression "
    "has bound variables does nothing.",
    "X = (a,b,c);\n"
    "Y = (RemoveLast X)\\cup Sequence{}(Last X) - X;\n"
    "RemoveLast{}(());\n"
    "RemoveLast{}();\n",
    "CalculatorFunctions::innerRemoveLastElement",
    "RemoveLast",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cap",
    CalculatorFunctions::innerIntersection,
    "",
    "Intersects lists. For the time being, the "
    "output order is not specified (will be fixed in the future).",
    "(a,b,c)\\cap (c, d, e);\n"
    "x = (a,b,c)\\cap (c, d, e);\n"
    "a = 1;\n"
    "d =1;\n"
    "(a, b, c)\\cap (c, d, e); x",
    "CalculatorFunctions::innerIntersection",
    "\\cap",
    innerStandard
  );
  this->AddOperationHandler(
    "\\cup",
    CalculatorFunctions::innerUnion,
    "",
    "If all arguments of \\cup are of type list, substitutes the expression with "
    "a list containing the union of all members (with repetition). "
    "If a flag is set requesting that (a,b) is interpreted as an interval, does nothing if "
    "either of the two sequences has two elements.",
    "x\\cup (MakeSequence{} x \\cup MakeSequence{}x \\cup (a,b,x))",
    "CalculatorFunctions::innerUnion",
    "\\cup",
    innerStandard
  );
  this->AddOperationHandler(
    "\\sqcup",
    CalculatorFunctions::innerUnionNoRepetition,
    "",
    "If all arguments of \\sqcup are of type list, substitutes the expression "
    "with a list containing the union of all members; "
    "all repeating members are discarded.",
    "(x,y,x)\\sqcup(1,x,y,2)",
    "CalculatorFunctions::innerUnionNoRepetition",
    "\\sqcup",
    innerStandard
  );
}

void Calculator::initPredefinedOperationsComposite() {
  MacroRegisterFunctionWithName("Calculator::initPredefinedOperationsComposite");
  Function::Options compositeStandard;
  compositeStandard.flagIsCompositeHandler = true;
  compositeStandard.flagIsInner = true;

  this->AddOperationHandler(
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
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "Rational",
    CalculatorFunctions::innerConstantFunction,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;2{}y;(\\sqrt{}2){}x;",
    "CalculatorFunctions::innerConstantFunction",
    "ConstantFunction",
    compositeStandard
  );
  this->AddOperationHandler(
    "RationalFunction",
    CalculatorFunctions::innerRationalFunctionSubstitution,
    "",
    "If x is a constant, replaces x{}({{anything}})=x; ",
    "0{}3;2{}y;(\\sqrt{}2){}x;",
    "CalculatorFunctions::innerRationalFunctionSubstitution",
    "RationalFunctionSubstitution",
    compositeStandard
  );
  this->AddOperationHandler(
    "+",
    CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a +b){}x = (a{}x) +(b{}x) ",
    "(a +b){}x;",
    "CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentAddition",
    compositeStandard
  );
  this->AddOperationHandler(
    "*",
    CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a*b){}x = (a{}x)*(b{}x) ",
    "(a*b){}x;",
    "CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentMultiplication",
    compositeStandard
  );
  this->AddOperationHandler(
    "/",
    CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument,
    "",
    "Equivalent to (a/b){}x = (a{}x)/(b{}x) ",
    "(a/b){}x;",
    "CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument",
    "CompositeArithmeticOperationEvaluatedOnArgumentDivision",
    compositeStandard
  );
  this->AddOperationHandler(
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
  this->AddOperationHandler(
    "ElementWeylAlgebra",
    CalculatorFunctions::innerCompositeEWAactOnPoly,
    "",
    "Differential operation acting on a polynomial. ",
    "x = ElementWeylAlgebraPoly{}(\\partial, x);\\partial = ElementWeylAlgebraDO{}(\\partial, x);\n"
    "\\partial{}(x); \\partial^{2}{}(x^3+x^2); x{}(x^2)",
    "CalculatorFunctions::innerCompositeEWAactOnPoly",
    "EWAactOnPoly",
    compositeStandard
  );
  this->AddOperationHandler(
    "*",
    CalculatorFunctions::innerCompositeConstTimesAnyActOn,
    "",
    "Rule (a*f){}x = a*(f{}x), provided a is a constant. ",
    "(2\\sin){}x-2(\\sin x) ",
    "CalculatorFunctions::innerCompositeConstTimesAnyActOn",
    "ConstTimesAnyAction",
    compositeStandard
  );
  this->AddOperationHandler(
    "^",
    CalculatorFunctions::innerCompositeApowerBevaluatedAtC,
    "",
    "Provided that n is not equal to - 1 and f is not a sequence, use the rule ({{f}}^{{n}}){}{{x}}= (f{}x)^n.",
    "\\tan^2 x; (f^-2) {}x ; (f^- 1){}x ",
    "CalculatorFunctions::innerCompositeApowerBevaluatedAtC",
    "ApowerBevaluatedAtC",
    compositeStandard
  );
  this->AddOperationHandler(
    "Differentiate",
    CalculatorFunctions::innerCompositeDifferentiateLog,
    "",
    "Differentiates log.",
    "d/dx (\\log x)",
    "CalculatorFunctions::innerCompositeDifferentiateLog",
    "DifferentiateLog",
    compositeStandard
  );
}

void Calculator::initPredefinedStandardOperationsWithoutHandler() {
  MacroRegisterFunctionWithName("Calculator::initPredefinedStandardOperationsWithoutHandler");
  //additional operations treated like function names but otherwise not parsed as syntactic elements.

  this->AddOperationNoRepetitionAllowed("RulesOff");
  this->AddOperationNoRepetitionAllowed("RulesOn");
  this->AddOperationNoRepetitionAllowed("Freeze");
  this->AddOperationNoRepetitionAllowed("\\infty");
  this->AddOperationNoRepetitionAllowed("\\phantom");
  this->AddOperationNoRepetitionAllowed("CommandEnclosure");
  this->AddOperationNoRepetitionAllowed("MonomialCollection");
  this->AddOperationNoRepetitionAllowed("MonomialPoly");
  this->AddOperationNoRepetitionAllowed("Melt");
  this->AddOperationNoRepetitionAllowed("Bind");
  this->AddOperationNoRepetitionAllowed("\\limits");
  this->AddOperationNoRepetitionAllowed("[)");
  this->AddOperationNoRepetitionAllowed("(]");
  this->AddOperationNoRepetitionAllowed("IntervalClosed");
  this->AddOperationNoRepetitionAllowed("IntervalOpen");
  this->AddOperationNoRepetitionAllowed("IndefiniteIndicator");
  this->AddOperationNoRepetitionAllowed("\\log");
  //additional operations with the same status as user-input expressions.
  this->AddOperationNoRepetitionAllowed("\\emptyset");
  this->AddOperationNoRepetitionAllowed("\\pi");
  this->AddOperationNoRepetitionAllowed("e");
  this->AddOperationNoRepetitionAllowed("i");
  this->AddOperationNoRepetitionAllowed("\\arctan");
  this->AddOperationNoRepetitionAllowed("CommandEnclosureStart");
  this->AddOperationNoRepetitionAllowed("CommandEnclosureFinish");
  this->AddOperationNoRepetitionAllowed("ExpressionHistory");
  this->AddOperationNoRepetitionAllowed("ExpressionHistorySet");
  this->AddOperationNoRepetitionAllowed("ExpressionHistorySetChild");
}

void Calculator::initAtomsNonCacheable() {
  MacroRegisterFunctionWithName("Calculator::initAtomsNonCacheable");
  this->atomsThatMustNotBeCached.SetExpectedSize(30);
  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("RandomInteger");
  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("SelectAtRandom");
  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("GenerateRandomPrime");
  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("TurnOffApproximations");
  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("TurnOnApproximations");
  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("PrintAlgebraicClosureStatus");
//  this->atomsThatMustNotBeCached.AddOnTopNoRepetitionMustBeNewCrashIfNot("PrintRuleStack");
}

void Calculator::initAtomsNotGoodForChainRule() {
  MacroRegisterFunctionWithName("Calculator::initAtomsNotGoodForChainRule");
  this->atomsNotAllowingChainRule.AddOnTopNoRepetitionMustBeNewCrashIfNot("Bind");
}

void Calculator::initStringsThatSplitIfFollowedByDigit() {
  MacroRegisterFunctionWithName("Calculator::initStringsThatSplitIfFollowedByDigit");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cdot");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\circ");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\frac");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\ln");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\log");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sin");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cos");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\tan");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cot");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sec");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\csc");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arctan");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arcsin");
  this->stringsThatSplitIfFollowedByDigit.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arccos");
}

void Calculator::initAtomsThatAllowCommutingOfArguments() {
  MacroRegisterFunctionWithName("Calculator::initAtomsThatAllowCommutingOfArguments");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.SetExpectedSize(30);
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("+");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("*");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("/");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("^");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("|");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sin");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cos");
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\log");
}

void Calculator::initArithmeticOperations() {
  MacroRegisterFunctionWithName("Calculator::initArithmeticOperations");
  this->arithmeticOperations.AddOnTopNoRepetitionMustBeNewCrashIfNot("+");
  this->arithmeticOperations.AddOnTopNoRepetitionMustBeNewCrashIfNot("-");
  this->arithmeticOperations.AddOnTopNoRepetitionMustBeNewCrashIfNot("*");
  this->arithmeticOperations.AddOnTopNoRepetitionMustBeNewCrashIfNot("/");
  this->arithmeticOperations.AddOnTopNoRepetitionMustBeNewCrashIfNot("^");
}

void Calculator::initBuiltInAtomsWhosePowersAreInterpretedAsFunctions() {
  MacroRegisterFunctionWithName("Calculator::initBuiltInAtomsWhosePowersAreInterpretedAsFunctions");
  //Related heavily to initOperationsInterpretedAsFunctionsMultiplicatively
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sin");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cos");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\tan");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cot");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sec");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\csc");
  this->atomsWhoseExponentsAreInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\log");
}

void Calculator::initOperationsInterpretedAsFunctionsMultiplicatively() {
  MacroRegisterFunctionWithName("Calculator::initOperationsInterpretedAsFunctionsMultiplicatively");
  //Related heavily to initBuiltInAtomsWhosePowersAreInterpretedAsFunctions
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sin");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cos");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\tan");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cot");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sec");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\csc");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arctan");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arcsin");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arccos");
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\log");
}

void Calculator::initOperationsThatAreKnownFunctions() {
  MacroRegisterFunctionWithName("Calculator::initOperationsThatAreKnownFunctions");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("+");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("-");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("*");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("/");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("^");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("|");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sqrt");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sin");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cos");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\tan");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\cot");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\sec");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\csc");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arctan");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arcsin");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\arccos");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\log");
  this->knownFunctionsWithComplexRange.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogBase");
}

void Calculator::AddKnownDoubleConstant(const std::string& theConstantName, double theConstantValue) {
  this->atomsNotInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot(theConstantName);
  Expression theConstantE;
  theConstantE.MakeAtom(theConstantName, *this);
  this->knownDoubleConstants.AddOnTopNoRepetitionMustBeNewCrashIfNot(theConstantE);
  this->knownDoubleConstantValues.AddOnTop(theConstantValue);
}

void Calculator::initBuiltInAtomsNotInterpretedAsFunctions() {
  MacroRegisterFunctionWithName("Calculator::initBuiltInAtomsNotInterpretedAsFunctions");
  this->atomsNotInterpretedAsFunctions.SetExpectedSize(30);

  this->AddKnownDoubleConstant("\\pi", MathRoutines::Pi());
  this->AddKnownDoubleConstant("e", MathRoutines::E());
  this->atomsNotInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("\\int");
  this->atomsNotInterpretedAsFunctions.AddOnTopNoRepetitionMustBeNewCrashIfNot("i");
}

void Calculator::AddTrigSplit(const std::string& trigFun, const List<std::string>& theVars) {
  MacroRegisterFunctionWithName("Calculator::AddTrigSplit");
  List<std::string> theSplit;
  for (int i = 0; i < theVars.size; i ++) {
    const std::string& theVar = theVars[i];
    theSplit.SetSize(0);
    theSplit.AddOnTop("\\" + trigFun);
    theSplit.AddOnTop(theVar);
    this->predefinedWordSplits.SetKeyValue(trigFun + theVar, theSplit);
    this->predefinedWordSplits.SetKeyValue("\\" + trigFun + theVar, theSplit);
    theSplit.SetSize(0);
    theSplit.AddOnTop(theVar);
    theSplit.AddOnTop("\\" + trigFun);
    this->predefinedWordSplits.SetKeyValue(theVar + trigFun, theSplit);
    this->predefinedWordSplits.SetKeyValue(theVar + "\\" + trigFun, theSplit);
  }
  for (int i = 0; i < theVars.size; i ++) {
    for (int j = 0; j < theVars.size; j ++) {
      theSplit.SetSize(0);
      theSplit.AddOnTop(theVars[i]);
      theSplit.AddOnTop("\\" + trigFun);
      theSplit.AddOnTop(theVars[j]);
      this->predefinedWordSplits.SetKeyValue(theVars[i] + trigFun + theVars[j], theSplit);
    }
  }
}

void Calculator::initPredefinedWordSplits() {
  MacroRegisterFunctionWithName("Calculator::initPredefinedWordSplits");
  List<std::string> theSplit;
  List<std::string> theVars;
  theVars.AddOnTop("x");
  theVars.AddOnTop("y");
  theSplit.SetSize(0);
  theSplit.AddOnTop("x"); theSplit.AddOnTop("y");
  this->predefinedWordSplits.SetKeyValue("xy", theSplit);
  theSplit.SetSize(0);
  theSplit.AddOnTop("y"); theSplit.AddOnTop("x");
  this->predefinedWordSplits.SetKeyValue("yx", theSplit);
  this->AddTrigSplit("sin", theVars);
  this->AddTrigSplit("cos", theVars);
  this->AddTrigSplit("tan", theVars);
  this->AddTrigSplit("cot", theVars);
  this->AddTrigSplit("sec", theVars);
  this->AddTrigSplit("csc", theVars);
}

void Calculator::initAtomsThatFreezeArguments() {
  MacroRegisterFunctionWithName("Calculator::initAtomsThatFreezeArguments");
  this->atomsThatFreezeArguments.SetExpectedSize(this->builtInTypes.size + 100);
  this->atomsThatFreezeArguments.AddOnTop(this->builtInTypes);
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("ElementWeylAlgebraDO"); //<-needed to facilitate civilized context handling
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("ElementWeylAlgebraPoly"); //<-needed to facilitate civilized context handling
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("Freeze");
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("if");
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("Bind");
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogEvaluationSteps");
  this->atomsThatFreezeArguments.AddOnTopNoRepetitionMustBeNewCrashIfNot("LogEvaluationStepsDebug");
}
