// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_html_functions.h"
#include "calculator_educational_functions_1.h"

void Calculator::initializeFunctionsExtra() {
  MacroRegisterFunctionWithName("Calculator::initializeFunctionsExtra");
  Function::Options innerStandard = Function::Options::standard();
  Function::Options innerNoTest = Function::Options::innerNoTest();
  Function::Options innerAdminNoTest = Function::Options::adminNoTest();
  Function::Options innerInvisibleNoTest = Function::Options::invisibleNoTest();
  Function::Options innerInvisibleExperimentalNoTest = Function::Options::invisibleNoTest();
  Function::Options innerFreezesArguments = Function::Options::innerFreezesArguments();
  // Function::Options innerNoTestExperimental = Function::Options::innerNoTestExperimental();
  // Function::Options innerExperimental = Function::Options::experimental();

  this->addOperationHandler(
    "LoadFileIntoString",
    CalculatorConversions::innerLoadFileIntoString,
    "",
    "Loads a file into a string. "
    "The file must be given its relative file name displayed when browsing "
    "the web server. "
    "There are two exceptions. "
    "1) The file can be located in a folder in the project base "
    "that is otherwise not visible by the webserver "
    "**provided that** the folder is white-listed "
    "via the FileOperations class within the C++ source. "
    "Example: folder problems/ "
    "is white-listed. To access this file simply "
    "start your file name with the foldername. "
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
  this->addOperationHandler(
    "InterpretProblem",
    CalculatorHtmlFunctions::interpretProblem,
    "",
    "Does as ExtractCalculatorExpressionFromHtml but in "
    "addition interprets the calculator commands. ",
    "InterpretProblem(LoadFileIntoString("
    "\"problems/default/Functions-composing-fractional-linear-1.html\"))",
    "CalculatorHtmlFunctions::interpretProblem",
    "InterpretProblem",
    innerNoTest
  );
  this->addOperationHandler(
    "ProblemGiveUp",
    CalculatorHtmlFunctions::interpretProblemGiveUp,
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
    "CalculatorHtmlFunctions::interpretProblemGiveUp",
    "ProblemGiveUp",
    innerStandard
  );
  this->addOperationHandler(
    "ExtractCalculatorExpressionFromHtml",
    CalculatorHtmlFunctions::extractCalculatorExpressionFromHtml,
    "",
    "Reads html and extracts embedded calculator commands. Content enclosed in "
    "spans with appropriate class names is interpreted; all other content "
    "is copied without any change. At the moment of writing, "
    "the planned span class names are: \"calculator\", "
    "\"calculatorHidden\", \"calculatorAnswer\".",
    "ExtractCalculatorExpressionFromHtml(LoadFileIntoString("
    "\"problems/default/Functions-composing-fractional-linear-1.html\"))",
    "CalculatorHtmlFunctions::extractCalculatorExpressionFromHtml",
    "ExtractCalculatorExpressionFromHtml",
    innerStandard
  );
  this->addOperationHandler(
    "TestCalculatorIndicator",
    CalculatorFunctions::innerTestIndicator,
    "",
    "(This is not a mathematical function). "
    "Tests the calculator indicator mechanism."
    "First argument times number of iterations. "
    "Second argument = length of dummy comment appended to the calculator comments. "
    "Use a large dummy comment to test that large outputs are piped correctly to the "
    "monitoring process. ",
    "TestCalculatorIndicator(1000, 200000)",
    "CalculatorFunctions::innerTestIndicator",
    "TestCalculatorIndicator",
    innerNoTest
  );
  this->addOperationHandler(
    "TestTopCommand",
    CalculatorFunctions::innerTestTopCommand,
    "",
    "(This is not a mathematical function). Tests the top linux command. ",
    "TestTopCommand(1000)",
    "CalculatorFunctions::innerTestTopCommand",
    "TestTopCommand",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "Crash",
    CalculatorFunctions::crash,
    "",
    "Crashes the calculator: tests the "
    "crashing mechanism (are crash logs properly created, etc.). ",
    "Crash(0)",
    "CalculatorFunctions::crash",
    "Crash",
    innerNoTest
  );
  this->addOperationHandler(
    "CrashListOutOfBounds",
    CalculatorFunctions::crashByListOutOfBounds,
    "",
    "Crashes the calculator by attempting to "
    "use data out-of-bounds in a List data structure. ",
    "CrashListOutOfBounds(0)",
    "CalculatorFunctions::crashByListOutOfBounds",
    "CrashListOutOfBounds",
    innerNoTest
  );
  this->addOperationHandler(
    "CrashVectorOutOfBounds",
    CalculatorFunctions::crashByVectorOutOfBounds,
    "",
    "Crashes the calculator by attempting to use data "
    "out-of-bounds in a std::vector.",
    "CrashVectorOutOfBounds(0)",
    "CalculatorFunctions::crashByVectorOutOfBounds",
    "CrashVectorOutOfBounds",
    innerNoTest
  );
  this->addOperationHandler(
    "PlotExpressionTree",
    CalculatorFunctionsPlot::drawExpressionGraph,
    "",
    "Draws the internal tree structure of an expression. "
    "Does not unfold built-in types.",
    "PlotExpressionTree(e^x)",
    "CalculatorFunctions::innerDrawExpressionGraph",
    "PlotExpressionTree",
    innerStandard
  );
  this->addOperationHandler(
    "PlotExpressionTreeFull",
    CalculatorFunctionsPlot::drawExpressionGraphFull,
    "",
    "Draws the internal tree structure of an expression. Unfolds built-in types. ",
    "PlotExpressionTreeFull(1);\n"
    "PlotExpressionTree(1 + 1);\n"
    "PlotExpressionTree(Freeze{}(1 + 1));",
    "CalculatorFunctions::innerDrawExpressionGraphFull",
    "PlotExpressionTreeFull",
    innerStandard
  );
  this->addOperationHandler(
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
  this->addOperationHandler(
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
  this->addOperationHandler(
    "TestBase64",
    CalculatorFunctionsEncoding::base64ToCharToBase64Test,
    "",
    "Test function: converts a base64 string to bitstream and back to base64. "
    "Output must be identical to input. ",
    "TestBase64(\"TheQuickBrownFoxJumpsOverTheLazyDog=\");\n"
    "TestBase64(\"TheQuickBrownFoxJumpsOverTheLazyDog\")",
    "CalculatorFunctions::base64ToCharToBase64Test",
    "TestBase64",
    innerStandard
  );
  this->addOperationHandler(
    "StringDifference",
    CalculatorFunctions::stringDifference,
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

  this->addOperationHandler(
    "TestJSON",
    CalculatorFunctionsEncoding::testJSON,
    "",
    "Tests the JSON parsing mechanism. Input: json string, use backslash escapes for "
    "backslashes and quotes.",
    "TestJSON(\"{a:1, b: false,}\");\n"
    "TestJSON(\"{\\\"a\\\":\\\"\\\\\\\"\\\"}\");",
    "CalculatorFunctions::testJSON",
    "TestJSON",
    innerStandard
  );

  this->addOperationHandler(
    "FormatCPPCode",
    CalculatorFunctions::innerFormatCPPSourceCode,
    "",
    "Format cpp code. ",
    "FormatCPPCode{}(\"src/database.cpp\")",
    "Calculator::innerFormatCPPSourceCode",
    "FormatCPPCode",
    innerInvisibleExperimentalNoTest
  );
  this->addOperationHandler(
    "formatCPPDirectory",
    CalculatorFunctions::innerformatCPPDirectory,
    "",
    "Format cpp directory. ",
    "formatCPPDirectory{}(\"src/\")",
    "Calculator::innerformatCPPDirectory",
    "formatCPPDirectory",
    innerInvisibleNoTest
  );

  this->addOperationHandler(
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
  this->addOperationHandler(
    "TestTopicListProblems",
    CalculatorFunctions::testTopicListProblems,
    "",
    "Tests whether all files given in the default topic lists open properly. ",
    "TestTopicListProblems 0",
    "CalculatorFunctions::testTopicListProblems",
    "TestTopicListProblems",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "TestProblemInterpretation",
    CalculatorFunctions::testProblemInterpretation,
    "",
    "Runs an automated test of built in "
    "problems located in the problems/default/ folder. "
    "First argument = index of first problem to test. "
    "Second argument = number of problems to test. "
    "Third argument = starting random seed. "
    "To test a particular problem, set the index of the "
    "first problem to that of your problem, and set the "
    "number of problems to test to 1. To find out the index "
    "of your problem, run the entire test array once and "
    "the index of each problem file will be listed in the output. "
    "Until first error is seen, every test will be run a number of times, "
    "3 at the time of writing. "
    "The random seed will be incremented for each run. ",
    "TestProblemInterpretation{}(0, 0, 0)",
    "CalculatorFunctions::testProblemInterpretation",
    "TestProblemInterpretation",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "PrintRuleStack",
    CalculatorFunctions::printRuleStack,
    "",
    "Prints the current rule stack",
    "a = b;\n"
    "PrintRuleStack{}0;\n"
    "c = d; PrintRuleStack{}0; ",
    "CalculatorFunctions::printRuleStack",
    "PrintRuleStack",
    innerStandard
  );
  this->addOperationHandler(
    "Crawl",
    CalculatorFunctionsFreecalc::crawlTexFile,
    "",
    "Crawls a latex file collecting all local style files and all \\input "
    "commands to produce a single latex file. The function was "
    "originally designed for the purposes of the freecalc project "
    "as not been tested on any other projects, please use only for freecalc.",
    "Crawl(\"freecalc/homework/referenceallproblemsbycourse/calculusimasterproblemsheet.tex\")",
    "CalculatorFunctions::innerCrawlTexFile",
    "Crawl",
    Function::Options::administrativeTested()
  );
  this->addOperationHandler(
    "BuildFreecalc",
    CalculatorFunctionsFreecalc::buildFreecalc,
    "",
    "Builds the freecalc lecture pdfs. "
    "This function performs file operations and is allowed only to logged-in administrator accounts. "
    "Takes as arguments the lecture folder (within the freecalc project) and the file name. "
    "The function assumes the freecalc project is installed in a folder called freecalc, "
    "parallel to the calculator project folder "
    "(i.e, we have folders /vectorpartition and /freecalc next to one another). "
    "The folders are given relative to the /freecalc base.",
    "BuildFreecalc(\"freecalc/lectures/referencelectures/precalculus.tex\");\n"
    "BuildFreecalc(\"freecalc/lectures/referencelectures/calculusi.tex\");\n"
    "BuildFreecalc(\"freecalc/lectures/referencelectures/calculusii.tex\");\n"
    "BuildFreecalc(\"freecalc/lectures/referencelectures/calculusiiimultivariable.tex\");\n"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/precalculus.tex\");\n"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/calculusi.tex\");\n"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/calculusii.tex\");\n"
    "BuildFreecalc(\"freecalc/homework/referenceallproblemsbycourse/calculusiii.tex\");\n",
    "CalculatorFunctions::innerBuildFreecalc",
    "BuildFreecalc",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "BuildFreecalcWithSlides",
    CalculatorFunctionsFreecalc::buildFreecalcSingleSlides,
    "",
    "Same as BuildFreeCalc but attempts to also build individual slides. ",
    "BuildFreecalcWithSlides(\"freecalc/homework/referenceallproblemsbycourse/calculusi.tex\");",
    "CalculatorFunctions::BuildFreecalcWithSlides",
    "BuildFreecalcWithSlides",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "BuildSlidesInTopicList",
    CalculatorFunctionsFreecalc::buildFreecalcSlidesOnTopic,
    "",
    "Builds all slides in the current topic list. Available to logged-in admins only. ",
    "BuildSlidesInTopicList (0);",
    "CalculatorFunctions::innerBuildFreecalcSlidesOnTopic",
    "BuildSlidesInTopicList",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "SuffixNotationForPostScript",
    CalculatorFunctions::innerSuffixNotationForPostScript,
    "",
    "Suffix notation. for postscript, used to quickly generate pstricks drawings in LaTeX.",
    "suffixNotationForPostScript{}((1/3 + a + b) * c)",
    "CalculatorFunctions::innerSuffixNotationForPostScript",
    "SuffixNotationForPostScript",
    innerStandard
  );
  this->addOperationHandler(
    "MakeJavascriptExpression",
    CalculatorFunctions::makeJavascriptExpression,
    "",
    "Attempts to construct a javascript translation of the input. "
    "If not successful leaves the expression unchanged.",
    "MakeJavascriptExpression(a(b+c))",
    "Calculator::makeJavascriptExpression",
    "MakeJavascriptExpression",
    innerStandard
  );
  this->addOperationHandler(
    "CompareExpressionsJSON",
    CalculatorEducationalFunctions::compareExpressionsJSON,
    "",
    "Compares two expressions and returns the result in JSON format. Freezes its inputs.",
    "CompareExpressionsJSON((x+1)(x+2),x^2+3x+2);\n",
    "CalculatorFunctions::CompareExpressionsJSON",
    "CompareExpressionsJSON",
    innerFreezesArguments
  );
}

void Calculator::initializeFunctionsCryptoAndEncoding() {
  MacroRegisterFunctionWithName("Calculator::initializeFunctionsCryptoAndEncoding");
  Function::Options innerStandard = Function::Options::standard();
  Function::Options innerNoTest = Function::Options::innerNoTest();
  Function::Options innerAdminNoTest = Function::Options::adminNoTest();
  Function::Options innerNoTestExperimental = Function::Options::innerNoTestExperimental();
  Function::Options innerExperimental = Function::Options::experimental();

  this->addOperationHandler(
    "ConvertBase64ToHex",
    CalculatorFunctionsEncoding::convertBase64ToHex,
    "",
    "Converts base64 string to hexadecimal string. ",
    "ConvertBase64ToHex(\"AQAB\");",
    "CalculatorFunctions::convertBase64ToHex",
    "ConvertBase64ToHex",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertStringToHex",
    CalculatorFunctionsEncoding::convertStringToHex,
    "",
    "Converts a bitstream (not necessarily UTF-8 encoded) to hex. ",
    "ConvertStringToHex(Sha256(Sha256(\"hello\")));",
    "CalculatorFunctions::convertStringToHex",
    "ConvertStringToHex",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertHexToInteger",
    CalculatorFunctionsEncoding::convertHexToInteger,
    "",
    "Converts a hex string to an integer. ",
    "ConvertHexToInteger(Base64ToHex(\"AQAB\"));",
    "CalculatorFunctions::hexToInteger",
    "ConvertHexToInteger",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertIntegerToHex",
    CalculatorFunctionsEncoding::convertIntegerToHex,
    "",
    "Converts an integer to hex string. ",
    "ConvertIntegerToHex(65537);",
    "CalculatorFunctions::integerToHex",
    "ConvertIntegerToHex",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertHexToString",
    CalculatorFunctionsEncoding::convertHexToString,
    "",
    "Converts a hex string to a string. ",
    "ConvertHexToString(\"3031300d060960864801650304020105000420\");",
    "CalculatorFunctions::hexToString",
    "ConvertHexToString",
    innerStandard
  );
  this->addOperationHandler(
    "URLStringToNormalString",
    CalculatorFunctions::urlStringToNormalString,
    "",
    "Converts an url-encoded string to a normal string. ",
    "URLStringToNormalString(\"randomSeed%3d92742048%26submissionsAlgebraAnswer%3\")",
    "CalculatorConversions::urlStringToNormalString",
    "URLStringToNormalString",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertStringToURL",
    CalculatorFunctions::stringToURL,
    "",
    "Converts a normal string to a url-encoded one. ",
    "ConvertStringToURL(\"+ %\")",
    "CalculatorConversions::stringToURL",
    "ConvertStringToURL",
    innerStandard
  );
  this->addOperationHandler(
    "URLKeyValuePairsDecode",
    CalculatorFunctions::urlKeyValuePairsToNormalRecursive,
    "",
    "Converts an url-encoded string to a normal string. "
    "All % signs are interpreted recursively "
    "as double, triple, ... url encoded strings and decoded accordingly.",
    "URLKeyValuePairsDecode(\"Problems%2fFunctions%2dcomposing%2dfractional%2dlinear%2d1.html=weight%3d3%26deadlines%3d%26&Problems%2fLimits%2dbasic%2dsubstitution%2d1.html=weight%3d3%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dinfinity%2dRF%2dequal%2ddeg%2d1.html=weight%3d2%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2dequal%2ddeg%2d1.html=weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dpm%2dinfinity%2dRF%2dnum%2ddeg%2dsmaller%2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d1.html =weight%3d5%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dRF%2ddeg%2dden%2dsmaller%2d2.html =weight%3d6%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dpm%2dinfinity%2dquotient%2dradical%2deven%2dpower%2dbasic % 2d1.html =weight%3d1%26deadlines%3d%26&Problems%2fLimits%2dx%2dtends%2dto%2dminus%2dinfinity%2dquotient%2dradical%2dodd%2dpower%2dbasic % 2d1.html =weight%3d1%26deadlines%3d%26&Homework%2fPrecalculus%2dPrerequisites%2dUMB%2d1.html = deadlines%3d%26&\")",
    "CalculatorConversions::urlStringToNormalString",
    "URLKeyValuePairsDecode",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertIntegerToBase58",
    CalculatorFunctionsEncoding::convertIntegerUnsignedToBase58,
    "",
    "Converts an unsigned integer to base58. ",
    "theInt = ConvertHexToInteger(ConvertBase58ToHex(\"1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK\"));"
    "ConvertIntegerToBase58(theInt)",
    "CalculatorFunctions::convertIntegerUnsignedToBase58",
    "ConvertIntegerToBase58",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertBase58ToHex",
    CalculatorFunctionsEncoding::convertBase58ToHex,
    "",
    "Converts Base58 to hex. ",
    "ConvertBase58ToHex(\"1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK\");",
    "CalculatorFunctions::convertBase58ToHex",
    "ConvertBase58ToHex",
    innerStandard
  );
  this->addOperationHandler(
    "AESCBCEncrypt",
    CalculatorFunctionsCrypto::aes_cbc_256_encrypt,
    "",
    "Encodes using aes 256 bit in cbc (cipher block chain) mode. "
    "First argument = key. Second argument = text. Reference: NIST SP 800-38A.",
    "text = ConvertHexToString \"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710\";\n"
    "key = ConvertHexToString \"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4\";\n"
    "ConvertStringToHex AESCBCEncrypt(key, text);\n"
    "\"f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b\";\n",
    "CalculatorFunctionsCrypto::aes_cbc_256_encrypt",
    "AESCBCEncrypt",
    innerStandard
  );
  this->addOperationHandler(
    "AESCBCDecrypt",
    CalculatorFunctionsCrypto::aes_cbc_256_decrypt,
    "",
    "AES decryption. First argument key, second argument - text. ",
    "text = \"6bc1bee22e409f96e93d7e117393172aae2d8a571e03ac9c9eb76fac45af8e5130c81c46a35ce411e5fbc1191a0a52eff69f2445df4f9b17ad2b417be66c3710\";\n"
    "key = \"603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4\";\n"
    "cipherText = ConvertStringToHex AESCBCEncrypt(ConvertHexToString key, ConvertHexToString text);\n"
    "cipherText;\n"
    "\"f58c4c04d6e5f1ba779eabfb5f7bfbd69cfc4e967edb808d679f777bc6702c7d39f23369a9d9bacfa530e26304231461b2eb05e2c39be9fcda6c19078c6a9d1b\";\n"
    "ConvertStringToHex AESCBCDecrypt(ConvertHexToString key, ConvertHexToString cipherText);\n"
    "text",
    "CalculatorFunctionsCrypto::aes_cbc_256_decrypt",
    "AESCBCDecrypt",
    innerStandard
  );
  this->addOperationHandler(
    "AppendDoubleSha256Check",
    CalculatorFunctionsCrypto::appendDoubleSha256Check,
    "",
    "Appends a sha 256 checksum to a string. More precisely, appends the first 4 bytes "
    "of sha256 of the string to the string. ",
    "A= \"80aad3f1f5de25ff67a4fd3d7808d58510e00ec08a55c10ad5751facf35411509701\";\n"
    "B= ConvertHexToString(A);\n"
    "C= AppendDoubleSha256Check(B);\n"
    "D= ConvertStringToHex(C)",
    "CalculatorFunctions::appendDoubleSha256Check",
    "AppendDoubleSha256Check",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertHexToBase58",
    CalculatorFunctionsEncoding::convertHexToBase58,
    "",
    "Converts hex to base58. ",
    "ConvertHexToBase58(\"03aaf2d5530b1a5cbf80c248ca44635ac265f4104ffc5b76ef48f361c03b7f536f\");",
    "CalculatorFunctions::convertHexToBase58",
    "ConvertHexToBase58",
    innerStandard
  );
  this->addOperationHandler(
    "CharToBase64",
    CalculatorFunctionsEncoding::convertCharToBase64,
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
    "CalculatorFunctions::charToBase64",
    "CharToBase64",
    innerStandard
  );
  this->addOperationHandler(
    "ConvertBase64ToString",
    CalculatorFunctionsEncoding::convertBase64ToString,
    "",
    "Converts base64 to string",
    "ConvertBase64ToString("
    "\"k7qTF1hLeOdihfKG5IRnlb7us2FVo1pSC2r0DVLkYwRAQHMs4XatvGcdG81S64uoaqG4fZ9IHJNpZjqokojuX5VIwl6utBO9\""
    ");",
    "CalculatorFunctions::convertBase64ToString",
    "ConvertBase64ToString",
    innerStandard
  );
  this->addOperationHandler(
    "loadKnownCertificates",
    CalculatorFunctionsCrypto::loadKnownCertificates,
    "",
    "Loads known security certificates from the <a href=\"/certificates-public/\">certificates-public/</a> folder. ",
    "loadKnownCertificates(0);",
    "CalculatorFunctionsCrypto::loadKnownCertificates",
    "loadKnownCertificates",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "TestLoadPEMCertificate",
    CalculatorFunctionsCrypto::testLoadPEMCertificates,
    "",
    "Tests the pem parsing functions. ",
    "TestLoadPEMCertificate(ConvertBase64ToString(LoadFileIntoString(\"test/certificate_self_signed.base64\")));\n",
    "CalculatorFunctionsCrypto::testLoadPEMCertificates",
    "TestLoadPEMCertificate",
    innerStandard
  );
  this->addOperationHandler(
    "TestLoadPEMPrivateKey",
    CalculatorFunctionsCrypto::testLoadPEMPrivateKey,
    "",
    "Tests the pem private key functions. ",
    "TestLoadPEMPrivateKey(ConvertBase64ToString(LoadFileIntoString(\"test/private_key.base64\")));\n",
    "CalculatorFunctionsCrypto::testLoadPEMPrivateKey",
    "TestLoadPEMPrivateKey",
    innerStandard
  );
  this->addOperationHandler(
    "TestTLSMessageSequence",
    CalculatorFunctionsCrypto::testTLSMessageSequence,
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
    "CalculatorFunctionsCrypto::testTLSMessageSequence",
    "TestTLSMessageSequence",
    innerNoTestExperimental
  );
  this->addOperationHandler(
    "TestTLSDecodeSSLRecord",
    CalculatorFunctionsCrypto::testTLSDecodeSSLRecord,
    "",
    "Decodes a client hello. ",
    "%HideLHS\nTestTLSDecodeSSLRecord(\n"
    "LoadFileIntoString(\"test/message_client_ssl_0.hex\")\n"
    ")",
    "CalculatorFunctionsCrypto::testTLSDecodeSSLRecord",
    "TestTLSDecodeSSLRecord",
    innerExperimental
  );
  this->addOperationHandler(
    "TestASN1Decode",
    CalculatorFunctionsCrypto::testASN1Decode,
    "",
    "Tests decoding of abstract syntax one. ",
    "%HideLHS\n"
    "TestASN1Decode(ConvertBase64ToString(LoadFileIntoString(\"test/certificate_self_signed.base64\")));\n"
    "TestASN1Decode(ConvertBase64ToString(LoadFileIntoString(\"test/private_key.base64\")));\n",
    "CalculatorFunctions::testASN1Decode",
    "TestASN1Decode",
    innerExperimental
  );
  this->addOperationHandler(
    "X509CertificateServerBase64",
    CalculatorFunctionsCrypto::x509CertificateServer,
    "",
    "Returns the base 64 encoding of the X509 certificate of this server. ",
    "X509CertificateServerBase64 0; ",
    "CalculatorFunctions::innerX509certificateCrunch",
    "X509CertificateServerBase64",
    innerNoTest
  );
  this->addOperationHandler(
    "X509CertificateDecode",
    CalculatorFunctionsCrypto::x509CertificateDecode,
    "",
    "Decodes raw X509 certificate to a string. ",
    "%HideLHS\n"
    "X509CertificateDecode ConvertBase64ToString\n"
    "LoadFileIntoString(\"test/certificate_self_signed.pem\")",
    "CalculatorFunctions::x509CertificateDecode",
    "X509CertificateDecode",
    innerStandard
  );
  this->addOperationHandler(
    "ShowKnownASNObjectIds",
    CalculatorFunctionsCrypto::showKnownObjectIds,
    "",
    "Lists all abstract syntax one object ids hard-coded in the calculator.",
    "ShowKnownASNObjectIds 0",
    "CalculatorFunctionsCrypto::showKnownObjectIds",
    "ShowKnownASNObjectIds",
    innerStandard
  );
  this->addOperationHandler(
    "JWTVerifyAgainstKnownKeys",
    CalculatorFunctionsCrypto::jwtVerifyAgainstKnownKeys,
    "",
    "Tries to verify a Json Web Token.",
    "%HideLHS JWTVerifyAgainstKnownKeys(\"\"); ",
    "CalculatorFunctionsCrypto::jwtVerifyAgainstKnownKeys",
    "JWTVerifyAgainstKnownKeys",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "JWTVerifyRSA256",
    CalculatorFunctionsCrypto::jwtVerifyAgainstRSA256,
    "",
    "Tries to verify a Json Web Token with respect to a given rsa modulus and exponent. "
    "The reference JWT token was taken from: https://tools.ietf.org/html/rfc7515#page-38, "
    "Appendix A.2.",
    "%HideLHS "
    "token =\"eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJqb2UiLA0KICJleHAiOjEzMDA4MTkzODAsDQogImh0dHA6Ly9leGFtcGxlLmNvbS9pc19yb290Ijp0cnVlfQ.cC4hiUPoj9Eetdgtv3hF80EGrhuB__dzERat0XF9g2VtQgr9PJbu3XOiZj5RZmh7AAuHIm4Bh-0Qc_lF5YKt_O8W2Fp5jujGbds9uJdbF9CUAr7t1dnZcAcQjbKBYNX4BAynRFdiuB--f_nZLgrnbyTyWzO75vRK5h6xBArLIARNPvkSjtQBMHlb1L07Qe7K0GarZRmB_eSN9383LcOLn6_dO--xi12jzDwusC-eOkHWEsqtFZESc6BfI7noOPqvhJ1phCnvWh6IeYI2w9QOYEUipUTI8np6LbgGY9Fs98rqVt5AXLIhWkWywlVmtVrBp0igcN_IoypGlUPQGe77Rw\";\n"
    "modulus = \"ofgWCuLjybRlzo0tZWJjNiuSfb4p4fAkd_wWJcyQoTbji9k0l8W26mPddxHmfHQp-Vaw-4qPCJrcS2mJPMEzP1Pt0Bm4d4QlL-yRT-SFd2lZS-pCgNMsD1W_YpRPEwOWvG6b32690r2jZ47soMZo9wGzjb_7OMg0LOL-bSf63kpaSHSXndS5z5rexMdbBYUsLA9e-KXBdQOS-UTo7WTBEMa2R2CapHg665xsmtdVMTBQY4uDZlxvb3qCo5ZwKh9kG4LT6_I5IhlJH7aGhyxXFvUK-DWNmoudF8NAco9_h9iaGNj8q2ethFkMLs91kzk2PAcDTW9gb54h4FRWyuXpoQ\";\n"
    "exponent =\"AQAB\";\n"
    "JWTVerifyRSA256(token,modulus,exponent);",
    "CalculatorFunctionsCrypto::jwtVerifyAgainstRSA256",
    "JWTVerifyRSA256",
    innerStandard
  );
  this->addOperationHandler(
    "Sha1",
    CalculatorFunctionsCrypto::sha1OfString,
    "",
    "Converts characters to a sequence of bits and computes the sha1 hash value of those bits. "
    "The examples below are taken from Wikipedia. ",
    "Sha1(\"The quick brown fox jumps over the lazy dog\");\n"
    "Sha1(\"The quick brown fox jumps over the lazy cog\");",
    "CalculatorFunctions::sha1OfString",
    "Sha1",
    innerStandard
  );
  this->addOperationHandler(
    "Sha224",
    CalculatorFunctionsCrypto::sha224OfString,
    "",
    "Converts characters to a sequence of bits and computes the sha224 hash value of those bits. "
    "Reference: Wikipedia. ",
    "Sha224(\"\");",
    "CalculatorFunctionsCrypto::sha224OfString",
    "Sha224",
    innerStandard
  );
  this->addOperationHandler(
    "TestRSASign",
    CalculatorFunctionsCrypto::testRSASign,
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
    "CalculatorFunctions::testRSASign",
    "TestRSASign",
    innerStandard
  );
  this->addOperationHandler(
    "GenerateRandomPrime",
    CalculatorFunctionsCrypto::generateRandomPrime,
    "",
    "Generate random prime. Argument = desired number of bytes, max 128. ",
    "GenerateRandomPrime(10)",
    "CalculatorFunctionsCrypto::generateRandomPrime",
    "GenerateRandomPrime",
    innerNoTest
  );

  this->addOperationHandler(
    "RSAEncrypt",
    CalculatorFunctionsCrypto::RSAEncrypt,
    "",
    "Encrypts with RSA. First argument: modulus. Second argument: (public) exponent. "
    "Third argument: message given as a large integer.",
    "RSAEncrypt(3233, 17, 65)",
    "CalculatorFunctionsCrypto::RSAEncrypt",
    "RSAEncrypt",
    innerStandard
  );
  this->addOperationHandler(
    "Ripemd160",
    CalculatorFunctionsCrypto::ripemd160OfString,
    "",
    "Ripemd160 hash function. See wikipedia page. ",
    "ConvertStringToHex Ripemd160(\"The quick brown fox jumps over the lazy dog\");\n"
    "\"37f332f68db77bd9d7edd4969571ad671cf9dd3b\";\n"
    "ConvertStringToHex Ripemd160(\"The quick brown fox jumps over the lazy cog\");\n"
    "\"132072df690933835eb8b6ad0b77e7b6f14acad7\";\n",
    "CalculatorFunctionsCrypto::ripemd160OfString",
    "Ripemd160",
    innerStandard
  );
  this->addOperationHandler(
    "ShaThree256",
    CalculatorFunctionsCrypto::sha3_256OfString,
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
    "CalculatorFunctionsCrypto::sha3_256OfString",
    "ShaThree256",
    innerStandard
  );
  this->addOperationHandler(
    "Keccak256",
    CalculatorFunctionsCrypto::keccak256OfString,
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
    "CalculatorFunctionsCrypto::keccak256OfString",
    "Keccak256",
    innerStandard
  );
  this->addOperationHandler(
    "Sha256",
    CalculatorFunctionsCrypto::sha256OfString,
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
    "CalculatorFunctionsCrypto::sha256OfString",
    "Sha256",
    innerStandard
  );
  this->addOperationHandler(
    "Sha512",
    CalculatorFunctionsCrypto::sha512,
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
    "CalculatorFunctionsCrypto::sha512",
    "Sha512",
    innerStandard
  );
  this->addOperationHandler(
    "Sha256Verbose",
    CalculatorFunctionsCrypto::sha256OfStringVerbose,
    "",
    "Same as Sha256 but more verbose. ",
    "Sha256Verbose(\"\");",
    "CalculatorFunctionsCrypto::sha256OfStringVerbose",
    "Sha256Verbose",
    innerStandard
  );
  this->addOperationHandler(
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
}
