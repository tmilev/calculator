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

void ParserNode::EvaluateFunction(GlobalVariables& theGlobalVariables)
{ switch(this->intValue)
  { case Parser::functionGCD: this->EvaluateGCDorLCM(theGlobalVariables); break;
    case Parser::functionLCM: this->EvaluateGCDorLCM(theGlobalVariables); break;
    case Parser::functionEigen: this->EvaluateEigen(theGlobalVariables); break;
    case Parser::functionSecretSauce: this->EvaluateSecretSauce(theGlobalVariables); break;
    case Parser::functionSecretSauceOrdered: this->EvaluateSecretSauceOrdered(theGlobalVariables); break;
    case Parser::functionWeylDimFormula: this->EvaluateWeylDimFormula(theGlobalVariables); break;
    case Parser::functionOuterAutos: this->EvaluateOuterAutos(theGlobalVariables); break;
    case Parser::functionMod: this->EvaluateModVermaRelations(theGlobalVariables); break;
    case Parser::functionInvariants: this->EvaluateInvariants(theGlobalVariables); break;
    case Parser::functionEigenUE: this->EvaluateEigenUEUserInputGenerators(theGlobalVariables); break;
    case Parser::functionEigenUEofWeight: this->EvaluateEigenUEDefaultOperators(theGlobalVariables); break;
    default: this->SetError(this->errorUnknownOperation); break;
  }
}

void ParserNode::EvaluateInvariants(GlobalVariables& theGlobalVariables)
{ if (this->owner->DefaultWeylLetter!='B' && this->owner->DefaultWeylRank!=3)
  { this->SetError(this->errorUnknownOperation);
    return;
  }
  if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (theArgument.ExpressionType!=this->typeIntegerOrIndex)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  if (theArgument.intValue<0)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  std::stringstream out;
  SSalgebraModule theModule;
  theModule.InduceFromEmbedding(out, this->owner->theHmm, theGlobalVariables);
  theModule.ComputeInvariantsOfDegree(theArgument.intValue, out, theGlobalVariables);
  this->ExpressionType=this->typePoly;
  if (theModule.invariantsFound.size>0)
    this->polyValue.GetElement().operator=(theModule.invariantsFound.TheObjects[0]);
  this->outputString=out.str();
}

void ParserNode::EvaluateEigenUEDefaultOperators(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (theArgument.ExpressionType!=this->typeArray)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  int RangeRank=this->owner->theHmm.theRange.GetRank();
  if (theArgument.children.size!=RangeRank)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  root theWeight;
  theWeight.SetSizeExpandOnTopLight(RangeRank);
  for (int i=0; i< theArgument.children.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[theArgument.children.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeRational))
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
    theWeight.TheObjects[i]=currentNode.rationalValue;
  }
  std::stringstream log;
  List<ElementUniversalEnveloping> theGenerators;
  int rankDomain=this->owner->theHmm.theDomain.GetRank();
  theGenerators.SetSizeExpandOnTopNoObjectInit(rankDomain);
  roots theHsToBeQuotioned;
  theHsToBeQuotioned.SetSizeExpandOnTopNoObjectInit(rankDomain);
  for (int i=0; i<rankDomain; i++)
  { ElementUniversalEnveloping& currentUE=theGenerators.TheObjects[i];
    currentUE.AssignElementLieAlgebra(this->owner->theHmm.imagesSimpleChevalleyGenerators.TheObjects[i], 0, this->owner->theHmm.theRange);
    int currentHindex=this->owner->theHmm.theDomain.GetNumPosRoots()+i;
    theHsToBeQuotioned.TheObjects[i]=this->owner->theHmm.imagesAllChevalleyGenerators.TheObjects[currentHindex].Hcomponent;
    log << "<br>" << this->owner->theHmm.imagesAllChevalleyGenerators.TheObjects[currentHindex].ElementToStringNegativeRootSpacesFirst(false, false, this->owner->theHmm.theRange) << " current h index: " << currentHindex << " corresponding root: " << theHsToBeQuotioned.TheObjects[i].ElementToString();
  }
  List<ElementUniversalEnveloping> theEigenvectors;
  this->owner->theHmm.theRange.ComputeCommonAdEigenVectorsFixedWeight(theWeight, theHsToBeQuotioned, theGenerators, theEigenvectors, log, theGlobalVariables);
  this->ExpressionType=this->typeString;
  this->outputString=log.str();
}

void ParserNode::EvaluateEigenUEUserInputGenerators(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& ArrayOfArguments= this->owner->TheObjects[this->children.TheObjects[0]];
  if (ArrayOfArguments.ExpressionType!=this->typeArray)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& firstArgument=this->owner->TheObjects[ArrayOfArguments.children.TheObjects[0]];
  if (!(firstArgument.ExpressionType==this->typeIntegerOrIndex && firstArgument.intValue>=1) || ArrayOfArguments.children.size<=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  List<ElementUniversalEnveloping> theGenerators;
  theGenerators.SetSizeExpandOnTopNoObjectInit(ArrayOfArguments.children.size-1);
  for(int i=1; i<ArrayOfArguments.children.size; i++)
  { ParserNode& current=this->owner->TheObjects[ArrayOfArguments.children.TheObjects[i]];
    if (!current.ConvertToType(this->typeUEelement))
    { this->SetError(this->errorBadOrNoArgument);
      return;
    } else
      theGenerators.TheObjects[i-1].operator=(current.UEElement.GetElement());
  }
  std::stringstream log;
  List<ElementUniversalEnveloping> theEigenvectors, theNilradical;
  int numPosRoots= this->owner->theHmm.theRange.GetNumPosRoots();
  theNilradical.SetSizeExpandOnTopNoObjectInit(numPosRoots);
  for (int i=0; i<numPosRoots; i++)
  { ElementUniversalEnveloping& currentUE=theNilradical.TheObjects[i];
    currentUE.MakeOneGeneratorCoeffOne(i, 0, this->owner->theHmm.theRange);
  }
  this->owner->theHmm.theRange.ComputeCommonAdEigenVectors(firstArgument.intValue, theGenerators, theNilradical, theEigenvectors, log, theGlobalVariables);
  this->ExpressionType=this->typeString;
  this->outputString=log.str();
}

void ParserNode::EvaluateModVermaRelations(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (!this->ConvertChildrenToType(this->typeUEelement) )
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  this->UEElement.GetElement()=theArgument.UEElement.GetElement();
  this->outputString.append(this->UEElement.GetElement().ElementToString(true));
  this->UEElement.GetElement().Simplify();
  this->UEElement.GetElement().ModOutVermaRelations();
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
    case ';': this->TokenBuffer.AddObjectOnTop(Parser::tokenEndStatement); this->ValueBuffer.AddObjectOnTop(0); return true;
    case ':': this->TokenBuffer.AddObjectOnTop(Parser::tokenColon); this->ValueBuffer.AddObjectOnTop(0); return true;
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
  if (input=="f")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenF);
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
  if (input=="eigenVerma")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionEigenUE);
    return true;
  }
  if (input=="eigenVermaOfWeight")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionEigenUEofWeight);
    return true;
  }
  if (input=="\\mapsto")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenMapsTo);
    this->ValueBuffer.AddObjectOnTop(0);
    return true;
  }
  if (input =="secretSauce")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(this->functionSecretSauce);
    return true;
  }
  if (input=="secretSauceOrdered")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(this->functionSecretSauceOrdered);
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
  if (input=="invariant")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(Parser::functionInvariants);
    return true;
  }
  return false;
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

void ElementUniversalEnveloping::ModOutVermaRelations(bool SubHighestWeightWithZeroes)
{ MonomialUniversalEnveloping tempMon;
  ElementUniversalEnveloping output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelations(SubHighestWeightWithZeroes);
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

void ElementUniversalEnvelopingOrdered::ModOutVermaRelations(bool SubHighestWeightWithZeroes)
{ MonomialUniversalEnvelopingOrdered tempMon;
  ElementUniversalEnvelopingOrdered output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelations(SubHighestWeightWithZeroes);
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

void ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements
(int numVars, List<ElementUniversalEnveloping>& theElements, List<rootPoly>& outputCoordinates, ElementUniversalEnveloping& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping tempMon;
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
  PolynomialOutputFormat PolyFormatLocal;
  if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->Coefficient.IsEqualToZero())
    tempS="0";
  else
    tempS = this->Coefficient.ElementToString(PolyFormatLocal);
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
      out << thePower.ElementToString(PolyFormatLocal);
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

void MonomialUniversalEnveloping::ModOutVermaRelations(bool SubHighestWeightWithZeroes)
{ int numPosRoots=this->owner->theWeyl.RootsOfBorel.size;
  int theDimension=this->owner->theWeyl.CartanSymmetric.NumRows;
  int theNumVars=this->Coefficient.NumVars;
  this->ComputeDebugString();
  if (theNumVars<theDimension && !SubHighestWeightWithZeroes)
  { this->SetNumVariables(theDimension);
    theNumVars=theDimension;
  }
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->owner->ChevalleyGeneratorIndexToRootIndex(generatorsIndices.TheObjects[i]);
    //ChevalleyGeneratorToRootIndex returns a negative value if the generator is an element of the Cartan
    if (IndexCurrentGenerator<numPosRoots && IndexCurrentGenerator>=0)
    { this->Nullify(theNumVars, *this->owner);
      return;
    }
    if (IndexCurrentGenerator>=numPosRoots)
      return;
    if (IndexCurrentGenerator<0)
    { if (this->Powers.TheObjects[i].TotalDegree()!=0)
        return;
      if (SubHighestWeightWithZeroes)
      { this->Nullify(theNumVars, *this->owner);
        return;
      }
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

void MonomialUniversalEnvelopingOrdered::ModOutVermaRelations(bool SubHighestWeightWithZeroes)
{ int numPosRoots=this->owner->theOwner.GetNumPosRoots();
  int theDimension=this->owner->theOwner.GetRank();
  int theNumVars=this->Coefficient.NumVars;
  this->ComputeDebugString();
  if (theNumVars<theDimension && !SubHighestWeightWithZeroes)
  { this->SetNumVariables(theDimension);
    theNumVars=theDimension;
  }
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices.TheObjects[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->Nullify(theNumVars, *this->owner);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { if (this->Powers.TheObjects[i].TotalDegree()!=0)
        return;
      if (SubHighestWeightWithZeroes)
      { this->Nullify(theNumVars, *this->owner);
        return;
      }
      int theDegree = this->Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort;
      PolynomialRationalCoeff tempP;
      root tempRoot;
      tempRoot.Assign(this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent);
      tempP.MakeLinPolyFromRoot(tempRoot);
      tempP.SetNumVariablesSubDeletedVarsByOne(numPosRoots+theDimension);
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
{ this->indexParentNode=-1;
  this->indexInOwner=-1;
  this->intValue=0;
  this->ErrorType=this->errorNoError;
  this->Operation= Parser::tokenEmpty;
  this->Evaluated=false;
  this->ExpressionType=this->typeUndefined;
  this->children.size=0;
  this->rationalValue.MakeZero();
  if (this->owner!=0)
    this->ContextLieAlgebra=&this->owner->theHmm.theRange;
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

void ElementUniversalEnveloping::operator*=(const PolynomialRationalCoeff& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.MultiplyBy(other);
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
{ //this->ComputeDebugString();
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
//  Accum.ComputeDebugString();
  this->operator=(Accum);
 // this->ComputeDebugString();
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
  PolynomialOutputFormat PolyFormatLocal;
  out << "(";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString(PolyFormatLocal);
    if (i<this->size-1)
      out << ",";
  }
  out << ")";
  return out.str();
}

std::string RationalFunction::ElementToString(bool useLatex, bool breakLinesLatex)
{ std::stringstream out;
  PolynomialOutputFormat PolyFormatLocal;
  bool hasDenominator=!this->Denominator.IsEqualToOne();
  if (hasDenominator && useLatex)
    out << "\\frac{";
  out << this->Numerator.ElementToString(breakLinesLatex, PolyFormatLocal);
  if (hasDenominator)
  { if (useLatex)
      out << "}{";
    out << this->Denominator.ElementToString(breakLinesLatex, PolyFormatLocal);
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
  List<Monomial<Rational> > tempList;
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

std::string EigenVectorComputation::ComputeAndReturnStringOrdered
(GlobalVariables& theGlobalVariables, Parser& theParser, int NodeIndex)
{ std::stringstream out;
  ElementUniversalEnvelopingOrdered theElt, tempElt1, tempElt2;
  //this->MakeGenericMonomialBranchingCandidate(theParser.theHmm, theElt, theGlobalVariables);
  this->MakeGenericVermaElementOrdered(theElt, theParser.testAlgebra);
  int theDomainRank=theParser.theHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
//  int numDomainPosRoots= theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
  //int dimQuotient=numRangePosRoots-theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  this->theSystem.init(0,0);
  this->theOperators.size=0;
  this->theExponentShifts.size=0;
  theParser.TheObjects[NodeIndex].ExpressionType=ParserNode::typeArray;
  for (int i=0; i<theDomainRank; i++)
  { tempElt1.AssignElementLieAlgebra(theParser.theHmm.imagesSimpleChevalleyGenerators.TheObjects[i], theRangeRank+numRangePosRoots, theParser.testAlgebra);
    tempElt1.LieBracketOnTheRight(theElt, tempElt2);
    tempElt2.Simplify();
    std::cout << "<br>" << tempElt1.ElementToString();
    out << "<div class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
    out << "[" << tempElt1.ElementToString(true) << "," << theElt.ElementToString(true) << "]=" << tempElt2.ElementToString(true);
    out << "\\end{eqnarray*}</div>";
    std::cout << "<br>" << tempElt2.ElementToString();
    tempElt2.ModOutVermaRelations();
    out << "mod Verma rels:";
    out << "<div class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
    out << tempElt2.ElementToString(true);
    out << "\\end{eqnarray*}</div>";
    this->DetermineEquationsFromResultLieBracketEquationsPerTargetOrdered(theParser, NodeIndex, theElt, tempElt2, out, theGlobalVariables);
    theParser.ExtendOnTop(1);
    ParserNode& currentOutputNode=*theParser.LastObject();
    currentOutputNode.ExpressionType=ParserNode::typeWeylAlgebraElement;
    currentOutputNode.WeylAlgebraElement.GetElement().Assign(this->theOperators.TheObjects[i]);
    theParser.TheObjects[NodeIndex].array.GetElement().AddObjectOnTop(theParser.size-1);
  }
  this->theExponentShiftsTargetPerSimpleGenerator.CollectionToRoots(this->theExponentShifts);
  out << "<br><br> And the total rank is: " << this->theExponentShifts.GetRankOfSpanOfElements(theGlobalVariables);
  out << "<div class=\"math\" scale=\"50\">" << this->theSystem.ElementToString(false, true) << "</div>";
  return out.str();
}

std::string EigenVectorComputation::ComputeAndReturnStringNonOrdered
(GlobalVariables& theGlobalVariables, Parser& theParser)
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
  PolynomialOutputFormat PolyFormatLocal;
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
      out << theDiffPoly.ElementToString(PolyFormatLocal) << " ";
      if (theDiffPoly.size>1)
        currentShift.TheObjects[j]=-10000;
      if (theDiffPoly.size==0)
        currentShift.TheObjects[j]=0;
      if (theDiffPoly.size==1)
        currentShift.TheObjects[j]=-theDiffPoly.TheObjects[0].Coefficient;
    }
    this->GetDiffOperatorFromShiftAndCoefficient(theMon.Coefficient, currentShift, tempWeyl, theGlobalVariables);
    currentOperator.Add(tempWeyl);
  }
  out << "<br>... and the operator is... <div class=\"math\" scale=\"50\"> \\begin{eqnarray*}&&" << currentOperator.ElementToString(true) << "\\end{eqnarray*}</div>";
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

void EigenVectorComputation::DetermineEquationsFromResultLieBracketEquationsPerTargetOrdered
  (Parser& theParser, int nodeIndex, ElementUniversalEnvelopingOrdered& theStartingGeneric, ElementUniversalEnvelopingOrdered& theElt, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
//  int dimQuotient=theParser.theHmm.theRange.theWeyl.RootsOfBorel.size- theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numCoeffVars=theRangeRank+numRangePosRoots;
  PolynomialRationalCoeff theDiffPoly;
  MonomialUniversalEnvelopingOrdered& originalMon= theStartingGeneric.TheObjects[0];
  this->theExponentShiftsTargetPerSimpleGenerator.SetSizeExpandOnTopNoObjectInit(this->theExponentShiftsTargetPerSimpleGenerator.size+1);
  roots& currentTargetShifts =*this->theExponentShiftsTargetPerSimpleGenerator.LastObject();
  currentTargetShifts.SetSizeExpandOnTopNoObjectInit(theElt.size);
  ElementWeylAlgebra tempWeyl;
  this->theOperators.SetSizeExpandOnTopNoObjectInit(this->theOperators.size+1);
  ElementWeylAlgebra& currentOperator = *this->theOperators.LastObject();
  currentOperator.Nullify(numCoeffVars);
  PolynomialOutputFormat PolyFormatLocal;
  for (int i=0; i<theElt.size; i++)
  { MonomialUniversalEnvelopingOrdered& theMon= theElt.TheObjects[i];
    out << "<br>\nDifference in monomial index " << i << ": ";
    root& currentShift= currentTargetShifts.TheObjects[i];
    currentShift.MakeZero(numRangePosRoots);
    for (int j=0; j<numRangePosRoots; j++)
    { theDiffPoly = originalMon.Powers.TheObjects[j] - theMon.Powers.TheObjects[j];
      out << theDiffPoly.ElementToString(PolyFormatLocal) << " ";
      if (theDiffPoly.size>1)
        currentShift.TheObjects[j]=-10000;
      if (theDiffPoly.size==0)
        currentShift.TheObjects[j]=0;
      if (theDiffPoly.size==1)
        currentShift.TheObjects[j]=-theDiffPoly.TheObjects[0].Coefficient;
    }
    this->GetDiffOperatorFromShiftAndCoefficient(theMon.Coefficient, currentShift, tempWeyl, theGlobalVariables);
    currentOperator.Add(tempWeyl);
  }
  out << "<br>and the rank is: " << currentTargetShifts.GetRankOfSpanOfElements(theGlobalVariables);
  Selection tempSel;
  if (this->AreUnimodular(currentTargetShifts, tempSel, theGlobalVariables))
  { roots tempRoots, tempRoots2;
    currentTargetShifts.SubSelection(tempSel, tempRoots);
    out << "<br> one unimodular basis: " << tempRoots.ElementToString(false, true, false);
    currentTargetShifts.GetCoordsInBasis(tempRoots, tempRoots2, theGlobalVariables);
    out << "<br> in the above unimodular basis the coordinates are:<br> " << tempRoots2.ElementToString(false, true, true);
  }
  out << "<br>... and the operator is... <div class=\"math\" scale=\"50\"> \\begin{eqnarray*}&&" << currentOperator.ElementToString(true) << "\\end{eqnarray*}</div>";
  GeneralizedPolynomialRational tempGP;
  GeneralizedMonomialRational tempGM;
  tempGM.MakeGenericMon(numRangePosRoots+theRangeRank, theRangeRank);
  out << "<br>...and the generic monomial is: " << tempGM.ElementToString(PolyFormatLocal);
  this->WeylElementActsOnGeneralizedMonomial(currentOperator, tempGM, tempGP);
  out << "<br> ...and the action on the generic monomial is: <div class=\"math\">\\begin{eqnarray*}&&" << tempGP.ElementToString(PolyFormatLocal) << "\\end{eqnarray*}</div>";

  /*root tempRoot;
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
  }*/
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

void EigenVectorComputation::MakeGenericVermaElementOrdered(ElementUniversalEnvelopingOrdered& theElt, SemisimpleLieAlgebraOrdered& owner)
{ int numPosRoots= owner.theOwner.theWeyl.RootsOfBorel.size;
  int theRank=owner.theOwner.GetRank();
  theElt.Nullify(owner);
  MonomialUniversalEnvelopingOrdered tempMon;
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
(PolynomialRationalCoeff& theCoeff, root& theShift, ElementWeylAlgebra& output, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff AccumDiffPart, tempP, tempP2, AccumPolyPart;
  Monomial<Rational> currentMon;
  int dimCartan=theCoeff.NumVars- theShift.size;
  AccumDiffPart.MakeNVarConst(theCoeff.NumVars, (Rational) 1);
  AccumPolyPart.MakeNVarConst(theCoeff.NumVars, (Rational) 1);
  output.MakeConst(theCoeff.NumVars, (Rational) 1);
  ElementWeylAlgebra tempW;
  PolynomialOutputFormat PolyFormatLocal;
  for (int i=0; i<theShift.size; i++)
    if (theShift.TheObjects[i].NumShort<0)
      for (int j=0; j<-theShift.TheObjects[i].NumShort; j++)
      { tempP.MakeNVarDegOnePoly(theCoeff.NumVars, i+dimCartan, (Rational) 1, (Rational) -j);
        AccumDiffPart.MultiplyBy(tempP);
        tempW.Makedi(i+dimCartan, theCoeff.NumVars);
        output.MultiplyOnTheRight(tempW, theGlobalVariables);
      }
    else if (theShift.TheObjects[i].NumShort>0)
    { tempP.MakeMonomialOneLetter(theCoeff.NumVars, i+dimCartan, theShift.TheObjects[i].NumShort, (Rational) 1);
      AccumPolyPart.MultiplyBy(tempP);
    }
  std::cout << "<br>" << theCoeff.ElementToString(false, PolyFormatLocal) << "  " << theShift.ElementToString() << ": " << AccumDiffPart.ElementToString(false, PolyFormatLocal);
  theCoeff.DivideBy(AccumDiffPart, tempP, tempP2);
  if (!tempP2.IsEqualToZero())
    std::cout << "  problem problem problem!!!!";
  else
  { tempW.AssignPolynomial(tempP);
    output.MultiplyOnTheLeft(tempW, theGlobalVariables);
  }
  std::cout << " poly part: " << AccumPolyPart.ElementToString(false, PolyFormatLocal);
  tempW.AssignPolynomial(AccumPolyPart);
  output.MultiplyOnTheLeft(tempW, theGlobalVariables);
  //std::cout << "<div class=\"math\" scale=\"50\">"  << output.ElementToString(true) << "</div>";
  //while(!remainingCoeff.IsEqualToZero())
  //{ currentMon=remainingCoeff.TheObjects[remainingCoeff.GetIndexMaxMonomialTotalDegThenLexicographic()];

    //remainingCoeff.AddMonomial(
  //}
}

void SSalgebraModule::InduceFromEmbedding(std::stringstream& out, HomomorphismSemisimpleLieAlgebra& theHmm, GlobalVariables& theGlobalVariables)
{ this->theAlgebra.Assign(theHmm.theDomain);
  this->ambientAlgebrA.Assign(theHmm.theRange);
  int theDomainRank=this->theAlgebra.theWeyl.CartanSymmetric.NumRows;
  int numPosRootsDomain=this->theAlgebra.theWeyl.RootsOfBorel.size;
  int numGeneratorsDomain= theDomainRank+numPosRootsDomain*2;
  int numGeneratorsRange= theHmm.theRange.theWeyl.CartanSymmetric.NumRows+ theHmm.theRange.theWeyl.RootsOfBorel.size*2;
  List<ElementSimpleLieAlgebra> posGenerators, negGenerators, allSimpleGenerators;
  posGenerators.SetSizeExpandOnTopNoObjectInit(theDomainRank);
  negGenerators.SetSizeExpandOnTopNoObjectInit(theDomainRank);
  allSimpleGenerators.SetSizeExpandOnTopNoObjectInit(theDomainRank*2);
  for (int i=0; i<theDomainRank; i++)
  { posGenerators.TheObjects[i] =theHmm.imagesAllChevalleyGenerators.TheObjects[numPosRootsDomain+theDomainRank+i];
    negGenerators.TheObjects[i] =theHmm.imagesAllChevalleyGenerators.TheObjects[numPosRootsDomain-1-i];
    allSimpleGenerators.TheObjects[i]=posGenerators.TheObjects[i];
    allSimpleGenerators.TheObjects[i+theDomainRank]=negGenerators.TheObjects[i];
  }
  List<MatrixLargeRational> negGeneratorsMatForm, posGeneratorsMatForm, allSimpleGeneratorsMatForm;
  this->ConvertElementsToAdMatrixFormNegativeRootSpacesFirst(negGenerators, theHmm.theRange, negGeneratorsMatForm);
  this->ConvertElementsToAdMatrixFormNegativeRootSpacesFirst(posGenerators, theHmm.theRange, posGeneratorsMatForm);
  this->ConvertElementsToAdMatrixFormNegativeRootSpacesFirst(allSimpleGenerators, theHmm.theRange, allSimpleGeneratorsMatForm);
  rootsCollection theModules;
  roots newlyFoundStartingVectors, BasisAllFoundElements, BasisNewlyFoundElements;
  root currentElementRootForm, oneRootFromDomainAlgebra;
  ElementSimpleLieAlgebra currentElt;
  int IndexFirstModuleNotEqualToDomain=-1;
  theHmm.imagesAllChevalleyGenerators.TheObjects[0].ElementToVectorNegativeRootSpacesFirst(oneRootFromDomainAlgebra);
  this->moduleElementsEmbedded.MakeActualSizeAtLeastExpandOnTop(numGeneratorsRange);
  this->moduleElementsEmbedded.size=0;
  for (int i=0; i<numGeneratorsRange; i++)
  { currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, theHmm.theRange);
    currentElt.ElementToVectorNegativeRootSpacesFirst(currentElementRootForm);
    if (!BasisAllFoundElements.LinSpanContainsRoot(currentElementRootForm, theGlobalVariables))
    { this->ExtractHighestWeightVectorsFromVector(currentElementRootForm, newlyFoundStartingVectors, posGeneratorsMatForm, negGeneratorsMatForm);
      for (int j=0; j<newlyFoundStartingVectors.size; j++)
        if (!BasisAllFoundElements.LinSpanContainsRoot(newlyFoundStartingVectors.TheObjects[j], theGlobalVariables))
        { currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(newlyFoundStartingVectors.TheObjects[j], theHmm.theRange);
          out << "<br><br> starting vector: " << currentElt.ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange);
          this->GenerateSubmoduleFromRootNegativeRootSpacesFirst(newlyFoundStartingVectors.TheObjects[j], BasisNewlyFoundElements, allSimpleGeneratorsMatForm, theGlobalVariables);
          theModules.AddObjectOnTop(BasisNewlyFoundElements);
          BasisAllFoundElements.AddListOnTop(BasisNewlyFoundElements);
          out << "<br>basis of generated module: ";
          for (int k=0; k<BasisNewlyFoundElements.size; k++)
          { currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(BasisNewlyFoundElements.TheObjects[k], theHmm.theRange);
            out << "<br>" << currentElt.ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange);
          }
          if (IndexFirstModuleNotEqualToDomain==-1)
            if (!theModules.LastObject()->LinSpanContainsRoot(oneRootFromDomainAlgebra, theGlobalVariables))
            { IndexFirstModuleNotEqualToDomain=theModules.size-1;
              for (int k=0; k<BasisNewlyFoundElements.size; k++)
              { this->moduleElementsEmbedded.SetSizeExpandOnTopNoObjectInit(this->moduleElementsEmbedded.size+1);
                this->moduleElementsEmbedded.LastObject()->AssignVectorNegRootSpacesCartanPosRootSpaces(BasisNewlyFoundElements.TheObjects[k], theHmm.theRange);
              }
            }
        }
    }
    if (BasisAllFoundElements.size==numGeneratorsRange)
      break;
  }
  roots theAlgebra;
  roots& theModule= theModules.TheObjects[IndexFirstModuleNotEqualToDomain];
  this->actionsNegativeRootSpacesCartanPositiveRootspaces.SetSizeExpandOnTopNoObjectInit(numGeneratorsDomain);
  ElementSimpleLieAlgebra tempElt;
  for (int j=0; j<numGeneratorsDomain; j++)
  { MatrixLargeRational& theMat=this->actionsNegativeRootSpacesCartanPositiveRootspaces.TheObjects[j];
    ElementSimpleLieAlgebra& currentGenerator=theHmm.imagesAllChevalleyGenerators.TheObjects[j];
    this->ComputeAdMatrixFromModule(currentGenerator, theModule, theHmm.theRange, theMat, theGlobalVariables);
    out << "<br> <div class=\"math\" scale=\"50\">" << currentGenerator.ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange) << "\\to" << theMat.ElementToString(false, true) <<"</div>";
  }
}

void SSalgebraModule::ComputeAdMatrixFromModule
  (ElementSimpleLieAlgebra& theElt, roots& theModule, const SemisimpleLieAlgebra& ambientLieAlgebra, MatrixLargeRational& output,
   GlobalVariables& theGlobalVariables)
{ ElementSimpleLieAlgebra tempElt1, tempElt2;
  root tempRoot1, tempRoot2;
  output.init(theModule.size, theModule.size);
  for (int i=0; i<theModule.size; i++)
  { tempElt1.AssignVectorNegRootSpacesCartanPosRootSpaces(theModule.TheObjects[i], ambientLieAlgebra);
    ambientLieAlgebra.LieBracket(theElt, tempElt1, tempElt2);
    tempElt2.ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    assert(theModule.LinSpanContainsRoot(tempRoot1, theGlobalVariables));
    tempRoot1.GetCoordsInBasis(theModule, tempRoot2, theGlobalVariables);
    for (int j=0; j<theModule.size; j++)
      output.elements[j][i]=tempRoot2.TheObjects[j];
  }
}

void SSalgebraModule::GenerateSubmoduleFromRootNegativeRootSpacesFirst
  (root& input, roots& output, List<MatrixLargeRational>& allSimpleGenerators, GlobalVariables& theGlobalVariables)
{ output.size=0;
  output.AddObjectOnTop(input);
  root tempRoot;
  for (int lowestNonExplored=0; lowestNonExplored<output.size; lowestNonExplored++)
    for (int i=0; i<allSimpleGenerators.size; i++)
    { allSimpleGenerators.TheObjects[i].ActOnAroot(output.TheObjects[lowestNonExplored], tempRoot);
      output.AddObjectOnTop(tempRoot);
      if (output.GetRankOfSpanOfElements(theGlobalVariables)<output.size)
        output.PopLastObject();
    }
}

void SSalgebraModule::ExtractHighestWeightVectorsFromVector
  (root& input, roots& outputVectors, List<MatrixLargeRational>& simplePosGenerators, List<MatrixLargeRational>& simpleNegGenerators)
{ //Index ordering of sipleNegGenerators:
  //if simplePosGenerators.TheObjects[i] is a positive root space then its opposite root space should be
  //simpleNegGenerators.TheObjects[i]
  outputVectors.size=0;
  root remainderRoot=input;
  root componentRoot;
  root tempRoot;
  Rational theCoeff;
  List<int> GeneratorSequence;
  List<int> GeneratorPowers;
  while(!remainderRoot.IsEqualToZero())
  { this->ClimbUpFromVector(remainderRoot, tempRoot, simplePosGenerators, GeneratorSequence, GeneratorPowers);
    this->ClimbDownFromVectorAccordingToSequence(tempRoot, componentRoot, theCoeff, simplePosGenerators, simpleNegGenerators, GeneratorSequence, GeneratorPowers);
    assert(!theCoeff.IsEqualToZero());
    outputVectors.AddObjectOnTop(componentRoot/theCoeff);
    remainderRoot-=*outputVectors.LastObject();
  }
}

void SSalgebraModule::ClimbDownFromVectorAccordingToSequence
  (root& input, root& output, Rational& outputCoeff, List<MatrixLargeRational>& simplePosGenerators, List<MatrixLargeRational>& SimpleNegGenerators,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers)
{ assert(&input!=&output);
  output=input;
  root tempRoot;
  outputCoeff.MakeOne();
  Rational tempRat;
  for (int i=inputGeneratorSequence.size-1; i>=0; i--)
  { MatrixLargeRational& currentPosGen= simplePosGenerators.TheObjects[inputGeneratorSequence.TheObjects[i]];
    MatrixLargeRational& currentNegGen=SimpleNegGenerators.TheObjects[inputGeneratorSequence.TheObjects[i]];
    this->ClimbDownFromHighestWeightAlongSl2String(output, tempRoot, tempRat, currentPosGen, currentNegGen, inputGeneratorPowers.TheObjects[i]);
    outputCoeff.MultiplyBy(tempRat);
    output=tempRoot;
  }
}

void SSalgebraModule::ClimbDownFromHighestWeightAlongSl2String
  (root& input, root& output, Rational& outputCoeff, MatrixLargeRational& posGenerator, MatrixLargeRational& negGenerator, int generatorPower)
{ assert(&input!=&output);
  Rational currentWeight;
  negGenerator.ActOnAroot(input, output);
  posGenerator.ActOnAroot(output);
  input.IsProportionalTo(output, currentWeight);
  Rational RaiseCoeff;
  RaiseCoeff.MakeZero();
  outputCoeff.MakeOne();
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    negGenerator.ActOnAroot(output);
  }
}

void SSalgebraModule::ClimbUpFromVector
  (root& input, root& outputLastNonZero, List<MatrixLargeRational>& SimplePositiveGenerators, List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers)
{ root tempRoot;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  outputGeneratorPowers.size=0;
  outputGeneratorSequence.size=0;
  bool found=true;
  while (found)
  { found=false;
    for (int i=0; i<SimplePositiveGenerators.size; i++)
    { MatrixLargeRational& currentMat=SimplePositiveGenerators.TheObjects[i];
      int counter=0;
      for(currentMat.ActOnAroot(outputLastNonZero, tempRoot); !tempRoot.IsEqualToZero(); currentMat.ActOnAroot(tempRoot))
      { counter++;
        found=true;
        outputLastNonZero=tempRoot;
//        std::cout << "<br>" << outputLastNonZero.ElementToString();
//        std::cout.flush();
      }
      if (found)
      { outputGeneratorSequence.AddObjectOnTop(i);
        outputGeneratorPowers.AddObjectOnTop(counter);
      }
    }
  }
}

void SSalgebraModule::ConvertElementsToAdMatrixFormNegativeRootSpacesFirst
  (const List<ElementSimpleLieAlgebra>& input, const SemisimpleLieAlgebra& ambientLieAlgebra, List<MatrixLargeRational>& output)
{ output.SetSizeExpandOnTopNoObjectInit(input.size);
  for (int i=0; i<input.size; i++)
    this->ConvertElementsToAdMatrixFormNegativeRootSpacesFirst(input.TheObjects[i], ambientLieAlgebra, output.TheObjects[i]);
}

void SSalgebraModule::ConvertElementsToAdMatrixFormNegativeRootSpacesFirst
  (const ElementSimpleLieAlgebra& input, const SemisimpleLieAlgebra& ambientLieAlgebra, MatrixLargeRational& output)
{ ElementSimpleLieAlgebra tempElt1, tempElt2;
  int numGenerators=ambientLieAlgebra.GetNumGenerators();
  output.init(numGenerators, numGenerators);
  root tempRoot;
  for (int i=0; i<numGenerators; i++)
  { tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i,ambientLieAlgebra);
    ambientLieAlgebra.LieBracket(input, tempElt1, tempElt2);
    tempElt2.ElementToVectorNegativeRootSpacesFirst(tempRoot);
    for (int j=0; j<numGenerators; j++)
      output.elements[j][i]=tempRoot.TheObjects[j];
  }
//  std::cout << "<br><div class=\"math\">" << output.ElementToString(false, true) << "</div>";
}

void SSalgebraModule::ActOnPolynomialOverModule
  (const ElementSimpleLieAlgebra& theActingElement, const PolynomialOverModule& theArgument, PolynomialOverModule& output)
{ root vectorFormElement;
  theActingElement.ElementToVectorNegativeRootSpacesFirst(vectorFormElement);
  PolynomialOverModule Accum;
  Accum.Nullify(*this);
  PolynomialRationalCoeff tempP;
  for (int i=0; i<vectorFormElement.size; i++)
    if (!vectorFormElement.TheObjects[i].IsEqualToZero())
      for (int k=0; k<theArgument.size; k++)
      { this->ActOnMonomialOverModule(i, theArgument.TheObjects[k], tempP);
        tempP.TimesConstant(vectorFormElement.TheObjects[i]);
        //std::cout << "<br>" << tempP.ElementToString();
        Accum.AddPolynomial(tempP);
      }
  output.Assign(Accum);
}

void SSalgebraModule::ActOnMonomialOverModule
  (int indexChevalleyGeneratorNegativeSpacesFirst, const Monomial<Rational>& theArgument, PolynomialRationalCoeff& output)
{ int theDim=this->GetDim();
  output.Nullify(theDim);
  Monomial<Rational> tempMon;
  PolynomialRationalCoeff tempP;
  MatrixLargeRational& theOperator=this->actionsNegativeRootSpacesCartanPositiveRootspaces.TheObjects[indexChevalleyGeneratorNegativeSpacesFirst];
  for (int i=0; i<theArgument.NumVariables; i++)
    if (theArgument.degrees[i]>0)
    { tempP.Nullify(theDim);
      for (int k=0; k<theDim; k++)
      { tempMon.MakeNVarFirstDegree(k, (short) theDim, theOperator.elements[k][i]);
        tempP.AddMonomial(tempMon);
      }
      //std::cout <<"<br>" << tempP.ElementToString();
      tempP.TimesConstant(theArgument.degrees[i]);
      tempMon.Assign(theArgument);
      tempMon.degrees[i]--;
      tempP.MultiplyByMonomial(tempMon);
      output.AddPolynomial(tempP);
    }
}

void SSalgebraModule::ComputeInvariantsOfDegree
  (int degree, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ SelectionWithMaxMultiplicity theSel;
  int theDim=this->GetDim();
  int numPosRoots=this->theAlgebra.theWeyl.RootsOfBorel.size;
  int theRank= this->theAlgebra.theWeyl.CartanSymmetric.NumRows;
//  int numGenerators=this->actionsNegativeRootSpacesCartanPositiveRootspaces.size;
  theSel.initMaxMultiplicity(theDim, degree);
  PolynomialOutputFormat PolyFormatLocal;
  int numCycles=theSel.NumCombinationsOfCardinality(degree);
  PolynomialRationalCoeff basisMonsZeroWeight, basisMonsAll;
  basisMonsZeroWeight.Nullify(theDim);
  basisMonsZeroWeight.MakeActualSizeAtLeastExpandOnTop(numCycles);
  basisMonsAll.Nullify(theDim);
  basisMonsAll.MakeActualSizeAtLeastExpandOnTop(numCycles);
  Monomial<Rational> theMon;
  theMon.init(theDim);
  root theWeight;
  roots theCartanActions;
  theCartanActions.SetSizeExpandOnTopNoObjectInit(theDim);
  theWeight.SetSizeExpandOnTopLight(theDim);
  for (int i=0; i<theRank; i++)
  { root& current=theCartanActions.TheObjects[i];
    current.SetSizeExpandOnTopLight(theDim);
    for (int j=0; j<theDim; j++)
      current.TheObjects[j]=this->actionsNegativeRootSpacesCartanPositiveRootspaces.TheObjects[i+numPosRoots].elements[j][j];
  }
  theSel.IncrementSubsetFixedCardinality(degree);
  for (int i=0; i<numCycles; i++, theSel.IncrementSubsetFixedCardinality(degree))
  { for (int j=0; j<theDim; j++)
    { theMon.degrees[j]=theSel.Multiplicities.TheObjects[j];
      theWeight.TheObjects[j]=theMon.degrees[j];
    }
    bool isGood=true;
    for (int k=0; k<theRank; k++)
      if(!root::RootScalarEuclideanRoot(theWeight, theCartanActions.TheObjects[k]).IsEqualToZero())
      { isGood=false;
        break;
      }
    basisMonsAll.AddMonomial(theMon);
    if (isGood)
    { theMon.Coefficient.MakeOne();
      basisMonsZeroWeight.AddMonomial(theMon);
    }
  }
  out << "<br>Num cycles:" << numCycles << "<br>The basis mons (thera are " << basisMonsZeroWeight.size << " of them): "  << basisMonsZeroWeight.ElementToString(PolyFormatLocal);
  MatrixLargeRational tempMat;
  tempMat.init(basisMonsAll.size*theRank*2, basisMonsZeroWeight.size);
//  tempMat.init(basisMonsAll.size*numGenerators, basisMonsZeroWeight.size);
  PolynomialRationalCoeff tempP;
  for (int l=0; l<2; l++)
    for (int i=0; i<theRank; i++)
//    for (int i=0; i<numGenerators; i++)
      for (int k=0; k<basisMonsZeroWeight.size; k++)
      { int theIndex= (l==0) ? numPosRoots+theRank+i : numPosRoots-1-i;
        //int theIndex=i;
        this->ActOnMonomialOverModule(theIndex, basisMonsZeroWeight.TheObjects[k], tempP);
        for (int j=0; j<basisMonsAll.size; j++)
        { int indexInResult=tempP.IndexOfObjectHash(basisMonsAll.TheObjects[j]);
          int currentRow=(theRank*l+i)*basisMonsAll.size+j;
          //int currentRow=i*basisMonsAll.size+j;
          if (indexInResult==-1)
            tempMat.elements[currentRow][k]=0;
          else
            tempMat.elements[currentRow][k]=tempP.TheObjects[indexInResult].Coefficient;
        }
      }
  if (tempMat.NumRows<120)
    out << "<div class=\"math\" scale=\"50\">" << tempMat.ElementToString(false, true) << "</div>";
  roots tempRoots;
  tempMat.FindZeroEigenSpace(tempRoots, theGlobalVariables);
  this->invariantsFound.SetSizeExpandOnTopNoObjectInit(tempRoots.size);
  out << "<br> .... and the invariants are: ";
  for (int i=0; i<this->invariantsFound.size; i++)
  { Polynomial<Rational>& current=this->invariantsFound.TheObjects[i];
    current.Nullify(theDim);
    for (int j=0; j<basisMonsZeroWeight.size; j++)
      if (!tempRoots.TheObjects[i].TheObjects[j].IsEqualToZero())
      { theMon.Assign(basisMonsZeroWeight.TheObjects[j]);
        theMon.Coefficient=tempRoots.TheObjects[i].TheObjects[j];
        current.AddMonomial(theMon);
      }
    out << "<br>Invariant " << i << ":<br>" << current.ElementToString(PolyFormatLocal);
  }
}

void SSalgebraModule::mapInvariantsToEmbedding
  (std::stringstream& out, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ //PolynomialRationalCoeff tempP="x_1^2";
  PolynomialOutputFormat PolyFormatLocal;
  this->invariantsMappedToEmbedding.SetSizeExpandOnTopNoObjectInit(this->invariantsFound.size);
  for (int i=0; i<this->invariantsFound.size; i++)
  { ElementUniversalEnveloping& currentElt=this->invariantsMappedToEmbedding.TheObjects[i];
    Polynomial<Rational>& currentPoly=this->invariantsFound.TheObjects[i];
    currentElt.Nullify(owner);
    for (int k=0; k<currentPoly.size; k++)
    { this->mapMonomialToEmbedding(currentPoly.TheObjects[k], currentElt, owner, out, theGlobalVariables);
    }
//    currentElt.Simplify();
    out << "<br>The substitution: <div class=\"math\" scale=\"50\">\\begin{eqnarray*}" << currentPoly.ElementToString(false, PolyFormatLocal) << "&\\to&" << currentElt.ElementToString(true) << "\\end{eqnarray*}</div>";
  }
}

void SSalgebraModule::mapMonomialToEmbedding
  (Monomial<Rational>& input, ElementUniversalEnveloping& Accum, SemisimpleLieAlgebra& owner, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ permutation thePermutation;
  PolynomialOutputFormat PolyFormatLocal;
  thePermutation.initPermutation(input.NumVariables);
  int numPermutations= thePermutation.GetNumPermutations();
  ElementUniversalEnveloping tempElt;
  ElementUniversalEnveloping currentContribution;
  ElementUniversalEnveloping theUEUnit;
  theUEUnit.MakeConst((Rational) 1, 0, owner);
  List<int> permutationMap;
  thePermutation.GetPermutationLthElementIsTheImageofLthIndex(permutationMap);
  out << input.ElementToString(PolyFormatLocal);
  for (int i=0; i<numPermutations; i++, thePermutation.incrementAndGetPermutation(permutationMap))
  { currentContribution.MakeConst((Rational)1, 0, owner);
    for (int j=0; j<input.NumVariables; j++)
      if (input.degrees[j]>0)
      { tempElt.AssignElementLieAlgebra(this->moduleElementsEmbedded.TheObjects[permutationMap.TheObjects[j]], 0, owner);
        out << "<br>current generator: " << tempElt.ElementToString(false);
        MathRoutines::RaiseToPower(tempElt, input.degrees[j], theUEUnit);
        currentContribution*=tempElt;
      }
    currentContribution.Simplify();
    Accum+=currentContribution;
  }
}

void ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(List<ElementUniversalEnveloping>& theElements, roots& outputCoordinates, ElementUniversalEnveloping& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping tempMon;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddObjectOnTopNoRepetitionOfObjectHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSizeExpandOnTopNoObjectInit(theElements.size);
  Rational tempRat;
  for (int i=0; i<theElements.size; i++)
  { root& current=outputCoordinates.TheObjects[i];
    current.MakeZero(outputCorrespondingMonomials.size);
    ElementUniversalEnveloping& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping& currentMon=currentElt.TheObjects[j];
      if (currentMon.Coefficient.size>0)
        tempRat=currentMon.Coefficient.TheObjects[0].Coefficient;
      else
        tempRat.MakeZero();
      current.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=tempRat;
    }
  }
}

void ElementUniversalEnveloping::AssignFromCoordinateFormWRTBasis
  (List<ElementUniversalEnveloping>& theBasis, rootPoly& input, SemisimpleLieAlgebra& owner)
{ /*int numVars=0;
  if (theBasis.size>0)
    numVars= theBasis.TheObjects[0].GetNumVariables();*/
  this->Nullify(owner);
  ElementUniversalEnveloping tempElt;
  for (int i=0; i<input.size; i++)
    if (!input.TheObjects[i].IsEqualToZero())
    { tempElt.operator=(theBasis.TheObjects[i]);
      tempElt.operator*=(input.TheObjects[i]);
      this->operator+=(tempElt);
    }
}

void SemisimpleLieAlgebra::ComputeCommonAdEigenVectors
(int theDegree, List<ElementUniversalEnveloping>& theGenerators, List<ElementUniversalEnveloping>& generatorsBeingActedOn,
 List<ElementUniversalEnveloping>& output, std::stringstream& out,
 GlobalVariables& theGlobalVariables)
{ SelectionWithMaxMultiplicity theSel;
  PolynomialRationalCoeff::PreferredHashSize=1;
  int numGenerators=generatorsBeingActedOn.size;
  theSel.initMaxMultiplicity(numGenerators+1, theDegree);
  int numCycles=theSel.NumCombinationsOfCardinality(theDegree);
  List<ElementUniversalEnveloping> candidateElements, theBracketsOfTheElements;
  ElementUniversalEnveloping tempElt, tempElt2, currentOutput, Accum, UEUnit;
  theBracketsOfTheElements.ListActualSizeIncrement=50;
  theBracketsOfTheElements.size=0;
  theSel.IncrementSubsetFixedCardinality(theDegree);
  int numVars=this->GetRank();
  for (int i=0; i<numCycles; i++, theSel.IncrementSubsetFixedCardinality(theDegree))
  { Accum.MakeConst((Rational) 1, 0, *this);
    for (int j=0; j<theSel.elements.size; j++)
      if (theSel.elements.TheObjects[j]<numGenerators)
      { int generatorIndex=theSel.elements.TheObjects[j];
        int generatorPower=theSel.Multiplicities.TheObjects[generatorIndex];
        tempElt.operator=(generatorsBeingActedOn.TheObjects[generatorIndex]);
        tempElt.RaiseToPower(generatorPower);
        Accum.MultiplyBy(tempElt);
      }
    tempElt.operator=(Accum);
    tempElt.SetNumVariables(numVars);
    candidateElements.AddObjectOnTop(tempElt);
//    out << "<br>" << Accum.ElementToString(true);
    for (int j=0; j<theGenerators.size; j++)
    { tempElt2.operator=(theGenerators.TheObjects[j]);
      tempElt2.SetNumVariables(numVars);
      tempElt2.LieBracketOnTheRight(tempElt, currentOutput);
//      currentOutput.ComputeDebugString();
//      std::cout<< currentOutput.DebugString;
      currentOutput.Simplify();
      if (numVars==0)
        currentOutput.ModOutVermaRelations(true);
      else
        currentOutput.ModOutVermaRelations(false);
      theBracketsOfTheElements.AddObjectOnTop(currentOutput);
      out << "<br>";
      //out << "<div class=\"math\">";
      out << "[" << theGenerators.TheObjects[j].ElementToString() << "," << Accum.ElementToString() << "]=";
      //out << "\\begin{eqnarray*}&&";
      out << currentOutput.ElementToString();
      //out << "  (mod Verma relations)";
      //out << "\\end{eqnarray*}</div>";
    }
  }
  out << "<br>number of monomials of given degree: " << candidateElements.size;
  List<rootPoly> theBasisRootForm;
  ElementUniversalEnveloping theMonBasis;
  ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(numVars, theBracketsOfTheElements, theBasisRootForm, theMonBasis, theGlobalVariables);
  std::string tempS;
  out << "<br>...and the monomial basis is(" << theMonBasis.size << " elements total): ";
  for (int i=0; i<theMonBasis.size; i++)
    out << theMonBasis.TheObjects[i].ElementToString(false) << ", ";
  Matrix<RationalFunction> theSystem;
  theSystem.init(theMonBasis.size*theGenerators.size, candidateElements.size);
  for (int k=0; k<theGenerators.size; k++)
    for (int i=0; i<candidateElements.size; i++)
    { rootPoly& currentRoot=theBasisRootForm.TheObjects[k+i*theGenerators.size];
      //out << "<br>root index" << i << ": " << currentRoot.ElementToString();
      for (int j=0; j<currentRoot.size; j++)
        theSystem.elements[currentRoot.size*k+j][i]=currentRoot.TheObjects[j];
    }
  out << "<br>...and the system is: <div class=\"math\">" <<  theSystem.ElementToString(false, true) << "</div>";
  List<List<RationalFunction> > theEigenVectors;
  PolynomialRationalCoeff::PreferredHashSize=50;
  RationalFunction oneRF, minusOneRF, zeroRF;
  oneRF.MakeNVarConst(numVars, (Rational) 1);
  minusOneRF.MakeNVarConst(numVars, (Rational) -1);
  zeroRF.MakeNVarConst(numVars, (Rational) 0);

  theSystem.FindZeroEigenSpacE(theEigenVectors, oneRF, minusOneRF, zeroRF, theGlobalVariables);
  out << "<br> the dim of the eigen space: " << theEigenVectors.size;
  rootPoly tempProot;

  for (int i=0; i<theEigenVectors.size; i++)
  { List<RationalFunction>& currentEigen=theEigenVectors.TheObjects[i];
    RationalFunction::ScaleClearDenominator(currentEigen, tempProot);
    tempElt.AssignFromCoordinateFormWRTBasis(candidateElements, tempProot, *this);
    out << "<br>" << tempElt.ElementToString();
  }
}

void RationalFunction::ScaleClearDenominator
  (List<RationalFunction>& input, rootPoly& output)
{ PolynomialRationalCoeff tempP;
  List<RationalFunction> buffer;
  buffer.CopyFromBase(input);
  for (int i=0; i<buffer.size; i++)
  { RationalFunction& current=buffer.TheObjects[i];
    if (!current.Denominator.IsEqualToOne())
    { tempP.operator=(current.Denominator);
      for (int j=0; j<buffer.size; j++)
        buffer.TheObjects[j].operator*=(tempP);
    }
  }
  output.SetSizeExpandOnTopNoObjectInit(input.size);
  for (int i=0; i<buffer.size; i++)
    output.TheObjects[i].operator=(buffer.TheObjects[i].Numerator);
}

void SemisimpleLieAlgebra::ComputeCommonAdEigenVectorsFixedWeight
(root& theWeight, roots& theHs, List<ElementUniversalEnveloping>& theGenerators,
 List<ElementUniversalEnveloping>& output, std::stringstream& out,
 GlobalVariables& theGlobalVariables)
{ VectorPartition theVP;
  PolynomialRationalCoeff::PreferredHashSize=1;
  int numGenerators= this->GetNumPosRoots();
  int numVars=this->GetRank();
  int theRank=this->GetRank();
  theVP.PartitioningRoots.SetSizeExpandOnTopNoObjectInit(numGenerators);
  List<ElementUniversalEnveloping> generatorsBeingActedOn;
  generatorsBeingActedOn.SetSizeExpandOnTopNoObjectInit(numGenerators);
  theVP.PartitioningRoots.operator=(this->theWeyl.RootsOfBorel);
  out << "<br>" << theWeight.ElementToString() << "<br> the H's: " << theHs.ElementToString();
  out << "<br> Partitioning roots: " << theVP.PartitioningRoots.ElementToString();
  for (int i=0; i<numGenerators; i++)
  { ElementUniversalEnveloping& currentUE=generatorsBeingActedOn.TheObjects[i];
    currentUE.MakeOneGeneratorCoeffOne(i, 0, *this);
  }
  theVP.theRoot=theWeight;
  theVP.ComputeAllPartitions();
  out << "<br><br>" << theVP.ElementToString(true);
  root accumulatedHweight, tempRoot;
  for (int i=0; i<theVP.thePartitions.size; i++)
  { List<int>& currentPartition=theVP.thePartitions.TheObjects[i];
    accumulatedHweight.MakeZero(theRank);
    for (int generatorIndex=0; generatorIndex<currentPartition.size; generatorIndex++)
    { int generatorPower=currentPartition.TheObjects[generatorIndex];
      accumulatedHweight+=this->theWeyl.RootSystem.TheObjects[this->theWeyl.RootSystem.size-1-generatorIndex]* generatorPower;
    }
    bool isGood=true;
    for (int j=0; j<theHs.size; j++)
    { root& currentH=theHs.TheObjects[j];
      Rational theTargetScalar;
      Rational currentScalar;
      bool found=false;
      for (int k=0; k<currentH.size; k++)
        if (currentH.TheObjects[k]!=0)
        { tempRoot.MakeEi(currentH.size, k);
          currentScalar=this->theWeyl.RootScalarCartanRoot(tempRoot, accumulatedHweight);
          if (!found)
          { theTargetScalar=currentScalar;
            found=true;
          }
          if (theTargetScalar!=currentScalar)
          { isGood=false;
            break;
          }
        }
      if (!isGood)
        break;
    }
    if (isGood)
    { out << "<br>" << accumulatedHweight.ElementToString();
      out << " is good";
    } else
    { theVP.thePartitions.PopIndexSwapWithLast(i);
      i--;
    }
  }
  List<ElementUniversalEnveloping> candidateElements, theBracketsOfTheElements;
  ElementUniversalEnveloping tempElt, tempElt2, currentOutput, Accum, UEUnit;
  theBracketsOfTheElements.ListActualSizeIncrement=50;
  theBracketsOfTheElements.size=0;
  for (int i=0; i<theVP.thePartitions.size; i++)
  { Accum.MakeConst((Rational) 1, 0, *this);
    List<int>& currentPartition=theVP.thePartitions.TheObjects[i];
    for (int generatorIndex=0; generatorIndex<currentPartition.size; generatorIndex++)
    { int generatorPower=currentPartition.TheObjects[generatorIndex];
      tempElt.operator=(generatorsBeingActedOn.TheObjects[generatorIndex]);
      tempElt.RaiseToPower(generatorPower);
      Accum.MultiplyBy(tempElt);
    }
    tempElt.operator=(Accum);
    tempElt.SetNumVariables(numVars);
    candidateElements.AddObjectOnTop(tempElt);
//    out << "<br>" << Accum.ElementToString(true);
    for (int j=0; j<theGenerators.size; j++)
    { tempElt2.operator=(theGenerators.TheObjects[j]);
      tempElt2.SetNumVariables(numVars);
      tempElt2.LieBracketOnTheRight(tempElt, currentOutput);
//      currentOutput.ComputeDebugString();
//      std::cout<< currentOutput.DebugString;
      currentOutput.Simplify();
      if (numVars==0)
        currentOutput.ModOutVermaRelations(true);
      else
        currentOutput.ModOutVermaRelations(false);
      theBracketsOfTheElements.AddObjectOnTop(currentOutput);
      out << "<br>";
      //out << "<div class=\"math\">";
      out << "[" << theGenerators.TheObjects[j].ElementToString() << "," << Accum.ElementToString() << "]=";
      //out << "\\begin{eqnarray*}&&";
      out << currentOutput.ElementToString();
      //out << "  (mod Verma relations)";
      //out << "\\end{eqnarray*}</div>";
    }
  }
  out << "<br>number of monomials of given degree: " << candidateElements.size;
  List<rootPoly> theBasisRootForm;
  ElementUniversalEnveloping theMonBasis;
  ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(numVars, theBracketsOfTheElements, theBasisRootForm, theMonBasis, theGlobalVariables);
  std::string tempS;
  out << "<br>...and the monomial basis is(" << theMonBasis.size << " elements total): ";
  for (int i=0; i<theMonBasis.size; i++)
    out << theMonBasis.TheObjects[i].ElementToString(false) << ", ";
  Matrix<RationalFunction> theSystem;
  theSystem.init(theMonBasis.size*theGenerators.size, candidateElements.size);
  for (int k=0; k<theGenerators.size; k++)
    for (int i=0; i<candidateElements.size; i++)
    { rootPoly& currentRoot=theBasisRootForm.TheObjects[k+i*theGenerators.size];
      //out << "<br>root index" << i << ": " << currentRoot.ElementToString();
      for (int j=0; j<currentRoot.size; j++)
        theSystem.elements[currentRoot.size*k+j][i]=currentRoot.TheObjects[j];
    }
  out << "<br>...and the system is: <div class=\"math\">" <<  theSystem.ElementToString(false, true) << "</div>";
  List<List<RationalFunction> > theEigenVectors;
  PolynomialRationalCoeff::PreferredHashSize=50;
  RationalFunction oneRF, minusOneRF, zeroRF;
  oneRF.MakeNVarConst(numVars, (Rational) 1);
  minusOneRF.MakeNVarConst(numVars, (Rational) -1);
  zeroRF.MakeNVarConst(numVars, (Rational) 0);

  theSystem.FindZeroEigenSpacE(theEigenVectors, oneRF, minusOneRF, zeroRF, theGlobalVariables);
  out << "<br> the dim of the eigen space: " << theEigenVectors.size;
  rootPoly tempProot;

  for (int i=0; i<theEigenVectors.size; i++)
  { List<RationalFunction>& currentEigen=theEigenVectors.TheObjects[i];
    RationalFunction::ScaleClearDenominator(currentEigen, tempProot);
    tempElt.AssignFromCoordinateFormWRTBasis(candidateElements, tempProot, *this);
    out << "<br>" << tempElt.ElementToString();
  }
}

std::string HomomorphismSemisimpleLieAlgebra::WriteAllUEMonomialsWithWeightWRTDomain(List<ElementUniversalEnveloping>& output, root& theWeight, GlobalVariables& theGlobalVariables)
{ output.size=0;
//  this->theDomain.theWeyl.
  roots PosRootsEmbeddings, PosRootsProjections;
  std::stringstream out;
  int theDimension=this->theRange.theWeyl.CartanSymmetric.NumRows;
  int theDomainDimension= this->theDomain.theWeyl.CartanSymmetric.NumRows;
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
  out << "<br>Projections of roots in simple root coordinates with respect to the subalgebra: " << PosRootsProjections.ElementToString(false, true, true);
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
    for (int j=theVP.thePartitions.TheObjects[i].size-1; j>=0; j--)
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
    ElementUniversalEnveloping::GetCoordinateFormOfSpanOfElements(theDimension, currentTargets, tempList, basisMonomialBuffer, theGlobalVariables);
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


void ElementUniversalEnvelopingOrdered::Simplify()
{ ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  //this->ComputeDebugString();
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Simplify(buffer);
    output+=buffer;
    //output.ComputeDebugString();
  }
  *this=output;
}

void ElementUniversalEnvelopingOrdered::Nullify(SemisimpleLieAlgebraOrdered& theOwner)
{ this->ClearTheObjects();
  this->owner=&theOwner;
}

void MonomialUniversalEnvelopingOrdered::Simplify(ElementUniversalEnvelopingOrdered& output)
{ output.Nullify(*this->owner);
  output.AddObjectOnTopHash(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output);
}

void ElementUniversalEnvelopingOrdered::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexSwapWithLastHash(i);
      i--;
    }
}

void ElementUniversalEnvelopingOrdered::operator+=(const ElementUniversalEnvelopingOrdered& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  for (int i=0; i<other.size; i++)
    this->AddMonomialNoCleanUpZeroCoeff(other.TheObjects[i]);
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnvelopingOrdered::operator-=(const ElementUniversalEnvelopingOrdered& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  MonomialUniversalEnvelopingOrdered tempMon;
  for (int i=0; i<other.size; i++)
  { tempMon=other.TheObjects[i];
    tempMon.Coefficient.TimesConstant(-1);
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnvelopingOrdered::operator/=(const Rational& other)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.DivByRational(other);
}

void ElementUniversalEnvelopingOrdered::operator*=(const PolynomialRationalCoeff& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.MultiplyBy(other);
}

void ElementUniversalEnvelopingOrdered::operator*=(const Rational& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.TimesConstant(other);
}

void MonomialUniversalEnvelopingOrdered::MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other)
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

void ElementUniversalEnvelopingOrdered::operator*=(const ElementUniversalEnvelopingOrdered& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size*this->size);
  ElementUniversalEnvelopingOrdered output;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
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

void ElementUniversalEnvelopingOrdered::AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnvelopingOrdered& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
    this->TheObjects[theIndex].Coefficient+=input.Coefficient;
}

void MonomialUniversalEnvelopingOrdered::SimplifyAccumulateInOutputNoOutputInit(ElementUniversalEnvelopingOrdered& output)
{ int IndexlowestNonSimplified=0;
  ElementUniversalEnvelopingOrdered buffer2;
  MonomialUniversalEnvelopingOrdered tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified<output.size)
  { bool reductionOccurred=false;
    if (output.TheObjects[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred=true;
    else
      for (int i=0; i<output.TheObjects[IndexlowestNonSimplified].generatorsIndices.size-1; i++)
        if (output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i]>output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1])
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

bool MonomialUniversalEnvelopingOrdered::SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex, MonomialUniversalEnvelopingOrdered& output)
{ return false;
}

bool MonomialUniversalEnvelopingOrdered::CommutingRightIndexAroundLeftIndexAllowed(PolynomialRationalCoeff& theLeftPower, int leftGeneratorIndex, PolynomialRationalCoeff& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

void MonomialUniversalEnvelopingOrdered::CommuteConsecutiveIndicesRightIndexAroundLeft(int theIndeX, ElementUniversalEnvelopingOrdered& output)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
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
  MonomialUniversalEnvelopingOrdered startMon;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResulT, tempElT, theLeftElt;
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndeX, adResulT);
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndeX, theLeftElt);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1);
  root theCoeffs;
  do
  { //acquiredCoefficienT.ComputeDebugString();
    //theRightPoweR.ComputeDebugString();
    //theLeftPoweR.ComputeDebugString();
    //adResulT.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    this->owner->GetLinearCombinationFrom(adResulT, theCoeffs);
    for (int i=0; i<theCoeffs.size; i++)
      if (theCoeffs.TheObjects[i]!=0)
      { int theNewGeneratorIndex=i;
        tempMon=startMon;
        tempMon.Coefficient=acquiredCoefficienT;
        tempMon.Coefficient.TimesConstant(theCoeffs.TheObjects[i]);
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
        tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
        for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
        tempMon.ComputeDebugString();
        this->ComputeDebugString();
        output.AddObjectOnTopHash(tempMon);
      }
    acquiredCoefficienT.MultiplyBy(theLeftPoweR);
    theLeftPoweR-=1;
    this->owner->theOwner.LieBracket(theLeftElt, adResulT, tempElT);
    adResulT=tempElT;
    powerDroP++;
    acquiredCoefficienT.DivByInteger(powerDroP);
    //adResulT.ComputeDebugString(*this->owner, false, false);
  }while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

void MonomialUniversalEnvelopingOrdered::CommuteConsecutiveIndicesLeftIndexAroundRight(int theIndeX, ElementUniversalEnvelopingOrdered& output)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
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
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResult, tempElt, tempRightElt;
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndex, adResult);
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndex, tempRightElt);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1);
  root theCoeffs;
  do
  { //acquiredCoefficient.ComputeDebugString();
    //theRightPower.ComputeDebugString();
    //adResult.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    //tempMon.ComputeDebugString();
    this->owner->GetLinearCombinationFrom(adResult, theCoeffs);
    for (int i=0; i<theCoeffs.size; i++)
      if(theCoeffs.TheObjects[i]!=0)
      { int theNewGeneratorIndex= i;
        tempMon=startMon;
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
        tempMon.Coefficient=acquiredCoefficient;
        tempMon.Coefficient.TimesConstant(theCoeffs.TheObjects[i]);
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
        for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
        output.AddObjectOnTopHash(tempMon);
      }
    acquiredCoefficient.MultiplyBy(theRightPower);
    theRightPower-=1;
    this->owner->theOwner.LieBracket(adResult, tempRightElt, tempElt);
    adResult=tempElt;
    powerDrop++;
    acquiredCoefficient.DivByInteger(powerDrop);
    //adResult.ComputeDebugString(*this->owner, false, false);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

bool MonomialUniversalEnvelopingOrdered::CommutingLeftIndexAroundRightIndexAllowed(PolynomialRationalCoeff& theLeftPower, int leftGeneratorIndex, PolynomialRationalCoeff& theRightPower, int rightGeneratorIndex)
{ if (theLeftPower.TotalDegree()==0)
  { if(theRightPower.TotalDegree()==0)
      return true;
    int numPosRoots=this->owner->theOwner.theWeyl.RootsOfBorel.size;
    int theDimension= this->owner->theOwner.theWeyl.CartanSymmetric.NumRows;
    if(rightGeneratorIndex>= numPosRoots && rightGeneratorIndex<numPosRoots+theDimension)
    { ElementSimpleLieAlgebra tempElt;
      this->owner->theOwner.LieBracket(this->owner->theOrder.TheObjects[leftGeneratorIndex], this->owner->theOrder.TheObjects[rightGeneratorIndex], tempElt);
      if (tempElt.IsEqualToZero())
        return true;
      else
        return false;
    } else
      return true;
  }
  return false;
}

void MonomialUniversalEnvelopingOrdered::Nullify(int numVars, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Coefficient.Nullify((short)numVars);
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

int MonomialUniversalEnvelopingOrdered::HashFunction() const
{ int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result=0;
  for (int i=0; i<top; i++)
    result+=SomeRandomPrimes[i]*this->generatorsIndices.TheObjects[i];
  return result;
}

void MonomialUniversalEnvelopingOrdered::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower)
{ if (thePower==0)
    return;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst(this->Coefficient.NumVars, thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

void MonomialUniversalEnvelopingOrdered::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const PolynomialRationalCoeff& thePower)
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

void SemisimpleLieAlgebraOrdered::GetLinearCombinationFrom
  (ElementSimpleLieAlgebra& input, root& theCoeffs)
{ theCoeffs.MakeZero(this->theOwner.GetNumGenerators());
  for (int i=0; i<input.NonZeroElements.CardinalitySelection; i++)
  { int theIndex=input.NonZeroElements.elements[i];
    theCoeffs.TheObjects[this->theOwner.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(theIndex)]=input.coeffsRootSpaces.TheObjects[theIndex];
  }
  int numPosRoots=this->theOwner.GetNumPosRoots();
  for (int i=0; i<input.Hcomponent.size; i++)
    theCoeffs.TheObjects[numPosRoots+i]= input.Hcomponent.TheObjects[i];
  this->ChevalleyGeneratorsInCurrentCoords.ActOnAroot(theCoeffs);
}

void ElementUniversalEnvelopingOrdered::MakeOneGeneratorCoeffOne(int theIndex, int numVars, SemisimpleLieAlgebraOrdered& owner)
{ this->Nullify(owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.Nullify(numVars, owner);
  tempMon.Coefficient.MakeNVarConst((short)numVars, (Rational) 1);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddObjectOnTopHash(tempMon);
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
(List<ElementSimpleLieAlgebra>& inputOrder, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ if (inputOrder.size!=owner.GetNumGenerators())
    return;
  this->theOwner=owner;
  this->theOrder=inputOrder;
  this->ChevalleyGeneratorsInCurrentCoords.init(owner.GetNumGenerators(), owner.GetNumGenerators());
  this->ChevalleyGeneratorsInCurrentCoords.NullifyAll();
  for (int i=0; i<owner.GetNumGenerators(); i++)
  { ElementSimpleLieAlgebra& currentElt=inputOrder.TheObjects[i];
    for (int j=0; j<currentElt.NonZeroElements.CardinalitySelection; j++)
      this->ChevalleyGeneratorsInCurrentCoords.elements[this->theOwner.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(currentElt.NonZeroElements.elements[j])][i]=currentElt.coeffsRootSpaces.TheObjects[currentElt.NonZeroElements.elements[j]];
    for (int j=0; j<currentElt.Hcomponent.size; j++)
      this->ChevalleyGeneratorsInCurrentCoords.elements[owner.GetNumPosRoots()+j][i]=currentElt.Hcomponent.TheObjects[j];
  }
  //std::cout << this->ChevalleyGeneratorsInCurrentCoords.ElementToString(true, false) << "<br><br>";

  this->ChevalleyGeneratorsInCurrentCoords.Invert(theGlobalVariables);
  //std::cout << this->ChevalleyGeneratorsInCurrentCoords.ElementToString(true, false);
}

void SemisimpleLieAlgebraOrdered::initDefaultOrder
  (SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ List<ElementSimpleLieAlgebra> defaultOrder;
  defaultOrder.SetSizeExpandOnTopNoObjectInit(owner.GetNumGenerators());
  for (int i=0; i<defaultOrder.size; i++)
  { ElementSimpleLieAlgebra& currentElt=defaultOrder.TheObjects[i];
    currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, owner);
  }
  this->init(defaultOrder, owner, theGlobalVariables);
}

bool ElementSimpleLieAlgebra::MustUseBracketsWhenDisplayingMeRaisedToPower()
{ if (this->NonZeroElements.CardinalitySelection==1 && this->Hcomponent.IsEqualToZero())
    if (this->coeffsRootSpaces.TheObjects[this->NonZeroElements.elements[0]]==1)
      return false;
  if (this->NonZeroElements.CardinalitySelection==0)
  { int numNonZeroElements=0;
    for (int i=0; i<this->Hcomponent.size; i++)
    { if (this->Hcomponent.TheObjects[i]!=1 && this->Hcomponent.TheObjects[i]!=0)
        return true;
      if (this->Hcomponent.TheObjects[i]==1)
        numNonZeroElements++;
    }
    if (numNonZeroElements<=1)
      return false;
  }
  return true;
}

std::string MonomialUniversalEnvelopingOrdered::ElementToString(bool useLatex, bool useGeneratorLetters, PolynomialOutputFormat& PolyFormatLocal)
{ std::stringstream out;
  std::string tempS;
  if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->Coefficient.IsEqualToZero())
    tempS="0";
  else
    tempS = this->Coefficient.ElementToString(PolyFormatLocal);
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
    if (!useGeneratorLetters)
      tempS=this->owner->theOrder.TheObjects[theIndex].ElementToStringNegativeRootSpacesFirst(false, false, this->owner->theOwner);
    else
    { std::stringstream tempStream;
      tempStream << "f_{" << this->owner->GetDisplayIndexFromGeneratorIndex(theIndex) << "}";
      tempS=tempStream.str();
    }
    //if (thePower>1)
    //  out << "(";
    bool usebrackets=false;
    if (!useGeneratorLetters)
      usebrackets=this->owner->theOrder.TheObjects[theIndex].MustUseBracketsWhenDisplayingMeRaisedToPower();
    if (usebrackets)
      out << "(";
    out << tempS;
    if (usebrackets)
      out <<")";
    if (!thePower.IsEqualToOne())
    { out << "^";
     // if (useLatex)
      out << "{";
      out << thePower.ElementToString(PolyFormatLocal);
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

void ElementUniversalEnvelopingOrdered::ElementToString(std::string& output, bool useLatex)
{ std::stringstream out;
  std::string tempS;
  if (this->size==0)
    out << "0";
  int IndexCharAtLastLineBreak=0;
  PolynomialOutputFormat PolyFormatLocal;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnvelopingOrdered& current=this->TheObjects[i];
    tempS=current.ElementToString(false, true, PolyFormatLocal);
    if (i!=0)
      if (tempS.size()>0)
        if (tempS[0]!='-')
          out << '+';
    out << tempS;
    if (((int)out.tellp())- IndexCharAtLastLineBreak>150)
    { IndexCharAtLastLineBreak=out.tellp();
      out << "\\\\&&";
    }
  }
  output=out.str();
}

bool ElementUniversalEnvelopingOrdered::AssignElementUniversalEnveloping
  (ElementUniversalEnveloping& input, SemisimpleLieAlgebraOrdered& owner)
{ ElementUniversalEnvelopingOrdered tempElt;
  this->Nullify(owner);
  for (int i=0; i<input.size; i++)
  { if(!tempElt.AssignMonomialUniversalEnveloping(input.TheObjects[i], owner))
      return false;
    this->operator+=(tempElt);
  }
  return true;
}

bool ElementUniversalEnvelopingOrdered::AssignMonomialUniversalEnveloping
  (MonomialUniversalEnveloping& input, SemisimpleLieAlgebraOrdered& owner)
{ ElementUniversalEnvelopingOrdered theMon;
  ElementSimpleLieAlgebra tempElt;
  int numVars=input.Coefficient.NumVars;
  this->MakeConst( (Rational) 1, numVars, owner);
  for (int i=0; i<input.generatorsIndices.size; i++)
  { tempElt.Nullify(owner.theOwner);
    tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(input.generatorsIndices.TheObjects[i], owner.theOwner);
    theMon.AssignElementLieAlgebra(tempElt, numVars, owner);
    if (theMon.size==1 && theMon.TheObjects[0].Coefficient.IsEqualToOne())
    { theMon.TheObjects[0].Powers.TheObjects[0].operator=(input.Powers.TheObjects[i]);
    } else
    if (input.Powers.TheObjects[i].size==1 && input.Powers.TheObjects[i].TotalDegree()==0)
    { theMon.RaiseToPower(input.Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort);
      this->MultiplyBy(theMon);
    } else
      return false;
  }
  return true;
}

void ElementUniversalEnvelopingOrdered::MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebraOrdered& theOwner)
{ MonomialUniversalEnvelopingOrdered tempMon;
  this->Nullify(theOwner);
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((short)numVars, coeff);
  tempMon.MakeConst(tempP, theOwner);
  this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  this->CleanUpZeroCoeff();
}

void ElementUniversalEnvelopingOrdered::AssignElementLieAlgebra
(const ElementSimpleLieAlgebra& input, int numVars, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Nullify(theOwner);
  root ElementRootForm;
  input.ElementToVectorNegativeRootSpacesFirst(ElementRootForm);
  theOwner.ChevalleyGeneratorsInCurrentCoords.ActOnAroot(ElementRootForm);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.Nullify(numVars, theOwner);
  tempMon.generatorsIndices.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.SetSizeExpandOnTopNoObjectInit(1);
  tempMon.Powers.TheObjects[0].MakeNVarConst((short)numVars, 1);
  for (int theIndex=0; theIndex<ElementRootForm.size; theIndex++)
    if ( ElementRootForm.TheObjects[theIndex]!=0)
    { tempMon.Coefficient.MakeNVarConst((short)numVars, ElementRootForm.TheObjects[theIndex]);
      tempMon.generatorsIndices.TheObjects[0]=theIndex;
      this->AddObjectOnTopHash(tempMon);
    }
}

void ElementUniversalEnvelopingOrdered::RaiseToPower(int thePower)
{ ElementUniversalEnvelopingOrdered buffer;
  buffer.operator=(*this);
  if (this->size==0)
    return;
  this->MakeConst(1, this->TheObjects[0].Coefficient.NumVars, *this->owner);
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
}

void ElementUniversalEnvelopingOrdered::LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output)
{ ElementUniversalEnvelopingOrdered tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

void ElementUniversalEnvelopingOrdered::AddMonomial(const MonomialUniversalEnvelopingOrdered& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
  { this->TheObjects[theIndex].Coefficient+=input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

bool ParserNode::ConvertToType(int theType)
{ if (this->ExpressionType==this->typeError)
    return false;
  if (theType==this->typeUndefined)
    return false;
  if (this->ExpressionType==this->typePoly)
      if (this->polyValue.GetElement().NumVars< this->owner->NumVariables)
        this->polyValue.GetElement().SetNumVariablesSubDeletedVarsByOne((short)this->owner->NumVariables);
  if (this->ExpressionType==this->typeUEElementOrdered)
    this->UEElementOrdered.GetElement().SetNumVariables(this->owner->NumVariables);
  if (this->ExpressionType==this->typeUEelement)
    this->UEElement.GetElement().SetNumVariables(this->owner->NumVariables);
  if (this->ExpressionType==this->typeWeylAlgebraElement)
    this->WeylAlgebraElement.GetElement().SetNumVariablesPreserveExistingOnes(this->owner->NumVariables);
  if (this->ExpressionType==this->typeUEelement && theType==this->typeUEElementOrdered)
  { if (this->UEElementOrdered.GetElement().AssignElementUniversalEnveloping(this->UEElement.GetElement(), this->owner->testAlgebra))
    { this->ExpressionType=this->typeUEElementOrdered;
      return true;
    }
    return false;
  }
  if (this->ExpressionType==theType)
    return true;
  if (this->ExpressionType> theType)
    return false;
  //we have eliminated the corner cases. Time to do some real conversion :)
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { if (theType==this->typeRational)
      this->rationalValue= this->intValue;
    if (theType==this->typePoly)
      this->polyValue.GetElement().MakeNVarConst((short) this->owner->NumVariables, (Rational) this->intValue);
    if (theType==this->typeUEElementOrdered)
      this->UEElementOrdered.GetElement().MakeConst((Rational) this->intValue, (short) this->owner->NumVariables, this->owner->testAlgebra);
    if (theType==this->typeUEelement)
      this->UEElement.GetElement().AssignInt(this->intValue, (short)this->owner->NumVariables, *this->ContextLieAlgebra);
    if (theType==this->typeWeylAlgebraElement)
      this->WeylAlgebraElement.GetElement().MakeConst(this->owner->NumVariables, (Rational) this->typeIntegerOrIndex);
  }
  if (this->ExpressionType==this->typeRational)
  { if (theType==this->typePoly)
      this->polyValue.GetElement().MakeNVarConst((short)this->owner->NumVariables, this->rationalValue);
    if (theType==this->typeUEElementOrdered)
      this->UEElementOrdered.GetElement().MakeConst(this->rationalValue, this->owner->NumVariables, this->owner->testAlgebra);
    if (theType==this->typeUEelement)
      this->UEElement.GetElement().MakeConst(this->rationalValue, this->owner->NumVariables, *this->ContextLieAlgebra);
    if (theType==this->typeWeylAlgebraElement)
      this->WeylAlgebraElement.GetElement().MakeConst(this->owner->NumVariables, this->rationalValue);
  }
  if (this->ExpressionType==this->typePoly)
  { if (theType==this->typeUEElementOrdered)
      this->UEElementOrdered.GetElement().MakeConst(this->polyValue.GetElement(), this->owner->testAlgebra);
    if (theType==this->typeUEelement)
      this->UEElement.GetElement().MakeConst(this->polyValue.GetElement(), *this->ContextLieAlgebra);
    if (theType==this->typeWeylAlgebraElement)
      this->WeylAlgebraElement.GetElement().AssignPolynomial(this->polyValue.GetElement());
  }
  if (this->ExpressionType==this->typeUEElementOrdered)
  { if (theType==this->typeUEelement)
      if (!this->UEElementOrdered.GetElement().GetElementUniversalEnveloping(this->UEElement.GetElement(), this->owner->theHmm.theRange))
        return false;
    if (theType==this->typeWeylAlgebraElement)
      return false;
  }
  if (this->ExpressionType==this->typeUEelement)
    if (theType==this->typeWeylAlgebraElement)
      return false;
  this->ExpressionType=theType;
  return true;
}

bool ElementUniversalEnvelopingOrdered::GetElementUniversalEnveloping
  (ElementUniversalEnveloping& output, SemisimpleLieAlgebra& owner)
{ ElementUniversalEnveloping Accum, tempElt;
  Accum.Nullify(owner);
  for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].GetElementUniversalEnveloping(tempElt, owner))
      return false;
    else
      Accum+=tempElt;
  output=Accum;
  return true;
}

bool MonomialUniversalEnvelopingOrdered::GetElementUniversalEnveloping
  (ElementUniversalEnveloping& output, SemisimpleLieAlgebra& owner)
{ ElementUniversalEnveloping Accum;
  ElementUniversalEnveloping tempMon;
  int theIndex;
  Accum.MakeConst(this->Coefficient, owner);
  for (int i=0; i<this->generatorsIndices.size; i++)
    if (this->Powers.TheObjects[i].IsAnInteger())
    { tempMon.AssignElementLieAlgebra(this->owner->theOrder.TheObjects[this->generatorsIndices.TheObjects[i]], this->Coefficient.NumVars, owner);
      tempMon.RaiseToPower(this->Powers.TheObjects[i].TheObjects[0].Coefficient.NumShort);
      Accum.MultiplyBy(tempMon);
    }
    else
      if (this->owner->theOrder.TheObjects[this->generatorsIndices.TheObjects[i]].IsACoeffOneChevalleyGenerator(theIndex, owner))
      { tempMon.MakeOneGeneratorCoeffOne(theIndex, this->Coefficient.NumVars, owner);
        tempMon.TheObjects[0].Powers.TheObjects[0]=this->Powers.TheObjects[i];
        Accum.MultiplyBy(tempMon);
      } else
        return false;
  output.operator=(Accum);
  return true;
}

bool ElementSimpleLieAlgebra::IsACoeffOneChevalleyGenerator(int& outputGenerator, SemisimpleLieAlgebra& owner)
{ if (this->NonZeroElements.CardinalitySelection>1)
    return false;
  if (this->NonZeroElements.CardinalitySelection==1)
  { if (!this->Hcomponent.IsEqualToZero())
      return false;
    if (this->coeffsRootSpaces.TheObjects[this->NonZeroElements.elements[0]].IsEqualToOne())
    { outputGenerator=owner.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(this->NonZeroElements.elements[0]);
      return true;
    }
    return false;
  }
  outputGenerator=-1;
  for (int i=0; i<this->Hcomponent.size; i++)
    if (!this->Hcomponent.TheObjects[i].IsEqualToZero())
    { if (!this->Hcomponent.TheObjects[i].IsEqualToOne()|| outputGenerator!=-1)
        return false;
      outputGenerator=i;
    }
  outputGenerator+=owner.GetNumPosRoots();
  return true;
}

void ElementUniversalEnvelopingOrdered::SetNumVariables(int newNumVars)
{ //this->ComputeDebugString();
  if (this->size==0)
    return;
  int currentNumVars=this->TheObjects[0].Coefficient.NumVars;
  if (currentNumVars==newNumVars)
    return;
  ElementUniversalEnvelopingOrdered Accum;
  Accum.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon.SetNumVariables(newNumVars);
    Accum.AddMonomial(tempMon);
  }
//  Accum.ComputeDebugString();
  this->operator=(Accum);
 // this->ComputeDebugString();
}

void MonomialUniversalEnvelopingOrdered::SetNumVariables(int newNumVars)
{ if (this->Coefficient.NumVars==newNumVars)
    return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((short)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((short)newNumVars);
}

std::string ParserNode::ElementToStringValueOnly(bool useHtml)
{ std::stringstream out;
  std::stringstream LatexOutput;
  PolynomialOutputFormat PolyFormatLocal;
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { out << " an integer of value: ";
    LatexOutput << this->intValue;
  }
  if (this->ExpressionType==this->typeRational)
  { out << " a rational number of value: ";
    LatexOutput << this->rationalValue.ElementToString();
  }
  if (this->ExpressionType==this->typePoly)
  { out << " a polynomial of value: ";
    LatexOutput << this->polyValue.GetElement().ElementToString(PolyFormatLocal);
  }
  if (this->ExpressionType==this->typeUEElementOrdered)
  { out << "an element of U(g) ordered:";
    LatexOutput << this->UEElementOrdered.GetElement().ElementToString(true);
  }
  if (this->ExpressionType==this->typeUEelement)
  { out << " an element of U(g) of value: ";
    LatexOutput << this->UEElement.GetElement().ElementToString();
  }
  if (this->ExpressionType==this->typeWeylAlgebraElement)
  { out << " a Weyl algebra element: ";
    LatexOutput << this->WeylAlgebraElement.GetElement().ElementToString(true);
  }
  if (this->ExpressionType==this->typeArray)
    out << " an array of " << this->array.GetElement().size << " elements. ";
  if (this->outputString!="")
  { if (this->ExpressionType!=this->typeString)
      out << "In addition, the program generated the following printout. ";
    else
      out << "A printout of value: ";
    out << this->outputString;
  }
  if (this->ExpressionType==this->typeError)
    out << this->ElementToStringErrorCode(useHtml);
  std::string tempS=LatexOutput.str();
  if (tempS!="")
  { if (!useHtml)
      out << tempS;
    else
    { out << "\n<div id=\"theResult\" class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
      out << tempS;
      out << "\\end{eqnarray*}</div>";
      out << "<textarea id=\"theResultLatex\" style=\"display: none\">";
      out << "\\begin{eqnarray*}" << tempS << "\\end{eqnarray*}";
      out << "</textarea>";
      out << "<br>\n<button id=\"ButtonToggleLatex\" onclick=\"switchMenu('theResult'); switchMenu('theResultLatex');\"\">Show LaTeX/Show eye candy</button>";
    }
  }
  return out.str();
}

bool EigenVectorComputation::AreUnimodular
(roots& input, Selection& outputBasisUnimodularity, GlobalVariables& theGlobalVariables)
{ int theRank=input.GetRankOfSpanOfElements(theGlobalVariables);
  Selection theSel;
  theSel.init(input.size);
  int NumCycles=MathRoutines::NChooseK(input.size, theRank);
  theSel.incrementSelectionFixedCardinality(theRank);
  roots currentSelection;
  root tempRoot2;
  bool isIntegral=true;
  for (int i=0; i<NumCycles; i++, theSel.incrementSelectionFixedCardinality(theRank))
  { input.SubSelection(theSel, currentSelection);
    if (currentSelection.GetRankOfSpanOfElements(theGlobalVariables)==theRank)
    { isIntegral=true;
      for (int j=0; j<input.size; j++)
        if(!theSel.selected[j])
        { input.TheObjects[j].GetCoordsInBasis(currentSelection, tempRoot2, theGlobalVariables);
          isIntegral=tempRoot2.IsIntegral();
          if(!isIntegral)
            break;
        }
      if (isIntegral)
        break;
    }
  }
  if(isIntegral)
    outputBasisUnimodularity.Assign(theSel);
  return isIntegral;
}

/*void EigenVectorComputation::TakeOutOfTheBracketFromTheRight
  (PolynomialRationalCoeff& input, NonExpandedOnTheRightRationalPoly& output)
{ int numVars=input.NumVars;
  List<bool> Explored(input.size, false);
  PolynomialRationalCoeff currentInnerPoly;
  Monomial<Rational> tempMon;
  for (int i=0; i<input.size; i++)
    if(!Explored.TheObjects[i])
    { Monomial<Rational>& currentMon=input.TheObjects[i];
      Explored.TheObjects[i]=true;
      int indexFirstNonZeroPowerFromTheRight=-1;
      int powerFirstNonZeroPowerFromTheRight=-1;
      for (int k=0; k<currentMon.NumVariables; k++)
        if (currentMon.degrees[k]!=0)
        { indexFirstNonZeroPowerFromTheRight=k;
          inde
          break;
        }
      if (indexFirstNonZeroPowerFromTheRight!=-1)
      { currentInnerPoly.Nullify(numVars);
        for (int j=i; j<input.size; j++)
          if(!Explored.TheObjects[j] && input.TheObjects[j].degrees[indexFirstNonZeroPowerFromTheRight]!=0)
          { Monomial<Rational>& currentInnerMon=input.nput.TheObjects[j];
            tempMon.Assign(currentInnerMon);
            tempMon.degrees[indexFirstNonZeroPowerFromTheRight]=0;
          }
      }
    }
}*/

void GeneralizedMonomialRational::MakeOne(int NumVars)
{ PolynomialRationalCoeff tempP;
  tempP.Nullify((short)NumVars);
  this->degrees.initFillInObject(NumVars, tempP);
  this->Coefficient.MakeNVarConst((short) NumVars, (Rational) 1);
}

void GeneralizedMonomialRational::Nullify
  (int numVars)
{ PolynomialRationalCoeff tempP;
  tempP.Nullify((short)numVars);
  this->degrees.initFillInObject(numVars, tempP);
  this->Coefficient.Nullify(numVars);
}

void GeneralizedPolynomialRational::Nullify
(int numVarsCoeff, int numberMainVars)
{ this->ClearTheObjects();
  this->NumCoeffVars=numVarsCoeff;
  this->NumMainVars=numberMainVars;
}

void EigenVectorComputation::WeylElementActsOnGeneralizedMonomial
  (ElementWeylAlgebra& inputWeylElement, GeneralizedMonomialRational& inputMonomial, GeneralizedPolynomialRational& output)
{ output.Nullify(inputMonomial.degrees.size, inputMonomial.Coefficient.size);
  PolynomialRationalCoeff weylBuffer;
  GeneralizedMonomialRational tempM;
  inputWeylElement.GetStandardOrder(weylBuffer);
  for (int i=0; i<weylBuffer.size; i++)
  { this->WeylMonomialActsOnGeneralizedMonomial(weylBuffer.TheObjects[i], inputMonomial, tempM);
    output.operator+=(tempM);
  }
}

void EigenVectorComputation::WeylMonomialActsOnGeneralizedMonomial
  (Monomial<Rational>& inputWeylMon, GeneralizedMonomialRational& inputMonomial, GeneralizedMonomialRational& output)
{ output.operator=(inputMonomial);
  for (int i=0; i<inputWeylMon.NumVariables/2; i++)
  { int posInWeylMon=i+inputWeylMon.NumVariables/2;
    for (int j=0; j<inputWeylMon.degrees[posInWeylMon]; j++)
    { output.Coefficient.MultiplyBy(output.degrees.TheObjects[i]);
      if (output.Coefficient.IsEqualToZero())
      { output.Nullify(inputMonomial.Coefficient.NumVars);
        return;
      }
      output.degrees.TheObjects[i]-=1;
    }
  }
  Monomial<Rational> tempM;
  tempM.MakeConstantMonomial(inputWeylMon.NumVariables/2, inputWeylMon.Coefficient);
  for (int i=0; i<inputWeylMon.NumVariables/2; i++)
    tempM.degrees[i]=inputWeylMon.degrees[i];
  output.Coefficient.MultiplyByMonomial(tempM);
}

void GeneralizedPolynomialRational::operator+=(const GeneralizedMonomialRational& theMon)
{ if (theMon.IsEqualToZero())
    return;
  int theIndex=this->IndexOfObjectHash(theMon);
  if (theIndex==-1)
    this->AddObjectOnTopHash(theMon);
  else
  { this->TheObjects[theIndex].Coefficient+=theMon.Coefficient;
    if (this->TheObjects[theIndex].IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

void GeneralizedMonomialRational::MakeGenericMon
(int numVars,  int startingIndexFormalExponent)
{ if (numVars<0)
    return;
  this->degrees.SetSizeExpandOnTopNoObjectInit(numVars);
  for (int i=0; i<startingIndexFormalExponent; i++)
    this->degrees.TheObjects[i].Nullify(numVars);
  for (int i=startingIndexFormalExponent; i<this->degrees.size; i++)
    this->degrees.TheObjects[i].MakeMonomialOneLetter((short) numVars, i, 1, (Rational) 1);
  this->Coefficient.MakeNVarConst(numVars, (Rational) 1);
}

std::string GeneralizedMonomialRational::ElementToString(PolynomialOutputFormat& PolyFormatLocal)
{ if (this->IsConstant())
    return this->Coefficient.ElementToString(PolyFormatLocal);
  std::stringstream out;
  std::string tempS;
  if (this->Coefficient.size>1) out << "(";
  this->Coefficient.ElementToString(tempS, PolyFormatLocal);
  if (tempS=="1")
    tempS="";
  if (tempS=="-1")
    tempS="-";
  out << tempS;
  if (this->Coefficient.size>1) out << ")";
  for (int i=0; i<this->degrees.size; i++)
    if (!this->degrees.TheObjects[i].IsEqualToZero())
    { out << "y_{" << i+1 << "}";
      if (!this->degrees.TheObjects[i].IsEqualToOne())
        out << "^{" << this->degrees.TheObjects[i].ElementToString(PolyFormatLocal) << "}";
    }
  return out.str();
}

std::string GeneralizedPolynomialRational::ElementToString(PolynomialOutputFormat& PolyFormatLocal)
{ if (this->size==0)
    return "0";
  std::stringstream out;
  std::string buffer;
  int sizeSinceLastCutOff=0;
  for (int i=0; i<this->size; i++)
  { buffer=this->TheObjects[i].ElementToString(PolyFormatLocal);
    if (buffer[0]!='-' && i!=0)
      out << "+";
    out << buffer;
    sizeSinceLastCutOff+=buffer.size();
    if (sizeSinceLastCutOff>200)
    { sizeSinceLastCutOff=0;
      out << "\\\\&&";
    }
  }
  return out.str();
}

void ParserNode::EvaluateSubstitution(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=2)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  if(!this->ConvertChildrenToType(this->typePoly))
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  this->polyValue=this->owner->TheObjects[this->children.TheObjects[0]].polyValue;
  this->polyBeingMappedTo=this->owner->TheObjects[this->children.TheObjects[1]].polyValue;
  this->ExpressionType=this->typeMap;
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
  int arraySize=firstChild.array.GetElement().size;
  int arrayIndex=secondChild.intValue-1;
  if (arrayIndex>=arraySize || arrayIndex<0)
  { this->SetError(this->errorBadIndex);
    return;
  }
  ParserNode& relevantChild=this->owner->TheObjects[firstChild.array.GetElement().TheObjects[arrayIndex]];
  this->CopyValue(relevantChild);
  return;
}

void ParserNode::EvaluateApplySubstitution(GlobalVariables& theGlobalVariables)
{ for (int i=0; i<this->children.size-1; i++)
    if (this->owner->TheObjects[this->children.TheObjects[i]].ExpressionType!=this->typeMap)
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
  PolynomialsRationalCoeff theSub;
  int theDimension=this->owner->NumVariables;
  theSub.MakeIdSubstitution((short) theDimension, (Rational) 1);
  List<bool> Explored(theDimension, false);
  for (int i=0; i<this->children.size-1; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children.TheObjects[i]];
    if (currentNode.polyValue.GetElement().size!=1)
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
    Monomial<Rational>& theMon=currentNode.polyValue.GetElement().TheObjects[0];
    int theLetterIndex;
    if (!theMon.IsOneLetterFirstDegree(theLetterIndex))
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
    if (Explored.TheObjects[theLetterIndex])
    { this->SetError(this->errorBadSubstitution);
      return;
    }
    Explored.TheObjects[theLetterIndex]=true;
    theSub.TheObjects[theLetterIndex]=currentNode.polyBeingMappedTo.GetElement();
    theSub.TheObjects[theLetterIndex]/=theMon.Coefficient;
    theSub.TheObjects[theLetterIndex].SetNumVariablesSubDeletedVarsByOne(this->owner->NumVariables);
  }
  ParserNode& lastNode=this->owner->TheObjects[*this->children.LastObject()];
  switch(lastNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typePoly:
      if (!lastNode.ConvertToType(this->typePoly))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->polyValue.GetElement().operator=(lastNode.polyValue.GetElement());
      this->polyValue.GetElement().Substitution(theSub, theDimension);
      //theSub.ElementToString(this->outputString);
      this->ExpressionType=this->typePoly;
      break;
    case ParserNode::typeWeylAlgebraElement:
      if (!lastNode.ConvertToType(this->typeWeylAlgebraElement))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->WeylAlgebraElement.GetElement().Assign(lastNode.WeylAlgebraElement.GetElement());
      if (!this->WeylAlgebraElement.GetElement().SubstitutionPolyPartOnly(theSub))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->ExpressionType=this->typeWeylAlgebraElement;
      break;
    default: this->SetError(this->errorDunnoHowToDoOperation); return;
  }

}

void ParserNode::CopyValue(const ParserNode& other)
{ this->intValue=other.intValue;
  this->ErrorType=other.ErrorType;
  this->rationalValue=other.rationalValue;
  this->ContextLieAlgebra=other.ContextLieAlgebra;
  this->children.CopyFromBase(other.children);
  this->Evaluated= other.Evaluated;
  this->outputString= other.outputString;
  this->ExpressionType=other.ExpressionType;
  if (other.ExpressionType==this->typeWeylAlgebraElement)
    this->WeylAlgebraElement.GetElement().Assign(other.WeylAlgebraElement.GetElementConst());
  if (other.ExpressionType==this->typeUEelement)
    this->UEElement.GetElement().operator=(other.UEElement.GetElementConst());
  if (other.ExpressionType==this->typeUEElementOrdered)
    this->UEElementOrdered.GetElement().operator=(other.UEElementOrdered.GetElementConst());
  if (other.ExpressionType==this->typePoly)
    this->polyValue.GetElement().operator=(other.polyValue.GetElementConst());
  if (other.ExpressionType==this->typeMap)
  { this->polyValue.GetElement().operator=(other.polyValue.GetElementConst());
    this->polyBeingMappedTo.GetElement().operator=(other.polyBeingMappedTo.GetElementConst());
  }
  if (other.ExpressionType==this->typeArray)
    this->array.GetElement().CopyFromBase(other.array.GetElementConst());
}

void ParserNode::operator=(const ParserNode& other)
{ this->owner=other.owner;
  this->indexParentNode=other.indexParentNode;
  this->indexInOwner=other.indexInOwner;
  this->Operation=other.Operation;
  this->CopyValue(other);
}

bool Parser::lookAheadTokenAllowsMapsTo(int theToken)
{ return theToken==this->tokenComma || theToken==this->tokenCloseBracket || theToken==this->tokenEnd || theToken==this->tokenEndStatement || theToken==this->tokenColon;
}

bool Parser::StackTopIsARoot(int& outputDimension)
{ return this->StackTopIsADelimiter1ECdotsCEDelimiter2(outputDimension, this->tokenOpenBracket, this->tokenCloseBracket);
}

void Parser::AddXECdotsCEX(int theDimension, int theOperation)
{ this->ExtendOnTop(1);
  ParserNode& lastNode=*this->LastObject();
  lastNode.Operation=theOperation;
  lastNode.children.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
  { int indexChild=this->ValueStack.TheObjects[this->ValueStack.size-2-2*i];
    lastNode.children.TheObjects[theDimension-1-i]=indexChild;
    this->TheObjects[indexChild].indexParentNode=this->size-1;
  }
  this->DecreaseStackSetExpressionLastNode(theDimension*2);
}

void Parser::AddXECdotsCEXEX(int theDimension, int theOperation)
{ this->ExtendOnTop(1);
  ParserNode& lastNode=*this->LastObject();
  lastNode.Operation=theOperation;
  lastNode.children.SetSizeExpandOnTopNoObjectInit(theDimension+1);
  for (int i=0; i<theDimension+1; i++)
  { int indexChild=this->ValueStack.TheObjects[this->ValueStack.size- theDimension*2-2+i*2];
    lastNode.children.TheObjects[i]=indexChild;
    this->TheObjects[indexChild].indexParentNode=this->size-1;
  }
  this->DecreaseStackSetExpressionLastNode(theDimension*2+2);
}

bool Parser::StackTopIsADelimiter1ECdotsCEDelimiter2(int& outputDimension, int LeftDelimiter, int RightDelimiter)
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

bool ElementWeylAlgebra::SubstitutionPolyPartOnly
(PolynomialsRationalCoeff& theSub)
{ Monomial<Rational> polyPartMon;
  Monomial<Rational> diffPartMon;
  PolynomialRationalCoeff tempP, Accum;
  Accum.Nullify(this->NumVariables*2);
  if (theSub.size!=this->NumVariables)
    return false;
  for (int i=0; i<this->NumVariables; i++)
    if (theSub.TheObjects[i].NumVars!=this->NumVariables)
      return false;
  for (int i=0; i<this->StandardOrder.size; i++)
  { polyPartMon.Assign(this->StandardOrder.TheObjects[i]);
    diffPartMon.Assign(this->StandardOrder.TheObjects[i]);
    for (int j=0; j<this->NumVariables; j++)
      diffPartMon.degrees[j]=0;
    polyPartMon.SetNumVariablesSubDeletedVarsByOne(this->NumVariables);
    polyPartMon.Coefficient.MakeOne();
    polyPartMon.Substitution(theSub, tempP, this->NumVariables);
    tempP.SetNumVariablesSubDeletedVarsByOne(this->NumVariables*2);
    tempP.MultiplyByMonomial(diffPartMon);
    Accum+=tempP;
  }
  this->StandardOrder=tempP;
  return true;
}
