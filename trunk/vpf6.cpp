//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6cpp(__FILE__, "Implementation file for the calculator parser. ");

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
int Expression::GetTypeOperation<AlgebraicNumber>()const
{ this->CheckInitialization();
  return this->theBoss->opAlgNumber();
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
SemisimpleLieAlgebra
& inputValue)const
{ this->CheckInitialization();
  return this->theBoss->theObjectContainer.theLieAlgebras
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

//start Expression::GetValuENonConstUseWithCaution specializations.
template < >
Rational& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<Rational>())
  { std::cout << "This is a programming error: expression not of required type Rational. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theRationals.GetElement(this->GetLastChild().theData);
}

template < >
double& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<double>())
  { std::cout << "This is a programming error: expression not of required type Rational. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theDoubles.GetElement(this->GetLastChild().theData);
}

template < >
std::string& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<std::string>())
  { std::cout << "This is a programming error: expression not of required type std::string. "
    << " The expression equals " << this->ToString() << "."
    <<  CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theStrings.GetElement(this->GetLastChild().theData);
}

template < >
RationalFunctionOld& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<RationalFunctionOld>())
  { std::cout << "This is a programming error: expression not of required type RationalFunctionOld."
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theRFs.GetElement(this->GetLastChild().theData);
}

template < >
ElementUniversalEnveloping<RationalFunctionOld>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { std::cout << "This is a programming error: expression not of required"
    << " type ElementUniversalEnveloping_RationalFunctionOld."
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theUEs.GetElement(this->GetLastChild().theData);
}

template < >
Polynomial<Rational>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<Polynomial<Rational> >())
  { std::cout << "This is a programming error: expression not of required type Polynomial_Rational."
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.thePolys.GetElement(this->GetLastChild().theData);
}

template < >
LittelmannPath& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<LittelmannPath>())
  { std::cout << "This is a programming error: expression not of required type LittelmannPath."
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theLSpaths.GetElement(this->GetLastChild().theData);
}

template < >
MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >())
  { std::cout << "This is a programming error: expression not of required type MonomialTensor_int."
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theLittelmannOperators.GetElement(this->GetLastChild().theData);
}

template < >
ElementTensorsGeneralizedVermas<RationalFunctionOld>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { std::cout << "This is a programming error: expression not of required type ElementTensorsGeneralizedVermas_RationalFunction."
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theTensorElts.GetElement(this->GetLastChild().theData);
}

template < >
charSSAlgMod<Rational>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<charSSAlgMod<Rational> >())
  { std::cout << "This is a programming error: expression not of required type charSSAlgMod_Rational. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theCharsSSLieAlgFD.GetElement(this->GetLastChild().theData);
}

template < >
AlgebraicNumber& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<AlgebraicNumber>())
  { std::cout << "This is a programming error: expression not of required type AlgebraicNumber. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theAlgebraicNumbers.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleLieAlgebra& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<SemisimpleLieAlgebra>())
  { std::cout << "This is a programming error: expression not of required type AlgebraicNumber. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theLieAlgebras[this->GetLastChild().theData];
}

template < >
Matrix<Rational>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<Matrix<Rational> >())
  { std::cout << "This is a programming error: expression not of required type Matrix_Rational. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theMatRats.GetElement(this->GetLastChild().theData);
}

template < >
Matrix<RationalFunctionOld>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<Matrix<RationalFunctionOld> >())
  { std::cout << "This is a programming error: expression not of required type Matrix_RF. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theMatRFs.GetElement(this->GetLastChild().theData);
}

template < >
SemisimpleSubalgebras& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<SemisimpleSubalgebras>())
  { std::cout << "This is a programming error: expression not of required type Matrix_Rational. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theSSsubalgebras[this->GetLastChild().theData];
}

template < >
CalculusFunctionPlot& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<CalculusFunctionPlot>())
  { std::cout << "This is a programming error: expression not of required type CalculusPlot. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.thePlots[this->GetLastChild().theData];
}

template < >
WeylGroup& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<WeylGroup>())
  { std::cout << "This is a programming error: expression not of required type WeylGroup. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theWeylGroups[this->GetLastChild().theData];
}

template < >
WeylGroupRepresentation<Rational>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<WeylGroupRepresentation<Rational> >())
  { std::cout << "This is a programming error: expression not of required type WeylGroupRepresentation. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theWeylGroupReps.GetElement(this->GetLastChild().theData);
}

template < >
ElementWeylGroup& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<ElementWeylGroup>())
  { std::cout << "This is a programming error: expression not of required type CoxeterGroup. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theWeylGroupElements.GetElement(this->GetLastChild().theData);
}

template < >
WeylGroupVirtualRepresentation& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<WeylGroupVirtualRepresentation>())
  { std::cout << "This is a programming error: expression not of required type CoxeterGroup. "
    << " The expression equals " << this->ToString() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theWeylGroupVirtualReps.GetElement(this->GetLastChild().theData);
}

//end Expression::GetValuENonConstUseWithCaution specializations.
//start Expression::ContextGetPolynomialMonomial specializations.
template< >
bool Expression::ConvertToType<Polynomial<Rational> >(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_Polynomial_Rational");
  this->CheckInitialization();
  if (this->IsOfType<Rational>())
  { Polynomial<Rational> resultP;
    resultP.MakeConst(this->GetValuE<Rational>());
    return output.AssignValueWithContext(resultP, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { output=*this;
    return true;
  }
//  std::cout << "<hr>My mark!";
//  std::cout << " this: " << this->Lispify();
  if (!this->HasContext())
    return false;
  Expression expressionRemovedContext=*this;
  expressionRemovedContext.RemoveContext();
//  std::cout << "<br>Converting to polynomial: " << this->ToString()
//  << ", without context: " << expressionRemovedContext.ToString();
  Expression myPolyVars=this->GetContext().ContextGetPolynomialVariables();
  int indexInPolyVars=myPolyVars.GetIndexChild(expressionRemovedContext);
//  std::cout << "<br>Expression without context " << expressionRemovedContext.ToString()
//  << " (Lisp: " << expressionRemovedContext.Lispify()
//  << ") has index " << indexInPolyVars << " in " << myPolyVars.ToString()
//  << ". Lisp: " << myPolyVars.Lispify();
  if (indexInPolyVars==-1)
    return false;
  Polynomial<Rational> resultP;
  resultP.MakeMonomiaL(indexInPolyVars-1, 1, 1);
  return output.AssignValueWithContext(resultP, this->GetContext(), *this->theBoss);
}

template< >
bool Expression::ConvertToType<RationalFunctionOld>(Expression& output)const
{ MacroRegisterFunctionWithName("ConvertToType_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<Rational>())
  { RationalFunctionOld resultRF;
    resultRF.MakeConst(this->GetValuE<Rational>(), this->theBoss->theGlobalVariableS);
    return output.AssignValueWithContext(resultRF, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { RationalFunctionOld resultRF;
    resultRF.context=this->theBoss->theGlobalVariableS;
    resultRF=this->GetValuE<Polynomial<Rational> >();
    return output.AssignValueWithContext(resultRF, this->GetContext(), *this->theBoss);
  }
  if (this->IsOfType<RationalFunctionOld>())
  { output=*this;
    return true;
  }
  if (!this->HasContext())
  { this->theBoss->Comments << "Failed to convert " << this->ToString()
    << " to RF because the expression had no context. ";
    return false;
  }
  Expression expressionRemovedContext=*this;
  expressionRemovedContext.RemoveContext();
  //std::cout << "<hr>Converting to rational function: " << this->ToString()
  //<< ", without context: " << expressionRemovedContext.ToString() << "<br>";
  //std::cout << "The stack trace, master: "
  //<< CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__) << "<hr>";

  Expression myPolyVars=this->GetContext().ContextGetPolynomialVariables();
  int indexInPolyVars=myPolyVars.GetIndexChild(expressionRemovedContext);
  if (indexInPolyVars==-1)
  { this->theBoss->Comments << "<hr>Failed to convert " << this->ToString()
    << " to RF because my context " << this->GetContext()
    << " did not contain have my context-stripped value "
    << expressionRemovedContext.ToString() << ". <hr>";
    return false;
  }
  RationalFunctionOld resultRF;
  resultRF.MakeOneLetterMoN(indexInPolyVars-1, 1, *this->theBoss->theGlobalVariableS);
  return output.AssignValueWithContext(resultRF, this->GetContext(), *this->theBoss);
}

template< >
bool Expression::ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >
(Expression& output)const
{ MacroRegisterFunctionWithName
  ("Expression::ConvertToType_ElementUniversalEnveloping_RationalFunctionOld");
  this->CheckInitialization();
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { output=*this;
    return true;
  }
  int algebraIndex=this->GetContext().ContextGetIndexAmbientSSalg();
  if (algebraIndex==-1)
    return false;
  if (!this->ConvertToType<RationalFunctionOld>(output))
    return false;
  RationalFunctionOld tempRF=output.GetValuE<RationalFunctionOld>();
  ElementUniversalEnveloping<RationalFunctionOld> resultElt;
  resultElt.MakeConst
  (tempRF, this->theBoss->theObjectContainer.theLieAlgebras[algebraIndex]);
  return output.AssignValueWithContext(resultElt, this->GetContext(), *this->theBoss);
}

//end Expression::ContextGetPolynomialMonomial specializations.
//begin Expression::GetContextForConversionIgnoreMyContext specializations.
template <>
bool Expression::GetContextForConversionIgnoreMyContext<Polynomial<Rational> >
(Expression& output)const
{ this->CheckInitialization();
  if (this->IsOfType<Rational>() || this->IsOfType<Polynomial<Rational> >())
    return output.MakeEmptyContext(*this->theBoss);
  output.reset(*this->theBoss, 2);
  output.AddChildAtomOnTop(this->theBoss->opContext());
  Expression tempE;
  tempE.reset(*this->theBoss, 2);
  tempE.AddChildAtomOnTop(this->theBoss->opPolynomialVariables());
  tempE.AddChildOnTop(*this);
  output.AddChildOnTop(tempE);
//  std::cout << "<br>Output context without mine: " << output.ToString();
  return true;
}

template <>
bool Expression::GetContextForConversionIgnoreMyContext<RationalFunctionOld>
(Expression& output)const
{ if (this->IsOfType<RationalFunctionOld>())
    return output.MakeEmptyContext(*this->theBoss);
  return this->GetContextForConversionIgnoreMyContext<Polynomial<Rational> >(output);
}

template <>
bool Expression::GetContextForConversionIgnoreMyContext
<ElementUniversalEnveloping<RationalFunctionOld> >(Expression& output)const
{ if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
    return output.MakeEmptyContext(*this->theBoss);
  return this->GetContextForConversionIgnoreMyContext<RationalFunctionOld>(output);
}
//end Expression::GetContextForConversionIgnoreMyContext specializations.

const Expression& Expression::operator[](int n)const
{ this->CheckInitialization();
  int childIndex=this->children[n];
  if (childIndex<0)
  { std::cout << "<hr>This is a programming error: the child of position " << n
    << " out of " << this->children.size-1
    << " is not contained in the expression container. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theExpressionContainer[childIndex];
}

bool Expression::AddChildOnTop(const Expression& inputChild)
{ this->CheckInitialization();
  this->children.AddOnTop
  (this->theBoss->theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(inputChild));
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
  if (!(*this)[0].IsAtoM())
    return false;
  if (theOp==-1)
    return true;
  return (*this)[0].theData==theOp;
}

int Expression::ContextGetNumContextVariables()const
{ if (this->theBoss==0)
    return 0;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opContext()))
    return 0;
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom
        (this->theBoss->opPolynomialVariables()))
      return (*this)[i].children.size-1;
  return 0;
}

bool Expression::SetContextAtLeastEqualTo(Expression& inputOutputMinContext)
{ MacroRegisterFunctionWithName("Expression::SetContextAtLeastEqualTo");
  this->CheckInitialization();
  if (!inputOutputMinContext.IsContext())
  { this->theBoss->Comments << "<br>Warning: non-initialized input context in "
    << "Expression::SetContextAtLeastEqualTo. Stack trace:"
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
  PolynomialSubstitution<Rational> polySub;
  bool mustBeTrue= myOldContext.ContextGetPolySubFromSuperContext(newContext, polySub);
  if (!mustBeTrue)
  { std::cout << "This is a programming error: I was not able to extract a polynomial "
    << " substitution from what is supposed to be a super-context. Something is wrong. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (this->IsOfType<Rational>())
  { if (this->children.size==2)
      this->AddChildOnTop((*this)[1]);
    this->SetChilD(1, inputOutputMinContext);
    return true;
  }
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> > ())
  { ElementUniversalEnveloping<RationalFunctionOld> newUE=
    this->GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >();
    newUE.Substitution(polySub);
    return this->AssignValueWithContext(newUE, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { Polynomial<Rational> newPoly=this->GetValuE<Polynomial<Rational> >();
    //std::cout << "<br>Subbing " << polySub.ToString() << " in "
    //<< newPoly.ToString();
    if (!newPoly.Substitution(polySub))
      return false;
    return this->AssignValueWithContext(newPoly, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<RationalFunctionOld>())
  { RationalFunctionOld newRF=this->GetValuE<RationalFunctionOld>();
    newRF.Substitution(polySub);
    return this->AssignValueWithContext(newRF, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { ElementTensorsGeneralizedVermas<RationalFunctionOld> newETGV
    =this->GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
    newETGV.Substitution(polySub, this->theBoss->theObjectContainer.theCategoryOmodules);
    return this->AssignValueWithContext(newETGV, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsBuiltInType())
  { this->theBoss->Comments << "Expression " << this->ToString() << " is of built-in type "
    << " but is not handled by Expression::SetContextAtLeastEqualTo";
    //return false;
  }
  Expression tempE=*this;
  this->reset(*this->theBoss, 2);
  this->AddChildOnTop(tempE);
  return this->AddChildOnTop(inputOutputMinContext);
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

bool Expression::ContextSetSSLieAlgebrA(int indexInOwners, CommandList& owner)
{ if (!this->IsContext())
  { std::cout << "This is a programming error: calling Expression::ContextSetSSLieAlgebrA "
    << "on a non-context expression. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int index=this->ContextGetIndexAmbientSSalg();
  if (index!=-1 && index!=indexInOwners)
    return false;
  if (index==indexInOwners)
    return true;
  Expression tempE;
  tempE.reset(owner, 2);
  tempE.AddChildAtomOnTop(owner.opSSLieAlg());
  tempE.AddChildAtomOnTop(indexInOwners);
  this->AddChildOnTop(tempE);
  return true;
}

Expression Expression::ContextGetPolynomialVariables()const
{ MacroRegisterFunctionWithName("Expression::ContextGetPolynomialVariables");
  this->CheckInitialization();
  if (!this->IsContext())
  { std::cout << "This is a programming error: calling Expression::ContextGetPolynomialVariables"
    << " on a non-context expression. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opPolynomialVariables()))
      return (*this)[i];
  Expression output;
  output.reset(*this->theBoss, 1);
  output.AddChildAtomOnTop(this->theBoss->opPolynomialVariables());
  return output;
}

bool Expression::ContextMergeContexts
(const Expression& leftContext, const Expression& rightContext, Expression& outputContext)
{ MacroRegisterFunctionWithName("Expression::ContextMergeContexts");
  if (&leftContext==&outputContext || &rightContext==&outputContext)
  { Expression leftCopy=leftContext;
    Expression rightCopy=rightContext;
    return Expression::ContextMergeContexts(leftCopy, rightCopy, outputContext);
  }
  int leftSSindex=leftContext.ContextGetIndexAmbientSSalg();
  int rightSSindex=rightContext.ContextGetIndexAmbientSSalg();
  if (leftSSindex==-1)
    leftSSindex=rightSSindex;
  if (rightSSindex==-1)
    rightSSindex=leftSSindex;
  if (leftSSindex!=rightSSindex)
    return false;
  Expression leftPolyV=leftContext.ContextGetPolynomialVariables();
  Expression rightPolyV=rightContext.ContextGetPolynomialVariables();
//  std::cout << "<br>Merging contexts: " << leftContext.ToString() << " and " << rightContext.ToString();
  HashedList<Expression> varUnion;
  List<int> varUnionIndices;
  varUnion.SetExpectedSize(leftPolyV.children.size+rightPolyV.children.size-2);
  for (int i =1; i<leftPolyV.children.size; i++)
    varUnion.AddOnTopNoRepetition(leftPolyV[i]);
  for (int i =1; i<rightPolyV.children.size; i++)
    varUnion.AddOnTopNoRepetition(rightPolyV[i]);
  varUnion.QuickSortAscending();
  varUnionIndices.SetSize(varUnion.size);
  CommandList& owner=*leftContext.theBoss;
  for (int i=0; i<varUnion.size; i++)
    varUnionIndices[i]=owner.theExpressionContainer.GetIndex(varUnion[i]);
  outputContext.reset(owner, 5+varUnion.size);
  outputContext.AddChildAtomOnTop(owner.opContext());
  if (varUnion.size>0)
  { Expression polyVarsE;
    polyVarsE.reset(owner, varUnion.size+1);
    polyVarsE.AddChildAtomOnTop(owner.opPolynomialVariables());
    polyVarsE.children.AddOnTop(varUnionIndices);
    outputContext.AddChildOnTop(polyVarsE);
  }
  if (leftSSindex!=-1)
  { Expression ssAlgE;
    ssAlgE.reset(owner, 2);
    ssAlgE.AddChildAtomOnTop(owner.opSSLieAlg());
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
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opContext()))
    return;
  Expression thePolyE=this->ContextGetPolynomialVariables();
  output.polyAlphabeT.SetSize(thePolyE.children.size-1);
  for (int i=1; i<thePolyE.children.size; i++)
    output.polyAlphabeT[i-1]=thePolyE[i].ToString();
}

bool Expression::IsContext()const
{ if (this->theBoss==0)
    return false;
  return this->IsListNElementsStartingWithAtom(this->theBoss->opContext());
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
    return this->GetValuE<Rational>().IsEqualToZero();
  if (this->IsOfType<Polynomial<Rational> >())
    return this->GetValuE<Polynomial<Rational> >().IsEqualToZero();
  if (this->IsOfType<RationalFunctionOld>())
    return this->GetValuE<RationalFunctionOld>().IsEqualToZero();
  return false;
}

CommandList::CommandList()
{ this->theGlobalVariableS=0;
  this->numOutputFiles=0;
  this->flagHideLHS=false;
}

std::string CommandList::WriteDefaultLatexFileReturnHtmlLink
(const std::string& fileContent, bool useLatexDviPSpsTopdf)
{ std::fstream theFile;
  std::stringstream fileName;
  std::stringstream systemCommand1, systemCommand2, systemCommand3;

  fileName << this->PhysicalNameDefaultOutput << this->numOutputFiles;
  CGI::OpenFileCreateIfNotPresent(theFile, fileName.str()+".tex", false, true, false);
  theFile << fileContent;
  theFile.flush();
  theFile.close();
  systemCommand1 << " latex -output-directory="
  << this->PhysicalPathOutputFolder << " " << fileName.str()+".tex";
//  std::cout << "<br>system command:<br>" << systemCommand1.str();
  this->SystemCommands.AddOnTop(systemCommand1.str());
  if (useLatexDviPSpsTopdf)
  { systemCommand2 << " dvips -o " <<  fileName.str() << ".ps "
    << fileName.str() << ".dvi";
//    std::cout << "<br>system command:<br>" << systemCommand2.str();
    this->SystemCommands.AddOnTop(systemCommand2.str());
    systemCommand3 << " convert " << fileName.str() << ".ps "
    << fileName.str() << ".png";
//    std::cout << "<br>system command:<br>" << systemCommand3.str();
    this->SystemCommands.AddOnTop(systemCommand3.str());
  }
  std::stringstream out;
  out << "<img src=\"" << this->DisplayNameDefaultOutput << this->numOutputFiles << ".png\">"
  << "</img><a href=\""<< this->DisplayNameDefaultOutput << this->numOutputFiles << ".png\">"
  << "output png</a>";
  this->numOutputFiles++;
  return out.str();
}

inline int IntIdentity(const int& x)
{ return x;
}

void Expression::MakeProducT
  (CommandList& owner, const Expression& left, const Expression& right)
{ this->MakeXOX(owner, owner.opTimes(), left, right);
}

void Expression::MakeFunction
  (CommandList& owner, const Expression& theFunction, const Expression& theArgument)
{ this->reset(owner, 2);
  this->AddChildOnTop(theFunction);
  this->AddChildOnTop(theArgument);
  this->format=this->formatFunctionUseUnderscore;
}

bool Expression::MakeEmptyContext(CommandList& owner)
{ this->reset(owner, 1);
  this->AddChildAtomOnTop(owner.opContext());
  return true;
}

bool Expression::MakeXOX
  (CommandList& owner, int theOp, const Expression& left, const Expression& right)
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

bool Expression::MakeOX
  (CommandList& owner, int theOp, const Expression& opArgument)
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

bool CommandList::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.format=formatOptions;
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(middleE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(7);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceOXXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-9];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(opElt.controlIndex));
  newExpr.AddChildOnTop(leftE.theData);
  newExpr.AddChildOnTop(middleE.theData);
  newExpr.AddChildOnTop(rightE.theData);
  newExpr.format= formatOptions;
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(8);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceOXEByE(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(left.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRangeS(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceOEXByEX(int formatOptions)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(right.theData);
  newExpr.format= formatOptions;
  middle.theData=newExpr;
  middle.controlIndex=this->conExpression();
  this->DecreaseStackExceptLast(1);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::isRightSeparator(char c)
{ switch(c)
  { case ' ':
    case '\n':
    case '>':
    case '<':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '=':
    case '_':
    case '%':
    case '/':
    case '-':
    case '`':
    case '[':
    case '&':
    case '\\':
    case '}':
    case '{':
    case '~':
    case '$':
    case ']':
    case '(':
    case ')':
    case '.':
      return true;
    default:
      return false;
  }
}

bool CommandList::isLeftSeparator(char c)
{ switch(c)
  { case ' ':
    case '\n':
    case '>':
    case '<':
    case ':':
    case ',':
    case ';':
    case '+':
    case '*':
    case '^':
    case '&':
    case '=':
    case '_':
    case '/':
    case '-':
    case '`':
    case '[':
    case '%':
    case '}':
    case '{':
    case '~':
    case '$':
    case ']':
    case '(':
    case ')':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':
      return true;
    default:
      return false;
  }
}

SyntacticElement CommandList::GetEmptySyntacticElement()
{ SyntacticElement result;
  result.controlIndex=this->controlSequences.GetIndex(" ");
  result.theData.reset(*this);
  return result;
}

std::string BoundVariablesSubstitution::ToString()
{ std::stringstream out;
  out << "the pairs: ";
  for (int i=0; i<this->theBoundVariables.size; i++)
    out << this->theBoundVariables[i].ToString() << "->"
    << this->variableImages[i].ToString() << "<br>";
  return out.str();
}

void CommandList::ParseFillDictionary
  (const std::string& input)
{ std::string current;
  (*this->CurrrentSyntacticSouP).ReservE(input.size());
  (*this->CurrrentSyntacticSouP).SetSize(0);
  char LookAheadChar;
  SyntacticElement currentElement;
  int currentDigit;
  for (unsigned i=0; i<input.size(); i++)
  { current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if (current=="\n")
      current=" ";
    if (this->isLeftSeparator(current[0]) ||
        this->isRightSeparator(LookAheadChar) || current==" ")
    { if (this->controlSequences.Contains(current))
      { currentElement.controlIndex=this->controlSequences.GetIndex(current);
        currentElement.theData.reset(*this);
        (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
      } else if (MathRoutines::isADigit(current, currentDigit))
      { currentElement.theData.AssignValue<Rational>(currentDigit, *this);
        currentElement.controlIndex=this->conInteger();
        (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
      } else
      { currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
        currentElement.theData.MakeAtom
        (this->AddOperationNoRepetitionOrReturnIndexFirst(current), *this);
        (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
       // std::cout << "<br>Adding syntactic element " << currentElement.ToString(*this);
      }
      current="";
    }
  }
  currentElement.theData.reset(*this);
  currentElement.controlIndex=this->conEndProgram();
  (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
}

int CommandList::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->operations.GetIndex(this->controlSequences[controlIndex]);
}

int CommandList::GetExpressionIndex()
{ return this->controlSequences.GetIndex("Expression");
}

bool CommandList::ReplaceOXbyE(int inputFormat)
{ this->ReplaceOXbyEX(inputFormat);
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool CommandList::ReplaceXOXbyEusingO(int theControlIndex, int inputFormat)
{ this->ReplaceOXbyEXusingO(theControlIndex, inputFormat);
  return this->ReplaceXEXByE(inputFormat);
}

bool CommandList::ReplaceOXbyEX(int inputFormat)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  return this->ReplaceOXbyEXusingO(theElt.controlIndex, inputFormat);
}

bool CommandList::ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence(int inputFormat)
{ SyntacticElement& theSequenceElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& theFunctionElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.ReservE(theSequenceElt.theData.children.size);
  newExpr.AddChildOnTop(theFunctionElt.theData);
  if (theSequenceElt.theData.IsAtoM())
    newExpr.AddChildOnTop(theSequenceElt.theData);
  else
    for (int i=1; i<theSequenceElt.theData.children.size; i++)
      newExpr.AddChildOnTop(theSequenceElt.theData[i]);
  theFunctionElt.theData=newExpr;
  return this->DecreaseStackSetCharacterRangeS(4);
}

bool CommandList::ReplaceOXbyEXusingO(int theControlIndex, int inputFormat)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theElt.theData.format=inputFormat;
  theElt.controlIndex=this->conExpression();
  return true;
}

bool CommandList::ReplaceAXbyEX()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
//  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
//  std::cout << "replaceAbyE: " << theElt.theData.ToString();
  return true;
}

bool CommandList::LookAheadAllowsApplyFunction(const std::string& lookAhead)
{ return lookAhead!="{" && lookAhead!="_" && lookAhead!="\\circ" && lookAhead!="{}" &&  lookAhead!="$";
}

bool CommandList::ReplaceSequenceUXEYBySequenceZY(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& afterleft = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  left.theData.AddChildOnTop(right.theData);
  left.theData.format=inputFormat;
  left.controlIndex=theControlIndex;
  afterleft=*this->CurrentSyntacticStacK->LastObject();
  this->DecreaseStackExceptLast(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceSequenceXEBySequence(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  left.theData.AddChildOnTop(right.theData);
  left.theData.format=inputFormat;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int inputFormat, int numXs)
{ SyntacticElement& main = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(this->opSequence());
  newExpr.AddChildOnTop(main.theData);
  newExpr.format=inputFormat;
  main.theData=newExpr;
  main.controlIndex=theControlIndex;
  return true;
}

bool CommandList::ReplaceEXEBySequence(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->opSequence());
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=inputFormat;
  left.theData=newExpr;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRangeS(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEEByEusingO(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  //    newExpr.inputIndexBoundVars=
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(1);
  //    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEOXbyEX()
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& opElt = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  int theOp=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr.AddChildAtomOnTop(theOp);
  newExpr.AddChildOnTop(left.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(1);
  //    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEEXByEXusingO(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  //    newExpr.inputIndexBoundVars=
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(1);
  //    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXXEXEByEusingO(int theControlIndex)
{ SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRangeS(5);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXXEXEXByEXusingO(int theControlIndex)
{ SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-7];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 4);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(theControlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(middle.theData);
  newExpr.AddChildOnTop(right.theData);
  left.theData=newExpr;
  this->DecreaseStackExceptLast(5);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEOEXByEX(int formatOptions)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr.AddChildAtomOnTop(this->GetOperationIndexFromControlIndex(middle.controlIndex));
  newExpr.AddChildOnTop(left.theData);
  newExpr.AddChildOnTop(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  middle=*(*this->CurrentSyntacticStacK).LastObject();
//  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  this->DecreaseStackExceptLast(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::isSeparatorFromTheLeftGeneral(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input=="," || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" || input=="&";
}

bool CommandList::isSeparatorFromTheRightGeneral(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input=="," || input==":" || input==";" || input=="MatrixSeparator"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end" || input=="&" || input=="EndProgram";
}

bool CommandList::isSeparatorFromTheLeftForList(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" ;
}

bool CommandList::isSeparatorFromTheLeftForListMatrixRow(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" ;
}

bool CommandList::isSeparatorFromTheLeftForMatrixRow(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" ;
}

bool CommandList::isSeparatorFromTheRightForList(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" || input=="MatrixSeparator"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end";
}

bool CommandList::isSeparatorFromTheRightForListMatrixRow(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" || input=="MatrixSeparator"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end";
}

bool CommandList::isSeparatorFromTheRightForMatrixRow(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input==":" || input==";" || input=="MatrixSeparator" || input=="&"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end";
}

bool CommandList::isSeparatorFromTheLeftForDefinition(const std::string& input)
{ return input=="{" || input=="(" || input==";" || input=="SequenceStatements" || input==" ";
}

bool CommandList::isSeparatorFromTheRightForDefinition(const std::string& input)
{ return input=="}" || input==")" || input==";" || input=="EndProgram";
}

bool CommandList::AllowsTensorInPreceding(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="*" || lookAhead=="/" ||
    lookAhead=="Expression" || lookAhead==")" ||
    lookAhead=="(" || lookAhead=="[" ||
    lookAhead=="=" || lookAhead=="\\otimes" ||
//    lookAhead=="{" ||
    lookAhead=="Variable" || lookAhead=="," ||
    lookAhead==";" || lookAhead=="]" ||
    lookAhead=="}" || lookAhead==":" || lookAhead=="EndProgram"
    ;
}

bool CommandList::AllowsTimesInPreceding(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="*" || lookAhead=="/" ||
    lookAhead=="Expression" || lookAhead==")" ||
    lookAhead=="(" || lookAhead=="[" ||
    lookAhead=="=" ||
//    lookAhead=="{" ||
    lookAhead=="Variable" || lookAhead=="," ||
    lookAhead==";" || lookAhead=="]" ||
    lookAhead=="}" || lookAhead==":" ||
    lookAhead=="&" || lookAhead=="MatrixSeparator" || lookAhead=="\\" ||
    lookAhead=="EndProgram"
    ;
}

bool CommandList::AllowsPlusInPreceding(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="," || lookAhead=="=" ||
    lookAhead==")" || lookAhead==";" ||
    lookAhead=="]" || lookAhead=="}" ||
    lookAhead==":" || lookAhead=="," ||
    lookAhead=="EndProgram" ||
    lookAhead=="&" || lookAhead=="MatrixSeparator" || lookAhead=="\\";
    ;
}

bool CommandList::LookAheadAllowsDivide(const std::string& lookAhead)
{ return this->AllowsTimesInPreceding(lookAhead);
}

bool CommandList::ApplyOneRule()
{ //return false;
  MacroRegisterFunctionWithName("CommandList::ApplyOneRule");
  if (this->CurrentSyntacticStacK->size<=this->numEmptyTokensStart)
    return false;
  const SyntacticElement& lastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  const std::string& lastS=this->controlSequences[lastE.controlIndex];
  if (lastS==" ")
    return  this->PopTopSyntacticStack();
  const SyntacticElement& secondToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  const std::string& secondToLastS=this->controlSequences[secondToLastE.controlIndex];
  const SyntacticElement& thirdToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  const std::string& thirdToLastS=this->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  const std::string& fourthToLastS=this->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  const std::string& fifthToLastS=this->controlSequences[fifthToLastE.controlIndex];
  const SyntacticElement& sixthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  const std::string& sixthToLastS=this->controlSequences[sixthToLastE.controlIndex];
  const SyntacticElement& seventhToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-7];
  const std::string& seventhToLastS=this->controlSequences[seventhToLastE.controlIndex];
  const SyntacticElement& eighthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  const std::string& eighthToLastS=this->controlSequences[eighthToLastE.controlIndex];
//  const SyntacticElement& ninthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-9];
//  const std::string& ninthToLastS=this->theBoss->controlSequences[ninthToLastE.controlIndex];
  if (this->flagLogSyntaxRules)
    this->parsingLog+= "<hr>" + this->ElementToStringSyntacticStack();
  if (secondToLastS=="%" && lastS=="LogParsing")
  { this->parsingLog+= "<hr>" + this->ElementToStringSyntacticStack();
    this->flagLogSyntaxRules=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogFull")
  { std::cout
    << "<hr>You are requesting a full log of the evaluation process. "
    << "<br><b>WARNING requesting  a full log of the evaluation process is very slow, "
    << " and might produce a HUGE printout. </b>"
    << "<br><b>You have been warned. </b><hr>";
    this->flagLogFullTreeCrunching=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LogEvaluation")
  { this->flagLogEvaluatioN=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="LatexLink")
  { this->flagProduceLatexLink=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="FullTree")
  { this->flagDisplayFullExpressionTree=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }
  if (secondToLastS=="%" && lastS=="HideLHS")
  { this->flagHideLHS=true;
    this->PopTopSyntacticStack();
    return this->PopTopSyntacticStack();
  }

/*  if (lastE.theData.IndexBoundVars==-1)
  { std::cout << "<hr>The last expression, " << lastE.ToString(*this) << ", while reducing "
    << this->ElementToStringSyntacticStack()
    << " does not have properly initialized context. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__,  __LINE__);
    assert(false);
  }*/
  if (secondToLastS==":" && lastS=="=")
    return this->ReplaceXXByCon(this->conDefine());
  if (secondToLastS==":=" && lastS==":")
    return this->ReplaceXXByCon(this->conIsDenotedBy());
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->conApplyFunction(), Expression::formatDefault);
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->conApplyFunction(), Expression::formatFunctionUseCdot);
//  if ( thirdToLastS=="{" && secondToLastS=="{}" && lastS=="}")
//    return this->ReplaceXXXByCon(this->conBindVariable());
  if (fifthToLastS=="{" && fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS=="}")
    return this->ReplaceXXVXdotsXbyE_BOUND_XdotsX(2);
  if (secondToLastS=="Variable" && ((lastS!="}" && lastS!=" ") || thirdToLastS!="{" || fourthToLastS!="{"))
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(1);
  if (fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS!="}" && lastS!=" ")
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(2);
  if (secondToLastS=="\\left" && lastS == "(")
    return this->ReplaceXYByY();
  if (secondToLastS=="\\right" && lastS == ")")
    return this->ReplaceXYByY();
  if (lastS=="=" && secondToLastS=="=")
    return this->ReplaceXXByCon(this->conEqualEqual());
  if (lastS=="Integer" && secondToLastS=="Integer")
    return this->ReplaceIntIntBy10IntPlusInt();
  if (secondToLastS=="Integer" && lastS!="Integer" && lastS!=".")
  { this->registerPositionAfterDecimalPoint=0;
    return this->ReplaceAXbyEX();
  }
  if (thirdToLastS=="Integer" && secondToLastS=="." && lastS=="Integer" && this->registerPositionAfterDecimalPoint==0)
  { this->registerPositionAfterDecimalPoint=1;
    return this->ReplaceXYByY();
  }
  //there is an ambiguity on how should function application be associated
  //Which is better: x{}y{}z:= x{} (y{}z), or x{}y{}z:=(x{}y){}z ?
  //In our implementation, we choose x{}y{}z:= x{} (y{}z). Although this is slightly harder to implement,
  //it appears to be the more natural choice.
//  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression"
//      && this->LookAheadAllowsApplyFunction(lastS) )
//    return this->ReplaceEOEXByEX(secondToLastE.theData.format);
  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression" && this->LookAheadAllowsApplyFunction(lastS))
    return this->ReplaceEXEXByEX(Expression::formatDefault);
  if (fourthToLastS=="Expression" && thirdToLastS=="_" && secondToLastS=="Expression" && lastS!="_")
    return this->ReplaceEXEXByEX(Expression::formatFunctionUseUnderscore);
  //end of ambiguity.
  if (fourthToLastS=="{"  && thirdToLastS=="{}" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXYYXByYY();
  if (secondToLastS=="Expression" && thirdToLastS=="^" && fourthToLastS=="Expression" && this->LookAheadAllowsThePower(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="+" && fourthToLastS=="Expression" && this->AllowsPlusInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" && fourthToLastS=="Expression" && this->AllowsPlusInPreceding(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="-" && this->AllowsPlusInPreceding(lastS) )
    return this->ReplaceOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="\\otimes" && fourthToLastS=="Expression" && this->AllowsTensorInPreceding(lastS))
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="*" && fourthToLastS=="Expression" && this->AllowsTimesInPreceding(lastS) )
    return this->ReplaceEOEXByEX(Expression::formatTimesDenotedByStar);
  if (secondToLastS=="Expression" && thirdToLastS=="/" && fourthToLastS=="Expression" && this->LookAheadAllowsDivide(lastS) )
    return this->ReplaceEOEXByEX();
  if (secondToLastS=="Expression" && thirdToLastS=="Expression" && this->AllowsTimesInPreceding(lastS) )
    return this->ReplaceEEXByEXusingO(this->conTimes());
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE(secondToLastE.theData.format);
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXEXByE(secondToLastE.theData.format);
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (this->isSeparatorFromTheRightGeneral(lastS) && secondToLastS=="Expression" && thirdToLastS=="==" && fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheRightGeneral(lastS) && secondToLastS=="Expression" && thirdToLastS==">" &&
      fourthToLastS=="Expression")
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) &&
      fourthToLastS=="Expression" && thirdToLastS==":=" && secondToLastS=="Expression" &&
      this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEOEXByEX();
  if (this->isSeparatorFromTheLeftForDefinition(fifthToLastS) && fourthToLastS=="Expression" &&
      thirdToLastS==":=:" && secondToLastS=="Expression" &&
      this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Expression" && thirdToLastS=="\\cup" && secondToLastS== "Expression" && this->isSeparatorFromTheRightGeneral(lastS))
    return this->ReplaceEOEXByEX();
  if (lastS=="Sequence" && lastE.theData.children.size==0 && lastE.theData.theData==this->opLisT())
    return this->ReplaceXByCon(this->controlSequences.GetIndexIMustContainTheObject("MakeSequence"));
  //else
  //  std::cout << "lastS is sequence but lastE is |" << lastE.theData.ToString() << "|";
  if (fourthToLastS=="Expression" && thirdToLastS=="\\sqcup" && secondToLastS== "Expression" && this->isSeparatorFromTheRightGeneral(lastS))
    return this->ReplaceEOEXByEX();
  if (fourthToLastS=="Sequence" && thirdToLastS=="," && secondToLastS=="Expression" &&
      (lastS=="," || lastS==")" || lastS=="}"))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conSequence());
  if (thirdToLastS!="[" && secondToLastS=="Expression" && lastS==",")
    return this->ReplaceYXBySequenceX(this->conSequence(), secondToLastE.theData.format);
  if (thirdToLastS=="MakeSequence" && secondToLastS=="{}" && lastS=="Expression")
    return this->ReplaceXXYBySequenceY(this->conSequence());
  if (fourthToLastS=="MakeSequence" && thirdToLastS=="{}" && secondToLastS=="Expression")
    return this->ReplaceXXYXBySequenceYX(this->conSequence());
  if (fifthToLastS=="Expression" && fourthToLastS== "{}" && thirdToLastS=="(" &&
      secondToLastS=="Sequence" && lastS==")")
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  if (fifthToLastS=="Expression" && fourthToLastS== "{}" && thirdToLastS=="{" &&
      secondToLastS=="Sequence" && lastS=="}")
    return this->ReplaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  if (secondToLastS=="Sequence" && lastS!=",")
    return this->ReplaceOXbyEX();
  if (fifthToLastS=="\\begin" && fourthToLastS=="{" && thirdToLastS=="array" && secondToLastS=="}" && lastS=="Expression")
  { this->registerNumNonClosedBeginArray++;
    return this->ReplaceXXXXXByCon(this->conMatrixSeparator(), Expression::formatMatrix);
  }
  if (fourthToLastS=="\\end" && thirdToLastS=="{" && secondToLastS=="array" && lastS=="}" )
  { this->registerNumNonClosedBeginArray--;
    return this->ReplaceXXXXByCon(this->conMatrixSeparator(), Expression::formatMatrix);
  }
  if (lastS=="\\" && secondToLastS== "\\")
    return this->ReplaceXXByCon(this->controlSequences.GetIndexIMustContainTheObject("\\\\"));
  if (lastS=="\\\\")
  { if (this->registerNumNonClosedBeginArray>0)
      return this->ReplaceXByCon(this->conMatrixRowSeparator(), Expression::formatMatrixRow);
    else
      return this->PopTopSyntacticStack();
  }
  if (fourthToLastS=="SequenceMatrixRows" && thirdToLastS=="MatrixRowSeparator" && secondToLastS=="Expression" && this->isSeparatorFromTheRightForListMatrixRow(lastS))
    return this->ReplaceYXdotsXBySequenceYXdotsX(this->conMatrixRow(), Expression::formatMatrixRow, 1);
  if (fourthToLastS=="SequenceMatrixRows" && thirdToLastS=="MatrixRowSeparator" && secondToLastS=="MatrixRow" && this->isSeparatorFromTheRightForListMatrixRow(lastS))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conSequenceMatrixRow(), Expression::formatMatrix);
  if (fourthToLastS=="MatrixRow" && thirdToLastS=="&"  && secondToLastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lastS))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (fourthToLastS=="MatrixRow" && thirdToLastS=="&"  && secondToLastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lastS))
    return this->ReplaceSequenceUXEYBySequenceZY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="Expression" && lastS=="&")
    return this->ReplaceYXBySequenceX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="Expression" && (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXBySequenceX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="MatrixRow" &&  (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXBySequenceX(this->conSequenceMatrixRow(), Expression::formatMatrix);
  if (thirdToLastS=="MatrixSeparator" && secondToLastS=="SequenceMatrixRows" && lastS=="MatrixSeparator")
    return this->ReplaceXOXbyEusingO(this->conSequence(), Expression::formatMatrix);
  if (secondToLastS=="MatrixRowSeparator" && lastS == "MatrixSeparator")
    return this->ReplaceXYByY();
  if (fifthToLastS=="[" && fourthToLastS=="Expression" && thirdToLastS=="," && secondToLastS=="Expression" && lastS=="]")
    return this->ReplaceXEXEXByEusingO(this->conLieBracket());
  if (this->isSeparatorFromTheLeftForDefinition(eighthToLastS) &&
      seventhToLastS=="Expression" && sixthToLastS==":" &&
      fifthToLastS=="if" && fourthToLastS=="Expression" &&
      thirdToLastS==":=" && secondToLastS=="Expression" &&
      this->isSeparatorFromTheRightForDefinition(lastS))
    return this->ReplaceEXXEXEXByEXusingO(this->conDefineConditional());
  if (lastS==";")
  { this->NonBoundVariablesStack.LastObject()->Clear();
    this->BoundVariablesStack.LastObject()->Clear();
  }
  if (thirdToLastS=="SequenceStatements" && secondToLastS=="Expression" &&
      (lastS==")" || lastS =="}"))
    return this->ReplaceEXdotsXbySsXdotsX(1);
  if (secondToLastS=="Expression" && lastS==";")
    return this->ReplaceEXdotsXBySs(1);
  if (secondToLastS=="Expression" && lastS=="EndProgram")
    return this->ReplaceEXdotsXbySsXdotsX(1);
  if ((thirdToLastS=="(" || thirdToLastS=="{" ) && secondToLastS=="SequenceStatements" &&
      (lastS==")" || lastS== "}"))
    return this->ReplaceXEXByE();
  if (secondToLastS=="SequenceStatements" && lastS=="SequenceStatements")
    return this->ReplaceSsSsXdotsXbySsXdotsX(0);
  if (thirdToLastS=="SequenceStatements" && secondToLastS=="SequenceStatements" &&
      (lastS=="EndProgram" || lastS ==")" || lastS=="}"))
    return this->ReplaceSsSsXdotsXbySsXdotsX(1);
  if (secondToLastS=="SequenceStatements" && lastS=="EndProgram")
    return this->ReplaceXXByCon(this->conExpression());
  if (lastS=="EndProgram")
    return this->DecreaseStackSetCharacterRangeS(1);
  return false;
}

bool CommandList::fHWTAABF
  (CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fHWTAABF");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError("Function expects three arguments.", theCommands);
  Expression leftExpression, rightExpression, finalContext;
  const Expression& weightExpression=input[3];
  if (!theCommands.innerElementUniversalEnvelopingAlgebra
      (theCommands, input[1], leftExpression))
    return output.SetError("Failed to convert left expression to element UE.", theCommands);
  if (!theCommands.innerElementUniversalEnvelopingAlgebra
      (theCommands, input[2], rightExpression ))
    return output.SetError("Failed to convert left expression to element UE.", theCommands);
  if (!Expression::ContextMergeContexts
      (leftExpression.GetContext(), rightExpression.GetContext(), finalContext))
    return output.SetError
    ("Failed to merge the contexts of the two UE elements ", theCommands);
  int algebraIndex=finalContext.ContextGetIndexAmbientSSalg();
  if (algebraIndex==-1)
    return output.SetError("I couldn't extract a Lie algebra to compute hwtaabf.", theCommands);
  SemisimpleLieAlgebra* constSSalg= &theCommands.theObjectContainer.theLieAlgebras[algebraIndex];
  Vector<RationalFunctionOld> weight;
  if (!theCommands.GetVectoR<RationalFunctionOld>
      (weightExpression, weight, &finalContext, constSSalg->GetRank(), theCommands.innerRationalFunction))
  { std::stringstream errorStream;
    errorStream << "Error: could not obtain highest weight from the third argument which is "
    << weightExpression.ToString();
    return output.SetError(errorStream.str(), theCommands);
  }
  if (!leftExpression.SetContextAtLeastEqualTo(finalContext) ||
      ! rightExpression.SetContextAtLeastEqualTo(finalContext))
    return output.SetError
    ("Failed to merge the contexts of the highest weight and the elements of the Universal enveloping. ", theCommands);
  //std::cout << "<br>starting left elt after the very final conversion: " << leftD.ToString();
  //std::cout << "<br>starting right elt after conversion: " << rightD.ToString();
  //std::cout << "<br>Highest weight in fundamental coords after conversion: " << weight.ToString();
  //std::cout << "<br>Left context: " << leftD.GetContext().ToString();
  //std::cout << "<br>Right context: " << rightD.GetContext().ToString();

  RationalFunctionOld theRingZero, theRingUnit;
  theRingZero.MakeZero(theCommands.theGlobalVariableS);
  theRingUnit.MakeOne(theCommands.theGlobalVariableS);
  SemisimpleLieAlgebra theSSalgebra;
  theSSalgebra=*constSSalg;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  Vector<RationalFunctionOld> hwDualCoords;

  theSSalgebra.OrderSSalgebraForHWbfComputation();
  hwDualCoords=theWeyl.GetDualCoordinatesFromFundamental(weight);
  RationalFunctionOld outputRF;
  //std::cout << "<br>The highest weight in dual coordinates, as I understand it:" << hwDualCoords.ToString();
  if(!leftExpression.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >().HWTAAbilinearForm
     (rightExpression.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >(), outputRF,
      &hwDualCoords, *theCommands.theGlobalVariableS, theRingUnit, theRingZero, &theCommands.Comments))
    return output.SetError("Error: couldn't compute Shapovalov form, see comments.", theCommands);
  return output.AssignValueWithContext(outputRF, finalContext, theCommands);
}

template<class coefficient>
bool CommandList::innerGetTypeHighestWeightParabolic
(CommandList& theCommands, const Expression& input, Expression& output,
 Vector<coefficient>& outputWeightHWFundcoords, Selection& outputInducingSel,
 Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra,
 Expression::FunctionAddress ConversionFun)
{ if (!input.IsListNElements(4) && !input.IsListNElements(3))
    return output.SetError
    ("Function TypeHighestWeightParabolic is expected to have two or three arguments: \
     SS algebra type, highest weight, [optional] parabolic selection. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& middleE=input[2];
  std::string errorString;
  if (!CommandList::CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, leftE, ambientSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  if (!theCommands.GetVectoR<coefficient>
      (middleE, outputWeightHWFundcoords, &outputHWContext, ambientSSalgebra->GetRank(),
       ConversionFun))
  { std::stringstream tempStream;
    tempStream << "Failed to convert the second argument of HWV to a list of "
    << ambientSSalgebra->GetRank() << " polynomials. The second argument you gave is "
    << middleE.ToString() << ".";
    return output.SetError(tempStream.str(), theCommands);
  }
  if (input.IsListNElements(4))
  { Vector<Rational> parabolicSel;
    const Expression& rightE=input[3];
    if (!theCommands.GetVectoR<Rational>
        (rightE, parabolicSel, &outputHWContext, ambientSSalgebra->GetRank(), 0))
    { std::stringstream tempStream;
      tempStream << "Failed to convert the third argument of HWV to a list of "
      << ambientSSalgebra->GetRank()
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
  { std::cout << "This is a programming error: " << ambientSSalgebra->GetLieAlgebraName()
    << " contained object container more than once. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int algebraIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*ambientSSalgebra);
  outputHWContext.ContextSetSSLieAlgebrA(algebraIndex, theCommands);
//  std::cout << "final context of innerGetTypeHighestWeightParabolic: " << outputHWContext.ToString();
  return true;
}

bool CommandList::fDecomposeCharGenVerma
  (CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::fDecomposeCharGenVerma");
  Expression theContext;
  Vector<RationalFunctionOld> theHWfundcoords, theHWsimpCoords, theHWFundCoordsFDPart, theHWSimpCoordsFDPart;
  Selection parSel, invertedParSel;
  SemisimpleLieAlgebra* theSSlieAlg=0;
  output.reset(theCommands);
  if (!theCommands.innerGetTypeHighestWeightParabolic<RationalFunctionOld>
      (theCommands, input, output, theHWfundcoords, parSel, theContext, theSSlieAlg,
       theCommands.innerRationalFunction))
   return false;
  if (output.IsError())
    return true;
  std::stringstream out;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  out << "<br>Highest weight: " << theHWfundcoords.ToString(&theFormat)
  << "<br>Parabolic selection: " << parSel.ToString();
  theHWFundCoordsFDPart=theHWfundcoords;
  for (int i=0; i<parSel.CardinalitySelection; i++)
    theHWFundCoordsFDPart[parSel.elements[i]]=0;
  KLpolys theKLpolys;
  WeylGroup theWeyl;
  theWeyl=theSSlieAlg->theWeyl;
  if (!theKLpolys.ComputeKLPolys(&theWeyl))
    return output.SetError
    ("failed to generate Kazhdan-Lusztig polynomials (output too large?)", theCommands);
//  Vectors<Polynomial<Rational> > tempVects;
//  tempVects.AddOnTop(theSSlieAlg.theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords) );
//  HashedList<Vector<Polynomial<Rational> > > theOrbit;
//  if (!theSSlieAlg.theWeyl.GenerateOrbit(tempVects, true, theOrbit, false, -1, 0, 1000))
//    out << "Error: failed to generate highest weight \rho-modified orbit (too large?)";
  theHWSimpCoordsFDPart=theWeyl.GetSimpleCoordinatesFromFundamental
  (theHWFundCoordsFDPart);
  theHWSimpCoordsFDPart+=theWeyl.rho;
//  ElementWeylGroup raisingElt;
//  theSSlieAlg.theWeyl.RaiseToDominantWeight
//  (theHWSimpCoordsFDPart, 0, 0, &raisingElt);
  ReflectionSubgroupWeylGroup theSub;
  if (!theSub.MakeParabolicFromSelectionSimpleRoots
      (theWeyl, parSel, *theCommands.theGlobalVariableS, 1000))
    return output.SetError
    ("Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).", theCommands);
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
    out << "<tr><td>" << currentHW.ToString() << "</td><td>"
    << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToString() << "</td></tr>";
  }
  out << "</table>";
  out << "<br>The rho of the Levi part is: " << theSub.GetRho().ToString()
  << "<br>Weyl group of Levi part follows. "
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
    { std::cout << "This is a programming error. Something is wrong: I am getting that an element "
      << "of the Weyl group of the Levi part of the parabolic does not lie in the ambient Weyl group, which "
      << " is impossible. There is a bug somewhere; crashing in accordance. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    currentChar.MakeZero();
    theMon.owner=theSSlieAlg;
    for (int j=0; j< theKLpolys.theKLcoeffs[indexInWeyl].size; j++)
      if (!theKLpolys.theKLcoeffs[indexInWeyl][j].IsEqualToZero())
      { currentHW=theHWsimpCoords;
//        currentHW+=theSub.GetRho();
        theWeyl.ActOnRhoModified(j, currentHW);
//        currentHW-=theSub.GetRho();
        theMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(currentHW);
        int sign= (currentElt.size- theWeyl.theElements[j].size)%2==0 ? 1 :-1;
        currentChar.AddMonomial(theMon, theKLpolys.theKLcoeffs[indexInWeyl][j]*sign);
      }
    currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(indexInWeyl, currentHW);
    currentHW-=theSub.GetRho();
    out << "<td>" << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToStringLetterFormat("\\omega")
    << "</td>";
    out << "<td>" << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(currentChar.ToString(&formatChars)) << "</td>";
    if (currentElt.size%2==1)
      currentChar*=-1;
    theChar+=currentChar;
    out << "</tr>";
  }
  out << "</table>";
  out << "Final char: " << CGI::GetHtmlMathDivFromLatexAddBeginArrayL(theChar.ToString(&formatChars));
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool CommandList::innerPrintGenVermaModule
(CommandList& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.innerGetTypeHighestWeightParabolic
      (theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra,
       theCommands.innerRationalFunction) )
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  if (!theCommands.innerHWVCommon
      (theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra, false))
    return output.SetError("Failed to create Generalized Verma module", theCommands);
  if (output.IsError())
    return true;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt=
  output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  ModuleSSalgebra<RationalFunctionOld>&
  theModule=theElt.GetOwnerModule();
  return output.AssignValue(theModule.ToString(), theCommands);
}

bool CommandList::innerHWV
(CommandList& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.innerGetTypeHighestWeightParabolic
      (theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra,
       theCommands.innerRationalFunction) )
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  return theCommands.innerHWVCommon
  (theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra);
}

bool CommandList::fWriteGenVermaModAsDiffOperatorUpToLevel
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError
    ("Function fSplitGenericGenVermaTensorFD is expected to have three arguments: \
     SS algebra type, Number, List{}. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& levelNode=input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  std::string errorString;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, leftE, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVectoR
      (genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, Serialization::innerPolynomial))
  { theCommands.Comments
    << "Failed to convert the third argument of fSplitGenericGenVermaTensorFD to a list of "
    << theRank << " polynomials. The second argument you gave is "
    << genVemaWeightNode.ToString() << ".";
    return false;
  }
  int desiredHeight;
  if (!levelNode.IsSmallInteger(&desiredHeight))
    return output.SetError
    ("second argument of " + input.ToString()+ " must be a small integer", theCommands);

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
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHws, hwContext, selInducing, theSSalgebra);
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInParabolic
(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]]<0)
      return true;
  return false;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::IsNotInLevi
(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].IsEqualToZero())
      return true;
  return false;
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetGenericUnMinusElt
 (bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<RationalFunctionOld>& output,
  GlobalVariables& theGlobalVariables)
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
  tempMon.MakeConst(*this->owneR);
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
  std::string ToString(FormatExpressions* theFormat=0)const
  ;
  void GenerateBasisLieAlgebra
  (List<quasiDiffOp<coefficient> >& theElts,
 FormatExpressions* theFormat=0, GlobalVariables* theGlobalVariables=0)
  ;
  void operator*=(const quasiDiffOp<coefficient>& standsOnTheRight);
  void operator=(const  MonomialCollection<quasiDiffMon, coefficient>& other)
  { this->MonomialCollection<quasiDiffMon, coefficient>::operator=(other);
  }
  void LieBracketMeOnTheRight(const MonomialCollection<quasiDiffMon, coefficient>& standsOnTheRight)
  { quasiDiffOp<coefficient> tempRight;
    tempRight=standsOnTheRight;
    MathRoutines::LieBracket(*this, tempRight, *this);
  }
};

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
void quasiDiffOp<coefficient>::GenerateBasisLieAlgebra
(List<quasiDiffOp<coefficient> >& theElts,
 FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
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
        report << "Lie bracketing elements "
        << " of indices " << i+1 << " and "
        << j+1 << " out of " << theEltsConverted.size << "<br> "
        << tempQDO.ToString(theFormat) << "<br> with element <br>"
        << theEltsConverted[j].ToString(theFormat)
        << " to get <br>";
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
  std::cout << "<hr>" << "<b>Num times Gaussian Elimination was called = "
  << numTimesEliminationWasExecuted
   << ".</b>";
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
  ElementWeylAlgebra leftElt, rightElt, tempElt;
  quasiDiffMon outputMon;
  output.MakeZero();
  for (int j=0; j<standsOnTheRight.size; j++)
  { rightElt.MakeZero();
    rightElt.AddMonomial(standsOnTheRight[j].theWeylMon, standsOnTheRight.theCoeffs[j]);
    for (int i=0; i<this->size; i++)
    { leftElt.MakeZero();
      leftElt.AddMonomial((*this)[i].theWeylMon, this->theCoeffs[i]);
      outputMon.theMatMon= (*this)[i].theMatMon;
      outputMon.theMatMon*=standsOnTheRight[j].theMatMon;
      leftElt*=rightElt;
      for (int k=0; k<leftElt.size; k++)
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
  MatrixTensor<ElementWeylAlgebra> reordered;
  reordered.MakeZero();
  ElementWeylAlgebra tempP;
  for (int i=0; i<this->size; i++)
  { const quasiDiffMon& currentMon=(*this)[i];
    tempP.MakeZero();
    tempP.AddMonomial(currentMon.theWeylMon, this->theCoeffs[i]);
    reordered.AddMonomial(currentMon.theMatMon, tempP);
  }
  std::string result=reordered.ToString(theFormat);
  if (result=="0" && this->size!=0)
  { std::cout << "This is likely a programming error (crashing at any rate): I have a non-zero  "
    << " quasidifferential operator "
    << " with non-properly formatted LaTeX string "
    << this->MonomialCollection<quasiDiffMon, coefficient>::ToString(theFormat)
    << ", however its properly formatted string is 0. "
    << "Probably there is something wrong with the initializations of the monomials "
    << "of the qdo. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return result;
}

template <class coefficient>
bool ModuleSSalgebra<coefficient>::GetActionEulerOperatorPart
(const MonomialP& theCoeff, ElementWeylAlgebra& outputDO, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName
  ("ModuleSSalgebra_CoefficientType::GetActionMonGenVermaModuleAsDiffOperator");
//  int varShift=this->GetMinNumVars();
//  std::cout << "<br>varShift for Euler operator: " << varShift;
  int powerMonCoeff=0;
  ElementWeylAlgebra currentMonContribution;
  outputDO.MakeOne();
//  std::cout << "<br>Getting Euler part contribution of " << theCoeff.ToString()
//  <<  " with min num vars equal to " << theCoeff.GetMinNumVars();
  for (int i=0; i<theCoeff.GetMinNumVars(); i++)
  { if (!theCoeff(i).IsSmallInteger(&powerMonCoeff))
    { std::cout << "This is a programming error. "
      << "Getting euler operator part of action on generalized Verma module: I have an "
      << "exponent with non-small integer entry. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
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
(ElementSemisimpleLieAlgebra<Rational>& inputElt, quasiDiffOp<Rational>& output,
  GlobalVariables& theGlobalVariables)
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
  theGenElt.Simplify(theGlobalVariables);
//  std::cout << "<br>the generic elt simplified:" << CGI::GetHtmlMathSpanPure(theGenElt.ToString());
//  std::cout << "<br>" << CGI::GetHtmlMathSpanPure(result.ToString() ) << " times " << CGI::GetHtmlMathSpanPure(theGenElt.ToString()) << " = ";
  result*=(theGenElt);
  result.Simplify(theGlobalVariables);
//  std::cout << " <br>" << CGI::GetHtmlMathSpanPure(result.ToString());
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.MakeIdSpecial();
  MatrixTensor<RationalFunctionOld> tempMat1;

  int varShift=this->GetMinNumVars();
//  std::cout  << "<br>Num elements nilrad: " << indicesNilrad.size;
  ElementWeylAlgebra weylPartSummand, exponentContribution, oneIndexContribution,
  eulerOperatorContribution;
  Polynomial<Rational> tempP1, negativeExponentDenominatorContribution, theCoeff;
  quasiDiffMon monQDO, monQDO2;
  //static int problemCounter=0;
  //problemCounter++;
  Rational tempRat;
  output.MakeZero();
  Rational currentShift;
  for (int i=0; i<result.size; i++)
  { //problemCounter++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon=result[i];
    endoPart=idMT;
    for (int j=currentMon.Powers.size-1; j>=indicesNilrad.size; j--)
    { int thePower=0;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower))
        return false;
      tempMat1=this->GetActionGeneratorIndeX(currentMon.generatorsIndices[j], theGlobalVariables);
      tempMT.MakeZero();
      for (int k=0; k<tempMat1.size; k++)
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
      ElementWeylAlgebra::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
      (currentShift, j+varShift, oneIndexContribution,
      negativeExponentDenominatorContribution, theGlobalVariables);
//      std::cout << "<br>result from GetStandardOrderDiffOperatorCorrespondingToNraisedTo: "
//      << negativeExponentDenominatorContribution.ToString() << " divided by "
//      << oneIndexContribution.ToString();
      exponentContribution*=oneIndexContribution;
      theCoeff.DivideBy(negativeExponentDenominatorContribution, theCoeff, tempP1);
      if (!tempP1.IsEqualToZero())
      { std::cout << "This is a mathematical error! Something is very wrong with embedding "
        << " semisimple Lie algebras in Weyl algebras. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    }
//    std::cout << "<br>Endo part of " << currentMon.ToString() << ": " << endoPart.ToString();
//    std::cout << "<br>Exponent contribution of " << currentMon.ToString() << ": "
//    << exponentContribution.ToString();
    for (int l=0; l<theCoeff.size; l++)
    { //problemCounter++;
      //if (problemCounter==249)
        //std::cout << "ere be problem!";
      if (!this->GetActionEulerOperatorPart
         (theCoeff[l], eulerOperatorContribution, theGlobalVariables))
        return false;
//      std::cout << "<br>Euler operator contribution: " << eulerOperatorContribution.ToString();
      weylPartSummand=exponentContribution;
      weylPartSummand*=eulerOperatorContribution;
      weylPartSummand*=theCoeff.theCoeffs[l];
      for (int j=0; j<weylPartSummand.size; j++)
        for (int k=0; k<endoPart.size; k++)
        { monQDO.theMatMon=endoPart[k];
          monQDO.theWeylMon=weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff=endoPart.theCoeffs[k];
          for (int m=0; m<currentEndoCoeff.size; m++)
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

bool CommandList::innerWriteGenVermaModAsDiffOperatorInner
(CommandList& theCommands, const Expression& input, Expression& output,
  Vectors<Polynomial<Rational> >& theHws, Expression& hwContext, Selection& selInducing,
  SemisimpleLieAlgebra* owner, std::string* xLetter, std::string* partialLetter,
  std::string* exponentVariableLetter)
{ MacroRegisterFunctionWithName("CommandList::innerWriteGenVermaModAsDiffOperatorInner");
   /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  if (theHws.size==0)
    return false;
  Expression tempExp;
  SemisimpleLieAlgebra& theSSalgebra=*owner;
  List<ElementUniversalEnveloping<Polynomial<Rational> > > elementsNegativeNilrad;
  ElementSemisimpleLieAlgebra<Rational> theGenerator;
  ElementUniversalEnveloping<Polynomial<Rational> > genericElt, actionOnGenericElt;
  List<ElementWeylAlgebra> actionNilrad;
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
  for (int j=0; j<theSSalgebra.GetRank(); j++)
  { Vector<Rational> ei;
    ei.MakeEi(theSSalgebra.GetRank(), j);
    theGenerator.MakeGGenerator(ei, theSSalgebra);
    theGeneratorsItry.AddOnTop(theGenerator);
//    theGenerator.MakeHgenerator
//    (ei, theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra);
//    theGeneratorsItry.AddOnTop(theGenerator);
    ei.Minus();
    theGenerator.MakeGGenerator(ei, theSSalgebra);
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
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.ToString()
  << "} Differential operators corresponding to actions of simple positive generators for the "
  << selInducing.ToString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFunctionOld > > theMods;
  theMods.SetSize(theHws.size);
  Vector<RationalFunctionOld> tempV;
  int numStartingVars=hwContext.ContextGetNumContextVariables();
  //std::cout << "<br>num starting vars:" << numStartingVars;
  for (int i=0; i<theHws.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[i];
    tempV=theHws[i];
    if (!theMod.MakeFromHW
        (theSSalgebra, tempV, selInducing, *theCommands.theGlobalVariableS, 1, 0, 0, true))
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
      theWeylFormat.polyAlphabeT.SetSize
      (numStartingVars+ elementsNegativeNilrad.size);
      theWeylFormat.weylAlgebraLetters.SetSize
      (numStartingVars+ elementsNegativeNilrad.size);
      theUEformat.polyAlphabeT.SetSize
      (numStartingVars+ elementsNegativeNilrad.size);
      for (int k=0; k<numStartingVars; k++)
        theWeylFormat.weylAlgebraLetters[k]="error";
      //std::cout << "<br>HW num context vars: " << hwContext.ContextGetNumContextVariables();
      std::string theFinalXletter= (xLetter==0) ? "x": *xLetter;
      std::string theFinalPartialLetter=(partialLetter==0) ? "\\partial" : *partialLetter;
      for (int k=numStartingVars; k<theUEformat.polyAlphabeT.size; k++)
      { std::stringstream tmpStream, tempstream2, tempstream3, tempStream4;
        tmpStream << theUEformat.polyDefaultLetter << "_{"
        << k-hwContext.ContextGetNumContextVariables()+1 << "}";
        theUEformat.polyAlphabeT[k] = tmpStream.str();
        tempstream2 << theFinalXletter << "_{" << k-numStartingVars+1 << "}";
        tempstream3 << theFinalXletter << "_" << k-numStartingVars+1;
        tempStream4 << theFinalPartialLetter << "_{" << k-numStartingVars+1 << "}";
        if (theWeylFormat.polyAlphabeT.Contains(tempstream2.str()) ||
            theWeylFormat.polyAlphabeT.Contains(tempstream3.str()))
          return output.SetError
          ("Error: the variable "+ tempstream2.str()+
           " is reserved for me: you are not allowed to use it as a coordinate of the highest weight. ",
           theCommands);
        theWeylFormat.polyAlphabeT[k]=tempstream2.str();
        theWeylFormat.weylAlgebraLetters[k]=tempStream4.str();
      }
//      std::cout << "<br>theUEformat: ";
//      for (int k=0; k<theUEformat.polyAlphabeT.size; k++)
//        std::cout << theUEformat.polyAlphabeT[k] << ", ";
      out << "<tr><td>General monomial in U(n_-):</td><td>"
      << CGI::GetHtmlMathSpanPure(genericElt.ToString(&theUEformat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << theGeneratorsItry.size
      << "}{c}{Element acting}\\\\<br>\n ";
      latexReport << "Action on ";
      out << "<tr><td></td><td colspan=\"" << theGeneratorsItry.size
      << "\"> Element acting</td></td></tr>";
      out << "<tr><td>Action on</td>";
      for (int j=0; j<theGeneratorsItry.size; j++)
      { out << "<td>" << theGeneratorsItry[j].ToString(&theUEformat) << "</td>";
        latexReport << "& $" << theGeneratorsItry[j].ToString(&theUEformat)  << "$";
      }
      latexReport << "\\endhead \\hline<br>";
      out << "</tr>";
      out << "<tr><td>" << CGI::GetHtmlMathSpanPure(genericElt.ToString(&theUEformat)) << "</td>";
      latexReport << "$" << genericElt.ToString(&theUEformat) << "$";
      for (int j=0; j<theGeneratorsItry.size; j++)
      { actionOnGenericElt.AssignElementLieAlgebra(theGeneratorsItry[j], theSSalgebra, Pone, Pzero)
        ;
        actionOnGenericElt*=(genericElt);
        theSSalgebra.OrderSetNilradicalNegativeMost(theMod.parabolicSelectionNonSelectedAreElementsLevi);
        actionOnGenericElt.Simplify(*theCommands.theGlobalVariableS);
        theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
        out << "<td>" << CGI::GetHtmlMathSpanPure
        ("\\begin{array}{rcl}&&" +actionOnGenericElt.ToString(&theUEformat)+"\\end{array}")
        << "</td>";
        theUEformat.NumAmpersandsPerNewLineForLaTeX=0;
        latexReport << "& $\\begin{array}{l} " << actionOnGenericElt.ToString(&theUEformat)
        << "\\end{array}$ ";
      }
      latexReport << "\\\\ \\hline\\hline<br>";
      out << "</tr>";
    }
    out << "<tr><td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(theMod.theChaR.ToString())
    << "</td>";
    latexReport2 << "\\begin{longtable}{rll}";
    latexReport2 << "$\\gog$& $n$& element of $\\mathbb W_n$ \\\\\\hline"
    << "\\multirow{"
    << theGeneratorsItry.size
    << "}{*}{$"
    << theSSalgebra.GetLieAlgebraName() << "$}"
    << " &  \\multirow{"  << theGeneratorsItry.size << "}{*}{"
    << elementsNegativeNilrad.size << "}&";

    latexReport << "$\\begin{array}{r}" << theMod.theChaR.ToString()
    << "(\\mathfrak{l}) \\\\ \\\\dim:~" << theMod.GetDim() << " \\end{array}$";
    //std::cout << "<hr>hw: " << theMod.theHWFundamentalCoordsBaseField.ToString() << " nilrad elts: " << elementsNegativeNilrad.ToString();
    //std::cout << "<br>generic element: " << genericElt.ToString();
    for (int j=0; j<theGeneratorsItry.size; j++)
    { theGenerator=theGeneratorsItry[j];
      theMod.GetActionGenVermaModuleAsDiffOperator
      (theGenerator, theQDOs[j], *theCommands.theGlobalVariableS)
      ;
      theWeylFormat.CustomCoeffMonSeparator="\\otimes ";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
      out << "<td>" << CGI::GetHtmlMathSpanPure
      ("\\begin{array}{|r|c|l|}&&"+theQDOs[j].ToString(&theWeylFormat)+"\\end{array}")
      << "</td>";
      theWeylFormat.NumAmpersandsPerNewLineForLaTeX=0;
      theWeylFormat.MaxLineLength=300;
      latexReport << " & $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat)
      << "\\end{array}$";
      if (j!=0)
        latexReport2 << "&&";
      latexReport2 << " $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat)
      << "\\end{array}$\\\\ "
      << (j!=theGeneratorsItry.size-1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      theWeylFormat.CustomCoeffMonSeparator="";
    }
    //theQDOs[0].GenerateBasisLieAlgebra(theQDOs, &theWeylFormat, theCommands.theGlobalVariableS);
    //std::cout << "<br><b>Dimension generated Lie algebra: " << theQDOs.size << "</b>";
    //std::cout << "<br>The qdos: ";
    //for (int j=0; j<theQDOs.size; j++)
    //  std::cout << "<br>" << theQDOs[j].ToString();
    latexReport2 << "\\end{longtable}";
    latexReport << "\\\\\\hline<br>";
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>";
  out << "<br>" << latexReport.str();
  out << "<br><br>" << latexReport2.str();
  return output.AssignValue<std::string>(out.str(), theCommands);
}

template <class coefficient>
std::string ModuleSSalgebra<coefficient>::ToString(FormatExpressions* theFormat)const
{ if (this->owneR==0)
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
  out << "<br>Finite dimensional part h. w. fundamental coordinates:"
  << this->theHWFDpartFundamentalCoordS.ToString();
  out << "<br>Finite dimensinoal part h. w. simple coords: "
  << this->theHWFDpartSimpleCoordS.ToString();
  out << "<br>Inducing module character (over the Cartan subalgebra): ";
  FormatExpressions latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
  if (this->theCharOverH.size<100)
    out << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(this->theCharOverH.ToString(&latexFormat));
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
      tempWelt.SetSize(currentListInt[j].generatorsIndices.size);
      for (int k=0; k<currentListInt[j].generatorsIndices.size; k++)
        tempWelt[k]=theWeyl.RootsOfBorel.size-1 -currentListInt[j].generatorsIndices[k];
      out << "<tr><td>m_{ " << wordCounter << "} </td><td>"
      << currentList[j].ToString(&theGlobalVariables.theDefaultFormat) << "  v_\\lambda</td><td>"
      << tempWelt.ToString() << "</td> </tr>";
    }
  }
  out << "</table>";

  out << "<br>Character: " << this->theChaR.ToString();
  out << "<br>Computed generator actions (" << this->ComputedGeneratorActions.CardinalitySelection
  << " out of "
  << this->actionsGeneratorsMaT.size << " computed actions) follow. "
  << " Note that generator actions are computed on demand, only the simple "
  << " Chevalley generators are computed by default. ";
  out << "<table><tr><td>Generator </td><td>Action</td></tr>";
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  for (int i=0; i<this->actionsGeneratorsMaT.size; i++)
    if (this->ComputedGeneratorActions.selected[i])
    { tempSSElt.MakeGenerator(i, theAlgebrA);
      out << "<tr>";
      out << "<td>" << CGI::GetHtmlMathSpanPure(tempSSElt.ToString(theFormat)) << "</td>";
      out << "<td>";
      if (this->GetDim()<28)
      { Matrix<coefficient> outputMat;
        this->actionsGeneratorsMaT[i].GetMatrix(outputMat, this->GetDim());
        //std::cout << outputMat.ToString(&latexFormat);
        out << CGI::GetHtmlMathSpanPure(outputMat.ToString(&latexFormat), 5000) << " = ";
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
  out << " A picture of the weight support follows. "
  << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());

  bool isBad=false;
  for (int k=0; k<this->theBilinearFormsAtEachWeightLevel.size; k++)
  { Matrix<coefficient>& theBF=this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<coefficient>& theBFinverted= this->theBilinearFormsInverted[k];
    out << "<hr>weight in simple coords: "
    << this->theModuleWeightsSimpleCoords[k].ToString();
    List<MonomialUniversalEnveloping<coefficient> >& currentList=this->theGeneratingWordsGrouppedByWeight[k];
    for (int i=0; i<currentList.size; i++)
    { MonomialUniversalEnveloping<coefficient>& currentElt=currentList[i];
      out << "<br>monomial " << i+1 << ": "
      << currentElt.ToString(&theGlobalVariables.theDefaultFormat);
    }
    out << "; Matrix of Shapovalov form associated to current weight level: <br> "
    << theBF.ToString(&theGlobalVariables.theDefaultFormat);
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
  out << "<br>Cached Shapovalov products before generator action computation: "
  << this->cachedPairs.size << ". Dimension : " << this->GetDim();
  ElementSemisimpleLieAlgebra<Rational> tempSSelt;
  out << "<br>Cached Shapovalov products final: "
  << this->cachedPairs.size << "; dimension : " << this->GetDim();
  return out.str();
}

bool CommandList::innerHWVCommon
(CommandList& theCommands, Expression& output,
 Vector<RationalFunctionOld>& highestWeightFundCoords,
 Selection& selectionParSel, Expression& hwContext, SemisimpleLieAlgebra* owner, bool Verbose)
{ MacroRegisterFunctionWithName("CommandList::innerHWVCommon");
  RecursionDepthCounter therecursionIncrementer(&theCommands.RecursionDeptH);
  //  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  ListReferences<ModuleSSalgebra<RationalFunctionOld> >& theMods=
  theCommands.theObjectContainer.theCategoryOmodules;
  int indexOfModule=-1;

  for (int i=0; i<theMods.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& currentMod=theMods[i];
    if (highestWeightFundCoords==currentMod.theHWFundamentalCoordsBaseField &&
        selectionParSel==currentMod.parabolicSelectionNonSelectedAreElementsLevi )
    { indexOfModule=i;
      break;
    }
  }
  if (indexOfModule==-1)
  { indexOfModule=theMods.size;
    theMods.SetSize(theMods.size+1);
  }
  ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[indexOfModule];
  if (!theMod.flagIsInitialized)
  { bool isGood=theMod.MakeFromHW
    (*owner, highestWeightFundCoords, selectionParSel,
     *theCommands.theGlobalVariableS, RFOne, RFZero, &report);
    if (Verbose)
      theCommands.Comments << theMod.ToString();
    if (!isGood)
      return output.SetError
      ("Error while generating highest weight module. See comments for details. ", theCommands);
  }
  theElt.MakeHWV(theMod, RFOne);
  return output.AssignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunctionOld> >
  (theElt, hwContext, theCommands);
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

bool CommandList::fSplitGenericGenVermaTensorFD
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError
    ("Function fSplitGenericGenVermaTensorFD is expected to have three arguments: \
     SS algebra type, weight, weight. ", theCommands);
  const Expression& leftE=input[1];
  const Expression& genVemaWeightNode=input[3];
  const Expression& fdWeightNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, leftE, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<RationalFunctionOld> highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVectoR<RationalFunctionOld>
      (genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank,
       CommandList::innerRationalFunction))
  { theCommands.Comments
    << "Failed to convert the third argument of fSplitGenericGenVermaTensorFD to a list of "
    << theRank << " polynomials. The second argument you gave is "
    << genVemaWeightNode.ToString() << ".";
    return false;
  }
  Vector<Rational> theFDhw;
  if (!theCommands.GetVectoR<Rational>(fdWeightNode, theFDhw, &emptyContext, theRank, 0))
  { theCommands.Comments
    << "Failed to convert the second argument of fSplitGenericGenVermaTensorFD to a list of "
    << theRank << " rationals. The second argument you gave is "
    << fdWeightNode.ToString() << ".";
    return false;
  }
//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  int theNumVars=hwContext.GetNumContextVariables();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
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
      return output.SetError
      ("Error: the third argument of fSplitGenericGenVermaTensorFD must be a \
       list of small non-negative integers.", theCommands);
  }
  theCommands.innerHWVCommon
  (theCommands, hwvGenVerma, highestWeightFundCoords, selParSel1, hwContext, theSSalgebra);
  if (hwvGenVerma.IsError())
  { output=hwvGenVerma;
    return true;
  }
  Vector<RationalFunctionOld> theFDhwRF;
  theFDhwRF=theFDhw;
  theCommands.innerHWVCommon(theCommands, hwvFD, theFDhwRF, selFD, hwContext, theSSalgebra);
  if (hwvFD.IsError())
  { output=hwvFD;
    return true;
  }
  std::stringstream out;
  out << "hwv par sel: " << hwvGenVerma.ToString();
  out << "hwv fd: " << hwvFD.ToString();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWgenVerma=
  hwvGenVerma.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWfd=
  hwvFD.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();

  ModuleSSalgebra<RationalFunctionOld>& theGenMod=theHWgenVerma[0].theMons[0].GetOwner();
  //int indexGenMod=theHWgenVerma[0].theMons[0].indexInOwner;
  ModuleSSalgebra<RationalFunctionOld>& theFDMod=theHWfd[0].theMons[0].GetOwner();
  //int indexFDMod=theHWfd[0].theMons[0].indexInOwner;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir, theCasimirMinusChar;
  charSSAlgMod<RationalFunctionOld> theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.MakeFromWeight(theFDMod.theHWSimpleCoordSBaseField, theSSalgebra);
  ReflectionSubgroupWeylGroup tempSG;
  List<ElementUniversalEnveloping<RationalFunctionOld> > theLeviEigenVectors;
  Vectors<RationalFunctionOld> theEigenVectorWeightsFund;
  theFDMod.SplitOverLevi
  (&report, theGenMod.parabolicSelectionNonSelectedAreElementsLevi, *theCommands.theGlobalVariableS, RFOne, RFZero,
   &theLeviEigenVectors, &theEigenVectorWeightsFund, 0, &theFDLeviSplit)
  ;
  theFDMod.GetFDchar(theFDChaR);
  List<ElementUniversalEnveloping<RationalFunctionOld> > theCentralCharacters;
  theCasimir.MakeCasimir(*theSSalgebra, *theCommands.theGlobalVariableS, RFOne, RFZero);
  Vector<RationalFunctionOld> currentHWsimplecoords, currentHWdualcoords, currentWeightDiff;
  FormatExpressions tempFormat;
  tempFormat.MaxLineLength=60;
  tempFormat.flagUseLatex=true;
  tempFormat.fundamentalWeightLetter="\\psi";
  tempFormat.CustomPlusSign="\\oplus ";
  hwContext.ContextGetFormatExpressions(tempFormat);
  out << "<br>Character of finite dimensional module:" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(theFDChaR.ToString());
  if (theGenMod.parabolicSelectionSelectedAreElementsLevi.CardinalitySelection>0)
    out << "<br>theFDChar split over levi:" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(theFDLeviSplit.ToString(&tempFormat));
  //out << "<br> Splitting report: " << report;
  std::stringstream latexReport1;
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Character</td></tr>";
  latexReport1 << " \\begin{longtable}{rl}\\caption{\\label{table"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "GenVermatimes7DimCentralCharacters"
  << "} $" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
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
    out << "<tr><td>" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi")
    << "</td><td>" << currentChar.ToString(&tempFormat) << "</td></tr>";
    latexReport1 << " $"
    << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i+1 <<"}:=$ $" << currentChar.ToString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFunctionOld> tempElt, tempElt2;
  theFDMod.highestWeightVectorNotation="v";
  theGenMod.highestWeightVectorNotation="w";
  out << "Let w be the highest weight vector of the generalized Verma component, "
  << "and let v be the highest weight vector of the finite dimensional component";
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Algebraic expression</td>"
  << "<td>Additional multiplier</td>";
  if (theNumVars==1)
    out << "<td>gcd polynomial coeffs</td>";
  out << "<td>the hwv</td>";
  if (theNumVars==1)
    out << "<td>gcd divided out</td>";
  out << "</tr>";
  std::stringstream latexReport2;
  latexReport2 << "\\begin{longtable}{p{2.5cm}p{2.5cm}p{1.5cm}l}\\caption{"
  << "\\label{tableDecompo" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString() << "times7dim}"
  << "Decomposition for the $"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.ToString()
  << "$-parabolic subalgebra $\\bar{\\mathfrak{p}}$ } \\\\ Weight & Projector applied to &"
  << " Extra multiplier & Resulting $\\bar {\\mathfrak b}$-singular vector \\endhead\\hline";
  //std::cout << theGenMod.theGeneratingWordsNonReduced.ToString();
  for (int i=0; i<theCentralCharacters.size; i++)
  { Vector<RationalFunctionOld> currentWeightSimpleCoords=
    theSSalgebra->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[i]);
    tempElt.MakeHWV(theFDMod, RFOne);
    tempElt.MultiplyOnTheLeft
    (theLeviEigenVectors[i], theElt, *theSSalgebra, *theCommands.theGlobalVariableS,
     RFOne, RFZero);
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
        theElt.MultiplyOnTheLeft
        (theCasimirMinusChar, tempElt2, *theSSalgebra, *theCommands.theGlobalVariableS,
        RFOne, RFZero);
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
    << "</td><td>" << CGI::GetHtmlMathSpanPure(tempStream.str())
    << "</td><td>" << tempRat.ToString() << "</td>";
    latexReport2
    << "$" << theSSalgebra->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "$ &  " << tempStream2.str() << " &" << tempRat.ToString();
    Polynomial<Rational> tmpGCD, tmpRF;
    tempFormat.MaxLineLength=80;
    if (theNumVars==1)
    { tmpGCD= theElt.FindGCDCoefficientNumerators<Polynomial<Rational> >();
      tmpGCD.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
      out << "<td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(tmpGCD.ToString(&tempFormat)) << "</td>";
    }
    out << "<td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(theElt.ToString(&tempFormat)) << "</td>";
    latexReport2 << "&$\\begin{array}{l}" << theElt.ToString(&tempFormat) << "\\end{array}$\\\\<br>";
    if (theNumVars==1)
    { tmpRF=tmpGCD;
      theElt/=tmpRF;
      out << "<td>" << CGI::GetHtmlMathSpanPure("\\begin{array}{l}"+theElt.ToString(&tempFormat)+"\\end{array}")
      << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  latexReport2 << "\\end{longtable}";
  out << "<br>Ready LaTeX (table 1 and 2): <br><br><br>" << latexReport1.str();
  out << "<br><br><br>" << latexReport2.str() << "<br>";
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool CommandList::innerFunctionToMatrix
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerFunctionToMatrix");
  std::cout << input.ToString();
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
  { theCommands.Comments
    << "Max number of rows/columns is 1000. You requested " << numRows << " rows and "
    << numCols << " columns.<br>";
    return false;
  }
  output.reset(theCommands, numRows+1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.format=output.formatMatrix;
  Expression theIndices, currentRow, currentE;
  for (int i=0; i<numRows; i++)
  { currentRow.reset(theCommands, numCols+1);
    currentRow.AddChildAtomOnTop(theCommands.opSequence());
    currentRow.format=currentRow.formatMatrixRow;
    for (int j=0; j<numCols; j++)
    { theIndices.reset(theCommands, 3);
      theIndices.AddChildAtomOnTop(theCommands.opSequence());
      theIndices.AddChildValueOnTop((Rational)(i+1));
      theIndices.AddChildValueOnTop((Rational)(j+1));
      currentE.MakeFunction(theCommands, leftE, theIndices);
      currentRow.AddChildOnTop(currentE);
    }
    output.AddChildOnTop(currentRow);
  }
  return true;
}

bool CommandList::innerGetChevGen
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], theSSalg, &errorString))
    output.SetError(errorString, theCommands);
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
  theContext.ContextMakeContextSSLieAlgebrA(indexInOwner, theCommands);
  return output.AssignValueWithContext(theUE, theContext, theCommands);
}

bool CommandList::innerGetCartanGen
(CommandList& theCommands, const Expression& input, Expression& output)
{ //std::cout << "<br>Here I am with input: " << input.ToString();
  if (!input.IsListNElements(3))
    return false;
  SemisimpleLieAlgebra* theSSalg;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], theSSalg, &errorString))
    output.SetError(errorString, theCommands);
  //std::cout << "<br>Here I am at next phase: " << input.ToString();
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
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg);
  Expression theContext;
  int theAlgIndex=theCommands.theObjectContainer.theLieAlgebras.GetIndex(*theSSalg);
  theContext.ContextMakeContextSSLieAlgebrA(theAlgIndex, theCommands);
  //std::cout << "<br>And the context is: " << theContext.ToString();
  return output.AssignValueWithContext(theUE, theContext, theCommands);
}

bool CommandList::fKLcoeffs
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  std::string errorString;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  std::stringstream out;
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (theWeyl.theDynkinType.GetSizeWeylGroupByFormula()>192)
  { out << "I have been instructed to run only for Weyl groups that have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra->GetLieAlgebraName() << " has "
    << theWeyl.theDynkinType.GetSizeWeylGroupByFormula() << ".";
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(1);
  theFormat.polyAlphabeT[0]="q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, "
  << "Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << theSSalgebra->GetLieAlgebraName();
  KLpolys theKLpolys;
  theKLpolys.ComputeKLPolys(&theWeyl);
  theFormat.flagUseHTML=true;
  out << theKLpolys.ToString(&theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerPrintSSLieAlgebra
(CommandList& theCommands, const Expression& input, Expression& output, bool Verbose)
{ MacroRegisterFunctionWithName("CommandList::innerPrintSSLieAlgebra");
  std::string errorString;
  SemisimpleLieAlgebra *tempSSpointer;
  input.CheckInitialization();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input, tempSSpointer, &errorString))
    return output.SetError(errorString, theCommands);
  SemisimpleLieAlgebra& theSSalgebra=*tempSSpointer;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  FormatExpressions theFormat, latexFormat;
  latexFormat.flagUseLatex=true;
  latexFormat.flagUseHTML=false;
//      theFormat.chevalleyHgeneratorLetter="\\bar{h}";
//      theFormat.chevalleyGgeneratorLetter="\\bar{g}";
  out << "<hr>Lie algebra type: " << theWeyl.theDynkinType << ". ";
  out << "<br>Weyl group size: " << theWeyl.theDynkinType.GetSizeWeylGroupByFormula() << "."
  << "<br>To get extra details: ";
  std::stringstream tempStream;
  tempStream << "printSemisimpleLieAlgebra{}(" << theWeyl.theDynkinType << ")";
  out << theCommands.GetCalculatorLink(tempStream.str()) << "<br>";
  if (Verbose)
  { out
    << " The resulting Lie bracket pairing table follows. <hr> "
    << theSSalgebra.ToString(&theCommands.theGlobalVariableS->theDefaultFormat);
    out << "Ready for LaTeX consumption version of the first three columns: ";
    out << "<br>%Add to preamble: <br>\\usepackage{longtable} <br>%Add to body: <br>"
    << " \\begin{longtable}{ccc}generator & root simple coord. & root $\\varepsilon$-notation \\\\\\hline<br>\n";
    Vector<Rational> tempRoot, tempRoot2;
    ElementSemisimpleLieAlgebra<Rational> tempElt1;
    for (int i=0; i<theSSalgebra.GetNumGenerators(); i++)
    { tempElt1.MakeGenerator(i, theSSalgebra);
      tempRoot=theSSalgebra.GetWeightOfGenerator(i);
      theSSalgebra.theWeyl.GetEpsilonCoords(tempRoot, tempRoot2);
      out << "$" << tempElt1.ToString(&theFormat) << "$&$"<< tempRoot.ToString() << "$";
      out << "&$" << tempRoot2.ToStringLetterFormat("\\varepsilon") << "$";
      out << "\\\\\n";
    }
    out << "\\end{longtable}" << "<hr>";
  }
  out << "We define the symmetric Cartan matrix by requesting that the entry in "
  << "the i-th row and j-th column "
  << " be the scalar product of the i^th and j^th roots. "
  << "Symmetric Cartan matrix:<br>"
  << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(theWeyl.CartanSymmetric.ToString(&latexFormat) );
  Rational tempRat;
  Matrix<Rational> tempMat;
  tempMat = theWeyl.CartanSymmetric;
  tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
  out << "<br>The determinant of the symmetric Cartan matrix is: " << tempRat.ToString();
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
  out << "= " << CGI::GetHtmlMathSpanPure(tempRoot.ToStringLetterFormat("\\varepsilon"));
  out
  << "<hr>The fundamental weights (the j^th fundamental weight has scalar product 1 <br> "
  << " with the j^th simple root times 2 divided by the root length squared,<br> "
  << " and 0 with the remaining simple roots): ";
  theWeyl.GetEpsilonCoords(fundamentalWeights, fundamentalWeightsEpsForm);
  out << "<table>";
  for (int i=0; i< fundamentalWeights.size; i++)
  { out << "<tr><td>" << fundamentalWeights[i].ToString() << "</td><td> =</td><td> "
    << CGI::GetHtmlMathSpanPure(fundamentalWeightsEpsForm[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  out << "<hr>Simple basis in epsilon coordinates: <table>";
  simpleBasis.MakeEiBasis(theWeyl.GetDim());
  theWeyl.GetEpsilonCoords(simpleBasis, simplebasisEpsCoords);
  for (int i=0; i< simplebasisEpsCoords.size; i++)
  { out << "<tr><td>"
    << simpleBasis[i].ToString() << " </td><td>=</td> <td>"
    << CGI::GetHtmlMathSpanPure(simplebasisEpsCoords[i].ToStringEpsilonFormat())
    << "</td></tr>";
  }
  out << "</table>";
  DynkinSimpleType tempSimpleType;
  if (theWeyl.theDynkinType.IsSimple
      (&tempSimpleType.theLetter, &tempSimpleType.theRank, &tempSimpleType.lengthFirstCoRootSquared))
    if (tempSimpleType.lengthFirstCoRootSquared==2)
    { //std::cout << "here i am";
      Matrix<Rational> tempM, tempM2;
      theWeyl.theDynkinType.GetEpsilonMatrix(tempM);
      tempM2=tempM;
      tempM2.Transpose();
      tempM2.MultiplyOnTheRight(tempM);
      if (!(tempM2==theWeyl.CartanSymmetric))
      { std::cout << "This is a (non-critical) programming error: the epsilon coordinates of the "
        << "vectors are incorrect. Please fix function DynkinType::GetEpsilonMatrix. "
        << "The matrix of the epsilon coordinates is "
        << tempM.ToString()
        << ", the Symmetric Cartan matrix is "
        << theWeyl.CartanSymmetric.ToString() << ", and the  "
        << "transpose of the epsilon matrix times the epsilon matrix:  "
        << tempM2.ToString() << ". "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
    }
  if (Verbose)
  { out << "<hr>Root system:<table><tr><td>Simple basis coordinates</td><td></td>"
    << "<td>Epsilon coordinates non-LaTeX'ed (convention: see above)</td></tr> ";
    Vectors<Rational> rootSystemEpsCoords;
    theWeyl.GetEpsilonCoords(theWeyl.RootSystem, rootSystemEpsCoords);
    for (int i=0; i<theWeyl.RootSystem.size; i++)
    { const Vector<Rational>& current=theWeyl.RootSystem[i];
      out << "<tr><td>" << current.ToString() << "</td><td>=</td><td>"
      << rootSystemEpsCoords[i].ToStringLetterFormat("e")
      << "</td></tr>";
    }
    out << "</table>";
    DrawingVariables theDV;
    theWeyl.DrawRootSystem(theDV, true, *theCommands.theGlobalVariableS, true, 0, true, 0);
    out << "<hr>Below a drawing of the root system in its corresponding Coxeter plane "
    << "(computed as explained on John Stembridge's website). "
    << "<br>The darker red dots can be dragged with the mouse to rotate the picture."
    << "<br>The grey lines are the edges of the Weyl chamber."
    << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  }
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool Expression::HasBoundVariables()const
{ if (this->theBoss==0)
  { std::cout << "This is a programming error: calling function HasBoundVariables"
    << " on non-initialized expression. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  RecursionDepthCounter recursionCounter(&this->theBoss->RecursionDeptH);
  MacroRegisterFunctionWithName("Expression::HasBoundVariables");
  if (this->theBoss->RecursionDeptH>this->theBoss->MaxRecursionDeptH)
  { std::cout << "This is a programming error: function HasBoundVariables has "
    << "exceeded recursion depth limit. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (this->IsListOfTwoAtomsStartingWith(this->theBoss->opBind()))
    return true;
  for (int i=0; i<this->children.size; i++)
    if ((*this)[i].HasBoundVariables())
      return true;
  return false;
}

bool CommandList::innerNot
(CommandList& theCommands, const Expression& input, Expression& output)
{ int theInt;
  if (!input.IsSmallInteger(&theInt))
    return false;
  if (theInt==0)
    return output.AssignValue(1, theCommands);
  return output.AssignValue(0, theCommands);
}

bool CommandList::innerIsInteger
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (input.HasBoundVariables())
    return false;
  if (input.IsInteger())
    output.AssignValue(1, theCommands);
  else
    output.AssignValue(0, theCommands);
  return true;
}

bool CommandList::innerIsRational
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (input.HasBoundVariables())
    return false;
  if (input.IsOfType<Rational>())
    output.AssignValue(1, theCommands);
  else
    output.AssignValue(0, theCommands);
  return true;
}

bool CommandList::AppendOpandsReturnTrueIfOrderNonCanonical
(const Expression& input, List<Expression>& output, int theOp)
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
      if (i<input.children.size-1 &&
          input[i].IsListStartingWithAtom(theOp) &&
          input[i].children.size>2)
        result=true;
    }
  return result;
}

void CommandList::init(GlobalVariables& inputGlobalVariables)
{ //std::cout << "<br>Num lists created before command list init: " << NumListsCreated;
  this->theGlobalVariableS=& inputGlobalVariables;
//  this->MaxAlgTransformationsPerExpression=100000;
  this->formatVisibleStrings.flagExpressionIsFinal=true;
  this->MaxAlgTransformationsPerExpression=100;
  this->MaxRecursionDeptH=10000;
  this->RecursionDeptH=0;
  this->NumErrors=0;
  this->DepthRecursionReached=0;
  this->flagLogSyntaxRules=false;
  this->flagLogEvaluatioN=false;
  this->flagLogPatternMatching=false;
  this->flagLogFullTreeCrunching=false;
  this->flagNewContextNeeded=true;
  this->flagProduceLatexLink=false;
  this->flagDisplayFullExpressionTree=false;
  this->MaxLatexChars=2000;
  this->numEmptyTokensStart=9;
  this->MaxNumCachedExpressionPerContext=100000;

  this->theExpressionContainer.Clear();
//  this->theExpressionContainer.SetExpectedSize(1000);


  this->controlSequences.Clear();
  this->operations.Clear();
  this->builtInTypes.Clear();
  this->FunctionHandlers.SetSize(0);
  this->operations.SetExpectedSize(300);
  this->FunctionHandlers.SetExpectedSize(300);
  this->builtInTypes.SetHashSizE(30);

  this->syntacticSouP.SetSize(0);
  this->syntacticStacK.SetSize(0);
  this->flagTimeLimitErrorDetected=false;
  this->flagFirstErrorEncountered=false;
  this->flagMaxTransformationsErrorEncountered=false;
  this->flagMaxRecursionErrorEncountered=false;
  this->flagAbortComputationASAP=false;
  this->ExpressionStack.Clear();

  this->theCruncherIds.Clear();
  this->theCruncherS.SetSize(0);
  this->syntaxErrors="";
  this->evaluationErrors.SetSize(0);
  this->CurrentSyntacticStacK=&this->syntacticStacK;
  this->CurrrentSyntacticSouP=&this->syntacticSouP;
  //operation List is the very first operation. It signifies a non-atomic expression.
  //operation List is signified by the empty string
  //operation List must always have index 0.
  this->AddOperationNoRepetitionAllowed("");

  this->AddOperationNoRepetitionAllowed(":=");
  this->AddOperationNoRepetitionAllowed(";");
  this->AddOperationNoRepetitionAllowed("if:=");
  this->AddOperationNoRepetitionAllowed("{}");
  this->AddOperationNoRepetitionAllowed("+");
  this->AddOperationNoRepetitionAllowed("-");
  this->AddOperationNoRepetitionAllowed("/");
  this->AddOperationNoRepetitionAllowed("*");
  this->AddOperationNoRepetitionAllowed("\\otimes");
  this->AddOperationNoRepetitionAllowed("[]");
  this->AddOperationNoRepetitionAllowed(":=:");
  this->AddOperationNoRepetitionAllowed("^");
  this->AddOperationNoRepetitionAllowed(">");
  this->AddOperationNoRepetitionAllowed("==");
  this->AddOperationNoRepetitionAllowed("\\cup");
  this->AddOperationNoRepetitionAllowed("\\sqcup");
  this->AddOperationNoRepetitionAllowed("Error");
  this->AddOperationNoRepetitionAllowed("Sequence");

  this->AddOperationBuiltInType("Rational");
  this->AddOperationBuiltInType("Double");
  this->AddOperationBuiltInType("AlgebraicNumber");
  this->AddOperationBuiltInType("PolynomialRational");
  this->AddOperationBuiltInType("RationalFunction");
  this->AddOperationBuiltInType("string");
  this->AddOperationBuiltInType("ElementUEoverRF");
  this->AddOperationBuiltInType("ElementTensorGVM");
  this->AddOperationBuiltInType("CharSSAlgMod");
  this->AddOperationBuiltInType("SemisimpleLieAlg");
  this->AddOperationBuiltInType("LittelmannPath");
  this->AddOperationBuiltInType("LRO");
  this->AddOperationBuiltInType("Matrix_Rational");
  this->AddOperationBuiltInType("Matrix_RF");
  this->AddOperationBuiltInType("CalculusPlot");
  this->AddOperationBuiltInType("SemisimpleSubalgebras");
  this->AddOperationBuiltInType("CandidateSSsubalgebra");
  this->AddOperationBuiltInType("WeylGroup");
  this->AddOperationBuiltInType("ElementWeylGroup");
  this->AddOperationBuiltInType("WeylGroupRep");
  this->AddOperationBuiltInType("WeylGroupVirtualRep");

  this->AddOperationNoRepetitionAllowed("PolyVars");
  this->AddOperationNoRepetitionAllowed("Context");
  this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTop("{{}}");
  this->controlSequences.AddOnTop("Variable");
  this->controlSequences.AddOnTop(this->operations);//all operations defined up to this point are also control sequences
  this->controlSequences.AddOnTop("Expression");
  this->controlSequences.AddOnTop("Integer");
  this->controlSequences.AddOnTop(",");
  this->controlSequences.AddOnTop(".");
  this->controlSequences.AddOnTop("if");
  this->controlSequences.AddOnTop("\\cdot");
  this->controlSequences.AddOnTop("_");
  this->controlSequences.AddOnTop("(");
  this->controlSequences.AddOnTop(")");
  this->controlSequences.AddOnTop("[");
  this->controlSequences.AddOnTop("]");
  this->controlSequences.AddOnTop("{");
  this->controlSequences.AddOnTop("}");
  this->controlSequences.AddOnTop(":");
  this->controlSequences.AddOnTop("SequenceStatements");
  this->controlSequences.AddOnTop("MakeSequence");
  this->controlSequences.AddOnTop("SequenceMatrixRows");
  this->controlSequences.AddOnTop("MatrixRow");
  this->controlSequences.AddOnTop("=");
  this->controlSequences.AddOnTop("$");
  this->controlSequences.AddOnTop("MatrixSeparator");
  this->controlSequences.AddOnTop("MatrixRowSeparator");
  this->controlSequences.AddOnTop("\\begin");
  this->controlSequences.AddOnTop("\\left");
  this->controlSequences.AddOnTop("\\right");
  this->controlSequences.AddOnTop("array");
  this->controlSequences.AddOnTop("\\end");
  this->controlSequences.AddOnTop("\\\\");
  this->controlSequences.AddOnTop("\\");
  this->controlSequences.AddOnTop("&");
  this->controlSequences.AddOnTop("%");
  this->controlSequences.AddOnTop("LogParsing");
  this->controlSequences.AddOnTop("LogEvaluation");
  this->controlSequences.AddOnTop("LogFull");
  this->controlSequences.AddOnTop("LatexLink");
  this->controlSequences.AddOnTop("FullTree");
  this->controlSequences.AddOnTop("HideLHS");
  this->controlSequences.AddOnTop("EndProgram");
//additional operations treated like regular expressions.
  this->AddOperationNoRepetitionAllowed("MonomialCollection");
  this->AddOperationNoRepetitionAllowed("MonomialPoly");
  this->AddOperationNoRepetitionAllowed("Serialization");
  this->AddOperationNoRepetitionAllowed("Melt");
  this->AddOperationNoRepetitionAllowed("Bind");


  this->TotalNumPatternMatchedPerformed=0;
  this->initPredefinedStandardOperations();
  this->initPredefinedInnerFunctions();
  Expression theSSLieAlgrule;
  this->RuleStack.SetSize(0);
  this->RuleContextIdentifier=0;
/*  this->Evaluate
  ("(InternalVariable1{}{{InternalVariableA}})_{{InternalVariableB}}\
   :=getSemisimpleLieAlgGenerator{}\
   (InternalVariable1{}InternalVariableA,\
   InternalVariableB)")
  ;
  this->theProgramExpression[1][0].SetChildAtomValue(0, this->opSSLieAlg(), *this);
  this->theProgramExpression[2][1][1].SetChildAtomValue(0, this->opSSLieAlg(), *this);
  this->RuleStack.AddOnTop(this->theProgramExpression);
*/


//  std::cout << "<br>Num lists created at command list initialization exit: " << NumListsCreated;
}

Expression::FunctionAddress CommandList::GetInnerFunctionFromOp
(int theOp, const Expression& left, const Expression& right)
{ std::cout << "Function CommandList::GetfOp not implemented yet. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  assert(false);
  return 0;
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::TensorOnTheRight
  (const ElementTensorsGeneralizedVermas<coefficient>& right, GlobalVariables& theGlobalVariables,
   const coefficient& theRingUnit, const coefficient& theRingZero
   )
{ MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<coefficient>::TensorOnTheRight");
  if (right.IsEqualToZero())
  { this->MakeZero();
    return;
  }
  int maxNumMonsFinal=this->size*right.size;
  ElementTensorsGeneralizedVermas<coefficient> output;
  MonomialTensorGeneralizedVermas<coefficient> bufferMon;
  output.MakeZero();
  output.SetExpectedSize(maxNumMonsFinal);
  coefficient theCoeff;
  for (int i=0; i<right.size; i++)
    for (int j=0; j<this->size; j++)
    { bufferMon=this->TheObjects[j];
      bufferMon*=(right[i]);
      theCoeff=this->theCoeffs[j];
      theCoeff*=right.theCoeffs[i];
      output.AddMonomial(bufferMon, theCoeff);
      ParallelComputing::SafePointDontCallMeFromDestructors();
    }
  *this=output;
}

bool CommandList::outerTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ToString();
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::StandardTensor");
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

bool CommandList::outerExtractBaseMultiplication
(CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::outerExtractBaseMultiplication");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  bool result=false;
  //  std::cout << "<br>handling base extraction of: " << input.ToString();
  //handle Anything*Rational:=Rational*Anything
  output=input;
  if (output[2].IsOfType<Rational>())
  { output.children.SwapTwoIndices(1, 2);
    result=true;
//    std::cout << "swapped " << leftE.ToString() << " and " << rightE.ToString();
  }
//  Expression leftE=output[1];
//  Expression rightE=output[2];
  if (output[2].IsListStartingWithAtom(theCommands.opTimes()))
  { if (output[2].children.size!=3)
      return result;
//    Expression rightLeftE=rightE[1];
//    Expression rightRightE=rightE[2];
    //handle Anything1*(Rational*Anything2):=Rational*(Anything1*Anything2)
    if (output[2][1].IsOfType<Rational>())
    { Expression tempRight;
      tempRight.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][2]);
      output.MakeXOX(theCommands, theCommands.opTimes(), output[2][1], tempRight);
      result=true;
//      std::cout << " swapped " << rightLeftE.ToString() << " and " << leftE.ToString();
    }
    //<- handle a*(b*anything)
    //on condition that a*b has an inner handler
    Expression tempExp, newExpr;
    tempExp.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][1]);
    if (theCommands.innerTimes(theCommands, tempExp, newExpr))
    { output.MakeProducT(theCommands, newExpr, output[2][2]);
      result=true;
    }
  }
  //handle 0*anything=0
  if (output[1].IsEqualToZero())
    return output.AssignValue(0, theCommands);
  return result;
}

bool CommandList::innerCollectMultiplicands
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  Expression constPower, thePower;
  const Expression* left= &input[1];
  const Expression* right=&input[2];
  if (*left==*right)
  { constPower.AssignValue(2, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), *left, constPower);
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
          return output.MakeXOX(theCommands, theCommands.opThePower(), (*left)[1], thePower);
        }
    }
  return false;
}

bool CommandList::outerCombineFractions
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::outerCheckRule
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDefine(), 3))
    return false;
  if (input[1]!=input[2])
    return false;
  std::stringstream out;
  out << "Bad rule: you are asking me to substitute " << input[1] << " by itself (" << input[2] << ")"
  << " which is an infinite substitution cycle. ";
  return output.SetError(out.str(), theCommands);
}

bool CommandList::innerSubZeroDivAnythingWithZero
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsEqualToZero())
    if (!input[2].IsEqualToZero())
      return output.AssignValue(0, theCommands);
  return false;
}

bool CommandList::innerCancelMultiplicativeInverse
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::innerAssociateDivisionDivision
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  if (input[1].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { Expression newRightE;
    newRightE.MakeXOX(theCommands, theCommands.opTimes(), input[2], input[1][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], newRightE);
  }
  if (input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
  { Expression newLeftE;
    newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][1]);
  }
  return false;
}

bool CommandList::outerAssociateTimesDivision
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  if (!input[2].IsListNElementsStartingWithAtom(theCommands.opDivide(), 3))
    return false;
  Expression newLeftE;
  newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][1]);
  output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][2]);
  return true;
}

bool CommandList::outerAssociate
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::StandardIsDenotedBy
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::StandardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElementsStartingWithAtom(theCommands.opIsDenotedBy(), 3))
    return false;
  const Expression& withNotation=input[2];
  const Expression& theNotation=input[1];
  theCommands.Comments << "<br>Registering notation: globally, " << withNotation.ToString()
  << " will be denoted by " << theNotation.ToString();
  theCommands.theObjectContainer.ExpressionNotation.AddOnTop(theNotation.ToString());
  theCommands.theObjectContainer.ExpressionWithNotation.AddOnTop(withNotation);
  output=input;
  output.SetChildAtomValue(0, theCommands.opDefine());
  ////
  if (withNotation.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    if (withNotation.GetValuENonConstUseWithCaution
        <ElementTensorsGeneralizedVermas<RationalFunctionOld> >().IsHWV())
    { MonomialGeneralizedVerma<RationalFunctionOld>& theElt=
      withNotation.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()[0].theMons[0];
      theElt.GetOwner().highestWeightVectorNotation=theNotation.ToString();
    }
  return true;
}

bool CommandList::innerMultiplyByOne
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListStartingWithAtom(theCommands.opTimes()) || input.children.size!=3)
    return false;
  if (!input[1].IsEqualToOne())
    return false;
  output=input[2];
  return true;
}

bool CommandList::outerTimesToFunctionApplication
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes()))
    return false;
  if (input.children.size<2)
    return false;
  const Expression& firstElt=input[1];
  if (!firstElt.IsBuiltInOperation())
    return false;
  output=input;
  output.children.RemoveIndexShiftDown(0);
  return true;
}

bool CommandList::outerDistribute
(CommandList& theCommands, const Expression& input, Expression& output,
 int AdditiveOp, int multiplicativeOp)
{ if (theCommands.outerLeftDistributeBracketIsOnTheLeft
      (theCommands, input, output, AdditiveOp, multiplicativeOp))
    return true;
  return theCommands.outerRightDistributeBracketIsOnTheRight
  (theCommands, input, output, AdditiveOp, multiplicativeOp);
}

bool CommandList::outerLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, const Expression& input, Expression& output,
 int AdditiveOp, int multiplicativeOp)
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

bool CommandList::outerRightDistributeBracketIsOnTheRight
(CommandList& theCommands, const Expression& input, Expression& output,
 int AdditiveOp, int multiplicativeOp)
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

bool CommandList::CollectSummands
(CommandList& theCommands, const Expression& input, MonomialCollection<Expression, Rational>& outputSum)
{ List<Expression> summands;
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
  return true;
}

bool CommandList::innerAssociateExponentExponent
(CommandList& theCommands, const Expression& input, Expression& output)
{ int opPower=theCommands.opThePower();
  if (!input.IsListNElementsStartingWithAtom(opPower, 3))
    return false;
  if (!input[1].IsListNElementsStartingWithAtom(opPower, 3))
    return false;
  Expression tempE;
  tempE.MakeProducT(theCommands, input[1][2], input[2]);
  return output.MakeXOX(theCommands, opPower, input[1][1], tempE);
}

bool CommandList::outerPowerRaiseToFirst
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    return false;
  if (input[2].IsEqualToOne())
  { std::string tempS;
    if (input[1].IsOperation(&tempS))
      if (tempS=="A" || tempS=="B" || tempS=="C" || tempS=="D" || tempS=="E" ||
          tempS=="F" || tempS=="G")
        return false;
    output=input[1];
    return true;
  }
  return false;
}

bool CommandList::outerPlus
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerPlus");
//  theCommands.Comments << "<hr><hr>processing outer plus with input: " << input.Lispify();
  if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus()))
    return false;
  MonomialCollection<Expression, Rational> theSum;
  theCommands.CollectSummands(theCommands, input, theSum);
//  Rational constTerm=0;
//  bool foundConstTerm=false;
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
  if (theSum.IsEqualToZero())
    return output.AssignValue<Rational>(0, theCommands);
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(theSum.size);
  for (int i=0; i<theSum.size; i++)
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
    { std::cout << "This is a pgoramming error: bad comparison! "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  summandsWithCoeff.QuickSortAscending();
  if (summandsWithCoeff.size==1)
  { output=summandsWithCoeff[0];
    return true;
  }
  output.MakeXOX
  (theCommands, theCommands.opPlus(), summandsWithCoeff[summandsWithCoeff.size-1],
   summandsWithCoeff[summandsWithCoeff.size-2]);
  Expression result;
  for (int i=summandsWithCoeff.size-3; i>=0; i--)
  { result.reset(theCommands, 3);
    result.AddChildAtomOnTop(theCommands.opPlus());
    result.AddChildOnTop(output);
    result.AddChildOnTop(summandsWithCoeff[i]);
    output=result;
  }
  return true;
}

bool CommandList::EvaluateIf
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::outerStandardFunction
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerStandardFunction");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (&input==&output)
  { std::cout << "This is a programming error: the input and output object of "
    << " outerStandardFunction must be different."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (!input.IsLisT())
    return false;
  const Expression& functionNameNode =input[0];
  int theIndex;
//  std::cout << "<br>Simplifying " << input.ToString();
  if (functionNameNode.IsListStartingWithAtom(theCommands.opSequence()))
    if (input.children.size==2)
      if (input[1].IsSmallInteger(&theIndex))
        if (theIndex>0 && theIndex<functionNameNode.children.size)
        { output=input[theIndex];
          return true;
        }
  if (!functionNameNode.IsAtoM())
    return false;
//  std::cout << "<br>Evaluating: " << input.ToString();
  for (int i=0; i<theCommands.FunctionHandlers[functionNameNode.theData].size; i++)
    if (!theCommands.FunctionHandlers[functionNameNode.theData][i].flagIsInner)
    { Function& outerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (outerFun.theFunction(theCommands, input, output))
        if(output!=input)
        { return true;
        }
    } else
    { Function& innerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      //if (functionNameNode.ToString()=="+")
      //{ bool tempbool=true;
      //   std::cout << "<br>Here i am!";
      //}
      if (input.children.size>2)
      { //std::cout << "more than 2 children: " << input.Lispify();
        if (innerFun.inputFitsMyInnerType(input))
          if (innerFun.theFunction(theCommands, input, output))
          { return true;
          }
      } else
        if (innerFun.inputFitsMyInnerType(input[1]))
          if (innerFun.theFunction(theCommands, input[1], output))
          { return true;
          }
    }
  return false;
}

bool CommandList::outerGreaterThan
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::outerEqualEqual
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::outerLieBracket
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerLieBracket");
  if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (leftE.IsOfType<Rational>() && rightE.IsOfType<Rational>())
    return output.AssignValue(0, theCommands);
  if (leftE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >() &&
      rightE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >() )
  { Expression leftCopy=leftE;
    Expression rightCopy=rightE;
    Expression finalContext;
    if (!Expression::ContextConvertBothToCommonContext(leftCopy, rightCopy, finalContext))
      return false;
    ElementUniversalEnveloping<RationalFunctionOld> result;
    leftCopy.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >()
    .LieBracketOnTheRight
    (rightCopy.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >(), result);
    result.Simplify(*theCommands.theGlobalVariableS);
    return output.AssignValueWithContext(result, leftCopy.GetContext(), theCommands);
  }
  return false;
}

bool Expression::SetError(const std::string& theError, CommandList& owner)
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

bool Expression::IsInteger(LargeInt* whichInteger)const
{ Rational theRat;
  if (!this->IsOfType<Rational>(&theRat))
    return false;
  return theRat.IsInteger(whichInteger);
}

bool CommandList::outerUnion
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::outerUnionNoRepetition
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandList::outerDivide
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerDivide");
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

bool CommandList::outerMinus
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!(input.IsListNElementsStartingWithAtom(theCommands.opMinus(), 3) ||
        input.IsListNElementsStartingWithAtom(theCommands.opMinus(), 2)) )
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

bool CommandList::ExpressionMatchesPattern
(const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions,
 std::stringstream* theLog)
{ MacroRegisterFunctionWithName("CommandList::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.theBoss==this && input.theBoss==this))
  { std::cout << "This is a programming error. Either a pattern or an input has a wrongly "
    << " initialized owner: the pattern is " << thePattern.ToString() << " and the input is "
    << input.ToString() << ". "
    << " The error is certainly in the preceding code; here "
    << "is a stack trace"
    << ", however beware that the error might be in code preceding the stack loading. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
//  static int ExpressionMatchesPatternDebugCounter=-1;
  static const bool printLocalDebugInfo=false;
//  if (input.ToString()=="f{}((a)):=a+5")
//    printLocalDebugInfo=true;
  //ExpressionMatchesPatternDebugCounter++;
//  std::cout << " ExpressionMatchesPatternDebugCounter: " << ExpressionMatchesPatternDebugCounter;
//  printLocalDebugInfo=(ExpressionMatchesPatternDebugCounter>-1);
  if (printLocalDebugInfo)
  { std::cout << " <hr> current input: " << input.ToString() << "<br>current pattern: " << thePattern.ToString();
    std::cout << "<br> current matched expressions: " << matchedExpressions.ToString();
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Max recursion depth of " << this->MaxRecursionDeptH
    << " exceeded whlie trying to match expression pattern "
    << thePattern.ToString() << " onto expression " << input.ToString();
    this->evaluationErrors.AddOnTop(out.str());
    return false;
  }
//  if (this->opDefine()==input.theOperation)
//    return false;
  int opVarB=this->opBind();
  if (thePattern.IsListStartingWithAtom(opVarB))
  { int indexLeft= matchedExpressions.theBoundVariables.GetIndex(thePattern);
    if (indexLeft==-1)
    { matchedExpressions.theBoundVariables.AddOnTop(thePattern);
      matchedExpressions.variableImages.AddOnTop(input);
      indexLeft=matchedExpressions.theBoundVariables.size-1;
    }
    if (matchedExpressions.variableImages[indexLeft]!=input)
      return false;
    if (printLocalDebugInfo)
      std::cout << "<br><b>Match!</b>";
    return true;
  }
  if (thePattern.theData!=input.theData || thePattern.children.size!=input.children.size )
    return false;
  for (int i=0; i<thePattern.children.size; i++)
    if (!(this->ExpressionMatchesPattern(thePattern[i], input[i], matchedExpressions, theLog)))
      return false;
  if (printLocalDebugInfo)
    std::cout << "<br><b>Match!</b>";
  return true;
}

StackMaintainerRules::StackMaintainerRules(CommandList* inputBoss)
{ this->theBoss=inputBoss;
  if (this->theBoss!=0)
    this->startingRuleStackSize=inputBoss->RuleStack.size;
}

StackMaintainerRules::~StackMaintainerRules()
{ if (this->theBoss!=0)
    this->theBoss->RuleStack.SetSize(this->startingRuleStackSize);
  this->theBoss=0;
}

bool CommandList::EvaluateExpression
(const Expression& input, Expression& output, BoundVariablesSubstitution& bufferPairs,
 bool& outputIsFree)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::EvaluateExpression");
  if (this->flagLogFullTreeCrunching && this->RecursionDeptH<3)
  { this->Comments << "<br>";
    for (int i=0; i<this->RecursionDeptH; i++)
      this->Comments << "&nbsp&nbsp&nbsp&nbsp";
    this->Comments << "Evaluating " << input.Lispify()
    << " with rule stack of size " << this->RuleStack.size; // << this->RuleStack.ToString();
  }
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Recursion depth limit of " << this->MaxRecursionDeptH
    << " exceeded while evaluating expressions.";
    output.SetError(out.str(), *this);
    if (this->flagMaxRecursionErrorEncountered)
      this->evaluationErrors.AddOnTop(out.str());
    this->flagMaxRecursionErrorEncountered=true;
    this->flagAbortComputationASAP=true;
    return true;
  }
  output=input;
  if (output.IsError())
  { this->flagAbortComputationASAP=true;
    return true;
  }
  if (this->ExpressionStack.Contains(input))
  { std::stringstream errorStream;
    errorStream << "I think I have detected an infinite cycle: I am asked to  "
    << " reduce " << input.ToString()
    << " but I have already seen that expression in the expression stack. ";
    this->flagAbortComputationASAP=true;
    return output.SetError(errorStream.str(), *this);
  }
  this->ExpressionStack.AddOnTop(input);
  Expression theExpressionWithContext;
  theExpressionWithContext.reset(*this, 3);
  theExpressionWithContext.AddChildAtomOnTop(this->opSequence());
  theExpressionWithContext.AddChildValueOnTop(this->RuleContextIdentifier);
  theExpressionWithContext.AddChildOnTop(input);
  int indexInCache=this->cachedExpressions.GetIndex(theExpressionWithContext);
  if (indexInCache!=-1)
    output= this->imagesCachedExpressions[indexInCache];
  else if (this->cachedExpressions.size<this->MaxNumCachedExpressionPerContext)
  { this->cachedExpressions.AddOnTop(theExpressionWithContext);
    indexInCache=this->cachedExpressions.size-1;
    this->imagesCachedExpressions.SetSize(indexInCache+1);
    this->imagesCachedExpressions.LastObject()->reset(*this);
    this->imagesCachedExpressions.LastObject()->SetError("Error: not computed yet.", *this);
  }
  //reduction phase:
  outputIsFree=true;
  bool ReductionOcurred=true;
  int counterNumTransformations=0;
  Expression tempE;
  tempE.reset(*this);
  std::string beforePatternMatch;
  while (ReductionOcurred && !this->flagAbortComputationASAP)
  { StackMaintainerRules theRuleStackMaintainer(this);
    ReductionOcurred=false;
    counterNumTransformations++;
    //if (this->flagLogEvaluation && counterNumTransformations>1 )
    //{ this->Comments << "<br>input: " << input.ToString() << "->"
    //  << output.ToString();
    //}
    if (indexInCache!=-1)
      this->imagesCachedExpressions[indexInCache]=output;
//////------Handling naughty expressions------
    if (this->theGlobalVariableS->GetElapsedSeconds()!=0)
      if (this->theGlobalVariableS->GetElapsedSeconds()>this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2)
      { if (!this->flagTimeLimitErrorDetected)
          std::cout << "<br><b>Max allowed computational time is "
          << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2 << ";  so far, "
          << this->theGlobalVariableS->GetElapsedSeconds()-this->StartTimeEvaluationInSecondS
          << " have elapsed -> aborting computation ungracefully.</b>";
        this->flagTimeLimitErrorDetected=true;
        this->flagAbortComputationASAP=true;
        break;
      }
    if (counterNumTransformations>this->MaxAlgTransformationsPerExpression)
    { if (!this->flagMaxTransformationsErrorEncountered)
      { std::stringstream out;
        out << " While simplifying " << output.ToString(&this->formatVisibleStrings)
        << "<br>Maximum number of algebraic transformations of "
        << this->MaxAlgTransformationsPerExpression << " exceeded.";
        output.SetError(out.str(), *this);
        this->flagAbortComputationASAP=true;
        this->flagMaxTransformationsErrorEncountered=true;
      }
      break;
    }
//////------Handling naughty expressions end------
/////-------Evaluating children if the expression is not of built-in type-------
    //bool foundError=false;
    if (!output.IsBuiltInType())
      for (int i=0; i<output.children.size && !this->flagAbortComputationASAP; i++)
      { bool tempBool=true;
  //      bool debugBool=false;
  //      if (output[i].ToString()=="c");
  //        debugBool=true;
        if (this->EvaluateExpression(output[i], tempE, bufferPairs, tempBool))
          output.SetChilD(i, tempE);
        if (!tempBool)
          outputIsFree=false;
        if (output[i].IsError())
        { this->flagAbortComputationASAP=true;
          break;
        }
  /*      if (this->RecursionDeptH==1)
        { std::cout << "<hr>Considering whether "
          << output[i].Lispify() << " is rule-stack-worthy.";
          if (output.ToString()=="a:=b")
            std::cout << "<hr>a:=b is here<hr>";
        }*/
        if (output.IsListNElementsStartingWithAtom(this->opEndStatement()))
          if (output[i].IsListNElementsStartingWithAtom(this->opDefine()) ||
            output[i].IsListNElementsStartingWithAtom(this->opDefineConditional()))
          { this->RuleStack.AddOnTop(output[i]);
            this->RuleContextIdentifier++;
           // std::cout << ".. added !!!!";
          }
      }
    if (this->flagAbortComputationASAP)
      break;
    //->/////-------Default operation handling-------
    if (this->outerStandardFunction(*this, output, tempE))
    { ReductionOcurred=true;
      if (this->flagLogEvaluatioN)
        this->Comments << "<hr>Substitution:<br>" << output.ToString()
        << "  ->  " << tempE.ToString();
      output=tempE;
      continue;
    }
    //->/////-------Default operation handling end-------
    //if (foundError || !resultExpressionIsFree)
      //break;
/////-------Evaluating children end-------
/////-------User-defined pattern matching------
    for (int i=0; i<this->RuleStack.size && !this->flagAbortComputationASAP; i++)
    { Expression& currentPattern=this->RuleStack[i];
      this->TotalNumPatternMatchedPerformed++;
      bufferPairs.reset();
//      std::cout << "<br>Checking whether "
//      << output.ToString() << " matches " << currentPattern.ToString();
      if (this->flagLogEvaluatioN)
        beforePatternMatch=output.ToString();
      if(this->ProcessOneExpressionOnePatternOneSub
         (currentPattern, output, bufferPairs, &this->Comments, this->flagLogPatternMatching))
      { ReductionOcurred=true;
        if (this->flagLogEvaluatioN)
          this->Comments << "<hr>Substitution:<br>" << beforePatternMatch
          << "  ->  " << output.ToString();
        break;
      }
    }
/////-------User-defined pattern matching end------
  }
  this->ExpressionStack.RemoveIndexSwapWithLast(this->ExpressionStack.size-1);
  if (output.IsListStartingWithAtom(this->opBind()))
    outputIsFree=false;
  if (this->flagLogFullTreeCrunching && this->RecursionDeptH<3)
  { this->Comments << "<br>";
    for (int i=0; i<this->RecursionDeptH; i++)
      this->Comments << "&nbsp&nbsp&nbsp&nbsp";
    this->Comments << "to get: " << output.Lispify();
  }

  return true;
}

Expression* CommandList::PatternMatch
(const Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
 const Expression* condition, std::stringstream* theLog, bool logAttempts)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::PatternMatch");
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of "
    << this->MaxRecursionDeptH << " was exceeded";
    theExpression.SetError(out.str(), *this);
    return 0;
  }
//  if (theExpression.ToString()=="f{}((a)):=a+5")
//  { std::cout << "!here";
//  }
  thePattern.CheckInitialization();
  theExpression.CheckInitialization();
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, theLog))
    return 0;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>found pattern: " << theExpression.ToString() << " -> "
    << thePattern.ToString() << " with " << bufferPairs.ToString();
  if (condition==0)
    return &theExpression;
  Expression tempExp=*condition;
  tempExp.CheckInitialization();
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.ToString();
  this->SpecializeBoundVars(tempExp, bufferPairs);
  tempExp.CheckInitialization();
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specialized condition: " << tempExp.ToString() << "; evaluating...";
  BoundVariablesSubstitution tempPairs;
  bool isFree;
  Expression conditionResult;
  this->EvaluateExpression(tempExp, conditionResult, tempPairs, isFree);
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>The evaluated specialized condition: " << conditionResult.ToString() << "; evaluating...";
  if (conditionResult.IsEqualToOne())
    return &theExpression;
  return 0;
}

void CommandList::SpecializeBoundVars
(Expression& toBeSubbedIn, BoundVariablesSubstitution& matchedPairs)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (toBeSubbedIn.IsListOfTwoAtomsStartingWith(this->opBind()))
  { int indexMatching= matchedPairs.theBoundVariables.GetIndex(toBeSubbedIn);
    if (indexMatching!=-1)
    { toBeSubbedIn=matchedPairs.variableImages[indexMatching];
      //this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
      return;
    }
  }
  Expression subbedE;
  for (int i=0; i<toBeSubbedIn.children.size; i++)
  { subbedE=toBeSubbedIn[i];
    this->SpecializeBoundVars(subbedE, matchedPairs);
    toBeSubbedIn.SetChilD(i, subbedE);
  }
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
(Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
 std::stringstream* theLog, bool logAttempts)
{ MacroRegisterFunctionWithName("CommandList::ProcessOneExpressionOnePatternOneSub");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (!thePattern.IsListNElementsStartingWithAtom(this->opDefine(), 3) &&
      !thePattern.IsListNElementsStartingWithAtom(this->opDefineConditional(), 4))
    return false;
  if (theLog!=0 && logAttempts)
  { (*theLog) << "<hr>attempting to reduce expression " << theExpression.ToString();
    (*theLog) << " by pattern " << thePattern.ToString();
  }
  theExpression.CheckInitialization();
  const Expression& currentPattern=thePattern[1];
  const Expression* theCondition=0;
  bool isConditionalDefine=
  thePattern.IsListNElementsStartingWithAtom(this->opDefineConditional(), 4);
  if (isConditionalDefine)
    theCondition=&thePattern[2];
  Expression* toBeSubbed=this->PatternMatch
  (currentPattern, theExpression, bufferPairs, theCondition, theLog, logAttempts);
  if (toBeSubbed==0)
    return false;
  if (theLog!=0 && logAttempts)
    *theLog << "<br><b>found a match!</b>";
  if (isConditionalDefine)
    *toBeSubbed=thePattern[3];
  else
    *toBeSubbed=thePattern[2];
  this->SpecializeBoundVars(*toBeSubbed, bufferPairs);
  return true;
}

bool CommandList::fExpressionHasBoundVars
  (CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (theCommands.RecursionDeptH>theCommands.MaxRecursionDeptH)
    return false;
  if (input.IsListOfTwoAtomsStartingWith(theCommands.opBind()))
    return output.AssignValue(1, theCommands);
  else
    for (int i=0; i<input.children.size; i++)
      if (!theCommands.fExpressionHasBoundVars(theCommands, input[i], output))
        return false;
      else if (output.IsEqualToOne())
        return true;
  return output.AssignValue(0, theCommands);
}

bool CommandList::ExtractExpressions
(Expression& outputExpression, std::string* outputErrors)
{ //std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->CurrentSyntacticStacK).ReservE((*this->CurrrentSyntacticSouP).size+this->numEmptyTokensStart);
  (*this->CurrentSyntacticStacK).SetSize(this->numEmptyTokensStart);
  this->registerNumNonClosedBeginArray=0;
  this->registerPositionAfterDecimalPoint=0;
  for (int i=0; i<this->numEmptyTokensStart; i++)
    (*this->CurrentSyntacticStacK)[i]=this->GetEmptySyntacticElement();
  this->parsingLog="";
  this->NonBoundVariablesStack.SetSize(1);
  this->BoundVariablesStack.SetSize(1);
  this->NonBoundVariablesStack.LastObject()->Clear();
  this->BoundVariablesStack.LastObject()->Clear();
  for (this->counterInSyntacticSoup=0; this->counterInSyntacticSoup<(*this->CurrrentSyntacticSouP).size; this->counterInSyntacticSoup++)
  { (*this->CurrentSyntacticStacK).AddOnTop((*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup]);
    while(this->ApplyOneRule())
    {}
  }
  bool success=false;
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart)
    errorLog << "Non-meaningful input detected (spacebar, enter characters only?).";
  else if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
  { SyntacticElement& result=(*this->CurrentSyntacticStacK)[this->numEmptyTokensStart];
    if (result.errorString=="" && result.controlIndex==this->conExpression())
    { outputExpression=result.theData;
      success=true;
//      std::cout << "Success: " << result.theData.ToString();
    }
    else if (result.errorString!="")
      errorLog << "Syntax error with message: " << result.errorString;
    else
    { errorLog << "Syntax error: your command simplifies to a single syntactic element but it is not an expression. <br>";
      errorLog << "It simplifies to:<br> " << this->ElementToStringSyntacticStack();
    }
  } else
  { errorLog << "Syntax error: your command does not simplify to a single syntactic element. <br>";
    errorLog << "Instead it simplifies to:<br> " << this->ElementToStringSyntacticStack();
  }
  if (outputErrors!=0)
    *outputErrors=errorLog.str();
  return success;
}

void CommandList::EvaluateCommands()
{ MacroRegisterFunctionWithName("CommandList::EvaluateCommands");
  std::stringstream out;
  BoundVariablesSubstitution thePairs;

//  this->theLogs.resize(this->theCommands.size());
//this->ToString();
  //std::stringstream comments;
  if (this->syntaxErrors!="")
  { out << "<hr><b>Syntax errors encountered</b><br>";
    out << this->syntaxErrors;
    out << "<hr>";
  }
//  std::cout
//  << "Starting expression: " << this->theProgramExpression.ToString()
//  << "<hr>";
  Expression StartingExpression=this->theProgramExpression;
  this->flagAbortComputationASAP=false;
  bool tempBool;
  this->Comments.clear();
  this->EvaluateExpression
  (this->theProgramExpression, this->theProgramExpression, thePairs, tempBool);
  if (this->RecursionDeptH!=0)
  { std::cout << "This is a programming error: the starting recursion depth "
    << "before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->theGlobalVariableS->theDefaultFormat.flagMakingExpressionTableWithLatex=true;
  this->theGlobalVariableS->theDefaultFormat.flagUseLatex=true;
  this->theGlobalVariableS->theDefaultFormat.flagExpressionIsFinal=true;
  this->theGlobalVariableS->theDefaultFormat.flagExpressionNewLineAllowed=true;
  if (!this->flagDisplayFullExpressionTree)
    out << this->theProgramExpression.ToString
    (&this->theGlobalVariableS->theDefaultFormat, &StartingExpression);
  else
  { out << "<hr>Input:<br> " << StartingExpression.ToStringFull() << "<hr>"
    << "Output:<br>" << this->theProgramExpression.ToStringFull();
  }
  this->outputString=out.str();
  if (this->Comments.str()!="")
  { std::stringstream commentsStream;
    commentsStream << "<b>Comments.</b><br><span>" << this->Comments.str() << "</span>";
    this->outputCommentsString=commentsStream.str();
  }
}

std::string SyntacticElement::ToString(CommandList& theBoss)const
{ std::stringstream out;
  bool makeTable=this->controlIndex==theBoss.conExpression() || this->controlIndex==theBoss.conError()
  || this->controlIndex==theBoss.conSequence() ;
  if (makeTable)
    out << "<table border=\"1\"><tr><td>";
  if (this->controlIndex<0)
    out << "Error: control index is not initialized! This is likely a programming error.";
  else
    out << theBoss.controlSequences[this->controlIndex] << " ";
  if (makeTable)
  { out << "</td></tr><tr><td>";
    out << this->theData.ToString(0);
    out << "</td></tr><tr><td>" << this->theData.ToStringFull();
    if (this->errorString!="")
      out << "</td></tr><tr><td>" << this->errorString;
    out << "</td></tr></table>";
  } else
    out << this->theData.ToString(0);
  return out.str();
}

SemisimpleLieAlgebra* Expression::GetAmbientSSAlgebraNonConstUseWithCaution()const
{ this->CheckInitialization();
  Expression myContext=this->GetContext();
  return &this->theBoss->theObjectContainer.theLieAlgebras[myContext.ContextGetIndexAmbientSSalg()];
}

int Expression::ContextGetIndexAmbientSSalg()const
{ //std::cout << "<hr>trying to get ambient algebra from " << this->ToString();
  if (!this->IsContext() )
    return -1;
//  std::cout << ". I have " << this->children.size << " children. ";
  for (int i=1; i<this->children.size; i++)
    if ((*this)[i].IsListNElementsStartingWithAtom(this->theBoss->opSSLieAlg(), 2))
      return (*this)[i][1].theData;
  return -1;
}

Expression Expression::GetContext()const
{ this->CheckInitialization();
  if (this->HasContext())
    return (*this)[1];
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

bool Expression::operator>(const Expression& other)const
{ if (this->IsOfType<Rational>() && other.IsOfType<Rational>())
    return this->GetValuE<Rational>()>other.GetValuE<Rational>();
  if (this->children.size>other.children.size)
    return true;
  if (other.children.size>this->children.size)
    return false;
  if (this->children.size==0)
  { std::string leftS, rightS;
    if (this->IsOperation(&leftS))
      if (other.IsOperation(&rightS))
        return leftS>rightS;
    return this->theData>other.theData;
  }
  for (int i=0; i<this->children.size; i++)
  { if ((*this)[i]>other[i])
      return true;
    if (other[i]>(*this)[i])
      return false;
  }
  return false;
}

bool Expression::ToStringData
(std::string& output, FormatExpressions* theFormat, Expression* startingExpression)const
{ MacroRegisterFunctionWithName("Expression::ToStringData");
  std::stringstream out;
  bool result=false;
  bool isFinal=theFormat==0 ? false : theFormat->flagExpressionIsFinal;
  if (this->IsAtoM())
  { if (this->theData< this->theBoss->GetOperations().size && this->theData>=0)
      out << this->theBoss->GetOperations()[this->theData];
    else
      out << "(unknown~ atom~ of~ value~ " << this->theData << ")";
    result=true;
  } else if (this->IsOfType<std::string>())
  { if (isFinal)
      out << this->GetValuE<std::string>();
    else
    { out << "(string~ not~ shown~ to~ avoid~ javascript~ problems~ (in~ case~ the~ string~ has~ javascript)";
      //out << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    }
    result=true;
  } else if (this->IsOfType<Rational>())
  { if (this->HasNonEmptyContext())
      out << "Rational{}(" << (*this)[1].ToString() << ", ";
    out << this->GetValuE<Rational>().ToString();
    if (this->HasNonEmptyContext())
      out << ")";
    result=true;
  } else if (this->IsOfType<Polynomial<Rational> >())
  { out << "Polynomial{}(";
    Expression contextE=this->GetContext();
    FormatExpressions tempFormat;
//    out << "(" << contextE.ToString(theFormat) << ", "
    contextE.ContextGetFormatExpressions(tempFormat);
    out << this->GetValuE<Polynomial<Rational> >().ToString(&tempFormat) << ")";
    result=true;
  } else if (this->IsOfType<RationalFunctionOld>())
  { out << "RationalFunction{}";
    Expression contextE=this->GetContext();
    out << "(" << contextE.ToString(theFormat) << ", "
    << this->GetValuE<RationalFunctionOld>().ToString(theFormat) << ")";
    result=true;
  } else if (this->IsOfType<SemisimpleLieAlgebra>())
  { out << "SSLieAlg{}("
    //<< this->GetContext().ToString(theFormat) << ", "
    << this->GetValuE<SemisimpleLieAlgebra>().GetLieAlgebraName()
    << ")";
    result=true;
  } else if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { FormatExpressions tempFormat;
    this->GetContext().ContextGetFormatExpressions(tempFormat);
    out << "UEE{}(" //<< this->GetContext().ToString() << ", "
    << this->GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >().ToString(&tempFormat)
    << ")";
    result=true;
  } else if (this->IsOfType<Matrix<Rational> >())
  { FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    tempFormat.flagUseHTML=false;
    out << "innerMatrixRational{}("
    << this->GetValuE<Matrix<Rational> > ().ToString(&tempFormat) << ")";
    result=true;
  } else if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { FormatExpressions tempFormat;
    this->GetContext().ContextGetFormatExpressions(tempFormat);
    out << "ETGVM{}(";
    out << this->GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().ToString(&tempFormat);
    out << ")";
    result=true;
  } else if (this->IsOfType<CalculusFunctionPlot>())
  { if (isFinal)
    { CalculusFunctionPlot& thePlot=this->GetValuENonConstUseWithCaution<CalculusFunctionPlot>();
      out << this->theBoss->WriteDefaultLatexFileReturnHtmlLink
      (thePlot.GetPlotStringAddLatexCommands(false), true);
      out << "<br><b>LaTeX code used to generate the output. </b><br>"
      << thePlot.GetPlotStringAddLatexCommands(true);
    } else
      out << "(plot not shown)";
    result=true;
  } else if (this->IsOfType<WeylGroup>())
  { WeylGroup& theGroup=this->GetValuENonConstUseWithCaution<WeylGroup>();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    tempFormat.flagUseHTML=false;
    tempFormat.flagUseReflectionNotation=true;
    out << theGroup.ToString(&tempFormat);
    result=true;
  } else if (this->IsOfType<ElementWeylGroup>())
  { ElementWeylGroup& theElt=this->GetValuENonConstUseWithCaution<ElementWeylGroup>();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    tempFormat.flagUseHTML=false;
    tempFormat.flagUseReflectionNotation=true;
    out << theElt.ToString(&tempFormat);
    result=true;
  } else if (this->IsOfType<WeylGroupRepresentation<Rational> >())
  { WeylGroupRepresentation<Rational>& theElt=
    this->GetValuENonConstUseWithCaution<WeylGroupRepresentation<Rational> >();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    tempFormat.flagUseHTML=false;
    tempFormat.flagUseReflectionNotation=true;
    out << theElt.ToString(&tempFormat);
    result=true;
  } else if (this->IsOfType<WeylGroupVirtualRepresentation>())
  { WeylGroupVirtualRepresentation& theElt=
    this->GetValuENonConstUseWithCaution<WeylGroupVirtualRepresentation>();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    tempFormat.flagUseHTML=false;
    tempFormat.flagUseReflectionNotation=true;
    out << theElt.ToString(&tempFormat);
    result=true;
  } else if (this->IsOfType<charSSAlgMod<Rational> >())
  { charSSAlgMod<Rational> theElt=this->GetValuENonConstUseWithCaution<charSSAlgMod<Rational> >();
    out << theElt.ToString();
    result=true;
  } else if (this->IsOfType<SemisimpleSubalgebras>())
  { SemisimpleSubalgebras& theSubalgebras=
    this->GetValuENonConstUseWithCaution<SemisimpleSubalgebras>();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex=true;
    tempFormat.flagUseHTML=true;
    tempFormat.flagCandidateSubalgebraShortReportOnly=false;
    out << theSubalgebras.ToString(&tempFormat);
    result=true;
  } else if (this->IsOfType<double>())
  { out << this->GetValuENonConstUseWithCaution<double>();
    result=true;
  } else if (this->IsOfType<LittelmannPath>())
  { out << this->GetValuENonConstUseWithCaution<LittelmannPath>().ToString()  ;
    result=true;
  } else if (this->IsOfType<Matrix<RationalFunctionOld> >())
  { Expression contextE=this->GetContext();
    FormatExpressions tempFormat;
    contextE.ContextGetFormatExpressions(tempFormat);
    tempFormat.flagUseHTML=false;
    tempFormat.flagUseLatex=true;
    out << this->GetValuENonConstUseWithCaution<Matrix<RationalFunctionOld> >().ToString(&tempFormat)  ;
    result=true;
  }
  output=out.str();
  return result;
}

std::string Expression::ToStringFull()const
{ std::stringstream out;
  if (this->IsAtoM())
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

std::string Expression::ToString
(FormatExpressions* theFormat, Expression* startingExpression)const
{ MacroRegisterFunctionWithName("Expression::ToString");
  if (this->theBoss!=0)
  { if (this->theBoss->RecursionDeptH+1>this->theBoss->MaxRecursionDeptH)
      return "(...)";
  } else
    return "(Error:NoOwner)";
  RecursionDepthCounter theRecursionCounter(&this->theBoss->RecursionDeptH);

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
  int lineBreak=50;
  int charCounter=0;
  std::string tempS;
  if (this->ToStringData(tempS, theFormat, startingExpression))
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
    out << (*this)[1].ToString(theFormat)
    << ":=:" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opDefineConditional()))
    out <<  (*this)[1].ToString(theFormat) << " :if "
    << (*this)[2].ToString(theFormat)
    << ":=" << (*this)[3].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDivide(), 3))
  { std::string firstE= (*this)[1].ToString(theFormat);
    std::string secondE=(*this)[2].ToString(theFormat);
    bool firstNeedsBrackets=
    !((*this)[1].IsListStartingWithAtom(this->theBoss->opTimes())||
      (*this)[1].IsListStartingWithAtom(this->theBoss->opDivide()));
    bool secondNeedsBrackets=true;
    if ((*this)[2].IsOfType<Rational>())
      if ((*this)[2].GetValuE<Rational>().IsInteger())
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
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTensor(),3) )
  { out << (*this)[1].ToString(theFormat)
    << "\\otimes " << (*this)[2].ToString(theFormat);
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTimes(), 3))
  { std::string firstE= (*this)[1].ToString(theFormat);
    std::string secondE=(*this)[2].ToString(theFormat);
    bool firstNeedsBrackets=
    (!((*this)[1].IsListStartingWithAtom(this->theBoss->opTimes())||
      (*this)[1].IsListStartingWithAtom(this->theBoss->opDivide())))
    && !(*this)[1].IsOfType<Rational>() && !(*this)[1].IsOfType<double>();
    bool secondNeedsBrackets=true;
    if ((*this)[2].IsOfType<Rational>())
      secondNeedsBrackets=(*this)[2].GetValuE<Rational>().IsNonPositive();
    if (firstE=="-1" )
      firstE="-";
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
  } else if (this->IsListStartingWithAtom(this->theBoss->opThePower()))
  { std::string firstE=(*this)[1].ToString(theFormat);
    std::string secondE=(*this)[2].ToString(theFormat);
    if ((*this)[1].IsOfType<Rational>())
      out << firstE;
    else
      out << "(" << firstE << ")";
    out << "^{" << secondE << "}";
  } else if (this->IsListStartingWithAtom(this->theBoss->opPlus() ))
  { assert(this->children.size>=2);
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
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 3))
  { if (!(this->children.size==3))
    { std::cout << "This is a programming error: the minus function expects"
      << "1 or 2 arguments, instead there are " << this->children.size-1
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    out << (*this)[1].ToString(theFormat)
    << "-" << (*this)[2].ToString(theFormat);
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opBind(), 2))
    out << "{{" << (*this)[1].ToString(theFormat) << "}}";
  else if (this->IsListStartingWithAtom(this->theBoss->opApplyFunction()))
  { assert(this->children.size>=2);
    switch(this->format)
    { case Expression::formatFunctionUseUnderscore:
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=false;
        out <<  (*this)[1].ToString(theFormat)
        << "_{" << (*this)[2].ToString(theFormat) << "}";
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=true;
        break;
      case Expression::formatFunctionUseCdot:
        out <<  (*this)[1].ToString
        (theFormat)
        << "\\cdot(" << (*this)[1].ToString(theFormat) << ")";
        break;
      default:
        out << (*this)[1].ToString(theFormat)
        << "{}(" << (*this)[2].ToString(theFormat) << ")";
        break;
    }
  } else if (this->IsListStartingWithAtom(this->theBoss->opEqualEqual()))
    out << (*this)[1].ToString(theFormat)
    << "==" << (*this)[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opSequence()))
  { if (this->IsSequenceNElementS(1))
      out << "{Sequence{}";
    switch (this->format)
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
        break;
    }
    if (this->IsSequenceNElementS(1))
      out << "}";
  }
  else if (this->IsListStartingWithAtom(this->theBoss->opLieBracket()))
    out << "[" << (*this)[1].ToString(theFormat)
    << "," << (*this)[2].ToString(theFormat)
    << "]";
  else if (this->IsListStartingWithAtom(this->theBoss->opUnion()))
    out << (*this)[1].ToString(theFormat)
    << "\\cup " << (*this)[2].ToString(theFormat)
    ;
  else if (this->IsListStartingWithAtom(this->theBoss->opUnionNoRepetition()))
    out << (*this)[1].ToString(theFormat)
    << "\\sqcup " << (*this)[2].ToString(theFormat)
    ;
  else if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
  { if (startingExpression==0)
      out << "<table>";
    for (int i=1; i<this->children.size; i++)
    { out << "<tr><td valign=\"top\">";
      bool createTable=(startingExpression!=0);
      if (createTable)
        createTable=
        (startingExpression->IsListStartingWithAtom(this->theBoss->opEndStatement()));
      if (createTable)
      { out << "<hr> ";
        if (!this->theBoss->flagHideLHS)
        { if (i<(*startingExpression).children.size)
            out << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL
            ((*startingExpression)[i].ToString(theFormat));
          else
            out << "No matching starting expression- possible use of the Melt keyword.";
        } else
          out << "...";
        if (i!=this->children.size-1)
          out << ";";
        out << "</td><td valign=\"top\"><hr>";
        if ((*this)[i].IsOfType<std::string>() && isFinal)
          out << (*this)[i].GetValuE<std::string>();
        else if (((*this)[i].IsOfType<CalculusFunctionPlot> () ||
                  (*this)[i].IsOfType<SemisimpleSubalgebras>() ||
                  (*this)[i].IsOfType<WeylGroup>() ||
                  (*this)[i].IsOfType<WeylGroupRepresentation<Rational> >()
                  )
                 && isFinal)
          out << (*this)[i].ToString(theFormat);
        else
          out << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL
          ((*this)[i].ToString(theFormat));
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
    this->theBoss->Comments << "<br>Error " << this->theBoss->NumErrors
    << ". " << (*this)[1].ToString(theFormat);
  } else if (this->children.size==1)
    out << (*this)[0].ToString(theFormat);
  else if (this->children.size>=2)
  { out << (*this)[0].ToString(theFormat);
    if (this->format==this->formatFunctionUseUnderscore)
      out << "_";
    else if (this->format==this->formatFunctionUseCdot)
      out << "\\cdot";
    else
      out << "{}";
    if(this->format== this->formatFunctionUseUnderscore)
      out << "{";
    else
      out << "(";
    for (int i=1; i<this->children.size; i++)
    { out << (*this)[i].ToString(theFormat);
      if (i!=this->children.size-1)
        out << ", ";
    }
    if(this->format== this->formatFunctionUseUnderscore)
      out << "}";
    else
      out << ")";
  } else //<-not sure if this case is possible
    out << "(ProgrammingError:NotDocumented)" ;
  if (startingExpression!=0)
  { std::stringstream outTrue;
    outTrue << "<table>";
    outTrue << "<tr><th>Input in LaTeX</th><th>Result in LaTeX</th></tr>";
    outTrue << "<tr><td colspan=\"2\">Double click LaTeX image to get the LaTeX code. "
    << "Javascript LaTeXing courtesy of <a href=\"http://www.math.union.edu/~dpvc/jsmath/\">jsmath</a>: "
    << "many thanks for your great work!</td></tr>";
   // std::cout << this->Lispify();
    if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
      outTrue << out.str();
    else
    { outTrue << "<tr><td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(startingExpression->ToString(theFormat));
      if ((this->IsOfType<std::string>() ||
           this->IsOfType<CalculusFunctionPlot>() ||
           this->IsOfType<SemisimpleSubalgebras>() ||
           this->IsOfType<WeylGroup>()
           ) && isFinal)
        outTrue << "</td><td>" << out.str() << "</td></tr>";
      else
        outTrue << "</td><td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(out.str())
        << "</td></tr>";
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
  { std::cout << "This is a programming error. "
    << "List expressions must have data valule equal to  CommandList::opList(). "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return true;
}

bool Expression::IsBuiltInOperation(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (this->theData>= this->theBoss->NumPredefinedVars)
    return false;
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  return true;
}

bool Expression::IsOperation(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->GetOperations().size)
    return false;
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->GetOperations()[this->theData];
  return true;
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
  if (this->children.size<2)
    return false;
  //std::string debugString=(*this)[1].ToString();
  //std::cout << "<br>Trying to fetch context from: " << debugString ;
  return (*this)[1].IsListStartingWithAtom(this->theBoss->opContext());
}

bool Expression::HasNonEmptyContext()const
{ if (!this->HasContext())
    return false;
  return !this->GetContext().IsListNElementsStartingWithAtom(this->theBoss->opContext(), 1);
}

bool Expression::IsBuiltInType(std::string* outputWhichOperation)const
{ std::string tempS;
  if (!this->IsListNElementsStartingWithAtom())
    return false;
  if (this->children.size<2 || !(*this)[this->children.size-1].IsAtoM())
    return false;
  if (!(*this)[0].IsOperation(&tempS))
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

int CommandList::AddOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName)
{ int result=this->operations.GetIndex(theOpName);
  if (result==-1)
  { this->operations.AddOnTop(theOpName);
    this->FunctionHandlers.SetSize(this->operations.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
    result=this->operations.size-1;
  }
  return result;
}

void CommandList::AddOperationBuiltInType(const std::string& theOpName)
{ this->AddOperationNoRepetitionAllowed(theOpName);
  this->builtInTypes.AddOnTop(theOpName);
}

void CommandList::AddOperationNoRepetitionAllowed(const std::string& theOpName)
{ if (this->GetOperations().Contains(theOpName))
  { std::cout << "This is a programming error: operation " << theOpName << " already created. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->operations.AddOnTop(theOpName);
  this->FunctionHandlers.SetSize(this->operations.size);
  this->FunctionHandlers.LastObject()->SetSize(0);
}

void CommandList::AddOperationBinaryInnerHandlerWithTypes
  (const std::string& theOpName, Expression::FunctionAddress innerHandler,
   int leftType, int rightType,
   const std::string& opDescription, const std::string& opExample,
   bool visible, bool experimental)
{ int indexOp=this->operations.GetIndex(theOpName);
  if (indexOp==-1)
  { this->operations.AddOnTop(theOpName);
    indexOp=this->operations.size-1;
    this->FunctionHandlers.SetSize(this->operations.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  Function innerFunction
  (innerHandler, 0, opDescription, opExample, true, visible, experimental, true)
  ;
  innerFunction.theArgumentTypes.reset(*this, 2);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(leftType);
  innerFunction.theArgumentTypes.AddChildAtomOnTop(rightType);
  this->FunctionHandlers[indexOp].ReservE(10);
  this->FunctionHandlers[indexOp].AddOnTop(innerFunction);
}

void CommandList::AddOperationHandler
  (const std::string& theOpName, Expression::FunctionAddress handler,
   const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample,
   bool isInner, bool visible, bool isExperimental)
{ int indexOp=this->operations.GetIndex(theOpName);
  if (indexOp==-1)
  { this->operations.AddOnTop(theOpName);
    indexOp=this->operations.size-1;
    this->FunctionHandlers.SetSize(this->operations.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  if (opArgumentListIgnoredForTheTimeBeing!="")
  { std::cout << "This section of code is not implemented yet. Crashing to let you know. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Function theFun
  (handler, 0, opDescription, opExample, isInner, visible, isExperimental)
  ;
  if (theOpName=="*" || theOpName=="+" || theOpName=="/")
    this->FunctionHandlers[indexOp].ReservE(35);
  else
    this->FunctionHandlers[indexOp].ReservE(5);
  this->FunctionHandlers[indexOp].AddOnTop(theFun);
}

std::string CommandList::ElementToStringNonBoundVars()
{ std::stringstream out;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  out << "<br>\n" << this->operations.size << " operations and global variables " << " (= "
  << this->NumPredefinedVars  << " predefined + "
  << this->operations.size-this->NumPredefinedVars
  << " user-defined). <br>Predefined: \n<br>\n";
  for (int i=0; i<this->operations.size; i++)
  { out << openTag1 << this->operations[i] << closeTag1;
    if (this->FunctionHandlers[i].size>0)
    { out << " [handled by: ";
      for (int j=0; j<this->FunctionHandlers[i].size; j++)
      { out << this->FunctionHandlers[i][j].theFunction;
        if (this->FunctionHandlers[i][j].flagIsInner)
          out << "(inner)";
        else
          out << "(outer)";
        if (j!=this->FunctionHandlers[i].size-1)
          out << ", ";
      }
      out << "]";
    }
    if (i!=this->operations.size-1)
    { out << ", ";
      if (i==this->NumPredefinedVars-1 )
        out << "<br>user-defined:\n<br>\n";
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
  return
  input[1].IsListStartingWithAtom(this->theArgumentTypes[0].theData) &&
  input[2].IsListStartingWithAtom(this->theArgumentTypes[1].theData);
}

std::string Function::GetString(CommandList& theBoss)
{ if (!this->flagIamVisible)
    return "";
  std::stringstream out2;
  if (!this->flagIsExperimental)
  { std::stringstream out;
    out << this->theDescription;
//    out << " \nFunction memory address: " << std::hex << (int) this->theFunction << ". ";
    // use of unsigned long is correct on i386 and amd64
    // uintptr_t is only available in c++0x
    out << "Function memory address: " << std::hex << (unsigned long) this->theFunction << ". ";
    if (!this->flagIsInner)
      out << "This is a <b>``law''</b> - takes as argument the name of the operation as well. ";
    if (this->theExample!="")
      out << " <br> " << this->theExample << "&nbsp&nbsp&nbsp";
    out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());
    if (this->theExample!="")
      out2 << "<a href=\""
      << theBoss.DisplayNameCalculator  << "?"
      << " textType=Calculator&textDim=1&textInput="
      << CGI::UnCivilizeStringCGI(this->theExample)
      << "\"> " << " Example" << "</a>" ;
  } else
    out2 << "<b>Experimental, please don't use.</b>";
  return out2.str();
}

std::string CommandList::ToStringFunctionHandlers()
{ std::stringstream out;
  int numOpsHandled=0;
  int numHandlers=0;
  int numInnerHandlers=0;
  for (int i=0; i<this->operations.size; i++)
  { if (this->FunctionHandlers[i].size!=0)
      numOpsHandled++;
    numHandlers+=this->FunctionHandlers[i].size;
    for (int j=0; j<this->FunctionHandlers[i].size; j++)
      if (this->FunctionHandlers[i][j].flagIsInner)
        numInnerHandlers++;
  }
  out << "\n <b> " << numOpsHandled << "  operations handled, "
  << "by a total of " << numHandlers << " handler functions ("
  << numInnerHandlers << " inner and " << numHandlers-numInnerHandlers << " outer)."
  << "</b><br>\n";
  bool found=false;
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  for (int i=0; i<this->operations.size; i++)
    if (this->FunctionHandlers[i].size>0)
      for (int j=0; j<this->FunctionHandlers[i].size; j++)
        if (this->FunctionHandlers[i][j].flagIamVisible)
        { if (found)
            out << "<br>\n";
          found=true;
          out << openTag2 << this->operations[i] << closeTag2;
          if (this->FunctionHandlers[i].size>1)
            out << " (" << j+1 << " out of " << this->FunctionHandlers[i].size << ")";
          out << "\n" << this->FunctionHandlers[i][j].GetString(*this);
        }
  return out.str();
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
    { out << " Type " << this->theSSsubalgebras[i].owneR->GetLieAlgebraName()
      << " with " << this->theSSsubalgebras[i].Hcandidates.size << " candidates";
      if (i!=this->theLieAlgebras.size-1)
        out << ", ";
    }
  }
  return out.str();
}

std::string CommandList::ToString()
{ std::stringstream out, out2;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  std::string openTag3="<span style=\"color:#00FF00\">";
  std::string closeTag3="</span>";
  out2 << " Total number of pattern matches performed: " << this->TotalNumPatternMatchedPerformed << "";
  double elapsedSecs=this->theGlobalVariableS->GetElapsedSeconds();
  out2 << "<br>Computation time: "
  << elapsedSecs << " seconds (" << elapsedSecs*1000 << " milliseconds).<br>";
  std::stringstream tempStreamTime;
  tempStreamTime << " Of them "
  << this->StartTimeEvaluationInSecondS
  << " seconds (" << this->StartTimeEvaluationInSecondS*1000 << " millisecond(s)) boot + "
  << elapsedSecs-this->StartTimeEvaluationInSecondS << " ("
  << (elapsedSecs-this->StartTimeEvaluationInSecondS)*1000 << " milliseconds) user computation.<br>"
  << "Boot time is measured from start of main() until evaluation start and excludes static initializations "
  << "+ executable load. "
  << "Computation time excludes the time needed to compute the strings that follow below (which might take a while).";
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(tempStreamTime.str());
  out2 << "<br>Maximum computation time: "
  << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2
  << " seconds. ";
  if (this->DepthRecursionReached>0)
    out2 << "<br>Maximum recursion depth reached: " << this->DepthRecursionReached << ".";
  #ifdef MacroIncrementCounter
  out2 << "<br>Number of Lists created: " << NumListsCreated
  << "<br> Number of List resizes: " << NumListResizesTotal
  << "<br> Number HashedList hash resizing: " << NumHashResizes;
  if (Rational::TotalSmallAdditions>0)
    out2 << "<br>Number small rational number additions: " << Rational::TotalSmallAdditions
    << " (# successful calls Rational::TryToAddQuickly)";
  if (Rational::TotalSmallMultiplications>0)
    out2 << "<br>Number small rational number multiplications: "
    << Rational::TotalSmallMultiplications
    << " (# successful calls Rational::TryToMultiplyQuickly)";
  if (Rational::TotalSmallGCDcalls>0)
    out2 << "<br>Number small number gcd calls: " << Rational::TotalSmallGCDcalls
    << " (# calls of Rational::gcd)";
  if (Rational::TotalLargeAdditions>0)
    out2 << "<br>Number large integer additions: " << Rational::TotalLargeAdditions
    << " (# calls LargeIntUnsigned::AddNoFitSize)";
  if (Rational::TotalLargeMultiplications>0)
    out2 << "<br>Number large integer multiplications: " << Rational::TotalLargeMultiplications
    << " (# calls LargeIntUnsigned::MultiplyBy)";
  if (Rational::TotalLargeGCDcalls>0)
    out2 << "<br>Number large number gcd calls: " << Rational::TotalLargeGCDcalls
    << " (# calls LargeIntUnsigned::gcd)";
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
  out << "<br><b>Object container information</b>."
  << "The object container is the data structure storing all c++ built-in data types "
  << " requested by the user<br> "
  << this->theObjectContainer.ToString();
  out << "<hr>Control sequences (" << this->controlSequences.size << " total):\n<br>\n";
  for (int i=0; i<this->controlSequences.size; i++)
  { out << openTag1 << this->controlSequences[i] << closeTag1;
    if (i!=this->controlSequences.size)
      out << ", ";
  }
  out << "<br>\n Variables (" << this->operations.size << " = "
  << this->NumPredefinedVars << " predefined + " << this->operations.size-this->NumPredefinedVars << " user-defined):<br>\n";
  for (int i=0; i<this->operations.size; i++)
  { out << "\n" << i << ": " << openTag1 << this->operations[i] << closeTag1;
    if(i!=this->operations.size-1)
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
  out << "\n Cached expressions (" << this->cachedExpressions.size
  << " total):\n<br>\n";
  for (int i=0; i<this->cachedExpressions.size; i++)
  { out << this->cachedExpressions[i].ToString()
    << " -> " << this->imagesCachedExpressions[i].ToString();
    if (i!=this->cachedExpressions.size-1)
      out << "<br>";
  }
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());//, "Expand/collapse.");
  return out2.str();
}

bool CommandList::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Rational tempRat=left.theData.GetValuE<Rational>();
  if (this->registerPositionAfterDecimalPoint==0)
  { tempRat*=10;
    tempRat+=right.theData.GetValuE<Rational>();
  } else
  { Rational afterDecimal=right.theData.GetValuE<Rational>();
    for (int i=0; i<this->registerPositionAfterDecimalPoint; i++)
      afterDecimal/=10;
    tempRat+=afterDecimal;
    this->registerPositionAfterDecimalPoint++;
  }
  left.theData.AssignValue(tempRat, *this);
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool CommandList::ReplaceXEXEXByEusingO(int theControlIndex, int formatOptions)
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

bool CommandList::ReplaceEXdotsXbySsXdotsX(int numDots)
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

bool CommandList::ReplaceSsSsXdotsXbySsXdotsX(int numDots)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numDots-1];
  if (!left.theData.IsListNElementsStartingWithAtom(this->opEndStatement()))
  { std::cout << "This is a programming error: ReplaceSsSsXdotsXbySsXdotsX called but left expression "
    << " is not EndStatement." << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  left.theData.children.ReservE(left.theData.children.size+ right.theData.children.size-1);
  for (int i=1; i<right.theData.children.size; i++)
    left.theData.AddChildOnTop(right.theData[i]);
  left.theData.format=Expression::formatDefault;
  left.controlIndex=this->conSequenceStatements();
  (*this->CurrentSyntacticStacK).PopIndexShiftDown((*this->CurrentSyntacticStacK).size-numDots-1);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXEByEusingO(int theControlIndex, int formatOptions)
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

bool CommandList::ReplaceEXEXByEX(int formatOptions)
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

std::string CommandList::ElementToStringSyntacticStack()
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

bool CommandList::ReplaceXXXByCon(int theCon)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool CommandList::ReplaceXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(1);
  return true;
}

bool CommandList::ReplaceXXXXXByCon(int theCon, int inputFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=theCon;
  this->DecreaseStackSetCharacterRangeS(4);
  return true;
}

bool CommandList::ReplaceXXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool CommandList::ReplaceXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(2);
  return true;
}

bool CommandList::ReplaceXXXXByCon(int con1, int inputFormat1)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRangeS(3);
  return true;
}

bool CommandList::ReplaceXByCon(int theCon, int theFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].controlIndex=theCon;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1].theData.format=theFormat;
  //    this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool CommandList::ReplaceXByConCon
(int con1, int con2, int format1, int format2)
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
{ this->theTensorElts.Clear();
  this->theCategoryOmodules.SetSize(0);
//  this->theCategoryOmodulesContexts.SetSize();
  this->theLieAlgebras.SetSize(0);
  this->thePolys.Clear();
  this->theRFs.Clear();
  this->theLSpaths.Clear();
  this->theLittelmannOperators.Clear();
}

bool Expression::ContextGetPolySubFromSuperContext
  (const Expression& largerContext, PolynomialSubstitution<Rational>& output)const
{ PolynomialSubstitution<Rational> theSub;
  Expression polyVarsElargerContext=largerContext.ContextGetPolynomialVariables();
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

bool CommandList::IsBoundVarInContext(int inputOp)
{ for (int i=0; i<this->BoundVariablesStack.size; i++)
    if (this->BoundVariablesStack[i].Contains(inputOp))
      return true;
  return false;
}

bool CommandList::IsNonBoundVarInContext(int inputOp)
{ for (int i=0; i<this->NonBoundVariablesStack.size; i++)
    if (this->NonBoundVariablesStack[i].Contains(inputOp))
      return true;
  return false;
}

bool CommandList::ReplaceXXVXdotsXbyE_BOUND_XdotsX(int numXs)
{ SyntacticElement& theElt=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  int theBoundVar=theElt.theData.theData;
//  std::cout << "<br>Registering bound variable index: " << theBoundVar;
  if (this->IsNonBoundVarInContext(theBoundVar))
  { std::stringstream out;
    out << "Syntax error. In the same syntactic scope, the string "
    << this->operations[theBoundVar]
    << " is first used to denote a non-bound variable"
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

bool CommandList::ReplaceVXdotsXbyE_NONBOUND_XdotsX(int numXs)
{ SyntacticElement& theElt=
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1-numXs];
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

void CommandList::InitJavaScriptDisplayIndicator()
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

void CommandList::initDefaultFolderAndFileNames
  (const std::string& inputPathBinaryBaseIsFolderBelow, const std::string& inputDisplayPathBase, const std::string& scrambledIP)
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

bool CommandList::innerWriteGenVermaModAsDiffOperators
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerWriteGenVermaModAsDiffOperators");
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
  if (!theCommands.innerGetTypeHighestWeightParabolic<Polynomial<Rational> >
      (theCommands, truncatedInput, output, theHWs[0], theParSel, theContext, theSSalgebra,
       Serialization::innerPolynomial))
    return output.SetError
    ("Failed to extract type, highest weight, parabolic selection", theCommands);
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
  (theCommands, input, output, theHWs, theContext, theParSel, theSSalgebra, &letterString,
   &partialString, &exponentLetterString);
}

bool CommandList::innerFreudenthalFull
(CommandList& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.innerGetTypeHighestWeightParabolic<Rational>
      (theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeFullCharacter
      (resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return output.SetError(reportString, theCommands);
  std::stringstream out;
  out << resultChar.ToString();
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerFreudenthalEval
(CommandList& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.innerGetTypeHighestWeightParabolic<Rational>
      (theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight(hwSimple, theSSalg);
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeDominantWeightsOnly
      (resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return output.SetError(reportString, theCommands);
  return output.AssignValue(resultChar, theCommands);
}

bool CommandList::fElementSSAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  if (!input[1].IsOfType<SemisimpleLieAlgebra>())
    return false;
  SemisimpleLieAlgebra& ownerAlgebra=input[1].GetValuENonConstUseWithCaution<SemisimpleLieAlgebra>();
  int theDisplayIndex;
  if (!input[2].IsSmallInteger(&theDisplayIndex))
  { bool isGood=
    (theDisplayIndex>0 && theDisplayIndex<= ownerAlgebra.GetNumPosRoots()) ||
    (theDisplayIndex<0 && theDisplayIndex>=- ownerAlgebra.GetNumPosRoots());
    if (!isGood)
    { theCommands.Comments
      << "<b>Error</b>. You requested element of index " << theDisplayIndex
      << " of semisimple Lie algebra " << ownerAlgebra.GetLieAlgebraName()
      << ". The index of the Vector<Rational> space must be a non-zero integer "
      << " of absolute value between 1 and the number of positive roots. "
      << "The number of positive roots for the current semisimple Lie algebra is "
      << ownerAlgebra.GetNumPosRoots()
      << ". If you want to request an element of the Cartan, you should use two indices, "
      << "the first of which is zero. For example,"
      << " ElementSemisimpleAlgebra{}(0,1) gives you the an element of the Cartan "
      << "corresponding to the first simple Vector<Rational>. "
      ;
      return false;
    }
    ElementSemisimpleLieAlgebra<Rational> theElt;
    int actualIndex=theDisplayIndex;
    if (actualIndex<0)
      actualIndex+=ownerAlgebra.GetNumPosRoots();
    else
      actualIndex+=ownerAlgebra.GetNumPosRoots()+ownerAlgebra.GetRank()-1;
    theElt.MakeGenerator(actualIndex, ownerAlgebra);
    RationalFunctionOld rfOne, rfZero;
    rfOne.MakeOne(theCommands.theGlobalVariableS);
    rfZero.MakeZero(theCommands.theGlobalVariableS);
    ElementUniversalEnveloping<RationalFunctionOld> tempUE;
    //  std::cout << "tempelt: " << tempElt.ToString();
    tempUE.AssignElementLieAlgebra(theElt, ownerAlgebra, rfOne, rfZero);
    return output.AssignValue(tempUE, theCommands);
  }
  if (!input[2].IsSequenceNElementS(2))
    return false;
  int index1, indexInCartan;
  if (input[2][1].IsSmallInteger(&index1) || input[2][2].IsSmallInteger(&indexInCartan))
    return false;
  bool isGood=(index1==0 && indexInCartan <=ownerAlgebra.GetRank() && indexInCartan>0);
  if (!isGood)
  { theCommands.Comments
    << "You requested element of the Cartan subalgebra labeled by (" << index1
    << ", " << indexInCartan
    << " of semisimple Lie algebra " << ownerAlgebra.GetLieAlgebraName()
    << "). For your request to succeed, the first index must be zero and the second must be an integer"
    << " between 1 and the rank of the Algebra which is " << ownerAlgebra.GetRank()
    << ". If you want to request an element that is in a root space outside of the Cartan, you should only one index."
    ;
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  int actualIndeX=indexInCartan-1+ownerAlgebra.GetNumPosRoots();
  tempElt.MakeGenerator(actualIndeX, ownerAlgebra);
  RationalFunctionOld rfOne, rfZero;
  rfOne.MakeOne(theCommands.theGlobalVariableS);
  rfZero.MakeZero(theCommands.theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunctionOld> tempUE;
  tempUE.AssignElementLieAlgebra(tempElt, ownerAlgebra, rfOne, rfZero);
  return output.AssignValue(tempUE, theCommands);
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

bool CommandList::outerMeltBrackets
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerMeltBrackets");
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
