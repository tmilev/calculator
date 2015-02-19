//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"
#include "vpfHeader2Math15_UniversalEnveloping.h"
#include "vpfHeader2Math1_2SemisimpleLieAlgebras_RootSubalgebras.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"

ProjectInformationInstance ProjectInfoVpf9_2cpp(__FILE__, "Math routines implementation. ");

std::string MonomialWeylAlgebra::ToString(FormatExpressions* theFormat)const
{ if (this->IsConstant())
    return "1";
  std::stringstream out;
  FormatExpressions tempFormat;
  if (theFormat==0)
    tempFormat.polyDefaultLetter="\\partial";
  else
  { tempFormat.polyDefaultLetter=theFormat->WeylAlgebraDefaultLetter;
    tempFormat.polyAlphabeT=theFormat->weylAlgebraLetters;
  }
  std::string firstS=this->polynomialPart.ToString(theFormat);
  std::string secondS=this->differentialPart.ToString(&tempFormat);
  if (firstS!="1")
    out << firstS;
  if (secondS!="1")
    out << secondS;
  return out.str();
}

void SubgroupWeylGroupOLD::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << "generator_reflections: ";
  this->simpleGenerators.WriteToFile(output, theGlobalVariables);
  output << "\nouter_generators: ";
  this->ExternalAutomorphisms.WriteToFile(output, theGlobalVariables);
}

Vector<Rational> SubgroupWeylGroupOLD::GetRho()
{ Vector<Rational> result;
  this->RootsOfBorel.sum(result, this->AmbientWeyl.GetDim());
  result/=2;
  return result;
}

void SubgroupWeylGroupOLD::GetMatrixOfElement(const ElementWeylGroup<WeylGroup>& input, Matrix<Rational>& outputMatrix)const
{ Vectors<Rational> startBasis, imageBasis ;
  startBasis.MakeEiBasis(this->AmbientWeyl.GetDim());
  this->ActByElement(input, startBasis, imageBasis);
  outputMatrix.AssignVectorsToRows(imageBasis);
  outputMatrix.Transpose();
}

void SubgroupWeylGroupOLD::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS;
  input >> tempS;
  this->simpleGenerators.ReadFromFile(input, theGlobalVariables);
  input >> tempS;
  this->ExternalAutomorphisms.ReadFromFile(input, theGlobalVariables);
}

bool SubgroupWeylGroupOLD::ComputeSubGroupFromGeneratingReflections
(Vectors<Rational>* inputGenerators, List<Vectors<Rational> >* inputExternalAutos, GlobalVariables* theGlobalVariables, int UpperLimitNumElements, bool recomputeAmbientRho)
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
  this->AmbientWeyl.TransformToSimpleBasisGenerators(this->simpleGenerators, this->AmbientWeyl.RootSystem);
  this->ComputeRootSubsystem();
  ElementWeylGroup<WeylGroup> tempEW;
  tempEW.generatorsLastAppliedFirst.size=0;
  Vector<Rational> tempRoot;
  tempRoot=(this->AmbientWeyl.rho);
  // rho is invariant under external graph automorphisms (!)
  //Hence the below lines. Needs to be fixed (mathematically).
  //for the time being the below lines remain, until I think of how to do it properly.
  tempRoot*=(50);
  tempRoot[0].AddInteger(1);
  orbitRho.AddOnTop(tempRoot);
  this->AddOnTop(tempEW);
  Vector<Rational> currentRoot;
  for (int i=0; i<this->size; i++)
  { tempEW=(*this)[i];
    for (int j=0; j<this->simpleGenerators.size; j++)
    { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[j], orbitRho[i], false, currentRoot);
      if (!orbitRho.Contains(currentRoot))
      { orbitRho.AddOnTop(currentRoot);
        tempEW.generatorsLastAppliedFirst.AddOnTop(j);
        this->AddOnTop(tempEW);
        tempEW.generatorsLastAppliedFirst.RemoveLastObject();
      }
    }
    for (int j=1; j<this->ExternalAutomorphisms.size; j++)
    { orbitRho[i].GetCoordsInBasiS(this->ExternalAutomorphisms[j], currentRoot);
      if (!orbitRho.Contains(currentRoot))
      { orbitRho.AddOnTop(currentRoot);
        tempEW.generatorsLastAppliedFirst.AddOnTop(j+this->simpleGenerators.size);
        this->AddOnTop(tempEW);
        tempEW.generatorsLastAppliedFirst.RemoveLastObject();
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

void DrawingVariables::drawBufferNoIniT(DrawOperations& theOps)
{ MacroRegisterFunctionWithName("DrawingVariables::drawBufferNoIniT");
  //this->LockedWhileDrawing.LockMe();
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
    switch (theOps.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawText:
        if (this->theDrawTextFunction!=0)
        { DrawTextOperation& theDrawTextOp= theOps.theDrawTextOperations[theOps.IndexNthDrawOperation[i]];
          currentTextStyle=this->GetActualTextStyleFromFlagsAnd(theDrawTextOp.TextStyle);
          if (currentTextStyle==this->TextStyleInvisible)
            break;
          this->theDrawTextFunction(theDrawTextOp.X1, theDrawTextOp.Y1, theDrawTextOp.theText.c_str(), theDrawTextOp.theText.size(), theDrawTextOp.ColorIndex, theDrawTextOp.fontSize);
        }
        break;
      case DrawOperations::typeDrawLine:
        if (this->theDrawLineFunction!=0)
        { DrawLineOperation& theDrawLineOp= theOps.theDrawLineOperations[theOps.IndexNthDrawOperation[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineOp.X1, theDrawLineOp.Y1, theDrawLineOp.X2, theDrawLineOp.Y2, currentPenStyle, theDrawLineOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawLineBetweenTwoVectors:
        if (this->theDrawLineFunction!=0)
        { DrawLineBetweenTwoRootsOperation& theDrawLineBnTwoOp= theOps.theDrawLineBetweenTwoRootsOperations[theOps.IndexNthDrawOperation.TheObjects[i]];
          currentPenStyle= this->GetActualPenStyleFromFlagsAnd(theDrawLineBnTwoOp.thePenStyle);
          if (currentPenStyle==this->PenStyleInvisible)
            break;
          this->theDrawLineFunction(theDrawLineBnTwoOp.precomputedX1, theDrawLineBnTwoOp.precomputedY1, theDrawLineBnTwoOp.precomputedX2, theDrawLineBnTwoOp.precomputedY2, currentPenStyle, theDrawLineBnTwoOp.ColorIndex);
        }
        break;
      case DrawOperations::typeDrawTextAtVector:
        if (this->theDrawTextFunction!=0)
        { DrawTextAtVectorOperation& theDrawTextOp= theOps.theDrawTextAtVectorOperations[theOps.IndexNthDrawOperation.TheObjects[i]];
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
//  this->LockedWhileDrawing.UnlockMe();
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

void WeylGroup::WriteToFile(std::fstream& output)
{ output << "Weyl_group: ";
  stOutput << "This code is not implemented yet (due to a regression).";
  crash << crash;
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
  stOutput << "This code is not implemented yet (due to a regression).";
  crash << crash;
  //input >> this->WeylLetter >> tempI >> tempS;
  if(tempS!="Long_root_length:")
    crash << crash;
//  this->lengthLongestRootSquared.ReadFromFile(input);
  this->CartanSymmetric.ReadFromFile(input);
}

void SemisimpleLieAlgebra::ComputeOneAutomorphism(GlobalVariables& theGlobalVariables, Matrix<Rational>& outputAuto, bool useNegativeRootsFirst)
{ crash << crash;
  rootSubalgebra theRootSA;
//  theRootSA.init(*this);
  int theDimension= this->theWeyl.CartanSymmetric.NumRows;
  theRootSA.genK.MakeEiBasis(theDimension);
  SubgroupWeylGroupOLD theAutos;
  theRootSA.GenerateAutomorphismsPreservingBorel(theGlobalVariables, theAutos);
  Matrix<Rational> mapOnRootSpaces;
  int theAutoIndex= theAutos.ExternalAutomorphisms.size>1? 1 : 0;
  /*if (this->theWeyl.WeylLetter=='D' && theDimension==4)
    theAutoIndex=2;
*/
  mapOnRootSpaces.AssignVectorsToRows(theAutos.ExternalAutomorphisms[theAutoIndex]);
  mapOnRootSpaces.Transpose(theGlobalVariables);
//  mapOnRootSpaces.ComputeDebugString();
//  Matrix<Rational>  theDet=mapOnRootSpaces;
//  Rational tempRat;
//  theDet.ComputeDeterminantOverwriteMatrix(tempRat);
//  stOutput << " ... and the det is: " << tempRat.ToString();
  Selection NonExplored;
  int numRoots= this->theWeyl.RootSystem.size;
  NonExplored.init(numRoots);
  NonExplored.MakeFullSelection();
  Vector<Rational> domainRoot, rangeRoot;

  this->ComputeChevalleyConstants(&theGlobalVariables);
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
      Domain[theIndex]=tempElt;
      NonExplored.RemoveSelection(theIndex);
    }
  }
  Vector<Rational> left, right;
  while(NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      const Vector<Rational>& current = this->theWeyl.RootSystem[theIndex];
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
      currentMat.MakeZero();
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
    current.MakeZero();
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
//  crash << crash;
//  return -1;
}

bool HomomorphismSemisimpleLieAlgebra::ComputeHomomorphismFromImagesSimpleChevalleyGenerators(GlobalVariables& theGlobalVariables)
{ this->theDomain().ComputeChevalleyConstants(&theGlobalVariables);
  this->theRange().ComputeChevalleyConstants(&theGlobalVariables);
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
      tempDomain[index].MakeZero();
      ChevalleyGenerator tempGen;
      tempGen.MakeGenerator(this->theDomain(), this->theDomain().GetGeneratorFromRoot(tempRoot));
      tempDomain[index].AddMonomial(tempGen, 1);
      tempRange[index] = this->imagesSimpleChevalleyGenerators[i+j*theDomainDimension];
      NonExplored.RemoveSelection(index);
//      stOutput <<"<br>" << tempDomain.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//      stOutput <<"->" << tempRange.TheObjects[index].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
    }
  }
//  stOutput << this->ToString(theGlobalVariables) << "<br>";
  Vector<Rational> right;
  while (NonExplored.CardinalitySelection>0)
  { for (int i=0; i<NonExplored.CardinalitySelection; i++)
    { int theIndex = NonExplored.elements[i];
      const Vector<Rational>& current = this->theDomain().theWeyl.RootSystem[theIndex];
      for (int j=0; j<NonExplored.MaxSize; j++)
        if (!NonExplored.selected[j])
        { const Vector<Rational>& left= this->theDomain().theWeyl.RootSystem[j];
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
    this->imagesAllChevalleyGenerators[i].AssignVectorNegRootSpacesCartanPosRootSpaces(imageRoot, this->theRange());
//    stOutput << this->domainAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theDomain);
//    stOutput << "->" << this->imagesAllChevalleyGenerators.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, true, this->theRange);
//    stOutput << "<br>";
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    this->ProjectOntoSmallCartan(input[i], output[i], theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::ProjectOntoSmallCartan(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  invertedSmallCartan;
  invertedSmallCartan=this->theDomain().theWeyl.CartanSymmetric;
  invertedSmallCartan.Invert(&theGlobalVariables);
  int theSmallDimension=this->theDomain().theWeyl.CartanSymmetric.NumRows;
  output.MakeZero(theSmallDimension);
  for (int i=0; i<theSmallDimension; i++)
    output[i]= this->theRange().theWeyl.RootScalarCartanRoot(this->imagesAllChevalleyGenerators[this->theDomain().theWeyl.RootsOfBorel.size+i].GetCartanPart(), input);
  invertedSmallCartan.ActOnVectorColumn(output, output);
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
(const MonomialUniversalEnveloping<RationalFunctionOld>& input, const RationalFunctionOld& theCoeff, ElementUniversalEnveloping<RationalFunctionOld>& output,
 GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<RationalFunctionOld> tempElt;
  output.MakeZero(this->theRange());
  RationalFunctionOld polyOne;
  polyOne=theCoeff.GetOne();
  output.MakeConst(theCoeff, this->theRange());
  for (int i=0; i<input.generatorsIndices.size; i++)
  { if (input.generatorsIndices[i]>=this->imagesAllChevalleyGenerators.size)
      return false;
    tempElt.AssignElementLieAlgebra(this->imagesAllChevalleyGenerators[input.generatorsIndices[i]], this->theRange(), polyOne, polyOne.GetZero());
    RationalFunctionOld& thePower=input.Powers[i];
    int theIntegralPower;
    if (!thePower.IsSmallInteger(&theIntegralPower))
      return false;
    for (int j=0; j<theIntegralPower; j++)
      output*=tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output)
{ if(&output==&input)
    crash << crash;
  output.MakeZero();
  for (int i=0; i<input.size(); i++)
  { int currentIndex=input[i].theGeneratorIndex;
    output+=this->imagesAllChevalleyGenerators[currentIndex]*input.theCoeffs[i];
  }
}

void HomomorphismSemisimpleLieAlgebra::GetMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output)
{ output.init(this->theRange().GetRank(), this->theDomain().GetRank());
  ElementSemisimpleLieAlgebra<Rational> domainElt, imageElt;
  for (int i=0; i<this->theDomain().GetRank(); i++)
  { domainElt.MakeHgenerator
    (Vector<Rational>::GetEi(this->theDomain().GetRank(), i), this->theDomain());
    this->ApplyHomomorphism(domainElt, imageElt);
    output.AssignVectorToColumnKeepOtherColsIntactNoInit(i, imageElt.GetCartanPart());
  }
}

bool HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
(const ElementUniversalEnveloping<RationalFunctionOld>& input, ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)
{ if(&output==&input)
    crash << crash;
  output.MakeZero(this->theRange());
  ElementUniversalEnveloping<RationalFunctionOld> tempElt;
  for (int i=0; i<input.size(); i++)
  { if(!this->ApplyHomomorphism(input[i], input.theCoeffs[i], tempElt, theGlobalVariables))
      return false;
    output+=tempElt;
  }
  return true;
}

void HomomorphismSemisimpleLieAlgebra::MakeGinGWithId(char theWeylLetter, int theWeylDim, ListReferences<SemisimpleLieAlgebra>& ownerOfAlgebras, GlobalVariables& theGlobalVariables)
{ SemisimpleLieAlgebra tempSS;
  tempSS.theWeyl.MakeArbitrarySimple(theWeylLetter, theWeylDim);
  int theIndex=ownerOfAlgebras.AddNoRepetitionOrReturnIndexFirst(tempSS);
  this->domainAlg=&ownerOfAlgebras[theIndex];
  this->rangeAlg=&ownerOfAlgebras[theIndex];
  this->theDomain().ComputeChevalleyConstants(&theGlobalVariables);
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
//  stOutput << this->ToString(theGlobalVariables);
}

void HomomorphismSemisimpleLieAlgebra::ToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables)
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
  { out << "<br>" << this->domainAllChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat)
    << " \\mapsto " << this->imagesAllChevalleyGenerators[i].ToString(&theGlobalVariables.theDefaultFormat);
  }
  output=out.str();
}

class slTwoInSlN;

void HomomorphismSemisimpleLieAlgebra::GetRestrictionAmbientRootSystemToTheSmallerCartanSA(Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ List<Vector<Rational> >& theRootSystem= this->theRange().theWeyl.RootSystem;
  int rankSA=this->theDomain().theWeyl.GetDim();
  Matrix<Rational> tempMat;
  tempMat=(this->theDomain().theWeyl.CartanSymmetric);
  tempMat.Invert(&theGlobalVariables);
  int numPosRootsDomain=this->theDomain().theWeyl.RootsOfBorel.size;
  output.SetSize(theRootSystem.size);
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(rankSA);
//  tempMat.ComputeDebugString(true, false);
//  stOutput << tempMat.DebugString << "<br>";
  for (int i=0; i<theRootSystem.size; i++)
  { for (int j=0; j<rankSA; j++)
    { ElementSemisimpleLieAlgebra<Rational>& currentH=this->imagesAllChevalleyGenerators[j+numPosRootsDomain];
      theScalarProducts[j]=this->theRange().theWeyl.RootScalarCartanRoot(currentH.GetCartanPart(), theRootSystem[i]);
    }
    tempMat.ActOnVectorColumn(theScalarProducts, output[i]);
  }
  this->ImagesCartanDomain.SetSize(rankSA);
  for (int i=0; i<rankSA; i++)
    this->ImagesCartanDomain[i]=this->imagesAllChevalleyGenerators[i+numPosRootsDomain].GetCartanPart();
}

bool HomomorphismSemisimpleLieAlgebra::CheckClosednessLieBracket(GlobalVariables& theGlobalVariables)
{ ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vectors<Rational> tempRoots;
  Vector<Rational> tempRoot;
  tempRoots.SetSize(this->imagesAllChevalleyGenerators.size);
  for (int i=0; i<tempRoots.size; i++)
    this->imagesAllChevalleyGenerators[i].ElementToVectorNegativeRootSpacesFirst(tempRoots[i]);
  for (int i=0; i<this->imagesAllChevalleyGenerators.size; i++)
    for (int j=0; j<this->imagesAllChevalleyGenerators.size; j++)
    { this->theRange().LieBracket(this->imagesAllChevalleyGenerators[i], this->imagesAllChevalleyGenerators[j], tempElt);
      tempElt.ElementToVectorNegativeRootSpacesFirst(tempRoot);
      if(!tempRoots.LinSpanContainsVector(tempRoot))
        return false;
    }
  return true;
}

void ChevalleyGenerator::CheckConsistencyWithOther(const ChevalleyGenerator& other)const
{ if (this->owneR!=other.owneR)
    crash << "This is a programming error: attempting to compare Chevalley generators of different Lie algebras. " << crash;
}

bool ChevalleyGenerator::CheckInitialization()const
{ if (this->owneR==0)
  { crash << "This is a programming error: attempting to use a non-initialized Chevalley generator. " << crash;
    return false;
  }
  return true;
}

std::string ChevalleyGenerator::ToString(FormatExpressions* inputFormat)const
{ this->CheckInitialization();
  return this->owneR->GetStringFromChevalleyGenerator(this->theGeneratorIndex, inputFormat);
}

bool ChevalleyGenerator::operator>(const ChevalleyGenerator& other)const
{ return this->theGeneratorIndex>other.theGeneratorIndex;
}

std::string SemisimpleLieAlgebra::GetStringFromChevalleyGenerator(int theIndex, FormatExpressions* thePolynomialFormat)const
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

void SemisimpleLieAlgebra::OrderNilradicalFirstTotalWeightDescending(const Selection& parSelZeroMeansLeviPart)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::OrderNilradicalFirstTotalWeightDescending");
  Vector<Rational> tempVect;
  tempVect=parSelZeroMeansLeviPart;
  for (int i=0; i<this->GetNumGenerators(); i++)
    if(this->GetWeightOfGenerator(i).ScalarEuclidean(tempVect)<0)
      this->UEGeneratorOrderIncludingCartanElts[i]=-i-this->GetNumGenerators()*5;
}

void SemisimpleLieAlgebra::OrderNilradicalFirstTotalWeightAscending(const Selection& parSelZeroMeansLeviPart)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::OrderNilradicalFirstTotalWeightDescending");
  Vector<Rational> tempVect;
  tempVect=parSelZeroMeansLeviPart;
  for (int i=0; i<this->GetNumGenerators(); i++)
    if(this->GetWeightOfGenerator(i).ScalarEuclidean(tempVect)<0)
      this->UEGeneratorOrderIncludingCartanElts[i]=i-this->GetNumGenerators()*5;
}

void SemisimpleLieAlgebra::OrderNilradicalNilWeightAscending(const Selection& parSelZeroMeansLeviPart)
{ Vector<Rational> tempVect;
  tempVect=parSelZeroMeansLeviPart;
  for (int i=0; i<this->GetNumGenerators(); i++)
  { Rational translationCoeff=this->GetWeightOfGenerator(i).ScalarEuclidean(tempVect)* this->GetNumPosRoots();
    if (translationCoeff<0)
      this->UEGeneratorOrderIncludingCartanElts[i]=i+translationCoeff.NumShort* this->GetNumGenerators()*5;
  }
}

void SemisimpleLieAlgebra::OrderNilradicalNilWeightDescending(const Selection& parSelZeroMeansLeviPart)
{ Vector<Rational> tempVect;
  tempVect=parSelZeroMeansLeviPart;
  for (int i=0; i<this->GetNumGenerators(); i++)
  { Rational translationCoeff=this->GetWeightOfGenerator(i).ScalarEuclidean(tempVect)* this->GetNumPosRoots();
    if (translationCoeff<0)
      this->UEGeneratorOrderIncludingCartanElts[i]=-i+translationCoeff.NumShort* this->GetNumGenerators()*5;
  }
}

void SemisimpleLieAlgebra::OrderNilradical(const Selection& parSelZeroMeansLeviPart, bool useNilWeight, bool ascending)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::OrderNilradical");
  if (useNilWeight)
  { if (ascending)
      this->OrderNilradicalNilWeightAscending(parSelZeroMeansLeviPart);
    else
      this->OrderNilradicalNilWeightDescending(parSelZeroMeansLeviPart);
  } else
    if (ascending)
      this->OrderNilradicalFirstTotalWeightAscending(parSelZeroMeansLeviPart);
    else
      this->OrderNilradicalFirstTotalWeightDescending(parSelZeroMeansLeviPart);
}

void SemisimpleLieAlgebra::OrderSSalgebraForHWbfComputation()
{ int numPosRoots=this->GetNumPosRoots();
  for (int i=0; i<numPosRoots; i++)
    this->UEGeneratorOrderIncludingCartanElts[i]=-1;
}

void SemisimpleLieAlgebra::OrderStandardAscending()
{ int numGens=this->GetNumGenerators();
  for (int i=0; i<numGens; i++)
    this->UEGeneratorOrderIncludingCartanElts[i]=i;
}

void SemisimpleLieAlgebra::OrderStandardDescending()
{ int numGens=this->GetNumGenerators();
  for (int i=0; i<numGens; i++)
    this->UEGeneratorOrderIncludingCartanElts[i]=numGens-i-1;
}

bool SemisimpleLieAlgebra::AreOrderedProperly(int leftIndex, int rightIndex)
{ return this->UEGeneratorOrderIncludingCartanElts[leftIndex]<= this->UEGeneratorOrderIncludingCartanElts[rightIndex];
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

void CGI::ReplaceEqualitiesAndAmpersandsBySpaces(std::string& inputOutput)
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
    for (int j=0; j<this->thePartitions[i].size; j++)
    { int theCoefficient=thePartitions[i][j];
      if (theCoefficient!=0)
      { if(found)
          out << "+";
        found=true;
        if (theCoefficient>1)
          out << theCoefficient;
        out << this->PartitioningRoots[j].ToString();
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
    this->Numerator.GetElement().MakeConst(this->ratValue);
  }
  if (this->expressionType==this->typePoly && this->expressionType<theType)
  { this->expressionType=this->typeRationalFunction;
    this->Denominator.GetElement().MakeConst(1);
  }
  return true;
}

void RationalFunctionOld::Invert()
{ //stOutput << "inverting " << this->ToString();
  if(!this->checkConsistency())
    crash << crash;
  if (this->expressionType==this->typeRational)
  { if (this->ratValue.IsEqualToZero())
      crash  << "This is a programming error: division by zero. Division by zero errors must be caught earlier in the program and "
      << "handled gracefully. Crashing ungracefully. " << crash;
    this->ratValue.Invert();
    return;
  }
  if (this->expressionType==this->typePoly)
    this->ConvertToType(this->typeRationalFunction);
  if(this->Numerator.GetElement().IsEqualToZero())
    crash << crash;
  MathRoutines::swap(this->Numerator.GetElement(), this->Denominator.GetElement());
  this->expressionType=this->typeRationalFunction;
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if(!this->checkConsistency())
    crash << crash;
  //stOutput << " to get: " << this->ToString();
}

bool RationalFunctionOld::checkConsistency()const
{ if (this->expressionType==this->typePoly)
  { if (this->Numerator.IsZeroPointer())
    { crash << "This is a programming error: a rational function is flagged as being a non-constant polynomial, but the numerator pointer is zero. " << crash;
      return false;
    }
    if (this->Numerator.GetElementConst().IsConstant())
    { crash << "This is a programming error: a rational funtion is flagged as having a non-constant numerator, but the numerator is constant. " << crash;
      return false;
    }
//      int commentMeOutWhenDoneDebugging=-1;
//      this->Numerator.GetElementConst().GrandMasterConsistencyCheck();
  }
  if (this->expressionType==this->typeRationalFunction)
  { if (this->Numerator.IsZeroPointer() || this->Denominator.IsZeroPointer())
    { crash << "This is a programming error: a rational function is flagged as having non-constant denominator, but either the numerator or the denominator pointer is zero. "
      << crash;
      return false;
    }
    if (this->Denominator.GetElementConst().IsConstant())
    { crash << "This is a programming error: a rational function is flagged as having non-constant denominator, but the denominator is constant. " << crash;
      return false;
    }
  }
  if (this->expressionType!=this->typeRational && this->expressionType!=this->typePoly && this->expressionType!=this->typeRationalFunction)
  { crash << "This is a programming error: a rational function is not initialized properly: its type is " << this->expressionType
    << " which is not allowed. " << crash;
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
  bool useFrac= theFormat==0 ? false : theFormat->flagUseFrac;
  bool needParenthesis=false;
  if (!useFrac)
    needParenthesis=this->Numerator.GetElementConst().NeedsParenthesisForMultiplication();
  if(useFrac)
    out << "\\frac{";
  if (needParenthesis)
    out << "(";
  out << this->Numerator.GetElementConst().ToString(theFormat);
  if (needParenthesis)
    out << ")";
  if (useFrac)
    out << "}{";
  else
    out << "/(";
  out << this->Denominator.GetElementConst().ToString(theFormat);
  if (useFrac)
    out << "}";
  else
    out << ")";
//  out << " Num vars: " << this->GetNumVars();
  return out.str();
}

void RationalFunctionOld::gcd(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output, GlobalVariables* theGlobalVariables)
{ if (RationalFunctionOld::gcdQuicK(left, right, output))
    return;
  MacroRegisterFunctionWithName("RationalFunctionOld::gcd");
  Polynomial<Rational> lcmBuf, prodBuf, remBuf;
  RationalFunctionOld::lcm(left, right, lcmBuf, theGlobalVariables);
  prodBuf=left;
  prodBuf*=right;
//  stOutput << "<hr>the product: " << buffer2.ToString() << " and the lcm: " << buffer1.ToString() << "<br>";
  prodBuf.DivideBy(lcmBuf, output, remBuf);
  if (!remBuf.IsEqualToZero() || output.IsEqualToZero() )
    crash << "This is a programming error. <br>While computing the gcd of left=" << left.ToString() << " <br>and right="
    << right.ToString() << " <br>I got that left*right= " << prodBuf.ToString() << "<br>, and that lcm(left,right)="
    << lcmBuf.ToString() << " <br>but at the same time right*left divided by lcm (left, right) equals <br>" << output.ToString()
    << "<br> with remainder " << remBuf.ToString() << ", which is imposible. <br>The Groebner basis follows. <br>" << crash;
  output.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
//  stOutput << "<br>and the result of gcd (product/lcm)= " << output.ToString() << "<hr>";
}

void RationalFunctionOld::MakeOneLetterMoN(int theIndex, const Rational& theCoeff, GlobalVariables* theGlobalVariables, int ExpectedNumVars)
{ if ( theIndex<0)
    crash << "This is a programming error: I am asked to create Monomial which has a variable of negative index " << theIndex << ". " << crash;
  this->expressionType=this->typePoly;
  ExpectedNumVars=MathRoutines::Maximum(theIndex+1, ExpectedNumVars);
  this->Numerator.GetElement().MakeDegreeOne(ExpectedNumVars, theIndex, theCoeff);
  this->context=theGlobalVariables;
}

void RationalFunctionOld::MakeMonomiaL
(int LetterIndex, const Rational& Power, const Rational& Coeff, int ExpectedNumVars)
{ if (LetterIndex<0)
    crash << "This is a programming error: I am asked to create Monomial which has a variable of negative index " << LetterIndex << ". " << crash;
  this->expressionType=this->typePoly;
  this->Numerator.GetElement().MakeMonomiaL(LetterIndex, Power, Coeff, ExpectedNumVars);
}

void RationalFunctionOld::SetNumVariablesSubDeletedVarsByOne(int newNumVars)
{ int oldNumVars=this->GetMinNumVars();
  this->Numerator.GetElement().SetNumVariablesSubDeletedVarsByOne(newNumVars);
  this->Denominator.GetElement().SetNumVariablesSubDeletedVarsByOne(newNumVars);
  if (newNumVars<oldNumVars)
    this->Simplify();
//    if(!this->checkConsistency())
//      crash << crash;
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
      if(other.Numerator.IsZeroPointer() )
        crash << crash;
      this->Numerator.GetElement()=other.Numerator.GetElementConst();
      //The below is for testing purposes. I think it is generally better to comment those lines!
      //this->Denominator.FreeMemory();
      break;
    case RationalFunctionOld::typeRationalFunction:
      if(other.Numerator.IsZeroPointer() || other.Denominator.IsZeroPointer())
        crash << crash;
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

void RationalFunctionOld::lcm(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("RationalFunctionOld::lcm");
  MemorySaving<Polynomial<Rational> > buffer1, buffer2, buffer3;
  Polynomial<Rational> leftTemp, rightTemp, tempP;
  MemorySaving<List<Polynomial<Rational> > > bufferList;
  List<Polynomial<Rational> > theBasis;
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
/*  stOutput << "<br>In the beginning: <br>";
  for (int i=0; i<theBasis.size; i++)
  { stOutput << theBasis[i].ToString() << "<br>\n";
  }*/
  MemorySaving<GroebnerBasisComputation<Rational> > bufComp;
  GroebnerBasisComputation<Rational>& theComp=theGlobalVariables==0?
  bufComp.GetElement(): theGlobalVariables->theGroebnerBasisComputation.GetElement();
  theComp.thePolynomialOrder.theMonOrder=MonomialP::LeftIsGEQLexicographicLastVariableStrongest;
  theComp.TransformToReducedGroebnerBasis(theBasis, theGlobalVariables);
//  stOutput << "<br><br> ... and the basis is: <br>";
//  for (int i=0; i<tempList.size; i++)
//  { stOutput << tempList[i].ToString() << "<br>\n";
//  }
  int maxMonNoTIndex=-1;
  Rational MaxTotalDeg;
  for(int i=0; i<theBasis.size; i++)
  { const MonomialP& currentMon= theBasis[i][theBasis[i].GetIndexMaxMonomialLexicographicLastVariableStrongest()];
    if (currentMon(theNumVars)==0)
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
  { crash << "This is a programming error: failed to obtain lcm of two rational functions. The list of polynomials is: ";
    for (int i=0; i<theBasis.size; i++)
      crash << theBasis[i].ToString() << ", ";
    crash << crash;
  }
  output=theBasis[maxMonNoTIndex];
  output.SetNumVariablesSubDeletedVarsByOne(theNumVars);
  output.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
}

void RationalFunctionOld::operator*=(const MonomialP& other)
{ //stOutput << "<br>Multiplying " << this->ToString() << " times " << other.ToString();
  Polynomial<Rational> otherP;
  otherP.MakeZero();
  otherP.AddMonomial(other, 1);
  //stOutput << ", otherP is: " << otherP.ToString();
  (*this)*=otherP;
  //stOutput << " to get " << this->ToString();
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
  if(!tempP.IsEqualToZero())
    crash << crash;
  this->Numerator.GetElement()=theResult;
  this->Denominator.GetElement().DivideBy(theGCD, theResult, tempP);
  //theGCD.ComputeDebugString();
  //theResult.ComputeDebugString();
//  tempP.ComputeDebugString();
  if(!tempP.IsEqualToZero())
    crash << crash;
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
  if(!this->checkConsistency())
    crash << crash;
}


void RationalFunctionOld::operator*=(const Rational& other)
{ //if(!this->checkConsistency()) crash << crash;
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
    stOutput << out.str();
    theReport.Report(out.str());
  }
  RationalFunctionOld::gcd(other.Denominator.GetElementConst(), this->Numerator.GetElement(), theGCD1, this->context);
  RationalFunctionOld::gcd(this->Denominator.GetElement(), other.Numerator.GetElementConst(), theGCD2, this->context);
  this->Numerator.GetElement().DivideBy(theGCD1, tempP1, tempP2);
  this->Numerator.GetElement()=tempP1;
  if(!tempP2.IsEqualToZero())
    crash << crash;
  other.Denominator.GetElementConst().DivideBy(theGCD1, tempP1, tempP2);
  if(!tempP2.IsEqualToZero())
    crash << crash;
  this->Denominator.GetElement()*=tempP1;
  this->Denominator.GetElement().DivideBy(theGCD2, tempP1, tempP2);
  if(!tempP2.IsEqualToZero())
    crash << crash;
  this->Denominator.GetElement()=tempP1;
  other.Numerator.GetElementConst().DivideBy(theGCD2, tempP1, tempP2);
  if(!tempP2.IsEqualToZero())
    crash << crash;
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

void RationalFunctionOld::operator+=(const RationalFunctionOld& other)
{ if (this==&other)
  { *this*=(Rational)2;
    return;
  }
  if(!this->checkConsistency())
    crash << crash;
  if(!other.checkConsistency())
    crash << crash;
  if (this->context==0)
    this->context=other.context;
  if (other.expressionType< this->expressionType)
  { RationalFunctionOld tempRF;
    tempRF=other;
    tempRF.ConvertToType(this->expressionType);
    this->AddSameTypes(tempRF);
    if(!this->checkConsistency())
      crash << crash;
    return;
  }
  if (this->expressionType==other.expressionType)
  { //std::string tempS;
    //tempS=other.ToString();
    this->AddSameTypes(other);
    if(!this->checkConsistency())
      crash << crash;
    return;
  }
  if (this->expressionType<other.expressionType)
  { this->ConvertToType(other.expressionType);
    this->AddSameTypes(other);
    if(!this->checkConsistency())
      crash << crash;
  }
  if(!this->checkConsistency())
    crash << crash;
}

void RationalFunctionOld::Simplify()
{ MacroRegisterFunctionWithName("RationalFunctionOld::Simplify");
  if (this->expressionType==this->typeRationalFunction)
    if(!this->Numerator.GetElement().IsEqualToZero())
    { Polynomial<Rational> theGCD, tempP, tempP2;
//      stOutput << "<br>fetching gcd of " << this->Numerator.GetElement().ToString() << " and "
//      << this->Denominator.GetElement().ToString() << "<br>";
      this->gcd(this->Numerator.GetElement(), this->Denominator.GetElement(), theGCD, this->context);
      if (theGCD.IsEqualToZero())
        crash << "This is a programing error: " << " while fetching the gcd of " << this->Numerator.GetElement().ToString() << " and " << this->Denominator.GetElement().ToString()
        << " I got 0, which is impossible. " << crash;
//      stOutput << "to get " << theGCD.ToString();
//      stOutput << "<br>dividing " << this->Numerator.GetElement().ToString() << " by " << theGCD.ToString() << "<br>";
      this->Numerator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Numerator.GetElement()=(tempP);
//      stOutput << "<br>the critical divide: " << this->Denominator.GetElement().ToString() << " by " << theGCD.ToString() << ".";
      //crash << crash ;
      this->Denominator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Denominator.GetElement()=(tempP);
    }
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
//  int commentMeWhendone;
//  if(!this->checkConsistency())
//   crash << crash;
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
    if (this->Numerator.GetElement().IsConstant(&tempRat))
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

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::AssignFromCoordinateFormWRTBasis
(List<ElementUniversalEnveloping<coefficient> >& theBasis, Vector<coefficient>& input, SemisimpleLieAlgebra& owner)
{ /*int numVars=0;
  if (theBasis.size>0)
    numVars= theBasis[0].GetNumVars();*/
  this->MakeZero(owner);
  ElementUniversalEnveloping<coefficient> tempElt;
  for (int i=0; i<input.size; i++)
    if (!input[i].IsEqualToZero())
    { tempElt.operator=(theBasis[i]);
      tempElt.operator*=(input[i]);
      this->operator+=(tempElt);
    }
}

void RationalFunctionOld::ScaleClearDenominator(List<RationalFunctionOld>& input, Vector<Polynomial<Rational> >& output)
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

void SemisimpleLieAlgebraOrdered::GetLinearCombinationFrom(ElementSemisimpleLieAlgebra<Rational>& input, Vector<Rational>& theCoeffs)
{ theCoeffs.MakeZero(this->theOwner.GetNumGenerators());
  for (int i=0; i<input.size(); i++)
  { int theIndex=input[i].theGeneratorIndex;
    theCoeffs[this->theOwner.GetGeneratorFromRootIndex(theIndex)]=input.theCoeffs[i];
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
  this->ChevalleyGeneratorsInCurrentCoords.MakeZero();
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  Vector<Rational> coordsInCurrentBasis;
  ElementSemisimpleLieAlgebra<Rational> currentElt;
  for (int i=0; i<owner.GetNumGenerators(); i++)
  { currentElt.MakeGenerator(i, owner);
    currentElt.GetCoordsInBasis(this->theOrder, coordsInCurrentBasis, theGlobalVariables);
    for (int j=0; j<coordsInCurrentBasis.size; j++)
      this->ChevalleyGeneratorsInCurrentCoords.elements[j][i]=coordsInCurrentBasis[j];
//    stOutput << "<br> " << currentElt.ToString() << " in new coords becomes: " << coordsInCurrentBasis.ToString();
  }
//  stOutput << this->ChevalleyGeneratorsInCurrentCoords.ToString(true, false) << "<br><br>";

 // this->ChevalleyGeneratorsInCurrentCoords.Invert(theGlobalVariables);
  //stOutput << this->ChevalleyGeneratorsInCurrentCoords.ToString(true, false);
}

void SemisimpleLieAlgebraOrdered::initDefaultOrder(SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ List<ElementSemisimpleLieAlgebra<Rational> > defaultOrder;
  defaultOrder.SetSize(owner.GetNumGenerators());
  for (int i=0; i<defaultOrder.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt=defaultOrder[i];
    currentElt.MakeGenerator(i, owner);
  }
  this->init(defaultOrder, owner, theGlobalVariables);
}

template <class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::MustUseBracketsWhenDisplayingMeRaisedToPower()
{ if (this->size==1)
    if (this->theCoeffs[0]==1)
      return false;
  return true;
}

template <class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::IsACoeffOneChevalleyGenerator(int& outputGenerator, SemisimpleLieAlgebra& owner)
{ if (this->size==1)
    return this->theCoeffs[0]==1;
  return false;
}

void HomomorphismSemisimpleLieAlgebra::GetWeightsWrtKInSimpleCoordsK
(Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra<Rational> >& inputElts, GlobalVariables& theGlobalVariables)
{ outputWeights.SetSize(inputElts.size);
  Rational tempRat;
  ElementSemisimpleLieAlgebra<Rational> tempLieElement;
  for (int i=0; i<inputElts.size; i++)
  { Vector<Rational>& currentWeight= outputWeights[i];
    currentWeight.MakeZero(this->theDomain().GetRank());
    ElementSemisimpleLieAlgebra<Rational>& currentLieElt=inputElts[i];
    for (int j=0; j<this->theDomain().GetRank(); j++)
    { this->theRange().LieBracket(this->imagesAllChevalleyGenerators[j+this->theDomain().GetNumPosRoots()], currentLieElt, tempLieElement);
      if(!currentLieElt.IsProportionalTo(tempLieElement, tempRat))
        crash << crash;
      currentWeight[j]=tempRat;
    }
  }
  Matrix<Rational>  tempMat=this->theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert();
  tempMat.ActOnVectorsColumn(outputWeights);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::GetBasisFromSpanOfElements
(List<ElementSemisimpleLieAlgebra>& theElements, Vectors<RationalFunctionOld>& outputCoords, List<ElementSemisimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> theRootForm;
  theRootForm.SetSize(theElements.size);
  for(int i=0; i<theElements.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=theElements[i];
    currentElt.ElementToVectorNegativeRootSpacesFirst(theRootForm[i]);
  }
//  int theRank=0; int numRoots=0;
//  if (theElements.size>0)
//  { theRank=theElements.TheObjects[0].Hcomponent.size;
//    numRoots=theElements.TheObjects[0].coeffsRootSpaces.size;
//  }
  theRootForm.ChooseABasis(theGlobalVariables);
  outputTheBasis.SetSize(theRootForm.size);
  for(int i=0; i<theRootForm.size; i++)
  { ElementSemisimpleLieAlgebra& currentElt=outputTheBasis[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(theRootForm[i], (*currentElt.ownerArray)[currentElt.indexOfOwnerAlgebra]);
  }

}

bool RationalFunctionOld::gcdQuicK(const Polynomial<Rational>& left, const Polynomial<Rational> & right, Polynomial<Rational> & output)
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
      crash << "This is a programming error: attempting to raise 0 to the 0th power, which is undefined. " << crash;
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

void RationalFunctionOld::ClearDenominators(RationalFunctionOld& outputWasMultipliedBy)
{ //outputWasMultipliedBy.MakeConst(this->NumVars, (Rational) 1, this->context);
  Rational tempRat;
  switch(this->expressionType)
  { case RationalFunctionOld::typeRational:
      tempRat=this->ratValue.GetDenominator();
      outputWasMultipliedBy.MakeConst(tempRat, this->context);
      this->ratValue*=tempRat;
    break;
    case RationalFunctionOld::typePoly:
      this->Numerator.GetElement().ClearDenominators(tempRat);
      outputWasMultipliedBy.MakeConst(tempRat, this->context);
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

template <class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::NeedsParenthesisForMultiplication()const
{ return this->size>1;
}

void slTwoInSlN::ClimbDownFromHighestWeightAlongSl2String(Matrix<Rational>& input, Matrix<Rational>& output, Rational& outputCoeff, int generatorPower)
{ if(&input==&output);
    crash << crash;
  Rational currentWeight;
  Matrix<Rational> ::LieBracket(this->theH, input, output);
  bool tempBool=input.IsProportionalTo(output, currentWeight);
  if (!tempBool)
    stOutput << "<br>oh no! climbing down is fucked up!";
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

void slTwoInSlN::ExtractHighestWeightVectorsFromVector(Matrix<Rational> & input, List<Matrix<Rational> >& outputDecompositionOfInput, List<Matrix<Rational> >& outputTheHWVectors)
{ outputDecompositionOfInput.size=0; outputTheHWVectors.size=0;
  Matrix<Rational>  remainder; remainder=input;
  Matrix<Rational>  component, highestWeightVector, tempMat;
  Rational theCoeff, tempRat;
  int largestPowerNotKillingInput;
  while(!remainder.IsEqualToZero() )
  { //stOutput << "<br>remainder:<div class=\"math\">" << remainder.ToString(false, true) << "</div>";
    this->ClimbUpFromVector(remainder, highestWeightVector, largestPowerNotKillingInput);
    //stOutput << "<br>highest weight vector:<div class=\"math\">" << highestWeightVector.ToString(false, true) << "</div>";
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
    //if(theCoeff.IsEqualToZero())
      crash << crash;
    component/=(theCoeff);
    outputDecompositionOfInput.AddOnTop(component);
    //stOutput << "<br>component:<div class=\"math\">" << component.ToString(false, true) << "</div><br><br><br><br>";
    remainder-=(component);
  }
  //remainder.MakeZero();
//  for (int i=0; i<outputVectors.size; i++)
//    remainder.Add(outputVectors.TheObjects[i]);
//  stOutput << "<br>sum of all components:<div class=\"math\">" << remainder.ToString(false, true) << "</div>";

}

void slTwoInSlN::ClimbUpFromVector(Matrix<Rational>& input, Matrix<Rational>& outputLastNonZero, int& largestPowerNotKillingInput)
{ Matrix<Rational>  tempMat;
  if(&input==&outputLastNonZero)
    crash << crash;
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

std::string slTwoInSlN::initFromModuleDecomposition(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable)
{ std::stringstream out;
  this->thePartition=(decompositionDimensions);
  this->thePartition.QuickSortDescending();
  this->theDimension=0;
  for (int i=0; i<this->thePartition.size; i++)
    this->theDimension+=this->thePartition.TheObjects[i];
  theH.init(this->theDimension, this->theDimension); theH.MakeZero();
  theE.init(this->theDimension, this->theDimension); theE.MakeZero();
  theF.init(this->theDimension, this->theDimension); theF.MakeZero();
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
  { this->theProjectors[i].init(this->theDimension, this->theDimension);
    this->theProjectors[i].MakeZero();
    for (int j=0; j<this->thePartition[i]; j++)
    { theH.elements[currentOffset+j][currentOffset+j]=this->thePartition[i]-1-2*j;
      this->theProjectors[i].elements[currentOffset+j][currentOffset+j]=1;
      if (j!=this->thePartition[i]-1)
      { theF.elements[currentOffset +j+1][currentOffset +j]=1;
        theE.elements[currentOffset +j][currentOffset +j+1]=(j+1)*(this->thePartition[i]-j-1);
      }
    }
    currentOffset+=this->thePartition[i];
  }
  out << newLine << beginMath << "h=" << this->ElementMatrixToTensorString(this->theH, useHtml) << "="
  << this->theH.ElementToStringWithBlocks(this->thePartition) << endMath;
  out << newLine << beginMath << "e=" << this->ElementMatrixToTensorString(this->theE, useHtml) << "="
  << this->theE.ElementToStringWithBlocks(this->thePartition) << endMath;
  out << newLine << beginMath << "f=" << this->ElementMatrixToTensorString(this->theF, useHtml) << "="
  << this->theF.ElementToStringWithBlocks(this->thePartition) << endMath;
  Matrix<Rational>  tempMat;
  tempMat.init(this->theDimension, this->theDimension);
  List<Matrix<Rational> > Decomposition, theHwCandidatesBeforeProjection, theHwCandidatesProjected;
  this->theHighestWeightVectors.size=0;
  this->theGmodKModules.size=0;
  for (int i=0; i<this->theDimension; i++)
    for (int j=0; j< this->theDimension; j++)
    { tempMat.MakeZero();
      tempMat.elements[i][j]=1;
      this->ExtractHighestWeightVectorsFromVector(tempMat, Decomposition, theHwCandidatesBeforeProjection);
      theHwCandidatesProjected.size=0;
      for (int k=0; k<theHwCandidatesBeforeProjection.size; k++)
        for (int l=0; l<this->theProjectors.size; l++)
        { tempMat=theHwCandidatesBeforeProjection[k];
          tempMat.MultiplyOnTheLeft(this->theProjectors[l]);
          if (!tempMat.IsEqualToZero())
            theHwCandidatesProjected.AddOnTop(tempMat);
        }
      for (int k=0; k<theHwCandidatesProjected.size; k++)
        if (this->GetModuleIndexFromHighestWeightVector(theHwCandidatesProjected[k])==-1)
        { Matrix<Rational>& currentHighest=theHwCandidatesProjected[k];
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
  { out << newLine << beginMath << this->ElementMatrixToTensorString(theHighestWeightVectors[i], useHtml)
    << endMath << ", highest weight of ";
    out << beginMath << this->ElementModuleIndexToString(i, useHtml) << endMath;
   //for (int j=1; j<this->theGmodKModules[i].size; j++)
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
  { this->PairingTable[i].SetSize(this->theHighestWeightVectors.size);
    for(int j=0; j<this->PairingTable[i].size; j++)
    { List<int>& currentPairing=this->PairingTable[i][j];
      out << this->PairTwoIndices(currentPairing, i, j, useHtml);
    }
  }
  return out.str();
}

std::string slTwoInSlN::PairTwoIndices(List<int>& output, int leftIndex, int rightIndex, bool useHtml)
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
  List<Matrix<Rational> >& leftElements=this->theGmodKModules[leftIndex];
  List<Matrix<Rational> >& rightElements=this->theGmodKModules[rightIndex];
  Matrix<Rational> tempMat;
  List<Matrix<Rational> > HighestWeightsContainingModules;
  List<Matrix<Rational> > tempDecomposition;
  for (int i=0; i<leftElements.size; i++)
    for (int j=0; j<rightElements.size; j++)
    { Matrix<Rational>& leftElt=leftElements[i];
      Matrix<Rational>& rightElt=rightElements[j];
      Matrix<Rational>::LieBracket(leftElt, rightElt, tempMat);
      if (!tempMat.IsEqualToZero())
      { this->ExtractHighestWeightVectorsFromVector(tempMat, tempDecomposition, HighestWeightsContainingModules);
        for (int k=0; k<HighestWeightsContainingModules.size; k++)
        { output.AddOnTopNoRepetition(this->GetModuleIndexFromHighestWeightVector(HighestWeightsContainingModules[k]));
          if (this->GetModuleIndexFromHighestWeightVector(HighestWeightsContainingModules[k])==-1)
            stOutput << newLine << beginMath << "[" << leftElt.ToString(&latexFormat) << ", "
            << rightElt.ToString(&latexFormat) << "]=" << tempMat.ToString(&latexFormat) << endMath;
        }
      }
    }
  out << newLine << beginMath << this->ElementModuleIndexToString(leftIndex, useHtml) << endMath << " and "
  << beginMath << this->ElementModuleIndexToString(rightIndex, useHtml) << endMath << " pair to: ";
  for (int i=0; i<output.size; i++)
  { out << beginMath << this->ElementModuleIndexToString(output[i], useHtml) << endMath;
    if (i!=output.size-1)
      out << beginMath << "\\oplus" << endMath;
  }
  if (output.size>0)
  { out << "  hw vectors: ";
    for (int i=0; i<output.size; i++)
      out << beginMath << this->ElementMatrixToTensorString(this->theHighestWeightVectors[output[i]], useHtml) << endMath << ",";
  }
  return out.str();
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
    if(this->Denominator.GetElement().IsEqualToZero())
      crash << crash;
    this->Simplify();
//    this->ComputeDebugString();
  } else
  { stOutput << "Adding " << this->ToString() << " + " << other.ToString();
    this->Numerator.GetElement()*=(tempRat);
    this->Denominator.GetElement()*=(tempRat);
    this->Numerator.GetElement()+=(other.Numerator.GetElementConst());
    this->ReduceMemory();
    this->SimplifyLeadingCoefficientOnly();
  }
  if(!this->checkConsistency())
    crash << crash;
}

void CGI::CGIStringToNormalString(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (CGI::CGIStringToNormalStringOneStep(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

void CGI::CGIFileNameToFileName(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (readAhead=="+")
      { out << "+";
        break;
      }
      if (CGI::CGIStringToNormalStringOneStep(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

void CGI::ChopCGIInputStringToMultipleStrings(const std::string& input, List<std::string>& outputData, List<std::string>& outputFieldNames)
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

bool CGI::CGIStringToNormalStringOneStep(std::string& readAhead, std::stringstream& out)
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
  if (readAhead=="%40")
  { out << "@";
    return true;
  }
  if (readAhead=="%7E")
  { out << "~";
    return true;
  }
  if (readAhead=="%22")
  { out << "\"";
    return true;
  }
  if (readAhead=="%21")
  { out << "!";
    return true;
  }
  if (readAhead=="%09")//code for tab character, replaced by space
  { out << " ";
    return true;
  }
  if (readAhead=="%A0")//code for &nbsp;, replaced by space
  { out << " ";
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
    crash << "This is a programming error: comparing two monomials with different number of variables. " << crash;
  for (int i=0; i<m.monBody.size; i++)
    if ((*this)[i]<m[i])
      return false;
  return true;
}

void MonomialP::operator*=(const MonomialP& other)
{ this->SetNumVariablesSubDeletedVarsByOne(MathRoutines::Maximum(this->monBody.size, other.monBody.size));
  for (int i=0; i<other.monBody.size; i++)
    this->monBody[i]+=other.monBody[i];
}

void MonomialP::operator/=(const MonomialP& other)
{ this->SetNumVariablesSubDeletedVarsByOne(MathRoutines::Maximum(this->monBody.size, other.monBody.size));
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
  { tempRat=point.ScalarEuclidean(this->Normals[i]);
    if (tempRat.IsNegative())
      return false;
  }
  return true;
}

void MathRoutines::SplitStringInTwo(const std::string& inputString, int firstStringSize, std::string& outputFirst, std::string& outputSecond)
{ if (&outputFirst==&inputString || &outputSecond==&inputString)
  { std::string inputCopy=inputString;
    MathRoutines::SplitStringInTwo(inputCopy, firstStringSize, outputFirst, outputSecond);
    return;
  }
  if (firstStringSize<0)
    firstStringSize=0;
  if (firstStringSize>(signed) inputString.size())
    firstStringSize=inputString.size();
  outputFirst="";
  outputFirst=inputString.substr(0, firstStringSize);
  outputSecond="";
  int secondStringSize=inputString.size()-firstStringSize;
  if (secondStringSize>0)
    outputSecond= inputString.substr(firstStringSize, secondStringSize);
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
  if (this->IsConstant())
    return "1";
  for (int i=0; i<this->monBody.size; i++)
    if (!(this->monBody[i].IsEqualToZero()))
    { out << theFormat->GetPolyLetter(i);
      if (!(this->monBody[i]==1))
        out << "^{" << this->monBody[i] << "}";
      else
        out << " ";
    }
  return out.str();
}
