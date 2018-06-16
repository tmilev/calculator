//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfImplementationHeader2Math0_General.h"
#include "vpfHeader2Math1_2SemisimpleLieAlgebras_RootSubalgebras.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"

ProjectInformationInstance ProjectInfoVpf9_5RootSAsSl2sas(__FILE__, "Root and sl(2) subalgebras of semisimple Lie algebras. ");

void rootSubalgebra::GetCoxeterElement(Matrix<Rational>& output)
{ int theDim = this->GetAmbientWeyl().GetDim();
  output.MakeIdMatrix(theDim);
  Matrix<Rational> tempMat;
  for (int i = 0; i < this->SimpleBasisK.size; i ++)
  { this->GetAmbientWeyl().GetMatrixReflection(this->SimpleBasisK[i], tempMat);
    output.MultiplyOnTheLeft(tempMat);
  }
}

void rootSubalgebra::GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2)
{ //this->ComputeRho(true);
  int theDimension = this->GetAmbientWeyl().GetDim();
  if (theDimension < 2)
    return;
  if (this->SimpleBasisK.size < 2)
  { if (this->SimpleBasisK.size == 1)
      outputBasis1 = this->SimpleBasisK[0].GetVectorDouble();
    else
      outputBasis1.MakeEi(theDimension, 0);
    if (outputBasis1[0] == 0)
      outputBasis2.MakeEi(theDimension, 0);
    else
      outputBasis2.MakeEi(theDimension, 1);
    return;
  }
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
  Matrix<Rational>  matCoxeterElt;
  this->GetCoxeterElement(matCoxeterElt);
//  stOutput << "the SA Coxeter matrix: " << matCoxeterElt.ToString();
//  tempMat =matCoxeterElt;
  this->ComputeDynkinDiagramKandCentralizer();
  SubgroupWeylGroupOLD tempGroup;
  int coxeterNumber = 1;
  for (int i = 0; i < this->theDynkinDiagram.SimpleBasesConnectedComponents.size; i ++)
  { tempGroup.AmbientWeyl = &this->GetAmbientWeyl();
    tempGroup.simpleGenerators = this->theDynkinDiagram.SimpleBasesConnectedComponents[i];
    tempGroup.ComputeRootSubsystem();
    Vector<Rational>& lastRoot = *tempGroup.RootSubsystem.LastObject();
    Vector<Rational> lastRootInSimpleCoords;
    lastRoot.GetCoordsInBasiS(tempGroup.simpleGenerators, lastRootInSimpleCoords);
    coxeterNumber = MathRoutines::Maximum(lastRootInSimpleCoords.SumCoords().NumShort, coxeterNumber);
  }
//  stOutput << "<hr>the corresponding Coxeter number: " << coxeterNumber;
//  for (int i = 0; i <coxeterNumber- 1; i ++)
//    tempMat.MultiplyOnTheLeft(matCoxeterElt);
//  stOutput << "<br>coxeter transformation to the power of " << coxeterNumber << " equals: " << tempMat.ToString(true, false);
  CompleX<double> theEigenValue;
  theEigenValue.Re = FloatingPoint:: cos(2*MathRoutines::Pi() / coxeterNumber);
  theEigenValue.Im = FloatingPoint:: sin(2*MathRoutines::Pi() / coxeterNumber);
  Matrix<CompleX<double> > eigenMat;
  eigenMat.init(matCoxeterElt.NumRows, matCoxeterElt.NumCols);
  for (int i = 0; i < eigenMat.NumRows; i ++)
    for (int j = 0; j < eigenMat.NumCols; j ++)
    { eigenMat.elements[i][j] = matCoxeterElt.elements[i][j].GetDoubleValue();
      if (i == j)
        eigenMat.elements[i][i] -= theEigenValue;
    }
  List<Vector<CompleX<double> > > theEigenSpaceList;
  eigenMat.GetZeroEigenSpace(theEigenSpaceList);
  Vectors<CompleX<double> > theEigenSpace;
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(theDimension, 1);
  for (int i = 0; i < theDimension; i ++)
    for (int j = 0; j < theDimension; j ++)
      tempDO.theBilinearForm.elements[i][j] =
      this->GetAmbientWeyl().CartanSymmetric.elements[i][j].GetDoubleValue();
  outputBasis1.SetSize(theDimension);
  outputBasis2.SetSize(theDimension);
  if (theEigenSpace.size > 0)
  { if (coxeterNumber > 2)
    { for (int j = 0; j < theDimension; j ++)
      { outputBasis1[j] = theEigenSpace[0][j].Re;
        outputBasis2[j] = theEigenSpace[0][j].Im;
      }
      tempDO.ModifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
    } else if (coxeterNumber <= 2 && theEigenSpace.size > 1)
    { for (int j = 0; j < theDimension; j ++)
      { outputBasis1[j] = theEigenSpace[0][j].Re;
        outputBasis2[j] = theEigenSpace[1][j].Re;
      }
      tempDO.ModifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
    }
  }
}

void rootSubalgebra::ComputeDynkinDiagramKandCentralizer()
{ this->SimpleBasisK = this->genK;
  this->theDynkinDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisK, this->GetAmbientWeyl());
  this->SimpleBasisCentralizerRoots.size = 0;
  for (int i = 0; i < this->GetAmbientWeyl().RootsOfBorel.size; i ++)
    if (this->rootIsInCentralizer(this->GetAmbientWeyl().RootsOfBorel[i]))
      this->SimpleBasisCentralizerRoots.AddOnTop(this->GetAmbientWeyl().RootsOfBorel[i]);
  this->theCentralizerDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisCentralizerRoots, this->GetAmbientWeyl());
}

void rootSubalgebra::ComputeModuleDecompoAmbientAlgebraDimensionsOnly()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeModuleDecompoAmbientAlgebraDimensionsOnly");
  this->moduleDecompoAmbientAlgebraDimensionsOnly.MakeZero();
  for (int i = 0; i < this->GetNumModules(); i ++)
    this->moduleDecompoAmbientAlgebraDimensionsOnly.AddMonomial(MonomialVector(this->Modules[i].size - 1), 1);
}

void rootSubalgebra::ComputeCentralizerFromKModulesAndSortKModules()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeCentralizerFromKModulesAndSortKModules");
  this->CentralizerKmods.init(this->Modules.size);
  this->CentralizerRoots.size = 0;
  this->CentralizerRoots.Reserve(this->Modules.size);
  this->SimpleBasisCentralizerRoots.size = 0;
  this->SimpleBasisCentralizerRoots.Reserve(this->Modules.size);
  if (this->SimpleBasisK.size == 0)
  { if (this->Modules.size != this->GetOwnerSSalg().GetNumGenerators())
      crash << " bad number of modules!" << crash;
  } else
    if (this->theDynkinType.IsEqualToZero())
      crash << "Simple basis is " << this->SimpleBasisK.ToString() << " but Dynkin type is: " << this->theDynkinType.ToString() << crash;
  for (int i = 0; i < this->Modules.size; i ++)
    if (this->Modules[i].size == 1)
    { this->CentralizerKmods.AddSelectionAppendNewIndex(i);
      if (!this->WeightsModulesPrimalSimple[i][0].IsEqualToZero())
      { this->CentralizerRoots.AddOnTop(this->WeightsModulesPrimalSimple[i][0]);
        this->SimpleBasisCentralizerRoots.AddOnTop(this->WeightsModulesPrimalSimple[i][0]);
      }
    }
  this->theCentralizerDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisCentralizerRoots, this->GetAmbientWeyl());
  this->theCentralizerDiagram.GetDynkinType(this->theCentralizerDynkinType);
  if (this->theDynkinType.IsEqualToZero())
    if (this->theCentralizerDynkinType.GetRank() + this->theDynkinType.GetRank() != this->ownEr->owner->GetRank())
      crash << "Centralizer of " << this->theDynkinType.ToString() << " computed to be "
      << this->theCentralizerDynkinType.ToString()
      << " which is impossible. " << crash;
}

void rootSubalgebra::ComputeExtremeWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputW, bool lookingForHighest)
{ this->GetAmbientWeyl().ComputeExtremeRootInTheSameKMod(this->SimpleBasisK, input, outputW, lookingForHighest);
}

void rootSubalgebra::ComputeHighestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputHW)
{ this->ComputeExtremeWeightInTheSameKMod(input, outputHW, true);
}

void rootSubalgebra::ComputeLowestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputLW)
{ this->ComputeExtremeWeightInTheSameKMod(input, outputLW, false);
}

bool rootSubalgebra::RootsDefineASubalgebra(Vectors<Rational>& theRoots)
{ Vector<Rational> tempRoot;
  for (int i = 0; i < theRoots.size; i ++)
  { if (!this->IsARoot(theRoots[i]))
      return false;
    for (int j = i + 1; j < theRoots.size; j ++)
    { tempRoot = theRoots[i];
      tempRoot += theRoots[j];
      if (this->IsARoot(tempRoot))
        if (!theRoots.Contains(tempRoot))
          return false;
    }
  }
  return true;
}

bool rootSubalgebra::IsBKhighest(const Vector<Rational>& input)
{ for (int i = 0; i < this->SimpleBasisK.size; i ++)
    if (this->IsARootOrZero(input + this->SimpleBasisK[i]))
      return false;
  return true;
}

bool rootSubalgebra::IsBKlowest(const Vector<Rational>& input)
{ for (int i = 0; i < this->SimpleBasisK.size; i ++)
    if (this->IsARootOrZero(input - this->SimpleBasisK[i]))
      return false;
  return true;
}

bool rootSubalgebra::rootIsInCentralizer(const Vector<Rational>& input)
{ Vector<Rational> tempRoot;
  for (int i = 0; i < this->SimpleBasisK.size; i ++)
  { tempRoot = input + this->SimpleBasisK[i];
    if (this->IsARoot(tempRoot) || tempRoot.IsEqualToZero())
      return false;
    tempRoot = input - this->SimpleBasisK[i];
    if (this->IsARoot(tempRoot) || tempRoot.IsEqualToZero())
      return false;
  }
  return true;
}

void rootSubalgebra::WriteMultTableAndOppositeKmodsToFile(std::fstream &output, List<List<List<int> > >& inMultTable, List<int>& inOpposites)
{ output << "pairing_table_size: " << inMultTable.size << "\n";
  for (int i = 0; i < inMultTable.size; i ++)
    for (int j = 0; j < inMultTable.size; j ++)
    { output << inMultTable[i][j].size << " ";
      for (int k = 0; k < inMultTable[i][j].size; k ++)
        output << inMultTable[i][j][k] << " ";
    }
  output << "\nopposites: ";
  for (int i = 0; i < inMultTable.size; i ++)
    output << inOpposites[i] << " ";
}

void rootSubalgebra::ReadMultTableAndOppositeKmodsFromFile(std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites)
{ std::string tempS;
  int tempI, theSize;
  input >> tempS >> theSize;
  outMultTable.SetSize(theSize);
  outOpposites.SetSize(theSize);
  for (int i = 0; i < theSize; i ++)
  { outMultTable[i].SetSize(theSize);
    for (int j = 0; j < theSize; j ++)
    { input >> tempI;
      outMultTable[i][j].SetSize(tempI);
      for (int k = 0; k < outMultTable[i][j].size; k ++)
        input >> outMultTable[i][j][k];
    }
  }
  input >> tempS;
  for (int i = 0; i < outMultTable.size; i ++)
    input >> outOpposites[i];
  if (tempS != "opposites:")
    crash << crash;
}

bool rootSubalgebra::rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, Vector<Rational>& input)
{ Vector<Rational> tempRoot;
  bool found = true;
  Vector<Rational> currentRoot = input;
  bool foundPositive = false;
  while (found)
  { found = false;
    for (int k = 0; k < this->SimpleBasisCentralizerRoots.size; k ++)
    { tempRoot = currentRoot - this->SimpleBasisCentralizerRoots[k];
      if (this->IsARoot(tempRoot) || tempRoot.IsEqualToZero())
      { currentRoot = tempRoot;
        found = true;
        if (positiveSimpleRootsSel.selected[k])
          foundPositive = true;
        if (currentRoot.IsEqualToZero())
          return foundPositive;
      }
    }
  }
  return false;
}

void rootSubalgebra::GeneratePossibleNilradicalsRecursive(PauseThread& PauseMutex, List<List<List<int> > >& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner, int indexInOwner)
{ int& RecursionDepth = owner.RecursionDepthNilradicalsGeneration;
  List<int>& counters = owner.CountersNilradicalsGeneration;
  while (RecursionDepth > - 1)
  { while (counters[RecursionDepth] < this->Modules.size)
    { if (!impliedSelections[RecursionDepth].selected[counters[RecursionDepth]])
        if (this->IndexIsCompatibleWithPrevious(counters[RecursionDepth], RecursionDepth, multTable, impliedSelections, oppositeKmods, owner))
        { RecursionDepth ++;
          counters[RecursionDepth] = counters[RecursionDepth - 1];
        }
    /*  if (this->flagAnErrorHasOccuredTimeToPanic)
      { impliedSelections.ElementToStringGeneric(tempSsel, RecursionDepth+ 1);
        impliedSelections.TheObjects[RecursionDepth].ComputeDebugString();
      }*/
      counters[RecursionDepth] ++;
    }
    this->PossibleNilradicalComputation(impliedSelections[RecursionDepth], owner, indexInOwner);
//    if (this->flagAnErrorHasOccuredTimeToPanic)
//      tempSels.AddOnTop(impliedSelections.TheObjects[RecursionDepth]);
    RecursionDepth --;
    if (RecursionDepth > - 1)
      counters[RecursionDepth] ++;
    PauseMutex.SafePointDontCallMeFromDestructors();
  }
}

bool rootSubalgebra::ListHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel)
{ for (int j = 0; j < tempList.size; j ++)
    if (tempList[j] < index)
      if (!tempSel.selected[tempList[j]])
        return false;
  return true;
}

bool rootSubalgebra::IndexIsCompatibleWithPrevious(int startIndex, int RecursionDepth,  List<List<List<int> > >& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner)
{ Selection& targetSel = impliedSelections[RecursionDepth + 1];
  Selection& originalSel = impliedSelections[RecursionDepth];
  targetSel = originalSel;
  targetSel.AddSelectionAppendNewIndex(startIndex);
  for (int k = targetSel.CardinalitySelection - 1; k < targetSel.CardinalitySelection; k ++)
  { int tempI = targetSel.elements[k];
    for (int i = 0; i < targetSel.CardinalitySelection; i ++)
    { if (targetSel.selected[oppositeKmods[targetSel.elements[i]]])
        return false;
      List<int>& tempList = multTable[tempI][targetSel.elements[i]];
      for (int j = 0; j < tempList.size; j ++)
        if (tempList[j] < startIndex && !originalSel.selected[tempList[j]])
          return false;
        else
          targetSel.AddSelectionAppendNewIndex(tempList[j]);
    }
  }
  if (!owner.ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(targetSel))
    return false;
  return true;
}

void rootSubalgebra::PossibleNilradicalComputation(Selection& selKmods, rootSubalgebras& owner, int indexInOwner)
{ this->NumNilradicalsAllowed ++;
  if (owner.flagCountingNilradicalsOnlyNoComputation)
  { owner.numNilradicalsBySA[indexInOwner] ++;
    this->MakeProgressReportPossibleNilradicalComputation(owner);
    return;
  }
  //this->ComputeDebugString();
  if (this->flagFirstRoundCounting)
    this->NumTotalSubalgebras = this->NumNilradicalsAllowed;
  if (!this->flagFirstRoundCounting)
  { this->NilradicalKmods = selKmods;
    if (!this->ConeConditionHolds(owner, indexInOwner, owner.flagComputingLprohibitingWeights))
    { this->NumConeConditionFailures ++;
      owner.NumConeConditionFailures ++;
      if (owner.flagStoringNilradicals)
      { List<List<int> >& currentSAList = owner.storedNilradicals[indexInOwner];
        List<int> newNilradical;
        newNilradical.SetSize(selKmods.CardinalitySelection);
        for (int i = 0; i < selKmods.CardinalitySelection; i ++)
          newNilradical[i] = selKmods.elements[i];
        currentSAList.AddOnTop(newNilradical);
      }
    } else
    {
      //the below commented out code should be incapsulated. It computes whether a given nilradical is a nilradical of a parabolic subalgebra.
      //this task is pushed on the end of the to-do list.
      /* owner.NumConeConditionHoldsBySSpart.TheObjects[indexInOwner]++;
      if (owner.ReportStringNonNilradicalParabolic =="")
      { this->ComputeRootsOfK();
        Vectors<Rational> tempNilradical; Vectors<Rational> tempOthers; Vectors<Rational> tempK;
        for (int i = 0; i < this->kModules.size; i ++)
          if (this->NilradicalKmods.selected[i])
            tempNilradical.AddListOnTop(this->kModules.TheObjects[i]);
          else
            tempOthers.AddListOnTop(this->kModules.TheObjects[i]);
        for (int i = 0; i < this->PositiveRootsK.size; i ++)
        { tempOthers.AddOnTop(this->PositiveRootsK.TheObjects[i]);
          tempOthers.AddOnTop(-this->PositiveRootsK.TheObjects[i]);
          tempK.AddOnTop(this->PositiveRootsK.TheObjects[i]);
        }
        if (Vectors<Rational>::ConesIntersect(empNilradical, tempOthers, owner.AmbientWeyl.CartanSymmetric.NumRows))
        { Vectors<Rational> tempRoots; std::stringstream out; std::string tempS;
          this->AmbientWeyl.GetEpsilonCoords(tempNilradical, tempRoots);
          tempRoots.ElementToStringEpsilonForm(tempS, true, false, false);
          out << tempS;
          this->AmbientWeyl.GetEpsilonCoords(tempK, tempRoots);
          tempRoots.ElementToStringEpsilonForm(tempS, true, false, false);
          out << "\n\n" << tempS;
          owner.ReportStringNonNilradicalParabolic = out.str();
        }
      }*/
    }
  }
  this->MakeProgressReportPossibleNilradicalComputation(owner);
}

void rootSubalgebra::MakeProgressReportGenAutos(int progress, int outOf, int found)
{ ProgressReport theReport;
  std::stringstream out4, out5;
  out5 << progress + 1 << " out of " << outOf << " checked; ";
  out5 << found << " found pos. generators";
  theReport.Report(out5.str());
}

void rootSubalgebra::MakeProgressReportPossibleNilradicalComputation(rootSubalgebras& owner)
{ MacroRegisterFunctionWithName("rootSubalgebra::MakeProgressReportPossibleNilradicalComputation");
  if (!theGlobalVariables.flagReportEverything)
    return;
  ProgressReport report1, report2, report3, report4, report5;
  if (this->flagMakingProgressReport)
  { std::stringstream out1, out2, out3, out4, out5;
    if (this->flagFirstRoundCounting)
    { out1 << "Counting ss part " << this->theDynkinDiagram.ToString();
      out2 << "# nilradicals for fixed ss part: " << this->NumTotalSubalgebras;
      owner.NumSubalgebrasCounted ++;
      out3 << owner.NumSubalgebrasCounted << " total subalgebras counted";
    } else
    { out1 << "Computing ss part " << this->theDynkinDiagram.ToString();
      out2 << this->NumNilradicalsAllowed << " Nilradicals processed out of " << this->NumTotalSubalgebras;
      owner.NumSubalgebrasProcessed ++;
      out3 << "Total # subalgebras processed: " << owner.NumSubalgebrasProcessed;
      out4 << "Num cone condition failures: " << owner.NumConeConditionFailures;
      out5 << "Num failures to find l-prohibiting relations: " << owner.theBadRelations.size;
      report4.Report(out4.str());
      report5.Report(out5.str());
    }
    report1.Report(out1.str());
    report2.Report(out2.str());
    report3.Report(out3.str());
  }
}

void rootSubalgebra::GenerateKmodMultTable(List<List<List<int> > >& output, List<int>& oppositeKmods)
{ output.SetSize(this->Modules.size);
  oppositeKmods.SetSize(this->Modules.size);
  int numTotal = this->Modules.size* this->Modules.size;
  std::stringstream out;
  out << "Computing pairing table for the module decomposition of the root subalgebra of type "
  << this->theDynkinDiagram.ToString()
  << "\n<br>\nwith centralizer " << this->theCentralizerDiagram.ToString();
  ProgressReport theReport;
  theReport.Report(out.str());
  ProgressReport theReport2;
  for (int i = 0; i < this->Modules.size; i ++)
  { output[i].SetSize(this->Modules.size);
    for (int j = 0; j< this->Modules.size; j ++)
    { this->KmodTimesKmod(i, j, oppositeKmods, output[i][j]);
      if (theGlobalVariables.flagReportEverything)
      { std::stringstream out5;
        out5 << "Computing pairing table: " << i*this->Modules.size +j+ 1 << " out of " << numTotal;
        theReport2.Report(out5.str());
      }
    }
  }
}

bool rootSubalgebra::IsARoot(const Vector<Rational>& input)
{ if (input.size!= this->GetOwnerSSalg().GetRank())
    return false;
  return this->GetAmbientWeyl().RootSystem.Contains(input);
}

bool rootSubalgebra::IsARootOrZero(const Vector<Rational>& input)
{ return input.IsEqualToZero() || this->IsARoot(input);
}

void rootSubalgebra::KmodTimesKmod(int index1, int index2, List<int>& oppositeKmods, List<int>& output)
{ MacroRegisterFunctionWithName("rootSubalgebra::KmodTimesKmod");
  ElementSemisimpleLieAlgebra<Rational> theLieBracket;
  output.size= 0;
  for (int i = 0; i < this->Modules[index1].size; i ++)
    for (int j = 0; j< this->Modules[index2].size; j ++)
    { Vector<Rational>& leftWeight = this->WeightsModulesPrimalSimple[index1][i];
      Vector<Rational>& rightWeight = this->WeightsModulesPrimalSimple[index2][j];
      this->GetOwnerSSalg().LieBracket(this->Modules[index1][i],this->Modules[index2][j], theLieBracket);
      if (theLieBracket.IsEqualToZero())
        continue;
      if (theLieBracket.IsElementCartan())
      { oppositeKmods[index1]=index2;
        oppositeKmods[index2]=index1;
        continue;
      }
      Vector<Rational> weightSum=leftWeight+rightWeight;
      for (int k= 0; k< this->WeightsModulesPrimalSimple.size; k++)
        if (this->WeightsModulesPrimalSimple[k].GetIndex(weightSum)!= - 1)
        { output.AddOnTopNoRepetition(k);
          break;
        }
    }
}

Vector<Rational> rootSubalgebra::GetFundamentalCoordsOverKss(const Vector<Rational>& inputGweightSimpleCoords) const
{ MacroRegisterFunctionWithName("rootSubalgebra::GetFundamentalCoordsOverK");
  Vector<Rational> output;
  output.MakeZero(this->SimpleBasisK.size);
  for (int i = 0; i < this->SimpleBasisK.size; i ++)
    output[i]= this->GetAmbientWeyl().RootScalarCartanRoot(inputGweightSimpleCoords, this->SimpleBasisK[i])*2/
    this->GetAmbientWeyl().RootScalarCartanRoot(this->SimpleBasisK[i], this->SimpleBasisK[i]);
  return output;
}

Vector<Rational> rootSubalgebra::GetSimpleCoordsOverKss(const Vector<Rational>& inputGweightSimpleCoords) const
{ MacroRegisterFunctionWithName("rootSubalgebra::GetSimpleCoordsOverKss");
  Vector<Rational> result;
  result.MakeZero(this->SimpleBasisK.size);
  if (this->SimpleBasisK.size== 0)
    return result;
  for (int i = 0; i < this->SimpleBasisK.size; i ++)
    result[i]= this->GetAmbientWeyl().RootScalarCartanRoot(inputGweightSimpleCoords, this->SimpleBasisK[i]);
  this->scalarProdInvertedMatrixOrdered.ActOnVectorColumn(result);
  return result;
}

void rootSubalgebra::ComputeHighestVectorsHighestWeights()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeHighestVectorsHighestWeights");
  this->HighestVectors.SetExpectedSize(this->GetOwnerSSalg().GetNumGenerators());
  this->HighestWeightsPrimalSimple.SetExpectedSize(this->GetOwnerSSalg().GetNumGenerators());
  this->HighestWeightsNONPrimalFundamental.SetExpectedSize(this->GetOwnerSSalg().GetNumGenerators());
  this->HighestWeightsPrimalSimple.SetSize(0);
  this->HighestVectors.SetSize(0);
  this->HighestWeightsNONPrimalFundamental.SetSize(0);
  ElementSemisimpleLieAlgebra<Rational> currentElt;
  List<Vector<Rational> >& ambientRootSystem= this->GetAmbientWeyl().RootSystem;
  for (int i = 0; i <ambientRootSystem.size; i ++)
    if (this->IsBKhighest(ambientRootSystem[i]))
    { currentElt.MakeGGenerator(ambientRootSystem[i], this->GetOwnerSSalg());
      this->HighestVectors.AddOnTop(currentElt);
      this->HighestWeightsPrimalSimple.AddOnTop(ambientRootSystem[i]);
      this->HighestWeightsNONPrimalFundamental.AddOnTop(this->GetFundamentalCoordsOverKss(ambientRootSystem[i]));
//      stOutput << "Accounted highest weight non-primal: " << this->HighestWeightsNONPrimalFundamental.LastObject()->ToString();
    }
  Vectors<Rational> cartanCentralizer;
  this->SimpleBasisK.GetOrthogonalComplement(cartanCentralizer, &this->GetAmbientWeyl().CartanSymmetric);
  Vector<Rational> zeroRoot;
  zeroRoot.MakeZero(this->SimpleBasisK.size);
  for (int i = 0; i <cartanCentralizer.size; i ++)
  { currentElt.MakeHgenerator(cartanCentralizer[i], this->GetOwnerSSalg());
    this->HighestVectors.AddOnTop(currentElt);
    this->HighestWeightsPrimalSimple.AddOnTop(currentElt.GetRootIMustBeWeight());
    this->HighestWeightsNONPrimalFundamental.AddOnTop(zeroRoot);
  }
}

bool rootSubalgebra::CompareLeftGreaterThanRight(const Vector<Rational>& weightLeft, const Vector<Rational>& weightRight)
{ Vector<Rational> KssPartLeft = this->GetSimpleCoordsOverKss(weightLeft);
  Vector<Rational> KssPartRight = this->GetSimpleCoordsOverKss(weightRight);
  if (KssPartLeft>KssPartRight)
    return true;
  if (KssPartLeft<KssPartRight)
    return false;
  return weightLeft>weightRight;
}

void rootSubalgebra::ComputeModuleFromHighestVector(int moduleIndex)
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeModuleFromHighestVector");
  HashedList<Vector<Rational> > currentWeights;
  Vectors<Rational> zeroSpace;
  Vector<Rational> currentWeight;
  currentWeights.SetExpectedSize(this->GetAmbientWeyl().RootSystem.size);
  currentWeights.AddOnTop(this->HighestWeightsPrimalSimple[moduleIndex]);
  if (this->HighestWeightsPrimalSimple[moduleIndex].IsEqualToZero())
    zeroSpace.AddOnTop(this->HighestVectors[moduleIndex].GetCartanPart());
  else
  { for (int j = 0; j<currentWeights.size; j ++)
      for (int k= 0; k< this->SimpleBasisK.size; k++)
      { currentWeight = currentWeights[j]-this->SimpleBasisK[k];
        if (this->IsARoot(currentWeight))
          currentWeights.AddOnTopNoRepetition(currentWeight);
        if (currentWeight.IsEqualToZero())
          if (!zeroSpace.LinSpanContainsVector(currentWeights[j]))
          { zeroSpace.AddOnTop(currentWeights[j]);
            currentWeights.AddOnTop(-this->SimpleBasisK[k]);
          }
      }
    currentWeight.MakeZero(this->GetOwnerSSalg().GetRank());
    for (int i = 0; i <zeroSpace.size; i ++)
      currentWeights.AddOnTop(currentWeight);
  }
  Vectors<Rational>& wPrimalSimple= this->WeightsModulesPrimalSimple[moduleIndex];
  Vectors<Rational>& wNONprimalFundamental = this->WeightsModulesNONPrimalFundamental[moduleIndex];
  Vectors<Rational>& wNONprimalSimple= this->WeightsModulesNONPrimalSimple[moduleIndex];
  wPrimalSimple=currentWeights;
  wPrimalSimple.QuickSortDescendingCustom(*this);
  wNONprimalFundamental.SetSize(wPrimalSimple.size);
  wNONprimalSimple.SetSize(wPrimalSimple.size);
  for (int j = 0; j<wPrimalSimple.size; j ++)
  { wNONprimalFundamental[j] = this->GetFundamentalCoordsOverKss(wPrimalSimple[j]);
    wNONprimalSimple[j]= this->GetSimpleCoordsOverKss(wPrimalSimple[j]);
    if (this->IsBKlowest(wPrimalSimple[j]) || wPrimalSimple.size==1)
    { this->LowestWeightsPrimalSimple[moduleIndex]=wPrimalSimple[j];
      if (j!=wPrimalSimple.size- 1)
        crash << "Last module weight is not lowest. The simple basis is: " << this->SimpleBasisK.ToString() << ". The lowest weight is "
        << this->LowestWeightsPrimalSimple[moduleIndex].ToString() << " and the weights of the module are: "
        << wPrimalSimple.ToString() << ". I think this shouldn't happen, should it?" << crash;
    }
  }
  List<ElementSemisimpleLieAlgebra<Rational> >& theMod= this->Modules[moduleIndex];
  theMod.SetSize(wPrimalSimple.size);
  int indexInZeroSpace= 0;
  for (int i = 0; i <wPrimalSimple.size; i ++)
  { ElementSemisimpleLieAlgebra<Rational>& currentElt = theMod[i];
    if (!wPrimalSimple[i].IsEqualToZero())
    { currentElt.MakeGGenerator(wPrimalSimple[i], this->GetOwnerSSalg());
      continue;
    }
    currentElt.MakeHgenerator(zeroSpace[indexInZeroSpace], this->GetOwnerSSalg());
    indexInZeroSpace ++;
  }
}

void rootSubalgebra::ComputeModulesFromHighestVectors()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeModulesFromHighestVectors");
  this->CheckScalarProdMatrixOrdered();
  this->WeightsModulesPrimalSimple.SetSize(this->GetNumModules());
  this->WeightsModulesNONPrimalSimple.SetSize(this->GetNumModules());
  this->WeightsModulesNONPrimalFundamental.SetSize(this->GetNumModules());
  this->LowestWeightsPrimalSimple.SetSize(this->GetNumModules());
  this->Modules.SetSize(this->GetNumModules());
  for (int i = 0; i < this->GetNumModules(); i ++)
    this->ComputeModuleFromHighestVector(i);
}

void rootSubalgebra::ComputeModuleDecompo()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeModuleDecompo");
  this->ModuleDecompoHighestWeights.MakeZero();
  Weight<Rational> theM;
  for (int i = 0; i < this->Modules.size; i ++)
  { theM.weightFundamentalCoordS= this->HighestWeightsNONPrimalFundamental[i];
//    stOutput << "<br>weightFundamentalCoords equals: " << theM.ToString()
//    << " and this->HighestWeightsNONPrimalFundamental[i] equals: "
//    << this->HighestWeightsNONPrimalFundamental[i].ToString();

    this->ModuleDecompoHighestWeights.AddMonomial(theM, 1);
  }
}

void rootSubalgebra::ComputeKModules()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeKModules");
  this->ComputeRootsOfK();
  this->scalarProdInvertedMatrixOrdered= this->scalarProdMatrixOrdered;
  if (this->scalarProdInvertedMatrixOrdered.NumRows>0)
    this->scalarProdInvertedMatrixOrdered.Invert();
  this->ComputeHighestVectorsHighestWeights();
  this->ComputeModulesFromHighestVectors();
  this->ComputeModuleDecompo();
  this->ComputeModuleDecompoAmbientAlgebraDimensionsOnly();
  //Handle the elements of the Cartan.
  int dimFinal = 0;
  for (int i = 0; i < this->Modules.size; i ++)
    dimFinal+= this->Modules[i].size;
  if (dimFinal!= this->GetOwnerSSalg().GetNumGenerators())
    crash << "Sum of k-module dimensions does not equal the dimension of the ambient Lie algebra. " << crash;
}

int rootSubalgebra::NumRootsInNilradical()
{ int result = 0;
  for (int i = 0; i < this->NilradicalKmods.CardinalitySelection; i ++)
    result+= this->Modules[this->NilradicalKmods.elements[i]].size;
  return result;
}

int rootSubalgebra::GetIndexKmoduleContainingRoot(const Vector<Rational>& input)
{ for (int i = 0; i < this->WeightsModulesPrimalSimple.size; i ++)
    if (this->WeightsModulesPrimalSimple[i].Contains(input))
      return i;
  return - 1;
}

bool rootSubalgebra::ConeConditionHolds
(rootSubalgebras& owner, int indexInOwner, Vectors<Rational>& NilradicalRoots, Vectors<Rational>& Ksingular, bool doExtractRelations)
{ Matrix<Rational> tempA, tempB;
  if (Vectors<Rational>::ConesIntersect(NilradicalRoots, Ksingular, 0, 0))
  { if (doExtractRelations)
      this->ExtractRelations
      (tempA, tempB, NilradicalRoots, owner, indexInOwner, Ksingular);
    return false;
  }
  return true;
}

bool rootSubalgebra::ConeConditionHolds(rootSubalgebras& owner, int indexInOwner, bool doExtractRelations)
{ Vectors<Rational> NilradicalRoots;
  Vectors<Rational> Ksingular;
  if (this->Modules.size== 0)
    return true;
  NilradicalRoots.size= 0;
  int counter = 0;
  for (int i = 0; i < this->NilradicalKmods.CardinalitySelection; i ++)
  { Vectors<Rational>& tempKmod= this->WeightsModulesPrimalSimple[this->NilradicalKmods.elements[i]];
    for (int j = 0; j<tempKmod.size; j ++)
    { NilradicalRoots.AddOnTop(tempKmod[j]);
      counter++;
    }
  }
  Ksingular.size= 0;
  for (int i = 0; i < this->Modules.size; i ++)
    if (!this->NilradicalKmods.selected[i])
      Ksingular.AddOnTop(this->HighestWeightsPrimalSimple[i]);
  if (!this->ConeConditionHolds(owner, indexInOwner, NilradicalRoots, Ksingular, doExtractRelations))
    return false;
  else
    return true;
}

bool rootSubalgebra::CheckRankInequality() const
{ if ((this->theDynkinType.GetRank()+this->theCentralizerDynkinType.GetRank())*2< this->ownEr->owner->GetRank())
    crash << "2*(Centralizer rank + rank) < ambient rank, which is mathematically impossible. There was a programming error. "
    << crash;
  return true;
}

bool rootSubalgebra::CheckForSmallRelations(coneRelation& theRel, Vectors<Rational>& nilradicalRoots)
{ MacroRegisterFunctionWithName("rootSubalgebra::CheckForSmallRelations");
  Vector<Rational> weightSum;
  bool tempBool; int tempI;
  for (int i = 0; i < this->Modules.size; i ++)
    if (!this->NilradicalKmods.selected[i])
      for (int j =i+ 1; j< this->Modules.size; j ++)
        if (!this->NilradicalKmods.selected[j])
        { weightSum=(this->HighestWeightsPrimalSimple[i]);
          weightSum+=(this->HighestWeightsPrimalSimple[j]);
          if (!weightSum.IsEqualToZero())
          { theRel.BetaCoeffs.SetSize(0);
            theRel.Betas.SetSize(0);
            tempI= nilradicalRoots.GetIndex(weightSum);
            if (tempI!= - 1)
            { tempBool = true;
              theRel.BetaCoeffs.SetSize(1);
              theRel.Betas.SetSize(1);
              theRel.BetaCoeffs[0].MakeOne();
              theRel.Betas[0]=(nilradicalRoots[tempI]);
            } else
              tempBool =
              this->GetAmbientWeyl().HasStronglyPerpendicularDecompositionWRT
              (weightSum, - 1, nilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true)
              ;
            if (tempBool)
            { theRel.Alphas.size= 0;
              theRel.AlphaCoeffs.size= 0;
              theRel.Alphas.AddOnTop(this->HighestWeightsPrimalSimple[i]);
              theRel.Alphas.AddOnTop(this->HighestWeightsPrimalSimple[j]);
              theRel.AlphaCoeffs.AddOnTop(1);
              theRel.AlphaCoeffs.AddOnTop(1);
//              theRel.ComputeDebugString(*this, true);
              return true;
            }
          }
        }
  return false;
}

void rootSubalgebra::MatrixToRelation( coneRelation& output, Matrix<Rational> & matA, Matrix<Rational> & matX, int theDimension, Vectors<Rational>& NilradicalRoots)
{ output.AlphaCoeffs.size= 0;  output.Alphas.size= 0;
  output.BetaCoeffs.size= 0;  output.Betas.size= 0;
  Vector<Rational> tempRoot; tempRoot.SetSize(theDimension);
  matX.ScaleToIntegralForMinRationalHeightNoSignChange();
  if (matA.NumCols!=matX.NumRows)
    crash << crash;
  for (int i = 0; i <matA.NumCols; i ++)
    if (!matX.elements[i][0].IsEqualToZero())
    { for (int j = 0; j<theDimension; j ++)
        tempRoot.TheObjects[j].Assign(matA.elements[j][i]);
      if (!(matX.elements[i][0].DenShort ==1))
        crash << crash;
      if (i <NilradicalRoots.size)
      { output.Betas.AddOnTop(tempRoot);
        output.BetaCoeffs.AddOnTop(matX.elements[i][0]);
      } else
      { tempRoot.Minus();
        output.Alphas.AddOnTop(tempRoot);
        output.AlphaCoeffs.AddOnTop(matX.elements[i][0]);
      }
    }
}

void rootSubalgebra::ExtractRelations
(Matrix<Rational>& matA, Matrix<Rational>& matX, Vectors<Rational>& NilradicalRoots, rootSubalgebras& owner,
 int indexInOwner, Vectors<Rational>& Ksingular)
{ int theDimension = this->GetOwnerSSalg().GetRank();
  Vector<Rational> tempRoot; tempRoot.MakeZero(theDimension);
  coneRelation theRel; theRel.IndexOwnerRootSubalgebra =indexInOwner;
  if (owner.flagLookingForMinimalRels)
  { theRel.FixRightHandSide(*this, NilradicalRoots);
    theRel.MakeLookCivilized(*this);
    owner.theMinRels.AddRelationNoRepetition(theRel, owner);
  }
  bool tempBool = this->AttemptTheTripleTrick(theRel, NilradicalRoots);
  if (tempBool)
  { this->NumRelationsWithStronglyPerpendicularDecomposition ++;
    theRel.MakeLookCivilized(*this);
    owner.theGoodRelations.AddRelationNoRepetition(theRel, owner);
  }
  else
  { //if (!this->CheckForSmallRelations(theRel, NilradicalRoots))
    this->MatrixToRelation(theRel, matA, matX, theDimension, NilradicalRoots);
    this->MakeGeneratingSingularVectors(theRel, NilradicalRoots);
    theRel.FixRightHandSide(*this, NilradicalRoots);
    theRel.MakeLookCivilized(*this);
    theRel.ComputeDebugString(owner, true, true);
    if (false)
      if (theRel.theDiagram.ToString()=="C^{2}_3")
      { Selection tempSel;
        tempSel.init(Ksingular.size);
        int tempNum=MathRoutines::NChooseK(Ksingular.size, 2);
        for (int i = 0; i <tempNum; i ++)
        { tempSel.incrementSelectionFixedCardinality(2);
          theRel.Alphas.SetSize(2);
          theRel.AlphaCoeffs.SetSize(2);
          theRel.Betas.size= 0;
          theRel.BetaCoeffs.size= 0;
          for (int j = 0; j<tempSel.CardinalitySelection; j ++)
            theRel.Alphas[j]=(Ksingular[tempSel.elements[j]]);
          if (theRel.IsStrictlyWeaklyProhibiting(*this, NilradicalRoots, owner, indexInOwner))
            break;
        }
      if (!theRel.CheckForBugs(*this, NilradicalRoots))
        crash << crash;
    }
    owner.theBadRelations.AddOnTop(theRel);
  }
}

bool rootSubalgebra::AttemptTheTripleTrick(coneRelation& theRel, Vectors<Rational>& NilradicalRoots)
{ Vectors<Rational> tempRoots;
  tempRoots.size= 0;
  for (int i = 0; i < this->Modules.size; i ++)
    if (!this->NilradicalKmods.selected[i])
      if (this->IsGeneratingSingularVectors(i, NilradicalRoots))
        tempRoots.AddOnTop(this->HighestWeightsPrimalSimple[i]);
  //tempRoots.ComputeDebugString();
  return this->AttemptTheTripleTrickWRTSubalgebra(theRel, tempRoots, NilradicalRoots);
}

bool rootSubalgebra::AttemptTheTripleTrickWRTSubalgebra(coneRelation& theRel, Vectors<Rational>& highestWeightsAllowed, Vectors<Rational>& NilradicalRoots)
{ Vector<Rational> tempRoot, Accum;
  SelectionWithMaxMultiplicity tempSel;
  Vectors<Rational> chosenAlphas;
  int theRank= this->GetOwnerSSalg().GetRank();
  DynkinDiagramRootSubalgebra theDiagram;
  for (int i =2; i <=MathRoutines::Maximum(highestWeightsAllowed.size, theRank); i ++)
  { tempSel.initMaxMultiplicity(highestWeightsAllowed.size, i);
    int NumElts = tempSel.NumCombinationsOfCardinality(i);
    for (int j = 0; j<NumElts; j ++)
    { tempSel.IncrementSubsetFixedCardinality(i);
      Accum.MakeZero(this->GetAmbientWeyl().CartanSymmetric.NumRows);
      chosenAlphas.size= 0;
      for (int k= 0; k<tempSel.elements.size; k++)
      { tempRoot =(highestWeightsAllowed[tempSel.elements[k]]);
        chosenAlphas.AddOnTop(tempRoot);
        tempRoot*=(tempSel.Multiplicities[tempSel.elements[k]]);
        Accum+=(tempRoot);
      }
      theRel.Betas.size= 0; theRel.BetaCoeffs.size= 0;
      if (!Accum.IsEqualToZero())
        if (this->GetAmbientWeyl().HasStronglyPerpendicularDecompositionWRT(Accum, - 1, NilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true))
        { int startNumBetas = theRel.Betas.size;
          //int numAlphas = tempSel.CardinalitySelectionWithoutMultiplicities();
          //int numParticipatingRoots =numAlphas+startNumBetas;
          chosenAlphas.AddListOnTop(theRel.Betas);
          //chosenAlphas.ComputeDebugString();
          //theRel.Betas.ComputeDebugString();
          theDiagram.ComputeDiagramTypeModifyInput(chosenAlphas, this->GetAmbientWeyl());
          int theRank= theDiagram.RankTotal();
          if (theRank>4 || theDiagram.ToString()=="B^{2}_4" ||
              theDiagram.ToString()=="C^{2}_4")
          { int goalNumBetas = 2;
            theRel.Betas.size= 0; theRel.BetaCoeffs.size= 0;
            for (int l =goalNumBetas- 1; l<startNumBetas; l ++)
              if (this->GetAmbientWeyl().HasStronglyPerpendicularDecompositionWRT
                  (Accum, l+ 1, NilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true))
                break;
          }
          theRel.Alphas.SetSize(tempSel.CardinalitySelectionWithoutMultiplicities());
          theRel.AlphaCoeffs.SetSize(tempSel.elements.size);
          for (int k= 0; k<tempSel.elements.size; k++)
          { theRel.Alphas[k]=(highestWeightsAllowed[tempSel.elements[k]]);
            theRel.AlphaCoeffs[k]=(tempSel.Multiplicities[tempSel.elements[k]]);
          }
          //theRel.Alphas.ComputeDebugString();
          //theRel.Betas.ComputeDebugString();
          return true;
        }
    }
  }
  return false;
}

void rootSubalgebra::MakeSureAlphasDontSumToRoot(coneRelation& theRel, Vectors<Rational>& NilradicalRoots)
{ Vector<Rational> alpha1, alpha2, beta1, tempRoot;
  bool madeChange= true;
  while (madeChange)
  { madeChange= false;
    for (int i = 0; i <theRel.Alphas.size; i ++)
      for (int j =i+ 1; j<theRel.Alphas.size; j ++)
      { //theRel.ComputeDebugString(*this);
        beta1=(theRel.Alphas[i]);
        beta1+=(theRel.Alphas[j]);
        if (this->IsARootOrZero(beta1))
        { this->ComputeHighestWeightInTheSameKMod(beta1, tempRoot);
          if (NilradicalRoots.Contains(beta1))
          { alpha1=(theRel.Alphas[i]);
            alpha2=(theRel.Alphas[j]);
            theRel.Alphas.SetSize(2);
            theRel.AlphaCoeffs.SetSize(2);
            theRel.Alphas[0]=(alpha1);
            theRel.Alphas[1]=(alpha2);
            theRel.AlphaCoeffs[0]=(1);
            theRel.AlphaCoeffs[1]=(1);
            theRel.Betas.SetSize(1);
            theRel.BetaCoeffs.SetSize(1);
            theRel.BetaCoeffs[0]=1;
            theRel.Betas[0]=beta1;
            madeChange= false;
            break;
          }
          else
          { int changedIndex=i, otherIndex=j;
            Rational alpha1coeff, alpha2coeff;
            if (alpha1coeff.IsGreaterThanOrEqualTo(alpha2coeff))
            { changedIndex=j;
              otherIndex=i;
            }
            alpha1coeff=(theRel.AlphaCoeffs[changedIndex]);
            alpha2coeff=(theRel.AlphaCoeffs[otherIndex]);
            alpha2=(theRel.Alphas[otherIndex]);
            alpha2coeff.Subtract(alpha1coeff);
            madeChange= true;
            theRel.Alphas[changedIndex]=(beta1);
            theRel.AlphaCoeffs[changedIndex]=(alpha1coeff);
            if (alpha2coeff.IsEqualToZero())
            { theRel.Alphas.RemoveIndexSwapWithLast(otherIndex);
              theRel.AlphaCoeffs.RemoveIndexSwapWithLast(otherIndex);
            } else
            { theRel.Alphas[otherIndex]=(alpha2);
              theRel.AlphaCoeffs[otherIndex]=(alpha2coeff);
            }
          }
        }
      }
  }
}

void rootSubalgebra::ComputeEpsCoordsWRTk()
{ this->kModulesKepsCoords.SetSize(this->Modules.size);
  this->kModulesgEpsCoords.SetSize(this->Modules.size);
  Vectors<Rational> EpsCoordsWRTk;
  Vectors<Rational> simpleBasisG;
  int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  simpleBasisG.SetSize(theDimension);
  for (int i = 0; i <theDimension; i ++)
  { simpleBasisG[i].MakeZero(theDimension);
    simpleBasisG[i][i]=1;
  }
  Vector<Rational> tempRoot, tempRoot2, tempRoot3;
  for (int i = 0; i < this->Modules.size; i ++)
  { if (this->SimpleBasisK.size>0)
    { EpsCoordsWRTk.size= 0;
      for (int j = 0; j< this->Modules[i].size; j ++)
      { tempRoot.SetSize(this->SimpleBasisK.size);
        for (int k= 0; k< this->SimpleBasisK.size; k++)
          this->GetAmbientWeyl().RootScalarCartanRoot(this->WeightsModulesPrimalSimple[i][j], this->SimpleBasisK[k], tempRoot[k]);
        this->scalarProdInvertedMatrixOrdered.ActOnVectorColumn(tempRoot, tempRoot3);
        tempRoot2.MakeZero(this->GetAmbientWeyl().CartanSymmetric.NumRows);
        for (int j = 0; j< this->SimpleBasisK.size; j ++)
          tempRoot2+= this->SimpleBasisK[j]*tempRoot3[j];
        EpsCoordsWRTk.AddOnTop(tempRoot2);
      }
  //    tempRoots.ComputeDebugString();
      this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(this->SimpleBasisK, EpsCoordsWRTk, this->kModulesKepsCoords[i]);
      this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(simpleBasisG, this->WeightsModulesPrimalSimple[i], this->kModulesgEpsCoords[i]);
    } else
    { Vector<Rational> emptyV;
      this->kModulesgEpsCoords[i].initFillInObject(this->Modules[i].size, emptyV);
    }
    Vector<Rational> tempRoot;
    if (this->kModulesKepsCoords[i].size>0)
    { this->kModulesKepsCoords[i].average(tempRoot, this->kModulesKepsCoords[i][0].size);
      if (!tempRoot.IsEqualToZero())
        crash << crash;
    }
   // this->kModulesgEpsCoords.TheObjects[i].Average
     // (tempRoot, this->kModulesgEpsCoords.TheObjects[i].TheObjects[0].size);
    //if (!tempRoot.IsEqualToZero())crash << crash;
  //  this->kModulesEpsCoords.TheObjects[i].ComputeDebugString();
  }
  this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(this->SimpleBasisK, this->SimpleBasisK, this->SimpleBasisKEpsCoords);
  this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(simpleBasisG, this->SimpleBasisK, this->SimpleBasisgEpsCoords);
}

bool rootSubalgebra::attemptExtensionToIsomorphismNoCentralizer
(Vectors<Rational>& Domain, Vectors<Rational>& Range,
 int RecursionDepth, SubgroupWeylGroupOLD* outputAutomorphisms, bool GenerateAllpossibleExtensions,
 bool* abortKmodule, Vectors<Rational>* additionalDomain, Vectors<Rational>* additionalRange)
{ int CurrentRank=Domain.GetRankOfSpanOfElements();
  if (CurrentRank!=Range.GetRankOfSpanOfElements())
    crash << crash;
  if (abortKmodule!= 0)
    *abortKmodule= false;
  if (CurrentRank== this->GetAmbientWeyl().CartanSymmetric.NumRows)
    return this->IsAnIsomorphism(Domain, Range, outputAutomorphisms, additionalDomain, additionalRange);
  Vectors<Rational> domainRec =Domain;
  Vectors<Rational> rangeRec =Range;
  rootSubalgebra leftSA;
  rootSubalgebra rightSA;
  Rational tempRatD, tempRatR;
  Vector<Rational>& LastRootD=*domainRec.LastObject();
  Vector<Rational>& LastRootR=*rangeRec.LastObject();
  if (RecursionDepth!= 0)
    for (int i = 0; i <domainRec.size; i ++)
    { this->GetAmbientWeyl().RootScalarCartanRoot(domainRec[i], LastRootD, tempRatD);
      this->GetAmbientWeyl().RootScalarCartanRoot(rangeRec[i], LastRootR, tempRatR);
      if (!tempRatR.IsEqualTo(tempRatD))
        return false;
    }
  leftSA.ownEr = this->ownEr;
  rightSA.ownEr = this->ownEr;
  leftSA.genK=domainRec;
  rightSA.genK=rangeRec;
  leftSA.ComputeEssentialS();
  rightSA.ComputeEssentialS();
  if (RecursionDepth!= 0)
    if (leftSA.theDynkinDiagram.ToString()!=
        rightSA.theDynkinDiagram.ToString() ||
        leftSA.theCentralizerDiagram.ToString()!=
        rightSA.theCentralizerDiagram.ToString() ||
        rightSA.Modules.size!=leftSA.Modules.size)
    { if (abortKmodule!= 0)
        *abortKmodule= true;
      return false;
    }
  int counter = 0;
  domainRec.AddOnTop(leftSA.HighestWeightsPrimalSimple[counter]);
  while (domainRec.GetRankOfSpanOfElements()==CurrentRank)
  { counter++;
    if (leftSA.Modules.size<=counter)
      crash << crash;
    domainRec.RemoveIndexSwapWithLast(domainRec.size- 1);
    domainRec.AddOnTop(leftSA.HighestWeightsPrimalSimple[counter]);
  }
  //find a minimal possible new kmodule to throw in
  for (int i = 0; i <leftSA.Modules.size; i ++)
    if (leftSA.Modules[i].size> leftSA.Modules[counter].size)
    { domainRec.LastObject()->operator=(leftSA.HighestWeightsPrimalSimple[i]);
      if (domainRec.GetRankOfSpanOfElements()==CurrentRank)
        domainRec.LastObject()->operator=(leftSA.HighestWeightsPrimalSimple[counter]);
      else
        counter =i;
    }
  if (!(domainRec.GetRankOfSpanOfElements()==CurrentRank+ 1))
    crash << crash;
  Vectors<Rational>& firstKmodLeft = leftSA.WeightsModulesPrimalSimple[counter];
  bool result = false;
  bool tempBool;
  for (int i = 0; i <rightSA.Modules.size; i ++)
    if (firstKmodLeft.size==rightSA.Modules[i].size)
      for (int j = 0; j<firstKmodLeft.size; j ++)
      { rangeRec.AddOnTop(rightSA.WeightsModulesPrimalSimple[i][j]);
        if (rangeRec.GetRankOfSpanOfElements()!=(CurrentRank+ 1))
          continue;
        if (this->attemptExtensionToIsomorphismNoCentralizer(domainRec, rangeRec, RecursionDepth+ 1, outputAutomorphisms, GenerateAllpossibleExtensions, &tempBool, additionalDomain, additionalRange))
        { if (!GenerateAllpossibleExtensions)
            return true;
          else
            result = true;
        } else
          if (tempBool)
            break;
        rangeRec.RemoveLastObject();
      }
  return result;
}

bool rootSubalgebra::IsAnIsomorphism(Vectors<Rational>& domain, Vectors<Rational>& range, SubgroupWeylGroupOLD* outputAutomorphisms, Vectors<Rational>* additionalDomain, Vectors<Rational>* additionalRange)
{ Matrix<Rational> matB;
  Vectors<Rational> tempRoots;
  int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  tempRoots.SetSize(theDimension);
  matB.init(theDimension, theDimension);
  for (int i = 0; i <theDimension; i ++)
  { for (int j = 0; j<theDimension; j ++)
      matB.elements[i][j]=(domain[i][j]);
    tempRoots[i].MakeZero(theDimension);
  }
  matB.Invert();
  Rational tempRat2;
  for (int k= 0; k<theDimension; k++)
    for (int i = 0; i <theDimension; i ++)
      for (int j = 0; j<theDimension; j ++)
      { tempRat2=(range[j][k]);
        tempRat2.MultiplyBy(matB.elements[i][j]);
        tempRoots[i][k]+=(tempRat2);
      }
  //if (this->flagAnErrorHasOccuredTimeToPanic)
    //tempRoots.ComputeDebugString();
  Vector<Rational> tempRoot;
  if (additionalDomain!= 0)
    for (int i = 0; i <additionalDomain->size; i ++)
    { additionalDomain->TheObjects[i].GetCoordsInBasiS(tempRoots, tempRoot);
      if (!(tempRoot ==additionalRange->TheObjects[i]))
        return false;
    }
  for (int i = 0; i < this->GetAmbientWeyl().RootsOfBorel.size; i ++)
  { this->GetAmbientWeyl().RootsOfBorel[i].GetCoordsInBasiS(tempRoots, tempRoot);
    if (!this->IsARoot(tempRoot))
      return false;
  }
  if (outputAutomorphisms!= 0)
    outputAutomorphisms->ExternalAutomorphisms.AddOnTop(tempRoots);
  return true;
}

void rootSubalgebra::ToHTML(int index, FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("rootSubalgebra::ToHTML");
  this->CheckInitialization();
  std::fstream output;
  std::stringstream myPath;
  myPath << this->ownEr->owner->VirtualNameSSAlgOutputFolder;
  myPath << "rootSubalgebra_" << index + 1 << ".html";
  FileOperations::OpenFileCreateIfNotPresentVirtual(output, myPath.str(), false, true, false);
  output << "<html><title>" << this->GetAmbientWeyl().theDynkinType.GetLieAlgebraName() << " root subalgebra of type "
  << this->theDynkinDiagram.ToString() << "</title>";
  output << "<meta name=\"keywords\" content =\"" << this->GetAmbientWeyl().theDynkinType.GetLieAlgebraName()
  << " root subsystems, root subsystems, root systems";
  if (this->GetAmbientWeyl().theDynkinType.HasExceptionalComponent())
    output << ", exceptional Lie algebra";
  output << " \">";
  output << HtmlRoutines::GetJavascriptMathjax();
  output << "<body>" << this->ToString(theFormat) << "</body></html>";
  output.close();
}

std::string rootSubalgebra::ToString(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("rootSubalgebra::ToString");
  (void) theFormat;//taking care of unused parameter warning in a portable way
  std::stringstream out;
  std::string latexFooter, latexHeader;
  bool useLatex= false;
  bool useHtml = true;
  bool includeKEpsCoords = false;
  if (this->SimpleBasisgEpsCoords.size!= this->SimpleBasisK.size ||
      this->SimpleBasisKEpsCoords.size!= this->SimpleBasisK.size ||
      this->kModulesgEpsCoords.size!= this->Modules.size ||
      this->kModulesKepsCoords.size!= this->Modules.size)
    includeKEpsCoords = false;
  out << "Type: " << HtmlRoutines::GetMathSpanPure(this->theDynkinDiagram.ToString());
  out << " (Dynkin type computed to be: " << HtmlRoutines::GetMathSpanPure(this->theDynkinType.ToString()) << ")";
  out << "\n<br>\nSimple basis: " << this->SimpleBasisK.ToString();
  out << "\n<br>\nSimple basis epsilon form: " << this->SimpleBasisgEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  out << "\n<br>\nSimple basis epsilon form with respect to k: " << this->SimpleBasisKEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  out << "<br>Number of outer autos with trivial action on orthogonal complement and extending to autos of ambient algebra: "
  << this->outerSAautosExtendingToAmbientAutosGenerators.theElements.size;
  out << "<br>Number of outer autos with trivial action on orthogonal complement: "
  << this->outerSAautos.theElements.size << ". ";
  out << "<br>\nC(k_{ss})_{ss}: " << this->theCentralizerDiagram.ToString();
  out << "<br>\n simple basis centralizer: "<< this->SimpleBasisCentralizerRoots.ToString();
  out << "<hr>\n Number of k-submodules of g: " << this->Modules.size;
  out << "<br>Module decomposition, fundamental coords over k: ";
  out << HtmlRoutines::GetMathSpanPure(this->ModuleDecompoHighestWeights.ToString());
  out << "<br>\n";
  out << "\ng/k k-submodules<table border =\"1\">\n<tr><th>id</th><th>size</th>"
  << "<th>b\\cap k-lowest weight</th><th>b\\cap k-highest weight</th><th>Module basis</th><th>Weights epsilon coords</th>";
  if (includeKEpsCoords)
    out << "<th>epsilon coords wrt k</th>";
  out << "</tr>";
  this->kModulesgEpsCoords.SetSize(this->Modules.size);
  for (int i = 0; i < this->Modules.size; i ++)
  { out << "\n<tr><td>Module " << i+ 1 << "</td><td>" << this->Modules[i].size << "</td>";
    out << "<td>" << this->LowestWeightsPrimalSimple[i].ToString() << "</td>";
    out << "<td>" << this->HighestWeightsPrimalSimple[i].ToString() << "</td>";
    out << "<td>";
    for (int j = 0; j< this->Modules[i].size; j ++)
    { out << this->Modules[i][j].ToString();
      if (j!= this->Modules[i].size- 1)
        out << "<br>";
    }
    out << "</td><td>";
    this->GetAmbientWeyl().GetEpsilonCoords(this->WeightsModulesPrimalSimple[i], this->kModulesgEpsCoords[i]);
    for (int j = 0; j< this->Modules[i].size; j ++)
    { out << this->kModulesgEpsCoords[i][j].ToStringEpsilonFormat();
      if (j!= this->Modules[i].size- 1)
        out << "<br>";
    }
    out << "</td>";
    if (includeKEpsCoords)
    { out << "<td>";
      out << "N/A";
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  if (this->theMultTable.size!= 0)
  { if (useHtml)
      out << "\n\n Pairing table:\n\n";
    if (useLatex)
      out << "\n\n\\noindent Pairing table:\n\n\\noindent";
    out << this->ToStringMultTable(useLatex, useHtml, *this) << "\n";
  }
  out << "<hr>Information about the subalgebra generation algorithm.";
  out << "<br>Heirs rejected due to having symmetric Cartan type outside of list dictated by parabolic heirs: " << this->numHeirsRejectedBadAngleS
  << "<br>Heirs rejected due to not being maximally dominant: " << this->numHeirsRejectedNotMaximallyDominant
  << "<br>Heirs rejected due to not being maximal with respect to small Dynkin diagram automorphism that extends to "
  << " ambient automorphism: " << this->numHeirsRejectedNotMaximallyDominant
  << "<br>Heirs rejected due to having ambient Lie algebra decomposition iso to an already found subalgebra: " << this->numHeirsRejectedSameModuleDecompo;
  if (this->indexInducingSubalgebra == - 1)
    out << "<br>This subalgebra is not parabolically induced by anyone";
  else
    out << "<br>Parabolically induced by " << this->ownEr->ToStringAlgebraLink(this->indexInducingSubalgebra);
  out << "<br>Potential Dynkin type extensions: ";
  for (int i = 0; i < this->potentialExtensionDynkinTypes.size; i ++)
    out << this->potentialExtensionDynkinTypes[i].ToString() << ", ";
  return out.str();
}

bool rootSubalgebra::IsGeneratingSingularVectors(int indexKmod, Vectors<Rational>& NilradicalRoots)
{ Vector<Rational>& currentRoot = this->HighestWeightsPrimalSimple[indexKmod];
  if (currentRoot.IsEqualToZero())
    return false;
  Vector<Rational> theSum;
  for (int i = 0; i <NilradicalRoots.size; i ++)
  { theSum=currentRoot+NilradicalRoots[i];
    if (this->IsARootOrZero(theSum))
      if (!NilradicalRoots.Contains(theSum))
        return false;
  }
  return true;
}

void rootSubalgebra::MakeGeneratingSingularVectors(coneRelation& theRelation, Vectors<Rational>& nilradicalRoots)
{ bool isMaximal = false;
  Vector<Rational> beta, tempRoot;
  //theRelation.ComputeDebugString(*this);
  while (!isMaximal)
  { isMaximal = true;
    for (int i = 0; i <theRelation.AlphaCoeffs.size; i ++)
    { for (int j = 0; j<nilradicalRoots.size; j ++)
      { tempRoot =(theRelation.Alphas[i]);
        //theRelation.Alphas[i].ComputeDebugString();
        //nilradicalRoots[j].ComputeDebugString();
        tempRoot+=(nilradicalRoots[j]);
        //tempRoot.ComputeDebugString();
        //theRelation.ComputeDebugString(*this);
        if ((this->IsARoot(tempRoot) || tempRoot.IsEqualToZero()) &&(!nilradicalRoots.Contains(tempRoot)))
        { this->ComputeHighestWeightInTheSameKMod(tempRoot, tempRoot);
          //tempRoot.ComputeDebugString();
          tempRoot-=(theRelation.Alphas[i]);
          //tempRoot.ComputeDebugString();
          theRelation.Alphas[i]+=(tempRoot);
          int tempI= theRelation.Betas.GetIndex(tempRoot);
          if (tempI== - 1)
          { theRelation.Betas.AddOnTop(tempRoot);
            theRelation.BetaCoeffs.AddOnTop(theRelation.AlphaCoeffs[i]);
          }
          else
            theRelation.BetaCoeffs[tempI]+=(theRelation.AlphaCoeffs[i]);
          isMaximal = false;
          break;
        }
        if (theRelation.Alphas[i].IsEqualToZero())
        { theRelation.Alphas.RemoveIndexSwapWithLast(i);
          theRelation.AlphaCoeffs.RemoveIndexSwapWithLast(i);
          i--;
          isMaximal = false;
          break;
        }
      }
    }
  }
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  std::stringstream out2;
  std::stringstream out;
  //this->ComputeDebugString();
  out2 << this->ToString() << "\n";
  Matrix<Rational>  tempMat;
  this->SimpleBasisK.GetMatrixRootsToRows(tempMat);
  tempMat.Invert();
  int counter = 0;
  HashedList<Vector<Rational> >& AllRoots = this->GetAmbientWeyl().RootSystem;
  for (int i = 0; i <AllRoots.size; i ++)
  { Vector<Rational> linComb;
    if (this->AllRootsK.GetIndex(AllRoots[i])== - 1)
    { for (int j = 0; j<theDimension; j ++)
      { linComb[j].MakeZero();
        for (int k= 0; k<theDimension; k++)
        { Rational tempRat;
          tempRat.Assign(tempMat.elements[k][j]);
          tempRat.MultiplyBy(AllRoots[i][k]);
          linComb[j]+=(tempRat);
        }
      }
      int x= linComb.FindLCMDenominatorsTruncateToInt();
      linComb*=(-x);
      std::string tempS;
      if (this->LinCombToString(AllRoots[i], x, linComb, tempS))
      { out << tempS << "\n";
        counter++;
        if (this->LowestWeightsPrimalSimple.GetIndex(AllRoots[i]) != - 1)
          out2 << tempS << "\n";
      }
    }
  }
  out << "\\multicolumn{2}{|c|}{Number of relations: " << counter << " }\\\\\\hline";
  std::string tempS= out.str();
  out2 << "\n\n" << tempS;
  if (DoEnumeration)
  { this->TestedRootsAlpha = this->LowestWeightsPrimalSimple;
    this->DoKRootsEnumeration();
  }
//  this->GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2()
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  std::stringstream out;
  out << this->ToString() << "\n\n";
  Vector<Rational> tempRoot;
  tempRoot =(this->SimpleBasisK[0]);
  this->ComputeHighestWeightInTheSameKMod(tempRoot, tempRoot);
  HashedList<Vector<Rational> >& AllRoots = this->GetAmbientWeyl().RootSystem;
  for (int l = 0; l< this->SimpleBasisK.size; l ++)
  { Rational tempRat;
    this->GetAmbientWeyl().RootScalarCartanRoot(tempRoot, this->SimpleBasisK[l], tempRat);
    if (!tempRat.IsEqualToZero())
    { int counter = 0;
      Vectors<Rational> tempRoots;
      tempRoots =(this->SimpleBasisK);
      tempRoots[l]=(tempRoot);
      Matrix<Rational> tempMat;
      tempRoots.GetMatrixRootsToRows(tempMat);
      tempMat.Invert();
      for (int i = 0; i <AllRoots.size; i ++)
      { Vector<Rational> linComb;
        if (this->AllRootsK.GetIndex(AllRoots.TheObjects[i])== - 1)
        { for (int j = 0; j<theDimension; j ++)
          { linComb.TheObjects[j].MakeZero();
            for (int k= 0; k<theDimension; k++)
            { Rational tempRat;
              tempRat.Assign(tempMat.elements[k][j]);
              tempRat.MultiplyBy(AllRoots[i][k]);
              linComb.TheObjects[j]+=(tempRat);
            }
          }
          int x= linComb.FindLCMDenominatorsTruncateToInt();
          linComb*=(-x);
          std::string tempS;
          if (this->LinCombToStringDistinguishedIndex(l, AllRoots.TheObjects[i], x, linComb, tempS))
          { out << tempS << "\n";
            counter++;
          }
        }
      }
      out << "\\multicolumn{2}{|c|}{Number of relations: " << counter << " }\\\\\\hline";
    }
  }
}

bool rootSubalgebra::LinCombToString(const Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  if (coeff==1)
    return false;
  std::stringstream out;
  std::string tempS=alphaRoot.ToString();
  out << "(" << tempS << ")&$";
  out << coeff << "\\alpha_" << theDimension + 1;
  for (int i = 0; i <theDimension; i ++)
  {  //if (linComb.coordinates[i].IsEqualToZero())
    //  return false;
    tempS=linComb[i].ToString();
    if (tempS!="0")
    { if (tempS=="- 1")
        tempS="-";
      if (tempS=="1")
        tempS="+";
      if (!(tempS[0]=='+')&& !(tempS[0]=='-'))
        tempS.insert(0, "+");
      out << tempS << "\\beta_" << i+ 1;
    }
  }
  out << "= 0$\\\\";
  output = out.str();
  return true;
}

rootSubalgebra::rootSubalgebra()
{ this->flagDeallocated= false;
  this->ownEr = 0;
  this->NumGmodKtableRowsAllowedLatex=35;
  this->flagMakingProgressReport = true;
  this->flagComputeConeCondition = true;
  this->initForNilradicalGeneration();
  this->initNoOwnerReset();
}

void rootSubalgebra::initNoOwnerReset()
{ this->indexInducingSubalgebra = - 1;
  this->numHeirsRejectedNotMaximallyDominant = 0;
  this->numHeirsRejectedSameModuleDecompo= 0;
  this->numHeirsRejectedBadAngleS= 0;
  this->numHeirsRejectedNotMaxWRTouterAuto= 0;
}

void rootSubalgebra::initForNilradicalGeneration()
{ this->NumNilradicalsAllowed= 0;
  this->NumConeConditionFailures = 0;
  this->NumRelationsWithStronglyPerpendicularDecomposition = 0;
  this->flagFirstRoundCounting= true;
}

std::string rootSubalgebra::ToStringMultTable(bool useLaTeX, bool useHtml, rootSubalgebra& owner)
{ MacroRegisterFunctionWithName("rootSubalgebra::ToStringMultTable");
  std::stringstream out;
  out << "\t";
  if (!(useLaTeX||useHtml))
    for (int i = 0; i < this->theMultTable.size; i ++)
      out << i << "\t";
  else
  { if (useHtml)
      out << "<table><tr><th></th>";
    if (useLaTeX)
    { out << "\\begin{tabular}{c|";
      for (int i = 0; i < this->theMultTable.size; i ++)
        out << "c";
      out << "|} & ";
    }
    for (int i = 0; i < this->theMultTable.size; i ++)
    { if (useHtml)
        out << "<th>";
      out << i;
      if (useHtml)
        out << "</th>";
      else
      { if (i!= this->theMultTable.size- 1)
          out << "&";
      }
    }
    if (useHtml)
      out << "</tr>";
    if (useLaTeX)
      out << "\\\\\\hline";
  }
  for (int i = 0; i < this->theMultTable.size; i ++)
  { if (useHtml)
      out << "\n<tr><td>";
    out << "\n" << i;
    if (useLaTeX)
      out << " & ";
    if (useHtml)
      out << "</td>";
    for (int j = 0; j< this->theMultTable[i].size; j ++)
    { if ((j == owner.CentralizerRoots.size- 1) && (i <= owner.CentralizerRoots.size- 1))
      { if (useLaTeX)
          out << "\\multicolumn{1}{c|}{";
        if (useHtml)
          out << "<td rhs =\"1\">";
      } else
      { if (useHtml)
          out << "<td>";
      }
      for (int k= 0; k< this->theMultTable[i][j].size; k++)
        out << this->theMultTable[i][j][k] << ", ";
      if (useLaTeX )
      { if ((j == owner.CentralizerRoots.size- 1) && (i <= owner.CentralizerRoots.size- 1))
          out << "}";
        if (j!= this->theMultTable[i].size- 1)
          out << " & ";
      }
      if (useHtml)
        out << "</td>";
    }
    if (useHtml)
      out << "</tr>";
    if (useLaTeX)
    { out << "\\\\";
      if (i == owner.CentralizerRoots.size- 1)
        out << "\\cline{2-" << owner.CentralizerRoots.size + 1 << "}";
    }
  }
  if (useLaTeX)
    out << "\n\\hline opposite & ";
  if (useHtml)
    out << "<tr><td>opposite</td>";
  for (int i = 0; i <owner.theOppositeKmods.size; i ++)
  { if (useHtml)
      out << "<td>";
    out << i << "/" << owner.theOppositeKmods.TheObjects[i];
    if (useHtml)
      out << "</td>";
    if (useLaTeX)
    { if (i!= owner.theOppositeKmods.size- 1)
      { if (useLaTeX)
          out << " & ";
        else
          out << "\t";
      }
    }
  }
  if (useLaTeX)
    out << "\n\\end{tabular}";
  return out.str();
}

WeylGroupData& rootSubalgebra::GetAmbientWeyl() const
{ return this->GetOwnerSSalg().theWeyl;
}

void rootSubalgebra::WriteToFileNilradicalGeneration(std::fstream& output)
{ output << "Simple_basis_k: ";
  this->SimpleBasisK.WriteToFile(output);
}

void rootSubalgebra::ReadFromFileNilradicalGeneration(std::fstream& input, rootSubalgebras& inputOwner)
{ std::string tempS;
  input >> tempS;
  if (tempS!="Simple_basis_k:")
    crash << crash;
  this->SimpleBasisK.ReadFromFile(input);
  this->genK=(this->SimpleBasisK);
  this->ownEr = &inputOwner;
  this->ComputeEssentialS();
}

bool rootSubalgebra::LinCombToStringDistinguishedIndex(int distinguished, Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  if (coeff==1)
    return false;
  std::stringstream out;
  std::string tempS=alphaRoot.ToString();
  out << "(" << tempS << ")&$";
  out << coeff << "\\alpha_" << theDimension + 1;
  for (int i = 0; i <theDimension; i ++)
  { //if (linComb.coordinates[i].IsEqualToZero())
    //  return false;
    tempS=linComb.TheObjects[i].ToString();
    if (tempS!="0")
    { if (tempS=="- 1")
        tempS="-";
      if (tempS=="1")
        tempS="+";
      if (!(tempS[0]=='+') && !(tempS[0]=='-'))
        tempS.insert(0, "+");
      out << tempS;
      if (i!=distinguished)
        out << "\\beta_" << i+ 1;
      else
        out << "\\gamma";
    }
  }
  out << "= 0$\\\\";
  output = out.str();
  return true;
}

SemisimpleLieAlgebra& rootSubalgebra::GetOwnerSSalg() const
{ if (this->ownEr == 0)
    crash << "This is a programming error. Attempting to access ambient Lie algebra of non-initialized root subalgebras. " << crash;
  return *this->ownEr->owner;
}

bool rootSubalgebra::operator>(const rootSubalgebra& other) const //current implementation does not work as expected in types E_7 and for large D_n's
{ if (this->theDynkinType>other.theDynkinType)
    return true;
  if (other.theDynkinType>this->theDynkinType)
    return false;
  if (this->theCentralizerDynkinType>other.theCentralizerDynkinType)
    return true;
  if (other.theCentralizerDynkinType>this->theCentralizerDynkinType)
    return false;
  return false;//<-this might need to be fixed.
}

void rootSubalgebra::GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter)
{ impliedSelections.SetSize(this->Modules.size + 1);
  parabolicsCounter = 0;
}

void rootSubalgebra::GeneratePossibleNilradicals
(PauseThread& PauseMutex, List<Selection>& impliedSelections, int& parabolicsCounter,
 bool useParabolicsInNilradical, rootSubalgebras& owner, int indexInOwner)
{  //this->ComputeAll();
  this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods);
  this->NumTotalSubalgebras = 0;
  Selection emptySel;
  emptySel.init(this->SimpleBasisCentralizerRoots.size);
  owner.ComputeActionNormalizerOfCentralizerIntersectNilradical(emptySel, *this);
  int numCycles = MathRoutines::TwoToTheNth(this->SimpleBasisCentralizerRoots.size);
  List<Selection> StartingNilradicalsNoRepetition;
  StartingNilradicalsNoRepetition.Reserve(numCycles);
  Selection tempSel, ParabolicsGenerator;
  if (!owner.flagNilradicalComputationInitialized)
    owner.CountersNilradicalsGeneration.SetSize(this->Modules.size + 1);
  if (owner.flagStoringNilradicals)
    owner.storedNilradicals[indexInOwner].size= 0;
  Vectors<Rational> tempRootsTest;
  if (useParabolicsInNilradical)
  { this->flagFirstRoundCounting= false;
    ParabolicsGenerator.init(this->SimpleBasisCentralizerRoots.size);
    for (int i = 0; i <numCycles; i ++, ParabolicsGenerator.incrementSelection())
    { tempSel.init(this->Modules.size);
      for (int j = 0; j< this->CentralizerRoots.size; j ++)
        if (this->rootIsInNilradicalParabolicCentralizer(ParabolicsGenerator, this->CentralizerRoots[j]))
          tempSel.AddSelectionAppendNewIndex(j);
      if (owner.flagUsingActionsNormalizerCentralizerNilradical)
        owner.RaiseSelectionUntilApproval(tempSel);
      StartingNilradicalsNoRepetition.AddOnTopNoRepetition(tempSel);
      //StartingNilradicalsNoRepetition.AddOnTop(tempSel);
    }

/*      tempRootsTest.size= 0;
      std::string tempS; std::stringstream out;
      for (int s = 0; s<impliedSelections.TheObjects[0].CardinalitySelection; s++)
        tempRootsTest.AddOnTop(this->kModules.TheObjects[impliedSelections.TheObjects[0].elements[s]].TheObjects[0]);
      tempS= out.str();
      if (!this->RootsDefineASubalgebra(tempRootsTest)) crash << crash;*/
    for (; parabolicsCounter<StartingNilradicalsNoRepetition.size; parabolicsCounter++, owner.flagNilradicalComputationInitialized= false)
    { if (!owner.flagNilradicalComputationInitialized)
      { impliedSelections[0]=(StartingNilradicalsNoRepetition[parabolicsCounter]);
        owner.RecursionDepthNilradicalsGeneration = 0;
        owner.CountersNilradicalsGeneration[0]= this->CentralizerRoots.size;
      }
      this->GeneratePossibleNilradicalsRecursive(PauseMutex, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
    }
  } else
  { this->flagFirstRoundCounting= false;
    impliedSelections[0].init(this->Modules.size);
    owner.RecursionDepthNilradicalsGeneration = 0;
    owner.CountersNilradicalsGeneration[0]= 0;
    this->GeneratePossibleNilradicalsRecursive(PauseMutex, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner);
  }
}

bool rootSubalgebra::attemptExtensionToIsomorphism
(Vectors<Rational>& Domain, Vectors<Rational>& Range, SubgroupWeylGroupOLD* outputAutomorphisms,
 rootSubalgebras& inputOwner, bool* DomainAndRangeGenerateNonIsoSAs)
{ if (outputAutomorphisms!= 0)
    outputAutomorphisms->ExternalAutomorphisms.size= 0;
  if (DomainAndRangeGenerateNonIsoSAs!= 0)
    *DomainAndRangeGenerateNonIsoSAs = false;
  //rootSubalgebra::ProblemCounter++;
  rootSubalgebra theDomainRootSA;
  rootSubalgebra theRangeRootSA;
  theDomainRootSA.ownEr = &inputOwner;
  theRangeRootSA.ownEr = &inputOwner;
  theDomainRootSA.genK=(Domain);
  theRangeRootSA.genK=(Range);
  theDomainRootSA.ComputeEssentialS();
  theRangeRootSA.ComputeEssentialS();
  if (theDomainRootSA.theDynkinDiagram.ToString()!=
      theRangeRootSA.theDynkinDiagram.ToString() ||
      theDomainRootSA.theCentralizerDiagram.ToString()!=
      theRangeRootSA.theCentralizerDiagram.ToString())
  { if (DomainAndRangeGenerateNonIsoSAs!= 0)
      *DomainAndRangeGenerateNonIsoSAs = true;
    return false;
  }
  Vectors<Rational> isoDomain, isoRange;
  permutation permComponentsCentralizer;
  List<int> tempList, tempPermutation1, tempPermutation2;
  SelectionWithDifferentMaxMultiplicities tempAutosCentralizer;
  List<List<List<int> > > CentralizerDiagramAutomorphisms;
  theDomainRootSA.theCentralizerDiagram.GetAutomorphisms(CentralizerDiagramAutomorphisms);
  theDomainRootSA.theCentralizerDiagram.ToString();
  tempAutosCentralizer.initPart1(CentralizerDiagramAutomorphisms.size);
  for (int i = 0; i <CentralizerDiagramAutomorphisms.size; i ++)
    tempAutosCentralizer.MaxMultiplicities[i] = CentralizerDiagramAutomorphisms[i].size- 1;
  tempList.SetSize(theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size);
  int tempSize= 0;
  for (int i = 0; i <theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size; i ++)
  { tempList[i]= theDomainRootSA.theCentralizerDiagram.sameTypeComponents[i].size;
    tempSize += tempList[i];
  }
  permComponentsCentralizer.initPermutation(tempList, tempSize);
  int tempI2= permComponentsCentralizer.TotalNumSubsetsMustBeSmalInt();
  int NumAutosCentralizer = tempAutosCentralizer.TotalNumSubsetsMustBeSmalInt();
  permComponentsCentralizer.GetPermutationLthElementIsTheImageofLthIndex(tempPermutation2);
  Matrix<Rational> tempMat;
  Selection tempSel;
  for (int i = 0; i <Domain.size; i ++)
  { isoDomain.AddOnTop(Domain[i]);
    if (isoDomain.GetRankOfSpanOfElements(&tempMat, &tempSel)<isoDomain.size)
      isoDomain.RemoveLastObject();
    else
      isoRange.AddOnTop(Range[i]);
  }
  if (isoRange.GetRankOfSpanOfElements(&tempMat, &tempSel)<isoRange.size)
    return false;
  int givenSize=isoDomain.size;
  for (int j = 0; j<tempI2; j ++)
  { for (int l = 0; l<NumAutosCentralizer; l ++)
    { isoDomain.size=givenSize; isoRange.size=givenSize;
      theDomainRootSA.theCentralizerDiagram.GetMapFromPermutation(isoDomain, isoRange, tempPermutation2, CentralizerDiagramAutomorphisms, tempAutosCentralizer, theRangeRootSA.theCentralizerDiagram);
      if (theDomainRootSA.attemptExtensionToIsomorphismNoCentralizer(isoDomain, isoRange, 0, outputAutomorphisms, false, 0, &Domain, &Range))//GenerateAllAutos))
        if (outputAutomorphisms == 0)
          return true;
      if (outputAutomorphisms!= 0)
        theDomainRootSA.MakeProgressReportGenAutos(l+NumAutosCentralizer*j, tempI2*NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size);
      tempAutosCentralizer.IncrementReturnFalseIfPastLast();
    }
    permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
  }
  return false;
}

bool rootSubalgebra::GenerateIsomorphismsPreservingBorel(rootSubalgebra& right, SubgroupWeylGroupOLD* outputAutomorphisms)
{ if (this->theDynkinDiagram.ToString()!=right.theDynkinDiagram.ToString())
    return false;
  if (this->theCentralizerDiagram.ToString()!=right.theCentralizerDiagram.ToString())
    return false;
  if (outputAutomorphisms!= 0)
  { outputAutomorphisms->ExternalAutomorphisms.size= 0;
    outputAutomorphisms->simpleGenerators.size= 0;
    outputAutomorphisms->simpleGenerators =(this->SimpleBasisCentralizerRoots);
  }
  Vectors<Rational> isoDomain, isoRange;
  permutation permComponents, permComponentsCentralizer;
  List<int> tempList, tempPermutation1, tempPermutation2;
  SelectionWithDifferentMaxMultiplicities tempAutos, tempAutosCentralizer;
  List<List<List<int> > > DiagramAutomorphisms, CentralizerDiagramAutomorphisms;
  this->theDynkinDiagram.GetAutomorphisms(DiagramAutomorphisms);
  this->theCentralizerDiagram.GetAutomorphisms(CentralizerDiagramAutomorphisms);
  tempAutos.initPart1(DiagramAutomorphisms.size);
  tempAutosCentralizer.initPart1(CentralizerDiagramAutomorphisms.size);
  for (int i = 0; i <DiagramAutomorphisms.size; i ++)
    tempAutos.MaxMultiplicities[i]= DiagramAutomorphisms[i].size- 1;
  for (int i = 0; i <CentralizerDiagramAutomorphisms.size; i ++)
    tempAutosCentralizer.MaxMultiplicities[i]= CentralizerDiagramAutomorphisms[i].size- 1;
  tempList.SetSize(this->theDynkinDiagram.sameTypeComponents.size);
  int tempSize= 0;
  for (int i = 0; i < this->theDynkinDiagram.sameTypeComponents.size; i ++)
  { tempList[i]= this->theDynkinDiagram.sameTypeComponents[i].size;
    tempSize += tempList[i];
  }
  permComponents.initPermutation(tempList, tempSize);
  tempList.SetSize(this->theCentralizerDiagram.sameTypeComponents.size);
  tempSize= 0;
  for (int i = 0; i < this->theCentralizerDiagram.sameTypeComponents.size; i ++)
  { tempList[i]= this->theCentralizerDiagram.sameTypeComponents[i].size;
    tempSize += tempList[i];
  }
  permComponentsCentralizer.initPermutation(tempList, tempSize);
  int tempI1;
  int NumAutos;
  tempI1= permComponents.TotalNumSubsetsMustBeSmalInt();
  NumAutos = tempAutos.TotalNumSubsetsMustBeSmalInt();
  int tempI2= permComponentsCentralizer.TotalNumSubsetsMustBeSmalInt();
  int NumAutosCentralizer = tempAutosCentralizer.TotalNumSubsetsMustBeSmalInt();
  permComponents.GetPermutationLthElementIsTheImageofLthIndex(tempPermutation1);
  permComponentsCentralizer.GetPermutationLthElementIsTheImageofLthIndex(tempPermutation2);
  for (int i = 0; i <tempI1; i ++)
  { for (int j = 0; j<tempI2; j ++)
    { for (int k= 0; k<NumAutos; k++)
      { for (int l = 0; l<NumAutosCentralizer; l ++)
        { isoDomain.size= 0; isoRange.size= 0;
          this->theDynkinDiagram.GetMapFromPermutation(isoDomain, isoRange, tempPermutation1, DiagramAutomorphisms, tempAutos, right.theDynkinDiagram);
          this->theCentralizerDiagram.GetMapFromPermutation(isoDomain, isoRange, tempPermutation2, CentralizerDiagramAutomorphisms, tempAutosCentralizer, right.theCentralizerDiagram);
          if (this->attemptExtensionToIsomorphismNoCentralizer(isoDomain, isoRange, 0, outputAutomorphisms, false, 0, 0, 0))//GenerateAllAutos))
            if (outputAutomorphisms == 0)
              return true;
          if (outputAutomorphisms!= 0)
            this->MakeProgressReportGenAutos( l+NumAutosCentralizer*(k+NumAutos*(j+i*tempI2)), tempI1*tempI2*NumAutos*NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size);
          tempAutosCentralizer.IncrementReturnFalseIfPastLast();
        }
        tempAutos.IncrementReturnFalseIfPastLast();
      }
      permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
    }
    permComponents.incrementAndGetPermutation(tempPermutation1);
  }
  return false;
}

void rootSubalgebra::DoKRootsEnumeration()
{ this->theKEnumerations.SetSize(this->PosRootsKConnectedComponents.size);
  this->theKComponentRanks.SetSize(this->PosRootsKConnectedComponents.size);
  Matrix<Rational> tempMat;
  Selection tempSel;
  for (int i = 0; i < this->PosRootsKConnectedComponents.size; i ++)
  { this->theKEnumerations[i].init(this->PosRootsKConnectedComponents[i].size);
    this->theKComponentRanks[i]=
    this->PosRootsKConnectedComponents[i].GetRankOfSpanOfElements(&tempMat, &tempSel);
  }
  this->DoKRootsEnumerationRecursively(0);
}

void rootSubalgebra::DoKRootsEnumerationRecursively(int indexEnumeration)
{ int theRank= this->theKComponentRanks[indexEnumeration];
  int numRuns =MathRoutines::NChooseK(this->PosRootsKConnectedComponents[indexEnumeration].size, theRank);
  this->theKEnumerations[indexEnumeration].initNoMemoryAllocation();
  for (int i = 0; i <numRuns; i ++)
  { this->theKEnumerations[indexEnumeration].incrementSelectionFixedCardinality(theRank);
    if (indexEnumeration< this->PosRootsKConnectedComponents.size- 1)
      this->DoKRootsEnumerationRecursively(indexEnumeration + 1);
    else
      this->KEnumerationsToLinComb();
  }
}

void rootSubalgebra::KEnumerationsToLinComb()
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  Matrix<Rational> tempMat;
  Selection tempSelection;
  tempMat.init((int)theDimension, (int)theDimension);
  int counter = 0;
  for (int i = 0; i < this->PosRootsKConnectedComponents.size; i ++)
  { this->PosRootsKConnectedComponents[i].SelectionToMatrixAppend(this->theKEnumerations[i], theDimension, tempMat, counter);
    counter+= this->theKComponentRanks[i];
  }
  //tempMat.ComputeDebugString();
  if (tempMat.Invert())
  { //tempMat.ComputeDebugString();
    for (int l = 0; l< this->TestedRootsAlpha.size; l ++)
    { Vector<Rational> linComb;
      Vector<Rational>& TestedRootAlpha = this->TestedRootsAlpha[l];
      for (int j = 0; j<theDimension; j ++)
      { linComb[j].MakeZero();
        for (int k= 0; k<theDimension; k++)
        { Rational tempRat;
          tempRat.Assign(tempMat.elements[k][j]);
          tempRat.MultiplyBy(TestedRootAlpha[k]);
          linComb[j]+=(tempRat);
        }
      }
      int x= linComb.FindLCMDenominatorsTruncateToInt();
      linComb*=(-x);
      bool foundBadCombination = true;
      for (int i = 0; i <theDimension; i ++)
        if (linComb[i].NumShort == - 1 || linComb[i].NumShort == 1)
        { foundBadCombination = false;
          break;
        }
      if (foundBadCombination)
      { std::string tempS;
        this->LinCombToString(TestedRootAlpha, x, linComb, tempS);
      }
    }
  }
}

bool rootSubalgebra::CheckConsistency() const
{ if (this->flagDeallocated)
  { crash << "Programming error: use after free of root subalgebra. " << crash;
    return false;
  }
  return true;
}

bool rootSubalgebra::CheckInitialization() const
{ this->CheckConsistency();
  if (this->ownEr == 0)
  { crash << "Root subalgebra is not initialized properly. " << crash;
    return false;
  }
  return true;
}

bool rootSubalgebra::CheckScalarProdMatrixOrdered() const
{ Matrix<Rational> theMat;
  this->SimpleBasisK.GetGramMatrix(theMat, &this->GetAmbientWeyl().CartanSymmetric);
  if (theMat!= this->scalarProdMatrixOrdered)
    crash << "Bilinear product matrix does not match the stored value. " << crash;
  return true;
}

void rootSubalgebra::ComputePotentialExtensions()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputePotentialExtensions");
  this->ownEr->GrowDynkinType(this->theDynkinType, this->potentialExtensionDynkinTypes, &this->potentialExtensionRootPermutations);
  //stOutput << "<hr>" << this->theDynkinType.ToString() << " may potentially extend to "
  //<< this->potentialExtensionDynkinTypes.ToString();
  this->potentialExtensionCartanSymmetrics.SetSize(this->potentialExtensionDynkinTypes.size);
  for (int i = 0; i < this->potentialExtensionDynkinTypes.size; i ++)
    this->potentialExtensionDynkinTypes[i].GetCartanSymmetric(this->potentialExtensionCartanSymmetrics[i]);
}

bool rootSubalgebras::GrowDynkinType(const DynkinType& input, List<DynkinType>& output, List<List<int> >* outputPermutationSimpleRoots) const
{ MacroRegisterFunctionWithName("rootSubalgebras::GrowDynkinType");
  input.Grow(this->validScales, this->GetOwnerWeyl().GetDim(), output, outputPermutationSimpleRoots);
  char theLetter;
  if (!this->owner->theWeyl.theDynkinType.IsSimple(&theLetter))
    return true;
  for (int i = output.size- 1; i>= 0; i--)
  { bool isGood= true;
    if (theLetter =='A')
      if (output[i].ContainsType('B') || output[i].ContainsType('C') || output[i].ContainsType('D') ||
          output[i].ContainsType('E') || output[i].ContainsType('F') || output[i].ContainsType('G'))
        isGood= false;
    if (theLetter =='B')
      if (output[i].ContainsType('C') || output[i].ContainsType('E') || output[i].ContainsType('F') || output[i].ContainsType('G'))
        isGood= false;
    if (theLetter =='C')
      if (output[i].ContainsType('D') || output[i].ContainsType('E') || output[i].ContainsType('F') || output[i].ContainsType('G'))
        isGood= false;
    if (theLetter =='D')
      if (output[i].ContainsType('B') || output[i].ContainsType('C') ||
          output[i].ContainsType('E') || output[i].ContainsType('F') || output[i].ContainsType('G'))
        isGood= false;
    if (theLetter =='E')
      if (output[i].ContainsType('B') || output[i].ContainsType('C') || output[i].ContainsType('F') || output[i].ContainsType('G'))
        isGood= false;
    if (theLetter =='F')
      if (output[i].ContainsType('G'))
        isGood= false;
    if (theLetter =='G')
      if (output[i].ContainsType('B'))
        isGood= false;
    if (!isGood)
    { output.RemoveIndexSwapWithLast(i);
      if (outputPermutationSimpleRoots!= 0)
        outputPermutationSimpleRoots->RemoveIndexSwapWithLast(i);
    }
  }
  return true;
}

void rootSubalgebra::ComputeOuterSAautosExtendingToAmbientAutosGenerators()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeOuterSAautosExtendingToAmbientAutosGenerators");
  if (this->SimpleBasisK.size== 0)
    return;
  List<MatrixTensor<Rational> > outerAutos;
  this->theDynkinType.GetOuterAutosGeneratorsActOnVectorColumn(outerAutos);
  Matrix<Rational> simpleBasisMatrixTimesCartanSymm;
  simpleBasisMatrixTimesCartanSymm.AssignVectorsToRows(this->SimpleBasisK);
  simpleBasisMatrixTimesCartanSymm*= this->GetAmbientWeyl().CartanSymmetric;
  Vectors<Rational> basisOrthogonalRoots;
  simpleBasisMatrixTimesCartanSymm.GetZeroEigenSpaceModifyMe(basisOrthogonalRoots);
  Vectors<Rational> imagesWeightBasis, weightBasis = this->SimpleBasisK;
  weightBasis.AddListOnTop(basisOrthogonalRoots);
  Matrix<Rational> basisMatrixInverted, resultingOperator;
  basisMatrixInverted.AssignVectorsToColumns(weightBasis);
  basisMatrixInverted.Invert();
  this->outerSAautos.theGenerators.SetSize(outerAutos.size);
  for (int i = 0; i <outerAutos.size; i ++)
  { outerAutos[i].ActOnVectorROWSOnTheLeft(this->SimpleBasisK, imagesWeightBasis);
    imagesWeightBasis.AddListOnTop(basisOrthogonalRoots);
    resultingOperator.AssignVectorsToColumns(imagesWeightBasis);
    resultingOperator*=basisMatrixInverted;
    this->outerSAautos.theGenerators[i]=resultingOperator;
  }
//  bool doDebug= false;
//  if (this->theDynkinType.ToString()=="E^{1}_6")
//    doDebug= true;
  this->outerSAautos.GenerateElements(0);
//  if (doDebug)
//  { stOutput << "<br>Outer autos: ";
//    for (int i = 0; i < this->outerSAautos.theElements.size; i ++)
//      stOutput << "<br>" << this->outerSAautos.theElements[i].ToStringMatForm();
//  }
  this->outerSAautosExtendingToAmbientAutosGenerators.theElements.Clear();
  for (int i = 0; i < this->outerSAautos.theElements.size; i ++)
    if (this->GetAmbientWeyl().IsElementWeylGroupOrOuterAuto(this->outerSAautos.theElements[i]))
      this->outerSAautosExtendingToAmbientAutosGenerators.theElements.AddOnTop(this->outerSAautos.theElements[i]);
//    else
//      if (doDebug)
//        stOutput << "<br>" << this->outerSAautos.theElements[i].ToStringMatForm() << " ain't no good. ";
}

bool rootSubalgebra::CheckForMaximalDominanceCartanSA()
{ MacroRegisterFunctionWithName("rootSubalgebra::CheckForMaximalDominanceCartanSA");
  Vectors<Rational> simpleBasisOriginalOrderCopy;
  //bool doDebug= false;
  for (int i = 0; i < this->outerSAautos.theElements.size; i ++)
    if (!this->outerSAautos.theElements[i].IsID())
    { simpleBasisOriginalOrderCopy= this->SimpleBasisKinOrderOfGeneration;
      this->outerSAautos.theElements[i].ActOnVectorsColumn(simpleBasisOriginalOrderCopy);
      //if (doDebug)
      //  stOutput << "<br>" << this->outerSAautos.theElements[i].ToStringMatForm()
      //  << " acting on " << this->SimpleBasisKinOrderOfGeneration.ToString() << " yields " << simpleBasisOriginalOrderCopy.ToString();
      this->GetAmbientWeyl().RaiseToMaximallyDominant(simpleBasisOriginalOrderCopy, true);
      //if (doDebug)
      //  stOutput << "which get raised to: " << simpleBasisOriginalOrderCopy.ToString();
      for (int j = 0; j<simpleBasisOriginalOrderCopy.size; j ++)
        if (simpleBasisOriginalOrderCopy[j]!= this->SimpleBasisKinOrderOfGeneration[j])
        { if (simpleBasisOriginalOrderCopy[j].IsGreaterThanLexicographic(this->SimpleBasisKinOrderOfGeneration[j]))
          { if (this->indexInducingSubalgebra!= - 1)
              this->ownEr->theSubalgebras[this->indexInducingSubalgebra].numHeirsRejectedNotMaxWRTouterAuto++;
            return false;
          } else
            break;
        }
    }
  return true;
}

void rootSubalgebra::ComputeEssentialS()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeEssentialS");
  this->SimpleBasisK= this->genK;
  this->SimpleBasisK.GetGramMatrix(this->scalarProdMatrixOrdered, &this->GetAmbientWeyl().CartanSymmetric);
  this->theDynkinDiagram.AmbientRootSystem= this->GetAmbientWeyl().RootSystem;
  this->theDynkinDiagram.AmbientBilinearForm= this->GetAmbientWeyl().CartanSymmetric;
  this->theDynkinDiagram.ComputeDiagramInputIsSimple(this->SimpleBasisK);
  this->theDynkinDiagram.GetDynkinType(this->theDynkinType);
  if (this->SimpleBasisK.size!= 0)
    if (this->theDynkinType.ToString()=="0")
      crash << "Subalgebra dynkin type computed to be zero while the simple basis is: "
      << this->SimpleBasisK.ToString() << ". " << crash;

  this->ComputeKModules();
  this->ComputeCentralizerFromKModulesAndSortKModules();
  this->ComputeModuleDecompoAmbientAlgebraDimensionsOnly();
  this->CheckRankInequality();
  this->NilradicalKmods.init(this->Modules.size);
}

bool rootSubalgebra::ComputeEssentialsIfNew()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeEssentialsIfNew");
  this->genK= this->SimpleBasisK;
  this->CheckInitialization();
  ProgressReport theReport;
  std::stringstream reportStream;
  this->SimpleBasisKScaledToActByTwo= this->SimpleBasisK;
//  stOutput << "Simple basis k: " << this->SimpleBasisK.ToString();
  for (int i = 0; i < this->SimpleBasisK.size; i ++)
//  { stOutput << "the scalar prod: " << this->GetAmbientWeyl().RootScalarCartanRoot(this->SimpleBasisK[i], this->SimpleBasisK[i]).ToString();
    this->SimpleBasisKScaledToActByTwo[i]*=2/this->GetAmbientWeyl().RootScalarCartanRoot
    (this->SimpleBasisK[i], this->SimpleBasisK[i]);
//  }
  if (theGlobalVariables.flagReportEverything)
  { reportStream << "Computing root subalgebra... ";
    theReport.Report(reportStream.str());
  }
  if (this->indexInducingSubalgebra!= - 1)
  { //stOutput << "<hr>Testing simple basis: " << this->SimpleBasisK.ToString();
    this->SimpleBasisK.GetGramMatrix(this->scalarProdMatrixPermuted, &this->GetAmbientWeyl().CartanSymmetric);
    int goodPermutation = - 1;
    List<List<int> >& extensionRootPermutations = this->ownEr->theSubalgebras[this->indexInducingSubalgebra].potentialExtensionRootPermutations;
    List<Matrix<Rational> >& extensionCartanSymmetrics = this->ownEr->theSubalgebras[this->indexInducingSubalgebra].potentialExtensionCartanSymmetrics;
    for (int i = 0; i <extensionRootPermutations.size && goodPermutation == - 1; i ++)
    { this->scalarProdMatrixOrdered.MakeZeroMatrix(this->SimpleBasisK.size);
      for (int j = 0; j< this->SimpleBasisK.size; j ++)
        for (int k= 0; k< this->SimpleBasisK.size; k++)
          this->scalarProdMatrixOrdered(extensionRootPermutations[i][j], extensionRootPermutations[i][k])= this->scalarProdMatrixPermuted(j,k);
      if (this->scalarProdMatrixOrdered==extensionCartanSymmetrics[i])
      { goodPermutation =i;
        break;
      }
    }
    if (goodPermutation == - 1)
    { this->ownEr->theSubalgebras[this->indexInducingSubalgebra].numHeirsRejectedBadAngleS++;
      return false;
    }
    Vectors<Rational> copySimpleBasisK= this->SimpleBasisK;
    for (int i = 0; i < this->SimpleBasisK.size; i ++)
      this->SimpleBasisK[extensionRootPermutations[goodPermutation][i]]=copySimpleBasisK[i];
  } else
    this->SimpleBasisK.GetGramMatrix(this->scalarProdMatrixOrdered, &this->GetAmbientWeyl().CartanSymmetric);
  if (theGlobalVariables.flagReportEverything)
  { reportStream << "...found a candidate type... ";
    theReport.Report(reportStream.str());
  }
  if (this->SimpleBasisK.GetRankOfSpanOfElements()!= this->SimpleBasisK.size)
    crash << "<br>simple basis vectors not linearly independent! " << crash;
  if (!this->GetAmbientWeyl().AreMaximallyDominant(this->SimpleBasisKinOrderOfGeneration, true))
  { Vectors<Rational> tempVs = this->SimpleBasisKinOrderOfGeneration;
    tempVs.RemoveLastObject();
    if (!this->GetAmbientWeyl().AreMaximallyDominant(tempVs, true))
      crash << "<br>This is a programming error: first vectors " << tempVs.ToString() << " are not maximally dominant. " << crash;
    if (this->indexInducingSubalgebra!= - 1)
      this->ownEr->theSubalgebras[this->indexInducingSubalgebra].numHeirsRejectedNotMaximallyDominant++;
    return false;
  }
  if (theGlobalVariables.flagReportEverything)
  { reportStream << "...the candidate's roots are maximally dominant... ";
    theReport.Report(reportStream.str());
  }
  this->theDynkinDiagram.AmbientBilinearForm= this->GetAmbientWeyl().CartanSymmetric;
  this->theDynkinDiagram.AmbientRootSystem= this->GetAmbientWeyl().RootSystem;
//  stOutput << "<br>Ambient bilinear form: " << this->theDynkinDiagram.AmbientBilinearForm.ToString();
//  stOutput << "<br>Computing Dynkin diagram from roots: " << this->SimpleBasisK.ToString();
  this->theDynkinDiagram.ComputeDiagramInputIsSimple(this->SimpleBasisK);
  this->theDynkinDiagram.GetDynkinType(this->theDynkinType);
  this->ComputeKModules();
  this->ComputeCentralizerFromKModulesAndSortKModules();
  this->ComputeModuleDecompoAmbientAlgebraDimensionsOnly();
  this->CheckRankInequality();
  for (int i = 0; i < this->ownEr->theSubalgebras.size; i ++)
    if (this->ownEr->theSubalgebras[i].theDynkinDiagram== this->theDynkinDiagram &&
        this->ownEr->theSubalgebras[i].theCentralizerDynkinType== this->theCentralizerDynkinType &&
        this->moduleDecompoAmbientAlgebraDimensionsOnly== this->ownEr->theSubalgebras[i].moduleDecompoAmbientAlgebraDimensionsOnly        )
    { if (this->indexInducingSubalgebra!= - 1)
        this->ownEr->theSubalgebras[this->indexInducingSubalgebra].numHeirsRejectedSameModuleDecompo++;
      return false;
    }
  if (theGlobalVariables.flagReportEverything)
  { reportStream << "...module decomposition computed, subalgebra type: " << this->theDynkinType.ToString()
    << ", centralizer type: " << this->theCentralizerDynkinType.ToString() << ". Computing outer automorphisms that "
    << " have zero action on centralizer and extend to ambient automorphisms... ";
    theReport.Report(reportStream.str());
  }
  return true;
}

bool rootSubalgebra::IsEquivalentToByDiagramsAndDimensions
(const rootSubalgebra& other) const
{/* stOutput << "<br>Comparing " << this->theDynkinType.ToString() << " centralized by "
  << this->theCentralizerDynkinType.ToString() << " with mod decompo "
  << this->moduleDecompoAmbientAlgebraDimensionsOnly.ToString()
  << " to " << other.theDynkinType.ToString() << " centralized by "
  << other.theCentralizerDynkinType.ToString() << " with mod decompo "
  << other.moduleDecompoAmbientAlgebraDimensionsOnly.ToString();
*/
  return this->moduleDecompoAmbientAlgebraDimensionsOnly== other.moduleDecompoAmbientAlgebraDimensionsOnly
  && this->theDynkinType== other.theDynkinType && this->theCentralizerDynkinType== other.theCentralizerDynkinType;
}

void rootSubalgebra::GenerateAutomorphismsPreservingBorel(SubgroupWeylGroupOLD& outputAutomorphisms)
{ this->ComputeEssentialS();
  this->GenerateIsomorphismsPreservingBorel(*this, &outputAutomorphisms);
}


void rootSubalgebras::ComputeAllReductiveRootSubalgebrasUpToIsomorphismOLD(bool sort, bool computeEpsCoords)
{ MacroRegisterFunctionWithName("rootSubalgebras::ComputeAllReductiveRootSubalgebrasUpToIsomorphismOLD");
  this->theSubalgebras.size= 0;
  this->GetOwnerWeyl().ComputeRho(true);
  //this->initDynkinDiagramsNonDecided(this->AmbientWeyl, WeylLetter, WeylRank);
  rootSubalgebras rootSAsGenerateAll;
  rootSAsGenerateAll.theSubalgebras.SetSize(this->GetOwnerSSalgebra().GetRank()*2+ 1);
  rootSAsGenerateAll.theSubalgebras[0].genK.size= 0;
  rootSAsGenerateAll.theSubalgebras[0].ownEr = this;
  rootSAsGenerateAll.theSubalgebras[0].ComputeEssentialS();
  this->ComputeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(rootSAsGenerateAll.theSubalgebras, 1);
//  stOutput << this->ToString();
  if (sort)
    this->SortDescendingOrderBySSRank();
  if (computeEpsCoords)
    for (int i = 0; i < this->theSubalgebras.size; i ++)
      this->theSubalgebras[i].ComputeEpsCoordsWRTk();
}

bool slTwoSubalgebra::AttemptExtendingHFtoHEFWRTSubalgebra
(Vectors<Rational>& RootsWithCharacteristic2, Selection& theZeroCharacteristics, Vectors<Rational>& simpleBasisSA, Vector<Rational>& h,
 ElementSemisimpleLieAlgebra<Rational>& outputE, ElementSemisimpleLieAlgebra<Rational>& outputF, Matrix<Rational>& outputMatrixSystemToBeSolved,
 PolynomialSubstitution<Rational>& outputSystemToBeSolved, Matrix<Rational>& outputSystemColumnVector)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::AttemptExtendingHFtoHEFWRTSubalgebra");
  //bool ereBeProbs =(h.ToString()=="(18, 10)") || (h.ToString()=="(6, 10, 12)") ||
  //h.ToString()=="(6, 4)" || h.ToString() =="(3, 4, 3)";
  //if (ereBeProbs)
  //{ stOutput << "<br>Ere be probs ! hChar: " << h.ToString() << "<br>" ;
  //}
  if (theZeroCharacteristics.CardinalitySelection == theZeroCharacteristics.MaxSize)
    return false;
  Vectors<Rational> rootsInPlay;
  rootsInPlay.size= 0;
  int theRelativeDimension = simpleBasisSA.size;
//  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
  if (theRelativeDimension!= theZeroCharacteristics.MaxSize)
    crash << crash;
  Vector<Rational> tempRoot, tempRoot2;
  //format. We are looking for an sl(2) for which e= a_0 g^\alpha_0+... a_kg^\alpha_k, and
  // f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  //where the first \alpha's are ordered as in rootsInPlay.
  //Those are ordered as follows. First come the simple roots of characteristic 2,
  //and the last \alpha's are the members of SelectedExtraPositiveRoots
  //(i.e. root equal to the sum of one simple root
  // of characteristic 2 with a simple roots of characteristic 0).
  // Then the first k variables of the polynomials below will be a_0, ..., a_k., and
  // the last k variables will be the b_i's
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2}, where
  // l/2 is the index of the root
  // of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+ 1)/2}} otherwise
  for (int i = 0; i <theRelativeDimension; i ++)
    if (!theZeroCharacteristics.selected[i])
      rootsInPlay.AddOnTop(simpleBasisSA[i]);
  Vectors<Rational> SelectedExtraPositiveRoots;
  for (int i = 0; i <RootsWithCharacteristic2.size; i ++)
    if (!simpleBasisSA.Contains(RootsWithCharacteristic2[i]))
      SelectedExtraPositiveRoots.AddOnTop(RootsWithCharacteristic2[i]);
  int numRootsChar2 = rootsInPlay.size;
  rootsInPlay.AddListOnTop(SelectedExtraPositiveRoots);
//  if (ereBeProbs)
//    stOutput << "<hr>The roots in play are: " << rootsInPlay.ToString();

  int halfNumberVariables = rootsInPlay.size;
  int numberVariables = halfNumberVariables*2;
  MonomialP tempM;
  tempM.MakeOne(numberVariables);
  Matrix<Rational> coeffsF;
  coeffsF.init(1, halfNumberVariables);
  for (int i = 0; i <numRootsChar2; i ++)
    coeffsF.elements[0][i]=i*i+ 1; //(i%2== 0)? 1: 2;
  for (int i =numRootsChar2; i <coeffsF.NumCols; i ++)
    coeffsF.elements[0][i]=i*i+ 1;
  this->initHEFSystemFromECoeffs
  (rootsInPlay, numberVariables, halfNumberVariables,
   h, coeffsF, outputMatrixSystemToBeSolved, outputSystemColumnVector, outputSystemToBeSolved);
  Matrix<Rational> tempMat, tempMatColumn, tempMatResult;
  tempMat =(outputMatrixSystemToBeSolved);
  tempMatColumn =(outputSystemColumnVector);
  outputF.MakeZero();
  outputE.MakeZero();
//  if (Matrix<Rational> ::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(outputMatrixSystemToBeSolved, outputSystemColumnVector, tempMatResult))
//  if (ereBeProbs)
//    stOutput << "<br>Solving system with mat: " << tempMat.ToString() << " col v-r: " << tempMatColumn.ToString();
  ChevalleyGenerator tempGen;
  if (Matrix<Rational>::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(tempMat, tempMatColumn, tempMatResult))
  { //if (ereBeProbs)
    //  stOutput << "<br>'n the system is solved like a charm.";
    for (int i = 0; i <rootsInPlay.size; i ++)
    { tempGen.MakeGenerator(this->GetOwnerSSAlgebra(), this->GetOwnerSSAlgebra().GetGeneratorFromRoot(-rootsInPlay[i]));
      outputF.AddMonomial(tempGen, coeffsF.elements[0][i]);
      tempGen.MakeGenerator(this->GetOwnerSSAlgebra(), this->GetOwnerSSAlgebra().GetGeneratorFromRoot(rootsInPlay[i]));
      outputE.AddMonomial(tempGen, tempMatResult.elements[i][0]);
    }
    return true;
  } //else
//    if (ereBeProbs)
     // stOutput << "<br><b>System for hChar: " << h.ToString() << " not solved!!!</b>";
  return false;
}

void slTwoSubalgebra::initHEFSystemFromECoeffs
(Vectors<Rational>& rootsInPlay,
 int numberVariables, int halfNumberVariables, Vector<Rational>& targetH, Matrix<Rational>& inputFCoeffs, Matrix<Rational>& outputMatrixSystemToBeSolved,
 Matrix<Rational>& outputSystemColumnVector, PolynomialSubstitution<Rational>& outputSystemToBeSolved)
{ MacroRegisterFunctionWithName("slTwoSubalgebra::initHEFSystemFromECoeffs");
  Vector<Rational> tempRoot;
  MonomialP tempM;
  Rational tempRat;
  HashedList<Vector<Rational> > RootSpacesThatNeedToBeKilled;
  RootSpacesThatNeedToBeKilled.SetExpectedSize(this->GetOwnerWeyl().RootSystem.size);
//  IndicesEquationsByRootSpace.Reserve(this->theWeyl.RootSystem.size);
  outputSystemToBeSolved.size= 0;
  outputMatrixSystemToBeSolved.init(0, numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i = 0; i <rootsInPlay.size; i ++)
    if (this->GetOwnerWeyl().RootScalarCartanRoot(targetH, rootsInPlay[i])!=2)
      crash << "The scalar product of the h element: " << targetH.ToString()
      << " and the root in play " << rootsInPlay[i].ToString() << " must be 2, but equals instead "
      << this->GetOwnerWeyl().RootScalarCartanRoot(targetH, rootsInPlay[i]).ToString() << crash;
  for (int i = 0; i <rootsInPlay.size; i ++)
    for (int j = 0; j<rootsInPlay.size; j ++)
    { tempRoot = rootsInPlay[i]-rootsInPlay[j];
      if (this->GetOwnerWeyl().IsARoot(tempRoot))
      { int indexEquation = RootSpacesThatNeedToBeKilled.GetIndex(tempRoot);
        if (indexEquation == - 1)
        { RootSpacesThatNeedToBeKilled.AddOnTop(tempRoot);
          indexEquation = outputSystemToBeSolved.size;
//          IndicesEquationsByRootSpace.AddOnTop(indexEquation);
          outputSystemToBeSolved.SetSize(outputSystemToBeSolved.size + 1);
          outputSystemToBeSolved.LastObject()->MakeZero();
        }
        tempM.MakeOne(numberVariables);
        tempM[i]=1;
        tempM[j+halfNumberVariables]=1;
        Rational tempCoeff= this->GetOwnerSSAlgebra().GetConstant(rootsInPlay[i], -rootsInPlay[j]);
        outputSystemToBeSolved[indexEquation].AddMonomial(tempM, tempCoeff);
        //outputSystemToBeSolved.ComputeDebugString();
      }
    }
  int oldSize= outputSystemToBeSolved.size;
  outputSystemToBeSolved.SetSize(oldSize +this->GetOwnerWeyl().CartanSymmetric.NumRows);
  for (int i = oldSize; i <outputSystemToBeSolved.size; i ++)
    outputSystemToBeSolved[i].MakeZero();
  //outputSystemToBeSolved.ComputeDebugString();
//  ElementSemisimpleLieAlgebra g1, g2;
  for (int i = 0; i <rootsInPlay.size; i ++)
  { if (rootsInPlay.size!=halfNumberVariables)
      crash << crash;
    this->GetOwnerSSAlgebra().GetConstantOrHElement(rootsInPlay[i], -rootsInPlay[i], tempRat, tempRoot);
    for (int j = 0; j< this->GetOwnerSSAlgebra().GetRank(); j ++)
    { tempM.MakeOne(numberVariables);
      tempM[i]=1;
      tempM[i+halfNumberVariables]=1;
      outputSystemToBeSolved[j+oldSize].AddMonomial(tempM, tempRoot[j]);
    }
  }
  for (int i = 0; i < this->GetOwnerSSAlgebra().GetRank(); i ++)
    outputSystemToBeSolved[i+oldSize].AddConstant(targetH[i]*(- 1));
  outputMatrixSystemToBeSolved.init(outputSystemToBeSolved.size, halfNumberVariables);
  outputSystemColumnVector.init(outputSystemToBeSolved.size, 1);
  outputMatrixSystemToBeSolved.MakeZero();
  outputSystemColumnVector.MakeZero();
  for (int i = 0; i <outputSystemToBeSolved.size; i ++)
    for (int j = 0; j<outputSystemToBeSolved[i].size(); j ++)
    { int lowerIndex= - 1; int higherIndex= - 1;
      Polynomial<Rational>& currentPoly= outputSystemToBeSolved[i];
      Rational& currentCoeff=currentPoly.theCoeffs[j];
      for (int k= 0; k<numberVariables; k++)
        if (currentPoly[j](k)==1)
        { if (k<halfNumberVariables)
            lowerIndex=k;
          else
          { higherIndex=k;
            break;
          }
        }
      if (lowerIndex== - 1)
        outputSystemColumnVector.elements[i][0]= currentCoeff*(- 1);
      else
        outputMatrixSystemToBeSolved.elements[i][lowerIndex]=currentCoeff* inputFCoeffs.elements[0][higherIndex-halfNumberVariables];
    }
//  outputSystemToBeSolved.ComputeDebugString();
}

void rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition
(SltwoSubalgebras& output, int indexRootSAinContainer)
{ MacroRegisterFunctionWithName("rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition");
  //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1- 10.4
  int theRelativeDimension = this->SimpleBasisK.size;
  if (theRelativeDimension == 0)
    return;
  Selection selectionRootsWithZeroCharacteristic;
  Selection simpleRootsChar2;
  Vectors<Rational> rootsScalarProduct2HnonRaised;
  Vectors<Rational> reflectedSimpleBasisK;
  rootsScalarProduct2HnonRaised.Reserve(this->PositiveRootsK.size);
  ElementWeylGroup<WeylGroupData> raisingElt;
  selectionRootsWithZeroCharacteristic.init(theRelativeDimension);
  Matrix<Rational> InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k= 0; k<theRelativeDimension; k++)
    for (int j = 0; j<theRelativeDimension; j ++)
      InvertedRelativeKillingForm(k,j)= this->GetAmbientWeyl().RootScalarCartanRoot
      (this->SimpleBasisK[k], this->SimpleBasisK[j]);
  InvertedRelativeKillingForm.Invert();
  int numCycles = MathRoutines::TwoToTheNth(selectionRootsWithZeroCharacteristic.MaxSize);
  ProgressReport theReport;
  Vectors<Rational> rootsZeroChar;
  rootsZeroChar.Reserve(selectionRootsWithZeroCharacteristic.MaxSize);
  Vectors<Rational> relativeRootSystem;
//  Selection tempSel;
  this->PositiveRootsK.GetCoordsInBasis(this->SimpleBasisK, relativeRootSystem);
  slTwoSubalgebra theSl2;
  theSl2.container = &output;
  theSl2.owner = &this->GetOwnerSSalg();
  SemisimpleLieAlgebra& theLieAlgebra = this->GetOwnerSSalg();
  DynkinDiagramRootSubalgebra diagramZeroCharRoots;
//  stOutput << "<br>problems abound here!" << this->theDynkinDiagram.ToStringRelativeToAmbientType(this->owner->theWeyl.theDynkinType[0]);
  //bool ereBeProbs = this->theDynkinDiagram.ToStringRelativeToAmbientType(this->owner->theWeyl.theDynkinType[0])=="3A^{1}_1";
  //if (ereBeProbs)
  //{ stOutput << "<hr>Ere be probs. ";
  //}
  //if (ereBeProbs)
  //  stOutput << "<br>Simple basis k: " << this->SimpleBasisK.ToString();
  for (int cyclecounter = 0; cyclecounter<numCycles; cyclecounter++, selectionRootsWithZeroCharacteristic.incrementSelection())
  { this->SimpleBasisK.SubSelection(selectionRootsWithZeroCharacteristic, rootsZeroChar);
    diagramZeroCharRoots.ComputeDiagramTypeModifyInput(rootsZeroChar, this->GetAmbientWeyl());
    int theSlack= 0;
    rootsScalarProduct2HnonRaised.size= 0;
    simpleRootsChar2=selectionRootsWithZeroCharacteristic;
    simpleRootsChar2.InvertSelection();
    Vector<Rational> simpleRootsChar2Vect;
    simpleRootsChar2Vect =simpleRootsChar2;
    for (int j = 0; j<relativeRootSystem.size; j ++)
      if (simpleRootsChar2Vect.ScalarEuclidean(relativeRootSystem[j])==1)
      { theSlack++;
        rootsScalarProduct2HnonRaised.AddOnTop(this->PositiveRootsK[j]);
      }
    int theDynkinEpsilon =diagramZeroCharRoots.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is //
    //except for G_2 (go figure!).
    //(but selectionRootsWithZeroCharacteristic still have to be found)
    //this is done in the below code.
//    if (ereBeProbs)
//      stOutput << "<br>Sel: " << selectionRootsWithZeroCharacteristic.ToString() << ", dynkin epsilon: " << theDynkinEpsilon;
    if (theDynkinEpsilon!= 0)
      continue;
    Vector<Rational> relativeCharacteristic, relativeSimpleCoords;
    relativeCharacteristic.MakeZero(theRelativeDimension);
    for (int k= 0; k<theRelativeDimension; k++)
      if (!selectionRootsWithZeroCharacteristic.selected[k])
        relativeCharacteristic[k]=2;
    InvertedRelativeKillingForm.ActOnVectorColumn(relativeCharacteristic, relativeSimpleCoords);
    Vector<Rational> characteristicH;
    characteristicH.MakeZero(theLieAlgebra.GetRank());
    for (int j = 0; j<theRelativeDimension; j ++)
      characteristicH+= this->SimpleBasisK[j]*relativeSimpleCoords[j];
    for (int k= 0; k<rootsScalarProduct2HnonRaised.size; k++)
      if (this->GetAmbientWeyl().RootScalarCartanRoot(characteristicH, rootsScalarProduct2HnonRaised[k])!=2)
        crash << "Programming error: characteristicH is: " << characteristicH.ToString()
        << "; rootsWithScalarProduct2NonRaised: "
        << rootsScalarProduct2HnonRaised.ToString()
        << "; the scalar product with vector "<< rootsScalarProduct2HnonRaised[k].ToString() << " is:  "
        << this->GetAmbientWeyl().RootScalarCartanRoot
        (characteristicH, rootsScalarProduct2HnonRaised[k]).ToString()
        << " which is supposed to equal 2. " << crash;
//    stOutput << "Before raising char is: " << characteristicH.ToString();
    this->GetAmbientWeyl().RaiseToDominantWeight(characteristicH, 0, 0, &raisingElt);
//    stOutput << " After raising char is: " << characteristicH.ToString();
    ////////////////////
    //bool ereBeProbs =
    //(characteristicH.ToString()=="(18, 10)") || (characteristicH.ToString()=="(6, 10, 12)")||
    //(characteristicH.ToString()=="(6, 4)")
    //;

    reflectedSimpleBasisK= this->SimpleBasisK;
    ////////////////////
    for (int k= 0; k<reflectedSimpleBasisK.size; k++)
      this->GetAmbientWeyl().ActOn(raisingElt, reflectedSimpleBasisK[k]);
    //if (ereBeProbs)
    //{ stOutput << "<hr>'n the raising element is: drumroll..." << raisingElt.ToString()
    //  << ", acting on: " << reflectedSimpleBasisK.ToString();
    //  stOutput << "<br>so the reflected simple basis becomes: " << reflectedSimpleBasisK.ToString();
    //}
    theSl2.RootsWithScalar2WithH=rootsScalarProduct2HnonRaised;
    for (int k= 0; k<theSl2.RootsWithScalar2WithH.size; k++)
      this->GetAmbientWeyl().ActOn(raisingElt, theSl2.RootsWithScalar2WithH[k]);
    for (int i = 0; i <theSl2.RootsWithScalar2WithH.size; i ++)
      if (this->GetAmbientWeyl().RootScalarCartanRoot(characteristicH, theSl2.RootsWithScalar2WithH[i])!=2)
        crash << "Programming error, bad scalar product after raising: raised characteristic: "
        << characteristicH.ToString()
        << " simplebasisK: " << this->SimpleBasisK.ToString()
        << "raised by: " << raisingElt.ToString()
        << " to get: " << reflectedSimpleBasisK.ToString()
        << " theSl2.RootsWithScalar2WithH: "
        << theSl2.RootsWithScalar2WithH.ToString()
        << ", theSl2.RootsWithScalar2WithH[i]: "
        << theSl2.RootsWithScalar2WithH[i].ToString()
        << " scalar product: "
        << this->GetAmbientWeyl().RootScalarCartanRoot(characteristicH, theSl2.RootsWithScalar2WithH[i]).ToString()
        << ". The inverted relative cartan: " << InvertedRelativeKillingForm.ToString()
        << ". The cartan: " << this->GetAmbientWeyl().CartanSymmetric.ToString() << ". "
        << crash;
    //if (ereBeProbs)
    //  stOutput << "roots scalar 2 with h:" << theSl2.RootsWithScalar2WithH.ToString();
    theSl2.theH.MakeHgenerator(characteristicH, theLieAlgebra);
    theSl2.LengthHsquared= theSl2.GetOwnerSSAlgebra().theWeyl.RootScalarCartanRoot(characteristicH, characteristicH);
    theSl2.theE.MakeZero();
    theSl2.theF.MakeZero();
    //theSl2.ComputeDebugString(false, false);
//    stOutput << "<br>accounting " << characteristicH.ToString();
    if (theSl2.AttemptExtendingHFtoHEFWRTSubalgebra
       (theSl2.RootsWithScalar2WithH, selectionRootsWithZeroCharacteristic, reflectedSimpleBasisK,
        characteristicH, theSl2.theE, theSl2.theF, theSl2.theSystemMatrixForm, theSl2.theSystemToBeSolved,
        theSl2.theSystemColumnVector))
    { int indexIsoSl2= - 1;
      theSl2.MakeReportPrecomputations(indexRootSAinContainer, *this);
      if (output.ContainsSl2WithGivenHCharacteristic(theSl2.hCharacteristic, &indexIsoSl2))
      { output.GetElement(indexIsoSl2).IndicesContainingRootSAs.AddOnTop(indexRootSAinContainer);
        output.IndicesSl2sContainedInRootSA[indexRootSAinContainer].AddOnTop(indexIsoSl2);
      } else
      { output.IndicesSl2sContainedInRootSA[indexRootSAinContainer].AddOnTop(output.size);
        theSl2.indexInContainer = output.size;
        output.AddOnTop(theSl2);
      }
    } else
    { output.BadHCharacteristics.AddOnTop(characteristicH);
      DynkinType tempType;
      diagramZeroCharRoots.GetDynkinType(tempType);
      stOutput << "<br>obtained bad characteristic " << characteristicH.ToString() << ". The zero char root diagram is "
      << tempType.ToString() << "; the Dynkin epsilon is " << theDynkinEpsilon << "= the num roots generated by diagram "
      << diagramZeroCharRoots.NumRootsGeneratedByDiagram() << " + the relative dimension " << theRelativeDimension
      << " - the slack " << theSlack << "<br>The relative root system is: " << relativeRootSystem.ToString();
//      stOutput << "<br> I was exploring " << this->ToString();
    }
    std::stringstream out;
    out << "Exploring Dynkin characteristics case " << cyclecounter+ 1 << " out of " << numCycles;
//    stOutput << "<br>" << out.str();
    theReport.Report(out.str());
  }
//  stOutput << "Bad chracteristics: " << output.BadHCharacteristics.ToString();
}

void rootSubalgebras::ComputeAllReductiveRootSAsInit()
{ this->GetOwnerWeyl().ComputeRho(true);
  this->validScales.Clear();
  this->validScales.SetExpectedSize(this->owner->GetRank()*2);
  for (int i = 0; i < this->owner->GetRank(); i ++)
    this->validScales.AddOnTopNoRepetition(2/this->owner->theWeyl.CartanSymmetric(i,i));
//  stOutput << "Valid scales: " << this->validScales.ToString();
}

void rootSubalgebras::ComputeParabolicPseudoParabolicNeitherOrder()
{ MacroRegisterFunctionWithName("rootSubalgebras::ComputeParabolicPseudoParabolicNeitherOrder");
  Selection parSel;
  parSel.init(this->owner->GetRank());
  Vectors<Rational> basis, currentBasis;
  List<bool> Explored;
  Explored.initFillInObject(this->theSubalgebras.size, false);
  this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.SetSize(0);
  this->NumNonPseudoParabolic = 0;
  this->NumParabolic = 0;
  this->NumPseudoParabolicNonParabolic = 0;
  rootSubalgebra currentSA;
  currentSA.ownEr = this;
  basis.MakeEiBasis(this->owner->GetRank());
  List<rootSubalgebra> currentList;
  ProgressReport theReport;
  for (int i = 0; i <2; i ++)
  { currentList.SetSize(0);
    int counter = 0;
    do
    { if (theGlobalVariables.flagReportEverything)
      { std::stringstream reportStream;
        if (i == 0)
          reportStream << "Exploring which of the subalgebras are parabolic. ";
        else
          reportStream << "Exploring which of the subalgebras are pseudo-parabolic. ";
        reportStream << "Current " << (i == 0 ? "pseudo-parabolic " : "parabolic ") << "selection: "
        << parSel.ToString() << ", total  " << counter << " selections explored. ";
        counter++;
        theReport.Report(reportStream.str());
      }
      basis.SubSelection(parSel, currentBasis);
      if (currentBasis.GetRankOfSpanOfElements()!=currentBasis.size)
        continue;
      currentSA.genK=currentBasis;
      currentSA.ComputeEssentialS();
      if (currentBasis.size!= 0)
        if (currentSA.theDynkinType.ToString()=="0")
        { crash << "Subalgebra dynkin type computed to be zero while currentBasis is " << currentBasis.ToString()
          << " and simple basis k is: " << currentSA.SimpleBasisK.ToString() << crash;
        }
      int theIndex= this->GetIndexUpToEquivalenceByDiagramsAndDimensions(currentSA);
      if (theIndex== - 1)
        crash << "Experimental code has failed an internal check on currentSA: " << currentSA.ToString() << crash;
      if (!Explored[theIndex])
      { currentList.AddOnTop(this->theSubalgebras[theIndex]);
        Explored[theIndex]= true;
        if (i == 0)
          this->NumParabolic++;
        else
          this->NumPseudoParabolicNonParabolic++;
      }
    } while (parSel.IncrementReturnFalseIfPastLast());
    currentList.QuickSortAscending();
    this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.AddListOnTop(currentList);
    basis.AddOnTop(this->owner->theWeyl.RootSystem[0]);
    parSel.init(this->owner->GetRank()+ 1);
  }
  this->NumNonPseudoParabolic = this->theSubalgebras.size-this->NumParabolic-this->NumPseudoParabolicNonParabolic;
  currentList.SetSize(0);
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    if (!Explored[i])
      currentList.AddOnTop(this->theSubalgebras[i]);
  currentList.QuickSortAscending();
  this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.AddListOnTop(currentList);
}

void rootSubalgebras::ComputeAllReductiveRootSubalgebrasUpToIsomorphism()
{ MacroRegisterFunctionWithName("rootSubalgebras::ComputeAllReductiveRootSubalgebrasUpToIsomorphism");
  this->initOwnerMustBeNonZero();
  this->ComputeAllReductiveRootSAsInit();
  HashedList<Vector<Rational> > tempVs;
  this->flagPrintGAPinput = this->owner->theWeyl.LoadGAPRootSystem(tempVs);
  ProgressReport theReport1, theReport2;
  rootSubalgebra currentSA;
  currentSA.ownEr = this;
  currentSA.ComputeEssentialsIfNew();
  currentSA.ComputePotentialExtensions();
  this->theSubalgebras.Reserve(this->GetOwnerWeyl().RootsOfBorel.size);
  this->theSubalgebras.AddOnTop(currentSA);
  std::string reportString;
  for (int i = 0; i < this->theSubalgebras.size; i ++)
  { //bool doDebug;
    //doDebug= (this->theSubalgebras[i].theDynkinType.ToString()=="A^{3}_1+G^{1}_2");
    //doDebug= true;
    if (theGlobalVariables.flagReportEverything)
    { std::stringstream reportStream;
      for (int j = 0; j< this->theSubalgebras[i].potentialExtensionDynkinTypes.size; j ++)
      { reportStream << this->theSubalgebras[i].potentialExtensionDynkinTypes[j].ToString();
        if (j!= this->theSubalgebras[i].potentialExtensionDynkinTypes.size- 1)
          reportStream << ", ";
      }
      reportString=reportStream.str();
    }
//    if (doDebug)
//      stOutput << "Potential extensions of " << this->theSubalgebras[i].theDynkinType.ToString()
//      << ": " << reportString;
    for (int j = 0; j< this->theSubalgebras[i].Modules.size; j ++)
    { if (this->theSubalgebras[i].HighestWeightsPrimalSimple[j].IsEqualToZero())
        continue;
      if (theGlobalVariables.flagReportEverything)
      { std::stringstream out;
        out << "Exploring extensions of subalgebra " << i+ 1 << " out of " << this->theSubalgebras.size << ". Type current SA: "
        << this->theSubalgebras[i].theDynkinType.ToString() << ". Possible standard parabolic extensions: "
        << reportString << ". Exploring extension by lowest weight vector of module " << j+ 1 << " out of " << this->theSubalgebras[i].Modules.size;
        theReport2.Report(out.str());
      }
      currentSA.initNoOwnerReset();
      currentSA.SimpleBasisK= this->theSubalgebras[i].SimpleBasisK;
      currentSA.SimpleBasisK.AddOnTop(this->theSubalgebras[i].LowestWeightsPrimalSimple[j]);
      currentSA.SimpleBasisKinOrderOfGeneration = this->theSubalgebras[i].SimpleBasisKinOrderOfGeneration;
      currentSA.SimpleBasisKinOrderOfGeneration.AddOnTop(this->theSubalgebras[i].LowestWeightsPrimalSimple[j]);
      currentSA.indexInducingSubalgebra =i;

      if (!currentSA.ComputeEssentialsIfNew())
        continue;
      if (currentSA.SimpleBasisK.GetRankOfSpanOfElements()!=currentSA.SimpleBasisK.size)
        crash << "<br>simple basis vectors not linearly independent! " << crash;
      this->theSubalgebras.AddOnTop(currentSA);
      this->theSubalgebras.LastObject()->ComputePotentialExtensions();
    }
  }
  std::stringstream reportStream;
  if (theGlobalVariables.flagReportEverything)
  { reportStream << "Reductive root subalgebra computation done: total " << this->theSubalgebras.size
    << " subalgebras. Proceeding to sort the subalgebras...";
    theReport2.Report(reportStream.str());
  }
//  stOutput << "end!";
  this->SortDescendingOrderBySSRank();
  if (theGlobalVariables.flagReportEverything)
  { reportStream << "done. ";
    theReport2.Report(reportStream.str());
  }

  if (this->flagComputeConeCondition)
  { if (theGlobalVariables.flagReportEverything)
    { reportStream << "Proceeding to compute the module pairing tables ... ";
      theReport2.Report(reportStream.str());
    }
    this->ComputeKmodMultTables();
    if (theGlobalVariables.flagReportEverything)
    { reportStream << " done. ";
      theReport2.Report(reportStream.str());
    }
  }
  if (this->flagPrintParabolicPseudoParabolicInfo)
  { if (theGlobalVariables.flagReportEverything)
    { reportStream << "Computing which subalgebras are pseudo parabolic/parabolic/neither... ";
      theReport2.Report(reportStream.str());
    }
    this->ComputeParabolicPseudoParabolicNeitherOrder();
    if (theGlobalVariables.flagReportEverything)
    { reportStream << " done. ";
      theReport2.Report(reportStream.str());
    }
  }
}

void rootSubalgebras::ComputeAllRootSubalgebrasUpToIso(int StartingIndex, int NumToBeProcessed)
{ static PauseThread localController;
  this->NumSubalgebrasProcessed= 0;
  this->NumConeConditionFailures = 0;
  this->NumSubalgebrasCounted= 0;
  for (int i =StartingIndex; i <NumToBeProcessed+StartingIndex; i ++)
  { this->theSubalgebras[i].flagComputeConeCondition = this->flagComputeConeCondition;
    this->theSubalgebras[i].GeneratePossibleNilradicals(localController, this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration, false, *this, i);
    if (i!=NumToBeProcessed+StartingIndex- 1)
      this->theSubalgebras[i+ 1].GeneratePossibleNilradicalsInit(this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration);
  }
}

WeylGroupData& rootSubalgebras::GetOwnerWeyl() const
{ return this->GetOwnerSSalgebra().theWeyl;
}

SemisimpleLieAlgebra& rootSubalgebras::GetOwnerSSalgebra() const
{ this->CheckInitialization();
  return *this->owner;
}

void rootSubalgebras::ComputeLProhibitingRelations()
{ static PauseThread localController;
  if (this->flagStoringNilradicals)
  { this->storedNilradicals.SetSize(this->theSubalgebras.size);
  }
  for (; this->IndexCurrentSANilradicalsGeneration< this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration; this->IndexCurrentSANilradicalsGeneration ++)
  { this->theSubalgebras[this->IndexCurrentSANilradicalsGeneration].flagComputeConeCondition = this->flagComputeConeCondition;
    this->theSubalgebras[this->IndexCurrentSANilradicalsGeneration].GeneratePossibleNilradicals(localController, this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration, this->flagUsingParabolicsInCentralizers, *this, this->IndexCurrentSANilradicalsGeneration);
    if (this->IndexCurrentSANilradicalsGeneration!= this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration- 1)
      this->theSubalgebras[this->IndexCurrentSANilradicalsGeneration + 1].GeneratePossibleNilradicalsInit(this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration);
  }
}

void rootSubalgebras::SortDescendingOrderBySSRank()
{ MacroRegisterFunctionWithName("rootSubalgebras::SortDescendingOrderBySSRank");
  //Bubble sort
  rootSubalgebras output;
  List<int> SortingArray;
  List<int> inverseOfSortingArray;
  SortingArray.SetSize(this->theSubalgebras.size);
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    SortingArray[i]=i;
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    for (int j =i+ 1; j< this->theSubalgebras.size; j ++)
      if (this->theSubalgebras[SortingArray[j]].theDynkinDiagram.IsGreaterThan(this->theSubalgebras[SortingArray[i]].theDynkinDiagram))
        SortingArray.SwapTwoIndices(i, j);
  inverseOfSortingArray.SetSize(SortingArray.size);
  for (int i = 0; i <SortingArray.size; i ++)
    inverseOfSortingArray[SortingArray[i]]=i;
  output.theSubalgebras.Reserve(this->theSubalgebras.size);
  for (int i = 0; i < this->theSubalgebras.size; i ++)
  { output.theSubalgebras.AddOnTop(this->theSubalgebras[SortingArray[i]]);
    rootSubalgebra& currentSA=*output.theSubalgebras.LastObject();
    List<int>& otherArray= this->theSubalgebras[SortingArray[i]].indicesSubalgebrasContainingK;
    currentSA.indicesSubalgebrasContainingK.Reserve(otherArray.size);
    currentSA.indicesSubalgebrasContainingK.SetSize(0);
    for (int j = 0; j<otherArray.size; j ++)
      currentSA.indicesSubalgebrasContainingK.AddOnTop(inverseOfSortingArray[otherArray[j]]);
    if (currentSA.indexInducingSubalgebra!= - 1)
      currentSA.indexInducingSubalgebra =inverseOfSortingArray[currentSA.indexInducingSubalgebra];
  }
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    this->theSubalgebras[i]=(output.theSubalgebras[i]);
}

void rootSubalgebras::ToHTML(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("rootSubalgebras::ToHTML");
  this->CheckInitialization();
  std::string MyPathPhysical = this->owner->VirtualNameSSAlgOutputFolder+"rootSubalgebras.html";
  std::fstream output;
  FileOperations::OpenFileCreateIfNotPresentVirtual(output, MyPathPhysical, false, true, false);
  if (!FileOperations::FileExistsVirtual(MyPathPhysical))
  { crash << "This may or may not be a programming error. Failed to create file " << MyPathPhysical
    << ". Possible explanations. 1. File permissions - can I write in that folder? 2. Programming error (less likely). "
    << crash;
  }
  output << "<html><title> Root subsystems of "
  << this->theSubalgebras[0].theDynkinDiagram.ToString()
  << "</title>";
  output << "<meta name=\"keywords\" content =\""
  << this->theSubalgebras[0].theDynkinDiagram.ToString()
  << " root subsystems, root subsystems, root systems";
  if (this->GetOwnerWeyl().theDynkinType.HasExceptionalComponent())
    output << ", exceptional Lie algebra";
  output << " \">";
  output << HtmlRoutines::GetJavascriptMathjax();
  output << "<body>" << this->ToString(theFormat)
  << "<hr>LaTeX table with root subalgebra details.<br>"
  << this->ToStringDynkinTableFormat2LaTeX(theFormat)
  << "</body></html>";
  output.close();
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    this->theSubalgebras[i].ToHTML(i, theFormat);
}

std::string rootSubalgebras::ToString(FormatExpressions* theFormat)
{ return this->ToStringDynkinTableHTML(theFormat);
}

bool rootSubalgebras::ReadFromDefaultFileNilradicalGeneration()
{ std::fstream theFile;
  if (FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/theNilradicalsGenerator.txt", false, false, false))
  { theFile.seekg(0);
    this->ReadFromFileNilradicalGeneration(theFile);
    return true;
  }
  return false;
}

void rootSubalgebras::WriteToDefaultFileNilradicalGeneration()
{ std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/theNilradicalsGenerator.txt", false, true, false);
  this->WriteToFileNilradicalGeneration(theFile);
}

void rootSubalgebras::WriteToFileNilradicalGeneration(std::fstream& output)
{ this->GetOwnerWeyl().WriteToFile(output);
  output << "Number_subalgebras: " << this->theSubalgebras.size << "\n";
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
  for (int  i = 0; i < this->theSubalgebras.size; i ++)
    this->theSubalgebras[i].WriteToFileNilradicalGeneration(output);
  this->theGoodRelations.WriteToFile(output);
  this->theBadRelations.WriteToFile(output);
}

void rootSubalgebras::ReadFromFileNilradicalGeneration(std::fstream& input)
{ std::string tempS; int tempI;
  this->GetOwnerWeyl().ReadFromFile(input);
  this->GetOwnerWeyl().ComputeRho(true);
  input >> tempS >> tempI;
  if (tempS!="Number_subalgebras:")
    crash << crash;
  this->theSubalgebras.SetSize(tempI);
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
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    this->theSubalgebras[i].ReadFromFileNilradicalGeneration(input, *this);
  this->theGoodRelations.ReadFromFile(input, *this);
  this->theBadRelations.ReadFromFile(input, *this);
  this->flagNilradicalComputationInitialized= true;
}

void rootSubalgebras::ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess)
{ std::stringstream out; std::string tempS;
  //W'' stands for the graph isomorphisms of C(k_ss) extending to Vector<Rational> system isomorphisms of the entire algebra.
  for (int i = fromIndex; i <NumToProcess; i ++)
    this->GenerateKintersectBOuterIsos(this->theSubalgebras[i]);
  if (useLatex)
    out << "\\begin{tabular}{ccccc}$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  if (useHtml)
    out << "<br><table><tr><td>k_{ss}</td><td></td><td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  Vectors<Rational> emptyRoots;
  emptyRoots.size= 0;
  for (int i = fromIndex; i <NumToProcess; i ++)
  { rootSubalgebra& current = this->theSubalgebras[i];
    SubgroupWeylGroupOLD& theOuterIsos = this->CentralizerOuterIsomorphisms[i];
    theOuterIsos.ComputeSubGroupFromGeneratingReflections(&emptyRoots, &theOuterIsos.ExternalAutomorphisms, 0, true);
    Rational numInnerIsos = current.theCentralizerDiagram.GetSizeCorrespondingWeylGroupByFormula();
    if (useHtml)
      out << "<td>";
    tempS=current.theDynkinDiagram.ToString();
    out << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    tempS=current.theCentralizerDiagram.ToString();
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
  if (useHtml)
    out << "</table><br>";
  output = out.str();
}

std::string rootSubalgebras::ToStringAlgebraLink(int index)
{ std::stringstream out;
  out << "<a href=\"rootSubalgebra_" << index+ 1 << ".html\">"
  << this->theSubalgebras[index].theDynkinDiagram.ToString() << "</a>";
  return out.str();
}

template<class coefficient>
Vector<Rational> ElementSemisimpleLieAlgebra<coefficient>::GetRootIMustBeWeight() const
{ if (this->IsEqualToZero())
    crash << "Calling ElementSemisimpleLieAlgebra::GetRootIMustBeWeight on a zero element is forbidden." << crash;
  if (this->size()>1)
  { if (!this->IsElementCartan())
      crash << "Calling ElementSemisimpleLieAlgebra::GetRootIMustBeWeight on a non-weight element is forbidden. The element is: "
      << this->ToString() << crash;
    Vector<Rational> result;
    result.MakeZero(this->GetOwner()->GetRank());
    return result;
  }
  return this->GetOwner()->GetWeightOfGenerator((*this)[0].theGeneratorIndex);
}

std::string rootSubalgebras::ToStringDynkinTableHTML(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("rootSubalgebras::ToStringDynkinTable");
  (void) theFormat;//taking care of unused parameter in a portable way.
  std::stringstream out;
  std::string tempS, tempS2, tempS3;
  std::string tooltipSAs =
  "h - fixed Cartan subalgebra. k - subalgebra containing h. k_{ss}=[k, k] - regular semisimple subalgebra in the sense of Dynkin\
  , Semisimple Lie subalgebras of semisimple Lie algebras. k_{ss} is parametrized by a root subsytem of \\Delta(g). \
  C(k_{ss}) consists of root spaces with roots strongly orthogonal to \\Delta(k) and a part of the Cartan h";
  int col = 0;
  int row= 0;
  out << "g: " << this->theSubalgebras[0].theDynkinDiagram.ToString()
  << ". There are " << this->theSubalgebras.size << " table entries (= " << this->theSubalgebras.size-2
  << " larger than the Cartan subalgebra + the Cartan subalgebra + the full subalgebra).\n\n";
  out << "<table border =\"1\">\n <colgroup>";
  for (int i = 0; i < this->NumColsPerTableLatex; i ++)
    out << "<col width=\"180\">";
  out << "</colgroup>";
  for (int i = 0; i < this->theSubalgebras.size; i ++)
  { row = (i)/this->NumColsPerTableLatex;
    col = (i)% this->NumColsPerTableLatex;
    if (col == 0)
      out << "<tr>";
    out << "<td style=\"vertical-align: text-top;\" title=\"" << tooltipSAs << "\">";
    out << "\n\nType k_{ss}: " << this->ToStringAlgebraLink(i);
    out << "<br>";
    if (i == 0)
      out << "<b>(Full subalgebra)</b>";
    if (i == this->theSubalgebras.size- 1)
      out << "<b>(Cartan subalgebra)</b>";
    out << "\n<br>\nType C(k_{ss})_{ss}: " << this->theSubalgebras[i].theCentralizerDiagram.ToString();
    if (row == this->NumLinesPerTableLatex)
      row= 0;
    out << "</td>";
    if (col == this->NumColsPerTableLatex- 1 || i == this->theSubalgebras.size- 1)
      out << "</tr>";
  }
  out << "</table>\n\n";
  if (this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size>0)
  { out << "<hr>There are " << this->NumParabolic << " parabolic, " << this->NumPseudoParabolicNonParabolic << " pseudo-parabolic but not parabolic and "
    << this->NumNonPseudoParabolic << " non pseudo-parabolic root subsystems.";
    HashedList<Vector<Rational> > GAPPosRootSystem;
    if (this->flagPrintGAPinput && this->owner->theWeyl.LoadGAPRootSystem(GAPPosRootSystem))
    { out << " The roots needed to generate the root subsystems are listed below using the root indices in GAP. ";
      for (int i = 0; i < this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size; i ++)
      { rootSubalgebra& currentSA= this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither[i];
        out << "<br>";
        out << "[";
        if (i < this->NumParabolic)
          out << "\"parabolic\",";
        else if (i < this->NumParabolic+this->NumPseudoParabolicNonParabolic)
          out << "\"pseudoParabolicNonParabolic\",";
        else
          out << "\"nonPseudoParabolic\",";
        out << "\"" << currentSA.theDynkinType.ToString() << "\", ";
        out << "[";
        for (int j = 0; j<currentSA.SimpleBasisK.size; j ++)
        { int theIndex= GAPPosRootSystem.GetIndex(currentSA.SimpleBasisK[j]);
          if (theIndex== - 1)
            theIndex= GAPPosRootSystem.GetIndex(-currentSA.SimpleBasisK[j]);
          out << theIndex+ 1;
          if (j!=currentSA.SimpleBasisK.size- 1)
            out << ", ";
        }
        out << "]]";
        if (i!= this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size- 1)
          out << ",";
      }
    }
    out << "<hr>The roots needed to generate the root subsystems are listed below. ";
    for (int i = 0; i < this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size; i ++)
    { rootSubalgebra& currentSA= this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither[i];
      out << "<br>";
      out << "[";
      if (i < this->NumParabolic)
        out << "\"parabolic\",";
      else if (i < this->NumParabolic+this->NumPseudoParabolicNonParabolic)
        out << "\"pseudoParabolicNonParabolic\",";
      else
        out << "\"nonPseudoParabolic\",";
      out << "\"" << currentSA.theDynkinType.ToString() << "\", ";
      out << "[";
      for (int j = 0; j<currentSA.SimpleBasisK.size; j ++)
      { out << "[";
        for (int k= 0; k<currentSA.SimpleBasisK[j].size; k++)
        { out << currentSA.SimpleBasisK[j][k].ToString();
          if (k!=currentSA.SimpleBasisK[j].size- 1)
            out << ", ";
        }
        out << "]";
        if (j!=currentSA.SimpleBasisK.size- 1)
          out << ", ";
      }
      out << "]]";
      if (i!= this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size- 1)
        out << ",";
    }
  }
  return out.str();
}

std::string rootSubalgebras::ToStringDynkinTableFormat2LaTeX(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("rootSubalgebras::ToStringDynkinTableFormat2LaTeX");
  std::stringstream out;
  std::string tempS, tempS2, tempS3;
  std::string endline="\n<br>";
  if (theFormat!= 0)
    theFormat->flagSupressDynkinIndexOne= true;
  out << "\\documentclass{article}" << endline
  << "\\usepackage{longtable, amssymb, lscape}" << endline
  << "\\begin{document}" << endline;
  out << "Lie algebra type: $" << this->theSubalgebras[0].theDynkinType.ToString(theFormat)
  << "$. There are " << this->theSubalgebras.size << " table entries (= " << this->theSubalgebras.size-2
  << " larger than the Cartan subalgebra + the Cartan subalgebra + the full subalgebra)." << endline
  << "Let $\\mathfrak g$ stand for the type of the regular subalgebra and $C(\\mathfrak g)$ for the type of the centralizer."
  << " Let $r$ stand for the rank of $\\mathfrak g$, let $r_c$ stand for the rank of the semisimple part of the centralizer,"
  << " let $p$ stand for the number of positive roots of $\\mathfrak g$, "
  << "let $q$ stand for the number of positive roots of the centralizer, and let $m$ stand for the number of "
  << " $A_1$ components (of all root lengths) of $\\mathfrak g$. ";
//  out << "\\begin{landscape}" << endline;
  out << "\\begin{longtable}{cccccccc}" << endline;
  out << "$\\mathfrak g$ & $C(\\mathfrak g)$& $p$ & $q$&  $m$& $r$ & $c_r$ \\\\\\endhead" << endline;
  for (int i = 0; i < this->theSubalgebras.size; i ++)
  { rootSubalgebra& currentSA= this->theSubalgebras[i];
    out << "$" << currentSA.theDynkinType.ToString(theFormat) << "$&" ;
    out << "$" << currentSA.theCentralizerDynkinType.ToString(theFormat) << "$&" ;
    out << "$" << (currentSA.theDynkinType.GetRootSystemSize()/2) << "$&" ;
    out << "$" << (currentSA.theCentralizerDynkinType.GetRootSystemSize()/2)<< "$&" ;
    out << "$" << currentSA.theDynkinType.GetNumSimpleComponentsOfGivenRank(1) << "$&" ;
    out << "$" << currentSA.theDynkinType.GetRank() << "$&" ;
    out << "$" << currentSA.theCentralizerDynkinType.GetRank() << "$&" ;
    out << "\\\\" << endline;
/*    if (i == 0)
      out << "<b>(Full subalgebra)</b>";
    if (i == this->theSubalgebras.size- 1)
      out << "<b>(Cartan subalgebra)</b>";
    out << "\n<br>\nType C(k_{ss})_{ss}: " << this->theSubalgebras[i].theCentralizerDiagram.ToString();
    out << "</td>";*/
  }
  out << "\\end{longtable}" << endline;
//  out << "\\end{landscape}" << endline;
  out << "\\end{document}" << endline;
  return out.str();
}

void rootSubalgebras::ComputeKmodMultTables()
{ for (int i = 0; i < this->theSubalgebras.size; i ++)
    this->theSubalgebras[i].GenerateKmodMultTable(this->theSubalgebras[i].theMultTable, this->theSubalgebras[i].theOppositeKmods);
}

bool rootSubalgebras::CheckInitialization() const
{ if (this->owner == 0)
    crash << "This is a programming error: root subalgebras with non-initialized owner. " << crash;
  return true;
}

void rootSubalgebras::initOwnerMustBeNonZero()
{ MacroRegisterFunctionWithName("rootSubalgebras::initOwnerMustBeNonZero");
  this->CheckInitialization();
  this->theSubalgebras.SetSize(0);
  this->owner->theWeyl.ComputeRho(false);
}

int rootSubalgebras::GetIndexUpToEquivalenceByDiagramsAndDimensions(const rootSubalgebra& theSA)
{ MacroRegisterFunctionWithName("rootSubalgebras::GetIndexUpToEquivalenceByDiagramsAndDimensions");
  int result = - 1;
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    if (this->theSubalgebras[i].IsEquivalentToByDiagramsAndDimensions(theSA))
    { if (result!= - 1)
        crash << "Programming error: experimental code internal check failed. " << crash;
      result =i;

    }
  return result;
}

int rootSubalgebras::GetIndexSubalgebraIsomorphicTo(rootSubalgebra& input)
{ MacroRegisterFunctionWithName("rootSubalgebras::GetIndexSubalgebraIsomorphicTo");
  for (int i = 0; i < this->theSubalgebras.size; i ++)
    if (input.SimpleBasisK== this->theSubalgebras[i].SimpleBasisK)
      return i;
  return - 1;
}

void rootSubalgebras::ComputeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(List<rootSubalgebra>& bufferSAs, int RecursionDepth)
{ this->theSubalgebras.AddOnTop(bufferSAs[RecursionDepth- 1]);
  int currentAlgebraIndex= this->theSubalgebras.size- 1;
  if (RecursionDepth>=bufferSAs.size)
    bufferSAs.SetSize(bufferSAs.size +this->GetOwnerWeyl().CartanSymmetric.NumRows);
  bufferSAs[RecursionDepth].genK = bufferSAs[RecursionDepth- 1].genK;
  bufferSAs[RecursionDepth].ownEr = this;
  ProgressReport theReport;
  for (int k= 0; k<bufferSAs[RecursionDepth- 1].Modules.size; k++)
    if (bufferSAs[RecursionDepth- 1].HighestWeightsPrimalSimple[k].IsPositive())
    { bufferSAs[RecursionDepth].genK.AddOnTop(bufferSAs[RecursionDepth- 1].HighestWeightsPrimalSimple[k]);
      bufferSAs[RecursionDepth].ComputeDynkinDiagramKandCentralizer();
      std::stringstream out;
      out << "Included root " << k+ 1 << " out of " << bufferSAs[RecursionDepth- 1].Modules.size
      << " Total found SAs: " << this->theSubalgebras.size;
      theReport.Report(out.str());
      int indexSA= this->IndexSubalgebra(bufferSAs[RecursionDepth]);
      if (indexSA== - 1)
      { bufferSAs[RecursionDepth].ComputeEssentialS();
        this->theSubalgebras[currentAlgebraIndex].indicesSubalgebrasContainingK.AddOnTopNoRepetition(this->theSubalgebras.size);
        this->ComputeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(bufferSAs, RecursionDepth+ 1);
      } else
        this->theSubalgebras[currentAlgebraIndex].indicesSubalgebrasContainingK.AddOnTopNoRepetition(indexSA);
      bufferSAs[RecursionDepth].genK.RemoveIndexSwapWithLast(bufferSAs[RecursionDepth].genK.size- 1);
    }
}

void rootSubalgebras::MakeProgressReportAutomorphisms(SubgroupWeylGroupOLD& theSubgroup, rootSubalgebra& theRootSA)
{ std::stringstream out4, out1;
  out1 << "k_ss: " << theRootSA.theDynkinDiagram.ToString() << " C(k_ss): "
  << theRootSA.theCentralizerDiagram.ToString();
  out4 << "Num elements ";
  if (theSubgroup.truncated)
    out4 << "truncated ";
  out4 << "group preserving k: " << theSubgroup.size;
  ProgressReport theReport;
  theReport.Report(out1.str()+out4.str());
}

void rootSubalgebras::GenerateActionKintersectBIsos(rootSubalgebra& theRootSA)
{ Selection emptySel;
  emptySel.init(theRootSA.SimpleBasisCentralizerRoots.size);
  this->ComputeNormalizerOfCentralizerIntersectNilradical(emptySel, theRootSA);
}

void rootSubalgebras::GenerateKintersectBOuterIsos(rootSubalgebra& theRootSA)
{ Selection fullSel;
  fullSel.init(theRootSA.SimpleBasisCentralizerRoots.size);
  fullSel.incrementSelectionFixedCardinality(theRootSA.SimpleBasisCentralizerRoots.size);
  this->ComputeNormalizerOfCentralizerIntersectNilradical(fullSel, theRootSA);
}

void rootSubalgebras::ComputeActionNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, rootSubalgebra& theRootSA)
{ this->ComputeNormalizerOfCentralizerIntersectNilradical(SelectedBasisRoots, theRootSA);
  SubgroupWeylGroupOLD& theSubgroup = this->CentralizerIsomorphisms.LastObject();
  this->ActionsNormalizerCentralizerNilradical.SetSize(theSubgroup.size- 1);
  Vector<Rational> tempRoot;
  ProgressReport theReport;
  for (int i = 0; i <theSubgroup.size- 1; i ++)
  { this->ActionsNormalizerCentralizerNilradical[i].SetSize(theRootSA.Modules.size);
    for (int j = 0; j<theRootSA.Modules.size; j ++)
    { tempRoot =(theRootSA.HighestWeightsPrimalSimple[j]);
      theSubgroup.ActByElement(i+ 1, tempRoot);
      int tempI= theRootSA.GetIndexKmoduleContainingRoot(tempRoot);
      this->ActionsNormalizerCentralizerNilradical[i][j]= tempI;
    //  for (int k= 0; k<theRootSA.kModules.TheObjects[j].size; k++)
    //  { tempRoot.Assign(theRootSA.kModules.TheObjects[j].TheObjects[k]);
    //    theSubgroup.ActByElement(i+ 1, tempRoot);
    //    if (tempI!= theRootSA.GetIndexKmoduleContainingRoot(tempRoot))crash << crash;
    //  }
    }
    if (theGlobalVariables.IndicatorStringOutputFunction!= 0)
    { std::stringstream out;
      out << "Computing action of element " << i+ 1 << " out of " << theSubgroup.size;
      theReport.Report(out.str());
    }
  }
}

void rootSubalgebras::ComputeNormalizerOfCentralizerIntersectNilradical
(Selection& SelectedBasisRoots, rootSubalgebra& theRootSA)
{ Vectors<Rational> selectedRootsBasisCentralizer;
  selectedRootsBasisCentralizer.size= 0;
  for (int i = 0; i <SelectedBasisRoots.MaxSize; i ++)
    if (!SelectedBasisRoots.selected[i])
      selectedRootsBasisCentralizer.AddOnTop(theRootSA.SimpleBasisCentralizerRoots[i]);
  this->CentralizerIsomorphisms.Reserve(this->theSubalgebras.size);
  this->CentralizerOuterIsomorphisms.Reserve(this->theSubalgebras.size);

  this->CentralizerIsomorphisms.SetSize(this->CentralizerIsomorphisms.size + 1);
  this->CentralizerOuterIsomorphisms.SetSize(this->CentralizerIsomorphisms.size);
  SubgroupWeylGroupOLD& outputSubgroup = this->CentralizerIsomorphisms.LastObject();

  outputSubgroup.AmbientWeyl =(&theRootSA.GetAmbientWeyl());
  this->MakeProgressReportAutomorphisms(outputSubgroup, theRootSA);
  theRootSA.GenerateIsomorphismsPreservingBorel(theRootSA, &outputSubgroup);
  //std::string tempS;
  //theSubgroup.ToString(tempS);
  //theGlobalVariables.theIndicatorVariables.StatusString1= tempS;
  //theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh= true;
  //theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  outputSubgroup.ComputeSubGroupFromGeneratingReflections
  (&selectedRootsBasisCentralizer, &outputSubgroup.ExternalAutomorphisms, this->UpperLimitNumElementsWeyl, false);
  outputSubgroup.simpleGenerators =(selectedRootsBasisCentralizer);
  this->CentralizerOuterIsomorphisms.LastObject().ExternalAutomorphisms =(outputSubgroup.ExternalAutomorphisms);
  this->CentralizerOuterIsomorphisms.LastObject().AmbientWeyl =(&this->GetOwnerWeyl());
  this->MakeProgressReportAutomorphisms(outputSubgroup, theRootSA);
  //theSubgroup.ComputeDebugString();
}

rootSubalgebras::rootSubalgebras()
{ this->flagNilradicalComputationInitialized= false;
  this->flagStoringNilradicals = false;
  this->flagUsingParabolicsInCentralizers = true;
  this->flagCountingNilradicalsOnlyNoComputation = false;
  this->flagComputingLprohibitingWeights = false;
  this->flagComputeConeCondition = false;
  this->flagUsingActionsNormalizerCentralizerNilradical = true;
  this->flagLookingForMinimalRels = false;
  this->NumLinesPerTableLatex=20;
  this->NumColsPerTableLatex=4;
  this->UpperLimitNumElementsWeyl = 0;
  this->owner = 0;
  this->flagPrintGAPinput = false;
  this->flagPrintParabolicPseudoParabolicInfo= false;
  this->initForNilradicalGeneration();
  this->NumNonPseudoParabolic = 0;
  this->NumParabolic = 0;
  this->NumPseudoParabolicNonParabolic = 0;
}

void rootSubalgebras::initForNilradicalGeneration()
{ this->NumSubalgebrasProcessed= 0;
  this->NumConeConditionFailures = 0;
  this->NumSubalgebrasCounted= 0;
  this->IndexCurrentSANilradicalsGeneration = 0;
  this->ReportStringNonNilradicalParabolic ="";
  this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration = this->theSubalgebras.size- 1;
  if (this->theSubalgebras.size>0)
  { this->theSubalgebras[0].GeneratePossibleNilradicalsInit(this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration);
    this->NumConeConditionHoldsBySSpart.initFillInObject(this->theSubalgebras.size, 0);
  }
}

bool rootSubalgebras::ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel)
{ if (!this->flagUsingActionsNormalizerCentralizerNilradical)
    return true;
  for (int i = 0; i < this->ActionsNormalizerCentralizerNilradical.size; i ++)
    if (!this->ApproveSelAgainstOneGenerator(this->ActionsNormalizerCentralizerNilradical[i], targetSel))
      return false;
  return true;
}

void rootSubalgebras::RaiseSelectionUntilApproval(Selection& targetSel)
{ bool raised= true;
  while (raised)
  { raised= false;
    for (int i = 0; i < this->ActionsNormalizerCentralizerNilradical.size; i ++)
      if (!this->ApproveSelAgainstOneGenerator(this->ActionsNormalizerCentralizerNilradical[i], targetSel))
      { this->ApplyOneGenerator(this->ActionsNormalizerCentralizerNilradical[i], targetSel);
        raised= true;
      }
  }
}

void rootSubalgebras::ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm)
{ Vectors<Rational> tempRoots, tempRoots2;
  std::stringstream out;
  std::stringstream out2;
  std::string tempS;
  int numNonSolvableNonReductive= 0;
  char simpleType;
  int theRank;
  if (!this->GetOwnerWeyl().theDynkinType.IsSimple(&simpleType, &theRank))
    crash << "This is a programming error: ElementToStringConeConditionNotSatisfying called on a non-simple Lie algebra. " << crash;
  if (simpleType=='B')
    out << "$\\mathrm{so}(2n + 1)$ is realized as a matrix Lie algebra as $\\left\\{\\left(\\begin{array}{c|c|c}A&\\begin{array}{c}v_1\\\\ \\vdots \\\\ v_n\\end{array} &C= -C^T \\\\\\hline \\begin{array}{ccc}w_1 &\\dots&  w_n\\end{array} &0& \\begin{array}{ccc}-v_n &\\dots&  -v_1\\end{array} \\\\\\hline D= -D^T&\\begin{array}{c}-w_n\\\\ \\vdots \\\\ -w_1\\end{array} & -A^T\\end{array}\\right)\\right\\}$.\n\n";
  if (simpleType=='C')
    out << "$\\mathrm{sp}(2n)$ is realized as a matrix Lie algebra as $\\left\\{\\left(\\begin{array}{c|c}A& C \\\\\\hline D& -A^T\\end{array}\\right)| C=C^T, D=D^T\\right\\}.$";
  out << " In this realization, the Cartan subalgebra $\\mathfrak{h}$ can be chosen to consist of the diagonal matrices of the above form.\n\n";
  if (!includeMatrixForm)
  { out << "\n\\begin{longtable}{r|l}\n\\multicolumn{2}{c}{";
    if (simpleType=='B')
      out << " $ \\mathfrak{g}\\simeq \\mathrm{so(" << this->GetOwnerWeyl().CartanSymmetric.NumRows*2+ 1 << ")}$";
    if (simpleType=='C')
      out << " $\\mathfrak{g}\\simeq \\mathrm{sp(" << this->GetOwnerWeyl().CartanSymmetric.NumRows*2 << ")}$";
    out << "} \\\\\\hline";
  }
  for (int i = 0; i < this->theSubalgebras.size- 1; i ++)
    if (this->storedNilradicals[i].size>0)
    { rootSubalgebra& currentRootSA= this->theSubalgebras[i];
      tempRoots.size= 0;
      for (int j = 0; j<currentRootSA.PositiveRootsK.size; j ++)
      { tempRoots.AddOnTop(currentRootSA.PositiveRootsK[j]);
        tempRoots.AddOnTop(-currentRootSA.PositiveRootsK[j]);
      }
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{1.5pt}\n\n";
      else
        out << "\\hline\\begin{tabular}{r}";
      out << "$\\Delta(\\mathfrak{k})$ is of type "
      << currentRootSA.theDynkinDiagram.ToString() << "; ";
      if (!includeMatrixForm)
        out << "\\\\";
      currentRootSA.GetAmbientWeyl().GetEpsilonCoords(currentRootSA.PositiveRootsK, tempRoots2);
      tempS= tempRoots2.ElementToStringEpsilonForm(true, false, false);
      out << " $\\Delta^+(\\mathfrak{k})=$ " << tempS;
      if (includeMatrixForm)
        out << "\n\n\\noindent\\rule{\\textwidth}{0.3pt}\n\n";
      else
        out << "\\end{tabular} &\n\\begin{tabular}{l}";
      int numNonReductiveCurrent = 0;
      for (int j = 0; j< this->storedNilradicals[i].size; j ++)
      { List<int>& currentNilrad= this->storedNilradicals[i][j];
        if (currentNilrad.size>0)
        { numNonSolvableNonReductive ++;
          numNonReductiveCurrent++;
          tempRoots.size= currentRootSA.PositiveRootsK.size*2;
          for (int k= 0; k<currentNilrad.size; k++)
            tempRoots.AddListOnTop(currentRootSA.WeightsModulesPrimalSimple[currentNilrad[k]]);
          this->ElementToStringRootSpaces(tempS, includeMatrixForm, tempRoots);
          out << tempS << "\n";
          if (numNonReductiveCurrent%2== 0)
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
  out2 << "\n\nThe number of non-conjugate non-solvable non-reductive root subalgebras not satisfying the cone condition is: "
  << numNonSolvableNonReductive << "\n\n";
  tempS = out.str();
  out2 << tempS;
  output = out2.str();
}

void rootSubalgebras::ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input)
{ std::string tempS; std::stringstream out;
  Vectors<Rational> epsCoords;
  Matrix<int> tempMat;
  char simpleType;
  int theDimension;
  if (!this->GetOwnerWeyl().theDynkinType.IsSimple(&simpleType, &theDimension))
    crash << "This is a programming error: ElementToStringConeConditionNotSatisfying called on a non-simple Lie algebra. " << crash;
  if (simpleType=='B')
  { this->GetOwnerWeyl().GetEpsilonCoords(input, epsCoords);
    tempMat.MakeIdMatrix(theDimension*2+ 1, 1, 0);
    tempMat.elements[theDimension][theDimension]= 0;
    for (int i = 0; i <epsCoords.size; i ++)
    { bool isShort = false;
      int firstIndex= - 1;
      int secondIndex= - 1;
      bool firstSignIsPositive= true;
      bool secondSignIsPositive= true;
      Vector<Rational>& currentRoot =epsCoords[i];
      for (int j = 0; j<theDimension; j ++)
      { if (currentRoot[j]!= 0)
        { isShort =!isShort;
          if (isShort)
          { if (currentRoot[j].IsPositive())
              firstSignIsPositive= true;
            else
              firstSignIsPositive= false;
            firstIndex=j;
          } else
          { if (currentRoot[j].IsPositive())
              secondSignIsPositive= true;
            else
              secondSignIsPositive= false;
            secondIndex=j;
          }
        }
      }
      if (!isShort)
      { bool signsAreDifferent =(firstSignIsPositive!=secondSignIsPositive);
        if (signsAreDifferent)
        { int positiveIndex, negativeIndex;
          if (firstSignIsPositive)
          { positiveIndex= firstIndex;
            negativeIndex=secondIndex;
          } else
          { positiveIndex= secondIndex;
            negativeIndex= firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex]=1;
          tempMat.elements[theDimension + 1+negativeIndex][theDimension + 1+positiveIndex]= - 1;
        } else
        { if (firstSignIsPositive)
          { tempMat.elements[firstIndex][secondIndex+theDimension + 1]=1;
            tempMat.elements[secondIndex][firstIndex+theDimension + 1]= - 1;
          } else
          { tempMat.elements[theDimension + 1+firstIndex][secondIndex]=1;
            tempMat.elements[theDimension + 1+secondIndex][firstIndex]= - 1;
          }
        }
      } else
      { if (firstSignIsPositive)
        { tempMat.elements[firstIndex][theDimension]=1;
          tempMat.elements[theDimension][theDimension + 1+firstIndex]= - 1;
        } else
        { tempMat.elements[theDimension][firstIndex]=1;
          tempMat.elements[firstIndex+ 1+theDimension][theDimension]= - 1;
        }
      }
    }
  }
  if (simpleType=='C')
  { this->GetOwnerWeyl().GetEpsilonCoords(input, epsCoords);
    tempMat.MakeIdMatrix(theDimension*2, 1, 0);
    for (int i = 0; i <epsCoords.size; i ++)
    { bool isLong= false;
      int firstIndex= - 1;
      int secondIndex= - 1;
      bool firstSignIsPositive= true;
      bool secondSignIsPositive= true;
      Vector<Rational>& currentRoot =epsCoords[i];
      for (int j = 0; j<theDimension; j ++)
      { if (currentRoot[j]!= 0)
        { isLong=!isLong;
          if (isLong)
          { if (currentRoot[j].IsPositive())
              firstSignIsPositive= true;
            else
              firstSignIsPositive= false;
            firstIndex=j;
          } else
          { if (currentRoot[j].IsPositive())
              secondSignIsPositive= true;
            else
              secondSignIsPositive= false;
            secondIndex=j;
          }
        }
      }
      if (!isLong)
      { bool signsAreDifferent =(firstSignIsPositive!=secondSignIsPositive);
        if (signsAreDifferent)
        { int positiveIndex= - 1, negativeIndex= - 1;
          if (firstSignIsPositive)
          { positiveIndex= firstIndex;
            negativeIndex=secondIndex;
          } else
          { positiveIndex= secondIndex;
            negativeIndex= firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex]=1;
          tempMat.elements[theDimension +negativeIndex][theDimension +positiveIndex]= - 1;
        } else
        { if (firstSignIsPositive)
          { tempMat.elements[firstIndex][secondIndex+theDimension]=1;
            tempMat.elements[secondIndex][firstIndex+theDimension]=1;
          } else
          { tempMat.elements[theDimension +firstIndex][secondIndex]=1;
            tempMat.elements[theDimension +secondIndex][firstIndex]=1;
          }
        }
      } else
      { if (firstSignIsPositive)
          tempMat.elements[firstIndex][theDimension +firstIndex]=1;
        else
          tempMat.elements[theDimension +firstIndex][firstIndex]=1;
      }
    }
  }
  if (includeMatrixForm)
    out << "\\begin{tabular}{cc} \\begin{tabular}{l}";
  out << "$\\Delta(\\mathfrak{n})=$";
  if (includeMatrixForm)
    out <<"\\\\";
  int numNilradicalRootSpaces = 0;
  for (int i = 0; i <epsCoords.size; i ++)
  { Vector<Rational>& currentRoot =epsCoords.TheObjects[i];
    tempS=currentRoot.ToStringEpsilonFormat();
    if (!epsCoords.Contains(-currentRoot))
    { out << tempS << ", ";
      numNilradicalRootSpaces++;
    }
    if (includeMatrixForm)
      if (numNilradicalRootSpaces%2== 0 && numNilradicalRootSpaces!= 0)
        out << "\\\\";
  }
  if (includeMatrixForm)
  { out << "\\end{tabular} & $\\mathfrak{l}=\\left(\\begin{array}{";
    for (int i = 0; i <tempMat.NumCols; i ++)
    { out << "c";
      if (simpleType=='B' && (i == theDimension- 1 || i == theDimension))
        out << "|";
    }
    out << "}";
    for (int i = 0; i < tempMat.NumRows; i ++)
    { if (simpleType=='B' && (i == theDimension || i == theDimension + 1))
        out << "\\hline";
      for (int j = 0; j<tempMat.NumCols; j ++)
      { if (tempMat.elements[i][j]!= 0 && tempMat.elements[j][i]== 0)
          out << "\\blacktriangle";
        if (tempMat.elements[i][j]!= 0 && tempMat.elements[j][i]!= 0)
        out << "\\bullet";
        if (j!= tempMat.NumCols- 1)
          out << "&";
        else
          out << "\\\\";
      }
    }
    out << "\\end{array}\\right)$ \\end{tabular}  ";
  }
  output = out.str();
}

void rootSubalgebras::ApplyOneGenerator(List<int>& generator, Selection& targetSel)
{ Selection tempSel;
  tempSel.init(targetSel.MaxSize);
  for (int i = 0; i <targetSel.CardinalitySelection; i ++)
    tempSel.AddSelectionAppendNewIndex(generator[targetSel.elements[i]]);
  targetSel =(tempSel);
}

bool rootSubalgebras::ApproveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel)
{ Selection tempSel;
  tempSel.init(targetSel.MaxSize);
  for (int i = 0; i <targetSel.CardinalitySelection; i ++)
    tempSel.AddSelectionAppendNewIndex(generator[targetSel.elements[i]]);
  for (int i = 0; i <tempSel.MaxSize; i ++)
  { if (targetSel.selected[i] && !tempSel.selected[i])
      return true;
    if (!targetSel.selected[i] && tempSel.selected[i])
      return false;
  }
  return true;
}

int rootSubalgebras::IndexSubalgebra(rootSubalgebra& input)
{ for (int j = 0; j< this->theSubalgebras.size; j ++)
  { rootSubalgebra& right = this->theSubalgebras[j];
    if (input.theDynkinDiagram.ToString() == right.theDynkinDiagram.ToString() &&
        input.theCentralizerDiagram.ToString()==right.theCentralizerDiagram.ToString())
    { if (!this->GetOwnerWeyl().IsOfSimpleType('E', 7))
        return j;
      else
      { input.ComputeEssentialS();
        if (input.GenerateIsomorphismsPreservingBorel(right, 0))
          return j;
      }
    }
  }
  return - 1;
}

bool rootSubalgebras::IsANewSubalgebra(rootSubalgebra& input)
{ return this->IndexSubalgebra(input)== - 1;
}

void rootSubalgebra::ComputeRootsOfK()
{ MacroRegisterFunctionWithName("rootSubalgebra::ComputeRootsOfK");
  this->AllRootsK.Clear();
  HashedList<Vector<Rational> >& ambientRootSystem= this->GetAmbientWeyl().RootSystem;
  this->AllRootsK.SetExpectedSize(ambientRootSystem.size);
  Vector<Rational> currentRoot;
  this->AllRootsK.AddOnTop(this->SimpleBasisK);
  for (int i = 0; i < this->AllRootsK.size; i ++)
    for (int j = 0; j< this->SimpleBasisK.size; j ++)
    { currentRoot = this->AllRootsK[i]+this->SimpleBasisK[j];
      if (ambientRootSystem.Contains(currentRoot))
        this->AllRootsK.AddOnTopNoRepetition(currentRoot);
    }
  this->PositiveRootsK= this->AllRootsK;
  for (int i = 0; i < this->PositiveRootsK.size; i ++)
    this->AllRootsK.AddOnTop(-this->PositiveRootsK[i]);
  if (this->SimpleBasisK.size== 0)
    if (this->AllRootsK.size!= 0)
      crash << "Internal check went bad. " << crash;
}

void coneRelation::RelationOneSideToStringCoordForm(std::string& output, List<Rational>& coeffs, Vectors<Rational>& theRoots, bool EpsilonForm)
{ std::stringstream out;
  std::string tempS;
  for (int i = 0; i <theRoots.size; i ++)
  { tempS=coeffs[i].ToString();
    if (tempS=="1")
      tempS="";
    if (tempS=="- 1")
      tempS="-";
    if ((tempS=="0"))
      crash << crash;
    out<< tempS;
    if (!EpsilonForm)
      tempS= theRoots[i].ToString();
    else
      tempS= theRoots[i].ToStringEpsilonFormat();
    out << "(" << tempS << ")";
    if (i!= theRoots.size- 1)
      out << " + ";
  }
   output = out.str();
}

void coneRelation::RelationOneSideToString(std::string& output, const std::string& letterType, List<Rational>& coeffs, List<List<int> >& kComponents, Vectors<Rational>& theRoots, bool useLatex, rootSubalgebra& owner)
{ if (theRoots.size!=coeffs.size)
    crash << crash;
  std::stringstream out;
  std::string tempS;
  if (useLatex)
  { out << "\\begin{tabular}{";
    for (int i = 0; i <theRoots.size; i ++)
      out << "c";
    out << "}";
  }
  for (int i = 0; i <theRoots.size; i ++)
  { tempS=coeffs[i].ToString();
    if (tempS=="1")
      tempS="";
    if (tempS=="- 1")
      tempS="-";
    if ((tempS=="0"))
      crash << crash;
    out << tempS;
    if (!useLatex)
    { tempS= theRoots[i].ToString();
      out << "(" << tempS << ")";
      if (i!= theRoots.size- 1)
        out << " + ";
    } else
    { out << "$" << letterType << "_" << i+ 1 << "$";
      if (i!= theRoots.size- 1)
        out << "+ & ";
    }
  }
  if (useLatex)
    out << "\\\\";
  List<int> TakenIndices;
  List<int> NumPrimesUniTypicComponent;
  TakenIndices.initFillInObject(owner.theDynkinDiagram.SimpleBasesConnectedComponents.size, - 1);
  NumPrimesUniTypicComponent.initFillInObject(owner.theDynkinDiagram.sameTypeComponents.size, - 1);
  for (int i = 0; i <kComponents.size; i ++)
  { if (useLatex)
      out << "\\tiny{ ";
    for (int j = 0; j<kComponents[i].size; j ++)
    { int index= kComponents[i][j];
      int indexUniComponent = owner.theDynkinDiagram.indexUniComponent[index];
      out << owner.theDynkinDiagram.SimpleComponentTypes[index].ToString();
      if (TakenIndices[index]== - 1)
      { NumPrimesUniTypicComponent[indexUniComponent]++;
        TakenIndices[index]=NumPrimesUniTypicComponent[indexUniComponent];
      }
      for (int k= 0; k<TakenIndices[index]; k++)
        out << "'";
      if (j!=kComponents[i].size- 1)
        out << "+";
    }
    if (useLatex)
    { out << " }";
      if (i!=kComponents.size- 1)
        out << " & ";
    }
  }
  if (useLatex)
    out << "\\end{tabular}";
  output = out.str();
}

int coneRelation::ToString(std::string& output, rootSubalgebras& owners, bool useLatex, bool includeScalarsProductsEachSide, bool includeMixedScalarProducts)
{ std::string tempS;
  std::stringstream out;
  if (this->AlphaCoeffs.size!= this->Alphas.size || this->BetaCoeffs.size!= this->Betas.size)
    crash << crash;
  int LatexLineCounter = 0;
  this->ComputeConnectedComponents(this->Alphas, owners.theSubalgebras[this->IndexOwnerRootSubalgebra],  this->AlphaKComponents);
  this->ComputeConnectedComponents(this->Betas, owners.theSubalgebras[this->IndexOwnerRootSubalgebra], this->BetaKComponents);
  this->RelationOneSideToString(tempS, "\\alpha", this->AlphaCoeffs, this->AlphaKComponents, this->Alphas, useLatex, owners.theSubalgebras[this->IndexOwnerRootSubalgebra]);
  out << tempS;
  if (useLatex)
    out << " &\\begin{tabular}{c} ";
  out << "=";
  if (useLatex)
    out << " \\\\~ \\end{tabular} & ";
  this->RelationOneSideToString(tempS, "\\beta", this->BetaCoeffs, this->BetaKComponents, this->Betas, useLatex, owners.theSubalgebras[this->IndexOwnerRootSubalgebra]);
  out << tempS;
  if (useLatex)
    out << " & ";
  this->theDiagram.ToString();
  out << tempS;
  this->theDiagramRelAndK.ToString();
  if (useLatex)
    out << " & ";
  out << tempS;
  if (includeScalarsProductsEachSide)
  { out << " & ";
    LatexLineCounter+= this->RootsToScalarProductString(this->Alphas, this->Alphas, "\\alpha", "\\alpha", tempS, owners.theSubalgebras[this->IndexOwnerRootSubalgebra]);
    out << tempS;
    LatexLineCounter+= this->RootsToScalarProductString(this->Betas, this->Betas, "\\beta", "\\beta", tempS, owners.theSubalgebras[this->IndexOwnerRootSubalgebra]);
    out << " " << tempS;
  }
  if (includeMixedScalarProducts)
  { LatexLineCounter+= this->RootsToScalarProductString(this->Alphas, this->Betas, "\\alpha", "\\beta", tempS, owners.theSubalgebras[this->IndexOwnerRootSubalgebra]);
    out << tempS;
  }
  out << "\n";
  output = out.str();
  return MathRoutines::Maximum(2, LatexLineCounter);
}

int coneRelation::RootsToScalarProductString(Vectors<Rational>& inputLeft, Vectors<Rational>& inputRight, const std::string& letterTypeLeft, const std::string& letterTypeRight, std::string& output, rootSubalgebra& owner)
{ std::string tempS; std::stringstream out;
  int numLinesLatex= 0;
  Rational tempRat;
  for (int i = 0; i <inputLeft.size; i ++)
    for (int j = 0; j<inputRight.size; j ++)
      if (i <j || letterTypeLeft!=letterTypeRight)
      { owner.GetAmbientWeyl().RootScalarCartanRoot(inputLeft[i], inputRight[j], tempRat);
        if (!tempRat.IsEqualToZero())
        { tempS= tempRat.ToString();
          out << "$\\langle" << letterTypeLeft << "_" << i+ 1 << ", " << letterTypeRight << "_" << j+ 1 << "\\rangle=" << tempS << "$, ";
          numLinesLatex++;
        }
      }
  output = out.str();
  return numLinesLatex;
}

void coneRelation::ComputeConnectedComponents(Vectors<Rational>& input, rootSubalgebra& owner, List<List<int> >& output)
{ output.SetSize(input.size);
  for (int i = 0; i <input.size; i ++)
  { output[i].size= 0;
    for (int j = 0; j<owner.theDynkinDiagram.SimpleBasesConnectedComponents.size; j ++)
      if (owner.theDynkinDiagram.SimpleBasesConnectedComponents[j].
          ContainsARootNonPerpendicularTo(input[i], owner.GetAmbientWeyl().CartanSymmetric))
        output[i].AddOnTop(j);
  }
}

bool coneRelation::IsStrictlyWeaklyProhibiting(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots, rootSubalgebras& owners, int indexInOwner)
{ Vectors<Rational> tempRoots;
  tempRoots =(this->Alphas);
  tempRoots.AddListOnTop(this->Betas);
  tempRoots.AddListOnTop(owner.genK);
  //owner.AmbientWeyl.TransformToSimpleBasisGenerators(tempRoots);
  this->theDiagram.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
  if (this->theDiagram.ToString()=="F^{1}_4")
    return false;
  if (this->theDiagram.SimpleComponentTypes[0].theLetter =='A' && this->theDiagram.SimpleComponentTypes[0].theRank==1)
  {//  crash << crash;
  }
  SubgroupWeylGroupOLD tempSubgroup;
  tempSubgroup.AmbientWeyl = &(owner.GetAmbientWeyl());
  tempSubgroup.ComputeSubGroupFromGeneratingReflections(&tempRoots, &tempSubgroup.ExternalAutomorphisms, 0, true);
  Vectors<Rational> NilradicalIntersection, genSingHW;
  tempRoots =(tempSubgroup.RootSubsystem);
  NilradicalRoots.IntersectWith(tempRoots, NilradicalIntersection);
  for (int i = 0; i <owner.HighestWeightsPrimalSimple.size; i ++)
    if (!owner.NilradicalKmods.selected[i] && tempRoots.Contains(owner.HighestWeightsPrimalSimple[i]) && owner.IsGeneratingSingularVectors(i, NilradicalIntersection))
      genSingHW.AddOnTop(owner.HighestWeightsPrimalSimple[i]);
  if (owner.ConeConditionHolds(owners, indexInOwner, NilradicalIntersection, genSingHW, false))
    return false;
  if (!owner.AttemptTheTripleTrickWRTSubalgebra(*this, genSingHW, NilradicalIntersection))
    return false;
  this->SortRelation(owner);
  this->FixRepeatingRoots(this->Alphas, this->AlphaCoeffs);
  //this->FixRightHandSide(owner, NilradicalRoots);
  this->ComputeDiagramRelAndK(owner);
  return true;
}

void coneRelation::ComputeTheDiagramAndDiagramRelAndK(rootSubalgebra& owner)
{ Vectors<Rational> tempRoots;
  tempRoots =(this->Alphas);
  tempRoots.AddListOnTop(this->Betas);
  this->theDiagram.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
  this->ComputeDiagramRelAndK(owner);
}

void coneRelation::MakeLookCivilized(rootSubalgebra& owner)
{ if (this->Alphas.size== 0 || this->Betas.size== 0)
    return;
  Vectors<Rational> tempRoots;
//  this->Alphas.ComputeDebugString();
//  this->Betas.ComputeDebugString();
  tempRoots =(this->Alphas);
  tempRoots.AddListOnTop(this->Betas);
  //owner.AmbientWeyl.TransformToSimpleBasisGenerators(tempRoots);
  this->theDiagram.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
  if (this->theDiagram.SimpleComponentTypes[0].theLetter =='A' && this->theDiagram.SimpleComponentTypes[0].theRank==1)
  { this->ComputeDiagramRelAndK(owner);
    crash << crash;
  }
  this->SortRelation(owner);
  this->FixRepeatingRoots(this->Alphas, this->AlphaCoeffs);
  //this->FixRightHandSide(owner, NilradicalRoots);
  this->ComputeDiagramRelAndK(owner);
}

void coneRelation::FixRightHandSide(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots)
{ bool hasChanged= true;
  Vector<Rational> tempRoot;
  while (hasChanged)
  { hasChanged= false;
    for (int i = 0; i < this->Betas.size; i ++)
      for (int j =i+ 1; j< this->Betas.size; j ++)
      { tempRoot =(this->Betas[i]);
        tempRoot+=(this->Betas[j]);
        if (owner.GetAmbientWeyl().RootSystem.Contains(tempRoot))
        { int leavingIndex=j; int remainingIndex=i;
          if (this->BetaCoeffs[j].IsGreaterThan(this->BetaCoeffs[i]))
          { leavingIndex=i;
            remainingIndex=j;
          }
          this->Betas[leavingIndex]=(tempRoot);
          this->BetaCoeffs[remainingIndex].Subtract(this->BetaCoeffs[leavingIndex]);
          if (this->BetaCoeffs[remainingIndex].IsEqualToZero())
          { this->BetaCoeffs.RemoveIndexSwapWithLast(remainingIndex);
            this->Betas.RemoveIndexSwapWithLast(remainingIndex);
          }
          if (!NilradicalRoots.Contains(tempRoot))
            crash << crash;
          hasChanged= true;
        }
      }
  }
}

bool coneRelation::CheckForBugs(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots)
{ for (int i = 0; i < this->Alphas.size; i ++)
  { int tempI= owner.HighestWeightsPrimalSimple.GetIndex(this->Alphas[i]);
    if (tempI== - 1)
      return false;
    if (NilradicalRoots.Contains(this->Alphas[i]))
      return false;
  }
  for (int i = 0; i < this->Betas.size; i ++)
    if (!NilradicalRoots.Contains(this->Betas[i]))
      return false;
  return true;
}

void coneRelation::GetSumAlphas(Vector<Rational>& output, int theDimension)
{ if (this->AlphaCoeffs.size!= this->Alphas.size)
    crash << crash;
  output.MakeZero(theDimension);
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->Alphas.size; i ++)
  { tempRoot =(this->Alphas[i]);
    tempRoot*=(this->AlphaCoeffs.TheObjects[i]);
    output+=(tempRoot);
  }
}

void coneRelation::SortRelation(rootSubalgebra& owner)
{ this->ComputeKComponents(this->Alphas, this->AlphaKComponents, owner);
  this->ComputeKComponents(this->Betas, this->BetaKComponents, owner);
  //bubble sort
  for (int i = 0; i < this->Alphas.size; i ++)
    for (int j =i+ 1; j< this->Alphas.size; j ++)
      if (this->leftSortedBiggerThanOrEqualToRight(this->AlphaKComponents.TheObjects[j], this->AlphaKComponents.TheObjects[i]))
      { this->AlphaCoeffs.SwapTwoIndices(i, j);
        this->Alphas.SwapTwoIndices(i, j);
        this->AlphaKComponents.SwapTwoIndices(i, j);
      }
  for (int i = 0; i < this->Betas.size; i ++)
    for (int j =i+ 1; j< this->Betas.size; j ++)
      if (this->leftSortedBiggerThanOrEqualToRight(this->BetaKComponents.TheObjects[j], this->BetaKComponents.TheObjects[i]))
      { this->BetaCoeffs.SwapTwoIndices(i, j);
        this->Betas.SwapTwoIndices(i, j);
        this->BetaKComponents.SwapTwoIndices(i, j);
      }
}

void coneRelation::ComputeKComponents(Vectors<Rational>& input, List<List<int> >& output, rootSubalgebra& owner)
{ output.SetSize(input.size);
  for (int i = 0; i <input.size; i ++)
  { output[i].size= 0;
    for (int j = 0; j<owner.theDynkinDiagram.SimpleBasesConnectedComponents.size; j ++)
      if (owner.theDynkinDiagram.SimpleBasesConnectedComponents[j].ContainsARootNonPerpendicularTo
          (input[i], owner.GetAmbientWeyl().CartanSymmetric))
        output[i].AddOnTop(j);
  }
}

void coneRelation::ComputeDiagramRelAndK(rootSubalgebra& owner)
{ Vectors<Rational> tempRoots;
  tempRoots.size= 0;
  tempRoots.Reserve(owner.GetAmbientWeyl().CartanSymmetric.NumRows*2);
  tempRoots.AddListOnTop(owner.SimpleBasisK);
  for (int i = 0; i < this->theDiagram.SimpleBasesConnectedComponents.size; i ++)
    tempRoots.AddListOnTop(this->theDiagram.SimpleBasesConnectedComponents[i]);
  //owner.AmbientWeyl.TransformToSimpleBasisGenerators(tempRoots);
  this->theDiagramRelAndK.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
}

void coneRelation::FixRepeatingRoots(Vectors<Rational>& theRoots, List<Rational>& coeffs)
{ for (int i = 0; i <theRoots.size; i ++)
  { if (coeffs[i].IsEqualToZero())
    { coeffs.RemoveIndexSwapWithLast(i);
      theRoots.RemoveIndexSwapWithLast(i);
      i--;
    }
    for (int j =i+ 1; j<theRoots.size; j ++)
      if (theRoots[i]==(theRoots[j]))
      { coeffs[i]+=(coeffs[j]);
        theRoots.RemoveIndexSwapWithLast(j);
        coeffs.RemoveIndexSwapWithLast(j);
        j--;
      }
  }
}

bool coneRelation::leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right)
{ if (left.size>right.size)
    return true;
  if (right.size>left.size)
    return false;
  for (int i = 0; i <right.size; i ++)
  { if (right.TheObjects[i]>left.TheObjects[i])
      return false;
    if (left.TheObjects[i]>right.TheObjects[i])
      return true;
  }
  return true;
}

void coneRelation::WriteToFile(std::fstream& output)
{ this->AlphaCoeffs.WriteToFile(output);
  this->Alphas.WriteToFile(output);
  output << this->AlphaKComponents;
  this->BetaCoeffs.WriteToFile(output);
  this->Betas.WriteToFile(output);
  output << this->BetaKComponents;
  output << "Index_owner_root_SA: " << this->IndexOwnerRootSubalgebra << " ";
}

void coneRelation::ReadFromFile(std::fstream& input, rootSubalgebras& owner)
{ std::string tempS;
  this->AlphaCoeffs.ReadFromFile(input);
  this->Alphas.ReadFromFile(input);
  input >> this->AlphaKComponents;
  this->BetaCoeffs.ReadFromFile(input);
  this->Betas.ReadFromFile(input);
  input >> this->BetaKComponents;
  input >> tempS >> this->IndexOwnerRootSubalgebra;
  if (tempS!="Index_owner_root_SA:")
    crash << crash;
  this->ComputeTheDiagramAndDiagramRelAndK(owner.theSubalgebras[this->IndexOwnerRootSubalgebra]);
  this->ComputeDebugString(owner, true, true);
}

bool coneRelation::GenerateAutomorphisms(coneRelation& right)
{ /*if (this->DebugString.length()!=right.DebugString.length())
    return false;
  for (unsigned int i = 0; i < this->DebugString.length(); i ++)
  {  if (this->DebugString[i]!=right.DebugString[i])
      return false;
  }*/
  if (this->DebugString!=right.DebugString)
    return false;
  return true;
}

void coneRelations::AddRelationNoRepetition(coneRelation& input, rootSubalgebras& owners)
{ input.ComputeDebugString(owners, true, true);
  int i = this->GetHash(input);
  List<int>& theIndices = this->TheHashedArrays[i];
  for (int j = 0; j<theIndices.size; j ++)
    if (this->TheObjects[theIndices[j]].GenerateAutomorphisms(input))
      return;
  if (!this->flagIncludeSmallerRelations)
    if (input.theDiagramRelAndK.ToString()!= owners.theSubalgebras[0].theDynkinDiagram.ToString())
      return;
  this->AddOnTop(input);
  if (this->flagIncludeCoordinateRepresentation)
  {
  }
}

void coneRelations::GetLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter)
{ outputHeader.clear();
  outputFooter.clear();
  outputHeader.append("\\begin{tabular}{rcl p{1cm}p{1cm}p{3cm} } \\multicolumn{3}{c}");
  outputHeader.append("{ Relation / linked $\\mathfrak{k}$-components}");
  outputHeader.append(" &$\\alpha_i$'s, $\\beta_i$'s generate & adding $\\mathfrak{k}$ generates&");
  outputHeader.append("Non-zero scalar products\\\\");
  outputFooter.append("\\end{tabular}");
}

void coneRelations::ToString
(std::string& output, rootSubalgebras& owners, bool useLatex)
{ std::stringstream out;
  std::string tempS, header, footer;
  Vectors<Rational> tempAlphas, tempBetas;
  this->GetLatexHeaderAndFooter(header, footer);
  if (useLatex)
    out << header;
  int oldIndex= - 1;
  int lineCounter = 0;
  for (int i = 0; i < this->size; i ++)
  { if (oldIndex!= this->TheObjects[i].IndexOwnerRootSubalgebra)
    { oldIndex= this->TheObjects[i].IndexOwnerRootSubalgebra;
      if (useLatex)
        out << "\\hline\\multicolumn{5}{c}{$\\mathfrak{k}$-semisimple type: "
        << owners.theSubalgebras[oldIndex].theDynkinDiagram.ToString()
        << "}\\\\\n\\hline\\hline";
      //if (useHtml)
      //{ out << "<table>" << "<tr>"<< owners.TheObjects[oldIndex].theDynkinDiagram.DebugString
       //     <<"</tr>";
      //}
    }
    lineCounter+= this->TheObjects[i].ToString(tempS, owners, useLatex, true, true);
    out << tempS;
    if (useLatex)
      out << "\\\\";
    out << "\n";
    if (this->flagIncludeCoordinateRepresentation)
    { lineCounter+=2;
      out << "\\multicolumn{5}{c}{$\\varepsilon$-form~relative~to~the~subalgebra~generated~by~$\\mathfrak{k}$~and~the~relation}\\\\\n";
      // (*this)[i].GetEpsilonCoords(this->TheObjects[i].Alphas, tempAlphas, owners.GetOwnerWeyl());
      //this->TheObjects[i].GetEpsilonCoords(this->TheObjects[i].Betas, tempBetas, owners.GetOwnerWeyl());
      this->TheObjects[i].RelationOneSideToStringCoordForm(tempS, this->TheObjects[i].AlphaCoeffs, tempAlphas, true);
      out << "\\multicolumn{5}{c}{" << tempS;
      this->TheObjects[i].RelationOneSideToStringCoordForm(tempS, this->TheObjects[i].BetaCoeffs, tempBetas, true);
      out << "=" << tempS; //<<"~~~~";
    //  this->TheObjects[i].RelationOneSideToStringCoordForm
    //    (tempS, this->TheObjects[i].AlphaCoeffs, this->TheObjects[i].Alphas, false);
    //  out <<tempS;
    //  this->TheObjects[i].RelationOneSideToStringCoordForm
    //    (tempS, this->TheObjects[i].BetaCoeffs, this->TheObjects[i].Betas, false);
    //  out <<"="<<tempS<<"}\\\\\n";
      out << "}\\\\\\hline\n";
    }
    if (lineCounter>this->NumAllowedLatexLines)
    { out << footer << "\n\n\n" << header;
      lineCounter = 0;
    }
  }
  if (useLatex)
    out << footer;
  if (this->flagIncludeSubalgebraDataInDebugString)
  { FormatExpressions tempFormat;
    out << "\n\n\\newpage" << owners.ToString(&tempFormat);
  }
  output = out.str();
}

void coneRelations::WriteToFile(std::fstream& output)
{ output << "num_rels: " << this->size << "\n";
  for (int i = 0; i < this->size; i ++)
    this->TheObjects[i].WriteToFile(output);
}

void coneRelations::ReadFromFile(std::fstream& input, rootSubalgebras& owner)
{ std::string tempS; int tempI;
  this->Clear();
  input >> tempS >> tempI;
  coneRelation tempRel;
  for (int i = 0; i <tempI; i ++)
  { tempRel.ReadFromFile(input, owner);
    this->AddRelationNoRepetition(tempRel, owner);
  }
}

