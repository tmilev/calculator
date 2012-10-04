//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_3.h"
ProjectInformationInstance ProjectInfoVpf9_3cpp(__FILE__, "Implementation of rational radical extensions. ");


void RationalAlgebraic::operator=(const Rational& other)
{ this->minPoly.MakeDegreeOne(1,0, 1, -other);
  this->minPoly.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
}

void RationalAlgebraic::SqrtMe()
{ this->RadicalMe(2);
}

void RationalAlgebraic::RadicalMe(int theRad)
{ Polynomial<Rational> newMinPoly;
  newMinPoly.MakeZero(1);
  MonomialP tempM;
  for (int i=0; i<this->minPoly.size; i++)
  { tempM=this->minPoly[i];
    tempM.monBody*=theRad;
    newMinPoly.AddMonomial(tempM, this->minPoly.theCoeffs[i]);
  }
  this->rootIndex=0;
  this->minPoly=newMinPoly;
}

std::string RationalAlgebraic::ToString(FormatExpressions* theFormat)const
{ if (this->minPoly.IsEqualToZero())
    return "";
  if (this->DisplayString!="")
    return this->DisplayString;
  std::stringstream out;
  out << "(rootIndex: " << this->rootIndex << ", minpoly: " << this->minPoly.ToString(theFormat) << ")";
  return out.str();
}

void RationalAlgebraic::ReduceModAnBm
(Polynomial<Rational>& toBeReduced,
 const Polynomial<Rational>& An, const Polynomial<Rational>& Bm, int theN, int theM,
 Polynomial<Rational>& buffer
 )const
{ if (toBeReduced.IsEqualToZero())
    return;
  if (toBeReduced.GetNumVars()!=2)
  { std::cout << "This is a programming error: function RationalAlgebraic::ReduceModAnBm"
    << " expects as input two variable polynomial, but got the "
    << toBeReduced.ToString() << " polynomial of "
    << toBeReduced.GetNumVars() << " instead. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Rational currentCoeff;
  MonomialP tempM;
  std::cout << "<br>Reducing " << toBeReduced.ToString() << " mod "
  << "x_1^" << theN << "- (" << An.ToString() << ") (it has " << An.NumVars << " variables)  and mod "
  << "x_2^" << theM << "- (" << Bm.ToString() << ") (it has " << Bm.NumVars << " variables)";
  for (int i=0; i<toBeReduced.size; i++)
    if (toBeReduced[i][0]>=theN || toBeReduced[i][1]>=theM)
    { toBeReduced.PopMonomial(i, tempM, currentCoeff);
      std::cout << " select monomial " << tempM.ToString() << " with coeff " << currentCoeff;
      if (tempM[0]>=theN)
      { tempM[0]-=theN;
        buffer=An;
      } else
      { tempM[1]-=theM;
        buffer=Bm;
      }
      buffer.MultiplyBy(tempM, currentCoeff);
      std::cout << "toBeReduced: " << toBeReduced.ToString() << " buffer: " << buffer.ToString();
      if (buffer.ToString()=="4x_{1}")
        std::cout << "!";
      toBeReduced+=buffer;
      i=-1;
      std::cout << "<br>=" <<  toBeReduced.ToString();
    }
  std::cout << " <br>to get:  " << toBeReduced.ToString();
}

void RationalAlgebraic::AssignOperation
(Polynomial<Rational>& theOperationIsModified, const RationalAlgebraic& other)
{ MacroRegisterFunctionWithName("RationalAlgebraic::AssignOperation");
  if (&other==this)
  { RationalAlgebraic buffer=*this;
    this->AssignOperation(theOperationIsModified, buffer);
    return;
  }
  Polynomial<Rational> An, Bm, buffer1, buffer2;
  An=this->minPoly;
  Bm=other.minPoly;
  An.SetNumVariables(2);
  this->MakeOneVarPolyIntoTwoVarPoly(Bm);
  MonomialP tempM;
  Rational tempRat;
  int theN=An.TotalDegree();
  int theM=Bm.TotalDegree();
  int MtimesN=theN*theM;
  int MtimesNplusOne=MtimesN+1;
  if (MtimesN>=LargeIntUnsigned::SquareRootOfCarryOverBound)
  { std::cout << "Minimal polynomial out of bounds: the upper bound for the minimal poly "
    << " degree is <" << LargeIntUnsigned::SquareRootOfCarryOverBound
    << " which has been exceeded. Until proper error handling is implemented, I shall crash. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }

  An.PopMonomial(An.GetIndexMaxMonomialLexicographicLastVariableStrongest(), tempM, tempRat);
  An/=tempRat*(-1);
  Bm.PopMonomial(Bm.GetIndexMaxMonomialLexicographicLastVariableStrongest(), tempM, tempRat);
  Bm/=tempRat*(-1);
  Matrix<Rational> theDep;
  theDep.init(MtimesN, MtimesNplusOne);
  theDep.NullifyAll();
  this->ReduceModAnBm(theOperationIsModified, An, Bm, theN, theM, buffer1);

  buffer2.MakeOne(2);

  for (int i=0; i<MtimesNplusOne; i++)
  { for (int j=0; j<buffer2.size; j++)
    { int theIndex=buffer2[j][0].NumShort*theM+buffer2[j][1].NumShort;
      theDep(theIndex, i)=buffer2.theCoeffs[j];
    }
    if (i!=MtimesNplusOne-1)
    { buffer2*=theOperationIsModified;
      this->ReduceModAnBm(buffer2 , An, Bm, theN, theM, buffer1);
    }
  }
  std::cout << "<br>The matrix: " << theDep.ToString();
  Vectors<Rational> theEigenVectors;
  theDep.FindZeroEigenSpace(theEigenVectors);
  std::cout << "<br>result eigenvectors: " << theEigenVectors.ToString();
  if (theEigenVectors.size<1)
  { std::cout << "This is a programing error: I am asked to find the eigenspace of "
    << " a matrix with more columns than rows, but I get "
    << " zero eigenvectors. Something is very wrong. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->minPoly.MakeZero(1);
  tempM.monBody.SetSize(1);
  for (int i=0; i<theEigenVectors[0].size; i ++)
  { tempM[0]=i;
    this->minPoly.AddMonomial(tempM, theEigenVectors[0][i]);
  }
  this->minPoly.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
}


