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

// Determines what output scripts we use for static output pages.
// Here, static output pages are standalone pages that
// are not part of the calculator frontend.
enum MathBootstrapScriptType {
  // Don't use any bootstrap script.
  // For development purposes.
  none,
  // Render all math on the backend, boostrap
  // convenience functions (copy+paste) with a minimal script.
  backendRendering,
  // Render all math on the frontend using the equation editor.
  equationEditor,
  // Render all math on the frontend using a
  // katex rendering script.
  katex
};

class MathML {
public:
  static std::string toMathMLFinal(
    const std::string& mathMLContent,
    const std::string& latex,
    const FormatExpressions* format
  );
  static std::string toMathMLInteger(int integer);
  static std::string latexCommandToMathMLEquivalent(const std::string& input);
  // Replaces all latex enclosed in
  // \(\)-delimiters in the input with a mathml equivalent.
  // Not intended for strings that were input by the end user
  // but rather for internal strings and
  // programatically generated strings that inject unsanitized
  // end user input.
  static std::string processLatex(
    const std::string& input, FormatExpressions* format
  );
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
  std::string convertTextWithLatexTags(FormatExpressions* format);
  static std::string convertLatex(
    const std::string& input, Calculator& calculator, FormatExpressions* format
  );
  void extractTags();
  void addStringOnTop(const std::string& incomingSnippet, bool isLatex);
  void convertTags(FormatExpressions* format);
  std::string collectConvertedTags();
  // A globally shared, once-initialized static calculator
  // instance used solely for parsing.
  // Intended to take as inputs internal strings
  // that need mathML formatting.
  // This is not intended for processing end-user-input strings.
  static Calculator& converterCalculator();
};

template <class Object>
std::string List<Object>::toMathMLFinal(
  FormatExpressions* format, const std::string& separator
) const {
  List<std::string> result;
  for (const Object& object : *this) {
    result.addOnTop(object.toMathMLFinal(format));
  }
  return result.toStringWithSeparator(separator);
}

template <class Object>
std::string List<Object>::toMathML(FormatExpressions* format) const {
  std::stringstream out;
  out << "<mrow>" << MathML::leftParenthesis;
  for (int i = 0; i < this->size; i ++) {
    const Object& object = (*this)[i];
    out << object.toMathML(format);
    if (i != this->size - 1) {
      out << "<mo>,</mo>";
    }
  }
  out << MathML::rightParenthesis;
  out << "</mrow>";
  return out.str();
}

#endif // header_math_mathml_ALREADY_INCLUDED
