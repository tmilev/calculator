#include "vpf.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
#include "vpfHeader2Math4_Graph.h"
#include "vpfHeader3Calculator3_WeylGroupCharacters.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceCPP(__FILE__, "Weyl group calculator interface. Work in progress by Thomas & Todor. ");

template<>
typename List<ClassFunction<WeylGroup::WeylGroupBase, Rational> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<ClassFunction<WeylGroup::WeylGroupBase, Rational> >()
{ return 0;
}

bool WeylGroup::CheckConsistency()const
{ if (this->flagDeallocated)
    crash << "This is a programming error: use after free of WeylGroup. " << crash;
  for (int i=0; i<this->generators.size; i++)
    this->generators[i].CheckConsistency();
  this->RootsOfBorel.CheckConsistency();
  return true;
}

bool WeylGroup::CheckInitializationFDrepComputation()const
{ if (this->theElements.size==0)
  { crash << "This is a programming error: requesting to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->ConjugacyClassCount() << " conjugacy classes and " << this->theElements.size << " elements. "
    << crash;
    return false;
  }
  return this->CheckInitializationConjugacyClasses();
}

bool WeylGroup::CheckInitializationConjugacyClasses()const
{ if (this->ConjugacyClassCount()==0)
  { crash << "This is a programming error: requesting to compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->ConjugacyClassCount() << " conjugacy classes and " << this->theElements.size << " elements. "
    << crash;
    return false;
  }
  for (int i=0; i<this->irreps.size; i++)
    if (this->irreps[i].theCharacteR.data.IsEqualToZero())
    { crash << "This is a programming error: irrep number " << i+1 << " has zero character!" << crash;
      return false;
    }
/*  Rational sumSquares=0;
  Rational tempRat;
  for (int i=0; i<this->ConjugacyClassCount(); i++)
  { tempRat=this->ConjugacyClassCount()/this->GetGroupSizeByFormula();
    sumSquares+=tempRat*tempRat;
  }
  if (sumSquares!=1)
    crash << "This is a programming error: sumSquares equals " << sumSquares.ToString() << " when it should equal 1. " << crash;*/
  return true;
}

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::CheckAllSimpleGensAreOK()const
{ this->CheckInitialization();
  for (int i=0; i<this->ownerGroup->GetDim(); i++)
    if (this->generatorS[i].NumRows==0)
    { crash << "This is a programming error: working with a representation in which the action of the simple generators is not computed. " << crash;
      return false;
    }
  return true;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::CheckRepIsMultiplicativelyClosed()
{ HashedList<Matrix<Rational> > tempList;
  tempList.AddOnTop(this->theElementImages);
  Matrix<Rational> tempMat;
  ElementWeylGroup<WeylGroup> tempElt;
  for (int i=0; i<tempList.size; i++)
    for (int j=0; j<tempList.size; j++)
    { tempMat=tempList[i];
      tempMat.MultiplyOnTheLeft(tempList[j]);
      tempElt=this->ownerGroup->theElements[j];
      tempElt*=this->ownerGroup->theElements[i];
      tempElt.MakeCanonical();
      int targetIndex=this->ownerGroup->theElements.GetIndex(tempElt);
      if (!(tempMat==this->theElementImages[targetIndex]))
        crash << "this is a programming error: element " << i+1 << " times element "<< j+1 << " is outside of the set, i.e.,  "
        << tempList[i].ToString() << " * " << tempList[j].ToString() << " is bad. " << crash;
    }
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::ComputeAllGeneratorImagesFromSimple(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::ComputeAllGeneratorImagesFromSimple");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  HashedList<ElementWeylGroup<WeylGroup> > ElementsExplored;
  ElementsExplored.SetExpectedSize(this->ownerGroup->theElements.size);
  this->theElementImageS[0].MakeIdMatrix(this->GetDim());
  ElementWeylGroup<WeylGroup> currentElt;
  int theRank=this->ownerGroup->GetDim();
  currentElt.MakeID(*this->ownerGroup);
  ElementsExplored.AddOnTop(currentElt);
  List<ElementWeylGroup<WeylGroup> > theGens;
  theGens.SetSize(theRank);
  for (int i=0; i<theRank; i++)
    theGens[i].MakeSimpleReflection(i, *this->ownerGroup);
  for (int i=0; i<ElementsExplored.size; i++)
  { int indexParentElement=this->ownerGroup->theElements.GetIndex(ElementsExplored[i]);
    for (int j=0; j<theRank; j++)
    { currentElt=theGens[j]* ElementsExplored[i];
      if (!ElementsExplored.Contains(currentElt))
      { int indexCurrentElt=this->ownerGroup->theElements.GetIndex(currentElt);
        this->theElementIsComputed[indexCurrentElt]=true;
        this->theElementImageS[indexParentElement].MultiplyOnTheLeft(this->generatorS[j], this->theElementImageS[indexCurrentElt]);
        ElementsExplored.AddOnTop(currentElt);
      }
    }
  }
//  this->CheckRepIsMultiplicativelyClosed();
  //std::cout << "<hr>";
  //FormatExpressions tempFormat;
  //tempFormat.flagUseLatex=true;
  //for (int i=0; i<this->theElementImages.size; i++)
  //{ std::cout << "<br>Element  " << i+1 << ": " << this->theElementImages[i].ToString() << " = "
  //  << this->theElementImages[i].ToString(& tempFormat);
  //}
  //std::cout << "<hr>";
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::operator*=(const WeylGroupRepresentation<coefficient>& other)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::operator*=");
  //lazy programmers handling://////
  if (&other==this )
  { WeylGroupRepresentation<coefficient> otherCopy;
    otherCopy=other;
    *this*=otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->ownerGroup!=other.ownerGroup)
    crash << "This is a programming error: attempting to multiply representations with different owner groups. " << crash;
  if (!this->flagCharacterIsComputed || !other.flagCharacterIsComputed)
    crash << "Attempting to multiply weyl group reps whose characters have not been computed. " << crash;
  WeylGroupRepresentation<coefficient> output;
  output.init(*this->ownerGroup);
  output.theCharacteR=this->theCharacteR;
  output.theCharacteR*=other.theCharacteR;
  for (int i=0; i<output.generatorS.size; i++)
    output.generatorS[i].AssignTensorProduct(this->generatorS[i], other.generatorS[i]);
  for(int i=0; i<output.theElementImageS.size; i++)
    if (this->theElementIsComputed[i] && other.theElementIsComputed[i])
    { output.theElementImageS[i].AssignTensorProduct(this->theElementImageS[i],other.theElementImageS[i]);
      output.theElementIsComputed[i]=true;
    }
  *this=output;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::Restrict
(const Vectors<coefficient>& VectorSpaceBasisSubrep, const ClassFunction<WeylGroup::WeylGroupBase, Rational>& remainingCharacter,
 WeylGroupRepresentation<coefficient>& output, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::Restrict");
  this->CheckAllSimpleGensAreOK();
  if (VectorSpaceBasisSubrep.size==0)
    crash << "This is a programming error: restriction of representation to a zero subspace is not allowed. " << crash;
  output.init(*this->ownerGroup);
  output.basis = VectorSpaceBasisSubrep;
  output.basis.GetGramMatrix(output.gramMatrixInverted, 0);
  output.gramMatrixInverted.Invert();
  output.theCharacteR=remainingCharacter;
  ProgressReport theReport(theGlobalVariables);
  for(int i=0; i<this->generatorS.size; i++)
  { if (theGlobalVariables!=0)
    { std::stringstream reportStream;
      reportStream << "Restricting the action of generator of index " << i;
      theReport.Report(reportStream.str());
    }
    Matrix<coefficient>::MatrixInBasis(this->generatorS[i], output.generatorS[i], output.basis, output.gramMatrixInverted);
  }
  /*
  for (int i=0; i<this->classFunctionMatrices.size; i++)
    if (this->classFunctionMatricesComputed[i])
    { output.classFunctionMatricesComputed[i]=true;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Restricting class function matrix " << i+1 << " out of "
        << this->classFunctionMatrices.size;
        theReport.Report(reportStream.str());
      }
      Matrix<coefficient>::MatrixInBasis
      (this->classFunctionMatrices[i], output.classFunctionMatrices[i], output.vectorSpaceBasis,
       output.gramMatrixInverted);
    }
  */
  output.CheckAllSimpleGensAreOK();
}

template <class coefficient>
coefficient WeylGroupRepresentation<coefficient>::GetNumberOfComponents()
{ return this->GetCharacter().Norm();
}

template <class coefficient>
bool WeylGroupRepresentation<coefficient>::DecomposeTodorsVersion(WeylGroupVirtualRepresentation<coefficient>& outputIrrepMults, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeTodorsVersion");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  outputIrrepMults.MakeZero();
  return this->DecomposeTodorsVersionRecursive(outputIrrepMults, theGlobalVariables);
}

template <class Element>
bool Matrix<Element>::GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(List<Vectors<Element> >& output)const
{ int upperLimitComputations = 100000;
  output.SetSize(0);
  int found = 0;
  Polynomial<Element> theMinPoly;
  theMinPoly.AssignMinPoly(*this);
  Vector<Element> theEigenValueCandidate;
  theEigenValueCandidate.SetSize(1);
  Matrix<Rational> tempMat;
  for(int ii=0; ii<upperLimitComputations; ii++)
  { int i = ((ii+1)/2) * (2*(ii%2)-1); // 0,1,-1,2,-2,3,-3,...
//    std::cout << "checking " << i << " found " << found << std::endl;
    theEigenValueCandidate[0] = i;
    if(theMinPoly.Evaluate(theEigenValueCandidate) == 0)
    { tempMat = *this;
//      std::cout << "<hr>The min poly is: " << theMinPoly.ToString() << " and evaluates at "
//      << theEigenValueCandidate << " to " << theMinPoly.Evaluate(theEigenValueCandidate).ToString();
      output.SetSize(output.size+1);
      tempMat.GetEigenspaceModifyMe(theEigenValueCandidate[0], *output.LastObject());
      if (output.LastObject()->size==0)
        crash << "This is a programmig error: " << theEigenValueCandidate[0].ToString() << " is a zero of the minimal polynomial "
        << theMinPoly.ToString() << " of the operator " << this->ToString() << " but the corresponding eigenspace is empty. " << crash;
//      std::cout << " <br> And the resulting eigenspace is: " << *output.LastObject();
      found += output.LastObject()->size;
      if(found == this->NumCols)
        return true;
    }
  }
  return false;
}

template <class coefficient>
void WeylGroupRepresentation<coefficient>::SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace)
{ this->CheckInitialization();
  outputBasisGeneratedSpace.SetSize(1);
  outputBasisGeneratedSpace[0]=input;
  Vector<coefficient> tempV;
  int theRank=this->ownerGroup->GetDim();
  for (int i=0; i<outputBasisGeneratedSpace.size; i++)
    for (int j=0; j<theRank; j++)
    { tempV=outputBasisGeneratedSpace[i];
      this->theElementImageS[j+1].ActOnVectorColumn(tempV);
      if (!outputBasisGeneratedSpace.LinSpanContainsVector(tempV))
        outputBasisGeneratedSpace.AddOnTop(tempV);
    }
}

template <class coefficient>
void WeylGroupRepresentation<coefficient>::GetLargestDenominatorSimpleGens(LargeIntUnsigned& outputLCM, LargeIntUnsigned& outputDen)const
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::GetLargestDenominatorSimpleGens");
  outputLCM=1;
  outputDen=1;
  for(int gi=0; gi<this->generatorS.size; gi++)
    for(int mi=0; mi<this->generatorS[gi].NumRows; mi++)
      for(int mj=0; mj<this->generatorS[gi].NumCols; mj++)
      { if(this->generatorS[gi](mi,mj).GetDenominator() > outputDen)
          outputDen = this->generatorS[gi](mi,mj).GetDenominator();
        outputLCM=LargeIntUnsigned::lcm(outputLCM, this->generatorS[gi](mi,mj).GetDenominator());
      }
}

template <class coefficient>
bool WeylGroupRepresentation<coefficient>::DecomposeTodorsVersionRecursive(WeylGroupVirtualRepresentation<coefficient>& outputIrrepMults, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeTodorsVersionRecursive");
  this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  coefficient SumOfNumComponentsSquared=this->GetNumberOfComponents();
  if (SumOfNumComponentsSquared==0)
    crash << "This is a programming error: a module has character " << this->theCharacteR.ToString() << " of zero length, which is impossible. "
    << "Here is a printout of the module. " << this->ToString() << crash;
  if(SumOfNumComponentsSquared== 1)
  { int indexMe=this->ownerGroup->irreps.GetIndex(*this);
    if(indexMe== -1)
    { this->ownerGroup->AddIrreducibleRepresentation(*this);
      indexMe=this->ownerGroup->irreps.GetIndex(*this);
    }
    outputIrrepMults.AddMonomial(this->ownerGroup->characterTable[indexMe], 1);
    return true;
  }
  Matrix<coefficient> splittingOperatorMatrix;
  Vectors<coefficient> splittingMatrixKernel, remainingVectorSpace, tempSpace;
  ClassFunction<WeylGroup::WeylGroupBase, coefficient> remainingCharacter=this->theCharacteR;
  remainingVectorSpace.MakeEiBasis(this->GetDim());
  ProgressReport Report1(theGlobalVariables), Report2(theGlobalVariables),
  Report3(theGlobalVariables), Report4(theGlobalVariables);
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "<br>\nDecomposing module with character " << this->theCharacteR.ToString();
    LargeIntUnsigned largestDen, lcmDen;
    this->GetLargestDenominatorSimpleGens(lcmDen, largestDen);
    reportStream << "\n<br>\n Largest denominator is " << largestDen.ToString() << ", denominator lcm is: " << lcmDen.ToString();
    Report1.Report(reportStream.str());
  }
  //chop off already known pieces:
  for(int i=0; i<this->ownerGroup->irreps.size; i++)
  { coefficient NumIrrepsOfType=this->theCharacteR.InnerProduct(this->ownerGroup->characterTable[i]);
    if(NumIrrepsOfType!=0)
    { this->ownerGroup->CheckInitializationFDrepComputation();
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "<hr>\ncontains irrep " << this->ownerGroup->irreps[i].theCharacteR.ToString() << " with multiplicity "
        << NumIrrepsOfType << std::endl;
        reportStream << "<hr>\nGetting class f-n matrix from character: " << this->ownerGroup->irreps[i].theCharacteR;
        Report2.Report(reportStream.str());
      }
      this->GetClassFunctionMatrix(this->ownerGroup->irreps[i].theCharacteR, splittingOperatorMatrix);
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "<br>class f-n matrix: " << splittingOperatorMatrix.ToString() << "\n <br>\n" << " computing its zero eigenspace... ";
        Report3.Report(reportStream.str());
      }
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);

      remainingVectorSpace.IntersectTwoLinSpaces(splittingMatrixKernel, remainingVectorSpace, tempSpace);
      outputIrrepMults.AddMonomial(this->ownerGroup->characterTable[i],NumIrrepsOfType);
      remainingCharacter-=this->ownerGroup->irreps[i].theCharacteR*NumIrrepsOfType;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "<br>Intersecting kernel of class f-n matrix" << splittingMatrixKernel.ToString() << " with "
        << remainingVectorSpace.ToString() << " to get: " << tempSpace.ToString() << " with remaining character: " << remainingCharacter.ToString();
        Report4.Report(reportStream.str());
      }
      remainingVectorSpace=tempSpace;
      if(remainingCharacter[0]!=remainingVectorSpace.size)
      { crash << "<br>This is a programming error: remaining char " << remainingCharacter.ToString() << " indicates dimension " << remainingCharacter[0].ToString()
        << " but remaining vector space has dim " << remainingVectorSpace.size << crash;
      }
      if (remainingCharacter.IsEqualToZero())
        if (!remainingVectorSpace.size==0)
        { crash << "This is a programming error: remaining char is zero but remaining space is " << remainingVectorSpace.ToString()
          << ". Starting char: " << this->theCharacteR.ToString() << crash;
        }
    }
  }
  if((remainingVectorSpace.size < this->GetDim()) && (remainingVectorSpace.size > 0))
  { //std::cout << "<br>restricting to subrep(s)... ";
    WeylGroupRepresentation<coefficient> reducedRep;
    this->Restrict(remainingVectorSpace, remainingCharacter, reducedRep, theGlobalVariables);
    //std::cout << "done" << std::endl;
    //std::cout << "Decomposing remaining subrep(s) " << reducedRep.GetCharacter() << std::endl;
    return reducedRep.DecomposeTodorsVersionRecursive(outputIrrepMults, theGlobalVariables);
  }
  if(remainingVectorSpace.size == 0)
    return true;
  int NumClasses=this->ownerGroup->ConjugacyClassCount();
  ClassFunction<WeylGroup::WeylGroupBase, coefficient> virtualChar;
  List<Vectors<coefficient> > theSubRepsBasis;
  for(int cfi=NumClasses-1; cfi>=0; cfi--)
  { virtualChar.MakeZero(*this->ownerGroup);
    virtualChar[cfi] = 1;
//    std::cout << "<br>getting matrix of virtual char " << virtualChar << std::endl;
    this->GetClassFunctionMatrix(virtualChar, splittingOperatorMatrix, theGlobalVariables);
//    FormatExpressions tempFormat;
//    tempFormat.flagUseLatex=true;
//    std::cout << "... and the result is:<br>" << splittingOperatorMatrix.ToString(&tempFormat);
    bool tempB=splittingOperatorMatrix.GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(theSubRepsBasis);
    if (!tempB)
      crash << "<br>This is a mathematical or programming mistake: splittingOperatorMatrix should have small integral values, which it doesn't!" << crash;
    //std::cout << "<br>the eigenspaces were of dimensions: ";
//    for(int i=0; i<theSubRepsBasis.size; i++)
//      std::cout << theSubRepsBasis[i].size << " ";
    WeylGroupRepresentation<coefficient> newRep;
    if (theSubRepsBasis.size>1)//we found splitting, so let us recursively decompose:
    { for(int i=0; i<theSubRepsBasis.size; i++)
      { //std::cout << "<br>restricting current rep to basis " << theSubRepsBasis[i].ToString();
        remainingCharacter.MakeZero(*this->ownerGroup);
        this->Restrict(theSubRepsBasis[i], remainingCharacter, newRep);
        if (!newRep.DecomposeTodorsVersionRecursive(outputIrrepMults, theGlobalVariables))
          return false;
      }
      return true;
    }
//    std::cout << std::endl;
  }
  Vector<coefficient> startingVector, tempV, average;
  startingVector.MakeEi(this->GetDim(), 0);
  average.MakeZero(this->GetDim());
  for (int i=0; i<this->theElementImageS.size; i++)
  { if (!this->theElementIsComputed[i])
      crash << "<hr>This is a programming error: an internal check failed. " << crash;
    this->theElementImageS[i].ActOnVectorColumn(startingVector, tempV);
    average+=tempV;
  }
  std::cout << "<br>Current char: " << this->GetCharacter().ToString();
  std::cout << "<br>Spreading: " << average.ToString();
/*  Vectors<coefficient> theSpread;
  this->SpreadVector(average, theSpread);
  if (theSpread.size==this->GetDim())
    crash << "<hr><b>I was horribly wrong!</b>" << crash;
  else
  { std::cout << "<hr>The image of the spread is " << theSpread.size << " dimensional. ";
  }
*/
  return false;
}

void WeylGroup::AddIrreducibleRepresentation(const WeylGroupRepresentation<Rational>& p)
{ //std::cout << "Checking if " << p.theCharacteR.ToString() << " belongs to ";
  //for (int i=0; i<this->irreps.size; i++)
  //  std::cout << this->irreps[i].theCharacteR.ToString();
  if (this->irreps.Contains(p))
  { WeylGroupRepresentation<Rational>& currentRep=this->irreps[this->irreps.GetIndex(p)];
    currentRep.names.AddOnTopNoRepetition(p.names);
    return;
  }
  this->irreps.AddOnTop(p);
  this->characterTable.AddOnTop(p.theCharacteR);
}

void WeylGroup::AddCharacter(const ClassFunction<WeylGroup::WeylGroupBase, Rational>& X)
{ int i = this->characterTable.BSExpectedIndex(X);
  if(i==this->characterTable.size){
    this->characterTable.AddOnTop(X);
    return;
  }
  if(this->characterTable[i] != X)
  { this->characterTable.InsertAtIndexShiftElementsUp(X,i);
    return;
  }
}

void WeylGroup::ComputeIrreducibleRepresentationsTodorsVersion(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeIrreducibleRepresentationsTodorsVersion");
  if(this->theElements.size == 0)
    this->ComputeCCfromAllElements(theGlobalVariables);
  this->ComputeInitialIrreps(theGlobalVariables);
  WeylGroupRepresentation<Rational> newRep;
  int NumClasses=this->ConjugacyClassCount();
  WeylGroupVirtualRepresentation<Rational> decompositionNewRep;
  ProgressReport theReport1(theGlobalVariables);
  int indexFirstPredefinedRep=1; //<-this should be the index of the sign rep.
  int indexLastPredefinedrep=2; //<-this should be the index of the standard rep.
  for (int i=0; i<this->irreps.size && this->irreps.size!=NumClasses; i++)
    for (int j=indexFirstPredefinedRep; j<=indexLastPredefinedrep; j++)
    { if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << this->irreps.size << " irreducible representations found so far. ";
        reportStream << "<br>Decomposing " << this->irreps[j].theCharacteR.ToString() << " * " << this->irreps[i].theCharacteR.ToString() << std::endl;
        theReport1.Report(reportStream.str());
      }
      newRep= this->irreps[j];//we are initializing by the sign or natural rep.
      newRep*= this->irreps[i];
      bool tempB=newRep.DecomposeTodorsVersion(decompositionNewRep, theGlobalVariables);
      if (!tempB)
        crash << "This is a mathematical error: failed to decompose " << newRep.theCharacteR.ToString() << ". " << crash;
    }
  this->irreps.QuickSortAscending(0, &this->characterTable);
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "Irrep table:";
    for (int i=0; i<this->irreps.size; i++)
      reportStream << "\n<br>\n" << this->irreps[i].theCharacteR.ToString();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    for (int i=0; i<this->irreps.size; i++)
      reportStream << "<hr>irrep " << i+1 << "<br>" << this->irreps[i].ToString(&tempFormat);
    theReport1.Report(reportStream.str());
  }
  this->flagCharTableIsComputed=true;
  this->flagIrrepsAreComputed=true;
}

bool CalculatorFunctionsWeylGroup::innerWeylRaiseToMaximallyDominant(Calculator& theCommands, const Expression& input, Expression& output, bool useOuter)
{ if (input.children.size<2)
    return output.SetError("Raising to maximally dominant takes at least 2 arguments, type and vector", theCommands);
  const Expression& theSSalgebraNode=input[1];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, theSSalgebraNode, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  Vectors<Rational> theHWs;
  theHWs.SetSize(input.children.size-2);
  for (int i=2; i<input.children.size; i++)
    if (!theCommands.GetVectoR<Rational>(input[i], theHWs[i-2], 0, theSSalgebra->GetRank()))
      return output.SetError("Failed to extract rational vectors from arguments", theCommands);
  std::stringstream out;
  out << "Input: " << theHWs.ToString() << ", simultaneously raising to maximally dominant ";
  theSSalgebra->theWeyl.RaiseToMaximallyDominant(theHWs, useOuter);
  out << "<br>Maximally dominant output: " << theHWs.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitOuterSimple(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return output.SetError("innerWeylOrbit takes two arguments", theCommands);
  const Expression& theSSalgebraNode=input[1];
  const Expression& vectorNode=input[2];
  DynkinType theType;
  if (theSSalgebraNode.IsOfType<SemisimpleLieAlgebra>())
    theType=theSSalgebraNode.GetValue<SemisimpleLieAlgebra>().theWeyl.theDynkinType;
  else
    if (!CalculatorSerialization::innerLoadDynkinType(theCommands, theSSalgebraNode, theType))
      return false;
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVectoR(vectorNode, theHWfundCoords, &theContext, theType.GetRank(), CalculatorSerialization::innerPolynomial<Rational>))
    return output.SetError("Failed to extract highest weight", theCommands);
  WeylGroup theWeyl;
  theWeyl.MakeFromDynkinType(theType);
  theHWsimpleCoords=theHWfundCoords;
  theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroup orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theWeyl.GenerateOuterOrbit(theHWs, outputOrbit, &orbitGeneratingSet.theElements, 1921*2))
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size
    << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> <td>Epsilon coords</td><td>Fundamental coords</td>";
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElt;
  LargeInt tempInt;
  bool useMathTag=outputOrbit.size<150;
  Matrix<Rational> epsCoordMat;
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  for (int i=0; i<outputOrbit.size; i++)
  { theFormat.simpleRootLetter="\\alpha";
    theFormat.fundamentalWeightLetter="\\psi";
    std::string orbitEltString=outputOrbit[i].ToString(&theFormat);
    Vector<Polynomial<Rational> > epsVect=outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords=epsVect.ToStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString=
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat
    (theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitGeneratingSet.theElements[i].ToString()) : orbitGeneratingSet.theElements[i].ToString())
    << "</td><td>" << (useMathTag ? CGI::GetMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>" << (useMathTag ? CGI::GetMathSpanPure(weightEltString) : weightEltString) << "</td>";
    latexReport << "$" << orbitGeneratingSet.theElements[i].ToString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords << "$ & $"
    << weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat) << "$\\\\\n<br>";
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylOrbit(Calculator& theCommands, const Expression& input, Expression& output, bool useFundCoords, bool useRho)
{ if (!input.IsListNElements(3))
    return output.SetError("innerWeylOrbit takes two arguments", theCommands);
  const Expression& theSSalgebraNode=input[1];
  const Expression& vectorNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, theSSalgebraNode, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVectoR(vectorNode, theHWfundCoords, &theContext, theSSalgebra->GetRank(), CalculatorSerialization::innerPolynomial<Rational>))
    return output.SetError("Failed to extract highest weight", theCommands);
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (!useFundCoords)
  { theHWsimpleCoords=theHWfundCoords;
    theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  } else
    theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroup orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theSSalgebra->theWeyl.GenerateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet.theElements, 1921))
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> <td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho)
    out << "<td>Standard homomorphism generator candidate</td>";
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElt;
  LargeInt tempInt;
  bool useMathTag=outputOrbit.size<150;
  Matrix<Rational> epsCoordMat;
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  for (int i=0; i<outputOrbit.size; i++)
  { theFormat.simpleRootLetter="\\alpha";
    theFormat.fundamentalWeightLetter="\\psi";
    std::string orbitEltString=outputOrbit[i].ToString(&theFormat);
    Vector<Polynomial<Rational> > epsVect=outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords=epsVect.ToStringLetterFormat("\\varepsilon", &theFormat);
    std::string weightEltString=
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitGeneratingSet.theElements[i].ToString()) : orbitGeneratingSet.theElements[i].ToString())
    << "</td><td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? CGI::GetMathSpanPure(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet.theElements[i].ToString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords
    << "$ & $" <<  weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>";
    if (useRho)
    { currentWeight=theHWsimpleCoords;
      standardElt.MakeOne(*theSSalgebra);
      bool isGood=true;
      for (int j=orbitGeneratingSet.theElements[i].generatorsLastAppliedFirst.size-1; j>=0; j--)
      { int simpleIndex=orbitGeneratingSet.theElements[i].generatorsLastAppliedFirst[j];
        theExp=theWeyl.GetScalarProdSimpleRoot(currentWeight, simpleIndex);
        theWeyl.SimpleReflectionRhoModified(simpleIndex, currentWeight);
        theExp*=2;
        theExp/=theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex];
        theExp+=1;
        if (theExp.IsInteger(&tempInt))
          if (tempInt<0)
          { isGood=false;
            break;
          }
        standardElt.MultiplyByGeneratorPowerOnTheLeft(theSSalgebra->GetNumPosRoots() -simpleIndex-1, theExp);
      }
      out << "<td>";
      if (isGood)
        out << CGI::GetMathSpanPure(standardElt.ToString(&theFormat));
      else
        out << "-";
      out << "</td>";
    }
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupLoadOrComputeCharTable");
  if (!CalculatorSerialization::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  if (theGroup.GetDim()>8)
  { theCommands.Comments << "Computing character table disabled for rank>=8, modify file " << __FILE__
    << " line "  << __LINE__ << " to change that. ";
    return false;
  }
  std::stringstream reportStream;
  theGroup.ComputeOrLoadCharacterTable(theCommands.theGlobalVariableS, &reportStream);
  theCommands.Comments << reportStream.str();
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS");
  if (!CalculatorSerialization::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  if (theGroup.GetDim()>8)
  { theCommands.Comments << "Conjugacy classes computation disabled for rank >8, edit "
    << "file " << __FILE__ << " line " << __LINE__ << " to change that.";
    return false;
  }
  std::stringstream out;
  theGroup.ComputeOrLoadConjugacyClasses(theCommands.theGlobalVariableS, &out);
  theCommands.Comments << out.str();
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesFromAllElements");
  if (!CalculatorSerialization::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  if (theGroup.GetDim()>7)
  { theCommands.Comments << "<hr>Loaded Dynkin type " << theGroup.theDynkinType.ToString() << " of rank " << theGroup.GetDim() << " but I've been told "
    << "not to compute when the rank is larger than 7. ";
    return false;
  }
  double timeStart1=theCommands.theGlobalVariableS->GetElapsedSeconds();
  theGroup.ComputeCCfromAllElements(theCommands.theGlobalVariableS);
  //std::stringstream out;
  theCommands.Comments << "<hr> Computed conjugacy classes of " << theGroup.ToString() << " in " << theCommands.theGlobalVariableS->GetElapsedSeconds()-timeStart1
  << " second(s). ";
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClassesRepresentatives");
  if (!CalculatorSerialization::innerLoadWeylGroup(theCommands, input, output))
    return false;
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  theGroup.CheckConsistency();
  if (theGroup.GetDim()>8)
  { theCommands.Comments << "<hr>Loaded Dynkin type " << theGroup.theDynkinType.ToString() << " of rank " << theGroup.GetDim() << " but I've been told "
    << "not to compute when the rank is larger than 8. ";
    return false;
  }
  theGroup.CheckConsistency();
  double timeStart1=theCommands.theGlobalVariableS->GetElapsedSeconds();
  theGroup.CheckConsistency();
//  theGroup.ComputeCCRepresentatives(theCommands.theGlobalVariableS);
  theGroup.ComputeCCSizesAndRepresentatives(theCommands.theGlobalVariableS);
  //std::stringstream out;
  theCommands.Comments << "<hr> Computed conjugacy classes representatives of "
  << theGroup.theDynkinType.ToString() << " in " << theCommands.theGlobalVariableS->GetElapsedSeconds()-timeStart1
  << " second(s). ";
  return output.AssignValue(theGroup, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupIrrepsAndCharTableComputeFromScratch");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroup>())
    return true;
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
//  std::cout << "And the group is: " << theGroup.ToString();
  theGroup.ComputeIrreducibleRepresentationsTodorsVersion(theCommands.theGlobalVariableS);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  std::stringstream out;
  out << "Character table: ";
  Matrix<Rational> charMat;
  charMat.init(theGroup.ConjugacyClassCount(), theGroup.ConjugacyClassCount());
  for (int i=0; i<theGroup.irreps.size; i++)
  { //out << "<br>" << theGroup.irreps[i].theCharacteR.ToString();
    charMat.AssignVectorToRowKeepOtherRowsIntactNoInit(i, theGroup.irreps[i].GetCharacter().data);
  }
  out << CGI::GetMathSpanPure(charMat.ToString(&tempFormat));
  out << "<br>Explicit realizations of each representation follow.";
  for (int i=0; i<theGroup.irreps.size; i++)
    out << "<hr>" << theGroup.irreps[i].ToString(&tempFormat);
  out << theGroup.ToString(&tempFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupOuterAutoGeneratorsPrint");
  DynkinType theType;
  if (!CalculatorSerialization::innerLoadDynkinType(theCommands, input, theType))
    return output.SetError("Failed to extract Dynkin type from argument. ", theCommands);
  std::stringstream out, outCommand;
  FinitelyGeneratedMatrixMonoid<Rational> groupGeneratedByMatrices;
  theType.GetOuterAutosGeneratorsActOnVectorColumn(groupGeneratedByMatrices.theGenerators);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  for (int i=0; i<groupGeneratedByMatrices.theGenerators.size; i++)
  { outCommand << "<br>s_{" << i+1 << "}:=MatrixRationals" << groupGeneratedByMatrices.theGenerators[i].ToStringMatForm(&tempFormat) << ";";
    out << "<br>s_" << i+1 << " = " << CGI::GetMathSpanPure(groupGeneratedByMatrices.theGenerators[i].ToStringMatForm(&tempFormat) );
  }
  outCommand << "<br>GenerateFiniteMultiplicativelyClosedSet(1000, ";
  for (int i=0; i<groupGeneratedByMatrices.theGenerators.size; i++)
  { outCommand << "s_{" << i+1 << "}";
    if (i!=groupGeneratedByMatrices.theGenerators.size-1)
      outCommand << ", ";
  }
  outCommand << ");";
  out << outCommand.str();
  bool success= groupGeneratedByMatrices.GenerateElements(10000);
  if (!success)
    out << "<br>Did not succeed to generate all elements of the group - the group is of size larger than 10000";
  else
  { out << "<br>The group generated by the outer automorphisms is of size " << groupGeneratedByMatrices.theElements.size;
    if (groupGeneratedByMatrices.theElements.size>100)
      out << "<br>As the group has more than 100 elements, I shall abstain from printing them. ";
    else
    { out << "<table><tr><td>Element</td><td>Matrix</td></tr>";
      for (int i=0; i<groupGeneratedByMatrices.theElements.size; i++)
      { std::stringstream elementNameStream;
        elementNameStream << "t_" << i+1;
        out << "<tr><td>" << CGI::GetMathMouseHover(elementNameStream.str())<< "</td><td>"
        << CGI::GetMathMouseHover(groupGeneratedByMatrices.theElements[i].ToStringMatForm(&tempFormat)) << "</td></tr>";
      }
      out << "</table>";
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFundRho(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, true, true);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitFund(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, true, false);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupOrbitSimple
(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsWeylGroup::innerWeylOrbit(theCommands, input, output, false, false);
}

bool CalculatorFunctionsWeylGroup::innerTensorWeylReps(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTensorWeylReps");
  //std::cout << "Here i am!";
  if (input.children.size!=3)
    return false;
  WeylGroupRepresentation<Rational> leftRep;
  WeylGroupRepresentation<Rational> rightRep;
  if (!input[1].IsOfType<WeylGroupRepresentation<Rational> >(&leftRep))
    return false;
  if (!input[2].IsOfType<WeylGroupRepresentation<Rational> > (&rightRep))
    return false;
  FormatExpressions theFormat;
  theFormat.flagUseLatex=true;
  theFormat.flagUseHTML=false;
  //std::cout << "<br>left rep is: " << leftRep.ToString(&theFormat);
  //std::cout << "<br>right rep is: " << rightRep.ToString(&theFormat);
  if (leftRep.ownerGroup!=rightRep.ownerGroup)
    return output.SetError("Error: attempting to tensor irreps with different owner groups. ", theCommands);
  leftRep*=rightRep;
  return output.AssignValue(leftRep, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerTensorAndDecomposeWeylReps");
  Expression theTensor;
  if (!input.children.size==3)
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!leftE.IsOfType<WeylGroupVirtualRepresentation<Rational> >())
    return false;
  if (!rightE.IsOfType<WeylGroupVirtualRepresentation<Rational> >())
    return false;
  WeylGroupVirtualRepresentation<Rational> outputRep=leftE.GetValue<WeylGroupVirtualRepresentation<Rational> >();
  outputRep*=rightE.GetValue<WeylGroupVirtualRepresentation<Rational> >();
  return output.AssignValue(outputRep, theCommands);
}

std::string WeylGroup::ToStringIrrepLabel(int indexIrrep)
{ if (indexIrrep<this->irrepsCarterLabels.size)
    return this->irrepsCarterLabels[indexIrrep];
  std::stringstream out;
  out << "\\phi_{" << indexIrrep+1 << "}";
  return out.str();
}

std::string WeylGroup::ToStringSignSignatureRootSubsystem(const List<SubgroupRootReflections>& inputSubgroups)
{ MacroRegisterFunctionWithName("WeylGroup::ToStringSignSignatureRootSubsystem");
  if (inputSubgroups.size==0)
    return "";
  std::stringstream out;

  FormatExpressions formatSupressUpperIndexOne;
  formatSupressUpperIndexOne.flagSupressDynkinIndexOne=true;
  DynkinSimpleType simpleType;

  std::stringstream mainTableStream;
  int numParabolicClasses=0, numNonParabolicPseudoParabolic=0, numNonPseudoParabolic=0;
  for (int i=0; i<inputSubgroups.size; i++)
  { SubgroupRootReflections& currentSG=inputSubgroups[i];
    if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic)
    { numNonPseudoParabolic++;
      continue;
    }
    if (currentSG.flagIsParabolic)
      numParabolicClasses++;
    if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic)
      numNonParabolicPseudoParabolic++;
  }

  //check for repeating signatures
  List<List<Rational> > pseudoSignSig, fullSignSig, parabolicSignSig;
  fullSignSig.SetSize(this->ConjugacyClassCount());
  pseudoSignSig.SetSize(this->ConjugacyClassCount());
  parabolicSignSig.SetSize(this->ConjugacyClassCount());
  for (int i=0; i<this->ConjugacyClassCount(); i++)
  { fullSignSig[i].SetSize(inputSubgroups.size);
    pseudoSignSig[i].SetSize(numParabolicClasses+numNonParabolicPseudoParabolic);
    parabolicSignSig[i].SetSize(numParabolicClasses);
    for (int j=0; j<inputSubgroups.size; j++)
    { fullSignSig[i][j]= (inputSubgroups[j].tauSignature[i]==0 ? 0 : 1 );
      if (j<numParabolicClasses+numNonParabolicPseudoParabolic)
        pseudoSignSig[i][j]=fullSignSig[i][j];
      if (j<numParabolicClasses)
        parabolicSignSig[i][j]=fullSignSig[i][j];
    }
  }
  bool hasRepeatingFullSigs=false;
  bool hasRepeatingPseudoParabolicSigs=false;
  bool hasRepeatingParSigs=false;
  for (int i=0; i<fullSignSig.size && !hasRepeatingFullSigs; i++)
    for (int j=i+1; j<fullSignSig.size && !hasRepeatingFullSigs; j++)
      if (fullSignSig[i]==fullSignSig[j])
        hasRepeatingFullSigs=true;
  for (int i=0; i<pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; i++)
    for (int j=i+1; j<pseudoSignSig.size && !hasRepeatingPseudoParabolicSigs; j++)
      if (pseudoSignSig[i]==pseudoSignSig[j])
        hasRepeatingPseudoParabolicSigs=true;
  for (int i=0; i<parabolicSignSig.size && !hasRepeatingParSigs; i++)
    for (int j=i+1; j<parabolicSignSig.size && !hasRepeatingParSigs; j++)
      if (parabolicSignSig[i]==parabolicSignSig[j])
        hasRepeatingParSigs=true;
  if (hasRepeatingParSigs)
    out << "<hr><b>There are repeating parabolic sign signatures. </b><hr>";
  else
    out << "<hr>No repeating parabolic sign signatures. <hr>";
  if (hasRepeatingPseudoParabolicSigs)
    out << "<hr><b>There are repeating pseudo-parabolic sign signatures. </b><hr>";
  else
    out << "<hr>No repeating pseudo-parabolic sign signatures. <hr>";
  if (hasRepeatingFullSigs)
    out << "<hr><b>There are repeating extended sign signatures.</b> <hr>";
  else
    out << "<hr>No repeating extended sign signatures. <hr>";
  if (hasRepeatingParSigs)
  { HashedList<List<Rational> > parSignSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    parSignSigsNoRepetition.AddOnTopNoRepetition(parabolicSignSig);
    irrepsPerSignature.SetSize(parSignSigsNoRepetition.size);
    for (int i=0; i<parabolicSignSig.size; i++)
      irrepsPerSignature[parSignSigsNoRepetition.GetIndex(parabolicSignSig[i])].AddOnTop
      (this->irrepsCarterLabels[i]);
    mainTableStream << "\n<br>\n\n<br>\nThe following families of representations share the same sign signature. ";
    for (int i=0; i<irrepsPerSignature.size; i++)
      if (irrepsPerSignature[i].size>1)
      { mainTableStream << "$(";
        for (int j=0; j<irrepsPerSignature[i].size; j++)
        { mainTableStream << irrepsPerSignature[i][j];
          if (j!=irrepsPerSignature[i].size-1)
            mainTableStream << ", ";
        }
        mainTableStream << ")$ ";
      }
    mainTableStream << "\n<br>\n";
  } //end of check for repeating signatures
  if (hasRepeatingPseudoParabolicSigs)
  { HashedList<List<Rational> > pseudoSigsNoRepetition;
    List<List<std::string> > irrepsPerSignature;
    pseudoSigsNoRepetition.AddOnTopNoRepetition(pseudoSignSig);
    irrepsPerSignature.SetSize(pseudoSigsNoRepetition.size);
    for (int i=0; i<pseudoSignSig.size; i++)
      irrepsPerSignature[pseudoSigsNoRepetition.GetIndex(pseudoSignSig[i])].AddOnTop(this->irrepsCarterLabels[i]);
    mainTableStream << "\n<br>\n\n<br>\nThe following families of representations share the same pseudo-sign signature. ";
    for (int i=0; i<irrepsPerSignature.size; i++)
      if (irrepsPerSignature[i].size>1)
      { mainTableStream << "$(";
        for (int j=0; j<irrepsPerSignature[i].size; j++)
        { mainTableStream << irrepsPerSignature[i][j];
          if (j!=irrepsPerSignature[i].size-1)
            mainTableStream << ", ";
        }
        mainTableStream << ")$ ";
      }
    mainTableStream << "\n<br>\n";
  } //end of check for repeating signatures
  int startIndex=0;
  int numColsPerPage=25;
  int startIndexNextCol=0;
  for (;;)
  { startIndex=startIndexNextCol;
    if (startIndex>=inputSubgroups.size)
      break;
    startIndexNextCol=startIndex+numColsPerPage;
    if (startIndexNextCol-inputSubgroups.size>-3)
      startIndexNextCol=inputSubgroups.size;

    mainTableStream << "\n<br>\n\\begin{longtable}{c|";
    for (int i=startIndex; i<startIndexNextCol; i++)
    { if (i==numParabolicClasses)
        mainTableStream << "|";
      if (i==numParabolicClasses+numNonParabolicPseudoParabolic)
        mainTableStream << "|";
      mainTableStream << "p{0.275cm}";
    }
    mainTableStream << "}\n<br>\n" << "\\caption{\\label{table:SignSignature"
    << CGI::CleanUpForLaTeXLabelUse(this->theDynkinType.ToString())
    << "}Multiplicity of the sign representation over the classes of root subgroups. "
    << "There are " << numParabolicClasses << " parabolic subgroup classes, " << numNonParabolicPseudoParabolic
    << " pseudo-parabolic subgroup classes that are not parabolic, and "
    << numNonPseudoParabolic << " non-pseudo-parabolic subgroup classes. \n<br>\n"
    << "}\\\\ ";
    for (int i=startIndex; i<startIndexNextCol; i++)
    { SubgroupRootReflections& currentSG=inputSubgroups[i];
      if (!currentSG.flagIsParabolic && !currentSG.flagIsExtendedParabolic)
      { mainTableStream << "&-";
        continue;
      }
      if (currentSG.flagIsParabolic)
        mainTableStream << "&$\\mathfrak{p}_{" << i+1 << "}$";
      if (!currentSG.flagIsParabolic && currentSG.flagIsExtendedParabolic)
        mainTableStream << "&${\\widehat{\\mathfrak{ p}}}_{" << i-numParabolicClasses+1 << "}$";
    }
    mainTableStream << "\\\\\n<br>\n";
    mainTableStream << "Irrep label";
    if (this->theDynkinType.IsSimple(&simpleType.theLetter, &simpleType.theRank, &simpleType.CartanSymmetricInverseScale))
      for (int i=startIndex; i<startIndexNextCol; i++)
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.ToStringRelativeToAmbientType(simpleType, &formatSupressUpperIndexOne) << "$";
    else
      for (int i=startIndex; i<startIndexNextCol; i++)
        mainTableStream << "&$" << inputSubgroups[i].theDynkinType.ToString() << "$";
    for (int i=0; i<this->ConjugacyClassCount(); i++)
    { mainTableStream << "\\\\";
      if (i==0)
        mainTableStream << "\\hline";
      mainTableStream << "\n<br>\n$" << this->ToStringIrrepLabel(i) << "$";
      for (int j=startIndex; j<startIndexNextCol; j++)
        mainTableStream << "&" << inputSubgroups[j].tauSignature[i].ToString();
      mainTableStream << "\\\\\n<br>\n";
    }
    mainTableStream << "\\end{longtable}\n<br>\n";
  }
  for (int s=0; s<2; s++)
  { out << "<table style=\"white-space: nowrap;\" border=\"1\">";
    Selection parSelrootsAreOuttaLevi;
    out << "<tr><td>Irrep Label</td><td>Irreducible representation characters</td>";
    if (inputSubgroups[0].flagIsParabolic || inputSubgroups[0].flagIsExtendedParabolic)
    { for (int i=0; i<inputSubgroups.size; i++)
      { parSelrootsAreOuttaLevi=inputSubgroups[i].simpleRootsInLeviParabolic;
        parSelrootsAreOuttaLevi.InvertSelection();
        out << "<td>" << parSelrootsAreOuttaLevi.ToString() << "</td>";
      }
      out << "</tr><tr><td></td><td></td>";
    }
    if (this->theDynkinType.IsSimple(&simpleType.theLetter, &simpleType.theRank, &simpleType.CartanSymmetricInverseScale))
      for (int i=0; i<inputSubgroups.size; i++)
        out << "<td>" << inputSubgroups[i].theDynkinType.ToStringRelativeToAmbientType(simpleType, &formatSupressUpperIndexOne)
        << "</td>";
    else
      for (int i=0; i<inputSubgroups.size; i++)
        out << "<td>" << inputSubgroups[i].theDynkinType.ToString() << "</td>";
    out << "</tr>";
    for (int i=0; i<this->ConjugacyClassCount(); i++)
    { out << "<tr>";
      if (i<this->irrepsCarterLabels.size)
        out << "<td>" << this->irrepsCarterLabels[i] << "</td>";
      else
        out << "<td></td>";
      out << "<td>" << this->characterTable[i].ToString() << "</td>";
      if (s==0)
        for (int j=0; j<inputSubgroups.size; j++)
          out << "<td>" << inputSubgroups[j].tauSignature[i].ToString() << "</td>";
      if (s==1)
        for (int j=0; j<pseudoSignSig[i].size; j++)
          out << "<td>" << pseudoSignSig[i][j].ToString() << "</td>";
      out << "</tr>";
    }
    out << "</table>";
    out << "<br>";
    if (s==0)
      out << "Clipped tau signature follows.<br>";
  }

  out << "<br>A version of the table in ready for LaTeX consumption form follows.<br>\n<br>\n\n<br>\n\n<br>\n";
  out << "\\documentclass{article}\\usepackage{amssymb}\\usepackage{longtable}\\usepackage{pdflscape}"
  << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}"
  << " \\begin{document}\\begin{landscape}\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "{\\tiny \n<br>\n \\renewcommand{\\arraystretch}{0}%\n<br>\n";
  out << "\\begin{longtable}{rl}\\caption{\\label{tableIrrepChars" << this->theDynkinType.ToString()
  << "}\\\\ Irreducible representations and their characters}\\\\ \n<br>\n Irrep label & Character\\\\\n<br>\n";
  for (int i=0; i<this->characterTable.size; i++)
    out << "$" << this->ToStringIrrepLabel(i) << "$&$" << this->characterTable[i].ToString() << "$\\\\\n<br>\n";
  out << "\\end{longtable}\n<br>\n";
  out << "\\begin{longtable}{rcl}"<< "\\caption{\\label{tableConjugacyClassTable"
  << CGI::CleanUpForLaTeXLabelUse(this->theDynkinType.ToString()) << "}}\\\\ ";
  out << "Representative & Class size & Root subsystem label\\\\\n<br>\n";
  for (int i=0; i<this->ConjugacyClassCount(); i++)
  { out << "$" << this->conjugacyClasseS[i].representative.ToString() << "$&$ " << this->conjugacyClasseS[i].size.ToString() << "$";
    if (i<this->ccCarterLabels.size)
      out << "&$" << this->ccCarterLabels[i] << "$";
    out << "\\\\\n<br>\n";
  }
  out << "\\end{longtable}\n<br>\n";

  out << mainTableStream.str();
  out << "}%arraystretch renewcommand scope\n<br\n>\n<br>\n\n<br>\n\n<br>\n\n<br>\n";
  out << "\\end{landscape}\\end{document}";
  return out.str();
}

bool CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerSignSignatureRootSubsystems");
  if (!CalculatorSerialization::innerLoadWeylGroup(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroup>())
    return false;
  WeylGroup& theWeyl=output.GetValueNonConst<WeylGroup>();
  if (theWeyl.GetDim()>8)
  { theCommands.Comments << "<hr>Computing sign signatures restricted up to rank 8.";
    return false;
  }
  std::stringstream out;
  List<SubgroupRootReflections> parabolicSubgroupS, extendedParabolicSubgroups, allRootSubgroups, finalSubGroups;
  if (!theWeyl.LoadSignSignatures(finalSubGroups, theCommands.theGlobalVariableS))
  { theWeyl.GetSignSignatureParabolics(parabolicSubgroupS, theCommands.theGlobalVariableS);
    theWeyl.GetSignSignatureExtendedParabolics(extendedParabolicSubgroups, theCommands.theGlobalVariableS);
    theWeyl.GetSignSignatureAllRootSubsystems(allRootSubgroups, theCommands.theGlobalVariableS);
    List<Pair<std::string, List<Rational>, MathRoutines::hashString> > tauSigPairs;
    finalSubGroups.ReservE(allRootSubgroups.size);
    Pair<std::string, List<Rational>, MathRoutines::hashString> currentTauSig;
    for (int j=0; j<3; j++)
    { List<SubgroupRootReflections>* currentSGs=0;
      if (j==0)
        currentSGs=&parabolicSubgroupS;
      if (j==1)
        currentSGs=&extendedParabolicSubgroups;
      if (j==2)
        currentSGs=&allRootSubgroups;
      for (int i=0; i<currentSGs->size; i++)
      { currentTauSig.Object1=(*currentSGs)[i].theDynkinType.ToString();
        currentTauSig.Object2=(*currentSGs)[i].tauSignature;
        if (!tauSigPairs.Contains(currentTauSig))
        { tauSigPairs.AddOnTop(currentTauSig);
          finalSubGroups.AddOnTop((*currentSGs)[i]);
        }
      }
    }
  }
  out << theWeyl.ToStringSignSignatureRootSubsystem(finalSubGroups);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerDecomposeWeylRep(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerDecomposeWeylRep");
  if (!input.IsOfType<WeylGroupRepresentation<Rational> > ())
    return false;
//  theRep.Decomposition(theCFs, outputReps);
  WeylGroupRepresentation<Rational>& inputRep =input.GetValueNonConst<WeylGroupRepresentation<Rational> >();
  WeylGroupVirtualRepresentation<Rational> outputRep;
  inputRep.DecomposeTodorsVersion(outputRep, theCommands.theGlobalVariableS);
  return output.AssignValue(outputRep, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerIsOuterAutoWeylGroup");
  if (input.children.size!=3)
  { theCommands.Comments << "<hr>IsOuterAuto expects 2 arguments.";
    return false;
  }
  DynkinType theType;
  if (!CalculatorSerialization::innerLoadDynkinType(theCommands, input[1], theType))
  { theCommands.Comments << "<hr>Failed to get Dynkin type from argument. " << input[1].ToString();
    return false;
  }
  Matrix<Rational> theMat;
  if (!input[2].IsOfType<Matrix<Rational> >(&theMat))
    if (!theCommands.GetMatrix(input[2], theMat))
    { theCommands.Comments << "<hr>Failed to get matrix from argument. " << input[2].ToString();
      return false;
    }
  if (theMat.NumCols!=theMat.NumRows || theMat.NumCols!=theType.GetRank())
  { theCommands.Comments << "<hr>Extracted Dynkin type " << theType.ToString() << " is of rank " << theType.GetRank()
    << " but extracted linear operator has " << theMat.NumCols << " columns and " << theMat.NumRows << " rows.";
    return false;
  }
  WeylGroup tempW;
  tempW.MakeFromDynkinType(theType);
  tempW.ComputeRho(true);
  MatrixTensor<Rational> theOp;
  theOp=theMat;
  if (tempW.IsElementWeylGroupOrOuterAuto(theOp))
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupNaturalRep");
  if (!CalculatorFunctionsWeylGroup::innerWeylGroupConjugacyClasseS(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroup>())
    return false;
//  std::cout << "not implemented!";
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  theGroup.ComputeIrreducibleRepresentationsTodorsVersion(theCommands.theGlobalVariableS);
//  std::cout << theGroup.ToString();
  WeylGroupRepresentation<Rational> tempRep;
  theGroup.GetStandardRepresentation(tempRep);
  return output.AssignValue(tempRep, theCommands);
}

class MonomialMacdonald
{
public:
  SemisimpleLieAlgebra* owner;
  Selection rootSel;
  bool flagDeallocated;
  MonomialMacdonald():owner(0), flagDeallocated(false)
  {
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool CheckConsistency()
  { if (this->flagDeallocated)
    { crash << "This is a programming error: use after free of MonomialMacdonald. " << crash;
      return false;
    }
    return true;
  }
  static unsigned int HashFunction(const MonomialMacdonald& input)
  { return input.rootSel.HashFunction();
  }
  bool operator==(const MonomialMacdonald& other)const;
  void MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner);
  void ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple);
  void GenerateMyOrbit(HashedList<MonomialMacdonald>& output);
  ~MonomialMacdonald()
  { this->flagDeallocated=true;
  }
};

std::string MonomialMacdonald::ToString(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("MonomialMacdonald::ToString");
  if (this->owner==0)
    return "(non-initialized)";
  if (this->rootSel.CardinalitySelection==0)
    return "1";
  std::stringstream out;
  for (int i=0; i<this->rootSel.CardinalitySelection; i++)
  { out << "a_" << this->rootSel.elements[i] << "";
  }
  return out.str();
}

bool MonomialMacdonald::operator==(const MonomialMacdonald& other)const
{ return this->owner==other.owner && this->rootSel==other.rootSel;
}

void MonomialMacdonald::GenerateMyOrbit(HashedList<MonomialMacdonald>& output)
{ MacroRegisterFunctionWithName("MonomialMacdonald::GenerateMyOrbit");
  output.Clear();
  output.AddOnTop(*this);
  MonomialMacdonald currentMon;
  Rational tempRat;
  for (int i=0; i<output.size; i++)
    for (int j=0; j<this->owner->GetRank(); j++)
    { currentMon=output[i];
      currentMon.ActOnMeSimpleReflection(j, tempRat);
      output.AddOnTopNoRepetition(currentMon);
    }
}

void MonomialMacdonald::MakeFromRootSubsystem(const Vectors<Rational>& inputRoots, SemisimpleLieAlgebra& inputOwner)
{ MacroRegisterFunctionWithName("MonomialMacdonald::MakeFromRootSubsystem");
  this->owner=&inputOwner;
  this->rootSel.init(inputOwner.theWeyl.RootSystem.size);
  Vector<Rational> currentV;
  for (int i=0; i<inputRoots.size; i++)
  { currentV=inputRoots[i];
    if (currentV.IsNegative())
      currentV*=-1;
    int indexInRoots=inputOwner.theWeyl.RootSystem.GetIndex(currentV);
    if (indexInRoots<0)
      crash << "This is a programming error: attempting to make a Macdonald polynomial from " << inputRoots.ToString() << ": the vector " << currentV.ToString()
      << " is not a root. " << crash;
    this->rootSel.AddSelectionAppendNewIndex(indexInRoots);
  }
  this->rootSel.ComputeIndicesFromSelection();
}

void MonomialMacdonald::ActOnMeSimpleReflection(int indexSimpleReflection, Rational& outputMultiple)
{ Selection originalSel;
 // std::cout << "this->rootSel:" << this->rootSel.ToString() << ", elements: " << this->rootSel.elements;

  originalSel=this->rootSel;
  this->rootSel.init(this->owner->theWeyl.RootSystem.size);
  Vector<Rational> currentV;
  outputMultiple=1;
//  std::cout << originalSel.ToString() << ", elements: " << originalSel.elements;
  for (int i=0; i<originalSel.CardinalitySelection; i++)
  { currentV=this->owner->theWeyl.RootSystem[originalSel.elements[i]];
    this->owner->theWeyl.SimpleReflection(indexSimpleReflection, currentV);
    if (currentV.IsNegative())
    { currentV*=-1;
      outputMultiple*=-1;
    }
    this->rootSel.AddSelectionAppendNewIndex(this->owner->theWeyl.RootSystem.GetIndex(currentV));
  }
}

bool CalculatorFunctionsWeylGroup::innerMacdonaldPolys(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerMacdonaldPolys");
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input, thePointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  rootSubalgebras theRootSAs;
  theRootSAs.owneR=thePointer;
  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphismOLD(*theCommands.theGlobalVariableS, true, false);
  std::stringstream out;
  MonomialMacdonald theGenerator;
  HashedList<MonomialMacdonald> theOrbit;
  for (int i=0; i<theRootSAs.theSubalgebras.size; i++)
  { rootSubalgebra& currentRootSA=theRootSAs.theSubalgebras[i];
    theGenerator.MakeFromRootSubsystem(currentRootSA.PositiveRootsK, *thePointer);
    theGenerator.GenerateMyOrbit(theOrbit);
    out << "<hr>Root subsystem type " << currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(thePointer->theWeyl.theDynkinType[0]);
    out << ". Orbit has " << theOrbit.size << " elements, here they are: ";
    for (int j=0; j<theOrbit.size; j++)
      out << "<br>" << theOrbit[j].ToString();
  }
  out << "Type: " << theRootSAs.owneR->theWeyl.theDynkinType.ToString() << ". Number of root subsystems: " << theRootSAs.theSubalgebras.size;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsWeylGroup::innerWeylGroupElement(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerWeylGroupElement");
  //if (!input.IsSequenceNElementS(2))
  //return output.SetError("Function Coxeter element takes two arguments.", theCommands);
  if(input.children.size<2)
    return output.SetError("Function WeylElement needs to know what group the element belongs to", theCommands);
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(CalculatorSerialization::innerSSLieAlgebra, input[1], thePointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  ElementWeylGroup<WeylGroup> theElt;
  theElt.generatorsLastAppliedFirst.ReservE(input.children.size-2);
  for(int i=2; i<input.children.size; i++)
  { int tmp;
    if (!input[i].IsSmallInteger(& tmp))
      return false;
    theElt.generatorsLastAppliedFirst.AddOnTop(tmp-1);
  }
  WeylGroup theGroup;
  theGroup=thePointer->theWeyl;
  int indexOfownerGroupInObjectContainer=theCommands.theObjectContainer.theWeylGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  //std::cout << "Group type: " << theGroup.ToString() << "<br>Index in container: "
  //<< indexOfownerGroupInObjectContainer;

  theElt.owner=&theCommands.theObjectContainer.theWeylGroups[indexOfownerGroupInObjectContainer];
  //std::cout << "<br>theElt.owner: " << theElt.owner;
//  std::cout << "<b>Not implemented!!!!!</b> You requested reflection indexed by " << theReflection;
  for(int i=0; i<theElt.generatorsLastAppliedFirst.size; i++)
    if (theElt.generatorsLastAppliedFirst[i] >= thePointer->GetRank() || theElt.generatorsLastAppliedFirst[i] < 0)
      return output.SetError("Bad reflection index", theCommands);
//  std::cout << "\n" << theGroup.rho << " " << theElt.owner->rho << std::endl;
  theElt.MakeCanonical();
  return output.AssignValue(theElt, theCommands);
}

bool Calculator::innerGenerateMultiplicativelyClosedSet(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerGenerateMultiplicativelyClosedSet");
  if (input.children.size<=2)
    return output.SetError("I need at least two arguments - upper bound and at least one element to multiply.", theCommands);
  int upperLimit;
  if (!input[1].IsSmallInteger(&upperLimit))
    return output.SetError("First argument must be a small integer, serving as upper bound for the set.", theCommands);
  if (upperLimit <=0)
  { upperLimit=10000;
    theCommands.Comments << "The upper computation limit I got was 0 or less; I replaced it with the default value " << upperLimit << ".";
  }
  HashedList<Expression> theSet;
  theSet.SetExpectedSize(input.children.size-2);
  for (int i=2; i<input.children.size; i++)
    theSet.AddOnTop(input[i]);
  int numGenerators=theSet.size;
  Expression theProduct, evaluatedProduct;
  BoundVariablesSubstitution tempSub;
  bool tempBool;
  //std::cout << "<br>" << theSet[0].ToString() << "->" << theSet[0].ToStringFull() << " is with hash " << theSet[0].HashFunction();
  ProgressReport theReport(theCommands.theGlobalVariableS);
  for (int i=0; i<theSet.size; i++)
    for (int j=0; j<numGenerators; j++)
    { tempSub.reset();
      theProduct.MakeProducT(theCommands, theSet[j], theSet[i]);
      std::stringstream reportStream;
      reportStream << "found " << theSet.size << "elements so far, exploring element " << i+1;
      reportStream << "<br>Evaluating: " << theProduct.ToString();
      theReport.Report(reportStream.str());
      theCommands.EvaluateExpression(theProduct, evaluatedProduct, tempSub, tempBool);
      //std::cout << " to get " << evaluatedProduct.ToString() << "->" << evaluatedProduct.ToStringFull();
      //std::cout << " with hash " << evaluatedProduct.HashFunction();
      //if (evaluatedProduct==theSet[0])
      //{ //std::cout << " and equals the first element. ";
      //}
      theSet.AddOnTopNoRepetition(evaluatedProduct);
      if (theSet.size>upperLimit)
      { std::stringstream out;
        out << "<hr>While generating multiplicatively closed set, I went above the upper limit of " << upperLimit << " elements.";
        evaluatedProduct.SetError(out.str(), theCommands);
        theSet.AddOnTop(evaluatedProduct);
        i=theSet.size; break;
      }
    }
  theCommands.Comments << "<hr>Generated a list of " << theSet.size << " elements";
  output.reset(theCommands, theSet.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i=0; i<theSet.size; i++)
    output.AddChildOnTop(theSet[i]);
  return true;
}

template <class coefficient>
void WeylGroupVirtualRepresentation<coefficient>::operator*=(const WeylGroupVirtualRepresentation<coefficient>& other)
{ MacroRegisterFunctionWithName("WeylGroupVirtualRepresentation::operator*=");
  crash << "not implemented yet" << crash;
/*  WeylGroupVirtualRepresentation<coefficient> output, currentContribution;
  output.ownerGroup=this->ownerGroup;
  output.coefficientsIrreps.MakeZero(this->coefficientsIrreps.size);
  WeylGroupRepresentation<Rational> tempRep;
  for (int i=0; i<this->coefficientsIrreps.size; i++)
    for (int j=0; j<other.coefficientsIrreps.size; j++)
    { Rational theCoeff= this->coefficientsIrreps[i]*other.coefficientsIrreps[j];
      if (theCoeff==0)
        continue;
      tempRep=this->ownerGroup->irreps[i];
      tempRep*=this->ownerGroup->irreps[j];
      tempRep.DecomposeTodorsVersion(currentContribution, 0);
      output.coefficientsIrreps+=currentContribution*theCoeff;
    }
  *this=output;*/
}

template <class coefficient>
void WeylGroupVirtualRepresentation<coefficient>::AssignWeylGroupRep(const WeylGroupRepresentation<Rational>& other, GlobalVariables* theGlobalVariables)
{ crash << " not implemented " << crash;
  WeylGroupRepresentation<Rational> otherCopy;
  otherCopy=other;
//  otherCopy.DecomposeTodorsVersion(this->coefficientsIrreps, theGlobalVariables);
}

bool CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsWeylGroup::innerMakeVirtualWeylRep");
  if (input.IsOfType<WeylGroupVirtualRepresentation<Rational> >())
  { output=input;
    return true;
  }
  if (!input.IsOfType<WeylGroupRepresentation<Rational> >())
    return false;
  WeylGroupRepresentation<Rational>& inputRep=input.GetValueNonConst<WeylGroupRepresentation<Rational> >();
  if (inputRep.ownerGroup->irreps.size<inputRep.ownerGroup->ConjugacyClassCount())
    inputRep.ownerGroup->ComputeIrreducibleRepresentationsTodorsVersion(theCommands.theGlobalVariableS);
  WeylGroupVirtualRepresentation<Rational> outputRep;
  outputRep.AssignWeylGroupRep(inputRep);
  return output.AssignValue(outputRep, theCommands);
}

