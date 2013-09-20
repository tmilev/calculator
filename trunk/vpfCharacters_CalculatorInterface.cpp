#include "vpf.h"
#include "vpfHeader2Math3_Characters.h"
#include "vpfHeader2Math4_Graph.h"
#include "vpfHeader3Calculator3_WeylGroupCharacters.h"

static ProjectInformationInstance ProjectInfoVpfCharactersCalculatorInterfaceCPP
(__FILE__, "Weyl group calculator interface");

bool WeylGroup::CheckInitializationFDrepComputation()const
{ if (this->conjugacyClasses.size==0 || this->theElements.size==0)
  { std::cout << "This is a programming error: requesting to compute character hermitian product "
    << " in a group whose conjugacy classes and/or elements have not been computed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    return false;
  }
  for (int i=0; i<this->irreps.size; i++)
    if (this->irreps[i].theCharacter.IsEqualToZero())
    { std::cout << "This is a programming error: irrep number " << i+1
      << " has zero character!" << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
  return true;
}

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::CheckAllSimpleGensAreOK()const
{ this->CheckInitialization();
  for (int i=0; i<this->OwnerGroup->GetDim(); i++)
  { bool isOK=this->theElementIsComputed[i+1];
    if (this->theElementImages[i+1].NumRows==0)
      isOK=false;
    if (!isOK)
    { std::cout << "This is a programming error: working with a representation in which "
      << " the action of the simple generators is not computed. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
    }
  }

  return true;
}

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::CheckInitialization()const
{ if (this->OwnerGroup==0)
  { std::cout << "This is a programming error: working with a representation with "
    << " non-initialized owner Weyl group. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    return false;
  }
  return true;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::CheckRepIsMultiplicativelyClosed
()
{ HashedList<Matrix<Rational> > tempList;
  tempList.AddOnTop(this->theElementImages);
  Matrix<Rational> tempMat;
  ElementWeylGroup tempElt;
  for (int i=0; i<tempList.size; i++)
    for (int j=0; j<tempList.size; j++)
    { tempMat=tempList[i];
      tempMat.MultiplyOnTheLeft(tempList[j]);
      tempElt=this->OwnerGroup->theElements[j];
      tempElt*=this->OwnerGroup->theElements[i];
      tempElt.MakeCanonical();
      int targetIndex=this->OwnerGroup->theElements.GetIndex(tempElt);
      if (!(tempMat==this->theElementImages[targetIndex]))
      { std::cout << "this is a programming error: element " << i+1 << " times element "
        << j+1 << " is outside of the set, i.e.,  " << tempList[i].ToString() << " * "
        << tempList[j].ToString() << " is bad. ";
        assert(false);
      }
    }
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::ComputeAllGeneratorImagesFromSimple
( GlobalVariables* theGlobalVariables)
{ this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  HashedList<ElementWeylGroup> ElementsExplored;
  ElementsExplored.SetExpectedSize(this->OwnerGroup->theElements.size);
  this->theElementImages[0].MakeIdMatrix(this->GetDim());
  ElementWeylGroup tempElt;
  int theRank=this->OwnerGroup->GetDim();
  tempElt.owner=this->OwnerGroup;
  ElementsExplored.AddOnTop(tempElt);
  tempElt.SetSize(1);
  for (int i=0; i<theRank; i++)
  { tempElt[0]=i;
    ElementsExplored.AddOnTop(tempElt);
  }
  for (int i=0; i<ElementsExplored.size; i++)
  { int indexParentElement=this->OwnerGroup->theElements.GetIndex(ElementsExplored[i]);
    for (int j=0; j<theRank; j++)
    { tempElt=ElementsExplored[i];
      tempElt.AddOnTop(j);
      tempElt.MakeCanonical();
      if (!ElementsExplored.Contains(tempElt))
      { int indexCurrentElt=this->OwnerGroup->theElements.GetIndex(tempElt);
        this->theElementIsComputed[indexCurrentElt]=true;
        this->theElementImages[indexParentElement].MultiplyOnTheLeft
        (this->theElementImages[j+1], this->theElementImages[indexCurrentElt]);
        ElementsExplored.AddOnTop(tempElt);
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
void WeylGroupRepresentation<coefficient>::operator*=
(const WeylGroupRepresentation<coefficient>& other)
{ //lazy programmers handling://////
  if (&other==this )
  { WeylGroupRepresentation<coefficient> otherCopy;
    otherCopy=other;
    *this*=otherCopy;
    return;
  }
  //////////////////////////////////
  if (this->OwnerGroup!=other.OwnerGroup)
  { std::cout << "This is a programming error: attempting to multiply representations with "
    << " different owner groups. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  WeylGroupRepresentation<coefficient> output;
  output.reset(this->OwnerGroup);
  output.theCharacter=this->theCharacter;
  for (int i=0; i<output.theCharacter.size; i++)
    output.theCharacter[i]*=other.theCharacter[i];
  for(int i=0; i<output.theElementImages.size; i++)
    if (this->theElementIsComputed[i] && other.theElementIsComputed[i])
    { output.theElementImages[i].AssignTensorProduct
      (this->theElementImages[i],other.theElementImages[i]);
      output.theElementIsComputed[i]=true;
    }
  *this=output;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::Restrict
(const Vectors<coefficient>& VectorSpaceBasisSubrep, const Vector<Rational>& remainingCharacter,
 WeylGroupRepresentation<coefficient>& output, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::Restrict");
  this->CheckAllSimpleGensAreOK();
  if (VectorSpaceBasisSubrep.size==0)
  { std::cout << "This is a programming error: restriction of representation to a zero "
    << " subspace is not allowed. ";
    assert(false);
  }
  output.reset(this->OwnerGroup);
  output.vectorSpaceBasis = VectorSpaceBasisSubrep;
  output.vectorSpaceBasis.GetGramMatrix(output.gramMatrixInverted, 0);
  output.gramMatrixInverted.Invert();
  output.theCharacter=remainingCharacter;
  ProgressReport theReport(theGlobalVariables);
  for(int i=1; i<this->OwnerGroup->GetDim()+1; i++)
    if (this->theElementIsComputed[i])
    { output.theElementIsComputed[i]=true;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "Restricting the action of generator of index " << i;
        theReport.Report(reportStream.str());
      }
      Matrix<coefficient>::MatrixInBasis
      (this->theElementImages[i], output.theElementImages[i], output.vectorSpaceBasis,
       output.gramMatrixInverted);

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
  LargeIntUnsigned l,d;
  output.GetLargestDenominatorSimpleGens(l,d);
  while(d < 10)
    return;
/*
  List<HashedList<VectorSpace<Rational> > > spaces;
  for(int i=0; i<OwnerGroup->theElements.size; i++)
  { output.GetElementImage(i);
    List<VectorSpace<Rational> > spsi = GetRationalSpecialSpaces(this->theElementImages[i]);
    for(int j=0; j<spsi.size; j++)
    { if(spaces.size < spsi[j].rank-1)
        spaces.SetSize(spsi[j].rank);
      spaces[spsi[j].rank-1].AddOnTop(spsi[j]);
    }
    if(spaces[0].size >= VectorSpaceBasisSubrep.size)
      goto have_enough_1spaces;
  }
  for(int ri=1; ri<spaces.size; ri++)
  { for(int i=0; i<spaces[ri].size-1; i++)
    { for(int j=i+1; j<spaces[ri].size; j++)
      { VectorSpace<coefficient> k = spaces[ri][i].Intersection(spaces[ri][j]);
        spaces[k.rank-1].AddOnTop(k);
        if(spaces[0].size >= VectorSpaceBasisSubrep.size)
          goto have_enough_1spaces;
      }
    }
  }

  have_enough_1spaces: // if not, we crash in two lines from here
  Vectors<Rational> newbasis;
  for(int i=0; i<VectorSpaceBasisSubrep.size; i++)
   newbasis.AddOnTop(spaces[0][i].GetBasisVector(0));
  this->Restrict(newbasis, remainingCharacter, output, theGlobalVariables);
  */

//  std::cout << "<hr>The restriction result: " << output.ToString();
//  this->CheckRepIsMultiplicativelyClosed();
}

template <class coefficient>
coefficient WeylGroupRepresentation<coefficient>::GetNumberOfComponents()
{ return this->OwnerGroup->GetCharacterNorm(this->GetCharacter());
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::GetClassFunctionMatrix
(Vector<coefficient>& virtualCharacter, Matrix<coefficient>& outputMat, GlobalVariables* theGlobalVariables)
{ this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  outputMat.MakeZeroMatrix(this->GetDim());
  int numClasses=this->OwnerGroup->conjugacyClasses.size;
  ProgressReport theReport(theGlobalVariables);
  for(int cci=0; cci<numClasses; cci++)
  { if(virtualCharacter[cci] == 0)
      continue;
    if (!this->classFunctionMatricesComputed[cci])
    { this->classFunctionMatricesComputed[cci]=true;
      // classFunctionMatrices does not have to be initialized.
      bool useParent=false;
      if (this->parent!=0)
        useParent=
        (this->parent->classFunctionMatrices.size == this->OwnerGroup->conjugacyClasses.size) &&
        (this->parent->classFunctionMatricesComputed[cci]);
      if(useParent)
      { Matrix<coefficient>::MatrixInBasis(this->parent->classFunctionMatrices[cci], this->classFunctionMatrices[cci], this->vectorSpaceBasis,this->gramMatrixInverted);
      } else {
        List<int>& currentConjugacyClass=this->OwnerGroup->conjugacyClasses[cci];
        this->classFunctionMatrices[cci].MakeZeroMatrix(this->GetDim());
        for (int i=0; i<currentConjugacyClass.size; i++)
        { if (!this->theElementIsComputed[currentConjugacyClass[i]])
            this->ComputeAllGeneratorImagesFromSimple(theGlobalVariables);
          this->classFunctionMatrices[cci]+=this->theElementImages[currentConjugacyClass[i]];
          if (theGlobalVariables!=0)
          { std::stringstream reportstream;
            reportstream << " Computing conjugacy class " << currentConjugacyClass[i]+1
            << " (total num classes is " << numClasses << ").";
            theReport.Report(reportstream.str());
          }
        }
      }
      if (theGlobalVariables!=0)
      { std::stringstream reportstream;
        reportstream << "<br>Class function matrix of conjugacy class "
        << cci+1 << " (total num classes is " << numClasses << ") computed to be: "
        << this->classFunctionMatrices[cci].ToString();
        theReport.Report(reportstream.str());
      }

    }
    for(int j=0; j<outputMat.NumRows; j++)
      for(int k=0; k<outputMat.NumCols; k++)
        outputMat(j,k)+= this->classFunctionMatrices[cci](j,k) * virtualCharacter[cci].GetComplexConjugate();
  }
}

template <class coefficient>
bool WeylGroupRepresentation<coefficient>::DecomposeMeIntoIrreps(Vector<Rational>& outputIrrepMults, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeMeIntoIrreps");
  this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  outputIrrepMults.MakeZero(this->OwnerGroup->conjugacyClasses.size);
  return this->DecomposeMeIntoIrrepsRecursive(outputIrrepMults, theGlobalVariables);
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
      { std::cout << "This is a programmig error: " << theEigenValueCandidate[0]
        << " is a zero of the minimal polynomial " << theMinPoly.ToString() << " of the operator "
        << this->ToString() << " but the corresponding eigenspace is empty. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);

      }
//      std::cout << " <br> And the resulting eigenspace is: " << *output.LastObject();
      found += output.LastObject()->size;
      if(found == this->NumCols)
        return true;
    }
  }
  return false;
}

template <typename coefficient>
Matrix<coefficient>& WeylGroupRepresentation<coefficient>::GetElementImage(int elementIndex)
{ this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  Matrix<coefficient>& theMat=this->theElementImages[elementIndex];
  if (this->theElementIsComputed[elementIndex])
    return theMat;
  const ElementWeylGroup& theElt=this->OwnerGroup->theElements[elementIndex];
  this->theElementIsComputed[elementIndex]=true;
  theMat.MakeIdMatrix(this->GetDim());
  for (int i=0; i<theElt.size; i++)
    theMat.MultiplyOnTheLeft(this->theElementImages[theElt[i]+1]);
  return theMat;
}

template <typename coefficient>
Vector<coefficient>& WeylGroupRepresentation<coefficient>::GetCharacter()
{ this->CheckInitialization();
  if (this->theCharacter.size>0)
    return this->theCharacter;
//  std::cout << "Computing character: ";
  int numClasses=this->OwnerGroup->conjugacyClasses.size;
  this->theCharacter.SetSize(numClasses);
  for (int i=0; i<numClasses; i++)
    this->theCharacter[i]= this->GetElementImage(this->OwnerGroup->conjugacyClasses[i][0]).GetTrace();
//  std::cout << "... done: character is: " << this->theCharacter.ToString();
  return this->theCharacter;
}

template <class coefficient>
void WeylGroupRepresentation<coefficient>::SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace)
{ this->CheckInitialization();
  outputBasisGeneratedSpace.SetSize(1);
  outputBasisGeneratedSpace[0]=input;
  Vector<coefficient> tempV;
  int theRank=this->OwnerGroup->GetDim();
  for (int i=0; i<outputBasisGeneratedSpace.size; i++)
    for (int j=0; j<theRank; j++)
    { tempV=outputBasisGeneratedSpace[i];
      this->theElementImages[j+1].ActOnVectorColumn(tempV);
      if (!outputBasisGeneratedSpace.LinSpanContainsVector(tempV))
        outputBasisGeneratedSpace.AddOnTop(tempV);
    }
}

template <class coefficient>
void WeylGroupRepresentation<coefficient>::GetLargestDenominatorSimpleGens
(LargeIntUnsigned& outputLCM, LargeIntUnsigned& outputDen)const
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::GetLargestDenominatorSimpleGens");
  outputLCM=1;
  outputDen=1;
  for(int gi=1; gi<this->OwnerGroup->GetDim()+1; gi++)
    if (!this->theElementIsComputed[gi])
    { std::cout << "This is a programming error: the simple generator has not been computed, but "
      << " is being used. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    } else
      for(int mi=0; mi<this->theElementImages[gi].NumRows; mi++)
        for(int mj=0; mj<this->theElementImages[gi].NumCols; mj++)
        { if(this->theElementImages[gi](mi,mj).GetDenominator() > outputDen)
            outputDen = this->theElementImages[gi](mi,mj).GetDenominator();
          outputLCM=
          LargeIntUnsigned::lcm(outputLCM, this->theElementImages[gi](mi,mj).GetDenominator());
        }
}


template <class coefficient>
bool WeylGroupRepresentation<coefficient>::DecomposeMeIntoIrrepsRecursive
(Vector<Rational>& outputIrrepMults, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName(" WeylGroupRepresentation::DecomposeMeIntoIrrepsRecursive");
  this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  coefficient SumOfNumComponentsSquared=this->GetNumberOfComponents();
  if (SumOfNumComponentsSquared==0)
  { std::cout << "This is a programming error: a module has character "
    << this->theCharacter.ToString() << " of zero length, which is impossible. "
    << "Here is a printout of the module. " << this->ToString()
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if(SumOfNumComponentsSquared== 1)
  { int indexMe=this->OwnerGroup->irreps.GetIndex(*this);
    if(indexMe== -1)
    { this->OwnerGroup->irreps.AddOnTop(*this);
      indexMe=this->OwnerGroup->irreps.size-1;
    }
    outputIrrepMults[indexMe]+=1;
    return true;
  }
  Matrix<coefficient> splittingOperatorMatrix;
  Vectors<coefficient> splittingMatrixKernel, remainingVectorSpace, tempSpace;
  Vector<coefficient> remainingCharacter;
  remainingCharacter=this->theCharacter;
  remainingVectorSpace.MakeEiBasis(this->GetDim());
  ProgressReport Report1(theGlobalVariables), Report2(theGlobalVariables),
  Report3(theGlobalVariables), Report4(theGlobalVariables);
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "<br>\nDecomposing module with character " << this->theCharacter.ToString();
    LargeIntUnsigned largestDen, lcmDen;
    this->GetLargestDenominatorSimpleGens(lcmDen, largestDen);
    reportStream << "\n<br>\n Largest denominator is " << largestDen.ToString()
    << ", denominator lcm is: " << lcmDen.ToString();
    Report1.Report(reportStream.str());
  }
  //chop off already known pieces:
  for(int i=0; i<this->OwnerGroup->irreps.size; i++)
  { coefficient NumIrrepsOfType=this->OwnerGroup->GetHermitianProduct
       (this->theCharacter, this->OwnerGroup->irreps[i].theCharacter);
    if(NumIrrepsOfType!=0)
    { this->OwnerGroup->CheckInitializationFDrepComputation();
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "<hr>\ncontains irrep " << this->OwnerGroup->irreps[i].theCharacter.ToString()
        << " with multiplicity " << NumIrrepsOfType << std::endl;
        reportStream << "<hr>\nGetting class f-n matrix from character: "
        << this->OwnerGroup->irreps[i].theCharacter;
        Report2.Report(reportStream.str());
      }
      this->GetClassFunctionMatrix(this->OwnerGroup->irreps[i].theCharacter, splittingOperatorMatrix);
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "<br>class f-n matrix: " << splittingOperatorMatrix.ToString() << "\n <br>\n"
        << " computing its zero eigenspace... ";
        Report3.Report(reportStream.str());
      }
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);

      remainingVectorSpace.IntersectTwoLinSpaces(splittingMatrixKernel, remainingVectorSpace, tempSpace);
      outputIrrepMults[i]+=NumIrrepsOfType;
      remainingCharacter-=this->OwnerGroup->irreps[i].theCharacter*NumIrrepsOfType;
      if (theGlobalVariables!=0)
      { std::stringstream reportStream;
        reportStream << "<br>Intersecting kernel of class f-n matrix"
        << splittingMatrixKernel.ToString()
        << " with " << remainingVectorSpace.ToString()
        << " to get: " << tempSpace.ToString()
        << " with remaining character: " << remainingCharacter.ToString();
        Report4.Report(reportStream.str());
      }
      remainingVectorSpace=tempSpace;
      if(remainingCharacter[0]!=remainingVectorSpace.size)
      { std::cout << "<br>This is a programming error: remaining char "
        << remainingCharacter.ToString() << " indicates dimension " << remainingCharacter[0]
        << " but remaining vector space has dim "
        << remainingVectorSpace.size;
        //assert(false);
      }
      if (remainingCharacter.IsEqualToZero())
        if (!remainingVectorSpace.size==0)
        { std::cout << "This is a programming error: remaining char is zero but remaining space is "
          << remainingVectorSpace.ToString() << ". Starting char: " << this->theCharacter.ToString()
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          //assert(false);
        }
    }
  }
  if((remainingVectorSpace.size < this->GetDim()) && (remainingVectorSpace.size > 0))
  { //std::cout << "<br>restricting to subrep(s)... ";
    WeylGroupRepresentation<coefficient> reducedRep;
    this->Restrict(remainingVectorSpace, remainingCharacter, reducedRep, theGlobalVariables);
    //std::cout << "done" << std::endl;
    //std::cout << "Decomposing remaining subrep(s) " << reducedRep.GetCharacter() << std::endl;
    return reducedRep.DecomposeMeIntoIrrepsRecursive(outputIrrepMults, theGlobalVariables);
  }
  if(remainingVectorSpace.size == 0)
    return true;
  int NumClasses=this->OwnerGroup->conjugacyClasses.size;
  Vector<coefficient> virtualChar;
  List<Vectors<coefficient> > theSubRepsBasis;
  for(int cfi=NumClasses-1; cfi>=0; cfi--)
  { virtualChar.MakeZero(NumClasses);
    virtualChar[cfi] = 1;
//    std::cout << "<br>getting matrix of virtual char " << virtualChar << std::endl;
    this->GetClassFunctionMatrix(virtualChar, splittingOperatorMatrix, theGlobalVariables);
//    FormatExpressions tempFormat;
//    tempFormat.flagUseLatex=true;
//    std::cout << "... and the result is:<br>" << splittingOperatorMatrix.ToString(&tempFormat);
    bool tempB=
    splittingOperatorMatrix.GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim
    (theSubRepsBasis);
    if (!tempB)
    { std::cout << "<br>This is a mathematical or programming mistake: "
      << "splittingOperatorMatrix should have small integral values, which it doesn't!"
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    //std::cout << "<br>the eigenspaces were of dimensions: ";
//    for(int i=0; i<theSubRepsBasis.size; i++)
//      std::cout << theSubRepsBasis[i].size << " ";
    WeylGroupRepresentation<coefficient> newRep;
    if (theSubRepsBasis.size>1)//we found splitting, so let us recursively decompose:
    { for(int i=0; i<theSubRepsBasis.size; i++)
      { //std::cout << "<br>restricting current rep to basis " << theSubRepsBasis[i].ToString();
        remainingCharacter.SetSize(0);
        this->Restrict(theSubRepsBasis[i], remainingCharacter, newRep);
        if (!newRep.DecomposeMeIntoIrrepsRecursive(outputIrrepMults, theGlobalVariables))
          return false;
      }
      return true;
    }
//    std::cout << std::endl;
  }
  Vector<coefficient> startingVector, tempV, average;
  startingVector.MakeEi(this->GetDim(), 0);
  average.MakeZero(this->GetDim());
  for (int i=0; i<this->theElementImages.size; i++)
  { if (!this->theElementIsComputed[i])
    { std::cout << "<hr>Oh shit! ";
      assert(false);
    }
    this->theElementImages[i].ActOnVectorColumn(startingVector, tempV);
    average+=tempV;
  }
  std::cout << "<br>Current char: " << this->GetCharacter().ToString();
  std::cout << "<br>Spreading: " << average.ToString();
/*  Vectors<coefficient> theSpread;
  this->SpreadVector(average, theSpread);
  if (theSpread.size==this->GetDim())
  { std::cout << "<hr><b>I was horribly wrong!</b>";
    assert(false);
  } else
  { std::cout << "<hr>The image of the spread is " << theSpread.size << " dimensional. ";
  }
*/
  return false;
}

void WeylGroup::ComputeIrreducibleRepresentations
(GlobalVariables* theGlobalVariables)
{ if(this->theElements.size == 0)
    this->ComputeConjugacyClasses(theGlobalVariables);
  WeylGroupRepresentation<Rational> theStandardRep, newRep;
  this->StandardRepresentation(theStandardRep);
  int NumClasses=this->conjugacyClasses.size;
  Vector<Rational> decompositionNewRep;
  ProgressReport theReport1(theGlobalVariables);
  for (int i=0; i<this->irreps.size && this->irreps.size!=NumClasses; i++)
  { if (theGlobalVariables!=0)
    { std::stringstream reportStream;
      reportStream << "Irreducible representations found so far: ";
      for (int j=0; j<this->irreps.size; j++)
        reportStream << "\n<br>\n" << this->irreps[j].theCharacter.ToString();
      reportStream << "<hr>\nDecomposing\n <br>\n" << theStandardRep.theCharacter.ToString()
      << " * <br>" << this->irreps[i].theCharacter.ToString() << std::endl;
      theReport1.Report(reportStream.str());
    }
    newRep= theStandardRep;
    newRep*= this->irreps[i];
    bool tempB=
    newRep.DecomposeMeIntoIrreps(decompositionNewRep, theGlobalVariables);
    if (!tempB)
    { std::cout << "This is a mathematical error: failed to decompose " << newRep.theCharacter.ToString() << ". "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  this->irreps.QuickSortAscending();
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "Irrep table:";
    for (int i=0; i<this->irreps.size; i++)
      reportStream << "\n<br>\n" << this->irreps[i].theCharacter.ToString();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    for (int i=0; i<this->irreps.size; i++)
      reportStream << "<hr>irrep " << i+1 << "<br>" << this->irreps[i].ToString(&tempFormat);
    theReport1.Report(reportStream.str());
  }
}

bool WeylGroupCalculatorFunctions::innerWeylRaiseToMaximallyDominant
(CommandList& theCommands, const Expression& input, Expression& output, bool useOuter)
{ if (input.children.size<2)
    return output.SetError("Raising to maximally dominant takes at least 2 arguments, type and vector", theCommands);
  const Expression& theSSalgebraNode=input[1];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, theSSalgebraNode, theSSalgebra))
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

template <class coefficient>
bool WeylGroup::GenerateOuterOrbit
(Vectors<coefficient>& theRoots, HashedList<Vector<coefficient> >& output,
 HashedList<ElementWeylGroup>* outputSubset, int UpperLimitNumElements)
{ this->ComputeExternalAutos();
  output.Clear();
  for (int i=0; i<theRoots.size; i++)
    output.AddOnTop(theRoots[i]);
  Vector<coefficient> currentRoot;
  ElementWeylGroup tempEW;
  tempEW.owner=this;
  output.SetExpectedSize(UpperLimitNumElements);
  if (outputSubset!=0)
  { tempEW.size=0;
    outputSubset->SetExpectedSize(UpperLimitNumElements);
    outputSubset->Clear();
    outputSubset->AddOnTop(tempEW);
  }
  int numGens=this->GetDim()+this->OuterAutomorphisms.size;
  for (int i=0; i<output.size; i++)
  { if (outputSubset!=0)
      tempEW=outputSubset->TheObjects[i];
    for (int j=0; j<numGens; j++)
    { currentRoot=output[i];
      if(j<this->GetDim())
        this->SimpleReflection(j, currentRoot);
      else
        this->OuterAutomorphisms[j-this->GetDim()].ActOnVectorColumn(currentRoot);
      if (output.AddOnTopNoRepetition(currentRoot))
        if (outputSubset!=0)
        { tempEW.AddOnTop(j);
          outputSubset->AddOnTop(tempEW);
          tempEW.RemoveIndexSwapWithLast(tempEW.size-1);
        }
      if (UpperLimitNumElements>0)
        if (output.size>=UpperLimitNumElements)
          return false;
    }
  }
  return true;
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitOuterSimple(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return output.SetError("innerWeylOrbit takes two arguments", theCommands);
  const Expression& theSSalgebraNode=input[1];
  const Expression& vectorNode=input[2];
  DynkinType theType;
  if (theSSalgebraNode.IsOfType<SemisimpleLieAlgebra>())
    theType=theSSalgebraNode.GetValue<SemisimpleLieAlgebra>().theWeyl.theDynkinType;
  else
    if (!Serialization::innerLoadDynkinType(theCommands, theSSalgebraNode, theType))
      return false;
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVectoR(vectorNode, theHWfundCoords, &theContext, theType.GetRank(), Serialization::innerPolynomial<Rational>))
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
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& "
  << "Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> "
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
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
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitGeneratingSet.theElements[i].ToString()) : orbitGeneratingSet.theElements[i].ToString())
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet.theElements[i].ToString(&theFormat) << "$ & $"
    << orbitEltStringEpsilonCoords
    << "$ & $"
    <<  weightEltString << "$ & $"
    << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>"
    ;
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool WeylGroupCalculatorFunctions::innerWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
{ if (!input.IsListNElements(3))
    return output.SetError("innerWeylOrbit takes two arguments", theCommands);
  const Expression& theSSalgebraNode=input[1];
  const Expression& vectorNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, theSSalgebraNode, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVectoR(vectorNode, theHWfundCoords, &theContext, theSSalgebra->GetRank(), Serialization::innerPolynomial<Rational>))
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
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size
    << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& "
  << "Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> "
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho)
    out << "<td>Corresponding b-singular vector candidate</td>";
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
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitGeneratingSet.theElements[i].ToString()) : orbitGeneratingSet.theElements[i].ToString())
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords)
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet.theElements[i].ToString(&theFormat) << "$ & $" << orbitEltStringEpsilonCoords
    << "$ & $" <<  weightEltString << "$ & $" << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>";
    if (useRho)
    { currentWeight=theHWsimpleCoords;
      standardElt.MakeOne(*theSSalgebra);
      bool isGood=true;
      for (int j=0; j<orbitGeneratingSet.theElements[i].size; j++)
      { int simpleIndex=orbitGeneratingSet.theElements[i][j];
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
        out << CGI::GetHtmlMathSpanPure(standardElt.ToString(&theFormat));
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

bool WeylGroupCalculatorFunctions::innerWeylGroupIrrepsAndCharTable
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!WeylGroupCalculatorFunctions::innerWeylGroupConjugacyClasses(theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroup>())
    return true;
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  theGroup.ComputeIrreducibleRepresentations(theCommands.theGlobalVariableS);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  std::stringstream out;
  out << "Character table: ";
  Matrix<Rational> charMat;
  charMat.init(theGroup.irreps.size, theGroup.irreps.size);
  for (int i=0; i<theGroup.irreps.size; i++)
  { //out << "<br>" << theGroup.irreps[i].theCharacter.ToString();
    charMat.AssignVectorToRowKeepOtherRowsIntactNoInit(i, theGroup.irreps[i].GetCharacter());
  }
  out << CGI::GetHtmlMathSpanPure(charMat.ToString(&tempFormat));
  out << "<br>Explicit realizations of each representation follow.";
  for (int i=0; i<theGroup.irreps.size; i++)
  { out << "<hr>" << theGroup.irreps[i].ToString(&tempFormat);
  }
  out << theGroup.ToString(&tempFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOuterAutoGeneratorsPrint
(CommandList& theCommands, const Expression& input, Expression& output)
{ DynkinType theType;
  if (!Serialization::innerLoadDynkinType(theCommands, input, theType))
    return output.SetError("Failed to extract Dynkin type from argument. ", theCommands);
  std::stringstream out, outCommand;
  List<Matrix<Rational> > theGens;
  theType.GetOuterAutosGenerators(theGens);
  FormatExpressions tempFormat;
  tempFormat.flagUseLatex=true;
  tempFormat.flagUseHTML=false;
  for (int i=0; i<theGens.size; i++)
  { outCommand << "<br>s_{" << i+1 << "}:=MatrixRationals" << theGens[i].ToString(&tempFormat) << ";";
    out << "<br>s_" << i+1 << " = " << CGI::GetHtmlMathSpanPure(theGens[i].ToString(&tempFormat) );
  }
  outCommand << "<br>GenerateFiniteMultiplicativelyClosedSet(1000, ";
  for (int i=0; i<theGens.size; i++)
  { outCommand << "s_{" << i+1 << "}";
    if (i!=theGens.size-1)
      outCommand << ", ";
  }
  outCommand << ");";
  out << outCommand.str();
  return output.AssignValue(out.str(), theCommands);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitFundRho
(CommandList& theCommands, const Expression& input, Expression& output)
{ return WeylGroupCalculatorFunctions::innerWeylOrbit(theCommands, input, output, true, true);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitFund
(CommandList& theCommands, const Expression& input, Expression& output)
{ return WeylGroupCalculatorFunctions::innerWeylOrbit(theCommands, input, output, true, false);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupOrbitSimple
(CommandList& theCommands, const Expression& input, Expression& output)
{ return WeylGroupCalculatorFunctions::innerWeylOrbit(theCommands, input, output, false, false);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupConjugacyClasses
(CommandList& theCommands, const Expression& input, Expression& output)
{ SemisimpleLieAlgebra* thePointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input, thePointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  output.AssignValue(thePointer->theWeyl, theCommands);
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  if (theGroup.CartanSymmetric.NumRows>6)
    return output.AssignValue<std::string>
    ("I have been instructed not to do this for Weyl groups of rank greater \
     than 6 because of the size of the computation.", theCommands);

//  CoxeterGroup otherGroup;
//  otherGroup.MakeFrom(theGroup.CartanSymmetric);
  double timeStart1=theCommands.theGlobalVariableS->GetElapsedSeconds();
  theGroup.ComputeConjugacyClasses();
  theCommands.Comments << "<hr> Computed conjugacy classes of " << theGroup.ToString() << " in "
  << theCommands.theGlobalVariableS->GetElapsedSeconds()-timeStart1 << " second(s).";
//  double timeStart2=theCommands.theGlobalVariableS->GetElapsedSeconds();
//  otherGroup.ComputeConjugacyClasses();
//  std::cout << "Time of conjugacy class computation method2: "
//  << theCommands.theGlobalVariableS->GetElapsedSeconds()-timeStart2;
  ElementWeylGroup tempTestElt;
  for (int i=0; i<theGroup.theElements.size; i++)
  { tempTestElt=theGroup.theElements[i];
    tempTestElt.MakeCanonical();
  }
  return true;
}

bool WeylGroupCalculatorFunctions::innerTensorWeylReps
(CommandList& theCommands, const Expression& input, Expression& output)
{ //std::cout << "Here i am!";
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
  if (leftRep.OwnerGroup!=rightRep.OwnerGroup)
    return output.SetError("Error: attempting to tensor irreps with different owner groups. ", theCommands);
  leftRep*=rightRep;
  return output.AssignValue(leftRep, theCommands);
}

bool WeylGroupCalculatorFunctions::innerTensorAndDecomposeWeylReps(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("WeylGroupCalculatorFunctions::innerTensorAndDecomposeWeylReps");
  Expression theTensor;
  if (!input.children.size==3)
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!leftE.IsOfType<WeylGroupVirtualRepresentation>())
    return false;
  if (!rightE.IsOfType<WeylGroupVirtualRepresentation>())
    return false;
  WeylGroupVirtualRepresentation outputRep=leftE.GetValue<WeylGroupVirtualRepresentation>();
  outputRep*=rightE.GetValue<WeylGroupVirtualRepresentation>();
  return output.AssignValue(outputRep, theCommands);
}

bool WeylGroupCalculatorFunctions::innerDecomposeWeylRep(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("WeylGroupCalculatorFunctions::innerDecomposeWeylRep");
  if (!input.IsOfType<WeylGroupRepresentation<Rational> > ())
    return false;
//  theRep.Decomposition(theCFs, outputReps);
  WeylGroupRepresentation<Rational>& inputRep =input.GetValueNonConst<WeylGroupRepresentation<Rational> >();
  WeylGroupVirtualRepresentation outputRep;
  outputRep.ownerGroup=inputRep.OwnerGroup;
  inputRep.DecomposeMeIntoIrreps(outputRep.coefficientsIrreps, theCommands.theGlobalVariableS);
  return output.AssignValue(outputRep, theCommands);
}

bool WeylGroupCalculatorFunctions::innerWeylGroupNaturalRep(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("WeylGroupCalculatorFunctions::innerWeylGroupNaturalRep");
  if (!WeylGroupCalculatorFunctions::innerWeylGroupConjugacyClasses
      (theCommands, input, output))
    return false;
  if (!output.IsOfType<WeylGroup>())
    return false;
//  std::cout << "not implemented!";
  WeylGroup& theGroup=output.GetValueNonConst<WeylGroup>();
  theGroup.ComputeIrreducibleRepresentations(theCommands.theGlobalVariableS);
//  std::cout << theGroup.ToString();
  WeylGroupRepresentation<Rational> tempRep;
  theGroup.StandardRepresentation(tempRep);
  return output.AssignValue(tempRep, theCommands);
}

bool WeylGroupCalculatorFunctions::innerCoxeterElement(CommandList& theCommands, const Expression& input, Expression& output)
{ //if (!input.IsSequenceNElementS(2))
  //return output.SetError("Function Coxeter element takes two arguments.", theCommands);
  if(input.children.size<2){
    return output.SetError
    ("Function CoxeterElement needs to know what group the element belongs to", theCommands);
  }
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], thePointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  ElementWeylGroup theElt;
  theElt.ReservE(input.children.size-2);
  for(int i=2; i<input.children.size; i++){
    int tmp;
    if (!input[i].IsSmallInteger(& tmp))
      return false;
    theElt.AddOnTop(tmp-1);
  }
  WeylGroup theGroup;
  theGroup=thePointer->theWeyl;
  int indexOfOwnerGroupInObjectContainer=theCommands.theObjectContainer.theWeylGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  //std::cout << "Group type: " << theGroup.ToString() << "<br>Index in container: "
  //<< indexOfOwnerGroupInObjectContainer;

  theElt.owner=&theCommands.theObjectContainer.theWeylGroups[indexOfOwnerGroupInObjectContainer];
  //std::cout << "<br>theElt.owner: " << theElt.owner;
//  std::cout << "<b>Not implemented!!!!!</b> You requested reflection indexed by " << theReflection;
  for(int i=0; i<theElt.size; i++){
    if (theElt[i] >= thePointer->GetRank() || theElt[i] < 0)
      return output.SetError("Bad reflection index", theCommands);
  }
//  std::cout << "\n" << theGroup.rho << " " << theElt.owner->rho << std::endl;
  theElt.MakeCanonical();
  return output.AssignValue(theElt, theCommands);
}

bool CommandList::innerMinPolyMatrix(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!theCommands.innerMatrixRational(theCommands, input, output))
    return false;
  Matrix<Rational> theMat;
  if (!output.IsOfType<Matrix<Rational> >(&theMat))
  { theCommands.Comments << "<hr> Successfully called innerMatrixRational onto input " << input.ToString()
    << " to get " << output.ToString()
    << " but the return type was not a matrix of rationals. ";
    return true;
  }
  if (theMat.NumRows!=theMat.NumCols || theMat.NumRows<=0)
    return output.SetError("Error: matrix is not square!", theCommands);
  FormatExpressions tempF;
  tempF.polyAlphabeT.SetSize(1);
  tempF.polyAlphabeT[0]="q";
  Polynomial<Rational> theMinPoly;
  theMinPoly.AssignMinPoly(theMat);
  return output.AssignValue(theMinPoly.ToString(&tempF), theCommands);
}

bool CommandList::innerGenerateMultiplicativelyClosedSet(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerGenerateMultiplicativelyClosedSet");
  if (input.children.size<=2)
    return output.SetError("I need at least two arguments - upper bound and at least one element to multiply.", theCommands);
  int upperLimit;
  if (!input[1].IsSmallInteger(&upperLimit))
    return output.SetError
    ("First argument must be a small integer, serving as upper bound for the set.", theCommands);
  if (upperLimit <=0)
  { upperLimit=10000;
    theCommands.Comments << "The upper computation limit I got was 0 or less; I replaced it with the default value "
    << upperLimit << ".";
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
        out << "<hr>While generating multiplicatively closed set, I went above the upper limit of "
        << upperLimit << " elements.";
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

void WeylGroupVirtualRepresentation::operator+=(const WeylGroupVirtualRepresentation& other)
{ if (this->ownerGroup!=other.ownerGroup)
  { std::cout << "This is a programming error: adding virtual representations of different groups. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->coefficientsIrreps+=other.coefficientsIrreps;
}

void WeylGroupVirtualRepresentation::operator*=(const WeylGroupVirtualRepresentation& other)
{ if (this->ownerGroup!=other.ownerGroup)
  { std::cout << "This is a programming error: adding virtual representations of different groups. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  WeylGroupVirtualRepresentation output;
  output.ownerGroup=this->ownerGroup;
  output.coefficientsIrreps.MakeZero(this->coefficientsIrreps.size);
  WeylGroupRepresentation<Rational> tempRep;
  Vector<Rational> currentContribution;
  for (int i=0; i<this->coefficientsIrreps.size; i++)
    for (int j=0; j<other.coefficientsIrreps.size; j++)
    { Rational theCoeff= this->coefficientsIrreps[i]*other.coefficientsIrreps[j];
      if (theCoeff==0)
        continue;
      tempRep=this->ownerGroup->irreps[i];
      tempRep*=this->ownerGroup->irreps[j];
      tempRep.DecomposeMeIntoIrreps(currentContribution, 0);
      output.coefficientsIrreps+=currentContribution*theCoeff;
    }
  *this=output;
}

std::string WeylGroupVirtualRepresentation::ToString(FormatExpressions* theFormat)const
{ if (this->ownerGroup==0)
    return "(not initialized)";
  std::stringstream out;
  bool found=false;
  std::string tempS;
  for (int i=0; i<this->coefficientsIrreps.size; i++)
    if (this->coefficientsIrreps[i]!=0)
    { if (found)
        if (this->coefficientsIrreps[i]>0)
          out << "+";
      found=true;
      tempS=this->coefficientsIrreps[i].ToString();
      if(tempS=="-1")
        tempS="-";
      if (tempS=="1")
        tempS="";
      out << tempS << "V_{" << this->ownerGroup->irreps[i].GetCharacter().ToString() << "}";
    }
  return out.str();
}

void WeylGroupVirtualRepresentation::AssignWeylGroupRep
(const WeylGroupRepresentation<Rational>& other, GlobalVariables* theGlobalVariables)
{ WeylGroupRepresentation<Rational> otherCopy;
  this->ownerGroup=other.OwnerGroup;
  otherCopy=other;
  otherCopy.DecomposeMeIntoIrreps(this->coefficientsIrreps, theGlobalVariables);
}

bool WeylGroupCalculatorFunctions::innerMakeVirtualWeylRep
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (input.IsOfType<WeylGroupVirtualRepresentation>())
  { output=input;
    return true;
  }
  if (!input.IsOfType<WeylGroupRepresentation<Rational> >())
    return false;
  WeylGroupRepresentation<Rational>& inputRep=input.GetValueNonConst<WeylGroupRepresentation<Rational> >();
  if (inputRep.OwnerGroup->irreps.size<inputRep.OwnerGroup->conjugacyClasses.size)
    inputRep.OwnerGroup->ComputeIrreducibleRepresentations(theCommands.theGlobalVariableS);
  WeylGroupVirtualRepresentation outputRep;
  outputRep.AssignWeylGroupRep(inputRep);
  return output.AssignValue(outputRep, theCommands);
}

