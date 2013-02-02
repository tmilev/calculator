//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"
#include "vpfHeader1_3.h"

ProjectInformationInstance ProjectInfoVpf9_2cpp(__FILE__, "Main implementation file, part 3. ");

void ReflectionSubgroupWeylGroup::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << "generator_reflections: ";
  this->simpleGenerators.WriteToFile(output, theGlobalVariables);
  output << "\nouter_generators: ";
  this->ExternalAutomorphisms.WriteToFile(output, theGlobalVariables);
}

void ReflectionSubgroupWeylGroup::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS;
  input >> tempS;
  this->simpleGenerators.ReadFromFile(input, theGlobalVariables);
  input >> tempS;
  this->ExternalAutomorphisms.ReadFromFile(input, theGlobalVariables);
}

bool ReflectionSubgroupWeylGroup::ComputeSubGroupFromGeneratingReflections
(Vectors<Rational>* inputGenerators, List<Vectors<Rational> >* inputExternalAutos,
 GlobalVariables* theGlobalVariables,
 int UpperLimitNumElements, bool recomputeAmbientRho)
{ MemorySaving< HashedList<Vector<Rational> > > bufferOrbit;
  HashedList<Vector<Rational> >& orbitRho = (theGlobalVariables==0) ? bufferOrbit.GetElement() :
  theGlobalVariables->hashedRootsComputeSubGroupFromGeneratingReflections.GetElement();
  this->truncated=false;
  this->Clear();
  orbitRho.Clear();
  if (this->AmbientWeyl.CartanSymmetric.NumRows<1)
    return false;
  if (recomputeAmbientRho)
    this->AmbientWeyl.ComputeRho(false);
  if (inputGenerators!=0)
    this->simpleGenerators=(*inputGenerators);
  if (inputExternalAutos!=0)
    this->ExternalAutomorphisms=*inputExternalAutos;
  this->AmbientWeyl.TransformToSimpleBasisGenerators(this->simpleGenerators);
  this->ComputeRootSubsystem();
  ElementWeylGroup tempEW;
  tempEW.size=0;
  Vector<Rational> tempRoot;
  tempRoot=(this->AmbientWeyl.rho);
  // rho is invariant under external graph automorphisms (!)
  //Hence the below lines. Needs to be fixed (mathematically).
  //for the time being the below lines remain, until I think of how to do it properly.
  tempRoot*=(50);
  tempRoot.TheObjects[0].AddInteger(1);
  orbitRho.AddOnTop(tempRoot);
  this->AddOnTop(tempEW);
  Vector<Rational> currentRoot;
  for (int i=0; i<this->size; i++)
  { tempEW=this->TheObjects[i];
    for (int j=0; j<this->simpleGenerators.size; j++)
    { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[j], orbitRho.TheObjects[i], false, currentRoot);
      if (!orbitRho.Contains(currentRoot))
      { orbitRho.AddOnTop(currentRoot);
        tempEW.AddOnTop(j);
        this->AddOnTop(tempEW);
        tempEW.PopLastObject();
      }
    }
    for (int j=1; j<this->ExternalAutomorphisms.size; j++)
    { orbitRho[i].GetCoordsInBasiS(this->ExternalAutomorphisms[j], currentRoot);
      if (!orbitRho.Contains(currentRoot))
      { orbitRho.AddOnTop(currentRoot);
        tempEW.AddOnTop(j+this->simpleGenerators.size);
        this->AddOnTop(tempEW);
        tempEW.PopLastObject();
      }
    }
    if (UpperLimitNumElements>0)
      if (this->size>=UpperLimitNumElements)
      { this->truncated=true;
        return false;
      }
/*    if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Generated: " << i+1<<" elements";
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
      theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
    }*/
  }
  return true;
}

void ElementWeylAlgebra::MultiplyTwoMonomials
(MonomialWeylAlgebra& left, MonomialWeylAlgebra& right, ElementWeylAlgebra& output)
{ SelectionWithDifferentMaxMultiplicities tempSel;
  int theDimensioN=MathRoutines::Maximum(left.GetMinNumVars(), right.GetMinNumVars());
  tempSel.Multiplicities.initFillInObject(theDimensioN, 0);
  tempSel.MaxMultiplicities.SetSize(theDimensioN);
  for (int i=0; i<theDimensioN; i++)
  { int powerDiffOp=0;
    if (!left.differentialPart[i].IsSmallInteger(&powerDiffOp))
    { std::cout << "This is a programming error. Requested operations "
      << " with elements of weyl algebra"
      << " that have monomials of exponent " << left.differentialPart[i]
      << " which I cannot handle. "
      << " If this is bad user input, it should have been caught at an earlier level. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    tempSel.MaxMultiplicities[i]=powerDiffOp;
  }
  tempSel.elements.initFillInObject(theDimensioN, 0);
  MonomialWeylAlgebra buffer;
  buffer.MakeOne(theDimensioN);
  output.MakeZero();
  int numCycles= tempSel.getTotalNumSubsets();
  Rational coeffBuff;
  for (int i=0; i<numCycles; i++)
  { coeffBuff=1;
    for (int k=0; k<theDimensioN; k++)
    { int multDrop=tempSel.Multiplicities[k];
      int theDOPower=0;
      int thePolPower=0;
      left.differentialPart[k].IsSmallInteger(&theDOPower);
      right.polynomialPart[k].IsSmallInteger(&thePolPower);
      coeffBuff*=Rational::NChooseK(theDOPower, multDrop)
      *Rational::NChooseK(thePolPower, multDrop)* Rational::Factorial(multDrop);
      buffer.polynomialPart[k]=left.polynomialPart[k]+right.polynomialPart[k]-multDrop;
      buffer.differentialPart[k]= left.differentialPart[k]+right.differentialPart[k]-multDrop;
    }
    output.AddMonomial(buffer, coeffBuff);
    tempSel.IncrementSubset();
  }
}

void ElementWeylAlgebra::LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report)
{ this->LieBracketOnTheLeft(standsOnTheLeft, theGlobalVariables);
  report=this->ToString();
}

void ElementWeylAlgebra::LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report)
{ this->LieBracketOnTheRight(standsOnTheRight, theGlobalVariables);
  report=this->ToString();
}

void ElementWeylAlgebra::LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1=(*this);
  tempEl1.MultiplyOnTheLeft(standsOnTheLeft, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2=(standsOnTheLeft);
  tempEl2.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  *this=(tempEl1);
  *this-=(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1=(standsOnTheRight);
  tempEl1.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2=(*this);
  tempEl2.MultiplyOnTheLeft(standsOnTheRight, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  *this=(tempEl1);
  *this-=(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MultiplyOnTheLeft
(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.MakeZero();
  for (int j=0; j<standsOnTheLeft.size; j++)
    for (int i=0; i<this->size; i++)
    { this->MultiplyTwoMonomials(standsOnTheLeft[j], (*this)[i], buffer);
      buffer*=standsOnTheLeft.theCoeffs[j]*this->theCoeffs[i];
      Accum+=(buffer);
    }
  *this=Accum;
}

void ElementWeylAlgebra::operator*=(const ElementWeylAlgebra& standsOnTheRight)
{ ElementWeylAlgebra buffer;
  ElementWeylAlgebra Accum;
  Accum.MakeZero();
  for (int j=0; j<standsOnTheRight.size; j++)
    for (int i=0; i<this->size; i++)
    { this->MultiplyTwoMonomials((*this)[i], standsOnTheRight[j], buffer);
      buffer*=this->theCoeffs[i]*standsOnTheRight.theCoeffs[j];
      Accum+=(buffer);
    }
  *this=Accum;
}

void ElementWeylAlgebra::RaiseToPower(int thePower)
{ ElementWeylAlgebra WeylOne;
  WeylOne.MakeOne(this->GetMinNumVars());
  MathRoutines::RaiseToPower(*this, thePower, WeylOne);
}

std::string MonomialWeylAlgebra::ToString(FormatExpressions* theFormat)const
{ if (this->IsAConstant())
    return "1";
  FormatExpressions tempFormat;
  std::stringstream out;
  int NumVariables=this->GetMinNumVars();
  tempFormat.polyAlphabeT.SetSize(NumVariables);
  for (int i=0; i<NumVariables; i++)
  { std::stringstream tempStream;
    tempStream << "\\partial";
    if (theFormat==0)
      tempStream << "_{" << i+1 << "}";
    else if (theFormat->GetPolyLetter(i)[0]!='x')
      tempStream << "_{" << theFormat->GetPolyLetter(i) << "}";
    else
      tempStream << "_{" << i+1 << "}";
    tempFormat.polyAlphabeT[i]=tempStream.str();
  }
  std::string firstS=this->polynomialPart.ToString(theFormat);
  std::string secondS=this->differentialPart.ToString(&tempFormat);
  if (firstS!="1")
    out << firstS;
  if (secondS!="1")
    out << secondS;
  return out.str();
}

void ElementWeylAlgebra::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i]=1;
  tempMon.differentialPart[j+NumVars]=1;
  this->AddMonomial(tempMon, 1);
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[j]=1;
  tempMon.differentialPart[i+NumVars]=1;
  this->AddMonomial(tempMon,1);
}

void ElementWeylAlgebra::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i]=1;
  tempMon.differentialPart[j]=1;
  this->AddMonomial(tempMon,1);
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[j+NumVars]=1;
  tempMon.differentialPart[i+NumVars]=1;
  this->AddMonomial(tempMon,1);
}

void ElementWeylAlgebra::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i+NumVars]=1;
  tempMon.differentialPart[j]=1;
  this->AddMonomial(tempMon, 1);
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[j+NumVars]=1;
  tempMon.differentialPart[i]=1;
  this->AddMonomial(tempMon,1);
}

void ElementWeylAlgebra::Makedidj(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.differentialPart[i]=1;
  tempMon.differentialPart[j]=1;
  this->AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makexixj(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i]=1;
  tempMon.polynomialPart[j]=1;
  this->AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makexi(int i, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i]=1;
  this->AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makedi(int i, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.differentialPart[i]=1;
  this->AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::Makexidj(int i, int j, int NumVars)
{ this->MakeZero();
  MonomialWeylAlgebra tempMon;
  tempMon.MakeOne(NumVars);
  tempMon.polynomialPart[i]=1;
  tempMon.differentialPart[j]=1;
  this->AddMonomial(tempMon, 1);
}

void ElementWeylAlgebra::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
(const Rational& inputRationalPower, int indexVar, ElementWeylAlgebra& outputDO,
 Polynomial<Rational>& outputDenominator, GlobalVariables& theGlobalVariables)
{ outputDenominator.MakeOne();
  MonomialWeylAlgebra tempMon;
  outputDO.MakeZero();
  int inputPower=0;
  if (!inputRationalPower.IsSmallInteger(&inputPower))
  { std::cout << "This is a programming error: "
    << " I can give you a differential operator only from integer exponent. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (inputPower>=0)
    tempMon.polynomialPart.MakeEi(indexVar, inputPower);
  else
    tempMon.differentialPart.MakeEi(indexVar, -inputPower);
  outputDO.AddMonomial(tempMon, 1);
  inputPower*=-1;
  Polynomial<Rational> newMult;
  newMult.MakeDegreeOne(0, indexVar, 1);
  for (int i=0; i<inputPower; i++)
  { outputDenominator*=newMult;
    newMult-=1;
  }
//  output/=den;
}

bool ElementWeylAlgebra::ActOnPolynomial(Polynomial<Rational>& thePoly)
{ Polynomial<Rational> result;
  result.MakeZero();
  MonomialP resultMon;
  Rational coeff;
  for (int i=0; i<this->size; i++)
    for (int j=0; j<thePoly.size; j++)
    { MonomialP& currentPolMon=thePoly[j];
      MonomialWeylAlgebra& currentOpMon=(*this)[i];
      resultMon=currentPolMon;
      coeff=thePoly.theCoeffs[j];
      coeff*=this->theCoeffs[i];
      for (int k=0; k<currentOpMon.GetMinNumVars(); k++)
      { int numDiff=0;
        if (!currentOpMon.differentialPart[k].IsSmallInteger(&numDiff))
          return false;
        for (; numDiff>0; numDiff--)
        { coeff*=resultMon[k];
          if (coeff.IsEqualToZero())
            break;
          resultMon[k]-=1;
        }
        if (coeff.IsEqualToZero())
          break;
      }
      resultMon*=currentOpMon.polynomialPart;
      result.AddMonomial(resultMon, coeff);
    }
  thePoly=result;
  return true;
}

bool SemisimpleLieAlgebra::AttemptExtendingHEtoHEFWRTSubalgebra
(Vectors<Rational>& RootsWithCharacteristic2,
 Selection& theZeroCharacteristics, Vectors<Rational>& simpleBasisSA, Vector<Rational>& h,
 ElementSemisimpleLieAlgebra<Rational>& outputE, ElementSemisimpleLieAlgebra<Rational>& outputF,
 Matrix<Rational>& outputMatrixSystemToBeSolved,
 PolynomialSubstitution<Rational>& outputSystemToBeSolved,
 Matrix<Rational>& outputSystemColumnVector, GlobalVariables& theGlobalVariables)
{ if (theZeroCharacteristics.CardinalitySelection== theZeroCharacteristics.MaxSize)
    return false;
  Vectors<Rational> rootsInPlay;
  rootsInPlay.size=0;
  int theRelativeDimension = simpleBasisSA.size;
//  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  assert(theRelativeDimension==theZeroCharacteristics.MaxSize);
  Vector<Rational> tempRoot, tempRoot2;
  //format. We are looking for an sl(2) for which e= a_0 g^\alpha_0+... a_kg^\alpha_k, and
  // f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  //where the first \alpha's are ordered as in rootsInPlay.
  //Those are ordered as follows. First come the simple roots of characteristic 2,
  //and the last \alpha's are the members of SelectedExtraPositiveRoots
  //(i.e. Vectors<Rational> equal to the sum of one simple roots
  // of characteristic 2 a simple root of characteristic 0).
  // Then the first k variables of the polynomials below will be a_0, ..., a_k., and
  // the last k variables will be the b_i's
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2}, where
  // l/2 is the index of the root
  // of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+1)/2}} otherwise
  for (int i=0; i<theRelativeDimension; i++)
    if (!theZeroCharacteristics.selected[i])
      rootsInPlay.AddOnTop(simpleBasisSA[i]);
  Vectors<Rational> SelectedExtraPositiveRoots;
  for (int i=0; i<RootsWithCharacteristic2.size; i++)
    if (!simpleBasisSA.Contains(RootsWithCharacteristic2[i]))
      SelectedExtraPositiveRoots.AddOnTop(RootsWithCharacteristic2[i]);
  int numRootsChar2 = rootsInPlay.size;
  rootsInPlay.AddListOnTop(SelectedExtraPositiveRoots);
  int halfNumberVariables = rootsInPlay.size;
  int numberVariables = halfNumberVariables*2;
  MonomialP tempM;
  tempM.MakeOne(numberVariables);
  Matrix<Rational> coeffsF;
  coeffsF.init(1, halfNumberVariables);
  for (int i=0; i<numRootsChar2; i++)
    coeffsF.elements[0][i]=i+1; //(i%2==0)? 1: 2;
  for (int i=numRootsChar2; i<coeffsF.NumCols; i++)
    coeffsF.elements[0][i]=i+1;
  this->initHEFSystemFromECoeffs
  (theRelativeDimension, rootsInPlay, simpleBasisSA,
   SelectedExtraPositiveRoots, numberVariables, numRootsChar2, halfNumberVariables,
   h, coeffsF, outputMatrixSystemToBeSolved, outputSystemColumnVector, outputSystemToBeSolved);
  Matrix<Rational> tempMat, tempMatColumn, tempMatResult;
  tempMat=(outputMatrixSystemToBeSolved);
  tempMatColumn=(outputSystemColumnVector);
  outputF.MakeZero(*this);
  outputE.MakeZero(*this);
//  if(Matrix<Rational> ::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(outputMatrixSystemToBeSolved, outputSystemColumnVector, tempMatResult))
  ChevalleyGenerator tempGen;
  if(Matrix<Rational>::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists
     (tempMat, tempMatColumn, tempMatResult))
  { for (int i=0; i<rootsInPlay.size; i++)
    { tempGen.MakeGenerator(*this, this->GetGeneratorFromRoot(-rootsInPlay[i]));
      outputF.AddMonomial(tempGen, coeffsF.elements[0][i]);
      tempGen.MakeGenerator(*this, this->GetGeneratorFromRoot(rootsInPlay[i]));
      outputE.AddMonomial(tempGen, tempMatResult.elements[i][0]);
    }
    return true;
  }
  return false;
}

void SemisimpleLieAlgebra::initHEFSystemFromECoeffs
(int theRelativeDimension, Vectors<Rational>& rootsInPlay, Vectors<Rational>& simpleBasisSA,
 Vectors<Rational>& SelectedExtraPositiveRoots, int numberVariables, int numRootsChar2, int halfNumberVariables,
 Vector<Rational>& targetH, Matrix<Rational>& inputFCoeffs, Matrix<Rational>& outputMatrixSystemToBeSolved,
 Matrix<Rational>& outputSystemColumnVector, PolynomialSubstitution<Rational>& outputSystemToBeSolved)
{ Vector<Rational> tempRoot;
  MonomialP tempM;
  Rational tempRat;
  HashedList<Vector<Rational> > RootSpacesThatNeedToBeKilled;
  RootSpacesThatNeedToBeKilled.SetExpectedSize(this->theWeyl.RootSystem.size);
//  IndicesEquationsByRootSpace.Reserve(this->theWeyl.RootSystem.size);
  outputSystemToBeSolved.size=0;
  outputMatrixSystemToBeSolved.init(0, numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
    for (int j=0; j<rootsInPlay.size; j++)
    { tempRoot= rootsInPlay[i]-rootsInPlay[j];
      if (this->theWeyl.IsARoot(tempRoot))
      { int indexEquation= RootSpacesThatNeedToBeKilled.GetIndex(tempRoot);
        if (indexEquation==-1)
        { RootSpacesThatNeedToBeKilled.AddOnTop(tempRoot);
          indexEquation=outputSystemToBeSolved.size;
//          IndicesEquationsByRootSpace.AddOnTop(indexEquation);
          outputSystemToBeSolved.SetSize(outputSystemToBeSolved.size+1);
          outputSystemToBeSolved.LastObject()->MakeZero();
        }
        tempM.MakeOne(numberVariables);
        tempM[i]=1;
        tempM[j+halfNumberVariables]=1;
        Rational tempCoeff= this->GetConstant(rootsInPlay[i], -rootsInPlay[j]);
        outputSystemToBeSolved[indexEquation].AddMonomial(tempM, tempCoeff);
        //outputSystemToBeSolved.ComputeDebugString();
      }
    }
  int oldSize=outputSystemToBeSolved.size;
  outputSystemToBeSolved.SetSize(oldSize+this->theWeyl.CartanSymmetric.NumRows);
  for(int i=oldSize; i<outputSystemToBeSolved.size; i++)
    outputSystemToBeSolved[i].MakeZero();
  //outputSystemToBeSolved.ComputeDebugString();
//  ElementSemisimpleLieAlgebra g1, g2;
  for (int i=0; i<rootsInPlay.size; i++)
  { assert(rootsInPlay.size==halfNumberVariables);
    this->GetConstantOrHElement(rootsInPlay[i], -rootsInPlay[i], tempRat, tempRoot);
    for (int j=0; j<this->theWeyl.CartanSymmetric.NumRows; j++)
    { tempM.MakeOne(numberVariables);
      tempM[i]=1;
      tempM[i+halfNumberVariables]=1;
      outputSystemToBeSolved[j+oldSize].AddMonomial(tempM, tempRoot[j]);
    }
  }
  for (int i=0; i<this->theWeyl.CartanSymmetric.NumRows; i++)
    outputSystemToBeSolved[i+oldSize].AddConstant(targetH[i]*(-1));
  outputMatrixSystemToBeSolved.init(outputSystemToBeSolved.size, halfNumberVariables);
  outputSystemColumnVector.init(outputSystemToBeSolved.size, 1);
  outputMatrixSystemToBeSolved.NullifyAll();
  outputSystemColumnVector.NullifyAll();
  for (int i=0; i<outputSystemToBeSolved.size; i++)
    for (int j=0; j<outputSystemToBeSolved[i].size; j++)
    { int lowerIndex=-1; int higherIndex=-1;
      Polynomial<Rational>& currentPoly= outputSystemToBeSolved[i];
      Rational& currentCoeff=currentPoly.theCoeffs[j];
      for (int k=0; k<numberVariables; k++)
        if (currentPoly[j][k]==1)
        { if (k<halfNumberVariables)
            lowerIndex=k;
          else
          { higherIndex=k;
            break;
          }
        }
      if (lowerIndex==-1)
        outputSystemColumnVector.elements[i][0]= currentCoeff*(-1);
      else
        outputMatrixSystemToBeSolved.elements[i][lowerIndex]=currentCoeff* inputFCoeffs.elements[0][higherIndex-halfNumberVariables];
    }
//  outputSystemToBeSolved.ComputeDebugString();
}

void WeylGroup::PerturbWeightToRegularWRTrootSystem(const Vector<Rational>& inputH, Vector<Rational>& output)
{ output=(inputH);
  int indexFirstNonRegular;
  while(!this->IsRegular(output, &indexFirstNonRegular))
  { Vector<Rational>& theBadRoot= this->RootSystem.TheObjects[indexFirstNonRegular];
    Rational maxMovement=0; Rational tempRat1, tempRat2, tempMaxMovement;
    for (int i=0; i<this->RootsOfBorel.size; i++)
    { this->RootScalarCartanRoot(theBadRoot, this->RootsOfBorel.TheObjects[i], tempRat1);
      this->RootScalarCartanRoot(output, this->RootsOfBorel.TheObjects[i], tempRat2);
      if ((!tempRat1.IsEqualToZero()) && (!tempRat2.IsEqualToZero()))
      { tempMaxMovement = tempRat2/tempRat1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement< maxMovement) || maxMovement.IsEqualToZero())
          maxMovement = tempMaxMovement;
      }
    }
    int tempInt=2;
    if (this->RootScalarCartanRoot(theBadRoot, inputH).IsNegative())
      tempInt=-2;
    output+=theBadRoot*maxMovement/tempInt;
  }
}

bool WeylGroup::IsRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot)
{ if (indexFirstPerpendicularRoot!=0)
    *indexFirstPerpendicularRoot=-1;
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootScalarCartanRoot(input, this->RootSystem.TheObjects[i]).IsEqualToZero())
    { if (indexFirstPerpendicularRoot!=0)
        *indexFirstPerpendicularRoot=i;
      return false;
    }
  return true;
}

Rational DynkinDiagramRootSubalgebra::GetSizeCorrespondingWeylGroupByFormula()
{ Rational output=1;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    output*=WeylGroup::GetSizeWeylByFormula(this->DynkinTypeStrings.TheObjects[i].at(0), this->SimpleBasesConnectedComponents.TheObjects[i].size);
  return output;
}

Rational WeylGroup::GetSizeWeylByFormula(char weylLetter, int theDim)
{ //Humphreys, Introduction to Lie algebras and representation theory(1980), page 66, Table 1
  Rational theOutput=1;
  if (weylLetter=='A')
    theOutput= Rational::Factorial(theDim+1);
  if (weylLetter=='B' || weylLetter=='C')
    theOutput= Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim);
  if (weylLetter=='D')
    theOutput= Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim-1);
  if (weylLetter=='E')
  { if (theDim==6)
      theOutput= 51840;
    if (theDim==7)
    { theOutput=1024;
      theOutput*=81*35;
    }
    if (theDim==8)
    { theOutput=1024*16;
      theOutput*=81*3;
      theOutput*=25*7;
    }
  }
  if (weylLetter=='F')
    theOutput=128*9;
  if (weylLetter=='G')
    theOutput=12;
  return theOutput;
}

void rootSubalgebras::ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  //W'' stands for the graph isomorphisms of C(k_ss) extending to Vector<Rational> system isomorphisms of the entire algebra.
  for (int i=fromIndex; i<NumToProcess; i++)
    this->GenerateKintersectBOuterIsos(this->TheObjects[i], theGlobalVariables);
  if (useLatex)
    out << "\\begin{tabular}{ccccc}$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  if (useHtml)
    out << "<br><table><tr><td>k_{ss}</td><td></td><td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  Vectors<Rational> emptyRoots;
  emptyRoots.size=0;
  for (int i=fromIndex; i<NumToProcess; i++)
  { rootSubalgebra& current= this->TheObjects[i];
    ReflectionSubgroupWeylGroup& theOuterIsos= this->CentralizerOuterIsomorphisms.TheObjects[i];
    theOuterIsos.ComputeSubGroupFromGeneratingReflections(&emptyRoots, &theOuterIsos.ExternalAutomorphisms, &theGlobalVariables, 0, true);
    Rational numInnerIsos = current.theCentralizerDiagram.GetSizeCorrespondingWeylGroupByFormula();
    if (useHtml)
      out << "<td>";
    current.theDynkinDiagram.ElementToStrinG(tempS, true);
    out << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    current.theCentralizerDiagram.ElementToStrinG(tempS, true);
    out << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << theOuterIsos.size;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << numInnerIsos.ToString();
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << (numInnerIsos*theOuterIsos.size).ToString();
    if (useHtml)
      out << "</td></tr>";
    if (useLatex)
      out << " \\\\\n";
  }
  if (useLatex)
    out << "\\end{tabular}";
  if(useHtml)
    out << "</table><br>";
  output= out.str();
}

void DrawOperations::drawLineBetweenTwoVectorsBuffer(const Vector<Rational>& vector1, const Vector<Rational>& vector2, unsigned long thePenStyle, int ColorIndex)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawLineBetweenTwoVectors);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawLineBetweenTwoRootsOperations.size);
  this->theDrawLineBetweenTwoRootsOperations.AddObjectOnTopCreateNew();
  this->theDrawLineBetweenTwoRootsOperations.LastObject()->init(vector1, vector2, thePenStyle, ColorIndex);
}

void DrawOperations::drawCircleAtVectorBuffer
(const Vector<Rational>& input, double radius, unsigned long thePenStyle, int theColor)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawCircleAtVector);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawCircleAtVectorOperations.size);
  this->theDrawCircleAtVectorOperations.AddObjectOnTopCreateNew();
  this->theDrawCircleAtVectorOperations.LastObject()->init(input, radius, thePenStyle, theColor);
}

void DrawOperations::drawTextAtVectorBuffer(const Vector<Rational>& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawTextAtVector);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawTextAtVectorOperations.size);
  this->theDrawTextAtVectorOperations.AddObjectOnTopCreateNew();
  this->theDrawTextAtVectorOperations.LastObject()->init(input, inputText, ColorIndex, theFontSize, theTextStyle);
}

void DrawOperations::drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawLine);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawLineOperations.size);
  this->theDrawLineOperations.AddObjectOnTopCreateNew();
  this->theDrawLineOperations.LastObject()->init(X1, Y1, X2, Y2, thePenStyle, ColorIndex);
}

void DrawOperations::drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle)
{ this->TypeNthDrawOperation.AddOnTop(this->typeDrawText);
  this->IndexNthDrawOperation.AddOnTop(this->theDrawTextOperations.size);
  this->theDrawTextOperations.AddObjectOnTopCreateNew();
  this->theDrawTextOperations.LastObject()->init(X1, Y1, inputText, ColorIndex, theFontSize, theTextStyle);
}

void DrawingVariables::drawBufferNoIniT
(DrawOperations& theOps)
{ this->LockedWhileDrawing.LockMe();
  theOps.EnsureProperInitialization();
  theOps.ComputeProjectionsEiVectors();
  theOps.ComputeXYsFromProjectionsEisAndGraphicsUnit();
  int currentPenStyle, currentTextStyle;
  if (this->theDrawClearScreenFunction!=0)
    this->theDrawClearScreenFunction();
  int numOps=theOps.IndexNthDrawOperation.size;
  //the bad debugline is here:
  //numOps=(numOps>10)? 10: numOps;
  for (int i=0; i<numOps; i++)
    switch (theOps.TypeNthDrawOperation.TheObjects[i])
    { case DrawOperations::typeDrawText:
        if (this->theDrawTextFunction!=0)
        { DrawTextOperation& theDrawTextOp= theOps.theDrawTextOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentTextStyle=this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->theDrawTextFunction(theDrawTextOp.X1, theDrawTextOp.Y1, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      case DrawOperations::typeDrawLine:
        if (this->theDrawLineFunction!=0)
        { DrawLineOperation& theDrawLineOp= theOps.theDrawLineOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineOp.X1, theDrawLineOp.Y1, theDrawLineOp.X2, theDrawLineOp.Y2, currentPenStyle, theDrawLineOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawLineBetweenTwoVectors:
        if (this->theDrawLineFunction!=0)
        { DrawLineBetweenTwoRootsOperation& theDrawLineBnTwoOp= theOps.theDrawLineBetweenTwoRootsOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineBnTwoOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineBnTwoOp.precomputedX1, theDrawLineBnTwoOp.precomputedY1, theDrawLineBnTwoOp.precomputedX2, theDrawLineBnTwoOp.precomputedY2, currentPenStyle, theDrawLineBnTwoOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawTextAtVector:
        if (this->theDrawTextFunction!=0)
        { DrawTextAtVectorOperation& theDrawTextOp= theOps.theDrawTextAtVectorOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentTextStyle= this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->theDrawTextFunction(theDrawTextOp.precomputedX, theDrawTextOp.precomputedY, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      case DrawOperations::typeDrawCircleAtVector:
        if (this->theDrawCircleFunction!=0)
        { DrawCircleAtVectorOperation& theDrawCircleOp= theOps.theDrawCircleAtVectorOperations.TheObjects[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawCircleOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawCircleFunction(theDrawCircleOp.precomputedX, theDrawCircleOp.precomputedY, theDrawCircleOp.radius, theDrawCircleOp.thePenStyle, theDrawCircleOp.ColorIndex);
        }
        break;
      default: break;
    }
  this->LockedWhileDrawing.UnlockMe();
}

int DrawingVariables::GetActualPenStyleFromFlagsAnd(int inputPenStyle)
{ if (inputPenStyle==this->PenStyleInvisible)
    return this->PenStyleInvisible;
  if (inputPenStyle== this->PenStyleDashed)
    return this->PenStyleDashed;
  if (inputPenStyle==this->PenStyleDotted)
    return this->PenStyleDotted;
  if (inputPenStyle==this->PenStyleNormal)
    return this->PenStyleNormal;
  if (!this->flagDrawingInvisibles)
    if (inputPenStyle == this->PenStyleLinkToOriginPermanentlyZeroChamber || inputPenStyle == this->PenStyleLinkToOriginZeroChamber || inputPenStyle == this->PenStyleZeroChamber || inputPenStyle == this->PenStylePermanentlyZeroChamber)
      return this->PenStyleInvisible;
  if (inputPenStyle==this->PenStyleLinkToOrigin || inputPenStyle==this->PenStyleLinkToOriginPermanentlyZeroChamber || inputPenStyle==this->PenStyleLinkToOriginZeroChamber)
  { if (this->flagDrawingLinkToOrigin)
      return this->PenStyleDashed;
    else
      return this->PenStyleInvisible;
  }
  if (inputPenStyle==this->PenStylePermanentlyZeroChamber || inputPenStyle==this->PenStyleZeroChamber)
    return this->PenStyleDotted;
  return this->PenStyleNormal;
}

int DrawingVariables::GetActualTextStyleFromFlagsAnd(int inputTextStyle)
{ if (inputTextStyle==this->TextStyleInvisible)
    return this->TextStyleInvisible;
  if (inputTextStyle==this->TextStyleNormal)
    return this->TextStyleNormal;
  if (!this->flagDrawChamberIndices && (inputTextStyle==this->TextStyleChamber || inputTextStyle==this->TextStylePermanentlyZeroChamber || inputTextStyle==this->TextStyleZeroChamber))
    return this->TextStyleInvisible;
  if (!this->flagDrawingInvisibles && (inputTextStyle==this->TextStylePermanentlyZeroChamber || inputTextStyle==this->TextStyleZeroChamber))
    return this->TextStyleInvisible;
  return this->TextStyleNormal;
}

void DrawingVariables::drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ this->theBuffer.drawLineBuffer(X1, Y1, X2, Y2, thePenStyle, ColorIndex);
}

void DrawingVariables::drawTextBuffer(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile)
{ this->theBuffer.drawTextBuffer(X1, Y1, inputText, color, this->fontSizeNormal, this->TextStyleNormal);
}

void DrawingVariables::drawString(DrawElementInputOutput& theDrawData, const std::string& input, int theFontSize, int theTextStyle)
{ theDrawData.outputHeight=0; theDrawData.outputWidth=0;
  if (input=="")
    return;
  for (unsigned int i=0; i<input.size(); i++)
  { std::string tempS;
    tempS=input.at(i);
    this->theBuffer.drawTextBuffer(theDrawData.outputWidth+theDrawData.TopLeftCornerX, theDrawData.outputHeight+theDrawData.TopLeftCornerY, tempS, 0, theFontSize, theTextStyle);
    theDrawData.outputWidth+=(int)(((double) theFontSize)/1.15);
  }
}

void Rational::DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData)
{ std::string tempS;
  tempS=this->ToString();
  theGlobalVariables.theDrawingVariables.theBuffer.drawTextBuffer(theDrawData.TopLeftCornerX, theDrawData.TopLeftCornerY, tempS, 0, theGlobalVariables.theDrawingVariables.fontSizeNormal, theGlobalVariables.theDrawingVariables.TextStyleNormal);
  theDrawData.outputHeight=10;
  theDrawData.outputWidth=10*tempS.size();
}

bool rootSubalgebras::ReadFromDefaultFileNilradicalGeneration(GlobalVariables* theGlobalVariables)
{ std::fstream theFile;
  if (CGI::OpenFileCreateIfNotPresent(theFile, "./theNilradicalsGenerator.txt", false, false, false))
  { theFile.seekg(0);
    this->ReadFromFileNilradicalGeneration(theFile, theGlobalVariables);
    return true;
  }
  return false;
}

void rootSubalgebras::WriteToDefaultFileNilradicalGeneration(GlobalVariables* theGlobalVariables)
{ std::fstream theFile;
  CGI::OpenFileCreateIfNotPresent(theFile, "./theNilradicalsGenerator.txt", false, true, false);
  this->WriteToFileNilradicalGeneration(theFile, theGlobalVariables);
}

void rootSubalgebras::WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables* theGlobalVariables)
{ this->GetOwnerWeyl().WriteToFile(output);
  output << "Number_subalgebras: " << this->size << "\n";
  //////////////////////////////////////////////////////////////////////////////////////
  output << "Index_current_SA_nilradicals_generation: " << this->IndexCurrentSANilradicalsGeneration << "\n";
  output << "Num_SAs_to_be_processed: " << this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration << "\n";
  output << "Parabolics_counter_nilradical_generation: " << this->parabolicsCounterNilradicalGeneration << "\n";
  output << "Num_SAs_processed: " << this->NumSubalgebrasProcessed << "\n";
  output << "Num_cone_condition_failures: " << this->NumConeConditionFailures << "\n";
  output << "Implied_selections: ";
  this->ImpiedSelectionsNilradical.WriteToFile(output);
  output << "Counters_nilradicals_generation: ";
  output << this->CountersNilradicalsGeneration;
  output << "\nRecursion_depth: " << this->RecursionDepthNilradicalsGeneration << "\n";
  ////////////////////////////////////////////////////////////////////////////////////////
  for (int  i=0; i<this->size; i++)
    this->TheObjects[i].WriteToFileNilradicalGeneration(output, theGlobalVariables, *this);
  this->theGoodRelations.WriteToFile(output, theGlobalVariables);
  this->theBadRelations.WriteToFile(output, theGlobalVariables);
}

void rootSubalgebras::ReadFromFileNilradicalGeneration(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS; int tempI;
  this->GetOwnerWeyl().ReadFromFile(input);
  this->GetOwnerWeyl().ComputeRho(true);
  input >> tempS >> tempI;
  assert(tempS=="Number_subalgebras:");
  this->SetSize(tempI);
  //////////////////////////////////////////////////////////////////////////////////////
  input >> tempS >> this->IndexCurrentSANilradicalsGeneration;
  input >> tempS >> this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration;
  input >> tempS >> this->parabolicsCounterNilradicalGeneration;
  input >> tempS >> this->NumSubalgebrasProcessed;
  input >> tempS >> this->NumConeConditionFailures;
  input >> tempS;
  this->ImpiedSelectionsNilradical.ReadFromFile(input);
  input >> tempS;
  input >> this->CountersNilradicalsGeneration;
  input >> tempS >> this->RecursionDepthNilradicalsGeneration;
  /////////////////////////////////////////////////////////////////////////////////////
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].ReadFromFileNilradicalGeneration(input, theGlobalVariables, *this);
  this->theGoodRelations.ReadFromFile(input, theGlobalVariables, *this);
  this->theBadRelations.ReadFromFile(input, theGlobalVariables, *this);
  this->flagNilradicalComputationInitialized=true;
}

void rootSubalgebra::WriteToFileNilradicalGeneration(std::fstream& output, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ output << "Simple_basis_k: ";
  this->SimpleBasisK.WriteToFile(output, theGlobalVariables);
}

void rootSubalgebra::ReadFromFileNilradicalGeneration
(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS;
  input >> tempS;
  assert(tempS=="Simple_basis_k:");
  this->SimpleBasisK.ReadFromFile(input, theGlobalVariables);
  this->genK=(this->SimpleBasisK);
  this->init(*owner.owneR);
  this->ComputeAll();
}

WeylGroup& rootSubalgebra::GetAmbientWeyl()
{ return this->GetOwnerSSalg().theWeyl;
}

SemisimpleLieAlgebra& rootSubalgebra::GetOwnerSSalg()
{ if (this->owneR==0)
  { std::cout << "This is a programming error. Attempting to access "
    << "ambient Lie algebra of non-initialized root subalgebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return (*this->owneR);
}

void rootSubalgebra::GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter)
{ impliedSelections.SetSize(this->kModules.size+1);
  parabolicsCounter=0;
}

void rootSubalgebra::GeneratePossibleNilradicals
(Controller& PauseMutex, List<Selection>& impliedSelections, int& parabolicsCounter,
 GlobalVariables& theGlobalVariables, bool useParabolicsInNilradical,
 rootSubalgebras& owner, int indexInOwner)
{  //this->ComputeAll();
  this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods, &theGlobalVariables);
  if (this->flagAnErrorHasOccuredTimeToPanic)
    this->theMultTable.ComputeDebugString(*this);
  this->NumTotalSubalgebras=0;
  Selection emptySel;
  emptySel.init(this->SimpleBasisCentralizerRoots.size);
  owner.ComputeActionNormalizerOfCentralizerIntersectNilradical(emptySel, *this, theGlobalVariables);
  int numCycles= MathRoutines::TwoToTheNth(this->SimpleBasisCentralizerRoots.size);
  List<Selection> StartingNilradicalsNoRepetition;
  StartingNilradicalsNoRepetition.ReservE(numCycles);
  Selection tempSel, ParabolicsGenerator;
  if (!owner.flagNilradicalComputationInitialized)
    owner.CountersNilradicalsGeneration.SetSize(this->kModules.size+1);
  if (owner.flagStoringNilradicals)
    owner.storedNilradicals.TheObjects[indexInOwner].size=0;
  Vectors<Rational> tempRootsTest;
  if (useParabolicsInNilradical)
  { this->flagFirstRoundCounting=false;
    ParabolicsGenerator.init(this->SimpleBasisCentralizerRoots.size);
    for (int i=0; i<numCycles; i++, ParabolicsGenerator.incrementSelection())
    { tempSel.init(this->kModules.size);
      for (int j=0; j<this->CentralizerRoots.size; j++)
        if (this->rootIsInNilradicalParabolicCentralizer(ParabolicsGenerator, this->CentralizerRoots.TheObjects[j]))
          tempSel.AddSelectionAppendNewIndex(j);
      if (owner.flagUsingActionsNormalizerCentralizerNilradical)
        owner.RaiseSelectionUntilApproval(tempSel, theGlobalVariables);
      StartingNilradicalsNoRepetition.AddOnTopNoRepetition(tempSel);
      //StartingNilradicalsNoRepetition.AddOnTop(tempSel);
    }

/*      tempRootsTest.size=0;
      std::string tempS; std::stringstream out;
      for (int s=0; s<impliedSelections.TheObjects[0].CardinalitySelection; s++)
        tempRootsTest.AddOnTop(this->kModules.TheObjects[impliedSelections.TheObjects[0].elements[s]].TheObjects[0]);
      tempS=out.str();
      assert(this->RootsDefineASubalgebra(tempRootsTest));*/
    for (; parabolicsCounter<StartingNilradicalsNoRepetition.size; parabolicsCounter++, owner.flagNilradicalComputationInitialized=false)
    { if (!owner.flagNilradicalComputationInitialized)
      { impliedSelections.TheObjects[0].Assign(StartingNilradicalsNoRepetition.TheObjects[parabolicsCounter]);
        owner.RecursionDepthNilradicalsGeneration=0;
        owner.CountersNilradicalsGeneration.TheObjects[0]=this->CentralizerRoots.size;
      }
      this->GeneratePossibleNilradicalsRecursive(PauseMutex, theGlobalVariables, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
    }
  }
  else
  { this->flagFirstRoundCounting=false;
    impliedSelections.TheObjects[0].init(this->kModules.size);
    owner.RecursionDepthNilradicalsGeneration=0;
    owner.CountersNilradicalsGeneration.TheObjects[0]=0;
    this->GeneratePossibleNilradicalsRecursive(PauseMutex, theGlobalVariables, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
  }
}

void coneRelations::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << "num_rels: " << this->size << "\n";
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].WriteToFile(output, theGlobalVariables);
}

void coneRelations::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS; int tempI;
  this->Clear();
  input >> tempS >> tempI;
  coneRelation tempRel;
  for (int i=0; i<tempI; i++)
  { tempRel.ReadFromFile(input, theGlobalVariables, owner);
    this->AddRelationNoRepetition(tempRel, owner, tempRel.IndexOwnerRootSubalgebra);
  }
}

void coneRelation::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ this->AlphaCoeffs.WriteToFile(output);
  this->Alphas.WriteToFile(output, theGlobalVariables);
  output << this->AlphaKComponents;
  this->BetaCoeffs.WriteToFile(output);
  this->Betas.WriteToFile(output, theGlobalVariables);
  output << this->BetaKComponents;
  output << "Index_owner_root_SA: " << this->IndexOwnerRootSubalgebra << " ";
}

void coneRelation::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, rootSubalgebras& owner)
{ std::string tempS;
  this->AlphaCoeffs.ReadFromFile(input);
  this->Alphas.ReadFromFile(input, theGlobalVariables);
  input >> this->AlphaKComponents;
  this->BetaCoeffs.ReadFromFile(input);
  this->Betas.ReadFromFile(input, theGlobalVariables);
  input >> this->BetaKComponents;
  input >> tempS >> this->IndexOwnerRootSubalgebra;
  assert(tempS=="Index_owner_root_SA:");
  this->ComputeTheDiagramAndDiagramRelAndK(owner.TheObjects[this->IndexOwnerRootSubalgebra]);
  this->ComputeDebugString(owner, true, true);
}

void WeylGroup::WriteToFile(std::fstream& output)
{ output << "Weyl_group: ";
  std::cout << "This code is not implemented yet (due to a regression).";
  assert(false);
//  output << this->WeylLetter << " " << this->CartanSymmetric.NumRows << "\n";
  output << "Long_root_length: ";
//  this->lengthLongestRootSquared.WriteToFile(output);
  output << "\n";
  this->CartanSymmetric.WriteToFile(output);
}

void WeylGroup::ReadFromFile(std::fstream& input)
{ std::string tempS;
//  int tempI;
  input >> tempS;
  std::cout << "This code is not implemented yet (due to a regression).";
  assert(false);
  //input >> this->WeylLetter >> tempI >> tempS;
  assert(tempS=="Long_root_length:");
//  this->lengthLongestRootSquared.ReadFromFile(input);
  this->CartanSymmetric.ReadFromFile(input);
}

void rootSubalgebras::ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots, tempRoots2;
  std::stringstream out;
  std::stringstream out2;
  std::string tempS;
  int numNonSolvableNonReductive=0;
  char simpleType;
  int theRank;
  if (!this->GetOwnerWeyl().theDynkinType.IsSimple(&simpleType, &theRank))
  { std::cout << "This is a programming error: ElementToStringConeConditionNotSatisfying"
    << " called on a non-simple Lie algebra. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (simpleType=='B')
    out << "$\\mathrm{so}(2n+1)$ is realized as a matrix Lie algebra as $\\left\\{\\left(\\begin{array}{c|c|c}A&\\begin{array}{c}v_1\\\\ \\vdots \\\\ v_n\\end{array} &C=-C^T \\\\\\hline \\begin{array}{ccc}w_1 &\\dots&  w_n\\end{array} &0& \\begin{array}{ccc}-v_n &\\dots&  -v_1\\end{array} \\\\\\hline D=-D^T&\\begin{array}{c}-w_n\\\\ \\vdots \\\\ -w_1\\end{array} & -A^T\\end{array}\\right)\\right\\}$.\n\n";
  if (simpleType=='C')
    out << "$\\mathrm{sp}(2n)$ is realized as a matrix Lie algebra as $\\left\\{\\left(\\begin{array}{c|c}A& C \\\\\\hline D& -A^T\\end{array}\\right)| C=C^T, D=D^T\\right\\}.$";
  out << " In this realization, the Cartan subalgebra $\\mathfrak{h}$ can be chosen to consist of the diagonal matrices of the above form.\n\n";
  if (!includeMatrixForm)
  { out << "\n\\begin{longtable}{r|l}\n\\multicolumn{2}{c}{";
    if (simpleType=='B')
      out << " $ \\mathfrak{g}\\simeq \\mathrm{so("
      << this->GetOwnerWeyl().CartanSymmetric.NumRows*2+1 << ")}$";
    if (simpleType=='C')
      out << " $\\mathfrak{g}\\simeq \\mathrm{sp("
      << this->GetOwnerWeyl().CartanSymmetric.NumRows*2 << ")}$";
    out << "} \\\\\\hline";
  }
  for (int i=0; i<this->size-1; i++)
    if (this->storedNilradicals[i].size>0)
    { rootSubalgebra& currentRootSA=this->TheObjects[i];
      tempRoots.size=0;
      for (int j=0; j<currentRootSA.PositiveRootsK.size; j++)
      { tempRoots.AddOnTop(currentRootSA.PositiveRootsK[j]);
        tempRoots.AddOnTop(-currentRootSA.PositiveRootsK[j]);
      }
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{1.5pt}\n\n";
      else
        out << "\\hline\\begin{tabular}{r}";
      out << "$\\Delta(\\mathfrak{k})$ is of type "
      << currentRootSA.theDynkinDiagram.DynkinStrinG << "; ";
      if (!includeMatrixForm)
        out << "\\\\";
      currentRootSA.GetAmbientWeyl().GetEpsilonCoords(currentRootSA.PositiveRootsK, tempRoots2);
      tempS=tempRoots2.ElementToStringEpsilonForm(true, false, false);
      out << " $\\Delta^+(\\mathfrak{k})=$ " << tempS;
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{0.3pt}\n\n";
      else
        out << "\\end{tabular} &\n\\begin{tabular}{l}";
      int numNonReductiveCurrent=0;
      for (int j=0; j<this->storedNilradicals[i].size; j++)
      { List<int>& currentNilrad= this->storedNilradicals[i][j];
        if (currentNilrad.size>0)
        { numNonSolvableNonReductive++;
          numNonReductiveCurrent++;
          tempRoots.size= currentRootSA.PositiveRootsK.size*2;
          for(int k=0; k<currentNilrad.size; k++)
            tempRoots.AddListOnTop(currentRootSA.kModules.TheObjects[currentNilrad.TheObjects[k]]);
          this->ElementToStringRootSpaces(tempS, includeMatrixForm, tempRoots, theGlobalVariables);
          out << tempS << "\n";
          if (numNonReductiveCurrent%2==0)
          { out << "\n\n";
            if (!includeMatrixForm)
              out << "\\\\";
          }
        }
      }
      if (!includeMatrixForm)
        out << "\\end{tabular}\n\n\\\\";
    }
  if (!includeMatrixForm)
    out << "\n\\end{longtable}";
  out2 << "\n\nThe number of non-conjugate non-solvable non-reductive "
  << "root subalgebras not satisfying the cone condition is: "
  << numNonSolvableNonReductive << "\n\n";
  tempS = out.str();
  out2 << tempS;
  output=out2.str();
}

void rootSubalgebras::ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; std::stringstream out;
  Vectors<Rational> epsCoords;
  Matrix<int> tempMat;
  char simpleType;
  int theDimension;
  if (!this->GetOwnerWeyl().theDynkinType.IsSimple(&simpleType, &theDimension))
  { std::cout << "This is a programming error: ElementToStringConeConditionNotSatisfying"
    << " called on a non-simple Lie algebra. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (simpleType=='B')
  { this->GetOwnerWeyl().GetEpsilonCoords(input, epsCoords);
    tempMat.MakeIdMatrix(theDimension*2+1, 1, 0);
    tempMat.elements[theDimension][theDimension]=0;
    for (int i=0; i<epsCoords.size; i++)
    { bool isShort=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      Vector<Rational>& currentRoot=epsCoords[i];
      for (int j=0; j<theDimension; j++)
      { if (currentRoot[j]!=0)
        { isShort=!isShort;
          if(isShort)
          { if (currentRoot[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot[j].IsPositive())
              secondSignIsPositive=true;
            else
              secondSignIsPositive=false;
            secondIndex=j;
          }
        }
      }
      if (!isShort)
      { bool signsAreDifferent=(firstSignIsPositive!=secondSignIsPositive);
        if (signsAreDifferent)
        { int positiveIndex, negativeIndex;
          if (firstSignIsPositive)
          { positiveIndex= firstIndex;
            negativeIndex=secondIndex;
          } else
          { positiveIndex= secondIndex;
            negativeIndex=firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex]=1;
          tempMat.elements[theDimension+1+negativeIndex][theDimension+1+positiveIndex]=-1;
        } else
        { if (firstSignIsPositive)
          { tempMat.elements[firstIndex][secondIndex+theDimension+1]=1;
            tempMat.elements[secondIndex][firstIndex+theDimension+1]=-1;
          } else
          { tempMat.elements[theDimension+1+firstIndex][secondIndex]=1;
            tempMat.elements[theDimension+1+secondIndex][firstIndex]=-1;
          }
        }
      } else
      { if (firstSignIsPositive)
        { tempMat.elements[firstIndex][theDimension]=1;
          tempMat.elements[theDimension][theDimension+1+firstIndex]=-1;
        } else
        { tempMat.elements[theDimension][firstIndex]=1;
          tempMat.elements[firstIndex+1+theDimension][theDimension]=-1;
        }
      }
    }
  }
  if (simpleType=='C')
  { this->GetOwnerWeyl().GetEpsilonCoords(input, epsCoords);
    tempMat.MakeIdMatrix(theDimension*2, 1, 0);
    for (int i=0; i<epsCoords.size; i++)
    { bool isLong=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      Vector<Rational>& currentRoot=epsCoords[i];
      for (int j=0; j<theDimension; j++)
      { if (currentRoot[j]!=0)
        { isLong=!isLong;
          if(isLong)
          { if (currentRoot[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot[j].IsPositive())
              secondSignIsPositive=true;
            else
              secondSignIsPositive=false;
            secondIndex=j;
          }
        }
      }
      if (!isLong)
      { bool signsAreDifferent=(firstSignIsPositive!=secondSignIsPositive);
        if (signsAreDifferent)
        { int positiveIndex=-1, negativeIndex=-1;
          if (firstSignIsPositive)
          { positiveIndex= firstIndex;
            negativeIndex=secondIndex;
          } else
          { positiveIndex= secondIndex;
            negativeIndex=firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex]=1;
          tempMat.elements[theDimension+negativeIndex][theDimension+positiveIndex]=-1;
        } else
        { if (firstSignIsPositive)
          { tempMat.elements[firstIndex][secondIndex+theDimension]=1;
            tempMat.elements[secondIndex][firstIndex+theDimension]=1;
          } else
          { tempMat.elements[theDimension+firstIndex][secondIndex]=1;
            tempMat.elements[theDimension+secondIndex][firstIndex]=1;
          }
        }
      } else
      { if (firstSignIsPositive)
          tempMat.elements[firstIndex][theDimension+firstIndex]=1;
        else
          tempMat.elements[theDimension+firstIndex][firstIndex]=1;
      }
    }
  }
  if (includeMatrixForm)
    out << "\\begin{tabular}{cc} \\begin{tabular}{l}";
  out << "$\\Delta(\\mathfrak{n})=$";
  if (includeMatrixForm)
    out <<"\\\\";
  int numNilradicalRootSpaces=0;
  for (int i=0; i<epsCoords.size; i++)
  { Vector<Rational>& currentRoot=epsCoords.TheObjects[i];
    tempS=currentRoot.ToStringEpsilonFormat();
    if (!epsCoords.Contains(-currentRoot))
    { out << tempS << ", ";
      numNilradicalRootSpaces++;
    }
    if (includeMatrixForm)
      if (numNilradicalRootSpaces%2==0 && numNilradicalRootSpaces!=0)
        out << "\\\\";
  }
  if (includeMatrixForm)
  { out << "\\end{tabular} & $\\mathfrak{l}=\\left(\\begin{array}{";
    for (int i=0; i<tempMat.NumCols; i++)
    { out << "c";
      if (simpleType=='B' && (i==theDimension-1 || i==theDimension))
        out << "|";
    }
    out << "}";
    for (int i=0; i< tempMat.NumRows; i++)
    { if (simpleType=='B' && (i==theDimension || i==theDimension+1))
        out << "\\hline";
      for (int j=0; j<tempMat.NumCols; j++)
      { if (tempMat.elements[i][j]!=0 && tempMat.elements[j][i]==0)
          out << "\\blacktriangle";
        if (tempMat.elements[i][j]!=0 && tempMat.elements[j][i]!=0)
        out << "\\bullet";
        if (j!=tempMat.NumCols-1)
          out << "&";
        else
          out << "\\\\";
      }
    }
    out << "\\end{array}\\right)$ \\end{tabular}  ";
  }
  output=out.str();
}

template <class CoefficientType>
void ElementSemisimpleLieAlgebra<CoefficientType>::MakeGGenerator
(const Vector<Rational>& theRoot, SemisimpleLieAlgebra& inputOwner)
{ this->MakeGenerator(inputOwner.GetGeneratorFromRoot(theRoot), inputOwner);
}

template <class CoefficientType>
void ElementSemisimpleLieAlgebra<CoefficientType>::AssignVectorNegRootSpacesCartanPosRootSpaces
(const Vector<Rational>& input, SemisimpleLieAlgebra& owner)
{ //Changing RootSystem order invalidates this function!
  this->MakeZero(owner);
  ChevalleyGenerator tempGenerator;
  for (int i=0; i<input.size; i++)
    if (input[i]!=0)
    { tempGenerator.MakeGenerator(*this->owneR, i);
      this->AddMonomial(tempGenerator, input[i]);
    }
}

template <class CoefficientType>
void ElementSemisimpleLieAlgebra<CoefficientType>::ElementToVectorNegativeRootSpacesFirst(Vector<Rational>& output)const
{ output.MakeZero(this->GetOwner().GetNumGenerators());
  for (int i=0; i<this->size; i++)
    output[this->TheObjects[i].theGeneratorIndex]=this->theCoeffs[i];
}

void SemisimpleLieAlgebra::ComputeOneAutomorphism
(GlobalVariables& theGlobalVariables, Matrix<Rational>& outputAuto, bool useNegativeRootsFirst)
{ rootSubalgebra theRootSA;
  theRootSA.init(*this);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  theRootSA.genK.MakeEiBasis(theDimension);
  ReflectionSubgroupWeylGroup theAutos;
  theRootSA.GenerateAutomorphismsPreservingBorel(theGlobalVariables, theAutos);
  Matrix<Rational> mapOnRootSpaces;
  int theAutoIndex= theAutos.ExternalAutomorphisms.size>1? 1 : 0;
  /*if (this->theWeyl.WeylLetter=='D' && theDimension==4)
    theAutoIndex=2;
*/
  mapOnRootSpaces.AssignRootsToRowsOfMatrix(theAutos.ExternalAutomorphisms.TheObjects[theAutoIndex]);
  mapOnRootSpaces.Transpose(theGlobalVariables);
//  mapOnRootSpaces.ComputeDebugString();
//  Matrix<Rational>  theDet=mapOnRootSpaces;
//  Rational tempRat;
//  theDet.ComputeDeterminantOverwriteMatrix(tempRat);
//  std::cout << " ... and the det is: " << tempRat.ToString();
  Selection NonExplored;
  int numRoots= this->theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;

  this->ComputeChevalleyConstantS(&theGlobalVariables);
  List<ElementSemisimpleLieAlgebra<Rational> > Domain, Range;
  Range.SetSize(numRoots+theDimension);
  Domain.SetSize(numRoots+theDimension);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i=0; i<theDimension; i++)
  { domainRoot.MakeEi(theDimension, i);
    mapOnRootSpaces.ActOnVectorColumn(domainRoot, rangeRoot);
    tempElt.MakeHgenerator(domainRoot, *this);
    Domain[numRoots+i]=tempElt;
    tempElt.MakeHgenerator(rangeRoot, *this);
    Range[numRoots+i]=tempElt;
    for (int i=0; i<2; i++, domainRoot.Minus(), rangeRoot.Minus())
    { int theIndex= this->theWeyl.RootSystem.GetIndex(rangeRoot);
      tempElt.MakeGGenerator(rangeRoot, *this);
      Range[theIndex]=tempElt;
      tempElt.MakeGGenerator(domainRoot, *this);
      Domain.TheObjects[theIndex]=tempElt;
      NonExplored.RemoveSelection(theIndex);
    }
  }
  Vector<Rational> left, right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      Vector<Rational>& current = this->theWeyl.RootSystem[theIndex];
      for (int j=0; j<theDimension; j++)
      { left.MakeEi(theDimension, j);
        for (int k=0; k<2; k++, left.Minus())
        { right= current-left;
          if (this->theWeyl.IsARoot(right))
          { int leftIndex= this->theWeyl.RootSystem.GetIndex(left);
            int rightIndex= this->theWeyl.RootSystem.GetIndex(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSemisimpleLieAlgebra<Rational>& leftDomainElt=Domain[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightDomainElt= Domain[rightIndex];
              this->LieBracket(leftDomainElt, rightDomainElt, Domain[theIndex]);
              ElementSemisimpleLieAlgebra<Rational>& leftRangeElt=Range[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightRangeElt= Range[rightIndex];
              this->LieBracket(leftRangeElt, rightRangeElt, Range[theIndex]);
              NonExplored.RemoveSelection(theIndex);
            }
          }
        }
      }
    }
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.SetSize(Range.size);
  vectorsRight.SetSize(Range.size);
  if (!useNegativeRootsFirst)
    for (int i=0; i<Range.size; i++)
    { Range[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
      Domain[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  else
    for (int i=0; i<Range.size; i++)
    { Range[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight[i]);
      Domain[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft[i]);
    }
  outputAuto.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight);
}

bool SemisimpleLieAlgebra::IsInTheWeightSupport(Vector<Rational>& theWeight, Vector<Rational>& highestWeight, GlobalVariables& theGlobalVariables)
{ Vector<Rational> correspondingDominant= theWeight;
  this->theWeyl.RaiseToDominantWeight(correspondingDominant);
  Vector<Rational> theDiff= highestWeight - correspondingDominant;
  if (!theDiff.IsPositiveOrZero())
    return false;
  return true;
}

void SemisimpleLieAlgebra::CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& theHighestWeight, GlobalVariables& theGlobalVariables)
{ /*Vectors<Rational> weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport, theGlobalVariables);
  int highestWeight, distanceToHW;
  this->EmbeddingsRootSpaces.SetSize(this->theWeyl.RootSystem.size);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  List<bool> Explored;
  Explored.initFillInObject(this->theWeyl.RootSystem.size, false);
  int numExplored=0;
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
  { Vector<Rational>& current= this->theWeyl.RootSystem.TheObjects[i];
    if (current.SumCoordinates()==1 || current.SumCoordinates()==-1)
    { numExplored++;
      Explored.TheObjects[i]=true;
      Matrix<Rational> & currentMat= this->EmbeddingsRootSpaces.TheObjects[i];
      currentMat.init(weightSupport.size, weightSupport.size);
      currentMat.NullifyAll();
      for (int j=0; j<weightSupport.size; j++)
      { int indexTarget= weightSupport.GetIndex(current+weightSupport.TheObjects[j]);
        if (indexTarget!=-1)
        { highestWeight = -1+ this->GetLengthStringAlongAlphaThroughBeta(current, weightSupport.TheObjects[j], distanceToHW, weightSupport);
          if (current.IsNegativeOrZero())
            currentMat.elements[indexTarget][j]=1;
          else
            currentMat.elements[indexTarget][j]=(highestWeight-distanceToHW+1)*distanceToHW;
        }
      }
    }
  }
  Vectors<Rational> simpleBasis;
  simpleBasis.MakeEiBasis(theDimension);
  while (numExplored<this->theWeyl.RootSystem.size)
  { for (int i=0; i<this->theWeyl.RootSystem.size; i++)
      if (Explored.TheObjects[i])
        for (int j=0; j<this->theWeyl.RootSystem.size; j++)
          if (Explored.TheObjects[j])
          { Vector<Rational> tempRoot= this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j];
            if (this->theWeyl.IsARoot(tempRoot))
            { int index= this->theWeyl.RootSystem.GetIndex(tempRoot);
              if (!Explored.TheObjects[index])
              { Explored.TheObjects[index]=true;
                numExplored++;
                this->EmbeddingsRootSpaces.TheObjects[index]= this->EmbeddingsRootSpaces.TheObjects[i];
                this->EmbeddingsRootSpaces.TheObjects[index].LieBracketWith(this->EmbeddingsRootSpaces.TheObjects[j]);
              }
            }
          }
  }
  this->EmbeddingsCartan.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { Matrix<Rational> & current= this->EmbeddingsCartan.TheObjects[i];
    current.init(weightSupport.size, weightSupport.size);
    current.NullifyAll();
    Vector<Rational> tempRoot;
    tempRoot.MakeEi(theDimension, i);
    for (int j=0; j<weightSupport.size; j++)
      current.elements[j][j]=this->theWeyl.RootScalarCartanRoot(tempRoot, weightSupport.TheObjects[j]);
  }*/
}

int SemisimpleLieAlgebra::GetLengthStringAlongAlphaThroughBeta(Vector<Rational>& alpha, Vector<Rational>& beta, int& distanceToHighestWeight, Vectors<Rational>& weightSupport)
{ Vector<Rational> tempRoot=beta;
  for (int i=0; ; i++)
  { tempRoot+= alpha;
    if (!weightSupport.Contains(tempRoot))
    { distanceToHighestWeight=i;
      break;
    }
  }
  for (int i=0; ; i++)
  { tempRoot-= alpha;
    if (!weightSupport.Contains(tempRoot))
      return i;
  }
//  assert(false);
//  return -1;
}

void rootSubalgebra::GenerateAutomorphismsPreservingBorel(GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup& outputAutomorphisms)
{ this->ComputeAll();
  this->GenerateIsomorphismsPreservingBorel(*this, theGlobalVariables, &outputAutomorphisms, false);
}

int DebugCounter=-1;

void ParserNode::EvaluateInteger(GlobalVariables& theGlobalVariables)
{ if (!this->rationalValue.IsSmallInteger())
    this->ExpressionType=this->typeRational;
  else
  { this->ExpressionType= this->typeIntegerOrIndex;
    this->intValue= this->rationalValue.NumShort;
  }
}

int ParserNode::GetStrongestExpressionChildrenConvertChildrenIfNeeded(GlobalVariables& theGlobalVariables)
{ assert(false);
  return -1;
}

void ParserNode::ConvertChildrenAndMyselfToStrongestExpressionChildren(GlobalVariables& theGlobalVariables)
{ this->ExpressionType=this->GetStrongestExpressionChildrenConvertChildrenIfNeeded(theGlobalVariables);
}

void ParserNode::EvaluateMinus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
//  this->InitForAddition(&theGlobalVariables);
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex: if (i==0) this->intValue+=currentChild.intValue; else this->intValue-=currentChild.intValue; break;
      case ParserNode::typeRational:
        if (i==0)
          this->rationalValue+=currentChild.rationalValue;
        else
          this->rationalValue-=currentChild.rationalValue;
        break;
      case ParserNode::typePoly: if(i==0) this->polyValue.GetElement()+=(currentChild.polyValue.GetElement()); else this->polyValue.GetElement()-=(currentChild.polyValue.GetElement()); break;
      case ParserNode::typeRationalFunction: if(i==0) this->ratFunction.GetElement()+=(currentChild.ratFunction.GetElement()); else this->ratFunction.GetElement()-=(currentChild.ratFunction.GetElement()); break;
      case ParserNode::typeUEelement: if (i==0) this->UEElement.GetElement()+=currentChild.UEElement.GetElement(); else this->UEElement.GetElement()-=currentChild.UEElement.GetElement(); break;
      case ParserNode::typeUEElementOrdered: if (i==0) this->UEElementOrdered.GetElement()+=currentChild.UEElementOrdered.GetElement(); else this->UEElementOrdered.GetElement()-=currentChild.UEElementOrdered.GetElement(); break;
      case ParserNode::typeWeylAlgebraElement: if (i==0) this->WeylAlgebraElement.GetElement()+=currentChild.WeylAlgebraElement.GetElement(); else this->WeylAlgebraElement.GetElement()-=currentChild.WeylAlgebraElement.GetElement(); break;
      case ParserNode::typeCharSSFDMod:
        if (i==0)
          this->theChar.GetElement()+=currentChild.theChar.GetElement();
        else
          this->theChar.GetElement()-=currentChild.theChar.GetElement();
        break;
      case ParserNode::typeGenVermaElt:
        if (i==0)
          this->theGenVermaElt.GetElement()+=currentChild.theGenVermaElt.GetElement();
        else
          this->theGenVermaElt.GetElement()-=currentChild.theGenVermaElt.GetElement();
        break;
      default: this->ExpressionType=this->typeError; return;
    }
  }
}

void ParserNode::EvaluateMinusUnary(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
//  this->InitForAddition(&theGlobalVariables);
  ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[0]];
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: this->intValue-=currentChild.intValue; break;
    case ParserNode::typeRational: this->rationalValue-=currentChild.rationalValue; break;
    case ParserNode::typePoly: this->polyValue.GetElement()-=(currentChild.polyValue.GetElement()); break;
    case ParserNode::typeUEelement: this->UEElement.GetElement()-=currentChild.UEElement.GetElement(); break;
    case ParserNode::typeCharSSFDMod: this->theChar.GetElement()-=currentChild.theChar.GetElement(); break;
    case ParserNode::typeUEElementOrdered: this->UEElementOrdered.GetElement()-=currentChild.UEElementOrdered.GetElement(); break;
    case ParserNode::typeWeylAlgebraElement: this->WeylAlgebraElement.GetElement()-=currentChild.WeylAlgebraElement.GetElement(); break;
    case ParserNode::typeGenVermaElt: this->theGenVermaElt.GetElement()-=currentChild.theGenVermaElt.GetElement(); break;
    default: this->ExpressionType=this->typeError; return;
  }
}

void ParserNode::ReduceRatFunction()
{ if (this->ExpressionType!=this->typeRationalFunction)
    return;
  this->ratFunction.GetElement().ReduceMemory();
  switch (this->ratFunction.GetElement().expressionType)
  { case RationalFunctionOld::typeRational:
      this->rationalValue= this->ratFunction.GetElement().ratValue;
      this->ExpressionType=this->typeRational;
      break;
    case RationalFunctionOld::typePoly:
      this->polyValue.GetElement().operator=(this->ratFunction.GetElement().Numerator.GetElement());
      this->ExpressionType=this->typePoly;
      break;
  }
}

bool ParserNode::OneChildrenOrMoreAreOfType(int theType)
{ for (int i=0; i<this->children.size; i++)
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==theType)
      return true;
  return false;
}

bool ParserNode::AllChildrenAreOfDefinedNonErrorType()
{ for (int i=0; i<this->children.size; i++)
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeError || this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeUndefined)
      return false;
  return true;
}

void ParserNode::EvaluateGCDorLCM(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theRootNode= this->owner->TheObjects[this->children.TheObjects[0]];
  List<int>& theTrueChildren= theRootNode.children;
  if (theRootNode.ExpressionType!=this->typeArray)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  if (theTrueChildren.size!=2)
  { this->SetError(this->errorWrongNumberOfArguments);
    return;
  }
  this->ExpressionType=theRootNode.GetStrongestExpressionChildrenConvertChildrenIfNeeded(theGlobalVariables);
  if (this->ExpressionType==this->typeError)
    return;
  ParserNode& leftNode=this->owner->TheObjects[theTrueChildren.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[theTrueChildren.TheObjects[1]];
  LargeIntUnsigned tempUI1, tempUI2, tempUI3;
  LargeInt tempInt=0;
  Rational tempRat;
  int tempI2;
  int theFunction=this->intValue;
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
      if (leftNode.intValue==0 || rightNode.intValue==0)
      { this->SetError(this->errorDivisionByZero);
        return;
      }
      if (theFunction==Parser::functionGCD)
      { this->intValue= Rational::gcd(leftNode.intValue, rightNode.intValue);
        this->ExpressionType=this->typeIntegerOrIndex;
      } else
      { tempRat=leftNode.intValue;
        tempRat.MultiplyByInt(rightNode.intValue);
        tempI2=Rational::gcd(leftNode.intValue, rightNode.intValue);
        tempRat.DivideByInteger(tempI2);
        if (tempRat.IsSmallInteger())
        { this->intValue=tempRat.NumShort;
          this->ExpressionType=this->typeIntegerOrIndex;
        } else
        { this->rationalValue=tempRat;
          this->ExpressionType=this->typeRational;
        }
      }
      break;
    case ParserNode::typeRational:
      if (!leftNode.rationalValue.IsInteger() && !rightNode.rationalValue.IsInteger())
        this->SetError(this->errorDunnoHowToDoOperation);
      else
      { leftNode.rationalValue.GetNumerator(tempUI1);
        rightNode.rationalValue.GetNumerator(tempUI2);
        if (theFunction==Parser::functionGCD)
          LargeIntUnsigned::gcd(tempUI1, tempUI2, tempUI3);
        else
          LargeIntUnsigned::lcm(tempUI1, tempUI2, tempUI3);
        tempInt.AddLargeIntUnsigned(tempUI3);
        this->rationalValue.AssignLargeInteger(tempInt);
        this->ExpressionType=this->typeRational;
      }
      break;
    case ParserNode::typePoly:
      if (leftNode.polyValue.GetElement().IsEqualToZero() || rightNode.polyValue.GetElement().IsEqualToZero())
      { this->SetError(this->errorDivisionByZero);
        return;
      }
      if (theFunction==Parser::functionGCD)
        RationalFunctionOld::gcd(leftNode.polyValue.GetElement(), rightNode.polyValue.GetElement(), this->polyValue.GetElement(), &theGlobalVariables);
      else
        RationalFunctionOld::lcm(leftNode.polyValue.GetElement(), rightNode.polyValue.GetElement(), this->polyValue.GetElement());
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->SetError(errorDunnoHowToDoOperation);
    break;
    default: this->SetError(this->errorDunnoHowToDoOperation); return;
  }
}

ParserNode::ParserNode()
{
}

void Parser::ToString(bool includeLastNode, std::string& output, bool useHtml, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ std::stringstream out; std::string tempS;
  std::string htmlSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(this->StringBeingParsed, htmlSafish))
    out << "&lt; -modified string: ";
  else
    out << "String: ";
  out << htmlSafish << "\n";
  if (useHtml)
    out << "<br>";
  out << "Tokens: ";
  for (int i=0; i<this->TokenBuffer.size; i++)
    this->TokenToStringStream(out, this->TokenBuffer.TheObjects[i]);
  out << "\nToken stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->TokenStack.size; i++)
    this->TokenToStringStream(out, this->TokenStack.TheObjects[i]);
  if (useHtml)
    out << "<br>";
  out << "\nValue stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->ValueStack.size; i++)
    out << this->ValueStack.TheObjects[i] << ", ";
  if (useHtml)
    out << "<br>";
  out << "\nElements:\n";
  if (useHtml)
    out << "<br>";
  int NumDisplayedNodes=this->size;
  if(!includeLastNode)
    NumDisplayedNodes--;
  for (int i=0; i<NumDisplayedNodes; i++)
  { this->TheObjects[i].ToString(tempS, theGlobalVariables, theFormat);
    out << " Index: " << i << " " << tempS << ";\n";
    if (useHtml)
      out << "<br>";
  }
  if (useHtml)
    out << "<br><br>";
  if (includeLastNode)
    out << "\n\nValue: " << this->theValue.ElementToStringValueAndType(false, theGlobalVariables, false, theFormat);

//  this->WeylAlgebraValue.ComputeDebugString(false, false);
//  this->LieAlgebraValue.ComputeDebugString(this->theLieAlgebra, false, false);
//  out << "\n\nWeyl algebra value: " << this->WeylAlgebraValue.DebugString;
//  out << "\nLie algebra value: " << this->LieAlgebraValue.DebugString;
  output=out.str();
}

void Parser::TokenToStringStream(std::stringstream& out, int theToken)
{ //out << theToken << " ";
  switch(theToken)
  { case Parser::tokenX: out << "x"; break;
    case Parser::tokenDigit: out << "D"; break;
    case Parser::tokenPlus: out << "+"; break;
    case Parser::tokenUnderscore: out << "_"; break;
    case Parser::tokenEmpty: out << " "; break;
    case Parser::tokenExpression: out << "E"; break;
    case Parser::tokenOpenLieBracket: out << "["; break;
    case Parser::tokenCloseLieBracket: out << "]"; break;
    case Parser::tokenLieBracket: out << "[, ]"; break;
    case Parser::tokenComma: out << ", "; break;
    case Parser::tokenOpenCurlyBracket: out << "{"; break;
    case Parser::tokenCloseCurlyBracket: out << "}"; break;
    case Parser::tokenOpenBracket: out << "("; break;
    case Parser::tokenCloseBracket: out << ")"; break;
    case Parser::tokenPartialDerivative: out << "d"; break;
    case Parser::tokenTimes: out << "*"; break;
    case Parser::tokenMinus: out << "-"; break;
    case Parser::tokenG : out << "g"; break;
    case Parser::tokenInteger: out << "Integer"; break;
    case Parser::tokenDivide: out << "/"; break;
    case Parser::tokenH: out << "h"; break;
    case Parser::tokenPower: out << "^"; break;
    case Parser::tokenC: out << "c"; break;
    case Parser::tokenEmbedding: out << "i"; break;
    case Parser::tokenMinusUnary: out << "-"; break;
    case Parser::tokenVariable: out << "variable"; break;
    case Parser::tokenArraY: out << "array"; break;
    case Parser::tokenDereferenceArray: out << "deref[]"; break;
    case Parser::tokenMapsTo: out << "->"; break;
    case Parser::tokenFunction: out << "function"; break;
    case Parser::tokenEndStatement: out << ";"; break;
    case Parser::tokenColon: out << ":"; break;
//    case Parser::tokenSubstitution: out << "Substitution: "; break;
    case Parser::tokenFunctionNoArgument: out << "functionNoArgument"; break;
    case Parser::tokenF: out << "f"; break;
    default: out << "?"; break;
  }
}

std::string ParserNode::ElementToStringErrorCode(bool useHtml)
{ std::stringstream out;
  switch (this->ErrorType)
  { case ParserNode::errorBadIndex: out << "error: bad index"; break;
    case ParserNode::errorDimensionProblem: out << "error with dimension and/or number of arguments"; break;
    case ParserNode::errorBadOrNoArgument: out << "error: bad or no argument"; break;
    case ParserNode::errorBadSyntax: out << "error: bad syntax."; break;
    case ParserNode::errorDunnoHowToDoOperation: out << "error: my master hasn't taught me how to do this operation (maybe he doesn't know how either)"; break;
    case ParserNode::errorDivisionByZero: out << "error: division by zero"; break;
    case ParserNode::errorDivisionByNonAllowedType: out << "error: division of/by non-allowed type"; break;
    case ParserNode::errorMultiplicationByNonAllowedTypes: out << "error: multiplication by non-allowed types"; break;
    case ParserNode::errorNoError: out << "error: error type claims no error, but expression type claims error. Slap the programmer."; break;
    case ParserNode::errorOperationByUndefinedOrErrorType: out << "error: operation with an undefined type"; break;
    case ParserNode::errorWrongNumberOfArguments: out << "error: wrong number of arguments."; break;
    case ParserNode::errorProgramming: out << "error: there has been some programming mistake (it's not your expression's fault). Slap the programmer!"; break;
    case ParserNode::errorUnknownOperation: out << "error: unknown operation. The lazy programmer has added the operation to the dictionary, but hasn't implemented it yet. Lazy programmers deserve no salary. "; break;
    case ParserNode::errorImplicitRequirementNotSatisfied: out << "Error: an implicit requirement for the funciton input has not been satisfied."; break;
    case ParserNode::errorBadFileFormat: out << "Bad input file format. "; break;
    case ParserNode::errorConversionError: out << "Error with the conversion routines. In some cases this may be a programming error."; break;
    default: out << "Non-documented error number " << this->ErrorType << ". Lazy programmers deserve no salaries."; break;
  }
  return out.str();
}

void ParserNode::ToString(std::string& output, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ std::stringstream out; std::string tempS;
  this->owner->TokenToStringStream(out, this->Operation);
  if (this->Operation==Parser::tokenFunction)
  { out << " " << this->owner->theFunctionList[this->intValue].functionName;

  }
  out << "; #implied vars: " << this->impliedNumVars;
  if (this->children.size>0)
  { out << ". Children: ";
    for (int i=0; i<this->children.size; i++)
      out << this->children.TheObjects[i] << ", ";
  }
  out << this->ElementToStringValueAndType(false, theGlobalVariables, false, theFormat);
  output=out.str();
}

bool HomomorphismSemisimpleLieAlgebra::ComputeHomomorphismFromImagesSimpleChevalleyGenerators
(GlobalVariables& theGlobalVariables)
{ this->theDomain().ComputeChevalleyConstantS(&theGlobalVariables);
  this->theRange().ComputeChevalleyConstantS(&theGlobalVariables);
  int theDomainDimension= this->theDomain().theWeyl.CartanSymmetric.NumRows;
  Selection NonExplored;
  int numRoots= this->theDomain().theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;
  List<ElementSemisimpleLieAlgebra<Rational> > tempDomain, tempRange;
  tempDomain.SetSize(numRoots+theDomainDimension);
  tempRange.SetSize(numRoots+theDomainDimension);
  Vector<Rational> tempRoot;
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    for (int j=0; j<2; j++, tempRoot.Minus())
    { int index= this->theDomain().theWeyl.RootSystem.GetIndex(tempRoot);
      tempDomain[index].MakeZero(this->theDomain());
      ChevalleyGenerator tempGen;
      tempGen.MakeGenerator(this->theDomain(), this->theDomain().GetGeneratorFromRoot(tempRoot));
      tempDomain[index].AddMonomial(tempGen, 1);
      tempRange[index] = this->imagesSimpleChevalleyGenerators[i+j*theDomainDimension];
      NonExplored.RemoveSelection(index);
//      std::cout <<"<br>" << tempDomain.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//      std::cout <<"->" << tempRange.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
    }
  }
//  std::cout << this->ToString(theGlobalVariables) << "<br>";
  Vector<Rational> right;
  while (NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      Vector<Rational>& current = this->theDomain().theWeyl.RootSystem[theIndex];
      for (int j=0; j<NonExplored.MaxSize; j++)
        if (!NonExplored.selected[j])
        { Vector<Rational>& left= this->theDomain().theWeyl.RootSystem[j];
          right= current-left;
//          left.ComputeDebugString(); right.ComputeDebugString(); current.ComputeDebugString();
          if (this->theDomain().theWeyl.IsARoot(right))
          { int leftIndex= this->theDomain().theWeyl.RootSystem.GetIndex(left);
            int rightIndex= this->theDomain().theWeyl.RootSystem.GetIndex(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSemisimpleLieAlgebra<Rational>& leftDomainElt=tempDomain[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightDomainElt= tempDomain[rightIndex];
              this->theDomain().LieBracket(leftDomainElt, rightDomainElt, tempDomain[theIndex]);
              ElementSemisimpleLieAlgebra<Rational>& leftRangeElt=tempRange[leftIndex];
              ElementSemisimpleLieAlgebra<Rational>& rightRangeElt= tempRange[rightIndex];
              this->theRange().LieBracket(leftRangeElt, rightRangeElt, tempRange[theIndex]);
              NonExplored.RemoveSelection(theIndex);
              break;
            }
          }
        }
    }
  }
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    int leftIndex= this->theDomain().theWeyl.RootSystem.GetIndex(tempRoot);
    int rightIndex= this->theDomain().theWeyl.RootSystem.GetIndex(-tempRoot);
    this->theDomain().LieBracket(tempDomain.TheObjects[leftIndex], tempDomain.TheObjects[rightIndex], tempDomain.TheObjects[numRoots+i]);
    this->theRange().LieBracket(tempRange.TheObjects[leftIndex], tempRange.TheObjects[rightIndex], tempRange.TheObjects[numRoots+i]);
  }
  Vectors<Rational> vectorsLeft, vectorsRight;
  vectorsLeft.SetSize(tempDomain.size);
  vectorsRight.SetSize(tempDomain.size);
  for (int i=0; i<tempRange.size; i++)
  { tempDomain[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft.TheObjects[i]);
    tempRange[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight.TheObjects[i]);
  }
  Matrix<Rational> tempMat;
  tempMat.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight);
  Vector<Rational> imageRoot;
  this->domainAllChevalleyGenerators.SetSize(tempDomain.size);
  this->imagesAllChevalleyGenerators.SetSize(tempDomain.size);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i=0; i<this->theDomain().GetNumGenerators(); i++)
    this->domainAllChevalleyGenerators[i].MakeGenerator(i, this->theDomain());
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
  { this->domainAllChevalleyGenerators[i].ElementToVectorNegativeRootSpacesFirst(tempRoot);
    tempMat.ActOnVectorColumn(tempRoot, imageRoot);
    this->imagesAllChevalleyGenerators[i].AssignVectorNegRootSpacesCartanPosRootSpaces
    (imageRoot, this->theRange());
//    std::cout << this->domainAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//    std::cout << "->" << this->imagesAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
//    std::cout << "<br>";
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    this->ProjectOntoSmallCartan(input.TheObjects[i], output.TheObjects[i], theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan
(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  invertedSmallCartan;
  invertedSmallCartan=this->theDomain().theWeyl.CartanSymmetric;
  invertedSmallCartan.Invert(theGlobalVariables);
  int theSmallDimension=this->theDomain().theWeyl.CartanSymmetric.NumRows;
  output.MakeZero(theSmallDimension);
  for (int i=0; i<theSmallDimension; i++)
    output.TheObjects[i]= this->theRange().theWeyl.RootScalarCartanRoot
    (this->imagesAllChevalleyGenerators.TheObjects[this->theDomain().theWeyl.RootsOfBorel.size+i].GetCartanPart(), input);
  invertedSmallCartan.ActOnVectorColumn(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
(MonomialUniversalEnveloping<RationalFunctionOld>& input, const RationalFunctionOld& theCoeff,
 ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<RationalFunctionOld> tempElt;
  output.MakeZero(this->theRange());
  RationalFunctionOld polyOne;
  polyOne=theCoeff.GetOne();
  output.MakeConst(theCoeff, this->theRange());
  for (int i=0; i<input.generatorsIndices.size; i++)
  { if (input.generatorsIndices[i]>=this->imagesAllChevalleyGenerators.size)
      return false;
    tempElt.AssignElementLieAlgebra
    (this->imagesAllChevalleyGenerators[input.generatorsIndices[i]], this->theRange(), polyOne, polyOne.GetZero());
    RationalFunctionOld& thePower=input.Powers[i];
    int theIntegralPower;
    if (!thePower.IsSmallInteger(&theIntegralPower))
      return false;
    for (int j=0; j<theIntegralPower; j++)
      output*=tempElt;
  }
  return true;
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
(ElementUniversalEnveloping<RationalFunctionOld>& input,
 ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)
{ assert(&output!=&input);
  output.MakeZero(this->theRange());
  ElementUniversalEnveloping<RationalFunctionOld> tempElt;
  for (int i=0; i<input.size; i++)
  { if(!this->ApplyHomomorphism(input[i], input.theCoeffs[i], tempElt, theGlobalVariables))
      return false;
    output+=tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::MakeGinGWithId
(char theWeylLetter, int theWeylDim, ListReferences<SemisimpleLieAlgebra>& ownerOfAlgebras, GlobalVariables& theGlobalVariables)
{ SemisimpleLieAlgebra tempSS;
  tempSS.theWeyl.MakeArbitrarySimple(theWeylLetter, theWeylDim);
  int theIndex=ownerOfAlgebras.AddNoRepetitionOrReturnIndexFirst(tempSS);
  this->domainAlg=&ownerOfAlgebras[theIndex];
  this->rangeAlg=&ownerOfAlgebras[theIndex];
  this->theDomain().ComputeChevalleyConstantS(&theGlobalVariables);
  int numPosRoots=this->theDomain().theWeyl.RootsOfBorel.size;
  this->imagesAllChevalleyGenerators.SetSize(numPosRoots*2+theWeylDim);
  this->domainAllChevalleyGenerators.SetSize(numPosRoots*2+theWeylDim);
  this->imagesSimpleChevalleyGenerators.SetSize(theWeylDim*2);
  for (int i=0; i<2*numPosRoots+theWeylDim; i++)
  { ElementSemisimpleLieAlgebra<Rational>& tempElt1=this->imagesAllChevalleyGenerators[i];
    ElementSemisimpleLieAlgebra<Rational>& tempElt2=this->domainAllChevalleyGenerators[i];
    tempElt2.MakeGenerator(i, this->theDomain());
    tempElt1.MakeGenerator(i, this->theRange());
  }
  for (int i=0; i<theWeylDim; i++)
  { ElementSemisimpleLieAlgebra<Rational>& tempElt1=this->imagesSimpleChevalleyGenerators[i];
    tempElt1.MakeGenerator(i, this->theRange());
    ElementSemisimpleLieAlgebra<Rational>& tempElt2=this->imagesSimpleChevalleyGenerators[theWeylDim+i];
    tempElt2.MakeGenerator(i+numPosRoots, this->theRange());
  }
//  std::cout << this->ToString(theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::ToString
(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS, tempS2;
  if (this->CheckClosednessLieBracket(theGlobalVariables))
    out << "Lie bracket closes, everything is good!";
  else
    out << "The Lie bracket is BAD BAD BAD!";
  if (useHtml)
    out << "<br>";
  out << "Images simple Chevalley generators:\n\n";
  if (useHtml)
    out << "<br>";
  for (int i=0; i<this->imagesSimpleChevalleyGenerators.size; i++)
  { out <<  this->imagesSimpleChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat) << "\n\n";
    if (useHtml)
      out << "<br>";
  }
  out << "Maps of Chevalley generators:\n\n";
  for (int i=0; i<this->domainAllChevalleyGenerators.size; i++)
  { out << "<br>"
    << this->domainAllChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat)
    << " \\mapsto "
    << this->imagesAllChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat)
    ;
  }
  output=out.str();
}

void ParserNode::EvaluateFunction(GlobalVariables& theGlobalVariables)
{ if (this->intValue<this->owner->theFunctionList.size && this->intValue>=0)
    this->owner->theFunctionList[this->intValue].CallMe(*this, theGlobalVariables);
  else
    this->SetError(this->errorUnknownOperation);
}

void ParserNode::EvaluateOrder(GlobalVariables& theGlobalVariables)
{ assert(false);
  this->ExpressionType=this->typeUEElementOrdered;
}

void ParserNode::EvaluatePrintWeyl(GlobalVariables& theGlobalVariables)
{ assert(false);
}

class slTwoInSlN;

void ParserNode::EvaluateOuterAutos(GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  tempMat;
  std::stringstream out;
  this->ExpressionType=this->typeString;
  this->owner->theHmm.theRange().ComputeOneAutomorphism(theGlobalVariables, tempMat, true);
  Matrix<Rational>  tempMat2=tempMat;
  Rational tempRat;
  tempMat2.ComputeDeterminantOverwriteMatrix(tempRat);
  out << "<br>one outer automorphism of the Lie algebra is realized as the following matrix. <br> The coordinates of the matrix are given in the ordered basis ";
  out << " <div class=\"math\" scale=\"50\"> g_{-n}, \\dots, g_{-1}, h_1,\\dots, h_k, g_1,\\dots, g_n</div> where the generators are as in the table on the right.<br> Its determinant is \n" << tempRat.ToString();
  out << "<div class=\"math\" scale=\"50\">" << tempMat.ToString() << "</div>";
  this->outputString=out.str();
}

void HomomorphismSemisimpleLieAlgebra::GetRestrictionAmbientRootSystemToTheSmallerCartanSA
(Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ List<Vector<Rational> >& theRootSystem= this->theRange().theWeyl.RootSystem;
  int rankSA=this->theDomain().theWeyl.GetDim();
  Matrix<Rational>  tempMat;
  tempMat=(this->theDomain().theWeyl.CartanSymmetric);
  tempMat.Invert(theGlobalVariables);
  int numPosRootsDomain=this->theDomain().theWeyl.RootsOfBorel.size;
  output.SetSize(theRootSystem.size);
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(rankSA);
//  tempMat.ComputeDebugString(true, false);
//  std::cout << tempMat.DebugString << "<br>";
  for (int i=0; i<theRootSystem.size; i++)
  { for (int j=0; j<rankSA; j++)
    { ElementSemisimpleLieAlgebra<Rational>& currentH=this->imagesAllChevalleyGenerators[j+numPosRootsDomain];
      theScalarProducts[j]=this->theRange().theWeyl.RootScalarCartanRoot
      (currentH.GetCartanPart(), theRootSystem[i]);
    }
    tempMat.ActOnVectorColumn(theScalarProducts, output[i]);
  }
  this->ImagesCartanDomain.SetSize(rankSA);
  for (int i=0; i<rankSA; i++)
    this->ImagesCartanDomain[i]=this->imagesAllChevalleyGenerators[i+numPosRootsDomain].GetCartanPart();
}

bool HomomorphismSemisimpleLieAlgebra::CheckClosednessLieBracket(GlobalVariables& theGlobalVariables)
{ ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vectors<Rational> tempRoots; Vector<Rational> tempRoot;
  tempRoots.SetSize(this->imagesAllChevalleyGenerators.size);
  for (int i=0; i<tempRoots.size; i++)
    this->imagesAllChevalleyGenerators[i].ElementToVectorNegativeRootSpacesFirst(tempRoots[i]);
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
    for (int j=0; j<this->imagesAllChevalleyGenerators.size; j++)
    { this->theRange().LieBracket
      (this->imagesAllChevalleyGenerators[i], this->imagesAllChevalleyGenerators[j], tempElt);
      tempElt.ElementToVectorNegativeRootSpacesFirst(tempRoot);
      if(!tempRoots.LinSpanContainsRoot(tempRoot))
        return false;
    }
  return true;
}

int ParserNode::GetMaxImpliedNumVarsChildren()
{ int result=0;
  for (int i=0; i<this->children.size; i++)
    result=MathRoutines::Maximum(result, this->owner->TheObjects[this->children[i]].impliedNumVars);
  return result;
}

void ChevalleyGenerator::CheckConsistencyWithOther(const ChevalleyGenerator& other)const
{ if (this->owneR!=other.owneR)
  { std::cout
    << "This is a programming error: attempting to compare Chevalley generators "
    << "of different Lie algebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__) << ".";
    assert(false);
  }
}

std::string ChevalleyGenerator::ToString(FormatExpressions* inputFormat)const
{ return this->owneR->GetStringFromChevalleyGenerator(this->theGeneratorIndex, inputFormat);
}

bool ChevalleyGenerator::operator>(const ChevalleyGenerator& other)const
{ return this->theGeneratorIndex>other.theGeneratorIndex;
}

std::string SemisimpleLieAlgebra::GetStringFromChevalleyGenerator
(int theIndex, FormatExpressions* thePolynomialFormat)const
{ std::stringstream out;
  MemorySaving<FormatExpressions> tempFormat;
  if (thePolynomialFormat==0)
    thePolynomialFormat=&tempFormat.GetElement();
  if (this->IsGeneratorFromCartan(theIndex))
    out << thePolynomialFormat->chevalleyHgeneratorLetter << "_{" << theIndex-this->GetNumPosRoots()+1 << "}";
  else
  { out << thePolynomialFormat->chevalleyGgeneratorLetter << "_{";
    if (theIndex>=this->GetNumPosRoots())
      out << theIndex-this->GetNumPosRoots()-this->GetRank()+1;
    else
      out << theIndex-this->GetNumPosRoots();
    out << "}";
  }
  return out.str();
}

bool SemisimpleLieAlgebra::AreOrderedProperly(int leftIndex, int rightIndex)
{ return this->UEGeneratorOrderIncludingCartanElts[leftIndex]<=
  this->UEGeneratorOrderIncludingCartanElts[rightIndex];
}

int SemisimpleLieAlgebra::GetRootIndexFromDisplayIndex(int theIndex)
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex<0)
    return theIndex+numPosRoots;
  if (theIndex>0)
    return theIndex+numPosRoots-1;
  return -10000000;
}

int SemisimpleLieAlgebra::GetDisplayIndexFromRootIndex(int theIndex)const
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex>=numPosRoots)
    return theIndex-numPosRoots+1;
  if (theIndex<numPosRoots)
    return theIndex-numPosRoots;
  return -10000000;
}

int SemisimpleLieAlgebra::GetGeneratorFromRootIndex(int theIndex)const
{ if (theIndex<0  || theIndex>=this->theWeyl.RootSystem.size)
    return -1;
  int theDimension=this->theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex>=numPosRoots)
    return theIndex+theDimension;
  return theIndex;
}

int SemisimpleLieAlgebra::GetRootIndexFromGenerator(int theIndex)const
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theDimension=this->theWeyl.CartanSymmetric.NumRows;
  if (theIndex<numPosRoots)
    return theIndex;
  if (theIndex>=numPosRoots+theDimension)
    return theIndex-theDimension;
  return -1;
}

void CGI::MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank)
{ if (theWeylLetter=='a') theWeylLetter='A';
  if (theWeylLetter=='b') theWeylLetter='B';
  if (theWeylLetter=='c') theWeylLetter='C';
  if (theWeylLetter=='d') theWeylLetter='D';
  if (theWeylLetter=='e') theWeylLetter='E';
  if (theWeylLetter=='f') theWeylLetter='F';
  if (theWeylLetter=='g') theWeylLetter='G';
  if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C' || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F' || theWeylLetter=='G'))
      theWeylLetter='A';
  if (theRank>8 || theRank<1)
    theRank=1;
  if (theWeylLetter!='A' && theRank==1)
    theRank=2;
  if (theWeylLetter=='E' && theRank<6)
    theRank=6;
  if (theWeylLetter=='F')
    theRank=4;
  if (theWeylLetter=='G')
    theRank=2;
  if (theWeylLetter=='D' && theRank <4)
    theRank=4;
}

void CGI::ReplaceEqualitiesAndAmpersantsBySpaces(std::string& inputOutput)
{ for (int i=0; i<(signed)inputOutput.size(); i++)
    if (inputOutput[i]=='=' || inputOutput[i]=='&')
      inputOutput[i]=' ';
}

void VectorPartition::ComputeAllPartitions()
{ List<int> currentPartition;
  currentPartition.initFillInObject(this->PartitioningRoots.size, 0);
  this->thePartitions.size=0;
  this->ComputeAllPartitionsRecursive(0, currentPartition, -1, theRoot);
}

void VectorPartition::ComputeAllPartitionsRecursive(int currentIndex, List<int>& CurrentPartition, int UpperBoundEachIndex, Vector<Rational>& toBePartitioned)
{ if (currentIndex>=this->PartitioningRoots.size)
    return;
  Vector<Rational> currentRoot=toBePartitioned;
  while (currentRoot.IsPositiveOrZero() && (CurrentPartition.TheObjects[currentIndex]<=UpperBoundEachIndex || UpperBoundEachIndex==-1))
  { if (currentRoot.IsEqualToZero())
      this->thePartitions.AddOnTop(CurrentPartition);
    else
    { this->ComputeAllPartitionsRecursive(currentIndex+1, CurrentPartition , UpperBoundEachIndex, currentRoot);
      for (int i=currentIndex+1; i<CurrentPartition.size; i++)
        CurrentPartition[i]=0;
    }
    currentRoot-=(this->PartitioningRoots[currentIndex]);
    CurrentPartition[currentIndex]++;
  }
}

std::string VectorPartition::ToString(bool useHtml)
{ std::stringstream out;
  out << this->theRoot.ToString() << "\n\n";
  if (useHtml)
    out << "<br>";
  for (int i=0; i<this->thePartitions.size; i++)
  { bool found=false;
    out << "=";
    for (int j=0; j<this->thePartitions.TheObjects[i].size; j++)
    { int theCoefficient=thePartitions.TheObjects[i].TheObjects[j];
      if (theCoefficient!=0)
      { if(found)
          out << "+";
        found=true;
        if (theCoefficient>1)
          out << theCoefficient;
        out << this->PartitioningRoots.TheObjects[j].ToString();
      }
    }
    out << "\n\n";
    if (useHtml)
      out << "<br>\n";
  }
  return out.str();
}

bool RationalFunctionOld::ConvertToType(int theType)
{ if (theType<this->expressionType)
    return false;
  if (theType==this->expressionType)
    return true;
  if (this->expressionType==this->typeRational && this->expressionType<theType)
  { this->expressionType=this->typePoly;
    this->Numerator.GetElement().MakeConsT(this->ratValue);
  }
  if (this->expressionType==this->typePoly && this->expressionType<theType)
  { this->expressionType=this->typeRationalFunction;
    this->Denominator.GetElement().MakeConsT(1);
  }
  return true;
}

void RationalFunctionOld::Invert()
{ //std::cout << "inverting " << this->ToString();
  assert(this->checkConsistency());
  if (this->expressionType==this->typeRational)
  { if (this->ratValue.IsEqualToZero())
    { std::cout << "This is a programming error: division by zero. Division by zero errors "
      << "must be caught earlier in the program and handled gracefully. Crashing ungracefully. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->ratValue.Invert();
    return;
  }
  if (this->expressionType==this->typePoly)
    this->ConvertToType(this->typeRationalFunction);
  assert(!this->Numerator.GetElement().IsEqualToZero());
  MathRoutines::swap(this->Numerator.GetElement(), this->Denominator.GetElement());
  this->expressionType=this->typeRationalFunction;
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  assert(this->checkConsistency());
  //std::cout << " to get: " << this->ToString();
}

bool RationalFunctionOld::checkConsistency()const
{ if (this->expressionType==this->typePoly)
  { if (this->Numerator.IsZeroPointer())
    { std::cout << "This is a programming error: "
      << "  a rational function is flagged as being a non-constant polynomial, but the numerator pointer is zero. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
    if (this->Numerator.GetElementConst().IsAConstant())
    { std::cout << "This is a programming error: "
      << " a rational funtion is flagged as having a non-constant numerator, but the numerator is constant. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
//      int commentMeOutWhenDoneDebugging=-1;
//      this->Numerator.GetElementConst().GrandMasterConsistencyCheck();
  }
  if (this->expressionType==this->typeRationalFunction)
  { if (this->Numerator.IsZeroPointer() || this->Denominator.IsZeroPointer())
    { std::cout << "This is a programming error: "
      << "  a rational function is flagged as having non-constant denominator, but either the numerator or the denominator pointer is zero. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
    if (this->Denominator.GetElementConst().IsAConstant())
    { std::cout << "This is a programming error: "
      << "  a rational function is flagged as having non-constant denominator, but the denominator is constant. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
  }
  if (this->expressionType!=this->typeRational &&
      this->expressionType!=this->typePoly &&
      this->expressionType!=this->typeRationalFunction)
  { std::cout << "This is a programming error: "
    << "  a rational function is not initialized properly: its type is " << this->expressionType
    << " which is not allowed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    return false;
  }
  return true;
}

std::string RationalFunctionOld::ToString(FormatExpressions* theFormat)const
{ //out << "( Number variables: " << this->NumVars << ", hash: " << this->HashFunction() << ")";
  if (this->expressionType==this->typeRational)
  { //out << "(type: rational)";
    return this->ratValue.ToString();
  }
  if (this->expressionType==this->typePoly)
    return this->Numerator.GetElementConst().ToString(theFormat);
  std::stringstream out;
  bool needBracket=false;
  if (this->Numerator.GetElementConst().NeedsBrackets())
  { needBracket=true;
    out << "(";
  }
  out << this->Numerator.GetElementConst().ToString(theFormat);
  if (needBracket)
    out << ")";
  out << "/(" << this->Denominator.GetElementConst().ToString(theFormat) << ")";
//  out << " Num vars: " << this->GetNumVars();
  return out.str();
}

void GroebnerBasisComputation::RemainderDivisionWithRespectToBasis
(Polynomial<Rational>& inputOutput,
 Polynomial<Rational>* outputRemainder, GlobalVariables* theGlobalVariables,
 int basisIndexToIgnore
 )
{ //Reference: Cox, Little, O'Shea, Ideals, Varieties and Algorithms, page 62
  MacroRegisterFunctionWithName("GroebnerBasisComputation::RemainderDivisionWithRespectToBasis");
  if (&inputOutput==outputRemainder ||
      &inputOutput==&this->bufPoly || outputRemainder==&this->bufPoly)
  { std::cout
    << "This is a programming error: the input, the output  "
    << " and the buffer member object must be pairwise distinct when carrying out "
    << " multi-polynomial division. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  MemorySaving<Polynomial<Rational> > tempPoly;
  if (outputRemainder==0)
    outputRemainder=&tempPoly.GetElement();
  ProgressReport theReportStart(theGlobalVariables);
  ProgressReport theReport(theGlobalVariables);
  if (theGlobalVariables!=0 && this->flagDoProgressReport)
  { theReportStart.Report
    ("Computing remainder  division");
    //+     inputOutput.ToString(&theGlobalVariables->theDefaultFormat) + " mod " +theBasiS.ToString(&theGlobalVariables->theDefaultFormat));
  }
  outputRemainder->MakeZero();
  Polynomial<Rational>& currentRemainder=inputOutput;
  Rational leadingMonCoeff;
  MonomialP& highestMonCurrentDivHighestMonOther=this->bufferMoN1;
  int numIntermediateRemainders=0;
  while (!currentRemainder.IsEqualToZero())
  { bool divisionOcurred=false;
    int i=0;
    int indexLeadingMonRemainder= currentRemainder.GetIndexMaxMonomial(this->theMonOrdeR);
    leadingMonCoeff=currentRemainder.theCoeffs[indexLeadingMonRemainder];
    highestMonCurrentDivHighestMonOther=currentRemainder[indexLeadingMonRemainder];
    while (i<this->theBasiS.size && !divisionOcurred)
    { MonomialP& highestMonBasis=this->leadingMons[i];
      bool shouldDivide=(i==basisIndexToIgnore)
      ? false :highestMonCurrentDivHighestMonOther.IsDivisibleBy(highestMonBasis);
      if (shouldDivide)
      { numIntermediateRemainders++;
        highestMonCurrentDivHighestMonOther/=highestMonBasis;
        if (!highestMonCurrentDivHighestMonOther.HasPositiveOrZeroExponents())
        { std::cout << "This is a programming error: the pivot monomial in the polynomial "
          << " division algorithm has negative exponent(s). This is not allowed. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
        this->bufPoly=this->theBasiS[i];
        leadingMonCoeff/=this->leadingCoeffs[i];
        this->bufPoly.MultiplyBy(highestMonCurrentDivHighestMonOther, leadingMonCoeff);
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out
          << "Total number of polynomial operations so far: " << this->NumberOfComputations;
          if (this->MaxNumComputations>0)
            out << ", with a limit of no more than " << this->MaxNumComputations << " operations.";
          out
          << "\n<br>Number of intermediate remainders: " << numIntermediateRemainders
          << "\n<br> Highest mon of current remainder: "
          << currentRemainder[indexLeadingMonRemainder].ToString() << ". "
          << "\n<br>Current index we are dividing by: " << i+1
          << " out of " << this->theBasiS.size
          << "\n<br>" << currentRemainder.size << " monomials in current remainder."
          << "\n<br>" << outputRemainder->size << " monomials in output remainder."
          ;
          theReport.Report(out.str());
          //std::cout << out.str();
        }
/*        if (this->NumberOfComputations>this->MaxNumComputations+1000)
        { std::cout << "<br>Dividing "
          << currentRemainder.ToString()
          <<  " by " << theBasiS[i].ToString() << "<br>i.e. subtracting "
          << this->bufPoly.ToString() ;
          Polynomial<Rational> currentRemainder1;
          currentRemainder1=currentRemainder;
          currentRemainder1-=this->bufPoly;
          std::cout << " I must get: " << currentRemainder1.ToString();
        }*/
        currentRemainder-=this->bufPoly;
        divisionOcurred=true;
/*        if (this->NumberOfComputations>this->MaxNumComputations+1000)
        { std::cout << "<br>Result:<br> " << currentRemainder.ToString()
          << "<br>Current divisor index: " << i+1;
          if(this->NumberOfComputations>this->MaxNumComputations+1010)
          { std::cout
            << "<br>This may or may not be a programming error. While handling computation excess limit, "
            << " I got that NumberOfComputations is much larger than MaxNumComputations. "
            << " I have no explanation for this issue right now, so I am crashing to let you know "
            << " something is fishy. ";
            std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
            assert(false);
          }
        }*/
        this->NumberOfComputations++;
        //std::cout << " to get " << currentRemainder.ToString(&theGlobalVariables->theDefaultFormat);
      } else
        i++;
    }
    if (!divisionOcurred)
    { outputRemainder->AddMonomial(highestMonCurrentDivHighestMonOther, leadingMonCoeff);
      currentRemainder.PopMonomial(indexLeadingMonRemainder);
      this->NumberOfComputations++;
      /*if (this->NumberOfComputations>this->MaxNumComputations+1000)
      { std::cout
        << "This may or may not be a programming error. While handling computation excess limit, "
        << " I got that NumberOfComputations is much larger than MaxNumComputations. "
        << " I have no explanation for this issue right now, so I am crashing to let you know "
        << " something is fishy. ";
        std::cout << "<br>Current remainder:<br> " << currentRemainder.ToString();

        std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }*/
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Number of intermediate remainders: " << numIntermediateRemainders
        << "\n<br> Highest mon of current remainder is no longer reducible. "
        << "\n<br>" << currentRemainder.size << " monomials in current remainder."
        << "\n<br>" << outputRemainder->size << " monomials in output remainder."
        ;
        theReport.Report(out.str());
          //std::cout << out.str();
      }
    }
  }
//  std::cout << " <br>final remainder: "
//  << outputRemainder.ToString(&theGlobalVariables->theDefaultFormat)
//  << "<hr>";
}

bool GroebnerBasisComputation::AddRemainderToBasis
 (GlobalVariables* theGlobalVariables)
{ if (this->leadingMons.size!=this->theBasiS.size)
  { std::cout << "This is a programming error: the number of leading monomials does not equal "
    << " the number of polynomials. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  MacroRegisterFunctionWithName("GroebnerBasisComputation::AddPolyToBasis");
  if (this->remainderDivision.IsEqualToZero())
    return false;
  this->remainderDivision.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
  int indexMaxMon=this->remainderDivision.GetIndexMaxMonomial(this->theMonOrdeR);
  MonomialP& theNewLeadingMon=this->remainderDivision[indexMaxMon];
  if (this->flagDoSortBasis)
  { this->theBasiS.SetSize(this->theBasiS.size+1);
    this->leadingMons.SetSize(this->theBasiS.size);
    this->leadingCoeffs.SetSize(this->theBasiS.size);
    for (int i=theBasiS.size-1; i>=0; i--)
    { bool shouldAddHere=(i==0) ? true: this->remainderDivision.size>this->theBasiS[i-1].size;
      if (shouldAddHere)
      { this->theBasiS[i]=this->remainderDivision;
        this->leadingMons[i]=theNewLeadingMon;
        this->leadingCoeffs[i]=this->remainderDivision.theCoeffs[indexMaxMon];
        break;
      } else
      { this->theBasiS[i]=this->theBasiS[i-1];
        this->leadingMons[i]=this->leadingMons[i-1];
        this->leadingCoeffs[i]=this->leadingCoeffs[i-1];
      }
    }
  } else
  { this->theBasiS.AddOnTop(this->remainderDivision);
    this->leadingMons.AddOnTop(theNewLeadingMon);
    this->leadingCoeffs.AddOnTop(this->remainderDivision.theCoeffs[indexMaxMon]);
  }
//  this->theBasiS.AddOnTop(this->remainderDivision);
//  this->leadingMons.AddOnTop(theNewLeadingMon);
//  this->leadingCoeffs.AddOnTop(this->remainderDivision.theCoeffs[indexMaxMon]);
  return true;
}

GroebnerBasisComputation::GroebnerBasisComputation()
{ this->theMonOrdeR=MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  this->NumberOfComputations=0;
  this->flagDoProgressReport=true;
  this->flagDoSortBasis=true;
  this->flagBasisGuaranteedToGenerateIdeal=false;
  this->MaxNumComputations=0;
}

void GroebnerBasisComputation::initTheBasis
(List<Polynomial<Rational> >& inputOutpuT, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("GroebnerBasisComputation::initTheBasis");
  if (inputOutpuT.size<=0)
  { std::cout << "This is a programming error: I cannot transform an "
    << "empty list to a Groebner basis. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->theBasiS.SetSize(0);
  this->theBasiS.ReservE(inputOutpuT.size);
  this->leadingMons.SetSize(0);
  this->leadingMons.ReservE(inputOutpuT.size);
  this->leadingCoeffs.SetSize(0);
  this->leadingCoeffs.ReservE(inputOutpuT.size);
  this->NumberOfComputations=0;
}

void GroebnerBasisComputation::ConsistencyCheck()
{ if (this->NumberOfComputations>this->MaxNumComputations+1000)
  { std::cout
    << "This may or may not be a programming error. While handling computation excess limit, "
    << " I got that NumberOfComputations is much larger than MaxNumComputations. "
    << " I have no explanation for this issue right now, so I am crashing to let you know "
    << " something is fishy. ";
    std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
}

bool GroebnerBasisComputation::TransformToReducedGroebnerBasis
  (List<Polynomial<Rational> >& inputOutpuT,
   GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("RationalFunctionOld::TransformToReducedGroebnerBasis");
  this->initTheBasis(inputOutpuT, theGlobalVariables);
  this->basisCandidates=inputOutpuT;
  ProgressReport theReport(theGlobalVariables);
 // std::string tempS;
//  bool changed=true;
  this->flagBasisGuaranteedToGenerateIdeal=false;
  this->AddPolyAndReduceBasis(theGlobalVariables);
  this->flagBasisGuaranteedToGenerateIdeal=true;
  bool changed=true;
  while (changed)
  { changed=false;
    for (int i=0; i<this->theBasiS.size; i++)
      for (int j=i+1; j<this->theBasiS.size && i<this->theBasiS.size; j++)
      { Polynomial<Rational>& currentLeft= this->theBasiS[i];
        Polynomial<Rational>& currentRight= this->theBasiS[j];
        int leftIndex=currentLeft.GetIndexMaxMonomial(this->theMonOrdeR);
        int rightIndex=currentRight.GetIndexMaxMonomial(this->theMonOrdeR);
        MonomialP& leftHighestMon=currentLeft[leftIndex];
        MonomialP& rightHighestMon=currentRight[rightIndex];
        int numVars=
        MathRoutines::Maximum(leftHighestMon.GetMinNumVars(), rightHighestMon.GetMinNumVars());
        this->SoPolyLeftShift.MakeOne(numVars);
        this->SoPolyRightShift.MakeOne(numVars);
        for (int k=0; k<numVars; k++)
          if (leftHighestMon[k]>rightHighestMon[k])
          { this->SoPolyRightShift[k]=leftHighestMon[k]-rightHighestMon[k];
            this->SoPolyLeftShift[k]=0;
          } else
          { this->SoPolyLeftShift[k]=rightHighestMon[k]-leftHighestMon[k];
            this->SoPolyRightShift[k]=0;
          }
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out << "Computing Sopoly of indices " << i +1 << " out of "
          << this->theBasiS.size << " and "
          << j+1 << " out of " << this->theBasiS.size;
          theReport.Report(out.str());
        }
        this->bufPoly=currentLeft;
        this->bufPoly.MultiplyBy(this->SoPolyLeftShift, currentRight.theCoeffs[rightIndex]);
        this->SoPolyBuf=currentRight;
        this->SoPolyBuf.MultiplyBy(this->SoPolyRightShift, currentLeft.theCoeffs[leftIndex]);
        this->SoPolyBuf-=(this->bufPoly);
        this->basisCandidates.AddOnTop(this->SoPolyBuf);
        this->NumberOfComputations++;
        if (this->MaxNumComputations>0)
          if (this->NumberOfComputations>this->MaxNumComputations)
            return false;
//        if (this->AddPolyAndReduceBasis(theGlobalVariables))
//        { i=0;
//          j=-1;
//          changed=true;
//        }
      }
    this->flagBasisGuaranteedToGenerateIdeal=true;
    changed=this->AddPolyAndReduceBasis(theGlobalVariables);
    this->flagBasisGuaranteedToGenerateIdeal=true;
    if (this->MaxNumComputations>0)
      if (this->NumberOfComputations>this->MaxNumComputations)
        return false;
  }
//  this->MakeMinimalBasis();
  inputOutpuT=this->theBasiS;
  return true;
}

bool GroebnerBasisComputation::AddPolyAndReduceBasis
 (GlobalVariables* theGlobalVariables)
{ bool changed=false;
  ProgressReport theReport(theGlobalVariables);
  while (this->basisCandidates.size>0)
  { bool addedNew=false;
    while (this->basisCandidates.size>0)
    { this->RemainderDivisionWithRespectToBasis
      (*this->basisCandidates.LastObject(), &this->remainderDivision, theGlobalVariables);
      this->basisCandidates.PopLastObject();
      if(this->AddRemainderToBasis(theGlobalVariables))
      { changed=true;
        addedNew=true;
      }
      this->NumberOfComputations++;
      if (this->MaxNumComputations>0)
        if (this->NumberOfComputations>this->MaxNumComputations)
        { if (!this->flagBasisGuaranteedToGenerateIdeal)
            this->theBasiS.AddListOnTop(this->basisCandidates);
          this->ConsistencyCheck();
          return true;
        }
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Reducing: " << theBasiS.size << " basis elements, "
        << this->basisCandidates.size << " candidates. ";
        theReport.Report(out.str());
//          std::cout << "<br>" << out.str();
      }
    }
    this->flagBasisGuaranteedToGenerateIdeal=true;

    if (!addedNew)
      break;
    for (int i=0; i<this->theBasiS.size; i++)
    { this->bufPolyForGaussianElimination=this->theBasiS[i];
      if (theGlobalVariables!=0 && this->flagDoProgressReport)
      { std::stringstream out;
        out << "Verifying basis element " << i+1 << " out of "
        << theBasiS.size << " basis elements, " << this->basisCandidates.size
        << " candidates. ";
        theReport.Report(out.str());
//        std::cout << "<br>" << out.str();
      }
      this->NumberOfComputations++;
      this->RemainderDivisionWithRespectToBasis
      (this->bufPolyForGaussianElimination, &this->remainderDivision,
      theGlobalVariables, i);
      if (this->MaxNumComputations>0)
        if (this->NumberOfComputations>this->MaxNumComputations)
        { this->ConsistencyCheck();
          return true;
        }
      if (!(this->remainderDivision==this->theBasiS[i]))
      { this->flagBasisGuaranteedToGenerateIdeal=false;
        this->basisCandidates.AddOnTop(this->remainderDivision);
        this->leadingMons.PopIndexSwapWithLast(i);
        this->leadingCoeffs.PopIndexSwapWithLast(i);
        this->theBasiS.PopIndexSwapWithLast(i);
        i--;
        changed=true;
      }
    }
  }
  return changed;
}

void GroebnerBasisComputation::MakeMinimalBasis()
{ MacroRegisterFunctionWithName("RationalFunctionOld::GroebnerBasisMakeMinimal");
/*  std::cout << "<br><br> and the leading monomials are: ";
  for (int i=0; i<LeadingCoeffs.size; i++)
    std::cout << LeadingCoeffs[i].ToString() << ", ";*/
  for (int i=0; i<this->theBasiS.size; i++)
    for (int j=0; j<this->leadingMons.size; j++)
      if (i!=j)
        if (this->leadingMons[i].IsDivisibleBy(this->leadingMons[j]))
        { /*std::cout << "<br>" << LeadingCoeffs[i].ToString() << " is divisible by "
          << LeadingCoeffs[j].ToString();*/
          this->leadingMons.PopIndexSwapWithLast(i);
          this->theBasiS.PopIndexSwapWithLast(i);
          i--;
          break;
        } //else
        //{ std::cout << "<br>" << LeadingCoeffs[i].ToString() << " is NOT divisible by "
        //  << LeadingCoeffs[j].ToString();
        //
/*  std::cout << "<br><br> final leading monomials are: ";
  for (int i=0; i<LeadingCoeffs.size; i++)
    std::cout <<"<br>" << LeadingCoeffs[i].ToString() << " of " << theBasis[i].ToString();
*/
}

bool GroebnerBasisComputation::CriterionCLOsh
(HashedListSpecialized
 <Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&
 thePairs, List<MonomialP>& theLeadingMons, MonomialP& leadingTermLCM)
{ //page 107,  Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity >&
  lastPair=*thePairs.LastObject();
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity >
  pairBeingTested1, pairBeingTested2;
  for (int k=0; k<theLeadingMons.size; k++)
    if (k!=lastPair.Object1 && k!=lastPair.Object2)
      if (leadingTermLCM.IsDivisibleBy(theLeadingMons[k]))
      { pairBeingTested1.Object1=MathRoutines::Minimum(lastPair.Object1, k);
        pairBeingTested1.Object2=MathRoutines::Maximum(lastPair.Object1, k);
        pairBeingTested2.Object1=MathRoutines::Minimum(lastPair.Object2, k);
        pairBeingTested2.Object2=MathRoutines::Maximum(lastPair.Object2, k);
        if (!thePairs.Contains(pairBeingTested1) && !thePairs.Contains(pairBeingTested2))
          return true;
      }
  return false;
}

bool GroebnerBasisComputation::TransformToReducedGroebnerBasisImprovedAlgorithm
(List<Polynomial<Rational> >& inputOutpuT,
 GlobalVariables* theGlobalVariables, int upperComputationBound
)
{ MacroRegisterFunctionWithName
  ("RationalFunction_CoefficientType::TransformToReducedGroebnerBasisImprovedAlgorithm");
   //This is an implementation of the algorithm on page 106, Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  this->initTheBasis(inputOutpuT, theGlobalVariables);
  this->theBasiS=inputOutpuT;
  HashedListSpecialized<PairInts > indexPairs;
//  Pair<int, int> currentPair;
  indexPairs.SetExpectedSize(this->theBasiS.size*this->theBasiS.size);
  this->leadingMons.SetExpectedSize(this->theBasiS.size*2);
  for (int i=0; i<this->theBasiS.size; i++)
  { for (int j=i+1; j<this->theBasiS.size; j++)
      indexPairs.AddOnTop(PairInts (i,j));
    this->theBasiS[i].ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
    int theIndex=this->theBasiS[i].GetIndexMaxMonomial(this->theMonOrdeR);
    this->leadingMons.AddOnTop(this->theBasiS[i][theIndex]);
    this->leadingCoeffs.AddOnTop(this->theBasiS[i].theCoeffs[theIndex]);
  }
  if (this->theBasiS.size<=0)
  { std::cout << "This is a programming error: "
    << "transforming to Groebner basis not allowed for empty basis. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  MonomialP leftShift, rightShift, monLCM;
  Polynomial<Rational> leftBuf, rightBuf, buffer1;
  Polynomial<Rational>& outputRemainder=rightBuf; //to save some RAM
  ProgressReport reportOuter(theGlobalVariables);
  ProgressReport reportInner(theGlobalVariables);
  while (indexPairs.size>0)
  { PairInts& lastPair=*indexPairs.LastObject();
    int currentPairIndex=indexPairs.size-1;
    bool isGood=false;
    Polynomial<Rational>& currentLeft = this->theBasiS[lastPair.Object1];
    Polynomial<Rational>& currentRight= this->theBasiS[lastPair.Object2];
    MonomialP& leftHighestMon= this->leadingMons[lastPair.Object1];
    MonomialP& rightHighestMon=this->leadingMons[lastPair.Object2];
    if (theGlobalVariables!=0 && this->flagDoProgressReport)
    { std::stringstream out;
      out
      << "Basis size: " << this->theBasiS.size
      << ".\n<br>Remaining cases current round: " << indexPairs.size;
      reportOuter.Report(out.str());
    }
    int numVars=MathRoutines::Maximum(leftHighestMon.GetMinNumVars(), rightHighestMon.GetMinNumVars());
    leftShift.MakeOne(numVars);
    rightShift.MakeOne(numVars);
    for (int k=0; k<numVars; k++)
    { if (leftHighestMon[k]>0 && rightHighestMon[k]>0)
        isGood=true;
      if (leftHighestMon[k]>rightHighestMon[k])
      { rightShift[k]=leftHighestMon[k]-rightHighestMon[k];
        leftShift[k]=0;
      } else
      { leftShift[k]=rightHighestMon[k]-leftHighestMon[k] ;
        rightShift[k]=0;
      }
    }
    monLCM=leftHighestMon;
    monLCM*=leftShift;
    if (isGood)
      if (!this->CriterionCLOsh(indexPairs, this->leadingMons, monLCM))
      { leftBuf=currentLeft;
        rightBuf=currentRight;
        leftBuf.MultiplyBy(leftShift, this->leadingCoeffs[lastPair.Object2]);
        rightBuf.MultiplyBy(rightShift, this->leadingCoeffs[lastPair.Object1]);
        leftBuf-=rightBuf;
//        std::cout << "<br>testing sopoly: "
//        << leftBuf.ToString(&theGlobalVariables->theDefaultFormat)
//        ;
        this->RemainderDivisionWithRespectToBasis
        (leftBuf, &outputRemainder, theGlobalVariables)
        ;
        if (theGlobalVariables!=0 && this->flagDoProgressReport)
        { std::stringstream out;
          out
          << "Basis size: " << this->theBasiS.size
          << ".\n<br>Remaining cases current round: " << indexPairs.size;
          reportOuter.Report(out.str());
        }
//        std::cout << "<br>and sopoly divided by "
//        << theBasis.ToString(&theGlobalVariables->theDefaultFormat) << " is "
//        << outputRemainder.ToString(&theGlobalVariables->theDefaultFormat);
        if (!outputRemainder.IsEqualToZero())
        { outputRemainder.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
          this->theBasiS.AddOnTop(outputRemainder);
          int theIndexMaxMon=this->theBasiS.LastObject()->GetIndexMaxMonomial(this->theMonOrdeR);
          this->leadingMons.AddOnTop((*this->theBasiS.LastObject())[theIndexMaxMon]);
          this->leadingCoeffs.AddOnTop(this->theBasiS.LastObject()->theCoeffs[theIndexMaxMon]);
          for (int i=0; i<this->theBasiS.size-1; i++)
            indexPairs.AddOnTop(PairInts(i, this->theBasiS.size-1));
        }
      } //else
        //std::cout << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
        //<< " failed the closh criterium";
    //else
      //std::cout << "<br>" << leftHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " and " << rightHighestMon.ToString(&theGlobalVariables->theDefaultFormat)
      //<< " failed the lcm criterium";
    indexPairs.PopIndexSwapWithLast(currentPairIndex);
  }
  this->MakeMinimalBasis();
  inputOutpuT=this->theBasiS;
  return true;
}

void RationalFunctionOld::gcd
(const Polynomial<Rational>& left, const Polynomial<Rational>& right,
 Polynomial<Rational>& output, GlobalVariables* theGlobalVariables)
{ if (RationalFunctionOld::gcdQuicK(left, right, output))
    return;
  MacroRegisterFunctionWithName("RationalFunctionOld::gcd");
  MemorySaving<Polynomial<Rational> > buf1, buf2, buf3;
  Polynomial<Rational>& lcmBuf
  = theGlobalVariables==0 ? buf1.GetElement() : theGlobalVariables->RFgcdBuffer1.GetElement();
  Polynomial<Rational>& prodBuf
  = theGlobalVariables==0 ? buf2.GetElement() : theGlobalVariables->RFgcdBuffer2.GetElement();
  Polynomial<Rational>& remBuf
  = theGlobalVariables==0 ? buf3.GetElement() : theGlobalVariables->RFgcdBuffer3.GetElement();

  RationalFunctionOld::lcm
  (left, right, lcmBuf, theGlobalVariables);
  prodBuf=left;
  prodBuf*=right;
//  std::cout << "<hr>the product: " << buffer2.ToString() << " and the lcm: " << buffer1.ToString() << "<br>";
  prodBuf.DivideBy(lcmBuf, output, remBuf);
  if (!remBuf.IsEqualToZero() || output.IsEqualToZero() )
  { std::cout << "This is a programming error. <br>While computing the gcd of left=" << left.ToString()
    << " <br>and right=" << right.ToString() << " <br>I got that left*right= "
    << prodBuf.ToString()
    << "<br>, and that lcm(left,right)=" << lcmBuf.ToString() << " <br>but at the same time "
    << "right*left divided by lcm (left, right) equals <br>" << output.ToString()
    << "<br> with remainder "
    << remBuf.ToString() << ", "
    << " which is imposible. "
    << " <br>The Groebner basis follows. <br>";
    std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
//  std::cout << "<br>and the result of gcd (product/lcm)= " << output.ToString() << "<hr>";
}

void RationalFunctionOld::MakeOneLetterMoN
  (int theIndex, const Rational& theCoeff, GlobalVariables& theGlobalVariables,
   int ExpectedNumVars)
{ if ( theIndex<0)
  { std::cout << "This is a programming error: I am asked to create Monomial "
    << " which has a variable of negative index " << theIndex << ". "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->expressionType=this->typePoly;
  ExpectedNumVars=MathRoutines::Maximum(theIndex+1, ExpectedNumVars);
  this->Numerator.GetElement().MakeDegreeOne
  (ExpectedNumVars, theIndex, theCoeff);
  this->context=&theGlobalVariables;
}

void RationalFunctionOld::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ int oldNumVars=this->GetMinNumVars();
  this->Numerator.GetElement().SetNumVariablesSubDeletedVarsByOne(newNumVars);
  this->Denominator.GetElement().SetNumVariablesSubDeletedVarsByOne(newNumVars);
  if (newNumVars<oldNumVars)
    this->Simplify();
//    assert(this->checkConsistency());
}

void RationalFunctionOld::operator=(const RationalFunctionOld& other)
{// int commentmewhendone2;
  //other.checkConsistency();
  //this->checkConsistency();
  this->expressionType=other.expressionType;
  if (other.context!=0)
    this->context=other.context;
  switch (this->expressionType)
  { case RationalFunctionOld::typeRational:
      this->ratValue=other.ratValue;
      //The below is for testing purposes. I think it is generally better to comment those lines!
      //this->Numerator.FreeMemory();
      //this->Denominator.FreeMemory();
      break;
    case RationalFunctionOld::typePoly:
      assert(!other.Numerator.IsZeroPointer() );
      this->Numerator.GetElement()=other.Numerator.GetElementConst();
      //The below is for testing purposes. I think it is generally better to comment those lines!
      //this->Denominator.FreeMemory();
      break;
    case RationalFunctionOld::typeRationalFunction:
      assert(!other.Numerator.IsZeroPointer() && !other.Denominator.IsZeroPointer());
      this->Numerator.GetElement()=other.Numerator.GetElementConst();
      this->Denominator.GetElement()=other.Denominator.GetElementConst();
      break;
    default:
      break;
  }
//    int commentmewhendone;
//    other.checkConsistency();
//    this->checkConsistency();
}

void RationalFunctionOld::lcm
(const Polynomial<Rational>& left, const Polynomial<Rational>& right,
 Polynomial<Rational>& output, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("RationalFunctionOld::lcm");
  MemorySaving<Polynomial<Rational> > buffer1, buffer2, buffer3;
  Polynomial<Rational>& leftTemp=
  theGlobalVariables==0? buffer1.GetElement() : theGlobalVariables->RFgcdBuffer1.GetElement();
  Polynomial<Rational>& rightTemp=
  theGlobalVariables==0? buffer2.GetElement() : theGlobalVariables->RFgcdBuffer2.GetElement();
  Polynomial<Rational>& tempP=
  theGlobalVariables==0? buffer3.GetElement() : theGlobalVariables->RFgcdBuffer3.GetElement();
  MemorySaving<List<Polynomial<Rational> > > bufferList;
  List<Polynomial<Rational> >& theBasis=
  theGlobalVariables==0? bufferList.GetElement():theGlobalVariables->RFgcdBufferList1.GetElement();

  leftTemp=(left);
  rightTemp=(right);
  int theNumVars=MathRoutines::Maximum(left.GetMinNumVars(), right.GetMinNumVars());
  leftTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars+1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars+1);
  leftTemp.ScaleToIntegralNoGCDCoeffs();
  rightTemp.ScaleToIntegralNoGCDCoeffs();
  tempP.MakeMonomiaL(theNumVars, 1, (Rational) 1, theNumVars+1);
  leftTemp*=(tempP);
  tempP*=-1;
  tempP+=1;
  rightTemp*=(tempP);
  theBasis.size=0;
  theBasis.AddOnTop(leftTemp);
  theBasis.AddOnTop(rightTemp);
/*  std::cout << "<br>In the beginning: <br>";
  for (int i=0; i<theBasis.size; i++)
  { std::cout << theBasis[i].ToString() << "<br>\n";
  }*/
  MemorySaving<GroebnerBasisComputation> bufComp;
  GroebnerBasisComputation& theComp=theGlobalVariables==0?
  bufComp.GetElement(): theGlobalVariables->theGroebnerBasisComputation.GetElement();
  theComp.theMonOrdeR=MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  theComp.TransformToReducedGroebnerBasis(theBasis, theGlobalVariables);
//  std::cout << "<br><br> ... and the basis is: <br>";
//  for (int i=0; i<tempList.size; i++)
//  { std::cout << tempList[i].ToString() << "<br>\n";
//  }
  int maxMonNoTIndex=-1;
  Rational MaxTotalDeg;
  for(int i=0; i<theBasis.size; i++)
  { MonomialP& currentMon= theBasis[i][theBasis[i].GetIndexMaxMonomialLexicographicLastVariableStrongest()];
    if (currentMon[theNumVars]==0)
    { if (maxMonNoTIndex==-1)
      { MaxTotalDeg= currentMon.TotalDegree();
        maxMonNoTIndex=i;
      }
      if (MaxTotalDeg<currentMon.TotalDegree())
      { MaxTotalDeg=currentMon.TotalDegree();
        maxMonNoTIndex=i;
      }
    }
  }
  if (maxMonNoTIndex==-1)
  { std::cout << "This is a programming error: failed to obtain lcm of two rational functions. "
    << "The list of polynomials is: ";
    for (int i=0; i<theBasis.size; i++)
      std::cout << theBasis[i].ToString() << ", ";
    std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output=theBasis[maxMonNoTIndex];
  output.SetNumVariablesSubDeletedVarsByOne(theNumVars);
  output.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
}

void RationalFunctionOld::operator*=(const MonomialP& other)
{ //std::cout << "<br>Multiplying " << this->ToString() << " times " << other.ToString();
  Polynomial<Rational> otherP;
  otherP.MakeZero();
  otherP.AddMonomial(other, 1);
  //std::cout << ", otherP is: " << otherP.ToString();
  (*this)*=otherP;
  //std::cout << " to get " << this->ToString();
}

void RationalFunctionOld::operator*=(const Polynomial<Rational>& other)
{ if (other.IsEqualToZero())
  { this->MakeZero(this->context);
    return;
  }
  if (this->expressionType== this->typeRational)
    this->ConvertToType(this->typePoly);
  if (this->expressionType==this->typePoly)
  { this->Numerator.GetElement()*=other;
    this->ReduceMemory();
    return;
  }
  Polynomial<Rational> theGCD, theResult, tempP;
  ProgressReport theReport(this->context);
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString(&this->context->theDefaultFormat) << " by "
    << other.ToString(&this->context->theDefaultFormat);
    theReport.Report(out.str());
  }
  RationalFunctionOld::gcd(this->Denominator.GetElement(), other, theGCD, this->context);
  this->Numerator.GetElement()*=other;
  this->Numerator.GetElement().DivideBy(theGCD, theResult, tempP);
  //theGCD.ComputeDebugString();
  //theResult.ComputeDebugString();
  //tempP.ComputeDebugString();
  assert(tempP.IsEqualToZero());
  this->Numerator.GetElement()=theResult;
  this->Denominator.GetElement().DivideBy(theGCD, theResult, tempP);
  //theGCD.ComputeDebugString();
  //theResult.ComputeDebugString();
//  tempP.ComputeDebugString();
  assert(tempP.IsEqualToZero());
  this->Denominator.GetElement()=theResult;
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString(&this->context->theDefaultFormat) << " by "
    << other.ToString(&this->context->theDefaultFormat);
    out << " and the result is:\n" << this->ToString();
    theReport.Report(out.str());
  }
//  this->ComputeDebugString();
}

void RationalFunctionOld::operator/=(const RationalFunctionOld& other)
{ this->checkConsistency();
  RationalFunctionOld tempRF;
  if (other.context!=0)
    this->context=other.context;
  tempRF=other;
  tempRF.checkConsistency();
  tempRF.Invert();
  tempRF.checkConsistency();
  *this*=(tempRF);
  assert(this->checkConsistency());
}


void RationalFunctionOld::operator*=(const Rational& other)
{ //assert(this->checkConsistency());
  if (other.IsEqualToZero())
  { this->MakeZero(this->context);
    return;
  }
  switch(this->expressionType)
  { case RationalFunctionOld::typeRational: this->ratValue*=other; return;
    case RationalFunctionOld::typePoly: this->Numerator.GetElement()*=(other); return;
    case RationalFunctionOld::typeRationalFunction:
      this->Numerator.GetElement()*=(other);
      this->SimplifyLeadingCoefficientOnly();
      return;
  }
}

void RationalFunctionOld::operator*=(const RationalFunctionOld& other)
{// int commentChecksWhenDoneDebugging=-1;
  this->checkConsistency();
  other.checkConsistency();
  if (this==&other)
  { this->RaiseToPower(2);
    return;
  }
  if (other.context!=0)
    this->context=other.context;
  if (other.IsEqualToZero() || this->IsEqualToZero())
  { this->MakeZero(this->context);
    return;
  }
  if (other.expressionType==this->typeRational)
  { this->operator*=(other.ratValue);
    return;
  }
  if (other.expressionType==this->typePoly)
  { this->operator*=(other.Numerator.GetElementConst());
    return;
  }
  if (this->expressionType ==this->typeRational)
  { Rational tempRat;
    tempRat=this->ratValue;
    this->operator=(other);
    this->operator*=(tempRat);
    return;
  }
  if (this->expressionType==this->typePoly)
  { Polynomial<Rational>  tempP;
    tempP=this->Numerator.GetElement();
    this->operator=(other);
    this->operator*=(tempP);
    return;
  }
  Polynomial<Rational>  theGCD1, theGCD2, tempP1, tempP2;
  //this->ComputeDebugString();
//  RationalFunctionOld tempde_Bugger;
//  tempde_Bugger=other;
//  tempde_Bugger.ComputeDebugString();
  ProgressReport theReport(this->context);
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString() << " by " << other.ToString();
    theReport.Report(out.str());
  }
  RationalFunctionOld::gcd(other.Denominator.GetElementConst(), this->Numerator.GetElement(), theGCD1, this->context);
  RationalFunctionOld::gcd(this->Denominator.GetElement(), other.Numerator.GetElementConst(), theGCD2, this->context);
  this->Numerator.GetElement().DivideBy(theGCD1, tempP1, tempP2);
  this->Numerator.GetElement()=tempP1;
  assert(tempP2.IsEqualToZero());
  other.Denominator.GetElementConst().DivideBy(theGCD1, tempP1, tempP2);
  assert(tempP2.IsEqualToZero());
  this->Denominator.GetElement()*=tempP1;
  this->Denominator.GetElement().DivideBy(theGCD2, tempP1, tempP2);
  assert(tempP2.IsEqualToZero());
  this->Denominator.GetElement()=tempP1;
  other.Numerator.GetElementConst().DivideBy(theGCD2, tempP1, tempP2);
  assert(tempP2.IsEqualToZero());
  this->Numerator.GetElement()*=(tempP1);
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ToString() << " by " << other.ToString();
    out << " and the result is:\n" << this->ToString();
    theReport.Report(out.str());
  }
}

void RationalFunctionOld::Simplify()
{ MacroRegisterFunctionWithName("RationalFunctionOld::Simplify");
  if (this->expressionType==this->typeRationalFunction)
    if(!this->Numerator.GetElement().IsEqualToZero())
    { Polynomial<Rational> theGCD, tempP, tempP2;
//      std::cout << "<br>fetching gcd of " << this->Numerator.GetElement().ToString() << " and "
//      << this->Denominator.GetElement().ToString() << "<br>";
      this->gcd(this->Numerator.GetElement(), this->Denominator.GetElement(), theGCD, this->context);
      if (theGCD.IsEqualToZero())
      { std::cout << "This is a programing error: "
        << " while fetching the gcd of " << this->Numerator.GetElement().ToString() << " and "
        << this->Denominator.GetElement().ToString() << " I got 0, which is impossible. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
//      std::cout << "to get " << theGCD.ToString();
//      std::cout << "<br>dividing " << this->Numerator.GetElement().ToString() << " by " << theGCD.ToString() << "<br>";
      this->Numerator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Numerator.GetElement()=(tempP);
      this->Denominator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Denominator.GetElement()=(tempP);
    }
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
//  int commentMeWhendone;
//  assert(this->checkConsistency());
}

void RationalFunctionOld::SimplifyLeadingCoefficientOnly()
{ if (this->expressionType!=this->typeRationalFunction)
    return;
  Rational tempRat=
  this->Denominator.GetElement().theCoeffs[this->Denominator.GetElement().GetIndexMaxMonomialLexicographicLastVariableStrongest()];
  tempRat.Invert();
  this->Denominator.GetElement()*=(tempRat);
  this->Numerator.GetElement()*=(tempRat);
  if (this->Denominator.GetElement().IsEqualToOne())
  { this->expressionType=this->typePoly;
    if (this->Numerator.GetElement().IsAConstant(&tempRat))
    { this->ratValue=tempRat;
      this->expressionType=this->typeRational;
    }
  }
}

void RootIndexToPoly(int theIndex, SemisimpleLieAlgebra& theAlgebra, Polynomial<Rational> & output)
{ int theRank=theAlgebra.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theAlgebra.theWeyl.RootsOfBorel.size;
  output.MakeDegreeOne((int)(theRank+numPosRoots), theIndex+theRank, (Rational) 1);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::AssignFromCoordinateFormWRTBasis
(List<ElementUniversalEnveloping<CoefficientType> >& theBasis, Vector<CoefficientType>& input,
 SemisimpleLieAlgebra& owner)
{ /*int numVars=0;
  if (theBasis.size>0)
    numVars= theBasis[0].GetNumVars();*/
  this->MakeZero(owner);
  ElementUniversalEnveloping<CoefficientType> tempElt;
  for (int i=0; i<input.size; i++)
    if (!input[i].IsEqualToZero())
    { tempElt.operator=(theBasis[i]);
      tempElt.operator*=(input[i]);
      this->operator+=(tempElt);
    }
}

void RationalFunctionOld::ScaleClearDenominator
  (List<RationalFunctionOld>& input, Vector<Polynomial<Rational> >& output)
{ Polynomial<Rational>  tempP;
  List<RationalFunctionOld> buffer;
  buffer=(input);
  for (int i=0; i<buffer.size; i++)
  { RationalFunctionOld& current=buffer.TheObjects[i];
    if (current.expressionType==RationalFunctionOld::typeRationalFunction)
    { tempP.operator=(current.Denominator.GetElement());
      for (int j=0; j<buffer.size; j++)
        buffer[j].operator*=(tempP);
    }
  }
  output.SetSize(input.size);
  for (int i=0; i<buffer.size; i++)
  { buffer[i].GetNumerator(tempP);
    output[i]=tempP;
  }
}

void SemisimpleLieAlgebraOrdered::GetLinearCombinationFrom
  (ElementSemisimpleLieAlgebra<Rational>& input, Vector<Rational>& theCoeffs)
{ theCoeffs.MakeZero(this->theOwner.GetNumGenerators());
  for (int i=0; i<input.size; i++)
  { int theIndex=input[i].theGeneratorIndex;
    theCoeffs.TheObjects[this->theOwner.GetGeneratorFromRootIndex(theIndex)]=input.theCoeffs[i];
  }
  int numPosRoots=this->theOwner.GetNumPosRoots();
  Vector<Rational> tempH=input.GetCartanPart();
  for (int i=0; i<this->theOwner.GetRank(); i++)
    theCoeffs[numPosRoots+i]= tempH[i];
  this->ChevalleyGeneratorsInCurrentCoords.ActOnVectorColumn(theCoeffs);
}

int SemisimpleLieAlgebraOrdered::GetDisplayIndexFromGeneratorIndex(int GeneratorIndex)
{ int numPosRoots=this->theOwner.GetNumPosRoots();
  int posRootsPlusRank=numPosRoots+this->theOwner.GetRank();
  if (GeneratorIndex>= posRootsPlusRank )
    return GeneratorIndex-posRootsPlusRank+1;
  if (GeneratorIndex>=numPosRoots)
    return GeneratorIndex+1;
  return -numPosRoots+GeneratorIndex;
}

void SemisimpleLieAlgebraOrdered::init
(List<ElementSemisimpleLieAlgebra<Rational> >& inputOrder, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ return;
  if (inputOrder.size!=owner.GetNumGenerators())
    return;
  this->theOwner=owner;
  this->theOrder=inputOrder;
  this->ChevalleyGeneratorsInCurrentCoords.init(owner.GetNumGenerators(), owner.GetNumGenerators());
  this->ChevalleyGeneratorsInCurrentCoords.NullifyAll();
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vector<Rational> coordsInCurrentBasis;
  ElementSemisimpleLieAlgebra<Rational> currentElt;
  for (int i=0; i<owner.GetNumGenerators(); i++)
  { currentElt.MakeGenerator(i, owner);
    currentElt.GetCoordsInBasis(this->theOrder, coordsInCurrentBasis, theGlobalVariables);
    for (int j=0; j<coordsInCurrentBasis.size; j++)
      this->ChevalleyGeneratorsInCurrentCoords.elements[j][i]=coordsInCurrentBasis[j];
//    std::cout << "<br> " << currentElt.ToString() << " in new coords becomes: " << coordsInCurrentBasis.ToString();
  }
//  std::cout << this->ChevalleyGeneratorsInCurrentCoords.ToString(true, false) << "<br><br>";

 // this->ChevalleyGeneratorsInCurrentCoords.Invert(theGlobalVariables);
  //std::cout << this->ChevalleyGeneratorsInCurrentCoords.ToString(true, false);
}

void SemisimpleLieAlgebraOrdered::initDefaultOrder
  (SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ List<ElementSemisimpleLieAlgebra<Rational> > defaultOrder;
  defaultOrder.SetSize(owner.GetNumGenerators());
  for (int i=0; i<defaultOrder.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt=defaultOrder[i];
    currentElt.MakeGenerator(i, owner);
  }
  this->init(defaultOrder, owner, theGlobalVariables);
}

template <class CoefficientType>
bool ElementSemisimpleLieAlgebra<CoefficientType>::MustUseBracketsWhenDisplayingMeRaisedToPower()
{ if (this->size==1)
    if (this->theCoeffs[0]==1)
      return false;
  return true;
}

template <class CoefficientType>
bool ElementSemisimpleLieAlgebra<CoefficientType>::IsACoeffOneChevalleyGenerator
(int& outputGenerator, SemisimpleLieAlgebra& owner)
{ if (this->size==1)
    return this->theCoeffs[0]==1;
  return false;
}

std::string ParserNode::ElementToStringValueOnlY
(bool useHtml, int RecursionDepth, int maxRecursionDepth, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ std::stringstream LatexOutput;
  int i; //can't declare variables within a switch clause!
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: LatexOutput << this->intValue; break;
    case ParserNode::typeRational: LatexOutput << this->rationalValue.ToString(); break;
    case ParserNode::typePoly: LatexOutput << "\\begin{array}{rcl}&&\n" << this->polyValue.GetElement().ToString(&theFormat) << "\n\\end{array}"; break;
    case ParserNode::typeRationalFunction: LatexOutput << "\\begin{array}{rcl}&&\n" << this->ratFunction.GetElement().ToString(&theFormat) << "\n\\end{array}"; break;
    case ParserNode::typeUEElementOrdered:
      LatexOutput << "\\begin{array}{rcl}&&\n"
      << this->UEElementOrdered.GetElement().ToString
      (true, theFormat.flagUseCalculatorFormatForUEOrdered, theFormat, theGlobalVariables)
      << "\n\\end{array}";
      break;
    case ParserNode::typeLittelman:
      break;
    case ParserNode::typeUEelement:
      LatexOutput << "\\begin{array}{rcl}&&\n"
      << this->UEElement.GetElement().ToString(&theFormat)
      << "\n\\end{array}";
      break;
    case ParserNode::typeWeylAlgebraElement:
      LatexOutput << "\\begin{array}{rcl}&&\n"
      << this->WeylAlgebraElement.GetElement().ToString()
      << "\n\\end{array}";
      break;
    case ParserNode::typeCharSSFDMod:
    //  LatexOutput << "\\begin{array}{rcl}&&\n"
//      << this->theChar.GetElement().ToString("char", "", true)
  //    << "\\end{array}";
      break;
    case ParserNode::typeGenVermaElt:
      LatexOutput << this->theGenVermaElt.GetElement().ToString(&theFormat);
      break;
    case ParserNode::typePartialFractions: LatexOutput << this->thePFs.GetElement().ToString(theGlobalVariables, theFormat); break;
    case ParserNode::typeLattice: LatexOutput << this->theLattice.GetElement().ToString(true, false); break;
//    case ParserNode::typeCone: LatexOutput << this->theCone.GetElement().ToString(false, false, true, false, theFormat); break;
    case ParserNode::typeArray:
      LatexOutput << "(";
      RecursionDepth++;
      if (RecursionDepth<=maxRecursionDepth)
        for (i=0; i<this->children.size; i++)
        { LatexOutput << this->owner->TheObjects[this->children.TheObjects[i]].ElementToStringValueOnlY(useHtml, RecursionDepth, maxRecursionDepth, theGlobalVariables, theFormat);
          if (i!=this->children.size-1)
            LatexOutput << ",";
        }
      else
        LatexOutput << "...";
      LatexOutput << ")";
      break;
    default: break;
  }
  return LatexOutput.str();
}

std::string ParserNode::ElementToStringValueAndType
(bool useHtml, int RecursionDepth, int maxRecursionDepth, GlobalVariables& theGlobalVariables, bool displayOutputString, FormatExpressions& theFormat)
{ std::stringstream out;
  std::string stringValueOnly= this->ElementToStringValueOnlY(useHtml, RecursionDepth, maxRecursionDepth, theGlobalVariables, theFormat);
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex:  out << " an integer of value: "; break;
    case ParserNode::typeRational: out << " a rational number of value: "; break;
    case ParserNode::typePoly: out << " a polynomial of value: "; break;
    case ParserNode::typeRationalFunction: out << " a rational function of value: "; break;
    case ParserNode::typeUEElementOrdered: out << "an element of U(g) ordered:"; break;
    case ParserNode::typeUEelement: out << " an element of U(g) of value: "; break;
    case ParserNode::typeWeylAlgebraElement: out << " a Weyl algebra element: "; break;
    case ParserNode::typeLittelman: out << "a Littelmann path: " << this->theLittelmann.GetElement().ToString(); break;
    case ParserNode::typeArray: out << " an array of " << this->children.size << " elements. "; break;
    case ParserNode::typeString: out << "<br>A printout of value: "; break;
    case ParserNode::typeCharSSFDMod: out << "Character of a finite dimensional representation of the ambient"
    << " simple Lie algebra:"; break;
    case ParserNode::typeError: out << this->ElementToStringErrorCode(useHtml); break;
    case ParserNode::typeGenVermaElt: out << "Tensor product of elemetns of generalized Verma modules: "; break;
    case ParserNode::typeLattice: out << "A lattice."; useHtml=true; break;
    case ParserNode::typeCone:
//      theGlobalVariables.theDrawingVariables.theBuffer.init();
//      theGlobalVariables.theDrawingVariables.drawCoordSystemBuffer(theGlobalVariables.theDrawingVariables, this->theCone.GetElement().GetDim(), 0);
      out << "a cone: ";// << this->theCone.GetElement().DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables);
      break;
    case ParserNode::typePiecewiseQP: out << "Piecewise quasi-polynomial: "; break;
    case ParserNode::typeQuasiPolynomial: out << "Quasipolynomial of value: "; break;
    case ParserNode::typePartialFractions: out << "Partial fraction(s): "; break;
    case ParserNode::typeUndefined: out << "Undefined expression (type 0)."; break;
    case ParserNode::typeAnimation: out << "Animation."; break;
    default: out << "Expression type " << this->ExpressionType << "; the programmer(s) have forgotten to enter a type description. "; break;
  }
  if (stringValueOnly!="")
  { if (!useHtml)
      out << stringValueOnly;
    else
      out << CGI::GetHtmlMathDivFromLatexFormulA(stringValueOnly);
  }
  if (displayOutputString)
  { if (this->outputString!="" && this->ExpressionType!=this->typeString)
      out << "<br>In addition, the program generated the following printout. ";
    out << this->outputString;
  } else
    if (this->outputString!="")
      out << "Output string not shown to avoid javascript conflicts.";
  if (this->ExpressionType==this->typeArray)
  { out << "<br>Elements of the array follow.";
    RecursionDepth++;
    if (RecursionDepth<=maxRecursionDepth)
      for (int i=0; i<this->children.size; i++)
        out << "<br>Element of index " << i+1 << ":" << this->owner->TheObjects[this->children.TheObjects[i]].ElementToStringValueAndType
        (useHtml, RecursionDepth, maxRecursionDepth, theGlobalVariables, displayOutputString, theFormat);
  }
  return out.str();
}

int ParserNode::EvaluateSubstitution(GlobalVariables& theGlobalVariables)
{ assert(false);
}

void ParserNode::EvaluateDereferenceArray(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=2)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& firstChild=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& secondChild=this->owner->TheObjects[this->children.TheObjects[1]];
  if (secondChild.ExpressionType!=this->typeIntegerOrIndex || firstChild.ExpressionType!=this->typeArray)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  int arraySize=firstChild.children.size;
  int arrayIndex=secondChild.intValue-1;
  if (arrayIndex>=arraySize || arrayIndex<0)
  { this->SetError(this->errorBadIndex);
    return;
  }
//  ParserNode& relevantChild=this->owner->TheObjects[firstChild.children.TheObjects[arrayIndex]];
//  this->CopyValue(relevantChild);
  return;
}

int ParserNode::EvaluateApplySubstitution(GlobalVariables& theGlobalVariables)
{ assert(false);
  return -1;
}

void ParserNode::TrimSubToMinNumVarsChangeImpliedNumVars(PolynomialSubstitution<Rational>& theSub, int theDimension)
{ theSub.SetSize(theDimension);
  for (int i=0; i<theSub.size; i++)
    theSub.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(theDimension);
  int minNumberVarsAfterSub=theSub.GetHighestIndexSuchThatHigherIndexVarsDontParticipate()+1;
//      std::cout << "minNumberVarsAfterSub: " << minNumberVarsAfterSub << "  " << theSub.ToString() << "<br>";
  for (int i=0; i<theDimension; i++)
    theSub.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(minNumberVarsAfterSub);
  this->impliedNumVars=minNumberVarsAfterSub;
}

void ParserNode::operator=(const ParserNode& other)
{ this->owner=other.owner;
  this->indexParentNode=other.indexParentNode;
  this->indexInOwner=other.indexInOwner;
  this->Operation=other.Operation;
//  this->CopyValue(other);
}

bool Parser::lookAheadTokenAllowsMapsTo(int theToken)
{ return theToken==this->tokenComma || theToken==this->tokenCloseBracket || theToken==this->tokenEnd || theToken==this->tokenEndStatement || theToken==this->tokenColon;
}

bool Parser::StackTopIsAVector(int& outputDimension)
{ return this->StackTopIsDelimiter1ECdotsCEDelimiter2(outputDimension, this->tokenOpenBracket, this->tokenCloseBracket);
}

bool Parser::ReplaceDdotsDbyEdoTheArithmetic()
{ int numTotalDigits=0;
  int indexFirstDoT=this->TokenStack.size;
  for (int i= this->TokenStack.size-1; i>=0; i--)
    if(this->TokenStack.TheObjects[i]!=this->tokenDigit)
    { if (this->TokenStack.TheObjects[i]==this->tokenDot)
      { if (indexFirstDoT!=this->TokenStack.size)
          return false;
        indexFirstDoT=i;
      } else
        break;
    }
    else
      numTotalDigits++;
  int numXs=numTotalDigits;
  if (indexFirstDoT!=this->TokenStack.size)
    numXs++;
  assert(numTotalDigits>0);
  LargeIntUnsigned LargeIntegerReader;
  LargeIntegerReader.MakeZero();

  for (int i=this->ValueStack.size-numXs; i<indexFirstDoT; i++)
  { LargeIntegerReader*=10;
    LargeIntegerReader+= this->ValueStack.TheObjects[i];
  }
  for (int i=indexFirstDoT+1; i<this->ValueStack.size; i++)
  { LargeIntegerReader*=10;
    LargeIntegerReader+= this->ValueStack.TheObjects[i];
  }
  Rational denominator=1;
  if (this->TokenStack.size>indexFirstDoT)
  { denominator=10;
    MathRoutines::RaiseToPower(denominator, this->TokenStack.size-indexFirstDoT-1, (Rational) 1);
    denominator.Invert();
  }
  this->ExpandOnTopIncreaseStacks(this->tokenInteger, this->tokenExpression, 0);
  this->LastObject()->rationalValue=LargeIntegerReader;
  if (!denominator.IsEqualToOne())
    this->LastObject()->rationalValue*=denominator;
  this->TransformRepeatXAtoA(numXs);
  return true;
}

bool Parser::ReplaceXECdotsCEXbyE(int theDimension, int theNewToken, int theOperation)
{ this->ExpandOnTopIncreaseStacks(theOperation, theNewToken, 0);
  int stackSize=this->ValueStack.size;
  for (int i=0; i<theDimension; i++)
    this->FatherByLastNodeChildrenWithIndexInNodeIndex(stackSize-1-2*theDimension+2*i);
  this->TransformRepeatXAtoA(theDimension*2+1);
  return true;
}

bool Parser::ReplaceXECdotsCEXEXbyE(int theDimension, int theNewToken, int theOperation)
{ return this->ReplaceXECdotsCEXbyE(theDimension+1, theNewToken, theOperation);
}

bool Parser::StackTopIsDelimiter1ECdotsCEDelimiter2(int& outputDimension, int LeftDelimiter, int RightDelimiter)
{ if (*this->TokenStack.LastObject()!=RightDelimiter)
    return false;
  outputDimension=0;
  for (int i=this->TokenStack.size-2; i>=1; i--)
  { if (this->TokenStack.TheObjects[i]!=this->tokenExpression)
      return false;
    i--;
    outputDimension++;
    if (this->TokenStack.TheObjects[i]==LeftDelimiter)
      return true;
    if (this->TokenStack.TheObjects[i]!=this->tokenComma)
      return false;
  }
  return false;
}

bool Parser::StackTopIsDelimiter1ECdotsCEDelimiter2EDelimiter3
  (int& outputDimension, int LeftDelimiter, int middleDelimiter, int rightDelimiter)
{ int LastToken=this->TokenStack.TheObjects[this->TokenStack.size-1];
  int SecondToLastToken=this->TokenStack.TheObjects[this->TokenStack.size-2];
  int ThirdToLastToken=this->TokenStack.TheObjects[this->TokenStack.size-3];
  if (LastToken!=rightDelimiter || SecondToLastToken!=this->tokenExpression || ThirdToLastToken!=middleDelimiter)
    return false;
  outputDimension=0;
  for (int i=this->TokenStack.size-4; i>=1; i--)
  { if (this->TokenStack.TheObjects[i]!=this->tokenExpression)
      return false;
    i--;
    outputDimension++;
    if (this->TokenStack.TheObjects[i]==LeftDelimiter)
      return true;
    if (this->TokenStack.TheObjects[i]!=this->tokenComma)
      return false;
  }
  return false;
}

void HomomorphismSemisimpleLieAlgebra::GetWeightsWrtKInSimpleCoordsK
(Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra<Rational> >& inputElts,
 GlobalVariables& theGlobalVariables)
{ outputWeights.SetSize(inputElts.size);
  Rational tempRat;
  ElementSemisimpleLieAlgebra<Rational> tempLieElement;
  for (int i=0; i<inputElts.size; i++)
  { Vector<Rational>& currentWeight= outputWeights[i];
    currentWeight.MakeZero(this->theDomain().GetRank());
    ElementSemisimpleLieAlgebra<Rational>& currentLieElt=inputElts[i];
    for (int j=0; j<this->theDomain().GetRank(); j++)
    { this->theRange().LieBracket(this->imagesAllChevalleyGenerators[j+this->theDomain().GetNumPosRoots()], currentLieElt, tempLieElement);
      if(!currentLieElt.IsProportionalTo(tempLieElement, tempRat)){ assert(false);}
      currentWeight[j]=tempRat;
    }
  }
  Matrix<Rational>  tempMat=this->theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert();
  tempMat.ActOnVectorsColumn(outputWeights);
}

template <class CoefficientType>
void ElementSemisimpleLieAlgebra<CoefficientType>::GetBasisFromSpanOfElements
(List<ElementSemisimpleLieAlgebra>& theElements, Vectors<RationalFunctionOld>& outputCoords, List<ElementSemisimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> theRootForm;
  theRootForm.SetSize(theElements.size);
  for(int i=0; i<theElements.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=theElements.TheObjects[i];
    currentElt.ElementToVectorNegativeRootSpacesFirst(theRootForm.TheObjects[i]);
  }
//  int theRank=0; int numRoots=0;
//  if (theElements.size>0)
//  { theRank=theElements.TheObjects[0].Hcomponent.size;
//    numRoots=theElements.TheObjects[0].coeffsRootSpaces.size;
//  }
  theRootForm.ChooseABasis(theGlobalVariables);
  outputTheBasis.SetSize(theRootForm.size);
  for(int i=0; i<theRootForm.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=outputTheBasis.TheObjects[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces
    (theRootForm.TheObjects[i], currentElt.ownerArray->TheObjects[currentElt.indexOfOwnerAlgebra]);
  }

}

template <class CoefficientType>
bool ElementSemisimpleLieAlgebra<CoefficientType>::GetCoordsInBasis
(const List<ElementSemisimpleLieAlgebra>& theBasis,
 Vector<Rational>& output, GlobalVariables& theGlobalVariables)const
{ Vectors<Rational> tempBasis;
  Vector<Rational> tempRoot;
  tempBasis.SetSize(theBasis.size);
  for (int i=0 ; i<theBasis.size; i++)
    theBasis.TheObjects[i].ElementToVectorNegativeRootSpacesFirst(tempBasis.TheObjects[i]);
  this->ElementToVectorNegativeRootSpacesFirst(tempRoot);
  return tempRoot.GetCoordsInBasiS(tempBasis, output, (Rational) 1, (Rational) 0);
}

bool RationalFunctionOld::gcdQuicK
(const Polynomial<Rational>& left, const Polynomial<Rational> & right, Polynomial<Rational> & output)
{ if (left.TotalDegree()>1 && right.TotalDegree()>1)
    return false;
  Polynomial<Rational> quotient, remainder;
  if (left.TotalDegree()>right.TotalDegree())
  { left.DivideBy(right, quotient, remainder);
    if (remainder.IsEqualToZero())
      output=right;
    else
      output.MakeOne(left.GetMinNumVars());
  } else
  { //std::string tempS1, tempS2, tempS3, tempS4;
//    tempS1=left.ToString(theGlobalVariables.theDefaultPolyFormat);
//    tempS2=right.ToString(theGlobalVariables.theDefaultPolyFormat);
    right.DivideBy(left, quotient, remainder);
//    tempS3=quotient.ToString();
//    tempS4=remainder.ToString();
    if (remainder.IsEqualToZero())
      output=left;
    else
      output.MakeOne(left.GetMinNumVars());
  }
  return true;
}

void ParserNode::EvaluateDivide(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  if (this->children.size!=2)
  { this->SetError(this->errorProgramming);
    return;
  }
  this->impliedNumVars=this->GetMaxImpliedNumVarsChildren();
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typePoly:
    case ParserNode::typeRationalFunction:
      this->ExpressionType=this->typeRationalFunction;
//      this->InitForMultiplication(&theGlobalVariables);
      this->ratFunction.GetElement()=leftNode.ratFunction.GetElementConst();
      this->ratFunction.GetElement()/=rightNode.ratFunction.GetElement();
      this->ExpressionType=this->typeRationalFunction;
      this->ReduceRatFunction();
      break;
    case ParserNode::typeUEelement:
      this->UEElement.GetElement()=leftNode.UEElement.GetElement();
      this->UEElement.GetElement()/=rightNode.rationalValue;
      this->ExpressionType=this->typeUEelement;
      break;
    case ParserNode::typeUEElementOrdered:
      this->UEElementOrdered.GetElement()=leftNode.UEElementOrdered.GetElement();
      this->UEElementOrdered.GetElement()/=rightNode.rationalValue;
      this->ExpressionType=this->typeUEElementOrdered;
      break;
    break;
    default: this->SetError(this->errorDivisionByNonAllowedType); return;
  }
}

void RationalFunctionOld::RaiseToPower(int thePower)
{ MacroRegisterFunctionWithName("RationalFunctionOld::RaiseToPower");
  Polynomial<Rational> theNum, theDen;
  this->checkConsistency();
  if (thePower<0)
  { this->Invert();
    thePower=-thePower;
  }
  if (thePower==0)
  { if (this->IsEqualToZero())
    { std::cout << "This is a programming error: attempting to raise 0 to the 0th power, which is undefined. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->MakeOne(this->context);
    return;
  }
  switch (this->expressionType)
  { case RationalFunctionOld::typeRational:
      this->ratValue.RaiseToPower(thePower);
      break;
    case RationalFunctionOld::typePoly:
      this->Numerator.GetElement().RaiseToPower(thePower, (Rational) 1);
      break;
    case RationalFunctionOld::typeRationalFunction:
      this->Numerator.GetElement().RaiseToPower(thePower, (Rational) 1);
      this->Denominator.GetElement().RaiseToPower(thePower, (Rational) 1);
      break;
  }
  this->checkConsistency();
}

void RationalFunctionOld::ClearDenominators
(RationalFunctionOld& outputWasMultipliedBy)
{ //outputWasMultipliedBy.MakeConst(this->NumVars, (Rational) 1, this->context);
  Rational tempRat;
  switch(this->expressionType)
  { case RationalFunctionOld::typeRational:
      this->ratValue.GetDenominator(tempRat);
      outputWasMultipliedBy.MakeConsT(tempRat, this->context);
      this->ratValue*=tempRat;
    break;
    case RationalFunctionOld::typePoly:
      this->Numerator.GetElement().ClearDenominators(tempRat);
      outputWasMultipliedBy.MakeConsT(tempRat, this->context);
    break;
    case RationalFunctionOld::typeRationalFunction:
      RationalFunctionOld tempRF;
      outputWasMultipliedBy.operator=(this->Denominator.GetElement());
      *this*=outputWasMultipliedBy;
      this->ClearDenominators(tempRF);
      outputWasMultipliedBy*=tempRF;
      break;
  }
}

template <class CoefficientType>
bool ElementSemisimpleLieAlgebra<CoefficientType>::NeedsBrackets()const
{ return this->size>1;
}

void slTwoInSlN::ClimbDownFromHighestWeightAlongSl2String
  (Matrix<Rational>& input, Matrix<Rational>& output, Rational& outputCoeff, int generatorPower)
{ assert(&input!=&output);
  Rational currentWeight;
  Matrix<Rational> ::LieBracket(this->theH, input, output);
  bool tempBool=input.IsProportionalTo(output, currentWeight);
  if (!tempBool)
    std::cout << "<br>oh no! climbing down is fucked up!";
  Rational RaiseCoeff;
  RaiseCoeff.MakeZero();
  outputCoeff.MakeOne();
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    Matrix<Rational> ::LieBracket(this->theF, output, output);
  }
}

std::string slTwoInSlN::ElementModuleIndexToString(int input, bool useHtml)
{ std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  Matrix<Rational> & currentHW=this->theHighestWeightVectors.TheObjects[input];
  int currentEtaHw=this->theGmodKModules.TheObjects[input].size-1;
  //currentEtaHw-=currentEtaHw/2;
  int firstNonZeroRow=-1, firstNonZeroColumn=-1;
  bool found=false;
  for (int i=0; i<currentHW.NumRows; i++)
  { if (found)
      break;
    for (int j=0; j<currentHW.NumCols; j++)
      if (!currentHW.elements[i][j].IsEqualToZero())
      { firstNonZeroColumn=j;
        firstNonZeroRow=i;
        found =true;
        break;
      }
  }
  int sRow, kRow, sColumn, kColumn;
  this->GetIsPlusKIndexingFrom(firstNonZeroRow, sRow, kRow);
  this->GetIsPlusKIndexingFrom(firstNonZeroColumn, sColumn, kColumn);
  std::stringstream out;
  out << "V_{";
  if (currentEtaHw!=0)
  { if (currentEtaHw!=1)
      out << currentEtaHw;
    out << "\\frac\\eta 2";
  }
  if (sRow!=sColumn)
    out << "-\\zeta_" << sColumn << "+\\zeta_" << sRow;
  out << "}";
  return out.str();
}

void slTwoInSlN::GetIsPlusKIndexingFrom(int input, int& s, int& k)
{ s=0;
  k=input;
  if (input >=this->theDimension || input <0)
    return;
  for (int offset=0; offset<=input; offset+=this->thePartition.TheObjects[s-1])
  { k=input-offset;
    s++;
  }
}

std::string slTwoInSlN::ElementMatrixToTensorString(const Matrix<Rational> & input, bool useHtml)
{ std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  std::stringstream out;
  std::string tempS;
  bool found=false;
  for (int i=0; i<input.NumRows; i++)
    for(int j=0; j<input.NumCols; j++)
      if (!input.elements[i][j].IsEqualToZero())
      { tempS=input.elements[i][j].ToString();
        if (tempS=="-1")
          tempS="-";
        if (tempS=="1")
        { tempS="";
          if (found)
            out << "+";
        } else
          if (found)
            if (tempS[0]!='-')
              out << "+";
        found=true;
        out << tempS;
        int sI, kI, sJ, kJ;
        this->GetIsPlusKIndexingFrom(i, sI, kI);
        this->GetIsPlusKIndexingFrom(j, sJ, kJ);
        out << "v_{i_{" << sI << "}";
        if (kI!=0)
          out <<"+" << kI;
        out << "}\\otimes v^*_{i_{" << sJ << "}";
        if (kJ!=0)
          out  << "+" << kJ;
        out << "}";
      }
  return out.str();
}

void slTwoInSlN::ExtractHighestWeightVectorsFromVector
  (Matrix<Rational> & input, List<Matrix<Rational> >& outputDecompositionOfInput, List<Matrix<Rational> >& outputTheHWVectors)
{ outputDecompositionOfInput.size=0; outputTheHWVectors.size=0;
  Matrix<Rational>  remainder; remainder=input;
  Matrix<Rational>  component, highestWeightVector, tempMat;
  Rational theCoeff, tempRat;
  int largestPowerNotKillingInput;
  while(!remainder.IsEqualToZero() )
  { //std::cout << "<br>remainder:<div class=\"math\">" << remainder.ToString(false, true) << "</div>";
    this->ClimbUpFromVector(remainder, highestWeightVector, largestPowerNotKillingInput);
    //std::cout << "<br>highest weight vector:<div class=\"math\">" << highestWeightVector.ToString(false, true) << "</div>";
    this->ClimbDownFromHighestWeightAlongSl2String(highestWeightVector, component, theCoeff, largestPowerNotKillingInput);
    for (int i=0; i<this->theProjectors.size; i++)
    { Matrix<Rational> & currentProjector=this->theProjectors.TheObjects[i];
      tempMat=highestWeightVector;
      tempMat.MultiplyOnTheLeft(currentProjector);
      if (!tempMat.IsEqualToZero())
      { tempMat.FindFirstNonZeroElementSearchEntireRow(tempRat);
        tempMat/=tempRat;
        outputTheHWVectors.AddOnTop(tempMat);
      }
    }
    //assert(!theCoeff.IsEqualToZero());
    component/=(theCoeff);
    outputDecompositionOfInput.AddOnTop(component);
    //std::cout << "<br>component:<div class=\"math\">" << component.ToString(false, true) << "</div><br><br><br><br>";
    remainder-=(component);
  }
  //remainder.NullifyAll();
//  for (int i=0; i<outputVectors.size; i++)
//    remainder.Add(outputVectors.TheObjects[i]);
//  std::cout << "<br>sum of all components:<div class=\"math\">" << remainder.ToString(false, true) << "</div>";

}

void slTwoInSlN::ClimbUpFromVector
  (Matrix<Rational> & input, Matrix<Rational> & outputLastNonZero, int& largestPowerNotKillingInput)
{ Matrix<Rational>  tempMat;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  largestPowerNotKillingInput=0;
  for(Matrix<Rational> ::LieBracket(this->theE, outputLastNonZero, tempMat); !tempMat.IsEqualToZero(); Matrix<Rational> ::LieBracket(this->theE, outputLastNonZero, tempMat))
  { largestPowerNotKillingInput++;
    outputLastNonZero=tempMat;
  }
}

std::string slTwoInSlN::GetNotationString(bool useHtml)
{ std::stringstream out;
  std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  out << newLine << "Let the starting index of the j-th block be " << beginMath << "i_j" << endMath;
  out << "." << newLine << "In particular let: ";
  int offset=1;
  for (int i=0; i< this->thePartition.size; i++)
  { out << beginMath << "i_" << i+1 << "=" << offset << endMath << " ";
    out << " (size of block = " << this->thePartition.TheObjects[i] << "), ";
    offset+=this->thePartition.TheObjects[i];
  }
  out << newLine << "Let " << beginMath << "\\eta" << endMath << " be the weight corresponding to h.";
  out << newLine << "Let " << beginMath << "\\zeta_{j}" << endMath << " be the weight corresponding to the j-th block";
  return out.str();
}

std::string slTwoInSlN::initFromModuleDecomposition
(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable)
{ std::stringstream out;
  this->thePartition=(decompositionDimensions);
  this->thePartition.QuickSortDescending();
  this->theDimension=0;
  for (int i=0; i<this->thePartition.size; i++)
    this->theDimension+=this->thePartition.TheObjects[i];
  theH.init(this->theDimension, this->theDimension); theH.NullifyAll();
  theE.init(this->theDimension, this->theDimension); theE.NullifyAll();
  theF.init(this->theDimension, this->theDimension); theF.NullifyAll();
  this->theProjectors.SetSize(this->thePartition.size);
  int currentOffset=0;
  std::string beginMath, endMath, newLine;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  for (int i=0; i<this->thePartition.size; i++)
  { this->theProjectors.TheObjects[i].init(this->theDimension, this->theDimension);
    this->theProjectors.TheObjects[i].NullifyAll();
    for (int j=0; j<this->thePartition.TheObjects[i]; j++)
    { theH.elements[currentOffset+j][currentOffset+j]=this->thePartition.TheObjects[i]-1-2*j;
      this->theProjectors.TheObjects[i].elements[currentOffset+j][currentOffset+j]=1;
      if (j!=this->thePartition.TheObjects[i]-1)
      { theF.elements[currentOffset +j+1][currentOffset +j]=1;
        theE.elements[currentOffset +j][currentOffset +j+1]=(j+1)*(this->thePartition.TheObjects[i]-j-1);
      }
    }
    currentOffset+=this->thePartition.TheObjects[i];
  }
  out << newLine << beginMath << "h=" << this->ElementMatrixToTensorString(this->theH, useHtml) << "="
  << this->theH.ElementToStringWithBlocks(this->thePartition) << endMath;
  out << newLine << beginMath << "e=" << this->ElementMatrixToTensorString(this->theE, useHtml) << "="
  << this->theE.ElementToStringWithBlocks(this->thePartition) << endMath;
  out << newLine << beginMath << "f=" << this->ElementMatrixToTensorString(this->theF, useHtml) << "="
  << this->theF.ElementToStringWithBlocks(this->thePartition)  << endMath;
  Matrix<Rational>  tempMat;
  tempMat.init(this->theDimension, this->theDimension);
  List<Matrix<Rational> > Decomposition, theHwCandidatesBeforeProjection, theHwCandidatesProjected;
  this->theHighestWeightVectors.size=0;
  this->theGmodKModules.size=0;
  for (int i=0; i<this->theDimension; i++)
    for (int j=0; j< this->theDimension; j++)
    { tempMat.NullifyAll();
      tempMat.elements[i][j]=1;
      this->ExtractHighestWeightVectorsFromVector(tempMat, Decomposition, theHwCandidatesBeforeProjection);
      theHwCandidatesProjected.size=0;
      for (int k=0; k<theHwCandidatesBeforeProjection.size; k++)
        for (int l=0; l<this->theProjectors.size; l++)
        { tempMat=theHwCandidatesBeforeProjection.TheObjects[k];
          tempMat.MultiplyOnTheLeft(this->theProjectors.TheObjects[l]);
          if (!tempMat.IsEqualToZero())
            theHwCandidatesProjected.AddOnTop(tempMat);
        }
      for (int k=0; k<theHwCandidatesProjected.size; k++)
        if (this->GetModuleIndexFromHighestWeightVector(theHwCandidatesProjected.TheObjects[k])==-1)
        { Matrix<Rational> & currentHighest=theHwCandidatesProjected.TheObjects[k];
          this->theHighestWeightVectors.AddOnTop(currentHighest);
          this->theGmodKModules.ExpandOnTop(1);
          List<Matrix<Rational> >& currentMod=*this->theGmodKModules.LastObject();
          currentMod.size=0;
          for (tempMat=currentHighest; !tempMat.IsEqualToZero(); Matrix<Rational> ::LieBracket(this->theF, tempMat, tempMat))
            currentMod.AddOnTop(tempMat);
        }
    }
  out << this->GetNotationString(useHtml);
  out << newLine << "...and the highest weights of the module decomposition are (" <<
  this->theHighestWeightVectors.size << " modules):";
  for (int i=0; i<this->theHighestWeightVectors.size; i++)
  { out << newLine << beginMath << this->ElementMatrixToTensorString(theHighestWeightVectors.TheObjects[i], useHtml)
    << endMath << ", highest weight of ";
    out << beginMath << this->ElementModuleIndexToString(i, useHtml) << endMath;
   //for (int j=1; j<this->theGmodKModules.TheObjects[i].size; j++)
   //   out << "<br><div class=\"math\">" << this->theGmodKModules.TheObjects[i].TheObjects[j].ToString(false, true) << "</div>";
   // out << "<br><br><br>";
  }
  if (computePairingTable)
    out << this->initPairingTable(useHtml);
  return out.str();
}

std::string slTwoInSlN::initPairingTable(bool useHtml)
{ std::stringstream out;
  this->PairingTable.SetSize(this->theHighestWeightVectors.size);
  for (int i=0; i<this->PairingTable.size; i++)
  { this->PairingTable.TheObjects[i].SetSize(this->theHighestWeightVectors.size);
    for(int j=0; j<this->PairingTable.TheObjects[i].size; j++)
    { List<int>& currentPairing=this->PairingTable.TheObjects[i].TheObjects[j];
      out << this->PairTwoIndices(currentPairing, i, j, useHtml);
    }
  }
  return out.str();
}

std::string slTwoInSlN::PairTwoIndices
  (List<int>& output, int leftIndex, int rightIndex, bool useHtml)
{ std::string beginMath, endMath, newLine;
  FormatExpressions latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
  if (useHtml)
  { beginMath= "<span class=\"math\">";
    endMath= "</span>";
    newLine="<br>";
  } else
  { beginMath="$";
    endMath="$";
    newLine="\n\n\n";
  }
  std::stringstream out;
  output.size=0;
  List<Matrix<Rational> >& leftElements=this->theGmodKModules.TheObjects[leftIndex];
  List<Matrix<Rational> >& rightElements=this->theGmodKModules.TheObjects[rightIndex];
  Matrix<Rational>  tempMat;
  List<Matrix<Rational> > HighestWeightsContainingModules;
  List<Matrix<Rational> > tempDecomposition;
  for (int i=0; i<leftElements.size; i++)
    for (int j=0; j<rightElements.size; j++)
    { Matrix<Rational>& leftElt=leftElements.TheObjects[i];
      Matrix<Rational>& rightElt=rightElements.TheObjects[j];
      Matrix<Rational>::LieBracket(leftElt, rightElt, tempMat);
      if (!tempMat.IsEqualToZero())
      { this->ExtractHighestWeightVectorsFromVector(tempMat, tempDecomposition, HighestWeightsContainingModules);
        for (int k=0; k<HighestWeightsContainingModules.size; k++)
        { output.AddOnTopNoRepetition(this->GetModuleIndexFromHighestWeightVector(HighestWeightsContainingModules.TheObjects[k]));
          if (this->GetModuleIndexFromHighestWeightVector(HighestWeightsContainingModules.TheObjects[k])==-1)
            std::cout << newLine << beginMath << "[" << leftElt.ToString(&latexFormat) << ", "
            << rightElt.ToString(&latexFormat) << "]=" << tempMat.ToString(&latexFormat) << endMath;
        }
      }
    }
  out << newLine << beginMath << this->ElementModuleIndexToString(leftIndex, useHtml) << endMath << " and "
  << beginMath << this->ElementModuleIndexToString(rightIndex, useHtml) << endMath << " pair to: ";
  for (int i=0; i<output.size; i++)
  { out << beginMath << this->ElementModuleIndexToString(output.TheObjects[i], useHtml) << endMath;
    if (i!=output.size-1)
      out << beginMath << "\\oplus" << endMath;
  }
  if (output.size>0)
  { out << "  hw vectors: ";
    for (int i=0; i<output.size; i++)
      out << beginMath << this->ElementMatrixToTensorString(this->theHighestWeightVectors.TheObjects[output.TheObjects[i]], useHtml) << endMath << ",";
  }
  return out.str();
}

int ParserNode::EvaluateSlTwoInSlN
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<int> thePartition;
  thePartition.SetSize(theArgumentList.size);
  for (int i=0; i<theArgumentList.size; i++)
  { thePartition.TheObjects[i]=theNode.owner->TheObjects[theArgumentList.TheObjects[i]].intValue;
//    std::cout<< thePartition.TheObjects[i] << ",";
  }
  slTwoInSlN theSl2;
  std::stringstream tempStream;
  tempStream << "\\documentclass{article} \\begin{document}\n"
  << theSl2.initFromModuleDecomposition(thePartition, false, true) << "\n\\end{document}";
  theNode.CreateDefaultLatexAndPDFfromString(tempStream.str());
  return theNode.errorNoError;
}

void RationalFunctionOld::AddHonestRF(const RationalFunctionOld& other)
{ MacroRegisterFunctionWithName("RationalFunctionOld::AddHonestRF");
  Rational tempRat;
  if (!this->Denominator.GetElement().IsProportionalTo(other.Denominator.GetElementConst(), tempRat, (Rational) 1))
  { Polynomial<Rational> buffer;
//    RationalFunctionOld debugger;
//    debugger=other;
//    debugger.ComputeDebugString();
//    this->ComputeDebugString();
    buffer=this->Denominator.GetElement();
    this->Numerator.GetElement()*=(other.Denominator.GetElementConst());
    buffer*=(other.Numerator.GetElementConst());
    this->Numerator.GetElement()+=(buffer);
    this->Denominator.GetElement()*=(other.Denominator.GetElementConst());
    assert(!this->Denominator.GetElement().IsEqualToZero());
    this->Simplify();
//    this->ComputeDebugString();
  } else
  { this->Numerator.GetElement()*=(tempRat);
    this->Denominator.GetElement()*=(tempRat);
    this->Numerator.GetElement()+=(other.Numerator.GetElementConst());
    this->ReduceMemory();
    this->SimplifyLeadingCoefficientOnly();
  }
  assert(this->checkConsistency());
}

int ParserNode::EvaluatePlus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
    return this->SetError(this->errorOperationByUndefinedOrErrorType);
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren(theGlobalVariables);
//  this->InitForAddition(&theGlobalVariables);
  LargeInt theInt;
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex:
        theInt=this->intValue;
        theInt+=currentChild.intValue;
        if (theInt.value.size>1)
        { this->ExpressionType= this->typeRational;
          this->rationalValue=theInt;
        } else
          this->intValue=theInt.value.TheObjects[0]*theInt.sign;
      break;
      case ParserNode::typeRational: this->rationalValue+=currentChild.rationalValue; break;
      case ParserNode::typeRationalFunction: this->ratFunction.GetElement()+=currentChild.ratFunction.GetElement(); break;
      case ParserNode::typePoly: this->polyValue.GetElement()+=(currentChild.polyValue.GetElement()); break;
      case ParserNode::typeUEElementOrdered: this->UEElementOrdered.GetElement().operator+=(currentChild.UEElementOrdered.GetElement()); break;
      case ParserNode::typeUEelement: this->UEElement.GetElement()+=currentChild.UEElement.GetElement(); break;
      case ParserNode::typeCharSSFDMod:
        this->theChar.GetElement()+=currentChild.theChar.GetElement();
        break;
      case ParserNode::typeLattice:
        this->theLattice.GetElement()=currentChild.theLattice.GetElement();
        this->theLattice.GetElement().RefineByOtherLattice(this->owner->TheObjects[this->children.TheObjects[1]].theLattice.GetElement());
        this->outputString=this->theLattice.GetElement().ToString();
        return this->errorNoError;
      case ParserNode::typeQuasiPolynomial: this->theQP.GetElement()+=currentChild.theQP.GetElement();
        this->outputString=this->theQP.GetElement().ToString(true, false);
        break;
      case ParserNode::typeAnimation:
        this->theAnimation.GetElement()+=currentChild.theAnimation.GetElement();
        break;
      case ParserNode::typeGenVermaElt:
        this->theGenVermaElt.GetElement()+=currentChild.theGenVermaElt.GetElement();
        break;
      default: return this->SetError(this->errorDunnoHowToDoOperation);
    }
  }
  return this->errorNoError;
}

void CGI::CivilizedStringTranslationFromCGI(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (CGI::AttemptToCivilize(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

void CGI::ChopCGIInputStringToMultipleStrings
(const std::string& input, List<std::string>& outputData, List<std::string>& outputFieldNames)
{ int inputLength= (signed) input.size();
  bool readingData=false;
  outputData.ReservE(10);
  outputFieldNames.ReservE(10);
  outputData.SetSize(1);
  outputFieldNames.SetSize(1);
  outputData[0]="";
  outputFieldNames[0]="";
  for (int i=0; i<inputLength; i++)
  { if (input[i]=='=')
      readingData=!readingData;
    if (input[i]=='&')
    { outputData.SetSize(outputData.size+1);
      outputFieldNames.SetSize(outputData.size);
      outputData.LastObject()->reserve(1000);
      outputFieldNames.LastObject()->reserve(20);
      *outputFieldNames.LastObject()="";
      *outputData.LastObject()="";
      readingData=false;
    }
    if (input[i]!='=' && input[i]!='&')
    { if (readingData)
        outputData.LastObject()->push_back(input[i]);
      else
        outputFieldNames.LastObject()->push_back(input[i]);
    }
  }
}

bool CGI::AttemptToCivilize(std::string& readAhead, std::stringstream& out)
{ if (readAhead[0]!='%' && readAhead[0]!='&' && readAhead[0]!='+')
  { out << readAhead[0];
    return true;
  }
  if (readAhead=="&")
  { out << " ";
    return true;
  }
  if (readAhead=="+")
  { out << " ";
    return true;
  }
  if (readAhead=="%2B")
  { out << "+";
    return true;
  }
  if (readAhead=="%28")
  { out << "(";
    return true;
  }
  if (readAhead=="%29")
  { out << ")";
    return true;
  }
  if (readAhead=="%5B")
  { out << "[";
    return true;
  }
  if (readAhead=="%5D")
  { out << "]";
    return true;
  }
  if (readAhead=="%2C")
  { out << ",";
    return true;
  }
  if (readAhead=="%7B")
  { out << "{";
    return true;
  }
  if (readAhead=="%27")
  { out << "'";
    return true;
  }
  if (readAhead=="%3B")
  { out << ";";
    return true;
  }
  if (readAhead=="%3C")
  { out << "<";
    return true;
  }
  if (readAhead=="%3E")
  { out << ">";
    return true;
  }
  if (readAhead=="%2F")
  { out << "/";
    return true;
  }
  if (readAhead=="%3A")
  { out << ":";
    return true;
  }
  if (readAhead=="%5E")
  { out << "^";
    return true;
  }
  if (readAhead=="%5C")
  { out << "\\";
    return true;
  }
  if (readAhead=="%26")
  { out << "&";
    return true;
  }
  if (readAhead=="%3D")
  { out << "=";
    return true;
  }
  if (readAhead=="%7D")
  { out << "}";
    return true;
  }
  if (readAhead=="%0D%0A")
  { out << "\n";
    return true;
  }
  if (readAhead=="%25")
  { out << "%";
    return true;
  }
  return false;
}

void MonomialP::MakeEi(int LetterIndex, int Power, int ExpectedNumVars)
{ ExpectedNumVars=MathRoutines::Maximum(ExpectedNumVars, LetterIndex+1);
  this->MakeOne(ExpectedNumVars);
  (*this)[LetterIndex]=Power;
}

void MonomialP::ExponentMeBy(const Rational& theExp)
{ for (int i=0; i<this->monBody.size; i++)
    this->monBody[i]*=theExp;
}

bool MonomialP::operator>(const MonomialP& other)const
{ if (this->monBody==other.monBody)
    return false;
  return this->IsGEQTotalDegThenLexicographicLastVariableStrongest(other);
}

bool MonomialP::IsDivisibleBy(const MonomialP& other)const
{ for (int i=other.monBody.size-1; i>=this->monBody.size; i--)
    if (other.monBody[i]>0)
      return false;
  int upperLimit=MathRoutines::Minimum(this->monBody.size, other.monBody.size);
  for (int i=0; i<upperLimit; i++)
    if (this->monBody[i]<other.monBody[i])
      return false;
  return true;
}

bool MonomialP::operator==(const MonomialP& other)const
{ for (int i=other.monBody.size-1; i>=this->monBody.size; i--)
    if (other.monBody[i]!=0)
      return false;
  for (int i=this->monBody.size-1; i>=other.monBody.size; i--)
    if (this->monBody[i]!=0)
      return false;
  int highestIndex=MathRoutines::Minimum(this->GetMinNumVars(), other.GetMinNumVars())-1;
  for (int i=highestIndex; i>=0; i--)
    if (this->monBody[i]!=other.monBody[i])
      return false;
  return true;
}

bool MonomialP::IsGEQLexicographicLastVariableStrongest(const MonomialP& other)const
{ for (int i=other.monBody.size-1; i>=this->monBody.size; i--)
  { if (other.monBody[i]>0)
      return false;
    if (other.monBody[i]<0)
      return true;
  }
  for (int i=this->monBody.size-1; i>=other.monBody.size; i--)
  { if (this->monBody[i]>0)
      return true;
    if (this->monBody[i]<0)
      return false;
  }
  int highestIndex=MathRoutines::Minimum(this->GetMinNumVars(), other.GetMinNumVars())-1;
  for (int i=highestIndex; i>=0; i--)
  { if (this->monBody[i]>other.monBody[i])
      return true;
    if (this->monBody[i]<other.monBody[i])
      return false;
  }
  return true;
}

bool MonomialP::IsGEQLexicographicLastVariableWeakest(const MonomialP& other)const
{ int commonSize=MathRoutines::Minimum(this->GetMinNumVars(), other.GetMinNumVars());
  for (int i=0; i<commonSize; i++)
  { if (this->monBody[i]>other.monBody[i])
      return true;
    if (this->monBody[i]<other.monBody[i])
      return false;
  }
  for (int i=this->monBody.size; i<other.monBody.size; i++)
  { if (other.monBody[i]>0)
      return false;
    if (other.monBody[i]<0)
      return true;
  }
  for (int i=other.monBody.size; i<this->monBody.size; i++)
  { if (this->monBody[i]>0)
      return true;
    if (this->monBody[i]<0)
      return false;
  }
  return true;
}

bool MonomialP::IsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& other)const
{ if (this->TotalDegree()>other.TotalDegree())
    return true;
  if (this->TotalDegree()<other.TotalDegree())
    return false;
  return this->IsGEQLexicographicLastVariableStrongest(other);
}

bool MonomialP::IsGEQpartialOrder(MonomialP& m)
{ if (this->monBody.size!=m.monBody.size)
  { std::cout << "This is a programming error: comparing two monomials with different number of variables. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  for (int i=0; i<m.monBody.size; i++)
    if ((*this)[i]<m[i])
      return false;
  return true;
}

void MonomialP::operator*=(const MonomialP& other)
{ this->SetNumVariablesSubDeletedVarsByOne
  (MathRoutines::Maximum(this->monBody.size, other.monBody.size));
  for (int i=0; i<other.monBody.size; i++)
    this->monBody[i]+=other.monBody[i];
}

void MonomialP::operator/=(const MonomialP& other)
{ this->SetNumVariablesSubDeletedVarsByOne
  (MathRoutines::Maximum(this->monBody.size, other.monBody.size));
  for (int i=0; i<other.monBody.size; i++)
    this->monBody[i]-=other.monBody[i];
}

void MonomialP::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ if (newNumVars<0)
    newNumVars=0;
  int oldSize=this->monBody.size;
  this->monBody.SetSize(newNumVars);
  for(int i=oldSize; i<this->monBody.size; i++)
    (*this)[i]=0;
}

bool Cone::IsInCone(const Vector<Rational>& point) const
{ if (this->flagIsTheZeroCone)
    return point.IsEqualToZero();
  Rational tempRat;
  for (int i=0; i<this->Normals.size; i++)
  { tempRat=Vector<Rational>::ScalarEuclidean(this->Normals[i], point);
    if (tempRat.IsNegative())
      return false;
  }
  return true;
}

void MathRoutines::NChooseK(int n, int k, LargeInt& result)
{ result=1;
  for (int i=0; i<k; i++)
  { result*=(n-i);
    result/=(i+1);
  }
}

std::string MonomialP::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  MemorySaving<FormatExpressions> tempFormat;
  if (theFormat==0)
    theFormat=&tempFormat.GetElement();
  if (this->IsAConstant())
    return "1";
  for (int i=0; i<this->monBody.size; i++)
    if (!(this->monBody[i].IsEqualToZero()))
    { out << theFormat->GetPolyLetter(i);
      if (!(this->monBody[i]==1))
        out << "^{" << this->monBody[i] << "}";
    }
  return out.str();
}
