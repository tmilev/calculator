//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_3.h"
ProjectInformationInstance ProjectInfoVpf9_3cpp(__FILE__, "Implementation of rational radical extensions. ");


void RationalAlgebraic::operator=(const Rational& other)
{ AlgebraicInteger tempAI;
  this->MakeZero();
  this->AddMonomial(tempAI, other);
}

void RationalAlgebraic::SqrtMe()
{ this->RadicalMe(2);
}

void RationalAlgebraic::RadicalMe(int theRad)
{ LargeInt tempInt;
  if (this->IsInteger(&tempInt))
  { AlgebraicInteger tempAI;
    tempAI.AssignRadical(tempInt, theRad);
    this->MakeZero();
    this->AddMonomial(tempAI, 1);
  }
  else
  { std::cout << "This part of the program has not been implemented yet. Crashing hard to let you know. "
    << " Email the author to get his lazy butt moving to fix this. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
}

std::string AlgebraicInteger::ToString(FormatExpressions* theFormat)const
{ if (this->minPoly.IsEqualToZero())
    return "";
  if (this->DisplayString!="")
    return this->DisplayString;
  std::stringstream out;
  out << "(rootIndex: " << this->rootIndex << ", minpoly: " << this->minPoly.ToString(theFormat) << ")";
  return out.str();
}

void RationalAlgebraic::Simplify()
{
}

void RationalAlgebraic::GetMinPolyAlgebraicInteger(Polynomial<Rational>& output)const
{
}
