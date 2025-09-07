#include "math_mathml.h"

std::string MathML::leftParenthesis =
"<mo fence='true' stretchy='true' form='prefix'>(</mo>";
std::string MathML::rightParenthesis =
"<mo fence='true' stretchy='true' form='suffix'>)</mo>";

std::string MathML::toMathML(
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
