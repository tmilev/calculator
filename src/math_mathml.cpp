#include "calculator_interface.h"
#include "math_mathml.h"

std::string MathML::leftParenthesis =
"<mo fence='true' stretchy='true' form='prefix'>(</mo>";
std::string MathML::rightParenthesis =
"<mo fence='true' stretchy='true' form='suffix'>)</mo>";
std::string MathML::negativeSign = "<mo>&minus;</mo>";
std::string MathML::positiveSign = "<mo>+</mo>";
std::string MathML::mtableDefault = "<mtable frame='solid' rowlines='solid'>";

std::string MathML::toMathMLFinal(
  const std::string& mathMLContent, const std::string& latex
) {
  std::stringstream out;
  out
  << "<span class='mathcalculatorbackendrendered'>"
  << "<math>"
  << mathMLContent
  << "<semantics>"
  << "<annotation>"
  << latex
  << "</annotation"
  << "</semantics></math>"
  << "</span>";
  return out.str();
}

MapList<std::string, std::string>& MathML::mapFromLatexToMathMLEquivalents() {
  static MapList<std::string, std::string> result;
  if (result.size() == 0) {
    result.setKeyValue("alpha", "<mi>&alpha;</mi>");
  }
  return result;
}

std::string MathML::latexCommandToMathMLEquivalent(const std::string& input) {
  MapList<std::string, std::string>& map =
  MathML::mapFromLatexToMathMLEquivalents();
  return map.getValue(input, input);
}

std::string MathML::toMathMLInteger(int integer) {
  std::stringstream out;
  if (integer > 0) {
    out << "<mn>" << integer << "</mn>";
    return out.str();
  }
  integer = - integer;
  out
  << "<mrow>"
  << MathML::negativeSign
  << "<mn>"
  << integer
  << "</mn>"
  << "</mrow>";
  return out.str();
}

std::string MathML::processLatex(const std::string& input) {
  MathMLConverter converter(input);
  return converter.convertTextWithLatexTags();
}

std::string MathMLConverter::convertTextWithLatexTags() {
  this->extractTags();
  this->convertTags();
  return this->collectConvertedTags();
}

void MathMLConverter::extractTags() {
  char previous = 0;
  bool isLatex = false;
  this->elements.clear();
  std::string current;
  bool hasPrevious = false;
  for (char element : this->toBeConverted) {
    if (!isLatex && previous == '\\' && element == '(') {
      isLatex = true;
      this->addStringOnTop(current, false);
      current = "";
      hasPrevious = false;
      previous = 0;
      continue;
    }
    if (isLatex && previous == '\\' && element == ')') {
      isLatex = false;
      this->addStringOnTop(current, true);
      current = "";
      hasPrevious = false;
      previous = 0;
      continue;
    }
    if (hasPrevious) {
      current.push_back(previous);
    }
    hasPrevious = true;
    previous = element;
  }
  if (hasPrevious) {
    current.push_back(previous);
    hasPrevious = false;
  }
  this->addStringOnTop(current, isLatex);
}

void MathMLConverter::addStringOnTop(
  const std::string& incomingSnippet, bool isLatex
) {
  if (incomingSnippet.empty()) {
    return;
  }
  this->elements.addOnTop(LatexOrString(incomingSnippet, isLatex));
}

void MathMLConverter::convertTags() {
  Calculator calculator;
  for (LatexOrString& latexOrString : this->elements) {
    if (!latexOrString.isLatex) {
      continue;
    }
    latexOrString.content =
    this->convertLatex(latexOrString.content, calculator);
  }
}

std::string MathMLConverter::convertLatex(
  const std::string& input, Calculator& calculator
) {
  Expression expression;
  bool shouldBeTrue = calculator.parser.parse(input, true, expression);
  if (!shouldBeTrue) {
    return "\\(" + input + "\\)";
  }
  return expression.toMathMLFinal();
}

std::string MathMLConverter::collectConvertedTags() {
  std::stringstream out;
  for (const LatexOrString& latexOrString : this->elements) {
    out << latexOrString.content;
  }
  return out.str();
}
