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
