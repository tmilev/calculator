#include "polyhedra.h"
// This file is meant to be for people to modify if they do not want to modify the main files polyhedra.cpp or polyhedra.h
// The reason I would recommend that is because the file polyhedra.cpp compiles very slowly (around 30 seconds), so small modifications
// take a long time to check. Using a separate file and linking it to the GUI is the solution I chose (may not be the best, see below for discussion).
// If the code you write here reaches the mature phase where you have realized all functions you think it should have
// and you are generally satisfied with it, simply cut & paste it in the main files (the class declarations in polyhedra.h and the implementation in polyhedra.cpp).

//Discussion: other options for speeding up the compilation that I have considered.
//1. Cut up the main files polyhedra.h and polyhedra.cpp into small quick-to-compile pieces. This might be the best solution in the long run. However, I do not want to
//   do it just yet, because 1) I am not sure what should those parts be - it is not yet clear in my head how to partition the code in conceptually distinct pieces
//   2) this would certainly create additional maintainance work 3) this will increase the learning curve for a person wanting to just use the program and wanting to eventually
//   modify some tiny bit and 4) I got an advice on the c++ forum www.cplusplus.com that partitioning the .h file will eventually lead to slower compile times,
//   especially with the massive use of templates that I do. Therefore, such a partitioning should not be done before the code reaches
//   greater maturity (see also point (1) ).
//2. Use precompiled headers or some other voodoo. I am tocally against that. Those are compiler specific, will require me to learn extra unnecessary info which might
//    be out of date in a few years, and will make even higher entry learning curve for another to join the project. This is bad.
//    I should mention in favor of Microsoft that their IDE does recompile very quickly small modifications of the file polyhedra.cpp. I believe it does so by
//    keeping a database of your recently changed code, and recompiling only the recently changed pieces. Hats off to Microsoft for doing that completely programmatically,
//    and not bothering the programmer with stupid things such as how to set up precompiled headers.
//
// To whomever might be reading this (if anyone): happy hacking and I hope you find my code useful, that it didn't cause you many headaches, and that you
// did something useful with it! Cheers!

void rootSubalgebras::ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, roots& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; std::stringstream out;
  roots epsCoords;
  Matrix<int> tempMat;
  int theDimension=this->AmbientWeyl.CartanSymmetric.NumRows;
  if (this->AmbientWeyl.WeylLetter=='B')
  { this->AmbientWeyl.GetEpsilonCoords(input, epsCoords, theGlobalVariables);
    tempMat.MakeIdMatrix(theDimension*2+1);
    tempMat.elements[theDimension][theDimension]=0;
    for (int i=0; i<epsCoords.size; i++)
    { bool isShort=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      root& currentRoot=epsCoords.TheObjects[i];
      epsCoords.ComputeDebugString();
      for (int j=0; j<theDimension; j++)
      { if (currentRoot.TheObjects[j]!=0)
        { isShort=!isShort;
          if(isShort)
          { if (currentRoot.TheObjects[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot.TheObjects[j].IsPositive())
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
  if (this->AmbientWeyl.WeylLetter=='C')
  { this->AmbientWeyl.GetEpsilonCoords(input, epsCoords, theGlobalVariables);
    tempMat.MakeIdMatrix(theDimension*2);
    for (int i=0; i<epsCoords.size; i++)
    { bool isLong=false;
      int firstIndex=-1;
      int secondIndex=-1;
      bool firstSignIsPositive=true;
      bool secondSignIsPositive=true;
      root& currentRoot=epsCoords.TheObjects[i];
      epsCoords.ComputeDebugString();
      for (int j=0; j<theDimension; j++)
      { if (currentRoot.TheObjects[j]!=0)
        { isLong=!isLong;
          if(isLong)
          { if (currentRoot.TheObjects[j].IsPositive())
              firstSignIsPositive=true;
            else
              firstSignIsPositive=false;
            firstIndex=j;
          } else
          { if (currentRoot.TheObjects[j].IsPositive())
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
  { root& currentRoot=epsCoords.TheObjects[i];
    currentRoot.ElementToStringEpsilonForm(tempS, true, false);
    if (!epsCoords.ContainsObject(-currentRoot))
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
      if (this->AmbientWeyl.WeylLetter=='B' && (i==theDimension-1 || i==theDimension))
        out  << "|";
    }
    out << "}";
    for (int i=0; i< tempMat.NumRows; i++)
    { if (this->AmbientWeyl.WeylLetter=='B' && (i==theDimension || i==theDimension+1))
        out  << "\\hline";
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

void ComputationSetup::G2InD4Experiment(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "\\documentclass{article}\\begin{document}";
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  SemisimpleLieAlgebra theLie;
  theLie.ComputeChevalleyConstants('G',  2, theGlobalVariables);
  theLie.ComputeDebugString(false, true, theGlobalVariables);
  out << theLie.DebugString;
  theLie.theWeyl.ComputeWeylGroup(-1);
  root tempHW="(0,1)";
  tempHW.ComputeDebugString();
  roots tempRoots;
  theLie.CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(tempHW, theGlobalVariables);
  std::string tempS;
  if (theLie.CheckClosedness(tempS, theGlobalVariables))
    out << "Lie bracket is good! ";
  else
    out << tempS;
  theLie.ElementToStringEmbedding(tempS);
  out << "\n\n\n" << tempS << "\n\n\n";
 // theLie.
  MatrixLargeRational theAuto;
  theLie.ComputeChevalleyConstants('D', 4, theGlobalVariables);
  theLie.ComputeOneAutomorphism(theGlobalVariables, theAuto, false );
  theAuto.ElementToString(tempS, false, true);
  out << tempS;
  MatrixLargeRational tempMat;
  tempMat.MakeIdMatrix(theAuto.NumRows);
  theAuto= theAuto-tempMat;
  theAuto.FindZeroEigenSpace(tempRoots, theGlobalVariables);
  tempMat.AssignRootsToRowsOfMatrix(tempRoots);
  tempMat.Transpose(theGlobalVariables);
  tempMat= theAuto*tempMat;
  tempMat.ElementToString(tempS, false, true);
  std::string tempS2;
  tempRoots.ElementToString(tempS2, true, false, true);
  out << "\n\n" << tempS2 << "\n\n" << tempS;

  out << "\\end{document}";

  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
}

void MatrixLargeRational::FindZeroEigenSpace(roots& output, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational tempMat;
  tempMat.Assign(*this);
  MatrixLargeRational emptyMat;
  Selection nonPivotPts;
  tempMat.GaussianEliminationByRows(tempMat, emptyMat, nonPivotPts);
  output.SetSizeExpandOnTopNoObjectInit(nonPivotPts.CardinalitySelection);
  for (int i=0; i<nonPivotPts.CardinalitySelection; i++)
  { root& current= output.TheObjects[i];
    current.MakeZero(this->NumCols);
    int currentPivotIndex = nonPivotPts.elements[i];
    current.TheObjects[currentPivotIndex]=1;
    int rowCounter=0;
    for (int j=0; j<this->NumCols; j++)
      if (!nonPivotPts.selected[j])
      { current.TheObjects[j]=-tempMat.elements[rowCounter][currentPivotIndex];
        rowCounter++;
      }
  }
}

void ElementSimpleLieAlgebra::AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(int generatorIndex, const SemisimpleLieAlgebra& owner)
{ int numPosRoots=owner.theWeyl.RootsOfBorel.size;
  int theDimension=owner.theWeyl.CartanSymmetric.NumRows;
  this->Nullify(owner);
  if(generatorIndex< numPosRoots || generatorIndex>=numPosRoots+theDimension)
  { this->SetCoefficient(owner.theWeyl.RootSystem.TheObjects[owner.ChevalleyGeneratorIndexToRootIndex(generatorIndex)], 1, owner);
    return;
  }
  this->Hcomponent.MakeEi(theDimension, generatorIndex-numPosRoots);
}

void ElementSimpleLieAlgebra::AssingVectorRootSpacesFirstThenCartan(const root& input, SemisimpleLieAlgebra& owner)
{ this->Nullify(owner);
  for (int i=0; i<this->coeffsRootSpaces.size; i++)
    this->coeffsRootSpaces.TheObjects[i]=input.TheObjects[i];
  for (int i=0; i<this->Hcomponent.size; i++)
   this->Hcomponent.TheObjects[i]=input.TheObjects[i+this->coeffsRootSpaces.size];
  this->ComputeNonZeroElements();
}

void ElementSimpleLieAlgebra::ElementToVectorNegativeRootSpacesFirst(root& output)
{ output.MakeZero(this->coeffsRootSpaces.size+ this->Hcomponent.size);
  int numPosRoots=this->coeffsRootSpaces.size/2;
  for (int i=0; i<this->NonZeroElements.CardinalitySelection; i++)
  { int theIndex=this->NonZeroElements.elements[i];
    int targetIndex= (theIndex<numPosRoots) ? theIndex+numPosRoots+this->Hcomponent.size : -theIndex+2*numPosRoots-1;
    output.TheObjects[targetIndex]= this->coeffsRootSpaces.TheObjects[theIndex];
  }
  for (int i=0; i<this->Hcomponent.size; i++)
    output.TheObjects[i+numPosRoots]= this->Hcomponent.TheObjects[i];
}

void ElementSimpleLieAlgebra::ElementToVectorRootSpacesFirstThenCartan(root& output)
{ output.MakeZero(this->coeffsRootSpaces.size+ this->Hcomponent.size);
  for (int i=0; i<this->NonZeroElements.CardinalitySelection; i++)
  { int theIndex=this->NonZeroElements.elements[i];
    output.TheObjects[theIndex]= this->coeffsRootSpaces.TheObjects[theIndex];
  }
  for (int i=0; i<this->Hcomponent.size; i++)
    output.TheObjects[i+this->coeffsRootSpaces.size]= this->Hcomponent.TheObjects[i];
}

void SemisimpleLieAlgebra::ComputeOneAutomorphism(GlobalVariables& theGlobalVariables, MatrixLargeRational& outputAuto, bool useNegativeRootsFirst)
{ rootSubalgebra theRootSA;
  theRootSA.AmbientWeyl.Assign(this->theWeyl);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  theRootSA.genK.MakeEiBasis(theDimension);
  ReflectionSubgroupWeylGroup theAutos;
  theRootSA.GenerateAutomorphismsPreservingBorel(theGlobalVariables, theAutos);
  MatrixLargeRational mapOnRootSpaces;
  int theAutoIndex= theAutos.ExternalAutomorphisms.size>1? 1 : 0;
  /*if (this->theWeyl.WeylLetter=='D' && theDimension==4)
    theAutoIndex=2;
*/
  mapOnRootSpaces.AssignRootsToRowsOfMatrix(theAutos.ExternalAutomorphisms.TheObjects[theAutoIndex]);
  mapOnRootSpaces.Transpose(theGlobalVariables);
//  mapOnRootSpaces.ComputeDebugString();
//  MatrixLargeRational theDet=mapOnRootSpaces;
//  Rational tempRat;
//  theDet.ComputeDeterminantOverwriteMatrix(tempRat);
//  std::cout << " ... and the det is: " << tempRat.ElementToString();
  Selection NonExplored;
  int numRoots= this->theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  root domainRoot, rangeRoot;
  this->ComputeChevalleyConstants(this->theWeyl, theGlobalVariables);
  List<ElementSimpleLieAlgebra> Domain, Range;
  Range.SetSizeExpandOnTopNoObjectInit(numRoots+theDimension);
  Domain.SetSizeExpandOnTopNoObjectInit(numRoots+theDimension);
  ElementSimpleLieAlgebra tempElt;
  for (int i=0; i<theDimension; i++)
  { domainRoot.MakeEi(theDimension, i);
    mapOnRootSpaces.ActOnAroot(domainRoot, rangeRoot);
    tempElt.Nullify(*this);
    tempElt.Hcomponent= domainRoot;
    Domain.TheObjects[numRoots+i]=tempElt;
    tempElt.Hcomponent= rangeRoot;
    Range.TheObjects[numRoots+i]=tempElt;
    for (int i=0; i<2; i++, domainRoot.MinusRoot(), rangeRoot.MinusRoot())
    { int theIndex= this->theWeyl.RootSystem.IndexOfObjectHash(rangeRoot);
      tempElt.Nullify(*this);
      tempElt.SetCoefficient(rangeRoot, Rational::TheRingUnit, *this);
      Range.TheObjects[theIndex]=tempElt;
      tempElt.Nullify(*this);
      tempElt.SetCoefficient(domainRoot, Rational::TheRingUnit, *this);
      Domain.TheObjects[theIndex]=tempElt;
      NonExplored.RemoveSelection(theIndex);
    }
  }
  root left, right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      root& current = this->theWeyl.RootSystem.TheObjects[theIndex];
      for (int j=0; j<theDimension; j++)
      { left.MakeEi(theDimension, j);
        for (int k=0; k<2; k++, left.MinusRoot())
        { right= current-left;
          if (this->theWeyl.IsARoot(right))
          { int leftIndex= this->theWeyl.RootSystem.IndexOfObjectHash(left);
            int rightIndex= this->theWeyl.RootSystem.IndexOfObjectHash(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSimpleLieAlgebra& leftDomainElt=Domain.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightDomainElt= Domain.TheObjects[rightIndex];
              this->LieBracket(leftDomainElt, rightDomainElt, Domain.TheObjects[theIndex]);
              ElementSimpleLieAlgebra& leftRangeElt=Range.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightRangeElt= Range.TheObjects[rightIndex];
              this->LieBracket(leftRangeElt, rightRangeElt, Range.TheObjects[theIndex]);
              NonExplored.RemoveSelection(theIndex);
            }
          }
        }
      }
    }
  }
  roots vectorsLeft, vectorsRight;
  vectorsLeft.SetSizeExpandOnTopNoObjectInit(Range.size);
  vectorsRight.SetSizeExpandOnTopNoObjectInit(Range.size);
  if (!useNegativeRootsFirst)
    for (int i=0; i<Range.size; i++)
    { Range.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsRight.TheObjects[i]);
      Domain.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsLeft.TheObjects[i]);
    }
  else
    for (int i=0; i<Range.size; i++)
    { Range.TheObjects[i].ElementToVectorNegativeRootSpacesFirst(vectorsRight.TheObjects[i]);
      Domain.TheObjects[i].ElementToVectorNegativeRootSpacesFirst(vectorsLeft.TheObjects[i]);
    }
  outputAuto.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight, theGlobalVariables);
}

void MatrixLargeRational::MakeLinearOperatorFromDomainAndRange(roots& domain, roots& range, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational A;
  MatrixLargeRational B;
  A.AssignRootsToRowsOfMatrix(domain);
  B.AssignRootsToRowsOfMatrix(range);
  A.Invert(theGlobalVariables);
  (*this)=A*B;
  this->Transpose(theGlobalVariables);
}

//void SemisimpleLieAlgebra::ComputeD

bool SemisimpleLieAlgebra::IsInTheWeightSupport(root& theWeight, root& highestWeight, GlobalVariables& theGlobalVariables)
{ root correspondingDominant= theWeight;
  this->theWeyl.RaiseToHighestWeight(correspondingDominant);

  root theDiff= highestWeight - correspondingDominant;
  correspondingDominant.ComputeDebugString();
  highestWeight.ComputeDebugString();
  theDiff.ComputeDebugString();
  if (!theDiff.IsPositiveOrZero())
    return false;
  return true;
}

bool WeylGroup::IsDominantWeight(root& theWeight)
{ int theDimension= this->CartanSymmetric.NumRows;
  root tempRoot;
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (this->RootScalarCartanRoot(tempRoot, theWeight)<0)
      return false;
  }
  return true;
}

void SemisimpleLieAlgebra::GenerateWeightSupport(root& theHighestWeight, roots& output, GlobalVariables& theGlobalVariables)
{ int indexFirstNonExplored=0;
  this->theWeyl.RaiseToHighestWeight(theHighestWeight);
  output.size=0;
  output.AddObjectOnTop(theHighestWeight);
  roots simpleBasis;
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  simpleBasis.MakeEiBasis(theDimension);
  root current;
  while (indexFirstNonExplored<output.size)
  { for (int i=0; i<theDimension; i++)
    { current= output.TheObjects[indexFirstNonExplored]-simpleBasis.TheObjects[i];
      current.ComputeDebugString();
      if (this->IsInTheWeightSupport(current, theHighestWeight, theGlobalVariables))
        output.AddOnTopNoRepetition(current);
    }
    indexFirstNonExplored++;
  }
}

void WeylGroup::RaiseToHighestWeight(root& theWeight)
{ root correspondingDominant;
  for (int i=0; i<this->size; i++)
  { correspondingDominant= theWeight;
    this->ActOnRootByGroupElement(i, correspondingDominant, false, false);
    if (this->IsDominantWeight(correspondingDominant))
    { theWeight=correspondingDominant;
      break;
    }
  }
}

void SemisimpleLieAlgebra::GenerateOneMonomialPerWeightInTheWeightSupport(root& theHighestWeight, GlobalVariables& theGlobalVariables)
{ roots weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport, theGlobalVariables);
  List<bool> Explored;
  Explored.initFillInObject(weightSupport.size, false);
  this->VermaMonomials.SetSizeExpandOnTopNoObjectInit(1);
  this->VermaMonomials.TheObjects[0].MakeConst(theHighestWeight, Rational::TheRingUnit, this);
  roots simpleBasis;
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  simpleBasis.MakeEiBasis(theDimension);
  root current, tempRoot;
  VermaModuleMonomial tempMon;
  for (int indexLowestNonExplored=0; indexLowestNonExplored<this->VermaMonomials.size; indexLowestNonExplored++)
  { Explored.TheObjects[indexLowestNonExplored]=true;
    for (int i=0; i<theDimension; i++)
    { this->VermaMonomials.TheObjects[indexLowestNonExplored].GetWeight(current);
      current.ComputeDebugString();
      current-=simpleBasis.TheObjects[i];
      current.ComputeDebugString();
      int index=weightSupport.IndexOfObject(current);
      if (index!=-1)
        if (!Explored.TheObjects[index])
        { Explored.TheObjects[index]=true;
          this->VermaMonomials.TheObjects[indexLowestNonExplored].MultiplyBySimpleGenerator(i, tempMon);
          this->VermaMonomials.AddObjectOnTop(tempMon);
        }
    }
  }
}

void VermaModuleMonomial::GetWeight(root& output)
{ int theDimension= this->owner->theWeyl.CartanSymmetric.NumRows;
  roots simpleBasis;
  simpleBasis.MakeEiBasis(theDimension);
  output=this->theHighestWeight;
  for (int i=0; i<this->theSimpleGenerators.size; i++)
    output-=simpleBasis.TheObjects[this->theSimpleGenerators.TheObjects[i]]*this->thePowers.TheObjects[i];
}

void VermaModuleMonomial::MultiplyBySimpleGenerator(int index, VermaModuleMonomial& output)
{ output=*this;
  bool tempBool=true;
  if (this->theSimpleGenerators.size==0)
    tempBool=false;
  else
    tempBool=(*this->theSimpleGenerators.LastObject()==index);
  if (tempBool)
    (*output.thePowers.LastObject())++;
  else
  { output.theSimpleGenerators.AddObjectOnTop(index);
    output.thePowers.AddObjectOnTop(1);
  }
}

void VermaModuleMonomial::MakeConst(root& highestWeight, Rational& theCoeff, SemisimpleLieAlgebra* theOwner)
{ this->theHighestWeight=highestWeight;
  this->coeff= theCoeff;
  this->owner=theOwner;
  this->thePowers.size=0;
  this->theSimpleGenerators.size=0;
}

void VermaModuleMonomial::operator=(const VermaModuleMonomial& right)
{ this->owner= right.owner;
  this->theHighestWeight= right.theHighestWeight;
  this->thePowers= right.thePowers;
  this->theSimpleGenerators= right.theSimpleGenerators;
  this->coeff= right.coeff;
}

void SemisimpleLieAlgebra::ElementToStringVermaMonomials(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->VermaMonomials.size; i++)
  { VermaModuleMonomial& tempMon= this->VermaMonomials.TheObjects[i];
    out << "$" << tempMon.ElementToString() << "$\n\n";
  }
  output=out.str();
}

void VermaModuleMonomial::ElementToString(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->theSimpleGenerators.size; i++)
  { int thePower= this->thePowers.TheObjects[i];
    int theGen= this->theSimpleGenerators.TheObjects[i];
    if (thePower>1)
      out << "(";
    out << "g^{-\\alpha_{" << theGen+1 << "}}";
    if (thePower>1)
      out << ")^{" << thePower << "}";
  }
  if (this->theSimpleGenerators.size>0)
    out << "\\cdot";
  out << " v";
  output=out.str();
}

void SemisimpleLieAlgebra::CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(root& theHighestWeight, GlobalVariables& theGlobalVariables)
{ roots weightSupport;
  this->GenerateWeightSupport(theHighestWeight, weightSupport, theGlobalVariables);
  int highestWeight, distanceToHW;
  this->EmbeddingsRootSpaces.SetSizeExpandOnTopNoObjectInit(this->theWeyl.RootSystem.size);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  List<bool> Explored;
  Explored.initFillInObject(this->theWeyl.RootSystem.size, false);
  int numExplored=0;
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
  { root& current= this->theWeyl.RootSystem.TheObjects[i];
    if (current.SumCoordinates()==1 || current.SumCoordinates()==-1)
    { numExplored++;
      Explored.TheObjects[i]=true;
      MatrixLargeRational& currentMat= this->EmbeddingsRootSpaces.TheObjects[i];
      currentMat.init(weightSupport.size, weightSupport.size);
      currentMat.NullifyAll();
      for (int j=0; j<weightSupport.size; j++)
      { int indexTarget= weightSupport.IndexOfObject(current+weightSupport.TheObjects[j]);
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
  roots simpleBasis;
  simpleBasis.MakeEiBasis(theDimension);
  while (numExplored<this->theWeyl.RootSystem.size)
  { for (int i=0; i<this->theWeyl.RootSystem.size; i++)
      if (Explored.TheObjects[i])
        for (int j=0; j<this->theWeyl.RootSystem.size; j++)
          if (Explored.TheObjects[j])
          { root tempRoot= this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j];
            if (this->theWeyl.IsARoot(tempRoot))
            { int index= this->theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
              if (!Explored.TheObjects[index])
              { Explored.TheObjects[index]=true;
                numExplored++;
                this->EmbeddingsRootSpaces.TheObjects[index]= this->EmbeddingsRootSpaces.TheObjects[i];
                this->EmbeddingsRootSpaces.TheObjects[index].LieBracketWith(this->EmbeddingsRootSpaces.TheObjects[j]);
              }
            }
          }
  }
  this->EmbeddingsCartan.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
  { MatrixLargeRational& current= this->EmbeddingsCartan.TheObjects[i];
    current.init(weightSupport.size, weightSupport.size);
    current.NullifyAll();
    root tempRoot;
    tempRoot.MakeEi(theDimension, i);
    for (int j=0; j<weightSupport.size; j++)
      current.elements[j][j]=this->theWeyl.RootScalarCartanRoot(tempRoot, weightSupport.TheObjects[j]);
  }
}

int SemisimpleLieAlgebra::GetLengthStringAlongAlphaThroughBeta(root& alpha, root& beta, int& distanceToHighestWeight, roots& weightSupport)
{ root tempRoot=beta;
  for (int i=0; ; i++)
  { tempRoot+= alpha;
    if (!weightSupport.ContainsObject(tempRoot))
    { distanceToHighestWeight=i;
      break;
    }
  }
  for (int i=0; ; i++)
  { tempRoot-= alpha;
    if (!weightSupport.ContainsObject(tempRoot))
      return i;
  }
//  assert(false);
//  return -1;
}

void SemisimpleLieAlgebra::ElementToStringEmbedding(std::string& output)
{ std::stringstream out;
  std::string tempS;
  for (int i=0; i<this->EmbeddingsRootSpaces.size; i++)
  { MatrixLargeRational& theMat = this->EmbeddingsRootSpaces.TheObjects[i];
    theMat.ElementToString(tempS, false, true);
    out << " $g^{\\alpha_{" << i+1 << "}}\\mapsto$ " << tempS << " \n\n";
  }
  for (int i=0; i<this->EmbeddingsCartan.size; i++)
  { MatrixLargeRational& theMat = this->EmbeddingsCartan.TheObjects[i];
    theMat.ElementToString(tempS, false, true);
    out << " $h_{" << i+1 << "}\\mapsto$ " << tempS << " \n\n";
  }
  output = out.str();
}

void MatrixLargeRational::LieBracketWith(MatrixLargeRational& right)
{ MatrixLargeRational tempMat, tempMat2, tempMat3;
  tempMat2=((*this)*right );
  tempMat2.ComputeDebugString();
  tempMat3= (right*(*this));
  tempMat3.ComputeDebugString();
  tempMat=tempMat2-tempMat3;
  tempMat.ComputeDebugString();
  this->Assign(tempMat);
}

bool roots::LinSpanContainsRoot(root& input, GlobalVariables& theGlobalVariables)
{ roots tempRoots;
  tempRoots.CopyFromBase(*this);
  tempRoots.AddObjectOnTop(input);
  this->ComputeDebugString();
  tempRoots.ComputeDebugString();
  input.ComputeDebugString();
  return this->GetRankOfSpanOfElements(theGlobalVariables)==tempRoots.GetRankOfSpanOfElements(theGlobalVariables);
}

bool SemisimpleLieAlgebra::CheckClosedness(std::string& output, GlobalVariables& theGlobalVariables)
{ List<MatrixLargeRational> tempElts;
  tempElts.size=0;
  tempElts.AddListOnTop(this->EmbeddingsCartan);
  tempElts.AddListOnTop(this->EmbeddingsRootSpaces);
  roots tempRoots;
  tempRoots.SetSizeExpandOnTopNoObjectInit(tempElts.size);
  for (int i=0; i<tempElts.size; i++)
    tempElts.TheObjects[i].MatrixToRoot(tempRoots.TheObjects[i]);
  MatrixLargeRational tempMat;
  for (int i=0; i<tempElts.size; i++)
    for (int j=0; j<tempElts.size; j++)
    { tempMat= tempElts.TheObjects[i];
      tempMat.ComputeDebugString();
      tempElts.TheObjects[j].ComputeDebugString();
      tempMat.LieBracketWith(tempElts.TheObjects[j]);
      tempMat.ComputeDebugString();
      root tempRoot;
      tempMat.MatrixToRoot(tempRoot);
      bool isGood=tempRoots.LinSpanContainsRoot(tempRoot, theGlobalVariables);
      if (!isGood)
      { std::stringstream out;
        std::string tempS1, tempS2, tempS3;
        tempMat.ElementToString(tempS1, false, true);
        tempElts.TheObjects[i].ElementToString(tempS2, false, true);
        tempElts.TheObjects[j].ElementToString(tempS3, false, true);
        out << "First bad: " << tempS1 << "=[" << tempS2 << "," << tempS3 << "]";
        output=out.str();
        return false;
      }
    }
  return true;
}

bool MatrixLargeRational::IsProportionalTo(MatrixLargeRational& right)
{ Rational coeff=0;
  bool found=false;
  for (int i=0; i<this->NumRows; i++)
    for(int j=0; j<this->NumCols; j++)
      if (this->elements[i][j]==0)
      { if (right.elements[i][j]!=0)
          return false;
      } else
      { if (!found)
        { found=true;
          coeff = right.elements[i][j]/this->elements[i][j];
        } else
        { if (coeff!=(right.elements[i][j]/this->elements[i][j]))
            return false;
        }
      }
  return true;
}

void MatrixLargeRational::MatrixToRoot(root& output)
{ output.SetSizeExpandOnTopLight(this->NumRows*this->NumCols);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      output.TheObjects[i*this->NumRows+j]=this->elements[i][j];
}

void rootSubalgebra::GenerateAutomorphismsPreservingBorel(GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup& outputAutomorphisms)
{ this->ComputeAll();
  this->GenerateIsomorphismsPreservingBorel(*this, theGlobalVariables, &outputAutomorphisms, false);
}

void Lattice::DuflosComputation(List<char>& WeylLetters, List<int>& ranks, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream body, tables;
  std::string tempBody, tempTable;
  tables << "\\documentclass{article}\n\\usepackage{amssymb}\n\\begin{document}\n";
  for (int i=0; i<WeylLetters.size; i++)
  { this->DuflosComputationOneSA(WeylLetters.TheObjects[i], ranks.TheObjects[i], tempTable, tempBody, theGlobalVariables);
    tables << tempTable << "\n\n";
    body << tempBody;
  }
  tables << body.str();
  tables << "\n\\end{document}";
  output = tables.str();
}

void Lattice::DuflosComputationOneSA(char WeylLetter, int rank, std::string& outputTable, std::string& outputBody, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::stringstream niceTable;
  rootSubalgebras theRootSubalgebras;
  theRootSubalgebras.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, WeylLetter, rank, true, true);
  WeylGroup& theWeyl= theRootSubalgebras.AmbientWeyl;
  int theDimension= theWeyl.CartanSymmetric.NumRows;
  niceTable << "\\begin{tabular}{cc}\n\\multicolumn{2}{c}{Root system $\\Delta$ of type" << theRootSubalgebras.TheObjects[0].theDynkinDiagram.DebugString << "} \\\\\\hline\n Dynkin type subsystem $\\Delta'$ & Structure of $\\Lambda(\\Delta)/\\Lambda(\\Delta')$\\\\\\hline\n";
  out << "\n\nRoot system of "<< theRootSubalgebras.TheObjects[0].theDynkinDiagram.DebugString << ":\n\n";
  roots tempRoots;
  tempRoots.CopyFromBase(theWeyl.RootSystem);
  out << tempRoots.ElementToString() << "\n\n";
  for (int i=1; i<theRootSubalgebras.size; i++)
  { rootSubalgebra& currentSA=theRootSubalgebras.TheObjects[i];
    if (currentSA.SimpleBasisK.size==theDimension)
    { this->LatticeBasis= currentSA.SimpleBasisK;
      this->GetZnModLatticeRepresentativesRootCase(theWeyl, this->RepresentativesQuotient, theGlobalVariables);
      //std::string tempS;
      out << "\n\nType subsystem: " << currentSA.theDynkinDiagram.DebugString << "\n\n Cardinality quotient: " << this->RepresentativesQuotient.size;
      out << "\n\nSimple basis root subsystem: " << currentSA.SimpleBasisK.ElementToString();
      out << "\n\nRepresentatives elements in quotient: " << this->RepresentativesQuotient.ElementToString();
      std::string tempS;
      List<int> list1, list2;
      this->GetStructureQuotientRootCase(currentSA.AmbientWeyl, tempS, list1, list2, theGlobalVariables);
      out << "\n\nStructure: " << tempS;
      niceTable << currentSA.theDynkinDiagram.DebugString << " & " << tempS <<" \\\\\n";
    }
  }
  niceTable << "\\end{tabular}";
  outputTable = niceTable.str();
  outputBody = out.str();
}

void ComputationSetup::DuflosComputation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  Lattice tempLattice;
  List<char> WeylLetters;
  List<int> ranks;
 /* WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(8);
*/
  WeylLetters.AddObjectOnTop('G');
  ranks.AddObjectOnTop(2);
  WeylLetters.AddObjectOnTop('D');
  ranks.AddObjectOnTop(4);
  WeylLetters.AddObjectOnTop('F');
  ranks.AddObjectOnTop(4);
  WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(6);
  WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(7);
  WeylLetters.AddObjectOnTop('E');
  ranks.AddObjectOnTop(8);
  tempLattice.DuflosComputation(WeylLetters, ranks, tempS, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=tempS;
  theGlobalVariables.MakeReport();
}

bool Lattice::IsInLattice(const root& input)
{ roots tempRoots;
  tempRoots = this->LatticeBasis;
  int theDimension = this->LatticeBasis.size;
  tempRoots.AddObjectOnTop(input);
  MatrixLargeRational tempMat;
  tempRoots.GetLinearDependence(tempMat);
  Rational coeffInFrontOfInput= tempMat.elements[theDimension][0];
  tempMat.ComputeDebugString();
  tempRoots.ComputeDebugString();
  if (coeffInFrontOfInput==0)
    return false;
  for (int i=0; i<theDimension; i++)
    if (!(tempMat.elements[i][0]/coeffInFrontOfInput).IsInteger())
      return false;
  return true;
}

void Lattice::GetZnModLatticeRepresentatives(WeylGroup* theWeyl, roots& representativesOutput, GlobalVariables& theGlobalVariables)
{ int theDimension= this->LatticeBasis.size;
  if (theDimension<1)
    return;
  if (theDimension!=this->LatticeBasis.TheObjects[0].size)
    return;
  if (theDimension!=this->LatticeBasis.GetRankOfSpanOfElements(theGlobalVariables))
    return;
  representativesOutput.size=0;
  root tempRoot;
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (!this->ContainsConjugacyClassRepresentedBy(representativesOutput, tempRoot))
      representativesOutput.AddObjectOnTop(tempRoot);
  }
  for (int lowestNonExploredIndex=0; lowestNonExploredIndex<representativesOutput.size; lowestNonExploredIndex++)
  { //root& current= representativesOutput.TheObjects[lowestNonExploredIndex];
    for (int k=0; k<=lowestNonExploredIndex; k++)
    { tempRoot= representativesOutput.TheObjects[lowestNonExploredIndex]+representativesOutput.TheObjects[k];
      if (!this->ContainsConjugacyClassRepresentedBy(representativesOutput, tempRoot))
      { if (theWeyl!=0)
          for (int l=0; l<theWeyl->RootSystem.size; l++)
          { if (this->IsInLattice(tempRoot- theWeyl->RootSystem.TheObjects[l]))
            { representativesOutput.AddObjectOnTop(theWeyl->RootSystem.TheObjects[l]);
              break;
            }
          }
        else
          representativesOutput.AddObjectOnTop(tempRoot);
      }
    }
  }
}

bool Lattice::ContainsConjugacyClassRepresentedBy(roots& representatives, root& input)
{ for (int i=0; i<representatives.size; i++)
    if (this->IsInLattice(representatives.TheObjects[i]-input))
      return true;
  return false;
}

void Lattice::GetStructureQuotientRootCase(WeylGroup& theWeyl, std::string& output, List<int>& outputIndices, List<int>& outputMults, GlobalVariables& theGlobalVariables)
{ Lattice tempLattice;
  std::stringstream out;
  tempLattice.LatticeBasis=this->LatticeBasis;
  tempLattice.GetZnModLatticeRepresentativesRootCase(theWeyl, theGlobalVariables);
  int maxRank=0;
  rootSubalgebra tempSA;
  tempSA.AmbientWeyl.Assign(theWeyl);
  tempSA.genK=tempLattice.LatticeBasis;
  tempSA.ComputeAll();
  outputIndices.size=0;
  outputMults.size=0;
  out << "$";
  for (int indexMaxRank=tempLattice.GetIndexFirstElementOfMaxRank(maxRank); maxRank>1; indexMaxRank=tempLattice.GetIndexFirstElementOfMaxRank(maxRank))
  { if (outputIndices.size>0)
      out << "+";
    if (outputIndices.ContainsObject(maxRank))
      outputMults.TheObjects[outputIndices.IndexOfObject(maxRank)]++;
    else
    { outputIndices.AddObjectOnTop(maxRank);
      outputMults.AddObjectOnTop(1);
    }
    out << "\\mathbb{Z}_{" << maxRank << "}";
    tempSA.genK.AddObjectOnTop(tempLattice.RepresentativesQuotient.TheObjects[indexMaxRank]);
    tempSA.ComputeAllButAmbientWeyl();
    tempLattice.LatticeBasis= tempSA.SimpleBasisK;
    tempLattice.GetZnModLatticeRepresentativesRootCase(theWeyl, theGlobalVariables);
  }
  out << "$";
  output=out.str();
}

int Lattice::GetIndexFirstElementOfMaxRank(int& outputRank)
{ int result=-1;
  outputRank=0;
  for (int i=0; i<this->RepresentativesQuotient.size; i++)
    if (outputRank< this->GetRankElementRepresentedBy(this->RepresentativesQuotient.TheObjects[i]))
    { outputRank=this->GetRankElementRepresentedBy(this->RepresentativesQuotient.TheObjects[i]);
      result=i;
    }
  return result;
}

int Lattice::GetRankElementRepresentedBy(root& elementRepresentative)
{ root tempRoot;
  tempRoot.MakeZero(elementRepresentative.size);
  for (int result=1; ; result++)
  { tempRoot+=elementRepresentative;
    if (this->IsInLattice(tempRoot))
      return result;
  }
//  assert(false);
//  return -1;
}

ElementUniversalEnveloping Parser::ParseAndCompute(const std::string& input, GlobalVariables& theGlobalVariables)
{ this->Parse(input);
  this->Evaluate(theGlobalVariables);
  this->theValue.UEElement.Simplify();
  return this->theValue.UEElement;
}

std::string Parser::ParseEvaluateAndSimplify(const std::string& input, GlobalVariables& theGlobalVariables)
{ this->theHmm.theRange.ComputeChevalleyConstants(this->DefaultWeylLetter, this->DefaultWeylRank, theGlobalVariables);
  this->Parse(input);
  this->ComputeDebugString(theGlobalVariables);
  this->Evaluate(theGlobalVariables);
  this->theValue.UEElement.Simplify();
  std::stringstream out;
  out << "<DIV class=\"math\" scale=\"50\">\\begin{eqnarray*}&&" << this->StringBeingParsed << "\\end{eqnarray*} = </div>" << this->theValue.ElementToStringValueOnly(true);
  return out.str();
}

int DebugCounter=-1;

void Parser::ParserInit(const std::string& input)
{ this->TokenStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->MakeActualSizeAtLeastExpandOnTop(input.size());
  this->TokenStack.size=0;
  this->ValueStack.size=0;
  this->TokenBuffer.size=0;
  this->ValueBuffer.size=0;
  this->size=0;
  std::string buffer;
  int theLength=(signed) input.size();
  char LookAheadChar;
  for (int i=0; i<theLength; i++)
  { buffer.push_back(input[i]);
    if (i<theLength-1)
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if (this->IsAWordSeparatingCharacter(buffer[0]) || this->IsAWordSeparatingCharacter(LookAheadChar))
    { this->LookUpInDictionaryAndAdd(buffer);
      buffer="";
    }
  }
  this->ValueStack.size=0;
  this->TokenStack.size=0;
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(this->ValueBuffer.size);
  this->TokenStack.MakeActualSizeAtLeastExpandOnTop(this->TokenBuffer.size);
  this->StringBeingParsed=input;
  for (int i=0; i<this->numEmptyTokensAtBeginning; i++)
  { this->TokenStack.AddObjectOnTop(this->tokenEmpty);
    this->ValueStack.AddObjectOnTop(0);
  }
}

bool Parser::IsAWordSeparatingCharacter(char c)
{ switch (c)
  { case '0': return true;
    case '1': return true;
    case '2': return true;
    case '3': return true;
    case '4': return true;
    case '5': return true;
    case '6': return true;
    case '7': return true;
    case '8': return true;
    case '9': return true;
    case '*': return true;
    case '}': return true;
    case '{': return true;
    case '[': return true;
    case ']': return true;
    case '(': return true;
    case ',': return true;
    case ')': return true;
    case '^': return true;
    case '+': return true;
    case '-': return true;
    case '_': return true;
    case '/': return true;
    case ' ': return true;
    default: return false;
  }
//  return false;
}

bool Parser::LookUpInDictionaryAndAdd(std::string& input)
{ switch (input.at(0))
  { case '0': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '1': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(1); return true;
    case '2': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(2); return true;
    case '3': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(3); return true;
    case '4': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(4); return true;
    case '5': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(5); return true;
    case '6': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(6); return true;
    case '7': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(7); return true;
    case '8': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(8); return true;
    case '9': this->TokenBuffer.AddObjectOnTop(Parser::tokenDigit); this->ValueBuffer.AddObjectOnTop(9); return true;
    case '*': this->TokenBuffer.AddObjectOnTop(Parser::tokenTimes); this->ValueBuffer.AddObjectOnTop(9); return true;
    case '}': this->TokenBuffer.AddObjectOnTop(Parser::tokenCloseCurlyBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '{': this->TokenBuffer.AddObjectOnTop(Parser::tokenOpenCurlyBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '[': this->TokenBuffer.AddObjectOnTop(Parser::tokenOpenLieBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ']': this->TokenBuffer.AddObjectOnTop(Parser::tokenCloseLieBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '(': this->TokenBuffer.AddObjectOnTop(Parser::tokenOpenBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ',': this->TokenBuffer.AddObjectOnTop(Parser::tokenComma); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ')': this->TokenBuffer.AddObjectOnTop(Parser::tokenCloseBracket); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '^': this->TokenBuffer.AddObjectOnTop(Parser::tokenPower); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '+': this->TokenBuffer.AddObjectOnTop(Parser::tokenPlus); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '-': this->TokenBuffer.AddObjectOnTop(Parser::tokenMinus); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '_': this->TokenBuffer.AddObjectOnTop(Parser::tokenUnderscore); this->ValueBuffer.AddObjectOnTop(0); return true;
    case '/': this->TokenBuffer.AddObjectOnTop(Parser::tokenDivide); this->ValueBuffer.AddObjectOnTop(0); return true;
    default: break;
  }
  if (input=="x")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenX);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="d")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenPartialDerivative);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="g")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenG);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input =="h")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenH);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="c")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenC);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input =="i")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenMap);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="n")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenVariable);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input=="gcd")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionGCD);
    return true;
  }
  if (input=="lcm")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionLCM);
    return true;
  }
  if (input=="eigen")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionEigen);
    return true;
  }
  if (input =="secretSauce")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(this->functionSecretSauce);
    return true;
  }
  if (input=="dim")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionWeylDimFormula);
    return true;
  }
  if (input=="outerAuto")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(Parser::functionOuterAutos);
    return true;
  }
  if (input=="mod")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(Parser::functionMod);
    return true;
  }
  return false;
}

void Parser::Parse(const std::string& input)
{ this->ParserInit(input);
  this->ParseNoInit(0, this->TokenBuffer.size-1);
  this->ComputeNumberOfVariablesAndAdjustNodes();
}

void Parser::ParseNoInit(int indexFrom, int indexTo)
{ if (indexFrom<0 || indexTo>= this->TokenBuffer.size)
    return;
  for (int i=indexFrom; i<=indexTo; i++)
  { this->ValueStack.AddObjectOnTop(this->ValueBuffer.TheObjects[i]);
    this->TokenStack.AddObjectOnTop(this->TokenBuffer.TheObjects[i]);
    int lookAheadToken=this->tokenEnd;
    if (i<this->TokenBuffer.size-1)
      lookAheadToken=this->TokenBuffer.TheObjects[i+1];
    while(this->ApplyRules(lookAheadToken))
    {}
  }
}

bool Parser::TokenProhibitsUnaryMinus(int theToken)
{ if (theToken==this->tokenExpression)
    return true;
  return false;
}

bool Parser::lookAheadTokenProhibitsPlus(int theToken)
{ if (theToken==this->tokenEnd)
    return false;
  if (theToken==this->tokenPlus)
    return false;
  if (theToken==this->tokenMinus)
    return false;
  if (theToken==this->tokenCloseBracket)
    return false;
  if (theToken==this->tokenCloseLieBracket)
    return false;
  if (theToken==this->tokenCloseCurlyBracket)
    return false;
  if (theToken==this->tokenComma)
    return false;
//  if (theToken==this->tokenExpression)
//    return true;
//  if (theToken==this->tokenUnderscore)
//    return true;
  return true;
}

bool Parser::lookAheadTokenProhibitsTimes(int theToken)
{ if (theToken==this->tokenPower)
    return true;
  if (theToken==this->tokenUnderscore)
    return true;
  return false;
}

bool Parser::ApplyRules(int lookAheadToken)
{ if (this->TokenStack.size<=this->numEmptyTokensAtBeginning)
    return false;
  int tokenLast=*this->TokenStack.LastObject();
  int tokenSecondToLast= this->TokenStack.TheObjects[this->TokenStack.size-2];
  int tokenThirdToLast=this->TokenStack.TheObjects[this->TokenStack.size-3];
  int tokenFourthToLast=this->TokenStack.TheObjects[this->TokenStack.size-4];
  int tokenFifthToLast=this->TokenStack.TheObjects[this->TokenStack.size-5];
//  int tokenSixthToLast=this->TokenStack.TheObjects[this->TokenStack.size-6];
  if (tokenLast==this->tokenEmpty)
  { this->PopTokenAndValueStacksLast();
    return true;
  }
  if (tokenLast==this->tokenCloseCurlyBracket && tokenThirdToLast==this->tokenOpenCurlyBracket && tokenSecondToLast==this->tokenExpression)
  { this->PopTokenAndValueStacksLast();
    this->PopTokenAndValueStacksShiftDown(this->TokenStack.size-2);
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenMap && lookAheadToken!=this->tokenUnderscore)
  { this->AddMapOnTop();
    return true;
  }
  if (tokenLast== this->tokenFunctionNoArgument)
  { this->ExtendOnTop(1);
    this->LastObject()->Operation=this->tokenFunction;
    this->LastObject()->intValue=*this->ValueStack.LastObject();
    *this->TokenStack.LastObject()=this->tokenExpression;
    *this->ValueStack.LastObject()=this->size-1;
    return true;
  }
  if (tokenSecondToLast==this->tokenFunction && tokenLast==this->tokenExpression)
  { this->AddFunctionOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenMinus && !this->TokenProhibitsUnaryMinus(tokenThirdToLast) && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddUnaryMinusOnTop();
    return true;
  }
  if (tokenLast==this->tokenCloseBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenOpenBracket)
  { this->PopTokenAndValueStacksLast();
    this->PopTokenAndValueStacksShiftDown(this->TokenStack.size-2);
    return true;
  }
  if (tokenLast== this->tokenCloseLieBracket && tokenSecondToLast==this->tokenExpression && tokenThirdToLast==this->tokenComma && tokenFourthToLast==this->tokenExpression && tokenFifthToLast==this->tokenOpenLieBracket)
  { this->AddLieBracketOnTop();
    return true;
  }
  if (tokenLast==this->tokenDigit)
  { *this->TokenStack.LastObject()=this->tokenInteger;
    if (tokenSecondToLast!=this->tokenInteger)
      this->LargeIntegerReader=*this->ValueStack.LastObject();
    else
      this->MergeLastTwoIntegers();
    return true;
  }
  if (tokenLast==this->tokenInteger && lookAheadToken!=this->tokenDigit)
  { this->AddIntegerOnTopConvertToExpression();
    return true;
  }
  if (tokenLast==this->tokenPartialDerivative || tokenLast==this->tokenG || tokenLast==this->tokenX || tokenLast==this->tokenH || tokenLast== this->tokenC || tokenLast==this->tokenVariable)
  { this->AddLetterExpressionOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenUnderscore && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && tokenFourthToLast!=this->tokenUnderscore)
  { this->AddIndexingExpressionOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast== this->tokenPower && tokenThirdToLast==this->tokenExpression)
  { this->AddPowerOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast== this->tokenDivide && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->AddDivideOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && tokenSecondToLast==tokenTimes && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->PopTokenAndValueStacksShiftDown(this->TokenStack.size-2);
    this->AddTimesOnTop();
    return true;
  }
  if (tokenLast==this->tokenExpression && tokenSecondToLast==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->AddTimesOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenPlus && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddPlusOnTop();
    return true;
  }
  if (tokenSecondToLast==this->tokenMinus && tokenLast==this->tokenExpression && tokenThirdToLast==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddMinusOnTop();
    return true;
  }
  int rootDim;
  if (this->StackTopIsARoot(rootDim))
  { this->AddRootOnTop(rootDim);
    return true;
  }
  return false;
}

bool Parser::StackTopIsARoot(int& outputDimension)
{ if (*this->TokenStack.LastObject()!=this->tokenCloseBracket)
    return false;
  outputDimension=0;
  for (int i=this->TokenStack.size-2; i>=1; i--)
  { if (this->TokenStack.TheObjects[i]!=this->tokenExpression)
      return false;
    i--;
    outputDimension++;
    if (this->TokenStack.TheObjects[i]==this->tokenOpenBracket)
      return true;
    if (this->TokenStack.TheObjects[i]!=this->tokenComma)
      return false;
  }
  return false;
}

void Parser::DecreaseStackSetExpressionLastNode(int Decrease)
{ this->TokenStack.size-=Decrease;
  this->ValueStack.size-=Decrease;
  *this->TokenStack.LastObject()=this->tokenExpression;
  *this->ValueStack.LastObject()=this->size-1;
}

void Parser::MergeLastTwoIntegers()
{ this->LargeIntegerReader=this->LargeIntegerReader*10+(*this->ValueStack.LastObject());
  this->PopTokenAndValueStacksLast();
}

void Parser::AddPowerOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Clear();
  this->LastObject()->Operation=this->tokenPower;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddLetterExpressionOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Clear();
  this->LastObject()->Operation=*this->TokenStack.LastObject();
  this->DecreaseStackSetExpressionLastNode(0);
}

void Parser::AddIntegerOnTopConvertToExpression()
{ this->ExtendOnTop(1);
  this->LastObject()->rationalValue=this->LargeIntegerReader;
  this->LastObject()->Operation=this->tokenInteger;
  this->DecreaseStackSetExpressionLastNode(0);
}

void Parser::AddFunctionOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Operation=this->TokenStack.TheObjects[this->TokenStack.size-2];
  this->LastObject()->intValue=this->ValueStack.TheObjects[this->ValueStack.size-2];
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddIndexingExpressionOnTop()
{ this->ExtendOnTop(1);
  this->LastObject()->Operation = this->tokenUnderscore;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddPlusOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenPlus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddDivideOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenDivide;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddMinusOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMinus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void Parser::AddUnaryMinusOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMinusUnary;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddMapOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMap;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddTimesOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenTimes;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-2], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void Parser::AddLieBracketOnTop()
{ this->ExtendOnTop(1);
  ParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenLieBracket;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-4], this->ValueStack.TheObjects[this->ValueStack.size-2]);
  this->DecreaseStackSetExpressionLastNode(4);
}

void Parser::ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  this->Parse(input);
  out << "\\begin{eqnarray*}&&" << input << " = \\\\\n";
  this->Evaluate(theGlobalVariables);
//  this->WeylAlgebraValue.ElementToString(tempS, false, true, false);
  out << tempS;
  out << "\\end{eqnarray*}";
  output=out.str();
}

void Parser::AddRootOnTop(int theDimension)
{ this->ExtendOnTop(1);
  ParserNode& lastNode=*this->LastObject();
  lastNode.Operation=this->tokenRoot;
  lastNode.children.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
  { int indexChild=this->ValueStack.TheObjects[this->ValueStack.size-2-2*i];
    lastNode.children.TheObjects[theDimension-1-i]=indexChild;
    this->TheObjects[indexChild].indexParent=this->size-1;
  }
  this->DecreaseStackSetExpressionLastNode(theDimension*2);
}

void Parser::Own(int indexParent, int indexChild1)
{ ParserNode* theNode= &this->TheObjects[indexParent];
  theNode->children.SetSizeExpandOnTopNoObjectInit(1);
  theNode->children.TheObjects[0]=indexChild1;
  this->TheObjects[indexChild1].indexParent= indexParent;
}

void Parser::Own(int indexParent, int indexChild1, int indexChild2)
{ ParserNode* theNode= & this->TheObjects[indexParent];
  theNode->children.SetSizeExpandOnTopNoObjectInit(2);
  theNode->children.TheObjects[0]=indexChild1;
  theNode->children.TheObjects[1]=indexChild2;
  this->TheObjects[indexChild1].indexParent= indexParent;
  this->TheObjects[indexChild2].indexParent= indexParent;
}

void Parser::Evaluate(GlobalVariables& theGlobalVariables)
{ if (this->TokenStack.size== this->numEmptyTokensAtBeginning+1)
    if (*this->TokenStack.LastObject()==this->tokenExpression)
    { this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].Evaluate(theGlobalVariables);
//      this->WeylAlgebraValue.Assign(this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].WeylAlgebraElement);
//      this->LieAlgebraValue= this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]].LieAlgebraElement;
      this->theValue=this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]];
    }
  if (this->TokenStack.size>this->numEmptyTokensAtBeginning+1)
    this->theValue.SetError(ParserNode::errorBadSyntax);
//  this->WeylAlgebraValue.ComputeDebugString(false, false);
}

void Parser::ExtendOnTop(int numNew)
{ this->SetSizeExpandOnTopNoObjectInit(this->size+numNew);
  for (int i=0; i<numNew; i++)
  { this->TheObjects[this->size-1-i].owner=this;
    this->TheObjects[this->size-1-i].Clear();
  }
}

void ParserNode::Evaluate(GlobalVariables& theGlobalVariables)
{ //this->UEElement.ComputeDebugString();
  this->Evaluated=true;
  for (int i=0; i<this->children.size; i++)
  { if (this->Operation==Parser::tokenMap)
      this->ContextLieAlgebra=&this->owner->theHmm.theDomain;
    this->owner->TheObjects[this->children.TheObjects[i]].ContextLieAlgebra=this->ContextLieAlgebra;
    this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeError)
    { this->CopyError(this->owner->TheObjects[this->children.TheObjects[i]]);
      return;
    }
  }
  switch (this->Operation)
  { case Parser::tokenPlus: this->EvaluatePlus(theGlobalVariables); break;
    case Parser::tokenMinus: this->EvaluateMinus(theGlobalVariables); break;
    case Parser::tokenMinusUnary: this->EvaluateMinusUnary(theGlobalVariables); break;
    case Parser::tokenTimes: this->EvaluateTimes(theGlobalVariables); break;
    case Parser::tokenG: break;
    case Parser::tokenH: break;
    case Parser::tokenVariable: break;
    case Parser::tokenC: this->ExpressionType=this->typeUEelement; this->UEElement.MakeCasimir(*this->ContextLieAlgebra, this->owner->NumVariables, theGlobalVariables); break;
    case Parser::tokenDivide: this->EvaluateDivide(theGlobalVariables); break;
    case Parser::tokenUnderscore: this->EvaluateUnderscore(theGlobalVariables); break;
    case Parser::tokenPartialDerivative: this->ExpressionType=this->typeWeylAlgebraElement; break;
    case Parser::tokenX: this->ExpressionType=this->typeWeylAlgebraElement; break;
    case Parser::tokenInteger: this->EvaluateInteger(theGlobalVariables); break;
    case Parser::tokenLieBracket: this->EvaluateLieBracket(theGlobalVariables); break;
    case Parser::tokenPower: this->EvaluateThePower(theGlobalVariables); break;
    case Parser::tokenMap: this->EvaluateEmbedding(theGlobalVariables); break;
    case Parser::tokenFunction: this->EvaluateFunction(theGlobalVariables); break;
    case Parser::tokenRoot: this->ExpressionType=this->typeRoot; break;
    default: this->SetError(this->errorUnknownOperation); return;
  }
}

void ParserNode::EvaluateInteger(GlobalVariables& theGlobalVariables)
{ if (this->rationalValue.Extended!=0)
    this->ExpressionType=this->typeRational;
  else
  { this->ExpressionType= this->typeIntegerOrIndex;
    this->intValue= this->rationalValue.NumShort;
  }
}

void ParserNode::EvaluateThePower(GlobalVariables& theGlobalVariables)
{ this->ExpressionType=this->typeError;
  if (this->children.size!=2 || !this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  if (rightNode.ExpressionType!=this->typeIntegerOrIndex)
  { if ((rightNode.ExpressionType==this->typeRational || rightNode.ExpressionType==this->typePoly) && leftNode.ExpressionType==this->typeUEelement)
      if (leftNode.UEElement.IsAPowerOfASingleGenerator())
        { rightNode.ConvertToType(this->typePoly);
          leftNode.UEElement.SetNumVariables(this->owner->NumVariables);
          MonomialUniversalEnveloping tempMon;
          tempMon.operator=(leftNode.UEElement.TheObjects[0]);
          tempMon.Powers.TheObjects[0].MultiplyBy(rightNode.polyValue);
          this->UEElement.Nullify(*this->ContextLieAlgebra);
          this->UEElement.AddMonomial(tempMon);
          this->ExpressionType=this->typeUEelement;
          return;
        }
    this->SetError(this->errorDunnoHowToDoOperation);
    return;
  }
  int thePower= rightNode.intValue;
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
      this->rationalValue=leftNode.intValue;
      this->rationalValue.RaiseToPower(thePower);
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typeRational:
      this->rationalValue=leftNode.rationalValue;
      this->rationalValue.RaiseToPower(thePower);
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typePoly:
      this->polyValue=leftNode.polyValue;
      this->polyValue.RaiseToPower(thePower);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->UEElement=leftNode.UEElement;
      this->UEElement.RaiseToPower(thePower);
      this->ExpressionType=this->typeUEelement;
    break;
    default: this->ExpressionType=this->typeError; return;
  }
}

void ParserNode::EvaluateUnderscore(GlobalVariables& theGlobalVariables)
{ this->ExpressionType=this->typeError;
  if (this->children.size!=2)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  if (rightNode.ExpressionType!=this->typeIntegerOrIndex)
  { this->SetError(this->errorBadIndex);
    return;
  }
  int theIndex= rightNode.intValue;
  int theDimension= this->ContextLieAlgebra->theWeyl.CartanSymmetric.NumRows;
  if (leftNode.Operation==Parser::tokenH)
  { theIndex--;
    if (theIndex>=theDimension || theIndex<0)
    { this->SetError(this->errorBadIndex);
      return;
    }
    root tempRoot;
    tempRoot.MakeEi(theDimension, theIndex);
    this->UEElement.AssignElementCartan(tempRoot, this->owner->NumVariables, *this->ContextLieAlgebra);
    this->ExpressionType=this->typeUEelement;
    return;
  }
  if (leftNode.Operation==Parser::tokenG)
  { theIndex=this->ContextLieAlgebra->DisplayIndexToRootIndex(theIndex);
    theIndex=this->ContextLieAlgebra->RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(theIndex);
    if (theIndex<0 || theIndex>this->ContextLieAlgebra->theWeyl.RootSystem.size+theDimension)
    { this->SetError(this->errorBadIndex);
      return;
    }
    this->UEElement.MakeOneGeneratorCoeffOne(theIndex, this->owner->NumVariables, *this->ContextLieAlgebra);
    this->ExpressionType=this->typeUEelement;
    return;
  }
  if (leftNode.Operation==Parser::tokenX)
    leftNode.Operation=Parser::tokenVariable;
  if (leftNode.Operation==Parser::tokenVariable)
  { if (theIndex<1 || theIndex>1000)
    { this->SetError(this->errorBadIndex);
      return;
    }
    this->owner->NumVariables=MathRoutines::Maximum(theIndex, this->owner->NumVariables);
    this->polyValue.MakeMonomialOneLetter((short)this->owner->NumVariables, theIndex-1, 1, (Rational) 1);
    this->ExpressionType=this->typePoly;
    return;
  }
}

bool ParserNode::ConvertChildrenToType(int theType)
{ for (int i=0; i<this->children.size; i++)
  { ParserNode& current= this->owner->TheObjects[this->children.TheObjects[i]];
    if (!current.ConvertToType(theType))
      return false;
  }
  return true;
}

bool ParserNode::ConvertToType(int theType)
{ if (this->ExpressionType==this->typeError)
    return false;
  if (theType==this->typeUndefined)
    return false;
  if (this->ExpressionType==this->typePoly)
      if (this->polyValue.NumVars< this->owner->NumVariables)
        this->polyValue.SetNumVariablesSubDeletedVarsByOne((short)this->owner->NumVariables);
  if (this->ExpressionType==this->typeUEelement)
    this->UEElement.SetNumVariables(this->owner->NumVariables);
  if (this->ExpressionType==theType)
    return true;
  if (this->ExpressionType> theType)
    return false;
  //we have eliminated the corner cases. Time to do some real conversion :)
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { if (theType==this->typeRational)
      this->rationalValue= this->intValue;
    if (theType==this->typePoly)
      this->polyValue.MakeNVarConst((short) this->owner->NumVariables, (Rational) this->intValue);
    if (theType==this->typeUEelement)
      this->UEElement.AssignInt(this->intValue, (short)this->owner->NumVariables, *this->ContextLieAlgebra);
  }
  if (this->ExpressionType==this->typeRational)
  { if (theType==this->typePoly)
      this->polyValue.MakeNVarConst((short)this->owner->NumVariables, this->rationalValue);
    if (theType==this->typeUEelement)
      this->UEElement.MakeConst(this->rationalValue, this->owner->NumVariables, *this->ContextLieAlgebra);
  }
  if (this->ExpressionType==this->typePoly)
    if (theType==this->typeUEelement)
      this->UEElement.MakeConst(this->polyValue, *this->ContextLieAlgebra);
  this->ExpressionType=theType;
  return true;
}

void ParserNode::InitForAddition()
{ this->intValue=0;
  this->rationalValue.MakeZero();
  this->polyValue.Nullify((short)this->owner->NumVariables);
  this->UEElement.Nullify(*this->ContextLieAlgebra);
//  this->WeylAlgebraElement.Nullify(this->owner->NumVariables);
}

void ParserNode::InitForMultiplication()
{ this->intValue=1;
  this->polyValue.MakeNVarConst((short)this->owner->NumVariables, (Rational)1);
  this->rationalValue=1;
  this->UEElement.AssignInt(1, this->owner->NumVariables, *this->ContextLieAlgebra);
}

int ParserNode::GetStrongestExpressionChildrenConvertChildrenIfNeeded()
{ int result=this->typeUndefined;
  for (int i=0; i<this->children.size; i++)
  { int childExpressionType=this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType;
    if (childExpressionType>result)
      result=childExpressionType;
  }
  for (int i=0; i<this->children.size; i++)
    if(!this->owner->TheObjects[this->children.TheObjects[i]].ConvertToType(result))
    { this->SetError(this->owner->TheObjects[this->children.TheObjects[i]].ErrorType);
      return this->typeError;
    }
  return result;
}

void ParserNode::EvaluateFunction(GlobalVariables& theGlobalVariables)
{ switch(this->intValue)
  { case Parser::functionGCD: this->EvaluateGCDorLCM(theGlobalVariables); break;
    case Parser::functionLCM: this->EvaluateGCDorLCM(theGlobalVariables); break;
    case Parser::functionEigen: this->EvaluateEigen(theGlobalVariables); break;
    case Parser::functionSecretSauce: this->EvaluateSecretSauce(theGlobalVariables); break;
    case Parser::functionWeylDimFormula: this->EvaluateWeylDimFormula(theGlobalVariables); break;
    case Parser::functionOuterAutos: this->EvaluateOuterAutos(theGlobalVariables); break;
    case Parser::functionMod: this->EvaluateModVermaRelations(theGlobalVariables); break;
    default: this->SetError(this->errorUnknownOperation); break;
  }
}

void ParserNode::EvaluateModVermaRelations(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (!theArgument.ConvertChildrenToType(this->typeUEelement) )
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  this->UEElement=theArgument.UEElement;
  this->UEElement.Simplify();
  this->UEElement.ModOutVermaRelations();
  this->ExpressionType=this->typeUEelement;
}

void ParserNode::EvaluateOuterAutos(GlobalVariables& theGlobalVariables)
{ MatrixLargeRational tempMat;
  std::stringstream out;
  this->ExpressionType=this->typeString;
  this->owner->theHmm.theRange.ComputeOneAutomorphism(theGlobalVariables, tempMat, true);
  MatrixLargeRational tempMat2=tempMat;
  Rational tempRat;
  tempMat2.ComputeDeterminantOverwriteMatrix(tempRat);
  out << "<br>one outer automorphism of the Lie algebra is realized as the following matrix. <br> The coordinates of the matrix are given in the ordered basis ";
  out << " <div class=\"math\" scale=\"50\"> g_{-n}, \\dots, g_{-1}, h_1,\\dots, h_k, g_1,\\dots, g_n</div> where the generators are as in the table on the right.<br> Its determinant is \n" << tempRat.ElementToString();
  out << "<div class=\"math\" scale=\"50\">" << tempMat.ElementToString(false, true) << "</div>";
  this->outputString=out.str();
}

void ParserNode::EvaluateWeylDimFormula(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  int theDimension= theArgument.children.size;
  HomomorphismSemisimpleLieAlgebra& theHmm= this->owner->theHmm;
  if (!theArgument.ConvertChildrenToType(this->typeRational) || theDimension!=theHmm.theRange.theWeyl.CartanSymmetric.NumRows)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  root theWeight;
  theWeight.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theDimension; i++)
  { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
    theWeight.TheObjects[i]=current.rationalValue;
  }
  this->rationalValue= theHmm.theRange.theWeyl.WeylDimFormula(theWeight, theGlobalVariables);
  this->ExpressionType=this->typeRational;
}

void ParserNode::EvaluateEigen(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  int theDimension= theArgument.children.size;
  HomomorphismSemisimpleLieAlgebra& theHmm= this->owner->theHmm;
  if (theArgument.GetStrongestExpressionChildrenConvertChildrenIfNeeded()!=this->typeIntegerOrIndex || theDimension!=theHmm.theDomain.theWeyl.CartanSymmetric.NumRows)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  List<ElementUniversalEnveloping> theList;
  root theWeight;
  theWeight.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theDimension; i++)
  { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
    theWeight.TheObjects[i]=current.intValue;
  }
  this->outputString= this->owner->theHmm.WriteAllUEMonomialsWithWeightWRTDomain(theList, theWeight, theGlobalVariables);
  this->ExpressionType=this->typeUndefined;
}

void ParserNode::ConvertChildrenAndMyselfToStrongestExpressionChildren()
{ this->ExpressionType=this->GetStrongestExpressionChildrenConvertChildrenIfNeeded();
}

void ParserNode::EvaluatePlus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  this->InitForAddition();
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
      case ParserNode::typePoly: this->polyValue.AddPolynomial(currentChild.polyValue); break;
      case ParserNode::typeUEelement: this->UEElement+=currentChild.UEElement; break;
      default: this->ExpressionType=this->typeError; return;
    }
  }
}

void ParserNode::EvaluateMinus(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->InitForAddition();
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex: if (i==0) this->intValue+=currentChild.intValue; else this->intValue-=currentChild.intValue; break;
      case ParserNode::typeRational: if (i==0) this->rationalValue+=currentChild.rationalValue; else this->rationalValue-=currentChild.rationalValue; break;
      case ParserNode::typePoly: if(i==0) this->polyValue.AddPolynomial(currentChild.polyValue); else this->polyValue.Subtract(currentChild.polyValue); break;
      case ParserNode::typeUEelement: if (i==0) this->UEElement+=currentChild.UEElement; else this->UEElement-=currentChild.UEElement; break;
      default: this->ExpressionType=this->typeError; return;
    }
  }
}

void ParserNode::EvaluateMinusUnary(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->InitForAddition();
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[0]];
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: this->intValue-=currentChild.intValue; break;
    case ParserNode::typeRational: this->rationalValue-=currentChild.rationalValue; break;
    case ParserNode::typePoly: this->polyValue.Subtract(currentChild.polyValue); break;
    case ParserNode::typeUEelement: this->UEElement-=currentChild.UEElement; break;
    default: this->ExpressionType=this->typeError; return;
  }
}

bool ParserNode::AllChildrenAreOfDefinedNonErrorType()
{ for (int i=0; i<this->children.size; i++)
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeError || this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType==this->typeUndefined)
      return false;
  return true;
}

void ParserNode::EvaluateSecretSauce(GlobalVariables& theGlobalVariables)
{ EigenVectorComputation theComp;
  this->outputString=theComp.ComputeAndReturnString(theGlobalVariables, *this->owner);
  this->ExpressionType=this->typeString;
}

void ParserNode::EvaluateLieBracket(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=2 || !this->AllChildrenAreOfDefinedNonErrorType())
  { this->ExpressionType=this->typeError;
    return;
  }
  this->ExpressionType=this->typeUEelement;
  for (int i=0; i<this->children.size; i++)
  { ParserNode& current= this->owner->TheObjects[this->children.TheObjects[i]];
    current.ConvertToType(this->typeUEelement);
  }
  ElementUniversalEnveloping& left= this->owner->TheObjects[this->children.TheObjects[0]].UEElement;
  ElementUniversalEnveloping& right= this->owner->TheObjects[this->children.TheObjects[1]].UEElement;
  left.LieBracketOnTheRight(right, this->UEElement);
}

void ParserNode::EvaluateEmbedding(GlobalVariables& theGlobalVariables)
{ if (!this->children.size==1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& child= this->owner->TheObjects[this->children.TheObjects[0]];
  if (child.ExpressionType!=this->typeUEelement)
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  if (! this->owner->theHmm.ApplyHomomorphism(child.UEElement, this->UEElement, theGlobalVariables))
  { this->SetError(this->errorBadIndex);
    return;
  }
  this->ExpressionType=this->typeUEelement;
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
  if (theRootNode.ExpressionType!=this->typeRoot)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  if (theTrueChildren.size!=2)
  { this->SetError(this->errorWrongNumberOfArguments);
    return;
  }
  this->ExpressionType=theRootNode.GetStrongestExpressionChildrenConvertChildrenIfNeeded();
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
      { leftNode.rationalValue.GetNumUnsigned(tempUI1);
        rightNode.rationalValue.GetNumUnsigned(tempUI2);
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
      if (leftNode.polyValue.IsEqualToZero() || rightNode.polyValue.IsEqualToZero())
      { this->SetError(this->errorDivisionByZero);
        return;
      }
      if (theFunction==Parser::functionGCD)
        RationalFunction::gcd(leftNode.polyValue, rightNode.polyValue, this->polyValue);
      else
        RationalFunction::lcm(leftNode.polyValue, rightNode.polyValue, this->polyValue);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->SetError(errorDunnoHowToDoOperation);
    break;
    default: this->SetError(this->errorDunnoHowToDoOperation); return;
  }
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
  this->InitForMultiplication();
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  if (rightNode.ExpressionType==this->typeIntegerOrIndex)
  { rightNode.ExpressionType=this->typeRational;
    rightNode.rationalValue=rightNode.intValue;
  }
  if (rightNode.ExpressionType!=this->typeRational)
  { this->SetError(this->errorDivisionByNonAllowedType);
    return;
  }
  if (rightNode.rationalValue.IsEqualToZero())
  { this->SetError(this->errorDivisionByZero);
    return;
  }
  Rational& theDenominator= rightNode.rationalValue;
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
      this->rationalValue=leftNode.intValue;
      this->rationalValue/=theDenominator;
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typeRational:
      this->rationalValue=leftNode.rationalValue;
      this->rationalValue/=theDenominator;
      this->ExpressionType=this->typeRational;
      break;
    case ParserNode::typePoly:
      this->polyValue=leftNode.polyValue;
      this->polyValue.DivByRational(theDenominator);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeUEelement:
      this->UEElement=leftNode.UEElement;
      this->UEElement/=theDenominator;
      this->ExpressionType=this->typeUEelement;
    break;
    default: this->SetError(this->errorDivisionByNonAllowedType); return;
  }
}

void ParserNode::EvaluateTimes(GlobalVariables& theGlobalVariables)
{ if (!this->AllChildrenAreOfDefinedNonErrorType())
  { this->SetError(this->errorOperationByUndefinedOrErrorType);
    return;
  }
  this->InitForMultiplication();
  this->ConvertChildrenAndMyselfToStrongestExpressionChildren();
  LargeInt theInt;
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentChild=this->owner->TheObjects[this->children.TheObjects[i]];
    switch (this->ExpressionType)
    { case ParserNode::typeIntegerOrIndex:
        theInt=this->intValue;
        theInt*=currentChild.intValue;
        if (theInt.value.size>1)
        { this->ExpressionType= this->typeRational;
          this->rationalValue=theInt;
        } else
          this->intValue=theInt.value.TheObjects[0]*theInt.sign;
      break;
      case ParserNode::typeRational: this->rationalValue*=currentChild.rationalValue; break;
      case ParserNode::typePoly: this->polyValue.MultiplyBy(currentChild.polyValue); break;
      case ParserNode::typeUEelement: this->UEElement*=currentChild.UEElement; break;
      default: this->SetError(this->errorMultiplicationByNonAllowedTypes); return;
    }
  }
}

ParserNode::ParserNode()
{ this->owner=0;
  this->Clear();
}

void Parser::ElementToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  out << "String: " << this->StringBeingParsed << "\n";
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
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS);
    out << " Index: " << i << " " << tempS << ";\n";
    if (useHtml)
      out << "<br>";
  }
  if (useHtml)
    out << "<br><br>";
  out << "\n\nValue: " << this->theValue.ElementToStringValueOnly(true);

//  this->WeylAlgebraValue.ComputeDebugString(false, false);
//  this->LieAlgebraValue.ComputeDebugString(this->theLieAlgebra, false, false);
//  out << "\n\nWeyl algebra value: " << this->WeylAlgebraValue.DebugString;
//  out << "\nLie algebra value: " << this->LieAlgebraValue.DebugString;
  if (!useHtml)
  { out << "\nAmbient Lie algebra details:\n";
    out << this->theHmm.theRange.ElementToStringLieBracketPairing();
  }
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
    case Parser::tokenMap: out << "i"; break;
    case Parser::tokenMinusUnary: out << "-"; break;
    case Parser::tokenVariable: out << "n"; break;
    case Parser::tokenRoot: out << "root"; break;
    case Parser::tokenFunction: out << "function"; break;
    case Parser::tokenFunctionNoArgument: out << "functionNoArgument"; break;
    default: out << "?"; break;
  }
}

void Parser::Clear()
{ this->theValue.Clear();
  this->theValue.UEElement.Nullify(this->theHmm.theRange);
  this->TokenStack.size=0;
  this->ValueStack.size=0;
}

void Parser::ComputeNumberOfVariablesAndAdjustNodes()
{ this->NumVariables=0;
  for(int i=0; i<this->size; i++)
    if (this->NumVariables< this->TheObjects[i].WeylAlgebraElement.NumVariables)
      this->NumVariables=this->TheObjects[i].WeylAlgebraElement.NumVariables;
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].WeylAlgebraElement.SetNumVariablesPreserveExistingOnes(this->NumVariables);
}

std::string ParserNode::ElementToStringValueOnly(bool useHtml)
{ std::stringstream out;
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { if (!useHtml)
      out << " an integer of value: " << this->intValue;
    else
      out << " an integer of value: <div class=\"math\">" << this->intValue << "</div>";
  }
  if (this->ExpressionType==this->typeRational)
  { if (!useHtml)
      out << " a rational number of value: " << this->rationalValue.ElementToString();
    else
      out << " a rational number of value: <div class=\"math\">" << this->rationalValue.ElementToString() << "</div>";
  }
  if (this->ExpressionType==this->typeRoot)
  { out << " is a an ordered tuple ";
  }
  if (this->ExpressionType==this->typePoly)
  { if (!useHtml)
      out << " a polynomial of value: " << this->polyValue.ElementToString();
    else
      out << " a polynomial of value: <div class=\"math\">\\begin{eqnarray*}&&" << this->polyValue.ElementToString() << "\\end{eqnarray*}</div>";
  }
  if (this->ExpressionType==this->typeUEelement)
  { if (!useHtml)
      out << " an element of U(g) of value: " << this->UEElement.ElementToString();
    else
      out << " an element of U(g) of value: <div class=\"math\">\\begin{eqnarray*}&&" << this->UEElement.ElementToString(true) << "\\end{eqnarray*}</div>";
  }
  if (this->outputString!="")
    out << "a printout: " << this->outputString;
  if (this->ExpressionType==this->typeError)
    out << this->ElementToStringErrorCode(useHtml);
  return out.str();
}

std::string ParserNode::ElementToStringErrorCode(bool useHtml)
{ std::stringstream out;
  switch (this->ErrorType)
  { case ParserNode::errorBadIndex: out << "error: bad index"; break;
    case ParserNode::errorBadOrNoArgument: out << "error: bad or no argument"; break;
    case ParserNode::errorBadSyntax: out << "error: bad syntax."; break;
    case ParserNode::errorDunnoHowToDoOperation: out << "error: my master hasn't taught me how to do this operation (maybe he doesn't know how either)"; break;
    case ParserNode::errorDivisionByZero: out << "error: division by zero"; break;
    case ParserNode::errorDivisionByNonAllowedType: out << "error: division of/by non-allowed type"; break;
    case ParserNode::errorMultiplicationByNonAllowedTypes: out << "error: multiplication by non-allowed types"; break;
    case ParserNode::errorNoError: out << "error: error type claims no error, but expression type claims error. Slap the programmer."; break;
    case ParserNode::errorOperationByUndefinedOrErrorType: out << "error: operation with an undefined type"; break;
    case ParserNode::errorProgramming: out << "error: there has been some programming mistake (it's not your expression's fault). Slap the programmer!"; break;
    case ParserNode::errorUnknownOperation: out << "error: unknown operation. The lazy programmer has added the operation to the dictionary, but hasn't implemented it yet. Lazy programmers deserve no salary. "; break;
    default: out << "Non-documented error. Lazy programmers deserve no salaries.";
  }
  return out.str();
}

void ParserNode::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  owner->TokenToStringStream(out, this->Operation);
  PolyFormatLocal.MakeAlphabetArbitraryWithIndex("x");
  if (this->ExpressionType==this->typeRational)
    out << " is the rational number " << this->rationalValue.ElementToString();
  if (this->ExpressionType==this->typeIntegerOrIndex)
    out << " is the integer " << this->intValue;
  if (this->ExpressionType==this->typePoly)
  { this->polyValue.ElementToString(tempS);
    out << " is the polynomial " << tempS;
  }
  if (this->ExpressionType==this->typeRoot)
    out << " is a root ";
  if (this->ExpressionType==this->typeUndefined)
    out << " is of type undefined ";
  if (this->ExpressionType==this->typeWeylAlgebraElement)
  { this->WeylAlgebraElement.ElementToString(tempS, false, false, false);
    out << " is the algebra element " << tempS << " ";
  }
  if (this->ExpressionType==this->typeUEelement)
  { this->UEElement.ElementToString(tempS);
    out << " is the universal enveloping algebra element: " << tempS;
  }
  if (this->children.size>0)
  { out << " Its children are: ";
    for (int i=0; i<this->children.size; i++)
      out << this->children.TheObjects[i] << ", ";
  }
  output=out.str();
}

bool HomomorphismSemisimpleLieAlgebra::ComputeHomomorphismFromImagesSimpleChevalleyGenerators(GlobalVariables& theGlobalVariables)
{ int theDomainDimension= this->theDomain.theWeyl.CartanSymmetric.NumRows;
  Selection NonExplored;
  int numRoots= this->theDomain.theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  root domainRoot, rangeRoot;
  this->theDomain.ComputeChevalleyConstants(this->theDomain.theWeyl, theGlobalVariables);
  this->theRange.ComputeChevalleyConstants(this->theRange.theWeyl, theGlobalVariables);
  List<ElementSimpleLieAlgebra> tempDomain, tempRange;
  tempDomain.SetSizeExpandOnTopNoObjectInit(numRoots+theDomainDimension);
  tempRange.SetSizeExpandOnTopNoObjectInit(numRoots+theDomainDimension);
  root tempRoot;
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    for (int j=0; j<2; j++, tempRoot.MinusRoot())
    { int index= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
      tempDomain.TheObjects[index].Nullify(this->theDomain);
      tempDomain.TheObjects[index].SetCoefficient(tempRoot, 1, this->theDomain);
      tempRange.TheObjects[index] = this->imagesSimpleChevalleyGenerators.TheObjects[i+j*theDomainDimension];
      NonExplored.RemoveSelection(index);
//      std::cout <<"<br>" << tempDomain.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//      std::cout <<"->" << tempRange.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
    }
  }
//  std::cout << this->ElementToString(theGlobalVariables) << "<br>";
  ElementSimpleLieAlgebra tempElt;
  root right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      root& current = this->theDomain.theWeyl.RootSystem.TheObjects[theIndex];
      for (int j=0; j<NonExplored.MaxSize; j++)
        if (!NonExplored.selected[j])
        { root& left= this->theDomain.theWeyl.RootSystem.TheObjects[j];
          right= current-left;
//          left.ComputeDebugString(); right.ComputeDebugString(); current.ComputeDebugString();
          if (this->theDomain.theWeyl.IsARoot(right))
          { int leftIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(left);
            int rightIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(right);
            if (!NonExplored.selected[rightIndex])
            { ElementSimpleLieAlgebra& leftDomainElt=tempDomain.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightDomainElt= tempDomain.TheObjects[rightIndex];
              this->theDomain.LieBracket(leftDomainElt, rightDomainElt, tempDomain.TheObjects[theIndex]);
              ElementSimpleLieAlgebra& leftRangeElt=tempRange.TheObjects[leftIndex];
              ElementSimpleLieAlgebra& rightRangeElt= tempRange.TheObjects[rightIndex];
              this->theRange.LieBracket(leftRangeElt, rightRangeElt, tempRange.TheObjects[theIndex]);
              NonExplored.RemoveSelection(theIndex);
              break;
            }
          }
        }
    }
  }
  for (int i=0; i<theDomainDimension; i++)
  { tempRoot.MakeEi(theDomainDimension, i);
    int leftIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
    int rightIndex= this->theDomain.theWeyl.RootSystem.IndexOfObjectHash(-tempRoot);
    this->theDomain.LieBracket(tempDomain.TheObjects[leftIndex], tempDomain.TheObjects[rightIndex], tempDomain.TheObjects[numRoots+i]);
    this->theRange.LieBracket(tempRange.TheObjects[leftIndex], tempRange.TheObjects[rightIndex], tempRange.TheObjects[numRoots+i]);
  }
  roots vectorsLeft, vectorsRight;
  vectorsLeft.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  vectorsRight.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  for (int i=0; i<tempRange.size; i++)
  { tempDomain.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsLeft.TheObjects[i]);
    tempRange.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(vectorsRight.TheObjects[i]);
  }
  MatrixLargeRational tempMat;
  tempMat.MakeLinearOperatorFromDomainAndRange(vectorsLeft, vectorsRight, theGlobalVariables);
  root imageRoot;
  this->domainAllChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  this->imagesAllChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(tempDomain.size);
  for (int i=0; i<this->theDomain.theWeyl.RootSystem.size; i++)
  { tempElt.Nullify(this->theDomain);
    tempElt.SetCoefficient(this->theDomain.theWeyl.RootSystem.TheObjects[i], 1, this->theDomain);
    this->domainAllChevalleyGenerators.TheObjects[this->theDomain.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(i)]=tempElt;
  }
//  std::cout << "Domain type:" << this->theDomain.theWeyl.WeylLetter;
//  std::cout <<  tempMat.ElementToString( true, false);
  for (int i=0; i<theDomainDimension; i++)
  { tempElt.Nullify(this->theDomain);
    tempElt.Hcomponent.MakeEi(theDomainDimension, i);
    this->domainAllChevalleyGenerators.TheObjects[this->theDomain.CartanIndexToChevalleyGeneratorIndex(i)]=tempElt;
  }
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
  { this->domainAllChevalleyGenerators.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(tempRoot);
    tempMat.ActOnAroot(tempRoot, imageRoot);
    this->imagesAllChevalleyGenerators.TheObjects[i].AssingVectorRootSpacesFirstThenCartan(imageRoot, this->theRange);
//    std::cout << this->domainAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//    std::cout << "->" << this->imagesAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
//    std::cout << "<br>";
  }
  return true;
}

std::string HomomorphismSemisimpleLieAlgebra::WriteAllUEMonomialsWithWeightWRTDomain(List<ElementUniversalEnveloping>& output, root& theWeight, GlobalVariables& theGlobalVariables)
{ output.size=0;
//  this->theDomain.theWeyl.
  roots PosRootsEmbeddings, PosRootsProjections;
  root imageOfTheWeight;
  std::stringstream out;
  int theDimension=this->theRange.theWeyl.CartanSymmetric.NumRows;
  int theDomainDimension= this->theDomain.theWeyl.CartanSymmetric.NumRows;
  imageOfTheWeight.MakeZero(theDimension);
  int numPosRootsDomain=this->theDomain.theWeyl.RootsOfBorel.size;
  int numPosRootsRange=this->theRange.theWeyl.RootsOfBorel.size;
  PosRootsEmbeddings.SetSizeExpandOnTopNoObjectInit(numPosRootsDomain);
  for (int i=0; i<numPosRootsDomain; i++)
  { PosRootsEmbeddings.TheObjects[i].MakeZero(theDimension);
    for (int j=0; j<theDomainDimension; j++)
      PosRootsEmbeddings.TheObjects[i]+=this->imagesAllChevalleyGenerators.TheObjects[numPosRootsDomain+j].Hcomponent*this->theDomain.theWeyl.RootsOfBorel.TheObjects[i].TheObjects[j];
  }
  out << "Embeddings of roots:" << PosRootsEmbeddings.ElementToString(false, true, true);
  PosRootsProjections.SetSizeExpandOnTopNoObjectInit(numPosRootsRange);
  for (int i=0; i<numPosRootsRange; i++)
    this->ProjectOntoSmallCartan(this->theRange.theWeyl.RootsOfBorel.TheObjects[i], PosRootsProjections.TheObjects[i], theGlobalVariables);
  out << "<br>Projections of roots: " << PosRootsProjections.ElementToString(false, true, true);
  VectorPartition theVP;
  theVP.PartitioningRoots=PosRootsProjections;
  theVP.theRoot=theWeight;
  theVP.ComputeAllPartitions();
  out << "the partitions: <br>" << theVP.ElementToString(true);
  output.SetSizeExpandOnTopNoObjectInit(theVP.thePartitions.size);
  MonomialUniversalEnveloping currentMon;
  ElementUniversalEnveloping tempElt;
  for (int i=0; i<output.size; i++)
  { currentMon.Nullify(theDomainDimension, this->theRange);
    Rational tempRat=1;
    currentMon.Coefficient.ComputeDebugString();
    currentMon.Coefficient.MakeNVarConst((short)theDimension, tempRat);
    for (int j=0; j<theVP.thePartitions.TheObjects[i].size; j++)
      currentMon.MultiplyByGeneratorPowerOnTheRight(this->theRange.RootToIndexInUE(-this->theRange.theWeyl.RootsOfBorel.TheObjects[j]), theVP.thePartitions.TheObjects[i].TheObjects[j]);
    out << currentMon.ElementToString(false) << "<br>" ;
    tempElt.Nullify(this->theRange);
    tempElt.AddObjectOnTopHash(currentMon);
    output.TheObjects[i]=tempElt;
  }
  List<List<ElementUniversalEnveloping> > targets;
  List<List<ElementUniversalEnveloping> > targetsNoMod;
  targets.SetSizeExpandOnTopNoObjectInit(theDomainDimension);
  targetsNoMod.SetSizeExpandOnTopNoObjectInit(theDomainDimension);
  ElementUniversalEnveloping theSimpleGenerator;
  std::string beginMath = "<DIV class=\"math\" scale=\"50\">";
  std::string endMath = "</DIV>";
  List<List<PolynomialRationalCoeff> > theSystem;
  theSystem.size=0;
  theSystem.SetSizeExpandOnTopNoObjectInit(output.size);
  ElementUniversalEnveloping basisMonomialBuffer;
  for (int i=0; i<targets.size; i++)
  { List<ElementUniversalEnveloping>& currentTargets= targets.TheObjects[i];
    List<ElementUniversalEnveloping>& currentTargetsNoMod= targetsNoMod.TheObjects[i];
    theSimpleGenerator.AssignElementLieAlgebra(this->imagesSimpleChevalleyGenerators.TheObjects[i], theDimension, this->theRange);
    theSimpleGenerator.ComputeDebugString();
    out << "Generator number " << i+1 << ": " << beginMath;
    for (int j=0; j<output.size; j++)
    { theSimpleGenerator.LieBracketOnTheRight(output.TheObjects[j], tempElt);
      tempElt.Simplify();
      currentTargetsNoMod.AddObjectOnTop(tempElt);
      tempElt.ModOutVermaRelations();
      currentTargets.AddObjectOnTop(tempElt);
      out << tempElt.ElementToString() << ", \\quad ";
    }
    out << endMath << "\n<br>";
    out << "Elements before modding out: " << beginMath;
    for (int j=0; j<output.size; j++)
      out << currentTargetsNoMod.TheObjects[j].ElementToString() << ", \\quad ";
    out << endMath << "\n<br>";
    List<rootPoly> tempList;
    //Let the monomials corresponding to the given partition be m_1, \dots, m_l
    //Let the Chevalley generators of the smaller Lie algebra be k_1,\dots, k_s
    //Then the elements [k_i, m_1], \dots, [k_i, m_l] are recorded in this order in currentTargets
    ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(currentTargets, tempList, basisMonomialBuffer, theGlobalVariables);
    out << "Coordinate form of the above elements: ";
    for (int j=0; j<tempList.size; j++)
    { out << tempList.TheObjects[j].ElementToString() << ",";
      //theSystem holds in the j^th row the action on the monomial m_j
      theSystem.TheObjects[j].AddListOnTop(tempList.TheObjects[j]);
    }
    out << "<br>";
  }
  Matrix<RationalFunction> matSystem;
  int numEquations=theSystem.TheObjects[0].size;
  matSystem.init(numEquations, output.size);
  for (int i=0; i<matSystem.NumRows; i++)
    for (int j=0; j<matSystem.NumCols; j++)
      matSystem.elements[i][j]=theSystem.TheObjects[j].TheObjects[i];
  matSystem.ComputeDebugString(false, true);
  out << "<br>The system we need to solve:<br>" << beginMath << matSystem.DebugString << endMath << "<br>";
  RationalFunction ZeroPoly, UnitPoly, MinusUnitPoly;
  ZeroPoly.MakeNVarConst(theDimension, (Rational) 0);
  UnitPoly.MakeNVarConst(theDimension, (Rational) 1);
  MinusUnitPoly.MakeNVarConst(theDimension, (Rational) -1);
  List<List<RationalFunction> > theAnswer;
  matSystem.FindZeroEigenSpacE(theAnswer, UnitPoly, MinusUnitPoly, ZeroPoly, theGlobalVariables);
  out << "The found solutions: <br>";
  rootRationalFunction tempRatRoot;
  std::string tempS;
  for (int i=0; i<theAnswer.size; i++)
  { tempRatRoot.CopyFromBase(theAnswer.TheObjects[i]);
    out << beginMath << tempRatRoot.ElementToString() << endMath << "<br>";
    out << "Corresponding expression in monomial form: " << beginMath;
    for (int j=0; j<output.size; j++)
    { RationalFunction& currentCoeff= theAnswer.TheObjects[i].TheObjects[j];
      if (!currentCoeff.IsEqualToZero())
      { tempS= currentCoeff.ElementToString(true, false);
        if (tempS=="-1")
          out << "-";
        else
        { if (j!=0)
            out << "+";
          if (tempS!="1")
            out << "(" << tempS << ")";
        }
        out << output.TheObjects[j].ElementToString();
      }
    }
    out << endMath << "<br>";
  }
  return out.str();
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan(root& input, root& output, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational invertedSmallCartan;
  invertedSmallCartan=this->theDomain.theWeyl.CartanSymmetric;
  invertedSmallCartan.Invert(theGlobalVariables);
  int theSmallDimension=this->theDomain.theWeyl.CartanSymmetric.NumRows;
  output.MakeZero(theSmallDimension);
  for (int i=0; i<theSmallDimension; i++)
    output.TheObjects[i]= this->theRange.theWeyl.RootScalarCartanRoot(this->imagesAllChevalleyGenerators.TheObjects[this->theDomain.theWeyl.RootsOfBorel.size+i].Hcomponent, input);
  invertedSmallCartan.ActOnAroot(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism(MonomialUniversalEnveloping& input, ElementUniversalEnveloping& output, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping tempElt;
  output.Nullify(this->theRange);
  output.MakeConst(input.Coefficient, this->theRange);
  for (int i=0; i<input.generatorsIndices.size; i++)
  { if (input.generatorsIndices.TheObjects[i]>=this->imagesAllChevalleyGenerators.size)
      return false;
    tempElt.AssignElementLieAlgebra(this->imagesAllChevalleyGenerators.TheObjects[input.generatorsIndices.TheObjects[i]], input.Coefficient.NumVars, this->theRange);
    PolynomialRationalCoeff& thePower=input.Powers.TheObjects[i];
    if (thePower.TotalDegree()>0)
      return false;
    int theIntegralPower=thePower.TheObjects[0].Coefficient.NumShort;
    for (int j=0; j<theIntegralPower; j++)
      output*=tempElt;
  }
  return true;
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism(ElementUniversalEnveloping& input, ElementUniversalEnveloping& output, GlobalVariables& theGlobalVariables)
{ assert(&output!=&input);
  output.Nullify(this->theRange);
  ElementUniversalEnveloping tempElt;
  for (int i=0; i<input.size; i++)
  { if(!this->ApplyHomomorphism(input.TheObjects[i], tempElt, theGlobalVariables))
      return false;
    output+=tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::MakeGinGWithId(char theWeylLetter, int theWeylDim, GlobalVariables& theGlobalVariables)
{ this->theDomain.ComputeChevalleyConstants(theWeylLetter, theWeylDim, theGlobalVariables);
  this->theRange.ComputeChevalleyConstants(theWeylLetter, theWeylDim, theGlobalVariables);
  int numPosRoots=this->theDomain.theWeyl.RootsOfBorel.size;
  this->imagesAllChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(numPosRoots*2+theWeylDim);
  this->domainAllChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(numPosRoots*2+theWeylDim);
  this->imagesSimpleChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(theWeylDim*2);
  for (int i=0; i<2*numPosRoots+theWeylDim; i++)
  { ElementSimpleLieAlgebra& tempElt1=this->imagesAllChevalleyGenerators.TheObjects[i];
    ElementSimpleLieAlgebra& tempElt2=this->domainAllChevalleyGenerators.TheObjects[i];
    tempElt2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, this->theDomain);
    tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, this->theRange);
  }
  for (int i=0; i<theWeylDim; i++)
  { ElementSimpleLieAlgebra& tempElt1=this->imagesSimpleChevalleyGenerators.TheObjects[i];
    tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, this->theRange);
    ElementSimpleLieAlgebra& tempElt2=this->imagesSimpleChevalleyGenerators.TheObjects[theWeylDim+i];
    tempElt2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i+numPosRoots, this->theRange);
  }
//  std::cout << this->ElementToString(theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::MakeG2InB3(Parser& owner, GlobalVariables& theGlobalVariables)
{ owner.DefaultWeylLetter='B';
  owner.DefaultWeylRank=3;
  owner.theHmm.theRange.ComputeChevalleyConstants(owner.DefaultWeylLetter, owner.DefaultWeylRank, theGlobalVariables);
  this->theDomain.ComputeChevalleyConstants('G', 2, theGlobalVariables);
  this->theRange.ComputeChevalleyConstants('B', 3, theGlobalVariables);
  this->imagesSimpleChevalleyGenerators.SetSizeExpandOnTopNoObjectInit(4);
  (owner.ParseAndCompute("g_2", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->imagesSimpleChevalleyGenerators.TheObjects[0]);
  (owner.ParseAndCompute("g_1+g_3", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->imagesSimpleChevalleyGenerators.TheObjects[1]);
  (owner.ParseAndCompute("g_{-2}", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->imagesSimpleChevalleyGenerators.TheObjects[2]);
  (owner.ParseAndCompute("g_{-1}+g_{-3}", theGlobalVariables)).ConvertToLieAlgebraElementIfPossible(this->imagesSimpleChevalleyGenerators.TheObjects[3]);
  this->ComputeHomomorphismFromImagesSimpleChevalleyGenerators(theGlobalVariables);
  owner.Clear();
  this->GetRestrictionAmbientRootSystemToTheSmallerCartanSA(this->RestrictedRootSystem, theGlobalVariables);
  //this->ComputeDebugString(true, theGlobalVariables);
  //std::cout << this->DebugString;
  //if (this->CheckClosednessLieBracket(theGlobalVariables))
  //{ std::cout <<"good good good good!!!!";
  //}
}

void HomomorphismSemisimpleLieAlgebra::ElementToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
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
  { this->imagesSimpleChevalleyGenerators.TheObjects[i].ElementToString(tempS, this->theRange, false, false);
    out << tempS << "\n\n";
    if (useHtml)
      out << "<br>";
  }
  out << "Maps of Chevalley generators:\n\n";
  for (int i=0; i<this->domainAllChevalleyGenerators.size; i++)
  { this->imagesAllChevalleyGenerators.TheObjects[i].ElementToString(tempS, this->theRange, false, false);
    this->domainAllChevalleyGenerators.TheObjects[i].ElementToString(tempS2, this->theDomain, false, false);
    out << tempS2 << " \\mapsto " << tempS << "\n\n";
    if  (useHtml)
      out << "<br>";
  }

  output=out.str();
}

void HomomorphismSemisimpleLieAlgebra::GetRestrictionAmbientRootSystemToTheSmallerCartanSA(roots& output, GlobalVariables& theGlobalVariables)
{ List<root>& theRootSystem= this->theRange.theWeyl.RootSystem;
  int rankSA=this->theDomain.theWeyl.CartanSymmetric.NumRows;
  MatrixLargeRational tempMat;
  tempMat.Assign(this->theDomain.theWeyl.CartanSymmetric);
  tempMat.Invert(theGlobalVariables);
  int numPosRootsDomain=this->theDomain.theWeyl.RootsOfBorel.size;
  output.SetSizeExpandOnTopNoObjectInit(theRootSystem.size);
  root theScalarProducts;
  theScalarProducts.SetSizeExpandOnTopLight(rankSA);
//  tempMat.ComputeDebugString(true, false);
//  std::cout << tempMat.DebugString << "<br>";
  for (int i=0; i<theRootSystem.size; i++)
  { for (int j=0; j<rankSA; j++)
    { ElementSimpleLieAlgebra& currentH=this->imagesAllChevalleyGenerators.TheObjects[j+numPosRootsDomain];
      theScalarProducts.TheObjects[j]=this->theRange.theWeyl.RootScalarCartanRoot(currentH.Hcomponent, theRootSystem.TheObjects[i]);
    }
    tempMat.ActOnAroot(theScalarProducts, output.TheObjects[i]);
  }
}

bool HomomorphismSemisimpleLieAlgebra::CheckClosednessLieBracket(GlobalVariables& theGlobalVariables)
{ ElementSimpleLieAlgebra tempElt;
  roots tempRoots; root tempRoot;
  tempRoots.SetSizeExpandOnTopNoObjectInit(this->imagesAllChevalleyGenerators.size);
  for (int i=0; i<tempRoots.size; i++)
    this->imagesAllChevalleyGenerators.TheObjects[i].ElementToVectorRootSpacesFirstThenCartan(tempRoots.TheObjects[i]);
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
    for (int j=0; j<this->imagesAllChevalleyGenerators.size; j++)
    { this->theRange.LieBracket(this->imagesAllChevalleyGenerators.TheObjects[i], this->imagesAllChevalleyGenerators.TheObjects[j], tempElt);
      tempElt.ElementToVectorRootSpacesFirstThenCartan(tempRoot);
      if(!tempRoots.LinSpanContainsRoot(tempRoot, theGlobalVariables))
        return false;
    }
  return true;
}

void MonomialUniversalEnveloping::SimplifyAccumulateInOutputNoOutputInit(ElementUniversalEnveloping& output)
{ int IndexlowestNonSimplified=0;
  ElementUniversalEnveloping buffer2;
  MonomialUniversalEnveloping tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified<output.size)
  { bool reductionOccurred=false;
    if (output.TheObjects[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred=true;
    else
      for (int i=0; i<output.TheObjects[IndexlowestNonSimplified].generatorsIndices.size-1; i++)
        if (!this->owner->AreOrderedProperly(output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
        { if (output.TheObjects[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute(i, tempMon))
          { output.AddMonomialNoCleanUpZeroCoeff(tempMon);
            tempMon.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft(i, buffer2);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight(i, buffer2);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
        }
    if (reductionOccurred)
      output.PopIndexSwapWithLastHash(IndexlowestNonSimplified);
    else
      IndexlowestNonSimplified++;
    output.ComputeDebugString();
  }
  output.CleanUpZeroCoeff();
}

bool MonomialUniversalEnveloping::CommutingLeftIndexAroundRightIndexAllowed(PolynomialRationalCoeff& theLeftPower, int leftGeneratorIndex, PolynomialRationalCoeff& theRightPower, int rightGeneratorIndex)
{ if (theLeftPower.TotalDegree()==0)
  { if(theRightPower.TotalDegree()==0)
      return true;
    int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
    int theDimension= this->owner->theWeyl.CartanSymmetric.NumRows;
    if(rightGeneratorIndex>= numPosRoots && rightGeneratorIndex<numPosRoots+theDimension)
    { if (this->owner->theLiebracketPairingCoefficients.elements[leftGeneratorIndex][rightGeneratorIndex].IsEqualToZero())
        return true;
      else
        return false;
    } else
      return true;
  }
  return false;
}

bool MonomialUniversalEnveloping::SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex, MonomialUniversalEnveloping& output)
{ if (theLeftIndex>= this->generatorsIndices.size-1)
    return false;
  int leftGenerator=this->generatorsIndices.TheObjects[theLeftIndex];
  int rightGenerator=this->generatorsIndices.TheObjects[theLeftIndex+1];
  if (this->owner->OppositeRootSpaces.TheObjects[leftGenerator]==rightGenerator)
    return false;
  if (this->owner->theLiebracketPairingCoefficients.elements[leftGenerator][rightGenerator].IsEqualToZero())
  { this->ComputeDebugString();
    output.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Nullify(this->Coefficient.NumVars, *this->owner);
    output.Coefficient=this->Coefficient;
    output.ComputeDebugString();
    for (int i=0; i<theLeftIndex; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex+1], this->Powers.TheObjects[theLeftIndex+1]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex], this->Powers.TheObjects[theLeftIndex]);
    for (int i=theLeftIndex+2; i<this->generatorsIndices.size; i++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    return true;
  }
  return false;
}

bool MonomialUniversalEnveloping::CommutingRightIndexAroundLeftIndexAllowed(PolynomialRationalCoeff& theLeftPower, int leftGeneratorIndex, PolynomialRationalCoeff& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

void MonomialUniversalEnveloping::Simplify(ElementUniversalEnveloping& output)
{ output.Nullify(*this->owner);
  output.AddObjectOnTopHash(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output);
}

void MonomialUniversalEnveloping::MultiplyByNoSimplify(const MonomialUniversalEnveloping& other)
{ this->generatorsIndices.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Powers.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(other.Coefficient);
  if (other.generatorsIndices.size==0)
    return;
  int firstIndex=other.generatorsIndices.TheObjects[0];
  int i=0;
  if (this->generatorsIndices.size>0)
    if (firstIndex==(*this->generatorsIndices.LastObject()))
    { *this->Powers.LastObject()+=other.Powers.TheObjects[0];
      i=1;
    }
  for (; i<other.generatorsIndices.size; i++)
  { this->Powers.AddObjectOnTop(other.Powers.TheObjects[i]);
    this->generatorsIndices.AddObjectOnTop(other.generatorsIndices.TheObjects[i]);
  }
}

void MonomialUniversalEnveloping::CommuteConsecutiveIndicesRightIndexAroundLeft(int theIndeX, ElementUniversalEnveloping& output)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(this->Coefficient.NumVars, *this->owner);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndeX= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndeX=this->generatorsIndices.TheObjects[theIndeX];
  PolynomialRationalCoeff theRightPoweR, theLeftPoweR;
  theRightPoweR= this->Powers.TheObjects[theIndeX+1];
  theLeftPoweR= this->Powers.TheObjects[theIndeX];
  theRightPoweR-=1;
  int powerDroP=0;
  PolynomialRationalCoeff acquiredCoefficienT, polyOne;
  acquiredCoefficienT.Assign(this->Coefficient);
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  MonomialUniversalEnveloping startMon;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResulT, tempElT, tempLefttElt;
  adResulT.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(rightGeneratorIndeX, *this->owner);
  tempLefttElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(leftGeneratorIndeX, *this->owner);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1);
  do
  { //acquiredCoefficienT.ComputeDebugString();
    //theRightPoweR.ComputeDebugString();
    //theLeftPoweR.ComputeDebugString();
    //adResulT.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    if (adResulT.NonZeroElements.CardinalitySelection>0)
    { int theNewGeneratorIndex= this->owner->RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(adResulT.NonZeroElements.elements[0]);
      tempMon.Coefficient=acquiredCoefficienT;
      tempMon.Coefficient.TimesConstant(adResulT.coeffsRootSpaces.TheObjects[adResulT.NonZeroElements.elements[0]]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
      tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
      //tempMon.ComputeDebugString();
      output.AddObjectOnTopHash(tempMon);
    } else
    { int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
      int numPosRoots= this->owner->theWeyl.RootsOfBorel.size;
      for (int i=0; i<theDimension; i++)
        if (!adResulT.Hcomponent.TheObjects[i].IsEqualToZero())
        { tempMon=startMon;
          tempMon.Coefficient=acquiredCoefficienT;
          tempMon.Coefficient.TimesConstant(adResulT.Hcomponent.TheObjects[i]);
          tempMon.MultiplyByGeneratorPowerOnTheRight(i+numPosRoots, polyOne);
          tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
          tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
          for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
            tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
          //tempMon.ComputeDebugString();
          output.AddObjectOnTopHash(tempMon);
        }
    }
    acquiredCoefficienT.MultiplyBy(theLeftPoweR);
    theLeftPoweR-=1;
    tempMon=startMon;
    this->owner->LieBracket(tempLefttElt, adResulT, tempElT);
    adResulT=tempElT;
    powerDroP++;
    acquiredCoefficienT.DivByInteger(powerDroP);
    //adResulT.ComputeDebugString(*this->owner, false, false);
  }while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

void MonomialUniversalEnveloping::CommuteConsecutiveIndicesLeftIndexAroundRight(int theIndeX, ElementUniversalEnveloping& output)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(this->Coefficient.NumVars, *this->owner);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndex= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndex=this->generatorsIndices.TheObjects[theIndeX];
  PolynomialRationalCoeff theRightPower, theLeftPower;
  theRightPower= this->Powers.TheObjects[theIndeX+1];
  theLeftPower= this->Powers.TheObjects[theIndeX];
  theLeftPower-=1;
  int powerDrop=0;
  PolynomialRationalCoeff acquiredCoefficient, polyOne;
  acquiredCoefficient.Assign(this->Coefficient);
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theIndeX], theLeftPower);
  MonomialUniversalEnveloping startMon, tempMon2;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResult, tempElt, tempRightElt;
  adResult.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(leftGeneratorIndex, *this->owner);
  tempRightElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(rightGeneratorIndex, *this->owner);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1);
  do
  { //acquiredCoefficient.ComputeDebugString();
    //theRightPower.ComputeDebugString();
    //adResult.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
    //tempMon.ComputeDebugString();
    if (adResult.NonZeroElements.CardinalitySelection>0)
    { int theNewGeneratorIndex= this->owner->RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(adResult.NonZeroElements.elements[0]);
      tempMon.Coefficient=acquiredCoefficient;
      tempMon.Coefficient.TimesConstant(adResult.coeffsRootSpaces.TheObjects[adResult.NonZeroElements.elements[0]]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
      //tempMon.ComputeDebugString();
      output.AddObjectOnTopHash(tempMon);
    } else
    { int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
      int numPosRoots= this->owner->theWeyl.RootsOfBorel.size;
      tempMon.Coefficient=acquiredCoefficient;
      tempMon2=tempMon;
      for (int i=0; i<theDimension; i++)
        if (!adResult.Hcomponent.TheObjects[i].IsEqualToZero())
        { tempMon=tempMon2;
          tempMon.Coefficient.TimesConstant(adResult.Hcomponent.TheObjects[i]);
          tempMon.MultiplyByGeneratorPowerOnTheRight(i+numPosRoots, polyOne);
          for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
            tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
          //tempMon.ComputeDebugString();
          output.AddObjectOnTopHash(tempMon);
        }
    }
    acquiredCoefficient.MultiplyBy(theRightPower);
    theRightPower-=1;
    tempMon=startMon;
    this->owner->LieBracket(adResult, tempRightElt, tempElt);
    adResult=tempElt;
    powerDrop++;
    acquiredCoefficient.DivByInteger(powerDrop);
    //adResult.ComputeDebugString(*this->owner, false, false);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

void MonomialUniversalEnveloping::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower)
{ if (thePower==0)
    return;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst(this->Coefficient.NumVars, thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

void MonomialUniversalEnveloping::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const PolynomialRationalCoeff& thePower)
{ if (thePower.IsEqualToZero())
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { this->Powers.LastObject()->AddPolynomial(thePower);
      return;
    }
  this->Powers.AddObjectOnTop(thePower);
  this->generatorsIndices.AddObjectOnTop(theGeneratorIndex);
}

void ElementUniversalEnveloping::AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnveloping& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
    this->TheObjects[theIndex].Coefficient+=input.Coefficient;
}

void ElementUniversalEnveloping::Simplify()
{ ElementUniversalEnveloping buffer;
  ElementUniversalEnveloping output;
  //this->ComputeDebugString();
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Simplify(buffer);
    output+=buffer;
    //output.ComputeDebugString();
  }
  *this=output;
}

void ElementUniversalEnveloping::LieBracketOnTheRight(const ElementUniversalEnveloping& right, ElementUniversalEnveloping& output)
{ ElementUniversalEnveloping tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

void ElementUniversalEnveloping::AddMonomial(const MonomialUniversalEnveloping& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
  { this->TheObjects[theIndex].Coefficient+=input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

void ElementUniversalEnveloping::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexSwapWithLastHash(i);
      i--;
    }
}

void ElementUniversalEnveloping::MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(theOwner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.Coefficient.MakeNVarConst((short)numVars, (Rational) 1);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddObjectOnTopHash(tempMon);
}

bool ElementUniversalEnveloping::ConvertToLieAlgebraElementIfPossible(ElementSimpleLieAlgebra& output)const
{ output.Nullify(*this->owner);
  int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  Rational tempRat=0;
  PolynomialRationalCoeff tempP;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping& tempMon= this->TheObjects[i];
    tempMon.GetDegree(tempP);
    if (!tempP.IsEqualToOne())
      return false;
    if (tempMon.Coefficient.TotalDegree()>0)
      return false;
    if (tempMon.Coefficient.size>0)
      tempRat=tempMon.Coefficient.TheObjects[0].Coefficient;
    else
      tempRat=0;
    int theGeneratorIndex=tempMon.generatorsIndices.TheObjects[0];
    int correspondingRootIndex=this->owner->ChevalleyGeneratorIndexToRootIndex(theGeneratorIndex);
    if(correspondingRootIndex<0)
    { theGeneratorIndex-=numPosRoots;
      output.Hcomponent.TheObjects[theGeneratorIndex]=tempRat;
    } else
      output.SetCoefficient(this->owner->theWeyl.RootSystem.TheObjects[correspondingRootIndex], tempRat, *this->owner);
  }
  return true;
}

void ElementUniversalEnveloping::AssignElementLieAlgebra(const ElementSimpleLieAlgebra& input, int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Nullify(theOwner);
  this->AssignElementCartan(input.Hcomponent, numVars, theOwner);
  MonomialUniversalEnveloping tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.generatorsIndices.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.TheObjects[0].MakeNVarConst((short)numVars, 1);
  for (int i=0; i<input.NonZeroElements.CardinalitySelection; i++)
  { int theIndex=input.NonZeroElements.elements[i];
    int theGeneratorIndex=theOwner.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(theIndex);
    tempMon.Coefficient.MakeNVarConst((short)numVars, input.coeffsRootSpaces.TheObjects[theIndex]);
    tempMon.generatorsIndices.TheObjects[0]=theGeneratorIndex;
    this->AddObjectOnTopHash(tempMon);
  }
}

void ElementUniversalEnveloping::ModOutVermaRelations()
{ MonomialUniversalEnveloping tempMon;
  ElementUniversalEnveloping output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelations();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

void ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(List<ElementUniversalEnveloping>& theElements, List<rootPoly>& outputCoordinates, ElementUniversalEnveloping& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping tempMon;
  int numVars= outputCorrespondingMonomials.owner->theWeyl.CartanSymmetric.NumRows;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddObjectOnTopNoRepetitionOfObjectHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSizeExpandOnTopNoObjectInit(theElements.size);
  PolynomialRationalCoeff ZeroPoly;
  ZeroPoly.Nullify((short)numVars);
  for (int i=0; i<theElements.size; i++)
  { rootPoly& current=outputCoordinates.TheObjects[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnveloping& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping& currentMon=currentElt.TheObjects[j];
      current.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
}

void ElementUniversalEnveloping::AssignElementCartan(const root& input, int numVars, SemisimpleLieAlgebra& theOwner)
{ MonomialUniversalEnveloping tempMon;
  this->Nullify(theOwner);
  tempMon.Nullify(numVars, theOwner);
  int theDimension= theOwner.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theOwner.theWeyl.RootsOfBorel.size;
  tempMon.generatorsIndices.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.SetSizeExpandOnTopNoObjectInit(1);
  for (int i=0; i<theDimension; i++)
    if (!input.TheObjects[i].IsEqualToZero())
    { (*tempMon.generatorsIndices.LastObject())=i+numPosRoots;
      tempMon.Powers.LastObject()->MakeNVarConst((short)numVars, 1);
      tempMon.Coefficient.MakeNVarConst((short)numVars, input.TheObjects[i]);
      this->AddObjectOnTopHash(tempMon);
    }
}

void ElementUniversalEnveloping::ElementToString(std::string& output, bool useLatex)
{ std::stringstream out;
  std::string tempS;
  if (this->size==0)
    out << "0";
  int IndexCharAtLastLineBreak=0;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping& current=this->TheObjects[i];
    tempS=current.ElementToString(false);
    if (i!=0)
      if (tempS.size()>0)
        if (tempS[0]!='-')
          out << '+';
    out << tempS;
    if (((int)out.tellp())- IndexCharAtLastLineBreak> 150)
    { IndexCharAtLastLineBreak=out.tellp();
      out << "\\\\&&";
    }
  }
  output=out.str();
}

void MonomialUniversalEnveloping::SetNumVariables(int newNumVars)
{ if (this->Coefficient.NumVars==newNumVars)
    return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((short)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((short)newNumVars);
}

std::string MonomialUniversalEnveloping::ElementToString(bool useLatex)
{ std::stringstream out;
  std::string tempS;
  if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->Coefficient.IsEqualToZero())
    tempS="0";
  else
    tempS = this->Coefficient.ElementToString();
  if (this->generatorsIndices.size>0)
  { if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
  }
  if (this->Coefficient.size>1)
    out << "(" << tempS << ")";
  else
    out << tempS;
  for (int i=0; i<this->generatorsIndices.size; i++)
  { PolynomialRationalCoeff& thePower=this->Powers.TheObjects[i];
    int theIndex=this->generatorsIndices.TheObjects[i];
    tempS=this->owner->GetLetterFromGeneratorIndex(theIndex, useLatex);
    //if (thePower>1)
    //  out << "(";
    out << tempS;
    if (!thePower.IsEqualToOne())
    { out << "^";
     // if (useLatex)
      out << "{";
      out << thePower.ElementToString();
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

void MonomialUniversalEnveloping::ModOutVermaRelations()
{ int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  int theNumVars=this->Coefficient.NumVars;
  if (theNumVars<theDimension)
    theNumVars=theDimension;
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->owner->ChevalleyGeneratorIndexToRootIndex(generatorsIndices.TheObjects[i]);
    //ChevalleyGeneratorToRootIndex returns a negative value if the generator is an element of the Cartan
    if (IndexCurrentGenerator<numPosRoots && IndexCurrentGenerator>0)
    { this->Nullify(theDimension, *this->owner);
      return;
    }
    if (IndexCurrentGenerator>=numPosRoots)
      return;
    if (IndexCurrentGenerator<0)
    { if (this->Powers.TheObjects[i].TotalDegree()!=0)
        return;

      int theDegree = this->Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort;
      PolynomialRationalCoeff tempP;
      root tempRoot;
      tempRoot.MakeEi(theNumVars, this->owner->ChevalleyGeneratorIndexToElementCartanIndex(this->generatorsIndices.TheObjects[i]));
      tempP.MakeLinPolyFromRoot(tempRoot);
      if (theDegree!=1)
        tempP.RaiseToPower(theDegree);
      this->Coefficient.MultiplyBy(tempP);
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

void MonomialUniversalEnveloping::Nullify(int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Coefficient.Nullify((short)numVars);
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

int MonomialUniversalEnveloping::HashFunction() const
{ int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result=0;
  for (int i=0; i<top; i++)
    result+=SomeRandomPrimes[i]*this->generatorsIndices.TheObjects[i];
  return result;
}

void ParserNode::Clear()
{ this->indexParent=-1;
  this->intValue=0;
  this->ErrorType=this->errorNoError;
  this->Operation= Parser::tokenEmpty;
  this->Evaluated=false;
  this->ExpressionType=this->typeUndefined;
  this->children.size=0;
  this->rationalValue.MakeZero();
  if (this->owner!=0)
    this->ContextLieAlgebra=&this->owner->theHmm.theRange;
  this->polyValue.Nullify(0);
}

std::string SemisimpleLieAlgebra::GetLetterFromGeneratorIndex(int theIndex, bool useLatex)
{ int numPosRoots= this->theWeyl.RootsOfBorel.size;
  int rank= this->theWeyl.CartanSymmetric.NumRows;
  std::stringstream out;
  if (theIndex<numPosRoots || theIndex>= numPosRoots+rank)
  { out << "g";
    if (useLatex)
      out << "^{\\alpha_{" << this->ChevalleyGeneratorIndexToRootIndex(theIndex)+1 << "}}";
    else
      out << "_{" << this->ChevalleyGeneratorIndexToDisplayIndex(theIndex) << "}";
  } else
  { out << "h";
    if (useLatex)
      out << "_{\\alpha_{" << theIndex-numPosRoots+1 << "}}";
    else
      out << "_{" << theIndex-numPosRoots+1 << "}";
  }
  return out.str();
}

bool SemisimpleLieAlgebra::AreOrderedProperly(int leftIndex, int rightIndex)
{ return leftIndex<=rightIndex;
}

int SemisimpleLieAlgebra::DisplayIndexToRootIndex(int theIndex)
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex<0)
    return -theIndex+numPosRoots-1;
  return theIndex-1;
}

int SemisimpleLieAlgebra::RootIndexToDisplayIndexNegativeSpacesFirstThenCartan(int theIndex)const
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  //int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  if (theIndex>=numPosRoots)
    return -theIndex+numPosRoots-1;
  if (theIndex<numPosRoots)
    return theIndex+1;
  return -10000;
}

int SemisimpleLieAlgebra::RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(int theIndex)const
{ if (theIndex<0  || theIndex>=this->theWeyl.RootSystem.size)
    return -1;
  int theDimension=this->theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=this->theWeyl.RootsOfBorel.size;
  if (theIndex<numPosRoots)
    return theIndex+theDimension+numPosRoots;
  if (theIndex>=numPosRoots)
    return numPosRoots*2-1-theIndex;
  return -1;
}

int SemisimpleLieAlgebra::ChevalleyGeneratorIndexToRootIndex(int theIndex)const
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theDimension=this->theWeyl.CartanSymmetric.NumRows;
  if (theIndex<numPosRoots)
    return -theIndex-1+2*numPosRoots;
  //returns a negative number if the generator is an element of the Cartan subalgebra
  return theIndex-theDimension-numPosRoots;
}

std::string SemisimpleLieAlgebra::ElementToStringLieBracketPairing()
{ std::stringstream out;
  std::string tempS;
  out << "<br>\nThe generator indices:<br>\n ";
  for (int i=0; i<this->theLiebracketPairingIndices.NumRows; i++)
  { for (int j=0; j<this->theLiebracketPairingIndices.NumCols; j++)
      out << this->theLiebracketPairingIndices.elements[i][j] << ", ";
    out << "<br>\n";
  }
  this->theLiebracketPairingCoefficients.ElementToString(tempS);
  out << "<br>\nThe coefficients:\n" << tempS;
  return out.str();
}

void ElementUniversalEnveloping::MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebra& theOwner)
{ MonomialUniversalEnveloping tempMon;
  this->Nullify(theOwner);
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((short)numVars, coeff);
  tempMon.MakeConst(tempP, theOwner);
  this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnveloping::Nullify(SemisimpleLieAlgebra& theOwner)
{ this->ClearTheObjects();
  this->owner=&theOwner;
}

void ElementUniversalEnveloping::operator+=(const ElementUniversalEnveloping& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  for (int i=0; i<other.size; i++)
    this->AddMonomialNoCleanUpZeroCoeff(other.TheObjects[i]);
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnveloping::operator-=(const ElementUniversalEnveloping& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  MonomialUniversalEnveloping tempMon;
  for (int i=0; i<other.size; i++)
  { tempMon=other.TheObjects[i];
    tempMon.Coefficient.TimesConstant(-1);
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnveloping::operator/=(const Rational& other)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.DivByRational(other);
}

void ElementUniversalEnveloping::operator*=(const Rational& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.TimesConstant(other);
}

void ElementUniversalEnveloping::operator*=(const ElementUniversalEnveloping& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size*this->size);
  ElementUniversalEnveloping output;
  output.Nullify(*this->owner);
  MonomialUniversalEnveloping tempMon;
  int sizeOriginal=0;
  PolynomialRationalCoeff powerOriginal, CoeffOriginal;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    sizeOriginal=tempMon.generatorsIndices.size;
    if (sizeOriginal>0)
      powerOriginal=*tempMon.Powers.LastObject();
    CoeffOriginal.Assign(tempMon.Coefficient);
    for(int j=0; j<other.size; j++)
    { tempMon.generatorsIndices.size=sizeOriginal;
      tempMon.Powers.size=sizeOriginal;
      if (sizeOriginal>0)
        *tempMon.Powers.LastObject()=powerOriginal;
      tempMon.Coefficient.Assign(CoeffOriginal);
      tempMon.MultiplyByNoSimplify(other.TheObjects[j]);
      //tempMon.ComputeDebugString();
      output.AddMonomialNoCleanUpZeroCoeff(tempMon);
    }
  }
  *this=output;
}

void ElementUniversalEnveloping::SetNumVariables(int newNumVars)
{ this->ComputeDebugString();
  if (this->size==0)
    return;
  int currentNumVars=this->TheObjects[0].Coefficient.NumVars;
  if (currentNumVars==newNumVars)
    return;
  ElementUniversalEnveloping Accum;
  Accum.Nullify(*this->owner);
  MonomialUniversalEnveloping tempMon;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon.SetNumVariables(newNumVars);
    Accum.AddMonomial(tempMon);
  }
  Accum.ComputeDebugString();
  this->operator=(Accum);
  this->ComputeDebugString();
}

void ElementUniversalEnveloping::RaiseToPower(int thePower)
{ ElementUniversalEnveloping buffer;
  buffer=*this;
  if (this->size==0)
    return;
  this->MakeConst(1, this->TheObjects[0].Coefficient.NumVars, *this->owner);
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
}

void ElementUniversalEnveloping::MakeCasimir(SemisimpleLieAlgebra& theOwner, int numVars, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  this->Nullify(theOwner);
  WeylGroup& theWeyl= this->owner->theWeyl;
  int theDimension=theWeyl.CartanSymmetric.NumRows;
  root tempRoot1, tempRoot2;
  MatrixLargeRational killingForm;
  killingForm.init(theDimension, theDimension);
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
    for (int j=0; j<theDimension; j++)
    { killingForm.elements[i][j]=0;
      tempRoot2.MakeEi(theDimension, j);
      for (int k=0; k<theWeyl.RootSystem.size; k++)
        killingForm.elements[i][j]+= theWeyl.RootScalarCartanRoot(tempRoot1, theWeyl.RootSystem.TheObjects[k])* theWeyl.RootScalarCartanRoot(tempRoot2, theWeyl.RootSystem.TheObjects[k]);
    }
  }
  killingForm.Invert(theGlobalVariables);
  killingForm.ComputeDebugString();
  out << killingForm.DebugString;
  ElementUniversalEnveloping tempElt1, tempElt2;
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
    killingForm.ActOnAroot(tempRoot1, tempRoot2);
    tempElt1.AssignElementCartan(tempRoot1, numVars, theOwner);
    tempElt2.AssignElementCartan(tempRoot2, numVars, theOwner);
    tempElt1*=tempElt2;
    *this+=tempElt1;
  }
  Rational tempRat;
  root theSum;
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { tempRat=0;
    root& theRoot=theWeyl.RootSystem.TheObjects[i];
    int indexOfOpposite=theWeyl.RootSystem.IndexOfObjectHash(-theRoot);
    root& theOpposite= theWeyl.RootSystem.TheObjects[indexOfOpposite];
    for (int j=0; j<theWeyl.RootSystem.size; j++)
    { root& current=theWeyl.RootSystem.TheObjects[j];
      if (current==theOpposite)
        tempRat+=2;
      else
      { int indexOfSum=theWeyl.RootSystem.IndexOfObjectHash(current+theRoot);
        if (indexOfSum!=-1)
          tempRat+=(theOwner.ChevalleyConstants.elements[i][j]*theOwner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
      }
    }
    tempRat+=2;
    tempRat= 1/tempRat;
    tempElt2.MakeOneGeneratorCoeffOne(theOpposite, numVars, theOwner);
    tempElt1.MakeOneGeneratorCoeffOne(theRoot, numVars, theOwner);
    tempElt2*=tempElt1;
    tempElt2*=tempRat;
    *this+=tempElt2;
  }
  this->ComputeDebugString();
  out << "\n" << this->DebugString;
  this->DebugString=out.str();
/*  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    if (!length1Explored)
    { length1= theWeyl.RootScalarCartanRoot(tempRoot, tempRoot);
      length1Explored=true;
      coefficient1=0;
      for (int j=0; j<theWeyl.RootsOfBorel.size; j++)
      { coefficient1+=theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j])*theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j]);
        coef
      }
    }
  }*/
}

void ComputationSetup::ExperimentSSsubalgebras(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ SemisimpleSubalgebras theComputation;
  theComputation.FindHCandidates(inputData.WeylGroupLetter, inputData.WeylGroupIndex, theGlobalVariables);
}

void ComputationSetup::TestParser(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ std::string inputString;
  std::stringstream out;
  SemisimpleLieAlgebra theLieAlgebra;
  theLieAlgebra.ComputeChevalleyConstants('A', 1, theGlobalVariables);
  theLieAlgebra.ComputeDebugString(false, false, theGlobalVariables);
  ElementUniversalEnveloping theElt;
  theElt.MakeCasimir(theLieAlgebra, 1, theGlobalVariables);
  out << theElt.DebugString << "\n\n";
  out << theLieAlgebra.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1 = out.str();
  theGlobalVariables.MakeReport();
}

void CGIspecificRoutines::MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank)
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
}

void SemisimpleLieAlgebra::ElementToStringNegativeRootSpacesFirst(std::string& output, bool useHtml, bool useLatex, bool usePNG, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  root tempRoot;
  std::string beginMath;
  std::string endMath;
  if (!usePNG)
  { beginMath="<DIV class=\"math\" scale=\"50\">\\begin{array}{";
    endMath="\\end{array}</DIV>";
  } else
  { beginMath="\\begin{tabular}{";
    endMath="\\end{tabular}";
  }
  int numRoots=this->theWeyl.RootSystem.size;
  int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
  ElementSimpleLieAlgebra tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  if (usePNG)
  { out << "\\begin{tabular}{cc}";
    out << "\\hline generator &corresponding root space\\\\\\hline";
    for (int i=0; i<numRoots+theDimension; i++)
    { if (i==numPosRoots)
      { out << "\\hline\\begin{tabular}{c}$h_i$:=$[g_{i},g_{-i}]$\\\\$h_i$ is dual to the i$^{th}$ root\\end{tabular} & 0 \\\\\\hline";
//        out << "  \\\\\\hline";
        //out << "\\hline generator & corresponding root space\\\\\\hline";
        i+=theDimension;
      }
      out << "$" << this->GetLetterFromGeneratorIndex(i, false) << "$&";
      int tempI=this->ChevalleyGeneratorIndexToRootIndex(i);
      out << this->theWeyl.RootSystem.TheObjects[tempI].ElementToString() << "\\\\";
    }
    out << "\\end{tabular}";
  }
  out << beginMath;
//  ElementUniversalEnveloping tempSSElt1, tempSSElt2, tempSSElt3;
  for (int i=0; i<numRoots+theDimension+1; i++)
    out << "c";
  out << "}";
  std::string tempHeader=out.str();
  if(usePNG)
    out << "$";
  out << "[\\bullet, \\bullet]\n";
  if(usePNG)
    out << "$";
  for (int i=0; i<numRoots+theDimension; i++)
  { tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, *this);
    tempS=tempElt1.ElementToStringNegativeRootSpacesFirst(true, false, *this);
    out << " & ";
    if(usePNG)
      out << "$";
    out << tempS;
    if(usePNG)
      out << "$";
  }
  out << "\\\\\n";
  Rational tempRat;
  for (int i=0; i<numRoots+theDimension; i++)
  { tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i,*this);
    tempS=tempElt1.ElementToStringNegativeRootSpacesFirst(true, false, *this);
    if(usePNG)
      out << "$";
    out << tempS;
    if(usePNG)
      out << "$";
    for (int j=0; j<numRoots+theDimension; j++)
    { tempElt2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(j, *this);
      this->LieBracket(tempElt1, tempElt2, tempElt3);
      tempS=tempElt3.ElementToStringNegativeRootSpacesFirst(true, false, *this);
      out << "& ";
      if(usePNG)
        out << "$";
      out << tempS;
      if(usePNG)
        out << "$";
    }
    out << "\\\\\n";
    //the below is a hack to avoid a latex crash due to memory overuse
    //if (usePNG && i%130==129)
    //  out << "\\end{tabular}\n\n\n" << tempHeader;
  }
  out << endMath;
  output=out.str();
}

void CGIspecificRoutines::ReplaceEqualitiesAndAmpersantsBySpaces(std::string& inputOutput)
{ for (int i=0; i<(signed)inputOutput.size(); i++)
    if (inputOutput[i]=='=' || inputOutput[i]=='&')
      inputOutput[i]=' ';
}

void VectorPartition::ComputeAllPartitions()
{ List<int> currentPartition;
  currentPartition.initFillInObject(PartitioningRoots.size, 0);
  this->thePartitions.size=0;
  this->ComputeAllPartitionsRecursive(0, currentPartition, -1, theRoot);
}

void VectorPartition::ComputeAllPartitionsRecursive(int currentIndex, List<int>& CurrentPartition, int UpperBoundEachIndex, root& toBePartitioned)
{ if (currentIndex>=this->PartitioningRoots.size)
    return;
  root currentRoot=toBePartitioned;
  while (currentRoot.IsPositiveOrZero() && (CurrentPartition.TheObjects[currentIndex]<=UpperBoundEachIndex || UpperBoundEachIndex==-1))
  { if (currentRoot.IsEqualToZero())
      this->thePartitions.AddObjectOnTop(CurrentPartition);
    else
    { this->ComputeAllPartitionsRecursive(currentIndex+1, CurrentPartition , UpperBoundEachIndex, currentRoot);
      for (int i=currentIndex+1; i<CurrentPartition.size; i++)
        CurrentPartition.TheObjects[i]=0;
    }
    currentRoot.Subtract(this->PartitioningRoots.TheObjects[currentIndex]);
    CurrentPartition.TheObjects[currentIndex]++;
  }
}

std::string VectorPartition::ElementToString(bool useHtml)
{ std::stringstream out;
  out << this->theRoot.ElementToString() << "\n\n";
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
        out << this->PartitioningRoots.TheObjects[j].ElementToString();
      }
    }
    out << "\n\n";
    if (useHtml)
      out << "<br>\n";
  }
  return out.str();
}


std::string rootPoly::ElementToString()
{ std::stringstream out;
  out << "(";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString();
    if (i<this->size-1)
      out << ",";
  }
  out << ")";
  return out.str();
}

std::string RationalFunction::ElementToString(bool useLatex, bool breakLinesLatex)
{ std::stringstream out;
  bool hasDenominator=!this->Denominator.IsEqualToOne();
  if (hasDenominator && useLatex)
    out << "\\frac{";
  out << this->Numerator.ElementToString(breakLinesLatex);
  if (hasDenominator)
  { if (useLatex)
      out << "}{";
    out << this->Denominator.ElementToString(breakLinesLatex);
    if (useLatex)
      out << "}";
  }
  return out.str();
}

void RationalFunction::RemainderDivisionWithRespectToBasis(PolynomialRationalCoeff& input, List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& outputRemainder, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, Monomial<Rational>& bufferMon1)
{ assert(&outputRemainder!=&input);
  PolynomialRationalCoeff* currentRemainder=&input;
  PolynomialRationalCoeff* nextRemainder=&buffer1;
  for (int i=0; i<theBasis.size; i++)
  { RationalFunction::RemainderDivision(*currentRemainder, theBasis.TheObjects[i], *nextRemainder, buffer2, bufferMon1);
    MathRoutines::swap(currentRemainder, nextRemainder);
    if (currentRemainder->IsEqualToZero())
      break;
  }
  if (currentRemainder!=&outputRemainder)
    outputRemainder.Assign(*currentRemainder);
}

void RationalFunction::RemainderDivision(PolynomialRationalCoeff& input, PolynomialRationalCoeff& divisor, PolynomialRationalCoeff& outputRemainder, PolynomialRationalCoeff& buffer, Monomial<Rational>& bufferMon1)
{ assert(&input!=&outputRemainder);
  outputRemainder.Assign(input);
  int divisorHighest=divisor.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  Monomial<Rational>& highestMonDivisor=divisor.TheObjects[divisorHighest];
  int remainderHighest=-1;
  int theNumVars=input.NumVars;
  bufferMon1.init((short)theNumVars);
  assert(input.NumVars==theNumVars);
  assert(divisor.NumVars==theNumVars);
  for(;;)
  { if (outputRemainder.size==0)
      return;
    remainderHighest=  outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    Monomial<Rational>& highestMonRemainder=outputRemainder.TheObjects[remainderHighest];
    /*outputRemainder.ComputeDebugString();
    highestMonRemainder.ComputeDebugString();
    highestMonDivisor.ComputeDebugString();*/
    for (int i=0; i<theNumVars; i++)
    { if (highestMonRemainder.degrees[i]<highestMonDivisor.degrees[i])
        return;
      bufferMon1.degrees[i]=highestMonRemainder.degrees[i]-highestMonDivisor.degrees[i];
    }
    bufferMon1.Coefficient=highestMonRemainder.Coefficient/highestMonDivisor.Coefficient;
    divisor.MultiplyByMonomial(bufferMon1, buffer);
/*    outputRemainder.ComputeDebugString();
    tempP.ComputeDebugString();
    highestMonRemainder.ComputeDebugString();
    tempMon.ComputeDebugString();
    highestMonDivisor.ComputeDebugString();*/
    outputRemainder.Subtract(buffer);/*
    outputRemainder.ComputeDebugString();*/
  }
}

void RationalFunction::TransformToGroebnerBasis(List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2)
{ PolynomialRationalCoeff& tempP=buffer1;
  PolynomialRationalCoeff& Spoly=buffer2;
  Monomial<Rational>& leftShift=bufferMon1;
  Monomial<Rational>& rightShift=bufferMon2;
  int theNumVars=theBasis.TheObjects[0].NumVars;
  leftShift.init((short)theNumVars);
  rightShift.init((short)theNumVars);
 // std::string tempS;
  for (int lowestNonExplored=0; lowestNonExplored< theBasis.size; lowestNonExplored++)
  { //warning! currentPoly may expire if theBasis.TheObjects changes size
//    PolynomialRationalCoeff& currentPoly=;
    for (int j=0; j<theBasis.size; j++)
    { PolynomialRationalCoeff& currentLeft= theBasis.TheObjects[lowestNonExplored];
      PolynomialRationalCoeff& currentRight= theBasis.TheObjects[j];
      Monomial<Rational>& leftHighestMon=currentLeft.TheObjects[currentLeft.GetIndexMaxMonomialLexicographicLastVariableStrongest()];
      Monomial<Rational>& rightHighestMon=currentRight.TheObjects[currentRight.GetIndexMaxMonomialLexicographicLastVariableStrongest()];
      //leftHighestMon.ComputeDebugString();
      //rightHighestMon.ComputeDebugString();
      //currentRight.ComputeDebugString();
      //currentLeft.ComputeDebugString();
      leftShift.Coefficient=-rightHighestMon.Coefficient;
      rightShift.Coefficient=leftHighestMon.Coefficient;
      for (int k=0; k<leftHighestMon.NumVariables; k++)
        if (leftHighestMon.degrees[k]>rightHighestMon.degrees[k])
        { rightShift.degrees[k]=leftHighestMon.degrees[k]-rightHighestMon.degrees[k];
          leftShift.degrees[k]=0;
        }
        else
        { leftShift.degrees[k]=rightHighestMon.degrees[k]-leftHighestMon.degrees[k];
          rightShift.degrees[k]=0;
        }
      currentLeft.MultiplyByMonomial(leftShift, tempP);
      currentRight.MultiplyByMonomial(rightShift, Spoly);
      Spoly.AddPolynomial(tempP);
      //Spoly.ComputeDebugString();
//      theBasis.ElementToStringGeneric(tempS);
      RationalFunction::RemainderDivisionWithRespectToBasis(Spoly, theBasis, tempP, buffer3, buffer4, bufferMon1);

      //tempP.ComputeDebugString();
      if (!tempP.IsEqualToZero())
      { tempP.ScaleToIntegralNoGCDCoeffs();
        theBasis.AddObjectOnTop(tempP);
//        std::cout << "<br> new element found: " << tempP.ElementToString();
      }
    }
  }
//  std::cout << "<br> ... and the basis before reduction is: <br>";
//  for (int i=0; i<theBasis.size; i++)
//    std::cout << theBasis.TheObjects[i].ElementToString() << ", ";
  RationalFunction::ReduceGroebnerBasis(theBasis, buffer1);

}

void RationalFunction::ReduceGroebnerBasis(List<PolynomialRationalCoeff>& theBasis, PolynomialRationalCoeff& buffer1)
{ PolynomialRationalCoeff& LeadingCoeffs=buffer1;
  LeadingCoeffs.MakeActualSizeAtLeastExpandOnTop(theBasis.size);
  LeadingCoeffs.ClearTheObjects();
  List<Monomial< Rational> > tempList;
//  std::cout << "<br> ... and the leading coefficients are: <br>";
  for (int i=0; i<theBasis.size; i++)
  { PolynomialRationalCoeff& current=theBasis.TheObjects[i];
    LeadingCoeffs.AddObjectOnTopHash(current.TheObjects[current.GetIndexMaxMonomialLexicographicLastVariableStrongest()]);
    LeadingCoeffs.LastObject()->Coefficient=1;
//    LeadingCoeffs.LastObject()->ComputeDebugString();
//    std::cout << LeadingCoeffs.LastObject()->DebugString << ", ";
  }
  tempList.CopyFromBase(LeadingCoeffs);
  tempList.QuickSortAscending();
//  std::cout << "<br><br> and the sorted leading monomials are: ";
//  for (int i=0; i<theBasis.size; i++)
//  { tempList.TheObjects[i].ComputeDebugString();
//    std::cout << tempList.TheObjects[i].DebugString << ", ";
//  }
  for (int i=0; i<LeadingCoeffs.size; i++)
  { Monomial<Rational>& currentMon=LeadingCoeffs.TheObjects[i];
    for (int j=0; j<LeadingCoeffs.size; j++)
      if (i!=j)
        if (currentMon.IsDivisibleBy(LeadingCoeffs.TheObjects[j]))
        { LeadingCoeffs.PopIndexSwapWithLastHash(i);
          theBasis.PopIndexSwapWithLast(i);
          i--;
          break;
        }
  }
}

void RationalFunction::gcd(PolynomialRationalCoeff& left, PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, PolynomialRationalCoeff& buffer5, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList)
{ RationalFunction::lcm(left, right, buffer4, buffer1, buffer2, buffer3, buffer5, bufferMon1, bufferMon2, bufferList);
  left.MultiplyBy(right, buffer2, buffer1, bufferMon1);
//  std::cout << "<br>the product: " << buffer2.DebugString << " and the gcd: " << buffer4.DebugString << "<br>";
  buffer2.DivideBy(buffer4, output, buffer3);
}

void RationalFunction::lcm(PolynomialRationalCoeff& left, PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList)
{ PolynomialRationalCoeff& leftTemp=buffer1;
  PolynomialRationalCoeff& rightTemp=buffer2;
  PolynomialRationalCoeff& tempP=buffer3;
  List<PolynomialRationalCoeff>& tempList=bufferList;
  leftTemp.Assign(left);
  rightTemp.Assign(right);
  assert(left.NumVars==right.NumVars);
  int theNumVars=left.NumVars;
  leftTemp.SetNumVariablesSubDeletedVarsByOne((short)theNumVars+1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne((short)theNumVars+1);
  leftTemp.ScaleToIntegralNoGCDCoeffs();
  rightTemp.ScaleToIntegralNoGCDCoeffs();
  tempP.MakeMonomialOneLetter((short)theNumVars+1,(short) theNumVars, 1, (Rational) 1);
  leftTemp.MultiplyBy(tempP);
  tempP.TimesConstant((Rational)-1);
  tempP.AddConstant((Rational) 1);
  rightTemp.MultiplyBy(tempP);
  tempList.size=0;
  tempList.AddObjectOnTop(leftTemp);
  tempList.AddObjectOnTop(rightTemp);
//  std::cout << "<br>In the beginning: <br>";
//  for (int i=0; i<tempList.size; i++)
//  { std::cout << "the groebner basis element with index " << i << " is " << tempList.TheObjects[i].ElementToString() << "<br>\n";
//  }
  RationalFunction::TransformToGroebnerBasis(tempList, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2);
//  std::cout << "<br><br> ... and the basis is: <br>";
//  for (int i=0; i<tempList.size; i++)
//  { std::cout << tempList.TheObjects[i].ElementToString() << "<br>\n";
//  }
  for(int i=0; i<tempList.size; i++)
  { PolynomialRationalCoeff& current=tempList.TheObjects[i];
    Monomial<Rational>& currentMon= current.TheObjects[current.GetIndexMaxMonomialLexicographicLastVariableStrongest()];
    currentMon.ComputeDebugString();
    if (currentMon.degrees[theNumVars]==0)
    { output=current;
//      std::cout << "<br> the highest mon is: " << currentMon.DebugString << "<br>";
      output.SetNumVariablesSubDeletedVarsByOne((short)theNumVars);
      return;
    }
  }
  output.Nullify((short)theNumVars);
}

std::string rootRationalFunction::ElementToString()
{ std::stringstream out;
  out << "(";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString(true, false);
    if (i!=this->size-1)
      out << ",";
  }
  out << ")";
  return out.str();
}

void RationalFunction::Simplify()
{ if (this->Numerator.IsEqualToZero())
  { this->Denominator.MakeNVarConst((short)this->NumVariables, (Rational) 1);
    return;
  }
  if (this->Denominator.IsEqualToOne())
    return;
  PolynomialRationalCoeff theGCD, tempP, tempP2;
  this->gcd(this->Numerator, this->Denominator, theGCD);
  this->Numerator.DivideBy(theGCD, tempP, tempP2);
  this->Numerator.Assign(tempP);
  this->Denominator.DivideBy(theGCD, tempP, tempP2);
  this->Denominator.Assign(tempP);
  Rational tempRat, tempRat2;
  tempRat=this->Denominator.FindGCDCoefficientDenominators();
  tempRat2=this->Denominator.FindGCDCoefficientNumerators();
  tempRat.DivideBy(tempRat2);
  if (!tempRat.IsEqualToOne())
  { this->Denominator.TimesConstant(tempRat);
    this->Numerator.TimesConstant(tempRat);
  }
}

Rational PolynomialRationalCoeff::FindGCDCoefficientNumerators()
{ if (this->size==0)
    return (Rational) 1;
  Rational result; LargeIntUnsigned Accum, tempUI;
  this->TheObjects[0].Coefficient.GetNumUnsigned(Accum);
  for (int i=1; i<this->size; i++)
  { this->TheObjects[i].Coefficient.GetNumUnsigned(tempUI);
    LargeIntUnsigned::gcd(Accum, tempUI, Accum);
  }
  LargeInt tempInt;
  tempInt.AssignLargeIntUnsigned(Accum);
  result.AssignLargeInteger(tempInt);
  if (this->size>0)
  { if (this->TheObjects[0].Coefficient.IsNegative())
      result.Minus();
  }
  return result;
}

Rational PolynomialRationalCoeff::FindGCDCoefficientDenominators()
{ if (this->size==0)
    return (Rational) 1;
  Rational result; LargeIntUnsigned Accum, tempUI;
  this->TheObjects[0].Coefficient.GetDen(Accum);
  for (int i=1; i<this->size; i++)
  { this->TheObjects[i].Coefficient.GetDen(tempUI);
    LargeIntUnsigned::gcd(Accum, tempUI, Accum);
  }
  LargeInt tempInt;
  tempInt.AssignLargeIntUnsigned(Accum);
  result.AssignLargeInteger(tempInt);
  return result;
}

void PolynomialRationalCoeff::ScaleToIntegralNoGCDCoeffs()
{ if(this->size==0)
    return;
  int indexHighestMon=0;
  LargeIntUnsigned tempInt1, tempInt2, accumNum, accumDen;
  accumDen.MakeOne();
  this->TheObjects[0].Coefficient.GetNumUnsigned(accumNum);
  for (int i=0; i<this->size; i++)
  { if (this->TheObjects[i].IsGEQLexicographicLastVariableStrongest(this->TheObjects[indexHighestMon]))
      indexHighestMon=i;
    Rational& tempRat=this->TheObjects[i].Coefficient;
    tempRat.GetDen(tempInt1);
    tempRat.GetNumUnsigned(tempInt2);
    LargeIntUnsigned::lcm(tempInt1, accumDen, accumDen);
    LargeIntUnsigned::gcd(tempInt2, accumNum, accumNum);
  }
  Rational theMultiple;
  theMultiple.MakeOne();
  if (this->TheObjects[indexHighestMon].Coefficient.IsNegative())
    theMultiple.MakeMOne();
  theMultiple.MultiplyByLargeIntUnsigned(accumDen);
  Rational tempRat2;
  LargeInt tempInt3;
  tempInt3.AssignLargeIntUnsigned(accumNum);
  tempRat2.AssignLargeInteger(tempInt3);
  theMultiple.DivideBy(tempRat2);
  this->TimesConstant(theMultiple);
}

std::string EigenVectorComputation::ComputeAndReturnString(GlobalVariables& theGlobalVariables, Parser& theParser)
{ std::stringstream out;
  ElementUniversalEnveloping theElt, tempElt1, tempElt2;
  //this->MakeGenericMonomialBranchingCandidate(theParser.theHmm, theElt, theGlobalVariables);
  this->MakeGenericVermaElement(theElt, theParser.theHmm.theRange);
  int theDomainRank=theParser.theHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
//  int numDomainPosRoots= theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
  //int dimQuotient=numRangePosRoots-theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  this->theSystem.init(0,0);
  this->theOperators.size=0;
  this->theExponentShifts.size=0;
  for (int i=0; i<theDomainRank; i++)
  { tempElt1.AssignElementLieAlgebra(theParser.theHmm.imagesSimpleChevalleyGenerators.TheObjects[i], theRangeRank+numRangePosRoots, theParser.theHmm.theRange);
    tempElt1.LieBracketOnTheRight(theElt, tempElt2);
    tempElt2.Simplify();
    out << "<div class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
    out << "[" << tempElt1.ElementToString(true) << "," << theElt.ElementToString(true) << "]=" << tempElt2.ElementToString(true);
    out << "\\end{eqnarray*}</div>";
    tempElt2.ModOutVermaRelations();
    out << "mod Verma rels:";
    out << "<div class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
    out << tempElt2.ElementToString(true);
    out << "\\end{eqnarray*}</div>";
    this->DetermineEquationsFromResultLieBracketEquationsPerTarget(theParser, theElt, tempElt2, out, theGlobalVariables);
  }
  out << "<div class=\"math\" scale=\"50\">" << this->theSystem.ElementToString(false, true) << "</div>";
  return out.str();
}

void RootIndexToPoly(int theIndex, SemisimpleLieAlgebra& theAlgebra, PolynomialRationalCoeff& output)
{ int theRank=theAlgebra.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theAlgebra.theWeyl.RootsOfBorel.size;
  output.MakeNVarDegOnePoly((short)(theRank+numPosRoots), theIndex+theRank, (Rational) 1);
}

void EigenVectorComputation::DetermineEquationsFromResultLieBracketEquationsPerTarget(Parser& theParser, ElementUniversalEnveloping& theStartingGeneric, ElementUniversalEnveloping& theElt, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
//  int dimQuotient=theParser.theHmm.theRange.theWeyl.RootsOfBorel.size- theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numCoeffVars=theRangeRank+numRangePosRoots;
  PolynomialRationalCoeff theDiffPoly;
  MonomialUniversalEnveloping& originalMon= theStartingGeneric.TheObjects[0];
  this->theExponentShiftsTargetPerSimpleGenerator.SetSizeExpandOnTopNoObjectInit(this->theExponentShiftsTargetPerSimpleGenerator.size+1);
  roots& currentTargetShifts =*this->theExponentShiftsTargetPerSimpleGenerator.LastObject();
  currentTargetShifts.SetSizeExpandOnTopNoObjectInit(theElt.size);
  ElementWeylAlgebra tempWeyl;
  this->theOperators.SetSizeExpandOnTopNoObjectInit(this->theOperators.size+1);
  ElementWeylAlgebra& currentOperator = *this->theOperators.LastObject();
  currentOperator.Nullify(numCoeffVars);
  for (int i=0; i<theElt.size; i++)
  { MonomialUniversalEnveloping& theMon= theElt.TheObjects[i];
    out << "<br>\nDifference in monomial index " << i << ": ";
    root& currentShift= currentTargetShifts.TheObjects[i];
    currentShift.MakeZero(numRangePosRoots);
    for (int j=0; j<numRangePosRoots; j++)
    { theDiffPoly = originalMon.Powers.TheObjects[j] - theMon.Powers.TheObjects[j];
      out << theDiffPoly.ElementToString() << " ";
      if (theDiffPoly.size>1)
        currentShift.TheObjects[j]=-10000;
      if (theDiffPoly.size==0)
        currentShift.TheObjects[j]=0;
      if (theDiffPoly.size==1)
        currentShift.TheObjects[j]=-theDiffPoly.TheObjects[0].Coefficient;
    }
    this->GetDiffOperatorFromShiftAndCoefficient(theMon.Coefficient, currentShift, tempWeyl);
    currentOperator.Add(tempWeyl);
  }
  root tempRoot;
  RationalFunction ZeroRF;
  ZeroRF.MakeNVarConst(numCoeffVars, (Rational) 0);
  tempRoot.MakeZero(numRangePosRoots);
  if(this->theExponentShifts.AddOnTopNoRepetition(tempRoot))
    this->theSystem.Resize(this->theSystem.NumRows, this->theSystem.NumCols+1, true, &ZeroRF);
  int indexMainMon=this->theExponentShifts.IndexOfObject(tempRoot);
  PolynomialsRationalCoeff theSub;
  PolynomialRationalCoeff tempP;
  for (int i=0; i<theElt.size; i++)
  { int indexCurrentRow=this->theSystem.NumRows;
    this->theSystem.Resize(this->theSystem.NumRows+1, this->theSystem.NumCols, true, &ZeroRF);
    this->theSystem.elements[indexCurrentRow][indexMainMon]=theElt.TheObjects[i].Coefficient;
    for (int j=0; j<theElt.size; j++)
      if (i!=j)
      { theSub.MakeIdSubstitution(numCoeffVars, (Rational) 1);
        tempRoot= currentTargetShifts.TheObjects[i]-currentTargetShifts.TheObjects[j];
        for (int k=0; k<tempRoot.size; k++)
          theSub.TheObjects[k+theRangeRank].AddConstant(tempRoot.TheObjects[k]);
        int theVarIndex = this->theExponentShifts.IndexOfObject(tempRoot);
        if (theVarIndex<0)
        { theVarIndex=this->theExponentShifts.size;
          this->theExponentShifts.AddObjectOnTop(tempRoot);
          this->theSystem.Resize(this->theSystem.NumRows, this->theExponentShifts.size, true, &ZeroRF);
          out << "<br>Shift of monomial in play: " << tempRoot.ElementToString();
        }
        tempP=theElt.TheObjects[j].Coefficient;
        theSub.ComputeDebugString();
        tempP.ComputeDebugString();
        tempP.Substitution(theSub, numCoeffVars);
        tempP.ComputeDebugString();
        this->theSystem.elements[indexCurrentRow][theVarIndex]=tempP;
      }
  }
}

void EigenVectorComputation::MakeGenericMonomialBranchingCandidate(HomomorphismSemisimpleLieAlgebra& theHmm, ElementUniversalEnveloping& theElt, GlobalVariables& theGlobalVariables)
{ int RangeRank=theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numPosRootsRange=theHmm.theRange.theWeyl.RootsOfBorel.size;
  int dimPosPartQuotient=numPosRootsRange- theHmm.theDomain.theWeyl.RootsOfBorel.size;
  theElt.Nullify(theHmm.theRange);
  MonomialUniversalEnveloping tempMon;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((short) RangeRank+dimPosPartQuotient, (Rational) 1);
  tempMon.MakeConst(tempP, theHmm.theRange);
  roots selectedRootSpaces;
  //int indexFirstPosRootDomain=theHmm.theDomain.theWeyl.RootsOfBorel.size+ theHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  selectedRootSpaces.MakeActualSizeAtLeastExpandOnTop(theHmm.theDomain.theWeyl.RootsOfBorel.size);
  root tempRoot;
  for (int i=0; i<theHmm.theDomain.theWeyl.RootsOfBorel.size; i++)
  { ElementSimpleLieAlgebra& tempElt= theHmm.imagesAllChevalleyGenerators.TheObjects[i];
    tempElt.ElementToVectorRootSpacesFirstThenCartan(tempRoot);
    selectedRootSpaces.AddObjectOnTop(tempRoot);
    //std::cout << "<br>" << tempElt.ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange);
  }
  ElementSimpleLieAlgebra tempElt2;
  int numFound=0;
  for (int i=0; i<numPosRootsRange; i++)
  { tempElt2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, theHmm.theRange);
    tempElt2.ElementToVectorRootSpacesFirstThenCartan(tempRoot);
    selectedRootSpaces.AddObjectOnTop(tempRoot);
    if (selectedRootSpaces.GetRankOfSpanOfElements(theGlobalVariables)==selectedRootSpaces.size)
    { tempP.MakeNVarDegOnePoly(RangeRank+dimPosPartQuotient, RangeRank+numFound, (Rational) 1);
      numFound++;
      tempMon.MultiplyByGeneratorPowerOnTheRight(i, tempP);
      //std::cout << "<br>" << tempElt2.ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange);
    } else
      selectedRootSpaces.PopLastObject();
  }
  theElt.AddMonomial(tempMon);
}

void EigenVectorComputation::MakeGenericVermaElement(ElementUniversalEnveloping& theElt, SemisimpleLieAlgebra& owner)
{ theElt.owner=&owner;
  int numPosRoots= owner.theWeyl.RootsOfBorel.size;
  int theRank=owner.theWeyl.CartanSymmetric.NumRows;
  theElt.Nullify(owner);
  MonomialUniversalEnveloping tempMon;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((short) theRank+numPosRoots, (Rational) 1);
  tempMon.MakeConst(tempP, owner);
  for (int i=0; i<numPosRoots; i++)
  { tempP.MakeNVarDegOnePoly(theRank+numPosRoots, theRank+i, (Rational) 1);
    tempMon.MultiplyByGeneratorPowerOnTheRight(i, tempP);
  }
  theElt.AddMonomial(tempMon);
}

Rational WeylGroup::WeylDimFormula(root& theWeightInFundamentalBasis, GlobalVariables& theGlobalVariables)
{ root theWeightInSimpleBasis=theWeightInFundamentalBasis;
  MatrixLargeRational invertedCartan;
  for (int i=0; i<this->CartanSymmetric.NumRows; i++)
    theWeightInSimpleBasis.TheObjects[i]/=2/this->CartanSymmetric.elements[i][i];
  invertedCartan=this->CartanSymmetric;
  invertedCartan.Invert(theGlobalVariables);
  invertedCartan.ActOnAroot(theWeightInSimpleBasis, theWeightInSimpleBasis);
  Rational Result=1;
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { Result.MultiplyBy(this->RootScalarCartanRoot(theWeightInSimpleBasis+this->rho, this->RootsOfBorel.TheObjects[i]));
    Result/=this->RootScalarCartanRoot(this->rho, this->RootsOfBorel.TheObjects[i]);
  }
  return Result;
}

std::string SemisimpleSubalgebras::ElementToString()
{ std::stringstream out; std::string tempS;
  this->theHcandidates.ElementToStringGeneric(tempS);
  out << tempS;
  return out.str();
}

void SemisimpleSubalgebras::FindHCandidates(char WeylLetter, int WeylDim, GlobalVariables& theGlobalVariables)
{ roots emptyStart;
//  List<List<root> >::ListActualSizeIncrement=10000;
  this->theAlgebra.FindSl2Subalgebras(this->theSl2s, WeylLetter, WeylDim, theGlobalVariables);
  this->theHcandidates.size=0;
  this->theHcandidates.AddObjectOnTop(emptyStart);
  for (this->indexLowestUnexplored=0; this->indexLowestUnexplored<this->theHcandidates.size; this->indexLowestUnexplored++)
    this->FindHCandidatesWithOneExtraHContaining(this->theHcandidates.TheObjects[this->indexLowestUnexplored], theGlobalVariables);
  this->ComputeDebugString();
  theGlobalVariables.theIndicatorVariables.StatusString1.append(this->DebugString);
  theGlobalVariables.MakeReport();
}

void  SemisimpleSubalgebras::FindHCandidatesWithOneExtraHContaining(roots& inpuT, GlobalVariables& theGlobalVariables)
{ int theDimension= this->theAlgebra.theWeyl.CartanSymmetric.NumRows;
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(theDimension, 2);
  int theCounter=MathRoutines::KToTheNth(3, theDimension);
  root theRoot;
  WeylGroup& theWeyl=this->theAlgebra.theWeyl;
  MatrixLargeRational invertedCartan=this->theAlgebra.theWeyl.CartanSymmetric;
  invertedCartan.Invert(theGlobalVariables);
  roots tempRoots;
  tempRoots=inpuT;
  roots inputCopy;
  inputCopy=inpuT;
  for (int i=0; i<theCounter; i++, theSel.IncrementSubset())
  { //slTwo& currentSl2=this->theSl2s.TheObjects[i];
    theRoot=theSel;
    invertedCartan.ActOnAroot(theRoot, theRoot);
    bool isGood=true;
    if (!inputCopy.LinSpanContainsRoot(theRoot, theGlobalVariables))
    { for (int j=0; j<inputCopy.size; j++)
        if (theWeyl.RootScalarCartanRoot(inputCopy.TheObjects[j], theRoot).IsPositive())
        { isGood=false;
          break;
        }
      if (isGood)
      { tempRoots= inputCopy;
        tempRoots.AddObjectOnTop(theRoot);
        for (int k=tempRoots.size-1; k>0; k--)
          if (tempRoots.TheObjects[k]< tempRoots.TheObjects[k-1])
            tempRoots.SwapTwoIndices(k, k-1);
        this->theHcandidates.AddOnTopNoRepetition(tempRoots);
        //this->th
      }
      std::stringstream out;
      out << "index lowest non explored: " << this->indexLowestUnexplored+1 << " Total number found: " << this->theHcandidates.size;
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
      theGlobalVariables.MakeReport();
    }
  }
}

void EigenVectorComputation::GetDiffOperatorFromShiftAndCoefficient
(PolynomialRationalCoeff& theCoeff, root& theShift, ElementWeylAlgebra& output)
{ PolynomialRationalCoeff AccumDiffPart, tempP, tempP2, AccumPolyPart;
  Monomial<Rational> currentMon;
  int dimCartan=theCoeff.NumVars- theShift.size;
  AccumDiffPart.MakeNVarConst(theCoeff.NumVars, (Rational) 1);
  AccumPolyPart.MakeNVarConst(theCoeff.NumVars, (Rational) 1);

  for (int i=0; i<theShift.size; i++)
    if (theShift.TheObjects[i].NumShort<0)
      for (int j=0; j<-theShift.TheObjects[i].NumShort; j++)
      { tempP.MakeNVarDegOnePoly(theCoeff.NumVars, i+dimCartan, (Rational) 1, (Rational) -j);
        AccumDiffPart.MultiplyBy(tempP);
      }
    else if (theShift.TheObjects[i].NumShort>0)
    { tempP.MakeMonomialOneLetter(theCoeff.NumVars, i+dimCartan, theShift.TheObjects[i].NumShort, (Rational) 1);
      AccumPolyPart.MultiplyBy(tempP);
    }
  std::cout << "<br>" << theShift.ElementToString() << ": " << AccumDiffPart.ElementToString(false);
  theCoeff.DivideBy(AccumDiffPart, tempP, tempP2);
  if (!tempP2.IsEqualToZero())
    std::cout << "  problem problem problem!!!!";
  else
  { AccumDiffPart=tempP;
    std::cout << " diff part: " << AccumDiffPart.ElementToString(false);
  }
  std::cout << " poly part: " << AccumPolyPart.ElementToString(false);

  //while(!remainingCoeff.IsEqualToZero())
  //{ currentMon=remainingCoeff.TheObjects[remainingCoeff.GetIndexMaxMonomialTotalDegThenLexicographic()];

    //remainingCoeff.AddMonomial(
  //}
}
