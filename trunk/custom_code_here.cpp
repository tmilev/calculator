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
    case Parser::functionEigenOrdered: this->EvaluateEigenOrdered(theGlobalVariables); break;
    case Parser::functionSecretSauce: this->EvaluateSecretSauce(theGlobalVariables); break;
    case Parser::functionSecretSauceOrdered: this->EvaluateSecretSauceOrdered(theGlobalVariables); break;
    case Parser::functionWeylDimFormula: this->EvaluateWeylDimFormula(theGlobalVariables); break;
    case Parser::functionOuterAutos: this->EvaluateOuterAutos(theGlobalVariables); break;
    case Parser::functionMod: this->EvaluateModVermaRelations(theGlobalVariables); break;
    case Parser::functionInvariants: this->EvaluateInvariants(theGlobalVariables); break;
    case Parser::functionPrintDecomposition: this->EvaluatePrintDecomposition(theGlobalVariables); break;
    case Parser::functionEmbedding: this->EvaluatePrintEmbedding(theGlobalVariables); break;
    case Parser::functionPrintRootSystem: this->EvaluatePrintRootSystem(theGlobalVariables); break;
    case Parser::functionOrder: this->EvaluateOrder(theGlobalVariables); break;
   default: this->SetError(this->errorUnknownOperation); break;
  }
}

void ParserNode::EvaluateOrder(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (!theArgument.ConvertToType(this->typeUEElementOrdered, theGlobalVariables))
  { this->SetError(this->errorConversionError);
    return;
  }
  this->UEElementOrdered.GetElement()=theArgument.UEElementOrdered.GetElement();
  this->ExpressionType=this->typeUEElementOrdered;
}

void ParserNode::EvaluatePrintDecomposition(GlobalVariables& theGlobalVariables)
{ SSalgebraModule theModule;
  std::stringstream out, out2;
  theModule.InduceFromEmbedding(out2, this->owner->theHmm, theGlobalVariables);
  out << out2.str();
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
}

void ParserNode::EvaluatePrintRootSystem(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "<br>Symmetric Cartan matrix in Bourbaki order:<br><div class=\"math\">" << this->owner->theHmm.theRange.theWeyl.CartanSymmetric.ElementToString(false, true) << "</div>Root system:";
  for (int i=0; i<this->owner->theHmm.theRange.theWeyl.RootSystem.size; i++)
  { root& current=this->owner->theHmm.theRange.theWeyl.RootSystem.TheObjects[i];
    out << "<br>" << current.ElementToString();
  }
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
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

void ParserNode::EvaluateModVermaRelations(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (!this->ConvertChildrenToType(this->typeUEelement, theGlobalVariables))
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
  if (!theArgument.ConvertChildrenToType(this->typeRational, theGlobalVariables) || theDimension!=theHmm.theRange.theWeyl.CartanSymmetric.NumRows)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  root theWeight;
  theWeight.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
    theWeight.TheObjects[i]=current.rationalValue;
  }
  this->rationalValue= theHmm.theRange.theWeyl.WeylDimFormula(theWeight, theGlobalVariables);
  this->ExpressionType=this->typeRational;
}

void ParserNode::EvaluateEigenOrdered(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  int theDimension= theArgument.children.size;
  HomomorphismSemisimpleLieAlgebra& theHmm= this->owner->theHmm;
  if (theArgument.GetStrongestExpressionChildrenConvertChildrenIfNeeded(theGlobalVariables)!=this->typeIntegerOrIndex || theDimension!=theHmm.theDomain.theWeyl.CartanSymmetric.NumRows)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> > theList;
  root theWeight;
  theWeight.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
    theWeight.TheObjects[i]=current.intValue;
  }
  EigenVectorComputation theEigenComputation;
  this->outputString=theEigenComputation.ComputeEigenVectorsOfWeightConventionOrdered(this->owner->theHmm, this->owner->testAlgebra, theList, theWeight, theGlobalVariables);
  this->ExpressionType=this->typeString;
}

void ParserNode::EvaluateEigen(GlobalVariables& theGlobalVariables)
{ if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  int theDimension= theArgument.children.size;
  HomomorphismSemisimpleLieAlgebra& theHmm= this->owner->theHmm;
  if (theArgument.GetStrongestExpressionChildrenConvertChildrenIfNeeded(theGlobalVariables)!=this->typeIntegerOrIndex || theDimension!=theHmm.theDomain.theWeyl.CartanSymmetric.NumRows)
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  List<ElementUniversalEnveloping> theList;
  root theWeight;
  theWeight.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
    theWeight.TheObjects[i]=current.intValue;
  }
  EigenVectorComputation theEigenComputation;
  this->outputString=theEigenComputation.ComputeEigenVectorsOfWeight(this->owner->theHmm, this->owner->testAlgebra, theList, theWeight, theGlobalVariables);
  this->ExpressionType=this->typeString;
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
  if (input=="\\partial")
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
  if (input=="printEmbedding")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(this->functionEmbedding);
    return true;
  }
  if (input=="printDecomposition")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(this->functionPrintDecomposition);
    return true;
  }
  if (input=="printRootSystem")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunctionNoArgument);
    this->ValueBuffer.AddObjectOnTop(this->functionPrintRootSystem);
    return true;
  }
  if (input=="eigenOrdered")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionEigenOrdered);
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
  if (input=="order")
  { this->TokenBuffer.AddObjectOnTop(Parser::tokenFunction);
    this->ValueBuffer.AddObjectOnTop(this->functionOrder);
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
  output.SetSize(theRootSystem.size);
  root theScalarProducts;
  theScalarProducts.SetSize(rankSA);
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
  tempRoots.SetSize(this->imagesAllChevalleyGenerators.size);
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
  tempMon.Coefficient.MakeNVarConst((int)numVars, (Rational) 1);
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
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  tempMon.Powers.TheObjects[0].MakeNVarConst((int)numVars, 1);
  for (int i=0; i<input.NonZeroElements.CardinalitySelection; i++)
  { int theIndex=input.NonZeroElements.elements[i];
    int theGeneratorIndex=theOwner.RootIndexOrderAsInRootSystemToGeneratorIndexNegativeRootsThenCartanThenPositive(theIndex);
    tempMon.Coefficient.MakeNVarConst((int)numVars, input.coeffsRootSpaces.TheObjects[theIndex]);
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

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelations
  (bool SubHighestWeightWithZeroes, const PolynomialsRationalCoeff& highestWeightSub, GlobalVariables* theContext, const CoefficientType& theRingUnit)
{ MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  ElementUniversalEnvelopingOrdered<CoefficientType> output;
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ComputeDebugString();
    tempMon.ModOutVermaRelations(SubHighestWeightWithZeroes, highestWeightSub, theContext, theRingUnit);
    tempMon.ComputeDebugString();
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
  outputCoordinates.SetSize(theElements.size);
  PolynomialRationalCoeff ZeroPoly;
  ZeroPoly.Nullify((int)numVars);
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
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  for (int i=0; i<theDimension; i++)
    if (!input.TheObjects[i].IsEqualToZero())
    { (*tempMon.generatorsIndices.LastObject())=i+numPosRoots;
      tempMon.Powers.LastObject()->MakeNVarConst((int)numVars, 1);
      tempMon.Coefficient.MakeNVarConst((int)numVars, input.TheObjects[i]);
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
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
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
        tempP.RaiseToPower(theDegree, (Rational) 1);
      this->Coefficient.MultiplyBy(tempP);
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelations
  (bool SubHighestWeightWithZeroes, const PolynomialsRationalCoeff& highestWeightSub, GlobalVariables* theContext, const CoefficientType& theRingUnit)
{ int numPosRoots=this->owner->theOwner.GetNumPosRoots();
  int theDimension=this->owner->theOwner.GetRank();
  int theNumVars=this->Coefficient.NumVars;
  //this->ComputeDebugString();
  if (theNumVars<theDimension && !SubHighestWeightWithZeroes)
  { this->SetNumVariables(theDimension);
    theNumVars=theDimension;
  }
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices.TheObjects[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->Nullify(theNumVars, *this->owner, theContext);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { int theDegree;
      if (!this->Powers.TheObjects[i].IsSmallInteger(theDegree))
        return;
      if (SubHighestWeightWithZeroes)
      { this->Nullify(theNumVars, *this->owner, theContext);
        return;
      }
      PolynomialRationalCoeff tempP;
      root tempRoot;
      tempRoot.MakeZero(theNumVars);
      for (int i=0; i<this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.size; i++)
        tempRoot.TheObjects[i]=this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.TheObjects[i];
      tempP.MakeLinPolyFromRoot(tempRoot);
      tempP.Substitution(highestWeightSub, theNumVars, (Rational) 1);
      tempP.RaiseToPower(theDegree, (Rational) 1);
      tempP.ComputeDebugString();
      this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.ComputeDebugString();
      assert(this->Coefficient.checkConsistency());
      this->Coefficient.operator*=(tempP);
      assert(this->Coefficient.checkConsistency());
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

void MonomialUniversalEnveloping::Nullify(int numVars, SemisimpleLieAlgebra& theOwner)
{ this->Coefficient.Nullify((int)numVars);
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
  tempP.MakeNVarConst((int)numVars, coeff);
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

std::string RationalFunction::ElementToString(bool useLatex, bool breakLinesLatex)const
{ std::stringstream out;
  if (this->expressionType==this->typeRational)
  { out << this->ratValue.ElementToString();
    return out.str();
  }
  PolynomialOutputFormat PolyFormatLocal;
  bool hasDenominator=(this->expressionType==this->typeRationalFunction);
  if (hasDenominator && useLatex)
    out << "\\frac{";
  out << this->Numerator.GetElementConst().ElementToString(breakLinesLatex, PolyFormatLocal);
  if (hasDenominator)
  { if (useLatex)
      out << "}{";
    out << this->Denominator.GetElementConst().ElementToString(breakLinesLatex, PolyFormatLocal);
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
  bufferMon1.init((int)theNumVars);
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
  leftShift.init((int)theNumVars);
  rightShift.init((int)theNumVars);
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

void RationalFunction::gcd(const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, PolynomialRationalCoeff& buffer5, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList)
{ RationalFunction::lcm(left, right, buffer4, buffer1, buffer2, buffer3, buffer5, bufferMon1, bufferMon2, bufferList);
  left.MultiplyBy(right, buffer2, buffer1, bufferMon1);
//  std::cout << "<br>the product: " << buffer2.DebugString << " and the gcd: " << buffer4.DebugString << "<br>";
  buffer2.DivideBy(buffer4, output, buffer3);
}

void RationalFunction::lcm(const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, PolynomialRationalCoeff& buffer1, PolynomialRationalCoeff& buffer2, PolynomialRationalCoeff& buffer3, PolynomialRationalCoeff& buffer4, Monomial<Rational>& bufferMon1, Monomial<Rational>& bufferMon2, List<PolynomialRationalCoeff>& bufferList)
{ PolynomialRationalCoeff& leftTemp=buffer1;
  PolynomialRationalCoeff& rightTemp=buffer2;
  PolynomialRationalCoeff& tempP=buffer3;
  List<PolynomialRationalCoeff>& tempList=bufferList;
  leftTemp.Assign(left);
  rightTemp.Assign(right);
  assert(left.NumVars==right.NumVars);
  int theNumVars=left.NumVars;
  leftTemp.SetNumVariablesSubDeletedVarsByOne((int)theNumVars+1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne((int)theNumVars+1);
  leftTemp.ScaleToIntegralNoGCDCoeffs();
  rightTemp.ScaleToIntegralNoGCDCoeffs();
  tempP.MakeMonomialOneLetter((int)theNumVars+1,(int) theNumVars, 1, (Rational) 1);
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
      output.SetNumVariablesSubDeletedVarsByOne((int)theNumVars);
      return;
    }
  }
  output.Nullify((int)theNumVars);
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

void RationalFunction::operator*=(const PolynomialRationalCoeff& other)
{ if (other.IsEqualToZero())
  { this->Nullify(this->NumVars, this->context);
    return;
  }
  if (this->expressionType== this->typeRational)
    this->ConvertToType(this->typePoly);
  if (this->expressionType==this->typePoly)
  { this->Numerator.GetElement()*=other;
    this->ReduceMemory();
    return;
  }
  PolynomialRationalCoeff theGCD, theResult, tempP;
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ElementToString() << " by " << other.ElementToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
  RationalFunction::gcd(this->Denominator.GetElement(), other, theGCD, this->context);
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
  tempP.ComputeDebugString();
  assert(tempP.IsEqualToZero());
  this->Denominator.GetElement()=theResult;
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ElementToString() << " by " << other.ElementToString();
    out << " and the result is:\n" << this->ElementToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
//  this->ComputeDebugString();
}

void RationalFunction::operator*=(const Rational& other)
{ assert(this->checkConsistency());
  if (other.IsEqualToZero())
  { this->Nullify(this->NumVars, this->context);
    return;
  }
  switch(this->expressionType)
  { case RationalFunction::typeRational: this->ratValue*=other; return;
    case RationalFunction::typePoly: this->Numerator.GetElement().TimesConstant(other); return;
    case RationalFunction::typeRationalFunction: this->Numerator.GetElement().TimesConstant(other); this->SimplifyLeadingCoefficientOnly(); return;
  }
}

void RationalFunction::operator*=(const RationalFunction& other)
{ assert(this->NumVars==other.NumVars);
  assert(this->checkConsistency());
  assert(other.checkConsistency());
  assert(this!=&other);
  if (other.context!=0)
    this->context=other.context;
  if (other.IsEqualToZero() || this->IsEqualToZero())
  { this->Nullify(this->NumVars, this->context);
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
  { PolynomialRationalCoeff tempP;
    tempP=this->Numerator.GetElement();
    this->operator=(other);
    this->operator*=(tempP);
    return;
  }
  PolynomialRationalCoeff theGCD1, theGCD2, tempP1, tempP2;
  //this->ComputeDebugString();
//  RationalFunction tempde_Bugger;
//  tempde_Bugger=other;
//  tempde_Bugger.ComputeDebugString();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ElementToString() << " by " << other.ElementToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
  RationalFunction::gcd(other.Denominator.GetElementConst(), this->Numerator.GetElement(), theGCD1, this->context);
  RationalFunction::gcd(this->Denominator.GetElement(), other.Numerator.GetElementConst(), theGCD2, this->context);
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
  this->Numerator.GetElement().MultiplyBy(tempP1);
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
  if (this->context!=0)
  { std::stringstream out;
    out << "Multiplying " << this->ElementToString() << " by " << other.ElementToString();
    out << " and the result is:\n" << this->ElementToString();
    this->context->theIndicatorVariables.StatusString1NeedsRefresh=true;
    this->context->theIndicatorVariables.StatusString1=out.str();
    this->context->MakeReport();
  }
}

void RationalFunction::Simplify()
{ if (this->expressionType==this->typeRationalFunction)
    if(!this->Numerator.GetElement().IsEqualToZero())
    { PolynomialRationalCoeff theGCD, tempP, tempP2;
      this->gcd(this->Numerator.GetElement(), this->Denominator.GetElement(), theGCD, this->context);
      this->Numerator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Numerator.GetElement().Assign(tempP);
      this->Denominator.GetElement().DivideBy(theGCD, tempP, tempP2);
      this->Denominator.GetElement().Assign(tempP);
    }
  this->ReduceMemory();
  this->SimplifyLeadingCoefficientOnly();
}

void RationalFunction::SimplifyLeadingCoefficientOnly()
{ if (this->expressionType!=this->typeRationalFunction)
    return;
  Rational tempRat= this->Denominator.GetElement().TheObjects[this->Denominator.GetElement().GetIndexMaxMonomialTotalDegThenLexicographic()].Coefficient;
  tempRat.Invert();
  this->Denominator.GetElement().TimesConstant(tempRat);
  this->Numerator.GetElement().TimesConstant(tempRat);
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

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductElement;
template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductSpaceAndElements;

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductSpace
{
public:
  List<ElementLeft> leftSpaceBasis;
  List<ElementRight> rightSpaceBasis;
  roots leftWeights;
  roots rightWeights;
  CoefficientType theRingZerO;
  CoefficientType theRingUniT;
  std::string ElementToString();
  bool GetInternalRepresentationFromVectorLeft
  (const ElementLeft& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
  ;
  void GetInternalRepresentationFromVectorRight
  (const ElementRight& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
  ;
  void GetInternalRepresentationFromLeftAndRight
  (const ElementLeft& leftInput, const ElementRight& rightInput, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
  { Polynomial<CoefficientType> tempP;
    this->GetInternalRepresentationFromVectorLeft(leftInput, tempP, theGlobalVariables);
    this->GetInternalRepresentationFromVectorRight(rightInput, output, theGlobalVariables);
    output.MultiplyBy(tempP);
  }
  void GetEltFromIndex(int theIndex, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output, GlobalVariables& theGlobalVariables)
  { int leftIndex=theIndex/this->rightSpaceBasis.size;
    int rightIndex=theIndex% this->rightSpaceBasis.size;
    output.SetTensorProduct(this->leftSpaceBasis.TheObjects[leftIndex], this->rightSpaceBasis.TheObjects[rightIndex], *this, theGlobalVariables);
  }
  void GetLeftAndRightIndexFromMonomial(const Monomial<CoefficientType>& input, int& leftIndex, int& rightIndex)
  { for (int i=0; i<this->leftSpaceBasis.size; i++)
      if (input.degrees[i]==1)
      { leftIndex=i;
        break;
      }
    for (int i=0; i<this->rightSpaceBasis.size; i++)
      if (input.degrees[i+this->leftSpaceBasis.size]==1)
      { rightIndex=i;
        return;
      }
  }
  int GetIndexFromMonomial(const Monomial<CoefficientType>& input)
  { int leftIndex=-1; int rightIndex=-1;
    this->GetLeftAndRightIndexFromMonomial(input, leftIndex, rightIndex);
    assert(rightIndex!=-1 && leftIndex!=-1);
    return leftIndex*this->rightSpaceBasis.size+rightIndex;
  }
  void GetComponentsFromInternalRepresentation
  (const Monomial<CoefficientType>& input, ElementLeft& leftOutput, ElementRight& rightOutput)
  ;
};

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductElement
{
  //Format.
  //Let the owner TensorProductSpace be of the form V_1 \otimes V_2.
  //Then the internalRepresentation is a second degree homogeneous polynomial of dim V_1 +dim V_2 variables.
  //A monomial from the internalRepresentation must be of the form (coefficient)*x_i*x_j, where i<dim V_1 and j>=dim V_1.
  //Then the monomial (coefficient)*x_i*x_j, corresponds to the tensor product (coefficient)v_i\otimes v_k, where k=j-dim V_1,
  // v_i corresponds to the i^th member of TensorProductSpace::leftSpaceBasis and v_k correponds to the k^th member of TensorProductSpace::rightSpaceBasis
  friend class TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>;
  Polynomial<CoefficientType> internalRepresentation;
public:
  std::string DebugString;
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  std::string ElementToString();
  std::string ElementToString
  (TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner)
  ;
  void SetTensorProduct
  (const  ElementLeft& leftVector, const  ElementRight& rightVector, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
  ;
  void SetTensorProductBasisElement
  (int leftIndex, int rightIndex, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
  ;
  void ActOnMe
  (const ElementSimpleLieAlgebra& theElt, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theStartingSpace,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theTargetSpace,
   SemisimpleLieAlgebra& theAlgebra, GlobalVariables& theGlobalVariables)
  ;
  void ActOnMe
  (Polynomials<CoefficientType>& theSub, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output, const CoefficientType& theRingUnit)
  { this->internalRepresentation.Substitution(theSub, output.internalRepresentation, this->internalRepresentation.NumVars, theRingUnit);
  }
  void ActOnMe
  (Polynomials<CoefficientType>& theSub, const CoefficientType& theRingUnit)
  { this->internalRepresentation.Substitution(theSub, this->internalRepresentation.NumVars, theRingUnit);
  }
  void ElementToVector
  (Vector<CoefficientType>& output, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)const
  ;
  void AssignVector
  (Vector<CoefficientType>& input, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
  ;
  void Nullify(TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner){ this->internalRepresentation.Nullify(owner.leftSpaceBasis.size+owner.rightSpaceBasis.size);}
  void operator+=(const TensorProductElement& other){ this->internalRepresentation+=other.internalRepresentation; }
  void operator/=(const CoefficientType& theCoeff){ this->internalRepresentation/=theCoeff; }
  void operator-=(const TensorProductElement& other){ this->internalRepresentation.Subtract(other.internalRepresentation); }
  bool IsEqualToZero()const{return this->internalRepresentation.IsEqualToZero();}
};

template <class ElementLeft, class ElementRight, class CoefficientType>
class TensorProductSpaceAndElements: public List<TensorProductElement<ElementLeft, ElementRight, CoefficientType> >
{
  public:
  TensorProductSpace<ElementLeft, ElementRight, CoefficientType> theTargetSpace, theStartingSpace;
  List<Polynomials<CoefficientType> > simplePositiveGeneratorsActions;
  List<Polynomials<CoefficientType> > simpleNegativeGeneratorsActions;
  std::string DebugString;
  std::string ElementToString();
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> theSeedVector;
  root theRightLowestWeight;
  static bool flagAnErrorHasOccurredTimeToPanic;
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  void initTheSpacesForAdjointAction
    (List<ElementSimpleLieAlgebra>& theElementsActing,
     SemisimpleLieAlgebra& theAlgebra,
     List<ElementLeft>& theLeftElts,
     List<ElementRight>& theRightElts,
     const ElementLeft& leftZero,
     const ElementRight& rightZero,
     const CoefficientType& theRingZero,
     const CoefficientType& theRingUnit,
     GlobalVariables& theGlobalVariables)
     ;
  std::string ExtractHighestWeightsCorrespondingToMaxVermaSummands
    (root& input, roots& outputVectors, List<MatrixLargeRational>& simplePosGenerators, List<MatrixLargeRational>& simpleNegGenerators,
     GlobalVariables& theGlobalVariables)
;
  std::string ExtractHighestWeightVectorsFromVector
( TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input, List<TensorProductElement<ElementLeft, ElementRight, CoefficientType> >& outputVectors,
  GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
  ;
  void ClimbDownFromHighestWeightAlongSl2String
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
    CoefficientType& outputCoeff,
    Polynomials<CoefficientType>& posGenerator,
    Polynomials<CoefficientType>& negGenerator, int generatorPower, const CoefficientType& theRingUnit)
  ;
  void ClimbDownFromVectorAccordingToSequence
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& outputLastNonZero, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
  ;
  void ClimbUpFromVector
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
  ;

  void initTheSpacesSl2StringVersion
  ( List<ElementSimpleLieAlgebra>& theSimplePosElts,  List<ElementSimpleLieAlgebra>& theSimpleNegElts, SemisimpleLieAlgebra& theAlgebra,
    const ElementLeft& leftZero, const ElementRight& rightZero, const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
    GlobalVariables& theGlobalVariables)
     ;
  void GetRootForm(roots& output, GlobalVariables& theGlobalVariables);
  bool FindEigenVectorsWithRespectTo
  (List<ElementSimpleLieAlgebra>& theElementsActing,  List<ElementLeft>& theLeftElts, List<ElementRight>& theRightElts,
   SemisimpleLieAlgebra& theAlgebra, TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>& output,
   const ElementLeft& leftZero, const ElementRight& rightZero,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
  ;
  bool FindEigenVectorsWithRespectToSl2StringVersion
  (List<ElementSimpleLieAlgebra>& theSimplePosElts,  List<ElementSimpleLieAlgebra>& theSimpleNegElts,
   SemisimpleLieAlgebra& theAlgebra, TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>& output,
   const ElementLeft& leftZero, const ElementRight& rightZero,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
  ;
  void PrepareTheMatrix
  (List<ElementSimpleLieAlgebra>& theElementsActing, SemisimpleLieAlgebra& theAlgebra, Matrix<CoefficientType>& output, GlobalVariables& theGlobalVariables)
  ;
};

template < > int List<TensorProductElement<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> >::ListActualSizeIncrement=10;

template <class ElementLeft, class ElementRight, class CoefficientType>
bool TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::flagAnErrorHasOccurredTimeToPanic=false;

class TranslationFunctorGmodKVermaModule
{
public:
  List<TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> > theElements;
  List<TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> > theEigenVectors;
  TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> theSpace;
  List<TensorProductElement<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction> >  theOutput;
  roots theModuleWeightsShifted;
  roots theModuleWeightsNonShifted;
  List<ElementSimpleLieAlgebra> ElementsOfKofNegativeWeights;
  List<ElementVermaModuleOrdered<RationalFunction> > leftComponents;
  List<ElementSimpleLieAlgebra> rightComponents;
  List<List<ElementVermaModuleOrdered<RationalFunction> > > theLeftComponentsByWeight;
  List<List<ElementSimpleLieAlgebra> > theRightComponentsByWeight;
  VectorPartition theVectorPartition;
  PolynomialsRationalCoeff HighestWeightSub;
  std::string RunTheComputationSl2StringVersion
  (Parser& theParser, GlobalVariables& theGlobalVariables, EigenVectorComputation& owner)
  ;

  std::string RunTheComputation
  (Parser& theParser, GlobalVariables& theGlobalVariables, EigenVectorComputation& owner)
  ;
  std::string initTheWeights
    (Parser& theParser, GlobalVariables& theGlobalVariables)
  ;

  std::string initTheCandidateMonomials
    (Parser& theParser, GlobalVariables& theGlobalVariables)
  ;
  std::string initTheCandidateMonomialsSl2StringVersion
    (Parser& theParser, GlobalVariables& theGlobalVariables)
  ;
  std::string GetLeftAndRightFromVP
  (int indexTotalWeight, int indexRightWeight, VectorPartition& theVP, Parser& theParser, GlobalVariables& theGlobalVariables)
 ;
  std::string GetLeftFromVP
  ( VectorPartition& theVP, Parser& theParser, GlobalVariables& theGlobalVariables)
 ;
};


template<class CoefficientType>
class GeneralizedVermaModuleData
{
  public:
  List<ElementSimpleLieAlgebra> theFDspace;
  SemisimpleLieAlgebraOrdered theOwner;
  PolynomialsRationalCoeff VermaHighestWeighSub;
  //The order of the below is chosen so that the opposite generator of posGenerators.TheObjects[i] is negGenerators.TheObjects[i]
  List<ElementSimpleLieAlgebra> posGenerators, negGenerators;
  CoefficientType theRingUnit;
  CoefficientType theRingZero;
  void init(Parser& input, GlobalVariables* theContext)
  { RationalFunction RFOne, RFZero;
    RFOne.MakeNVarConst(input.theHmm.theRange.GetRank(), (Rational) 1, theContext);
    RFZero.Nullify(input.theHmm.theRange.GetRank(), theContext);
    this->theRingUnit=RFOne;
    this->theRingZero=RFZero;
    this->posGenerators.size=0; this->negGenerators.size=0;
    int domainRank=input.theHmm.theDomain.GetRank();
    int numDomainPosRoots=input.theHmm.theDomain.GetNumPosRoots();
    for (int i=0; i<domainRank; i++)
    { this->posGenerators.AddObjectOnTop(input.theHmm.imagesAllChevalleyGenerators.TheObjects
        [domainRank+numDomainPosRoots+i]);
      this->negGenerators.AddObjectOnTop(input.theHmm.imagesAllChevalleyGenerators.TheObjects
        [numDomainPosRoots-i-1]);
    }
    this->theOwner=input.testAlgebra;
    this->theFDspace=input.theHmm.GmodK;
  }
};

template<class CoefficientType>
class ElementGeneralizedVerma
{
  public:
  std::string DebugString;
  std::string ElementToString(){ return this->ElementToString(true, false, true);}
  std::string ElementToString
  (bool displayLeftComponenetsOnTheRight, bool useOtimes, bool useFs)
;
  void ComputeDebugString(){this->DebugString=this->ElementToString();}
  GeneralizedVermaModuleData<CoefficientType>* theOwner;
  List<ElementVermaModuleOrdered<CoefficientType> > leftComponents;
  ElementGeneralizedVerma(){this->theOwner=0;}
  std::string ExtractHighestWeightVectors
  (Parser& theParser, List<ElementGeneralizedVerma<CoefficientType> >& outputVectors,
    GlobalVariables& theGlobalVariables)
;
  void Nullify
  (GeneralizedVermaModuleData<CoefficientType>& owner)
    ;
  void AssignDefaultGeneratorIndex
  (int theIndex, GeneralizedVermaModuleData<CoefficientType>& owner, GlobalVariables* theContext)
  { this->Nullify(owner);
    ElementVermaModuleOrdered<CoefficientType>& currentElt=this->leftComponents.TheObjects[theIndex];
    ElementVermaModuleOrdered<CoefficientType> zeroVermaElt;
    zeroVermaElt.Nullify(this->theOwner->theOwner, this->theOwner->VermaHighestWeighSub);
    ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
    tempElt.MakeConst(this->theOwner->theRingUnit, this->theOwner->theOwner);
    currentElt.AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector(tempElt, zeroVermaElt, theContext, this->theOwner->theRingUnit);
    currentElt.theSub=this->theOwner->VermaHighestWeighSub;
  }
  std::string GetStringFormGmodKindex(int theIndex);
  void ActOnMe
  (const ElementSimpleLieAlgebra& theElt, ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
;
  bool ActOnMe
  (const ElementUniversalEnvelopingOrdered<CoefficientType>& theElt, ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
;
  void ActOnMe(const ElementSimpleLieAlgebra& theElt, GlobalVariables* theContext) {ElementGeneralizedVerma<CoefficientType> output; this->ActOnMe(theElt, output, theContext); this->operator=(output);};

  void ClimbDownFromHighestWeightAlongSl2String
(ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& output,
    CoefficientType& outputCoeff,
   const ElementSimpleLieAlgebra& posGenerator, const ElementSimpleLieAlgebra& negGenerator,
     int generatorPower, GlobalVariables& theGlobalVariables)
  ;
  void ClimbDownFromVectorAccordingToSequence
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& outputLastNonZero, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables)
  ;
  void ClimbUpFromVector
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& outputLastNonZero,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
  ;
  void operator=(const ElementGeneralizedVerma<CoefficientType>& other)
  { this->leftComponents=other.leftComponents;
    this->theOwner=other.theOwner;
  }
  bool IsEqualToZero()const
  { for (int i=0; i<this->leftComponents.size; i++)
      if (!this->leftComponents.TheObjects[i].IsEqualToZero())
        return false;
    return true;
  }
  void operator/=(const CoefficientType& theConst)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]/=theConst;
  }
  void operator*=(const CoefficientType& theConst)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]*=theConst;
  }
  void operator-=(const ElementGeneralizedVerma<CoefficientType>& other)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]-=other.leftComponents.TheObjects[i];
  }
  void operator+=(const ElementGeneralizedVerma<CoefficientType>& other)
  { for (int i=0; i<this->leftComponents.size; i++)
      this->leftComponents.TheObjects[i]+=other.leftComponents.TheObjects[i];
  }
  bool IsProportionalTo(const ElementGeneralizedVerma<CoefficientType>& other, CoefficientType& outputTimesMeEqualsInput)const
  { assert(this->leftComponents.size==other.leftComponents.size);
    if (this->IsEqualToZero())
    { if (other.IsEqualToZero())
      { outputTimesMeEqualsInput=this->theOwner->theRingZero;
        return true;
      }
      return false;
    }
    for (int i=0; i<this->leftComponents.size; i++)
      if (!this->leftComponents.TheObjects[i].IsEqualToZero())
      { if (!this->leftComponents.TheObjects[i].IsProportionalTo(other.leftComponents.TheObjects[i], outputTimesMeEqualsInput, this->theOwner->theRingZero))
          return false;
        else
          break;
      }
    ElementGeneralizedVerma<CoefficientType> tempElt;
    tempElt=*this;
    tempElt*=outputTimesMeEqualsInput;
    tempElt-=other;
    return tempElt.IsEqualToZero();
  }
  void ClearDenominators
  (CoefficientType& outputWasMultipliedByThis)
  ;
};

template < > int List<ElementGeneralizedVerma<RationalFunction> >::ListActualSizeIncrement=10;

std::string EigenVectorComputation::ComputeAndReturnStringOrdered
(GlobalVariables& theGlobalVariables, Parser& theParser, int NodeIndex)
{ std::stringstream out;
//  TranslationFunctorGmodKVermaModule theTranslationFunctor;
//  out << theTranslationFunctor.RunTheComputationSl2StringVersion(theParser, theGlobalVariables, *this);
  roots theAlgebraWeights;
  theParser.theHmm.GetWeightsGmodK(this->theModuleWeightsShifted, theGlobalVariables);
  theParser.theHmm.GetWeightsK(theAlgebraWeights, theGlobalVariables);
  GeneralizedVermaModuleData<RationalFunction> theData;
  ElementGeneralizedVerma<RationalFunction> startingElement;
  this->PrepareCartanSub(theParser.testAlgebra, theData.VermaHighestWeighSub, theGlobalVariables);
  theData.init(theParser, &theGlobalVariables);
//  startingElement.AssignDefaultGeneratorIndex(0, theData, &theGlobalVariables);
  startingElement.AssignDefaultGeneratorIndex(0, theData, &theGlobalVariables);
  List<ElementGeneralizedVerma<RationalFunction> > theEigenVectors;
  out << "<br>the starting element is: " << startingElement.ElementToString();
  out << startingElement.ExtractHighestWeightVectors(theParser, theEigenVectors, theGlobalVariables);
  return out.str();
  ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> theElt, tempElt1, tempElt2;
  PolynomialOutputFormat PolyFormatLocal;
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
  PolynomialRationalCoeff polyOne, polyZero;
  polyOne.MakeNVarConst(theRangeRank+numRangePosRoots, (Rational) 1);
  polyZero.Nullify(theRangeRank+numRangePosRoots);
  for (int i=0; i<theDomainRank; i++)
  { tempElt1.AssignElementLieAlgebra(theParser.theHmm.imagesSimpleChevalleyGenerators.TheObjects[i], polyOne, polyZero, theParser.testAlgebra);
    tempElt1.LieBracketOnTheRight(theElt, tempElt2);
    tempElt2.Simplify(&theGlobalVariables);
    std::cout << "<br>" << tempElt1.ElementToString();
    out << "<div class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
    out << "[" << tempElt1.ElementToString(true, PolyFormatLocal) << "," << theElt.ElementToString(true, PolyFormatLocal) << "]=" << tempElt2.ElementToString(true, PolyFormatLocal);
    out << "\\end{eqnarray*}</div>";
    std::cout << "<br>" << tempElt2.ElementToString();
    tempElt2.ModOutVermaRelationS(&theGlobalVariables, polyOne);
    out << "mod Verma rels:";
    out << "<div class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
    out << tempElt2.ElementToString(true, PolyFormatLocal);
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
  theParser.NumVariables=theRangeRank+numRangePosRoots;
//  out << this->CrunchTheOperators(theGlobalVariables, theParser);
  for (int i=0; i<theParser.testAlgebra.theOrder.size; i++)
  { out << "<br>f_{" << i-theParser.testAlgebra.theOwner.GetNumPosRoots()-theParser.testAlgebra.theOwner.GetRank();
    out << "} = " << theParser.testAlgebra.theOrder.TheObjects[i].ElementToStringNegativeRootSpacesFirst(true, false, theParser.theHmm.theRange);
  }
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
  theParser.NumVariables=numRangePosRoots+theRangeRank;
  return out.str();
}

void RootIndexToPoly(int theIndex, SemisimpleLieAlgebra& theAlgebra, PolynomialRationalCoeff& output)
{ int theRank=theAlgebra.theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=theAlgebra.theWeyl.RootsOfBorel.size;
  output.MakeNVarDegOnePoly((int)(theRank+numPosRoots), theIndex+theRank, (Rational) 1);
}

void EigenVectorComputation::DetermineEquationsFromResultLieBracketEquationsPerTarget(Parser& theParser, ElementUniversalEnveloping& theStartingGeneric, ElementUniversalEnveloping& theElt, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
//  int dimQuotient=theParser.theHmm.theRange.theWeyl.RootsOfBorel.size- theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numCoeffVars=theRangeRank+numRangePosRoots;
  PolynomialRationalCoeff theDiffPoly;
  PolynomialOutputFormat PolyFormatLocal;
  MonomialUniversalEnveloping& originalMon= theStartingGeneric.TheObjects[0];
  this->theExponentShiftsTargetPerSimpleGenerator.SetSize(this->theExponentShiftsTargetPerSimpleGenerator.size+1);
  roots& currentTargetShifts =*this->theExponentShiftsTargetPerSimpleGenerator.LastObject();
  currentTargetShifts.SetSize(theElt.size);
  ElementWeylAlgebra tempWeyl;
  this->theOperators.SetSize(this->theOperators.size+1);
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
  RationalFunction ZeroRF;//, RFOne;
  ZeroRF.MakeNVarConst(numCoeffVars, (Rational) 0, &theGlobalVariables);
 // RFOne.MakeNVarConst(numCoeffVars, (Rational) 1, &theGlobalVariables);
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
        tempP.Substitution(theSub, numCoeffVars, (Rational) 1);
        tempP.ComputeDebugString();
        this->theSystem.elements[indexCurrentRow][theVarIndex]=tempP;
      }
  }
}

void EigenVectorComputation::DetermineEquationsFromResultLieBracketEquationsPerTargetOrdered
  (Parser& theParser, int nodeIndex, ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theStartingGeneric, ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theElt, std::stringstream& out, GlobalVariables& theGlobalVariables)
{ int theRangeRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int numRangePosRoots= theParser.theHmm.theRange.theWeyl.RootsOfBorel.size;
//  int dimQuotient=theParser.theHmm.theRange.theWeyl.RootsOfBorel.size- theParser.theHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numCoeffVars=theRangeRank+numRangePosRoots;
  PolynomialRationalCoeff theDiffPoly;
  MonomialUniversalEnvelopingOrdered<PolynomialRationalCoeff>& originalMon= theStartingGeneric.TheObjects[0];
  this->theExponentShiftsTargetPerSimpleGenerator.SetSize(this->theExponentShiftsTargetPerSimpleGenerator.size+1);
  roots& currentTargetShifts =*this->theExponentShiftsTargetPerSimpleGenerator.LastObject();
  currentTargetShifts.SetSize(theElt.size);
  ElementWeylAlgebra tempWeyl;
  this->theOperators.SetSize(this->theOperators.size+1);
  ElementWeylAlgebra& currentOperator = *this->theOperators.LastObject();
  currentOperator.Nullify(numCoeffVars);
  PolynomialOutputFormat PolyFormatLocal;
  for (int i=0; i<theElt.size; i++)
  { MonomialUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theMon= theElt.TheObjects[i];
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
  tempP.MakeNVarConst((int) RangeRank+dimPosPartQuotient, (Rational) 1);
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
  tempP.MakeNVarConst((int) theRank+numPosRoots, (Rational) 1);
  tempMon.MakeConst(tempP, owner);
  for (int i=0; i<numPosRoots; i++)
  { tempP.MakeNVarDegOnePoly(theRank+numPosRoots, theRank+i, (Rational) 1);
    tempMon.MultiplyByGeneratorPowerOnTheRight(i, tempP);
  }
  theElt.AddMonomial(tempMon);
}

void EigenVectorComputation::MakeGenericVermaElementOrdered(ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>& theElt, SemisimpleLieAlgebraOrdered& owner)
{ int numPosRoots= owner.theOwner.theWeyl.RootsOfBorel.size;
  int theRank=owner.theOwner.GetRank();
  theElt.Nullify(owner);
  MonomialUniversalEnvelopingOrdered<PolynomialRationalCoeff> tempMon;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst((int) theRank+numPosRoots, (Rational) 1);
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
        output.MultiplyOnTheRight(tempW);
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
  posGenerators.SetSize(theDomainRank);
  negGenerators.SetSize(theDomainRank);
  allSimpleGenerators.SetSize(theDomainRank*2);
  this->moduleElementsEmbedded.size=0;
  theHmm.GmodK.size=0;
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
          if (!theModules.LastObject()->LinSpanContainsRoot(oneRootFromDomainAlgebra, theGlobalVariables))
          { for (int k=0; k<BasisNewlyFoundElements.size; k++)
            { this->moduleElementsEmbedded.SetSize(this->moduleElementsEmbedded.size+1);
              this->moduleElementsEmbedded.LastObject()->AssignVectorNegRootSpacesCartanPosRootSpaces(BasisNewlyFoundElements.TheObjects[k], theHmm.theRange);
              theHmm.GmodK.AddObjectOnTop(*this->moduleElementsEmbedded.LastObject());
              //std::cout << "<br> Just added to g mod k: " << theHmm.GmodK.LastObject()->ElementToStringNegativeRootSpacesFirst(true, false, theHmm.theRange);
            }
            if (IndexFirstModuleNotEqualToDomain==-1)
              IndexFirstModuleNotEqualToDomain=theModules.size-1;
          } else
          { out << "<br>Basis of the subalgebra:<br><table>";
            for (int k=0; k<theHmm.imagesAllChevalleyGenerators.size; k++)
            { std::stringstream displayString;
              if (k>= theHmm.theDomain.GetNumPosRoots()+theHmm.theDomain.GetRank())
                displayString << "f_{" << k+(-theHmm.theDomain.GetNumGenerators()+1+theHmm.theRange.GetNumPosRoots()) << "}=";
              if (k< theHmm.theDomain.GetNumPosRoots()+theHmm.theDomain.GetRank() && k>=theHmm.theDomain.GetNumPosRoots())
                displayString << "f_{0," << k-theHmm.theDomain.GetNumPosRoots() << "}=f_{" << k+theHmm.theRange.GetNumPosRoots()+1-theHmm.theDomain.GetNumPosRoots() << "}=";
              if (k< theHmm.theDomain.GetNumPosRoots())
                displayString << "f_{" << -theHmm.theRange.GetNumPosRoots()+k<< "}=";
              out << "<tr><td>" << displayString.str() << "</td><td>" << theHmm.imagesAllChevalleyGenerators.TheObjects[k].ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange) << "</td></tr>";
            }
            out << "</table>";
          }
        }
    }
    if (BasisAllFoundElements.size==numGeneratorsRange)
      break;
  }
  out << "<br>basis of generated module:<br><table> ";
  for (int k=0; k<theHmm.GmodK.size; k++)
  { std::stringstream displayString;
    if (k==theHmm.GmodK.size/2)
      displayString << "f_{0," << theHmm.theRange.GetRank() << "}=f_{" << theHmm.theRange.GetNumPosRoots()+theHmm.theRange.GetRank() << "}=";
    if (k>theHmm.GmodK.size/2)
      displayString << "f_{" << k-theHmm.GmodK.size/2 << "}=";
    if (k<theHmm.GmodK.size/2)
      displayString << "f_{" << k-theHmm.GmodK.size/2 << "}=";
    out << "<tr><td>" << displayString.str() <<"</td><td>" << theHmm.GmodK.TheObjects[k].ElementToStringNegativeRootSpacesFirst(false, false, theHmm.theRange) << "</td></tr>" ;
  }
  out << "</table>";
  roots theAlgebra;
  roots& theModule= theModules.TheObjects[IndexFirstModuleNotEqualToDomain];
  this->actionsNegativeRootSpacesCartanPositiveRootspaces.SetSize(numGeneratorsDomain);
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
    //theModule.ComputeDebugString();
    //tempRoot1.ComputeDebugString();
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
{ output.SetSize(input.size);
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
      { tempMon.MakeNVarFirstDegree(k, (int) theDim, theOperator.elements[k][i]);
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
  theCartanActions.SetSize(theDim);
  theWeight.SetSize(theDim);
  for (int i=0; i<theRank; i++)
  { root& current=theCartanActions.TheObjects[i];
    current.SetSize(theDim);
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
  this->invariantsFound.SetSize(tempRoots.size);
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
  this->invariantsMappedToEmbedding.SetSize(this->invariantsFound.size);
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
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddObjectOnTopNoRepetitionOfObjectHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSize(theElements.size);
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
  oneRF.MakeNVarConst(numVars, (Rational) 1, &theGlobalVariables);
  minusOneRF.MakeNVarConst(numVars, (Rational) -1, &theGlobalVariables);
  zeroRF.MakeNVarConst(numVars, (Rational) 0, &theGlobalVariables);

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
    if (current.expressionType==RationalFunction::typeRationalFunction)
    { tempP.operator=(current.Denominator.GetElement());
      for (int j=0; j<buffer.size; j++)
        buffer.TheObjects[j].operator*=(tempP);
    }
  }
  output.SetSize(input.size);
  for (int i=0; i<buffer.size; i++)
  { buffer.TheObjects[i].GetNumerator(tempP);
    output.TheObjects[i]=tempP;
  }
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
  theVP.PartitioningRoots.SetSize(numGenerators);
  List<ElementUniversalEnveloping> generatorsBeingActedOn;
  generatorsBeingActedOn.SetSize(numGenerators);
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
  oneRF.MakeNVarConst(numVars, (Rational) 1, &theGlobalVariables);
  minusOneRF.MakeNVarConst(numVars, (Rational) -1, &theGlobalVariables);
  zeroRF.MakeNVarConst(numVars, (Rational) 0, &theGlobalVariables);

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

std::string EigenVectorComputation::ComputeEigenVectorsOfWeight
  (HomomorphismSemisimpleLieAlgebra& inputHmm, SemisimpleLieAlgebraOrdered& theOwner, List<ElementUniversalEnveloping>& output, root& theWeight, GlobalVariables& theGlobalVariables)
{ output.size=0;
  roots PosRootsEmbeddings, PosRootsProjections;
  std::stringstream out;
  int theDimension=inputHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int theDomainDimension= inputHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  int numPosRootsDomain=inputHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numPosRootsRange=inputHmm.theRange.theWeyl.RootsOfBorel.size;
  PosRootsEmbeddings.SetSize(numPosRootsDomain);
  for (int i=0; i<numPosRootsDomain; i++)
  { PosRootsEmbeddings.TheObjects[i].MakeZero(theDimension);
    for (int j=0; j<theDomainDimension; j++)
      PosRootsEmbeddings.TheObjects[i]+=inputHmm.imagesAllChevalleyGenerators.TheObjects[numPosRootsDomain+j].Hcomponent*inputHmm.theDomain.theWeyl.RootsOfBorel.TheObjects[i].TheObjects[j];
  }
  out << "Embeddings of roots:" << PosRootsEmbeddings.ElementToString(false, true, true);
  PosRootsProjections.SetSize(numPosRootsRange);
  for (int i=0; i<numPosRootsRange; i++)
    inputHmm.ProjectOntoSmallCartan(inputHmm.theRange.theWeyl.RootsOfBorel.TheObjects[i], PosRootsProjections.TheObjects[i], theGlobalVariables);
  out << "<br>Projections of roots in simple root coordinates with respect to the subalgebra: " << PosRootsProjections.ElementToString(false, true, true);
  VectorPartition theVP;
  theVP.PartitioningRoots=PosRootsProjections;
  theVP.theRoot=theWeight;
  theVP.ComputeAllPartitions();
  out << "the partitions: <br>" << theVP.ElementToString(true);
  output.SetSize(theVP.thePartitions.size);
  MonomialUniversalEnveloping currentMon;
  ElementUniversalEnveloping tempElt;
  for (int i=0; i<output.size; i++)
  { currentMon.Nullify(theDomainDimension, inputHmm.theRange);
    Rational tempRat=1;
    currentMon.Coefficient.ComputeDebugString();
    currentMon.Coefficient.MakeNVarConst((int)theDimension, tempRat);
    for (int j=theVP.thePartitions.TheObjects[i].size-1; j>=0; j--)
      currentMon.MultiplyByGeneratorPowerOnTheRight(inputHmm.theRange.RootToIndexInUE(-inputHmm.theRange.theWeyl.RootsOfBorel.TheObjects[j]), theVP.thePartitions.TheObjects[i].TheObjects[j]);
    out << currentMon.ElementToString(false) << "<br>" ;
    tempElt.Nullify(inputHmm.theRange);
    tempElt.AddObjectOnTopHash(currentMon);
    output.TheObjects[i]=tempElt;
  }
  List<List<ElementUniversalEnveloping> > targets;
  List<List<ElementUniversalEnveloping> > targetsNoMod;
  targets.SetSize(theDomainDimension);
  targetsNoMod.SetSize(theDomainDimension);
  ElementUniversalEnveloping theSimpleGenerator;
  std::string beginMath = "<DIV class=\"math\" scale=\"50\">";
  std::string endMath = "</DIV>";
  List<List<PolynomialRationalCoeff> > theSystem;
  theSystem.size=0;
  theSystem.SetSize(output.size);
  ElementUniversalEnveloping basisMonomialBuffer;
  for (int i=0; i<targets.size; i++)
  { List<ElementUniversalEnveloping>& currentTargets= targets.TheObjects[i];
    List<ElementUniversalEnveloping>& currentTargetsNoMod= targetsNoMod.TheObjects[i];
    theSimpleGenerator.AssignElementLieAlgebra(inputHmm.imagesSimpleChevalleyGenerators.TheObjects[i], theDimension, inputHmm.theRange);
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
  RationalFunction ZeroRF, unitRF, MinusUnitPoly;
  ZeroRF.MakeNVarConst(theDimension, (Rational) 0, &theGlobalVariables);
  unitRF.MakeNVarConst(theDimension, (Rational) 1, &theGlobalVariables);
  PolynomialRationalCoeff unitPoly, zeroPolynomial;
  unitPoly.MakeNVarConst(theDimension, (Rational) 1);
  zeroPolynomial.Nullify(theDimension);
  MinusUnitPoly.MakeNVarConst(theDimension, (Rational) -1, &theGlobalVariables);
  List<List<RationalFunction> > theAnswer;
  matSystem.FindZeroEigenSpacE(theAnswer, unitRF, MinusUnitPoly, ZeroRF, theGlobalVariables);
  out << "The found solutions: <br>";
  rootRationalFunction tempRatRoot;
  std::string tempS;
  ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> tempEltOrdered, AccumOrdered;
  PolynomialRationalCoeff tempP;
  PolynomialsRationalCoeff theSub;
  PolynomialOutputFormat polyformatLocal;
  polyformatLocal.MakeAlphabetyi();
  this->PrepareCartanSub(theOwner, theSub, theGlobalVariables);
  for (int i=0; i<theAnswer.size; i++)
  { tempRatRoot.CopyFromBase(theAnswer.TheObjects[i]);
    tempRatRoot.ScaleToPolynomial();
    AccumOrdered.Nullify(theOwner);
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
        tempEltOrdered.AssignElementUniversalEnveloping(output.TheObjects[j], theOwner, unitPoly, zeroPolynomial, &theGlobalVariables);
        tempRatRoot.TheObjects[j].GetNumerator(tempP);
        tempP.Substitution(theSub, theDimension, (Rational) 1);
        tempEltOrdered.operator*=(tempP);
      }
      AccumOrdered.operator+=(tempEltOrdered);
    }
    out << endMath << "<br>";
    out << "Corresponding ordered expression: " << "<div class=\"math\">\\begin{eqnarray*}&&" << AccumOrdered.ElementToString(true, polyformatLocal) << "\\end{eqnarray*}</div><br>";
  }
  return out.str();
}

void EigenVectorComputation::PrepareCartanSub
(SemisimpleLieAlgebraOrdered& theOwner, PolynomialsRationalCoeff& outputSub, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational MatrixCartanSub;
  int theDimension=theOwner.theOwner.GetRank();
  MatrixCartanSub.init(theDimension, theDimension);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      MatrixCartanSub.elements[i][j]=theOwner.theOrder.TheObjects[i+theOwner.theOwner.GetNumPosRoots()].Hcomponent.TheObjects[j];
  MatrixCartanSub.Invert(theGlobalVariables);
  root tempRoot; tempRoot.MakeZero(theDimension);
  outputSub.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { MatrixCartanSub.RowToRoot(i, tempRoot);
    tempRoot.ComputeDebugString();
    outputSub.TheObjects[i].MakeLinPolyFromRoot(tempRoot);
  }
}

std::string EigenVectorComputation::ComputeEigenVectorsOfWeightConventionOrdered
  (HomomorphismSemisimpleLieAlgebra& inputHmm, SemisimpleLieAlgebraOrdered& theOwner, List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> >& output, root& theWeight, GlobalVariables& theGlobalVariables)
{ output.size=0;
  roots PosRootsProjections;
  std::stringstream out;
  int theDimension=inputHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  int theDomainDimension= inputHmm.theDomain.theWeyl.CartanSymmetric.NumRows;
  int numPosRootsDomain=inputHmm.theDomain.theWeyl.RootsOfBorel.size;
  int numPosRootsRange=inputHmm.theRange.theWeyl.RootsOfBorel.size;
  PolynomialOutputFormat polyFormatLocal;
  PosRootsProjections.SetSize(numPosRootsRange);
  Rational tempRat;
  ElementSimpleLieAlgebra tempElt1;
  MatrixLargeRational invertedSmallCartan=inputHmm.theDomain.theWeyl.CartanSymmetric;
  invertedSmallCartan.Invert(theGlobalVariables);
  for (int i=0; i<numPosRootsRange; i++)
  { PosRootsProjections.TheObjects[i].MakeZero(theDomainDimension);
    ElementSimpleLieAlgebra& currentElt= theOwner.theOrder.TheObjects[i];
    for (int j=0; j<theDomainDimension; j++)
    { inputHmm.theRange.LieBracket(inputHmm.imagesAllChevalleyGenerators.TheObjects[numPosRootsDomain+j], currentElt, tempElt1);
      if (currentElt.IsProportionalTo(tempElt1, tempRat))
        PosRootsProjections.TheObjects[i].TheObjects[j]=-tempRat;
      else
        return "error: the given basis does not consist of eigen spaces with respect to the subalgebra.";
    }
    invertedSmallCartan.ActOnAroot(PosRootsProjections.TheObjects[i]);
  }
  out << "<br>Projections of roots in simple root coordinates with respect to the subalgebra: " << PosRootsProjections.ElementToString(false, true, true);
  //return out.str();
  VectorPartition theVP;
  theVP.PartitioningRoots=PosRootsProjections;
  theVP.theRoot=theWeight;
  theVP.ComputeAllPartitions();
  out << "the partitions: <br>" << theVP.ElementToString(true);
  output.SetSize(theVP.thePartitions.size);
  MonomialUniversalEnvelopingOrdered<PolynomialRationalCoeff> currentMon;
  ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> tempElt;
  for (int i=0; i<output.size; i++)
  { currentMon.Nullify(theDomainDimension, theOwner, &theGlobalVariables);
    Rational tempRat=1;
    currentMon.Coefficient.ComputeDebugString();
    currentMon.Coefficient.MakeNVarConst(theDimension, tempRat);
    for (int j=theVP.thePartitions.TheObjects[i].size-1; j>=0; j--)
      currentMon.MultiplyByGeneratorPowerOnTheRight(j, theVP.thePartitions.TheObjects[i].TheObjects[j]);
    out << currentMon.ElementToString(false, true, polyFormatLocal) << "<br>" ;
    tempElt.Nullify(theOwner);
    tempElt.AddObjectOnTopHash(currentMon);
    output.TheObjects[i]=tempElt;
  }
  List<List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> > > targets;
  List<List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> > > targetsNoMod;
  targets.SetSize(theDomainDimension);
  targetsNoMod.SetSize(theDomainDimension);
  ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> theSimpleGenerator;
  std::string beginMath = "<DIV class=\"math\" scale=\"50\">";
  std::string endMath = "</DIV>";
  List<List<PolynomialRationalCoeff> > theSystem;
  theSystem.size=0;
  theSystem.SetSize(output.size);
  ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> basisMonomialBuffer;
  PolynomialsRationalCoeff theSub;
  PolynomialRationalCoeff polyOne, polyZero;
  polyOne.MakeNVarConst(theDimension, (Rational)1);
  RationalFunction RFOne;
  RFOne=polyOne;
  polyZero.Nullify(theDimension);
  this->PrepareCartanSub(theOwner, theSub, theGlobalVariables);
  for (int i=0; i<targets.size; i++)
  { List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> >& currentTargets= targets.TheObjects[i];
    List<ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> >& currentTargetsNoMod= targetsNoMod.TheObjects[i];
    theSimpleGenerator.AssignElementLieAlgebra(inputHmm.imagesSimpleChevalleyGenerators.TheObjects[i], polyOne, polyZero, theOwner);
    theSimpleGenerator.ComputeDebugString();
    out << "Generator number " << i+1 << ": " << beginMath;
    for (int j=0; j<output.size; j++)
    { theSimpleGenerator.LieBracketOnTheRight(output.TheObjects[j], tempElt);
      tempElt.Simplify(&theGlobalVariables);
      currentTargetsNoMod.AddObjectOnTop(tempElt);
      tempElt.ModOutVermaRelations(false, theSub, &theGlobalVariables, polyOne);
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
    ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff>::GetCoordinateFormOfSpanOfElements(theDimension, currentTargets, tempList, basisMonomialBuffer, theGlobalVariables);
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
  RationalFunction ZeroRF, UnitPoly, MinusUnitPoly;
  ZeroRF.MakeNVarConst(theDimension, (Rational) 0, &theGlobalVariables);
  UnitPoly.MakeNVarConst(theDimension, (Rational) 1, &theGlobalVariables);
  MinusUnitPoly.MakeNVarConst(theDimension, (Rational) -1, &theGlobalVariables);
  List<List<RationalFunction> > theAnswer;
  //return out.str();
  matSystem.FindZeroEigenSpacE(theAnswer, UnitPoly, MinusUnitPoly, ZeroRF, theGlobalVariables);
  if (theAnswer.size>0)
    out << "The found solutions: <br>";
  else
    out << "No solutions<br>";
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
  ElementUniversalEnvelopingOrdered<PolynomialRationalCoeff> tempOElt;
  for (int i=0; i<theOwner.theOrder.size; i++)
  { tempOElt.AssignElementLieAlgebra(theOwner.theOrder.TheObjects[i], polyOne, polyZero, theOwner);
    out << "<br>" << tempOElt.ElementToString(true, polyFormatLocal);
    out << " = " << theOwner.theOrder.TheObjects[i].ElementToStringNegativeRootSpacesFirst(true, false, inputHmm.theRange);
  }
  return out.str();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::Simplify(GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  //this->ComputeDebugString();
  output.Nullify(*this->owner);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Simplify(buffer, theContext);
    output+=buffer;
    //output.ComputeDebugString();
  }
  *this=output;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::Nullify(SemisimpleLieAlgebraOrdered& theOwner)
{ this->ClearTheObjects();
  this->owner=&theOwner;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::Simplify(ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext)
{ output.Nullify(*this->owner);
  output.AddObjectOnTopHash(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theContext);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexSwapWithLastHash(i);
      i--;
    }
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator+=(const ElementUniversalEnvelopingOrdered<CoefficientType>& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  for (int i=0; i<other.size; i++)
    this->AddMonomialNoCleanUpZeroCoeff(other.TheObjects[i]);
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator-=(const ElementUniversalEnvelopingOrdered<CoefficientType>& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  for (int i=0; i<other.size; i++)
  { tempMon=other.TheObjects[i];
    tempMon.Coefficient.TimesConstant(-1);
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator*=(const CoefficientType& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.operator*=(other);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator/=(const CoefficientType& other)
{ for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.operator/=(other);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator*=(const Rational& other)
{ if (other.IsEqualToZero())
  { this->Nullify(*this->owner);
    return;
  }
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Coefficient.TimesConstant(other);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other)
{ assert(this!=&other);
  this->generatorsIndices.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Powers.MakeActualSizeAtLeastExpandOnTop(other.generatorsIndices.size+this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(other.Coefficient);
  if (other.generatorsIndices.size==0)
    return;
  int firstIndex=other.generatorsIndices.TheObjects[0];
  int i=0;
//  std::string tempS;
//  tempS=other.ElementToString();
  if (this->generatorsIndices.size>0)
    if (firstIndex==(*this->generatorsIndices.LastObject()))
    { //this->ComputeDebugString();
      *this->Powers.LastObject()+=other.Powers.TheObjects[0];
      i=1;
    }
  for (; i<other.generatorsIndices.size; i++)
  { this->Powers.AddObjectOnTop(other.Powers.TheObjects[i]);
    this->generatorsIndices.AddObjectOnTop(other.generatorsIndices.TheObjects[i]);
  }
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::IsProportionalTo
 (const ElementUniversalEnvelopingOrdered<CoefficientType>& other, CoefficientType& outputTimesMeEqualsOther,
  const CoefficientType& theRingZero)const
{ if (this->IsEqualToZero())
  { if (other.IsEqualToZero())
      return true;
    return false;
  }
  if (other.IsEqualToZero())
  { outputTimesMeEqualsOther=theRingZero;
    return true;
  }
  if (other.size!=this->size)
    return false;
  MonomialUniversalEnvelopingOrdered<CoefficientType>& theMon= this->TheObjects[0];
  int theIndex=other.IndexOfObjectHash(theMon);
  if (theIndex==-1)
    return false;
  MonomialUniversalEnvelopingOrdered<CoefficientType>& otherMon= other.TheObjects[theIndex];
  theMon.ComputeDebugString();
  otherMon.ComputeDebugString();
  outputTimesMeEqualsOther=otherMon.Coefficient;
  outputTimesMeEqualsOther/=theMon.Coefficient;
  ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;

  tempElt=*this;
  tempElt*=outputTimesMeEqualsOther;
  tempElt-=other;
  return tempElt.IsEqualToZero();
}

template<class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::operator*=
(const MonomialUniversalEnvelopingOrdered& other)
{ assert(this!=&other);
  this->Coefficient*=other.Coefficient;
  for (int i=0; i<other.generatorsIndices.size; i++)
    this->MultiplyByGeneratorPowerOnTheRight(other.generatorsIndices.TheObjects[i], other.Powers.TheObjects[i]);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::operator*=(const ElementUniversalEnvelopingOrdered<CoefficientType>& other)
{ ElementUniversalEnvelopingOrdered output;
  output.Nullify(*this->owner);
  output.MakeActualSizeAtLeastExpandOnTop(this->size*other.size);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  for (int i=0; i<this->size; i++)
    for(int j=0; j<other.size; j++)
    { tempMon=this->TheObjects[i];
      tempMon*=other.TheObjects[j];
      output.AddMonomial(tempMon);
    }
  *this=output;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnvelopingOrdered<CoefficientType>& input)
{ int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
    this->TheObjects[theIndex].Coefficient+=input.Coefficient;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::SimplifyAccumulateInOutputNoOutputInit
(ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext)
{ int IndexlowestNonSimplified=0;
  ElementUniversalEnvelopingOrdered<CoefficientType> buffer2;
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified<output.size)
  { bool reductionOccurred=false;
    if (output.TheObjects[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred=true;
    else
      for (int i=0; i<output.TheObjects[IndexlowestNonSimplified].generatorsIndices.size-1; i++)
        if (output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i]>output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1])
        { if (output.TheObjects[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute(i, tempMon, theContext))
          { output.AddMonomialNoCleanUpZeroCoeff(tempMon);
            tempMon.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft(i, buffer2, theContext);
            for (int j=0; j<buffer2.size; j++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2.TheObjects[j]);
            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed(output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i], output.TheObjects[IndexlowestNonSimplified].Powers.TheObjects[i+1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices.TheObjects[i+1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight(i, buffer2, theContext);
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

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex, MonomialUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext)
{ if (theLeftIndex>=this->generatorsIndices.size-1)
    return false;
  int theLeftGeneratorIndex=this->generatorsIndices.TheObjects[theLeftIndex];
  int theRightGeneratorIndex=this->generatorsIndices.TheObjects[theLeftIndex+1];
  ElementSimpleLieAlgebra tempElt;
  this->owner->theOwner.LieBracket(this->owner->theOrder.TheObjects[theLeftGeneratorIndex], this->owner->theOrder.TheObjects[theRightGeneratorIndex], tempElt);
  if (tempElt.IsEqualToZero())
  { output.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size);
    output.Nullify(this->Coefficient.NumVars, *this->owner, theContext);
    output.Coefficient=this->Coefficient;
    //output.ComputeDebugString();
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

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::CommutingRightIndexAroundLeftIndexAllowed(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::CommuteConsecutiveIndicesRightIndexAroundLeft(int theIndeX, ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.Nullify(this->Coefficient.NumVars, *this->owner, theContext);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndeX= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndeX=this->generatorsIndices.TheObjects[theIndeX];
  CoefficientType theRightPoweR, theLeftPoweR;
  theRightPoweR= this->Powers.TheObjects[theIndeX+1];
  theLeftPoweR= this->Powers.TheObjects[theIndeX];
  theRightPoweR-=1;
  int powerDroP=0;
//  if (this->flagAnErrorHasOccurredTimeToPanic)
//  if (this->ElementToString()=="2f_{5}f_{-5}f_{-4}" || this->ElementToString()=="2f_{11}f_{-4}")
//  { std::cout << "here we are!";
//    this->flagAnErrorHasOccurredTimeToPanic=true;
//  }

  CoefficientType acquiredCoefficienT, polyOne;
  acquiredCoefficienT=this->Coefficient;
  tempMon.Coefficient=this->Coefficient;
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  MonomialUniversalEnvelopingOrdered startMon;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResulT, tempElT, theLeftElt;
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndeX, adResulT);
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndeX, theLeftElt);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1, theContext);
  root theCoeffs;
  do
  { if(this->flagAnErrorHasOccurredTimeToPanic)
    { acquiredCoefficienT.ComputeDebugString();
      theRightPoweR.ComputeDebugString();
      theLeftPoweR.ComputeDebugString();
      adResulT.ComputeDebugString( false, false);
      tempMon.ComputeDebugString();
    }
    this->owner->GetLinearCombinationFrom(adResulT, theCoeffs);
    for (int i=0; i<theCoeffs.size; i++)
      if (theCoeffs.TheObjects[i]!=0)
      { int theNewGeneratorIndex=i;
        tempMon=startMon;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient=acquiredCoefficienT;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient.TimesConstant(theCoeffs.TheObjects[i]);
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, polyOne);
        tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
        for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        output.AddObjectOnTopHash(tempMon);
      }
    acquiredCoefficienT.MultiplyBy(theLeftPoweR);
    theLeftPoweR-=1;
    this->owner->theOwner.LieBracket(theLeftElt, adResulT, tempElT);
    adResulT=tempElT;
    powerDroP++;
    acquiredCoefficienT/=powerDroP;
    if (this->flagAnErrorHasOccurredTimeToPanic)
      adResulT.ComputeDebugString(false, false);
  }while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::CommuteConsecutiveIndicesLeftIndexAroundRight(int theIndeX, ElementUniversalEnvelopingOrdered<CoefficientType>& output, GlobalVariables* theContext)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.Nullify(this->Coefficient.NumVars, *this->owner, theContext);
  tempMon.Powers.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.MakeActualSizeAtLeastExpandOnTop(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndex= this->generatorsIndices.TheObjects[theIndeX+1];
  int leftGeneratorIndex=this->generatorsIndices.TheObjects[theIndeX];
  CoefficientType theRightPower, theLeftPower;
  theRightPower= this->Powers.TheObjects[theIndeX+1];
  theLeftPower= this->Powers.TheObjects[theIndeX];
  theLeftPower-=1;
  int powerDrop=0;
 /* if (this->ElementToString()=="2f_{5}f_{-5}f_{-4}" || this->ElementToString()=="2f_{11}f_{-4}")
  { std::cout << "here we are!";
    this->flagAnErrorHasOccurredTimeToPanic=true;
  }*/

  CoefficientType acquiredCoefficient, polyOne;
  acquiredCoefficient.Assign(this->Coefficient);
  tempMon.Coefficient=this->Coefficient;
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theIndeX], theLeftPower);
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon=tempMon;
  ElementSimpleLieAlgebra adResult, tempElt, tempRightElt;
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndex, adResult);
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndex, tempRightElt);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  polyOne.MakeNVarConst(this->Coefficient.NumVars, (Rational) 1, theContext);
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
    acquiredCoefficient/=powerDrop;
    //adResult.ComputeDebugString(*this->owner, false, false);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::CommutingLeftIndexAroundRightIndexAllowed(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex)
{ int tempInt;
  if (theLeftPower.IsSmallInteger(tempInt))
  { if(theRightPower.IsSmallInteger(tempInt))
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

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::Nullify(int numVars, SemisimpleLieAlgebraOrdered& theOwner, GlobalVariables* theContext)
{ this->Coefficient.Nullify((int)numVars, theContext);
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::Nullify(const CoefficientType& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Coefficient=theRingZero;
  this->owner=&theOwner;
  this->generatorsIndices.size=0;
  this->Powers.size=0;
}

template <class CoefficientType>
int MonomialUniversalEnvelopingOrdered<CoefficientType>::HashFunction() const
{ int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result=0;
  for (int i=0; i<top; i++)
    result+=SomeRandomPrimes[i]*this->generatorsIndices.TheObjects[i];
  return result;
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower)
{ if (thePower==0)
    return;
  PolynomialRationalCoeff tempP;
  tempP.MakeNVarConst(this->Coefficient.NumVars, thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const CoefficientType& thePower)
{ if (thePower.IsEqualToZero())
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { (*this->Powers.LastObject())+=thePower;
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
  ElementSimpleLieAlgebra tempElt;
  Vector<Rational> coordsInCurrentBasis;
  ElementSimpleLieAlgebra currentElt;
  for (int i=0; i<owner.GetNumGenerators(); i++)
  { currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(i, this->theOwner);
    currentElt.GetCoordsInBasis(this->theOrder, coordsInCurrentBasis, theGlobalVariables);
    for (int j=0; j<coordsInCurrentBasis.size; j++)
      this->ChevalleyGeneratorsInCurrentCoords.elements[j][i]=coordsInCurrentBasis.TheObjects[j];
//    std::cout << "<br> " << currentElt.ElementToString() << " in new coords becomes: " << coordsInCurrentBasis.ElementToString();
  }
//  std::cout << this->ChevalleyGeneratorsInCurrentCoords.ElementToString(true, false) << "<br><br>";

 // this->ChevalleyGeneratorsInCurrentCoords.Invert(theGlobalVariables);
  //std::cout << this->ChevalleyGeneratorsInCurrentCoords.ElementToString(true, false);
}

void SemisimpleLieAlgebraOrdered::initDefaultOrder
  (SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables)
{ List<ElementSimpleLieAlgebra> defaultOrder;
  defaultOrder.SetSize(owner.GetNumGenerators());
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

template <class CoefficientType>
std::string MonomialUniversalEnvelopingOrdered<CoefficientType>::ElementToString(bool useLatex, bool useGeneratorLetters, const PolynomialOutputFormat& PolyFormatLocal)const
{ if (this->owner==0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  if (this->generatorsIndices.size>0)
  { tempS= MathRoutines::ElementToStringBrackets(this->Coefficient);
    if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
  } else
    tempS= this->Coefficient.ElementToString(PolyFormatLocal);
  out <<tempS;
  for (int i=0; i<this->generatorsIndices.size; i++)
  { CoefficientType& thePower=this->Powers.TheObjects[i];
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
      out << ")";
    tempS=thePower.ElementToString(PolyFormatLocal);
    if (tempS!="1")
    { out << "^";
     // if (useLatex)
      out << "{";
      out << tempS;
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::ElementToString
(std::string& output, bool useLatex, const PolynomialOutputFormat& PolyFormatLocal)const
{ std::stringstream out;
  std::string tempS;
  if (this->size==0)
    out << "0";
  int IndexCharAtLastLineBreak=0;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnvelopingOrdered<CoefficientType>& current=this->TheObjects[i];
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

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::AssignElementUniversalEnveloping
  (ElementUniversalEnveloping& input, SemisimpleLieAlgebraOrdered& owner, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  this->Nullify(owner);
  for (int i=0; i<input.size; i++)
  { if(!tempElt.AssignMonomialUniversalEnveloping(input.TheObjects[i], owner, theRingUnit, theRingZero, theContext))
      return false;
    this->operator+=(tempElt);
  }
  this->Simplify(theContext);
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::AssignMonomialUniversalEnveloping
  (MonomialUniversalEnveloping& input, SemisimpleLieAlgebraOrdered& owner, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered theMon;
  ElementSimpleLieAlgebra tempElt;
  CoefficientType theCoeff;
  theCoeff=theRingUnit;
  theCoeff=input.Coefficient;
  this->MakeConst(theCoeff, owner);
  //std::cout << "<br>after initialization with constant I am " << this->ElementToString();
  for (int i=0; i<input.generatorsIndices.size; i++)
  { int thePower;
    bool isASmallInt=input.Powers.TheObjects[i].IsSmallInteger(thePower);
    if (isASmallInt)
    { tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE(input.generatorsIndices.TheObjects[i], owner.theOwner);
      theMon.AssignElementLieAlgebra(tempElt, theRingUnit, theRingZero, owner);
      //std::cout << "<br>raising " << theMon.ElementToString() << " to power " << thePower;
      theMon.RaiseToPower(thePower, theRingUnit);
      //std::cout << " to obtain " << theMon.ElementToString();
    }
    else
      return false;
    this->MultiplyBy(theMon);

  }
  //this->Simplify(theContext);
  return true;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::AssignElementLieAlgebra
(const ElementSimpleLieAlgebra& input, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Nullify(theOwner);
  root ElementRootForm;
  input.ElementToVectorNegativeRootSpacesFirst(ElementRootForm);
  theOwner.ChevalleyGeneratorsInCurrentCoords.ActOnAroot(ElementRootForm);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  tempMon.Nullify(theRingZero, theOwner);
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  tempMon.Powers.TheObjects[0]=theRingUnit;
  for (int theIndex=0; theIndex<ElementRootForm.size; theIndex++)
    if ( ElementRootForm.TheObjects[theIndex]!=0)
    { tempMon.Coefficient=theRingUnit;
      tempMon.Coefficient*=ElementRootForm.TheObjects[theIndex];
      tempMon.generatorsIndices.TheObjects[0]=theIndex;
      this->AddObjectOnTopHash(tempMon);
    }
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::RaiseToPower(int thePower, const CoefficientType& theRingUnit)
{ if (this->size==0)
    return;
  ElementUniversalEnvelopingOrdered<CoefficientType> buffer;
  buffer.operator=(*this);
  this->MakeConst(theRingUnit, *this->owner);
  //std::cout << "<br>raising " <<buffer.ElementToString() << " to power " << thePower;
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
  //std::cout << "<br> and the result is " << this->ElementToString();
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output)
{ ElementUniversalEnvelopingOrdered tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

template<class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::AddMonomial(const MonomialUniversalEnvelopingOrdered<CoefficientType>& input)
{ if (input.IsEqualToZero())
    return;
  int theIndex= this->IndexOfObjectHash(input);
  if (theIndex==-1)
    this->AddObjectOnTopHash(input);
  else
  { this->TheObjects[theIndex].Coefficient+=input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->PopIndexSwapWithLastHash(theIndex);
  }
}

bool ParserNode::ConvertToNextType
(int GoalType, bool& ErrorHasOccured, GlobalVariables& theGlobalVariables)
{ ErrorHasOccured=false;
  switch(this->ExpressionType)
  { case ParserNode::typePoly:
      this->polyValue.GetElement().SetNumVariablesSubDeletedVarsByOne(this->owner->NumVariables);
      break;
    case ParserNode::typeUEelement:
      this->UEElement.GetElement().SetNumVariables(this->owner->NumVariables);
      break;
    case ParserNode::typeUEElementOrdered:
      this->UEElementOrdered.GetElement().SetNumVariables(this->owner->NumVariables);
      break;
    case ParserNode::typeRationalFunction:
      this->ratFunction.GetElement().SetNumVariablesSubDeletedVarsByOne(this->owner->NumVariables);
      break;
    case ParserNode::typeWeylAlgebraElement:
      this->WeylAlgebraElement.GetElement().SetNumVariablesPreserveExistingOnes(this->owner->NumVariables);
      break;
  }
  if (GoalType==this->ExpressionType)
    return false;
  if (this->ExpressionType==this->typeIntegerOrIndex)
  { this->rationalValue= this->intValue;
    this->ExpressionType=this->typeRational;
    return true;
  }
  if (this->ExpressionType==this->typeRational)
  { this->polyValue.GetElement().MakeNVarConst((int)this->owner->NumVariables, this->rationalValue);
    this->ExpressionType=this->typePoly;
    return true;
  }
  if (this->ExpressionType==this->typePoly)
  { int typeToConvertTo=this->typeRationalFunction;
    if (GoalType==this->typeUEElementOrdered || GoalType==this->typeUEelement)
      typeToConvertTo=this->typeUEElementOrdered;
    if (GoalType==this->typeWeylAlgebraElement)
      typeToConvertTo=this->typeWeylAlgebraElement;
    switch (typeToConvertTo)
    { case ParserNode::typeUEelement:
        this->UEElement.GetElement().MakeConst(this->polyValue.GetElement(), *this->ContextLieAlgebra);
        this->ExpressionType=this->typeUEelement;
        return true;
      case ParserNode::typeWeylAlgebraElement:
        this->WeylAlgebraElement.GetElement().AssignPolynomial(this->polyValue.GetElement());
        this->ExpressionType=this->typeWeylAlgebraElement;
        return true;
      case ParserNode::typeRationalFunction:
        this->ratFunction.GetElement()=this->polyValue.GetElement();
        this->ExpressionType=this->typeRationalFunction;
        return true;
      case ParserNode::typeUEElementOrdered:
        this->UEElementOrdered.GetElement().MakeConst(this->polyValue.GetElement(), this->owner->testAlgebra);
        this->ExpressionType=this->typeUEElementOrdered;
        return true;
    }
  }
  if (this->ExpressionType==this->typeUEelement)
  { PolynomialRationalCoeff unitPoly, zeroPoly;
    unitPoly.MakeNVarConst(this->UEElement.GetElement().GetNumVariables(), (Rational) 1);
    zeroPoly.Nullify(unitPoly.NumVars);
    if (this->UEElementOrdered.GetElement().AssignElementUniversalEnveloping(this->UEElement.GetElement(), this->owner->testAlgebra, unitPoly, zeroPoly, &theGlobalVariables))
    { this->ExpressionType=this->typeUEElementOrdered;
      return true;
    } else
    { ErrorHasOccured=true;
      return false;
    }
  }
  if (this->ExpressionType==this->typeUEElementOrdered && GoalType==this->typeUEelement)
  { if (!this->UEElementOrdered.GetElement().GetElementUniversalEnveloping(this->UEElement.GetElement(), this->owner->theHmm.theRange))
    { ErrorHasOccured=true;
      return false;
    }
    this->ExpressionType=this->typeUEelement;
    return true;
  }
  return false;
}

bool ParserNode::ConvertToType
(int theType, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType==this->typeError)
    return false;
  if (theType==this->typeUndefined)
    return false;
  switch(this->ExpressionType)
  { case ParserNode::typeMapPolY: this->polyBeingMappedTo.GetElement().SetNumVariablesSubDeletedVarsByOne(this->owner->NumVariables);
    case ParserNode::typePoly: this->polyValue.GetElement().SetNumVariablesSubDeletedVarsByOne(this->owner->NumVariables); break;
    case ParserNode::typeRationalFunction: this->ratFunction.GetElement().SetNumVariablesSubDeletedVarsByOne(this->owner->NumVariables); break;
    case ParserNode::typeUEelement: this->UEElement.GetElement().SetNumVariables(this->owner->NumVariables); break;
    case ParserNode::typeUEElementOrdered: this->UEElementOrdered.GetElement().SetNumVariables(this->owner->NumVariables); break;
    case ParserNode::typeMapWeylAlgebra: this->weylEltBeingMappedTo.GetElement().SetNumVariablesPreserveExistingOnes(this->owner->NumVariables);
    case ParserNode::typeWeylAlgebraElement: this->WeylAlgebraElement.GetElement().SetNumVariablesPreserveExistingOnes(this->owner->NumVariables); break;
  }
  bool ConversionError;
  while (this->ConvertToNextType(theType, ConversionError, theGlobalVariables))
    if (ConversionError)
    { this->SetError(this->errorConversionError);
      return false;
    }
  if (this->ExpressionType!=theType)
  { this->SetError(this->errorConversionError);
    return false;
  }
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::GetElementUniversalEnveloping
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

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::GetElementUniversalEnveloping
  (ElementUniversalEnveloping& output, SemisimpleLieAlgebra& owner)
{ ElementUniversalEnveloping Accum;
  ElementUniversalEnveloping tempMon;
  int theIndex;
  Accum.MakeConst(this->Coefficient, owner);
  for (int i=0; i<this->generatorsIndices.size; i++)
    if (this->Powers.TheObjects[i].IsSmallInteger())
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

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::SetNumVariables(int newNumVars)
{ //this->ComputeDebugString();
  if (this->size==0)
    return;
  int currentNumVars=this->TheObjects[0].Coefficient.NumVars;
  if (currentNumVars==newNumVars)
    return;
  ElementUniversalEnvelopingOrdered Accum;
  Accum.Nullify(*this->owner);
  MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon.SetNumVariables(newNumVars);
    Accum.AddMonomial(tempMon);
  }
//  Accum.ComputeDebugString();
  this->operator=(Accum);
 // this->ComputeDebugString();
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::SetNumVariables(int newNumVars)
{ if (this->Coefficient.NumVars==newNumVars)
    return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
}

std::string ParserNode::ElementToStringValueOnlY(bool useHtml, int RecursionDepth, int maxRecursionDepth)
{ std::stringstream LatexOutput;
  PolynomialOutputFormat PolyFormatLocal;
  int i;
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex: LatexOutput << this->intValue; break;
    case ParserNode::typeRational: LatexOutput << this->rationalValue.ElementToString(); break;
    case ParserNode::typePoly: LatexOutput << this->polyValue.GetElement().ElementToString(PolyFormatLocal); break;
    case ParserNode::typeRationalFunction: LatexOutput << this->ratFunction.GetElement().ElementToString(PolyFormatLocal); break;
    case ParserNode::typeUEElementOrdered: LatexOutput << this->UEElementOrdered.GetElement().ElementToString(true, PolyFormatLocal); break;
    case ParserNode::typeUEelement: LatexOutput << this->UEElement.GetElement().ElementToString(); break;
    case ParserNode::typeWeylAlgebraElement: LatexOutput << this->WeylAlgebraElement.GetElement().ElementToString(true); break;
    case ParserNode::typeArray:
      LatexOutput << "(";
      RecursionDepth++;
      if (RecursionDepth<=maxRecursionDepth)
        for (i=0; i<this->array.GetElement().size; i++)
        { LatexOutput << this->owner->TheObjects[this->array.GetElement().TheObjects[i]].ElementToStringValueOnlY(useHtml, RecursionDepth, maxRecursionDepth);
          if (i!=this->array.GetElement().size-1)
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

std::string ParserNode::ElementToStringValueAndType(bool useHtml, int RecursionDepth, int maxRecursionDepth)
{ std::stringstream out;
  PolynomialOutputFormat PolyFormatLocal;
  std::string stringValueOnly= this->ElementToStringValueOnlY(useHtml, RecursionDepth, maxRecursionDepth);
  switch (this->ExpressionType)
  { case ParserNode::typeIntegerOrIndex:  out << " an integer of value: "; break;
    case ParserNode::typeRational: out << " a rational number of value: "; break;
    case ParserNode::typePoly: out << " a polynomial of value: "; break;
    case ParserNode::typeRationalFunction: out << " a rational function of value: "; break;
    case ParserNode::typeUEElementOrdered: out << "an element of U(g) ordered:"; break;
    case ParserNode::typeUEelement: out << " an element of U(g) of value: "; break;
    case ParserNode::typeWeylAlgebraElement: out << " a Weyl algebra element: "; break;
    case ParserNode::typeArray: out << " an array of " << this->array.GetElement().size << " elements. "; break;
    case ParserNode::typeString: out << "<br>A printout of value: "; break;
    case ParserNode::typeError: out << this->ElementToStringErrorCode(useHtml); break;
    default: out << "The programmer(s) have forgotten to enter a type description. "; break;
  }
  if (stringValueOnly!="")
  { if (!useHtml)
      out << stringValueOnly;
    else
    { out << "\n<div id=\"theResult" << this->indexInOwner << "\" class=\"math\" scale=\"50\">\\begin{eqnarray*}&&";
      out << stringValueOnly;
      out << "\\end{eqnarray*}</div>";
      out << "<textarea id=\"theResultLatex" << this->indexInOwner << "\" style=\"display: none\">";
      out << "\\begin{eqnarray*}" << stringValueOnly << "\\end{eqnarray*}";
      out << "</textarea>";
      out << "<br>\n<button id=\"ButtonToggleLatex\" onclick=\"switchMenu('theResult" << this->indexInOwner << "'); switchMenu('theResultLatex" << this->indexInOwner << "');\"\">Show LaTeX/Show eye candy</button>";
    }
  }
  if (this->outputString!="" && this->ExpressionType!=this->typeString)
    out << "<br>In addition, the program generated the following printout. ";
  out << this->outputString;
  if (this->ExpressionType==this->typeArray)
  { out << "<br>Elements of the array follow.";
    RecursionDepth++;
    if (RecursionDepth<=maxRecursionDepth)
      for (int i=0; i<this->array.GetElement().size; i++)
        out << "<br>Element of index " << i+1 << ":" << this->owner->TheObjects[this->array.GetElement().TheObjects[i]].ElementToStringValueAndType(useHtml, RecursionDepth, maxRecursionDepth);
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
  tempP.Nullify((int)NumVars);
  this->degrees.initFillInObject(NumVars, tempP);
  this->Coefficient.MakeNVarConst((int) NumVars, (Rational) 1);
}

void GeneralizedMonomialRational::Nullify
  (int numVars)
{ PolynomialRationalCoeff tempP;
  tempP.Nullify((int)numVars);
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
  this->degrees.SetSize(numVars);
  for (int i=0; i<startingIndexFormalExponent; i++)
    this->degrees.TheObjects[i].Nullify(numVars);
  for (int i=startingIndexFormalExponent; i<this->degrees.size; i++)
    this->degrees.TheObjects[i].MakeMonomialOneLetter((int) numVars, i, 1, (Rational) 1);
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
  if(!this->ConvertChildrenToType(this->typeWeylAlgebraElement, theGlobalVariables))
  { this->SetError(this->errorBadOrNoArgument);
    return;
  }
  this->WeylAlgebraElement.GetElement()=this->owner->TheObjects[this->children.TheObjects[0]].WeylAlgebraElement.GetElementConst();
  this->weylEltBeingMappedTo.GetElement()=this->owner->TheObjects[this->children.TheObjects[1]].WeylAlgebraElement.GetElementConst();
  this->ExpressionType=this->typeMapWeylAlgebra;
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
{ if (this->children.size<2)
  { this->SetError(this->errorProgramming);
    return;
  }
  PolynomialsRationalCoeff theSub;
  int theDimension=this->owner->NumVariables;
  theSub.MakeIdSubstitution((int) theDimension*2, (Rational) 1);
  List<bool> Explored(theDimension*2, false);
  PolynomialRationalCoeff currentLeft, currentRight;
  for (int i=0; i<this->children.size-1; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children.TheObjects[i]];
    currentNode.WeylAlgebraElement.GetElement().SetNumVariablesPreserveExistingOnes(theDimension);
    currentNode.weylEltBeingMappedTo.GetElement().SetNumVariablesPreserveExistingOnes(theDimension);
    currentNode.WeylAlgebraElement.GetElement().GetStandardOrder(currentLeft);
    currentNode.weylEltBeingMappedTo.GetElement().GetStandardOrder(currentRight);
    if (currentLeft.size!=1)
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
    Monomial<Rational>& theMon=currentLeft.TheObjects[0];
    int theLetterIndex;
    if (!theMon.IsOneLetterFirstDegree(theLetterIndex))
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
    if (Explored.TheObjects[theLetterIndex])
    { this->SetError(this->errorBadSubstitution);
      return;
    }
//    std::cout << "<br> currentRight is: " << currentRight.ElementToString();
    Explored.TheObjects[theLetterIndex]=true;
    theSub.TheObjects[theLetterIndex]=currentRight;
//    std::cout << "<br> and currently we have: " << theSub.TheObjects[theLetterIndex].ElementToString();
    theSub.TheObjects[theLetterIndex]/=theMon.Coefficient;
//    std::cout << "<br> and currently we have: " << theSub.TheObjects[theLetterIndex].ElementToString();
    theSub.TheObjects[theLetterIndex].SetNumVariablesSubDeletedVarsByOne(theDimension*2);
//    std::cout << "<br> and currently we have: " << theSub.TheObjects[theLetterIndex].ElementToString();
  }
  ParserNode& lastNode=this->owner->TheObjects[*this->children.LastObject()];
  this->CopyValue(lastNode);
  this->CarryOutSubstitutionInMe(theSub, theGlobalVariables);
}

void ParserNode::CarryOutSubstitutionInMe(PolynomialsRationalCoeff& theSub, GlobalVariables& theGlobalVariables)
{ int theDimension=this->owner->NumVariables;
  Rational ratOne;
  ratOne.MakeOne();
  switch(this->ExpressionType)
  { case ParserNode::typeWeylAlgebraElement:
      this->WeylAlgebraElement.GetElement().SubstitutionTreatPartialsAndVarsAsIndependent(theSub);
      this->ExpressionType=this->typeWeylAlgebraElement;
      return;
    case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typePoly:
 //     std::cout << "<br> ... and the sub is: " << theSub.ElementToString();
      theSub.SetSize(theDimension);
      for (int i=0; i<theDimension; i++)
        theSub.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(theDimension);
   //   std::cout << "<br> ... and the sub is: " << theSub.ElementToString();
      this->polyValue.GetElement().Substitution(theSub, theDimension, ratOne);
      this->ExpressionType=this->typePoly;
      return;
    case ParserNode::typeArray:
      for (int i=0; i<this->array.GetElement().size; i++)
        this->owner->TheObjects[this->array.GetElement().TheObjects[i]].CarryOutSubstitutionInMe(theSub, theGlobalVariables);
      return;
    default:
      this->SetError(this->errorDunnoHowToDoOperation);
      return;
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
  if (other.ExpressionType==this->typeMapPolY)
  { this->polyValue.GetElement().operator=(other.polyValue.GetElementConst());
    this->polyBeingMappedTo.GetElement().operator=(other.polyBeingMappedTo.GetElementConst());
  }
  if (other.ExpressionType==this->typeMapWeylAlgebra)
  { this->WeylAlgebraElement.GetElement().operator=(other.WeylAlgebraElement.GetElementConst());
    this->weylEltBeingMappedTo.GetElement().operator=(other.weylEltBeingMappedTo.GetElementConst());
  }
  if (other.ExpressionType==this->typeArray)
    this->array.GetElement().CopyFromBase(other.array.GetElementConst());
  if(other.ExpressionType==this->typeRationalFunction)
    this->ratFunction.GetElement().operator=(other.ratFunction.GetElementConst());
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
  lastNode.children.SetSize(theDimension);
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
  lastNode.children.SetSize(theDimension+1);
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

void ElementWeylAlgebra::SubstitutionTreatPartialsAndVarsAsIndependent
(PolynomialsRationalCoeff& theSub)
{ assert(theSub.size==this->NumVariables*2);
//  std::cout << "<br>...and the sub is: " << theSub.ElementToString();
  this->StandardOrder.Substitution(theSub, this->NumVariables*2, (Rational) 1);
}

void ElementWeylAlgebra::RaiseToPower(int thePower)
{ ElementWeylAlgebra WeylOne;
  WeylOne.MakeConst(this->NumVariables, (Rational) 1);
  MathRoutines::RaiseToPower(*this, thePower, WeylOne);
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::GetCoordsInBasis
  (List<ElementUniversalEnvelopingOrdered<CoefficientType> >& theBasis, Vector<CoefficientType>& output,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables& theGlobalVariables)const
{ List<ElementUniversalEnvelopingOrdered<CoefficientType> > tempBasis, tempElts;
  tempBasis=theBasis;
  tempBasis.AddObjectOnTop(*this);
  Vectors<CoefficientType> tempCoords;
  this->GetBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero, theGlobalVariables);
  Vector<CoefficientType> tempRoot;
  tempRoot=*tempCoords.LastObject();
  tempCoords.SetSize(theBasis.size);
  return tempRoot.GetCoordsInBasiS(tempCoords, output, theRingUnit, theRingZero);
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::GetCoordinateFormOfSpanOfElements
  (int numVars, List<ElementUniversalEnvelopingOrdered>& theElements, List<rootPoly>& outputCoordinates,
   ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  MonomialUniversalEnveloping tempMon;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddObjectOnTopNoRepetitionOfObjectHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordinates.SetSize(theElements.size);
  PolynomialRationalCoeff ZeroPoly;
  ZeroPoly.Nullify((int)numVars);
  for (int i=0; i<theElements.size; i++)
  { rootPoly& current=outputCoordinates.TheObjects[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnvelopingOrdered& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnvelopingOrdered<CoefficientType>& currentMon=currentElt.TheObjects[j];
      current.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
}

void rootRationalFunction::ScaleToPolynomial()
{ PolynomialRationalCoeff tempP1;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].expressionType==RationalFunction::typeRationalFunction)
    { this->TheObjects[i].GetDenominator(tempP1);
      for (int j=0; j<this->size; j++)
        this->TheObjects[j]*=tempP1;
    }
}

void HomomorphismSemisimpleLieAlgebra::GetWeightsWrtK
  (roots& outputWeights, List<ElementSimpleLieAlgebra>& inputElts, GlobalVariables& theGlobalVariables)
{ outputWeights.SetSize(inputElts.size);
  Rational tempRat;
  ElementSimpleLieAlgebra tempLieElement;
  for (int i=0; i<inputElts.size; i++)
  { root& currentWeight= outputWeights.TheObjects[i];
    currentWeight.MakeZero(this->theDomain.GetRank());
    ElementSimpleLieAlgebra& currentLieElt=inputElts.TheObjects[i];
    for (int j=0; j<this->theDomain.GetRank(); j++)
    { this->theRange.LieBracket(this->imagesAllChevalleyGenerators.TheObjects[j+this->theDomain.GetNumPosRoots()], currentLieElt, tempLieElement);
      if(!currentLieElt.IsProportionalTo(tempLieElement, tempRat)){ assert(false);}
      currentWeight.TheObjects[j]=tempRat;
    }
  }
}

std::string TranslationFunctorGmodKVermaModule::RunTheComputationSl2StringVersion
  (Parser& theParser, GlobalVariables& theGlobalVariables, EigenVectorComputation& owner)
{ std::stringstream out;
  owner.PrepareCartanSub(theParser.testAlgebra, this->HighestWeightSub, theGlobalVariables);
  std::cout << this->HighestWeightSub.ElementToString();

  out << this->initTheCandidateMonomialsSl2StringVersion(theParser, theGlobalVariables);

  int theDomainRank=theParser.theHmm.theDomain.GetRank();
  int numPosRootsDomain=theParser.theHmm.theDomain.GetNumPosRoots();
  List<ElementSimpleLieAlgebra> theSimplePosGenerators, theSimpleNegGenerators;
  theSimplePosGenerators.SetSize(theDomainRank);
  theSimpleNegGenerators.SetSize(theDomainRank);
  for (int i=0; i<theDomainRank; i++)
  { theSimplePosGenerators.TheObjects[i]=theParser.theHmm.imagesAllChevalleyGenerators.TheObjects[i+theDomainRank+numPosRootsDomain];
    theSimpleNegGenerators.TheObjects[i]=theParser.theHmm.imagesAllChevalleyGenerators.TheObjects[-1-i+numPosRootsDomain];
  }
  ElementSimpleLieAlgebra zeroAlgebraElement;
  ElementVermaModuleOrdered<RationalFunction> zeroVermaElement;
  zeroAlgebraElement.Nullify(theParser.theHmm.theRange);
  zeroVermaElement.Nullify(theParser.testAlgebra, this->HighestWeightSub);
  RationalFunction RFOne, RFZero;
  RFOne.MakeNVarConst(theParser.theHmm.theRange.GetRank(), (Rational) 1, &theGlobalVariables);
  RFZero.Nullify(theParser.theHmm.theRange.GetRank(), &theGlobalVariables);
  this->theSpace.initTheSpacesSl2StringVersion(theSimplePosGenerators, theSimpleNegGenerators, theParser.theHmm.theRange, zeroVermaElement, zeroAlgebraElement, RFOne, RFZero, theGlobalVariables);

  //this->theSpace.ExtractHighestWeightVectorsFromVector(this->theSpace.theSeedVector, this->theOutput, theGlobalVariables, this->theSpace.theTargetSpace.theRingUniT);
  return out.str();
}

std::string TranslationFunctorGmodKVermaModule::RunTheComputation
  (Parser& theParser, GlobalVariables& theGlobalVariables, EigenVectorComputation& owner)
{ std::stringstream out;
  owner.PrepareCartanSub(theParser.testAlgebra, this->HighestWeightSub, theGlobalVariables);
  std::cout << this->HighestWeightSub.ElementToString();
//return "";
  out << this->initTheCandidateMonomials(theParser, theGlobalVariables);
  int theDomainRank=theParser.theHmm.theDomain.GetRank();
  int numPosRootsDomain=theParser.theHmm.theDomain.GetNumPosRoots();
  List<ElementSimpleLieAlgebra> theSimpleGenerators;
  theSimpleGenerators.SetSize(theDomainRank);
  for (int i=0; i<theDomainRank; i++)
    theSimpleGenerators.TheObjects[i]=theParser.theHmm.imagesAllChevalleyGenerators.TheObjects[i+theDomainRank+numPosRootsDomain];
  //return out.str();
  ElementSimpleLieAlgebra zeroAlgebraElement;
  ElementVermaModuleOrdered<RationalFunction> zeroVermaElement;
  zeroAlgebraElement.Nullify(theParser.theHmm.theRange);
  zeroVermaElement.Nullify(theParser.testAlgebra, this->HighestWeightSub);
  RationalFunction RFOne, RFZero;
  RFOne.MakeNVarConst(theParser.theHmm.theRange.GetRank(), (Rational) 1, &theGlobalVariables);
  RFZero.Nullify(theParser.theHmm.theRange.GetRank(), &theGlobalVariables);
  //for (int i=this->theModuleWeightsShifted.size-1; i>=0; i--)
  int i=0;
  { TensorProductSpaceAndElements<ElementVermaModuleOrdered<RationalFunction>, ElementSimpleLieAlgebra, RationalFunction>& currentCollection=this->theElements.TheObjects[i];
    currentCollection.FindEigenVectorsWithRespectTo(theSimpleGenerators, this->theLeftComponentsByWeight.TheObjects[i], this->theRightComponentsByWeight.TheObjects[i], theParser.theHmm.theRange, this->theEigenVectors.TheObjects[i], zeroVermaElement, zeroAlgebraElement, RFOne, RFZero, theGlobalVariables);
  }
  return out.str();
}

std::string TranslationFunctorGmodKVermaModule::initTheWeights
    (Parser& theParser, GlobalVariables& theGlobalVariables)
{std::stringstream out;
//  int theDomainRank=theParser.theHmm.theDomain.GetRank();
//  int numPosRootsDomain=theParser.theHmm.theDomain.GetNumPosRoots();
  roots theAlgebraWeights;
  theParser.theHmm.GetWeightsGmodK(this->theModuleWeightsShifted, theGlobalVariables);
  theParser.theHmm.GetWeightsK(theAlgebraWeights, theGlobalVariables);
//  SemisimpleLieAlgebraOrdered& theAlgebra=theParser.testAlgebra;
  this->theElements.SetSize(this->theModuleWeightsShifted.size);
  this->theEigenVectors.SetSize(this->theModuleWeightsShifted.size);
  this->theLeftComponentsByWeight.SetSize(this->theModuleWeightsShifted.size);
  this->theRightComponentsByWeight.SetSize(this->theModuleWeightsShifted.size);

  MatrixLargeRational invertedCartan;
  invertedCartan=theParser.theHmm.theDomain.theWeyl.CartanSymmetric;
  invertedCartan.Invert(theGlobalVariables);
//  out << "<br>" << this->theModuleWeightsShifted.ElementToString() << "<br>";
  invertedCartan.ActOnRoots(this->theModuleWeightsShifted);
  invertedCartan.ActOnRoots(theAlgebraWeights);
//  out << "<br>" << this->theModuleWeightsShifted.ElementToString() << "<br>";
  root tempRoot=*this->theModuleWeightsShifted.LastObject();
  this->theModuleWeightsNonShifted=this->theModuleWeightsShifted;
  for (int i=0; i<this->theModuleWeightsShifted.size; i++)
    this->theModuleWeightsShifted.TheObjects[i]-=tempRoot;
//  out << "<br>" << this->theModuleWeightsShifted.ElementToString() << "<br>";
  this->ElementsOfKofNegativeWeights.size=0;
  this->theVectorPartition.PartitioningRoots.size=0;
  for (int i=0; i<theAlgebraWeights.size; i++)
    if (theAlgebraWeights.TheObjects[i].IsNegativeOrZero()&& !theAlgebraWeights.TheObjects[i].IsEqualToZero())
    { this->theVectorPartition.PartitioningRoots.AddObjectOnTop(-theAlgebraWeights.TheObjects[i]);
      this->ElementsOfKofNegativeWeights.AddObjectOnTop(theParser.theHmm.imagesAllChevalleyGenerators.TheObjects[i]);
    }
  out << "<br>" << this->theVectorPartition.PartitioningRoots.ElementToString() << "<br>";
  return out.str();
}

std::string TranslationFunctorGmodKVermaModule::initTheCandidateMonomials
  (Parser& theParser, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << this->initTheWeights(theParser, theGlobalVariables);
  assert(this->HighestWeightSub.size==3);
  for (int i=this->theModuleWeightsShifted.size-1; i>=0; i--)
  {     //for testing purposes:
    //if (i<theModuleWeightsShifted.size-2)
     // break;

    root& weightTotal=theModuleWeightsShifted.TheObjects[i];
    for (int k=i; k<this->theModuleWeightsShifted.size; k++)
    { root& weightRight=this->theModuleWeightsShifted.TheObjects[k];
      root weightLeft=-weightTotal+weightRight;
      if (weightLeft.IsPositiveOrZero())
      { this->theVectorPartition.theRoot=weightLeft;
//        theVP.ComputeDebugString(false);
//        theVP.PartitioningRoots.ComputeDebugString();
        this->theVectorPartition.ComputeAllPartitions();
//        out << theVP.ElementToString(true) << "<br><br>";
        out << this->GetLeftAndRightFromVP(i, k, this->theVectorPartition, theParser, theGlobalVariables);
        out << "<br><br><br>";
      }
    }
  }

  return out.str();
}

std::string TranslationFunctorGmodKVermaModule::initTheCandidateMonomialsSl2StringVersion
  (Parser& theParser, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << this->initTheWeights(theParser, theGlobalVariables);
  out << this->theVectorPartition.ElementToString(true);
  assert(this->HighestWeightSub.size==3);
  this->theSpace.theTargetSpace.leftSpaceBasis.size=0;
  this->theSpace.theRightLowestWeight=this->theModuleWeightsShifted.TheObjects[0];
  SelectionWithDifferentMaxMultiplicities theSel;
  theSel.initIncomplete(this->theSpace.theRightLowestWeight.size);
  this->theSpace.theTargetSpace.rightWeights.SetSize(this->theModuleWeightsNonShifted.size);
  for (int i=0; i<this->theModuleWeightsNonShifted.size; i++)
    this->theSpace.theTargetSpace.rightWeights.TheObjects[i]=this->theModuleWeightsNonShifted.TheObjects[this->theModuleWeightsShifted.size-1-i];
  for (int i=0; i<this->theSpace.theRightLowestWeight.size; i++)
    theSel.MaxMultiplicities.TheObjects[i]=-this->theSpace.theRightLowestWeight.TheObjects[i].NumShort;
  int numCycles=theSel.getTotalNumSubsets();
  out << "Total num weights: " << numCycles << "<br>";
  this->theSpace.theTargetSpace.rightSpaceBasis.size=0;
  for (int i=0; i<this->theModuleWeightsShifted.size; i++)
  { ElementSimpleLieAlgebra& rightComponent=theParser.theHmm.GmodK.TheObjects[i];
    this->theSpace.theTargetSpace.rightSpaceBasis.AddObjectOnTop(rightComponent);
  }

  root weightTotal;
  for (int i=0; i<numCycles; i++, theSel.IncrementSubset())
  { weightTotal.operator=(theSel);
    weightTotal.MinusRoot();
    this->theVectorPartition.theRoot=-weightTotal;
    this->theVectorPartition.ComputeAllPartitions();
//    this->theVectorPartition.ComputeDebugString(false);
    out << this->GetLeftFromVP( this->theVectorPartition, theParser, theGlobalVariables);
    out << "<br><br><br>";
  }
  return out.str();
}

std::string TranslationFunctorGmodKVermaModule::GetLeftFromVP
  ( VectorPartition& theVP, Parser& theParser, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "<br>total weight: " << theVP.theRoot.ElementToString();
  ElementVermaModuleOrdered<RationalFunction> leftComponent;
  ElementUniversalEnvelopingOrdered<RationalFunction> leftComponentBeforeModdingVermaRels, tempElt;
  RationalFunction RFOne, RFZero;
  PolynomialOutputFormat polyFormatLocal;
  int theNumVars=theParser.theHmm.theRange.GetRank();
  RFOne.MakeNVarConst(theNumVars, (Rational) 1, &theGlobalVariables);
  RFZero.Nullify(theNumVars, &theGlobalVariables);
  out << "<br> the vector partition: " << theVP.ElementToString(true);
  ElementVermaModuleOrdered<RationalFunction> theVermaZero;
  assert(this->HighestWeightSub.size==3);
  theVermaZero.Nullify(theParser.testAlgebra, this->HighestWeightSub);
  for (int i=0; i<theVP.thePartitions.size; i++)
  { leftComponentBeforeModdingVermaRels.MakeConst(RFOne, theParser.testAlgebra);
    for (int j=0; j<theVP.thePartitions.TheObjects[i].size; j++)
    { int currentMultiplicity=theVP.thePartitions.TheObjects[i].TheObjects[j];
      if (currentMultiplicity>0)
      { tempElt.AssignElementLieAlgebra(this->ElementsOfKofNegativeWeights.TheObjects[j], RFOne, RFZero, theParser.testAlgebra);
        tempElt.RaiseToPower(currentMultiplicity, RFOne);
        leftComponentBeforeModdingVermaRels.operator*=(tempElt);
      }
    }
    leftComponent.AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
    (leftComponentBeforeModdingVermaRels, theVermaZero, &theGlobalVariables,
     RFOne
     );
    this->theSpace.theTargetSpace.leftSpaceBasis.AddObjectOnTop(leftComponent);
    this->theSpace.theTargetSpace.leftWeights.AddObjectOnTop(theVP.theRoot);
    //out << "<br>Left element before modding: " << leftComponentBeforeModdingVermaRels.ElementToString() << " and after modding: "<< leftComponent.ElementToString();
    out << "<br>Left element: "<< leftComponent.ElementToString();
  }
  return out.str();
//  currentElement.AssignTensor(leftComponent, rightComponent);
}


std::string TranslationFunctorGmodKVermaModule::GetLeftAndRightFromVP
  (int indexTotalWeight, int indexRightWeight, VectorPartition& theVP, Parser& theParser, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "<br>total weight: " << this->theModuleWeightsShifted.TheObjects[indexTotalWeight].ElementToString();
  out << "<br> left weight: " << (-theVP.theRoot).ElementToString() << "  right weight: " << this->theModuleWeightsShifted.TheObjects[indexRightWeight].ElementToString();
  ElementSimpleLieAlgebra& rightComponent=theParser.theHmm.GmodK.TheObjects[indexRightWeight];
  ElementVermaModuleOrdered<RationalFunction> leftComponent;
  ElementUniversalEnvelopingOrdered<RationalFunction> leftComponentBeforeModdingVermaRels, tempElt;
  RationalFunction RFOne, RFZero;
  PolynomialOutputFormat polyFormatLocal;
  int theNumVars=theParser.theHmm.theRange.GetRank();
  RFOne.MakeNVarConst(theNumVars, (Rational) 1, &theGlobalVariables);
  RFZero.Nullify(theNumVars, &theGlobalVariables);
  //RFOne.operator=(PolyOne);
  out << "<br>right element:" << rightComponent.ElementToStringNegativeRootSpacesFirst(false, false, theParser.theHmm.theRange);
  List<ElementVermaModuleOrdered<RationalFunction> >& currentLeftCollection= this->theLeftComponentsByWeight.TheObjects[indexTotalWeight];
  List<ElementSimpleLieAlgebra>& currentRightCollection = this->theRightComponentsByWeight.TheObjects[indexTotalWeight];
  out << "<br> the vector partition: " << theVP.ElementToString(true);
  ElementVermaModuleOrdered<RationalFunction> theVermaZero;
  assert(this->HighestWeightSub.size==3);
  theVermaZero.Nullify(theParser.testAlgebra, this->HighestWeightSub);

  for (int i=0; i<theVP.thePartitions.size; i++)
  { leftComponentBeforeModdingVermaRels.MakeConst(RFOne, theParser.testAlgebra);
    for (int j=0; j<theVP.thePartitions.TheObjects[i].size; j++)
    { int currentMultiplicity=theVP.thePartitions.TheObjects[i].TheObjects[j];
      if (currentMultiplicity>0)
      { tempElt.AssignElementLieAlgebra(this->ElementsOfKofNegativeWeights.TheObjects[j], RFOne, RFZero, theParser.testAlgebra);
        tempElt.RaiseToPower(currentMultiplicity, RFOne);
        leftComponentBeforeModdingVermaRels.operator*=(tempElt);
      }
    }

    leftComponent.AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
    (leftComponentBeforeModdingVermaRels, theVermaZero, &theGlobalVariables,
     RFOne)
     ;
    currentLeftCollection.AddObjectOnTop(leftComponent);
    currentRightCollection.AddObjectOnTop(rightComponent);
    out << "<br>Left element before modding: " << leftComponentBeforeModdingVermaRels.ElementToString() << " and after modding: "<< leftComponent.ElementToString();
  }
  return out.str();
//  currentElement.AssignTensor(leftComponent, rightComponent);
}

template <class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
  (ElementUniversalEnvelopingOrdered<CoefficientType>& input, const ElementVermaModuleOrdered<CoefficientType>& theRingZero, GlobalVariables* theContext, const CoefficientType& theRingUnit)
{ this->theElT.operator=(input);
  assert(theRingZero.theSub.size==3);
  this->theSub=theRingZero.theSub;
  this->theElT.ModOutVermaRelations(false, this->theSub, theContext, theRingUnit);
}

template <class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::MultiplyOnTheLeft
(const ElementSimpleLieAlgebra& other, ElementVermaModuleOrdered<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  tempElt.AssignElementLieAlgebra(other, theRingUnit, theRingZero, *this->theElT.owner);
  //std::cout << "<br>multiplying " << tempElt.ElementToString() << " times " << this->ElementToString();
  tempElt*=this->theElT;
  output.theElT=tempElt;
  //std::cout << "<br> ... and the result before simplifying is: " << output.theElT.ElementToString();
  output.theElT.Simplify(theContext);
  //std::cout << "<br> before modding out we get: " << output.theElT.ElementToString();
  output.theElT.ModOutVermaRelations(false, this->theSub, theContext, theRingUnit);
  //std::cout << "<br> finally we get: " << output.theElT.ElementToString();
}

template <class CoefficientType>
std::string ElementVermaModuleOrdered<CoefficientType>::ElementToString()const
{ std::stringstream out;
  PolynomialOutputFormat polyFormatLocal;
  std::string tempS=MathRoutines::ElementToStringBrackets(this->theElT, polyFormatLocal);
  if (tempS.size()>1)
    out << "(";
  if (tempS!="1")
    out << tempS;
  if (tempS.size()>1)
    out << ")";
  if (tempS!="0")
    out << " v";
  return out.str();
}

template<class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::ActOnMe
  (const ElementSimpleLieAlgebra& actingElt, ElementVermaModuleOrdered<CoefficientType>& output, SemisimpleLieAlgebra& owner,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables* theContext)const
{ ElementUniversalEnvelopingOrdered<CoefficientType> tempElt;
  tempElt.AssignElementLieAlgebra(actingElt, theRingUnit, theRingZero, *this->theElT.owner);
//  std::cout << "<br>" << actingElt.ElementToString() << " acts on " << this->ElementToString();
  tempElt.LieBracketOnTheRight(this->theElT, output.theElT);
  output.theElT.Simplify(theContext);
  output.theSub=this->theSub;
//  std::cout << "<br>and the result before modding out is: " << output.ElementToString();
//  int numVars=output.theElt.GetNumVariables();
  output.theElT.ModOutVermaRelations(false, this->theSub, theContext, theRingUnit);
//  std::cout << "<br>and after modding out we get: " << output.ElementToString();
}

void ElementSimpleLieAlgebra::ActOnMe
  (const ElementSimpleLieAlgebra& theElt, ElementSimpleLieAlgebra& output, SemisimpleLieAlgebra& owner)
{ owner.LieBracket(theElt, *this, output);
}

void ElementSimpleLieAlgebra::ActOnMe
  (const ElementSimpleLieAlgebra& theElt, ElementSimpleLieAlgebra& output, SemisimpleLieAlgebra& owner,
   const RationalFunction& theRingUnit, const RationalFunction& theRingZero, GlobalVariables* theGlobalVariables)
{ owner.LieBracket(theElt, *this, output);
}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ElementToString()
{ std::stringstream out;
  out << "The elements (" << this->size << ") are: ";
  for (int i=0; i<this->size; i++)
  { out << this->TheObjects[i].ElementToString() << "<br>";
  }
  out << "starting space: " << this->theStartingSpace.ElementToString() << "<br>target space: " << this->theTargetSpace.ElementToString();
  return out.str();
}

template <class ElementLeft, class ElementRight, class CoefficientType>
bool TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::FindEigenVectorsWithRespectTo
  (List<ElementSimpleLieAlgebra>& theElementsActing,  List<ElementLeft>& theLeftElts, List<ElementRight>& theRightElts,
   SemisimpleLieAlgebra& theAlgebra, TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>& output,
   const ElementLeft& leftZero, const ElementRight& rightZero,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
{ Matrix<RationalFunction> theSystem;
  this->initTheSpacesForAdjointAction(theElementsActing, theAlgebra, theLeftElts, theRightElts, leftZero, rightZero, theRingUnit, theRingZero, theGlobalVariables);
  std::cout << "<br><br>the starting space: " << this->theStartingSpace.ElementToString();
  std::cout << "<br><br>the target space: " << this->theTargetSpace.ElementToString();
  this->PrepareTheMatrix(theElementsActing, theAlgebra, theSystem, theGlobalVariables);
  return false;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::initTheSpacesForAdjointAction
    (List<ElementSimpleLieAlgebra>& theElementsActing,
     SemisimpleLieAlgebra& theAlgebra,
     List<ElementLeft>& theLeftElts,
     List<ElementRight>& theRightElts,
     const ElementLeft& leftZero,
     const ElementRight& rightZero,
     const CoefficientType& theRingUnit,
     const CoefficientType& theRingZero,
     GlobalVariables& theGlobalVariables)
{ List<ElementLeft> tempLeftElts;
  List<ElementRight> tempRightElts;
  Vectors<RationalFunction> tempCoords;
  ElementLeft tempLeft;
  ElementRight tempRight;
  this->SetSize(theLeftElts.size);
  for (int i=0; i<theLeftElts.size; i++)
    for (int j=0; j<theElementsActing.size; j++)
    { assert(theLeftElts.TheObjects[i].theSub.size==3);
      theLeftElts.TheObjects[i].ActOnMe(theElementsActing.TheObjects[j], tempLeft, theAlgebra, theRingUnit, theRingZero, &theGlobalVariables);
      tempLeftElts.AddObjectOnTop(tempLeft);
      theRightElts.TheObjects[i].ActOnMe(theElementsActing.TheObjects[j], tempRight, theAlgebra);
      tempRightElts.AddObjectOnTop(tempRight);
      tempRight.ComputeDebugString( false, false);
      tempLeft.ComputeDebugString();
    }
  this->theStartingSpace.theRingUniT=theRingUnit;
  this->theTargetSpace.theRingUniT=theRingUnit;
  this->theStartingSpace.theRingZerO=theRingZero;
  this->theTargetSpace.theRingZerO=theRingZero;
  tempLeftElts.AddListOnTop(theLeftElts);
  tempRightElts.AddListOnTop(theRightElts);
  std::cout << "<br><br><br> before extracting a basis the new left elements are:<br>";
  for (int i=0; i<tempLeftElts.size; i++)
    std::cout << tempLeftElts.TheObjects[i].ElementToString() << "<br>";
  std::cout << "<br> before extracting a basis the new right elements are:<br>";
  for (int i=0; i<tempRightElts.size; i++)
    std::cout << tempRightElts.TheObjects[i].ElementToString() << "<br>";
  std::stringstream out;
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
    out << "Obtaining basis on the *target* left vector space...";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementLeft::GetBasisFromSpanOfElements(tempLeftElts, tempCoords, this->theTargetSpace.leftSpaceBasis, theRingUnit, theRingZero, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\nObtaining basis on the *target* right vector space...";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementRight::GetBasisFromSpanOfElements(tempRightElts, tempCoords, this->theTargetSpace.rightSpaceBasis, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\nObtaining basis on the *starting* left vector space";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementLeft::GetBasisFromSpanOfElements(theLeftElts, tempCoords, this->theStartingSpace.leftSpaceBasis, theRingUnit, theRingZero, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\nObtaining basis on the *starting* right vector space";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  ElementRight::GetBasisFromSpanOfElements(theRightElts, tempCoords, this->theStartingSpace.rightSpaceBasis, theGlobalVariables);
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
  { out << "done\n";
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  std::cout << "<br> starting space LOOK here: " << this->theStartingSpace.ElementToString();
  std::cout << "<br> target space LOOK here: " << this->theTargetSpace.ElementToString();
  std::cout << "<br> the elements we are acting upon are (" << theLeftElts.size << "): ";
  for (int i=0; i<theLeftElts.size; i++)
  { this->TheObjects[i].SetTensorProduct(theLeftElts.TheObjects[i], theRightElts.TheObjects[i], this->theStartingSpace, theGlobalVariables);
    std::cout << this->TheObjects[i].ElementToString() << ", ";
  }

}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::initTheSpacesSl2StringVersion
  ( List<ElementSimpleLieAlgebra>& theSimplePosElts,  List<ElementSimpleLieAlgebra>& theSimpleNegElts, SemisimpleLieAlgebra& theAlgebra,
    const ElementLeft& leftZero, const ElementRight& rightZero, const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
    GlobalVariables& theGlobalVariables)
{ this->theTargetSpace.theRingUniT=theRingUnit;
  this->theTargetSpace.theRingZerO=theRingZero;
  this->MakeActualSizeAtLeastExpandOnTop(this->theTargetSpace.leftSpaceBasis.size*this->theTargetSpace.rightSpaceBasis.size);
  this->size=0;
  std::stringstream de_Bugger;
  bool foundSeed=false;
  for (int i=0; i< this->theTargetSpace.leftSpaceBasis.size; i++)
    for (int j=0; j< this->theTargetSpace.rightSpaceBasis.size; j++)
    { this->ExpandOnTop(1);
      this->LastObject()->SetTensorProductBasisElement(i, j, this->theTargetSpace, theGlobalVariables);
      de_Bugger << this->TheObjects[i].ElementToString() << "  <br>\n";
      std::cout << "Element of index " << i << ", " << j << ": " << this->TheObjects[i].ElementToString() << "<br>";
      if(!foundSeed)
        if (this->theRightLowestWeight==this->theTargetSpace.rightWeights.TheObjects[j] && i==this->theTargetSpace.leftSpaceBasis.size-1)
          this->theSeedVector=*this->LastObject();
    }
  std::cout << this->theTargetSpace.ElementToString();
  //return;

  ElementLeft resultLeft; ElementRight  resultRight;
  Vector<CoefficientType> theResultVector;
  this->simplePositiveGeneratorsActions.SetSize(theSimplePosElts.size);
  this->simpleNegativeGeneratorsActions.SetSize(theSimpleNegElts.size);
  List<ElementSimpleLieAlgebra>* curentGeneratorCollection=&theSimplePosElts;
  for (int j=0; j<2; j++, curentGeneratorCollection=& theSimpleNegElts)
    for (int i=0; i<curentGeneratorCollection->size; i++)
    { ElementSimpleLieAlgebra& currentGenerator=curentGeneratorCollection->TheObjects[i];
      Polynomials<CoefficientType>* currentAction= (j==0)? (&this->simplePositiveGeneratorsActions.TheObjects[i]) : (&this->simpleNegativeGeneratorsActions.TheObjects[i]);
      currentAction->SetSize(this->theTargetSpace.leftSpaceBasis.size+this->theTargetSpace.rightSpaceBasis.size);
      for (int k=0; k<this->theTargetSpace.leftSpaceBasis.size; k++)
      { ElementLeft& currentLeftElt=this->theTargetSpace.leftSpaceBasis.TheObjects[k];
        Polynomial<CoefficientType>& currentPoly=currentAction->TheObjects[k];
        currentLeftElt.ActOnMe(currentGenerator, resultLeft, theAlgebra, this->theTargetSpace.theRingUniT, this->theTargetSpace.theRingZerO, &theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << " maps " << currentLeftElt.ElementToString() << " to " << resultLeft.ElementToString();
        this->theTargetSpace.GetInternalRepresentationFromVectorLeft(resultLeft, currentPoly, theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << "maps x_{" << k << "} to: " << currentPoly.ElementToString() << "<br><br>";
      }
      for (int k=0; k<this->theTargetSpace.rightSpaceBasis.size; k++)
      { ElementRight& currentRightElt=this->theTargetSpace.rightSpaceBasis.TheObjects[k];
        Polynomial<CoefficientType>& currentPoly=currentAction->TheObjects[k];
        currentRightElt.ActOnMe(currentGenerator, resultRight, theAlgebra, this->theTargetSpace.theRingUniT, this->theTargetSpace.theRingZerO, &theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << " maps " << currentRightElt.ElementToString() << " to " << resultRight.ElementToString();
        this->theTargetSpace.GetInternalRepresentationFromVectorRight(resultRight, currentPoly, theGlobalVariables);
        std::cout << "<br>" << currentGenerator.ElementToString() << "maps x_{" << k << "} to: " << currentPoly.ElementToString() << "<br><br>";
      }
    }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::PrepareTheMatrix
  (List<ElementSimpleLieAlgebra>& theElementsActing, SemisimpleLieAlgebra& theAlgebra, Matrix<CoefficientType>& output, GlobalVariables& theGlobalVariables)
{ TensorProductElement<ElementLeft, ElementRight, CoefficientType> resultElt;
  output.init(this->theTargetSpace.leftSpaceBasis.size*this->theTargetSpace.rightSpaceBasis.size*theElementsActing.size, this->size);
  output.NullifyAll(this->theStartingSpace.theRingZerO);
  this->ComputeDebugString();
  Vector<CoefficientType> theVector;
  for (int i=0; i<theElementsActing.size; i++)
  { ElementSimpleLieAlgebra& currentGenerator= theElementsActing.TheObjects[i];
    //if (i<1)
    //  i=1;
    for (int j=0; j<this->size; j++)
    { //if (j<6 && i==0)
        //j=6;
      TensorProductElement<ElementLeft, ElementRight, CoefficientType>& currentElt= this->TheObjects[j];
      assert(currentElt.internalRepresentation.NumVars==this->theStartingSpace.leftSpaceBasis.size+this->theStartingSpace.rightSpaceBasis.size);
      currentElt.ActOnMe(currentGenerator, resultElt, this->theStartingSpace, this->theTargetSpace, theAlgebra, theGlobalVariables);
      /*resultElt.ComputeDebugString();
      if (i==1 && j==2)
      { resultElt.ComputeDebugString();
        this->flagAnErrorHasOccurredTimeToPanic=true;
      }*/
      resultElt.ElementToVector(theVector, this->theTargetSpace, theGlobalVariables);
      theVector.ComputeDebugString();
      std::cout << "<br>" << currentGenerator.ElementToString() << " acting on ";
      std::cout << currentElt.ElementToString(this->theStartingSpace) << " gives ";
      std::cout << resultElt.ElementToString(this->theTargetSpace);
      for (int k=0; k<theVector.size; k++)
        output.elements[i*theVector.size+k][j]=theVector.TheObjects[k];
    }
  }
  std::cout << "<br><br><br><br>... and the matrix is: <div class=\"math\">" << output.ElementToString(false, true) <<"</div>";

}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ElementToString
  (TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner)
{ int theLeftIndex=-1, theRightIndex=-1;
  std::stringstream out;
  for (int i=0; i<this->internalRepresentation.size; i++)
  { Monomial<CoefficientType>& currentMon=this->internalRepresentation.TheObjects[i];
    owner.GetLeftAndRightIndexFromMonomial(currentMon, theLeftIndex, theRightIndex);
    std::string tempS= currentMon.Coefficient.ElementToString();
    if (tempS=="1")
      out << "(" << tempS << ")";
    out << "(" << owner.leftSpaceBasis.TheObjects[theLeftIndex].ElementToString() << ")\\otimes(" << owner.rightSpaceBasis.TheObjects[theRightIndex].ElementToString() << ")";
  }
  return out.str();
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ElementToVector
  (Vector<CoefficientType>& output, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)const
{ output.MakeZero(owner.leftSpaceBasis.size*owner.rightSpaceBasis.size, owner.theRingZerO);
  for (int i=0; i<this->internalRepresentation.size; i++)
  { int relevantIndex=owner.GetIndexFromMonomial(this->internalRepresentation.TheObjects[i]);
    if (TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::flagAnErrorHasOccurredTimeToPanic)
    { output.ComputeDebugString();
      this->internalRepresentation.TheObjects[i].Coefficient.ComputeDebugString();
    }
    output.TheObjects[relevantIndex]=this->internalRepresentation.TheObjects[i].Coefficient;
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::AssignVector
  (Vector<CoefficientType>& input, TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
{ this->Nullify(owner);
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> tempElt;
  for (int i=0; i<input.size; i++)
  { owner.GetEltFromIndex(i, tempElt, theGlobalVariables);
    this->operator+=(tempElt);
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ActOnMe
  (const ElementSimpleLieAlgebra& theElt, TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theStartingSpace,
   TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& theTargetSpace,
   SemisimpleLieAlgebra& theAlgebra, GlobalVariables& theGlobalVariables)
{ assert(this!=&output);
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> newSummand;
  output.Nullify(theTargetSpace);
  ElementLeft leftComponent, newLeftComponent;
  ElementRight rightComponent, newRightComponent;
  Polynomial<CoefficientType> tempP;
  for (int i=0; i<this->internalRepresentation.size; i++)
  { theStartingSpace.GetComponentsFromInternalRepresentation(this->internalRepresentation.TheObjects[i], leftComponent, rightComponent);
    //leftComponent.ComputeDebugString();
    //rightComponent.ComputeDebugString(false, false);
    leftComponent.ActOnMe(theElt, newLeftComponent, theAlgebra, theStartingSpace.theRingUniT, theStartingSpace.theRingZerO, &theGlobalVariables);
    std::cout << "<br>" << theElt.ElementToString() << " acting on " << leftComponent.ElementToString() << " gives " << newLeftComponent.ElementToString();
    rightComponent.ActOnMe(theElt, newRightComponent, theAlgebra);
    leftComponent.ComputeDebugString();
    rightComponent.ComputeDebugString(false, false);
    newRightComponent.ComputeDebugString(false, false);
    newLeftComponent.ComputeDebugString();
    std::cout << "<br>" << theElt.ElementToString() << " acting on " << rightComponent.ElementToString() << " gives " << newRightComponent.ElementToString();

    theTargetSpace.GetInternalRepresentationFromLeftAndRight(newLeftComponent, rightComponent, tempP, theGlobalVariables);
    tempP.ComputeDebugString();
    output.internalRepresentation.AddPolynomial(tempP);
    theTargetSpace.GetInternalRepresentationFromLeftAndRight(leftComponent, newRightComponent, tempP, theGlobalVariables);
    tempP.ComputeDebugString();
    output.internalRepresentation.AddPolynomial(tempP);
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::SetTensorProduct
  (const  ElementLeft& leftVector, const  ElementRight& rightVector, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
{ owner.GetInternalRepresentationFromLeftAndRight(leftVector, rightVector, this->internalRepresentation, theGlobalVariables);
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductElement<ElementLeft, ElementRight, CoefficientType>::SetTensorProductBasisElement
  (int leftIndex, int rightIndex, const TensorProductSpace<ElementLeft, ElementRight, CoefficientType>& owner, GlobalVariables& theGlobalVariables)
{ Polynomial<CoefficientType> tempP;
  tempP.MakeMonomialOneLetter(owner.leftSpaceBasis.size+owner.rightSpaceBasis.size, leftIndex, 1 , owner.theRingUniT);
  this->internalRepresentation.MakeMonomialOneLetter(owner.leftSpaceBasis.size+owner.rightSpaceBasis.size, rightIndex+owner.leftSpaceBasis.size, 1 , owner.theRingUniT);
  this->internalRepresentation*=tempP;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductElement<ElementLeft, ElementRight, CoefficientType>::ElementToString()
{ std::stringstream out;
  std::stringstream tempS;
  out << this->internalRepresentation.ElementToString();
  return out.str();
}

template<class CoefficientType>
template<class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<CoefficientType>::GetBasisFromSpanOfElements
  (List<ElementUniversalEnvelopingOrdered<CoefficientType> >& theElements,
   Vectors<CoefficientTypeQuotientField>& outputCoords,
   List<ElementUniversalEnvelopingOrdered<CoefficientType> >& outputTheBasis, const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero,
   GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  ElementUniversalEnvelopingOrdered<CoefficientType> outputCorrespondingMonomials;
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddObjectOnTopNoRepetitionOfObjectHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordsBeforeReduction.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
  { Vector<CoefficientTypeQuotientField>& currentList=outputCoordsBeforeReduction.TheObjects[i];
    currentList.MakeZero(outputCorrespondingMonomials.size, theFieldZero);
    ElementUniversalEnvelopingOrdered<CoefficientType>& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnvelopingOrdered<CoefficientType>& currentMon=currentElt.TheObjects[j];
      currentList.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
    }
  }
  outputTheBasis.size=0;
  outputTheBasis.MakeActualSizeAtLeastExpandOnTop(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.MakeActualSizeAtLeastExpandOnTop(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis, theGlobalVariables);
  for (int i=0; i<selectedBasis.CardinalitySelection; i++)
    outputTheBasis.AddObjectOnTop(theElements.TheObjects[selectedBasis.elements[i]]);
  Matrix<CoefficientType> bufferMat;
  Vectors<CoefficientType> bufferVectors;
  outputCoordsBeforeReduction.GetCoordsInBasis(basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero);
}

void ElementSimpleLieAlgebra::GetBasisFromSpanOfElements
  (List<ElementSimpleLieAlgebra>& theElements, Vectors<RationalFunction>& outputCoords, List<ElementSimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables)
{ roots theRootForm;
  theRootForm.SetSize(theElements.size);
  for(int i=0; i<theElements.size; i++)
  { ElementSimpleLieAlgebra& currentElt=theElements.TheObjects[i];
    currentElt.ElementToVectorNegativeRootSpacesFirst(theRootForm.TheObjects[i]);
  }
  int theRank=0; int numRoots=0;
  if (theElements.size>0)
  { theRank=theElements.TheObjects[0].Hcomponent.size;
    numRoots=theElements.TheObjects[0].coeffsRootSpaces.size;
  }
  theRootForm.ChooseABasis(theGlobalVariables);
  outputTheBasis.SetSize(theRootForm.size);
  for(int i=0; i<theRootForm.size; i++)
  { ElementSimpleLieAlgebra& currentElt=outputTheBasis.TheObjects[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(theRootForm.TheObjects[i], numRoots, theRank);
  }

}

void roots::ChooseABasis(GlobalVariables& theGlobalVariables)
{ roots output;
  for (int i=0; i<this->size; i++)
  { output.AddObjectOnTop(this->TheObjects[i]);
    if (output.GetRankOfSpanOfElements(theGlobalVariables)< output.size)
      output.PopLastObject();
  }
  this->operator=(output);
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::GetInternalRepresentationFromVectorRight
  (const ElementRight& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
{ output.Nullify(this->leftSpaceBasis.size+this->rightSpaceBasis.size);
  Monomial<CoefficientType> tempMon;
  Vector<CoefficientType> vectorFormInput;
  input.GetCoordsInBasis(this->rightSpaceBasis, vectorFormInput, theGlobalVariables);
  for(int i=0; i<vectorFormInput.size; i++)
    if (!vectorFormInput.TheObjects[i].IsEqualToZero())
    { tempMon.MakeMonomialOneLetter(output.NumVars, i+this->leftSpaceBasis.size, 1, vectorFormInput.TheObjects[i]);
      output.AddMonomial(tempMon);
    }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
bool TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::GetInternalRepresentationFromVectorLeft
  (const ElementLeft& input, Polynomial<CoefficientType>& output, GlobalVariables& theGlobalVariables)const
{ output.Nullify(this->leftSpaceBasis.size+this->rightSpaceBasis.size);
  Monomial<CoefficientType> tempMon;
  Vector<CoefficientType> vectorFormInput;
  std::string tempS=input.ElementToString();
  std::string tempS2=this->leftSpaceBasis.ElementToStringGeneric();
  if (! input.GetCoordsInBasis(this->leftSpaceBasis, vectorFormInput, this->theRingUniT, this->theRingZerO, theGlobalVariables))
    return false;
  vectorFormInput.ComputeDebugString();
  for(int i=0; i<vectorFormInput.size; i++)
    if (!vectorFormInput.TheObjects[i].IsEqualToZero())
    { tempMon.MakeMonomialOneLetter(output.NumVars, i, 1, vectorFormInput.TheObjects[i]);
      output.AddMonomial(tempMon);
    }
  return true;
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::
GetComponentsFromInternalRepresentation
  (const Monomial<CoefficientType>& input, ElementLeft& leftOutput, ElementRight& rightOutput)
{ for (int i=0; i<this->leftSpaceBasis.size; i++)
    if (input.degrees[i]==1)
    { leftOutput.operator=(this->leftSpaceBasis.TheObjects[i]);
      break;
    }
  for (int i=0; i<this->rightSpaceBasis.size; i++)
    if (input.degrees[i+leftSpaceBasis.size]==1)
    { rightOutput.operator=(this->rightSpaceBasis.TheObjects[i]);
      break;
    }
  leftOutput.operator*=(input.Coefficient);

}

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductSpace<ElementLeft, ElementRight, CoefficientType>::ElementToString()
{ std::stringstream out;
  out << "<br> Basis of the left components (" << this->leftSpaceBasis.size << " elements): <br>";
  for (int i=0; i<this->leftSpaceBasis.size; i++)
    out << this->leftSpaceBasis.TheObjects[i].ElementToString() << "<br> ";
  out << "<br> Basis of the right components (" << this->rightSpaceBasis.size << " elements): <br>";
  for (int i=0; i<this->rightSpaceBasis.size; i++)
    out << this->rightSpaceBasis.TheObjects[i].ElementToString() << "<br> ";
  return out.str();
}

template<class CoefficientType>
void ElementVermaModuleOrdered<CoefficientType>::GetBasisFromSpanOfElements
  (List<ElementVermaModuleOrdered>& theElements, Vectors<RationalFunction>& outputCoordinates, List<ElementVermaModuleOrdered>& outputTheBasis,
    const RationalFunction& RFOne, const RationalFunction& RFZero, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnvelopingOrdered<CoefficientType> > theEltsUEform;
  theEltsUEform.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
    theEltsUEform.TheObjects[i]=theElements.TheObjects[i].theElT;
  List<ElementUniversalEnvelopingOrdered<CoefficientType> > theBasisUEform;
  ElementUniversalEnvelopingOrdered<CoefficientType>::GetBasisFromSpanOfElements(theEltsUEform, outputCoordinates, theBasisUEform, RFOne, RFZero, theGlobalVariables);
  outputTheBasis.SetSize(theBasisUEform.size);
  for (int i=0; i<theBasisUEform.size; i++)
  { outputTheBasis.TheObjects[i].theElT=theBasisUEform.TheObjects[i];
    outputTheBasis.TheObjects[i].theSub=theElements.TheObjects[0].theSub;
  }
}

template<class CoefficientType>
bool ElementVermaModuleOrdered<CoefficientType>::GetCoordsInBasis
  (const List<ElementVermaModuleOrdered<CoefficientType> >& theBasis, Vector<CoefficientType>& output, const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables& theGlobalVariables)const
{ List<ElementUniversalEnvelopingOrdered<CoefficientType> > theEltsUEform;
  theEltsUEform.SetSize(theBasis.size);
  for (int i=0; i<theBasis.size; i++)
  { theEltsUEform.TheObjects[i]=theBasis.TheObjects[i].theElT;
  }
  return this->theElT.GetCoordsInBasis(theEltsUEform, output, theRingUnit, theRingZero, theGlobalVariables);
}

bool ElementSimpleLieAlgebra::GetCoordsInBasis(const List<ElementSimpleLieAlgebra>& theBasis, Vector<Rational>& output, GlobalVariables& theGlobalVariables)const
{ Vectors<Rational> tempBasis;
  root tempRoot;
  tempBasis.SetSize(theBasis.size);
  for (int i=0 ; i<theBasis.size; i++)
    theBasis.TheObjects[i].ElementToVectorNegativeRootSpacesFirst(tempBasis.TheObjects[i]);
  this->ElementToVectorNegativeRootSpacesFirst(tempRoot);
  return tempRoot.GetCoordsInBasiS(tempBasis, output, (Rational) 1, (Rational) 0);
}

bool RationalFunction::gcdQuicK
  (const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output)
{ if (left.TotalDegree()>1 && right.TotalDegree()>1)
    return false;
  PolynomialRationalCoeff quotient, remainder;
  if (left.TotalDegree()>right.TotalDegree())
  { left.DivideBy(right, quotient, remainder);
    if (remainder.IsEqualToZero())
      output=right;
    else
      output.MakeNVarConst(left.NumVars, (Rational) 1);
  } else
  { std::string tempS1, tempS2, tempS3, tempS4;
    tempS1=left.ElementToString();
    tempS2=right.ElementToString();
    if ((&right)==(PolynomialRationalCoeff*)0x82ea6b8)
      std::cout << tempS1;
    right.DivideBy(left, quotient, remainder);
    tempS3=quotient.ElementToString();
    tempS4=remainder.ElementToString();
    if (remainder.IsEqualToZero())
      output=left;
    else
      output.MakeNVarConst(left.NumVars, (Rational) 1);
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
  ParserNode& leftNode=this->owner->TheObjects[this->children.TheObjects[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children.TheObjects[1]];
  switch(leftNode.ExpressionType)
  { case ParserNode::typeIntegerOrIndex:
    case ParserNode::typeRational:
    case ParserNode::typePoly:
    case ParserNode::typeRationalFunction:
      this->ExpressionType=this->typeRationalFunction;
      this->InitForMultiplication(&theGlobalVariables);
      if (! rightNode.ConvertToType(this->typeRationalFunction, theGlobalVariables))
      { this->SetError(this->errorConversionError);
        return;
      }
      leftNode.ConvertToType(this->typeRationalFunction, theGlobalVariables);
      this->ratFunction.GetElement()=leftNode.ratFunction.GetElementConst();
      this->ratFunction.GetElement()/=rightNode.ratFunction.GetElement();
      this->ExpressionType=this->typeRationalFunction;
      this->ReduceRatFunction();
      break;
    case ParserNode::typeUEelement:
      if (!rightNode.ConvertToType(this->typeRational, theGlobalVariables))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->UEElement.GetElement()=leftNode.UEElement.GetElement();
      this->UEElement.GetElement()/=rightNode.rationalValue;
      this->ExpressionType=this->typeUEelement;
    case ParserNode::typeUEElementOrdered:
      if (!rightNode.ConvertToType(this->typeRational, theGlobalVariables))
      { this->SetError(this->errorDunnoHowToDoOperation);
        return;
      }
      this->UEElementOrdered.GetElement()=leftNode.UEElementOrdered.GetElement();
      this->UEElementOrdered.GetElement()/=rightNode.rationalValue;
      this->ExpressionType=this->typeUEElementOrdered;
    break;
    default: this->SetError(this->errorDivisionByNonAllowedType); return;
  }
}


void RationalFunction::RaiseToPower(int thePower)
{ PolynomialRationalCoeff theNum, theDen;
  this->checkConsistency();
  switch (this->expressionType)
  { case RationalFunction::typeRational:
      this->ratValue.RaiseToPower(thePower);
      break;
    case RationalFunction::typePoly:
      this->Numerator.GetElement().RaiseToPower(thePower, (Rational) 1);
      break;
    case RationalFunction::typeRationalFunction:
      this->Numerator.GetElement().RaiseToPower(thePower, (Rational) 1);
      this->Denominator.GetElement().RaiseToPower(thePower, (Rational) 1);
      break;
  }
  this->checkConsistency();
}

void ComputationSetup::TheG2inB3Computation(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ Parser theParser;
  theParser.theHmm.MakeG2InB3(theParser, theGlobalVariables);
  SSalgebraModule theModule;
  std::stringstream out;
  theModule.InduceFromEmbedding(out, theParser.theHmm, theGlobalVariables);
  List<ElementSimpleLieAlgebra> theBasis;
  theBasis.SetSize(theParser.theHmm.theRange.GetNumGenerators());
  for (int i=0; i<theParser.theHmm.imagesAllChevalleyGenerators.size; i++)
  { int theIndex=i;
    if (i>=6 && i<8)
      theIndex=3+i;
    if (i>=8)
      theIndex=i+7;
      /*
      if (i==0) theIndex=3;
      if (i==1) theIndex=4;
      if (i==2) theIndex=5;
      if (i==3) theIndex=6;
      if (i==4) theIndex=7;
      if (i==5) theIndex=8;
      if (i==6) theIndex=9;
      if (i==7) theIndex=10;
      if (i==8) theIndex=12;
      if (i==9) theIndex=13;
      if (i==10) theIndex=14;
      if (i==11) theIndex=15;
      if (i==12) theIndex=16;
      if (i==13) theIndex=17;
      */
    ElementSimpleLieAlgebra& currentElt=theBasis.TheObjects[theIndex];
    currentElt=theParser.theHmm.imagesAllChevalleyGenerators.TheObjects[i];
  }
  for (int i=0; i<theModule.moduleElementsEmbedded.size; i++)
  { int theIndex=i+6;
    if (i>=3)
      theIndex=8+i;
    /*
    if (i==0) theIndex=0;
    if (i==1) theIndex=1;
    if (i==2) theIndex=2;
    if (i==3) theIndex=11;
    if (i==4) theIndex=18;
    if (i==5) theIndex=19;
    if (i==6) theIndex=20;
    */
    ElementSimpleLieAlgebra& currentElt=theBasis.TheObjects[theIndex];
    currentElt=theModule.moduleElementsEmbedded.TheObjects[i];
  }
  for (int i=0; i<theBasis.size; i++)
  { //std::cout << "<br>" << theBasis.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, false, theParser.theHmm.theRange);
  }
  theParser.testAlgebra.init(theBasis, theParser.theHmm.theRange, theGlobalVariables);
  std::string  civilizedInput="secretSauceOrdered";
  theParser.ParseEvaluateAndSimplify(civilizedInput, theGlobalVariables);
}

void RationalFunction::gcd
(const PolynomialRationalCoeff& left, const PolynomialRationalCoeff& right, PolynomialRationalCoeff& output, GlobalVariables* theContext)
{ if (RationalFunction::gcdQuicK(left, right, output))
    return;
  if (theContext==0)
  { PolynomialRationalCoeff buffer1, buffer2, buffer3, buffer4, buffer5; List<PolynomialRationalCoeff> bufferList; Monomial<Rational> tempMon1, tempMon2;
    RationalFunction::gcd(left, right, output, buffer1, buffer2, buffer3, buffer4, buffer5, tempMon1, tempMon2, bufferList);
  } else
    RationalFunction::gcd(left, right, output, theContext->RFgcdBuffer1, theContext->RFgcdBuffer2, theContext->RFgcdBuffer3, theContext->RFgcdBuffer4, theContext->RFgcdBuffer5,  theContext->RFgcdBuferMon1, theContext->RFgcdBuferMon2,  theContext->RFgcdBufferList1);
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ClimbDownFromHighestWeightAlongSl2String
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output,
    CoefficientType& outputCoeff,
    Polynomials<CoefficientType>& posGenerator,
    Polynomials<CoefficientType>& negGenerator, int generatorPower, const CoefficientType& theRingUnit)
{ assert(&input!=&output);
  CoefficientType currentWeight;
  input.ActOnMe(negGenerator, output, theRingUnit);
  output.ActOnMe(posGenerator, theRingUnit);
  input.internalRepresentation.IsProportionalTo(output.internalRepresentation, currentWeight, theRingUnit);
  CoefficientType RaiseCoeff;
  RaiseCoeff=this->theTargetSpace.theRingZerO;
  outputCoeff=theRingUnit;
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    output.ActOnMe(negGenerator, output, theRingUnit);
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ClimbDownFromVectorAccordingToSequence
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& output, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
{ assert(&input!=&output);
  output=input;
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> tempElt;
  outputCoeff=theRingUnit;
  CoefficientType tempCoeff;
  for (int i=inputGeneratorSequence.size-1; i>=0; i--)
  { Polynomials<CoefficientType>& currentPosGen= this->simplePositiveGeneratorsActions.TheObjects[inputGeneratorSequence.TheObjects[i]];
    Polynomials<CoefficientType>& currentNegGen=this->simpleNegativeGeneratorsActions.TheObjects[inputGeneratorSequence.TheObjects[i]];
    this->ClimbDownFromHighestWeightAlongSl2String
    (output, tempElt, tempCoeff, currentPosGen, currentNegGen, inputGeneratorPowers.TheObjects[i], theRingUnit);
    outputCoeff*=(tempCoeff);
    output=tempElt;
  }
}

template <class ElementLeft, class ElementRight, class CoefficientType>
void TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ClimbUpFromVector
  (TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input,
   TensorProductElement<ElementLeft, ElementRight, CoefficientType>& outputLastNonZero,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
{ TensorProductElement<ElementLeft, ElementRight, CoefficientType> tempElt;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  outputGeneratorPowers.size=0;
  outputGeneratorSequence.size=0;
  bool found=true;
  while (found)
  { found=false;
    for (int i=0; i<this->simplePositiveGeneratorsActions.size; i++)
    { Polynomials<CoefficientType>& currentAction=this->simplePositiveGeneratorsActions.TheObjects[i];
      int counter=0;
      for(outputLastNonZero.ActOnMe(currentAction, tempElt, this->theTargetSpace.theRingUniT); !tempElt.IsEqualToZero(); tempElt.ActOnMe(currentAction, this->theTargetSpace.theRingUniT))
      { counter++;
        found=true;
        outputLastNonZero=tempElt;
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

template <class ElementLeft, class ElementRight, class CoefficientType>
std::string TensorProductSpaceAndElements<ElementLeft, ElementRight, CoefficientType>::ExtractHighestWeightVectorsFromVector
( TensorProductElement<ElementLeft, ElementRight, CoefficientType>& input, List<TensorProductElement<ElementLeft, ElementRight, CoefficientType> >& outputVectors,
  GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit)
{ //Index ordering of simpleNegGenerators:
  //if simplePosGenerators.TheObjects[i] is a positive root space then its opposite root space should be
  //simpleNegGenerators.TheObjects[i]
  std::stringstream out;
  outputVectors.size=0;
  TensorProductElement<ElementLeft, ElementRight, CoefficientType> remainderElement, componentElement, currentHighestWeightElement;
  remainderElement=input;
  CoefficientType theCoeff;
  List<int> GeneratorSequence, GeneratorPowers;
  while(!remainderElement.IsEqualToZero())
  { this->ClimbUpFromVector
      (remainderElement, currentHighestWeightElement,
       GeneratorSequence, GeneratorPowers, theGlobalVariables);
    this->ClimbDownFromVectorAccordingToSequence
      (currentHighestWeightElement, componentElement, theCoeff, GeneratorSequence, GeneratorPowers, theGlobalVariables, theRingUnit);
    assert(!theCoeff.IsEqualToZero());
    componentElement/=theCoeff;
    outputVectors.AddObjectOnTop(componentElement);
    remainderElement-=*outputVectors.LastObject();
    std::cout << "<br>found element: " << componentElement.ElementToString();
  }
  return out.str();
}

template<class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::Nullify
  (GeneralizedVermaModuleData<CoefficientType>& owner)
{ this->theOwner=&owner;
  this->leftComponents.SetSize(owner.theFDspace.size);
  for (int i=0; i<this->leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentElt=this->leftComponents.TheObjects[i];
    currentElt.Nullify(this->theOwner->theOwner, this->theOwner->VermaHighestWeighSub);
  }
}

template <class CoefficientType>
std::string ElementGeneralizedVerma<CoefficientType>::ExtractHighestWeightVectors
  (Parser& theParser, List<ElementGeneralizedVerma<CoefficientType> >& outputVectors,
    GlobalVariables& theGlobalVariables)
{ //Index ordering of simpleNegGenerators:
  //if simplePosGenerators.TheObjects[i] is a positive root space then its opposite root space should be
  //simpleNegGenerators.TheObjects[i]
  std::stringstream out;
  outputVectors.size=0;
  ElementGeneralizedVerma<CoefficientType> remainderElement, tempElt, currentHighestWeightElement;
  remainderElement=*this;
  CoefficientType tempCoeff, theCoeff, CentralCharacterAction;
  List<int> GeneratorSequence, GeneratorPowers;
  int counter=0;
  ElementUniversalEnveloping abstractCasimir, embeddedCasimirNonOrdered;
  abstractCasimir.MakeCasimir(theParser.theHmm.theDomain, theParser.theHmm.theRange.GetRank(), theGlobalVariables);
  out << "<br>abstract Casimir: " << abstractCasimir.ElementToString();
  theParser.theHmm.ApplyHomomorphism(abstractCasimir, embeddedCasimirNonOrdered, theGlobalVariables);
  embeddedCasimirNonOrdered.Simplify();
  out << "<br> embedded Casimir non-ordered: " << embeddedCasimirNonOrdered.ElementToString();
  ElementUniversalEnvelopingOrdered<CoefficientType> embeddedCasimir;
  embeddedCasimir.AssignElementUniversalEnveloping(embeddedCasimirNonOrdered, theParser.testAlgebra, this->theOwner->theRingUnit, this->theOwner->theRingZero, &theGlobalVariables);
  out << "<br>the embedded Casimir is: " << embeddedCasimir.ElementToString();
  //return out.str();
  while(!remainderElement.IsEqualToZero() )//&& counter<7)
  { //std::cout << "<br>remainder element: " << remainderElement.ElementToString();
    this->ClimbUpFromVector
      (remainderElement, currentHighestWeightElement,
       GeneratorSequence, GeneratorPowers, theGlobalVariables);
    assert(GeneratorSequence.size==GeneratorPowers.size);
    std::cout << "<br><br><br>And the current sequence is: ";
    for (int i=0; i<GeneratorSequence.size; i++)
    { std:: cout << "(generator " << GeneratorSequence.TheObjects[i] << ")^" << GeneratorPowers.TheObjects[i] << ", ";
    }
    out << "<br>current highest weight element: <div class=\"math\"> " << currentHighestWeightElement.ElementToString() << "</div>";
    currentHighestWeightElement.ActOnMe(embeddedCasimir, tempElt, &theGlobalVariables);
    out << "<br>Casimir acting on current highest: <div class=\"math\"> " << tempElt.ElementToString() <<"</div>";
    bool tempBool=currentHighestWeightElement.IsProportionalTo(tempElt, CentralCharacterAction);
    out << "<br>proportionality coefficient: " << CentralCharacterAction.ElementToString();
    assert(tempBool);
    remainderElement.ActOnMe(embeddedCasimir, tempElt, &theGlobalVariables);
    out << "<br>Casimir acting on <div class=\"math\">" << remainderElement.ElementToString() << "</div> yields <div class=\"math\">" << tempElt.ElementToString() << "</div>";
    remainderElement*=CentralCharacterAction;
    remainderElement-=tempElt;
    remainderElement.ClearDenominators(tempCoeff);
    std::cout << "<br><br>Remaining after clearing the denominators: " << remainderElement.ElementToString();
    counter++;
  }
  return out.str();
}

template <class CoefficientType>
std::string ElementGeneralizedVerma<CoefficientType>::GetStringFormGmodKindex(int theIndex)
{ std::stringstream out;
  if (theIndex<3)
    out << "f_{" << theIndex-3 << "}";
  if (theIndex==3)
    out << "f_{0,3}";
  if (theIndex>3)
    out << "f_{" << theIndex-3 << "}";
  return out.str();
}

template <class CoefficientType>
std::string ElementGeneralizedVerma<CoefficientType>::ElementToString
(bool displayLeftComponenetsOnTheRight, bool useOtimes, bool useFs)
{ if (this->IsEqualToZero())
    return "0";
  std::stringstream out; std::string leftString, rightString;
  bool foundFirstNonZero=false;
  for (int i=0; i<this->leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentElt= this->leftComponents.TheObjects[i];
    if (!currentElt.IsEqualToZero())
    { if (foundFirstNonZero)
        out << "+";
      foundFirstNonZero=true;
      leftString=MathRoutines::ElementToStringBrackets(currentElt);
      if (!useFs)
        rightString=MathRoutines::ElementToStringBrackets(this->theOwner->theFDspace.TheObjects[i]);
      else
        rightString= this->GetStringFormGmodKindex(i);
      if (displayLeftComponenetsOnTheRight)
        MathRoutines::swap(leftString, rightString);
      out << leftString;
      if (useOtimes)
        out << "\\otimes";
      out << rightString;
    }
  }
  return out.str();
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClimbDownFromHighestWeightAlongSl2String
(ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& output,
    CoefficientType& outputCoeff,
   const ElementSimpleLieAlgebra& posGenerator, const ElementSimpleLieAlgebra& negGenerator,
     int generatorPower, GlobalVariables& theGlobalVariables)
{ assert(&input!=&output);
  CoefficientType currentWeight;
  ElementGeneralizedVerma<CoefficientType> debugElt;
  input.ActOnMe(posGenerator, debugElt, &theGlobalVariables);
  std::cout << "<br>climbing down with (" << negGenerator.ElementToString() << " )^" << generatorPower << ": ";
  std::cout << input.ElementToString() << "->";
  //if (!debugElt.IsEqualToZero())
  //{ std::cout << "<br>positive generator acting on the supposedly highest weight vector equals: " << debugElt.ElementToString();
  //  std::cout.flush();
  //  assert(false);
  //}
  input.ActOnMe(negGenerator, output, &theGlobalVariables);
  std::cout << output.ElementToString() << "->";
  output.ActOnMe(posGenerator, &theGlobalVariables);
  std::cout << output.ElementToString() << "->";
  std::cout.flush();
  bool tempBool=input.IsProportionalTo(output, currentWeight);
  assert(tempBool);
  CoefficientType RaiseCoeff;
  RaiseCoeff=this->theOwner->theRingZero;
  outputCoeff=this->theOwner->theRingUnit;
  output=input;
  for (int i=0; i<generatorPower; i++)
  { RaiseCoeff+=currentWeight;
    currentWeight-=2;
    outputCoeff*=RaiseCoeff;
    output.ActOnMe(negGenerator, &theGlobalVariables);

  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClimbUpFromVector
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& outputLastNonZero,
   List<int>& outputGeneratorSequence, List<int>& outputGeneratorPowers, GlobalVariables& theGlobalVariables)
{ ElementGeneralizedVerma<CoefficientType> tempElt;
  assert(&input!=&outputLastNonZero);
  outputLastNonZero=input;
  outputGeneratorPowers.size=0;
  outputGeneratorSequence.size=0;
  bool found=true;
  while (found)
  { for (int i=0; i<this->theOwner->posGenerators.size; i++)
    { ElementSimpleLieAlgebra& currentGen=this->theOwner->posGenerators.TheObjects[i];
      int counter=0;
      found=false;
      for(outputLastNonZero.ActOnMe(currentGen, tempElt, &theGlobalVariables); !tempElt.IsEqualToZero(); tempElt.ActOnMe(currentGen, &theGlobalVariables))
      { counter++;
        found=true;
        outputLastNonZero=tempElt;
  //      std::cout << "<br> Current state of the element: " << outputLastNonZero.ElementToString();
    //    std::cout.flush();
      }
      if (found)
      { outputGeneratorSequence.AddObjectOnTop(i);
        outputGeneratorPowers.AddObjectOnTop(counter);
      //  std:: cout << "<br> the corresponding generator: " << i << " to the power of " << counter;
      }
    }
  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ActOnMe
(const ElementSimpleLieAlgebra& theElt, ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
{ assert(this!=&output);
  ElementGeneralizedVerma<CoefficientType> actingOnRight;
  output.Nullify(*this->theOwner);
  actingOnRight.Nullify(*this->theOwner);
  ElementSimpleLieAlgebra tempLieElt;
  Vector<RationalFunction> coordFormRightElt;
  for (int i=0; i<output.leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentLeft= this->leftComponents.TheObjects[i];
    currentLeft.ActOnMe(theElt, output.leftComponents.TheObjects[i], this->theOwner->theOwner.theOwner, this->theOwner->theRingUnit, this->theOwner->theRingZero, theContext);
    currentLeft.MultiplyOnTheLeft(theElt, output.leftComponents.TheObjects[i], this->theOwner->theRingUnit, this->theOwner->theRingZero, theContext);
  }
//  std::cout << "<br><br>acting on the Universal enveloping part we get:" << output.ElementToString();
  ElementVermaModuleOrdered<CoefficientType> tempElt;
  for (int i=0; i<output.leftComponents.size; i++)
  { ElementSimpleLieAlgebra& currentAlgebraElt=this->theOwner->theFDspace.TheObjects[i];
    this->theOwner->theOwner.theOwner.LieBracket(theElt, currentAlgebraElt, tempLieElt);
    //std::cout << "<br><br>" << theElt.ElementToString() << " acts on " << currentAlgebraElt.ElementToString() << " to obtain: " << tempLieElt.ElementToString();

    bool tempBool=tempLieElt.GetCoordsInBasis(this->theOwner->theFDspace, coordFormRightElt, *theContext);
    //std::cout << "<br>translated in coordinate form this is: " << coordFormRightElt.ElementToString();
    assert(tempBool);
    for (int j=0; j<coordFormRightElt.size; j++)
    { tempElt=this->leftComponents.TheObjects[i];
      tempElt*=(coordFormRightElt.TheObjects[j]);
      output.leftComponents.TheObjects[j]+=tempElt;
    }
  }
//  std::cout<< "<br><br>finally we have: " << output.ElementToString() << " <br>";

}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClimbDownFromVectorAccordingToSequence
  (ElementGeneralizedVerma<CoefficientType>& input,
   ElementGeneralizedVerma<CoefficientType>& output, CoefficientType& outputCoeff,
   List<int>& inputGeneratorSequence, List<int>& inputGeneratorPowers, GlobalVariables& theGlobalVariables)
 { assert(&input!=&output);
  output=input;
  ElementGeneralizedVerma<CoefficientType> tempElt;
  outputCoeff=this->theOwner->theRingUnit;
  CoefficientType tempCoeff;
  for (int i=inputGeneratorSequence.size-1; i>=0; i--)
  { ElementSimpleLieAlgebra& currentPosGen= this->theOwner->posGenerators.TheObjects[inputGeneratorSequence.TheObjects[i]];
    ElementSimpleLieAlgebra& currentNegGen=this->theOwner->negGenerators.TheObjects[inputGeneratorSequence.TheObjects[i]];
    this->ClimbDownFromHighestWeightAlongSl2String
    (output, tempElt, tempCoeff, currentPosGen, currentNegGen, inputGeneratorPowers.TheObjects[i], theGlobalVariables);
    outputCoeff*=(tempCoeff);
//    std::cout << "<br><br><br>... and the coefficient currently is: " << outputCoeff.ElementToString();
    output=tempElt;
  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ClearDenominators
  (CoefficientType& outputWasMultipliedByThis)
{ outputWasMultipliedByThis=this->theOwner->theRingUnit;
  CoefficientType CurrentCoeff;
  for (int i=0; i<this->leftComponents.size; i++)
  { ElementVermaModuleOrdered<CoefficientType>& currentElt=this->leftComponents.TheObjects[i];
    currentElt.ClearDenominators(CurrentCoeff, this->theOwner->theRingUnit);
    for (int j=0; j<this->leftComponents.size; j++)
      if (i!=j)
        this->leftComponents.TheObjects[j]*=CurrentCoeff;
    outputWasMultipliedByThis*=CurrentCoeff;
  }
}

void RationalFunction::ClearDenominators
(RationalFunction& outputWasMultipliedBy)
{ //outputWasMultipliedBy.MakeNVarConst(this->NumVars, (Rational) 1, this->context);
  Rational tempRat;
  switch(this->expressionType)
  { case RationalFunction::typeRational:
      this->ratValue.GetDen(tempRat);
      outputWasMultipliedBy.MakeNVarConst(this->NumVars, tempRat, this->context);
      this->ratValue*=tempRat;
    break;
    case RationalFunction::typePoly:
      this->Numerator.GetElement().ClearDenominators(tempRat);
      outputWasMultipliedBy.MakeNVarConst(this->NumVars, tempRat, this->context);
    break;
    case RationalFunction::typeRationalFunction:
      RationalFunction tempRF;
      outputWasMultipliedBy.operator=(this->Denominator.GetElement());
      *this*=outputWasMultipliedBy;
      this->ClearDenominators(tempRF);
      outputWasMultipliedBy*=tempRF;
      break;
  }
}

template <class CoefficientType>
bool ElementGeneralizedVerma<CoefficientType>::ActOnMe
  (const ElementUniversalEnvelopingOrdered<CoefficientType>& theElt, ElementGeneralizedVerma<CoefficientType>& output, GlobalVariables* theContext)
{ ElementGeneralizedVerma<CoefficientType> Accum, tempElt;
  Accum.Nullify(*this->theOwner);
  for (int i=0; i<theElt.size; i++)
  { MonomialUniversalEnvelopingOrdered<CoefficientType>& currentMon=theElt.TheObjects[i];
    tempElt=*this;
    for (int j=currentMon.generatorsIndices.size-1; j>=0 ; j--)
    { int thePower;
      if (!currentMon.Powers.TheObjects[j].IsSmallInteger(thePower))
        return false;
      ElementSimpleLieAlgebra& currentLieElt=this->theOwner->theOwner.theOrder.TheObjects[currentMon.generatorsIndices.TheObjects[j]];
      for (int k=0; k<thePower; k++)
        tempElt.ActOnMe(currentLieElt, theContext);
    }
    tempElt*=currentMon.Coefficient;
    Accum+=tempElt;
  }
  output=Accum;
  return true;
}

bool ElementSimpleLieAlgebra::ElementToStringNeedsBracketsForMultiplication()const
{ return this->Hcomponent.GetNumNonZeroCoords()+this->NonZeroElements.CardinalitySelection>1;
}

template<class CoefficientType>
bool ElementVermaModuleOrdered<CoefficientType>::ElementToStringNeedsBracketsForMultiplication()const
{ return this->theElT.ElementToStringNeedsBracketsForMultiplication();
}
