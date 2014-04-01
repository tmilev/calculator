//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
ProjectInformationInstance ProjectInfoVpf6_4ExpressionsImplementationcpp(__FILE__, "Calculator expression implementation. ");

Expression operator*(const Expression& left, const Expression& right)
{ left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.theBoss, left.theBoss->opTimes(), left, right);
  return result;
}

Expression operator/(const Expression& left, const Expression& right)
{ left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.theBoss, left.theBoss->opDivide(), left, right);
  return result;
}

Expression operator+(const Expression& left, const Expression& right)
{ left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.theBoss, left.theBoss->opPlus(), left, right);
  return result;
}

Expression operator-(const Expression& left, const Expression& right)
{ left.CheckInitialization();
  Expression result;
  result.MakeXOX(*left.theBoss, left.theBoss->opMinus(), left, right);
  return result;
}

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
int Expression::GetTypeOperation<Weight<Rational> >()const
{ this->CheckInitialization();
  return this->theBoss->opWeightLieAlg();
}

template < >
int Expression::GetTypeOperation<Weight<Polynomial<Rational> > >()const
{ this->CheckInitialization();
  return this->theBoss->opWeightLieAlgPoly();
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
int Expression::GetTypeOperation<Plot>()const
{ this->CheckInitialization();
  return this->theBoss->opCalculusPlot();
}

template < >
int Expression::GetTypeOperation<PlotObject>()const
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
int Expression::GetTypeOperation<ElementWeylGroup<WeylGroup> >()const
{ this->CheckInitialization();
  return this->theBoss->opWeylGroupElement();
}

template < >
int Expression::GetTypeOperation<WeylGroupVirtualRepresentation<Rational> >()const
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
  if (!this->theBoss->theObjectContainer.theSSsubalgebras.Contains(inputValue))
    crash << "Semisimple subalgebras must be allocated directly in the object container. " << crash;
  return this->theBoss->theObjectContainer.theSSsubalgebras.GetIndex(inputValue);
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
Weight<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeights
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
Weight<Polynomial<Rational> >
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeightsPoly
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
  if (!this->theBoss->theObjectContainer.theLieAlgebras.Contains(inputValue))
    crash << "Semisimple Lie algebra must be allocated directly in the object container. " << crash;
  return this->theBoss->theObjectContainer.theLieAlgebras.GetIndex(inputValue);
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
Plot
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.thePlots
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
PlotObject
& inputValue)const
{ this->CheckInitialization();
  Plot plotContainer;
  plotContainer+=inputValue;
  return this->theBoss->theObjectContainer.thePlots
  .AddNoRepetitionOrReturnIndexFirst(plotContainer);
}

template < >
int Expression::AddObjectReturnIndex(const
WeylGroup
& inputValue)const
{ this->CheckInitialization();
  int index=-1;
  for (int i=0; i<this->theBoss->theObjectContainer.theLieAlgebras.size; i++)
    if (this->theBoss->theObjectContainer.theLieAlgebras[i].theWeyl==inputValue)
    { index=i;
      break;
    }
  if (index==-1)
    crash << "Weyl group must be allocated directly in the object container. " << crash;
  return index;
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
WeylGroupVirtualRepresentation<Rational>
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theWeylGroupVirtualReps
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

template < >
int Expression::AddObjectReturnIndex(const
ElementWeylGroup<WeylGroup>
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
Weight<Polynomial<Rational> >& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Weight<Polynomial<Rational> > >())
    crash << "This is a programming error: expression not of required type LittelmannPath. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeightsPoly.GetElement(this->GetLastChild().theData);
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
Plot& Expression::GetValueNonConst()const
{ if (!this->IsOfType<Plot>())
    crash << "This is a programming error: expression not of required type Plot. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.thePlots[this->GetLastChild().theData];
}

template < >
WeylGroup& Expression::GetValueNonConst()const
{ if (!this->IsOfType<WeylGroup>())
    crash << "This is a programming error: expression not of required type WeylGroup. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theLieAlgebras.GetElement(this->GetLastChild().theData).theWeyl;
}

template < >
WeylGroupRepresentation<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<WeylGroupRepresentation<Rational> >())
    crash << "This is a programming error: expression not of required type WeylGroupRepresentation_Rational. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylGroupReps.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylGroup<WeylGroup>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<ElementWeylGroup<WeylGroup> >())
    crash << "This is a programming error: expression not of required type ElementWeylGroup. The expression equals " << this->ToString() << "." << crash;
  return this->theBoss->theObjectContainer.theWeylGroupElements.GetElement(this->GetLastChild().theData);
}

template < >
WeylGroupVirtualRepresentation<Rational>& Expression::GetValueNonConst()const
{ if (!this->IsOfType<WeylGroupVirtualRepresentation<Rational> >())
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
bool Expression::ConvertToType<Weight<Polynomial<Rational> > >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_Weight_Polynomial");
  this->CheckInitialization();
  if (this->IsOfType<Weight<Polynomial<Rational> > >())
  { output=*this;
    return true;
  }
  return false;
}

template< >
bool Expression::ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  //stOutput << "<hr>convert to ue called on: " << this->ToString();
  this->CheckInitialization();
  if (!this->IsBuiltInType())
    return false;
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { output=*this;
    return true;
  }
//  stOutput << "<hr>Getting ue from: " << this->ToString();
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
    << Crasher::GetStackTraceEtcErrorMessage();
    inputOutputMinContext.MakeEmptyContext(*this->theBoss);
  }
//  stOutput << "<br>Setting context at least equal to " << inputOutputMinContext.ToString()
//  << " in expression " << this->ToString();
  Expression newContext;
  Expression myOldContext=this->GetContext();
//  stOutput << "<br>In " << this->ToString() << ", merging my old context, " << myOldContext.Lispify() << " and the input context, " << inputOutputMinContext.Lispify();
  if (!this->ContextMergeContexts(myOldContext, inputOutputMinContext, newContext))
    return false;
  if (myOldContext==newContext)
    return true;
  inputOutputMinContext=newContext;
  if (this->IsOfType<Rational>())
  { this->SetChilD(1, inputOutputMinContext);
//    stOutput << "<hr>Context of rational set; rational is: " << this->ToString();
    return true;
  }
  if (this->IsOfType<ElementWeylGroup<WeylGroup> >())
    return this->AssignValueWithContext(this->GetValue<ElementWeylGroup<WeylGroup> >(), inputOutputMinContext, *this->theBoss);
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
    //stOutput << "<br>Subbing " << polySub.ToString() << " in "
    PolynomialSubstitution<Rational> subPolyPart;
    //<< newPoly.ToString();
    myOldContext.ContextGetPolySubFromSuperContextNoFailure<Rational>(newContext, subPolyPart);
    if (!newPoly.Substitution(subPolyPart))
      return false;
    return this->AssignValueWithContext(newPoly, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<Polynomial<AlgebraicNumber> >())
  { Polynomial<AlgebraicNumber> newPoly=this->GetValue<Polynomial<AlgebraicNumber> >();
    //stOutput << "<br>Subbing " << polySub.ToString() << " in "
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
//    stOutput << "<hr>Subbing in: " << outputEWA.ToString();
    if (!outputEWA.Substitution(subPolyPart, subEWApart))
    { this->theBoss->Comments << "<hr>Failed to convert " << outputEWA.ToString() << ": failed to carry out substitution "
      << subEWApart.ToString() << ", " << subPolyPart.ToString();
      return false;
    }
//    stOutput << "... to get: " << outputEWA.ToString();
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
  if (this->IsOfType<Weight<Polynomial<Rational> > >())
  { PolynomialSubstitution<Rational> subPolyPart;
    myOldContext.ContextGetPolySubFromSuperContextNoFailure(newContext, subPolyPart);
    Weight<Polynomial<Rational> > theWeight=this->GetValue<Weight<Polynomial<Rational> > >();
    for (int i=0; i<theWeight.weightFundamentalCoordS.size; i++)
      theWeight.weightFundamentalCoordS[i].Substitution(subPolyPart);
    return this->AssignValueWithContext(theWeight, inputOutputMinContext, *this->theBoss);
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
//  stOutput << "<br>left, right semisimple Lie algebra indices extracted from contexts " << leftContext.ToString() << " and "
//  << rightContext.ToString() << " are " << leftSSindex << ", " << rightSSindex << ". ";
  if (leftSSindex==-1)
    leftSSindex=rightSSindex;
  if (rightSSindex==-1)
    rightSSindex=leftSSindex;
  if (leftSSindex!=rightSSindex)
    return false;
//  stOutput << "<br>Merging contexts: " << leftContext.ToString() << " and " << rightContext.ToString();
  Expression leftPolyV=leftContext.ContextGetPolynomialVariables();
  Expression rightPolyV=rightContext.ContextGetPolynomialVariables();
//  stOutput << "<br>Merging contexts: " << leftContext.ToString() << " and " << rightContext.ToString();
  HashedList<Expression> polyVarUnion;
  MemorySaving<HashedList<Expression> > EWAVarUnion;
  polyVarUnion.SetExpectedSize(leftPolyV.children.size+rightPolyV.children.size-2);
  for (int i=1; i<leftPolyV.children.size; i++)
    polyVarUnion.AddOnTopNoRepetition(leftPolyV[i]);
  for (int i =1; i<rightPolyV.children.size; i++)
    polyVarUnion.AddOnTopNoRepetition(rightPolyV[i]);
  polyVarUnion.QuickSortAscending();
//  stOutput << "<br>Polyvarunion is: " << polyVarUnion.ToString();
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
//  stOutput << "<br>The result of the merge is: " << outputContext.ToString();
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

bool Expression::IsMonEqualToZero()const
{ return this->IsEqualToZero();
}

bool Expression::IsEqualToZero()const
{ if (this->IsOfType<Rational>())
    return this->GetValue<Rational>().IsEqualToZero();
  if (this->IsOfType<double>())
    return this->GetValue<double>()==0;
  if (this->IsOfType<Rational>())
    return this->GetValue<Rational>().IsEqualToZero();
  if (this->IsOfType<Polynomial<Rational> >())
    return this->GetValue<Polynomial<Rational> >().IsEqualToZero();
  if (this->IsOfType<RationalFunctionOld>())
    return this->GetValue<RationalFunctionOld>().IsEqualToZero();
  return false;
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

bool Expression::IsConstantNumber()const
{ if (this->theBoss==0)
    return false;
  if (this->IsAtomGivenData(this->theBoss->opPi()))
    return true;
  if (this->IsAtomGivenData(this->theBoss->opE()))
    return true;
//  stOutput << "testing for constant: " << this->ToString();
//  stOutput << " i am of type: " << this->theBoss->GetOperations()[(*this)[0].theData];
  return this->IsOfType<Rational>() || this->IsOfType<AlgebraicNumber>() || this->IsOfType<double>();
}

bool Expression::IsInteger(LargeInt* whichInteger)const
{ Rational theRat;
  if (!this->IsOfType<Rational>(&theRat))
    return false;
  return theRat.IsInteger(whichInteger);
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

bool Expression::MakeAtom(const std::string& atomName, Calculator& newBoss)
{ this->reset(newBoss);
  this->theData=newBoss.AddOperationNoRepetitionOrReturnIndexFirst(atomName);
  return true;
}
