#include "calculator_interface.h"
#include "math_mathml.h"

std::string MathML::leftParenthesis =
"<mo fence='true' stretchy='true' form='prefix'>(</mo>";
std::string MathML::rightParenthesis =
"<mo fence='true' stretchy='true' form='suffix'>)</mo>";
std::string MathML::leftBracket =
"<mo fence='true' stretchy='true' form='prefix'>[</mo>";
std::string MathML::rightBracket =
"<mo fence='true' stretchy='true' form='suffix'>]</mo>";
std::string MathML::leftAbsoluteValueDelimiter =
"<mo fence='true' stretchy='true' form='prefix'>|</mo>";
std::string MathML::rightAbsoluteValueDelimiter =
"<mo fence='true' stretchy='true' form='suffix'>|</mo>";
std::string MathML::negativeSign = "<mo>&minus;</mo>";
std::string MathML::positiveSign = "<mo>+</mo>";
std::string MathML::mtableDefault = "<mtable>";
std::string MathML::centerDot = "<mo>&sdot;</mo>";

std::string MathML::toMathMLFinal(
  const std::string& mathMLContent,
  const std::string& latex,
  const FormatExpressions* format
) {
  std::stringstream out;
  MathBootstrapScriptType bootstrapType = format ==
  nullptr ?
  MathBootstrapScriptType::backendRendering :
  format->bootstrapScriptType;
  if (
    bootstrapType == MathBootstrapScriptType::equationEditor ||
    bootstrapType == MathBootstrapScriptType::katex
  ) {
    out << "\\(" << latex << "\\)";
    return out.str();
  }
  out
  << "<span class='mathcalculatorbackendrendered'>"
  << "<math displaystyle='true'>"
  << "<semantics>"
  << mathMLContent
  << "<annotation encoding='application/x-tex'>"
  << latex
  << "</annotation>"
  << "</semantics></math>"
  << "</span>";
  return out.str();
}

MapList<std::string, std::string>& MathML::mapFromLatexToMathMLEquivalents() {
  static MapList<std::string, std::string> result;
  if (result.size() == 0) {
    result.setKeyValue("\\sin", "<mo>sin</mo>");
    result.setKeyValue("\\cos", "<mo>cos</mo>");
    result.setKeyValue("\\cot", "<mo>cot</mo>");
    result.setKeyValue("\\tan", "<mo>tan</mo>");
    result.setKeyValue("\\log", "<mo>log</mo>");
    result.setKeyValue("\\int", "<mo>&int;</mo>");
    result.setKeyValue("\\sum", "<mo>&sum;</mo>");
    result.setKeyValue("\\partial", "<mo>&part;</mo>");
    result.setKeyValue("\\alpha", "<mi>&alpha;</mi>");
    result.setKeyValue("\\beta", "<mi>&beta;</mi>");
    result.setKeyValue("\\gamma", "<mi>&gamma;</mi>");
    result.setKeyValue("\\delta", "<mi>&delta;</mi>");
    result.setKeyValue("\\epsilon", "<mi>&epsilon;</mi>");
    result.setKeyValue("\\varepsilon", "<mi>&epsiv;</mi>");
    result.setKeyValue("\\zeta", "<mi>&zeta;</mi>");
    result.setKeyValue("\\eta", "<mi>&eta;</mi>");
    result.setKeyValue("\\theta", "<mi>&theta;</mi>");
    result.setKeyValue("\\iota", "<mi>&iota;</mi>");
    result.setKeyValue("\\kappa", "<mi>&kappa;</mi>");
    result.setKeyValue("\\lambda", "<mi>&lambda;</mi>");
    result.setKeyValue("\\mu", "<mi>&mu;</mi>");
    result.setKeyValue("\\nu", "<mi>&nu;</mi>");
    result.setKeyValue("\\xi", "<mi>&xi;</mi>");
    result.setKeyValue("\\omicron", "<mi>&omicron;</mi>");
    result.setKeyValue("\\pi", "<mi>&pi;</mi>");
    result.setKeyValue("\\rho", "<mi>&rho;</mi>");
    result.setKeyValue("\\sigma", "<mi>&sigma;</mi>");
    result.setKeyValue("\\tau", "<mi>&tau;</mi>");
    result.setKeyValue("\\upsilon", "<mi>&upsilon;</mi>");
    result.setKeyValue("\\phi", "<mi>&phi;</mi>");
    result.setKeyValue("\\chi", "<mi>&chi;</mi>");
    result.setKeyValue("\\psi", "<mi>&psi;</mi>");
    result.setKeyValue("\\omega", "<mi>&omega;</mi>");
    result.setKeyValue("\\Alpha", "<mi>&Alpha;</mi>");
    result.setKeyValue("\\Beta", "<mi>&Beta;</mi>");
    result.setKeyValue("\\Gamma", "<mi>&Gamma;</mi>");
    result.setKeyValue("\\Delta", "<mi>&Delta;</mi>");
    result.setKeyValue("\\Epsilon", "<mi>&Epsilon;</mi>");
    result.setKeyValue("\\Varepsilon", "<mi>&Varepsilon;</mi>");
    result.setKeyValue("\\Zeta", "<mi>&Zeta;</mi>");
    result.setKeyValue("\\Eta", "<mi>&Eta;</mi>");
    result.setKeyValue("\\Theta", "<mi>&Theta;</mi>");
    result.setKeyValue("\\Iota", "<mi>&Iota;</mi>");
    result.setKeyValue("\\Kappa", "<mi>&Kappa;</mi>");
    result.setKeyValue("\\Lambda", "<mi>&Lambda;</mi>");
    result.setKeyValue("\\Mu", "<mi>&Mu;</mi>");
    result.setKeyValue("\\Nu", "<mi>&Nu;</mi>");
    result.setKeyValue("\\Xi", "<mi>&Xi;</mi>");
    result.setKeyValue("\\Omicron", "<mi>&Omicron;</mi>");
    result.setKeyValue("\\Pi", "<mi>&Pi;</mi>");
    result.setKeyValue("\\Rho", "<mi>&Rho;</mi>");
    result.setKeyValue("\\Sigma", "<mi>&Sigma;</mi>");
    result.setKeyValue("\\Tau", "<mi>&Tau;</mi>");
    result.setKeyValue("\\Upsilon", "<mi>&Upsilon;</mi>");
    result.setKeyValue("\\Phi", "<mi>&Phi;</mi>");
    result.setKeyValue("\\Chi", "<mi>&Chi;</mi>");
    result.setKeyValue("\\Psi", "<mi>&Psi;</mi>");
    result.setKeyValue("\\Omega", "<mi>&Omega;</mi>");
    result.setKeyValue("\\psi", "<mo>&psi;</mo>");
    result.setKeyValue("\\dots", "<mo>&mldr;</mo>");
  }
  return result;
}

std::string MathML::latexCommandToMathMLEquivalent(const std::string& input) {
  MapList<std::string, std::string>& map =
  MathML::mapFromLatexToMathMLEquivalents();
  return map.getValue(input, "<mi>" + input + "</mi>");
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

std::string MathML::processLatex(
  const std::string& input, FormatExpressions* format
) {
  MathMLConverter converter(input);
  return converter.convertTextWithLatexTags(format);
}

std::string MathMLConverter::convertTextWithLatexTags(
  FormatExpressions* format
) {
  this->extractTags();
  this->convertTags(format);
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

void MathMLConverter::convertTags(FormatExpressions* format) {
  Calculator& calculator = this->converterCalculator();
  for (LatexOrString& latexOrString : this->elements) {
    if (!latexOrString.isLatex) {
      continue;
    }
    latexOrString.content =
    this->convertLatex(latexOrString.content, calculator, format);
  }
}

std::string MathMLConverter::convertLatex(
  const std::string& input, Calculator& calculator, FormatExpressions* format
) {
  Expression expression;
  bool shouldBeTrue = calculator.parser.parse(input, true, expression);
  if (!shouldBeTrue) {
    return "\\(" + input + "\\)";
  }
  return expression.toMathMLFinal(format);
}

std::string MathMLConverter::collectConvertedTags() {
  std::stringstream out;
  for (const LatexOrString& latexOrString : this->elements) {
    out << latexOrString.content;
  }
  return out.str();
}

Calculator& MathMLConverter::converterCalculator() {
  static Calculator result;
  if (result.operations.size() == 0) {
    result.initialize(Calculator::Mode::educational);
  }
  return result;
}

std::string MathExpressionFormattingProperties::toString() const {
  std::stringstream out;
  out << "{";
  List<std::string> properties;
  if (this->needsParenthesesForMultiplicationOnTheRight) {
    properties.addOnTop("needsParenthesesForMultiplicationOnTheRight");
  }
  if (this->startsWithDigit) {
    properties.addOnTop("startsWithDigit");
  }
  if (this->endsWithDigit) {
    properties.addOnTop("endsWithDigit");
  }
  if (this->startsWithMinus) {
    properties.addOnTop("startsWithMinus");
  }
  if (this->isOne) {
    properties.addOnTop("isOne");
  }
  out << StringRoutines::join(properties, ", ");
  out << "}";
  return out.str();
}
