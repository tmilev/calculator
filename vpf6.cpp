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
int Expression::GetTypeOperation<int>()const
{ this->CheckInitialization();
  return this->theBoss->opRational();
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

//Expression::GetTypeOperation specializations end.

//Expression::AddObjectReturnIndex specializations follow
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
  return this->theBoss->theObjectContainer.theChars
  .AddNoRepetitionOrReturnIndexFirst(inputValue);
}

//Expression::AddObjectReturnIndex specializations end

//start Expression::GetValuENonConstUseWithCaution specializations.
template < >
Rational& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<Rational>())
  { std::cout << "This is a programming error: expression not of required type Rational."
    << ". Please debug file " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theRationals[this->children.LastObject()->theData];
}

template < >
std::string& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<std::string>())
  { std::cout << "This is a programming error: expression not of required type std::string. "
    <<  CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theStrings[this->children.LastObject()->theData];
}

template < >
RationalFunctionOld& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<RationalFunctionOld>())
  { std::cout << "This is a programming error: expression not of required type RationalFunctionOld."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theRFs[this->children.LastObject()->theData];
}

template < >
ElementUniversalEnveloping<RationalFunctionOld>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { std::cout << "This is a programming error: expression not of required"
    << " type ElementUniversalEnveloping_RationalFunctionOld."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theUEs[this->children.LastObject()->theData];
}

template < >
Polynomial<Rational>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<Polynomial<Rational> >())
  { std::cout << "This is a programming error: expression not of required type Polynomial_Rational."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.thePolys[this->children.LastObject()->theData];
}

template < >
LittelmannPath& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<LittelmannPath>())
  { std::cout << "This is a programming error: expression not of required type LittelmannPath."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theLSpaths[this->children.LastObject()->theData];
}

template < >
MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >())
  { std::cout << "This is a programming error: expression not of required type MonomialTensor_int."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->theBoss->theObjectContainer.theLittelmannOperators[this->children.LastObject()->theData];
}

template < >
ElementTensorsGeneralizedVermas<RationalFunctionOld>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { std::cout << "This is a programming error: expression not of required type ElementTensorsGeneralizedVermas_RationalFunction."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theTensorElts[this->children.LastObject()->theData];
}

template < >
charSSAlgMod<Rational>& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<charSSAlgMod<Rational> >())
  { std::cout << "This is a programming error: expression not of required type charSSAlgMod_Rational. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theChars[this->children.LastObject()->theData];
}

template < >
AlgebraicNumber& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<AlgebraicNumber>())
  { std::cout << "This is a programming error: expression not of required type AlgebraicNumber. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theAlgebraicNumbers[this->children.LastObject()->theData];
}

template < >
SemisimpleLieAlgebra& Expression::GetValuENonConstUseWithCaution()const
{ if (!this->IsOfType<SemisimpleLieAlgebra>())
  { std::cout << "This is a programming error: expression not of required type AlgebraicNumber. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theObjectContainer.theLieAlgebras[this->children.LastObject()->theData];
}

//end Expression::GetValuENonConstUseWithCaution specializations.
//start Expression::ContextGetPolynomialMonomial specializations.
template< >
bool Expression::ContextGetPolynomialMonomial
(const Expression& input, Polynomial<Rational>& output, GlobalVariables& theGlobalVariables)const
{ int theIndex= this->ContextGetPolynomialVariables().children.GetIndex(input);
  if (theIndex==-1)
  { std::cout << "This is a programming error: Expression::ContextGetPolynomialMonomial is called on "
    << " expression " << input.ToString() << " but "
    << " the context does not contain that expression. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    return false;
  }
  output.MakeMonomiaL(theIndex-1, 1, 1);
  return true;
}

template< >
bool Expression::ContextGetPolynomialMonomial
(const Expression& input, RationalFunctionOld& output, GlobalVariables& theGlobalVariables)const
{ int theIndex= this->ContextGetPolynomialVariables().children.GetIndex(input);
  if (theIndex==-1)
  { std::cout << "This is a programming error: Expression::ContextGetPolynomialMonomial is called on "
    << " expression " << input.ToString() << " but "
    << " the context does not contain that expression. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    return false;
  }
  output.MakeOneLetterMoN(theIndex-1, 1, theGlobalVariables);
  return true;
}

template< >
bool Expression::ContextGetPolynomialMonomial
(const Expression& input, ElementUniversalEnveloping<RationalFunctionOld>& output,
 GlobalVariables& theGlobalVariables)const
{ this->CheckInitialization();
  int theVarIndex= this->ContextGetPolynomialVariables().children.GetIndex(input);
  int algebraIndex=this->ContextGetIndexAmbientSSalg();
  if (theVarIndex==-1 || algebraIndex==-1)
  { std::cout
    << "This is a programming error:  Expression::ContextGetPolynomialMonomial "
    << "should never fail. Something is wrong with the context. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    return false;
  }
  Polynomial<Rational> tempP;
  tempP.MakeMonomiaL(theVarIndex-1, 1, 1);
  RationalFunctionOld tempMon;
  tempMon=tempP;
  output.MakeConst(tempMon, this->theBoss->theObjectContainer.theLieAlgebras, algebraIndex);
  return true;
}
//end Expression::ContextGetPolynomialMonomial specializations.

bool Expression::IsListNElementsStartingWithAtom(int theOp, int N)const
{ if (N!=-1)
  { if (this->children.size!=N)
      return false;
  } else
    if (this->children.size==0)
      return false;
  if (!this->children[0].IsAtoM())
    return false;
  if (theOp==-1)
    return true;
  return this->children[0].theData==theOp;
}

int Expression::ContextGetNumContextVariables()const
{ if (this->theBoss==0)
    return 0;
  if (!this->IsListNElementsStartingWithAtom(this->theBoss->opContext()))
    return 0;
  for (int i=1; i<this->children.size; i++)
    if (this->children[i].IsListNElementsStartingWithAtom
        (this->theBoss->opPolynomialVariables()))
      return this->children[i].children.size-1;
  return 0;
}

bool Expression::SetContextAtLeastEqualTo(Expression& inputOutputMinContext)
{ if (!inputOutputMinContext.IsContext())
    return false;
  if (this->IsOfType<Rational>())
    return true;
  Expression newContext;
  Expression myOldContext=this->GetContext();
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
  if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> > ())
  { ElementUniversalEnveloping<RationalFunctionOld> newUE=
    this->GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >();
    newUE.Substitution(polySub);
    return this->AssignValueWithContext(newUE, inputOutputMinContext, *this->theBoss);
  }
  if (this->IsOfType<Polynomial<Rational> >())
  { Polynomial<Rational> newPoly=this->GetValuE<Polynomial<Rational> >();
    newPoly.Substitution(polySub);
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
    newETGV.Substitution(polySub);
    return this->AssignValueWithContext(newETGV, inputOutputMinContext, *this->theBoss);
  }
  std::cout
  << "This may or may not be a programing error. "
  << "Function Expression::SetContextAtLeastEqualTo"
  << " does not cover all expression data types. "
  << " I cannot decide at the moment whether this should be the desired behavior of this "
  << " function. Letting you know of my indecision by humbly crashing in flames. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
  assert(false);
  return false;
}

Expression Expression::ContextGetContextVariable(int variableIndex)
{ return this->ContextGetPolynomialVariables()[variableIndex+1];
}

Expression Expression::ContextGetPolynomialVariables()const
{ if (this->theBoss==0)
  { std::cout
    << "This is a programming error: calling Expression::ContextGetPolynomialVariables "
    << " is forbidden for non-initialized expressions. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Expression output;
  for (int i=1; i<this->children.size; i++)
    if (this->children[i].IsListNElementsStartingWithAtom
        (this->theBoss->opPolynomialVariables()))
    { output=this->children[i];
      break;
    }
  if (output.children.size==0)
  { output.reset(*this->theBoss, 1);
    output[0].MakeAtom(this->theBoss->opPolynomialVariables(), *this->theBoss);
  }
  return output;
}

bool Expression::ContextMergeContexts
(const Expression& leftContext, const Expression& rightContext, Expression& outputContext)
{ if (&leftContext==&outputContext || &rightContext==&outputContext)
  { std::cout
    << "This is a programming error: at least one of the inputs "
    << "coincides with the output of Expression::ContextMergeContexts."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
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
  HashedList<Expression> varUnion;
  varUnion.SetExpectedSize(leftPolyV.children.size+rightPolyV.children.size-2);
  for (int i =1; i<leftPolyV.children.size; i++)
    varUnion.AddOnTopNoRepetition(leftPolyV[i]);
  for (int i =1; i<rightPolyV.children.size; i++)
    varUnion.AddOnTopNoRepetition(rightPolyV[i]);
  varUnion.QuickSortAscending();
  CommandList& owner=*leftContext.theBoss;
  outputContext.children.ReservE(3);
  outputContext.reset(owner, 1);
  outputContext[0].MakeAtom(owner.opContext(), owner);
  if (varUnion.size>0)
  { outputContext.children.SetSize(outputContext.children.size+1);
    Expression& polyVarsE=*outputContext.children.LastObject();
    polyVarsE.children.ReservE(varUnion.size+1);
    polyVarsE.reset(owner,1);
    polyVarsE[0].MakeAtom(owner.opPolynomialVariables(), owner);
    polyVarsE.children.AddListOnTop(varUnion);
  }
  if (leftSSindex!=-1)
  { outputContext.children.SetSize(outputContext.children.size+1);
    Expression& ssAlgE=*outputContext.children.LastObject();
    ssAlgE.reset(owner, 2);
    ssAlgE[0].MakeAtom(owner.opSSLieAlg(), owner);
    ssAlgE[1].MakeAtom(leftSSindex, owner);
  }
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
  return this->children[1].IsOfType<std::string>(outputErrorMessage);
}

bool Expression::IsSequenceNElementS(int N)const
{ if (this->theBoss==0)
    return false;
  return this->IsListNElementsStartingWithAtom(this->theBoss->opSequence(), N+1);
}

void Expression::operator=(const Expression& other)
{ if (this==&other)
    return;
  this->CopyValueFromNoChildrenCopy(other);
  this->children.SetSize(other.children.size);
  if (this->children.size>0)
  { if (this->theBoss==0)
    { std::cout
      << "This is a programming error: expressions that do not have initialized boss "
      << "are not allowed to have children. "
      << "The error was noticed while trying to copy such an expression "
      << "(I am not displaying the expression as its ToString() method"
      << " cannot be computed correctly if it doesn't have a boss). "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    RecursionDepthCounter recursionCounter(&this->theBoss->RecursionDeptH);
    for (int i=0; i<other.children.size; i++)
      this->children[i]=(other.children[i]);
  }
}

bool Expression::IsEqualToOne()const
{ std::cout << "Function Expression::IsEqualToOne not implemented!!!!!"
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  assert(false);
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
    systemCommand3 << " ps2png " << fileName.str() << ".ps "
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
{ this->MakeXOX(owner, owner.opApplyFunction(), theFunction, theArgument);
  this->format=this->formatFunctionUseUnderscore;
}

void Expression::MakeEmptyContext(CommandList& owner)
{ this->reset(owner,1);
  this->children[0].MakeAtom(owner.opContext(), owner);
}

void Expression::MakeXOX
  (CommandList& owner, int theOp, const Expression& left, const Expression& right)
{ this->reset(owner);
  this->theData=owner.opLisT();
  this->children.SetSize(3);
  this->children[0].MakeAtom(theOp, owner);
  this->children[0].theData=theOp;
  this->children[1]=left;
  this->children[2]=right;
}

bool CommandList::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(4);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(opElt.controlIndex), *this);
  newExpr.format=formatOptions;
  newExpr.children[1]=(leftE.theData);
  newExpr.children[2]=(middleE.theData);
  newExpr.children[3]=(rightE.theData);
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRanges(7);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceOXXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-9];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(4);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(opElt.controlIndex), *this);
  newExpr.children[1]=(leftE.theData);
  newExpr.children[2]=(middleE.theData);
  newExpr.children[3]=(rightE.theData);
  newExpr.format= formatOptions;
  opElt.theData=newExpr;
  opElt.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRanges(8);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceOXEByE(int formatOptions)
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(2);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(left.controlIndex), *this);
  newExpr.children[1]=(right.theData);
  newExpr.format= formatOptions;
  left.theData=newExpr;
  left.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceOEByE(int formatOptions)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(2);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(middle.controlIndex), *this);
  newExpr.children[1]=(right.theData);
  newExpr.format= formatOptions;
  middle.theData=newExpr;
  middle.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRanges(1);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::isRightSeparator(char c)
{ switch(c)
  { case ' ':
    case '\n':
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
      return true;
    default:
      return false;
  }
}

bool CommandList::isLeftSeparator(char c)
{ switch(c)
  { case ' ':
    case '\n':
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
      } else if (this->isADigit(current, currentDigit))
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
}

int CommandList::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->operationS.GetIndex(this->controlSequences[controlIndex]);
}

int CommandList::GetExpressionIndex()
{ return this->controlSequences.GetIndex("Expression");
}

bool CommandList::ReplaceOXbyE(int inputFormat)
{ this->ReplaceOXbyEX(inputFormat);
  this->DecreaseStackSetCharacterRanges(1);
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

bool CommandList::ReplaceOXbyEXusingO(int theControlIndex, int inputFormat)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  theElt.theData.format=inputFormat;
  theElt.controlIndex=this->conExpression();
  return true;
}

bool CommandList::ReplaceAbyE()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
//  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
//  std::cout << "replaceAbyE: " << theElt.theData.ToString();
  return true;
}

bool CommandList::LookAheadAllowsApplyFunction(const std::string& lookAhead)
{ return lookAhead!="{" && lookAhead!="_" && lookAhead!="\\circ" && lookAhead!="{}" &&  lookAhead!="$";
}

bool CommandList::ReplaceSequenceXEYBySequenceY(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& afterleft = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  left.theData.children.AddOnTop(right.theData);
  left.theData.format=inputFormat;
  left.controlIndex=theControlIndex;
  afterleft=*this->CurrentSyntacticStacK->LastObject();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceSequenceXEBySequence(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  left.theData.children.AddOnTop(right.theData);
  left.theData.format=inputFormat;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceYXdotsXBySequenceYXdotsX(int theControlIndex, int inputFormat, int numXs)
{ SyntacticElement& main = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr[0].MakeAtom(this->opSequence(), *this);
  newExpr[1]=(main.theData);
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
  newExpr[0].MakeAtom(this->opSequence(), *this);
  newExpr[1]=(left.theData);
  newExpr[2]=(right.theData);
  newExpr.format=inputFormat;
  left.theData=newExpr;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}


bool CommandList::ReplaceEEByEusingO(int theControlIndex)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(3);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(theControlIndex), *this);
  //    newExpr.inputIndexBoundVars=
  newExpr.children[1]=(left.theData);
  newExpr.children[2]=(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(1);
  //    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXXEXEByEusingO(int theControlIndex)
{ SyntacticElement& middle = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(4);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(theControlIndex), *this);
  newExpr.children[1]=(left.theData);
  newExpr.children[2]=(middle.theData);
  newExpr.children[3]=(right.theData);
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(5);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEOEXByEX(int formatOptions)
{ SyntacticElement& middle=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this);
  newExpr.children.SetSize(3);
  newExpr.children[0].MakeAtom(this->GetOperationIndexFromControlIndex(middle.controlIndex),*this);
  newExpr.children[1]=(left.theData);
  newExpr.children[2]=(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  middle=*(*this->CurrentSyntacticStacK).LastObject();
  left.IndexLastCharPlusOne=right.IndexLastCharPlusOne;
  (*this->CurrentSyntacticStacK).SetSize((*this->CurrentSyntacticStacK).size-2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::isSeparatorFromTheLeftGeneral(const std::string& input)
{ return input=="{" || input=="(" || input=="[" || input=="," || input==":" || input==";" || input==" "
  || input=="MatrixSeparator" || input == "MatrixRowSeparator" || input=="&";
}

bool CommandList::isSeparatorFromTheRightGeneral(const std::string& input)
{ return input=="}" || input==")" || input=="]" || input=="," || input==":" || input==";" || input=="MatrixSeparator"
  || input=="MatrixRowSeparator" || input=="\\\\" || input=="\\end" || input=="&";
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
{ return input=="{" || input=="(" || input==";" || input==" ";
}

bool CommandList::isSeparatorFromTheRightForDefinition(const std::string& input)
{ return input=="}" || input==")" || input==";";
}

bool CommandList::isSeparatorFromTheLeftForStatement(const std::string& input)
{ return input=="{" || input=="(" || input==";" || input==" ";
}

bool CommandList::isSeparatorFromTheRightForStatement(const std::string& input)
{ return input=="}" || input==")" || input==";";
}

bool CommandList::LookAheadAllowsTensor(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="*" || lookAhead=="/" ||
    lookAhead=="Expression" || lookAhead==")" ||
    lookAhead=="(" || lookAhead=="[" ||
    lookAhead=="=" || lookAhead=="\\otimes" ||
//    lookAhead=="{" ||
    lookAhead=="Variable" || lookAhead=="," ||
    lookAhead==";" || lookAhead=="]" ||
    lookAhead=="}" || lookAhead==":"
    ;
}

bool CommandList::LookAheadAllowsTimes(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="*" || lookAhead=="/" ||
    lookAhead=="Expression" || lookAhead==")" ||
    lookAhead=="(" || lookAhead=="[" ||
    lookAhead=="=" ||
//    lookAhead=="{" ||
    lookAhead=="Variable" || lookAhead=="," ||
    lookAhead==";" || lookAhead=="]" ||
    lookAhead=="}" || lookAhead==":"
    ;
}

bool CommandList::LookAheadAllowsPlus(const std::string& lookAhead)
{ return
    lookAhead=="+" || lookAhead=="-" ||
    lookAhead=="," || lookAhead=="=" ||
    lookAhead==")" || lookAhead==";" ||
    lookAhead=="]" || lookAhead=="}" ||
    lookAhead==":"
    ;
}

bool CommandList::LookAheadAllowsDivide(const std::string& lookAhead)
{ return this->LookAheadAllowsTimes(lookAhead);
}

bool CommandList::ApplyOneRule(const std::string& lookAhead)
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
//  const SyntacticElement& eighthToLastE=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
//  const std::string& eighthToLastS=this->theBoss->controlSequences[eighthToLastE.controlIndex];
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
  if (lastS=="Variable" && ((lookAhead!="}" && lookAhead!=" ") || secondToLastS!="{" || thirdToLastS!="{"))
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(0);
  if ( thirdToLastS=="{" && secondToLastS=="Variable" && lastS=="}" && lookAhead!="}" && lookAhead!=" ")
    return this->ReplaceVXdotsXbyE_NONBOUND_XdotsX(1);

  if (lastS=="=" && secondToLastS=="=")
    return this->ReplaceXXByCon(this->conEqualEqual());
  if (lastS=="Integer" && secondToLastS=="Integer")
    return this->ReplaceIntIntBy10IntPlusInt();
  if (lastS=="Integer" && lookAhead!="Integer")
    return this->ReplaceAbyE();
  //there is an ambiguity on how should function application be associated
  //Which is better: x{}y{}z:= x{} (y{}z), or x{}y{}z:=(x{}y){}z ?
  //In our implementation, we choose x{}y{}z:= x{} (y{}z). Although this is slightly harder to implement,
  //it appears to be the more natural choice.
//  if (fourthToLastS=="Expression" && thirdToLastS=="{}" && secondToLastS=="Expression"
//      && this->LookAheadAllowsApplyFunction(lastS) )
//    return this->ReplaceEOEXByEX(secondToLastE.theData.format);
  if (thirdToLastS=="Expression" && secondToLastS=="{}" && lastS=="Expression" && this->LookAheadAllowsApplyFunction(lookAhead))
    return this->ReplaceEXEByE(Expression::formatDefault);
  if (thirdToLastS=="Expression" && secondToLastS=="_" && lastS=="Expression" && lookAhead!="_")
    return this->ReplaceEXEByE(Expression::formatFunctionUseUnderscore);
  //end of ambiguity.
  if (fourthToLastS=="{"  && thirdToLastS=="{}" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXYYXByYY();
  if (lastS=="Expression" && secondToLastS=="^" && thirdToLastS=="Expression" && this->LookAheadAllowsThePower(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="+" && thirdToLastS=="Expression" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="-" && thirdToLastS=="Expression" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="-" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceOEByE();
  if (lastS=="Expression" && secondToLastS=="\\otimes" && thirdToLastS=="Expression" && this->LookAheadAllowsTensor(lookAhead))
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="*" && thirdToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEOEByE(Expression::formatTimesDenotedByStar);
  if (lastS=="Expression" && secondToLastS=="/" && thirdToLastS=="Expression" && this->LookAheadAllowsDivide(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="Expression" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEEByEusingO(this->conTimes());
  if (thirdToLastS=="List" && secondToLastS=="{}" && lastS=="Expression")
    return this->ReplaceXXYBySequenceY(this->conExpression(), lastE.theData.format);
  if (thirdToLastS=="(" && secondToLastS=="Expression" && lastS==")")
    return this->ReplaceXEXByE(secondToLastE.theData.format);
  if (thirdToLastS=="{" && secondToLastS=="Expression" && lastS=="}")
    return this->ReplaceXEXByE(secondToLastE.theData.format);
  if (lastS=="Expression" && secondToLastS=="~" && thirdToLastS=="Expression" )
    return this->ReplaceEOEByE();
  if (this->isSeparatorFromTheRightGeneral(lookAhead) && lastS=="Expression" && secondToLastS=="==" && thirdToLastS=="Expression")
    return this->ReplaceEOEByE();
  if (this->isSeparatorFromTheLeftForDefinition(fourthToLastS) && thirdToLastS=="Expression" && secondToLastS==":=" && lastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lookAhead))
    return this->ReplaceEOEByE();
  if (this->isSeparatorFromTheLeftForDefinition(fourthToLastS) && thirdToLastS=="Expression" && secondToLastS==":=:" && lastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lookAhead))
    return this->ReplaceEOEByE();
  if (thirdToLastS=="Expression" && secondToLastS=="\\cup" && lastS== "Expression" && this->isSeparatorFromTheRightGeneral(lookAhead))
    return this->ReplaceEOEByE();
  if (thirdToLastS=="Expression" && secondToLastS=="\\sqcup" && lastS== "Expression" && this->isSeparatorFromTheRightGeneral(lookAhead))
    return this->ReplaceEOEByE();
  if (thirdToLastS!="[" && secondToLastS=="Expression" && lastS==",")
    return this->ReplaceYXBySequenceX(this->conSequence(), secondToLastE.theData.format);
  if (thirdToLastS=="Sequence" && secondToLastS=="," && lastS=="Expression")
    return this->ReplaceSequenceXEBySequence(this->conSequence());
  if (thirdToLastS=="(" && secondToLastS=="Sequence" && lastS==")")
    return this->ReplaceOXbyEX();
  if (thirdToLastS=="{" && secondToLastS=="Sequence" && lastS=="}")
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
  if (secondToLastS=="Expression" && lastS=="&")
    return this->ReplaceYXBySequenceX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (thirdToLastS=="MatrixRow" && secondToLastS=="&"  && lastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lookAhead))
    return this->ReplaceSequenceXEBySequence(this->conMatrixRow(), Expression::formatMatrixRow);
  if (fourthToLastS=="MatrixRow" && thirdToLastS=="&"  && secondToLastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lastS))
    return this->ReplaceSequenceXEYBySequenceY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="Expression" && (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXBySequenceX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="MatrixRow" &&  (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXBySequenceX(this->conSequenceMatrixRow(), Expression::formatMatrix);
  if (thirdToLastS=="SequenceMatrixRows" && secondToLastS=="MatrixRowSeparator" && lastS=="Expression" && this->isSeparatorFromTheRightForListMatrixRow(lookAhead) )
    return this->ReplaceYBySequenceY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (thirdToLastS=="SequenceMatrixRows" && secondToLastS=="MatrixRowSeparator" && lastS=="MatrixRow" && this->isSeparatorFromTheRightForListMatrixRow(lookAhead) )
    return this->ReplaceSequenceXEBySequence(this->conSequenceMatrixRow(), Expression::formatMatrix);
  if (thirdToLastS=="MatrixSeparator" && secondToLastS=="SequenceMatrixRows" && lastS=="MatrixSeparator")
    return this->ReplaceXOXbyEusingO(this->conSequence(), Expression::formatMatrix);
  if (fifthToLastS=="[" && fourthToLastS=="Expression" && thirdToLastS=="," && secondToLastS=="Expression" && lastS=="]")
    return this->ReplaceXEXEXByEusingO(this->conLieBracket());
  if (this->isSeparatorFromTheLeftForDefinition(seventhToLastS) && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression" && fourthToLastS=="if" && fifthToLastS==":" && sixthToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lookAhead))
    return this->ReplaceEXXEXEByEusingO(this->conDefineConditional());
  if (lastS==";")
  { this->NonBoundVariablesStack.LastObject()->Clear();
    this->BoundVariablesStack.LastObject()->Clear();
  }
  if (this->isSeparatorFromTheLeftForStatement(fourthToLastS) && thirdToLastS=="Expression" && secondToLastS==";" && lastS=="Expression" && this->isSeparatorFromTheRightForStatement(lookAhead))
    return this->ReplaceEEndCommandEbyE();
  if (lastS==";" && lookAhead==";")
    return this->DecreaseStackSetCharacterRanges(1);
  return false;
}

bool CommandList::fHWTAABF
  (CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fHWTAABF");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (input.IsListNElements(3))
    return output.SetError("Function expects three arguments.", theCommands);
  Expression leftExpression, rightExpression, finalContext;
  Expression& weightExpression=input.children[3];
  if (!theCommands.fElementUniversalEnvelopingAlgebra
      (theCommands, input.children[1], leftExpression))
    return output.SetError("Failed to convert left expression to element UE.", theCommands);
  if (!theCommands.fElementUniversalEnvelopingAlgebra
      (theCommands, input.children[2], rightExpression ))
    return output.SetError("Failed to convert left expression to element UE.", theCommands);
  if (!Expression::ContextMergeContexts
      (leftExpression.GetContext(), rightExpression.GetContext(), finalContext))
    return output.SetError
    ("Failed to merge the contexts of the two UE elements ", theCommands);
  SemisimpleLieAlgebra* constSSalg= leftExpression.GetAmbientSSAlgebraNonConstUseWithCaution();
  Vector<RationalFunctionOld> weight;
  if (!theCommands.GetVector<RationalFunctionOld>
      (weightExpression, weight, &finalContext, constSSalg->GetRank(), theCommands.innerPolynomial))
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

template<class CoefficientType>
bool CommandList::fGetTypeHighestWeightParabolic
(CommandList& theCommands, const Expression& input, Expression& output,
 Vector<CoefficientType>& outputWeightHWFundcoords, Selection& outputInducingSel,
 Expression& outputHWContext, SemisimpleLieAlgebra*& ambientSSalgebra,
 Expression::FunctionAddress ConversionFun)
{ if (!input.IsSequenceNElementS(3) && !input.IsSequenceNElementS(2))
    return output.SetError
    ("Function TypeHighestWeightParabolic is expected to have two or three arguments: \
     SS algebra type, highest weight, [optional] parabolic selection. ", theCommands);
  Expression& leftE=input[1];
  Expression& middleE=input[2];
  std::string errorString;
  if (!CommandList::CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, leftE, ambientSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  if (!theCommands.GetVector<CoefficientType>
      (middleE, outputWeightHWFundcoords, &outputHWContext, ambientSSalgebra->GetRank(),
       ConversionFun))
  { std::stringstream tempStream;
    tempStream << "Failed to convert the second argument of HWV to a list of "
    << ambientSSalgebra->GetRank() << " polynomials. The second argument you gave is "
    << middleE.ToString() << ".";
    return output.SetError(tempStream.str(), theCommands);
  }
  if (input.IsSequenceNElementS(3))
  { Vector<Rational> parabolicSel;
    Expression& rightE=input[3];
    if (!theCommands.GetVector<Rational>
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
  if (!theCommands.fGetTypeHighestWeightParabolic<RationalFunctionOld>
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
  for (int i=0; i<theWeyl.size; i++)
  { currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(i, currentHW, false, false);
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
  theChar.MakeZero(*theSSlieAlg->owner, theSSlieAlg->indexInOwner);
  FormatExpressions formatChars;
  formatChars.FDrepLetter="L";
  formatChars.fundamentalWeightLetter="\\omega";
  formatChars.flagUseLatex=true;
  for (int i=0; i<theWeylElements.size; i++)
  { ElementWeylGroup& currentElt=theWeylElements[i];
    out << "<tr><td>" << currentElt.ToString() << "</td>";
    int indexInWeyl=theKLpolys.TheWeylGroup->GetIndex(currentElt);
    if (indexInWeyl==-1)
    { std::cout << "This is a programming error. Something is wrong: I am getting that an element "
      << "of the Weyl group of the Levi part of the parabolic does not lie in the ambient Weyl group, which "
      << " is impossible. There is a bug somewhere; crashing in accordance. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    currentChar.MakeZero(*theSSlieAlg->owner, theSSlieAlg->indexInOwner);
    for (int j=0; j< theKLpolys.theKLcoeffs[indexInWeyl].size; j++)
      if (!theKLpolys.theKLcoeffs[indexInWeyl][j].IsEqualToZero())
      { currentHW=theHWsimpCoords;
//        currentHW+=theSub.GetRho();
        theWeyl.ActOn(j, currentHW, true, false);
//        currentHW-=theSub.GetRho();
        theMon.weightFundamentalCoords=theWeyl.GetFundamentalCoordinatesFromSimple(currentHW);
        int sign= (currentElt.size- theWeyl[j].size)%2==0 ? 1 :-1;
        currentChar.AddMonomial(theMon, theKLpolys.theKLcoeffs[indexInWeyl][j]*sign);
      }
    currentHW=theHWsimpCoords;
    currentHW+=theSub.GetRho();
    theWeyl.ActOn(indexInWeyl, currentHW, false, false);
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

bool CommandList::fHWV
(CommandList& theCommands, const Expression& input, Expression& output)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Expression hwContext(theCommands);
  SemisimpleLieAlgebra* theSSalgebra=0;
  if(!theCommands.fGetTypeHighestWeightParabolic
      (theCommands, input, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra,
       theCommands.innerRationalFunction) )
    return output.SetError("Failed to extract highest weight vector data", theCommands);
  else
    if (output.IsError())
      return true;
  return theCommands.fHWVinner
  (theCommands, output, theHWfundcoords, selectionParSel, hwContext, theSSalgebra->indexInOwner);
}

bool CommandList::fWriteGenVermaModAsDiffOperatorUpToLevel
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError
    ("Function fSplitGenericGenVermaTensorFD is expected to have three arguments: \
     SS algebra type, Number, List{}. ", theCommands);
  Expression& leftE=input[1];
  Expression& genVemaWeightNode=input[3];
  Expression& levelNode=input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  std::string errorString;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, leftE, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVector
      (genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, &CommandList::innerPolynomial))
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
  return theCommands.fWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHws, hwContext, selInducing,
   theSSalgebra->indexInOwner);
}

template <class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::IsNotInParabolic
(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]]<0)
      return true;
  return false;
}

template <class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::IsNotInLevi
(int theGeneratorIndex)
{ Vector<Rational> theWeight=  this->GetOwner().GetWeightOfGenerator(theGeneratorIndex);
  for (int j=0; j<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; j++)
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].IsEqualToZero())
      return true;
  return false;
}

template <class CoefficientType>
void ModuleSSalgebra<CoefficientType>::GetGenericUnMinusElt
 (bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<RationalFunctionOld>& output,
  GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnveloping<CoefficientType> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true);
  RationalFunctionOld tempRF;
  output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFunctionOld> tempMon;
  tempMon.MakeConst(*this->theAlgebras, this->indexAlgebra);
  int varShift=0;
  if (shiftPowersByNumVarsBaseField)
    varShift=this->GetNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeOneLetterMoN(i+varShift, 1, theGlobalVariables);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(&theGlobalVariables);
  output.AddMonomial(tempMon, tempRF);
}

template <class CoefficientType>
void ModuleSSalgebra<CoefficientType>::GetGenericUnMinusElt
(bool shiftPowersByNumVarsBaseField, ElementUniversalEnveloping<Polynomial<Rational> >& output, GlobalVariables& theGlobalVariables)
{ List<ElementUniversalEnveloping<CoefficientType> > eltsNilrad;
  this->GetElementsNilradical(eltsNilrad, true);
  Polynomial<Rational> tempRF;
  output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.MakeConst(*this->theAlgebras, this->indexAlgebra);
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

template <class CoefficientType>
class quasiDiffOp : public MonomialCollection<quasiDiffMon, CoefficientType>
{
public:
  static void prepareFormatFromShiftAndNumWeylVars(int theShift, int inputNumWeylVars, FormatExpressions& output);
  std::string ToString(FormatExpressions* theFormat=0)const
  ;
  void GenerateBasisLieAlgebra
  (List<quasiDiffOp<CoefficientType> >& theElts,
 FormatExpressions* theFormat=0, GlobalVariables* theGlobalVariables=0)
  ;
  void operator*=(const quasiDiffOp<CoefficientType>& standsOnTheRight);
  void operator=(const  MonomialCollection<quasiDiffMon, CoefficientType>& other)
  { this->MonomialCollection<quasiDiffMon, CoefficientType>::operator=(other);
  }
  void LieBracketMeOnTheRight(const MonomialCollection<quasiDiffMon, CoefficientType>& standsOnTheRight)
  { quasiDiffOp<CoefficientType> tempRight;
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

template <class CoefficientType>
void quasiDiffOp<CoefficientType>::GenerateBasisLieAlgebra
(List<quasiDiffOp<CoefficientType> >& theElts,
 FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("quasiDiffOp<CoefficientType>::GenerateBasisLieAlgebra");
  ProgressReport theReport (theGlobalVariables);
  HashedList<quasiDiffMon> bufferMons;
  //std::cout << "<br> the elts:" << theElts.ToString();
  List< MonomialCollection<quasiDiffMon, CoefficientType> > theEltsConverted;
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
            theEltsConverted.PopIndexSwapWithLast(k);
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

template <class CoefficientType>
void quasiDiffOp<CoefficientType>::operator*=(const quasiDiffOp<CoefficientType>& standsOnTheRight)
{ quasiDiffOp<CoefficientType> output;
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

template <class CoefficientType>
std::string quasiDiffOp<CoefficientType>::ToString(FormatExpressions* theFormat)const
{ bool combineWeylPart=true;
  if (theFormat!=0)
    combineWeylPart=theFormat->flagQuasiDiffOpCombineWeylPart;
  if (!combineWeylPart)
    return this->MonomialCollection<quasiDiffMon, CoefficientType>::ToString(theFormat);
  MatrixTensor<ElementWeylAlgebra> reordered;
  reordered.MakeZero();
  ElementWeylAlgebra tempP;
  for (int i=0; i<this->size; i++)
  { quasiDiffMon& currentMon=(*this)[i];
    tempP.MakeZero();
    tempP.AddMonomial(currentMon.theWeylMon, this->theCoeffs[i]);
    reordered.AddMonomial(currentMon.theMatMon, tempP);
  }
  std::string result=reordered.ToString(theFormat);
  if (result=="0" && this->size!=0)
  { std::cout << "This is likely a programming error (crashing at any rate): I have a non-zero  "
    << " quasidifferential operator "
    << " with non-properly formatted LaTeX string "
    << this->MonomialCollection<quasiDiffMon, CoefficientType>::ToString(theFormat)
    << ", however its properly formatted string is 0. "
    << "Probably there is something wrong with the initializations of the monomials "
    << "of the qdo. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return result;
}

template <class CoefficientType>
void quasiDiffOp<CoefficientType>::prepareFormatFromShiftAndNumWeylVars(int theShift, int inputNumWeylVars, FormatExpressions& output)
{ output.polyAlphabeT.SetSize((inputNumWeylVars+theShift)*2);
  for (int i=0; i<inputNumWeylVars; i++)
  { std::stringstream tempStream, tempStream2;
    tempStream2 << "x_{" << i+1 << "}";
    output.polyAlphabeT[theShift+i]=tempStream2.str();
    tempStream << "\\partial" << "_{" << i+1 << "}";
    output.polyAlphabeT[2*theShift+i+inputNumWeylVars]=tempStream.str();
  }
}

template <class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::GetActionEulerOperatorPart
(MonomialP& theCoeff, ElementWeylAlgebra& outputDO, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName
  ("ModuleSSalgebra_CoefficientType::GetActionMonGenVermaModuleAsDiffOperator");
  int varShift=this->GetMinNumVars();
  int powerMonCoeff=0;
  ElementWeylAlgebra currentMonContribution;
  outputDO.MakeOne();
  //std::cout << "<br>Getting Euler part contribution of " << theCoeff.ToString();
  for (int i=0; i<theCoeff.GetMinNumVars(); i++)
  { if (!theCoeff[i].IsSmallInteger(&powerMonCoeff))
    { std::cout << "This is a programming error. "
      << "Getting euler operator part of action on generalized Verma module: I have an "
      << "exponent with non-small integer entry. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    currentMonContribution.Makexidj(i+varShift, i+varShift, 0);
    currentMonContribution.RaiseToPower(powerMonCoeff);
    outputDO*=currentMonContribution;
    //std::cout << "<br>Accounted index " << i+1 << "  out of " << theCoeff.GetMinNumVars()
    //<< ", power is " << powerMonCoeff << ", current output DO has "
    //<< outputDO.size << " monomials.";
  }
  return true;
}

template <class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::GetActionGenVermaModuleAsDiffOperator
(ElementSemisimpleLieAlgebra<Rational>& inputElt, quasiDiffOp<Rational>& output,
  GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::GetActionGenVermaModuleAsDiffOperator");
  List<ElementUniversalEnveloping<CoefficientType> > eltsNilrad;
  List<int> indicesNilrad;
  this->GetElementsNilradical(eltsNilrad, true, &indicesNilrad);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->GetGenericUnMinusElt(true, theGenElt, theGlobalVariables);
//  Polynomial<Rational> Pone, Pzero;
  result.AssignElementLieAlgebra(inputElt, *this->theAlgebras, this->indexAlgebra, 1, 0);
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
    MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon=result[i];
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

bool CommandList::fWriteGenVermaModAsDiffOperatorInner
(CommandList& theCommands, const Expression& input, Expression& output,
  Vectors<Polynomial<Rational> >& theHws, Expression& hwContext, Selection& selInducing, int indexOfAlgebra)
{ MacroRegisterFunctionWithName("CommandList::fWriteGenVermaModAsDiffOperatorInner");
   /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  if (theHws.size==0)
    return false;
  Expression tempExp;
  SemisimpleLieAlgebra& theSSalgebra=theCommands.theObjectContainer.theLieAlgebras[indexOfAlgebra];
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
  theUEformat.polyDefaultLetter="a";
  theUEformat.MaxLineLength=178;
  theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
  theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
  hwContext.ContextGetFormatExpressions(theWeylFormat);
  List<ElementSemisimpleLieAlgebra<Rational> > theGeneratorsItry;
  for (int j=0; j<theSSalgebra.GetRank(); j++)
  { Vector<Rational> ei;
    ei.MakeEi(theSSalgebra.GetRank(), j);
    theGenerator.MakeGGenerator
    (ei, theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra);
    theGeneratorsItry.AddOnTop(theGenerator);
//    theGenerator.MakeHgenerator
//    (ei, theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra);
//    theGeneratorsItry.AddOnTop(theGenerator);
    ei.Minus();
    theGenerator.MakeGGenerator(ei, theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra);
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
  latexReport << "\\begin{longtable}{rll}";
  for (int i =0; i<theGeneratorsItry.size; i++)
    latexReport << "l";
  latexReport << "}\\caption{\\label{tableDiffOps" << selInducing.ToString()
  << "} Differential operators corresponding to actions of simple positive generators for the "
  << selInducing.ToString() << "-parabolic subalgebra.}\\\\<br>";
  List<ModuleSSalgebra<RationalFunctionOld > > theMods;
  theMods.SetSize(theHws.size);
  Vector<RationalFunctionOld> tempV;
  for (int i=0; i<theHws.size; i++)
  { ModuleSSalgebra<RationalFunctionOld>& theMod=theMods[i];
    tempV=theHws[i];
    if (!theMod.MakeFromHW
        (*theSSalgebra.owner, theSSalgebra.indexInOwner, tempV, selInducing,
         *theCommands.theGlobalVariableS, 1, 0, 0, true) )
      return output.SetError("Failed to create module.", theCommands);
    if (i==0)
    { theMod.GetElementsNilradical(elementsNegativeNilrad, true);
      Polynomial<Rational> Pone, Pzero;
      Pone.MakeOne(elementsNegativeNilrad.size+theMod.GetMinNumVars());
      Pzero.MakeZero();
      theMod.GetGenericUnMinusElt(true, genericElt, *theCommands.theGlobalVariableS);
      quasiDiffOp<Polynomial<Rational> >::prepareFormatFromShiftAndNumWeylVars
      (hwContext.GetNumContextVariables(), elementsNegativeNilrad.size, theWeylFormat);

//      std::cout << "theWeylFormat: ";
//      for (int k=0; k<theWeylFormat.polyAlphabeT.size; k++)
//        std::cout << theWeylFormat.polyAlphabeT[k] << ", ";
      theUEformat.polyAlphabeT.SetSize
      (hwContext.GetNumContextVariables()+ elementsNegativeNilrad.size);
      for (int k=hwContext.GetNumContextVariables(); k<theUEformat.polyAlphabeT.size; k++)
      { std::stringstream tmpStream;
        tmpStream << "a_{" << k-hwContext.GetNumContextVariables()+1 << "}";
        theUEformat.polyAlphabeT[k] = tmpStream.str();
      }
//      std::cout << "<br>theUEformat: ";
//      for (int k=0; k<theUEformat.polyAlphabeT.size; k++)
//        std::cout << theUEformat.polyAlphabeT[k] << ", ";
      out << "<tr><td>Generic element U(n_-):</td><td>"
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
      { actionOnGenericElt.AssignElementLieAlgebra
        (theGeneratorsItry[j], theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra,
         Pone, Pzero)
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
//    theQDOs[0].GenerateBasisLieAlgebra(theQDOs, &theWeylFormat, theCommands.theGlobalVariableS);
 //   std::cout << "<br><b>Dimension generated Lie algebra: " << theQDOs.size << "</b>";
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

bool CommandList::fHWVinner
(CommandList& theCommands, Expression& output,
 Vector<RationalFunctionOld>& highestWeightFundCoords,
 Selection& selectionParSel, Expression& hwContext, int indexOfAlgebra)
{ MacroRegisterFunctionWithName("CommandList::fHWVinner");
  RecursionDepthCounter therecursionIncrementer(&theCommands.RecursionDeptH);
  //  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  List<ModuleSSalgebra<RationalFunctionOld> >& theMods=theCommands.theObjectContainer.theCategoryOmodules;
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
    (theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra, highestWeightFundCoords, selectionParSel,
     *theCommands.theGlobalVariableS, RFOne, RFZero, &report);
    theCommands.Comments << report;
    if (!isGood)
      return output.SetError
      ("Error while generating highest weight module. See comments for details. ", theCommands);
  }
  theElt.MakeHWV(theMods, indexOfModule, RFOne);
  return output.AssignValueWithContext<ElementTensorsGeneralizedVermas<RationalFunctionOld> >
  (theElt, hwContext, theCommands);
}

template <class CoefficientType>
void ModuleSSalgebra<CoefficientType>::GetFDchar(charSSAlgMod<CoefficientType>& output)
{ output.MakeZero(*this->theAlgebras, this->indexAlgebra);
  if (this->theHWFundamentalCoordsBaseField.size<=0)
    return;
  MonomialChar<CoefficientType> tempMon;
  WeylGroup& theWeyl=this->GetOwner().theWeyl;
  for (int i =0; i<this->theModuleWeightsSimpleCoords.size; i++)
  { tempMon.weightFundamentalCoords=theWeyl.GetFundamentalCoordinatesFromSimple(this->theModuleWeightsSimpleCoords[i]);
    output.AddMonomial(tempMon, this->theGeneratingWordsGrouppedByWeight[i].size);
  }
}

bool CommandList::fSplitGenericGenVermaTensorFD
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (input.IsListNElements(4))
    return output.SetError
    ("Function fSplitGenericGenVermaTensorFD is expected to have three arguments: \
     SS algebra type, weight, weight. ", theCommands);
  Expression& leftE=input[1];
  Expression& genVemaWeightNode=input[3];
  Expression& fdWeightNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, leftE, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  int theRank=theSSalgebra->GetRank();
  Vector<RationalFunctionOld> highestWeightFundCoords;
  Expression hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVector<RationalFunctionOld>
      (genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank,
       CommandList::innerPolynomial))
  { theCommands.Comments
    << "Failed to convert the third argument of fSplitGenericGenVermaTensorFD to a list of "
    << theRank << " polynomials. The second argument you gave is "
    << genVemaWeightNode.ToString() << ".";
    return false;
  }
  Vector<Rational> theFDhw;
  if (!theCommands.GetVector<Rational>(fdWeightNode, theFDhw, &emptyContext, theRank, 0))
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
  theCommands.fHWVinner
  (theCommands, hwvGenVerma, highestWeightFundCoords, selParSel1,
   hwContext, theSSalgebra->indexInOwner);
  if (hwvGenVerma.IsError())
  { output=hwvGenVerma;
    return true;
  }
  Vector<RationalFunctionOld> theFDhwRF;
  theFDhwRF=theFDhw;
  theCommands.fHWVinner
  (theCommands, hwvFD, theFDhwRF, selFD, hwContext, theSSalgebra->indexInOwner);
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
  int indexGenMod=theHWgenVerma[0].theMons[0].indexInOwner;
  ModuleSSalgebra<RationalFunctionOld>& theFDMod=theHWfd[0].theMons[0].GetOwner();
  int indexFDMod=theHWfd[0].theMons[0].indexInOwner;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir, theCasimirMinusChar;
  charSSAlgMod<RationalFunctionOld> theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.MakeFromWeight
  (theFDMod.theHWSimpleCoordSBaseField, theCommands.theObjectContainer.theLieAlgebras,
   theSSalgebra->indexInOwner);
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
  theFDLeviSplitShifteD.MakeZero(theCommands.theObjectContainer.theLieAlgebras, theSSalgebra->indexInOwner);
  MonomialChar<RationalFunctionOld> tempMon;
  ElementUniversalEnveloping<RationalFunctionOld> currentChar;
  for (int i=0; i<theLeviEigenVectors.size; i++)
  { tempMon.weightFundamentalCoords=theEigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoords+=theGenMod.theHWFundamentalCoordsBaseField;
    theFDLeviSplitShifteD.AddMonomial(tempMon, RFOne);
    currentHWdualcoords=theSSalgebra->theWeyl.GetDualCoordinatesFromFundamental(tempMon.weightFundamentalCoords);
    currentChar=theCasimir;
    currentChar.ModOutVermaRelations(theCommands.theGlobalVariableS, & currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.AddOnTop(currentChar);
    out << "<tr><td>"
    << theFDLeviSplitShifteD[i].weightFundamentalCoords.ToStringLetterFormat("\\psi")
    << "</td><td>" << currentChar.ToString(&tempFormat) << "</td></tr>";
    latexReport1 << " $"
    << theFDLeviSplitShifteD[i].weightFundamentalCoords.ToStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i+1 <<"}:=$ $" << currentChar.ToString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFunctionOld> tempElt, tempElt2;
  List<ModuleSSalgebra<RationalFunctionOld> >& theMods=theCommands.theObjectContainer.theCategoryOmodules;
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
    tempElt.MakeHWV(theMods, indexFDMod, RFOne);
    tempElt.MultiplyOnTheLeft
    (theLeviEigenVectors[i], theElt, theMods, *theSSalgebra, *theCommands.theGlobalVariableS,
     RFOne, RFZero);
    tempElt.MakeHWV(theMods, indexGenMod, RFOne);
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
        (theCasimirMinusChar, tempElt2, theMods, *theSSalgebra, *theCommands.theGlobalVariableS,
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

bool CommandList::fMatrix
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (input.IsListNElements(4))
    return false;
  Expression& leftE  =input[1];
  Expression& middleE=input[2];
  Expression& rightE =input[3];
  int numRows, numCols;
  if (!middleE.IsSmallInteger(&numRows) || !rightE.IsSmallInteger(&numCols))
    return false;
  if (numRows<=0 || numCols<=0)
    return false;
  if (numRows>1000 || numCols>1000)
  { theCommands.Comments
    << "Max number of rows/columns is 1000. You requested " << numRows << " rows and "
    << numCols << " columns.<br>";
    return false;
  }
  output.reset(theCommands, numRows+1);
  output.children[0].MakeAtom(theCommands.opSequence(), theCommands);
  output.format=output.formatMatrix;
  output.children.SetSize(numRows);
  Expression theIndices;
  theIndices.reset(theCommands, 3);
  theIndices.children[0].MakeAtom(theCommands.opSequence(), theCommands);
  for (int i=0; i<numRows; i++)
  { Expression& currentRow=output[i];
    currentRow.reset(theCommands, numCols+1);
    currentRow[0].MakeAtom(theCommands.opSequence(), theCommands);
    currentRow.format=currentRow.formatMatrixRow;
    for (int j=0; j<numCols; j++)
    { Expression& currentE=currentRow.children[j];
      theIndices[1].AssignValue<Rational>(i+1, theCommands);
      theIndices[2].AssignValue<Rational>(j+1, theCommands);
      currentE.MakeFunction(theCommands, leftE, theIndices);
    }
  }
  return true;
}

bool CommandList::innerPolynomial
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPolynomial");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  //std::cout << "<br>Evaluating innerPolynomial on: " << input.ToString();
  //std::cout << "<br>First elt input is:" << input[0].ToString();
  return theCommands.outerExtractAndEvaluatePMTDtree<Polynomial<Rational> >
  (theCommands, input, output);
}

bool CommandList::innerRationalFunction
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerPolynomial");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  //std::cout << "<br>Evaluating innerPolynomial on: " << input.ToString();
  //std::cout << "<br>First elt input is:" << input[0].ToString();
  return theCommands.outerExtractAndEvaluatePMTDtree<RationalFunctionOld>
  (theCommands, input, output);
}


bool CommandList::innerGetChevGen
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsSequenceNElementS(2))
    return false;
  SemisimpleLieAlgebra* theSSalg;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, input[1], theSSalg, &errorString))
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
  theElt.MakeGenerator(theIndex, *theSSalg->owner, theSSalg->indexInOwner);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg->owner, theSSalg->indexInOwner);
  return output.AssignValue(theUE, theCommands);
}

bool CommandList::innerGetCartanGen
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsSequenceNElementS(2))
    return false;
  SemisimpleLieAlgebra* theSSalg;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, input[1], theSSalg, &errorString))
    output.SetError(errorString, theCommands);
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex))
    return false;
  if (theIndex<1 || theIndex>theSSalg->GetRank() )
    return output.SetError("Bad Cartan subalgebra genrator index.", theCommands);
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH;
  theH.MakeEi(theSSalg->GetRank(), theIndex);
  theElt.MakeHgenerator(theH, *theSSalg->owner, theSSalg->indexInOwner);
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  theUE.AssignElementLieAlgebra(theElt, *theSSalg->owner, theSSalg->indexInOwner);
  return output.AssignValue(theUE, theCommands);
}

bool CommandList::fElementUniversalEnvelopingAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fElementUniversalEnvelopingAlgebra");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (input.IsOfType<ElementUniversalEnveloping<RationalFunctionOld > >())
  { output=input;
    return true;
  }
  if (!theCommands.outerExtractAndEvaluatePMTDtree<ElementUniversalEnveloping<RationalFunctionOld > >
      (theCommands, input, output))
    return output.SetError("Failed to convert to element universal enveloping.", theCommands);
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  if (!output.IsOfType(&outputUE))
    return output.SetError("Failed to convert to element universal enveloping.", theCommands);
  outputUE.Simplify(*theCommands.theGlobalVariableS, 1, 0);
  return output.AssignValueWithContext(outputUE, output.GetContext(), theCommands);
}

bool CommandList::fKLcoeffs
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  std::string errorString;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, input, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  std::stringstream out;
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (theWeyl.theDynkinType.GetSizeWeylByFormula()>192)
  { out << "I have been instructed to run only for Weyl groups that have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra->GetLieAlgebraName() << " has "
    << theWeyl.theDynkinType.GetSizeWeylByFormula() << ".";
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

bool CommandList::fWeylOrbit
(CommandList& theCommands, const Expression& input, Expression& output,
 bool useFundCoords, bool useRho)
{ if (input.IsListNElements(3))
    return output.SetError("fWeylOrbit takes two arguments", theCommands);
  Expression& theSSalgebraNode=input[1];
  Expression& vectorNode=input[2];
  SemisimpleLieAlgebra* theSSalgebra;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, theSSalgebraNode, theSSalgebra, &errorString))
    return output.SetError(errorString, theCommands);
  Vector<Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Expression theContext;
  if (!theCommands.GetVector(vectorNode, theHWfundCoords, &theContext, theSSalgebra->GetRank(), theCommands.innerPolynomial))
    return output.SetError("Failed to extract highest weight", theCommands);
  WeylGroup& theWeyl=theSSalgebra->theWeyl;
  if (!useFundCoords)
  { theHWsimpleCoords=theHWfundCoords;
    theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  } else
    theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroup orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theSSalgebra->theWeyl.GenerateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet, 1921))
    out << "Failed to generate the entire orbit (maybe too large?), generated the first " << outputOrbit.size
    << " elements only.";
  else
    out << "The orbit has " << outputOrbit.size << " elements.";
  latexReport
  << "\\begin{longtable}{p{3cm}p{4cm}p{4cm}p{4cm}}Element & Eps. coord. & Image fund. coordinates& "
  << "Hw minus wt. \\\\\n<br>";
  out << "<table><tr> <td>Group element</td> <td>Image in simple coords</td> "
  << "<td>Epsilon coords</td><td>Fundamental coords</td>";
  if (useRho)
    out << "<td>Corresponding b-singular vector candidate</td>";
  out << "</tr>";
  MonomialUniversalEnveloping<Polynomial<Rational> > standardElt;
  LargeInt tempInt;
  bool useMathTag=outputOrbit.size<150;
  Matrix<Rational> epsCoordMat;
  theWeyl.theDynkinType.GetEpsilonMatrix(epsCoordMat);
  for (int i=0; i<outputOrbit.size; i++)
  { theFormat.simpleRootLetter="\\alpha";
    theFormat.fundamentalWeightLetter="\\psi";
    std::string orbitEltString=outputOrbit[i].ToString(&theFormat);
    Vector<Polynomial<Rational> > epsVect=outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords=epsVect.ToStringEpsilonFormat();
    std::string weightEltString=
    theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat
    (theFormat.fundamentalWeightLetter, &theFormat);
    out << "<tr>" << "<td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitGeneratingSet[i].ToString()) : orbitGeneratingSet[i].ToString())
    << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltString) : orbitEltString) << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(orbitEltStringEpsilonCoords) : orbitEltStringEpsilonCoords) << "</td><td>"
    << (useMathTag ? CGI::GetHtmlMathSpanPure(weightEltString) : weightEltString)
    << "</td>";
    latexReport << "$" << orbitGeneratingSet[i].ToString(&theFormat) << "$ & $"
    << orbitEltStringEpsilonCoords
    << "$ & $"
    <<  weightEltString << "$ & $"
    << (outputOrbit[0]-outputOrbit[i]).ToStringLetterFormat(theFormat.simpleRootLetter, &theFormat)
    << "$\\\\\n<br>"
    ;
    if (useRho)
    { currentWeight=theHWsimpleCoords;
      standardElt.MakeConst(*theSSalgebra->owner, theSSalgebra->indexInOwner);
      bool isGood=true;
      for (int j=0; j<orbitGeneratingSet[i].size; j++)
      { int simpleIndex=orbitGeneratingSet[i][j];
        theExp=theWeyl.GetScalarProdSimpleRoot(currentWeight, simpleIndex);
        theWeyl.SimpleReflection(simpleIndex, currentWeight, useRho, false);
        theExp*=2;
        theExp/=theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex];
        if (useRho)
          theExp+=1;
        if (theExp.IsInteger(&tempInt))
          if (tempInt<0)
          { isGood=false;
            break;
          }
        standardElt.MultiplyByGeneratorPowerOnTheLeft
        (theSSalgebra->GetNumPosRoots() -simpleIndex-1, theExp);
      }
      out << "<td>";
      if (isGood)
        out << CGI::GetHtmlMathSpanPure(standardElt.ToString(&theFormat));
      else
        out << "-";
      out << "</td>";
    }
    out << "</tr>";
  }
  latexReport << "\\end{longtable}";
  out << "</table>" << "<br> " << latexReport.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::innerPrintSSLieAlgebra
(CommandList& theCommands, const Expression& input, Expression& output, bool Verbose)
{ MacroRegisterFunctionWithName("CommandList::innerPrintSSLieAlgebra");
  std::string errorString;
  SemisimpleLieAlgebra *tempSSpointer;
  input.CheckInitialization();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerSSLieAlgebra, input, tempSSpointer, &errorString))
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
  out << "<br>Weyl group size: " << theWeyl.theDynkinType.GetSizeWeylByFormula() << "."
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
    Vector<Rational> tempRoot;
    ElementSemisimpleLieAlgebra<Rational> tempElt1;
    for (int i=0; i<theSSalgebra.GetNumGenerators(); i++)
    { tempElt1.MakeGenerator
      (i,*theSSalgebra.owner, theSSalgebra.indexInOwner);
      tempRoot=theSSalgebra.GetWeightOfGenerator(i);
      out << "$" << tempElt1.ToString(&theFormat) << "$&$"<< tempRoot.ToString() << "$";
      out << "&$" << theSSalgebra.theWeyl.GetEpsilonCoords(tempRoot).ToStringLetterFormat("\\varepsilon") << "$";
      out << "\\\\\n";
    }
    out << "\\end{longtable}" << "<hr>";
  }
  out << "We define the symmetric Cartan matrix by requesing that the entry in "
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
  if (Verbose)
  { out << "<hr>Simple basis in epsilon coordinates: <table>";
    simpleBasis.MakeEiBasis(theWeyl.GetDim());
    theWeyl.GetEpsilonCoords(simpleBasis, simplebasisEpsCoords);
    for (int i=0; i< simplebasisEpsCoords.size; i++)
    { out << "<tr><td>"
      << simpleBasis[i].ToString() << " </td><td>=</td> <td>"
      << CGI::
      GetHtmlMathFromLatexFormulA
      (simplebasisEpsCoords[i].ToStringEpsilonFormat(), "", "</td><td>", false, false)
      << "</td></tr>";
    }
    out << "</table>";
    out << "Note on root system convention. Except for F_4, "
    << "our epsilon notation follows the convention "
    << " of <br> Humphreys, Introduction to Lie algebras and representation theory, page 65."
    << " <br> For F_4, we follow "
    << " our own convention.  <br>Motivation: in our convention, 1) the symmetric Cartan matrix is "
    << " integral; 2) the long roots come first. <br>Point (1) does not hold either "
    << "for the convention of Humphreys, nor for the May 2012 convention of Wikipedia. "
    << "<br>Having an integral symmetric Cartan matrix is beneficial both for the speed "
    << "of computations, <br>and for reducing sizes of the printouts.";
    out << "<hr>Root system:<table><tr><td>Simple basis coordinates</td><td></td>"
    << "<td>Epsilon coordinates non-LaTeX'ed (convention: see above)</td></tr> ";
    Vectors<Rational> rootSystemEpsCoords;
    theWeyl.GetEpsilonCoords(theWeyl.RootSystem, rootSystemEpsCoords);
    for (int i=0; i<theWeyl.RootSystem.size; i++)
    { Vector<Rational>& current=theWeyl.RootSystem[i];
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

bool CommandList::innerSSLieAlgebra
(CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::innerSSLieAlgebra");
  //std::cout << "<br>Now I'm here!";
  if (!theCommands.innerPolynomial(theCommands, input, output))
    return output.SetError
    ("Failed to extract the semismiple Lie algebra type from " + input.ToString(), theCommands);
  if (output.IsError())
    return true;
  Polynomial<Rational> theType=output.GetValuE<Polynomial<Rational> >();
  Expression theContext=output.GetContext();
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  SemisimpleLieAlgebra tempSSalgebra;
  DynkinType theDynkinType;
  DynkinSimpleType simpleComponent;
  theDynkinType.MakeZero();
  char theWeylLetter='X';
  for (int i=0; i<theType.size; i++)
  { MonomialP& currentMon=theType[i];
    int variableIndex;
    if (!currentMon.IsOneLetterFirstDegree(&variableIndex))
      return output.SetError
      ("Failed to extract type from monomial "+ currentMon.ToString(&theFormat), theCommands);
    Expression typEE= theContext.ContextGetContextVariable(variableIndex);
    if (typEE.children.size!=2)
      return output.SetError
      ("The monomial "+ currentMon.ToString(&theFormat)+
       " appears not to be a Dynkin simple type ", theCommands);
    Expression rankE=typEE[1];
    Expression typeLetter=typEE[0];
    Rational firstCoRootSquaredLength=2;
    bool foundLengthFromExpression=false;
    if (typeLetter.IsListStartingWithAtom(theCommands.opThePower()))
    { if (!typeLetter[2].IsOfType<Rational>(&firstCoRootSquaredLength))
        return output.SetError
        ("Couldn't extract first co-root length from " + currentMon.ToString(&theFormat), theCommands);
      if (firstCoRootSquaredLength<=0)
        return output.SetError
        ("Couldn't extract positive rational first co-root length from " + currentMon.ToString(&theFormat), theCommands);
      typeLetter.AssignChild(1);
      foundLengthFromExpression=true;
    }
    std::string theTypeName;
    if (!typeLetter.IsOperation(&theTypeName))
      return output.SetError
      ("I couldn't extract a type letter from "+ currentMon.ToString(&theFormat), theCommands);
    if (theTypeName.size()!=1)
      return output.SetError
      ("The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G.\
        Instead, it is "+ theTypeName + "; error while processing "
       + currentMon.ToString(&theFormat), theCommands);
    theWeylLetter=theTypeName[0];
    if (theWeylLetter=='a') theWeylLetter='A';
    if (theWeylLetter=='b') theWeylLetter='B';
    if (theWeylLetter=='c') theWeylLetter='C';
    if (theWeylLetter=='d') theWeylLetter='D';
    if (theWeylLetter=='e') theWeylLetter='E';
    if (theWeylLetter=='f') theWeylLetter='F';
    if (theWeylLetter=='g') theWeylLetter='G';
    if (!(theWeylLetter=='A' || theWeylLetter=='B' || theWeylLetter=='C'
          || theWeylLetter=='D' || theWeylLetter=='E' || theWeylLetter=='F'
          || theWeylLetter=='G'))
      return output.SetError
      ("The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G; \
       error while processing "
       + currentMon.ToString(&theFormat), theCommands);
    int theRank;
    if (!rankE.IsSmallInteger(&theRank))
      return output.SetError
      ("I wasn't able to extract rank from " + currentMon.ToString(&theFormat), theCommands);
    if (theRank<1 || theRank>8)
      return output.SetError
      ("The rank of a simple Lie algebra must be between 1 and 8; error while processing "
       + currentMon.ToString(&theFormat), theCommands);
    simpleComponent.theLetter=theWeylLetter;
    simpleComponent.theRank= theRank;
    if (!foundLengthFromExpression)
      if (theWeylLetter=='F')
        firstCoRootSquaredLength=1;
    simpleComponent.lengthFirstCoRootSquared= firstCoRootSquaredLength;
    int theMultiplicity=-1;
    if (!theType.theCoeffs[i].IsSmallInteger(&theMultiplicity))
      theMultiplicity=-1;
    if (theMultiplicity<0)
    { std::stringstream out;
      out << "I failed to convert the coefficient " << theType.theCoeffs[i]
      << " of " << currentMon.ToString(&theFormat) << " to a small integer";
      return output.SetError(out.str(), theCommands);
    }
    theDynkinType.AddMonomial(simpleComponent, theMultiplicity);
  }
  if (theDynkinType.GetRank()>20)
  { std::stringstream out;
    out << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << " If you would like to relax this limitation edit file " << __FILE__ << " line "
    << __LINE__ << ". Note that the Chevalley constant computation reserves a dim(g)*dim(g)"
    << " table of RAM memory, which means the RAM memory rises with the 4^th power of dim(g). "
    << " You have been warned. "
    << " Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
    return output.SetError(out.str(), theCommands);
  }
  tempSSalgebra.theWeyl.MakeFromDynkinType(theDynkinType);
  int indexInOwner=theCommands.theObjectContainer.theLieAlgebras.GetIndex(tempSSalgebra);
  bool feelsLikeTheVeryFirstTime=(indexInOwner==-1);
  if (feelsLikeTheVeryFirstTime)
  { tempSSalgebra.owner=&theCommands.theObjectContainer.theLieAlgebras;
    tempSSalgebra.indexInOwner=theCommands.theObjectContainer.theLieAlgebras.size;
    indexInOwner=tempSSalgebra.indexInOwner;
    theCommands.theObjectContainer.theLieAlgebras.AddOnTop(tempSSalgebra);
  }
  SemisimpleLieAlgebra& theSSalgebra=
  theCommands.theObjectContainer.theLieAlgebras[indexInOwner];
  output.AssignValue(theSSalgebra, theCommands);
  if (feelsLikeTheVeryFirstTime)
  { theSSalgebra.ComputeChevalleyConstantS(theCommands.theGlobalVariableS);
    Expression tempE;
    theCommands.innerPrintSSLieAlgebra(theCommands, output, tempE, false);
    theCommands.Comments << tempE.GetValuE<std::string>();
  }
  //theSSalgebra.TestForConsistency(*theCommands.theGlobalVariableS);
  return true;
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
    if (this->children[i].HasBoundVariables())
      return true;
  return false;
}

bool CommandList::fDrawPolarRfunctionTheta
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fDrawPolarRfunctionTheta");
  if (!input.IsListNElements(4))
    return output.SetError
    ("Drawing polar coordinates takes three arguments: function, lower angle \
      bound and upper angle bound. ", theCommands);
  Expression& lowerE=input[2];
  Expression& upperE=input[3];
  Expression functionE;
  Rational upperBound, lowerBound;
  if (!lowerE.IsOfType(&upperBound) || !upperE.IsOfType(&lowerBound))
    return
    output.SetError
    ("Failed to convert upper and lower bounds of drawing function to rational numbers.", theCommands);
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  if (! theCommands.fSuffixNotationForPostScript(theCommands, input[1], functionE))
    return false;
  std::stringstream out, resultStream;
  out << CGI::GetHtmlMathSpanPure(input[1].ToString())
  << "<br>";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}"
  << "\\usepackage{pst-3dplot}\\begin{document} \\pagestyle{empty}";
  resultStream << " \\begin{pspicture}(-5, 5)(5,5)";
  resultStream << "\\psaxes[labels=none]{<->}(0,0)(-4.5,-4.5)(4.5,4.5)";
  resultStream << "\\parametricplot[linecolor=red, plotpoints=1000]{"
  << lowerBound.DoubleValue() << "}{" << upperBound.DoubleValue() << "}{";
  std::string funString=functionE.GetValuE<std::string>();
  resultStream << funString << " t cos mul " << funString << " t sin mul" << "}";
  resultStream << "\\end{pspicture}\\end{document}";
  out << theCommands.WriteDefaultLatexFileReturnHtmlLink(resultStream.str(), true);
  out << "<br><b>LaTeX code used to generate the output. </b><br>" << resultStream.str();
  return output.AssignValue(out.str(), theCommands);
}

bool CommandList::fSuffixNotationForPostScript
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fSuffixNotation");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (*theCounter.theCounter ==theCommands.MaxRecursionDeptH-2)
    return output.AssignValue((std::string) "...", theCommands);
  if (!input.IsLisT())
    return output.SetError("Error: typeless expression has no postscript notation. ", theCommands);
  Rational ratValue;
  if (input.IsAtoM())
    return output.AssignValue(theCommands.operationS[input.theData], theCommands);
  else if (input.IsOfType<Rational>(&ratValue))
    return output.AssignValue(ratValue.ToString(), theCommands);
  std::stringstream out;
  Expression currentE;
  std::string currentString;
  for (int i=1; i<input.children.size; i++)
  { if (!theCommands.fSuffixNotationForPostScript(theCommands, input[i], currentE))
      return output.SetError("Failed to convert"+input[i].ToString(), theCommands);
    if (!currentE.IsOfType(&currentString))
      return output.SetError("Failed to convert"+input[i].ToString(), theCommands);
    out << currentString << " ";
  }
  if (!input[0].IsAtoM())
  { if (!theCommands.fSuffixNotationForPostScript(theCommands, input[0], currentE))
      return output.SetError("Failed to convert "+currentE.ToString(), theCommands);
    if (!currentE.IsOfType(&currentString))
      return output.SetError("Failed to convert "+currentE.ToString(), theCommands);
    out << currentString << " ";
    return output.AssignValue(out.str(), theCommands);
  }
  int theOp=input[0].theData;
  if (theOp==theCommands.opDivide())
  { out << "div ";
    return output.AssignValue(out.str(), theCommands);
  } else if (theOp==theCommands.opPlus())
  { out << "add ";
    return output.AssignValue(out.str(), theCommands);
  } else if (theOp==theCommands.opMinus())
  { if (output.children.size==2)
      out << "-1 mul ";
    else
      out << "sub ";
    return output.AssignValue(out.str(), theCommands);
  } else if (theOp==theCommands.opTimes())
  { out << "mul";
    return output.AssignValue(out.str(), theCommands);
  } else if (theOp==theCommands.opThePower())
  { out << "exp ";
    return output.AssignValue(out.str(), theCommands);
  }
  return output.SetError("Failed to convert "+ input.ToString(), theCommands);
}

bool CommandList::fIsInteger
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (input.IsInteger())
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
  this->flagNewContextNeeded=true;
  this->MaxLatexChars=2000;
  this->numEmptyTokensStart=9;
  this->MaxNumCachedExpressionPerContext=100000;
  this->controlSequences.Clear();

  this->operationS.Clear();
  this->FunctionHandlers.SetSize(0);
  this->operationS.SetExpectedSize(300);
  this->FunctionHandlers.SetExpectedSize(300);

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
  this->AddOperationNoRepetitionAllowed("==");
  this->AddOperationNoRepetitionAllowed("Bind");
  this->AddOperationNoRepetitionAllowed("\\cup");
  this->AddOperationNoRepetitionAllowed("\\sqcup");
  this->AddOperationNoRepetitionAllowed("Error");
  this->AddOperationNoRepetitionAllowed("Sequence");
  this->AddOperationNoRepetitionAllowed("Rational");
  this->AddOperationNoRepetitionAllowed("AlgebraicNumber");
  this->AddOperationNoRepetitionAllowed("Polynomial<Rational>");
  this->AddOperationNoRepetitionAllowed("RationalFunction");
  this->AddOperationNoRepetitionAllowed("string");
  this->AddOperationNoRepetitionAllowed("ElementUEoverRF");
  this->AddOperationNoRepetitionAllowed("ElementTensorGVM");
  this->AddOperationNoRepetitionAllowed("CharSSAlgMod");
  this->AddOperationNoRepetitionAllowed("SemisimpleLieAlg");
  this->AddOperationNoRepetitionAllowed("LittelmannPath");
  this->AddOperationNoRepetitionAllowed("LRO");
  this->AddOperationNoRepetitionAllowed("PolyVars");
  this->AddOperationNoRepetitionAllowed("Context");

  this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTop("{{}}");
  this->controlSequences.AddOnTop("Variable");
  this->controlSequences.AddOnTop(this->operationS);//all operations defined up to this point are also control sequences
  this->controlSequences.AddOnTop("Expression");
  this->controlSequences.AddOnTop("Integer");
  this->controlSequences.AddOnTop(",");
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
  this->controlSequences.AddOnTop("SequenceMatrixRows");
  this->controlSequences.AddOnTop("MatrixRow");
  this->controlSequences.AddOnTop("=");
  this->controlSequences.AddOnTop("$");
  this->controlSequences.AddOnTop("MatrixSeparator");
  this->controlSequences.AddOnTop("MatrixRowSeparator");
  this->controlSequences.AddOnTop("\\begin");
  this->controlSequences.AddOnTop("array");
  this->controlSequences.AddOnTop("\\end");
  this->controlSequences.AddOnTop("\\\\");
  this->controlSequences.AddOnTop("\\");
  this->controlSequences.AddOnTop("&");
  this->controlSequences.AddOnTop("%");
  this->controlSequences.AddOnTop("LogParsing");
//  this->controlSequences.AddOnTop("c...c");
//    this->thePropertyNames.AddOnTop("IsCommutative");
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
  this->theProgramExpression[1][0][0].MakeAtom(this->opSSLieAlg(), *this);
  this->theProgramExpression[2][1][1][0].MakeAtom(this->opSSLieAlg(), *this);
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

template <class CoefficientType>
void ElementTensorsGeneralizedVermas<CoefficientType>::TensorOnTheRight
  (const ElementTensorsGeneralizedVermas<CoefficientType>& right, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<CoefficientType>::TensorOnTheRight");
  if (right.IsEqualToZero())
  { this->MakeZero();
    return;
  }
  int maxNumMonsFinal=this->size*right.size;
  ElementTensorsGeneralizedVermas<CoefficientType> output;
  MonomialTensorGeneralizedVermas<CoefficientType> bufferMon;
  output.MakeZero();
  output.SetExpectedSize(maxNumMonsFinal);
  CoefficientType theCoeff;
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
  MacroRegisterFunctionWithName("CommandList::EvaluateDoExtractBaseMultiplication");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  bool result=false;
  output=input;
  Expression& leftE=output[1];
  Expression& rightE=output[2];
//  std::cout << "<br>handling base extraction of: " << input.ToString();
  //handle Anything*Rational:=Rational*Anything
  if (rightE.IsOfType<Rational>())
  { MathRoutines::swap(leftE, rightE);
    result=true;
//    std::cout << "swapped " << leftE.ToString() << " and " << rightE.ToString();
  }
  if (rightE.IsListStartingWithAtom(theCommands.opTimes()))
  { if (rightE.children.size!=3)
      return result;
    Expression& rightLeftE=rightE[1];
    Expression& rightRightE=rightE[2];
    //handle Anything1*(Rational*Anything2):=Rational*(Anything1*Anything2)
    if (rightLeftE.IsOfType<Rational>())
    { MathRoutines::swap(rightLeftE, leftE);
      result=true;
//      std::cout << " swapped " << rightLeftE.ToString() << " and " << leftE.ToString();
    }
    //<- handle a*(b*anything)
    //on condition that a*b has an inner handler
    Expression tempExp, newExpr;
    tempExp.reset(theCommands,2);
    tempExp[0]=leftE;
    tempExp[1]=rightLeftE;
    if (theCommands.innerTimes(theCommands, tempExp, newExpr))
    { output.MakeProducT(theCommands, newExpr, rightRightE);
      result=true;
    }
  }
  //handle 0*anything=0
  if (leftE.IsEqualToZero())
    return output.AssignValue(0, theCommands);
  return result;
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
  Expression& withNotation=input[2];
  Expression& theNotation=input[1];
  theCommands.Comments << "<br>Registering notation: globally, " << withNotation.ToString()
  << " will be denoted by " << theNotation.ToString();
  theCommands.theObjectContainer.ExpressionNotation.AddOnTop(theNotation.ToString());
  theCommands.theObjectContainer.ExpressionWithNotation.AddOnTop(withNotation);
  input[0].theData=theCommands.opDefine();
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
  int theFormat=input.format;
  output.reset(theCommands, 3);
  output[0].MakeAtom(theAdditiveOp, theCommands);
  output[1].MakeXOX(theCommands, theMultiplicativeOP, input[1][1], input[2]);
  output[2].MakeXOX(theCommands, theMultiplicativeOP, input[1][2], input[2]);
  output[1].format=theFormat;
  output[2].format=theFormat;
  return true;
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
  int theFormat=input.format;
  output.reset(theCommands, 3);
  output[0].MakeAtom(theAdditiveOp, theCommands);
  output[1].MakeXOX(theCommands, theMultiplicativeOP, input[1], input[2][1]);
  output[2].MakeXOX(theCommands, theMultiplicativeOP, input[1], input[2][2]);
  output[1].format=theFormat;
  output[2].format=theFormat;
  return true;
}

bool CommandList::outerPlus
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::outerPlus");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opPlus()))
    return false;
  List<Expression>& summands= theCommands.buffer1;
  summands.SetSize(0);
  //bool needSimplification=
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical
  (input, summands);
  MonomialCollection<Expression, Rational> theSum;
//  Rational constTerm=0;
//  bool foundConstTerm=false;
  Expression oneE; //used to record the constant term
  oneE.AssignValue<Rational>(1, theCommands);
//  std::cout << "<b>" << theExpression.ToString() << "</b>";
//  if (theExpression.ToString()=="(4)*(a) b+(a) b")
//    std::cout << "problem!";
//assert(false);
//return false;
  theSum.MakeZero();
  for (int i=0; i<summands.size; i++)
  { Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
    Rational theCoeff=1;
    if (currentSummandNoCoeff->IsListStartingWithAtom(theCommands.opTimes()))
    { if(currentSummandNoCoeff->children[1].IsOfType<Rational>(&theCoeff))
        currentSummandNoCoeff=&currentSummandNoCoeff->children[2];
    } else if (currentSummandNoCoeff->IsOfType<Rational>(&theCoeff))
      currentSummandNoCoeff=&oneE;
    theSum.AddMonomial(*currentSummandNoCoeff, theCoeff);
  }
//  std::cout << "<br>Collected sum: " << theSum.ToString();
/*
  std::cout << "<hr>summands: ";
  for (unsigned i=0; i< summands.size(); i++)
    std::cout << summands[i].ToString() << ", ";
  std::cout << " const term: " << constTerm;
    std::cout << "<br>coeff->summand_no_coeff: ";
  for (int i=0; i< summandsNoCoeff.size(); i++)
    std::cout << theCoeffs[i] << "->" << summandsNoCoeff[i].ToString() << ", ";
  std::cout << " const term: " << constTerm;
*/
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
      current.children[0].MakeAtom(theCommands.opTimes(), theCommands);
      current.children[1].AssignValue(theSum.theCoeffs[i], theCommands);
      current.children[2]=theSum[i];
    } else
      current=theSum[i];
  }
  Expression* currentExp;
  currentExp=&output;
  for (int i=0; i<summandsWithCoeff.size-1; i++)
  { currentExp->reset(theCommands, 3);
    currentExp->children[0].MakeAtom(theCommands.opPlus(), theCommands);
    currentExp->children[1]=summandsWithCoeff[i];
    currentExp=& currentExp->children[2];
  }
  *currentExp=summandsWithCoeff[summandsWithCoeff.size-1];
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
  Expression& functionNameNode =input[0];
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
  for (int i=0; i<theCommands.FunctionHandlers[functionNameNode.theData].size; i++)
    if (!theCommands.FunctionHandlers[functionNameNode.theData][i].flagIsInner)
    { Function& outerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      if (outerFun.theFunction(theCommands, input, output))
        return output!=input;
    } else
    { Function& innerFun=theCommands.FunctionHandlers[functionNameNode.theData][i];
      Expression arguments;
      if (input.children.size!=2)
      { arguments.reset(theCommands, input.children.size-1);
        for (int i=1; i<input.children.size; i++)
          arguments[i-1]=input[i];
      } else
        arguments=input[1];
      bool isGood=true;
      //the following if clause needs to be rewritten.
      if (innerFun.theArgumentTypes.children.size==2)
      { if (input.children.size!=3)
          isGood=false;
        else
        { if (!input[1].IsListNElementsStartingWithAtom(innerFun.theArgumentTypes[0].theData) ||
              !input[2].IsListNElementsStartingWithAtom(innerFun.theArgumentTypes[1].theData))
          isGood=false;
        }
      }
      if (isGood)
        if (innerFun.theFunction(theCommands, arguments, output))
          return true;
    }
  return false;
}

bool CommandList::outerEqualEqual
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (input.IsSequenceNElementS(3))
    return false;
  Expression& left=input[1];
  Expression& right=input[2];
  if (left.HasBoundVariables() || right.HasBoundVariables())
    return false;
  if (left==right)
    return output.AssignValue(1, theCommands);
  else
    return output.AssignValue(0, theCommands);
}

bool CommandList::outerLieBracket
(CommandList& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  Expression& leftE=input[1];
  Expression& rightE=input[2];
  if (leftE.IsOfType<Rational>() && rightE.IsOfType<Rational>())
    return output.AssignValue(0, theCommands);
  if (leftE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >() ||
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
{ this->children.SetSize(2);
  this->children[0].MakeAtom(owner.opError(), owner);
  this->children[1].AssignValue<std::string>(theError, owner);
  return true;
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
  output.children.ReservE(numElts);
  output.reset(theCommands, 1);
  output[0].MakeAtom(theCommands.opSequence(), theCommands);
  for (int i=1; i<input.children.size; i++)
    for (int j=1; j<input[i].children.size; j++)
      output.children.AddOnTop(input[i][j]);
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
  theList.SetExpectedSize(numElts);
  for (int i=1; i<input.children.size; i++)
    for (int j=1; j<input[i].children.size; j++)
      theList.AddOnTopNoRepetition(input[i][j]);
  output.children.ReservE(numElts);
  output.reset(theCommands, 1);
  output[0].MakeAtom(theCommands.opSequence(), theCommands);
  output.children.AddListOnTop(theList);
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
  output[0].MakeAtom(theCommands.opTimes(), theCommands);
  return output[2].AssignValue(tempRat, theCommands);
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
{ if (this->theBoss==0 || other.theBoss==0)
    return this->AreEqualExcludingChildren(other) && this->children.size==0;
  return this->theBoss->ExpressionsAreEqual(*this, other);
}

template <class dataType>
bool CommandList::innerExtractPMTDtreeContext
(CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::innerExtractPMTDtreeContext");
  if (theCommands.RecursionDeptH>theCommands.MaxRecursionDeptH)
  { std::stringstream out;
    out << "Max recursion depth of " << theCommands.MaxRecursionDeptH
    << " exceeded while trying to evaluate to polynomial an expression "
    << "(i.e., your polynomial expression is too large).";
    return output.SetError(out.str(), theCommands);
  }
//  std::cout << "<br>Extracting context from: " << input.ToString();
  if (input.IsListStartingWithAtom(theCommands.opTimes()) ||
      input.IsListStartingWithAtom(theCommands. opPlus()) ||
      input.IsListStartingWithAtom(theCommands.opMinus()) )
  { output.reset(theCommands, 1);
    output[0].MakeAtom(theCommands.opContext(), theCommands);
    Expression newContext, intermediateContext;
    for (int i=1; i<input.children.size; i++)
      if (theCommands.innerExtractPMTDtreeContext<dataType>(theCommands, input[i], newContext))
      { if (!Expression::ContextMergeContexts(newContext, output, intermediateContext))
          return false;
        output=intermediateContext;
      } else
        return false;
    return true;
  }
  int thePower;
  if (input.IsListNElementsStartingWithAtom(theCommands.opThePower(), 3))
    if (input[2].IsSmallInteger(&thePower))
      return theCommands.innerExtractPMTDtreeContext<dataType>(theCommands, input[1], output);
  Expression theContext=input.GetContext();
  if(input.IsAtoM(theCommands.opContext()))
  { theCommands.Comments << "Error in context extraction: encountered context keyword. ";
    return false;
  }
  output.reset(theCommands, 2);
  output[0].MakeAtom(theCommands.opContext(), theCommands);
  output[1].reset(theCommands, 2);
  output[1][0].MakeAtom(theCommands.opPolynomialVariables(), theCommands);
  output[1][1]=input;
  return true;
}

template <class dataType>
bool CommandList::outerExtractAndEvaluatePMTDtree
(CommandList& theCommands, const Expression& input, Expression& output)
{ Expression contextE;
  if (!theCommands.innerExtractPMTDtreeContext<dataType>(theCommands, input, contextE))
    return false;
//  std::cout << "<br>Extracted context from " << input.ToString() << ": "
//  << contextE.ToString();
  return theCommands.EvaluatePMTDtree<dataType>(contextE, input, output);
}

template <class dataType>
bool CommandList::EvaluatePMTDtree
(const Expression& inputContext, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::EvaluatePMTDtree");
  RecursionDepthCounter theRecursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { this->Comments << "Max recursion depth of " << this->MaxRecursionDeptH
    << " exceeded while trying to evaluate polynomial expression "
    << "(i.e. your polynomial expression is too large).";
    return false;
  }
  dataType outputData;
//  std::cout << "<br>Context:" <<  inputContext.ToString()
//  << " Evaluating PMTD tree of " << input.ToString();
  if (input.IsListStartingWithAtom(this->opTimes()) ||
      input.IsListStartingWithAtom(this-> opPlus()) )
  { for (int i=1; i<input.children.size; i++)
    { if (!this->EvaluatePMTDtree<dataType>(inputContext, input[i], output))
        return false;
      //std::cout << "<hr>Status outputBuffer data after variable change: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
      //std::cout << "<hr>Status bufferData data after variable change: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
      if (input[0].IsAtoM(this->opTimes()))
      { if (i==0)
          outputData=output.GetValuE<dataType>();
        else
        { //std::cout << "<hr>Multiplying: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
          //<< " and " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
          outputData*=output.GetValuE<dataType>();
          //std::cout << "<br>Result: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat) << "<br>";
        }
      } else if (input[0].IsAtoM(this->opPlus()))
      { //std::cout << "<hr>Status outputBuffer data before addition: " << outputBuffer.ToString(this->theGlobalVariableS->theDefaultLieFormat);
        if (i==0)
        { outputData=output.GetValuE<dataType>();
//          std::cout << "<hr> outputBuffer has been set to: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
//          << ", which should equal the bufferData: " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
        } else
        { //std::cout << "<hr>Adding: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
          //<< " and " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
          outputData+=output.GetValuE<dataType>();
          //std::cout << "<hr>Result: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat) << "<br>";
        }
      }
    }
    return output.AssignValueWithContext(outputData, inputContext, *this);
  }
  int thePower;
  if (input.IsListNElementsStartingWithAtom(this->opThePower(), 3))
    if (input[2].IsSmallInteger(&thePower))
    { if(!this->EvaluatePMTDtree<dataType>(inputContext, input[1], output))
        return false;
      outputData=output.GetValuE<dataType>();
      outputData.RaiseToPower(thePower);
      return output.AssignValueWithContext(outputData, inputContext, *this);
    }
//  std::cout << "<br>input: " << input.ToString();
  if (input.IsOfType<Rational>())
  { outputData=input.GetValuE<Rational>();//<-type conversion here
    return output.AssignValueWithContext(outputData, inputContext, *this);
  }
  if (!inputContext.ContextGetPolynomialMonomial(input, outputData, *this->theGlobalVariableS))
  { this->Comments << "Expression" << input.ToString()
    << "  is not contained as a variable image in the context "
    << inputContext.ToString() << "."
    << " This could be a programming error. ";
    return false;
  }
  return output.AssignValueWithContext(outputData, inputContext, *this);
}

bool CommandList::ExpressionsAreEqual
  (const Expression& left, const Expression& right)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Error: maximium recursion depth of " << this->MaxRecursionDeptH << " exceeded while comparing expressions: " ;
    out << left.ToString() << " and " << right.ToString();
    std::cout << out.str();
    this->evaluationErrors.AddOnTop(out.str());
    return false;
  }
  if (!left.AreEqualExcludingChildren(right))
    return false;
  for (int i=0; i<left.children.size; i++)
    if (!this->ExpressionsAreEqual (left.children[i], right.children[i]))
      return false;
  return true;
}

bool CommandList::ExpressionMatchesPattern
(const Expression& thePattern, const Expression& input, BoundVariablesSubstitution& matchedExpressions,
 std::stringstream* theLog)
{ MacroRegisterFunctionWithName("CommandList::ExpressionMatchesPattern");
  RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (!(thePattern.theBoss==this && input.theBoss==this))
  { std::cout << "This is a programming error. Either a pattern or an input has a wrongly "
    << " initialized owner. The error is certainly in the preceding code; here "
    << "is a stack trace "
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
  MacroRegisterFunctionWithName("CommandList::EvaluateExpressionReturnFalseIfExpressionIsBound");
  StackMaintainerRules theRuleStackMaintainer(this);
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
  theExpressionWithContext[0].MakeAtom(this->opSequence(), *this);
  theExpressionWithContext[2]=input;
  theExpressionWithContext[1].AssignValue(this->RuleContextIdentifier, *this);
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
  while (ReductionOcurred && !this->flagAbortComputationASAP)
  { ReductionOcurred=false;
    counterNumTransformations++;
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
/////-------Evaluating children -------
    //bool foundError=false;
    for (int i=0; i<output.children.size && !this->flagAbortComputationASAP; i++)
    { Expression& currentChild=output[i];
      bool tempBool=true;
      this->EvaluateExpression
      (currentChild, currentChild, bufferPairs, tempBool);
      if (!tempBool)
        outputIsFree=false;
      if (currentChild.IsError())
      { this->flagAbortComputationASAP=true;
        break;
      }
      if (output.IsListNElementsStartingWithAtom(this->opEndStatement()))
        if (currentChild.IsListNElementsStartingWithAtom(this->opDefine()) ||
            currentChild.IsListNElementsStartingWithAtom(this->opDefineConditional()))
        { this->RuleStack.AddOnTop(currentChild);
          this->RuleContextIdentifier++;
          //std::cout << "Added to rule stack rule " << currentChild.ToString();
        }
    }
    if (this->flagAbortComputationASAP)
      break;
    //->/////-------Default operation handling-------
    if (this->outerStandardFunction(*this, output, tempE))
    { ReductionOcurred=true;
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
      //bool doLog=this->RuleStack.size==3;
      if(this->ProcessOneExpressionOnePatternOneSub
         (currentPattern, output, bufferPairs, &this->Comments
          //,doLog
          ))
      { ReductionOcurred=true;
        break;
      }
    }
/////-------User-defined pattern matching end------
  }
  this->ExpressionStack.PopIndexSwapWithLast(this->ExpressionStack.size-1);
  if (output.IsListStartingWithAtom(this->opBind()))
    outputIsFree=false;
  return true;
}

Expression* CommandList::PatternMatch
(Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
 Expression* condition, std::stringstream* theLog, bool logAttempts)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
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
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, theLog))
    return 0;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>found pattern: " << theExpression.ToString() << " -> "
    << thePattern.ToString() << " with " << bufferPairs.ToString();
  if (condition==0)
    return &theExpression;
  Expression tempExp=*condition;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.ToString();
  this->SpecializeBoundVars(tempExp, bufferPairs);
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specialized condition: " << tempExp.ToString() << "; evaluating...";
  BoundVariablesSubstitution tempPairs;
  bool isFree;
  this->EvaluateExpression(tempExp, tempExp, tempPairs, isFree);
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>The evaluated specialized condition: " << tempExp.ToString() << "; evaluating...";
  if (tempExp.IsEqualToOne())
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
  for (int i=0; i<toBeSubbedIn.children.size; i++)
    this->SpecializeBoundVars(toBeSubbedIn[i], matchedPairs);
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
(Expression& thePattern, Expression& theExpression, BoundVariablesSubstitution& bufferPairs,
 std::stringstream* theLog, bool logAttempts)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (!thePattern.IsListNElementsStartingWithAtom(this->opDefine(), 3) &&
      !thePattern.IsListNElementsStartingWithAtom(this->opDefineConditional(), 4))
    return false;
  if (theLog!=0 && logAttempts)
  { (*theLog) << "<hr>attempting to reduce expression " << theExpression.ToString();
    (*theLog) << " by pattern " << thePattern.ToString();
  }
  Expression& currentPattern=thePattern[1];
  Expression* theCondition=0;
  bool isConditionalDefine=
  thePattern.IsListNElementsStartingWithAtom(this->opDefineConditional(), 4);
  if (isConditionalDefine)
    theCondition=&thePattern.children[2];
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
{ std::string lookAheadToken;
  std::stringstream errorLog;
  (*this->CurrentSyntacticStacK).ReservE((*this->CurrrentSyntacticSouP).size+this->numEmptyTokensStart);
  (*this->CurrentSyntacticStacK).SetSize(this->numEmptyTokensStart);
  this->registerNumNonClosedBeginArray=0;
  for (int i=0; i<this->numEmptyTokensStart; i++)
    (*this->CurrentSyntacticStacK)[i]=this->GetEmptySyntacticElement();
  this->parsingLog="";
  this->NonBoundVariablesStack.SetSize(1);
  this->BoundVariablesStack.SetSize(1);
  this->NonBoundVariablesStack.LastObject()->Clear();
  this->BoundVariablesStack.LastObject()->Clear();
  for (this->counterInSyntacticSoup=0; this->counterInSyntacticSoup<(*this->CurrrentSyntacticSouP).size; this->counterInSyntacticSoup++)
  { if (this->counterInSyntacticSoup+1<(*this->CurrrentSyntacticSouP).size)
      lookAheadToken=this->controlSequences[(*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup+1].controlIndex];
    else
      lookAheadToken=";";
    (*this->CurrentSyntacticStacK).AddOnTop((*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup]);
    while(this->ApplyOneRule(lookAheadToken))
    {}
  }
  bool success=false;
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
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
  out << this->theProgramExpression.ToString
  (&this->theGlobalVariableS->theDefaultFormat, &StartingExpression);
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
{ if (!this->IsContext() )
    return -1;
  for (int i=1; i<this->children.size; i++)
    if (this->children[i].IsListNElementsStartingWithAtom(this->theBoss->opSSLieAlg(), 2))
      return this->children[i][1].theData;
  return -1;
}

Expression Expression::GetContext()const
{ this->CheckInitialization();
  for (int i=1; i<this->children.size; i++)
    if (this->children[i].IsContext())
      return this->children[i];
  Expression output;
  output.reset(*this->theBoss,1);
  output[0].MakeAtom(this->theBoss->opContext(), *this->theBoss);
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
    if (this->children[i].format==this->formatMatrixRow &&
        this->children[i].IsSequenceNElementS())
      theMax=MathRoutines::Maximum(this->children[i].children.size-1, theMax);
  return theMax;
}

bool Expression::operator>(const Expression& other)const
{ if (this->IsOfType<Rational>() && other.IsOfType<Rational>())
    return this->GetValuE<Rational>()>other.GetValuE<Rational>();
  return this->ToString()>other.ToString();
}

bool Expression::ToStringData
(std::string& output, FormatExpressions* theFormat, Expression* startingExpression)const
{ std::stringstream out;
  bool result=false;
  bool isFinal=theFormat==0 ? false : theFormat->flagExpressionIsFinal;
  if (this->IsAtoM())
  { if (this->theData< this->theBoss->operationS.size && this->theData>=0)
      out << this->theBoss->operationS[this->theData];
    else
      out << "(unknown~ atom~ of~ value~ " << this->theData << ")";
    result=true;
  } else if (this->IsOfType<std::string>())
  { if (isFinal)
      out << this->GetValuE<std::string>();
    else
      out << "(string~ not~ shown~ to~ avoid~ javascript~ problems)";
    result=true;
  } else if (this->IsOfType<Rational>())
  { out << this->GetValuE<Rational>().ToString();
    result=true;
  } else if (this->IsOfType<Polynomial<Rational> >())
  { out << "Polynomial{}";
    Expression contextE=this->GetContext();
    out << "(" << contextE.ToString() << ", "
    << this->GetValuE<Polynomial<Rational> >().ToString() << ")";
    result=true;
  } else if (this->IsOfType<SemisimpleLieAlgebra>())
  { out << "SemisimpleLieAlgebra{}("
    << this->GetValuE<SemisimpleLieAlgebra>().GetLieAlgebraName()
    << ")";
    result=true;
  } else if (this->IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
  { out << this->GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >().ToString();
    result=true;
  } else if (this->IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { out << this->GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().ToString();
    result=true;
  }
  output=out.str();
  return result;
}

std::string Expression::ToString
(FormatExpressions* theFormat, Expression* startingExpression)const
{ if (this->theBoss!=0)
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
  int charCounter=0;
  std::string tempS;
  if (this->ToStringData(tempS, theFormat, startingExpression))
    out << tempS;
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDefine(), 3))
  { std::string firstE=this->children[1].ToString(theFormat);
    std::string secondE=this->children[2].ToString(theFormat);
    if (this->children[1].IsListStartingWithAtom(this->theBoss->opDefine()))
      out << "(" << firstE << ")";
    else
      out << firstE;
    out << ":=";
    if (this->children[2].IsListStartingWithAtom(this->theBoss->opDefine()))
      out << "(" << secondE << ")";
    else
      out << secondE;
  } else if (this->IsListStartingWithAtom(this->theBoss->opIsDenotedBy()))
    out << this->children[1].ToString(theFormat)
    << ":=:" << this->children[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opDefineConditional()))
    out <<  this->children[1].ToString(theFormat) << " :if "
    << this->children[2].ToString(theFormat)
    << ":=" << this->children[3].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opDivide(), 3))
  { std::string firstE= this->children[1].ToString(theFormat);
    std::string secondE=this->children[2].ToString(theFormat);
    bool firstNeedsBrackets=
    !(this->children[1].IsListStartingWithAtom(this->theBoss->opTimes())||
      this->children[1].IsListStartingWithAtom(this->theBoss->opDivide()));
    bool secondNeedsBrackets= !this->children[2].IsOfType<Rational>();
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
  { out << this->children[1].ToString(theFormat)
    << "\\otimes " << this->children[2].ToString(theFormat);
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opTimes(), 3))
  { std::string firstE= this->children[1].ToString(theFormat);
    std::string secondE=this->children[2].ToString(theFormat);
    bool firstNeedsBrackets=
    (!(this->children[1].IsListStartingWithAtom(this->theBoss->opTimes())||
      this->children[1].IsListStartingWithAtom(this->theBoss->opDivide())))
    && !this->children[1].IsOfType<Rational>();
    bool secondNeedsBrackets=true;
    if (this->children[2].IsOfType<Rational>())
      secondNeedsBrackets=this->children[2].GetValuE<Rational>().IsNonPositive();
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
  { std::string firstE=this->children[1].ToString(theFormat);
    std::string secondE=this->children[2].ToString(theFormat);
    if (this->children[1].IsOfType<Rational>())
      out << firstE;
    else
      out << "(" << firstE << ")";
    out << "^{" << secondE << "}";
  }
  else if (this->IsListStartingWithAtom(this->theBoss->opPlus() ))
  { assert(this->children.size>=2);
    std::string tempS2= this->children[1].ToString(theFormat);
    tempS=this->children[2].ToString(theFormat);
    out << tempS2;
//    std::cout << "<br>here i am! tempS2.size=" << tempS2.size() << ", allowNewLine="
//    << allowNewLine;
    if (allowNewLine && tempS2.size()>100)
      out << "\\\\\n";
    if (tempS.size()>0)
      if (tempS[0]!='-')
        out << "+";
    out << tempS;
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 2))
    out << "-" << this->children[1].ToString(theFormat);
  else if (this->IsListNElementsStartingWithAtom(this->theBoss->opMinus(), 3))
  { if (!(this->children.size==3))
    { std::cout << "This is a programming error: the minus function expects"
      << "1 or 2 arguments, instead there are " << this->children.size-1
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    out << this->children[1].ToString(theFormat)
    << "-" << this->children[2].ToString(theFormat);
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opBind(), 2))
    out << "{{" << this->children[1].ToString(theFormat) << "}}";
  else if (this->IsListStartingWithAtom(this->theBoss->opApplyFunction()))
  { assert(this->children.size>=2);
    switch(this->format)
    { case Expression::formatFunctionUseUnderscore:
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=false;
        out <<  this->children[1].ToString(theFormat)
        << "_{" << this->children[2].ToString(theFormat) << "}";
        if (allowNewLine)
          theFormat->flagExpressionNewLineAllowed=true;
        break;
      case Expression::formatFunctionUseCdot:
        out <<  this->children[1].ToString
        (theFormat)
        << "\\cdot(" << this->children[1].ToString(theFormat) << ")";
        break;
      default:
        out << this->children[1].ToString(theFormat)
        << "{}(" << this->children[2].ToString(theFormat) << ")";
        break;
    }
  } else if (this->IsListStartingWithAtom(this->theBoss->opEqualEqual()))
    out << this->children[1].ToString(theFormat)
    << "==" << this->children[2].ToString(theFormat);
  else if (this->IsListStartingWithAtom(this->theBoss->opSequence()))
  { switch (this->format)
    { case Expression::formatMatrixRow:
        for (int i=1; i<this->children.size; i++)
        { out << this->children[i].ToString(theFormat);
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
        { out << this->children[i].ToString(theFormat);
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
        { std::string currentChildString=this->children[i].ToString(theFormat);
          out << currentChildString;
          charCounter+=currentChildString.size();
          if (i!=this->children.size-1)
          { out << ", ";
            if (allowNewLine && charCounter >200 )
              out << "\\\\";
          }
          charCounter%=200;
        }
        out << ")";
        break;
    }
  } else if (this->IsListStartingWithAtom(this->theBoss->opLieBracket()))
    out << "[" << this->children[1].ToString(theFormat)
    << "," << this->children[2].ToString(theFormat)
    << "]";
  else if (this->IsListStartingWithAtom(this->theBoss->opUnion()))
    out << this->children[1].ToString(theFormat)
    << "\\cup " << this->children[2].ToString(theFormat)
    ;
  else if (this->IsListStartingWithAtom(this->theBoss->opUnionNoRepetition()))
    out << this->children[1].ToString(theFormat)
    << "\\sqcup " << this->children[2].ToString(theFormat)
    ;
  else if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
  { if (startingExpression==0)
      out << "<table>";
    for (int i=1; i<this->children.size; i++)
    { out << "<tr><td valign=\"top\">";
      bool createTable=(startingExpression!=0);
      if (createTable)
        createTable=
        (startingExpression->IsListStartingWithAtom(this->theBoss->opEndStatement()) &&
         startingExpression->children.size==this->children.size);
      if (createTable)
      { out << "<hr> "
        << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL
        (startingExpression->children[i].ToString(theFormat));
        if (i!=this->children.size-1)
          out << ";";
        out << "</td><td valign=\"top\"><hr>";
        if (!this->children[i].IsOfType<std::string>() || !isFinal)
          out << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL
          (this->children[i].ToString(theFormat));
        else
          out << this->children[i].GetValuE<std::string>();
        if (i!=this->children.size-1)
          out << ";";
      }
      else
      { out << this->children[i].ToString(theFormat);
        if (i!=this->children.size-1)
          out << ";";
      }
//      out << "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp Context index: " << this->IndexBoundVars;
      out << "</td></tr>";
    }
    if (startingExpression==0)
      out << "</table>";
  } else if (this->IsListNElementsStartingWithAtom(this->theBoss->opError(), 2))
  { if (isFinal)
    { this->theBoss->NumErrors++;
      out << "(Error~ " << this->theBoss->NumErrors << ":~ see~ comments)";
      this->theBoss->Comments << "<br>Error " << this->theBoss->NumErrors
      << ". " << this->children[1].ToString(theFormat);
    } else
      out << "(Error)";
  } else if (this->children.size==1)
  { out << this->children[0].ToString(theFormat);
  } else if (this->children.size>=2)
  { out << this->children[0].ToString(theFormat);
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
    { out << this->children[i].ToString(theFormat);
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
    << "Javascript LaTeXing courtesy of <a href=\"http://www.math.union.edu/~dpvc/jsmath/\">jsmath</a>: many thanks for your great work!</td></tr>";
    if (this->IsListStartingWithAtom(this->theBoss->opEndStatement()))
      outTrue << out.str();
    else
    { outTrue << "<tr><td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(startingExpression->ToString(theFormat));
      if (this->IsOfType<std::string>() && isFinal)
        outTrue << "</td><td>" << out.str() << "</td></tr>";
      else
        outTrue << "</td><td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(out.str()) << "</td></tr>";
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
    out << " " << this->children[i].Lispify();
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

bool Expression::IsOperation(std::string* outputWhichOperation)const
{ if (this->theBoss==0)
    return false;
  if (this->IsLisT())
    return false;
  if (this->theData<0 || this->theData>=this->theBoss->operationS.size)
    return false;
  if (outputWhichOperation!=0)
    *outputWhichOperation=this->theBoss->operationS[this->theData];
  return true;
}

int CommandList::AddOperationNoRepetitionOrReturnIndexFirst(const std::string& theOpName)
{ int result=this->operationS.GetIndex(theOpName);
  if (result==-1)
  { this->operationS.AddOnTop(theOpName);
    this->FunctionHandlers.SetSize(this->operationS.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
    result=this->operationS.size-1;
  }
  return result;
}

void CommandList::AddOperationNoRepetitionAllowed(const std::string& theOpName)
{ if (this->operationS.Contains(theOpName))
  { std::cout << "This is a programming error: operation " << theOpName << " already created. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->operationS.AddOnTop(theOpName);
  this->FunctionHandlers.SetSize(this->operationS.size);
  this->FunctionHandlers.LastObject()->SetSize(0);
}

void CommandList::AddOperationBinaryInnerHandlerWithTypes
  (const std::string& theOpName, Expression::FunctionAddress innerHandler,
   int leftType, int rightType,
   const std::string& opDescription, const std::string& opExample,
   bool visible)
{ int indexOp=this->operationS.GetIndex(theOpName);
  if (indexOp==-1)
  { this->operationS.AddOnTop(theOpName);
    indexOp=this->operationS.size-1;
    this->FunctionHandlers.SetSize(this->operationS.size);
    this->FunctionHandlers.LastObject()->SetSize(0);
  }
  Function innerFunction
  (innerHandler, 0, opDescription, opExample, true, visible)
  ;
  innerFunction.theArgumentTypes.reset(*this, 2);
  innerFunction.theArgumentTypes[0].MakeAtom(leftType, *this);
  innerFunction.theArgumentTypes[1].MakeAtom(rightType, *this);
  this->FunctionHandlers[indexOp].ReservE(5);
  this->FunctionHandlers[indexOp].AddOnTop(innerFunction);
}

void CommandList::AddOperationHandler
  (const std::string& theOpName, Expression::FunctionAddress handler,
   const std::string& opArgumentListIgnoredForTheTimeBeing,
   const std::string& opDescription, const std::string& opExample,
   bool isInner, bool visible)
{ int indexOp=this->operationS.GetIndex(theOpName);
  if (indexOp==-1)
  { this->operationS.AddOnTop(theOpName);
    indexOp=this->operationS.size-1;
    this->FunctionHandlers.SetSize(this->operationS.size);
  }
  if (opArgumentListIgnoredForTheTimeBeing!="")
  { std::cout << "This section of code is not implemented yet. Crashing to let you know. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Function theFun
  (handler, 0, opDescription, opExample, isInner, visible)
  ;
  this->FunctionHandlers[indexOp].ReservE(5);
  this->FunctionHandlers[indexOp].AddOnTop(theFun);
}

std::string CommandList::ElementToStringNonBoundVars()
{ std::stringstream out;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  out << "<br>\n" << this->operationS.size << " operations and global variables " << " (= "
  << this->NumPredefinedVars  << " predefined + "
  << this->operationS.size-this->NumPredefinedVars
  << " user-defined). <br>Predefined: \n<br>\n";
  for (int i=0; i<this->operationS.size; i++)
  { out << openTag1 << this->operationS[i] << closeTag1;
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
    if (i!=this->operationS.size-1)
    { out << ", ";
      if (i==this->NumPredefinedVars-1 )
        out << "<br>user-defined:\n<br>\n";
    }
  }
  return out.str();
}

std::string Function::GetString(CommandList& theBoss)
{ std::stringstream out2;
  if (this->flagIamVisible)
  { std::stringstream out;
    out << this->theDescription;
    out << "Address: " << std::hex << (int) this->theFunction << ". ";
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
  for (int i=0; i<this->operationS.size; i++)
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
  for (int i=0; i<this->operationS.size; i++)
    if (this->FunctionHandlers[i].size>0)
      for (int j=0; j<this->FunctionHandlers[i].size; j++)
      { if (found)
          out << "<br>\n";
        found=true;
        out << openTag2 << this->operationS[i] << closeTag2;
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
  out2 << "<br>Number small rational number additions: " << Rational::TotalSmallAdditions
  << " (# successful calls Rational::TryToAddQuickly)";
  out2 << "<br>Number small rational number multiplications: "
  << Rational::TotalSmallMultiplications
  << " (# successful calls Rational::TryToMultiplyQuickly)";
  out2 << "<br>Number small number gcd calls: " << Rational::TotalSmallGCDcalls
  << " (# calls of Rational::gcd)";
  out2 << "<br>Number large integer additions: " << Rational::TotalLargeAdditions
  << " (# calls LargeIntUnsigned::AddNoFitSize)";
  out2 << "<br>Number large integer multiplications: " << Rational::TotalLargeMultiplications
  << " (# calls LargeIntUnsigned::MultiplyBy)";
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
  out << "<br>\n Variables (" << this->operationS.size << " = "
  << this->NumPredefinedVars << " predefined + " << this->operationS.size-this->NumPredefinedVars << " user-defined):<br>\n";
  for (int i=0; i<this->operationS.size; i++)
  { out << "\n" << i << ": " << openTag1 << this->operationS[i] << closeTag1;
    if(i!=this->operationS.size-1)
      out << ", ";
  }
  out << this->ElementToStringNonBoundVars();
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
  tempRat*=10;
  tempRat+=right.theData.GetValuE<Rational>();
  left.theData.AssignValue(tempRat, *this);
  this->DecreaseStackSetCharacterRanges(1);
  return true;
}

bool CommandList::ReplaceXEXEXByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& lefT = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr[0].MakeAtom(this->GetOperationIndexFromControlIndex(theControlIndex), *this);
  newExpr[1]=lefT.theData;
  newExpr[2]=right.theData;
  newExpr.format=formatOptions;
  result.theData=newExpr;
  result.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRanges(4);
  return true;
}

bool CommandList::ReplaceEEndCommandEbyE()
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  if (left.theData.IsListNElementsStartingWithAtom(this->opEndStatement()))
    left.theData.children.AddOnTop(right.theData);
  else
  { Expression newExpr;
    newExpr.reset(*this, 3);
    newExpr[0].MakeAtom(this->opEndStatement(), *this);
    newExpr[1]=(left.theData);
    newExpr[2]=(right.theData);
    newExpr.format=Expression::formatDefault;
    left.theData=newExpr;
  }
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXEByE(int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 2);
  newExpr[0]=left.theData;
  newExpr[1]=right.theData;
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXEByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, 3);
  newExpr[0].MakeAtom(this->GetOperationIndexFromControlIndex(theControlIndex), *this);
  newExpr[1]=(left.theData);
  newExpr[2]=(right.theData);
  newExpr.format=formatOptions;
  left.theData=newExpr;
  this->DecreaseStackSetCharacterRanges(2);
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
  this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool CommandList::ReplaceXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRanges(1);
  return true;
}

bool CommandList::ReplaceXXXXXByCon(int theCon, int inputFormat)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=theCon;
  this->DecreaseStackSetCharacterRanges(4);
  return true;
}

bool CommandList::ReplaceXXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRanges(3);
  return true;
}

bool CommandList::ReplaceXXXXByConCon(int con1, int con2, int inputFormat1, int inputFormat2)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].controlIndex=con2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3].theData.format=inputFormat2;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRanges(2);
  return true;
}

bool CommandList::ReplaceXXXXByCon(int con1, int inputFormat1)
{ (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].controlIndex=con1;
  (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4].theData.format=inputFormat1;
  this->DecreaseStackSetCharacterRanges(3);
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
  { int theNewIndex=polyVarsElargerContext.children.GetIndex(polyVarsEsmallContext[i]);
    if (theNewIndex==-1)
      return false;
    output[i].MakeMonomiaL(theNewIndex, 1, 1, numVars);
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
    << this->operationS[theBoundVar]
    << " is first used to denote a non-bound variable"
    << " but later to denote a bound variable. This is not allowed. ";
    theElt.errorString=out.str();
    theElt.controlIndex=this->conError();
    this->DecreaseStackSetCharacterRanges(numXs);
    this->ReplaceXXYByY();
    return true;
  }
  if (!this->IsBoundVarInContext(theBoundVar))
    this->BoundVariablesStack.LastObject()->AddOnTopNoRepetition(theBoundVar);
  theElt.theData.reset(*this, 2);
  theElt.theData[0].MakeAtom(this->opBind(), *this);
  theElt.theData[1].MakeAtom(theBoundVar, *this);
  theElt.controlIndex=this->conExpression();
//  std::cout << ", got to element: " << theElt.theData.ToString();
  this->DecreaseStackSetCharacterRanges(numXs);
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
    theElt.theData[0].MakeAtom(this->opBind(), *this);
    theElt.theData[1].MakeAtom(theBoundVar, *this);
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
{ this->PhysicalPathServerBase=inputPathBinaryBaseIsFolderBelow+"../";
  this->DisplayPathServerBase=inputDisplayPathBase;

  this->PhysicalPathOutputFolder=this->PhysicalPathServerBase+"output/";
  this->DisplayPathOutputFolder= this->DisplayPathServerBase + "output/";

  this->userLabel=scrambledIP;

  this->PhysicalNameDefaultOutput=this->PhysicalPathOutputFolder+"default"+this->userLabel+"output";
  this->DisplayNameDefaultOutputNoPath="default"+this->userLabel+"output";
  this->DisplayNameDefaultOutput=this->DisplayPathOutputFolder+this->DisplayNameDefaultOutputNoPath;

  this->indicatorFileNamE=this->PhysicalPathOutputFolder + "indicator" + this->userLabel + ".html" ;
  this->indicatorFileNameDisplaY=this->DisplayPathOutputFolder +"indicator" + this->userLabel+ ".html" ;
}

bool CommandList::fWriteGenVermaModAsDiffOperators
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::fWriteGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.SetSize(1);
  Expression theContext;
  Selection theParSel;
  SemisimpleLieAlgebra* theSSalgebra;
  if (!theCommands.fGetTypeHighestWeightParabolic<Polynomial<Rational> >
      (theCommands, input, output, theHWs[0], theParSel, theContext, theSSalgebra,
       theCommands.innerPolynomial))
    return output.SetError
    ("Failed to extract type, highest weight, parabolic selection", theCommands);
  if (output.IsError())
    return true;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.fWriteGenVermaModAsDiffOperatorInner
  (theCommands, input, output, theHWs, theContext, theParSel,
   theSSalgebra->indexInOwner);
}

bool CommandList::fFreudenthalEval
(CommandList& theCommands, const Expression& input, Expression& output)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  SemisimpleLieAlgebra* theSSalg;
  Expression context;
  if (!theCommands.fGetTypeHighestWeightParabolic<Rational>
      (theCommands, input, output, hwFundamental, tempSel, context, theSSalg, 0))
    return output.SetError("Failed to extract highest weight and algebra", theCommands);
  if (output.IsError())
    return true;
  if (tempSel.CardinalitySelection>0)
    return output.SetError("Failed to extract highest weight. ", theCommands);
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg->theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight
  (hwSimple, theCommands.theObjectContainer.theLieAlgebras, theSSalg->indexInOwner)
  ;
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
  const SemisimpleLieAlgebra& ownerAlgebra=input[1].GetValuE<SemisimpleLieAlgebra>();
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
    theElt.MakeGenerator
    (actualIndex, theCommands.theObjectContainer.theLieAlgebras, ownerAlgebra.indexInOwner);
    RationalFunctionOld rfOne, rfZero;
    rfOne.MakeOne(theCommands.theGlobalVariableS);
    rfZero.MakeZero(theCommands.theGlobalVariableS);
    ElementUniversalEnveloping<RationalFunctionOld> tempUE;
    //  std::cout << "tempelt: " << tempElt.ToString();
    tempUE.AssignElementLieAlgebra
    (theElt, theCommands.theObjectContainer.theLieAlgebras,
     ownerAlgebra.indexInOwner, rfOne, rfZero);
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
  tempElt.MakeGenerator
  (actualIndeX, theCommands.theObjectContainer.theLieAlgebras, ownerAlgebra.indexInOwner);
  RationalFunctionOld rfOne, rfZero;
  rfOne.MakeOne(theCommands.theGlobalVariableS);
  rfZero.MakeZero(theCommands.theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunctionOld> tempUE;
  tempUE.AssignElementLieAlgebra
  (tempElt, theCommands.theObjectContainer.theLieAlgebras,
   ownerAlgebra.indexInOwner, rfOne, rfZero);
  return output.AssignValue(tempUE, theCommands);
}
