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
    << " More precisely, the scalar product of " << theWeight.ToString() << " and " << tempVect.ToString()
    << " equals " << tempRF.ToString() << ". I cannot decide (more precisely, do not want to *silently* decide for you) "
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
    << " More precisely, the scalar product of " << theWeight.ToString() << " and " << tempVect.ToString()
    << " equals " << tempRF.ToString() << ". I cannot decide (more precisely, do not want to *silently* decide for you) "
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
{ IncrementRecursion recursionCounter(&theCommands);
  SemisimpleLieAlgebra* theSSowner=0;
  if (theExpression.children.size!=2)
  { theExpression.SetError("This function takes 2 arguments");
    return true;
  }
  Expression LieAlgebraNameNode=theExpression.children[0];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, LieAlgebraNameNode, comments))
  { theExpression.SetError("Failed to convert the first argument of "+theExpression.ToString()+ " to a semisimple Lie algebra. ");
    return true;
  }
  if (LieAlgebraNameNode.errorString!="")
  { theExpression.SetError("While trying to generate Lie algebra from the first argument of the Weyl dim formula, I got the error: " + LieAlgebraNameNode.errorString);
    return true;
  }
  theSSowner=&LieAlgebraNameNode.GetAtomicValue().GetAmbientSSAlgebra();
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
  //std::cout << "The fundamental coords: " << theWeight.ToString();
  if (comments!=0)
    *comments << "<br>Your input in simple coords: " << theWeightInSimpleCoords.ToString();
  RationalFunction tempRF= theSSowner->theWeyl.WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
  //std::cout << "<br>The result: " << tempRF.ToString();
  theExpression.MakeRFAtom(tempRF, newContext, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::fAnimateLittelmannPaths
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ IncrementRecursion recursionCounter(&theCommands);
  SemisimpleLieAlgebra* theSSowner=0;
  if (theExpression.children.size!=2)
  { theExpression.SetError("This function takes 2 arguments");
    return true;
  }
  Expression LieAlgebraNameNode=theExpression.children[0];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, LieAlgebraNameNode, comments))
  { theExpression.SetError("Failed to convert the first argument of "+theExpression.ToString()+ " to a semisimple Lie algebra. ");
    return true;
  }
  if (LieAlgebraNameNode.errorString!="")
  { theExpression.SetError("While trying to generate Lie algebra from the first argument of the Weyl dim formula, I got the error: " + LieAlgebraNameNode.errorString);
    return true;
  }
  theSSowner=&LieAlgebraNameNode.GetAtomicValue().GetAmbientSSAlgebra();
  Vector<Rational> theWeight;
  Context tempContext(theCommands);
  if (!theExpression.children[1].GetVector<Rational>(theWeight, tempContext, theSSowner->GetRank(), 0, comments))
  { theExpression.SetError("Failed to convert the argument of the function to a highest weight vector");
    return true;
  }
  Vector<Rational> theWeightInSimpleCoords;
  theWeightInSimpleCoords = theSSowner->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  //std::cout << "The fundamental coords: " << theWeight.ToString();
  if (comments!=0)
    *comments << "<br>Your input in simple coords: " << theWeightInSimpleCoords.ToString();
  LittelmannPath thePath;
  thePath.MakeFromWeightInSimpleCoords(theWeightInSimpleCoords, theSSowner->theWeyl);
  theExpression.MakeStringAtom
  (theCommands, inputIndexBoundVars, thePath.GenerateOrbitAndAnimate(*theCommands.theGlobalVariableS), tempContext)
;
  return true;
}

bool CommandList::fRootSAsAndSltwos
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //bool showIndicator=true;
  Expression* zeTrueArgument=& theExpression;
  std::cout << "ze expression i get: " << theExpression.ToString() << "<hr>";
  std::cout << "which is of type " << theExpression.ToString();
  if (theExpression.theOperation==theCommands.opList() && theExpression.children.size>0)
  { zeTrueArgument=&theExpression.children[0];
    std::cout << "ZeTRueArgument: " << zeTrueArgument->ToString();
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
  SemisimpleLieAlgebra& ownerSS=zeTrueArgument->GetAtomicValue().GetAmbientSSAlgebra();
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
      << " Clicking back/refresh button in the browser will cause broken links in the calculator due to a technical "
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
{ IncrementRecursion recursionCounter(&theCommands);
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
  SemisimpleLieAlgebra& ownerSS=typeNode.GetAtomicValue().GetAmbientSSAlgebra();
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
    << theWeightFundCoords.ToString();
    *comments << "<br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ToString();
    *comments << "<br>Your inducing parabolic subalgebra: " << inducingParSel.ToString() << ".";
    *comments << "<br>The parabolic subalgebra I should split over: " << splittingParSel.ToString() << ".";
  }
  ModuleSSalgebraNew<RationalFunction> theMod;
  Selection selInducing= inducingParSel;
  theMod.MakeFromHW
  (*ownerSS.owner, ownerSS.indexInOwner, theWeightFundCoords, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0);
  std::string report;
  theMod.SplitOverLevi(& report, splittingParSel, *theCommands.theGlobalVariableS, 1, 0);
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, report, finalContext);
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
  .ToString();
  out << ". <br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ToString();
  out << ".<br>Your parabolic subalgebra selection: " << parSel.ToString() << ".";
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
{ IncrementRecursion recursionCounter(&theCommands);
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, theExpression, comments))
    return theExpression.SetError("Failed to convert the argument "+theExpression.ToString()+ " to a semisimple Lie algebra. ");
  if (theExpression.errorString!="")
    return theExpression.SetError("While trying to generate Lie algebra for the Casimir element, I got the error: " + theExpression.errorString);
  SemisimpleLieAlgebra& theSSowner=theExpression.GetAtomicValue().GetAmbientSSAlgebra();
  if (theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds<50)
    theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds=50;
  RationalFunction rfOne, rfZero;
  rfOne.MakeOne(0, theCommands.theGlobalVariableS);
  rfZero.MakeZero(0, theCommands.theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunction> theCasimir;
  theCasimir.MakeCasimir(theSSowner, *theCommands.theGlobalVariableS, rfOne, rfZero);
//  theCasimir.Simplify(*theCommands.theGlobalVariableS);
  if (comments!=0)
  { *comments << "Context Lie algebra: " << theSSowner.GetLieAlgebraName
    (theSSowner.theWeyl.WeylLetter, theSSowner.GetRank());
    *comments << ". The coefficient: " << theSSowner.theWeyl.GetKillingDivTraceRatio().ToString()
    <<  ". The Casimir element of the ambient Lie algebra. ";
  }
  Data tempData;
  tempData.MakeUE(theCommands, theCasimir, theExpression.GetAtomicValue().theContextIndex);
  theExpression.MakeAtom(tempData, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::fDrawWeightSupportWithMults
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments)
{ //theNode.owner->theHmm.MakeG2InB3(theParser, theGlobalVariables);
  if (theExpression.children.size!=2)
  { std::stringstream errorStream;
    errorStream << "Error: the function for drawing weight support takes two arguments (type and highest weight)"
    << ", but you gave " << theExpression.children.size << " arguments instead. ";
    return theExpression.SetError(errorStream.str());
  }
  Expression& typeNode=theExpression.children[0];
  Expression& hwNode=theExpression.children[1];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, typeNode, comments))
    return false;
  if (typeNode.errorString!="")
    return theExpression.SetError(typeNode.errorString);
  SemisimpleLieAlgebra& theAlg=typeNode.GetAtomicValue().GetAmbientSSAlgebra();
  Vector<Rational> highestWeightFundCoords;
  Context tempContext;
  if (!hwNode.GetVector<Rational>(highestWeightFundCoords, tempContext, theAlg.GetRank(), 0, comments))
    return false;
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroup& theWeyl=theAlg.theWeyl;
  highestWeightSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  //Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod<Rational> theChar;
  theChar.MakeFromWeight
  (highestWeightSimpleCoords, theCommands.theObjectContainer.theLieAlgebras, theAlg.indexInOwner);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeWithMults(report, *theCommands.theGlobalVariableS, theDV, 10000);
  out << report << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str(), tempContext);
  return true;
}

bool CommandList::fDrawWeightSupport
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments)
{ //theNode.owner->theHmm.MakeG2InB3(theParser, theGlobalVariables);
  if (theExpression.children.size!=2)
  { std::stringstream errorStream;
    errorStream << "Error: the function for drawing weight support takes two arguments (type and highest weight)"
    << ", but you gave " << theExpression.children.size << " arguments instead. ";
    return theExpression.SetError(errorStream.str());
  }
  Expression& typeNode=theExpression.children[0];
  Expression& hwNode=theExpression.children[1];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, typeNode, comments))
    return false;
  if (typeNode.errorString!="")
    return theExpression.SetError(typeNode.errorString);
  SemisimpleLieAlgebra& theAlg=typeNode.GetAtomicValue().GetAmbientSSAlgebra();
  Vector<Rational> highestWeightFundCoords;
  Context tempContext;
  if (!hwNode.GetVector<Rational>(highestWeightFundCoords, tempContext, theAlg.GetRank(), 0, comments))
    return false;
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroup& theWeyl=theAlg.theWeyl;
  highestWeightSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  //Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod<Rational> theChar;
  theChar.MakeFromWeight
  (highestWeightSimpleCoords, theCommands.theObjectContainer.theLieAlgebras, theAlg.indexInOwner);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeNoMults(report, *theCommands.theGlobalVariableS, theDV, 10000);
  out << report << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str(), tempContext);
  return true;
}

bool CommandList::fEmbedG2inB3
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ theCommands.fElementUniversalEnvelopingAlgebra(theCommands, inputIndexBoundVars, theExpression, comments);
  if (!theExpression.IsElementUE())
    return theExpression.SetError("Failed to convert argument to element of the Universal enveloping algebra. ");
  SemisimpleLieAlgebra& ownerSS=theExpression.GetAtomicValue().GetAmbientSSAlgebra();
  if (ownerSS.GetRank()!=2 || ownerSS.theWeyl.WeylLetter!='G')
    return theExpression.SetError("Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments.");
  Data g2Data, b3Data;
  b3Data.MakeSSAlgebra(theCommands, 'B', 3);
  g2Data.MakeSSAlgebra(theCommands, 'G', 2);
  HomomorphismSemisimpleLieAlgebra theHmm;
  theCommands.MakeHmmG2InB3(theHmm);

  ElementUniversalEnveloping<RationalFunction> argument=theExpression.GetAtomicValue().GetUE();
  ElementUniversalEnveloping<RationalFunction> output;
  if(!theHmm.ApplyHomomorphism(argument, output, *theCommands.theGlobalVariableS))
    return theExpression.SetError("Failed to apply homomorphism for unspecified reason");
//  std::cout << theHmm.ToString(*theCommands.theGlobalVariableS);
  output.Simplify(*theCommands.theGlobalVariableS, 1, 0);
  Data tempData;
  tempData.MakeUE(theCommands, output, b3Data.theContextIndex);
  theExpression.MakeAtom(tempData, theCommands, inputIndexBoundVars);
  return true;
}

std::string CGI::GetAnimateShowHideJavascriptMustBEPutInHTMLHead()
{ std::stringstream out;
  out << "<script src=\"//ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js\" type=\"text/javascript\"></script>";
  return out.str();
}

std::string CGI::GetSliderSpanStartsHidden(const std::string& content, const std::string& label, const std::string& desiredID)
{ std::stringstream out;
  CGI::GlobalFormulaIdentifier++;
  std::stringstream idStringStream;
  idStringStream << desiredID;
  if (desiredID=="")
    idStringStream << "UnnamedSpan" << CGI::GlobalFormulaIdentifier;
  out << "<a href=\"javascript:;\" onmusedown=\"document.getElementById('"  << idStringStream.str() << "').slideToggle('slow');\">Expand/collapse</a>";
  out << "<span id=\"" << idStringStream.str() << "\" style=\"display:none\">" << content << "</span>";
  return out.str();
}

std::string LittelmannPath::GenerateOrbitAndAnimate(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  List<LittelmannPath> theOrbit;
  List<List<int> > theGens;
  this->GenerateOrbit(theOrbit, theGens, theGlobalVariables, 1000, 0);
  AnimationBuffer theBuffer;
//  int theDim=this->owner->GetDim();
  Vectors<double> coxPlane;
  coxPlane.SetSize(2);
  Vectors<double> draggableBAsis;
  this->owner->GetCoxeterPlane(coxPlane[0], coxPlane[1], theGlobalVariables);
//  theBuffer.theBuffer.initDimensions(this->owner.CartanSymmetric, draggableBAsis, coxPlane, 1);
  DrawingVariables tempDV, tempDV2;
  this->owner->DrawRootSystem(tempDV, true, theGlobalVariables, true);
  this->owner->DrawRootSystem(tempDV2, true, theGlobalVariables, true);
  theBuffer.theBuffer=tempDV.theBuffer;
  theBuffer.theFrames.SetSize(theOrbit.size);
  for (int i=0; i<theOrbit.size; i++)
  { DrawingVariables& currentOps=theBuffer.theFrames[i];
    LittelmannPath& currentPath=theOrbit[i];
    currentOps.theBuffer=tempDV.theBuffer;
    for (int j=0; j<currentPath.Waypoints.size; j++)
    { if (j!=currentPath.Waypoints.size-1)
      { currentOps.drawLineBetweenTwoVectorsBuffer(currentPath.Waypoints[j], currentPath.Waypoints[j+1], DrawingVariables::PenStyleNormal, 0);
        tempDV2.theBuffer.drawLineBetweenTwoVectorsBuffer(currentPath.Waypoints[j], currentPath.Waypoints[j+1], DrawingVariables::PenStyleNormal, 0);
      }
      currentOps.drawCircleAtVectorBuffer(currentPath.Waypoints[j], 2, DrawingVariables::PenStyleNormal, 0);
      tempDV2.theBuffer.drawCircleAtVectorBuffer(currentPath.Waypoints[j], 4, DrawingVariables::PenStyleNormal, 0);
    }
  }
  out << "<br>Animation of the Littelmann paths follows.";
  out << theBuffer.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->owner->GetDim());
  out << "<br>Here are all Littelmann paths drawn simultaneously. ";
  out << tempDV2.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->owner->GetDim());
  out << "Littelmann paths in simple coordinates given in the order in which they are generated (" << theOrbit.size << " total):<br>";
  for (int i=0; i<theOrbit.size; i++)
    out << theOrbit[i].ToString() << "&nbsp&nbsp" << this->ElementToStringOperatorSequenceStartingOnMe(theGens [i]) << "<br>";
  return out.str();
}

std::string AnimationBuffer::GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension)
{ if (theDimension<2)
    return "<br><b>Pictures of dimension less than two are not drawn.</b><br>";
  if (theFrames.size<1)
    return "<br><b>Empty animation</b>";
  std::stringstream out, tempStream1, tempStream2, tempStream3, tempStream4, tempStream5, tempStream6;
  std::stringstream tempStream7, tempStream8, tempStream9, tempStream10, tempStream11,
  tempStream12, tempStream13, tempStream14, tempStream15, tempStream4_1, tempStream11_1;
  DrawingVariables* boss=&this->theFrames[0];
  this->theBuffer=boss->theBuffer;
  boss->NumHtmlGraphics++;
  int timesCalled=boss->NumHtmlGraphics;
  tempStream1 << "drawConeInit" << timesCalled;
  std::string theInitFunctionName= tempStream1.str();
  tempStream5 << "drawAll" << timesCalled;
  std::string theDrawFunctionName= tempStream5.str();
  tempStream2 << "idCanvasCone" << timesCalled;
  std::string theCanvasId= tempStream2.str();
  tempStream3 << "surf" << timesCalled;
  std::string theSurfaceName=tempStream3.str();
  tempStream4 << "pts1" << timesCalled;
  std::string Points1ArrayName=tempStream4.str();
  tempStream4_1 << "colorsLines" << timesCalled;
  std::string Points1ArrayNameColors=tempStream4_1.str();

  tempStream10 << "pts2" << timesCalled;
  std::string Points2ArrayName=tempStream10.str();

  tempStream11 << "circ" << timesCalled;
  std::string circArrayName=tempStream11.str();
  tempStream11_1 << "circRadii" << timesCalled;
  std::string circRadiiArrayName=tempStream11_1.str();

  tempStream12 << "txt" << timesCalled;
  std::string txtArrayName=tempStream12.str();

  tempStream6 << "basisCircles" << timesCalled;
  std::string basisCircles = tempStream6.str();
  tempStream13 << "proj" << timesCalled;
  std::string projName = tempStream13.str();
  tempStream14 << "projCirc" << timesCalled;
  std::string projBasisCircles= tempStream14.str();
  tempStream15 << "eiBasis" << timesCalled;
  std::string eiBasis= tempStream15.str();

  tempStream7 << "shiftXCone" << timesCalled;
  std::string shiftX=tempStream7.str();
  tempStream8 << "shiftYCone" << timesCalled;
  std::string shiftY=tempStream8.str();
  tempStream9 << "convXY" << timesCalled;
  std::string functionConvertToXYName=tempStream9.str();
  out << "<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/dojo/1.6/dojo/dojo.xd.js\""
  << " djConfig = \"parseOnLoad: true\"></script>";
  out << "<div style=\"width:" << boss->DefaultHtmlWidth << ";height:" << boss->DefaultHtmlHeight << ";border:solid 1px\" id=\"" << theCanvasId
  << "\" onmousedown=\"clickCanvasCone" << timesCalled << "(event.clientX, event.clientY);\" onmouseup=\"selectedBasisIndexCone" << timesCalled
  << "=-1;\" onmousemove=\"mouseMoveRedrawCone" <<  timesCalled << "(event.clientX, event.clientY);\" "
  << "onmousewheel=\"mouseHandleWheelCone" << timesCalled << "(event);\""
  << "></div>";
  out
  << "<br>The projection plane (drawn on the screen) is spanned by the following two vectors<br> \n";
  List<List<std::string> > textEbasisNamesUserInput;
  List<List<std::string> > textEbasisNamesReadOnly;
  textEbasisNamesUserInput.SetSize(2);
  textEbasisNamesReadOnly.SetSize(2);
  for (int i=0; i<2; i++)
  { textEbasisNamesReadOnly[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream;
      tmpStream << "textEbasisReadOnly" << timesCalled << "_" << i << "_" << j;
      textEbasisNamesReadOnly[i][j]=tmpStream.str();
      out << "<span id=\"" << textEbasisNamesReadOnly[i][j]
      << "\"> </span>\n";
    }
    out << "<br>";
  }
  for (int i=0; i<2; i++)
  { textEbasisNamesUserInput[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream, tmpStream2;
      tmpStream << "textEbasisUserInput" << timesCalled << "_" << i << "_" << j;
      textEbasisNamesUserInput[i][j]=tmpStream.str();
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << textEbasisNamesUserInput[i][j]
      << "\">" << "</textarea>\n";
    }
    out << "<br>";
  }
  out << "<button onclick=\"startProjectionPlaneUser" << timesCalled << "();\">Change to basis</button><br>";
  out
  << "The bilinear form of the vector space follows. The ij^th element "
  << " gives the scalar product of e_i and e_j. If you enter a degenerate or non-positive definite "
  << " symmetric bilinear form"
  << " the javascript might crash. You are expected to enter a symmetric strictly positive definite matrix. <br> \n";
  for (int i=0; i<this->theBuffer.theBilinearForm.NumRows; i++)
  { for (int j=0; j<this->theBuffer.theBilinearForm.NumCols; j++)
    { std::stringstream tmpStream;
      tmpStream << "textBilinearForm" << timesCalled << "_" << i << "_" << j;
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << tmpStream.str()
      << "\" \n onChange=\"BilinearForm"
      << timesCalled << "[" << i << "]["
      << j << "]=document.getElementById('" << tmpStream.str() << "').value;\">"
      << this->theBuffer.theBilinearForm.elements[i][j] << "</textarea>\n";
    }
    out << "<br>";
  }
  out << CGI::GetHtmlButton("button"+theCanvasId, theDrawFunctionName+"();", "redraw");
  out << "<br>The picture is drawn using javascript."
  << "<br> Left click + hold+ move the mouse on a special vector = rotates the special vector. "
  << "Special vectors, if any, should be labeled according to the author's ``artistic'' inspiration (usually dark red dots).  "
  << "<br>Moving a vector rotates ``infinitesimally'' the projection plane of your screen "
  << "<br>1) inside the projection plane "
  << "<br>2) in the plane spanned by the selected vector and its orthogonal complement relative to the projection plane. "
  << "<br>The angle change matches the motion of your mouse pointer.  "
  << " Special care must be taken if the selected vector lies "
  << "inside the projection plane or the selected vector is orthogonal to the projection plane. "
  << "If one of these cases happens, the picture might jump around a bit."
  << "<br>The mouse wheel zooms in/out. "
  << " Zooming is tested to work on Firefox and google Chrome browsers on Ubuntu."
  ;
  out << "<br>Currently, " << this->theBuffer.IndexNthDrawOperation.size << " elements are drawn. ";
  if (this->theBuffer.IndexNthDrawOperation.size>500)
    out  << " The visualization is javascript/pc processor <b>intensive</b> so it will <b>not work well</b> "
    <<  " for graphics with lots of elements. This message is displayed only when the number of drawn elements is more than 500.";
  out << "<hr>";

  out << "<script type=\"text/javascript\">\n";

  if (this->theBuffer.ProjectionsEiVectors.size!= theDimension || this->theBuffer.theBilinearForm.NumRows!=theDimension)
  { this->theBuffer.MakeMeAStandardBasis(theDimension);
    //std::cout << "made a standard basis!";
  }
  out << "var BilinearForm" << timesCalled << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
  { out << "BilinearForm" << timesCalled << "[" << i << "]=new Array(" << theDimension << ");\n";
    for (int j=0; j<theDimension; j++)
      out << "BilinearForm" << timesCalled << "[" << i << "][" << j << "]=" << this->theBuffer.theBilinearForm.elements[i][j] << ";\t";
    out << "\n";
  }
  this->theBuffer.ModifyToOrthonormalNoShiftSecond
  (this->theBuffer.BasisProjectionPlane[0][1], this->theBuffer.BasisProjectionPlane[0][0]);
//  (this->theBuffer.BasisProjectionPlane[0][0], this->theBuffer.BasisProjectionPlane[0][1]);
  out
  << "var VectorE1Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n";

  for (int i=0; i<theDimension; i++)
  { out << "VectorE2Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][1][i] << ";\t";
    out << "VectorE1Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][0][i] << ";\n";
  }
  out << "var frameCount" << timesCalled << "=0;\n";
  out << "var frameCountGoesUp" << timesCalled << "=true;\n";
  int numFramesUserPlane=50;
  out << "function startProjectionPlaneUser" << timesCalled << "(){\n"
  << " frameCount" << timesCalled << "=0;\n";
  for (int i=0; i<theDimension; i++)
    out
    << "  VectorE1ConeStart" << timesCalled << "[" << i << "]=VectorE1Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE2ConeStart" << timesCalled << "[" << i << "]=VectorE2Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE1ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNamesUserInput[0][i] << "\").value;\n"
    << "  VectorE2ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNamesUserInput[1][i] << "\").value;\n";
  out
  << "  changeProjectionPlaneUser" << timesCalled << "();\n"
  << "\n}\n";
  out << "function changeProjectionPlaneUser" << timesCalled << "(){\n"
  << "  frameCount" << timesCalled << "++;\n"
  << "  if (frameCount" << timesCalled << "> " << numFramesUserPlane << ")\n"
  << "    return;\n"
  << "  for (i=0; i<" << theDimension << "; i++)\n"
  << "  { "
  << "VectorE1Cone" << timesCalled << "[i]=VectorE1ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE1ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "    VectorE2Cone" << timesCalled << "[i]=VectorE2ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE2ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "  }\n"
  << "  MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "  window.setTimeout(\"changeProjectionPlaneUser" << timesCalled << "()\",100);\n"
  << "}";
  if (this->theBuffer.BasisProjectionPlane.size>2)
  { out << "BasisProjectionPlane" << timesCalled << "=new Array(" << this->theBuffer.BasisProjectionPlane.size << ");\n";
    for (int j=0; j<this->theBuffer.BasisProjectionPlane.size; j++)
    { out << "BasisProjectionPlane" << timesCalled << "[" << j << "]=new Array(2);\n";
      for (int k=0; k<2; k++)
      { out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "]=new Array(" << theDimension << ");\n";
        for (int l=0; l<theDimension; l++)
          out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "][" << l << "]="
          << this->theBuffer.BasisProjectionPlane[j][k][l] << ";\t";
        out << "\n";
      }
      out << "\n";
    }
    out << "function changeProjectionPlaneOnTimer" << timesCalled << "(){\n"
    << "if(frameCountGoesUp" << timesCalled << ")\n"
    << "  frameCount" << timesCalled << "++;\n"
    << "else\n"
    << "  frameCount" << timesCalled << "--;\n"
    << "if (frameCount" << timesCalled << "==" << this->theBuffer.BasisProjectionPlane.size-1 << " || "
    << "frameCount" << timesCalled << "==0)\n"
    << "{ \n"
    << "  frameCountGoesUp" << timesCalled << "=! frameCountGoesUp" << timesCalled << ";\n"
    << "}\n"
    << "if (frameCount" << timesCalled << ">= " << this->theBuffer.BasisProjectionPlane.size << ")\n"
    << "  return;"
    << "for (i=0; i<" << theDimension << "; i++)\n"
    << "{ VectorE1Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][0][i];\n"
    << "  VectorE2Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][1][i];\n"
    << "\n}\n";
    out << theDrawFunctionName << "();\n";
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n"
    << "}\n";
  }

  out << "var " << projName << "= new Array(" << theDimension << ");\n";
  out << "var " << eiBasis << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
    out << projName << "[" << i << "]= new Array(2);\n";
  out  << "var " << basisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  out  << "var " << projBasisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  for (int i=0; i<this->theBuffer.BasisToDrawCirclesAt.size; i++)
  { out << basisCircles << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << this->theBuffer.BasisToDrawCirclesAt[i][j];
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
  }
  for (int i=0; i<theDimension; i++)
  { ////////////////////
    out << eiBasis << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << (i==j)? 1 :0;
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
    //////////////////
    out << projBasisCircles << "[" << i << "]= new Array(2);\n";
  }
  out << "var " << Points1ArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << Points2ArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << circArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << circRadiiArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << txtArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << Points1ArrayNameColors << "=new Array(" << this->theFrames.size << ");\n"
  ;
  for (int k=0; k<this->theFrames.size; k++)
  { DrawOperations& currentOps= this->theFrames[k].theBuffer;
    out << Points1ArrayName << "[" << k << "]" << "=new Array(" << currentOps.theDrawLineBetweenTwoRootsOperations.size << ");\n"
    << Points2ArrayName << "[" << k << "]=new Array(" << currentOps.theDrawLineBetweenTwoRootsOperations.size << ");\n"
    << circArrayName         << "[" << k << "]=new Array(" << currentOps.theDrawCircleAtVectorOperations.size << ");\n"
    << circRadiiArrayName << "[" << k << "]=new Array(" << currentOps.theDrawCircleAtVectorOperations.size << ");\n"
    << txtArrayName << "[" << k << "]=new Array(" << currentOps.theDrawTextAtVectorOperations.size << ");\n"
    << Points1ArrayNameColors << "[" << k << "]=new Array(" << currentOps.theDrawLineBetweenTwoRootsOperations.size << ");\n"
    ;
    for (int i=0; i<currentOps.theDrawLineBetweenTwoRootsOperations.size; i++)
    { Vector<double>& current1=currentOps.theDrawLineBetweenTwoRootsOperations[i].v1;
      Vector<double>& current2=currentOps.theDrawLineBetweenTwoRootsOperations[i].v2;
      out << Points1ArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current1[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
      out << Points1ArrayNameColors << "[" << k << "]" << "[" << i << "]=\""
      << boss->GetColorHtmlFromColorIndex(currentOps.theDrawLineBetweenTwoRootsOperations[i].ColorIndex)
      << "\";";
      out << Points2ArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current2[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
    }
    for (int i=0; i<currentOps.theDrawCircleAtVectorOperations.size; i++)
    { Vector<double>& current1=currentOps.theDrawCircleAtVectorOperations[i].theVector;
      out << circRadiiArrayName<< "[" << k << "]" << "[" << i << "]=" << currentOps.theDrawCircleAtVectorOperations[i].radius << ";\n";
      out << circArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current1[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
    }
    for (int i=0; i<currentOps.theDrawTextAtVectorOperations.size; i++)
    { Vector<double>& current1=currentOps.theDrawTextAtVectorOperations [i].theVector;
      out << txtArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current1[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
    }
  }
  out << "var " << shiftX << "=" <<
  this->theBuffer.centerX[0]
  //100
  << ";\n";
  out << "var " <<  shiftY << "=" <<
  this->theBuffer.centerY[0]
  //100
  << ";\n";
  out << "var GraphicsUnitCone" << timesCalled << "=" << this->theBuffer.GraphicsUnit[0] << ";\n";
  out << "function " << functionConvertToXYName << "(vector){\n";
  out << "resultX=" << shiftX << "; resultY=" << shiftY << ";\nfor (i=0; i<" << theDimension << "; i++){\n";
  out << "resultX+=vector[i]*" << projName << "[i][0];\n";
  out << "resultY+=vector[i]*" << projName << "[i][1];\n}\n";
  out << "result=[resultX, resultY];\n";
  out << "return result;\n";
  out << "}\n";
  out << "var globalFrameCounter=0;\n";
  int frameDelay=3000;
  out
  << "function incrementGlobalFrameCounter" << timesCalled << "(){\n"
  << "globalFrameCounter++;\n"
  << "if (globalFrameCounter>=" << this->theFrames.size << ")\n"
  << "globalFrameCounter=0;\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "window.setTimeout(\"incrementGlobalFrameCounter" << timesCalled << "()\"," << frameDelay << ");\n"
  << "}\n"
  ;
  out << "var " << theSurfaceName << "=0;\n"
  << "function " << theDrawFunctionName << "(){\n"
  << "  if (" << theSurfaceName << "==0)\n"
  << "    " << theInitFunctionName << "();\n"
  << "ComputeProjections" << timesCalled << "();\n"
  << theSurfaceName << ".clear();\n";
  out
  << "for (s=0; s<" << Points1ArrayName << "[globalFrameCounter].length" << "; s++ )\n  "
  << theSurfaceName << ".createLine({"
  << " x1 : " << functionConvertToXYName << "( " << Points1ArrayName << "[globalFrameCounter][s])[0],"
  << " y1 :" << functionConvertToXYName << "( " << Points1ArrayName << "[globalFrameCounter][s])[1],"
  << " x2 :"  << functionConvertToXYName << "( " << Points2ArrayName << "[globalFrameCounter][s])[0],"
  << " y2 :" << functionConvertToXYName << "( " << Points2ArrayName << "[globalFrameCounter][s])[1] }).setStroke({color : "
  << Points1ArrayNameColors <<  "[globalFrameCounter][s]"
  << "});\n";
  out
  << "for (s=0; s<" << circArrayName << "[globalFrameCounter].length" << "; s++ )\n  "
  << theSurfaceName << ".createEllipse({"
  << " cx :" << functionConvertToXYName << "( " << circArrayName <<"[globalFrameCounter]" << "["
  << "s" << "])[0],"
  << " cy :" << functionConvertToXYName << "( " << circArrayName <<"[globalFrameCounter]" << "["
  << "s" << "])[1],"
  << " rx : "  <<  circRadiiArrayName << "[globalFrameCounter][s]" << ","
  << " ry :" << circRadiiArrayName << "[globalFrameCounter][s]"  << " }).setStroke({color : \""
  << "#FF0000"
  << "\"});\n";
  /*  for (int i=0; i<this->theBuffer.IndexNthDrawOperation.size; i++)
  { int currentIndex=this->theBuffer.IndexNthDrawOperation[i];
    switch(theBuffer.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawLineBetweenTwoVectors:
        out << theSurfaceName << ".createLine({"
              << " x1 :" << functionConvertToXYName << "( " << Points1ArrayName << "["
              << currentIndex<< "])[0],"
              << " y1 :" << functionConvertToXYName << "( " << Points1ArrayName << "["
              << currentIndex << "])[1],"
              << " x2 :"  << functionConvertToXYName << "( " << Points2ArrayName << "["
              << currentIndex << "])[0],"
              << " y2 :" << functionConvertToXYName << "( " << Points2ArrayName << "["
              << currentIndex << "])[1] }).setStroke({color : \""
              << boss->GetColorHtmlFromColorIndex(this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].ColorIndex)
              << "\"});\n";
        break;
      case DrawOperations::typeDrawCircleAtVector:
        out << theSurfaceName << ".createEllipse({"
              << " cx :" << functionConvertToXYName << "( " << circArrayName << "["
              << currentIndex << "])[0],"
              << " cy :" << functionConvertToXYName << "( " << circArrayName << "["
              << currentIndex << "])[1],"
              << " rx : "  <<  this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius << ","
              << " ry :" << this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius
              << " }).setStroke({color : \""
              << boss->GetColorHtmlFromColorIndex(this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].ColorIndex)
              << "\"});\n";
        break;
      case DrawOperations::typeDrawTextAtVector:
        out << theSurfaceName << ".createText({"
              << " x :" << functionConvertToXYName << "( " << txtArrayName << "["
              << currentIndex<< "])[0],"
              << " y :" << functionConvertToXYName << "( " << txtArrayName << "["
              << currentIndex << "])[1],"
              << " text : \""  <<  this->theBuffer.theDrawTextAtVectorOperations[currentIndex].theText
              << "\" }).setStroke({color : \""
              << boss->GetColorHtmlFromColorIndex(this->theBuffer.theDrawTextAtVectorOperations[currentIndex].ColorIndex)
              << "\"});\n";
      default: break;
    }
  }*/
  out << "}\n"
  << "function " << theInitFunctionName << "(){\n"
  << "node = dojo.byId(\"" << theCanvasId << "\");\n"
//  << " dojo.require(\"dojox.gfx\", function(){"
  << "if (dojox.gfx!=undefined)\n"
  << "{ dojo.require(\"dojox.gfx\"); "
  << theSurfaceName << "  = dojox.gfx.createSurface(node,"
  << boss->DefaultHtmlWidth << "," << boss->DefaultHtmlHeight << ");}\n"
  << " else return;\n "
//  << "});\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << theDrawFunctionName << "();\n";
  if (this->theBuffer.BasisProjectionPlane.size>2)
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n";
  if (this->theFrames.size>1)
    out <<"window.setTimeout(\"incrementGlobalFrameCounter" << timesCalled << "()\"," << frameDelay << ");\n";
  out << " }\n";
  out << "var selectedBasisIndexCone" << timesCalled << "=-1;\n"
  << "var clickTolerance=5;\n"
  << "function ptsWithinClickToleranceCone" << timesCalled << "(x1, y1, x2, y2)\n"
  << "{ if (x1-x2>clickTolerance || x2-x1>clickTolerance || y1-y2>clickTolerance || y2-y1>clickTolerance )\n    return false;\n  return true;\n}";

  out << "function MultiplyVector" << timesCalled << "(output, coeff)"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]*=coeff;\n"
  << "}\n"
  << "function AddVectorTimes" << timesCalled << "(output, inputVector, coeff)\n"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]+=inputVector[i]*coeff;\n"
  << "}\n"
  << "function ScaleToUnitLength" << timesCalled << "(vector)\n"
  << "{ MultiplyVector" << timesCalled << "(vector, 1/Math.sqrt(getScalarProduct" << timesCalled << "(vector,vector)));\n"
  << "}\n";
  out
  << "function RotateOutOfPlane" << timesCalled << "(input, orthoBasis1, orthoBasis2, oldTanSquared, newTanSquared, newX, newY, oldX, oldY)"
  << "{ var projection= dojo.clone(orthoBasis1);\n"
  << "  var vComponent= dojo.clone(input);\n"
  << "  var scal1= getScalarProduct" << timesCalled << "(orthoBasis1, input);\n"
  << "  var scal2= getScalarProduct" << timesCalled << "(orthoBasis2, input);\n"
  << "  MultiplyVector" << timesCalled << "(projection, scal1);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(vComponent, projection, -1);\n"
  << "  var oldAngle=Math.atan(Math.sqrt(oldTanSquared));\n"
  << "  var newAngle=Math.atan(Math.sqrt(newTanSquared));\n"
  << "  if (isNaN(oldAngle) || isNaN(newAngle))\n"
  << "    return input;\n"
  << "  var angleChange=-oldAngle+newAngle;\n"
  << "  if (newX*oldX<0 && newY*oldY<0)\n"
//  <<  " {
//  << "    angleChange=oldAngle+newAngle;\n"
  << "    angleChange*=-1;\n"
//  << "  };"
//  << "  topBound=Math.PI/2;\n"
//  << "  bottomBound=-Math.PI/2;\n"
//  << "  while (angleChange>=topBound || angleChange<bottomBound)\n"
//  << "    if(angleChange>=topBound)\n"
//  << "      angleChange-=Math.PI;\n"
//  << "    else"
//  << "      angleChange+=Math.PI;\n"
  << "  projection=dojo.clone(orthoBasis1);\n"
  << "  MultiplyVector" << timesCalled << "(projection, Math.cos(angleChange)*scal1-Math.sin(angleChange)*scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, Math.sin(angleChange)*scal1+Math.sin(angleChange)*scal2);\n"
  << "  var result= vComponent;\n"
  << "  AddVectorTimes" << timesCalled << "(result, projection, 1);\n"
  << "  return result;\n"
  << "}\n";
  out << "function MakeVectorE1AndE2orthonormal" << timesCalled << "(){\n"
  << "  AddVectorTimes" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", VectorE1Cone" << timesCalled << ", -getScalarProduct"
  << timesCalled << "(VectorE1Cone" << timesCalled << ", VectorE2Cone" << timesCalled
  << ")/getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", VectorE1Cone" << timesCalled << "));\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE1Cone" << timesCalled << ");\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE2Cone" << timesCalled << ");\n"
  << "}\n";
  out << "function ComputeProjections" << timesCalled << "()\n"
  << "{ for (var i=0; i<" << theDimension << "; i++)\n"
  << "  { " << projName << "[i][0]=GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ","  << eiBasis << "[i]);\n"
  << "    " << projName << "[i][1]=-GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << eiBasis << "[i]);\n"
  << "  }\n";
  for (int j=0; j<theDimension; j++)
    out << "  document.getElementById(\"" << textEbasisNamesReadOnly[0][j] << "\").innerHTML="
    << "VectorE1Cone" << timesCalled << "[" << j << "];\n"
    << "  document.getElementById(\"" << textEbasisNamesReadOnly[1][j] << "\").innerHTML="
    << "VectorE2Cone" << timesCalled << "[" << j << "];\n";
  out
  << "  for (var i=0; i<" << this->theBuffer.BasisToDrawCirclesAt.size << "; i++)\n"
  << "  { " << projBasisCircles << "[i][0]=GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "    " << projBasisCircles << "[i][1]=-GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "  }\n"
  << "}\n";
  out << "\nfunction getScalarProduct" << timesCalled << "(root1, root2)\n"
  << "{ var result=0;\n"
  << "  for (var i=0; i<" << theDimension << "; i++)\n"
  << "    for (var j=0; j<" << theDimension << "; j++)\n"
  << "      result+= root1[i]*root2[j]*BilinearForm" << timesCalled << "[i][j];\n"
  << "  return result;\n"
  << "}\n";
  out
  << "function getAngleFromXandY" << timesCalled << "(x, y, neighborX, neighborY)\n"
  << "{ var result;\n"
  << "  if (x!=0)\n"
  << "   result= Math.atan(y/x);\n"
  << "  else\n"
  << "    if (y>0)\n"
  << "      result= Math.PI/2;\n"
  << "    else\n"
  << "      result= -Math.PI/2;\n"
  << "  return result;\n"
  << "}\n";
  out
  << "function getAngleChange" << timesCalled << "(newX, newY, oldX, oldY)\n"
  << "{ var result=getAngleFromXandY" << timesCalled
  << "(newX, newY, oldX, oldY)-getAngleFromXandY" << timesCalled << "(oldX, oldY, newX, newY);\n"
  << "  topBound=Math.PI/2;\n"
  << "  bottomBound=-Math.PI/2;\n"
  << "  while (result>topBound || result< bottomBound)\n"
  << "    if (result>topBound)\n"
  << "      result-=Math.PI;\n"
  << "    else\n"
  << "      result+=Math.PI;\n"
  << "  return result;\n"
  << "}\n";
  out << "\nfunction changeBasis" << timesCalled <<  "(selectedIndex, newX, newY)\n"
  << "{ if (newX==0 && newY==0)\n"
  << "    return;\n"
  << "  var selectedRoot=" << basisCircles << "[selectedIndex];\n"
  << "  var selectedRootLength=getScalarProduct" << timesCalled << "(selectedRoot,selectedRoot);\n"
  << "  var projectionSelected=" << projBasisCircles << "[selectedIndex];\n"
  << "  var oldX=projectionSelected[0]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  var oldY=-projectionSelected[1]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  newX/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  newY/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  var epsilon=0.000000015;\n"
  << "  if (newX*newX+newY*newY>0.003)\n"
  << "  { var AngleChange= -getAngleChange" << timesCalled << "(newX, newY, oldX, oldY);\n"
  << "    var NewVectorE1=dojo.clone(VectorE1Cone" << timesCalled << ");\n"
  << "    var NewVectorE2= dojo.clone(VectorE2Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE1, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE1, VectorE2Cone" << timesCalled << ", Math.sin(AngleChange));\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE2, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE2, VectorE1Cone" << timesCalled << ", -Math.sin(AngleChange));\n"
  << "    VectorE1Cone" << timesCalled << "=NewVectorE1;\n"
  << "    VectorE2Cone" << timesCalled << "=NewVectorE2;\n"
  << "  }\n"
  << "  if (newX*newX+newY*newY>0.0001)\n"
  << "  { var RootTimesE1=getScalarProduct" << timesCalled << "(selectedRoot, VectorE1Cone" << timesCalled << ");\n"
  << "    var RootTimesE2=getScalarProduct" << timesCalled << "(selectedRoot, VectorE2Cone" << timesCalled << ");\n"
  << "    var vOrthogonal=dojo.clone(selectedRoot);\n"
  << "    var vProjection=dojo.clone(VectorE1Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(vProjection, RootTimesE1);\n"
  << "    AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", RootTimesE2" << ");\n"
  << "    AddVectorTimes" << timesCalled << "(vOrthogonal, vProjection, -1);\n"
  << "    var oldRatioProjectionOverHeightSquared = (oldX*oldX+oldY*oldY)/ (selectedRootLength-oldX*oldX-oldY*oldY);\n"
  << "    var newRatioProjectionOverHeightSquared = (newX*newX+newY*newY)/ (selectedRootLength-newX*newX-newY*newY);\n";
  if (theDimension>2)
  { out
    << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)<epsilon && getScalarProduct" << timesCalled
    << "(vOrthogonal, vOrthogonal)>-epsilon)\n"
    << "    { vOrthogonal=dojo.clone(" << eiBasis <<  "[2]);\n"
    << "    }\n";
  }
  out
  << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)>epsilon || getScalarProduct" << timesCalled
  << "(vOrthogonal, vOrthogonal)<-epsilon)\n"
  << "    { if (oldRatioProjectionOverHeightSquared==0)\n"
  << "      { vProjection=dojo.clone(VectorE1Cone" << timesCalled << ");\n"
  << "        MultiplyVector" << timesCalled << "(vProjection, -newX);\n"
  << "        AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", newY" << ");\n"
  << "      }\n"
  << "      ScaleToUnitLength" << timesCalled << "(vProjection);\n"
  << "      ScaleToUnitLength" << timesCalled << "(vOrthogonal);\n"
  << "      VectorE1Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE1Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "      VectorE2Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "    }\n"
  << "    MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "    ComputeProjections" << timesCalled << "();\n"
  << "  }\n"
  << "}\n";

  out << "\nfunction clickCanvasCone" << timesCalled << "(cx,cy)\n"
  << "{ divPosX=0;\n  divPosY=0;\n  thePointer= document.getElementById(\"idCanvasCone" << timesCalled << "\");\n"
  << "  while(thePointer)  {\n  divPosX += thePointer.offsetLeft;\n  divPosY += thePointer.offsetTop;\n  thePointer = thePointer.offsetParent;\n  }"
  << "\n  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");"
  << "\n  posy=(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n  selectedBasisIndexCone" << timesCalled <<"=-1;\n"
  << "if (ptsWithinClickToleranceCone" << timesCalled << "(posx,posy,0,0))" << "\nselectedBasisIndexCone" << timesCalled << "=-2;\n"
  <<  "for (i=0; i<" << theDimension << ";i++)  {\n if (ptsWithinClickToleranceCone" << timesCalled
  << "(posx, posy, " << projBasisCircles << "[i][0]" << ", " << projBasisCircles
  << "[i][1]" << "))\n"
  << "  selectedBasisIndexCone" << timesCalled << "=i;  \n}\n}\nfunction mouseMoveRedrawCone" << timesCalled << "(cx, cy)\n"
  << "{ if (selectedBasisIndexCone" << timesCalled << "==-1)\n    return;\n  divPosX=0;\n  divPosY=0;\n"
  << "  thePointer= document.getElementById(\"idCanvasCone"<< timesCalled << "\");\n  while(thePointer)\n  { divPosX += thePointer.offsetLeft;\n"
  << "    divPosY += thePointer.offsetTop;\n    thePointer = thePointer.offsetParent;\n  }\n"
  << "  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");\n"
  << "  posy=-(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n"
  << "if (selectedBasisIndexCone" << timesCalled << "==-2)\n{ shiftXCone" << timesCalled << "=(cx-divPosX+document.body.scrollLeft);\n"
  << shiftY << "=(cy-divPosY+document.body.scrollTop);\n  }  else\n"
  << "{ changeBasis" << timesCalled << "(selectedBasisIndexCone" << timesCalled << ", posx, posy);\n  }\n  "
  << theDrawFunctionName << " ();\n}\n";
  out << "\n tempDiv=document.getElementById(\"" << theCanvasId << "\");";
  out << "\ntempDiv.addEventListener (\"DOMMouseScroll\", mouseHandleWheelCone"
  << timesCalled << ", true);\n";
  out << "function mouseHandleWheelCone" << timesCalled << "(theEvent){\n"
  << "theEvent = theEvent ? theEvent : window.event;\n theEvent.preventDefault();\ntheEvent.stopPropagation();\ntheWheelDelta = theEvent.detail ? theEvent.detail * -1 : theEvent.wheelDelta / 40;\n"
  << "GraphicsUnitCone" << timesCalled << "+=theWheelDelta;\n"
  << "if (GraphicsUnitCone" << timesCalled << "==0)\n GraphicsUnitCone" << timesCalled << "=3;\n"
//  << "for (i=0; i<" << theDimension << "; i++){\n "
//  << basisName << "[i][0]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << basisName << "[i][1]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << "}\n"
  << theDrawFunctionName << "();\n}\n";
out
  << "dojo.require(\"dojox.gfx\");\n"
  << "dojo.addOnLoad(" << theInitFunctionName << ");\n"
  << "dojo.addOnLoad(" << theDrawFunctionName << ");\n"
  << "function dojoOnLoadDoesntWork" << timesCalled << " (){\n"
  << "  if (" << theSurfaceName << "==0){\n  "
  << theInitFunctionName << "();\n"
  << "  window.setTimeout(function(){dojoOnLoadDoesntWork" << timesCalled << "();}, 1000);\n"
  << "  }\n"
  << "}"
  << "dojoOnLoadDoesntWork" << timesCalled << "();\n"
  << "</script>\n"
  ;
  return out.str();
}

template<class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::SplitOverLevi
  (std::string* Report, Vector<Rational>& splittingParSel, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero, List<ElementUniversalEnveloping<CoefficientType> >* outputEigenVectors,
   Vectors<CoefficientType>* outputWeightsFundCoords)
{ if (this->theChaR.size!=1)
  { if (Report!=0)
    { std::stringstream out;
      out << "I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
      out << " Instead I got the character " << this->theChaR.ToString() << " (" << this->theChaR.size << " monomials)";
      *Report=out.str();
    }
    return;
  }
  ReflectionSubgroupWeylGroup subWeyl;
  charSSAlgMod<CoefficientType> charWRTsubalgebra;
  this->theChaR.SplitCharOverLevi
  (Report, charWRTsubalgebra, splittingParSel, this->parabolicSelectionNonSelectedAreElementsLevi,
   subWeyl, theGlobalVariables);
  Vector<Rational> theHWsimpleCoords, theHWfundCoords;
  std::stringstream out;
  if(Report!=0)
    out << *Report;
  Selection splittingParSelectedInLevi;
  splittingParSelectedInLevi=splittingParSel;
  splittingParSelectedInLevi.InvertSelection();
  if (!splittingParSelectedInLevi.IsSubset(this->parabolicSelectionSelectedAreElementsLevi))
  { out << "The parabolic subalgebra you selected is not a subalgebra of the ambient parabolic subalgebra."
    << " The parabolic has Vectors<Rational> of Levi given by " << splittingParSel.ToString()
    <<" while the ambient parabolic subalgebra has Vectors<Rational> of Levi given by "
    << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
    if (Report!=0)
      *Report=out.str();
    return;
  }
  out << "<br>Parabolic selection: " << splittingParSel.ToString();
  List<List<List<CoefficientType> > > eigenSpacesPerSimpleGenerator;
 // if (false)
  eigenSpacesPerSimpleGenerator.SetSize(splittingParSelectedInLevi.CardinalitySelection);
  Vectors<CoefficientType> theFinalEigenSpace, tempSpace1, tempSpace2;
//  WeylGroup& theWeyL=this->theAlgebra.theWeyl;
  for (int i=0; i<splittingParSelectedInLevi.CardinalitySelection; i++)
  { int theGenIndex=splittingParSelectedInLevi.elements[i]+this->GetOwner().GetRank();
    Matrix<CoefficientType>& currentOp=this->actionsSimpleGensMatrixForM[theGenIndex];
    currentOp.FindZeroEigenSpacE(eigenSpacesPerSimpleGenerator[i], 1, -1, 0, theGlobalVariables);
    if (i==0)
      theFinalEigenSpace.AssignListListCoefficientType(eigenSpacesPerSimpleGenerator[i]);
    else
    { tempSpace1=theFinalEigenSpace;
      tempSpace2.AssignListListCoefficientType(eigenSpacesPerSimpleGenerator[i]);
      theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace, theGlobalVariables);
    }
  }
  out << "<br>Eigenvectors:<table> ";
//  Vector<Rational> zeroRoot;
//  zeroRoot.MakeZero(this->theAlgebra->GetRank());
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";

  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.ElementToStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<CoefficientType> currentWeight;
  Vector<CoefficientType> hwFundCoordsNilPart;
  hwFundCoordsNilPart=this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart-=this->theHWFundamentalCoordS;
  ElementUniversalEnveloping<CoefficientType> currentElt, tempElt;
  for (int j=0; j<theFinalEigenSpace.size; j++)
  { out << "<tr><td>";
    currentElt.MakeZero(*this->theAlgebras, this->indexAlgebra);
    Vector<CoefficientType>& currentVect= theFinalEigenSpace[j];
    int lastNonZeroIndex=-1;
    for (int i=0; i<currentVect.size; i++)
      if (!(currentVect[i].IsEqualToZero()))
      { tempElt.MakeZero(*this->theAlgebras, this->indexAlgebra);
        tempElt.AddMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt*=currentVect[i];
        currentElt+=tempElt;
        lastNonZeroIndex=i;
      }
    currentWeight=subWeyl.AmbientWeyl.GetFundamentalCoordinatesFromSimple
    (this->theGeneratingWordsNonReducedWeights[lastNonZeroIndex]);//<-implicitTypeConversionHere
    currentWeight+=hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.ElementToStringLetterFormat("\\omega")
    << "$&$" << currentVect.ElementToStringLetterFormat("m", true, false) << "$";
    if (currentElt.size>1)
      out << "(";
    if (outputEigenVectors!=0)
      outputEigenVectors->AddOnTop(currentElt);
    if (outputWeightsFundCoords!=0)
      outputWeightsFundCoords->AddOnTop(currentWeight);
    out << currentElt.ToString(&theGlobalVariables.theDefaultFormat);
    if (currentElt.size>1)
      out << ")";
    out << " v_\\lambda";
    out << "</td><td>(weight: "
    << currentWeight.ElementToStringLetterFormat("\\omega") << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  if (Report!=0)
    *Report=out.str();
}

bool CommandList::fSplitFDpartB3overG2CharsOnly
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.theOperation!=theCommands.opList() || theExpression.children.size!=3)
    return theExpression.SetError("Splitting the f.d. part of a B_3-representation over G_2 requires 3 arguments");
  Vector<RationalFunction> theWeightFundCoords;
  Context theContext;
  if (!theExpression.GetVector<RationalFunction>(theWeightFundCoords, theContext, 3, theCommands.fPolynomial, comments))
  { std::stringstream errorStream;
    errorStream << "Failed to extract highest weight in fundamental coordinates from the expression " << theExpression.ToString() << ".";
    return theExpression.SetError(errorStream.str());
  }
  ModuleSSalgebraNew<RationalFunction> theMod;
  Selection selInducing;
  selInducing.init(3);
  for (int i=0; i<theWeightFundCoords.size; i++)
    if (!theWeightFundCoords[i].IsSmallInteger())
      selInducing.AddSelectionAppendNewIndex(i);
  Data tempData;
  tempData.MakeSSAlgebra(theCommands, 'B', 3);
  SemisimpleLieAlgebra& ownerSS=tempData.GetAmbientSSAlgebra();

  std::stringstream out;
//  std::cout << "Highest weight: " << theWeightFundCoords.ToString() << "; Parabolic selection: " << selInducing.ToString();
  theMod.MakeFromHW
  (*ownerSS.owner, ownerSS.indexInOwner, theWeightFundCoords, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0);
  std::string report;
  Selection SelSplittingParSel=selInducing;
  if (SelSplittingParSel.selected[0]!=SelSplittingParSel.selected[2])
  { SelSplittingParSel.AddSelectionAppendNewIndex(0);
    SelSplittingParSel.AddSelectionAppendNewIndex(2);
  }
  out << "Highest weight: " << theWeightFundCoords.ToString() << "; Parabolic selection: " << selInducing.ToString()
  << " common Levi part of G_2 and B_3: " << SelSplittingParSel.ToString() << "<br>";
  Vector<Rational> splittingParSel;
  splittingParSel=SelSplittingParSel;
  theMod.SplitOverLevi(&report, splittingParSel, *theCommands.theGlobalVariableS, 1, 0);
  out << report;
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str(), theContext);
  return true;
}

void CommandList::MakeHmmG2InB3(HomomorphismSemisimpleLieAlgebra& output)
{ Data tempDataB3, tempDataG2;
  tempDataB3.MakeSSAlgebra(*this, 'B', 3);
  tempDataG2.MakeSSAlgebra(*this, 'G', 2);
  output.owners=&this->theObjectContainer.theLieAlgebras;
  output.indexRange=tempDataB3.theIndex;
  output.indexDomain=tempDataG2.theIndex;
  output.theRange().ComputeChevalleyConstantS(*this->theGlobalVariableS);
  output.theDomain().ComputeChevalleyConstantS(*this->theGlobalVariableS);
  ElementSemisimpleLieAlgebra g_2, g_1plusg_3, g_m2, g_m1plusg_m3, tempElt;
  g_2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (13, this->theObjectContainer.theLieAlgebras, tempDataB3.theIndex);
  g_m2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (7, this->theObjectContainer.theLieAlgebras, tempDataB3.theIndex);
  g_1plusg_3.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (12, this->theObjectContainer.theLieAlgebras, tempDataB3.theIndex);
  tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (14, this->theObjectContainer.theLieAlgebras, tempDataB3.theIndex);
  g_1plusg_3+=tempElt;
  g_m1plusg_m3.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (6, this->theObjectContainer.theLieAlgebras, tempDataB3.theIndex);
  tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (8, this->theObjectContainer.theLieAlgebras, tempDataB3.theIndex);
  g_m1plusg_m3+=tempElt;

//  std::cout << "<hr>g_2: " << g_2.ToString();
//  std::cout << "<hr>g_{1}+g_{3}: " << g_1plusg_3.ToString();
//  std::cout << "<hr>g_{-2}: " << g_m2.ToString();
//  std::cout << "<hr>g_{-1}+g_{-3}: " << g_m1plusg_m3.ToString();
  output.imagesSimpleChevalleyGenerators.SetSize(4);
  output.imagesSimpleChevalleyGenerators.TheObjects[1]=g_2;
  output.imagesSimpleChevalleyGenerators.TheObjects[0]=g_1plusg_3;
  output.imagesSimpleChevalleyGenerators.TheObjects[3]=g_m2;
  output.imagesSimpleChevalleyGenerators.TheObjects[2]=g_m1plusg_m3;
  output.ComputeHomomorphismFromImagesSimpleChevalleyGenerators(*this->theGlobalVariableS);
  output.GetRestrictionAmbientRootSystemToTheSmallerCartanSA(output.RestrictedRootSystem, *this->theGlobalVariableS);
}

bool CommandList::fSplitFDpartB3overG2
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.theOperation!=theCommands.opList() || theExpression.children.size!=3)
    return theExpression.SetError("Splitting the f.d. part of a B_3-representation over G_2 requires 3 arguments");
  Vector<RationalFunction> theWeightFundCoords;
  Context theContext;
  if (!theExpression.GetVector<RationalFunction>(theWeightFundCoords, theContext, 3, theCommands.fPolynomial, comments))
  { std::stringstream errorStream;
    errorStream << "Failed to extract highest weight in fundamental coordinates from the expression " << theExpression.ToString() << ".";
    return theExpression.SetError(errorStream.str());
  }
  ModuleSSalgebraNew<RationalFunction> theModCopy;
  Selection selInducing;
  selInducing.init(3);
  for (int i=0; i<theWeightFundCoords.size; i++)
    if (!theWeightFundCoords[i].IsSmallInteger())
      selInducing.AddSelectionAppendNewIndex(i);
  HomomorphismSemisimpleLieAlgebra theHmm;
  theCommands.MakeHmmG2InB3(theHmm);

  std::stringstream out;
//  std::cout << "Highest weight: " << theWeightFundCoords.ToString() << "; Parabolic selection: " << selInducing.ToString();
  theModCopy.MakeFromHW
  (theCommands.theObjectContainer.theLieAlgebras, theHmm.indexRange, theWeightFundCoords, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0);
  std::string report;
  Selection SelSplittingParSel=selInducing;
  if (SelSplittingParSel.selected[0]!=SelSplittingParSel.selected[2])
  { SelSplittingParSel.AddSelectionAppendNewIndex(0);
    SelSplittingParSel.AddSelectionAppendNewIndex(2);
  }
  out << "<br>Highest weight: " << theWeightFundCoords.ToString() << "<br>Parabolic selection: " << selInducing.ToString()
  << "<br>common Levi part of G_2 and B_3: " << SelSplittingParSel.ToString();
  Vector<Rational> splittingParSel;
  splittingParSel=SelSplittingParSel;
  List<ElementUniversalEnveloping<RationalFunction> > outputEigenWords;
  Vectors<RationalFunction> outputWeightsFundCoordS;
  Vectors<RationalFunction> outputWeightsSimpleCoords;
  theCommands.theObjectContainer.theCategoryOmodules.AddOnTopNoRepetition(theModCopy);
  int theModIndex=theCommands.theObjectContainer.theCategoryOmodules.IndexOfObject(theModCopy);
  ModuleSSalgebraNew<RationalFunction>& theMod=
  theCommands.theObjectContainer.theCategoryOmodules[theModIndex];
  theMod.SplitOverLevi(&report, splittingParSel, *theCommands.theGlobalVariableS, 1, 0, &outputEigenWords, &outputWeightsFundCoordS);
  //out << report;
//  int numVars=theWeightFundCoords[0].NumVars;
  Vector<RationalFunction> ih1, ih2;
  ih1="(1,0,2)";
  ih2="(0,3,0)";
  Vectors<RationalFunction> g2Weights, g2DualWeights;
  g2Weights.SetSize(outputWeightsFundCoordS.size);
  g2DualWeights.SetSize(outputWeightsFundCoordS.size);
  Matrix<Rational> invertedG2cartanMat;
  invertedG2cartanMat=theHmm.theDomain().theWeyl.CartanSymmetric;
  invertedG2cartanMat.Invert();
  outputWeightsSimpleCoords=theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(outputWeightsFundCoordS);
  for (int i=0; i< outputWeightsSimpleCoords.size; i++)
  { Vector<RationalFunction>& currentWeight=outputWeightsSimpleCoords[i];
    Vector<RationalFunction>& currentG2Weight=g2Weights[i];
    Vector<RationalFunction>& currentG2DualWeight=g2DualWeights[i];
    currentG2DualWeight.SetSize(2);
    currentG2DualWeight[0]=theHmm.theRange().theWeyl.RootScalarCartanRoot(ih1, currentWeight);//<-initialize with scalar products
    currentG2DualWeight[1]=theHmm.theRange().theWeyl.RootScalarCartanRoot(ih2, currentWeight);//<-initialize with scalar products
    invertedG2cartanMat.ActOnVectorColumn(currentG2DualWeight, currentG2Weight);//<-g2weight is now computed;
  }
  ElementUniversalEnveloping<RationalFunction> theG2Casimir, theG2CasimirCopy, imageCasimirInB3, bufferCasimirImage, tempElt;
  theG2Casimir.MakeCasimir(theHmm.theDomain(), *theCommands.theGlobalVariableS, 1, 0);
  Vector<RationalFunction> highestWeightG2dualCoords=*g2DualWeights.LastObject();
  List<RationalFunction> theChars;
  theChars.SetSize(outputWeightsFundCoordS.size);
//  RationalFunction& baseChar=*theChars.LastObject();
//  baseChar.MakeZero(numVars, theCommands.theGlobalVariableS);
//  theG2Casimir.ModOutVermaRelations(theCommands.theGlobalVariableS, &highestWeightG2dualCoords, 1, 0);
//  if (!theG2Casimir.IsEqualToZero())
//    baseChar=theG2Casimir.theCoeffs[0];
//  out << "<br>Base G_2-character: " << baseChar.ToString() << " corresponding to g2-dual weight " << highestWeightG2dualCoords.ToString();
//  highestWeightG2
  bool useLatex=theCommands.theGlobalVariableS->theDefaultFormat.flagUseLatex;
  theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds=400;
  out
  << "<table border=\"1\"><tr><td>word</td><td>B_3-weight simple coords</td><td>B_3-weight fund. coords </td>"
  << "<td>G_2 simple coordinates</td><td>G2-fund. coords</td><td>G2-dual coordinates</td><td>character</td></tr>";
  for (int i=0; i< outputWeightsSimpleCoords.size; i++)
  { Vector<RationalFunction>& currentWeightSimpleB3coords=outputWeightsSimpleCoords[i];
    Vector<RationalFunction>& currentWeightFundB3coords=outputWeightsFundCoordS[i];
    Vector<RationalFunction>& currentG2Weight=g2Weights[i];
    Vector<RationalFunction>& currentG2DualWeight=g2DualWeights[i];
    out << "<tr><td>" << outputEigenWords[i].ToString() << "</td><td> "
    << currentWeightSimpleB3coords.ToString() << "</td><td> " << currentWeightFundB3coords.ToString()
    << "</td><td>" << currentG2Weight.ToString() << "</td><td> "
    << theHmm.theDomain().theWeyl.GetFundamentalCoordinatesFromSimple(currentG2Weight).ToString()
    << "</td><td> " << currentG2DualWeight.ToString() << "</td>";
    theG2CasimirCopy=theG2Casimir;
    theG2CasimirCopy.ModOutVermaRelations(theCommands.theGlobalVariableS, &currentG2DualWeight, 1,0);
    theChars[i]=theG2CasimirCopy.theCoeffs[0];
    out << "<td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayRCL(theChars[i].ToString()) << "</td>";
    out << "</tr>";
  }
  out << "</table>";
  out << "<br>";
//  theCommands.theGlobalVariableS->MaxAllowedComputationTimeInSeconds=1000;
  out << "<table border=\"1\"><tr><td>weight</td><td>the elt closed form</td><td>the elt</td></tr>";
  List<ElementTensorsGeneralizedVermas<RationalFunction> > theEigenVectors;
  ElementTensorsGeneralizedVermas<RationalFunction> intermediateElt;
  theEigenVectors.SetSize(g2Weights.size);
  theEigenVectors.LastObject()->MakeHWV(theCommands.theObjectContainer.theCategoryOmodules, theModIndex, 1);
  Vector<RationalFunction> weightDifference, weightDifferenceDualCoords;
  theHmm.ApplyHomomorphism(theG2Casimir, imageCasimirInB3, *theCommands.theGlobalVariableS);
  theG2Casimir.checkConsistency();
  imageCasimirInB3.checkConsistency();
  for (int k=0; k<g2Weights.size; k++)
  { out << "<tr><td>" << g2Weights[k].ToString() << "</td><td>";
    ElementTensorsGeneralizedVermas<RationalFunction>& currentTensorElt=theEigenVectors[k];
    intermediateElt=*theEigenVectors.LastObject();
    intermediateElt.MultiplyOnTheLeft(outputEigenWords[k], currentTensorElt, theCommands.theObjectContainer.theCategoryOmodules,
    theHmm.theRange(), *theCommands.theGlobalVariableS, 1, 0);
    intermediateElt=currentTensorElt;
    std::stringstream formulaStream1;
    for (int j=0; j<g2Weights.size; j++)
    { weightDifference=g2Weights[j] - g2Weights[k];
      if (weightDifference.IsPositive())
      { formulaStream1 << "(i(\\bar c) - " << theChars[j].ToString() <<  ")";
        theG2CasimirCopy=imageCasimirInB3;
        tempElt.MakeConst(theChars[j], theCommands.theObjectContainer.theLieAlgebras, theHmm.indexRange);
        theG2CasimirCopy-=tempElt;
        intermediateElt.MultiplyOnTheLeft
        (theG2CasimirCopy, currentTensorElt, theCommands.theObjectContainer.theCategoryOmodules,
         theHmm.theRange(), *theCommands.theGlobalVariableS, 1, 0);
        intermediateElt=currentTensorElt;
      }
    }
    formulaStream1 << "v_\\lambda";
    out << CGI::GetHtmlMathSpanNoButtonAddBeginArrayRCL(formulaStream1.str())
    << "</td><td>" <<  CGI::GetHtmlMathSpanNoButtonAddBeginArrayRCL(currentTensorElt.ToString()) << "</td></tr>";
  }
  out << "</table>";
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str(), theContext);
  return true;
}

class DoxygenInstance
{
  public:
  ElementTensorsGeneralizedVermas<RationalFunction> doXyCanYouParseME;
};
