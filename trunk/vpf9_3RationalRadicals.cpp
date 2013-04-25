//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_3.h"
ProjectInformationInstance ProjectInfoVpf9_3cpp(__FILE__, "Implementation of rational radical extensions. ");

void AlgebraicNumber::operator=(const Rational& other)
{ this->minPolyIndex=this->theRegistry->RegisterRational(other);
  this->rootIndex=0;
}

void AlgebraicNumber::SqrtMe()
{ this->RadicalMe(2);
}

void AlgebraicNumber::RadicalMe(int theRad)
{ Polynomial<Rational> newMinPoly;
  newMinPoly.MakeZero();
  MonomialP tempM;
  for (int i=0; i<this->GetMinPoly().size; i++)
  { tempM=this->GetMinPoly()[i];
    tempM.ExponentMeBy(theRad);
    newMinPoly.AddMonomial(tempM, this->GetMinPoly().theCoeffs[i]);
  }
  this->rootIndex=0;
  this->minPolyIndex= this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(newMinPoly);
}

std::string AlgebraicNumber::ToString(FormatExpressions* theFormat)const
{ if (this->DisplayString!="")
    return this->DisplayString;
  if (this->minPolyIndex==-1 || this->theRegistry==0)
    return "(NonInitializedAlgebraicNumber)";
  std::stringstream out;
  out << "(rootIndex: " << this->rootIndex << ", minpoly: " << this->GetMinPoly().ToString(theFormat) << ")";
  return out.str();
}

void AlgebraicNumber::
ReduceMod
(Polynomial<Rational>& toBeReduced, const List<Polynomial<Rational> >& thePolys,
 List<int>& theNs, Polynomial<Rational>& buffer
 )const
{ if (toBeReduced.IsEqualToZero())
    return;
  if (toBeReduced.GetMinNumVars()!=theNs.size)
  { std::cout << "This is a programming error: function AlgebraicNumber::ReduceModAnBm"
    << " expects as input a polynomial of  " << theNs.size << " variables, but got the "
    << toBeReduced.ToString() << " polynomial of "
    << toBeReduced.GetMinNumVars() << "variables instead. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Rational currentCoeff;
  MonomialP tempM;
//  std::cout << "<br>Reducing " << toBeReduced.ToString() << " mod "
//  << "x_1^" << theN << "- (" << An.ToString() << ") (it has " << An.NumVars << " variables)  and mod "
//  << "x_2^" << theM << "- (" << Bm.ToString() << ") (it has " << Bm.NumVars << " variables)";
  for (int i=0; i<toBeReduced.size; i++)
    for (int j=0; j<theNs.size; j++)
      if (toBeReduced[i](j)>=theNs[j])
      { toBeReduced.PopMonomial(i, tempM, currentCoeff);
  //      std::cout << " select monomial " << tempM.ToString() << " with coeff " << currentCoeff;
        int thePower=tempM[j].NumShort/theNs[j];
        tempM[j]-=theNs[j]*thePower;
        buffer=thePolys[j];
        buffer.RaiseToPower(thePower);
        buffer.MultiplyBy(tempM, currentCoeff);
  //      std::cout << "toBeReduced: " << toBeReduced.ToString() << " buffer: " << buffer.ToString();
        toBeReduced+=buffer;
        i=-1;
        break;
  //      std::cout << "<br>=" <<  toBeReduced.ToString();
      }
//  std::cout << " <br>to get:  " << toBeReduced.ToString();
}

bool AlgebraicNumber::AssignOperation
  (Polynomial<Rational>& theOperationIsModified, const List<AlgebraicNumber>& theOperationArguments)
{ MacroRegisterFunctionWithName("AlgebraicNumber::AssignOperation");
  List<Polynomial<Rational> > thePolys;
  List<int> theNs;
  MonomialP tempM;
  Rational tempRat;
  Polynomial<Rational> buffer1, buffer2;
  thePolys.SetSize(theOperationArguments.size);
  theNs.SetSize(theOperationArguments.size);
  int ProductNs=1;
  int tempN=0;
  for (int i=0; i<theOperationArguments.size; i++)
  { thePolys[i]=theOperationArguments[i].GetMinPoly();
    thePolys[i].ShiftVariableIndicesToTheRight(i);
    if (!thePolys[i].TotalDegree().IsSmallInteger(&tempN))
    { std::cout << "This may or may not be a programming error. "
      << "I am getting an algebraic operation with a polynomial whose exponent is too large "
      << " or is not an integer. If this is not an error , it "
      << "should be handled at a higher level, however"
      << " I don't have time to fix this right now. Crashing to let you know of the situation. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    theNs[i]=tempN;
    ProductNs*=theNs[i];
    thePolys[i].PopMonomial
    (thePolys[i].GetIndexMaxMonomialLexicographicLastVariableStrongest(), tempM, tempRat);
    thePolys[i]/=tempRat*(-1);
  }
  int ProductNsPlusOne= ProductNs+1;
  if (ProductNsPlusOne>=LargeIntUnsigned::SquareRootOfCarryOverBound)
  { std::cout << "This is a programming error. "
    << "Minimal polynomial out of bounds: the upper bound for the minimal poly "
    << " degree is <" << LargeIntUnsigned::SquareRootOfCarryOverBound
    << " which has been exceeded. Until proper error handling is implemented, I shall crash. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Matrix<Rational> theDep;
  theDep.init(ProductNs, ProductNsPlusOne);
  theDep.NullifyAll();
  this->ReduceMod(theOperationIsModified, thePolys, theNs, buffer1);
  buffer2.MakeOne(theNs.size);
  for (int i=0; i<ProductNsPlusOne; i++)
  { for (int j=0; j<buffer2.size; j++)
    { const MonomialP& currentMon=buffer2[j];
      int theIndex=currentMon(0).NumShort;
      for (int k=1; k<theNs.size; k++)
        theIndex=theIndex*theNs[k]+ currentMon(k).NumShort;
      theDep(theIndex, i)=buffer2.theCoeffs[j];
    }
    if (i!=ProductNs)
    { buffer2*=theOperationIsModified;
      this->ReduceMod(buffer2, thePolys, theNs, buffer1);
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
  Polynomial<Rational> newMinPoly;
  newMinPoly.MakeZero();
  tempM.MakeOne(1);
  for (int i=0; i<theEigenVectors[0].size; i++)
  { tempM[0]=i;
    newMinPoly.AddMonomial(tempM, theEigenVectors[0][i]);
  }
  newMinPoly.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
  this->minPolyIndex= this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(newMinPoly);
//  this->theRegistry->theOrigins.SetSize(this->theRegistry.theOrigins.size+1);
  return true;
}

const Polynomial<Rational>& AlgebraicNumber::GetMinPoly()const
{ return this->theRegistry->theMinPolys[this->minPolyIndex];
}

bool AlgebraicNumber::AssignRadical(const LargeInt& undertheRadical, int theRadical)
{ Polynomial<Rational> theMinPoly;
  MonomialP tempM;
  tempM.MakeOne(1);
  tempM[0]=theRadical;
  theMinPoly.MakeZero();
  theMinPoly.AddMonomial(tempM, 1);
  tempM[0]=0;
  theMinPoly.AddMonomial(tempM, undertheRadical);
  this->rootIndex=0;
  this->minPolyIndex=this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(theMinPoly);
//  this->theRegistry.theOrigins.SetSize(this->theRegistry.theOrigins.size+1);
  std::stringstream out;
  out << "\\sqrt{{}" << undertheRadical.ToString() << "}";
  this->DisplayString= out.str();
  return true;
}
