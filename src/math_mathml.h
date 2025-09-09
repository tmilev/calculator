#ifndef header_math_mathml_ALREADY_INCLUDED
#define header_math_mathml_ALREADY_INCLUDED

#include "general_lists.h"
#include "general_maps.h"
#include <sstream>

// Properties of a printed mathml expression.
class MathExpressionProperties {
public:
  bool startsWithMinus;
  bool isNegativeOne;
  bool isOne;
  bool startsWithDigit;
  bool endsWithDigit;
  bool startsWithFraction;
  MathExpressionProperties():
  startsWithMinus(false),
  isNegativeOne(false),
  isOne(false),
  startsWithDigit(false),
  endsWithDigit(false),
  startsWithFraction(false) {}
};

class MathML {
public:
  static std::string toMathMLFinal(
    const std::string& mathMLContent, const std::string& latex
  );
  static std::string toMathMLInteger(int integer);
  static std::string latexCommandToMathMLEquivalent(const std::string& input);
  // Replaces all latex enclosed in
  // \(\)-delimiters in the input with a mathml equivalent.
  // Not intended for strings that were input by the end user
  // but rather for internal strings and
  // programatically generated strings that inject unsanitized
  // end user input.
  static std::string processLatex(const std::string& input);
  static std::string leftParenthesis;
  static std::string rightParenthesis;
  static std::string negativeSign;
  static std::string positiveSign;
  static std::string mtableDefault;
  static MapList<std::string, std::string>& mapFromLatexToMathMLEquivalents();
};

class LatexOrString {
public:
  std::string content;
  bool isLatex;
  LatexOrString(
    const std::string& inputContent = "", bool inputIsLatex = false
  ):
  content(inputContent),
  isLatex(inputIsLatex) {}
};

class Calculator;

// Replaces all latex enclosed in
// \(\)-delimiters in the input with a mathml equivalent.
// This class is to be used once per conversion.
// Not indended for strings that were input by the end-user.
class MathMLConverter {
  std::string toBeConverted;
  List<LatexOrString> elements;
public:
  MathMLConverter(const std::string& input): toBeConverted(input) {}
  std::string convertTextWithLatexTags();
  static std::string convertLatex(
    const std::string& input, Calculator& calculator
  );
  void extractTags();
  void addStringOnTop(const std::string& incomingSnippet, bool isLatex);
  void convertTags();
  std::string collectConvertedTags();
  // A globally shared, once-initialized static calculator
  // instance used solely for parsing.
  // Intended to take as inputs internal strings
  // that need mathML formatting.
  // This is not intended for processing end-user-input strings.
  static Calculator& converterCalculator();
};

#endif // header_math_mathml_ALREADY_INCLUDED
