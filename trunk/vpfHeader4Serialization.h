//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader4_h_already_included
#define vpfHeader4_h_already_included

#include "vpfHeader2.h"
ProjectInformationInstance ProjectInfoVpfHeader4Serialization
(__FILE__, "Header file: cpp to expression serialization. ");

class Serialization
{
public:
static bool Serialize(const SemisimpleSubalgebras& input, Expression& output, CommandList& theCommands);
static bool Serialize(const CandidateSSSubalgebra& input, Expression& output, CommandList& theCommands);
static bool Serialize(const Rational& input, Expression& output, CommandList& theCommands)
{ return output.AssignValue(input, theCommands);
}
template <typename TemplateMonomial, typename CoefficientType>
static bool Serialize
(const MonomialCollection<TemplateMonomial, CoefficientType>& input,
 Expression& output, CommandList& theCommands)
{ MacroRegisterFunctionWithName("Serialization::Serialize");
  output.reset(theCommands);
  Expression tempE, coeffExpr, monExpr;
  tempE.MakeAtom(theCommands.opMonomialCollection(), theCommands);
  output.AddChildOnTop(tempE);
  coeffExpr.reset(theCommands);
  monExpr.reset(theCommands);
  tempE.MakeAtom(theCommands.opSequence(), theCommands);
  coeffExpr.AddChildOnTop(tempE);
  monExpr.AddChildOnTop(tempE);
  for (int i=0; i<input.size; i++)
  { if (!Serialization::Serialize(input[i], tempE, theCommands))
      return false;
    else
      monExpr.AddChildOnTop(tempE);
    if (!Serialization::Serialize(input.theCoeffs[i], tempE, theCommands))
      return false;
    else
      coeffExpr.AddChildOnTop(tempE);
  }
  output.AddChildOnTop(monExpr);
  output.AddChildOnTop(coeffExpr);
  return true;
}
template <class Object>
static bool Serialize(Vector<Object>& input, Expression& output, CommandList& theCommands)
{ MacroRegisterFunctionWithName("Serialization::Serialize");
  output.reset(theCommands, 1);
  output.AssignChildAtomValue(0, theCommands.opSequence(), theCommands);
  Expression tempE;
  for (int i=0; i<input.size; i++)
    if (!Serialization::Serialize(input[i], tempE, theCommands))
      return false;
    else
      output.AddChildOnTop(tempE);
  return true;
}
template <class Object>
static bool Serialize(const List<Object>& input, Expression& output, CommandList& theCommands)
{ MacroRegisterFunctionWithName("Serialization::Serialize");
  output.reset(theCommands, 1);
  output.AssignChildAtomValue(0, theCommands.opSequence(), theCommands);
  Expression tempE;
  for (int i=0; i<input.size; i++)
    if (!Serialization::Serialize(input[i], tempE, theCommands))
      return false;
    else
      output.AddChildOnTop(tempE);
  return true;
}
static bool Serialize
(const DynkinSimpleType& input, Expression& output, CommandList& theCommands);
static bool Serialize(const WeylGroup& input, Expression& output, CommandList& theCommands);
};

bool Serialization::Serialize
(const DynkinSimpleType& input, Expression& output, CommandList& theCommands)
{ MacroRegisterFunctionWithName("Serialization::Serialize");
  output.reset(theCommands);
  std::string theType;
  theType=input.theLetter;
  Expression typeE, tempE;
  tempE.MakeAtom(theCommands.opThePower(), theCommands);
  output.AddChildOnTop(tempE);
  typeE.reset(theCommands);
  tempE.MakeAtom
  (theCommands.operationS.AddNoRepetitionOrReturnIndexFirst(theType), theCommands);
  typeE.AddChildOnTop(tempE);
  tempE.AssignValue(input.theRank, theCommands);
  typeE.AddChildOnTop(tempE);
  typeE.format=typeE.formatFunctionUseUnderscore;
  output.AddChildOnTop(typeE);
  tempE.AssignValue(input.lengthFirstCoRootSquared, theCommands);
  output.AddChildOnTop(tempE);
  return true;
}

bool Serialization::Serialize(const SemisimpleSubalgebras& input, Expression& output, CommandList& theCommands)
{ MacroRegisterFunctionWithName("Serialization::Serialize");
  if (input.owneR==0)
  { std::cout << "This is a programming error: use of non-initialized Semisimple subalgebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output.reset(theCommands);
  Expression tempE;
  tempE.MakeAtom(theCommands.opSemisimpleSubalgebras(), theCommands);
  output.AddChildOnTop(tempE);
  tempE.AssignValue(*input.owneR, theCommands);
  output.AddChildOnTop(tempE);
//  SltwoSubalgebras theSl2s;
//  List<SemisimpleLieAlgebra> SimpleComponentsSubalgebras;
//  HashedListReferences<SemisimpleLieAlgebra> theSubalgebrasNonEmbedded;
//  List<SltwoSubalgebras> theSl2sOfSubalgebras;
  Serialization::Serialize(input.Hcandidates, tempE, theCommands);
  output.AddChildOnTop(tempE);
//  int theRecursionCounter;
  return true;
}

bool Serialization::Serialize
(const WeylGroup& theWeyl, Expression& output, CommandList& theCommands)
{ output.reset(theCommands);
  Expression tempE;
  tempE.MakeAtom(theCommands.opSemisimpleSubalgebras(), theCommands);
  output.AddChildOnTop(tempE);
  Serialization::Serialize(theWeyl.theDynkinType, tempE, theCommands);
  output.AddChildOnTop(tempE);
  return true;
}

bool Serialization::Serialize(const CandidateSSSubalgebra& input, Expression& output, CommandList& theCommands)
{ output.reset(theCommands);
  Expression tempE;
  tempE.MakeAtom(theCommands.opCandidateSSsubalgebra(), theCommands);
  output.AddChildOnTop(tempE);
  Serialization::Serialize(input.owner->GetSSowner().theWeyl, tempE, theCommands);
  output.AddChildOnTop(tempE);
  Serialization::Serialize(input.theWeylNonEmbeddeD, tempE, theCommands);
  output.AddChildOnTop(tempE);
  Serialization::Serialize(input.theWeylNonEmbeddeD, tempE, theCommands);
  output.AddChildOnTop(tempE);
  Serialization::Serialize(input.theWeylNonEmbeddeDdefaultScale, tempE, theCommands);
  output.AddChildOnTop(tempE);
  Serialization::Serialize(input.CartanSAsByComponent, tempE, theCommands);
  output.AddChildOnTop(tempE);

  //Vectors<Rational> theCoRoots;
  //Vectors<Rational> theHs;
/*  List<DynkinSimpleType> theTypes;
  List<ElementSemisimpleLieAlgebra<Rational> > thePosGens;
  List<ElementSemisimpleLieAlgebra<Rational> > theNegGens;
  List<ElementSemisimpleLieAlgebra<Polynomial<Rational> > > theUnknownPosGens;
  List<ElementSemisimpleLieAlgebra<Polynomial<Rational> > > theUnknownNegGens;

  List<List<int> > theHorbitIndices;
//  List<List<ElementWeylGroup> > theHWeylGroupElts;
  Vector<Rational> aSolution;
  List<List<ChevalleyGenerator> > theInvolvedPosGenerators;
  List<List<ChevalleyGenerator> > theInvolvedNegGenerators;
  DynkinType theTypeTotal;
  charSSAlgMod<Rational> theCharFundamentalCoordsRelativeToCartan;
  charSSAlgMod<Rational> theCharFundCoords;
  Vectors<Rational> PosRootsPerpendicularPrecedingWeights;
  List<Polynomial<Rational> > theSystemToSolve;
  List<Polynomial<Rational> > transformedSystem;
  FormatExpressions theCoeffLetters;
  SemisimpleSubalgebras* owner;
  int indexInOwnersOfNonEmbeddedMe;
  bool flagSystemSolved;
  bool flagSystemProvedToHaveNoSolution;
  bool flagSystemGroebnerBasisFound;
  int totalNumUnknowns;*/
  return true;
}

bool CommandList::innerSerialize
  (CommandList& theCommands, const Expression& input, Expression& output)
{ int theType;
  if (!input.IsBuiltInType(&theType))
    return false;
  if (theType==theCommands.opSemisimpleSubalgebras())
    return Serialization::Serialize(input.GetValuE<SemisimpleSubalgebras>(), output, theCommands);
  return output.SetError("Serialization not implemented for this data type.", theCommands);
}

bool CommandList::innerDeSerialize
  (CommandList& theCommands, const Expression& input, Expression& output)
{
  return false;
}
#endif
