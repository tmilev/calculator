//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf5cpp(__FILE__, "Implementation file for the calculator parser part 2: meant for built-in functions. ");

//If you get a the C++ multiple definition shit error:
//1) It's not your fault. C++ is idiotic.
//2) To fix it:
//- Try moving template *EXPLICIT* specializations (i.e. template <>)
//  to the *beginning* (!) of .cpp files.
//- Try moving template generics into .h files.
//- Write at least one angry email to the highest C++ authority you can get an email hold of.
//  They have to be aware of the great anger and pain they are causing to people smarter than them.

template <>
bool ReflectionSubgroupWeylGroup::IsDominantWRTgenerator<RationalFunction>(const Vector<RationalFunction>& theWeight, int generatorIndex)
{ Vector<RationalFunction> tempVect;
  RationalFunction tempRF;
  tempVect=this->simpleGenerators[generatorIndex].GetVectorRational();
  tempRF=this->AmbientWeyl.RootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType!=tempRF.typeRational)
  { std::cout << "This might or might not be a programming mistake: I am being asked whether a weight"
    << " with rational function coefficients is dominant. I took the scalar products with the positive simple roots "
    << " whose reflections generate the ambient group, however one of the scalar products in question was non-constant. "
    << " More precisely, the scalar product of " << theWeight.ElementToString() << " and " << tempVect.ElementToString()
    << " equals " << tempRF.ElementToString() << ". I cannot decide (more precisely, do not want to *silently* decide for you) "
    << " whether a non-constant function is positive or not. "
    << " If this is not a programming mistake, you might want to consider introducing a substitution "
    << " evaluating the rational function, some sort of a monomial order, or some other method of deciding the \"sign\" of a rational function."
    << " If you want to do that, you need to edit file "
    << CGI::GetHtmlLinkFromFileName(__FILE__) << " line " << __LINE__ << ". ";
    assert(false);
    return false;
  }
  return !tempRF.ratValue.IsNegative();
}

template <>
bool ReflectionSubgroupWeylGroup::IsDominantWRTgenerator<Rational>(const Vector<Rational>& theWeight, int generatorIndex)
{ return !this->AmbientWeyl.RootScalarCartanRoot
  (theWeight, this->simpleGenerators[generatorIndex]).IsNegative();
}

template <>
bool WeylGroup::IsDominantWRTgenerator<RationalFunction>(const Vector<RationalFunction>& theWeight, int generatorIndex)
{ Vector<Rational> tempVect;
  RationalFunction tempRF;
  tempVect.MakeEi(this->GetDim(), generatorIndex);
  tempRF=this->RootScalarCartanRoot(theWeight, tempVect);
  if (tempRF.expressionType!=tempRF.typeRational)
  { std::cout << "This might or might not be a programming mistake: I am being asked whether a weight"
    << " with rational function coefficients is dominant. I took the scalar products with the positive simple roots "
    << " whose reflections generate the ambient group, however one of the scalar products in question was non-constant. "
    << " More precisely, the scalar product of " << theWeight.ElementToString() << " and " << tempVect.ElementToString()
    << " equals " << tempRF.ElementToString() << ". I cannot decide (more precisely, do not want to *silently* decide for you) "
    << " whether a non-constant function is positive or not. "
    << " If this is not a programming mistake, you might want to consider introducing a substitution "
    << " evaluating the rational function, some sort of a monomial order, or some other method of deciding the \"sign\" of a rational function."
    << " If you want to do that, you need to edit file "
    << CGI::GetHtmlLinkFromFileName(__FILE__) << " line " << __LINE__ << ". ";
    assert(false);
    return false;
  }
  return !tempRF.ratValue.IsNegative();
}

template <>
bool WeylGroup::IsDominantWRTgenerator<Rational>(const Vector<Rational>& theWeight, int generatorIndex)
{ return !this->GetScalarProdSimpleRoot(theWeight, generatorIndex).IsNegative();
}

void ReflectionSubgroupWeylGroup::MakeParabolicFromSelectionSimpleRoots
(WeylGroup& inputWeyl, const Vector<Rational> & ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements)
{ Selection tempSel;
  tempSel=ZeroesMeanSimpleRootSpaceIsInParabolic;
  this->MakeParabolicFromSelectionSimpleRoots(inputWeyl, tempSel, theGlobalVariables, UpperLimitNumElements);
}

bool ReflectionSubgroupWeylGroup::GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra
(Vector<Rational>& highestWeightSimpleCoords, HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
    return false;
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  int numPosRoots=this->AmbientWeyl.RootsOfBorel.size;
  Vector<Rational> currentWeight, currentWeightRaisedToDominantWRTAmbientAlgebra;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<Rational> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->AmbientWeyl.RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { currentWeightRaisedToDominantWRTAmbientAlgebra=currentWeight;
          this->AmbientWeyl.RaiseToDominantWeight(currentWeightRaisedToDominantWRTAmbientAlgebra);
          currentWeightRaisedToDominantWRTAmbientAlgebra-=highestWeightTrue;
          if (currentWeightRaisedToDominantWRTAmbientAlgebra.IsNegativeOrZero())
          { int currentIndexShift=this->AmbientWeyl.RootsOfBorel[i].SumCoords().NumShort;
            currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
            if (outputWeightsByHeight[currentIndexShift].AddNoRepetition(currentWeight))
            { numTotalWeightsFound++;
              outputWeightsByHeight[currentIndexShift].AdjustHashes();
            }
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    std::cout << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound>=upperBoundDominantWeights)
    out << "<hr>This message is generated either because the number of weights has "
    << "exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter "
    << " is the case, make sure to send an angry email to the author(s).";
  outputDetails=out.str();
  //std::cout << "<hr><hr>Total time spent generating weights: " << -startTime+theGlobalVariables.GetElapsedSeconds();
  return (numTotalWeightsFound<=upperBoundDominantWeights);
}

bool CommandList::fWeylDimFormula
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ IncrementRecursion recursionCounter(theCommands);
  SemisimpleLieAlgebra* theSSowner=0;
  if (theExpression.children.size!=2)
  { theExpression.SetError("This function takes 2 arguments");
    return true;
  }
  Expression LieAlgebraNameNode=theExpression.children[0];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, LieAlgebraNameNode, comments))
  { theExpression.SetError("Failed to convert the first argument of "+theExpression.ElementToString()+ " to a semisimple Lie algebra. ");
    return true;
  }
  if (LieAlgebraNameNode.errorString!="")
  { theExpression.SetError("While trying to generate Lie algebra from the first argument of the Weyl dim formula, I got the error: " + LieAlgebraNameNode.errorString);
    return true;
  }
  theSSowner=&LieAlgebraNameNode.GetData().GetAmbientSSAlgebra();
  Vector<RationalFunction> theWeight;
  Context tempContext(theCommands);
  if (!theExpression.children[1].GetVector<RationalFunction>(theWeight, tempContext, theSSowner->GetRank(), theCommands.fPolynomial, comments))
  { theExpression.SetError("Failed to convert the argument of the function to a highest weight vector");
    return true;
  }
  int newContext=theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(tempContext);
  RationalFunction rfOne;
  rfOne.MakeOne(tempContext.VariableImages.size, theCommands.theGlobalVariableS);
  Vector<RationalFunction> theWeightInSimpleCoords;
  theWeightInSimpleCoords = theSSowner->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  //std::cout << "The fundamental coords: " << theWeight.ElementToString();
  if (comments!=0)
    *comments << "<br>Your input in simple coords: " << theWeightInSimpleCoords.ElementToString();
  RationalFunction tempRF= theSSowner->theWeyl.WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
  //std::cout << "<br>The result: " << tempRF.ElementToString();
  theExpression.MakeRF(tempRF, newContext, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::fRootSAsAndSltwos
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  Expression* zeTrueArgument=& theExpression;
  std::cout << "ze expression i get: " << theExpression.ElementToString() << "<hr>";
  std::cout << "which is of type " << theExpression.ElementToString();
  if (theExpression.theOperation==theCommands.opList() && theExpression.children.size>0)
  { zeTrueArgument=&theExpression.children[0];
    std::cout << "ZeTRueArgument: " << zeTrueArgument->ElementToString();
    //showIndicator=false;
  }
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, *zeTrueArgument, comments))
    return zeTrueArgument->SetError("Failed to generate semisimple Lie algebra from the argument you gave me");
  if (zeTrueArgument->errorString!="")
    return zeTrueArgument->SetError
    ("While attempting to create a semisimple Lie algebra I got the error message" +
     zeTrueArgument->errorString);
  CGI::SetCGIServerIgnoreUserAbort();
  std::stringstream outSltwoPath, outMainPath, outSltwoDisplayPath, outMainDisplayPath;
  theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds=10000;
  SemisimpleLieAlgebra& ownerSS=zeTrueArgument->GetData().GetAmbientSSAlgebra();
  char weylLetter=ownerSS.theWeyl.WeylLetter;
  int theRank=ownerSS.theWeyl.GetDim();

  outMainPath << theCommands.PhysicalPathOutputFolder <<  weylLetter << theRank << "/";
  outMainDisplayPath << theCommands.DisplayPathOutputFolder
  << weylLetter << theRank << "/";
  outSltwoPath << outMainPath.str() << "sl2s/";
  outSltwoDisplayPath << outMainDisplayPath.str() << "sl2s/";
  bool NeedToCreateFolders=(!CGI::FileExists(outMainPath.str()) || !CGI::FileExists(outSltwoPath.str()));
  if (NeedToCreateFolders)
  { std::stringstream outMkDirCommand1, outMkDirCommand2;
    outMkDirCommand1 << "mkdir " << outMainPath.str();
    outMkDirCommand2 << "mkdir " << outSltwoPath.str();
    theCommands.SystemCommands.AddOnTop(outMkDirCommand1.str());
    theCommands.SystemCommands.AddOnTop(outMkDirCommand2.str());
    if (comments!=0)
    { *comments << "<br><br>... Created the missing folders for the database. <b> Running  a second time... Please wait for automatic redirection."
      <<  " Clicking back/refresh button in the browser will cause broken links in the calculator due to a technical "
      << "(Apache server configuration) problem.</b><br><br>"
      << "<META http-equiv=\"refresh\" content=\"3; url="
      << theCommands.DisplayNameCalculator << "?"
      << theCommands.inputStringRawestOfTheRaw;
      *comments << "\">  Redirecting in 3 seconds";
    }
    return false;
  }
  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile, outSltwoFileDisplayName;
  outSltwoMainFile << outSltwoPath.str() << "sl2s.html";
  outSltwoFileDisplayName << outSltwoDisplayPath.str() << "sl2s.html";
  outRootHtmlFileName << outMainPath.str() << "rootHtml.html";
  outRootHtmlDisplayName << outMainDisplayPath.str() << "rootHtml.html";
  bool mustRecompute=false;
  if (!CGI::FileExists(outSltwoMainFile.str()) || !CGI::FileExists(outRootHtmlFileName.str()))
    mustRecompute=true;
  if (mustRecompute)
  { std::cout << theCommands.javaScriptDisplayingIndicator;
    std::cout.flush();

/*    if (showIndicator)
    { if (comments!=0)
      { *comments << "<br>The computation is in progress. <b><br>Please do not click back/refresh button: it will cause broken links in the calculator. <br>Appologies for this technical (Apache server configuration) problem. <br>Hope to alleviate it soon.</b>"
        << "<br>Below is an indicator for the progress of the computation."
        << "<br> The computation is slow, up to around 10 minutes for E_8."
        << "<br>Once it is done, you should be redirected automatically to the result page."
        << "<br>To go back to the calculator main page use the back button on your browser.";
        *comments << "\n<br>\nComputing ...<br>" << theCommands.javaScriptDisplayingIndicator
        << "<br>" << "<META http-equiv=\"refresh\" content=\"10; url="
        << theCommands.DisplayNameCalculator << "?";
        std::stringstream civilizedCommand;
        civilizedCommand << "printSlTwoSubalgebrasAndRootSubalgebras{}(" << weylLetter << "_" << theRank
        << ",NoIndicatorDisplay)";
        *comments << "textInput=" << CGI::UnCivilizeStringCGI(civilizedCommand.str()) << "\">";
        std::cout << "<br>META http-equiv=\"refresh\" content=\"10; url="
        << theCommands.DisplayNameCalculator << "?"
        << CGI::UnCivilizeStringCGI(civilizedCommand.str()) << "\"";
      }
      return false;
    } else
    { std::cout << "so far so good said he who was falling, around floor 2.";*/
      SltwoSubalgebras theSl2s;
      theSl2s.owner[0].FindSl2Subalgebras(theSl2s, weylLetter, theRank, *theCommands.theGlobalVariableS);
      std::string PathSl2= outSltwoPath.str(); std::string DisplayPathSl2=outSltwoDisplayPath.str();
      theSl2s.ElementToHtml
      (*theCommands.theGlobalVariableS, theSl2s.owner[0].theWeyl, true, PathSl2, DisplayPathSl2,
       theCommands.DisplayNameCalculator);
      theCommands.SystemCommands.AddListOnTop(theSl2s.listSystemCommandsLatex);
      theCommands.SystemCommands.AddListOnTop(theSl2s.listSystemCommandsDVIPNG);
//    }
  }
  if (comments!=0)
  {// *comments << "<META http-equiv=\"refresh\" content=\"0; url=";
   // *comments << outRootHtmlDisplayName.str();
    //*comments << "\">";
  }
//  theGlobalVariables.MaxAllowedComputationTimeInSeconds=oldMaxAllowedComputationTimeInSeconds;
  return false;
}

bool CommandList::fDecomposeFDPartGeneralizedVermaModuleOverLeviPart
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ IncrementRecursion recursionCounter(theCommands);
  if (theExpression.children.size!=4)
    return theExpression.SetError("The function expects 4 arguments.");

  Expression& typeNode=theExpression.children[0];
  Expression& weightNode=theExpression.children[1];
  Expression& inducingParNode=theExpression.children[2];
  Expression& splittingParNode=theExpression.children[3];
  bool success=theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, typeNode, comments);
  if (!success || typeNode.errorString!="")
    return theExpression.SetError("Failed to construct semisimple Lie algebra from the first argument. See comments.");

  Vector<RationalFunction> theWeightFundCoords;
  Vector<Rational> inducingParSel, splittingParSel;
  SemisimpleLieAlgebra& ownerSS=typeNode.GetData().GetAmbientSSAlgebra();
  WeylGroup& theWeyl=ownerSS.theWeyl;
  int theDim=ownerSS.GetRank();
  Context finalContext;
  if (!weightNode.GetVector<RationalFunction>
      (theWeightFundCoords, finalContext, theDim, theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract highest weight from the second argument.");
  if (! inducingParNode.GetVector<Rational>(inducingParSel, finalContext, theDim, 0, comments))
    return theExpression.SetError("Failed to extract parabolic selection from the third argument");
  if (! splittingParNode.GetVector<Rational>(splittingParSel, finalContext, theDim, 0, comments))
    return theExpression.SetError("Failed to extract parabolic selection from the fourth argument");
  if (comments!=0)
  { *comments << "Your input weight in fundamental coordinates: "
    << theWeightFundCoords.ElementToString();
    *comments << "<br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ElementToString();
    *comments << "<br>Your inducing parabolic subalgebra: " << inducingParSel.ElementToString() << ".";
    *comments << "<br>The parabolic subalgebra I should split over: " << splittingParSel.ElementToString() << ".";
  }
  ModuleSSalgebraNew<RationalFunction> theMod;
  Selection selInducing= inducingParSel;
  theMod.MakeFromHW
  (*ownerSS.owner, ownerSS.indexInOwner, theWeightFundCoords, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0);
  std::string report;
  theMod.SplitOverLevi(& report, splittingParSel, *theCommands.theGlobalVariableS, 1, 0);
  theExpression.MakeString(theCommands, inputIndexBoundVars, report, finalContext);
  return true;
}

int ParserNode::EvaluateSplitIrrepOverLeviParabolic
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> theWeightFundCoords, parSel;
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& selNode=theNode.owner->TheObjects[theArgumentList[1]];
  WeylGroup& theWeyl=theNode.GetContextLieAlgebra().theWeyl;
  int theDim=theNode.owner->theHmm.theRange().GetRank();
  if (! weightNode.GetRootRationalDontUseForFunctionArguments(theWeightFundCoords, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (! selNode.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeightFundCoords.size!=theDim || parSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  out << "Your input weight in fundamental coordinates: "
  << theWeyl.GetFundamentalCoordinatesFromSimple(theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords)  )
  .ElementToString();
  out << ". <br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ElementToString();
  out << ".<br>Your parabolic subalgebra selection: " << parSel.ElementToString() << ".";
  ModuleSSalgebraNew<Rational> theMod;

  Selection emptySel;
  emptySel.init(theDim);
  theMod.MakeFromHW
  (theNode.owner->theAlgebras, 0, theWeightFundCoords, emptySel, theGlobalVariables, 1, 0, 0);
  std::string report;
  theMod.SplitOverLevi(& report, parSel, theGlobalVariables, 1, 0);
  out << "<br>" << report;

  theNode.ExpressionType=theNode.typeString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

bool CommandList::fCasimir
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ IncrementRecursion recursionCounter(theCommands);
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, theExpression, comments))
    return theExpression.SetError("Failed to convert the argument "+theExpression.ElementToString()+ " to a semisimple Lie algebra. ");
  if (theExpression.errorString!="")
    return theExpression.SetError("While trying to generate Lie algebra for the Casimir element, I got the error: " + theExpression.errorString);
  SemisimpleLieAlgebra& theSSowner=theExpression.GetData().GetAmbientSSAlgebra();
  if (theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds<50)
    theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds=50;
  RationalFunction rfOne, rfZero;
  rfOne.MakeOne(0, theCommands.theGlobalVariableS);
  rfZero.MakeZero(0, theCommands.theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunction> theCasimir;
  theCasimir.MakeCasimir(theSSowner, *theCommands.theGlobalVariableS, rfOne, rfZero);
  if (comments!=0)
  { *comments << "Context Lie algebra: " << theSSowner.GetLieAlgebraName
    (theSSowner.theWeyl.WeylLetter, theSSowner.GetRank());
    *comments << ". The coefficient: " << theSSowner.theWeyl.GetKillingDivTraceRatio().ElementToString()
    <<  ". The Casimir element of the ambient Lie algebra. ";
  }
  Data tempData;
  tempData.MakeUE(theCommands, theCasimir, theExpression.GetData().theContextIndex);
  theExpression.MakeDatA(tempData, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::fEmbedG2inB3
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ bool success=theCommands.fElementUniversalEnvelopingAlgebra(theCommands, inputIndexBoundVars, theExpression, comments);
  if (!success || !theExpression.IsElementUE())
    return theExpression.SetError("Failed to convert argument to element of the Universal enveloping algebra. ");
  SemisimpleLieAlgebra& ownerSS=theExpression.GetData().GetAmbientSSAlgebra();
  if (ownerSS.GetRank()!=2 || ownerSS.theWeyl.WeylLetter!='G')
    return theExpression.SetError("Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments.");
  Data g2Data, b3Data;
  b3Data.MakeSSAlgebra(theCommands, 'B', 3);
  g2Data.MakeSSAlgebra(theCommands, 'G', 2);
  HomomorphismSemisimpleLieAlgebra theHmm;
  theHmm.owners=&theCommands.theObjectContainer.theLieAlgebras;
  theHmm.indexRange=b3Data.theIndex;
  theHmm.indexDomain=g2Data.theIndex;
  theHmm.theRange().ComputeChevalleyConstantS(*theCommands.theGlobalVariableS);
  theHmm.theDomain().ComputeChevalleyConstantS(*theCommands.theGlobalVariableS);
  ElementSemisimpleLieAlgebra g_2, g_1plusg_3, g_m2, g_m1plusg_m3, tempElt;
  g_2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (13, theCommands.theObjectContainer.theLieAlgebras, b3Data.theIndex);
  g_m2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (7, theCommands.theObjectContainer.theLieAlgebras, b3Data.theIndex);
  g_1plusg_3.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (12, theCommands.theObjectContainer.theLieAlgebras, b3Data.theIndex);
  tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (14, theCommands.theObjectContainer.theLieAlgebras, b3Data.theIndex);
  g_1plusg_3+=tempElt;
  g_m1plusg_m3.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (6, theCommands.theObjectContainer.theLieAlgebras, b3Data.theIndex);
  tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (8, theCommands.theObjectContainer.theLieAlgebras, b3Data.theIndex);
  g_m1plusg_m3+=tempElt;

  std::cout << "<hr>g_2: " << g_2.ElementToString();
  std::cout << "<hr>g_{1}+g_{3}: " << g_1plusg_3.ElementToString();
  std::cout << "<hr>g_{-2}: " << g_m2.ElementToString();
  std::cout << "<hr>g_{-1}+g_{-3}: " << g_m1plusg_m3.ElementToString();
  theHmm.imagesSimpleChevalleyGenerators.SetSize(4);
  theHmm.imagesSimpleChevalleyGenerators.TheObjects[0]=g_2;
  theHmm.imagesSimpleChevalleyGenerators.TheObjects[1]=g_1plusg_3;
  theHmm.imagesSimpleChevalleyGenerators.TheObjects[2]=g_m2;
  theHmm.imagesSimpleChevalleyGenerators.TheObjects[3]=g_m1plusg_m3;
  theHmm.ComputeHomomorphismFromImagesSimpleChevalleyGenerators(*theCommands.theGlobalVariableS);
  theHmm.GetRestrictionAmbientRootSystemToTheSmallerCartanSA(theHmm.RestrictedRootSystem, *theCommands.theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunction> argument=theExpression.GetData().GetUE();
  ElementUniversalEnveloping<RationalFunction> output;
  if(!theHmm.ApplyHomomorphism(argument, output, *theCommands.theGlobalVariableS))
    return theExpression.SetError("Failed to apply homomorphism for unspecified reason");
  Data tempData;
  tempData.MakeUE(theCommands, output, b3Data.theContextIndex);
  theExpression.MakeDatA(tempData, theCommands, inputIndexBoundVars);
  return true;
}


class DoxygenInstance
{
  public:
  ElementTensorsGeneralizedVermas<RationalFunction> doXyCanYouParseME;
};
