//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"


ProjectInformationInstance ProjectInfoVpf6cpp(__FILE__, "Calculator parser, implementation. ");

//If you get a specialization after instantiation error:
//the following template specialization funcitons must appear
//here and nowhere else (discovered through extremely painful experimentation).

template<>
typename List<Expression>::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<Expression>()
{ return 0;
}

//Expression::GetTypeOperation specializations follow
template < >
int Expression::GetTypeOperation<RationalFunctionOld>()const
{ this->CheckInitialization();
  return this->theBoss->opRationalFunction();
}

template < >
int Expression::GetTypeOperation<Rational>()const
{ this->CheckInitialization();
  return this->theBoss->opRational();
}

template < >
int Expression::GetTypeOperation<std::string>()const
{ this->CheckInitialization();
  return this->theBoss->opString();
}

template < >
int Expression::GetTypeOperation<AlgebraicNumber>()const
{ this->CheckInitialization();
  return this->theBoss->opAlgNumber();
}

template < >
int Expression::GetTypeOperation<ElementZmodP>()const
{ this->CheckInitialization();
  return this->theBoss->opEltZmodP();
}

template < >
int Expression::GetTypeOperation<ElementUniversalEnveloping<RationalFunctionOld> >()const
{ this->CheckInitialization();
  return this->theBoss->opElementUEoverRF();
}

template < >
int Expression::GetTypeOperation<Matrix<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opMatRat();
}

template < >
int Expression::GetTypeOperation<MatrixTensor<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opMatTensorRat();
}

template < >
int Expression::GetTypeOperation<Matrix<RationalFunctionOld> >()const
{ this->CheckInitialization();
  return this->theBoss->opMatRF();
}

template < >
int Expression::GetTypeOperation<int>()const
{ this->CheckInitialization();
  return this->theBoss->opRational();
}

template < >
int Expression::GetTypeOperation<double>()const
{ this->CheckInitialization();
  return this->theBoss->opDouble();
}

template < >
int Expression::GetTypeOperation<Polynomial<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opPoly();
}

template < >
int Expression::GetTypeOperation<Polynomial<AlgebraicNumber> >()const
{ this->CheckInitialization();
  return this->theBoss->opPolyOverANs();
}

template < >
int Expression::GetTypeOperation<ElementWeylAlgebra<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opElementWeylAlgebra();
}

template < >
int Expression::GetTypeOperation<SemisimpleLieAlgebra>()const
{ this->CheckInitialization();
  return this->theBoss->opSSLieAlg();
}

template < >
int Expression::GetTypeOperation<LittelmannPath>()const
{ this->CheckInitialization();
  return this->theBoss->opLittelmannPath();
}

template < >
int Expression::GetTypeOperation<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >()const
{ this->CheckInitialization();
  return this->theBoss->opLRO();
}

template < >
int Expression::GetTypeOperation<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()const
{ this->CheckInitialization();
  return this->theBoss->opElementTensorGVM();
}

template < >
int Expression::GetTypeOperation<charSSAlgMod<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opCharSSAlgMod();
}

template < >
int Expression::GetTypeOperation<CalculusFunctionPlot>()const
{ this->CheckInitialization();
  return this->theBoss->opCalculusPlot();
}

template < >
int Expression::GetTypeOperation<SemisimpleSubalgebras>()const
{ this->CheckInitialization();
  return this->theBoss->opSemisimpleSubalgebras();
}

template < >
int Expression::GetTypeOperation<WeylGroup>()const
{ this->CheckInitialization();
  return this->theBoss->opWeylGroup();
}

template < >
int Expression::GetTypeOperation<WeylGroupRepresentation<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opWeylGroupRep();
}

template < >
int Expression::GetTypeOperation<ElementWeylGroup>()const
{ this->CheckInitialization();
  return this->theBoss->opWeylGroupElement();
}

template < >
int Expression::GetTypeOperation<WeylGroupVirtualRepresentation>()const
{ this->CheckInitialization();
  return this->theBoss->opWeylGroupVirtualRep();
}

//Expression::GetTypeOperation specializations end.

//Expression::AddObjectReturnIndex specializations follow
template < >
int Expression::AddObjectReturnIndex(const
SemisimpleSubalgebras
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theSSsubalgebras
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
std::string
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theStrings
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Rational
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theRationals
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
double
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theDoubles
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
RationalFunctionOld
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theRFs
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementUniversalEnveloping<RationalFunctionOld>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theUEs
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
int
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theRationals
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Polynomial<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.thePolys
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Polynomial<AlgebraicNumber>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.thePolysOverANs
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementWeylAlgebra<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeylAlgebraElements
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
SemisimpleLieAlgebra
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theLieAlgebras
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
AlgebraicNumber
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theAlgebraicNumbers
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementZmodP
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theEltsModP
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
MonomialTensor<int, MathRoutines::IntUnsignIdentity>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theLittelmannOperators
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementTensorsGeneralizedVermas<RationalFunctionOld>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theTensorElts
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
charSSAlgMod<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theCharsSSLieAlgFD
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
LittelmannPath
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theLSpaths
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Matrix<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theMatRats
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
MatrixTensor<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theMatTensorRats
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Matrix<RationalFunctionOld>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theMatRFs
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
CalculusFunctionPlot
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.thePlots
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
WeylGroup
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeylGroups
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
WeylGroupRepresentation<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeylGroupReps
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
WeylGroupVirtualRepresentation
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeylGroupVirtualReps
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementWeylGroup
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeylGroupElements
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

//Expression::AddObjectReturnIndex specializations end

//start Expression::GetValueNonConst specializations.
template < >
Rational& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Rational>())
    crash << "This is a programming error: expression not of required type Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theRationals.GetElement(this->GetLastChild().theData);
}

template < >
ElementZmodP& Expression::GetValueNonConst()const
{ if (!this->IsOfType<ElementZmodP>())
    crash << "This is a programming error: expression not of required type ElementZmodP. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theEltsModP.GetElement(this->GetLastChild().theData);
}

template < >
AlgebraicNumber& Expression::GetValueNonConst()const
{ if (!this->IsOfType<AlgebraicNumber>())
    crash << "This is a programming error: expression not of required type AlgebraicNumber. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theAlgebraicNumbers.GetElement(this->GetLastChild().theData);
}

template < >
double& Expression::GetValueNonConst()const
{ if (!this->IsOfType<double>())
    crash << "This is a programming error: expression not of required type double. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theDoubles.GetElement(this->GetLastChild().theData);
}

template < >
std::string& Expression::GetValueNonConst()const
{ if (!this->IsOfType<std::string>())
    crash << "This is a programming error: expression not of required type std::string. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theStrings.GetElement(this->GetLastChild().theData);
}

template < >
RationalFunctionOld& Expression::GetValueNonConst()const
{ if (!this->IsOfType<RationalFunctionOld>())
    crash << "This is a programming error: expression not of required type RationalFunctionOld. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theRFs.GetElement(this->GetLastChild().theData);
}

template < >
ElementUniversalEnveloping<RationalFunctionOld>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
    crash << "This is a programming error: expression not of required type ElementUniversalEnveloping_RationalFunctionOld. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theUEs.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Polynomial<Rational> >())
    crash << "This is a programming error: expression not of required type Polynomial_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.thePolys.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<AlgebraicNumber>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Polynomial<AlgebraicNumber> >())
    crash << "This is a programming error: expression not of required type Polynomial_AlgebraicNumber. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.thePolysOverANs.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylAlgebra<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<ElementWeylAlgebra<Rational> >())
    crash << "This is a programming error: expression not of required type ElementWeylAlgebra_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylAlgebraElements.GetElement(this->GetLastChild().theData);
}

template < >
LittelmannPath& Expression::GetValueNonConst()const
{ if (!this->IsOfType<LittelmannPath>())
    crash << "This is a programming error: expression not of required type LittelmannPath. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theLSpaths.GetElement(this->GetLastChild().theData);
}

template < >
MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >())
    crash << "This is a programming error: expression not of required type MonomialTensor. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theLittelmannOperators.GetElement(this->GetLastChild().theData);
}

template < >
ElementTensorsGeneralizedVermas<RationalFunctionOld>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    crash << "This is a programming error: expression not of required type ElementTGVM_RationalFunctionOld. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theTensorElts.GetElement(this->GetLastChild().theData);
}

template < >
charSSAlgMod<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<charSSAlgMod<Rational> >())
    crash << "This is a programming error: expression not of required type charSSAlgMod_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theCharsSSLieAlgFD.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleLieAlgebra& Expression::GetValueNonConst()const
{ if (!this->IsOfType<SemisimpleLieAlgebra>())
    crash << "This is a programming error: expression not of required type SemisimpleLieAlgebra. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theLieAlgebras.GetElement(this->GetLastChild().theData);
}

template < >
Matrix<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Matrix<Rational> >())
    crash << "This is a programming error: expression not of required type Matrix_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theMatRats.GetElement(this->GetLastChild().theData);
}

template < >
MatrixTensor<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<MatrixTensor<Rational> >())
    crash << "This is a programming error: expression not of required type MatrixTensor_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theMatTensorRats.GetElement(this->GetLastChild().theData);
}

template < >
Matrix<RationalFunctionOld>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Matrix<RationalFunctionOld> >())
    crash << "This is a programming error: expression not of required type Matrix_RF. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theMatRFs.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleSubalgebras& Expression::GetValueNonConst()const
{ if (!this->IsOfType<SemisimpleSubalgebras>())
    crash << "This is a programming error: expression not of required type SemisimpleSubalgebras. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theSSsubalgebras[this->GetLastChild().theData];
}

template < >
CalculusFunctionPlot& Expression::GetValueNonConst()const
{ if (!this->IsOfType<CalculusFunctionPlot>())
    crash << "This is a programming error: expression not of required type CalculusFunctionPlot. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.thePlots[this->GetLastChild().theData];
}

template < >
WeylGroup& Expression::GetValueNonConst()const
{ if (!this->IsOfType<WeylGroup>())
    crash << "This is a programming error: expression not of required type WeylGroup. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylGroups[this->GetLastChild().theData];
}

template < >
WeylGroupRepresentation<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<WeylGroupRepresentation<Rational> >())
    crash << "This is a programming error: expression not of required type WeylGroupRepresentation_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylGroupReps.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylGroup& Expression::GetValueNonConst()const
{ if (!this->IsOfType<ElementWeylGroup>())
    crash << "This is a programming error: expression not of required type ElementWeylGroup. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylGroupElements.GetElement(this->GetLastChild().theData);
}

template < >
WeylGroupVirtualRepresentation& Expression::GetValueNonConst()const
{ if (!this->IsOfType<WeylGroupVirtualRepresentation>())
    crash << "This is a programming error: expression not of required type WeylGroupVirtualRepresentation. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylGroupVirtualReps.GetElement(this->GetLastChild().theData);
}

//end Expression::GetValueNonConst specializations.
//start Expression::ConvertToType specializations.
template< >
bool Expression::ConvertToType<Polynomial<AlgebraicNumber> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_Polynomial_AlgebraicNumber");
  this->CheckInitialization();
  if (this->IsOfType<Rational>())
  { Polynomial<AlgebraicNumber> resultP;
    resultP.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<AlgebraicNumber>())
  { Polynomial<AlgebraicNumber> resultP;
    resultP.MakeConst(this->GetValue<AlgebraicNumber>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { Polynomial<AlgebraicNumber> resultP;
    resultP=this->GetValue<Polynomial<Rational> >();
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<AlgebraicNumber> >())
  { output=*this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<Polynomial<Rational> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_Polynomial_Rational");
  this->CheckInitialization();
  if (this->IsOfType<Rational>())
  { Polynomial<Rational> resultP;
    resultP.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { output=*this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<RationalFunctionOld>(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<Rational>())
  { RationalFunctionOld resultRF;
    resultRF.MakeConst(this->GetValue<Rational>(), this->theBoss->theGlobalVariableS);
    return output.AssignValueWithContext(resultRF, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { RationalFunctionOld resultRF;
    resultRF.context=this->theBoss->theGlobalVariableS;
    resultRF=this->GetValue<Polynomial<Rational> >();
    return output.AssignValueWithContext(resultRF, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<RationalFunctionOld>())
  { output=*this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<ElementWeylAlgebra<Rational> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_ElementWeylAlgebra");
  this->CheckInitialization();
  if (this->IsOfType<Rational>())
  { ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.MakeConst(this->GetValue<Rational>());
    return output.AssignValueWithContext(resultEWA, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { ElementWeylAlgebra<Rational> resultEWA;
    resultEWA.AssignPolynomial(this->GetValue<Polynomial<Rational> >());
    return output.AssignValueWithContext(resultEWA, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<ElementWeylAlgebra<Rational> >())
  { output=*this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  //std::cout << "<hr>convert to ue called on: " << this->ToString();
  this->CheckInitialization();
  if (!this->IsBuiltInType())
    return false;
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { output=*this;
    return true;
  }
//  std::cout << "<hr>Getting ue from: " << this->ToString();
  SemisimpleLieAlgebra* theOwner=this->GetAmbientSSAlgebraNonConstUseWithCaution();
  if (theOwner==0)
  { this->theBoss->Comments << "<hr>Failed to convert " << this->ToString() << " (Lispified: " << this->ToStringFull()
    << ") to element of universal enveloping -  failed to extract ambient Lie algebra. ";
    return false;
  }
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  if (this->IsOfType<Rational>())
  { outputUE.MakeConst(this->GetValue<Rational>(), *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->theBoss);
  } else if (this->IsOfType<Polynomial<Rational> >())
  { RationalFunctionOld tempRF;
    tempRF=this->GetValue<Polynomial<Rational> >();
    outputUE.MakeConst(tempRF, *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->theBoss);
  } else if (this->IsOfType<RationalFunctionOld>())
  { outputUE.MakeConst(this->GetValue<RationalFunctionOld>(), *theOwner);
    return output.AssignValueWithContext(outputUE, this->GetContext(), *this->theBoss);
  }
  this->theBoss->Comments << "<hr>Failed to convert " << this->ToString() << " to element of universal enveloping -  I don't know how. ";
  return false;
}

template< >
bool Expression::ConvertToType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { output=*this;
    return true;
  }
  return false;
}
//end Expression::ConvertToType specializations.

bool Expression::CheckConsistency()const
{ MacroRegisterFunctionWithName("Expression::CheckConsistency");
  //warning: do not use ToString method from here: ToString itself calls CheckConosistency, so that causes an "infinite" recursion call cycle,
  //i.e., stack overflow.
  if (this->flagDeallocated)
    crash << "Use after free of Expression. " << crash;
  this->CheckInitialization();
  if (this->children.size==0)
    return false;
  if (this->IsBuiltInType())
  { if (this->children.size!=3)
      crash << "This is a programming error. At the moment of writing, an expression of built-in type must have 3 "
      << "children: type, context, and index in Calculator. The expression is " << this->ToStringFull()
      << crash;
    const Expression& mustBeTheContext=(*this)[1];
    if (!mustBeTheContext.IsListNElementsStartingWithAtom(this->theBoss->opContexT()))
      crash << "This is a programming error. At the moment of writing, the second child of a built-in type must be a context. It is instead "
      << mustBeTheContext.ToStringFull() << crash;
    for (int i=1; i<mustBeTheContext.children.size; i++)
    { bool isGood=false;
      const Expression& currentE=mustBeTheContext[i];
      if (currentE.IsListNElementsStartingWithAtom(this->theBoss->opPolynomialVariables()))
        isGood=true;
      if (currentE.IsListNElementsStartingWithAtom(this->theBoss->opSSLieAlg()))
        isGood=true;
      if (currentE.IsListNElementsStartingWithAtom(this->theBoss->opWeylAlgebraVariables()))
        isGood=true;
      if (!isGood)
        crash << "This is a programming error. The context " << mustBeTheContext.ToStringFull() << " has an entry which I do not recognize, namely, "
        << currentE.ToStringFull() << ". " << crash;
    }
  }
  return true;
}

const Expression& Expression::operator[](int n)const
{ this->CheckInitialization();
  int childIndex=this->children[n];
  if (childIndex<0)
    crash << "<hr>This is a programming error: the child of position " << n << " out of " << this->children.size-1
    << " is not contained in the expression container. " << crash;
  return this->theBoss->theExpressionContainer[childIndex];
}

bool Expression::AddChildOnTop(const Expression& inputChild)
{ this->CheckInitialization();
  this->children.AddOnTop(this->theBoss->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(inputChild));
  return true;
}

bool Expression::SetChildAtomValue(int childIndex, int TheAtomValue)
{ this->CheckInitialization();
  Expression tempE;
  tempE.MakeAtom(TheAtomValue, *this->theBoss);
  this->children.SetObjectAtIndex
  (childIndex, this->theBoss->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(tempE));
  return true;
}

bool Expression::SetChilD(int childIndexInMe, const Expression& inputChild)
{ this->CheckInitialization();
  int theIndexOfTheExpression=this->theBoss->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(inputChild);
  this->children.SetObjectAtIndex(childIndexInMe, theIndexOfTheExpression);
  return true;
}

bool Expression::AddChildAtomOnTop(const std::string& theOperationString)
{ this->CheckInitialization();
  return this->AddChildAtomOnTop
  (this->theBoss->AddOperationNoRepetitionOrReturnIndexFirst(theOperationString));
}

bool Expression::IsListNElementsStartingWithAtom(int theOp, int N)const
{ if (N!=-1)
  { if (this->children.size!=N)
      return false;
  } else
    if (this->children.size==0)
      return false;
  if (!(*this)[0].IsAtom())
    return false;
  if (theOp==-1)
    return true;
  return (*this)[0].theData==theOp;
}

int Expression::ContextGetNumContextVariables()const
{ if (this->theBoss==0)
    return 0;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opContexT()))
    return 0;
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opPolynomialVariables()))
      return (*this)[i].children.size-1;
  return 0;
}

bool Expression::SetContextAtLeastEqualTo(Expression& inputOutputMinContext)
{ MacroRegisterFunctionWithName("Expression::SetContextAtLeastEqualTo");
  this->CheckInitialization();
  if (!this->IsBuiltInType())
    crash << "This is a programming error: calling Expression::SetContextAtLeastEqualTo on an expression that is not of built-in type. "
    << "Contexts are reserved for built-in data types. " << crash;
  if (!inputOutputMinContext.IsContext())
  { this->theBoss->Comments << "<br>Warning: non-initialized input context in Expression::SetContextAtLeastEqualTo. Stack trace: "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    inputOutputMinContext.MakeEmptyContext(*this->theBoss);
  }
//  std::cout << "<br>Setting context at least equal to " << inputOutputMinContext.ToString()
//  << " in expression " << this->ToString();
  Expression newContext;
  Expression myOldContext=this->GetContext();
//  std::cout << "<br>In " << this->ToString() << ", merging my old context, " << myOldContext.Lispify() << " and the input context, " << inputOutputMinContext.Lispify();
  if (!this->ContextMergeContexts(myOldContext, inputOutputMinContext, newContext))
    return false;
  if (myOldContext==newContext)
    return true;
  inputOutputMinContext=newContext;
  if (this->IsOfType<Rational>())
  { this->SetChilD(1, inputOutputMinContext);
//    std::cout << "<hr>Context of rational set; rational is: " << this->ToString();
    return true;
  }
  if (this->IsOfType<AlgebraicNumber>())
    return this->SetChilD(1, inputOutputMinContext);
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> > ())
  { ElementUniversalEnveloping<RationalFunctionOld> newUE=this->GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<Rational>(newContext, subPolyPart);
    newUE.Substitution(subPolyPart);
    return this->AssignValueWithContext(newUE, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { Polynomial<Rational> newPoly=this->GetValue<Polynomial<Rational> >();
    //std::cout << "<br>Subbing " << polySub.ToString() << " in "
    PolynomialSubstitution<Rational> subPolyPart;
    //<< newPoly.ToString();
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<Rational>(newContext, subPolyPart);
    if (!newPoly.Substitution(subPolyPart))
      return false;
    return this->AssignValueWithContext(newPoly, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<Polynomial<AlgebraicNumber> >())
  { Polynomial<AlgebraicNumber> newPoly=this->GetValue<Polynomial<AlgebraicNumber> >();
    //std::cout << "<br>Subbing " << polySub.ToString() << " in "
    PolynomialSubstitution<AlgebraicNumber> subPolyPart;
    //<< newPoly.ToString();
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<AlgebraicNumber>(newContext, subPolyPart);
    if (!newPoly.Substitution(subPolyPart))
      return false;
    return this->AssignValueWithContext(newPoly, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<ElementWeylAlgebra<Rational> >())
  { PolynomialSubstitution<Rational> subEWApart;
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolyAndEWASubFromSuperContextNoFailure(newContext, subPolyPart, subEWApart);
    ElementWeylAlgebra<Rational> outputEWA =this->GetValue<ElementWeylAlgebra<Rational> >();
//    std::cout << "<hr>Subbing in: " << outputEWA.ToString();
    if (!outputEWA.Substitution(subPolyPart, subEWApart))
    { this->theBoss->Comments << "<hr>Failed to convert " << outputEWA.ToString() << ": failed to carry out substitution "
      << subEWApart.ToString() << ", " << subPolyPart.ToString();
      return false;
    }
//    std::cout << "... to get: " << outputEWA.ToString();
    return this->AssignValueWithContext(outputEWA, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<RationalFunctionOld>())
  { RationalFunctionOld newRF=this->GetValue<RationalFunctionOld>();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure(newContext, subPolyPart);
    newRF.Substitution(subPolyPart);
    return this->AssignValueWithContext(newRF, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { ElementTensorsGeneralizedVermas<RationalFunctionOld> newETGV=this->GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
    PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure(newContext, subPolyPart);
    newETGV.Substitution(subPolyPart, this->theBoss->theObjectContainer.theCategoryOmodules);
    return this->AssignValueWithContext(newETGV, inputOutputMinContext, *this->theBoss);
  }
  this->theBoss->Comments << "Expression " << this->ToString() << " is of built-in type but is not handled by Expression::SetContextAtLeastEqualTo";
  return false;
}

bool Expression::MergeContexts(Expression& leftE, Expression& rightE)
{ Expression leftC=leftE.GetContext();
  Expression rightC=rightE.GetContext();
  Expression outputC;
  if (!leftC.ContextMergeContexts(leftC, rightC, outputC))
    return false;
  if (!leftE.SetContextAtLeastEqualTo(outputC))
    return false;
  return rightE.SetContextAtLeastEqualTo(outputC);
}

Expression Expression::ContextGetContextVariable(int variableIndex)const
{ Expression thePolVars=this->ContextGetPolynomialVariables();
  if (thePolVars.children.size<=variableIndex+1)
  { Expression errorE;
    std::stringstream out;
    out << "Context ~does ~not ~have ~variable ~index ~" << variableIndex+1 << ". ";
    errorE.SetError(out.str(), *this->theBoss);
    return errorE;
  }
  return thePolVars[variableIndex+1];
}

bool Expression::ContextSetSSLieAlgebrA(int indexInOwners, Calculator& owner)
{ if (!this->IsContext())
    crash << "This is a programming error: calling Expression::ContextSetSSLieAlgebrA on a non-context expression. " << crash;
  int index=this->ContextGetIndexAmbientSSalg();
  if (index!=-1 && index!=indexInOwners)
    return false;
  if (index==indexInOwners)
    return true;
  Expression LieAlgContextE;
  Expression emptyContext;
  emptyContext.MakeEmptyContext(owner);
  LieAlgContextE.reset(owner, 3);
  LieAlgContextE.AddChildAtomOnTop(owner.opSSLieAlg());
  LieAlgContextE.AddChildOnTop(emptyContext);
  LieAlgContextE.AddChildAtomOnTop(indexInOwners);
  return this->AddChildOnTop(LieAlgContextE);
}

bool Expression::ContextSetDiffOperatorVar(const Expression& thePolyVar, const Expression& theDiffOpVar)
{ if (!this->IsContext())
    crash << "This is a programming error: calling Expression::ContextSetDiffOperatorVar on a non-context expression. " << crash;
  Expression diffVarsE, polyVarsE;
  diffVarsE.reset(*this->theBoss, 2);
  diffVarsE.AddChildAtomOnTop(this->theBoss->opWeylAlgebraVariables());
  diffVarsE.AddChildOnTop(theDiffOpVar);
  polyVarsE.reset(*this->theBoss, 2);
  polyVarsE.AddChildAtomOnTop(this->theBoss->opPolynomialVariables());
  polyVarsE.AddChildOnTop(thePolyVar);
  bool foundDiffVarsE=false;
  bool foundPolyVarsE=false;
  for (int i=0; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opWeylAlgebraVariables()))
    { this->SetChilD(i, diffVarsE);
      foundDiffVarsE=true;
    } else if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opPolynomialVariables()))
    { this->SetChilD(i, polyVarsE);
      foundPolyVarsE=true;
    }
  if (!foundPolyVarsE)
    this->AddChildOnTop(polyVarsE);
  if (!foundDiffVarsE)
    this->AddChildOnTop(diffVarsE);
  return true;
}

Expression Expression::ContextGetDifferentialOperatorVariables()const
{ this->CheckInitialization();
  return this->ContextGetContextType(this->theBoss->opWeylAlgebraVariables());
}

Expression Expression::ContextGetPolynomialVariables()const
{ this->CheckInitialization();
  return this->ContextGetContextType(this->theBoss->opPolynomialVariables());
}

Expression Expression::ContextGetContextType(int theType)const
{ MacroRegisterFunctionWithName("Expression::ContextGetPolynomialVariables");
  this->CheckInitialization();
  if (!this->IsContext())
  { //crash << "This is a programming error: calling Expression::ContextGetPolynomialVariables"
    //<< " on a non-context expression. "
    //<< crash;
  }
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(theType))
      return (*this)[i];
  Expression output;
  output.reset(*this->theBoss, 1);
  output.AddChildAtomOnTop(theType);
  return output;
}

bool Expression::ContextMergeContexts(const Expression& leftContext, const Expression& rightContext, Expression& outputContext)
{ MacroRegisterFunctionWithName("Expression::ContextMergeContexts");
  if (&leftContext==&outputContext || &rightContext==&outputContext)
  { Expression leftCopy=leftContext;
    Expression rightCopy=rightContext;
    return Expression::ContextMergeContexts(leftCopy, rightCopy, outputContext);
  }
  int leftSSindex=leftContext.ContextGetIndexAmbientSSalg();
  int rightSSindex=rightContext.ContextGetIndexAmbientSSalg();
//  std::cout << "<br>left, right semisimple Lie algebra indices extracted from contexts " << leftContext.ToString() << " and "
//  << rightContext.ToString() << " are " << leftSSindex << ", " << rightSSindex << ". ";
  if (leftSSindex==-1)
    leftSSindex=rightSSindex;
  if (rightSSindex==-1)
    rightSSindex=leftSSindex;
  if (leftSSindex!=rightSSindex)
    return false;
//  std::cout << "<br>Merging contexts: " << leftContext.ToString() << " and " << rightContext.ToString();
  Expression leftPolyV=leftContext.ContextGetPolynomialVariables();
  Expression rightPolyV=rightContext.ContextGetPolynomialVariables();
//  std::cout << "<br>Merging contexts: " << leftContext.ToString() << " and " << rightContext.ToString();
  HashedList<Expression> polyVarUnion;
  MemorySaving<HashedList<Expression> > EWAVarUnion;
  polyVarUnion.SetExpectedSize(leftPolyV.children.size+rightPolyV.children.size-2);
  for (int i=1; i<leftPolyV.children.size; i++)
    polyVarUnion.AddOnTopNoRepetition(leftPolyV[i]);
  for (int i =1; i<rightPolyV.children.size; i++)
    polyVarUnion.AddOnTopNoRepetition(rightPolyV[i]);
  polyVarUnion.QuickSortAscending();
  Calculator& owner=*leftContext.theBoss;
  outputContext.reset(owner, 5+polyVarUnion.size);
  outputContext.AddChildAtomOnTop(owner.opContexT());
  if (polyVarUnion.size>0)
  { Expression polyVarsE;
    polyVarsE.reset(owner, polyVarUnion.size+1);
    polyVarsE.AddChildAtomOnTop(owner.opPolynomialVariables());
    for (int i=0; i<polyVarUnion.size; i++)
      polyVarsE.AddChildOnTop(polyVarUnion[i]);
    outputContext.AddChildOnTop(polyVarsE);
  }
  //Converting differential operators if needed.
  Expression leftEWAV=leftContext.ContextGetDifferentialOperatorVariables();
  Expression rightEWAV=rightContext.ContextGetDifferentialOperatorVariables();
  if (leftEWAV.children.size>1 || rightEWAV.children.size>1)
  { Selection foundEWAVar;
    List<Expression> EWAVars;
    foundEWAVar.init(polyVarUnion.size);
    EWAVars.SetSize(polyVarUnion.size);
    Expression* currentPolyV=&leftPolyV;
    Expression* currentEWAV=&leftEWAV;
    for (int k=0; k<2; k++, currentPolyV=&rightPolyV, currentEWAV=&rightEWAV)
      for (int i=1; i<currentEWAV->children.size; i++)
      { int theIndex=polyVarUnion.GetIndex((*currentPolyV)[i]);
        if (foundEWAVar.selected[theIndex])
          if ((*currentEWAV)[i]!=EWAVars[theIndex])
          { owner.Comments << "<hr>Failed to merge contexts " << leftContext.ToString() << " and " << rightContext.ToString()
            << " because " << (*currentPolyV)[i].ToString() << " has two different corresponding differential operator variables: "
            << EWAVars[theIndex].ToString() << " and " << (*currentEWAV)[i].ToString();
            return false;
          }
        foundEWAVar.AddSelectionAppendNewIndex(theIndex);
        EWAVars[theIndex]=(*currentEWAV)[i];
      }
    for (int i=0; i<foundEWAVar.MaxSize; i++)
      if (!foundEWAVar.selected[i])
      { Expression currentEWAVar;
        currentEWAVar.reset(owner, 2);
        currentEWAVar.AddChildAtomOnTop(owner.AddOperationNoRepetitionOrReturnIndexFirst("\\partial"));
        Expression indexE;
        indexE.AssignValue(i, owner);
        currentEWAVar.AddChildOnTop(indexE);
        if (EWAVars.Contains(currentEWAVar))
        { owner.Comments << "<hr>Failed to merge contexts " << leftContext.ToString() << " and " << rightContext.ToString()
          << ": " << polyVarUnion[i].ToString() << " had no differential letter assigned to it. I tried to assign automatically  "
          << currentEWAVar.ToString() << " as differential operator letter, but it was already taken. ";
          return false;
        }
      }
    Expression diffVarsE;
    diffVarsE.reset(owner, polyVarUnion.size+1);
    diffVarsE.AddChildAtomOnTop(owner.opWeylAlgebraVariables());
    for (int i=0; i<EWAVars.size; i++)
      diffVarsE.AddChildOnTop(EWAVars[i]);
    outputContext.AddChildOnTop(diffVarsE);
  }
  if (leftSSindex!=-1)
  { Expression ssAlgE;
    Expression emptyContext;
    emptyContext.MakeEmptyContext(owner);
    ssAlgE.reset(owner, 2);
    ssAlgE.AddChildAtomOnTop(owner.opSSLieAlg());
    ssAlgE.AddChildOnTop(emptyContext);
    ssAlgE.AddChildAtomOnTop(leftSSindex);
    outputContext.AddChildOnTop(ssAlgE);
  }
//  std::cout << "<br>The result of the merge is: " << outputContext.ToString();
  return true;
}

void Expression::ContextGetFormatExpressions(FormatExpressions& output)const
{ output.polyAlphabeT.SetSize(0);
  output.polyDefaultLetter="x";
  if (this->theBoss==0)
    return;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opContexT()))
    return;
  Expression thePolyE=this->ContextGetPolynomialVariables();
  output.polyAlphabeT.SetSize(thePolyE.children.size-1);
  for (int i=1; i<thePolyE.children.size; i++)
    output.polyAlphabeT[i-1]=thePolyE[i].ToString();
  Expression theEWAE=this->ContextGetDifferentialOperatorVariables();
  output.weylAlgebraLetters.SetSize(theEWAE.children.size-1);
  for (int i=1; i<theEWAE.children.size; i++)
    output.weylAlgebraLetters[i-1]=theEWAE[i].ToString();
}

bool Expression::IsContext()const
{ if (this->theBoss==0)
    return false;
  return this->IsListNElementsStartingWithAtom(this->theBoss->opContexT());
}

bool Expression::IsError(std::string* outputErrorMessage)const
{ if (this->theBoss==0)
    return false;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opError(), 2))
    return false;
  return (*this)[1].IsOfType<std::string>(outputErrorMessage);
}

bool Expression::IsSequenceNElementS(int N)const
{ if (this->theBoss==0)
    return false;
  return this->IsListNElementsStartingWithAtom(this->theBoss->opSequence(), N+1);
}

bool Expression::IsEqualToOne()const
{ int theInt;
  if (this->IsSmallInteger(&theInt))
    return theInt==1;
  return false;
}

bool Expression::IsEqualToZero()const
{ if (this->IsOfType<Rational>())
    return this->GetValue<Rational>().IsEqualToZero();
  if (this->IsOfType<Rational>())
    return this->GetValue<Rational>().IsEqualToZero();
  if (this->IsOfType<Polynomial<Rational> >())
    return this->GetValue<Polynomial<Rational> >().IsEqualToZero();
  if (this->IsOfType<RationalFunctionOld>())
    return this->GetValue<RationalFunctionOld>().IsEqualToZero();
  return false;
}

Calculator::Calculator()
{ this->theGlobalVariableS=0;
  this->numOutputFiles=0;
  this->flagHideLHS=false;
}

std::string Calculator::WriteDefaultLatexFileReturnHtmlLink
(const std::string& fileContent, bool useLatexDviPSpsTopdf)
{ std::fstream theFile;
  std::stringstream fileName;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;

  fileName << this->PhysicalNameDefaultOutput << this->numOutputFiles;
  XML::OpenFileCreateIfNotPresent(theFile, fileName.str()+".tex", false, true, false);
  theFile << fileContent;
  theFile.flush();
  theFile.close();
  systemCommand1 << " latex -output-directory=" << this->PhysicalPathOutputFolder << " " << fileName.str()+".tex";
  //std::cout << "<br>system command:<br>" << systemCommand1.str();
  this->SystemCommands.AddOnTop(systemCommand1.str());
  if (useLatexDviPSpsTopdf)
  { systemCommand2 << " dvips -o " <<  fileName.str() << ".ps " << fileName.str() << ".dvi";
    //std::cout << "<br>system command:<br>" << systemCommand2.str();
    this->SystemCommands.AddOnTop(systemCommand2.str());
    systemCommand3 << " convert " << fileName.str() << ".ps " << fileName.str() << ".png";
    //std::cout << "<br>system command:<br>" << systemCommand3.str();
    this->SystemCommands.AddOnTop(systemCommand3.str());
  }
  std::stringstream out;
  out << "<img src=\"" << this->DisplayNameDefaultOutput << this->numOutputFiles << ".png\"></img><a href=\"" << this->DisplayNameDefaultOutput
  << this->numOutputFiles << ".png\">output png</a>";
  this->numOutputFiles++;
  return out.str();
}

inline int IntIdentity(const int& x)
{ return x;
}

void Expression::MakeProducT(Calculator& owner, const Expression& left, const Expression& right)
{ this->MakeXOX(owner, owner.opTimes(), left, right);
}

void Expression::MakeFunction(Calculator& owner, const Expression& theFunction, const Expression& theArgument)
{ this->reset(owner, 2);
  this->AddChildOnTop(theFunction);
  this->AddChildOnTop(theArgument);
  this->format=this->formatFunctionUseUnderscore;
}

bool Expression::MakeEmptyContext(Calculator& owner)
{ this->reset(owner, 1);
  return this->AddChildAtomOnTop(owner.opContexT());
}

bool Expression::MakeContextWithOnePolyVar(Calculator& owner, const Expression& inputPolyVarE)
{ this->MakeEmptyContext(owner);
  Expression thePolyVars;
  thePolyVars.reset(owner, 2);
  thePolyVars.AddChildAtomOnTop(owner.opPolynomialVariables());
  thePolyVars.AddChildOnTop(inputPolyVarE);
  return this->AddChildOnTop(thePolyVars);
}

bool Expression::MakeContextWithOnePolyVarOneDiffVar(Calculator& owner, const Expression& inputPolyVarE, const Expression& inputDiffVarE)
{ this->MakeEmptyContext(owner);
  return this->ContextSetDiffOperatorVar(inputPolyVarE, inputDiffVarE);
}

bool Expression::MakeContextSSLieAlg(Calculator& owner, const SemisimpleLieAlgebra& theSSLiealg)
{ this->MakeEmptyContext(owner);
  return this->ContextSetSSLieAlgebrA(owner.theObjectContainer.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(theSSLiealg), owner);
}

bool Expression::MakeContextWithOnePolyVar(Calculator& owner, const std::string& inputPolyVarName)
{ Expression varNameE;
  varNameE.MakeAtom(owner.AddOperationNoRepetitionOrReturnIndexFirst(inputPolyVarName), owner);
  return this->MakeContextWithOnePolyVar(owner, varNameE);
}

bool Expression::MakeSqrt(Calculator& owner, const Rational& argument, const Rational& radicalSuperIndex)
{ this->reset(owner,3);
  Expression radicalIndexE, argumentE;
  radicalIndexE.AssignValue(radicalSuperIndex, owner);
  argumentE.AssignValue(argument, owner);
  this->AddChildAtomOnTop(owner.opSqrt());
  this->AddChildOnTop(radicalIndexE);
  this->AddChildOnTop(argumentE);
  return true;
}

bool Expression::MakeXOX(Calculator& owner, int theOp, const Expression& left, const Expression& right)
{ if (&left==this || &right==this)
  { Expression leftCopy=left;
    Expression rightCopy=right;
    return this->MakeXOX(owner, theOp, leftCopy, rightCopy);
  }
  this->reset(owner, 3);
  this->theData=owner.opLisT();
  this->AddChildAtomOnTop(theOp);
  this->AddChildOnTop(left);
  return this->AddChildOnTop(right);
}

bool Expression::MakeOX(Calculator& owner, int theOp, const Expression& opArgument)
{ if (&opArgument==this)
  { Expression copyE=opArgument;
    return this->MakeOX(owner, theOp, copyE);
  }
  this->reset(owner);
  this->theData=owner.opLisT();
  this->AddChildAtomOnTop(theOp);
  return this->AddChildOnTop(opArgument);
}

bool Expression::Sequencefy()
{ this->CheckInitialization();
  if (this->IsSequenceNElementS())
    return true;
  return this->MakeOX(*this->theBoss, this->theBoss->opSequence(), *this);
}

bool Calculator::GetVectorExpressions(const Expression& input, List<Expression>& output, int targetDimNonMandatory)
{ MacroRegisterFunctionWithName("Calculator::GetVectorExpressions");
  output.ReservE(input.children.size);
  output.SetSize(0);
  if (!input.IsSequenceNElementS())
  { if (targetDimNonMandatory>0)
      if (targetDimNonMandatory!=1)
      { this->Comments << "<hr>GetVector failure: target dim is " << targetDimNonMandatory << " but the input " << input.ToString()
        << " can only be interpretted as a single element";
        return false;
      }
    output.AddOnTop(input);
    return true;
  }
  if (targetDimNonMandatory>0)
    if (targetDimNonMandatory!=input.children.size-1)
    { this->Comments << "<hr>Failed to GetVector: the input is required to have " << targetDimNonMandatory << " columns but it has "
      << input.children.size-1 << " columns instead. <hr>";
      return false;
    }
  targetDimNonMandatory=input.children.size-1;
  for (int i=0; i<targetDimNonMandatory; i++)
    output.AddOnTop(input[i+1]);
  return true;
}

std::string BoundVariablesSubstitution::ToString()
{ std::stringstream out;
  out << "the pairs: ";
  for (int i=0; i<this->theBoundVariables.size; i++)
    out << this->theBoundVariables[i].ToString() << "->" << this->variableImages[i].ToString() << "<br>";
  return out.str();
}

template<class coefficient>
bool Calculator::GetTypeHighestWeightParabolic
(Calculator& theCommands, const Expression& input, Expression& output, Vector<coefficient>& outputWeightHWFundcoords,
 Selection& outputInducingSel, Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra, Expression::FunctionAddress ConversionFun)
{ if (!input.IsListNElements(4) && !input.IsListNElements(3))
    return output.SetError
    ("Function TypeHighestWeightParabolic is expected to have two or three arguments: SS algebra type, highest weight, [optional] parabolic selection. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& middleE=input[2];
  if (!Calculator::CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, leftE, ambientSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  if (!theCommands.GetVectoR<coefficient>(middleE, outputWeightHWFundcoords, &outputHWContext, ambientSSalgebra->GetRank(), ConversionFun))
  { std::stringstream tempStream;
    tempStream << "Failed to convert the second argument of HWV to a list of " << ambientSSalgebra->GetRank()
    << " polynomials. The second argument you gave is " << middleE.ToString() << ".";
    return output.SetError(tempStream.str(), theCommands);
  }
  if (input.IsListNElements(4))
  { Vector<Rational> parabolicSel;
    const Expression& rightE=input[3];
    if (!theCommands.GetVectoR<Rational>(rightE, parabolicSel, &outputHWContext, ambientSSalgebra->GetRank(), 0))
    { std::stringstream tempStream;
      tempStream << "Failed to convert the third argument of HWV to a list of " << ambientSSalgebra->GetRank()
      << " rationals. The third argument you gave is " << rightE.ToString() << ".";
      return output.SetError(tempStream.str(), theCommands);
    }
    outputInducingSel=parabolicSel;
  } else
  { outputInducingSel.init(ambientSSalgebra->GetRank());
    for (int i=0; i<outputWeightHWFundcoords.size; i++)
      if (!outputWeightHWFundcoords[i].IsSmallInteger())
        outputInducingSel.AddSelectionAppendNewIndex(i);
  }
  if (!theCommands.theObjectContainer.theLieAlgebras.ContainsExactlyOnce(*ambientSSalgebra))
    crash << "This is a programming error: " << ambientSSalgebra->GetLieAlgebraName() << " contained object container more than once. " << crash;
  int algebraIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*ambientSSalgebra);
  outputHWContext.ContextSetSSLieAlgebrA(algebraIndex, theCommands);
//  std::cout << "final context of GetTypeHighestWeightParabolic: " << outputHWContext.ToString();
  return true;
}

bool Calculator::fDecomposeCharGenVerma(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::fDecomposeCharGenVerma");
  Expression theContext;
  Vector<RationalFunctionOld> theHWfundcoords, theHWsimpCoords, theHWFundCoordsFDPart, theHWSimpCoordsFDPart;
  Selection parSel, invertedParSel;
  SemisimpleLieAlgebra* theSSlieAlg=0;
  output.reset(theCommands);
  if (!theCommands.GetTypeHighestWeightParabolic<RationalFunctionOld>
      (theCommands, input, output, theHWfundcoords, parSel, theContext, theSSlieAlg, theCommands.innerRationalFunction))
   return false;
  if (output.IsError())
    return true;
  std::stringstream out;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  out << "<br>Highest weight: " << theHWfundcoords.ToString(&theFormat) << "<br>Parabolic selection: " << parSel.ToString();
  theHWFundCoordsFDPart=theHWfundcoords;
  for (int i=0; i<parSel.CardinalitySelection; i++)
    theHWFundCoordsFDPart[parSel.elements[i]]=0;
  KLpolys theKLpolys;
  WeylGroup theWeyl;
  theWeyl=theSSlieAlg->theWeyl;
  if (!theKLpolys.ComputeKLPolys(&theWeyl))
    return output.SetError("failed to generate Kazhdan-Lusztig polynomials (output too large?)", theCommands);
//  Vectors<Polynomial<Rational> > tempVects;
//  tempVects.AddOnTop(theSSlieAlg.theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords) );
//  HashedList<Vector<Polynomial<Rational> > > theOrbit;
//  if (!theSSlieAlg.theWeyl.GenerateOrbit(tempVects, true, theOrbit, false, -1, 0, 1000))
//    out << "Error: failed to generate highest weight \rho-modified orbit (too large?)";
  theHWSimpCoordsFDPart=theWeyl.GetSimpleCoordinatesFromFundamental(theHWFundCoordsFDPart);
  theHWSimpCoordsFDPart+=theWeyl.rho;
//  ElementWeylGroup raisingElt;
//  theSSlieAlg.theWeyl.RaiseToDominantWeight
//  (theHWSimpCoordsFDPart, 0, 0, &raisingElt);
  ReflectionSubgroupWeylGroup theSub;
  if (!theSub.MakeParabolicFromSelectionSimpleRoots(theWeyl, parSel, *theCommands.theGlobalVariableS, 1000))
    return output.SetError("Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).", theCommands);
  theHWsimpCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords);
  List<ElementWeylGroup> theWeylElements;
  theSub.GetGroupElementsIndexedAsAmbientGroup(theWeylElements);
  Vector<RationalFunctionOld> currentHW;
  out << "<br>Orbit modified with small rho: <table><tr><td>Simple coords</td><td>Fund coords</td></tr>";
  for (int i=0; i<theWeyl.theElements.size; i++)
  { currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(i, currentHW);
    currentHW-=theSub.GetRho();
    out << "<tr><td>" << currentHW.ToString() << "</td><td>" << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToString() << "</td></tr>";
  }
  out << "</table>";
  out << "<br>The rho of the Levi part is: " << theSub.GetRho().ToString() << "<br>Weyl group of Levi part follows. "
  << "<br><table><tr><td>Weyl element<td><td>Image hw under small rho modified action fund coords</td>"
  << "<td>Character Verma given h.w.</td></tr>";
  invertedParSel=parSel;
  invertedParSel.InvertSelection();
  charSSAlgMod<RationalFunctionOld> theChar, currentChar;
  MonomialChar<RationalFunctionOld> theMon;
  theChar.MakeZero();
  FormatExpressions formatChars;
  formatChars.FDrepLetter="L";
  formatChars.fundamentalWeightLetter="\\omega";
  formatChars.flagUseLatex=true;
  for (int i=0; i<theWeylElements.size; i++)
  { ElementWeylGroup& currentElt=theWeylElements[i];
    out << "<tr><td>" << currentElt.ToString() << "</td>";
    int indexInWeyl=theKLpolys.TheWeylGroup->theElements.GetIndex(currentElt);
    if (indexInWeyl==-1)
      crash << "This is a programming error. Something is wrong: I am getting that an element of the Weyl group of the Levi part of "
      << "the parabolic does not lie in the ambient Weyl group, which is impossible. There is a bug somewhere; crashing in accordance. " << crash;
    currentChar.MakeZero();
    theMon.owner=theSSlieAlg;
    for (int j=0; j< theKLpolys.theKLcoeffs[indexInWeyl].size; j++)
      if (!theKLpolys.theKLcoeffs[indexInWeyl][j].IsEqualToZero())
      { currentHW=theHWsimpCoords;
//        currentHW+=theSub.GetRho();
        theWeyl.ActOnRhoModified(j, currentHW);
//        currentHW-=theSub.GetRho();
        theMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(currentHW);
        int sign= (currentElt.reflections.size- theWeyl.theElements[j].reflections.size)%2==0 ? 1 :-1;
        currentChar.AddMonomial(theMon, theKLpolys.theKLcoeffs[indexInWeyl][j]*sign);
      }
    currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(indexInWeyl, currentHW);
    currentHW-=theSub.GetRho();
    out << "<td>" << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToStringLetterFormat("\\omega") << "</td>";
    out << "<td>" << CGI::GetMathMouseHover(currentChar.ToString(&formatChars)) << "</td>";
    if (currentElt.reflections.size%2==1)
      currentChar*=-1;
    theChar+=currentChar;
    out << "</tr>";
  }
  out << "</table>";
  out << "Final char: " << CGI::GetMathMouseHover(theChar.ToString(&formatChars));
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool Calculator::innerPrintGenVermaModule(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerPrintGenVermaModule");
  Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.GetTypeHighestWeightParabolic
      (theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, theCommands.innerRationalFunction))
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  if (!theCommands.innerHWVCommon(theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, false))
    return output.SetError("Failed to create Generalized Verma module", theCommands);
  if (output.IsError())
    return true;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt=output.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  ModuleSSalgebra<RationalFunctionOld>& theModule= *theElt[0].theMons[0].owneR;
  return output.AssignValue(theModule.ToString(), theCommands);
}

bool Calculator::innerHWV(Calculator& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.GetTypeHighestWeightParabolic(theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, theCommands.innerRationalFunction) )
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  return theCommands.innerHWVCommon(theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra);
}

bool Calculator::innerWriteGenVermaModAsDiffOperatorUpToLevel(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError("Function innerSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, Number, List{}. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& levelNode=input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, leftE, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVectoR(genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, Serialization::innerPolynomial<Rational>))
  { theCommands.Comments
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
    return false;
  }
  int desiredHeight;
  if (!levelNode.IsSmallInteger(&desiredHeight))
    return output.SetError("second argument of " + input.ToString()+ " must be a small integer", theCommands);

//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();

//  int theNumVars=hwContext.VariableImages.size;
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  Selection selInducing;
  Expression hwvGenVerma, hwvFD;
  selInducing.MakeFullSelection(theRank);
  int theCoeff;
  for (int i=0; i<theRank; i++)
    if (highestWeightFundCoords[i].IsSmallInteger(&theCoeff))
      if (theCoeff==0)
        selInducing.RemoveSelection(i);
//  std::cout << "Your input so far: " << theSSalgebra.GetLieAlgebraName() << " with hw: " << highestWeightFundCoords.ToString()
//  << " parabolic selection: " << selInducing.ToString() << " degree: " << desiredHeight;
  Vectors<Polynomial<Rational> > theHws;
  Selection invertedSelInducing=selInducing;
  invertedSelInducing.InvertSelection();
  theHws.SetSize(0);
  SelectionWithMaxMultiplicity theHWenumerator;
  Vector<Polynomial<Rational> > theHWrf;
  for (int j=0; j<=desiredHeight; j++)
  { theHWenumerator.initMaxMultiplicity(theRank-selInducing.CardinalitySelection, j);
    theHWenumerator.IncrementSubsetFixedCardinality(j);
    int numCycles=theHWenumerator.NumCombinationsOfCardinality(j);
    for (int i=0; i<numCycles; i++, theHWenumerator.IncrementSubsetFixedCardinality(j))
    { theHWrf=highestWeightFundCoords;
      for (int k=0; k<invertedSelInducing.CardinalitySelection; k++)
        theHWrf[invertedSelInducing.elements[k]]+=theHWenumerator.Multiplicities[k];
      theHws.AddOnTop(theHWrf);
    }
  }
  FormatExpressions theFormat;
  hwContext.ContextGetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner(theCommands, input, output, theHws, hwContext, selInducing, theSSalgebra, false);
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInParabolic(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]]<0)
      return true;
  return false;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInLevi(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].IsEqualToZero())
      return true;
  return false;
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
 (bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true);
  RationalFunctionOld tempRF;
  output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFunctionOld> tempMon;
  tempMon.MakeConst(*this->theAlgebras, this->indexAlgebra);
  int varShift=0;
  if (shiftPowersByNumVarsBaseField)
    varShift=this->GetMinNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeOneLetterMoN(i+varShift, 1, theGlobalVariables);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(&theGlobalVariables);
  output.AddMonomial(tempMon, tempRF);
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
(bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<Polynomial<Rational> >& output, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true);
  Polynomial<Rational> tempRF;
  output.MakeZero(*this->owneR);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.MakeOne(*this->owneR);
  int varShift=0;
  if (shiftPowersByNumVarsBaseField)
    varShift=this->GetMinNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeMonomiaL(i+varShift, 1, 1, numVars);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(numVars);
  output.AddMonomial(tempMon, tempRF);
}

class quasiDiffMon
{
  friend std::ostream& operator << (std::ostream& output, const quasiDiffMon& theMon)
  { output << theMon.ToString();
    return output;
  }
  public:
  MonomialWeylAlgebra theWeylMon;
  MonomialMatrix theMatMon;
  static unsigned int HashFunction(const quasiDiffMon& input)
  { return input.theWeylMon.HashFunction()*SomeRandomPrimes[0]+input.theMatMon.HashFunction()*SomeRandomPrimes[1];
  }
  unsigned int HashFunction()const
  { return HashFunction(*this);
  }
  bool IsEqualToZero()const
  { return this->theMatMon.IsEqualToZero();
  }
  bool operator==(const quasiDiffMon& other)const
  { return this->theWeylMon==other.theWeylMon && this->theMatMon==other.theMatMon;
  }
  void operator=(const quasiDiffMon& other)
  { this->theWeylMon=other.theWeylMon;
    this->theMatMon=other.theMatMon;
  }
  bool operator>(const quasiDiffMon& other)const
  { if (this->theMatMon==other.theMatMon)
      return this->theWeylMon>other.theWeylMon;
    return this->theMatMon>other.theMatMon;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
};

std::string quasiDiffMon::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << this->theWeylMon.ToString(theFormat) << "\\otimes ";
  out << this->theMatMon.ToString(theFormat);
  return out.str();
}

template <class coefficient>
class quasiDiffOp : public MonomialCollection<quasiDiffMon, coefficient>
{
public:
  std::string ToString(FormatExpressions* theFormat=0)const;
  void GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat=0, GlobalVariables* theGlobalVariables=0);
  void operator*=(const quasiDiffOp<coefficient>& standsOnTheRight);
  void operator=(const  MonomialCollection<quasiDiffMon, coefficient>& other)
  { this->MonomialCollection<quasiDiffMon, coefficient>::operator=(other);
  }
  void LieBracketMeOnTheRight(const MonomialCollection<quasiDiffMon, coefficient>& standsOnTheRight)
  { quasiDiffOp<coefficient> tempRight;
    tempRight=standsOnTheRight;
    MathRoutines::LieBracket(*this, tempRight, *this);
  }
  void FourierTransformDiffPartOnly(quasiDiffOp<coefficient>& output)const;
  void GetEWAsetMatrixPartsToId(ElementWeylAlgebra<coefficient>& output)const;
};

template <class coefficient>
void quasiDiffOp<coefficient>::FourierTransformDiffPartOnly(quasiDiffOp<coefficient>& output)const
{ if (& output==this)
  { quasiDiffOp<coefficient> thisCopy;
    thisCopy.FourierTransformDiffPartOnly(output);
    return;
  }
  output.MakeZero();
  ElementWeylAlgebra<coefficient> startDO, finalDO;
  quasiDiffMon theMon;
  for (int i=0; i<this->size(); i++)
  { startDO.MakeZero();
    startDO.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
    startDO.FourierTransform(finalDO);
    for (int j=0; j<finalDO.size(); j++)
    { theMon.theMatMon=(*this)[i].theMatMon;
      theMon.theWeylMon=finalDO[j];
      output.AddMonomial(theMon, finalDO.theCoeffs[j]);
    }
  }
}

template <class coefficient>
void quasiDiffOp<coefficient>::GetEWAsetMatrixPartsToId(ElementWeylAlgebra<coefficient>& output)const
{ output.MakeZero();
  for (int i=0; i<this->size(); i++)
    output.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
}

template<class Element>
void MathRoutines::LieBracket(const Element& standsOnTheLeft, const Element& standsOnTheRight, Element& output)
{ if (&standsOnTheLeft==&output || &standsOnTheRight==&output)
  { Element standsOnTheLeftNew, standsOnTheRightNew;
    standsOnTheLeftNew=standsOnTheLeft;
    standsOnTheRightNew=standsOnTheRight;
    MathRoutines::LieBracket(standsOnTheLeftNew, standsOnTheRightNew, output);
    return;
  }
//  FormatExpressions tempFormat;
//  tempFormat.polyAlphabeT.SetSize(4);
//  tempFormat.polyAlphabeT[0]="x_1";
//  tempFormat.polyAlphabeT[1]="x_2";
//  tempFormat.polyAlphabeT[2]="\\partial_1";
//  tempFormat.polyAlphabeT[3]="\\partial_2";

  Element tempE;
//  std::cout << "<hr>[ " << standsOnTheLeft.ToString(&tempFormat);
//  std::cout << " , " << standsOnTheRight.ToString(&tempFormat) << " ] = ";
  output=standsOnTheLeft;
  output*=standsOnTheRight;
//  std::cout << "<br>intermediate: " << output.ToString(&tempFormat);
  tempE=standsOnTheRight;
  tempE*=standsOnTheLeft;
//  std::cout << "<br>intermediate2: " << tempE.ToString(&tempFormat);
  output-=tempE;
//  std::cout << "<br>finally:" << output.ToString(&tempFormat) << "<hr>";
}

template <class coefficient>
void quasiDiffOp<coefficient>::GenerateBasisLieAlgebra(List<quasiDiffOp<coefficient> >& theElts, FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("quasiDiffOp<coefficient>::GenerateBasisLieAlgebra");
  ProgressReport theReport (theGlobalVariables);
  HashedList<quasiDiffMon> bufferMons;
  //std::cout << "<br> the elts:" << theElts.ToString();
  List< MonomialCollection<quasiDiffMon, coefficient> > theEltsConverted;
  theEltsConverted=theElts;
  //std::cout << "<br>the elts converted: " << theEltsConverted.ToString();
  this->GaussianEliminationByRows(theEltsConverted);
  quasiDiffOp tempQDO;
  bool foundNew=true;
  int numTimesEliminationWasExecuted=0;
  while (foundNew)
  { foundNew=false;
    for(int i=0; i<theEltsConverted.size; i++)
      for (int j=i+1; j<theEltsConverted.size; j++)
      { tempQDO=theEltsConverted[i];
        std::stringstream report;
        report << "Lie bracketing elements " << " of indices " << i+1 << " and " << j+1 << " out of " << theEltsConverted.size << "<br> "
        << tempQDO.ToString(theFormat) << "<br> with element <br>" << theEltsConverted[j].ToString(theFormat) << " to get <br>";
        //      std::cout << "<hr>Lie bracketing " << tempQDO.ToString(&tempFormat)
  //      << " and " << theEltsConverted[j].ToString(&tempFormat);
        tempQDO.LieBracketMeOnTheRight(theEltsConverted[j]);
        theReport.Report(report.str());
        report << tempQDO.ToString(theFormat);
        theReport.Report(report.str());
  //      std::cout << ", to get " << tempQDO.ToString(&tempFormat);
        theEltsConverted.AddOnTop(tempQDO);
        quasiDiffOp::GaussianEliminationByRows(theEltsConverted, 0, &bufferMons);
        numTimesEliminationWasExecuted++;
        if (!theEltsConverted.LastObject()->IsEqualToZero())
          foundNew=true;
        for (int k=theEltsConverted.size-1; k>=0; k--)
          if (theEltsConverted[k].IsEqualToZero())
            theEltsConverted.RemoveIndexSwapWithLast(k);
          else
            break;
      }
  }
  std::cout << "<hr>" << "<b>Num times Gaussian Elimination was called = " << numTimesEliminationWasExecuted << ".</b>";
  theElts.SetSize(theEltsConverted.size);
  for (int i=0; i<theEltsConverted.size; i++)
    theElts[i]=theEltsConverted[i];
//  std::cout << "<hr>the elts at end: ";
//  theElts=theEltsConverted;
//  for (int i=0; i<theElts.size; i++)
//    std::cout << "<br>" << theElts[i].ToString();
}

template <class coefficient>
void quasiDiffOp<coefficient>::operator*=(const quasiDiffOp<coefficient>& standsOnTheRight)
{ quasiDiffOp<coefficient> output;
  ElementWeylAlgebra<Rational> leftElt, rightElt, tempElt;
  quasiDiffMon outputMon;
  output.MakeZero();
  for (int j=0; j<standsOnTheRight.size(); j++)
  { rightElt.MakeZero();
    rightElt.AddMonomial(standsOnTheRight[j].theWeylMon, standsOnTheRight.theCoeffs[j]);
    for (int i=0; i<this->size(); i++)
    { leftElt.MakeZero();
      leftElt.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
      outputMon.theMatMon= (*this)[i].theMatMon;
      outputMon.theMatMon*=standsOnTheRight[j].theMatMon;
      leftElt*=rightElt;
      for (int k=0; k<leftElt.size(); k++)
      { outputMon.theWeylMon=leftElt[k];
        output.AddMonomial(outputMon, leftElt.theCoeffs[k]);
      }
    }
  }
  *this=output;
}

template <class coefficient>
std::string quasiDiffOp<coefficient>::ToString(FormatExpressions* theFormat)const
{ bool combineWeylPart=true;
  if (theFormat!=0)
    combineWeylPart=theFormat->flagQuasiDiffOpCombineWeylPart;
  if (!combineWeylPart)
    return this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat);
  MatrixTensor<ElementWeylAlgebra<Rational> > reordered;
  reordered.MakeZero();
  ElementWeylAlgebra<Rational> tempP;
  for (int i=0; i<this->size(); i++)
  { const quasiDiffMon& currentMon=(*this)[i];
    tempP.MakeZero();
    tempP.AddMonomial(currentMon.theWeylMon, this->theCoeffs[i]);
    reordered.AddMonomial(currentMon.theMatMon, tempP);
  }
  std::string result=reordered.ToString(theFormat);
  if (result=="0" && this->size()!=0)
    crash << "This is likely a programming error (crashing at any rate): I have a non-zero quasidifferential operator "
    << " with non-properly formatted LaTeX string " << this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat)
    << ", however its properly formatted string is 0. Probably there is something wrong with the initializations of the monomials of the qdo. " << crash;
  return result;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionEulerOperatorPart(const MonomialP& theCoeff, ElementWeylAlgebra<Rational>& outputDO, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionMonGenVermaModuleAsDiffOperator");
//  int varShift=this->GetMinNumVars();
//  std::cout << "<br>varShift for Euler operator: " << varShift;
  int powerMonCoeff=0;
  ElementWeylAlgebra<Rational> currentMonContribution;
  outputDO.MakeOne();
//  std::cout << "<br>Getting Euler part contribution of " << theCoeff.ToString()
//  <<  " with min num vars equal to " << theCoeff.GetMinNumVars();
  for (int i=0; i<theCoeff.GetMinNumVars(); i++)
  { if (!theCoeff(i).IsSmallInteger(&powerMonCoeff))
      crash << "This is a programming error. Getting euler operator part of action on generalized Verma module: I have an exponent with non-small integer entry. "
      << crash;
    currentMonContribution.Makexidj(i, i, 0);
    currentMonContribution.RaiseToPower(powerMonCoeff);
    outputDO*=currentMonContribution;
    //std::cout << "<br>Accounted index " << i+1 << "  out of " << theCoeff.GetMinNumVars()
    //<< ", power is " << powerMonCoeff << ", current output DO has "
    //<< outputDO.size << " monomials.";
  }
//  std::cout << ".. Final euler: "  << outputDO.ToString();
  return true;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionGenVermaModuleAsDiffOperator
(ElementSemisimpleLieAlgebra<Rational>& inputElt, quasiDiffOp<Rational>& output, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionGenVermaModuleAsDiffOperator");
  List<ElementUniversalEnveloping<coefficient> > eltsNilrad;
  List<int> indicesNilrad;
  this->GetElementsNilradical(eltsNilrad, true, &indicesNilrad);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->GetGenericUnMinusElt(true, theGenElt, theGlobalVariables);
//  Polynomial<Rational> Pone, Pzero;
  result.AssignElementLieAlgebra(inputElt, *this->owneR, 1, 0);
  std::stringstream out;
//  std::cout << "<br>the generic elt:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
  theGenElt.Simplify(&theGlobalVariables);
//  std::cout << "<br>the generic elt simplified:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
//  std::cout << "<br>" << CGI::GetHtmlMathSpanPure(result.ToString() ) << " times " << CGI::GetHtmlMathSpanPure(theGenElt.ToString()) << " = ";
  result*=(theGenElt);
  result.Simplify(&theGlobalVariables);
//  std::cout << " <br>" << CGI::GetHtmlMathSpanPure(result.ToString());
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.MakeIdSpecial();
  MatrixTensor<RationalFunctionOld> tempMat1;

  int varShift=this->GetMinNumVars();
//  std::cout  << "<br>Num elements nilrad: " << indicesNilrad.size;
  ElementWeylAlgebra<Rational> weylPartSummand, exponentContribution, oneIndexContribution,
  eulerOperatorContribution;
  Polynomial<Rational> tempP1, negativeExponentDenominatorContribution, theCoeff;
  quasiDiffMon monQDO, monQDO2;
  //static int problemCounter=0;
  //problemCounter++;
  Rational tempRat;
  output.MakeZero();
  Rational currentShift;
  for (int i=0; i<result.size(); i++)
  { //problemCounter++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon=result[i];
    endoPart=idMT;
    for (int j=currentMon.Powers.size-1; j>=indicesNilrad.size; j--)
    { int thePower=0;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower))
        return false;
      tempMat1=this->GetActionGeneratorIndeX(currentMon.generatorsIndices[j], theGlobalVariables);
      tempMT.MakeZero();
      for (int k=0; k<tempMat1.size(); k++)
      { if (tempMat1.theCoeffs[k].expressionType==RationalFunctionOld::typeRationalFunction)
          return false;
        tempMat1.theCoeffs[k].GetNumerator(tempP1);
        tempMT.AddMonomial(tempMat1[k], tempP1);
      }
      MathRoutines::RaiseToPower(tempMT, thePower, idMT);
      endoPart*=tempMT;
    }
    exponentContribution.MakeOne();
    theCoeff=result.theCoeffs[i];
    for (int j=0; j<indicesNilrad.size; j++)
    { //if (problemCounter==249)
        //std::cout << "ere be problem!";
      //problemCounter++;
      currentMon.Powers[j].GetConstantTerm(currentShift);
      ElementWeylAlgebra<Rational>::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
      (currentShift, j+varShift, oneIndexContribution, negativeExponentDenominatorContribution, theGlobalVariables);
//      std::cout << "<br>result from GetStandardOrderDiffOperatorCorrespondingToNraisedTo: "
//      << negativeExponentDenominatorContribution.ToString() << " divided by "
//      << oneIndexContribution.ToString();
      exponentContribution*=oneIndexContribution;
      theCoeff.DivideBy(negativeExponentDenominatorContribution, theCoeff, tempP1);
      if (!tempP1.IsEqualToZero())
        crash << "This is a mathematical error! Something is very wrong with embedding semisimple Lie algebras in Weyl algebras. " << crash;
    }
//    std::cout << "<br>Endo part of " << currentMon.ToString() << ": " << endoPart.ToString();
//    std::cout << "<br>Exponent contribution of " << currentMon.ToString() << ": "
//    << exponentContribution.ToString();
    for (int l=0; l<theCoeff.size(); l++)
    { //problemCounter++;
      //if (problemCounter==249)
        //std::cout << "ere be problem!";
      if (!this->GetActionEulerOperatorPart(theCoeff[l], eulerOperatorContribution, theGlobalVariables))
        return false;
//      std::cout << "<br>Euler operator contribution: " << eulerOperatorContribution.ToString();
      weylPartSummand=exponentContribution;
      weylPartSummand*=eulerOperatorContribution;
      weylPartSummand*=theCoeff.theCoeffs[l];
      for (int j=0; j<weylPartSummand.size(); j++)
        for (int k=0; k<endoPart.size(); k++)
        { monQDO.theMatMon=endoPart[k];
          monQDO.theWeylMon=weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff=endoPart.theCoeffs[k];
          for (int m=0; m<currentEndoCoeff.size(); m++)
          { monQDO2=monQDO;
            monQDO2.theWeylMon.polynomialPart*=currentEndoCoeff[m];
            tempRat=currentEndoCoeff.theCoeffs[m];
            tempRat*=weylPartSummand.theCoeffs[j];
  //          if (problemCounter==5)
  //          std::cout << "<br>adding " << monQDO.ToString() << " times "
  //          << tempRat.ToString()  << " to "
  //          << output.ToString();
            output.AddMonomial(monQDO2, tempRat);
  //          if (problemCounter==5)
  //          std::cout << " to get " << output.ToString();
          }
        }
    }
  }
//  std::cout << "<br>result qdo: " << output.ToString();
  return true;
}

bool Calculator::innerWriteGenVermaModAsDiffOperatorInner
(Calculator& theCommands, const Expression& input, Expression& output, Vectors<Polynomial<Rational> >& theHws, Expression& hwContext,
 Selection& selInducing, SemisimpleLieAlgebra* owner, bool AllGenerators, std::string* xLetter, std::string* partialLetter, std::string* exponentVariableLetter)
{ MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperatorInner");
   /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  if (theHws.size==0)
    return false;
  Expression tempExp;
  SemisimpleLieAlgebra& theSSalgebra=*owner;
  List<ElementUniversalEnveloping<Polynomial<Rational> > > elementsNegativeNilrad;
  ElementSemisimpleLieAlgebra<Rational> theGenerator;
  ElementUniversalEnveloping<Polynomial<Rational> > genericElt, actionOnGenericElt;
  List<ElementWeylAlgebra<Rational> > actionNilrad;
  List<int> vectorIndices, dualIndices;
  List<quasiDiffOp<Rational> > theQDOs;
  FormatExpressions theWeylFormat, theUEformat;
  std::stringstream out, latexReport, latexReport2;
  theWeylFormat.MaxLineLength=40;
  theWeylFormat.flagUseLatex=true;
  theUEformat.MaxLineLength=20;
  theUEformat.flagUseLatex=true;
  theUEformat.chevalleyGgeneratorLetter="g";
  theUEformat.chevalleyHgeneratorLetter="h";
  hwContext.ContextGetFormatExpressions(theUEformat);
  theUEformat.polyDefaultLetter= exponentVariableLetter==0  ? "a" : *exponentVariableLetter;
  theUEformat.MaxLineLength=178;
  theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
  theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
  hwContext.ContextGetFormatExpressions(theWeylFormat);
  List<ElementSemisimpleLieAlgebra<Rational> > theGeneratorsItry;
  if (!AllGenerators)
    for (int j=0; j<theSSalgebra.GetRank(); j++)
    { Vector<Rational> ei;
      ei.MakeEi(theSSalgebra.GetRank(), j);
      theGenerator.MakeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
      theGenerator.MakeHgenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
      ei.Minus();
      theGenerator.MakeGGenerator(ei, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
    }
  else
    for (int j=0; j<theSSalgebra.GetNumGenerators(); j++)
    { theGenerator.MakeGenerator(j, theSSalgebra);
      theGeneratorsItry.AddOnTop(theGenerator);
    }
  theQDOs.SetSize(theGeneratorsItry.size);
/*  if (false)
    if (theSSalgebra.GetRank()==3 && theSSalgebra.theWeyl.WeylLetter=='B')
    { theGenerator=theGeneratorsItry[0];
      theGenerator+=theGeneratorsItry[2];
      theGeneratorsItry.AddOnTop(theGenerator);
    }*/
  out << "<table border=\"1\">";
  latexReport << "\\begin{longtable}{rll";
  for (int i =0; i<theGeneratorsItry.size; i++)
    latexReport << "l";
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.ToString() << "} Differential operators corresponding to actions"
  << " of simple positive generators for the " << selInducing.ToString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFunctionOld > > theMods;
  theMods.SetSize(theHws.size);
  Vector<RationalFunctionOld> tempV;
  int numStartingVars=hwContext.ContextGetNumContextVariables();
  //std::cout << "<br>num starting vars:" << numStartingVars;
  std::stringstream reportFourierTransformedCalculatorCommands, reportCalculatorCommands;
  for (int i=0; i<theHws.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[i];
    tempV=theHws[i];
    if (!theMod.MakeFromHW(theSSalgebra, tempV, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0, true))
      return output.SetError("Failed to create module.", theCommands);
    if (i==0)
    { theMod.GetElementsNilradical(elementsNegativeNilrad, true);
      Polynomial<Rational> Pone, Pzero;
      Pone.MakeOne(elementsNegativeNilrad.size+theMod.GetMinNumVars());
      Pzero.MakeZero();
      theMod.GetGenericUnMinusElt(true, genericElt, *theCommands.theGlobalVariableS);
      //std::cout << "<br>highest weight: " << tempV.ToString();
      //std::cout << "<br>generic elt: " <<  genericElt.ToString();

      //std::cout << "<br>theWeylFormat: ";
//      for (int k=0; k<theWeylFormat.polyAlphabeT.size; k++)
//        std::cout << theWeylFormat.polyAlphabeT[k] << ", ";
      theWeylFormat.polyAlphabeT.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      theWeylFormat.weylAlgebraLetters.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      theUEformat.polyAlphabeT.SetSize(numStartingVars+ elementsNegativeNilrad.size);
      for (int k=0; k<numStartingVars; k++)
        theWeylFormat.weylAlgebraLetters[k]="error";
      //std::cout << "<br>HW num context vars: " << hwContext.ContextGetNumContextVariables();
      std::string theFinalXletter= (xLetter==0) ? "x": *xLetter;
      std::string theFinalPartialLetter=(partialLetter==0) ? "\\partial" : *partialLetter;
      for (int k=numStartingVars; k<theUEformat.polyAlphabeT.size; k++)
      { std::stringstream tmpStream, tempstream2, tempstream3, tempStream4;
        tmpStream << theUEformat.polyDefaultLetter << "_{" << k-hwContext.ContextGetNumContextVariables()+1 << "}";
        theUEformat.polyAlphabeT[k] = tmpStream.str();
        tempstream2 << theFinalXletter << "_{" << k-numStartingVars+1 << "}";
        tempstream3 << theFinalXletter << "_" << k-numStartingVars+1;
        tempStream4 << theFinalPartialLetter << "_{" << k-numStartingVars+1 << "}";
        if (theWeylFormat.polyAlphabeT.Contains(tempstream2.str()) || theWeylFormat.polyAlphabeT.Contains(tempstream3.str()))
          return output.SetError
          ("Error: the variable "+ tempstream2.str()+" is reserved for me: you are not allowed to use it as a coordinate of the highest weight. ", theCommands);
        theWeylFormat.polyAlphabeT[k]=tempstream2.str();
        theWeylFormat.weylAlgebraLetters[k]=tempStream4.str();
      }
//      std::cout << "<br>theUEformat: ";
//      for (int k=0; k<theUEformat.polyAlphabeT.size; k++)
//        std::cout << theUEformat.polyAlphabeT[k] << ", ";
      out << "<tr><td>General monomial in U(n_-):</td><td>" << CGI::GetMathMouseHover(genericElt.ToString(&theUEformat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << theGeneratorsItry.size << "}{c}{Element acting}\\\\<br>\n ";
      latexReport << "Action on ";
      out << "<tr><td></td><td colspan=\"" << theGeneratorsItry.size << "\"> Element acting</td></td></tr>";
      out << "<tr><td>Action on</td>";
      for (int j=0; j<theGeneratorsItry.size; j++)
      { out << "<td>" << theGeneratorsItry[j].ToString(&theUEformat) << "</td>";
        latexReport << "& $" << theGeneratorsItry[j].ToString(&theUEformat)  << "$";
      }
      latexReport << "\\endhead \\hline<br>";
      out << "</tr>";
      out << "<tr><td>" << CGI::GetMathMouseHover(genericElt.ToString(&theUEformat)) << "</td>";
      latexReport << "$" << genericElt.ToString(&theUEformat) << "$";
      for (int j=0; j<theGeneratorsItry.size; j++)
      { actionOnGenericElt.AssignElementLieAlgebra(theGeneratorsItry[j], theSSalgebra, Pone, Pzero);
        actionOnGenericElt*=(genericElt);
        theSSalgebra.OrderSetNilradicalNegativeMost(theMod.parabolicSelectionNonSelectedAreElementsLevi);
        theSSalgebra.OrderSetNilradicalNegativeMostReversed(theMod.parabolicSelectionNonSelectedAreElementsLevi);
        actionOnGenericElt.Simplify(theCommands.theGlobalVariableS);
        theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
        out << "<td>" << CGI::GetMathMouseHover("\\begin{array}{rcl}&&" +actionOnGenericElt.ToString(&theUEformat)+"\\end{array}") << "</td>";
        theUEformat.NumAmpersandsPerNewLineForLaTeX=0;
        latexReport << "& $\\begin{array}{l} " << actionOnGenericElt.ToString(&theUEformat) << "\\end{array}$ ";
      }
      latexReport << "\\\\ \\hline\\hline<br>";
      out << "</tr>";
    }
    out << "<tr><td>" << CGI::GetMathMouseHover(theMod.theChaR.ToString()) << "</td>";
    latexReport2 << "\\begin{longtable}{rll}";
    latexReport2 << "$\\gog$& $n$& element of $\\mathbb W_n$ \\\\\\hline" << "\\multirow{" << theGeneratorsItry.size
    << "}{*}{$" << theSSalgebra.GetLieAlgebraName() << "$}" << " &  \\multirow{"  << theGeneratorsItry.size << "}{*}{"
    << elementsNegativeNilrad.size << "}&";

    latexReport << "$\\begin{array}{r}" << theMod.theChaR.ToString() << "(\\mathfrak{l}) \\\\ \\\\dim:~" << theMod.GetDim() << " \\end{array}$";
    //std::cout << "<hr>hw: " << theMod.theHWFundamentalCoordsBaseField.ToString() << " nilrad elts: " << elementsNegativeNilrad.ToString();
    //std::cout << "<br>generic element: " << genericElt.ToString();
    for (int j=0; j<theGeneratorsItry.size; j++)
    { theGenerator=theGeneratorsItry[j];
      theMod.GetActionGenVermaModuleAsDiffOperator(theGenerator, theQDOs[j], *theCommands.theGlobalVariableS);
      theWeylFormat.CustomCoeffMonSeparator="\\otimes ";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
      out << "<td>" << CGI::GetMathMouseHover("\\begin{array}{|r|c|l|}&&"+theQDOs[j].ToString(&theWeylFormat)+"\\end{array}")
      << "</td>";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX=0;
      theWeylFormat.MaxLineLength=300;
      latexReport << " & $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$";
      if (j!=0)
        latexReport2 << "&&";
      latexReport2 << " $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$\\\\ "
      << (j!=theGeneratorsItry.size-1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      theWeylFormat.CustomCoeffMonSeparator="";
    }
    latexReport2 << "\\end{longtable}";
    latexReport << "\\\\\\hline<br>";
    out << "</tr>";
    if (theMod.GetDim()==1)
    { ElementWeylAlgebra<Rational> diffOpPart, transformedDO;
      reportFourierTransformedCalculatorCommands << "<hr>" << CGI::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators Fourier transformed - formatted for calculator input. <br><br>";
      reportFourierTransformedCalculatorCommands << "x_{{i}}:=ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}:=ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";
      reportCalculatorCommands << "<hr>" << CGI::GetMathMouseHover(theMod.theChaR.ToString())
      << ", differential operators - formatted for calculator input. <br><br>";
      reportCalculatorCommands << "x_{{i}}:=ElementWeylAlgebraPoly{}(\\partial_i, x_i);\n<br>"
      << "\\partial_{{i}}:=ElementWeylAlgebraDO{}(\\partial_i, x_i);\n";

      for (int j=0; j<theGeneratorsItry.size; j++)
      { theQDOs[j].GetEWAsetMatrixPartsToId(diffOpPart);
        diffOpPart.FourierTransform(transformedDO);
        reportFourierTransformedCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << ":=" << transformedDO.ToString() << ";";
        reportCalculatorCommands << "<br>" << theGeneratorsItry[j].ToString() << ":=" << diffOpPart.ToString() << ";";
      }
      reportFourierTransformedCalculatorCommands << "<br>GenerateVectorSpaceClosedWRTLieBracket{}(248," ;
      reportCalculatorCommands << "<br>GenerateVectorSpaceClosedWRTLieBracket{}(248," ;
      for (int j=0; j<theGeneratorsItry.size; j++)
      { reportFourierTransformedCalculatorCommands << theGeneratorsItry[j].ToString();
        reportCalculatorCommands << theGeneratorsItry[j].ToString();
        if (j!=theGeneratorsItry.size-1)
        { reportFourierTransformedCalculatorCommands << ", ";
          reportCalculatorCommands << ", ";
        }
      }
      reportFourierTransformedCalculatorCommands << ");";
      reportFourierTransformedCalculatorCommands << "<hr>";
      reportCalculatorCommands << ");";
      reportCalculatorCommands << "<hr>";
    }
//    theQDOs[0].GenerateBasisLieAlgebra(theQDOs, &theWeylFormat, theCommands.theGlobalVariableS);
//    std::cout << "<br><b>Dimension generated Lie algebra: " << theQDOs.size << "</b>";
//    std::cout << "<br>The qdos: ";
//    for (int j=0; j<theQDOs.size; j++)
 //     std::cout << "<br>" << theQDOs[j].ToString();
  }
  latexReport << "\\end{longtable}";
  out << "</table>";
  out << reportCalculatorCommands.str();
  out << reportFourierTransformedCalculatorCommands.str();
  out << "<br>" << latexReport.str();
  out << "<br><br>" << latexReport2.str();
  return output.AssignValue<std::string>(out.str(), theCommands);
}

template <class coefficient>
std::string ModuleSSalgebra<coefficient>::ToString(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("ModuleSSalgebra::ToString");
  if (this->owneR==0)
    return "(Error: module not initialized)";
  SemisimpleLieAlgebra& theAlgebrA=*this->owneR;
  WeylGroup theWeyl;
  theWeyl=theAlgebrA.theWeyl;
  std::stringstream out;
  GlobalVariables theGlobalVariables;
  out << "<br>Semisimple Lie algebra acting on generalized Verma module: " << theAlgebrA.GetLieAlgebraName() << ".";
  out << "<br>Parabolic selection: " << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
  out << "<br>Highest weight of Generalized Verma module in fundamental coordinates: " << this->theHWFundamentalCoordsBaseField.ToString();
  out << "<br>In simple coordinates: " << this->theHWSimpleCoordSBaseField.ToString();
  out << "<br>Finite dimensional part h. w. fundamental coordinates: " << this->theHWFDpartFundamentalCoordS.ToString();
  out << "<br>Finite dimensinoal part h. w. simple coords: " << this->theHWFDpartSimpleCoordS.ToString();
  out << "<br>Inducing module character (over the Cartan subalgebra): ";
  FormatExpressions latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
  if (this->theCharOverH.size()<100)
    out << CGI::GetMathMouseHover(this->theCharOverH.ToString(&latexFormat));
  else
    out << this->theCharOverH.ToString();
  out << "<br>Dimensionn of the finite dimensional part of the module: " << this->GetDim();
  out << "<br>A module basis follows.";
  out << "<table><tr><td>Monomial label</td><td>Definition</td><td>Littelmann path string</td></tr>";
  ElementWeylGroup tempWelt;
  int wordCounter=0;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<coefficient> >& currentList=
    this->theGeneratingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentListInt=
    this->theGeneratingWordsIntGrouppedByWeight[i];
    for (int j=0; j<currentList.size; j++)
    { wordCounter++;
      tempWelt.reflections.SetSize(currentListInt[j].generatorsIndices.size);
      for (int k=0; k<currentListInt[j].generatorsIndices.size; k++)
        tempWelt.reflections[k]=theWeyl.RootsOfBorel.size-1 -currentListInt[j].generatorsIndices[k];
      out << "<tr><td>m_{ " << wordCounter << "} </td><td>" << currentList[j].ToString(&theGlobalVariables.theDefaultFormat)
      << "  v_\\lambda</td><td>" << tempWelt.ToString() << "</td> </tr>";
    }
  }
  out << "</table>";

  out << "<br>Character: " << this->theChaR.ToString();
  out << "<br>Computed generator actions (" << this->ComputedGeneratorActions.CardinalitySelection << " out of "
  << this->actionsGeneratorsMaT.size << " computed actions) follow. Note that generator actions are computed on demand, only the simple "
  << " Chevalley generators are computed by default. ";
  out << "<table><tr><td>Generator </td><td>Action</td></tr>";
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  for (int i=0; i<this->actionsGeneratorsMaT.size; i++)
    if (this->ComputedGeneratorActions.selected[i])
    { tempSSElt.MakeGenerator(i, theAlgebrA);
      out << "<tr>";
      out << "<td>" << CGI::GetMathMouseHover(tempSSElt.ToString(theFormat)) << "</td>";
      out << "<td>";
      if (this->GetDim()<28)
      { Matrix<coefficient> outputMat;
        this->actionsGeneratorsMaT[i].GetMatrix(outputMat, this->GetDim());
        //std::cout << outputMat.ToString(&latexFormat);
        out << CGI::GetMathMouseHover(outputMat.ToString(&latexFormat), 5000) << " = ";
        out << this->actionsGeneratorsMaT[i].ToString();
      }
      else
        out << this->actionsGeneratorsMaT[i].ToString();
      out << "</td></tr>";
    }
  out << "</table>";
  std::string tempS;
  DrawingVariables theDV;
  this->theCharOverH.DrawMeAssumeCharIsOverCartan(theWeyl, theGlobalVariables, theDV);
  out << " A picture of the weight support follows. " << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());

  bool isBad=false;
  for (int k=0; k<this->theBilinearFormsAtEachWeightLevel.size; k++)
  { Matrix<coefficient>& theBF=this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<coefficient>& theBFinverted= this->theBilinearFormsInverted[k];
    out << "<hr>weight in simple coords: "
    << this->theModuleWeightsSimpleCoords[k].ToString();
    List<MonomialUniversalEnveloping<coefficient> >& currentList=this->theGeneratingWordsGrouppedByWeight[k];
    for (int i=0; i<currentList.size; i++)
    { MonomialUniversalEnveloping<coefficient>& currentElt=currentList[i];
      out << "<br>monomial " << i+1 << ": " << currentElt.ToString(&theGlobalVariables.theDefaultFormat);
    }
    out << "; Matrix of Shapovalov form associated to current weight level: <br> " << theBF.ToString(&theGlobalVariables.theDefaultFormat);
/*    if (!theBF.IsPositiveDefinite())
    { monomialDetailStream << "<b>Is not positive definite!</b>";
      this->flagConjectureCholds=false;
    }
    else
      monomialDetailStream << " (positive definite)";*/
    if (!theBF.IsNonNegativeAllEntries())
      out << "<b>Has negative entries</b>";
    else
      out << " (positive entries only )";
    out << " corresonding inverted matrix:<br>";
    if (theBFinverted.NumRows>0)
      out << theBFinverted.ToString(&theGlobalVariables.theDefaultFormat);
    else
    { out << "<b>The matrix of the bilinear form is not invertible!</b>";
      isBad=true;
    }
  }
  if (isBad)
    out << "<br>Error: the Littelmann-path induced monomials do not give a monomial basis. ";
  out << "<br>Cached Shapovalov products before generator action computation: " << this->cachedPairs.size << ". Dimension : " << this->GetDim();
  ElementSemisimpleLieAlgebra<Rational> tempSSelt;
  out << "<br>Cached Shapovalov products final: " << this->cachedPairs.size << "; dimension : " << this->GetDim();
  return out.str();
}

bool Calculator::innerHWVCommon
(Calculator& theCommands, Expression& output, Vector<RationalFunctionOld>& highestWeightFundCoords, Selection& selectionParSel,
 Expression& hwContext, SemisimpleLieAlgebra* owner, bool Verbose)
{ MacroRegisterFunctionWithName("Calculator::innerHWVCommon");
  RecursionDepthCounter therecursionIncrementer(&theCommands.RecursionDeptH);
  //  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  ListReferences<ModuleSSalgebra<RationalFunctionOld> >& theMods= theCommands.theObjectContainer.theCategoryOmodules;
  int indexOfModule=-1;

  for (int i=0; i<theMods.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& currentMod=theMods[i];
    if (highestWeightFundCoords==currentMod.theHWFundamentalCoordsBaseField && selectionParSel==currentMod.parabolicSelectionNonSelectedAreElementsLevi && currentMod.owneR==owner)
    { indexOfModule=i;
      break;
    }
  }
  if (indexOfModule==-1)
  { indexOfModule=theMods.size;
    theMods.SetSize(theMods.size+1);
    theMods.LastObject().reset();
  }
  ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[indexOfModule];
  if (!theMod.flagIsInitialized)
  { bool isGood=theMod.MakeFromHW(*owner, highestWeightFundCoords, selectionParSel, *theCommands.theGlobalVariableS, RFOne, RFZero, &report);
    if (Verbose)
      theCommands.Comments << theMod.ToString();
    if (!isGood)
      return output.SetError("Error while generating highest weight module. See comments for details. ", theCommands);
  }
  if (&theMod.GetOwner()!=owner)
    crash << "This is a programming error: module has owner that is not what it should be. " << crash;
  theElt.MakeHWV(theMod, RFOne);
  if (&theElt.GetOwnerSS()!=owner)
    crash << "This is a programming error: just created an ElementTensorsGeneralizedVermas whose owner is not what it should be. " << crash;
  return output.AssignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(theElt, hwContext, theCommands);
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetFDchar(charSSAlgMod<coefficient>& output)
{ output.MakeZero();
  if (this->theHWFundamentalCoordsBaseField.size<=0)
    return;
  MonomialChar<coefficient> tempMon;
  tempMon.owner=&this->GetOwner();
  WeylGroup& theWeyl=this->GetOwner().theWeyl;
  for (int i =0; i<this->theModuleWeightsSimpleCoords.size; i++)
  { tempMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(this->theModuleWeightsSimpleCoords[i]);
    output.AddMonomial(tempMon, this->theGeneratingWordsGrouppedByWeight[i].size);
  }
}

bool Calculator::innerSplitGenericGenVermaTensorFD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError
    ("Function innerSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, weight, weight. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& fdWeightNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, leftE, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<RationalFunctionOld> highestWeightFundCoords;
  Expression hwContext(theCommands);
  if (!theCommands.GetVectoR<RationalFunctionOld>(genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, Calculator::innerRationalFunction))
  { theCommands.Comments
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
    return false;
  }
  Vector<Rational> theFDhw;
  if (!theCommands.GetVectoR<Rational>(fdWeightNode, theFDhw, 0, theRank, 0))
  { theCommands.Comments
    << "Failed to convert the second argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " rationals. The second argument you gave is " << fdWeightNode.ToString() << ".";
    return false;
  }
//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  int theNumVars=hwContext.ContextGetNumContextVariables();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  Selection selParSel1, selFD;
  Expression hwvFD, hwvGenVerma;
  selParSel1.MakeFullSelection(theRank);
  selFD.init(theRank);
  int theCoeff;
  for (int i=0; i<theRank; i++)
  { if (highestWeightFundCoords[i].IsSmallInteger(&theCoeff))
      if (theCoeff>=0)
        selParSel1.RemoveSelection(i);
    bool isGood=false;
    if (theFDhw[i].IsSmallInteger(&theCoeff))
      if (theCoeff>=0)
        isGood=true;
    if (!isGood)
      return output.SetError("Error: the third argument of innerSplitGenericGenVermaTensorFD must be a list of small non-negative integers.", theCommands);
  }
  if (!theCommands.innerHWVCommon(theCommands, hwvGenVerma, highestWeightFundCoords, selParSel1, hwContext, theSSalgebra))
    return false;
  if (hwvGenVerma.IsError())
  { output=hwvGenVerma;
    return true;
  }
  Vector<RationalFunctionOld> theFDhwRF;
  theFDhwRF=theFDhw;
  if (!theCommands.innerHWVCommon(theCommands, hwvFD, theFDhwRF, selFD, hwContext, theSSalgebra))
    return false;
  if (hwvFD.IsError())
  { output=hwvFD;
    return true;
  }
  std::stringstream out;
  out << "hwv par sel: " << hwvGenVerma.ToString() << "hwv fd: " << hwvFD.ToString();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWgenVerma= hwvGenVerma.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWfd=hwvFD.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();

  ModuleSSalgebra<RationalFunctionOld>& theGenMod=theHWgenVerma[0].theMons[0].GetOwner();
  //int indexGenMod=theHWgenVerma[0].theMons[0].indexInOwner;
  ModuleSSalgebra<RationalFunctionOld>& theFDMod=theHWfd[0].theMons[0].GetOwner();
  if (theGenMod.owneR!=theFDMod.owneR ||
      theGenMod.GetOwner().GetRank()!=theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize ||
      theFDMod.GetOwner().GetRank()!=theFDMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize
      )
    crash << "This is a programming error: the two modules have owners, " << theFDMod.GetOwner().theWeyl.theDynkinType.ToString()
    << " and " << theGenMod.GetOwner().theWeyl.theDynkinType.ToString() << ", and parabolic selections of max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << " and " << theFDMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << crash;
  //int indexFDMod=theHWfd[0].theMons[0].indexInOwner;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir, theCasimirMinusChar;
  charSSAlgMod<RationalFunctionOld> theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.MakeFromWeight(theFDMod.theHWSimpleCoordSBaseField, theSSalgebra);
  List<ElementUniversalEnveloping<RationalFunctionOld> > theLeviEigenVectors;
  Vectors<RationalFunctionOld> theEigenVectorWeightsFund;
  if (theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize!=theGenMod.GetOwner().GetRank())
    crash << "This is a programming error: module has parabolic selection with max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << " but the ambient semisimple Lie algebra is of rank "
    << theGenMod.GetOwner().GetRank() << ". " << crash;
  std::string report;
  theFDMod.SplitOverLevi
  (&report, theGenMod.parabolicSelectionNonSelectedAreElementsLevi, *theCommands.theGlobalVariableS, RFOne, RFZero,
   &theLeviEigenVectors, &theEigenVectorWeightsFund, 0, &theFDLeviSplit);
  theFDMod.GetFDchar(theFDChaR);
  List<ElementUniversalEnveloping<RationalFunctionOld> > theCentralCharacters;
  theCasimir.MakeCasimir(*theSSalgebra);
  Vector<RationalFunctionOld> currentHWsimplecoords, currentHWdualcoords, currentWeightDiff;
  FormatExpressions tempFormat;
  tempFormat.MaxLineLength=60;
  tempFormat.flagUseLatex=true;
  tempFormat.fundamentalWeightLetter="\\psi";
  tempFormat.CustomPlusSign="\\oplus ";
  hwContext.ContextGetFormatExpressions(tempFormat);
  out << "<br>Character of finite dimensional module:" << CGI::GetMathMouseHover(theFDChaR.ToString());
  if (theGenMod.parabolicSelectionSelectedAreElementsLevi.CardinalitySelection>0)
    out << "<br>theFDChar split over levi:" << CGI::GetMathMouseHover(theFDLeviSplit.ToString(&tempFormat));
  //out << "<br> Splitting report: " << report;
  std::stringstream latexReport1;
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Character</td></tr>";
  latexReport1 << " \\begin{longtable}{rl}\\caption{\\label{table" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "GenVermatimes7DimCentralCharacters} $" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "$- parabolic $\\bar{\\mathfrak{p}}$} \\\\ $\\mu+\\gamma$ & Action of $\\bar c$\\\\\\hline";
  tempFormat.CustomPlusSign="";
  tempFormat.chevalleyGgeneratorLetter="\\bar{g}";
  tempFormat.chevalleyHgeneratorLetter="\\bar{h}";
  theFDLeviSplitShifteD.MakeZero();
  MonomialChar<RationalFunctionOld> tempMon;
  tempMon.owner=theSSalgebra;
  ElementUniversalEnveloping<RationalFunctionOld> currentChar;
  for (int i=0; i<theLeviEigenVectors.size; i++)
  { tempMon.weightFundamentalCoordS=theEigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoordS+=theGenMod.theHWFundamentalCoordsBaseField;
    theFDLeviSplitShifteD.AddMonomial(tempMon, RFOne);
    currentHWdualcoords=theSSalgebra->theWeyl.GetDualCoordinatesFromFundamental(tempMon.weightFundamentalCoordS);
    currentChar=theCasimir;
    currentChar.ModOutVermaRelations(theCommands.theGlobalVariableS, & currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.AddOnTop(currentChar);
    out << "<tr><td>" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi") << "</td><td>"
    << currentChar.ToString(&tempFormat) << "</td></tr>";
    latexReport1 << " $" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i+1 <<"}:=$ $" << currentChar.ToString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFunctionOld> tempElt, tempElt2;
  theFDMod.highestWeightVectorNotation="v";
  theGenMod.highestWeightVectorNotation="w";
  out << "Let w be the highest weight vector of the generalized Verma component, and let v be the highest weight vector of the finite dimensional component";
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Algebraic expression</td><td>Additional multiplier</td>";
  if (theNumVars==1)
    out << "<td>gcd polynomial coeffs</td>";
  out << "<td>the hwv</td>";
  if (theNumVars==1)
    out << "<td>gcd divided out</td>";
  out << "</tr>";
  std::stringstream latexReport2;
  latexReport2 << "\\begin{longtable}{p{2.5cm}p{2.5cm}p{1.5cm}l}\\caption{\\label{tableDecompo"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString() << "times7dim}Decomposition for the $"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "$-parabolic subalgebra $\\bar{\\mathfrak{p}}$ } \\\\ Weight & Projector applied to &"
  << " Extra multiplier & Resulting $\\bar {\\mathfrak b}$-singular vector \\endhead\\hline";
  //std::cout << theGenMod.theGeneratingWordsNonReduced.ToString();
  for (int i=0; i<theCentralCharacters.size; i++)
  { Vector<RationalFunctionOld> currentWeightSimpleCoords=
    theSSalgebra->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[i]);
    tempElt.MakeHWV(theFDMod, RFOne);
    tempElt.MultiplyOnTheLeft(theLeviEigenVectors[i], theElt, *theSSalgebra, *theCommands.theGlobalVariableS, RFOne, RFZero);
    tempElt.MakeHWV(theGenMod, RFOne);
//      tempElt.MultiplyOnTheLeft
//      (theGenMod.theGeneratingWordsNonReduced[0], tempElt2, theMods, theSSalgebra,
//         *theCommands.theGlobalVariableS,
//       RFOne, RFZero);
    theElt.TensorOnTheRight(tempElt, *theCommands.theGlobalVariableS, RFOne, RFZero);
    theElt*=-1;
    std::string startingEltString=theElt.ToString(&tempFormat);
    std::stringstream tempStream, tempStream2;
    tempStream << "\\begin{array}{l}";
    bool found=false;
    for (int j=0; j<theCentralCharacters.size; j++)
    { Vector<RationalFunctionOld> otherWeightSimpleCoords=
      theSSalgebra->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[j]);
      if ((otherWeightSimpleCoords-currentWeightSimpleCoords).IsPositive())
      { theCasimirMinusChar=theCasimir;
        theCasimirMinusChar-=theCentralCharacters[j];
        theElt.MultiplyOnTheLeft(theCasimirMinusChar, tempElt2, *theSSalgebra, *theCommands.theGlobalVariableS, RFOne, RFZero);
        theElt=tempElt2;
        tempStream << "(i(\\bar c)- (" << theCentralCharacters[j].ToString() << ") )\\\\";
        tempStream2 << " $(\\bar c-p_" << j+1 << ") $ ";
        found=true;
      }
    }
    if (found)
    { tempStream << "\\cdot";
      tempStream2 << " $\\cdot$ ";
    }
    tempStream << "(" << startingEltString << ")";
    tempStream << "\\end{array}";
    tempStream2 << " $(" << startingEltString << ")$ ";
//      std::cout << "<hr><hr>(" << theElt.ToString();
    Rational tempRat= theElt.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    currentHWsimplecoords=theGenMod.theHWSimpleCoordSBaseField;
    currentHWsimplecoords+=theFDMod.theModuleWeightsSimpleCoords[i];
//      std::cout << ") * " << tempRat.ToString()  << "<hr>=" << theElt.ToString() << "<hr><hr>";
    out << "<tr><td>"
    << theSSalgebra->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "</td><td>" << CGI::GetMathMouseHover(tempStream.str()) << "</td><td>" << tempRat.ToString() << "</td>";
    latexReport2
    << "$" << theSSalgebra->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "$ &  " << tempStream2.str() << " &" << tempRat.ToString();
    Polynomial<Rational> tmpGCD, tmpRF;
    tempFormat.MaxLineLength=80;
    if (theNumVars==1)
    { tmpGCD= theElt.FindGCDCoefficientNumeratorsOverRationals();
      tmpGCD.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
      out << "<td>" << CGI::GetMathMouseHover(tmpGCD.ToString(&tempFormat)) << "</td>";
    }
    out << "<td>" << CGI::GetMathMouseHover(theElt.ToString(&tempFormat)) << "</td>";
    latexReport2 << "&$\\begin{array}{l}" << theElt.ToString(&tempFormat) << "\\end{array}$\\\\<br>";
    if (theNumVars==1)
    { tmpRF=tmpGCD;
      theElt/=tmpRF;
      out << "<td>" << CGI::GetMathMouseHover("\\begin{array}{l}" + theElt.ToString(&tempFormat)+"\\end{array}") << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  latexReport2 << "\\end{longtable}";
  out << "<br>Ready LaTeX (table 1 and 2): <br><br><br>" << latexReport1.str() << "<br><br><br>" << latexReport2.str() << "<br>";
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerFunctionToMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerFunctionToMatrix");
//  std::cout << input.ToString();
  if (!input.IsListNElements(4))
    return false;
  const Expression& leftE  =input[1];
  const Expression& middleE=input[2];
  const Expression& rightE =input[3];
//  std::cout << leftE.ToString() << ", " << rightE.ToString() << ", " << middleE.ToString();
  int numRows, numCols;
  if (!middleE.IsSmallInteger(&numRows) || !rightE.IsSmallInteger(&numCols))
    return false;
//  std::cout << "<br>Rows, cols: " << numRows << ", " << numCols;
  if (numRows<=0 || numCols<=0)
    return false;
  if (numRows>1000 || numCols>1000)
  { theCommands.Comments << "Max number of rows/columns is 1000. You requested " << numRows << " rows and " << numCols << " columns.<br>";
    return false;
  }
  output.reset(theCommands, numRows+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.format=output.formatMatrix;
  Expression theIndices, currentRow, currentE, leftIE, rightIE;
  for (int i=0; i<numRows; i++)
  { currentRow.reset(theCommands, numCols+1);
    currentRow.AddChildAtomOnTop(theCommands.opSequence());
    currentRow.format=currentRow.formatMatrixRow;
    for (int j=0; j<numCols; j++)
    { leftIE.AssignValue(i+1, theCommands);
      rightIE.AssignValue(j+1, theCommands);
      currentE.reset(theCommands, 3);
      currentE.AddChildOnTop(leftE);
      currentE.AddChildOnTop(leftIE);
      currentE.AddChildOnTop(rightIE);
      currentRow.AddChildOnTop(currentE);
    }
    output.AddChildOnTop(currentRow);
  }
  return true;
}

bool Calculator::innerGetChevGen(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerGetChevGen");
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], theSSalg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex>theSSalg->GetNumPosRoots() || theIndex==0 || theIndex<-theSSalg->GetNumPosRoots())
    return output.SetError("Bad Chevalley-Weyl generator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  if (theIndex>0)
    theIndex+=theSSalg->GetRank()-1;
  theIndex+=theSSalg->GetNumPosRoots();
  theElt.MakeGenerator(theIndex, *theSSalg);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int indexInOwner=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*theSSalg);
  //theCommands.ToString();
  theContext.ContextMakeContextSSLieAlgebrA(indexInOwner, theCommands);
  //theCommands.ToString();
  output.AssignValueWithContext(theUE, theContext, theCommands);
  //theCommands.ToString();
  return true;
}

bool Calculator::innerGetCartanGen(Calculator& theCommands, const Expression& input, Expression& output)
{ //std::cout << "<br>Here I am with input: " << input.ToString();
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg=0;
//  std::cout << "<br>before calling inner ss: " << input.ToString();
//  std::cout.flush();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input[1], theSSalg))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  if (theSSalg==0)
    crash << "This is a programming error: called conversion function successfully, but the output is a zero pointer to a semisimple Lie algebra. "
    << crash;
//  std::cout << "<br>Here I am at next phase: " << input.ToString() << ", ss alg: " << theSSalg->ToString();
//  std::cout.flush();
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex<1 || theIndex>theSSalg->GetRank() )
    return output.SetError("Bad Cartan subalgebra genrator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH;
  theIndex--;
  theH.MakeEi(theSSalg->GetRank(), theIndex);
  theElt.MakeHgenerator(theH, *theSSalg);
//  std::cout << "<br>good before ue! " << input.ToString();
//  std::cout.flush();
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int theAlgIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*theSSalg);
  theContext.ContextMakeContextSSLieAlgebrA(theAlgIndex, theCommands);
  //std::cout << "<br>And the context is: " << theContext.ToString();
  return output.AssignValueWithContext(theUE, theContext, theCommands);
}

bool Calculator::fKLcoeffs(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::fKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input, theSSalgebra))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  std::stringstream out;
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (theWeyl.theDynkinType.GetSizeWeylGroupByFormula()>192)
  { out << "I have been instructed to run only for Weyl groups that have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra->GetLieAlgebraName() << " has " << theWeyl.theDynkinType.GetSizeWeylGroupByFormula() << ".";
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(1);
  theFormat.polyAlphabeT[0]="q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << theSSalgebra->GetLieAlgebraName();
  KLpolys theKLpolys;
  theKLpolys.ComputeKLPolys(&theWeyl);
  theFormat.flagUseHTML=true;
  out << theKLpolys.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerPrintSSLieAlgebra(Calculator& theCommands, const Expression& input, Expression& output, bool Verbose)
{ MacroRegisterFunctionWithName("Calculator::innerPrintSSLieAlgebra");
  SemisimpleLieAlgebra *tempSSpointer=0;
  input.CheckInitialization();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully(Serialization::innerSSLieAlgebra, input, tempSSpointer))
    return output.SetError("Error extracting Lie algebra.", theCommands);
  SemisimpleLieAlgebra& theSSalgebra=*tempSSpointer;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  FormatExpressions theFormat, latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
//      theFormat.chevalleyHgeneratorLetter="\\bar{h}";
//      theFormat.chevalleyGgeneratorLetter="\\bar{g}";
  out << "<hr>Lie algebra type: " << theWeyl.theDynkinType << ". ";
  out << "<br>Weyl group size: " << theWeyl.theDynkinType.GetSizeWeylGroupByFormula() << "." << "<br>To get extra details: ";
  std::stringstream tempStream;
  tempStream << "printSemisimpleLieAlgebra{}(" << theWeyl.theDynkinType << ")";
  out << theCommands.GetCalculatorLink(tempStream.str()) << "<br>";
  if (Verbose)
  { out << " The resulting Lie bracket pairing table follows. <hr> " << theSSalgebra.ToString(&theCommands.theGlobalVariableS->theDefaultFormat);
    out << "Ready for LaTeX consumption version of the first three columns: ";
    out << "<br>%Add to preamble: <br>\\usepackage{longtable} <br>%Add to body: <br>"
    << " \\begin{longtable}{ccc}generator & root simple coord. & root $\\varepsilon$-notation \\\\\\hline<br>\n";
    Vector<Rational> tempRoot, tempRoot2;
    ElementSemisimpleLieAlgebra<Rational> tempElt1;
    for (int i=0; i<theSSalgebra.GetNumGenerators(); i++)
    { tempElt1.MakeGenerator(i, theSSalgebra);
      tempRoot=theSSalgebra.GetWeightOfGenerator(i);
      theWeyl.GetEpsilonCoords(tempRoot, tempRoot2);
      out << "$" << tempElt1.ToString(&theFormat) << "$&$"<< tempRoot.ToString() << "$";
      out << "&$" << tempRoot2.ToStringLetterFormat("\\varepsilon") << "$";
      out << "\\\\\n";
    }
    out << "\\end{longtable}" << "<hr>";
  }
  out << "We define the symmetric Cartan matrix <br>by requesting that the entry in the i-th row and j-th column<br> "
  << " be the scalar product of the i^th and j^th roots. Symmetric Cartan matrix:<br>"
  << CGI::GetMathSpanPure(theWeyl.CartanSymmetric.ToString(&latexFormat));
  out << "Let the (i,j)^{th} entry of the symmetric Cartan matrix be a_{ij}. <br> Then we define the symmetric Cartan co-matrix as "
  << " the matrix whose (i,j)^{th} entry equals 4*a_{ij}/(a_{ii}*a_{jj}). In other words, the symmetric Cartan co-matrix is the "
  << "symmetric Cartan matrix of the dual root system. The symmetric Cartan co-matrix equals:<br>"
  << CGI::GetMathSpanPure(theWeyl.CoCartanSymmetric.ToStringLatex());
  out << "<br>The determinant of the symmetric Cartan matrix is: " << theWeyl.CartanSymmetric.GetDeterminant().ToString();
  /*  Rational theRatio;
    for (int j=0; j<theWeyl.GetDim(); j++)
    { theRatio=0;
      for (int i=0; i<theWeyl.RootSystem.size; i++)
      { Rational tempRat=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[j]);
        theRatio+=tempRat*tempRat;
      }
      theRatio.Invert();
      theRatio*=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[j], theWeyl.RootSystem[j]);
      Rational tempRat=theWeyl.GetKillingDivTraceRatio();
      tempRat.Invert();
  //    std::cout << "<br>" << j+1 << ": " << theRatio.ToString() << "=? " << tempRat.ToString();
    }*/
  //Lattice tempLattice;
  //theWeyl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  //out << "<br>The integral lattice in simple coordinates is (generated by): " << tempLattice.ToString(true, false);
  //Vectors<Rational> integralRoots, integralRootsEpsForm;
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  //integralRoots.AssignMatrixRows(tempLattice.basisRationalForm);
  //theWeyl.GetEpsilonCoords(integralRoots, integralRootsEpsForm, theGlobalVariables);
  //out << "<br>The integral lattice generators in epsilon format: " << integralRootsEpsForm.ElementToStringEpsilonForm();
  theWeyl.GetFundamentalWeightsInSimpleCoordinates(fundamentalWeights);
  Vectors<Rational> simpleBasis, simplebasisEpsCoords;
  out << "<hr> Half sum of positive roots: " << theWeyl.rho.ToString();
  Vector<Rational> tempRoot;
  theWeyl.GetEpsilonCoords(theWeyl.rho, tempRoot);
  out << "= " << CGI::GetMathSpanPure(tempRoot.ToStringLetterFormat("\\varepsilon"));
  out << "<hr>The fundamental weights (the j^th fundamental weight has scalar product 1 <br>with the j^th simple root times 2 divided by the root length squared,<br> "
  << " and 0 with the remaining simple roots): ";
  theWeyl.GetEpsilonCoords(fundamentalWeights, fundamentalWeightsEpsForm);
  out << "<table>";
  for (int i=0; i< fundamentalWeights.size; i++)
  { out << "<tr><td>" << fundamentalWeights[i].ToString() << "</td><td> =</td><td> " << CGI::GetMathSpanPure(fundamentalWeightsEpsForm[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  out << "<hr>Simple basis in epsilon coordinates: <table>";
  simpleBasis.MakeEiBasis(theWeyl.GetDim());
  theWeyl.GetEpsilonCoords(simpleBasis, simplebasisEpsCoords);
  for (int i=0; i< simplebasisEpsCoords.size; i++)
  { out << "<tr><td>" << simpleBasis[i].ToString() << " </td><td>=</td> <td>" << CGI::GetMathSpanPure(simplebasisEpsCoords[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  DynkinSimpleType tempSimpleType;
  if (theWeyl.theDynkinType.IsSimple(&tempSimpleType.theLetter, &tempSimpleType.theRank, &tempSimpleType.CartanSymmetricInverseScale))
    if (tempSimpleType.CartanSymmetricInverseScale==1)
    { //std::cout << "here i am";
      Matrix<Rational> tempM, tempM2;
      theWeyl.theDynkinType.GetEpsilonMatrix(tempM);
      tempM2=tempM;
      tempM2.Transpose();
      tempM2.MultiplyOnTheRight(tempM);
      if (!(tempM2==theWeyl.CartanSymmetric))
        crash << "This is a (non-critical) programming error: the epsilon coordinates of the vectors are incorrect. "
        << "Please fix function DynkinType::GetEpsilonMatrix. The matrix of the epsilon coordinates is " << tempM.ToString()
        << ", the Symmetric Cartan matrix is " << theWeyl.CartanSymmetric.ToString() << ", and the  "
        << "transpose of the epsilon matrix times the epsilon matrix:  " << tempM2.ToString() << ". " << crash;
    }
  if (Verbose)
  { out << "<hr>Root system:<table><tr><td>Simple basis coordinates</td><td></td><td>Epsilon coordinates non-LaTeX'ed (convention: see above)</td></tr> ";
    Vectors<Rational> rootSystemEpsCoords;
    theWeyl.GetEpsilonCoords(theWeyl.RootSystem, rootSystemEpsCoords);
    for (int i=0; i<theWeyl.RootSystem.size; i++)
    { const Vector<Rational>& current=theWeyl.RootSystem[i];
      out << "<tr><td>" << current.ToString() << "</td><td>=</td><td>" << rootSystemEpsCoords[i].ToStringLetterFormat("e") << "</td></tr>";
    }
    out << "</table>";
    out << "Comma delimited list of roots: ";
    for (int i=0; i<theWeyl.RootSystem.size; i++)
    { out << theWeyl.RootSystem[i].ToString();
      if (i!=theWeyl.RootSystem.size-1)
        out << ", ";
    }
    DrawingVariables theDV;
    theWeyl.DrawRootSystem(theDV, true, *theCommands.theGlobalVariableS, true, 0, true, 0);
    out << "<hr>Below a drawing of the root system in its corresponding Coxeter plane (computed as explained on John Stembridge's website). "
    << "<br>The darker red dots can be dragged with the mouse to rotate the picture.<br>The grey lines are the edges of the Weyl chamber.<br>"
    << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  }
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool Expression::HasBoundVariables()const
{ if (this->theBoss==0)
    crash << "This is a programming error: calling function HasBoundVariables on non-initialized expression. " << crash;
  RecursionDepthCounter recursionCounter(&this->theBoss->RecursionDeptH);
  MacroRegisterFunctionWithName("Expression::HasBoundVariables");
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
    crash << "This is a programming error: function HasBoundVariables has exceeded recursion depth limit. " << crash;
  if (this->IsListOfTwoAtomsStartingWith(this->theBoss->opBind()))
    return true;
  for (int i=0; i<this->children.size; i++)
    if ((*this)[i].HasBoundVariables())
      return true;
  return false;
}

bool Calculator::innerNot(Calculator& theCommands, const Expression& input, Expression& output)
{ int theInt;
  if (!input.IsSmallInteger(&theInt))
    return false;
  if (theInt==0)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool Calculator::innerIsInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ if (input.HasBoundVariables())
    return false;
  if (input.IsInteger())
    output.AssignValue(1, theCommands);
  else
    output.AssignValue(0, theCommands);
  return true;
}

bool Calculator::innerIsRational(Calculator& theCommands, const Expression& input, Expression& output)
{ if (input.HasBoundVariables())
    return false;
  if (input.IsOfType<Rational>())
    output.AssignValue(1, theCommands);
  else
    output.AssignValue(0, theCommands);
  return true;
}

bool Calculator::AppendOpandsReturnTrueIfOrderNonCanonical(const Expression& input, List<Expression>& output, int theOp)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
    return false;
  bool result=false;
  if (!input.IsListStartingWithAtom(theOp))
    output.AddOnTop(input);
  else
    for (int i=1; i<input.children.size; i++)
    { if (this->AppendOpandsReturnTrueIfOrderNonCanonical(input[i], output, theOp))
        result=true;
      if (i<input.children.size-1 && input[i].IsListStartingWithAtom(theOp) && input[i].children.size>2)
        result=true;
    }
  return result;
}

Expression::FunctionAddress Calculator::GetInnerFunctionFromOp(int theOp, const Expression& left, const Expression& right)
{ crash << "Function Calculator::GetfOp not implemented yet. " << crash;
  return 0;
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::TensorOnTheRight
(const ElementTensorsGeneralizedVermas<coefficient>& right, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero)
{ MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<coefficient>::TensorOnTheRight");
  if (right.IsEqualToZero())
  { this->MakeZero();
    return;
  }
  int maxNumMonsFinal=this->size()*right.size();
  ElementTensorsGeneralizedVermas<coefficient> output;
  MonomialTensorGeneralizedVermas<coefficient> bufferMon;
  output.MakeZero();
  output.SetExpectedSize(maxNumMonsFinal);
  coefficient theCoeff;
  for (int i=0; i<right.size(); i++)
    for (int j=0; j<this->size(); j++)
    { bufferMon=(*this)[j];
      bufferMon*=(right[i]);
      theCoeff=this->theCoeffs[j];
      theCoeff*=right.theCoeffs[i];
      output.AddMonomial(bufferMon, theCoeff);
      ParallelComputing::SafePointDontCallMeFromDestructors();
    }
  *this=output;
}

bool Calculator::outerTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ToString();
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::StandardTensor");
  if (theCommands.outerDistribute
      (theCommands, input, output, theCommands.opPlus(), theCommands.opTensor()))
    return true;
  //std::cout << "<br>After distribute: " << theExpression.ToString();
  if (theCommands.outerAssociate(theCommands, input, output))
    return true;
  if (theCommands.outerExtractBaseMultiplication(theCommands, input, output))
    return true;
//  if (theExpression.children.size!=2)
//    return false;
  //std::cout << "<br>After do associate: " << theExpression.ToString();
  return false;
}

bool Calculator::innerCollectMultiplicands(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerCollectMultiplicands");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
//  std::cout << "<hr>Collecting multiplicands. input: " << input.ToString();
  Expression constPower, thePower;
  const Expression* left= &input[1];
  const Expression* right=&input[2];
  if (*left==*right)
  { constPower.AssignValue(2, theCommands);
    output.MakeXOX(theCommands, theCommands.opThePower(), *left, constPower);
    //std::cout << "<br>output be at first place: " << output.ToString();
    return true;
  }
  for (int i=0; i<2; i++, MathRoutines::swap(left, right))
    if (left->IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    { if ((*left)[1]==(*right))
      { constPower.AssignValue(1, theCommands);
        thePower.MakeXOX(theCommands, theCommands.opPlus(), (*left)[2], constPower);
        return output.MakeXOX(theCommands, theCommands.opThePower(), *right, thePower);
      }
      if (right->IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
        if ((*left)[1]==(*right)[1])
        { thePower.MakeXOX(theCommands, theCommands.opPlus(), (*left)[2], (*right)[2]);
          output.MakeXOX(theCommands, theCommands.opThePower(), (*left)[1], thePower);
          //std::cout << "<br>output be at second place: " << output.ToString();
          return true;
        }
    }
  return false;
}

bool Calculator::outerCombineFractions(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus(), 3))
    return false;
  const Expression* quotientE=0;
  const Expression* summandE=0;
  if (input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { quotientE=&input[1];
    summandE=&input[2];
  } else if (input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { quotientE=&input[2];
    summandE=& input[1];
  } else
    return false;
  const Expression& quotientNumeratorE=(*quotientE)[1];
  const Expression& quotientDenominatorE=(*quotientE)[2];
  Expression outputSummandE, outputNumE;
  outputSummandE.MakeXOX(theCommands, theCommands.opTimes(), *summandE, quotientDenominatorE);
  outputNumE.MakeXOX(theCommands, theCommands.opPlus(), quotientNumeratorE, outputSummandE);
  return output.MakeXOX(theCommands, theCommands.opDivide(), outputNumE, quotientDenominatorE);
}

bool Calculator::outerCheckRule(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDefine(), 3))
    return false;
  if (input[1]!=input[2])
    return false;
  std::stringstream out;
  out << "Bad rule: you are asking me to substitute " << input[1] << " by itself (" << input[2] << ")" << " which is an infinite substitution cycle. ";
  return output.SetError(out.str(), theCommands);
}

bool Calculator::innerSubZeroDivAnythingWithZero(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsEqualToZero())
    if (!input[2].IsEqualToZero())
      return output.AssignValue(0, theCommands);
  return false;
}

bool Calculator::innerCancelMultiplicativeInverse(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1][2]==input[2])
  { output=input[1][1];
    return true;
  }
  return false;
}

bool Calculator::outerAssociateTimesDivision(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  Expression newLeftE;
  newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][1]);
  output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][2]);
  return true;
}

bool Calculator::outerAssociate(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(-1, 3))
    return false;
  int theOperation=input[0].theData;
  if (!input[1].IsListNElementsStartingWithAtom(theOperation, 3))
    return false;
  Expression newRight;
  newRight.MakeXOX(theCommands, theOperation, input[1][2], input[2]);
  output.MakeXOX(theCommands, theOperation, input[1][1], newRight);
  return true;
}

bool Calculator::StandardIsDenotedBy(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::StandardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElementsStartingWithAtom(theCommands.opIsDenotedBy(), 3))
    return false;
  const Expression& withNotation=input[2];
  const Expression& theNotation=input[1];
  theCommands.Comments << "<br>Registering notation: globally, " << withNotation.ToString() << " will be denoted by "
  << theNotation.ToString();
  theCommands.theObjectContainer.ExpressionNotation.AddOnTop(theNotation.ToString());
  theCommands.theObjectContainer.ExpressionWithNotation.AddOnTop(withNotation);
  output=input;
  output.SetChildAtomValue(0, theCommands.opDefine());
  ////
  if (withNotation.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    if (withNotation.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().IsHWV())
    { MonomialGeneralizedVerma<RationalFunctionOld>& theElt=
      withNotation.GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()[0].theMons[0];
      theElt.GetOwner().highestWeightVectorNotation=theNotation.ToString();
    }
  return true;
}

bool Calculator::innerMultiplyByOne(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListStartingWithAtom(theCommands.opTimes()) || input.children.size!=3)
    return false;
  if (!input[1].IsEqualToOne())
    return false;
  output=input[2];
  return true;
}

bool Calculator::outerTimesToFunctionApplication(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerTimesToFunctionApplication");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes()))
    return false;
  if (input.children.size<2)
    return false;
  const Expression& firstElt=input[1];
//  std::cout << " <hr>outer times to function ";
  if (!firstElt.IsBuiltInAtom())
  { if (!firstElt.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
      return false;
    if (!firstElt[1].IsAtomWhoseExponentsAreInterprettedAsFunction())
      return false;
  }
  if (firstElt.IsAtomNotInterprettedAsFunction())
    return false;
  const Expression& secondElt=input[2];
  if (secondElt.IsSequenceNElementS())
  { output=secondElt;
    return output.SetChilD(0, firstElt);
  }
  output=input;
  output.children.RemoveIndexShiftDown(0);
  return true;
}

bool Calculator::outerDistribute(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp)
{ if (theCommands.flagDontDistribute)
    return false;
  if (theCommands.outerLeftDistributeBracketIsOnTheLeft(theCommands, input, output, AdditiveOp, multiplicativeOp))
    return true;
  return theCommands.outerRightDistributeBracketIsOnTheRight(theCommands, input, output, AdditiveOp, multiplicativeOp);
}

bool Calculator::outerLeftDistributeBracketIsOnTheLeft(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp)
{ if (!input.IsListNElementsStartingWithAtom(-1, 3))
    return false;
  int theAdditiveOp=theCommands.opPlus();
  int theMultiplicativeOP=input[0].theData;
  if (!input[1].IsListNElementsStartingWithAtom(theAdditiveOp,3))
    return false;
//  int theFormat=input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOP, input[1][1], input[2]);
  rightE.MakeXOX(theCommands, theMultiplicativeOP, input[1][2], input[2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::outerRightDistributeBracketIsOnTheRight(Calculator& theCommands, const Expression& input, Expression& output, int AdditiveOp, int multiplicativeOp)
{ if (!input.IsListNElementsStartingWithAtom(-1, 3))
    return false;
  int theAdditiveOp=theCommands.opPlus();
  int theMultiplicativeOP=input[0].theData;
  if (!input[2].IsListNElementsStartingWithAtom(theAdditiveOp, 3))
    return false;
//  int theFormat=input.format;
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theMultiplicativeOP, input[1], input[2][1]);
  rightE.MakeXOX(theCommands, theMultiplicativeOP, input[1], input[2][2]);
  return output.MakeXOX(theCommands, theAdditiveOp, leftE, rightE);
}

bool Calculator::CollectSummands(Calculator& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum)
{ MacroRegisterFunctionWithName("Calculator::CollectSummands");
  List<Expression> summands;
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical(input, summands);
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  outputSum.MakeZero();
  for (int i=0; i<summands.size; i++)
  { const Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
    Rational theCoeff=1;
    if (currentSummandNoCoeff->IsListStartingWithAtom(theCommands.opTimes()))
    { if((*currentSummandNoCoeff)[1].IsOfType<Rational>(&theCoeff))
        currentSummandNoCoeff=&((*currentSummandNoCoeff)[2]);
    } else if (currentSummandNoCoeff->IsOfType<Rational>(&theCoeff))
      currentSummandNoCoeff=&oneE;
    outputSum.AddMonomial(*currentSummandNoCoeff, theCoeff);
  }
//  std::cout << "<hr> before mon sort, mon order: " << outputSum.theMonomials.ToString();
//  if (outputSum.theMonomials[0]>outputSum.theMonomials[1])
//    std::cout << outputSum.theMonomials[0].ToString() << " > " << outputSum.theMonomials[1].ToString();
//  else
//    std::cout << outputSum.theMonomials[0].ToString() << " < " << outputSum.theMonomials[1].ToString();
  outputSum.QuickSortDescending();

//  std::cout << " after mon sort: " << outputSum.theMonomials.ToString();
  return true;
}

bool Calculator::innerAssociateExponentExponent(Calculator& theCommands, const Expression& input, Expression& output)
{ int opPower=theCommands.opThePower();
  if (!input.IsListNElementsStartingWithAtom(opPower, 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(opPower, 3))
    return false;
  Expression tempE;
  tempE.MakeProducT(theCommands, input[1][2], input[2]);
  return output.MakeXOX(theCommands, opPower, input[1][1], tempE);
}

bool Calculator::outerPowerRaiseToFirst(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (input[2].IsEqualToOne())
  { std::string tempS;
//    if (input[1].IsOperation(&tempS))
//      if (tempS=="A" || tempS=="B" || tempS=="C" || tempS=="D" || tempS=="E" ||
//          tempS=="F" || tempS=="G")
//        return false;
    output=input[1];
    return true;
  }
  return false;
}

bool Expression::MakeXOdotsOX(Calculator& owner, int theOp, const List<Expression>& input)
{ MacroRegisterFunctionWithName("Expression::MakeXOdotsOX");
  if (input.size==0)
    crash << "This is a programming error: cannot create operation sequence from an empty list. " << crash;
  if (input.size==1)
  { *this=input[0];
    return true;
  }
  this->MakeXOX(owner, theOp, input[input.size-2], *input.LastObject());
  Expression result;
  for (int i=input.size-3; i>=0; i--)
  { result.reset(owner, 3);
    result.AddChildAtomOnTop(theOp);
    result.AddChildOnTop(input[i]);
    result.AddChildOnTop(*this);
    *this=result;
  }
  return true;
}

bool Expression::MakeSum(Calculator& theCommands, const MonomialCollection<Expression, Rational>& theSum)
{ MacroRegisterFunctionWithName("Expression::MakeSum");
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  if (theSum.IsEqualToZero())
    return this->AssignValue<Rational>(0, theCommands);
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(theSum.size());
  for (int i=0; i<theSum.size(); i++)
  { Expression& current=summandsWithCoeff[i];
    if (theSum[i]==oneE)
      current.AssignValue(theSum.theCoeffs[i], theCommands);
    else if (!theSum.theCoeffs[i].IsEqualToOne())
    { current.reset(theCommands, 3);
      current.AddChildAtomOnTop(theCommands.opTimes());
      current.AddChildValueOnTop(theSum.theCoeffs[i]);
      current.AddChildOnTop(theSum[i]);
    } else
      current=theSum[i];
  }
  if (summandsWithCoeff.size>=2)
    if (summandsWithCoeff[0]>summandsWithCoeff[1] && summandsWithCoeff[1]>summandsWithCoeff[0])
      crash << "This is a pgoramming error: bad comparison! " << crash;
  return this->MakeXOdotsOX(theCommands, theCommands.opPlus(), summandsWithCoeff);
}

bool Calculator::outerPlus(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerPlus");
//  theCommands.Comments << "<hr><hr>processing outer plus with input: " << input.Lispify();
  if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus()))
    return false;
  MonomialCollection<Expression, Rational> theSum;
  theCommands.CollectSummands(theCommands, input, theSum);
  return output.MakeSum(theCommands, theSum);
}

bool Calculator::EvaluateIf(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDefineConditional(), 4))
    return output.SetError("Error: operation :if := takes three arguments.", theCommands);
  Rational conditionRat;
  if (!input[1].IsOfType<Rational>(&conditionRat))
    return false;
  if (conditionRat.IsEqualToOne())
  { output=input[2];
    return true;
  }
  if (conditionRat.IsEqualToZero())
  { output=input[3];
    return true;
  }
  return false;
}

bool Calculator::outerGreaterThan(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  Rational leftRat, rightRat;
  if (!left.IsOfType<Rational> (&leftRat)|| ! right.IsOfType<Rational> (&rightRat))
    return false;
  if (leftRat>rightRat)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool Calculator::outerLessThan(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  Rational leftRat, rightRat;
  if (!left.IsOfType<Rational> (&leftRat)|| ! right.IsOfType<Rational>(&rightRat))
    return false;
  if (rightRat>leftRat)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool Calculator::outerEqualEqual(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& left=input[1];
  const Expression& right=input[2];
  if (left.HasBoundVariables() || right.HasBoundVariables())
    return false;
  if (left==right)
    return output.AssignValue(1, theCommands);
  else
    return output.AssignValue(0, theCommands);
}

bool Expression::SetError(const std::string& theError, Calculator& owner)
{ this->reset(owner, 2);
  this->AddChildAtomOnTop(owner.opError());
  return this->AddChildValueOnTop(theError);
}

bool Expression::IsSmallInteger(int* whichInteger)const
{ Rational theRat;
  if (!this->IsOfType<Rational>(&theRat))
    return false;
  return theRat.IsSmallInteger(whichInteger);
}

bool Expression::IsRealDouble(double* whichDouble)const
{ if (this->theBoss==0)
    return false;
  Expression doubleE;
  if (!this->theBoss->innerEvaluateToDouble(*this->theBoss, *this, doubleE))
    return false;
  if (!doubleE.IsOfType<double>())
    return false;
  if (whichDouble!=0)
    *whichDouble=doubleE.GetValue<double>();
  return true;
}

bool Expression::IsConstantNumber()const
{ if (this->theBoss==0)
    return false;
  if (this->IsAtomGivenData(this->theBoss->opPi()))
    return true;
  if (this->IsAtomGivenData(this->theBoss->opE()))
    return true;
//  std::cout << "testing for constant: " << this->ToString();
//  std::cout << " i am of type: " << this->theBoss->GetOperations()[(*this)[0].theData];
  return this->IsOfType<Rational>() || this->IsOfType<AlgebraicNumber>() || this->IsOfType<double>();
}

bool Expression::IsInteger(LargeInt* whichInteger)const
{ Rational theRat;
  if (!this->IsOfType<Rational>(&theRat))
    return false;
  return theRat.IsInteger(whichInteger);
}

bool Calculator::outerUnion(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opUnion()))
    return false;
  int numElts=1;
  for (int i=1; i<input.children.size; i++)
    if (!input[i].IsSequenceNElementS())
      return false;
    else
      numElts+=input[i].children.size-1;
  output.reset(theCommands, numElts);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i=1; i<input.children.size; i++)
    for (int j=1; j<input[i].children.size; j++)
      output.AddChildOnTop(input[i][j]);
  output.format=input.formatDefault;
  return true;
}

bool Calculator::outerUnionNoRepetition(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsLisT())
    return false;
  int numElts=1;
  for (int i=1; i<input.children.size; i++)
    if (!input[i].IsListStartingWithAtom(theCommands.opSequence()))
      return false;
    else
      numElts+=input[i].children.size-1;
  HashedList<Expression> theList;
  List<int> theIndices;
  theList.SetExpectedSize(numElts);
  for (int i=1; i<input.children.size; i++)
    for (int j=1; j<input[i].children.size; j++)
      theList.AddOnTopNoRepetition(input[i][j]);
  theIndices.SetSize(theList.size);
  for (int i=0; i<theList.size; i++)
    theIndices[i]=theCommands.theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(theList[i]);
  output.children.ReservE(numElts);
  output.reset(theCommands, theIndices.size+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.children.AddOnTop(theIndices);
  output.format=output.formatDefault;
  return true;
}

bool Calculator::outerDivide(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerDivide");
//  std::cout << "<br>Now I'm here 1! input: " << input.ToString() << " lisp: "
//  << input.Lispify() ;
  if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
//  std::cout << "<br>Now I'm here! 2";
  Rational tempRat;
  if (!input[2].IsOfType<Rational>(&tempRat))
    return false;
  if (tempRat.IsEqualToZero())
    return output.SetError("Division by zero. ", theCommands);
  output=input;
  tempRat.Invert();
  output.SetChildAtomValue(0, theCommands.opTimes());
  Expression tempE;
  tempE.AssignValue(tempRat, theCommands);
  return output.SetChilD(2, tempE);
}

bool Calculator::outerMinus(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!(input.IsListNElementsStartingWithAtom(theCommands.opMinus(), 3) || input.IsListNElementsStartingWithAtom(theCommands.opMinus(), 2)) )
    return false;
  Expression tempE, minusOne;
  minusOne.AssignValue(-1, theCommands);
  if (input.children.size==2)
    output.MakeXOX(theCommands, theCommands.opTimes(), minusOne, input[1]);
  else
  { tempE.MakeXOX(theCommands, theCommands.opTimes(), minusOne, input[2]);
    output.MakeXOX(theCommands, theCommands.opPlus(), input[1], tempE);
  }
  return true;
}

bool Expression::operator==(const Expression& other)const
{ if (this->theBoss!=other.theBoss)
    return false;
  return this->theData==other.theData && this->children==other.children;
}

SemisimpleLieAlgebra* Expression::GetAmbientSSAlgebraNonConstUseWithCaution()const
{ this->CheckInitialization();
  Expression myContext=this->GetContext();
  int indexSSalg=myContext.ContextGetIndexAmbientSSalg();
  if (indexSSalg==-1)
    return 0;
  return &this->theBoss->theObjectContainer.theLieAlgebras.GetElement(indexSSalg);
}

int Expression::ContextGetIndexAmbientSSalg()const
{ //std::cout << "<hr>trying to get ambient algebra from " << this->ToString();
  if (!this->IsContext() )
    return -1;
//  std::cout << ". I have " << this->children.size << " children. ";
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opSSLieAlg(), 3))
      return (*this)[i][2].theData;
  return -1;
}

void Expression::GetBaseExponentForm(Expression& outputBase, Expression& outputExponent)const
{ this->CheckInitialization();
  if (this->IsListNElementsStartingWithAtom(this->theBoss->opThePower(), 3))
  { outputBase=(*this)[1];
    outputExponent=(*this)[2];
    return;
  }
  outputBase=*this;
  outputExponent.AssignValue(1, *this->theBoss);
}

Expression Expression::GetContext()const
{ this->CheckInitialization();
  if (this->IsBuiltInType())
    return (*this)[1];
  crash << "This is a programming error: GetContext called on an Expression that is not a built-in data type.  "
  << " I can't display the expression as this may cause ``infinite'' recursion if the error is caused by the ToString method. Here is however the lisp form "
  << this->ToStringFull() << " of the expression. " << "Here's  a stack trace. " << crash;
  Expression output;
  output.MakeEmptyContext(*this->theBoss);
  return output;
}

int Expression::GetNumContextVariables()const
{ return this->GetContext().ContextGetPolynomialVariables().children.size-1;
}

int Expression::GetNumCols()const
{ if (!this->IsSequenceNElementS() || this->format!=this->formatMatrix)
    return -1;
  int theMax=1;
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].format==this->formatMatrixRow &&
        (*this)[i].IsSequenceNElementS())
      theMax=MathRoutines::Maximum((*this)[i].children.size-1, theMax);
  return theMax;
}

void Expression::GetCoefficientMultiplicandForm(Rational& outputCoeff, Expression& outputNoCoeff)const
{ this->CheckInitialization();
  if (this->IsListNElementsStartingWithAtom(this->theBoss->opTimes(), 3))
    if ((*this)[1].IsOfType(&outputCoeff))
    { outputNoCoeff=(*this)[2];
      return;
    }
  outputCoeff=1;
  outputNoCoeff=*this;
  return;
}

int Expression::GetExpressionTreeSize()const
{ this->CheckInitialization();
  RecursionDepthCounter theCounter(&this->theBoss->RecursionDeptH);
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
    crash << "This is a run-time error, which may or may not be caused by a programming error. "
    << "While computing expression tree size, I exceeded the "
    << " recrsion depth limit. To increase the recursion depth limit, modify Calculator::MaxRecursionDeptH. "
    << "The current max recursion depth limit is: " << this->theBoss->MaxRecursionDeptH << crash;
  if (this->IsAtom())
    return 1;
  int result=0;
  for (int i=0; i<this->children.size; i++)
    result+=(*this)[i].GetExpressionTreeSize();
  return result;
}

bool Expression::operator>(const Expression& other)const
{ Rational leftCoeff, rightCoeff;
  Expression leftMon, rightMon;
  this->GetCoefficientMultiplicandForm(leftCoeff, leftMon);
  other.GetCoefficientMultiplicandForm(rightCoeff, rightMon);
  if (leftMon==rightMon)
    return leftCoeff>rightCoeff;
  return leftMon.GreaterThanNoCoeff(rightMon);
}

bool Expression::GreaterThanNoCoeff(const Expression& other)const
{ MacroRegisterFunctionWithName("Expression::GreaterThanNoCoeff");
  if (this->IsOfType<Rational>() && other.IsOfType<Rational>())
    return this->GetValue<Rational>()>other.GetValue<Rational>();
  if (this->IsBuiltInType() && !other.IsBuiltInType())
    return false;
  if (!this->IsBuiltInType() && other.IsBuiltInType())
    return true;
  int thisExpressionTreeSize=this->GetExpressionTreeSize();
  int otherExpressionTreeSize=other.GetExpressionTreeSize();
  if (thisExpressionTreeSize>otherExpressionTreeSize)
    return true;
  if (otherExpressionTreeSize>thisExpressionTreeSize)
    return false;
  if (this->children.size==0)
  { std::string leftS, rightS;
    if (this->IsAtom(&leftS))
      if (other.IsAtom(&rightS))
        return leftS>rightS;
    return this->theData>other.theData;
  }
  return this->children>other.children;
}

bool Expression::ToStringData(std::string& output, FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("Expression::ToStringData");
  std::stringstream out;
  bool result=false;
  bool isFinal=theFormat==0 ? false : theFormat->flagExpressionIsFinal;
  MemorySaving<FormatExpressions> contextFormat;
  if (this->IsAtom())
  { if (this->theData< this->theBoss->GetOperations().size && this->theData>=0)
      out << this->theBoss->GetOperations()[this->theData];
    else
      out << "(unknown~ atom~ of~ value~ " << this->theData << ")";
    result=true;
  } else if (this->IsOfType<std::string>())
  { if (isFinal)
      out << this->GetValue<std::string>();
    else
    { out << "(string~ not~ shown~ to~ avoid~ javascript~ problems~ (in~ case~ the~ string~ has~ javascript))";
      //out << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    }
    result=true;
  } else if (this->IsOfType<Rational>())
  { if (this->HasNonEmptyContext())
      out << "Rational{}(" << (*this)[1].ToString() << ", ";
    if (!this->theBoss->flagUseFracInRationalLaTeX)
      out << this->GetValue<Rational>().ToString();
    else
      out << this->GetValue<Rational>().ToStringFrac();
    if (this->HasNonEmptyContext())
      out << ")";
    result=true;
  } else if (this->IsOfType<ElementZmodP>())
  { out << this->GetValue<ElementZmodP>().ToString();
    result=true;
  } else if (this->IsOfType<Polynomial<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "Polynomial{}(";
    out << this->GetValue<Polynomial<Rational> >().ToString(&contextFormat.GetElement()) << ")";
//    out << "[" << this->GetContext().ToString() << "]";
    result=true;
  } else if (this->IsOfType<Polynomial<AlgebraicNumber> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "PolynomialAlgebraicNumbers{}(";
    out << this->GetValue<Polynomial<AlgebraicNumber> >().ToString(&contextFormat.GetElement()) << ")";
    out << "[" << this->GetContext().ToString() << "]";
    result=true;
  } else if (this->IsOfType<RationalFunctionOld>())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "RationalFunction{}("
    << this->GetValue<RationalFunctionOld>().ToString(&contextFormat.GetElement()) << ")";
    result=true;
  } else if (this->IsOfType<SemisimpleLieAlgebra>())
  { out << "SSLieAlg{}("
    << this->GetValue<SemisimpleLieAlgebra>().GetLieAlgebraName()
    << ")";
    result=true;
  } else if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "UEE{}(" //<< this->GetContext().ToString() << ", "
    << this->GetValue<ElementUniversalEnveloping<RationalFunctionOld> >().ToString(&contextFormat.GetElement())
    << ")";
    result=true;
  } else if (this->IsOfType<MatrixTensor<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    if (this->GetValue<MatrixTensor<Rational> >().GetMinNumColsNumRows()<20)
      out << "MatrixRationalsTensorForm{}("
      << this->GetValue<MatrixTensor<Rational> > ().ToStringMatForm(&contextFormat.GetElement())
      << ")";
    else
      out << this->GetValue<MatrixTensor<Rational> >().ToString();
    result=true;
  } else if (this->IsOfType<Matrix<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    out << "innerMatrixRational{}(" << this->GetValue<Matrix<Rational> >().ToString(&contextFormat.GetElement()) << ")";
    result=true;
  } else if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    out << "ETGVM{}(";
    out << this->GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().ToString(&contextFormat.GetElement());
    out << ")";
    result=true;
  } else if (this->IsOfType<CalculusFunctionPlot>())
  { if (isFinal)
    { CalculusFunctionPlot& thePlot=this->GetValueNonConst<CalculusFunctionPlot>();
      out << this->theBoss->WriteDefaultLatexFileReturnHtmlLink(thePlot.GetPlotStringAddLatexCommands(false), true);
      out << "<br><b>LaTeX code used to generate the output. </b><br>" << thePlot.GetPlotStringAddLatexCommands(true);
    } else
      out << "(plot not shown)";
    result=true;
  } else if (this->IsOfType<WeylGroup>())
  { WeylGroup& theGroup=this->GetValueNonConst<WeylGroup>();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theGroup.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<ElementWeylGroup>())
  { const ElementWeylGroup& theElt=this->GetValue<ElementWeylGroup>();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theElt.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<WeylGroupRepresentation<Rational> >())
  { const WeylGroupRepresentation<Rational>& theElt=this->GetValue<WeylGroupRepresentation<Rational> >();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theElt.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<WeylGroupVirtualRepresentation>())
  { const WeylGroupVirtualRepresentation& theElt=this->GetValue<WeylGroupVirtualRepresentation>();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseReflectionNotation=true;
    out << theElt.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<charSSAlgMod<Rational> >())
  { charSSAlgMod<Rational> theElt=this->GetValue<charSSAlgMod<Rational> >();
    out << theElt.ToString();
    result=true;
  } else if (this->IsOfType<SemisimpleSubalgebras>())
  { SemisimpleSubalgebras& theSubalgebras=this->GetValueNonConst<SemisimpleSubalgebras>();
    contextFormat.GetElement().flagUseLatex=true;
    contextFormat.GetElement().flagUseHTML=true;
    contextFormat.GetElement().flagCandidateSubalgebraShortReportOnly=false;
    contextFormat.GetElement().flagIncludeMutableInformation=false;
    contextFormat.GetElement().flagUseMathSpanPureVsMouseHover=false;
    out << theSubalgebras.ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<double>())
  { out << std::fixed << this->GetValue<double>();
    result=true;
  } else if (this->IsOfType<AlgebraicNumber>())
  { out << this->GetValue<AlgebraicNumber>().ToString();
    result=true;
  } else if (this->IsOfType<LittelmannPath>())
  { out << this->GetValue<LittelmannPath>().ToString();
    result=true;
  } else if (this->IsOfType<Matrix<RationalFunctionOld> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseLatex=true;
    out << this->GetValue<Matrix<RationalFunctionOld> >().ToString(&contextFormat.GetElement());
    result=true;
  } else if (this->IsOfType<ElementWeylAlgebra<Rational> >())
  { this->GetContext().ContextGetFormatExpressions(contextFormat.GetElement());
    contextFormat.GetElement().flagUseHTML=false;
    contextFormat.GetElement().flagUseLatex=true;
    out << "ElementWeylAlgebra{}(";
    out << this->GetValue<ElementWeylAlgebra<Rational> >().ToString(&contextFormat.GetElement());
    out << ")";
    out << "[" << this->GetContext().ToString() << "]";
    result=true;
  }
  output=out.str();
  return result;
}

std::string Expression::ToStringSemiFull()const
{ std::stringstream out;
  std::string tempS;
  if (this->ToStringData(tempS))
    out << tempS << " ";
  else
    if (this->children.size>0)
    { out << "(";
      for (int i=0; i<this->children.size; i++)
      { out << (*this)[i].ToStringSemiFull();
        if (i!=this->children.size-1)
          out << ", ";
      }
      out << ")";
    }
  return out.str();
}

std::string Expression::ToStringFull()const
{ std::stringstream out;
  if (this->IsAtom())
    out << this->theData << " ";
  if (this->children.size>0)
  { out << "(";
    for (int i=0; i<this->children.size; i++)
    { out << (*this)[i].ToStringFull();
      if (i!=this->children.size-1)
        out << ", ";
    }
    out << ")";
  }
  return out.str();
}

bool Expression::NeedsParenthesisForBaseOfExponent()const
{ if (this->theBoss==0)
    return false;
  if (this->IsListNElementsStartingWithAtom())
    return true;
//  if (this->IsListNElementsStartingWithAtom(this->theBoss->opPlus()) || this->IsListNElementsStartingWithAtom(this->theBoss->opMinus()) ||
//      this->IsListNElementsStartingWithAtom(this->theBoss->opTimes()) || this->IsListNElementsStartingWithAtom(this->theBoss->opDivide()) ||
//      this->IsListNElementsStartingWithAtom(this->theBoss->opThePower()))
//    return true;
  return false;
}

bool Expression::NeedsParenthesisForMultiplication()const
{ if (this->theBoss==0)
    return false;
  if (this->IsListNElementsStartingWithAtom(this->theBoss->opPlus()) || this->IsListNElementsStartingWithAtom(this->theBoss->opMinus()))
    return true;
  return false;
}

std::string Expression::ToString(FormatExpressions* theFormat, Expression* startingExpression, Expression* Context)const
{ MacroRegisterFunctionWithName("Expression::ToString");
  if (this->theBoss!=0)
  { if (this->theBoss->RecursionDeptH+1>this->theBoss->MaxRecursionDeptH)
      return "(...)";
  } else
    return "(Error:NoOwner)";
  RecursionDepthCounter theRecursionCounter(&this->theBoss->RecursionDeptH);
  this->CheckConsistency();
  int notationIndex=theBoss->theObjectContainer.ExpressionWithNotation.GetIndex(*this);
  if (notationIndex!=-1)
    return theBoss->theObjectContainer.ExpressionNotation[notationIndex];
  std::stringstream out;
//  AddBrackets=true;
//  if (this->theBoss->flagLogSyntaxRules && recursionDepth<=1)
//  { out << "(ContextIndex=" << this->IndexBoundVars << ")";
//  }
  bool isFinal=theFormat==0 ? false : theFormat->flagExpressionIsFinal;
  bool allowNewLine= (theFormat==0) ? false : theFormat->flagExpressionNewLineAllowed;
  bool oldAllowNewLine= (theFormat==0) ? false : theFormat->flagExpressionNewLineAllowed;

  if (this->theBoss->flagUseFracInRationalLaTeX)
    allowNewLine=false;
  int lineBreak=50;
  int charCounter=0;
  std::string tempS;
  if (this->ToStringData(tempS, theFormat))
    out << tempS;
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDefine(), 3))
  { std::string firstE=(*this)[1].ToString(theFormat);
    std::string secondE=(*this)[2].ToString(theFormat);
    if ((*this)[1].IsListStartingWithAtom(this->theBoss->opDefine()))
      out << "(" << firstE << ")";
    else
      out << firstE;
    out << ":=";
    if ((*this)[2].IsListStartingWithAtom(this->theBoss->opDefine()))
      out << "(" << secondE << ")";
    else
      out << secondE;
  } else if (this->IsListStartingWithAtom(this->theBoss->opIsDenotedBy()))
    out << (*this)[1].ToString(theFormat) << ":=:" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opDefineConditional()))
    out << (*this)[1].ToString(theFormat) << " :if " << (*this)[2].ToString(theFormat) << ":=" << (*this)[3].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDivide(), 3))
  { bool doUseFrac= this->formatUseFrac || this->theBoss->flagUseFracInRationalLaTeX;
    if(!doUseFrac)
    { std::string firstE= (*this)[1].ToString(theFormat);
      std::string secondE=(*this)[2].ToString(theFormat);
      bool firstNeedsBrackets= !((*this)[1].IsListStartingWithAtom(this->theBoss->opTimes())|| (*this)[1].IsListStartingWithAtom(this->theBoss->opDivide()));
      bool secondNeedsBrackets=true;
      if ((*this)[2].IsOfType<Rational>())
        if ((*this)[2].GetValue<Rational>().IsInteger())
          secondNeedsBrackets=false;
      if (firstNeedsBrackets)
        out << "(" << firstE << ")";
      else
        out << firstE;
      out << "/";
      if (secondNeedsBrackets)
        out << "(" << secondE << ")";
      else
        out << secondE;
    } else
    { if (theFormat!=0)
        theFormat->flagExpressionNewLineAllowed=false;
      std::string firstE= (*this)[1].ToString(theFormat);
      std::string secondE=(*this)[2].ToString(theFormat);
      out << "\\frac{" << firstE << "}{" << secondE << "}";
      if (theFormat!=0)
        theFormat->flagExpressionNewLineAllowed=oldAllowNewLine;
    }
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTensor(),3) )
    out << (*this)[1].ToString(theFormat) << "\\otimes " << (*this)[2].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opChoose(),3) )
    out << (*this)[1].ToString(theFormat) << "\\choose " << (*this)[2].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTimes(), 3))
  { std::string secondE=(*this)[2].ToString(theFormat);
    if ((*this)[1].IsAtomGivenData(this->theBoss->opSqrt()))
      out << "\\sqrt{" << secondE << "}";
    else
    { std::string firstE= (*this)[1].ToString(theFormat);
      bool firstNeedsBrackets=(*this)[1].NeedsParenthesisForMultiplication();
      bool secondNeedsBrackets=(*this)[2].NeedsParenthesisForMultiplication();
      if (firstE=="-1" )
      { firstE="-";
        firstNeedsBrackets=false;
      }
      if (firstE=="1")
        firstE="";
      if (firstNeedsBrackets)
        out << "(" << firstE << ")";
      else
        out << firstE;
      if (this->format==this->formatTimesDenotedByStar && firstE!="-" && firstE!="")
        out << "*";
      else
        out << " ";
      if (secondNeedsBrackets)
        out << "(" << secondE << ")";
      else
        out << secondE;
    }
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opSqrt(), 3))
  { int thePower=0;
    bool hasPowerTwo=(*this)[1].IsSmallInteger(&thePower);
    if (hasPowerTwo)
      hasPowerTwo=(thePower==2);
    if (hasPowerTwo)
      out << "\\sqrt{" << (*this)[2].ToString() << "}";
    else
      out << "\\sqrt[" << (*this)[1].ToString() << "]{" << (*this)[2].ToString() << "}";
  } else if (this->IsListStartingWithAtom(this->theBoss->opFactorial()))
    out << (*this)[1].ToString(theFormat) << "!";
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opThePower(),3))
  { bool involvesExponentsInterprettedAsFunctions=false;
    const Expression& firstE=(*this)[1];
    if (firstE.IsListNElementsStartingWithAtom(-1, 2))
      if (firstE[0].IsAtomWhoseExponentsAreInterprettedAsFunction())
      { involvesExponentsInterprettedAsFunctions=true;
        Expression newE, newFunE;
        newFunE.MakeXOX(*this->theBoss, this->theBoss->opThePower(), firstE[0], (*this)[2]);
        newE.reset(*this->theBoss, 2);
        newE.AddChildOnTop(newFunE);
        newE.AddChildOnTop(firstE[1]);
        this->CheckConsistency();
        newE.CheckConsistency();
        newFunE.CheckConsistency();
        //std::cout << "<br> tostringing a very special case: " << newE.ToString() << " lispified: " << this->ToStringFull();
        out << newE.ToString(theFormat);
      }
    if (!involvesExponentsInterprettedAsFunctions)
    { std::string secondEstr=(*this)[2].ToString(theFormat);
      std::string firstEstr=(*this)[1].ToString(theFormat);
      if ((*this)[1].NeedsParenthesisForBaseOfExponent())
      { bool useBigParenthesis=false;
        if ((*this)[1].IsListNElementsStartingWithAtom(this->theBoss->opDivide()))
          useBigParenthesis=true;
        if (useBigParenthesis)
          out << "\\left(";
        else
          out << "(";
        out << firstEstr;
        if (useBigParenthesis)
          out << "\\right)";
        else
          out << ")";
      }
      else
        out << firstEstr;
      out << "^{" << secondEstr << "}";
    }
//    std::cout << "<br>tostringing: " << out.str() << "   lispified: " << this->ToStringFull();
  } else if (this->IsListStartingWithAtom(this->theBoss->opPlus() ))
  { if (this->children.size<2)
      crash << crash;
    std::string tempS2= (*this)[1].ToString(theFormat);
    tempS=(*this)[2].ToString(theFormat);
    out << tempS2;
//    std::cout << "<br>here i am! tempS2.size=" << tempS2.size() << ", allowNewLine="
//    << allowNewLine;
    if (allowNewLine && tempS2.size()>(unsigned)lineBreak)
      out << "\\\\\n";
    if (tempS.size()>0)
      if (tempS[0]!='-')
        out << "+";
    out << tempS;
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 2))
    out << "-" << (*this)[1].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opSqrt(), 2))
    out << "\\sqrt{" << (*this)[1].ToString(theFormat) << "}";
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 3))
  { if (!(this->children.size==3))
      crash << "This is a programming error: the minus function expects 1 or 2 arguments, instead there are " << this->children.size-1 << ". " << crash;
    out << (*this)[1].ToString(theFormat) << "-" << (*this)[2].ToString(theFormat);
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opBind(), 2))
    out << "{{" << (*this)[1].ToString(theFormat) << "}}";
  else if (this->IsListStartingWithAtom(this->theBoss->opApplyFunction()))
  { if (this->children.size<2)
      crash << crash;
    switch(this->format)
    { case Expression::formatFunctionUseUnderscore:
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=false;
        out <<  (*this)[1].ToString(theFormat) << "_{" << (*this)[2].ToString(theFormat) << "}";
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=true;
        break;
      case Expression::formatFunctionUseCdot:
        out <<  (*this)[1].ToString(theFormat) << "\\cdot(" << (*this)[1].ToString(theFormat) << ")";
        break;
      default:
        out << (*this)[1].ToString(theFormat) << "{}(" << (*this)[2].ToString(theFormat) << ")";
        break;
    }
  } else if (this->IsListStartingWithAtom(this->theBoss->opEqualEqual()))
    out << (*this)[1].ToString(theFormat) << "==" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opGreaterThan()))
    out << (*this)[1].ToString(theFormat) << ">" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opLessThan()))
    out << (*this)[1].ToString(theFormat) << "<" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opSequence()))
  { switch (this->format)
    { case Expression::formatMatrixRow:
        for (int i=1; i<this->children.size; i++)
        { out << (*this)[i].ToString(theFormat);
          if (i!=this->children.size-1)
            out << "& ";
        }
        break;
      case Expression::formatMatrix:
        if (theFormat!=0)
          if (theFormat->flagUseLatex)
          out << "\\left(";
        out << "\\begin{array}{";
        for (int i=0; i<this->GetNumCols(); i++)
          out << "c";
        out << "} ";
        for (int i=1; i<this->children.size; i++)
        { out << (*this)[i].ToString(theFormat);
          if (i!=this->children.size-1)
            out << "\\\\ \n";
        }
        out << " \\end{array}";
        if (theFormat!=0)
          if (theFormat->flagUseLatex)
            out << "\\right)";
        break;
      default:
        if (this->children.size==2)
          out << "{Sequence{}";
        out << "(";
        for (int i=1; i<this->children.size; i++)
        { std::string currentChildString=(*this)[i].ToString(theFormat);
          out << currentChildString;
          charCounter+=currentChildString.size();
          if (i!=this->children.size-1)
          { out << ", ";
            if (allowNewLine && charCounter >50 )
              out << "\\\\\n";
          }
          charCounter%=50;
        }
        out << ")";
        if (this->children.size==2)
          out << "}";
        break;
    }
  }
  else if (this->IsListStartingWithAtom(this->theBoss->opLieBracket()))
    out << "[" << (*this)[1].ToString(theFormat) << "," << (*this)[2].ToString(theFormat) << "]";
  else if (this->IsListStartingWithAtom(this->theBoss->opMod()))
    out << (*this)[1].ToString(theFormat) << " mod " << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opUnion()))
    out << (*this)[1].ToString(theFormat) << "\\cup " << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opUnionNoRepetition()))
    out << (*this)[1].ToString(theFormat) << "\\sqcup " << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
  { if (startingExpression==0)
      out << "<table>";
    for (int i=1; i<this->children.size; i++)
    { out << "<tr><td valign=\"top\">";
      bool createTable=(startingExpression!=0);
      if (createTable)
        createTable=(startingExpression->IsListStartingWithAtom(this->theBoss->opEndStatement()));
      if (createTable)
      { out << "<hr> ";
        if (!this->theBoss->flagHideLHS)
        { if (i<(*startingExpression).children.size)
            out << CGI::GetMathMouseHoverBeginArrayL((*startingExpression)[i].ToString(theFormat));
          else
            out << "No matching starting expression- possible use of the Melt keyword.";
        } else
          out << "...";
        if (i!=this->children.size-1)
          out << ";";
        out << "</td><td valign=\"top\"><hr>";
        if ((*this)[i].IsOfType<std::string>() && isFinal)
          out << (*this)[i].GetValue<std::string>();
        else if (((*this)[i].IsOfType<CalculusFunctionPlot> () || (*this)[i].IsOfType<SemisimpleSubalgebras>() || (*this)[i].IsOfType<WeylGroup>()
                  || (*this)[i].IsOfType<WeylGroupRepresentation<Rational> >()) && isFinal)
          out << (*this)[i].ToString(theFormat);
        else
          out << CGI::GetMathSpanBeginArrayL((*this)[i].ToString(theFormat));
        if (i!=this->children.size-1)
          out << ";";
      }
      else
      { out << (*this)[i].ToString(theFormat);
        if (i!=this->children.size-1)
          out << ";";
      }
//      out << "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp Context index: " << this->IndexBoundVars;
      out << "</td></tr>";
    }
    if (startingExpression==0)
      out << "</table>";
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opError(), 2))
  { this->theBoss->NumErrors++;
    out << "(Error~ " << this->theBoss->NumErrors << ":~ see~ comments)";
    this->theBoss->Comments << "<br>Error " << this->theBoss->NumErrors << ". " << (*this)[1].ToString(theFormat);
  } else if (this->children.size==1)
    out << (*this)[0].ToString(theFormat);
  else if (this->children.size>=2)
  { out << (*this)[0].ToString(theFormat);
    bool needParenthesis=true;
    if (this->children.size==2)
    { if ((*this)[0].IsAtomWhoseExponentsAreInterprettedAsFunction())
        needParenthesis=!(*this)[1].IsAtom();
      if ((*this)[0].IsPowerOfAtomWhoseExponentsAreInterprettedAsFunction())
        needParenthesis=!(*this)[1].IsAtom();
    }
    if (this->format==this->formatFunctionUseUnderscore)
      out << "_";
    else if (this->format==this->formatFunctionUseCdot)
      out << "\\cdot";
    else
      out << "{}";
    if(this->format== this->formatFunctionUseUnderscore)
      out << "{";
    else if (needParenthesis)
      out << "(";
    for (int i=1; i<this->children.size; i++)
    { out << (*this)[i].ToString(theFormat);
      if (i!=this->children.size-1)
        out << ", ";
    }
    if(this->format== this->formatFunctionUseUnderscore)
      out << "}";
    else if (needParenthesis)
      out << ")";
//    std::cout << "<br>tostringing: " << out.str() << "   lispified: " << this->ToStringFull();

  } else //<-not sure if this case is possible
    out << "(ProgrammingError:NotDocumented)" ;
  if (startingExpression!=0)
  { std::stringstream outTrue;
    outTrue << "<table>";
    outTrue << "<tr><th>Input in LaTeX</th><th>Result in LaTeX</th></tr>";
    outTrue << "<tr><td colspan=\"2\">Double click LaTeX image to get the LaTeX code. "
    << "Javascript LaTeXing courtesy of <a href=\"http://www.math.union.edu/~dpvc/jsmath/\">jsmath</a>: many thanks for your great work!</td></tr>";
   // std::cout << this->Lispify();
    if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
      outTrue << out.str();
    else
    { outTrue << "<tr><td>" << CGI::GetMathSpanBeginArrayL(startingExpression->ToString(theFormat));
      if ((this->IsOfType<std::string>() || this->IsOfType<CalculusFunctionPlot>() ||
           this->IsOfType<SemisimpleSubalgebras>() || this->IsOfType<WeylGroup>()) && isFinal)
        outTrue << "</td><td>" << out.str() << "</td></tr>";
      else
        outTrue << "</td><td>" << CGI::GetMathSpanBeginArrayL(out.str()) << "</td></tr>";
    }
    outTrue << "</table>";
    return outTrue.str();
  }
//  if (useLatex && recursionDepth==0 && this->theOperation!=theBoss->opEndStatement())
//    return CGI::GetHtmlMathSpanFromLatexFormula(out.str());
  return out.str();
}

std::string Expression::Lispify()const
{ if (this->theBoss==0)
    return "Error: not initialized";
  RecursionDepthCounter theCounter(&this->theBoss->RecursionDeptH);
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
    return "(error: max recursion depth ...)";
  std::string tempS;
  if (this->ToStringData(tempS))
    return tempS;
  if (this->children.size==0)
    return this->ToString();
  std::stringstream out;
  out << "(";
  for (int i=0; i<this->children.size; i++)
    out << " " << (*this)[i].Lispify();
  out << ")";
  return out.str();
}

bool Expression::IsLisT()const
{ if (this->theBoss==0)
    return false;
  if (this->children.size<=0)
    return false;
  if (this->theData!=this->theBoss->opLisT())
    crash << "This is a programming error. List expressions must have data valule equal to Calculator::opList(). " << crash;
  return true;
}

bool Expression::IsFrozen()const
{ std::string atomName;
  if (!this->IsListNElementsStartingWithAtom())
    return false;
  return (*this)[0].IsAtomThatFreezesArguments();
}

bool Expression::IsAtomThatFreezesArguments(std::string* outputWhichAtom)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichAtom!=0)
    *outputWhichAtom=this->theBoss->GetOperations()[this->theData];
  return this->theBoss->atomsThatFreezeArguments.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::IsPowerOfAtomWhoseExponentsAreInterprettedAsFunction()const
{ if (this->theBoss==0)
    return false;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opThePower(), 3))
    return false;
  return (*this)[1].IsAtomWhoseExponentsAreInterprettedAsFunction();
}

bool Expression::IsAtomWhoseExponentsAreInterprettedAsFunction(std::string* outputWhichAtom)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichAtom!=0)
    *outputWhichAtom=this->theBoss->GetOperations()[this->theData];
  return this->theBoss->atomsWhoseExponentsAreInterprettedAsFunctions.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::IsAtomNotInterprettedAsFunction(std::string* outputWhichAtom)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichAtom!=0)
    *outputWhichAtom=this->theBoss->GetOperations()[this->theData];
  return this->theBoss->atomsNotInterprettedAsFunctions.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::IsAtom(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (outputWhichOperation!=0)
  { if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    { std::stringstream out;
      out << "(data-atom:~" << this->theData << ")";
      *outputWhichOperation= out.str();
    }
    else
      *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  }
  return true;
}

bool Expression::IsAtomGivenData(int desiredDataUseMinusOneForAny)const
{ if (this->IsLisT())
    return false;
  if (desiredDataUseMinusOneForAny==-1)
    return true;
  return this->theData==desiredDataUseMinusOneForAny;
}

bool Expression::IsBuiltInAtom(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (this->theData>= this->theBoss->NumPredefinedAtoms)
    return false;
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  return true;
}

bool Expression::IsGoodForChainRuleFunction(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
//  std::cout << "ere be i - checking whether operation " << this->theBoss->GetOperations()[this->theData] << " is good for chain rule. ";
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  return !this->theBoss->atomsNotAllowingChainRule.Contains(this->theBoss->GetOperations()[this->theData]);
}

bool Expression::RemoveContext()
{ this->CheckInitialization();
  if (!this->HasContext())
    return true;
  this->children.RemoveIndexShiftDown(1);
  if (this->children.size==1)
    this->AssignMeMyChild(0);
  return true;
}

bool Expression::HasContext()const
{ this->CheckInitialization();
  if (!this->IsBuiltInType() || !this->children.size==3)
    return false;
  //std::string debugString=(*this)[1].ToString();
  //std::cout << "<br>Trying to fetch context from: " << debugString ;
  return (*this)[1].IsListStartingWithAtom(this->theBoss->opContexT());
}

bool Expression::HasNonEmptyContext()const
{ if (!this->HasContext())
    return false;
  return !this->GetContext().IsListNElementsStartingWithAtom(this->theBoss->opContexT(), 1);
}

bool Expression::IsBuiltInType(std::string* outputWhichOperation)const
{ std::string tempS;
  if (!this->IsListNElementsStartingWithAtom())
    return false;
//  if (this->children.size<2 || !(*this)[this->children.size-1].IsAtoM())
//    return false;
  if (!(*this)[0].IsAtom(&tempS))
    return false;
  if (this->theBoss->GetBuiltInTypes().Contains(tempS))
  { if (outputWhichOperation!=0)
      *outputWhichOperation=tempS;
    return true;
  }
  return false;
}

bool Expression::IsBuiltInType(int* outputWhichType)const
{ std::string theType;
  if (!this->IsBuiltInType(&theType))
    return false;
  if (outputWhichType!=0)
    *outputWhichType=this->theBoss->GetOperations().GetIndex(theType);
  return true;
}

int Calculator::AddOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName)
{ int result=this->theAtoms.GetIndex(theOpName);
  if (result==-1)
  { this->theAtoms.AddOnTop(theOpName);
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
    result=this->theAtoms.size-1;
  }
  return result;
}

void Calculator::AddOperationBuiltInType(const std::string& theOpName)
{ this->AddOperationNoRepetitionAllowed(theOpName);
  this->builtInTypes.AddOnTop(theOpName);
}

void Calculator::AddOperationNoRepetitionAllowed(const std::string& theOpName)
{ if (this->GetOperations().Contains(theOpName))
    crash << "This is a programming error: operation " << theOpName << " already created. " << crash;
  this->theAtoms.AddOnTop(theOpName);
  this->FunctionHandlers.SetSize(this->theAtoms.size);
  this->FunctionHandlers.LastObject()->SetSize(0);
}

void Calculator::AddOperationBinaryInnerHandlerWithTypes
(const std::string& theOpName, Expression::FunctionAddress innerHandler, int leftType, int rightType, const std::string& opDescription,
 const std::string& opExample, bool visible, bool experimental)
{ int indexOp=this->theAtoms.GetIndex(theOpName);
  if (indexOp==-1)
  { this->theAtoms.AddOnTop(theOpName);
    indexOp=this->theAtoms.size-1;
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  Function innerFunction(innerHandler, 0, opDescription, opExample, true, visible, experimental, true);
  innerFunction.theArgumentTypes.reset(*this, 2);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(leftType);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(rightType);
  this->FunctionHandlers[indexOp].ReservE(10);
  this->FunctionHandlers[indexOp].AddOnTop(innerFunction);
}

void Calculator::AddOperationHandler
(const std::string& theOpName, Expression::FunctionAddress handler, const std::string& opArgumentListIgnoredForTheTimeBeing,
 const std::string& opDescription, const std::string& opExample, bool isInner, bool visible, bool isExperimental)
{ int indexOp=this->theAtoms.GetIndex(theOpName);
  if (indexOp==-1)
  { this->theAtoms.AddOnTop(theOpName);
    indexOp=this->theAtoms.size-1;
    this->FunctionHandlers.SetSize(this->theAtoms.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  if (opArgumentListIgnoredForTheTimeBeing!="")
    crash << "This section of code is not implemented yet. Crashing to let you know. " << crash;
  Function theFun(handler, 0, opDescription, opExample, isInner, visible, isExperimental);
  if (theOpName=="*" || theOpName=="+" || theOpName=="/" || theOpName=="\\otimes" || theOpName=="^")
    this->FunctionHandlers[indexOp].ReservE(100);
  else
    this->FunctionHandlers[indexOp].ReservE(10);
  this->FunctionHandlers[indexOp].AddOnTop(theFun);
}

void Calculator::AddOperationComposite
(const std::string& theOpName, Expression::FunctionAddress handler, const std::string& opArgumentListIgnoredForTheTimeBeing,
 const std::string& opDescription, const std::string& opExample, bool isInner, bool visible, bool isExperimental)
{ MacroRegisterFunctionWithName("Calculator::AddOperationComposite");
  int theIndex=this->operationsComposite.GetIndex(theOpName);
  if (opArgumentListIgnoredForTheTimeBeing!="")
    crash << "This section of code is not implemented yet. Crashing to let you know. " << crash;
  if (theIndex==-1)
  { theIndex=this->operationsComposite.size;
    this->operationsComposite.AddOnTop(theOpName);
    this->operationsCompositeHandlers.SetSize(this->operationsCompositeHandlers.size+1);
    this->operationsCompositeHandlers.LastObject()->SetSize(0);
  }
  Function theFun(handler, 0, opDescription, opExample, isInner, visible, isExperimental);
  this->operationsCompositeHandlers[theIndex].AddOnTop(theFun);
}

std::string Calculator::ElementToStringNonBoundVars()
{ std::stringstream out;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  out << "<br>\n" << this->theAtoms.size << " atoms " << " (= " << this->NumPredefinedAtoms << " predefined atoms+ "
  << this->theAtoms.size-this->NumPredefinedAtoms << " user-or-run-time defined). <br>Predefined: \n<br>\n";
  for (int i=0; i<this->theAtoms.size; i++)
  { out << openTag1 << this->theAtoms[i] << closeTag1;
    if (this->FunctionHandlers[i].size>0)
    { out << " [handled by: ";
      for (int j=0; j<this->FunctionHandlers[i].size; j++)
      { out << std::hex << (unsigned long) this->FunctionHandlers[i][j].theFunction;
        if (this->FunctionHandlers[i][j].flagIsInner)
          out << "(inner)";
        else
          out << "(outer)";
        if (j!=this->FunctionHandlers[i].size-1)
          out << ", ";
      }
      out << "]";
    }
    if (i!=this->theAtoms.size-1)
    { out << ", ";
      if (i==this->NumPredefinedAtoms-1 )
        out << "<br>user or run-time defined:\n<br>\n";
    }
  }
  return out.str();
}

bool Function::inputFitsMyInnerType(const Expression& input)
{ if (!this->flagIsInner)
    return false;
  if (this->theArgumentTypes.children.size!=2)
    return true;
  if (input.children.size!=3)
    return false;
  bool argument1good=this->theArgumentTypes[0].theData==-1 ? true :input[1].IsListStartingWithAtom(this->theArgumentTypes[0].theData);
  bool argument2good=this->theArgumentTypes[1].theData==-1 ? true :input[2].IsListStartingWithAtom(this->theArgumentTypes[1].theData);
  return argument1good && argument2good;
}

std::string Function::GetString(Calculator& theBoss)
{ if (!this->flagIamVisible)
    return "";
  std::stringstream out2;
  if (!this->flagIsExperimental)
  { std::stringstream out;
    out << this->theDescription;
//    out << " \nFunction memory address: " << std::hex << (int) this->theFunction << ". ";
    // use of unsigned long is correct on i386 and amd64
    // uintptr_t is only available in c++0x
    out << " Function memory address: " << std::hex << (unsigned long) this->theFunction << ". ";
    if (!this->flagIsInner)
      out << "This is a <b>``law''</b> - substitution takes place only if output expression is different from input. ";
    if (this->theExample!="")
      out << " <br> " << this->theExample << "&nbsp&nbsp&nbsp";
    out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());
    if (this->theExample!="")
      out2 << "<a href=\"" << theBoss.DisplayNameCalculator  << "? textType=Calculator&textDim=1&textInput="
      << CGI::UnCivilizeStringCGI(this->theExample) << "\"> " << " Example" << "</a>" ;
  } else
    out2 << "<b>Experimental, please don't use.</b>";
  return out2.str();
}

std::string ObjectContainer::ToString()
{ std::stringstream out;
  if (this->theLieAlgebras.size>0)
  { out << "Lie algebras created (" << this->theLieAlgebras.size << " total): ";
    for (int i=0; i<this->theLieAlgebras.size; i++)
    { out << this->theLieAlgebras[i].GetLieAlgebraName();
      if (i!=this->theLieAlgebras.size-1)
        out << ", ";
    }
  }
  if (this->theSSsubalgebras.size>0)
  { out << "<br>Lie semisimple subalgebras computation data structures (" << this->theLieAlgebras.size << " total): ";
    for (int i=0; i<this->theSSsubalgebras.size; i++)
    { out << " Type " << this->theSSsubalgebras[i].owneR->GetLieAlgebraName() << " with "
      << this->theSSsubalgebras[i].theSubalgebraCandidates.size << " candidates";
      if (i!=this->theLieAlgebras.size-1)
        out << ", ";
    }
  }
  return out.str();
}

std::string Calculator::ToString()
{ MacroRegisterFunctionWithName("Calculator::ToString");
  std::stringstream out, out2;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  std::string openTag3="<span style=\"color:#00FF00\">";
  std::string closeTag3="</span>";
  out2 << " Total number of pattern matches performed: " << this->TotalNumPatternMatchedPerformed << "";
  double elapsedSecs=this->theGlobalVariableS->GetElapsedSeconds();
  out2 << "<br>Computation time: " << elapsedSecs << " seconds (" << elapsedSecs*1000 << " milliseconds).<br>";
  std::stringstream tempStreamTime;
  tempStreamTime << " Of them " << this->StartTimeEvaluationInSecondS << " seconds (" << this->StartTimeEvaluationInSecondS*1000
  << " millisecond(s)) boot + " << elapsedSecs-this->StartTimeEvaluationInSecondS << " (" << (elapsedSecs-this->StartTimeEvaluationInSecondS)*1000
  << " milliseconds) user computation.<br>Boot time is measured from start of main() until evaluation start and excludes static initializations "
  << "+ executable load. Computation time excludes the time needed to compute the strings that follow below (which might take a while).";
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(tempStreamTime.str());
  out2 << "<br>Maximum computation time: " << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2 << " seconds. ";
  if (this->DepthRecursionReached>0)
    out2 << "<br>Maximum recursion depth reached: " << this->DepthRecursionReached << ".";
  #ifdef MacroIncrementCounter
  out2 << "<br>Number of Lists created: " << NumListsCreated << "<br> Number of List resizes: " << NumListResizesTotal
  << "<br> Number HashedList hash resizing: " << NumHashResizes;
  if (Rational::TotalSmallAdditions>0)
    out2 << "<br>Number small rational number additions: " << Rational::TotalSmallAdditions << " (# successful calls Rational::TryToAddQuickly)";
  if (Rational::TotalSmallMultiplications>0)
    out2 << "<br>Number small rational number multiplications: " << Rational::TotalSmallMultiplications << " (# successful calls Rational::TryToMultiplyQuickly)";
  if (Rational::TotalSmallGCDcalls>0)
    out2 << "<br>Number small number gcd calls: " << Rational::TotalSmallGCDcalls << " (# calls of Rational::gcd)";
  if (Rational::TotalLargeAdditions>0)
    out2 << "<br>Number large integer additions: " << Rational::TotalLargeAdditions << " (# calls LargeIntUnsigned::AddNoFitSize)";
  if (Rational::TotalLargeMultiplications>0)
    out2 << "<br>Number large integer multiplications: " << Rational::TotalLargeMultiplications << " (# calls LargeIntUnsigned::MultiplyBy)";
  if (Rational::TotalLargeGCDcalls>0)
    out2 << "<br>Number large number gcd calls: " << Rational::TotalLargeGCDcalls << " (# calls LargeIntUnsigned::gcd)";
  #endif
  if (this->RuleStack.size>0)
  { out2 << "<hr><b>Predefined rules.</b><br>";
    for (int i=0; i<this->RuleStack.size; i++)
    { out2 << this->RuleStack[i].ToString();
      if (i!=this->RuleStack.size-1)
        out2 << "<br>";
    }
  }
  out2 << "<hr>" << this->ToStringFunctionHandlers() << "<hr><b>Further calculator details.</b>";
  out << "<br><b>Object container information</b>.The object container is the data structure storing all c++ built-in data types "
  << " requested by the user<br> " << this->theObjectContainer.ToString();
  out << "<hr>Control sequences (" << this->controlSequences.size << " total):\n<br>\n";
  for (int i=0; i<this->controlSequences.size; i++)
  { out << openTag1 << this->controlSequences[i] << closeTag1;
    if (i!=this->controlSequences.size)
      out << ", ";
  }
  out << "<br>\n User or run-time defined atoms = " << this->theAtoms.size << " (= " << this->NumPredefinedAtoms << " predefined + "
  << this->theAtoms.size-this->NumPredefinedAtoms << " user-defined):<br>\n";
  for (int i=0; i<this->theAtoms.size; i++)
  { out << "\n" << i << ": " << openTag1 << this->theAtoms[i] << closeTag1;
    if(i!=this->theAtoms.size-1)
      out << ", ";
  }
  out << this->ElementToStringNonBoundVars();
  out << "<hr>";
  out << "Children expressions (" << this->theExpressionContainer.size << " total):<br>";
  int numExpressionsToDisplay=this->theExpressionContainer.size;
  if (this->theExpressionContainer.size>2000)
  { numExpressionsToDisplay=2000;
    out << " <b>Diplaying first 2000 only </b><br>";
  }
  for (int i=0; i< numExpressionsToDisplay; i++)
    out << this->theExpressionContainer[i].ToString() << ", ";
  out << "<hr>";
  out << "\n Cached expressions (" << this->cachedExpressions.size << " total):\n<br>\n";
  for (int i=0; i<this->cachedExpressions.size; i++)
  { out << this->cachedExpressions[i].ToString() << " -> " << this->imagesCachedExpressions[i].ToString();
    if (i!=this->cachedExpressions.size-1)
      out << "<br>";
  }
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());//, "Expand/collapse.");
  return out2.str();
}

bool Calculator::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Rational tempRat=left.theData.GetValue<Rational>();
  if (this->registerPositionAfterDecimalPoint==0)
  { tempRat*=10;
    tempRat+=right.theData.GetValue<Rational>();
  } else
  { Rational afterDecimal=right.theData.GetValue<Rational>();
    for (int i=0; i<this->registerPositionAfterDecimalPoint; i++)
      afterDecimal/=10;
    tempRat+=afterDecimal;
    this->registerPositionAfterDecimalPoint++;
  }
  left.theData.AssignValue(tempRat, *this);
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXEXEXByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& lefT = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(lefT.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  result.theData=newExpr;
  result.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(4);
  return true;
}

bool Calculator::ReplaceEXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.ReservE(2);
  newExpr.AddChildAtomOnTop(this->opEndStatement());
  newExpr.AddChildOnTop(left.theData);
  left.theData=newExpr;
  left.controlIndex=this->conSequenceStatements();
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceSsSsXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  if (!left.theData.IsListNElementsStartingWithAtom(this->opEndStatement()))
    crash << "This is a programming error: ReplaceSsSsXdotsXbySsXdotsX called but left expression is not EndStatement." << crash;
  left.theData.children.ReservE(left.theData.children.size+ right.theData.children.size-1);
  for (int i=1; i<right.theData.children.size; i++)
    left.theData.AddChildOnTop(right.theData[i]);
  left.theData.format=Expression::formatDefault;
  left.controlIndex=this->conSequenceStatements();
  (*this->CurrentSyntacticStacK).PopIndexShiftDown((*this->CurrentSyntacticStacK).size-numDots-1);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool Calculator::ReplaceEXEXByEX(int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackExceptLast(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

std::string Calculator::ElementToStringSyntacticStack()
{ std::stringstream out;
  if ((*this->CurrentSyntacticStacK).size<this->numEmptyTokensStart)
    return "Error: this is a programming error: not enough empty tokens in the start of the syntactic stack.";
  bool HasMoreThanOneSignificantEntriesOrIsNotExpression=((*this->CurrentSyntacticStacK).size>this->numEmptyTokensStart+1);
  SyntacticElement& lastSyntacticElt=* (*this->CurrentSyntacticStacK).LastObject();
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
    if (lastSyntacticElt.controlIndex!=this->conExpression())
      HasMoreThanOneSignificantEntriesOrIsNotExpression=true;
  if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    out << "<table border=\"1\"><tr><td>";
  for (int i=this->numEmptyTokensStart; i<(*this->CurrentSyntacticStacK).size; i++)
  { out << (*this->CurrentSyntacticStacK)[i].ToString(*this);
    if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    { out << "</td>";
      if (i!=(*this->CurrentSyntacticStacK).size-1)
        out << "<td>";
    }
  }
  if (HasMoreThanOneSignificantEntriesOrIsNotExpression)
    out << "</td></tr></table>";
  return out.str();
}

bool Calculator::ReplaceXXXByCon(int theCon)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool Calculator::ReplaceXXXXXByCon(int theCon, int inputFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(4);
  return true;
}

bool Calculator::ReplaceXXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool Calculator::ReplaceXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool Calculator::ReplaceXXXXByCon(int con1, int inputFormat1)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool Calculator::ReplaceXByCon(int theCon, int theFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=theCon;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.format=theFormat;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceXByEusingO(int theOperation)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=this->conExpression();
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.MakeAtom(theOperation, *this);
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool Calculator::ReplaceXByConCon(int con1, int con2, int format1, int format2)
{ (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size+1);
  (*this->CurrentSyntacticStacK).LastObject()->theData.reset(*this);
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=format1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.format=format2;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

void ObjectContainer::reset()
{ this->theWeylGroupElements.Clear();
  this->theWeylGroupReps.Clear();
  this->theWeylGroupVirtualReps.Clear();
  this->theWeylGroups.SetSize(0);
  this->theCategoryOmodules.SetSize(0);
  this->theLieAlgebras.Clear();
  this->theSSsubalgebras.SetSize(0);
  this->theTensorElts.Clear();
  this->thePolys.Clear();
  this->thePolysOverANs.Clear();
  this->theWeylAlgebraElements.Clear();
  this->theUEs.Clear();
  this->theRFs.Clear();
  this->theRationals.Clear();
  this->theCharsSSLieAlgFD.Clear();
  this->theDoubles.Clear();
  this->theStrings.Clear();
  this->ExpressionNotation.Clear();
  this->ExpressionWithNotation.Clear();
  this->theLSpaths.Clear();
  this->theMatRats.Clear();
  this->theMatTensorRats.Clear();
  this->theMatRFs.Clear();
  this->theEltsModP.Clear();
  this->thePlots.SetSize(0);
  theAlgebraicClosure.reset();
  this->theAlgebraicNumbers.Clear();
  this->theLittelmannOperators.Clear();
  this->theSltwoSAs.SetSize(0);
}

template <class coefficient>
bool Expression::ContextGetPolySubFromSuperContextNoFailure(const Expression& largerContext, PolynomialSubstitution<coefficient>& output)const
{ bool mustBeTrue= this->ContextGetPolySubFromSuperContext(largerContext, output);
  if (!mustBeTrue)
    crash << "This is a programming error: I was not able to extract a polynomial substitution from smaller context "
    << this->ToString() << " relative to larger context " << largerContext.ToString() << ". " << crash;
  return mustBeTrue;
}

template <class coefficient>
bool Expression::ContextGetPolySubFromSuperContext(const Expression& largerContext, PolynomialSubstitution<coefficient>& output)const
{ Expression polyVarsElargerContext=largerContext.ContextGetPolynomialVariables();
  Expression polyVarsEsmallContext=this->ContextGetPolynomialVariables();
  output.SetSize(polyVarsElargerContext.children.size-1);
  int numVars=polyVarsElargerContext.children.size-1;
  for (int i=1; i<polyVarsEsmallContext.children.size; i++)
  { int theNewIndex=polyVarsElargerContext.GetIndexChild(polyVarsEsmallContext[i]);
    if (theNewIndex==-1)
      return false;
    output[i-1].MakeMonomiaL(theNewIndex-1, 1, 1, numVars);
  }
  return true;
}

bool Expression::ContextGetPolyAndEWASubFromSuperContextNoFailure
(const Expression& largerContext, PolynomialSubstitution<Rational>& outputPolyPart, PolynomialSubstitution<Rational>& outputEWApart)const
{ bool mustBeTrue= this->ContextGetPolyAndEWASubFromSuperContext(largerContext, outputPolyPart, outputEWApart);
  if (!mustBeTrue)
    crash << "This is a programming error: I was not able to extract a polynomial/differential operator substitution from smaller context "
    << this->ToString() << " relative to larger context " << largerContext.ToString() << crash;
  return mustBeTrue;
}

bool Expression::ContextGetPolyAndEWASubFromSuperContext
(const Expression& largerContext, PolynomialSubstitution<Rational>& outputPolyPart, PolynomialSubstitution<Rational>& outputEWApart)const
{ if(!this->ContextGetPolySubFromSuperContext(largerContext, outputPolyPart))
    return false;
  Expression EWAVarsElargerContext=largerContext.ContextGetDifferentialOperatorVariables();
  Expression EWAVarsEsmallContext=this->ContextGetDifferentialOperatorVariables();
  outputEWApart.SetSize(EWAVarsElargerContext.children.size-1);
  int numVars=EWAVarsElargerContext.children.size-1;
  for (int i=1; i<EWAVarsEsmallContext.children.size; i++)
  { int theNewIndex=EWAVarsElargerContext.GetIndexChild(EWAVarsEsmallContext[i]);
    if (theNewIndex==-1)
      return false;
    outputEWApart[i-1].MakeMonomiaL(theNewIndex-1, 1, 1, numVars);
  }
  return true;
}

bool Calculator::IsBoundVarInContext(int inputOp)
{ for (int i=0; i<this->BoundVariablesStack.size; i++)
    if (this->BoundVariablesStack[i].Contains(inputOp))
      return true;
  return false;
}

bool Calculator::IsNonBoundVarInContext(int inputOp)
{ for (int i=0; i<this->NonBoundVariablesStack.size; i++)
    if (this->NonBoundVariablesStack[i].Contains(inputOp))
      return true;
  return false;
}

bool Calculator::ReplaceXXVXdotsXbyE_BOUND_XdotsX(int numXs)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  int theBoundVar=theElt.theData.theData;
//  std::cout << "<br>Registering bound variable index: " << theBoundVar;
  if (this->IsNonBoundVarInContext(theBoundVar))
  { std::stringstream out;
    out << "Syntax error. In the same syntactic scope, the string " << this->theAtoms[theBoundVar] << " is first used to denote a non-bound variable"
    << " but later to denote a bound variable. This is not allowed. ";
    theElt.errorString=out.str();
    theElt.controlIndex=this->conError();
    this->DecreaseStackSetCharacterRangeS(numXs);
    this->ReplaceXXYByY();
    return true;
  }
  if (!this->IsBoundVarInContext(theBoundVar))
    this->BoundVariablesStack.LastObject()->AddOnTopNoRepetition(theBoundVar);
  theElt.theData.reset(*this, 2);
  theElt.theData.AddChildAtomOnTop(this->opBind());
  theElt.theData.AddChildAtomOnTop(theBoundVar);
  theElt.controlIndex=this->conExpression();
//  std::cout << ", got to element: " << theElt.theData.ToString();
  this->DecreaseStackSetCharacterRangeS(numXs);
  this->ReplaceXXYByY();
//  std::cout << ", finally got: "
//  << (*this->CurrentSyntacticStacK).LastObject()->ToString(*this);
  return true;
}

bool Calculator::ReplaceVXdotsXbyE_NONBOUND_XdotsX(int numXs)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1-numXs];
  int theBoundVar=theElt.theData.theData;
//  std::cout << "<br>index of variable: " << theElt.ToString(*this);
  if (this->IsBoundVarInContext(theBoundVar))
  { theElt.theData.reset(*this, 2);
    theElt.theData.AddChildAtomOnTop(this->opBind());
    theElt.theData.AddChildAtomOnTop(theBoundVar);
  } else
  { theElt.theData.MakeAtom(theBoundVar, *this);
    if (!this->IsNonBoundVarInContext(theBoundVar))
      this->NonBoundVariablesStack.LastObject()->AddOnTop(theBoundVar);
  }
  theElt.controlIndex=this->conExpression();
  return true;
}

void Calculator::InitJavaScriptDisplayIndicator()
{ std::stringstream output;
  output << " <!>\n";
  output << " <script type=\"text/javascript\"> \n";
  output << " var timeOutCounter=0;\n";
//  output << " var newReportString=\"\";\n";
  output << " var showProgress=false;";
  output << " function progressReport()\n";
  output << "{ var el = document.getElementById(\"idProgressReport\");	\n";
  output << "  if (!showProgress) \n";
  output << "  { el.style.display = 'none';\n";
  output << "    return;";
  output << "  }\n";
  output << "  el.style.display = '';\n";
//  output << "  el.contentWindow.location.reload();";
  output << "  timeOutCounter++;\n";
  output << "  var oRequest = new XMLHttpRequest();\n";
  output << "  var sURL  = \"" << this->indicatorFileNameDisplaY << "\";\n";
  output << "  oRequest.open(\"GET\",sURL,false);\n";
  output << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  output << "  oRequest.send(null)\n";
  output << "  if (oRequest.status==200)\n";
  output << "  { newReportString= oRequest.responseText;\n";
  output << "    el.innerHTML= \"<hr>Refreshing each second. Client time: ~\"+ timeOutCounter+\" second(s)<br>\" +newReportString+\"<hr>\";\n";
  output << "  }\n";
  output << "   window.setTimeout(\"progressReport()\",1000);\n";
  output << " }\n";
  output << " </script>\n";
  output << CGI::GetHtmlButton
  ("progressReportButton", "showProgress=!showProgress; progressReport()", "expand/collapse progress report");
  output << "<br><div "
  //<< "src=\"" << this->indicatorFileNameDisplaY << "\" "
  << "id=\"idProgressReport\" style=\"display:none\">\n";
  output << " </div>\n";
  output << " \n";
  output << " \n";
  this->javaScriptDisplayingIndicator=output.str();
}

void Calculator::initDefaultFolderAndFileNames(const std::string& inputPathBinaryBaseIsFolderBelow, const std::string& inputDisplayPathBase, const std::string& scrambledIP)
{ this->PhysicalPathServerBase = inputPathBinaryBaseIsFolderBelow + "../";
  this->DisplayPathServerBase = "/" + inputDisplayPathBase;

  this->PhysicalPathOutputFolder = this->PhysicalPathServerBase + "output/";
  this->DisplayPathOutputFolder = this->DisplayPathServerBase + "output/";

  this->userLabel=scrambledIP;

  this->PhysicalNameDefaultOutput = this->PhysicalPathOutputFolder + "default" + this->userLabel + "output";
  this->DisplayNameDefaultOutputNoPath = "default" + this->userLabel + "output";
  this->DisplayNameDefaultOutput = this->DisplayPathOutputFolder + this->DisplayNameDefaultOutputNoPath;

  this->indicatorFileNamE = this->PhysicalPathOutputFolder + "indicator" + this->userLabel + ".html" ;
  this->indicatorFileNameDisplaY = this->DisplayPathOutputFolder + "indicator" + this->userLabel + ".html" ;
  this->DisplayNameCalculator = this->DisplayPathServerBase +"cgi-bin/calculator";

}

bool Calculator::innerWriteGenVermaModAsDiffOperators(Calculator& theCommands, const Expression& input, Expression& output, bool AllGenerators)
{ MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.SetSize(1);
  Expression theContext;
  Selection theParSel;
  SemisimpleLieAlgebra* theSSalgebra;
  Expression truncatedInput=input;
  if (truncatedInput.children.size>4)
  { int numEltsToCut=truncatedInput.children.size-4;
    for (int i=0; i<numEltsToCut; i++)
      truncatedInput.children.RemoveLastObject();
  }
  if (!theCommands.GetTypeHighestWeightParabolic<Polynomial<Rational> >
      (theCommands, truncatedInput, output, theHWs[0], theParSel, theContext, theSSalgebra, Serialization::innerPolynomial<Rational>))
    return output.SetError("Failed to extract type, highest weight, parabolic selection", theCommands);
  if (output.IsError())
    return true;
  std::string letterString="x";
  std::string partialString="\\partial";
  std::string exponentLetterString="a";
  if (input.children.size>4)
    letterString=input[4].ToString();
  if (input.children.size>5)
    partialString=input[5].ToString();
  if (input.children.size>6)
    exponentLetterString=input[6].ToString();

//std::cout << "<br>theContext:" << theContext.ToString();
//std::cout << ", numvars: " << theContext.ContextGetNumContextVariables();
//  FormatExpressions theFormat;
//  theContext.ContextGetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHWs, theContext, theParSel, theSSalgebra, AllGenerators, &letterString, &partialString, &exponentLetterString);
}

bool Calculator::innerFreudenthalFull(Calculator& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeFullCharacter(resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return output.SetError(reportString, theCommands);
  std::stringstream out;
  out << resultChar.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerFreudenthalEval(Calculator& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg=0;
  Expression context;
  if (!theCommands.GetTypeHighestWeightParabolic<Rational>(theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeDominantWeightsOnly(resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return output.SetError(reportString, theCommands);
  return output.AssignValue(resultChar, theCommands);
}

bool Expression::IsMeltable(int* numResultingChildren)const
{ this->CheckInitialization();
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opMelt(), 2))
    return false;
  if (numResultingChildren!=0)
  { if (!(*this)[1].IsListNElementsStartingWithAtom(this->theBoss->opEndStatement()))
      *numResultingChildren=1;
    else
      *numResultingChildren=(*this)[1].children.size-1;
  }
  return true;
}

bool Expression::MergeContextsMyAruments(Expression& output)const
{ MacroRegisterFunctionWithName("Expression::MergeContextsMyAruments");
//  std::cout << "<hr>Merging contexts of expression " << this->ToString();
  this->CheckInitialization();
  if (this->children.size<2)
    return false;
//  std::cout << " ... continuing to merge..." ;
  for (int i=1; i< this->children.size; i++)
    if (!(*this)[i].IsBuiltInType())
    { this->theBoss->Comments << "<hr>Failed to merge the arguments of the expression" << this->ToString() << ": the argument "
      << (*this)[i].ToString() << "is not of built-in type";
      return false;
    }
  Expression commonContext=(*this)[1].GetContext();
  bool needsMerge=false;
  for (int i=2; i<this->children.size; i++)
    if (!(commonContext==(*this)[i].GetContext()))
    { needsMerge=true;
      break;
    }
//  if (needsMerge)
//    std::cout << "needs merge!"; else std::cout << "no need merge no nothin";
  if (!needsMerge)
  { output=*this;
    return true;
  }
  for (int i=2; i<this->children.size; i++)
  { if (!(*this)[i].IsBuiltInType())
    { this->theBoss->Comments << "<hr>Failed to merge contexts of arguments: an argument is not of built-in type";
      return false;
    }
//    std::cout << "<br>Merging context " << commonContext.ToString() << " with " << (*this)[i].GetContext().ToString();
    if (!commonContext.ContextMergeContexts(commonContext, (*this)[i].GetContext(), commonContext))
    { this->theBoss->Comments << "<hr>Failed to merge context " << commonContext.ToString() << " with " << (*this)[i].GetContext().ToString();
      return false;
    }
//    std::cout << " ...  to get context: " << commonContext.ToString();
  }
  output.reset(*this->theBoss, this->children.size);
  output.AddChildOnTop((*this)[0]);
  Expression convertedE;
  for (int i=1; i<this->children.size; i++)
  { convertedE=(*this)[i];
//    std::cout << "<hr>Setting context of " << convertedE.ToString() << " to be the context " << commonContext.ToString();
    if (!convertedE.SetContextAtLeastEqualTo(commonContext))
    { this->theBoss->Comments << "<hr>Failed to convert " << convertedE.ToString() << " to context " << commonContext.ToString();
      return false;
    }
//    std::cout << "... and the result is: " << convertedE.ToString();
    output.AddChildOnTop(convertedE);
  }
  return true;
}

bool Calculator::ConvertExpressionsToCommonContext(List<Expression>& inputOutputEs, Expression* inputOutputStartingContext)
{ MacroRegisterFunctionWithName("Calculator::ConvertExpressionsToCommonContext");
  Expression commonContext;
  commonContext.MakeEmptyContext(*this);
  if (inputOutputStartingContext!=0)
    if (inputOutputStartingContext->IsContext())
      commonContext=*inputOutputStartingContext;
  for (int i=0; i<inputOutputEs.size; i++)
  { if (!inputOutputEs[i].IsBuiltInType())
    { this->Comments << "<hr>Possible programming error: calling ConvertExpressionsToCommonContext on expressions without context. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      return false;
    }
    if (!commonContext.ContextMergeContexts(commonContext, inputOutputEs[i].GetContext(), commonContext))
    { this->Comments << "<hr>Failed to merge context " << commonContext.ToString() << " with " << inputOutputEs[i].GetContext().ToString();
      return false;
    }
  }
  for (int i=0; i<inputOutputEs.size; i++)
    if (!inputOutputEs[i].::Expression::SetContextAtLeastEqualTo(commonContext))
      return false;
  if (inputOutputStartingContext!=0)
    *inputOutputStartingContext=commonContext;
  return true;
}

bool Calculator::outerMeltBrackets(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::outerMeltBrackets");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  //std::cout << "outerMeltBrackets meldet sich!";
  if (!input.IsListNElementsStartingWithAtom(theCommands.opEndStatement()))
    return false;
  //std::cout << "<br>outerMeltBrackets meldet sich!";
  int tempInt;
  int ChildIncrease=0;
  bool found=false;
  for (int i=0; i<input.children.size; i++)
  { const Expression& currentChild=input[i];
    if (currentChild.IsMeltable(&tempInt))
    { found=true;
      ChildIncrease+=tempInt-1;
    }
  }
//  if (!found)
//    std::cout << "<br>not found!";
  if (!found)
    return false;
//  std::cout << "<br>ChildIncrease: " << ChildIncrease;
  output.reset(theCommands, input.children.size+ChildIncrease);
  output.AddChildAtomOnTop(theCommands.opEndStatement());
  for (int i=1; i<input.children.size; i++)
  { const Expression& currentChild=input[i];
    if (!currentChild.IsMeltable())
    { //output.SetChild(i+shift, input.children[i]);
      output.AddChildOnTop(input[i]);
      continue;
    }
//    std::cout << "<br>shift:" << shift;
    if (!currentChild[1].IsListNElementsStartingWithAtom(theCommands.opEndStatement()))
    { //output.SetChild(i+shift, currentChild.children[1]);
      output.AddChildOnTop(currentChild[1]);
      continue;
    }
    for (int j=1; j<currentChild[1].children.size; j++)
    { //output.SetChild(i+shift, currentChild[1].children[j]);
      output.AddChildOnTop(currentChild[1][j]);
    }
  }
//  std::cout << output.ToString();

  return true;
}
