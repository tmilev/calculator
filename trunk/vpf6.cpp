//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6cpp(__FILE__, "Implementation file for the calculator parser. ");

//If you get a specialization after instantiation error:
//the following template specialization funcitons must appear
//here and nowhere else (discovered through extremely painful experimentation).

template < >
const Rational& Data::GetValuE()const
{ if (this->type!=this->typeRational)
  { std::cout << "This is a programming error. Rational Data::GetValuE  is called on Data of type "
    << this->ElementToStringDataType()
    << ". Please debug file " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theRationals[this->theIndex];
}

template < >
const std::string& Data::GetValuE()const
{ if (this->type!=this->typeString)
  { std::cout << "This is a programming error.<b> std::string </b> Data::GetValuE  is called on Data of type <b> "
    << this->ElementToStringDataType()
    << "</b>. " <<  CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theStrings[this->theIndex];
}

template < >
const VariableNonBound& Data::GetValuE()const
{ if (this->type!=this->type)
  { std::cout << "This is a programming error. VariableNonBound Data::GetValuE  is called on Data of type "
    << this->ElementToStringDataType()
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theNonBoundVars[this->theIndex];
}

template < >
const RationalFunctionOld& Data::GetValuE()const
{ if (this->type!=this->typeRationalFunction)
  { std::cout << "This is a programming error. RationalFunctionOld Data::GetValuE  is called on Data of type "
    << this->ElementToStringDataType()
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theRFs[this->theIndex];
}

template < >
const ElementUniversalEnveloping<RationalFunctionOld>& Data::GetValuE()const
{ if (this->type!=this->typeElementUE)
  { std::cout << "This is a programming error. ElementUniversalEnveloping_RationalFunction Data::GetValuE  is called on Data of type "
    << this->ElementToStringDataType()
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theUEs[this->theIndex];
}

template < >
const Polynomial<Rational>& Data::GetValuE()const
{ if (this->type!=this->typePoly)
  { std::cout << "This is a programming error. Polynomial_Rational Data::GetValuE  is called on Data of type "
    << this->ElementToStringDataType()
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.thePolys[this->theIndex];
}

template < >
const LittelmannPath& Data::GetValuE()const
{ if (this->type!=this->typeLSpath)
  { std::cout << "This is a programming error. LittelmannPath Data::GetValuE  is called on Data of type "
    << this->ElementToStringDataType()
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theLSpaths[this->theIndex];
}

template < >
const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& Data::GetValuE()const
{ if (this->type!=this->typeLittelmannRootOperator)
  { std::cout << "This is a programming error. MonomialTensor_int Data::GetValuE  "
    << " is called on Data of type "
    << this->ElementToStringDataType()
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theLittelmannOperators[this->theIndex];
}

template < >
const ElementTensorsGeneralizedVermas<RationalFunctionOld>& Data::GetValuE()const
{ if (this->type!=this->typeEltTensorGenVermasOverRF)
  { std::cout << "This is a programming error. ElementTensorsGeneralizedVermas_RationalFunction Data::GetValuE is called on Data of type "
    << this->ElementToStringDataType() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->owner->theObjectContainer.theTensorElts[this->theIndex];
}

template < >
const charSSAlgMod<Rational>& Data::GetValuE()const
{ if (this->type!=this->typeCharSSalgFinite)
  { std::cout << "This is a programming error. charSSAlgMod_Rational Data::GetValuE is called on Data of type "
    << this->ElementToStringDataType() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->owner->theObjectContainer.theChars[this->theIndex];
}

template < >
const AlgebraicNumber& Data::GetValuE()const
{ if (this->type!=this->typeRationalRadical)
  { std::cout << "This is a programming error. typeRationalRadical Data::GetValuE is called on Data of type "
    << this->ElementToStringDataType() << "."
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->owner->theObjectContainer.theAlgebraicNumbers[this->theIndex];
}

int Data::GetNumContextVars()const
{ if (this->theContextIndex==-1)
    return 0;
  return this->GetContext().VariableImages.size;
}

template <>
bool Data::ConvertToTypE<AlgebraicNumber>()
{ if (this->owner==0)
  { std::cout << "This is a programming error: using data without properly initialized owner. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  AlgebraicNumber output(this->owner->theObjectContainer.theAlgebraicNumberRegistry);
  switch (this->type)
  { case Data::typeRational:
      output=this->GetValuE<Rational>();
      this->MakeRationalRadical(*this->owner, output);
      return true;
    case Data::typeRationalRadical:
      return true;
    default:
//      std::cout << " No conversion found.";
      return false;
  }
}

template <>
bool Data::ConvertToTypE<Rational>()
{ switch (this->type)
  { case Data::typeRational:
      return true;
    default:
//      std::cout << " No conversion found.";
      return false;
  }
}

template <>
bool Data::ConvertToTypE<Polynomial<Rational> >()
{ if (this->theContextIndex==-1)
    return false;
  Polynomial<Rational> output;
  MemorySaving<Expression> tempExp;
  MemorySaving<Data> tempData;
  switch (this->type)
  { case Data::typeRational:
      output.MakeConst(this->GetContext().VariableImages.size, this->GetValuE<Rational>());
      this->MakePoly(*this->owner, output, this->theContextIndex);
      return true;
    case Data::typePoly:
      return true;
/*    case Data::typeVariableNonBound:
    assert(false);
      tempData.GetElement()=*this;
      tempData.GetElement().theContextIndex=-1;
      tempExp.GetElement().MakeAtom(tempData.GetElement(), *this->owner, -1);
      return true;*/
    default:
//      std::cout << " No conversion found.";
      return false;
  }
}

template <>
bool Data::ConvertToTypE<RationalFunctionOld>()
{ if (this->theContextIndex==-1)
    return false;
  MemorySaving<RationalFunctionOld> output;
  MemorySaving<Expression> tempExp;
  MemorySaving<Data> tempData;
  int tempIndex;
  switch (this->type)
  { case Data::typeRational:
      output.GetElement().MakeConst
      (this->GetContext().VariableImages.size, this->GetValuE<Rational>(),
       this->owner->theGlobalVariableS);
      this->MakeRF(*this->owner, output.GetElement(), this->theContextIndex);
      return true;
    case Data::typePoly:
      output.GetElement()=this->GetValuE<Polynomial<Rational> >();
      this->MakeRF(*this->owner, output.GetElement(), this->theContextIndex);
      return true;
    case Data::typeRationalFunction:
      return true;
    case Data::typeVariableNonBound:
      tempData.GetElement()=*this;
      tempData.GetElement().theContextIndex=-1;
      tempExp.GetElement().MakeAtom(tempData.GetElement(), *this->owner, -1);
      tempIndex=this->GetContext().VariableImages.GetIndex(tempExp.GetElement());
      if (tempIndex==-1)
        return false;
      output.GetElement().MakeOneLetterMon
      (this->GetContext().VariableImages.size, tempIndex, 1, *this->owner->theGlobalVariableS);
      this->MakeRF(*this->owner, output.GetElement(), this->theContextIndex);
      return true;
    default:
//      std::cout << " No conversion found.";
      return false;
  }
}

template <>
bool Data::ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >()
{ int theAlgebraIndex=this->GetIndexAmbientSSLieAlgebra();
  if (theAlgebraIndex==-1)
  { std::cout << "theAlgebraIndex equals -1 for data " << this->ToString();
    return false;
  }
  int numVars=this->GetNumContextVars();
  RationalFunctionOld RFOne;
  RFOne.MakeOne(numVars, this->owner->theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunctionOld> conversionBuffer;
//  std::cout << "<hr><hr>attempting to convert: " << this->ToString();
  switch (this->type)
  { case Data::typeRational:
      RFOne*=this->GetValuE<Rational>();
      conversionBuffer.MakeConst(RFOne, this->owner->theObjectContainer.theLieAlgebras, theAlgebraIndex);
      this->MakeUE(*this->owner, conversionBuffer, this->theContextIndex);
      return true;
    case Data::typePoly:
      if (this->GetValuE<Polynomial<Rational> >().NumVars!=this->GetNumContextVars())
        return false;
      RFOne*=this->GetValuE<Polynomial<Rational> >();
      conversionBuffer.MakeConst(RFOne, this->owner->theObjectContainer.theLieAlgebras, theAlgebraIndex);
      this->MakeUE(*this->owner, conversionBuffer, this->theContextIndex);
      return true;
    case Data::typeElementUE:
      return true;
    default:
//      std::cout << " No conversion found.";
      return false;
  }
}

template <>
bool Data::ConvertToTypE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()
{ switch (this->type)
  { case Data::typeEltTensorGenVermasOverRF:
      return true;
    default:
//      std::cout << " No conversion found.";
      return false;
  }
}

bool Data::SetContextResizesContextArray
(const Context& desiredNewContext)
{ if (this->theContextIndex==-1)
  { this->theContextIndex=this->owner->theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(desiredNewContext);
    return true;
  }
  int candidateIndex=this->owner->theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(desiredNewContext);
  const Context& oldContext=this->GetContext();
  if (oldContext.indexAmbientSSalgebra!=-1 && oldContext.indexAmbientSSalgebra!=desiredNewContext.indexAmbientSSalgebra)
    return false;
  PolynomialSubstitution<Rational> polySub;
  if (!oldContext.GetPolySubFromVariableSuperSet(desiredNewContext, polySub))
    return false;
  this->theContextIndex=candidateIndex;
  MemorySaving<RationalFunctionOld> tempRF;
  MemorySaving<Polynomial<Rational> > tempP;
  MemorySaving<ElementUniversalEnveloping<RationalFunctionOld> > tempEUE;
  MemorySaving<ElementTensorsGeneralizedVermas<RationalFunctionOld> > tempETGV;
  switch(this->type)
  { case Data::typeRational:
      return true;
    case Data::typeElementUE:
      tempEUE.GetElement()=this->GetUE();
      tempEUE.GetElement().Substitution(polySub);
      this->MakeUE(*this->owner, tempEUE.GetElement(), candidateIndex);
      return true;
    case Data::typePoly:
      tempP.GetElement()=this->GetValuE<Polynomial<Rational> >();
      tempP.GetElement().Substitution(polySub);
      this->MakePoly(*this->owner, tempP.GetElement(), candidateIndex);
      return true;
    case Data::typeRationalFunction:
      tempRF.GetElement()=this->GetRF();
      tempRF.GetElement().Substitution(polySub);
      this->MakeRF(*this->owner, tempRF.GetElement(), candidateIndex);
      return true;
    case Data::typeEltTensorGenVermasOverRF:
      tempETGV.GetElement()=this->GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
//      std::cout << "<br>before the sub: " << tempETGV.GetElement().ToString();
//      std::cout << "<br>ze sub is: " << polySub.ToString();
      tempETGV.GetElement().Substitution(polySub);
//      std::cout << "<br>after the sub: " << tempETGV.GetElement().ToString();
      this->MakeElementTensorGeneralizedVermas(*this->owner, tempETGV.GetElement(), candidateIndex);
      return true;
    case Data::typeVariableNonBound:
      return true;
    default:
      std::cout
      << "This is a programing error: Data::SetContextResizesContextArray does not cover the "
      << " case of data of type "
      << this->ElementToStringDataType() << ". "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
      assert(false);
      return false;
  }
}

template <class dataType>
bool Data::ConvertToTypeResizesContextArrays
(const Context& desiredNewContext)
{ if (!this->SetContextResizesContextArray(desiredNewContext))
    return false;
  return this->ConvertToTypE<dataType>();
}

template < >
bool Data::IsOfType<RationalFunctionOld>()const
{ return this->type==this->typeRationalFunction;
}

template < >
bool Data::IsOfType<Polynomial<Rational> >()const
{ return this->type==this->typePoly;
}

template < >
bool Data::IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()const
{ return this->type==this->typeEltTensorGenVermasOverRF;
}

template < >
bool Data::IsOfType<Rational>()const
{ return this->type==this->typeRational;
}

template < >
bool Data::IsOfType<VariableNonBound>()const
{ return this->type==this->typeVariableNonBound;
}

bool Expression::EvaluatesToAtom()const
{ if (this->theDatA==-1)
    return false;
  return this->theOperation!= this->theBoss->opVariableBound();
}

bool Expression::EvaluatesToRational(Rational* whichRational)const
{ if (!this->EvaluatesToAtom())
    return false;
  if (! this->GetAtomicValue().IsOfType<Rational>())
    return false;
  if (whichRational!=0)
    *whichRational=this->GetAtomicValue().GetValuE<Rational>();
  return true;
}

bool Expression::EvaluatesToVariableNonBound()const
{ if (!this->EvaluatesToAtom())
    return false;
  return this->GetAtomicValue().IsOfType<VariableNonBound>();
}

bool Data::MakeElementSemisimpleLieAlgebra
(CommandList& inputOwner, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners,
 int theDisplayIndex, std::stringstream* comments)
{ SemisimpleLieAlgebra& ownerAlgebra=inputOwners[inputIndexInOwners];
  bool isGood=(theDisplayIndex>0 && theDisplayIndex<= ownerAlgebra.GetNumPosRoots()) ||
  (theDisplayIndex<0 && theDisplayIndex>=- ownerAlgebra.GetNumPosRoots());
  if (!isGood)
  { if (comments!=0)
      *comments
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
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  int actualIndex=theDisplayIndex;
  if (actualIndex<0)
    actualIndex+=ownerAlgebra.GetNumPosRoots();
  else
    actualIndex+=ownerAlgebra.GetNumPosRoots()+ownerAlgebra.GetRank()-1;
  tempElt.MakeGenerator
  (actualIndex, inputOwners, inputIndexInOwners);
  this->owner=&inputOwner;
  this->type=this->typeElementUE;
  RationalFunctionOld rfOne, rfZero;
  rfOne.MakeOne(0, this->owner->theGlobalVariableS);
  rfZero.MakeZero(0, this->owner->theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunctionOld> tempUE;
//  std::cout << "tempelt: " << tempElt.ToString();
  tempUE.AssignElementLieAlgebra(tempElt, inputOwners, inputIndexInOwners, rfOne, rfZero);
//  std::cout << "tempue: " << tempUE.ToString();
  this->theIndex=inputOwner.theObjectContainer.theUEs.AddNoRepetitionOrReturnIndexFirst(tempUE);
  return true;
}

void Data::MakeElementTensorGeneralizedVermas
(CommandList& theBoss, ElementTensorsGeneralizedVermas<RationalFunctionOld> & theElt, int inputContextIndex)
{ this->owner=&theBoss;
  this->theIndex = this->owner->theObjectContainer.theTensorElts.AddNoRepetitionOrReturnIndexFirst(theElt);
  this->theContextIndex=inputContextIndex;
  this->type=this->typeEltTensorGenVermasOverRF;
}

template <class dataType>
bool Data::MergeContextsAndConvertToType(Context& outputInputContext)
{ if (!this->MergeContext(outputInputContext))
    return false;
  return this->ConvertToTypE<dataType>();
}

bool Data::MergeContext
  (Context& outputInputContext)
{ if (this->theContextIndex==-1)
  { this->theContextIndex=this->owner->theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(outputInputContext);
    return true;
  }
  if (!outputInputContext.MergeContextWith(this->GetContext()))
    return false;
  return this->SetContextResizesContextArray(outputInputContext);
}

bool Data::MergeContexts(Data& left, Data& right)
{ if (left.theContextIndex==right.theContextIndex)
    return true;
  if (left.theContextIndex==-1)
  { left.theContextIndex=right.theContextIndex;
    return true;
  }
  if (right.theContextIndex==-1)
  { right.theContextIndex=left.theContextIndex;
    return true;
  }
  Context outputContext=left.GetContext();
  if (!outputContext.MergeContextWith(right.GetContext()))
    return false;
  if (!left.SetContextResizesContextArray(outputContext))
    return false;//<-this shouldn't happen I believe (because of the preceding check), but too tired to try to prove it atm.
  if (!right.SetContextResizesContextArray(outputContext))
    return false;//<-same comment as above.
  return true;
}

bool Data::LieBracket
  (const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ if (left.type==Data::typeError || right.type==Data::typeError)
    return false;
  Data leftCopy=left;
  Data rightCopy=right;
  if (!Data::MergeContexts(leftCopy, rightCopy))
    return false;
  if (left.type==Data::typeRational || right.type==Data::typeRational)
  { output=Data(0, *left.owner);
    return true;
  }
  if (leftCopy.type==Data::typeElementUE || leftCopy.type==Data::typeElementUE)
  { if (!leftCopy.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >())
      return false;
    if (!rightCopy.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >())
      return false;
    ElementUniversalEnveloping<RationalFunctionOld> result;
    leftCopy.GetUE().LieBracketOnTheRight(rightCopy.GetUE(), result);
    result.Simplify(*left.owner->theGlobalVariableS);
    output.MakeUE(*left.owner, result, leftCopy.theContextIndex);
    return true;
  }
  return false;
}

bool Data::operator!=(const Data& other)const
{ return ! this->operator==(other);
}

Data::Data(const Rational& x, CommandList& inputOwner)
{ this->owner=&inputOwner;
  this->type=this->typeRational;
  this->theIndex=this->owner->theObjectContainer.theRationals.AddNoRepetitionOrReturnIndexFirst(x);
  this->theContextIndex=-1;
}

bool Data::IsSmallInteger(int* whichInteger)const
{ switch (this->type)
  { case Data::typeRational:
      return this->GetValuE<Rational>().IsSmallInteger(whichInteger);
    default:
      return false;
  }
}

bool Data::IsInteger()const
{ if (this->type!=this->typeRational)
    return false;
  return this->GetValuE<Rational>().IsInteger();
}

const Context& Data::GetContext()const
{ if (this->theContextIndex==-1)
  { std::cout << "This is a programming error: requested context of data that has no context. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theContexts[this->theContextIndex];
}

void Context::GetFormatExpressions(FormatExpressions& output)const
{ output.polyAlphabeT.SetSize(this->VariableImages.size);
  for (int i=0; i<this->VariableImages.size; i++)
    output.polyAlphabeT[i]=this->VariableImages[i].ToString();
}

std::string Data::ToString(std::stringstream* comments, bool isFinal, FormatExpressions* inputFormat)const
{ std::stringstream out;
  if (this->owner==0)
    return "(ProgrammingError:NoOwner)";
  FormatExpressions theFormat;
  if (inputFormat!=0)
    theFormat.flagUseLatex=inputFormat->flagUseLatex;
  if (this->theContextIndex!=-1)
    this->GetContext().GetFormatExpressions(theFormat);
  switch(this->type)
  { case Data::typeSSalgebra:
      out << "SemisimpleLieAlgebra{}("
      << this->owner->theObjectContainer.theLieAlgebras[this->theIndex].GetLieAlgebraName(false) << ")";
//      if (comments!=0)
//        *comments << "Comments to data " << out.str() << ":<br>"
//        << this->owner->theLieAlgebras[this->theRational.GetElementConst().NumShort].
//        ToString(*this->owner->theGlobalVariableS);
      return out.str();
    case Data::typeString:
      if (isFinal)
        return this->owner->theObjectContainer.theStrings[this->theIndex];
      else
        return "(string not shown to avoid javascript problems)";
    case Data::typeElementUE:
      if (theFormat.flagUseLatex)
        out << "\\begin{array}{l}";
      out << "UE{}(" << this->owner->theObjectContainer.theUEs[this->theIndex].ToString(&theFormat) << ")";
      if (theFormat.flagUseLatex)
        out << "\\end{array}";
      return out.str();
    case Data::typeRationalFunction:
      out << "RF{}(" << this->GetValuE<RationalFunctionOld>().ToString(&theFormat) << ")";
      return out.str();
    case Data::typeRational:
      return this->GetValuE<Rational>().ToString();
    case Data::typeRationalRadical:
      return this->GetValuE<AlgebraicNumber> ().ToString();
    case Data::typeLittelmannRootOperator:
      out
      << this->GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >().ToString();
      return out.str();
    case Data::typePoly:
      if (theFormat.flagUseLatex)
        out << "\\begin{array}{l} ";
      out << "Polynomial{}(";
      out << "(" << this->owner->theObjectContainer.thePolys[this->theIndex].ToString(&theFormat) << ")";
      out  << ")";
      if (theFormat.flagUseLatex)
        out << "\\end{array}";
      return out.str();
    case Data::typeEltTensorGenVermasOverRF:
      if (theFormat.flagUseLatex)
        out << "\\begin{array}{l} ";
      out << "EltTensor{}("
      << this->GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().ToString(&theFormat)
      << ")";
      if (theFormat.flagUseLatex)
        out <<"\\end{array}";
      return out.str();
    case Data::typeError:
      out << "(Error:SeeComments)";
      if (comments!=0&& !this->theError.IsZeroPointer())
        *comments << this->theError.GetElementConst();
      return out.str();
    case Data::typeVariableNonBound:
      out << this->GetValuE<VariableNonBound>().ToString();
      return out.str();
    case Data::typeLSpath:
      if (this->GetValuE<LittelmannPath>().IsEqualToZero())
        out << "0";
      else
        out << "LSPath{}(" << this->GetValuE<LittelmannPath>().owner->WeylLetter
        << "_" <<  this->GetValuE<LittelmannPath>().owner->GetDim() << ","
        << this->GetValuE<LittelmannPath>().ToString(true, false, true) << ")";
      return out.str();
    case Data::typeCharSSalgFinite:
      return this->GetValuE<charSSAlgMod<Rational> >().ToString(&theFormat);
    default:
      std::cout << "This is a programming error: don't know how to convert element of type "
      << this->type
      << " (type " << this->ElementToStringDataType() << ") to string. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
      assert(false);
      return out.str();
  }
}

bool Data::SetError(const std::string& inputError)
{ this->type=this->typeError;
  this->theIndex=-1;
  this->theError.GetElement()=inputError;
  return false;
}

Data Data::operator/(const Data& right)const
{ Data result(*this);
  if (this->type!=this->typeRational || right.type!=this->typeRational)
  { std::cout << "This is a programming error: I cannot divide expression of type " << this->type
    << " by expression of type " << right.type
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
    return result;
  }
  if (right.GetValuE<Rational>().IsEqualToZero())
    result.SetError("(Error:DivisionByZero)");
  else
    result=Data(this->GetValuE<Rational>()/right.GetValuE<Rational>(), *this->owner);
  return result;
}

int Data::GetSmallInT()const
{ return this->GetValuE<Rational>().NumShort;
}

int Data::GetIndexAmbientSSLieAlgebra()const
{ if (this->theContextIndex==-1)
    return -1;
  return this->GetContext().indexAmbientSSalgebra;
}

SemisimpleLieAlgebra& Data::GetAmbientSSAlgebra()const
{ if (this->theContextIndex==-1)
  { std::cout << "This is a programming error: GetAmbientSSAlgebra is called on data with empty context. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  if (this->GetContext().indexAmbientSSalgebra==-1)
  { std::cout << "This is a programming error: GetAmbientSSAlgebra is called on data whose context does"
    << " not have an ambient Lie algebra. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
  return this->owner->theObjectContainer.theLieAlgebras[this->GetContext().indexAmbientSSalgebra];
}

const ElementUniversalEnveloping<RationalFunctionOld>& Data::GetUE()const
{ return this->GetValuE<ElementUniversalEnveloping<RationalFunctionOld> > ();
}

const RationalFunctionOld& Data::GetRF()const
{ return this->GetValuE<RationalFunctionOld> ();
}

Data Data::operator*(const Data& right)const
{ MacroRegisterFunctionWithName("Data::operator*");
  Data result(*this);
  if (this->type!=this->typeRational || right.type!=this->typeRational)
  { std::cout << "This is a programming error: I cannot divide expression of type "
    << this->type << " by expression of type " << right.type
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
    return result;
  }
  result=Data(this->GetValuE<Rational>()*right.GetValuE<Rational>(), *this->owner);
  return result;
}

bool Data::operator+=(const Data& other)
{ std::stringstream out;
  if (this->type!=other.type)
  { out << "Adding different types, " << this->ElementToStringDataType() << " and "
    << other.ElementToStringDataType() << ", is not allowed.";
    return this->SetError(out.str());
  }
  switch(this->type)
  { case Data::typeRational:
      this->MakeRational(*this->owner, this->GetValuE<Rational>()+other.GetValuE<Rational>());
      return true;
    default:
      out << "Don't know how to add elements of type " << this->ElementToStringDataType() << ". ";
      return this->SetError(out.str());
  }
}

bool Data::operator==(const Data& other)const
{ if(this->owner!=other.owner)
    return false;
  if (this->type!=other.type)
    return false;
  if (this->type==this->typeError)
    return this->theError.GetElementConst()==other.theError.GetElementConst();
  if (this->theContextIndex!=other.theContextIndex)
    return false;
  return this->theIndex==other.theIndex;
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

Rational Expression::GetRationalValue()const
{ return this->GetAtomicValue().GetValuE<Rational>();
}

const Data& Expression::GetAtomicValue()const
{ if (this->theDatA==-1)
  { std::cout << "This is a programming error. The unsafe method Expression::GetData is "
    << "called on an object that does not have atomic value. "
    << "The lazy programmer has forgotten to check whether Expression::EvaluatesToAtom() is true. "
    << "That same lazy programmer might want to  "
    << " consider using CommandList::fAtomicValue (the same function that is available directly from the "
    << " calculator). "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theData[this->theDatA];
}

void Data::MakePoly
(CommandList& theBoss, const Polynomial<Rational>& inputPoly, int inputContextIndex)
{ this->owner=&theBoss;
  this->type=this->typePoly;
  this->theIndex=theBoss.theObjectContainer.thePolys.AddNoRepetitionOrReturnIndexFirst(inputPoly);
  this->theContextIndex=inputContextIndex;
}

void Data::MakeRational
(CommandList& theBoss, const Rational& inputRational)
{ this->owner=&theBoss;
  this->type=this->typeRational;
  this->theIndex=theBoss.theObjectContainer.theRationals.AddNoRepetitionOrReturnIndexFirst(inputRational);
  this->theContextIndex=-1;
}

void Data::MakeRationalRadical
(CommandList& theBoss, const AlgebraicNumber& inputRationalRad)
{ this->owner=&theBoss;
  this->type=this->typeRationalRadical;
  this->theIndex=theBoss.theObjectContainer.theAlgebraicNumbers
  .AddNoRepetitionOrReturnIndexFirst(inputRationalRad);
  this->theContextIndex=-1;
}

void Data::MakeRF
(CommandList& theBoss, const RationalFunctionOld& inputRF, int inputContextIndex)
{ this->owner=&theBoss;
  this->type=this->typeRationalFunction;
  this->theContextIndex=inputContextIndex;
  this->theIndex=theBoss.theObjectContainer.theRFs.AddNoRepetitionOrReturnIndexFirst(inputRF);
}

void Data::MakeUE
(CommandList& theBoss, const ElementUniversalEnveloping<RationalFunctionOld>& inputUE, int inputContextIndex)
{ this->theIndex= theBoss.theObjectContainer.theUEs.AddNoRepetitionOrReturnIndexFirst(inputUE);
  this->type=this->typeElementUE;
  this->theContextIndex=inputContextIndex;
  this->owner=&theBoss;
}

void Data::MakeVariableNonBound(CommandList& theBoss, const VariableNonBound& input)
{ this->theIndex= theBoss.theObjectContainer.theNonBoundVars.AddNoRepetitionOrReturnIndexFirst(input);
  this->type=this->typeVariableNonBound;
  Expression tempExp;
  this->theContextIndex=-1;
  this->owner=&theBoss;
  tempExp.MakeAtom(*this, theBoss, -1);
  Context tempContext;
  tempContext.VariableImages.AddOnTop(tempExp);
  this->theContextIndex=theBoss.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(tempContext);
}

void Data::MakeString
(CommandList& theBoss, const std::string& inputString, const Context& inputContext)
{ this->theIndex= theBoss.theObjectContainer.theStrings.AddNoRepetitionOrReturnIndexFirst(inputString);
  this->type=this->typeString;
  this->theContextIndex=theBoss.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(inputContext);
  this->owner=&theBoss;
}

void Data::MakeString
(CommandList& theBoss, const std::string& inputString, int contextIndex)
{ this->theIndex= theBoss.theObjectContainer.theStrings.AddNoRepetitionOrReturnIndexFirst(inputString);
  this->type=this->typeString;
  this->theContextIndex=contextIndex;
  this->owner=&theBoss;
}

void Data::MakeChar
(CommandList& theBoss, charSSAlgMod<Rational>& theChar)
{ this->reset(theBoss);
  this->theIndex=theBoss.theObjectContainer.theChars.AddNoRepetitionOrReturnIndexFirst
  (theChar);
  this->type=this->typeCharSSalgFinite;
}

void Data::MakeSSAlgebra
  (CommandList& theBoss, char WeylLetter, int WeylRank)
{ WeylGroup tempW;
  tempW.MakeArbitrary(WeylLetter, WeylRank);
  this->MakeSSAlgebra(theBoss, tempW.CartanSymmetric);
}

void Data::MakeSSAlgebra
  (CommandList& theBoss, const Matrix<Rational>& cartanSymmetric)
{ this->owner=&theBoss;
  this->type=this->typeSSalgebra;
  SemisimpleLieAlgebra tempSS;
  tempSS.theWeyl.CartanSymmetric=cartanSymmetric;
  this->theIndex=theBoss.theObjectContainer.theLieAlgebras.AddNoRepetitionOrReturnIndexFirst(tempSS);
  Context newContext;
  newContext.indexAmbientSSalgebra=this->theIndex;
  this->theContextIndex= theBoss.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(newContext);
  theBoss.theObjectContainer.theLieAlgebras[this->theIndex].indexInOwner=this->theIndex;
  theBoss.theObjectContainer.theLieAlgebras[this->theIndex].owner=& theBoss.theObjectContainer.theLieAlgebras;
  SemisimpleLieAlgebra& realSS= theBoss.theObjectContainer.theLieAlgebras[this->theIndex];
  realSS.ComputeChevalleyConstantS(theBoss.theGlobalVariableS);
}

std::string Data::ElementToStringDataType() const
{ MacroRegisterFunctionWithName("Data::ElementToStringDataType");
  switch(this->type)
  { case Data::typeSSalgebra: return "SemisimpleLieAlgebra";
    case Data::typeString: return "String";
    case Data::typeRational:  return "Rational";
    case Data::typePoly: return "Polynomial";
    case Data::typeError: return "Error";
    case Data::typeRationalRadical: return "NumberFromAlgebraicExtensionRationals";
    case Data::typeRationalFunction: return "RationalFunctionOld";
    case Data::typeElementUE: return "ElementUniversalEnveloping";
    case Data::typeEltTensorGenVermasOverRF: return "ElementOfTensorProductOfGeneralizedVermaModules";
    case Data::typeEltSumGenVermas: return "ElementSumGeneralizedVermaModules";
    case Data::typeMonomialGenVerma: return "MonomialGeneralizedVerma";
    case Data::typeVariableNonBound: return "Variable";
    case Data::typeLittelmannRootOperator: return "LittelmannRootOperator";
    case Data::typeLSpath: return "LSpath";
    case Data::typeCharSSalgFinite: return "CharSSalg";
    default:
      std::cout << "This is a programming error: Data::ElementToStringDataType does not cover type "
      << this->type << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
      assert(false);
      return "unknown";
  }
}

unsigned int Data::HashFunction()const
{ return this->theIndex*SomeRandomPrimes[0]+this->type*SomeRandomPrimes[1];
}

bool Data::MakeElementSemisimpleLieAlgebra
(CommandList& inputOwner, int inputIndexInOwners, int index1, int index2, std::stringstream* comments)
{ List<SemisimpleLieAlgebra>& inputOwners=inputOwner.theObjectContainer.theLieAlgebras;
  SemisimpleLieAlgebra& ownerAlgebra=inputOwners[inputIndexInOwners];
  bool isGood=(index1==0 && index2 <=ownerAlgebra.GetRank() && index2>0);
  if (!isGood)
  { if (comments!=0)
      *comments
      << "You requested element of the Cartan subalgebra labeled by (" << index1 << ", " << index2
      << " of semisimple Lie algebra " << ownerAlgebra.GetLieAlgebraName()
      << "). For your request to succeed, the first index must be zero and the second must be an integer"
      << " between 1 and the rank of the Algebra which is " << ownerAlgebra.GetRank()
      << ". If you want to request an element that is in a root space outside of the Cartan, you should only one index."
      ;
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  int actualIndeX=index2-1+ownerAlgebra.GetNumPosRoots();
  tempElt.MakeGenerator
  (actualIndeX, inputOwners, inputIndexInOwners);
  RationalFunctionOld rfOne, rfZero;
  rfOne.MakeOne(0, this->owner->theGlobalVariableS);
  rfZero.MakeZero(0, this->owner->theGlobalVariableS);
  ElementUniversalEnveloping<RationalFunctionOld> tempUE;
  tempUE.AssignElementLieAlgebra(tempElt, inputOwners, inputIndexInOwners, rfOne, rfZero);
  Context newContext(*this->owner);
  newContext.indexAmbientSSalgebra=inputIndexInOwners;
  this->theContextIndex=inputOwner.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(newContext);
  this->owner=&inputOwner;
  this->type=this->typeElementUE;
  this->theIndex=inputOwner.theObjectContainer.theUEs.AddNoRepetitionOrReturnIndexFirst(tempUE);
  return true;
}

bool Data::operator*=(const Rational& other)
{ std::stringstream out;
  switch(this->type)
  { case Data::typeRational:
      this->MakeRational(*this->owner, this->GetValuE<Rational>()*other);
      return true;
    default:
      out << "Don't know how to multiply elements of type " << this->type << ". ";
      return this->SetError(out.str());
  }
}

bool Data::operator*=(const Data& other)
{ std::stringstream out;
  if (this->type!=other.type)
  { out << "Multiplying different types, " << this->type << " and " << other.type << ", is not allowed.";
    return this->SetError(out.str());
  }
  switch(this->type)
  { case Data::typeRational:
      this->MakeRational(*this->owner, this->GetValuE<Rational>()*other.GetValuE<Rational>());
      return true;
    default:
      out << "Don't know how to multiply elements of type " << this->type << ". ";
      return this->SetError(out.str());
  }
}

bool Data::operator/=(const Data& other)
{ std::stringstream out;
  if (this->type!=other.type)
  { out << "Multiplying different types, " << this->type << " and " << other.type << ", is not allowed.";
    return this->SetError(out.str());
  }
  if (other.IsEqualToZero())
    return false;
  switch(this->type)
  { case Data::typeRational:
      this->MakeRational(*this->owner, this->GetValuE<Rational>()/other.GetValuE<Rational>());
      return true;
    default:
      out << "Don't know how to multiply elements of type " << this->type << ". ";
      return this->SetError(out.str());
  }
}

bool Data::OperatorDereference
  (const Data& argument1, const Data& argument2, Data& output, std::stringstream* comments)const
{ int whichInteger1, whichInteger2;
  switch(this->type)
  { case Data::typeSSalgebra:
      if (!argument1.IsSmallInteger(&whichInteger1) ||!argument2.IsSmallInteger(&whichInteger2) )
      { if (comments!=0)
        *comments << "You requested element of a semisimple Lie algebra labeled by ("
        << argument1.ToString() << ", " << argument2.ToString()
        << ")  which is not a pair of small integers. ";
        return false;
      }
      return output.MakeElementSemisimpleLieAlgebra
      (*this->owner, this->theIndex, whichInteger1, whichInteger2,
       comments);
    default:
      return false;
  }
}

bool Data::OperatorDereference(const Data& argument, Data& output, std::stringstream* comments)const
{ int whichInteger;
  switch(this->type)
  { case Data::typeSSalgebra:
      if (!argument.IsSmallInteger(&whichInteger))
      { if (comments!=0)
        *comments << "You requested element of a semisimple Lie algebra labeled by "
        << argument.ToString() << " which is not a small integer. ";
        return false;
      }
      output.theContextIndex=this->theContextIndex;
      return output.MakeElementSemisimpleLieAlgebra
      (*this->owner, this->owner->theObjectContainer.theLieAlgebras, this->theIndex,
       whichInteger, comments);
    default:
      return false;
  }
}

bool Data::IsEqualToOne()const
{ switch (this->type)
  { case Data::typeRational:
      return this->GetValuE<Rational>().IsEqualToOne();
    default:
      return false;
  }
}

bool Data::IsEqualToZero()const
{ switch (this->type)
  { case Data::typeRational:
      return this->GetValuE<Rational>().IsEqualToZero();
    case Data::typePoly:
      return this->GetValuE<Polynomial<Rational> > ().IsEqualToZero();
    case Data::typeRationalFunction:
      return this->GetValuE<RationalFunctionOld> ().IsEqualToZero();
    default:
      std::cout << "This might or might not be a programming error: "
      <<" I have not been instructed how to check whether "
      << "data of type  " << this->ElementToStringDataType() << " is equal to zero. "
      << "In order to fix this problem you have to edit the current function and add"
      << " an extra case to a switch statement. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return false;
   }
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

void Expression::MakePolyAtom
(const Polynomial<Rational>& inputData, int inputContextIndex, CommandList& newBoss, int inputIndexBoundVars)
{ Data tempData;
  tempData.MakePoly(newBoss, inputData, inputContextIndex);
  this->MakeAtom(tempData, newBoss, inputIndexBoundVars);
}

bool Expression::MakeStringAtom
(CommandList& newBoss, int inputIndexBoundVars, const std::string& theString, const Context& inputContext)
{ Data tempData;
  tempData.MakeString(newBoss, theString, inputContext);
  this->MakeAtom(tempData, newBoss, inputIndexBoundVars);
  return true;
}

bool Expression::MakeStringAtom
(CommandList& newBoss, int inputIndexBoundVars, const std::string& theString)
{ Context tempContext;
  return this->MakeStringAtom(newBoss, inputIndexBoundVars, theString, tempContext);
}

void Expression::MakeRFAtom
(const RationalFunctionOld& inputData, int inputContextIndex, CommandList& newBoss, int inputIndexBoundVars)
{ Data tempData;
  tempData.MakeRF(newBoss, inputData, inputContextIndex);
  this->MakeAtom(tempData, newBoss, inputIndexBoundVars);
}

inline int IntIdentity(const int& x)
{ return x;
}

void Expression::MakeProducT
  (CommandList& owner, int inputIndexBoundVars, const Expression& left, const Expression& right)
{ this->MakeXOX(owner, inputIndexBoundVars, owner.opTimes(), left, right);
}

void Expression::MakeFunction
  (CommandList& owner, int inputIndexBoundVars, const Expression& theFunction, const Expression& theArgument)
{ this->MakeXOX(owner, inputIndexBoundVars, owner.opApplyFunction(), theFunction, theArgument);
  this->format=this->formatFunctionUseUnderscore;
}

void Expression::MakeXOX
  (CommandList& owner, int inputIndexBoundVars, int theOp, const Expression& left, const Expression& right)
{ this->reset(owner, inputIndexBoundVars);
  this->theOperation=theOp;
  this->children.SetSize(2);
  this->children[0]=left;
  this->children[1]=right;
}

bool CommandList::ReplaceOXEXEXEXByE(int formatOptions)
{ SyntacticElement& opElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-8];
  SyntacticElement& leftE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-6];
  SyntacticElement& middleE= (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& rightE = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.format=formatOptions;
  newExpr.children.AddOnTop(leftE.theData);
  newExpr.children.AddOnTop(middleE.theData);
  newExpr.children.AddOnTop(rightE.theData);
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
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(opElt.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.AddOnTop(leftE.theData);
  newExpr.children.AddOnTop(middleE.theData);
  newExpr.children.AddOnTop(rightE.theData);
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
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(left.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.AddOnTop(right.theData);
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
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
  newExpr.format= formatOptions;
  newExpr.children.AddOnTop(right.theData);
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
  result.theData.reset(*this, -1);
  return result;
}

std::string ExpressionPairs::ToString()
{ std::stringstream out;
  out << "the pairs: ";
  for (int i=0; i<this->BoundVariableIndices.size; i++)
    out << this->BoundVariableIndices[i] << "->" << this->variableImages[i].ToString() << "<br>";
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
  Data currentData(*this);
  for (unsigned i=0; i<input.size(); i++)
  { current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if (current==" " || current=="\n")
      current="";
    else
      if (this->isLeftSeparator(current[0]) || this->isRightSeparator(LookAheadChar) )
      { if (this->controlSequences.Contains(current))
        { currentElement.controlIndex=this->controlSequences.GetIndex(current);
          currentElement.theData.reset(*this, -1);
          (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
        } else if (this->isADigit(current, currentDigit))
        { currentData.MakeRational(*this, currentDigit);
          currentElement.controlIndex=this->conInteger();
          currentElement.theData.reset(*this, -1);
          currentElement.theData.MakeAtom(currentData, *this, -1);
          (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
        } else
        { currentData.MakeString(*this, current, -1);
          currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
          currentElement.theData.reset(*this, -1);
          currentElement.theData.theOperation=this->operations.GetIndex("Variable");
          currentElement.theData.theDatA=this->theData.AddNoRepetitionOrReturnIndexFirst(currentData);
          (*this->CurrrentSyntacticSouP).AddOnTop(currentElement);
        }
        current="";
      }
  }
}

int CommandList::GetOperationIndexFromControlIndex(int controlIndex)
{ return this->operations.GetIndex(this->controlSequences[controlIndex]);
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
  theElt.theData.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.theData.format=inputFormat;
  theElt.controlIndex=this->conExpression();
  return true;
}

bool CommandList::ReplaceAbyE()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
  return true;
}

bool CommandList::CreateNewExpressionContext()
{ this->theExpressionContext.SetSize(this->theExpressionContext.size+1);
  this->theExpressionContext.LastObject()->reset();
  return true;
}

bool CommandList::LookAheadAllowsApplyFunction(const std::string& lookAhead)
{ return lookAhead!="{" && lookAhead!="_" && lookAhead!="\\circ" && lookAhead!="{}" &&  lookAhead!="$";
}

bool CommandList::ReplaceListXEYByListY(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& afterleft = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];

  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->opList();
  newExpr.children.AddListOnTop(left.theData.children);
  newExpr.children.AddOnTop(right.theData);
  newExpr.format=inputFormat;
  left.theData=newExpr;
  left.controlIndex=theControlIndex;
  afterleft=*this->CurrentSyntacticStacK->LastObject();
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceListXEByList(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->opList();
  newExpr.children.AddListOnTop(left.theData.children);
  newExpr.children.AddOnTop(right.theData);
  newExpr.format=inputFormat;
  left.theData=newExpr;
  left.controlIndex=theControlIndex;
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceYXdotsXByListYXdotsX(int theControlIndex, int inputFormat, int numXs)
{ SyntacticElement& main = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-numXs-1];
  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->opList();
  newExpr.children.AddOnTop(main.theData);
  newExpr.format=inputFormat;
  main.theData=newExpr;
  main.controlIndex=theControlIndex;
  return true;
}

bool CommandList::ReplaceEXEByList(int theControlIndex, int inputFormat)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->opList();
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
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
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  //    newExpr.inputIndexBoundVars=
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
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
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(middle.theData);
  newExpr.children.AddOnTop(right.theData);
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
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(middle.controlIndex);
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
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
////add here rules that change this->counterInSyntacticSoup.
////In general, this should be avoided, but I couldn't figure out another sane solution for the following parsing problem
////The problem: parse {{a}+b}; {{{a} }+b;
  if (fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS!=" " && lastS!="}")
  { this->ReplaceVXdotsXbyEXdotsX(2);
    this->PopTopSyntacticStack();
    if (this->flagLogSyntaxRules)
      this->parsingLog+= "<b> Special rule: changing Variable to expression and pushing back the last token, token " + lastS
       +  ".</b>";
    this->counterInSyntacticSoup--;
    return true;
  }
////end of rules that change this->counterInSyntacticSoup
  if (lastE.theData.IndexBoundVars==-1)
  { std::cout << "<hr>The last expression, " << lastE.ToString(*this) << ", while reducing "
    << this->ElementToStringSyntacticStack()
    << " does not have properly initialized context. " << CGI::GetStackTraceEtcErrorMessage(__FILE__,  __LINE__);
    assert(false);
  }
  if (secondToLastS==":" && lastS=="=")
  { this->theExpressionContext.LastObject()->flagOpDefineEncountered=true;
    return this->ReplaceXXByCon(this->conDefine());
  }
  if (secondToLastS==":=" && lastS==":")
    return this->ReplaceXXByCon(this->conIsDenotedBy());
  if (secondToLastS=="{" && lastS=="}")
    return this->ReplaceXXByCon(this->conApplyFunction(), Expression::formatDefault);
  if (lastS=="\\cdot")
    return this->ReplaceXByCon(this->conApplyFunction(), Expression::formatFunctionUseCdot);
//  if ( thirdToLastS=="{" && secondToLastS=="{}" && lastS=="}")
//    return this->ReplaceXXXByCon(this->conBindVariable());
  if (fifthToLastS=="{" && fourthToLastS=="{" && thirdToLastS=="Variable" && secondToLastS=="}" && lastS=="}")
    return this->RegisterBoundVariable();
  if (lastS=="Variable" && (lookAhead!="}" || secondToLastS!="{" || thirdToLastS!="{"))
    return this->ReplaceVbyE();
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
    return this->ReplaceEOEByE();
  if (thirdToLastS=="Expression" && secondToLastS=="_" && lastS=="Expression" && lookAhead!="_")
    return this->ReplaceEXEByEusingO(this->conApplyFunction(), Expression::formatFunctionUseUnderscore);
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
    return this->ReplaceXXYByListY(this->conExpression(), lastE.theData.format);
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
    return this->ReplaceYXByListX(this->conList(), secondToLastE.theData.format);
  if (thirdToLastS=="OperationList" && secondToLastS==","  && lastS=="Expression" && this->isSeparatorFromTheRightGeneral(lookAhead))
    return this->ReplaceListXEByList(this->conList(), Expression::formatDefault);
  if (this->isSeparatorFromTheLeftForList(thirdToLastS) && secondToLastS=="OperationList" && this->isSeparatorFromTheRightForList(lastS))
    return this->ReplaceOXbyEXusingO(this->conList(), Expression::formatDefault);
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
    return this->ReplaceYXByListX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (thirdToLastS=="MatrixRow" && secondToLastS=="&"  && lastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lookAhead))
    return this->ReplaceListXEByList(this->conMatrixRow(), Expression::formatMatrixRow);
  if (fourthToLastS=="MatrixRow" && thirdToLastS=="&"  && secondToLastS=="Expression" && this->isSeparatorFromTheRightForMatrixRow(lastS))
    return this->ReplaceListXEYByListY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="Expression" && (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXByListX(this->conMatrixRow(), Expression::formatMatrixRow);
  if (secondToLastS=="MatrixRow" &&  (lastS=="MatrixRowSeparator" || lastS=="MatrixSeparator"))
    return this->ReplaceYXByListX(this->conListMatrixRow(), Expression::formatMatrix);
  if (thirdToLastS=="ListMatrixRows" && secondToLastS=="MatrixRowSeparator" && lastS=="Expression" && this->isSeparatorFromTheRightForListMatrixRow(lookAhead) )
    return this->ReplaceYByListY(this->conMatrixRow(), Expression::formatMatrixRow);
  if (thirdToLastS=="ListMatrixRows" && secondToLastS=="MatrixRowSeparator" && lastS=="MatrixRow" && this->isSeparatorFromTheRightForListMatrixRow(lookAhead) )
    return this->ReplaceListXEByList(this->conListMatrixRow(), Expression::formatMatrix);
  if (thirdToLastS=="MatrixSeparator" && secondToLastS=="ListMatrixRows" && lastS=="MatrixSeparator")
    return this->ReplaceXOXbyEusingO(this->conList(), Expression::formatMatrix);
  if (fifthToLastS=="[" && fourthToLastS=="Expression" && thirdToLastS=="," && secondToLastS=="Expression" && lastS=="]")
    return this->ReplaceXEXEXByEusingO(this->conLieBracket());
  if (this->isSeparatorFromTheLeftForDefinition(seventhToLastS) && lastS=="Expression" && secondToLastS==":=" && thirdToLastS=="Expression" && fourthToLastS=="if" && fifthToLastS==":" && sixthToLastS=="Expression" && this->isSeparatorFromTheRightForDefinition(lookAhead))
    return this->ReplaceEXXEXEByEusingO(this->conDefineConditional());
  if (this->isSeparatorFromTheLeftForStatement(fourthToLastS) && thirdToLastS=="Expression" && secondToLastS==";" && lastS=="Expression" && this->isSeparatorFromTheRightForStatement(lookAhead))
    return this->ReplaceEEndCommandEbyE();
  if (lastS==";" && lookAhead==";")
    return this->DecreaseStackSetCharacterRanges(1);
  return false;
}

bool CommandList::fHWTAABF
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (theExpression.children.size!=3)
  { if (comments!=0)
      *comments << "Error: this function expects 3 arguments. ";
    return false;
  }
  Expression leftExpression=theExpression.children[0];
  Expression rightExpression=theExpression.children[1];
  Expression& weightExpression=theExpression.children[2];
  theCommands.fElementUniversalEnvelopingAlgebra(theCommands, inputIndexBoundVars, leftExpression, comments);
  theCommands.fElementUniversalEnvelopingAlgebra(theCommands, inputIndexBoundVars, rightExpression, comments);
  if (!leftExpression.EvaluatesToAtom() || !rightExpression.EvaluatesToAtom())
  { //std::cout << "<br>left: "<< leftExpression.ToString();
    //std::cout << "<br>right: "<< rightExpression.ToString();
    return false;
  }
  Data leftD=leftExpression.GetAtomicValue();
  Data rightD=rightExpression.GetAtomicValue();
  //std::cout << "<br>starting left elt before merging: " << leftD.ToString();
  //std::cout << "<br>starting right elt before conversion: " << rightD.ToString();
  if (!Data::MergeContexts(leftD, rightD))
  { //std::cout << "failed to merge contexts: " ;
    return false;
  }
  if (!leftD.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  if (!rightD.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  //std::cout << "<br>Merge successful!<br>left elt after merging: " << leftD.ToString();
  //std::cout << "<br>right elt after merging: " << rightD.ToString();
  SemisimpleLieAlgebra& theSSalgebra=theCommands.theObjectContainer.theLieAlgebras[leftD.GetIndexAmbientSSLieAlgebra()];
  Vector<RationalFunctionOld> weight;
  Context finalContext=leftD.GetContext();
  if (!theCommands.GetVector<RationalFunctionOld>
      (weightExpression, weight, &finalContext, theSSalgebra.GetRank(), theCommands.fPolynomial, comments))
  { std::stringstream errorStream;
    errorStream << "Error: could not obtain highest weight from the third argument which is "
    << weightExpression.ToString();
    theExpression.SetError(errorStream.str());
    return true;
  }
  if (!leftD.SetContextResizesContextArray(finalContext))
  { std::cout << "This shouldn't happen!";
    return false;
  }
  if (!rightD.SetContextResizesContextArray(finalContext))
  { std::cout << "This shouldn't happen!";
    return false;
  }
  //std::cout << "<br>starting left elt after the very final conversion: " << leftD.ToString();
  //std::cout << "<br>starting right elt after conversion: " << rightD.ToString();
  //std::cout << "<br>Highest weight in fundamental coords after conversion: " << weight.ToString();
  //std::cout << "<br>Left context: " << leftD.GetContext().ToString();
  //std::cout << "<br>Right context: " << rightD.GetContext().ToString();

  RationalFunctionOld theRingZero, theRingUnit;
  theRingZero.MakeZero(leftD.GetNumContextVars(), theCommands.theGlobalVariableS);
  theRingUnit.MakeOne(rightD.GetNumContextVars(), theCommands.theGlobalVariableS);
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  Vector<RationalFunctionOld> hwDualCoords;
  theSSalgebra.OrderSSalgebraForHWbfComputation();
  hwDualCoords=theWeyl.GetDualCoordinatesFromFundamental(weight);
  RationalFunctionOld output;
  //std::cout << "<br>The highest weight in dual coordinates, as I understand it:" << hwDualCoords.ToString();
  if(!leftD.GetUE().HWTAAbilinearForm(rightD.GetUE(), output, &hwDualCoords, *theCommands.theGlobalVariableS, theRingUnit, theRingZero, comments))
  { theExpression.SetError("Error: couldn't compute Shapovalov form, see comments.");
    return true;
  }
  int newContextIndex= theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(finalContext);
  theExpression.MakeRFAtom(output, newContextIndex, theCommands, inputIndexBoundVars);
//  std::cout << " and the big bad result is " << output.ToString();

  return true;
}

template<class CoefficientType>
bool CommandList::fGetTypeHighestWeightParabolic
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments,
 Vector<CoefficientType>& outputWeightHWFundcoords, Selection& outputInducingSel,
 Context* outputContext)
{ if (theExpression.children.size!=3 && theExpression.children.size!=2 )
    return theExpression.SetError
    ("Function  TypeHighestWeightParabolic is expected to have two or three arguments: SS algebra type, highest weight, [optional] parabolic selection. ");
  Expression& leftE=theExpression.children[0];
  Expression& middleE=theExpression.children[1];
  if (!CommandList::CallCalculatorFunction(theCommands.fSSAlgebra, inputIndexBoundVars, leftE, comments))
    return theExpression.SetError("Failed to generate semisimple Lie algebra.");
  if (!leftE.EvaluatesToAtom())
  { std::stringstream tempStream;
    tempStream << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return theExpression.SetError(tempStream.str());
  }
  SemisimpleLieAlgebra& theSSalgebra=leftE.GetAtomicValue().GetAmbientSSAlgebra();
  MemorySaving<Context> tempContext;
  Context emptyContext;
  Context& theContext= outputContext!=0 ? *outputContext : tempContext.GetElement();
  if (!theCommands.GetVector<CoefficientType>
      (middleE, outputWeightHWFundcoords, &theContext, theSSalgebra.GetRank(), &CommandList::fPolynomial,
       comments))
  { std::stringstream tempStream;
    tempStream << "Failed to convert the second argument of HWV to a list of " << theSSalgebra.GetRank()
    << " polynomials. The second argument you gave is " << middleE.ToString() << ".";
    return theExpression.SetError(tempStream.str());
  }
  if (theExpression.children.size==3)
  { Vector<Rational> parabolicSel;
    Expression& rightE=theExpression.children[2];
    if (!theCommands.GetVector<Rational>
        (rightE, parabolicSel, &emptyContext, theSSalgebra.GetRank(), 0, comments))
    { std::stringstream tempStream;
      tempStream << "Failed to convert the third argument of HWV to a list of " << theSSalgebra.GetRank()
      << " rationals. The third argument you gave is " << rightE.ToString() << ".";
      return theExpression.SetError(tempStream.str());
    }
    outputInducingSel=parabolicSel;
  } else
  { outputInducingSel.init(theSSalgebra.GetRank());
    for (int i=0; i<outputWeightHWFundcoords.size; i++)
      if (!outputWeightHWFundcoords[i].IsSmallInteger())
        outputInducingSel.AddSelectionAppendNewIndex(i);
  }
  return true;
}

bool CommandList::fDecomposeCharGenVerma
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
   std::stringstream* comments)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::fDecomposeCharGenVerma");
  Context theContext;
  Vector<RationalFunctionOld> theHWfundcoords, theHWsimpCoords, theHWFundCoordsFDPart, theHWSimpCoordsFDPart;
  Selection parSel, invertedParSel;
  if (!theCommands.fGetTypeHighestWeightParabolic
  (theCommands, inputIndexBoundVars, theExpression, comments,
   theHWfundcoords, parSel, &theContext))
   return false;
  if (theExpression.errorString!="")
    return true;
  SemisimpleLieAlgebra& theSSlieAlg=theExpression.children[0].GetAtomicValue().GetAmbientSSAlgebra();
  std::stringstream out;
  FormatExpressions theFormat;
  theContext.GetFormatExpressions(theFormat);
  out << "<br>Highest weight: " << theHWfundcoords.ToString(&theFormat)
  << "<br>Parabolic selection: " << parSel.ToString();
  theHWFundCoordsFDPart=theHWfundcoords;
  for (int i=0; i<parSel.CardinalitySelection; i++)
    theHWFundCoordsFDPart[parSel.elements[i]]=0;
  KLpolys theKLpolys;
  if (!theKLpolys.ComputeKLPolys(&theSSlieAlg.theWeyl))
    return theExpression.SetError("failed to generate Kazhdan-Lusztig polynomials (output too large?)");
//  Vectors<Polynomial<Rational> > tempVects;
//  tempVects.AddOnTop(theSSlieAlg.theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords) );
//  HashedList<Vector<Polynomial<Rational> > > theOrbit;
//  if (!theSSlieAlg.theWeyl.GenerateOrbit(tempVects, true, theOrbit, false, -1, 0, 1000))
//    out << "Error: failed to generate highest weight \rho-modified orbit (too large?)";
  WeylGroup& theWeyl=theSSlieAlg.theWeyl;
  theHWSimpCoordsFDPart=theSSlieAlg.theWeyl.GetSimpleCoordinatesFromFundamental
  (theHWFundCoordsFDPart);
  theHWSimpCoordsFDPart+=theSSlieAlg.theWeyl.rho;
//  ElementWeylGroup raisingElt;
//  theSSlieAlg.theWeyl.RaiseToDominantWeight
//  (theHWSimpCoordsFDPart, 0, 0, &raisingElt);
  ReflectionSubgroupWeylGroup theSub;
  if (!theSub.MakeParabolicFromSelectionSimpleRoots
  (theSSlieAlg.theWeyl, parSel, *theCommands.theGlobalVariableS, 1000))
    return theExpression.SetError
    ("Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).");
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
  theChar.MakeZero(*theSSlieAlg.owner, theSSlieAlg.indexInOwner);
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
    currentChar.MakeZero(*theSSlieAlg.owner, theSSlieAlg.indexInOwner);
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
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fHWV
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ Selection selectionParSel;
  Vector<RationalFunctionOld> theHWfundcoords;
  Context hwContext(theCommands);
  if(!theCommands.fGetTypeHighestWeightParabolic
  (theCommands, inputIndexBoundVars, theExpression, comments, theHWfundcoords, selectionParSel, &hwContext)  )
    return theExpression.SetError("Failed to extract highest weight vector data");
  else
    if (theExpression.errorString!="")
      return true;
  SemisimpleLieAlgebra& theSSalgebra=
  theExpression.children[0].GetAtomicValue().GetAmbientSSAlgebra();

  return theCommands.fHWVinner
  (theCommands, inputIndexBoundVars, theExpression, comments,
  theHWfundcoords, selectionParSel, hwContext, theSSalgebra.indexInOwner);
}

bool CommandList::fWriteGenVermaModAsDiffOperatorUpToLevel
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (theExpression.children.size!=3)
  { if (comments!=0)
      *comments << "Function fSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, Number, List{}. ";
    return false;
  }
  Expression& leftE=theExpression.children[0];
  Expression& genVemaWeightNode=theExpression.children[2];
  Expression& levelNode=theExpression.children[1];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  if (!CommandList::fSSAlgebra(theCommands, inputIndexBoundVars, resultSSalgebraE, comments))
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  } else if (!resultSSalgebraE.EvaluatesToAtom())
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  if  (resultSSalgebraE.errorString!="")
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  if (!resultSSalgebraE.EvaluatesToAtom())
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  const Data& theSSdata=resultSSalgebraE.GetAtomicValue();
  if (theSSdata.type!=Data::typeSSalgebra)
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  if (theSSdata.theIndex>= theCommands.theObjectContainer.theLieAlgebras.size)
  { std::cout << "This is a programming error: semisimple Lie algebra referenced but not allocated. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int indexOfAlgebra=theSSdata.theIndex;
  SemisimpleLieAlgebra& theSSalgebra=theCommands.theObjectContainer.theLieAlgebras[indexOfAlgebra];
  int theRank=theSSalgebra.GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Context hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVector
      (genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, &CommandList::fPolynomial, comments))
  { if(comments!=0)
      *comments << "Failed to convert the third argument of fSplitGenericGenVermaTensorFD to a list of " << theRank
      << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
    return false;
  }
  int desiredHeight;
  if (!levelNode.IsSmallInteger(&desiredHeight))
    return theExpression.SetError("second argument of " +theExpression.ToString()+ " must be a small integer");

//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  hwContext.indexAmbientSSalgebra=indexOfAlgebra;
  int theNumVars=hwContext.VariableImages.size;
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeConst(theNumVars, 1, theCommands.theGlobalVariableS);
  RFZero.MakeZero(theNumVars, theCommands.theGlobalVariableS);
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
  hwContext.GetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.fWriteGenVermaModAsDiffOperatorInner(theCommands, inputIndexBoundVars, theExpression, comments, theHws, hwContext, selInducing, indexOfAlgebra);
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
  { tempRF.MakeOneLetterMon(numVars, i+varShift, 1, theGlobalVariables);
    tempMon.MultiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.MakeOne(numVars, &theGlobalVariables);
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
    varShift=this->GetNumVars();
  int numVars=varShift+eltsNilrad.size;
  for (int i=0; i<eltsNilrad.size; i++)
  { tempRF.MakeMonomial(numVars, i+varShift, 1, 1);
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
  //theWeylMon is a polynomial monomial in 2n variables if the Weyl algebra is in n variables.
  //The first n variables are the x_i generators, the second n variables are the
  //\partial_i (derivative)
  //generators.
  MonomialP theWeylMon;
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
  ElementWeylAlgebra leftElt, rightElt;
  Polynomial<CoefficientType> tempP;
  quasiDiffMon outputMon;
  output.MakeZero();
  for (int j=0; j<standsOnTheRight.size; j++)
  { quasiDiffMon& currentRightMon =standsOnTheRight[j];
    tempP.MakeZero(currentRightMon.theWeylMon.monBody.size);
    tempP.AddMonomial(currentRightMon.theWeylMon, standsOnTheRight.theCoeffs[j]);
    rightElt.AssignStandardOrder(tempP);
    for (int i=0; i<this->size; i++)
    { quasiDiffMon& currentLeftMon =(*this)[i];
      tempP.MakeZero(currentLeftMon.theWeylMon.monBody.size);
      tempP.AddMonomial(currentLeftMon.theWeylMon, this->theCoeffs[i]);
      leftElt.AssignStandardOrder(tempP);
      outputMon.theMatMon= (*this)[i].theMatMon;
      outputMon.theMatMon*=standsOnTheRight[j].theMatMon;
      leftElt*=rightElt;
      for (int k=0; k<leftElt.GetStandardOrder().size; k++)
      { outputMon.theWeylMon=(leftElt.GetStandardOrder())[k];
        output.AddMonomial(outputMon, leftElt.GetStandardOrder().theCoeffs[k]);
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
  MatrixTensor<Polynomial<Rational> > reordered;
  reordered.MakeZero();
  Polynomial<Rational> tempP;
  for (int i=0; i<this->size; i++)
  { quasiDiffMon& currentMon=(*this)[i];
    tempP.MakeZero(currentMon.theWeylMon.monBody.size);
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

void ElementWeylAlgebra::GetStandardOrderDiffOperatorCorrespondingToNraisedTo
(int inputPower, int numVars, int indexVar, RationalFunctionOld& output, GlobalVariables& theGlobalVariables)
{ if (inputPower>0)
  { output.MakeOneLetterMon(numVars*2, indexVar, 1, theGlobalVariables);
    output.RaiseToPower(inputPower);
    return;
  }
  inputPower*=-1;
  output.MakeOneLetterMon(numVars*2, indexVar+numVars, 1, theGlobalVariables);
  output.RaiseToPower(inputPower);
  Polynomial<Rational> newMult;
  newMult.MakeDegreeOne(numVars*2, indexVar, 1);
  for (int i=0; i<inputPower; i++)
  { output/=newMult;
    newMult-=1;
  }
//  output/=den;
}

template <class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::GetActionMonGenVermaModuleAsDiffOperator
(MonomialP& monCoeff, MonomialUniversalEnveloping<Polynomial<Rational> >& monUE,
 ElementWeylAlgebra& outputDO, List<int>& indicesNilrad, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::GetActionMonGenVermaModuleAsDiffOperator");
  Rational tempRat;
  int expConstPart, powerMonCoeff;
  int varShift=this->GetNumVars();
  int numVars=varShift+indicesNilrad.size;
  ElementWeylAlgebra tempElt, tempElt2;
  Polynomial<Rational> tempMon;
  outputDO.MakeConst(numVars, 1);
  for (int i=0; i<indicesNilrad.size; i++)
  { monUE.Powers[i].GetConstantTerm(tempRat, 0);
    tempRat.IsSmallInteger(&expConstPart);
    monCoeff[i+varShift].IsSmallInteger(&powerMonCoeff);
    tempElt.Makexidj(i+varShift, i+varShift, numVars);
    tempElt.RaiseToPower(powerMonCoeff);
    tempMon.MakeMonomial(numVars, i+varShift, expConstPart, 1);
    tempElt2.AssignPolynomial(tempMon);
    tempElt.MultiplyOnTheLeft(tempElt2, theGlobalVariables);
    outputDO*=tempElt;
  }
  return true;
}

template <class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::GetActionGenVermaModuleAsDiffOperator
(ElementSemisimpleLieAlgebra<Rational>& inputElt, quasiDiffOp<Rational>& output,
  GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::GetActionGenVermaModuleAsDiffOperator");
  List<ElementUniversalEnveloping<CoefficientType> > eltsNilrad;
  List<int> indicesNilrad;
  this->GetElementsNilradical(eltsNilrad, true, &indicesNilrad);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->GetGenericUnMinusElt(true, theGenElt, theGlobalVariables);
  int numVars=theGenElt.GetNumVars();
//  Polynomial<Rational> Pone, Pzero;
  result.AssignElementLieAlgebra(inputElt, *this->theAlgebras, this->indexAlgebra, 1, 0);
  theGenElt.SetNumVariables(numVars);
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

//  std::cout  << "<br>Num elements nilrad: " << indicesNilrad.size;
  ElementWeylAlgebra weylPartSummand;
  Polynomial<Rational> tempP, theCoeff;
  quasiDiffMon monQDO, monQDO2;
  Rational tempRat;
  output.MakeZero();
  for (int i=0; i<result.size; i++)
  { MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon=result[i];
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
        tempMat1.theCoeffs[k].GetNumerator(tempP);
        tempMT.AddMonomial(tempMat1[k], tempP);
      }
      MathRoutines::RaiseToPower(tempMT, thePower, idMT);
      endoPart*=tempMT;
    }
//    std::cout << "<br>Endo part of " << currentMon.ToString() << ": " << endoPart.ToString();
    for (int l=0; l<result.theCoeffs[i].size; l++)
    { if (!this->GetActionMonGenVermaModuleAsDiffOperator
          (result.theCoeffs[i][l], currentMon, weylPartSummand, indicesNilrad, theGlobalVariables))
        return false;
//      std::cout << "<br>Weyl part of " << currentMon.ToString() << " with coeff "
//      << result.theCoeffs[i].ToString() << ": " << weylPartSummand.ToString();
      weylPartSummand.GetStandardOrder(tempP);
      for (int j=0; j<tempP.size; j++)
        for (int k=0; k<endoPart.size; k++)
        { monQDO.theMatMon=endoPart[k];
          monQDO.theWeylMon=tempP[j];
          Polynomial<Rational>& currentEndoCoeff=endoPart.theCoeffs[k];
          for (int m=0; m<currentEndoCoeff.size; m++)
          { monQDO2=monQDO;
            for (int n=0; n<currentEndoCoeff.NumVars; n++)
              monQDO2.theWeylMon[n]+=currentEndoCoeff[m][n];
            tempRat=currentEndoCoeff.theCoeffs[m];
            tempRat*=tempP.theCoeffs[j];
            tempRat*=result.theCoeffs[i].theCoeffs[l];
  //          std::cout << "<br>adding " << monQDO.ToString() << " times " << tempRF.ToString()  << " to "
  //          << output.ToString();
            output.AddMonomial(monQDO2, tempRat);
  //          std::cout << " to get " << output.ToString();
          }
        }
    }
  }
//  std::cout << "<br>result qdo: " << output.ToString();
  return true;
}

bool CommandList::fWriteGenVermaModAsDiffOperatorInner
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments,
  Vectors<Polynomial<Rational> >& theHws, Context& hwContext, Selection& selInducing, int indexOfAlgebra)
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
  hwContext.GetFormatExpressions(theUEformat);
  theUEformat.polyDefaultLetter="a";
  theUEformat.MaxLineLength=178;
  theUEformat.NumAmpersandsPerNewLineForLaTeX=2;
  theWeylFormat.NumAmpersandsPerNewLineForLaTeX=2;
  hwContext.GetFormatExpressions(theWeylFormat);
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
  if (false)
    if (theSSalgebra.GetRank()==3 && theSSalgebra.theWeyl.WeylLetter=='B')
    { theGenerator=theGeneratorsItry[0];
      theGenerator+=theGeneratorsItry[2];
      theGeneratorsItry.AddOnTop(theGenerator);
    }
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
      return theExpression.SetError("Failed to create module.");
    if (i==0)
    { theMod.GetElementsNilradical(elementsNegativeNilrad, true);
      Polynomial<Rational> Pone, Pzero;
      Pone.MakeOne(elementsNegativeNilrad.size+theMod.GetNumVars());
      Pzero.MakeZero(elementsNegativeNilrad.size+theMod.GetNumVars());
      theMod.GetGenericUnMinusElt(true, genericElt, *theCommands.theGlobalVariableS);
      quasiDiffOp<Polynomial<Rational> >::prepareFormatFromShiftAndNumWeylVars
      (hwContext.VariableImages.size, elementsNegativeNilrad.size, theWeylFormat);
//      std::cout << "theWeylFormat: ";
//      for (int k=0; k<theWeylFormat.polyAlphabeT.size; k++)
//        std::cout << theWeylFormat.polyAlphabeT[k] << ", ";
      theUEformat.polyAlphabeT.SetSize(hwContext.VariableImages.size+ elementsNegativeNilrad.size);
      for (int k=hwContext.VariableImages.size; k<theUEformat.polyAlphabeT.size; k++)
      { std::stringstream tmpStream;
        tmpStream << "a_{" << k-hwContext.VariableImages.size+1 << "}";
        theUEformat.polyAlphabeT[k] = tmpStream.str();
      }
//      std::cout << "<br>theUEformat: ";
//      for (int k=0; k<theUEformat.polyAlphabeT.size; k++)
//        std::cout << theUEformat.polyAlphabeT[k] << ", ";
      out << "<tr><td>Generic element U(n_-):</td><td>"
      << CGI::GetHtmlMathSpanPure(genericElt.ToString(&theUEformat)) << "</td> </tr>";
      latexReport << "& \\multicolumn{" << theGeneratorsItry.size << "}{c}{Element acting}\\\\<br>\n ";
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
        (theGeneratorsItry[j], theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra, Pone, Pzero)
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
    << theSSalgebra.GetLieAlgebraName(false) << "$}"
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
      latexReport << " & $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat) << "\\end{array}$";
      if (j!=0)
        latexReport2 << "&&";
      latexReport2 << " $\\begin{array}{l}" << theQDOs[j].ToString(&theWeylFormat)
      << "\\end{array}$\\\\ "
      << (j!=theGeneratorsItry.size-1 ? "\\cline{3-3}" : "\\hline" ) << "\n<br>";
      theWeylFormat.CustomCoeffMonSeparator="";
    }
//    theQDOs[0].GenerateBasisLieAlgebra(theQDOs, &theWeylFormat, theCommands.theGlobalVariableS);
    //std::cout << "<br>Dimension generated Lie algebra: " << theQDOs.size;
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
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fHWVinner
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments,
 Vector<RationalFunctionOld>& highestWeightFundCoords,
 Selection& selectionParSel, Context& hwContext, int indexOfAlgebra)
{ MacroRegisterFunctionWithName("CommandList::fHWVinner");
  RecursionDepthCounter therecursionIncrementer(&theCommands.RecursionDeptH);
  //  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  hwContext.indexAmbientSSalgebra=indexOfAlgebra;
  int theNumVars=hwContext.VariableImages.size;
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeConst(theNumVars, 1, theCommands.theGlobalVariableS);
  RFZero.MakeZero(theNumVars, theCommands.theGlobalVariableS);
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
  { for (int i=0; i<highestWeightFundCoords.size; i++)
      if (highestWeightFundCoords[i].NumVars>RFOne.NumVars)
      { std::cout << "This is a programming error: the highest weight I was given has " << highestWeightFundCoords[i].NumVars
        << " variables but the context I was given indicates  " << RFOne.NumVars << " variables. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      } else
        highestWeightFundCoords[i].SetNumVariables(RFOne.NumVars);
    bool isGood=theMod.MakeFromHW
    (theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra, highestWeightFundCoords, selectionParSel,
     *theCommands.theGlobalVariableS, RFOne, RFZero, &report);
    if (comments!=0)
      *comments << report;
    if (!isGood)
    { theExpression.SetError("Error while generating highest weight module. See comments for details. ");
      return true;
    }
  }
  Data outputData;
  int outputContextIndex= theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(hwContext);
  std::cout.flush();
  theElt.MakeHWV(theMods, indexOfModule, RFOne);
//  std::cout << "<br>theElt:" << theElt.ToString();
  outputData.MakeElementTensorGeneralizedVermas(theCommands, theElt, outputContextIndex);
  theExpression.MakeAtom(outputData, theCommands, inputIndexBoundVars);
//  std::cout << "<hr>" << outputData.ToString();
  return true;
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
  (CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (theExpression.children.size!=3)
  { if (comments!=0)
      *comments << "Function fSplitGenericGenVermaTensorFD is expected to have three arguments: SS algebra type, List{}, List{}. ";
    return false;
  }
  Expression& leftE=theExpression.children[0];
  Expression& genVemaWeightNode=theExpression.children[2];
  Expression& fdWeightNode=theExpression.children[1];
  Expression resultSSalgebraE;
  resultSSalgebraE=leftE;
  if (!CommandList::fSSAlgebra(theCommands, inputIndexBoundVars, resultSSalgebraE, comments))
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  } else if (!resultSSalgebraE.EvaluatesToAtom())
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  if  (resultSSalgebraE.errorString!="")
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  if (!resultSSalgebraE.EvaluatesToAtom())
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  const Data& theSSdata=resultSSalgebraE.GetAtomicValue();
  if (theSSdata.type!=Data::typeSSalgebra)
  { if(comments!=0)
      *comments << "Failed to create a semisimple Lie algebra from the first argument, which is " << leftE.ToString();
    return false;
  }
  if (theSSdata.theIndex>= theCommands.theObjectContainer.theLieAlgebras.size)
  { std::cout << "This is a programming error: semisimple Lie algebra referenced but not allocated. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int indexOfAlgebra=theSSdata.theIndex;
  SemisimpleLieAlgebra& theSSalgebra=theCommands.theObjectContainer.theLieAlgebras[indexOfAlgebra];
  int theRank=theSSalgebra.GetRank();
  Vector<RationalFunctionOld> highestWeightFundCoords;
  Context hwContext(theCommands), emptyContext(theCommands);
  if (!theCommands.GetVector<RationalFunctionOld>
      (genVemaWeightNode, highestWeightFundCoords, &hwContext, theRank, &CommandList::fPolynomial, comments))
  { if(comments!=0)
      *comments << "Failed to convert the third argument of fSplitGenericGenVermaTensorFD to a list of " << theRank
      << " polynomials. The second argument you gave is " << genVemaWeightNode.ToString() << ".";
    return false;
  }
  Vector<Rational> theFDhw;
  if (!theCommands.GetVector<Rational>(fdWeightNode, theFDhw, &emptyContext, theRank, 0, comments))
  { if(comments!=0)
      *comments << "Failed to convert the second argument of fSplitGenericGenVermaTensorFD to a list of " << theRank
      << " rationals. The second argument you gave is " << fdWeightNode.ToString() << ".";
    return false;
  }
//  std::cout << "<br>highest weight in fundamental coords: " << highestWeightFundCoords.ToString() << "<br>";
//  std::cout << "<br>parabolic selection: " << parabolicSel.ToString();
  hwContext.indexAmbientSSalgebra=indexOfAlgebra;
  int theNumVars=hwContext.VariableImages.size;
  RationalFunctionOld RFOne, RFZero;
  RFOne.MakeConst(theNumVars, 1, theCommands.theGlobalVariableS);
  RFZero.MakeZero(theNumVars, theCommands.theGlobalVariableS);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> theElt;
  //=theElementData.theElementTensorGenVermas.GetElement();
  Selection selParSel1, selFD;
  Expression hwvGenVerma, hwvFD;
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
      return theExpression.SetError("Error: the third argument of fSplitGenericGenVermaTensorFD must be a list of small non-negative integers.");
  }
  theCommands.fHWVinner
  (theCommands, inputIndexBoundVars, hwvGenVerma, comments,
  highestWeightFundCoords, selParSel1, hwContext, indexOfAlgebra);
  if (hwvGenVerma.errorString!="")
    return theExpression.SetError(hwvGenVerma.errorString);
  Vector<RationalFunctionOld> theFDhwRF;
  theFDhwRF=theFDhw;
  for (int i=0; i<theRank; i++)
    theFDhwRF[i].SetNumVariables(theNumVars);
  theCommands.fHWVinner
  (theCommands, inputIndexBoundVars, hwvFD, comments,
  theFDhwRF, selFD, hwContext, indexOfAlgebra);
  if (hwvFD.errorString!="")
    return theExpression.SetError(hwvFD.errorString);
  std::stringstream out;
  out << "hwv par sel: " << hwvGenVerma.ToString();
  out << "hwv fd: " << hwvFD.ToString();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWgenVerma=
  hwvGenVerma.GetAtomicValue().GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& theHWfd=
  hwvFD.GetAtomicValue().GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();

  ModuleSSalgebra<RationalFunctionOld>& theGenMod=theHWgenVerma[0].theMons[0].GetOwner();
  int indexGenMod=theHWgenVerma[0].theMons[0].indexInOwner;
  ModuleSSalgebra<RationalFunctionOld>& theFDMod=theHWfd[0].theMons[0].GetOwner();
  int indexFDMod=theHWfd[0].theMons[0].indexInOwner;
  ElementUniversalEnveloping<RationalFunctionOld> theCasimir, theCasimirMinusChar;
  charSSAlgMod<RationalFunctionOld> theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.MakeFromWeight(theFDMod.theHWSimpleCoordSBaseField, theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra);
  ReflectionSubgroupWeylGroup tempSG;
  List<ElementUniversalEnveloping<RationalFunctionOld> > theLeviEigenVectors;
  Vectors<RationalFunctionOld> theEigenVectorWeightsFund;
  theFDMod.SplitOverLevi
  (&report, theGenMod.parabolicSelectionNonSelectedAreElementsLevi, *theCommands.theGlobalVariableS, RFOne, RFZero,
   &theLeviEigenVectors, &theEigenVectorWeightsFund, 0, &theFDLeviSplit)
  ;
  theFDMod.GetFDchar(theFDChaR);
  List<ElementUniversalEnveloping<RationalFunctionOld> > theCentralCharacters;
  theCasimir.MakeCasimir(theSSalgebra, *theCommands.theGlobalVariableS, RFOne, RFZero);
  Vector<RationalFunctionOld> currentHWsimplecoords, currentHWdualcoords, currentWeightDiff;
  FormatExpressions tempFormat;
  tempFormat.MaxLineLength=60;
  tempFormat.flagUseLatex=true;
  tempFormat.fundamentalWeightLetter="\\psi";
  tempFormat.CustomPlusSign="\\oplus ";
  hwContext.GetFormatExpressions(tempFormat);
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
  theFDLeviSplitShifteD.MakeZero(theCommands.theObjectContainer.theLieAlgebras, indexOfAlgebra);
  MonomialChar<RationalFunctionOld> tempMon;
  ElementUniversalEnveloping<RationalFunctionOld> currentChar;
  for (int i=0; i<theLeviEigenVectors.size; i++)
  { tempMon.weightFundamentalCoords=theEigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoords+=theGenMod.theHWFundamentalCoordsBaseField;
    theFDLeviSplitShifteD.AddMonomial(tempMon, RFOne);
    currentHWdualcoords=theSSalgebra.theWeyl.GetDualCoordinatesFromFundamental(tempMon.weightFundamentalCoords);
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
    theSSalgebra.theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[i]);
    tempElt.MakeHWV(theMods, indexFDMod, RFOne);
    tempElt.MultiplyOnTheLeft
    (theLeviEigenVectors[i], theElt, theMods, theSSalgebra, *theCommands.theGlobalVariableS,
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
      theSSalgebra.theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[j]);
      if ((otherWeightSimpleCoords-currentWeightSimpleCoords).IsPositive())
      { theCasimirMinusChar=theCasimir;
        theCasimirMinusChar-=theCentralCharacters[j];
        theElt.MultiplyOnTheLeft
        (theCasimirMinusChar, tempElt2, theMods, theSSalgebra, *theCommands.theGlobalVariableS,
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
    << theSSalgebra.theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "</td><td>" << CGI::GetHtmlMathSpanPure(tempStream.str())
    << "</td><td>" << tempRat.ToString() << "</td>";
    latexReport2
    << "$" << theSSalgebra.theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
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
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fMatrix
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=3)
    return false;
  Expression& leftE=theExpression.children[0];
  Expression& middleE=theExpression.children[1];
  Expression& rightE=theExpression.children[2];
  int numRows, numCols;
  if (!middleE.EvaluatesToSmallInteger(&numRows) || !rightE.EvaluatesToSmallInteger(&numCols))
    return false;
  if (numRows<=0 || numCols<=0)
    return false;
  if (numRows>1000 || numCols>1000)
  { if (comments!=0)
      *comments << "Max number of rows/columns is 1000. You requested " << numRows << " rows and "
      << numCols << " columns.<br>";
    return false;
  }
  Expression Result;
  Result.reset(theCommands, inputIndexBoundVars);
  Result.theOperation=theCommands.opList();
  Result.format=Result.formatMatrix;
  Result.children.SetSize(numRows);
  Expression theIndices;
  theIndices.reset(theCommands, inputIndexBoundVars);
  theIndices.theOperation=theCommands.opList();
  theIndices.children.SetSize(2);
  for (int i=0; i<numRows; i++)
  { Expression& currentRow=Result.children[i];
    currentRow.reset(theCommands, inputIndexBoundVars);
    currentRow.theOperation=theCommands.opList();
    currentRow.children.SetSize(numCols);
    currentRow.format=currentRow.formatMatrixRow;
    for (int j=0; j<numCols; j++)
    { Expression& currentE=currentRow.children[j];
      theIndices.children[0].MakeInt(i+1, theCommands, inputIndexBoundVars);
      theIndices.children[1].MakeInt(j+1, theCommands, inputIndexBoundVars);
      currentE.MakeFunction(theCommands, inputIndexBoundVars, leftE, theIndices);
    }
  }
  theExpression=Result;
  return true;
}

bool CommandList::fPolynomial
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fPolynomial");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (theCommands.ExpressionHasBoundVars(theExpression))
    return false;
  Polynomial<Rational> outputPoly;
  Context finalContext(theCommands);
  if (!theCommands.EvaluatePMTDtree<Polynomial<Rational> >(outputPoly, finalContext, theExpression, comments))
  { theExpression.SetError("Failed to convert to polynomial: see comments. ");
    return true;
  }
  int finalContextIndex=theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(finalContext);
  theExpression.MakePolyAtom(outputPoly, finalContextIndex,  theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::fElementUniversalEnvelopingAlgebra
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fElementUniversalEnvelopingAlgebra");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (theCommands.ExpressionHasBoundVars(theExpression))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  HashedList<Expression> varImagesBuffer;
  Context finalContext(theCommands);
  if (!theCommands.ExtractPMTDtreeContext<ElementUniversalEnveloping<RationalFunctionOld> >
      (finalContext, theExpression, comments))
    return false;
  if (finalContext.indexAmbientSSalgebra==-1)
  { if (comments!=0)
      *comments << " You asked me to convert expression "
      << theExpression.ToString() << " to an element of Universal enveloping algebra, "
      << "but none of the subparts of the expression had an ambient Lie algebra. ";
    if (theCommands.theObjectContainer.theLieAlgebras.size==0)
    { if (comments!=0)
        *comments << " As there is no Lie algebra created, I cannot select one for you. ";
      return false;
    }
    if (comments!=0)
      *comments << "I am selecting for you the first Universal enveloping algebra available in the object container, "
      << theCommands.theObjectContainer.theLieAlgebras[0].GetLieAlgebraName();
    finalContext.indexAmbientSSalgebra=0;
  }
//  std::cout << "<br>The extracted context is: " << outputContext.ToString();
  if (!theCommands.EvaluatePMTDtreeFromContextRecursive(outputUE, finalContext, theExpression, comments))
    return false;
  outputUE.Simplify(*theCommands.theGlobalVariableS,1, 0);
  int finalContextIndex=
  theCommands.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(finalContext);
  Data outputData;
  outputData.MakeUE(theCommands, outputUE, finalContextIndex);
  theExpression.MakeAtom(outputData, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::fKLcoeffs
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fKLcoeffs");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!theCommands.CallCalculatorFunction(theCommands.fSSAlgebra, inputIndexBoundVars, theExpression, comments))
    return theExpression.SetError("Failed to created Lie algebra");
  SemisimpleLieAlgebra& theSSalgebra= theExpression.GetAtomicValue().GetAmbientSSAlgebra();
  std::stringstream out;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  if (theWeyl.GetSizeWeylByFormula(theWeyl.WeylLetter, theWeyl.GetDim())>192)
  { out << "I have been instructed to run only for Weyl groups that have at most 192 elements (i.e. no larger than D_4). "
    << theSSalgebra.GetLieAlgebraName() << " has "
    << theWeyl.GetSizeWeylByFormula(theWeyl.WeylLetter, theWeyl.GetDim()).ToString() << ".";
    theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
    return true;
  }
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(1);
  theFormat.polyAlphabeT[0]="q";
  out << "Our notation follows that of the original Kazhdan-Lusztig paper, "
  << "Representations of Coxeter Groups and Hecke Algebras.<br>";
  out << " The algebra: " << theSSalgebra.GetLieAlgebraName();
  KLpolys theKLpolys;
  theKLpolys.ComputeKLPolys(&theWeyl);
  theFormat.flagUseHTML=true;
  out << theKLpolys.ToString(&theFormat);
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fWeylOrbit
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression,
 std::stringstream* comments, bool useFundCoords, bool useRho)
{ if (theExpression.children.size!=2)
    return theExpression.SetError("fWeylOrbit takes two arguments");
  Expression& theSSalgebraNode=theExpression.children[0];
  Expression& vectorNode=theExpression.children[1];
  if (!theCommands.fSSAlgebra(theCommands, inputIndexBoundVars, theSSalgebraNode, comments))
    return theExpression.SetError("Failed to created Lie algebra");
  if (theSSalgebraNode.errorString!="")
  { theExpression.AssignChild(0);
    return true;
  }
  SemisimpleLieAlgebra& theSSalgebra= theSSalgebraNode.GetAtomicValue().GetAmbientSSAlgebra();
  Vector <Polynomial<Rational> > theHWfundCoords, theHWsimpleCoords, currentWeight;
  Context theContext;
  if (!theCommands.GetVector<Polynomial<Rational> >(vectorNode, theHWfundCoords, &theContext, theSSalgebra.GetRank(), theCommands.fPolynomial, comments))
    return theExpression.SetError("Failed to extract highest weight");
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  if (!useFundCoords)
  { theHWsimpleCoords=theHWfundCoords;
    theHWfundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoords);
  } else
    theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundCoords);
  std::stringstream out, latexReport;
  Vectors<Polynomial<Rational> > theHWs;
  FormatExpressions theFormat;
  theContext.GetFormatExpressions(theFormat);
//  theFormat.fundamentalWeightLetter="\\psi";
  theHWs.AddOnTop(theHWsimpleCoords);
  HashedList<Vector<Polynomial<Rational> > > outputOrbit;
  WeylGroup orbitGeneratingSet;
  Polynomial<Rational> theExp;
  if (!theSSalgebra.theWeyl.GenerateOrbit(theHWs, useRho, outputOrbit, false, 1921, &orbitGeneratingSet, 1921))
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
  theSSalgebra.theWeyl.GetEpsilonMatrix
  (theSSalgebra.theWeyl.WeylLetter, theSSalgebra.GetRank(), epsCoordMat);
  for (int i=0; i<outputOrbit.size; i++)
  { theFormat.simpleRootLetter="\\alpha";
    theFormat.fundamentalWeightLetter="\\psi";
    std::string orbitEltString=outputOrbit[i].ToString(&theFormat);
    Vector<Polynomial<Rational> > epsVect=outputOrbit[i];
    epsCoordMat.ActOnVectorColumn(epsVect);
    std::string orbitEltStringEpsilonCoords=epsVect.ToStringEpsilonFormat();
    std::string weightEltString=
    theSSalgebra.theWeyl.GetFundamentalCoordinatesFromSimple(outputOrbit[i]).ToStringLetterFormat
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
      standardElt.MakeConst(*theSSalgebra.owner, theSSalgebra.indexInOwner);
      bool isGood=true;
      for (int j=0; j<orbitGeneratingSet[i].size; j++)
      { int simpleIndex=orbitGeneratingSet[i][j];
        theExp=theSSalgebra.theWeyl.GetScalarProdSimpleRoot(currentWeight, simpleIndex);
        theSSalgebra.theWeyl.SimpleReflection(simpleIndex, currentWeight, useRho, false);
        theExp*=2;
        theExp/=theSSalgebra.theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex];
        if (useRho)
          theExp+=1;
        if (theExp.IsInteger(&tempInt))
          if (tempInt<0)
          { isGood=false;
            break;
          }
        standardElt.MultiplyByGeneratorPowerOnTheLeft
        (theSSalgebra.GetNumPosRoots() -simpleIndex-1, theExp);
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
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fSSAlgebra
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, bool Verbose)
{ RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::fSSAlgebra");
  if (!theCommands.fPolynomial(theCommands, inputIndexBoundVars, theExpression, comments))
    return theExpression.SetError
    ("Failed to extract the semismiple Lie algebra type from " + theExpression.ToString());
  if (theExpression.errorString!="")
    return true;
  if (!theExpression.EvaluatesToAtom())
  { std::cout << "This is a programming error: I called successfully function fPolynomial, "
    << " but the output does not evaluate to an atom (polynomial). "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  FormatExpressions theFormat;
  Polynomial<Rational> theType=
  theExpression.GetAtomicValue().GetValuE< Polynomial<Rational> > ();
  Context theContext=theExpression.GetAtomicValue().GetContext();
  theContext.GetFormatExpressions(theFormat);
  Matrix<Rational> theCartanSymmetric;
  theCartanSymmetric.init(0,0);
  WeylGroup tempW;
  DynkinType dynkinType;
  DynkinSimpleType simpleComponent;
  dynkinType.MakeZero();
  char theWeylLetter='X';
  for (int i=0; i<theType.size; i++)
  { MonomialP& currentMon=theType[i];
    int variableIndex;
    if (!currentMon.IsOneLetterFirstDegree(&variableIndex))
      return theExpression.SetError
      ("Failed to extract type from monomial "+ currentMon.ToString(&theFormat));
    Expression typeE= theContext.VariableImages[variableIndex];
    if (typeE.children.size!=2)
      return theExpression.SetError
      ("The monomial "+ currentMon.ToString(&theFormat)+
       " appears not to be a Dynkin simple type ");
    Expression rankE;
    Expression lengthE;
    Rational firstCoRootLength=2;
    bool foundLengthFromExpression=false;
    if (typeE.theOperation==theCommands.opThePower())
    { lengthE=typeE.children[1];
      typeE.AssignChild(0);
      foundLengthFromExpression=true;
    }
    if (typeE.theOperation==theCommands.opApplyFunction())
    { rankE=typeE.children[1];
      typeE.AssignChild(0);
    }
    if (typeE.theOperation==theCommands.opThePower())
    { lengthE=typeE.children[1];
      typeE.AssignChild(0);
      foundLengthFromExpression=true;
    }
    if (foundLengthFromExpression)
    { if (!lengthE.EvaluatesToRational(&firstCoRootLength))
        return theExpression.SetError
        ("Couldn't extract first co-root length from " + currentMon.ToString(&theFormat));
      if (firstCoRootLength<=0)
        return theExpression.SetError
        ("Couldn't extract positive rational first co-root length from " + currentMon.ToString(&theFormat));
    }
    if (!typeE.EvaluatesToAtom())
      return theExpression.SetError
      ("I couldn't extract a type letter from "+ currentMon.ToString(&theFormat));
    const VariableNonBound& theTypeName=typeE.GetAtomicValue().GetValuE<VariableNonBound>();
    if (theTypeName.theName.size()!=1)
      return theExpression.SetError
      ("The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G.\
        Instead, it is "+ theTypeName.theName + "; error while processing "
       + currentMon.ToString(&theFormat));
    theWeylLetter=theTypeName.theName[0];
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
      return theExpression.SetError
      ("The type of a simple Lie algebra must be the letter A, B, C, D, E, F or G; \
       error while processing "
       + currentMon.ToString(&theFormat));
    int theRank;
    if (!rankE.EvaluatesToSmallInteger(&theRank))
      return theExpression.SetError
      ("I wasn't able to extract rank from " + currentMon.ToString(&theFormat));
    if (theRank<1 || theRank>8)
      return theExpression.SetError
      ("The rank of a simple Lie algebra must be between 1 and 8; error while processing "
       + currentMon.ToString(&theFormat));
    simpleComponent.theLetter=theWeylLetter;
    simpleComponent.theRank= theRank;
    if (!foundLengthFromExpression)
      if (theWeylLetter=='F')
        firstCoRootLength=1;
    simpleComponent.lengthFirstCoRootSquared= firstCoRootLength;
    tempW.MakeArbitrary(theWeylLetter, theRank, &firstCoRootLength);
    int theMultiplicity=-1;
    if (!theType.theCoeffs[i].IsSmallInteger(&theMultiplicity))
      theMultiplicity=-1;
    if (theMultiplicity<0)
    { std::stringstream out;
      out << "I failed to convert the coefficient " << theType.theCoeffs[i]
      << " of " << currentMon.ToString(&theFormat) << " to a small integer";
      return theExpression.SetError(out.str());
    }
    for (int k=0; k<theMultiplicity; k++)
    { dynkinType.AddMonomial(simpleComponent, 1);
      theCartanSymmetric.DirectSumWith(tempW.CartanSymmetric);
    }
  }
  if (theCartanSymmetric.NumCols>20)
  { std::stringstream out;
    out << "I have been instructed to allow semisimple Lie algebras of rank 20 maximum. "
    << " If you would like to relax this limitation edit file " << __FILE__ << " line "
    << __LINE__ << ". Note that the Chevalley constant computation reserves a dim(g)*dim(g)"
    << " table of RAM memory, so beware the computational risks. "
    << " Alternatively, you may want to implement a sparse structure constant table "
    << "(write me an email if you want to do that, I will help you). ";
    return theExpression.SetError(out.str());
  }
  int oldSize=theCommands.theObjectContainer.theLieAlgebras.size;
  Data tempData;
  tempData.MakeSSAlgebra(theCommands, theCartanSymmetric);
  SemisimpleLieAlgebra& theSSalgebra = tempData.GetAmbientSSAlgebra();
  theSSalgebra.theWeyl.WeylLetter=theWeylLetter;
  theSSalgebra.ComputeChevalleyConstantS(theCommands.theGlobalVariableS);
  std::stringstream out;
  if (oldSize<theCommands.theObjectContainer.theLieAlgebras.size)
    if (comments!=0)
    { FormatExpressions theFormat, latexFormat;
      latexFormat.flagUseLatex=true;
      latexFormat.flagUseHTML=false;
//      theFormat.chevalleyHgeneratorLetter="\\bar{h}";
//      theFormat.chevalleyGgeneratorLetter="\\bar{g}";
      out
      << "Lie algebra of type " << dynkinType << " generated.";
      if (Verbose)
      { out
        << " The resulting Lie bracket pairing table follows. <hr> "
        << tempData.GetAmbientSSAlgebra().ToString(&theCommands.theGlobalVariableS->theDefaultFormat);
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
        out << "\\end{longtable}";
      }
      WeylGroup& theWeyl=theSSalgebra.theWeyl;
      out << "<hr>Symmetric Cartan matrix.<br>"
      << "We define the symmetric Cartan matrix by requesing that the entry in "
      << "the i-th row and j-th column "
      << " be the scalar product of the i^th and j^th roots.<br>"
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
      out << "<hr>Size of Weyl group according to formula: "
      << dynkinType.GetSizeWeylByFormula();
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
      } else
      { out << "<hr>If you want extra details (root system info, interactive "
        << "picture in the Coxeter plane of the root system, "
        << " with the Weyl chamber drawn, etc.), use the function";
        std::stringstream tempStream;
        tempStream << "printSemisimpleLieAlgebra{}" << theSSalgebra.theWeyl.WeylLetter
        << "_" << theSSalgebra.GetRank();
        out << theCommands.GetCalculatorLink(tempStream.str());
        out << " instead. ";
      }
    }
  if (!Verbose)
  { *comments << out.str();
    theExpression.MakeAtom(tempData, theCommands, inputIndexBoundVars);
    theExpression.theOperation=theCommands.opAtom();
    theExpression.children.SetSize(0);
  } else
    theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  //theSSalgebra.TestForConsistency(*theCommands.theGlobalVariableS);
  return true;
}

bool Expression::HasBoundVariables()
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
  if (this->theOperation==this->theBoss->opVariableBound())
    return true;
  for (int i=0; i<this->children.size; i++)
    if (this->children[i].HasBoundVariables())
      return true;
  return false;
}

bool CommandList::fDrawPolarRfunctionTheta
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fDrawPolarRfunctionTheta");
  if (theExpression.children.size!=3)
    return theExpression.SetError
    ("Drawing polar coordinates takes three arguments: function, lower angle \
      bound and upper angle bound. ");
  Expression& lowerE=theExpression.children[1];
  Expression& upperE=theExpression.children[2];
  Expression functionE=theExpression.children[0];
  Rational upperBound, lowerBound;
  if (!lowerE.EvaluatesToRational(&upperBound) || !upperE.EvaluatesToRational(&lowerBound))
    return
    theExpression.SetError
    ("Failed to convert upper and lower bounds of drawing function to rational numbers.");
  if (upperBound<lowerBound)
    MathRoutines::swap(upperBound, lowerBound);
  theCommands.fSuffixNotationForPostScript
  (theCommands, inputIndexBoundVars, functionE, comments);
  std::stringstream out, resultStream;
  out << CGI::GetHtmlMathSpanPure(theExpression.children[0].ToString())
  << "<br>";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}"
  << "\\usepackage{pst-3dplot}\\begin{document} \\pagestyle{empty}";
  resultStream << " \\begin{pspicture}(-5, 5)(5,5)";
  resultStream << "\\psaxes[labels=none]{<->}(0,0)(-4.5,-4.5)(4.5,4.5)";
  resultStream << "\\parametricplot[linecolor=red, plotpoints=1000]{"
  << lowerBound.DoubleValue() << "}{" << upperBound.DoubleValue() << "}{";
  std::string funString=functionE.GetAtomicValue().GetValuE<std::string>();
  resultStream << funString << " t cos mul " << funString << " t sin mul" << "}";
  resultStream << "\\end{pspicture}\\end{document}";
  out << theCommands.WriteDefaultLatexFileReturnHtmlLink(resultStream.str(), true);
  out << "<br><b>LaTeX code used to generate the output. </b><br>" << resultStream.str();
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fSuffixNotationForPostScript
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fSuffixNotation");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  std::stringstream out;
  if (*theCounter.theCounter ==theCommands.MaxRecursionDeptH-2)
  { out << "...";
  } else if (theExpression.EvaluatesToAtom())
  { Rational tempRat;
    if (theExpression.EvaluatesToRational(&tempRat))
      out << tempRat.DoubleValue() << " ";
    else
      out << theExpression.ToString();
  } else
  { int theOp=theExpression.theOperation;
    int startIndex=0;
    if (theOp==theCommands.opApplyFunction())
      startIndex=1;
    Expression tempE;
    for (int i=startIndex; i<theExpression.children.size; i++)
    { tempE=theExpression.children[i];
      theCommands.fSuffixNotationForPostScript(theCommands, inputIndexBoundVars, tempE, comments);
      out << tempE.GetAtomicValue().GetValuE<std::string>() << " ";
    }
    if (startIndex==1)
    { tempE=theExpression.children[0];
      theCommands.fSuffixNotationForPostScript(theCommands, inputIndexBoundVars, tempE, comments);
      out << tempE.GetAtomicValue().GetValuE<std::string>() << " ";
    }
    if (theOp==theCommands.opDivide())
      out << "div ";
    else if (theOp==theCommands.opPlus())
      out << "add ";
    else if (theOp==theCommands.opMinus())
    { if (theExpression.children.size==1)
        out << "-1 mul ";
      else
        out << "sub ";
    } else if (theOp==theCommands.opTimes())
      out << "mul";
    else if (theOp==theCommands.opThePower())
      out << "exp ";
  }
  theExpression.MakeStringAtom(theCommands, inputIndexBoundVars, out.str());
  return true;
}

bool CommandList::fIsInteger
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.IsInteger())
    theExpression.MakeAtom(1, theCommands, inputIndexBoundVars);
  else
    theExpression.MakeAtom(0, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::AppendOpandsReturnTrueIfOrderNonCanonical
  (Expression& theExpression, List<Expression>& output, int theOp)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
    return false;
  bool result=false;
  if (theExpression.theOperation!=theOp)
    output.AddOnTop(theExpression);
  else
    for (int i=0; i<theExpression.children.size; i++)
    { if (this->AppendOpandsReturnTrueIfOrderNonCanonical(theExpression.children[i], output, theOp))
        result=true;
      if (i<theExpression.children.size-1 &&
          theExpression.children[i].theOperation==theOp &&
          theExpression.children[i].children.size>1)
        result=true;
    }
  return result;
}

void CommandList::initCrunchers()
{ this->RegisterCruncherNoFail(this->opTensor(), Data::typeEltTensorGenVermasOverRF, Data::typeEltTensorGenVermasOverRF, Data::TensorAnyByEltTensor);

  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRational, Data::typeEltTensorGenVermasOverRF, Data::MultiplyAnyByEltTensor);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typePoly, Data::typeEltTensorGenVermasOverRF, Data::MultiplyAnyByEltTensor);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalFunction, Data::typeEltTensorGenVermasOverRF, Data::MultiplyAnyByEltTensor);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeElementUE, Data::typeEltTensorGenVermasOverRF, Data::MultiplyAnyByEltTensor);

  this->RegisterCruncherNoFail(this->opApplyFunction(), Data::typeElementUE, Data::typeEltTensorGenVermasOverRF, Data::MultiplyAnyByEltTensor);

  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeEltTensorGenVermasOverRF, Data::typeRational, Data::MultiplyEltTensorByCoeff);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeEltTensorGenVermasOverRF, Data::typePoly, Data::MultiplyEltTensorByCoeff);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeEltTensorGenVermasOverRF, Data::typeRationalFunction, Data::MultiplyEltTensorByCoeff);

  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeElementUE, Data::typeRational, Data::MultiplyUEByAny);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeElementUE, Data::typePoly, Data::MultiplyUEByAny);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeElementUE, Data::typeRationalFunction, Data::MultiplyUEByAny);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeElementUE, Data::typeElementUE, Data::MultiplyUEByAny);

  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRational, Data::typeElementUE, Data::MultiplyAnyByUE);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typePoly, Data::typeElementUE, Data::MultiplyAnyByUE);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalFunction, Data::typeElementUE, Data::MultiplyAnyByUE);

  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRational, Data::typeRationalRadical, Data::MultiplyRatOrAlgebraicByRatOrAlgebraic);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalRadical, Data::typeRational, Data::MultiplyRatOrAlgebraicByRatOrAlgebraic);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalRadical, Data::typeRationalRadical, Data::MultiplyRatOrAlgebraicByRatOrAlgebraic);

  this->RegisterDivCruncherNoFail(Data::typeRationalFunction, Data::typeRationalFunction, Data::DivideRFOrPolyOrRatByRFOrPoly);
  this->RegisterDivCruncherNoFail(Data::typeRationalFunction, Data::typePoly, Data::DivideRFOrPolyOrRatByRFOrPoly);
  this->RegisterDivCruncherNoFail(Data::typePoly, Data::typeRationalFunction, Data::DivideRFOrPolyOrRatByRFOrPoly);
  this->RegisterDivCruncherNoFail(Data::typePoly, Data::typePoly, Data::DivideRFOrPolyOrRatByRFOrPoly);
  this->RegisterDivCruncherNoFail(Data::typeRational, Data::typeRationalFunction, Data::DivideRFOrPolyOrRatByRFOrPoly);
  this->RegisterDivCruncherNoFail(Data::typeRational, Data::typePoly, Data::DivideRFOrPolyOrRatByRFOrPoly);


  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalFunction, Data::typeRationalFunction, Data::MultiplyRatOrPolyOrRFByRatOrPolyOrRF);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typePoly, Data::typeRationalFunction, Data::MultiplyRatOrPolyOrRFByRatOrPolyOrRF);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRational, Data::typeRationalFunction, Data::MultiplyRatOrPolyOrRFByRatOrPolyOrRF);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalFunction, Data::typePoly, Data::MultiplyRatOrPolyOrRFByRatOrPolyOrRF);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRationalFunction, Data::typeRational, Data::MultiplyRatOrPolyOrRFByRatOrPolyOrRF);

  this->RegisterMultiplicativeDataCruncherNoFail(Data::typePoly, Data::typePoly, Data::MultiplyRatOrPolyByRatOrPoly);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeRational, Data::typePoly, Data::MultiplyRatOrPolyByRatOrPoly);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typePoly, Data::typeRational, Data::MultiplyRatOrPolyByRatOrPoly);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeLittelmannRootOperator, Data::typeLSpath, Data::MultiplyLRObyLSPath);
  this->RegisterMultiplicativeDataCruncherNoFail(Data::typeLittelmannRootOperator, Data::typeLittelmannRootOperator, Data::MultiplyLRObyLRO);

  this->RegisterAdditiveDataCruncherNoFail(Data::typeEltTensorGenVermasOverRF, Data::typeEltTensorGenVermasOverRF, Data::AddEltTensorToEltTensor);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeElementUE, Data::typeRational, Data::AddUEToAny);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeElementUE, Data::typePoly, Data::AddUEToAny);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeElementUE, Data::typeRationalFunction, Data::AddUEToAny);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeElementUE, Data::typeElementUE, Data::AddUEToAny);

  this->RegisterAdditiveDataCruncherNoFail(Data::typeRationalFunction, Data::typeRationalFunction, Data::AddRatOrPolyOrRFToRatOrPolyOrRF);
  this->RegisterAdditiveDataCruncherNoFail(Data::typePoly, Data::typeRationalFunction, Data::AddRatOrPolyOrRFToRatOrPolyOrRF);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeRational, Data::typeRationalFunction, Data::AddRatOrPolyOrRFToRatOrPolyOrRF);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeRationalFunction, Data::typePoly, Data::AddRatOrPolyOrRFToRatOrPolyOrRF);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeRationalFunction, Data::typeRational, Data::AddRatOrPolyOrRFToRatOrPolyOrRF);

  this->RegisterAdditiveDataCruncherNoFail(Data::typeRationalRadical, Data::typeRational, Data::AddRatOrAlgebraicToRatOrAlgebraic);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeRational, Data::typeRationalRadical, Data::AddRatOrAlgebraicToRatOrAlgebraic);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeRationalRadical, Data::typeRationalRadical, Data::AddRatOrAlgebraicToRatOrAlgebraic);

  this->RegisterAdditiveDataCruncherNoFail(Data::typePoly, Data::typePoly, Data::AddRatOrPolyToRatOrPoly);
  this->RegisterAdditiveDataCruncherNoFail(Data::typePoly, Data::typeRational, Data::AddRatOrPolyToRatOrPoly);
  this->RegisterAdditiveDataCruncherNoFail(Data::typeRational, Data::typePoly, Data::AddRatOrPolyToRatOrPoly);
}

void CommandList::init(GlobalVariables& inputGlobalVariables)
{ this->theGlobalVariableS=& inputGlobalVariables;
  this->MaxAlgTransformationsPerExpression=100000;
  this->MaxRecursionDeptH=10000;
  this->RecursionDeptH=0;
  this->DepthRecursionReached=0;
  this->flagLogSyntaxRules=false;
  this->flagNewContextNeeded=true;
  this->MaxLatexChars=2000;
  this->numEmptyTokensStart=9;
  this->MaxNumCachedExpressionPerContext=1000;
  this->controlSequences.Clear();
  this->operations.Clear();
  this->syntacticSouP.SetSize(0);
  this->syntacticStacK.SetSize(0);
  this->flagTimeLimitErrorDetected=false;
  this->flagFirstErrorEncountered=false;
  this->flagMaxTransformationsErrorEncountered=false;
  this->flagMaxRecursionErrorEncountered=false;
  this->theObjectContainer.theNonBoundVars.Clear();
  this->theExpressionContext.SetSize(0);
  this->ExpressionStack.Clear();

  this->theData.Clear();
  this->theFunctions.Clear();
  this->syntaxErrors="";
  this->evaluationErrors.SetSize(0);
  this->targetProperties.SetSize(0);
  this->CurrentSyntacticStacK=&this->syntacticStacK;
  this->CurrrentSyntacticSouP=&this->syntacticSouP;
  this->AddOperationNoFail
  ("+", this->StandardPlus, "",
   "Collects all terms (over the rationals), adding up terms proportional up to a rational number. \
    Zero summands are removed, unless zero is the only term left. ", "1+a-2a_1+1/2+a_1", true);
  this->AddOperationNoFail
  ("-", this->StandardMinus, "",
   "Transforms a-b to a+(-1)*b and -b to (-1)*b. Equivalent to a rule \
   -{{b}}:=MinnusOne*b; {{a}}-{{b}}:=a+MinnusOne*b", "-1+(-5)", true);
  this->AddOperationNoFail
  ("/", this->StandardDivide, "",
    "1) If a and b are rational substitutes a/b with its value. \
     <br>2)If b is rational computes (anything)/b with anything* (1/b). \
     This is equivalent to {{a}}/b:={{a}}*(1/b).", "3/5+(a+b)/5", true);
  this->AddOperationNoFail
  ("*", this->StandardTimes, "",
   "<br>The following description is out of date. Must be updated.\
   <br>1) If a and b are both of type built in-data, and there is a built in handler for a*b, \
   substitutes a*b by the result of the built-in handler.<br>\n \
   2) Reorders all multiplicative terms in regular order, e.g. ((a*b)*(c*d))*f:=a*(b*(c*(d*f))).<br> \
   3) Applies the left and right distributive laws ({{a}}+{{b}})*{{c}}:=a*c+b*c; \
   {{c}}*({{a}}+{{b}}):=c*a+c*b.<br> \
   4.1) If b is rational, substitutes a*b by b*a (i.e. {{a}}{{b}}:if IsRational{} b:=b*a;). <br>\
   4.2) If the expression is of the form a*(b*c) and  a and b are rational, \
   substitutes a*(b*c) by (a*b)*c. <br>\
   4.3) If the expression is of the form a*(b*c) and b is rational but a is not, \
   substitutes the expression by b*(a*c).",
   "2*c_1*d*3", true);
    this->AddOperationNoFail
  ("\\otimes", this->StandardTensor, "",
   "Please do note use (or use at your own risk): this is work-in-progress. Will be documented when implemented and tested. Tensor product of \
   generalized Verma modules. ",
   " g:= SemisimpleLieAlgebra{}G_2; h_{{i}}:=g_{0, i};\nv_\\lambda:=hwv{}(G_2, (1,0),(0,0));\
   \n g_{-1}(v_\\lambda\\otimes v_\\lambda);\n\
   g_{-1}g_{-1}(v_\\lambda\\otimes v_\\lambda); ", true);
  this->AddOperationNoFail
  ("[]", this->StandardLieBracket, "",
   "Lie bracket.", "g:=SemisimpleLieAlgebra{}A_1; [g_1,g_{-1}] ", true);
  this->AddOperationNoFail(":=", 0, "", "", "", true);
  this->AddOperationNoFail
  (":=:", this->StandardIsDenotedBy, "", "The operation :=: is the \"is denoted by\" operation. \
   The expression a:=:b always reduces to \
   a:=b. In addition to the transformation, the pair of expressions a,b is registered in a \
   special global \"registry\". This has the following effect. Every time \
   the expression b is met, it is displayed on the screen as a. We note that subsequent \
   occurrences of the expression a will first be replaced by b (as mandated\
   by the a:=b command), but then displayed on the screen as a.", "x:=:y;\ny;\nz;\nz:=y;\nz ", true);
  this->AddOperationNoFail("if:=", 0, "", "", "", true);
  std::stringstream StandardPowerStreamInfo, moreInfoOnIntegers;
  moreInfoOnIntegers
  << " LargeIntUnsigned::SquareRootOfCarryOverBound is "
  << " restricted to be smaller than the square root of a positive signed int on the system: in this way, multiplying two small integers and "
  << " storing the result in a signed int is guaranteed to produce the correct result. "
  << " Rationals are stored in the computer as 8 bytes+ one pointer (which is another 4 bytes on a 32 bit system). "
  << " The pointer either equals zero, or points to a dynamically resizable structure able to hold "
  << " arbitrary integer sizes (within the system's address space limits). If the pointer is zero, we call  "
  << " such a rational number small. In this case its denominator and numerator are stored in "
  << " the other 8 bytes, and should both be smaller than LargeIntUnsigned::CarryOverBound="
  << LargeIntUnsigned::CarryOverBound
  << " . When requesting an arithmetic operation, if both rationals are small, (i.e. their pointer zero)"
  << " a check is performed whether the denominator and numerator are smaller in absolute value than "
  << " LargeIntUnsigned::SquareRootOfCarryOverBound="
  << LargeIntUnsigned::SquareRootOfCarryOverBound
  << ". If that check passes, the two rationals are multiplied using the "
  << " built-in processor instructions for operations with integers. If any of the check fails, both rationals are converted to the larger dynamically "
  << " resizeable structure, sufficient memory is allocated, and multiplication/addition is carried using algorithms located in class "
  << " LargeIntUnsigned. The algorithms are O(log(n)^2) (the number of digits squared), "
  << " slower than the ones in the standard numerical computation libraries (such as GMP). However, the algorithms "
  << " provide the essential convenience of having a self-contained mathematical library. An implementation of "
  << " discrete Fourier transform multiplication algorithm (O(log(n)(log(log n))) is considered, and will be made if need arises. ";
  StandardPowerStreamInfo
  << "If the left argument evaluates to atom, and if the right argument is a small integer atom, "
  << " tries to carry out the raise-to-power operation. If successful, substitutes the expression with the obtained atom. "
  << " A small integer is defined at compile time in the variable LargeIntUnsigned::SquareRootOfCarryOverBound (currently equal to "
  << LargeIntUnsigned::SquareRootOfCarryOverBound << "). "
  << CGI::GetHtmlSpanHidableStartsHiddeN(moreInfoOnIntegers.str());
  this->AddOperationNoFail
  ("^", this->StandardPower, "", StandardPowerStreamInfo.str(),
   "3^3^3;\n3^{3^3}", true);
  this->AddOperationNoFail
  ("==", this->StandardEqualEqual, "",
   "If either the left or the right argument contains a bound variable does nothing. \
    Else evaluates to 1 if the left argument equals the right argument.",
   "x==y;\nx==1;\nIsEqualToX{} {{a}}:=a==x;\nIsEqualToX{}y;\nIsEqualToX{}x;\
   \nIsEqualToX{}1;\nx:=1;\nIsEqualToX{}1", true);
  //the following operation for function application is chosen on purpose so that it corresponds to LaTeX-undetectable
  //expression
  this->AddOperationNoFail
  ("{}", this->StandardFunction, "",
   "The first argument of this operator represents a name of the function, \
   the second argument represents the argument of that function.  \
   <br>1) If the first argument of {} is rational, the operation substitutes the \
   expression with that constant. \
   <br>2) If the first argument is of type Data, looks for a built-in c++ handler of \
   the function, depending on the types of the arguments.\
   <br>3) If the first argument is of type list and the second argument is a small \
   integer between 1 and the number of elements in the list,\
   the operation substitutes the list with the element indexed by the second argument.\
   <br>4) If the first argument of {} is of type NonBoundVariable and has a hard-coded \
   handler function, the handler is invoked onto the second argument.\
   If the invocation is successful, the expression is substituted with the result, \
   otherwise remains unchanged. <br> (2) and (4) are essentially equivalent: \
   the only noticeable difference is that the functions invoked in (2) are anonymous \
   from the calculator standpoint (i.e. have only technical c++ names), \
   while the functions in (4) have human-readable calculator-visible names. \
   In future versions, (1) might be merged into (2) and (2) might be merged in (4).",
   "Fibonacci{}0:=1;\nFibonacci{}1:=1;\nFibonacci{}({{x}}):if \
   IsInteger{}x:=Fibonacci{}(x-1)+Fibonacci{}(x-2);\nFibonacci{}100;\nFibonacci{}x;\
   \n5{}x;\n(x,y,z){}2;\n(1,2,3){}4 ", true);
  //the following is the binding variable operation
  this->AddOperationNoFail("VariableNonBound", 0, "", "", "", false);
  this->AddOperationNoFail("VariableBound", 0, "", "", "", false);
  this->AddOperationNoFail("OperationList", 0, "", "", "", false);
//  this->AddOperationNoFail("Matrix", 0, "Matrix", "", "", "");
  this->AddOperationNoFail
  ("\\cup", this->StandardUnion, "",
   "If all arguments of \\cup are of type list, substitutes the expression with \
   a list containing \
   the union of all members (with repetition).",
   "x\\cup List{} x \\cup List{}x \\cup (a,b,x)", true);
  this->AddOperationNoFail
  ("\\sqcup", this->StandardUnionNoRepetition, "",
   "If all arguments of \\sqcup are of type list, substitutes the expression with a list \
   containing \
   the union of all members; all repeating members are discarded.",
   "(x,y,x)\\sqcup(1,x,y,2)", true);
  this->AddOperationNoFail("Error", 0, "", "", "", false);
  this->AddOperationNoFail("Atom", 0, "", "", "", false);
  this->AddOperationNoFail(";", 0, "", "", "", false);

  this->controlSequences.AddOnTop(" ");//empty token must always come first!!!!
  this->controlSequences.AddOnTop("{{}}");
  this->controlSequences.AddOnTop("Variable");
  for (int i=0; i<this->operations.size; i++)
    this->controlSequences.AddOnTop(this->operations[i].theName);//all operations are also control sequences
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
  this->controlSequences.AddOnTop("ListMatrixRows");
  this->controlSequences.AddOnTop("List");
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
  this->initPredefinedVars();
  this->initCrunchers();
}

bool CommandList::CollectSummands(int inputIndexBoundVars, Expression& theExpression)
{ List<Expression>& summands= this->buffer1;
  summands.SetSize(0);
  bool needSimplification=this->AppendSummandsReturnTrueIfOrderNonCanonical
  (theExpression, summands);
  return this->CollectSummands(summands, needSimplification, inputIndexBoundVars, theExpression);
}

bool CommandList::CollectSummands
(List<Expression>& summands, bool needSimplification, int inputIndexBoundVars, Expression& theExpression)
{ HashedList<Expression> summandsNoCoeff;
  List<Rational> theCoeffs;
  Rational constTerm=0;
  bool foundConstTerm=false;
//  std::cout << "<b>" << theExpression.ToString() << "</b>";
//  if (theExpression.ToString()=="(4)*(a) b+(a) b")
//    std::cout << "problem!";
//assert(false);
//return false;
  for (int i=0; i<summands.size; i++)
  { Expression* currentSummandNoCoeff;
    currentSummandNoCoeff=&summands[i];
    Rational theCoeff=1;
    if (currentSummandNoCoeff->theOperation==this->opTimes())
    { if(currentSummandNoCoeff->children[0].EvaluatesToRational())
      { const Data& curData=currentSummandNoCoeff->children[0].GetAtomicValue();
        if (curData.type==curData.typeRational)
        { theCoeff=curData.GetValuE<Rational>();
          currentSummandNoCoeff=& currentSummandNoCoeff->children[1];
          if (theCoeff.IsEqualToZero())
            needSimplification=true;
        }
      }
    } else if (currentSummandNoCoeff->EvaluatesToRational())
    { const Data& curData=currentSummandNoCoeff->GetAtomicValue();
      if (!foundConstTerm)
        constTerm=curData.GetValuE<Rational>();
      else
        constTerm+=curData.GetValuE<Rational>();
      if (foundConstTerm || curData.IsEqualToZero())
        needSimplification=true;
      foundConstTerm=true;
      continue;
    }
    int currentIndex=summandsNoCoeff.GetIndex(*currentSummandNoCoeff);
    if (currentIndex==-1)
    { summandsNoCoeff.AddOnTop(*currentSummandNoCoeff);
      theCoeffs.AddOnTop(0);
      currentIndex=summandsNoCoeff.size-1;
    } else
      needSimplification=true;
    theCoeffs[currentIndex]+=theCoeff;
  }
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
  if (!needSimplification)
    return false;
  List<Expression> summandsWithCoeff;
  summandsWithCoeff.SetSize(0);
  summandsWithCoeff.ReservE(summandsNoCoeff.size);
  for (int i=0; i<summandsNoCoeff.size; i++)
  { if (theCoeffs[i].IsEqualToZero())
      continue;
    summandsWithCoeff.SetSize(summandsWithCoeff.size+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size-1];
    if (!theCoeffs[i].IsEqualToOne())
    { current.reset(*this, inputIndexBoundVars);
      current.theOperation=this->opTimes();
      current.children.SetSize(2);
      current.children[0].MakeAtom(theCoeffs[i], *this, inputIndexBoundVars);
      current.children[1]=summandsNoCoeff[i];
    } else
      current=summandsNoCoeff[i];
  }
  if (!constTerm.IsEqualToZero() || summandsWithCoeff.size==0)
  { summandsWithCoeff.SetSize(summandsWithCoeff.size+1);
    Expression& current=summandsWithCoeff[summandsWithCoeff.size-1];
    current.MakeAtom(constTerm, *this, inputIndexBoundVars);
  }
  if (summandsWithCoeff.size==1)
  { theExpression=summandsWithCoeff[0];
    return true;
  }
  Expression* currentExp;
  currentExp=&theExpression;
  for (int i=0; i<summandsWithCoeff.size-1; i++)
  { currentExp->reset(*this, inputIndexBoundVars);
    currentExp->theOperation=this->opPlus();
    currentExp->children.SetSize(2);
    currentExp->children[0]=summandsWithCoeff[i];
    currentExp=& currentExp->children[1];
  }
  *currentExp=summandsWithCoeff[summandsWithCoeff.size-1];
  return true;
}

bool CommandList::DoThePower
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.theOperation!=theCommands.opThePower() )
    return false;
  Expression& leftE=theExpression.children[0];
  Expression& rightE=theExpression.children[1];
  if (!leftE.EvaluatesToAtom())
    return false;
//  std::cout << "Gonna apply the power to " << leftE.ToString() <<  " with strength " << rightE.ToString();
  if (leftE.IsElementUE())
    if (!theCommands.fPolynomial(theCommands, inputIndexBoundVars, rightE, 0))
    { if (comments!=0)
        *comments << "<br>Failed to convert " << rightE.ToString() << ", the exponent of "
        << leftE.ToString()
        << ", to type polynomial. ";
      return false;
    }
//  std::cout << "<br>After eventual conversion I am using the power on " << leftE.ToString() <<  " with strength " << rightE.ToString();
  if (!rightE.EvaluatesToAtom())
    return false;
  const Data& LeftD=leftE.GetAtomicValue();
  const Data& RightD=rightE.GetAtomicValue();
  Data outputD;
  if (!LeftD.Exponentiate(RightD, outputD))
    return false;
//  std::cout << "<br>Exponentiation was successful and the result is: " << outputD.ToString();
  theExpression.MakeAtom(outputD, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::DoTheOperation
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theOperation)
{ if (theExpression.theOperation!=theOperation )
    return false;
  Expression& leftE=theExpression.children[0];
  Expression& rightE=theExpression.children[1];
//  std::cout << leftE.ElementToStringPolishForm() <<  "<br>" << rightE.ElementToStringPolishForm();
  if (!leftE.EvaluatesToAtom() || !rightE.EvaluatesToAtom())
    return false;
  const Data& LeftD=leftE.GetAtomicValue();
  const Data& RightD=rightE.GetAtomicValue();
  Data outputD;
//  std::cout << "<br>attempting to make standard multiplication between <br>" << RightD.ToString() << " and " << LeftD.ToString();
  DataCruncher::CruncherDataTypes theCruncher= theCommands.GetOpCruncher(theOperation, LeftD.type, RightD.type);
  if (theCruncher==0)
    return false;
  if (!theCruncher(LeftD, RightD, outputD, comments))
    return false;
//  std::cout << "<br> multiplication successful, result: " << outputD.ToString();
  theExpression.MakeAtom(outputD, theCommands, inputIndexBoundVars);
  return true;
}

bool Data::Exponentiate(const Data& right, Data& output)const
{ //std::cout << "<br>Attempting to apply the power " << right.ToString() << " on " << this->ToString();
  Rational resultRat;
  MemorySaving<Polynomial<Rational> > tempP;
  MemorySaving<RationalFunctionOld> tempRF;
  if (right.type==Data::typeRational)
  { int thePower;
    if (right.IsSmallInteger(&thePower))
      switch (this->type)
      { case Data::typeRational:
          resultRat=this->owner->theObjectContainer.theRationals[this->theIndex];
          resultRat.RaiseToPower(thePower);
          output.MakeRational(*this->owner, resultRat);
          return true;
        case Data::typePoly:
          tempP.GetElement()=this->GetValuE<Polynomial<Rational> >();
//          std::cout << tempP.GetElement().GetReport();
          tempP.GetElement().RaiseToPower(thePower);
//          std::cout << tempP.GetElement().GetReport();
          output.MakePoly(*this->owner, tempP.GetElement(), this->theContextIndex);
          return true;
        case Data::typeRationalFunction:
          tempRF.GetElement()=this->GetValuE<RationalFunctionOld>();
//          std::cout << tempP.GetElement().GetReport();
          tempRF.GetElement().RaiseToPower(thePower);
//          std::cout << tempP.GetElement().GetReport();
          output.MakeRF(*this->owner, tempRF.GetElement(), this->theContextIndex);
          return true;
        default: break;
      }
  }
  if (this->type!=this->typeElementUE)
    return false;
  Data rightCopy=right;
  output=*this;
  if (!Data::MergeContexts(output, rightCopy))
    return false;
  if (!rightCopy.ConvertToTypE<RationalFunctionOld>())
    return false;
//  std::cout << "<br>so far, so good, said the falling guy around somewhere the second floor. "
//  << " left: " << output.ToString() << " right: " << rightCopy.ToString();
  if (!this->GetUE().IsAPowerOfASingleGenerator())
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> result=output.GetUE();
//  std::cout << "<br>before exponentiation next step: raising " << result.ToString() << " to power " << rightCopy.ToString() << " = " << rightCopy.theBuiltIn.ToString();
//  std::cout << "<br>result.theBuiltIn[0].Powers[0] equals " << result.theBuiltIn[0].Powers[0].ToString();
  result[0].Powers[0]*=rightCopy.GetRF();
//  std::cout << "<br>result.theBuiltIn[0].Powers[0] equals after multiplication to: " << result.theBuiltIn[0].Powers[0].ToString();
  output.MakeUE(*this->owner, result, output.theContextIndex);
//  std::cout << "<br>the very final result of exponentiation: " << output.ToString();
  return true;
}

bool Data::MultiplyRatOrPolyOrRFByRatOrPolyOrRF(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!rightCopy.ConvertToTypE<RationalFunctionOld>())
    return false;
  if(!output.ConvertToTypE<RationalFunctionOld>())
    return false;
  RationalFunctionOld result;
  result=output.GetValuE<RationalFunctionOld>();
  result*=rightCopy.GetValuE<RationalFunctionOld>();
  output.MakeRF(*output.owner, result, output.theContextIndex);
  return true;
}

bool Data::DivideRFOrPolyOrRatByRFOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::DivideRFOrPolyOrRatByRFOrPoly");
  output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!rightCopy.ConvertToTypE<RationalFunctionOld>())
    return false;
  if(!output.ConvertToTypE<RationalFunctionOld>())
    return false;
  RationalFunctionOld result;
  result=output.GetValuE<RationalFunctionOld>();
  if (rightCopy.GetValuE<RationalFunctionOld>().IsEqualToZero())
  { output.SetError("Error:division by zero");
    return true;
  }
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
  result/=rightCopy.GetValuE<RationalFunctionOld>();
//  std::cout << " to get " << result.ToString();
  output.MakeRF(*output.owner, result, output.theContextIndex);
  return true;
}

bool Data::MultiplyRatOrPolyByRatOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!rightCopy.ConvertToTypE<Polynomial<Rational> >())
    return false;
  if(!output.ConvertToTypE<Polynomial<Rational> >())
    return false;
  Polynomial<Rational> result;
  result=output.GetValuE<Polynomial<Rational> >();
  result*=rightCopy.GetValuE<Polynomial<Rational> >();
  output.MakePoly(*output.owner, result, output.theContextIndex);
  return true;
}

bool Data::AddUEToAny(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(output, rightCopy))
  { //std::cout << "failed to merge contexts!";
    return false;
  }
  if (!rightCopy.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >())
  { //std::cout << "failed to convert " << rightCopy.ToString();
    return false;
  }
  ElementUniversalEnveloping<RationalFunctionOld> result;
  result=output.GetUE();
  result+=rightCopy.GetUE();
  result.Simplify(*output.owner->theGlobalVariableS);
  output.MakeUE(*output.owner, result, output.theContextIndex);
  return true;
}

bool Data::MultiplyLRObyLRO
(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::MultiplyLRObyLSPath");
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& leftMon
  =left.GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon
  =right.GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> outMon;
  outMon=leftMon;
  outMon*=rightMon;
  for (int i=0; i<outMon.generatorsIndices.size; i++)
    if (outMon.Powers[i]>100000 || outMon.Powers[i]<0)
      return output.SetError("The result of this operation is " + outMon.ToString() + " which is outside of the allowed range. ");
  output.reset(*left.owner);
  output.type=output.typeLittelmannRootOperator;
  output.theIndex=
  output.owner->theObjectContainer.theLittelmannOperators.AddNoRepetitionOrReturnIndexFirst
  (outMon);
  return true;
}

bool Data::MultiplyLRObyLSPath
(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::MultiplyLRObyLSPath");
  LittelmannPath outputPath=right.GetValuE<LittelmannPath>();
  WeylGroup& theWeyl=*outputPath.owner;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> theLRO=
  left.GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  for (int i=theLRO.generatorsIndices.size-1; i>=0; i--)
    if (theLRO.generatorsIndices[i]==0 || theLRO.generatorsIndices[i]< -theWeyl.GetDim()
        || theLRO.generatorsIndices[i]> theWeyl.GetDim())
    { std::stringstream out;
      out << " The Littelmann root operator must have an index whose absolute value is "
      << "between 1 and the rank of the ambient Lie algebra, instead I get index  "
      << theLRO.generatorsIndices[i];
      return output.SetError(out.str());
    } else
      for (int j=0; j<theLRO.Powers[i]; j++)
        outputPath.ActByEFDisplayIndex(theLRO.generatorsIndices[i]);
  output.MakeLSpath(*right.owner, right.GetAmbientSSAlgebra(), outputPath.Waypoints);
  return true;
}

bool Data::MultiplyAnyByEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ static bool theGhostHasAppeared=false;
  output=right;
  Data leftCopy=left;
//  std::cout << "<br>before merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
  if (!output.MergeContexts(leftCopy, output))
  { //std::cout << "<br>failed context merge of " << leftCopy.ToString() << " and " << output.ToString();
    return false;
  }
//  std::cout << "<br>after merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
  if (!leftCopy.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> > ())
  { //std::cout << "<br>failed converting to ue of " << leftCopy.ToString();
    return false;
  }
//  std::cout << "<br>after conversion, before multiplying the tensor, left copy is: " << leftCopy.ToString();
  if (!theGhostHasAppeared)
  { std::cout << "Ere I am J.H. ... The ghost in the machine...<br>";
    theGhostHasAppeared=true;
  }
  RationalFunctionOld RFOne, RFZero;
  int numVars=leftCopy.GetNumContextVars();
  RFZero.MakeZero(numVars, leftCopy.owner->theGlobalVariableS);
  RFOne.MakeConst(numVars, 1, leftCopy.owner->theGlobalVariableS);
  ElementTensorsGeneralizedVermas<RationalFunctionOld> outputElt;
//    std::cout << "<br>Multiplying " << leftCopy.GetUE().ToString() << " * " << output.ToString();
  if (!output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().MultiplyOnTheLeft
      (leftCopy.GetUE(), outputElt, leftCopy.owner->theObjectContainer.theCategoryOmodules,
       leftCopy.GetAmbientSSAlgebra(), *leftCopy.owner->theGlobalVariableS, RFOne, RFZero))
  { //std::cout << "<br>failed to multiply on the left";
    return false;
  }
  output.MakeElementTensorGeneralizedVermas(*leftCopy.owner, outputElt, output.theContextIndex);
  return true;
}

bool Data::AddEltTensorToEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ output=left;
//  std::cout << "Adding tensor elt to tensor elt!";
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
  { if (comments!=0)
      *comments << "failed to merge contexts trying to add " << left.ToString() << " to "
      << right.ToString();
    return false;
  }
  ElementTensorsGeneralizedVermas<RationalFunctionOld> result;
  result=output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  result+=rightCopy.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  output.MakeElementTensorGeneralizedVermas(*left.owner, result, output.theContextIndex);
  //std::cout << "result of addition: " << result.ToString();
  return true;
}

bool Data::AddRatOrPolyToRatOrPoly(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::AddRatOrPolyToRatOrPoly");
  output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!output.ConvertToTypE<Polynomial<Rational> >())
    return false;
  if (!rightCopy.ConvertToTypE<Polynomial<Rational> >())
    return false;
  Polynomial<Rational> resultpoly=output.GetValuE<Polynomial<Rational> >();
  resultpoly+=rightCopy.GetValuE<Polynomial<Rational> >();
  output.MakePoly(*output.owner, resultpoly, output.theContextIndex);
  return true;
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

bool Data::TensorAnyByEltTensor(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::TensorAnyByEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!output.ConvertToTypE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    return false;
  if (!rightCopy.ConvertToTypE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    return false;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> resultTensor=output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  resultTensor.TensorOnTheRight
  (rightCopy.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(), *output.owner->theGlobalVariableS);
  output.MakeElementTensorGeneralizedVermas(*output.owner, resultTensor, output.theContextIndex);
  return true;
}

bool Data::AddRatOrAlgebraicToRatOrAlgebraic
(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::AddRatOrAlgebraicToRatOrAlgebraic");
  output=left;
  Data rightCopy=right;
  output.ConvertToTypE<AlgebraicNumber>();
  rightCopy.ConvertToTypE<AlgebraicNumber>();
  AlgebraicNumber resultAlg=output.GetValuE<AlgebraicNumber>();
  if(!(resultAlg+=rightCopy.GetValuE<AlgebraicNumber>()))
    return false;
  output.MakeRationalRadical(*output.owner, resultAlg);
  return true;
}

bool Data::MultiplyRatOrAlgebraicByRatOrAlgebraic
(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::AddRatOrAlgebraicToRatOrAlgebraic");
  output=left;
  Data rightCopy=right;
  output.ConvertToTypE<AlgebraicNumber>();
  rightCopy.ConvertToTypE<AlgebraicNumber>();
  AlgebraicNumber resultAlg=output.GetValuE<AlgebraicNumber>();
  if (!(resultAlg*=rightCopy.GetValuE<AlgebraicNumber>()))
    return false;
  output.MakeRationalRadical(*output.owner, resultAlg);
  return true;
}

bool Data::AddRatOrPolyOrRFToRatOrPolyOrRF(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ MacroRegisterFunctionWithName("Data::AddRatOrPolyOrRFToRatOrPolyOrRF");
  output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!output.ConvertToTypE<RationalFunctionOld>())
    return false;
  if (!rightCopy.ConvertToTypE<RationalFunctionOld>())
    return false;
  RationalFunctionOld resultpoly=output.GetValuE<RationalFunctionOld>();
  resultpoly+=rightCopy.GetValuE<RationalFunctionOld>();
  output.MakeRF(*output.owner, resultpoly, output.theContextIndex);
  return true;
}

bool Data::MultiplyUEByAny(const Data& left, const Data& right, Data& output, std::stringstream* comments)
{ output=left;
  Data rightCopy=right;
  if (!output.MergeContexts(rightCopy, output))
    return false;
  if (!rightCopy.ConvertToTypE<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> result;
  result=output.GetUE();
  result*=rightCopy.GetUE();
  //result.Simplify(*output.owner->theGlobalVariableS);
  output.MakeUE(*output.owner, result, output.theContextIndex);
  return true;
}

bool CommandList::StandardPower
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ToString();
  if (theCommands.DoThePower(theCommands, inputIndexBoundVars, theExpression, comments))
    return true;
  //std::cout << "<br>After do associate: " << theExpression.ToString();
  return false;
}

bool CommandList::StandardTensor
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ToString();
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::StandardTensor");
  if (theCommands.DoTheOperation(theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opTensor()))
    return true;
  if (theCommands.EvaluateDoDistribute
      (theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opTensor(), theCommands.opPlus()))
    return true;
  //std::cout << "<br>After distribute: " << theExpression.ToString();
  if (theCommands.EvaluateDoAssociatE
      (theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opTensor()))
    return true;
//  if (theCommands.EvaluateDoExtractBaseMultiplication(theCommands, inputIndexBoundVars, theExpression, comments))
//    return true;
//  if (theExpression.children.size!=2)
//    return false;
  //std::cout << "<br>After do associate: " << theExpression.ToString();
  return false;
}

bool CommandList::StandardTimes
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ //std::cout << "<br>At start of evaluate standard times: " << theExpression.ToString();
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::StandardTimes");
  if (theCommands.DoTheOperation(theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opTimes()))
    return true;
  if (theCommands.EvaluateDoDistribute
      (theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opTimes(), theCommands.opPlus()))
    return true;
  //std::cout << "<br>After distribute: " << theExpression.ToString();
  if (theCommands.EvaluateDoAssociatE
      (theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opTimes()))
    return true;
  if (theCommands.EvaluateDoExtractBaseMultiplication(theCommands, inputIndexBoundVars, theExpression, comments))
    return true;
  if (theExpression.children.size!=2)
    return false;
  //std::cout << "<br>After do associate: " << theExpression.ToString();
  return false;
}

bool CommandList::EvaluateDoExtractBaseMultiplication
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::EvaluateDoExtractBaseMultiplication");
  if (theExpression.children.size!=2 || theExpression.theOperation!=theCommands.opTimes())
    return false;
  bool result=false;
  Expression& leftE=theExpression.children[0];
  Expression& rightE=theExpression.children[1];
  //std::cout << "<br>handling base extraction of: " << theExpression.ToString();
  //handle Rational*Rational:
  if (leftE.EvaluatesToAtom() && rightE.EvaluatesToAtom())
  { const Data& leftD=leftE.GetAtomicValue();
    const Data& rightD=rightE.GetAtomicValue();
    if (leftD.type==leftD.typeRational && rightD.type==rightD.typeRational)
    { theExpression.MakeAtom(leftD*rightD, theCommands, inputIndexBoundVars);
      return true;
    }
  }
  //handle Anything*Rational:=Rational*Anything
  if (rightE.EvaluatesToRational())
  { MathRoutines::swap(leftE, rightE);
    result=true;
//    std::cout << "swapped " << leftE.ToString() << " and " << rightE.ToString();
  }
  //handle (non-atom)*Atom
  if (rightE.EvaluatesToAtom() && !leftE.EvaluatesToAtom())
    if (!rightE.EvaluatesToVariableNonBound())
    { MathRoutines::swap(leftE, rightE);
      result=true;
    }
  if (rightE.theOperation==theCommands.opTimes())
  { assert(rightE.children.size==2);
    Expression& rightLeftE=rightE.children[0];
    //handle Anything1*(Rational*Anything2):=Rational*(Anything1*Anything2)
    if (rightLeftE.EvaluatesToRational())
    { MathRoutines::swap(rightLeftE, leftE);
      result=true;
//      std::cout << " swapped " << rightLeftE.ToString() << " and " << leftE.ToString();
    }
    //handle Rational1*(Rational2*Anything):=(Rational1*Rational2)*Anything
    if (leftE.EvaluatesToRational() && rightLeftE.EvaluatesToRational())
    { leftE.MakeAtom(leftE.GetRationalValue()*rightLeftE.GetRationalValue(), theCommands, inputIndexBoundVars);
      rightE.AssignChild(1);
      result=true;
    } else if (leftE.EvaluatesToAtom() && rightLeftE.EvaluatesToAtom()) //<- handle atom*(atom*anything)
    { Expression tempExp;
      tempExp.MakeProducT(theCommands, inputIndexBoundVars, leftE, rightLeftE);
      if (theCommands.DoTheOperation(theCommands, inputIndexBoundVars, tempExp, 0, theCommands.opTimes()))
      { Expression tempExp2=rightE.children[1];
        theExpression.MakeProducT(theCommands, inputIndexBoundVars, tempExp, tempExp2);
        return true;
      }
    }
  }
  //handle 0*Anything:=0
  if (leftE.EvaluatesToRational())
    if (leftE.GetRationalValue().IsEqualToZero())
    { theExpression.MakeInt(0, theCommands, inputIndexBoundVars);
      result=true;
    }
  return result;
}

bool CommandList::EvaluateDoAssociatE
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theOperation)
{ List<Expression>& opands=theCommands.buffer1;
  opands.SetSize(0);
  //std::cout << "<br>At start of do associate: " << theExpression.ToString();
  bool needsModification=theCommands.AppendOpandsReturnTrueIfOrderNonCanonical
  (theExpression, opands, theOperation);
  if (!needsModification)
    return false;
  Expression* currentExpression;
  currentExpression=&theExpression;
  Expression emptyE;
  for (int i=0; i<opands.size-1; i++)
  { currentExpression->MakeXOX(theCommands, inputIndexBoundVars, theOperation, opands[i], emptyE);
    currentExpression=&currentExpression->children[1];
  }
  *currentExpression=*opands.LastObject();
  //std::cout << "<br>At end do associate: " << theExpression.ToString();
  return true;
}

bool CommandList::StandardIsDenotedBy
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::StandardIsDenotedBy");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (theExpression.children.size!=2)
  { std::cout << "This is a programming error: operators isDenotedBy takes exactly two arguments, but I am getting "
    << theExpression.children.size << " arguments instead. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Expression& withNotation=theExpression.children[1];
  Expression& theNotation=theExpression.children[0];
  if (comments!=0)
    *comments << "<br>Registering notation: globally, " << withNotation.ToString() << " will be denoted by "
    << theNotation.ToString();
  theCommands.theObjectContainer.ExpressionNotation.AddOnTop(theNotation.ToString());
  theCommands.theObjectContainer.ExpressionWithNotation.AddOnTop(withNotation);
  if (withNotation.EvaluatesToAtom())
    if (withNotation.GetAtomicValue().IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
      if (withNotation.GetAtomicValue().GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().IsHWV())
      { MonomialGeneralizedVerma<RationalFunctionOld>& theElt=withNotation.GetAtomicValue().GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()[0].theMons[0];
        theElt.GetOwner().highestWeightVectorNotation=theNotation.ToString();
      }
  theExpression.theOperation=theCommands.opDefine();
  return true;
}

bool CommandList::EvaluateDoDistribute
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
{ Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if ((left.theOperation==theCommands.opPlus() || left.theOperation==theCommands.opMinus()) && left.children.size==2)
  { theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft(theCommands, inputIndexBoundVars, theExpression, comments, theMultiplicativeOP, theAdditiveOp);
    return true;
  }
  if ((right.theOperation==theCommands.opPlus() || right.theOperation==theCommands.opMinus()) && right.children.size==2)
  { theCommands.EvaluateDoRightDistributeBracketIsOnTheRight(theCommands, inputIndexBoundVars, theExpression, comments, theMultiplicativeOP);
    return true;
  }
  return false;
}

bool CommandList::EvaluateDoLeftDistributeBracketIsOnTheLeft
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP, int theAdditiveOp)
{ if (theExpression.theOperation!=theMultiplicativeOP)
    return false;
  if (theExpression.children[0].theOperation!=theAdditiveOp)
    return false;
  if (theExpression.children[0].children.size!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(theCommands, inputIndexBoundVars);
  theExpression.theOperation=theAdditiveOp;
  theExpression.children.SetSize(2);
  theExpression.children[0].MakeXOX(theCommands, inputIndexBoundVars, theMultiplicativeOP, left.children[0], right);
  theExpression.children[1].MakeXOX(theCommands, inputIndexBoundVars, theMultiplicativeOP, left.children[1], right);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::EvaluateDoRightDistributeBracketIsOnTheRight
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments, int theMultiplicativeOP)
{ if (theExpression.theOperation!=theMultiplicativeOP)
    return false;
  int rightOp=theExpression.children[1].theOperation;
  if (rightOp!=theCommands.opPlus() && rightOp!=theCommands.opMinus())
    return false;
  if (theExpression.children[1].children.size!=2)
    return false;
  Expression left=theExpression.children[0];
  Expression right=theExpression.children[1];
  int theFormat=theExpression.format;
  theExpression.reset(theCommands, inputIndexBoundVars);
  theExpression.theOperation=rightOp;
  theExpression.children.SetSize(2);
  theExpression.children[0].MakeXOX(theCommands, inputIndexBoundVars, theMultiplicativeOP, left, right.children[0]);
  theExpression.children[1].MakeXOX(theCommands, inputIndexBoundVars, theMultiplicativeOP, left, right.children[1]);
  theExpression.children[0].format=theFormat;
  theExpression.children[1].format=theFormat;
  return true;
}

bool CommandList::StandardPlus
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::StandardPlus");
  if (theExpression.children.size==2)
  { Expression& leftE=theExpression.children[0];
    Expression& rightE=theExpression.children[1];
    if (leftE.EvaluatesToAtom() && rightE.EvaluatesToAtom())
    { Data outputD;
//      std::cout << "<br>Getting additive cruncher for " << leftE.ToString( ) << " + " << rightE.ToString();
      DataCruncher::CruncherDataTypes theCruncher= theCommands.GetAdditiveCruncher(leftE.GetAtomicValue().type, rightE.GetAtomicValue().type);
      if (theCruncher!=0)
        if (theCruncher(leftE.GetAtomicValue(), rightE.GetAtomicValue(), outputD, comments))
        { theExpression.MakeAtom(outputD, theCommands, inputIndexBoundVars);
          return true;
        }
    }
  }
  return theCommands.CollectSummands(inputIndexBoundVars, theExpression);
}

bool CommandList::EvaluateIf
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=3)
  { theExpression.errorString="Programming error: operation :if := takes three arguments.";
    return true;
  }
  Expression& left= theExpression.children[0];
  if (!left.EvaluatesToAtom())
    return false;
assert(false);
//  if (theCommands.theData[left.theData].IsEqualToOne())
  { theExpression.AssignChild(1);
    return true;
  }
//  if (theCommands.theData[left.theData].IsEqualToZero())
  { theExpression.AssignChild(2);
    return true;
  }
  return false;
}

bool CommandList::EvaluateDereferenceOneArgument
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.theOperation!=theCommands.opApplyFunction())
    return false;
  Expression& functionNameNode =theExpression.children[0];
  Expression& functionArgumentNode=theExpression.children[1];
  if (!functionNameNode.EvaluatesToAtom() )
    return false;
  const Data& theFunData=functionNameNode.GetAtomicValue();
  Data tempData(theCommands);
  if (functionArgumentNode.theOperation!=theCommands.opList())
  { if (!functionArgumentNode.EvaluatesToAtom())
      return false;
    if (!theFunData.OperatorDereference(functionArgumentNode.GetAtomicValue(), tempData, comments))
      return false;
    theExpression.MakeAtom(tempData, theCommands, inputIndexBoundVars);
    return true;
  }
  if (functionArgumentNode.children.size!=2)
    return false;
  Expression& leftE=functionArgumentNode.children[0];
  Expression& rightE=functionArgumentNode.children[1];
  if (!leftE.EvaluatesToAtom() || !rightE.EvaluatesToAtom())
    return false;
  const Data& leftD=leftE.GetAtomicValue();
  const Data& rightD=rightE.GetAtomicValue();
  if (!theFunData.OperatorDereference(leftD, rightD, tempData, comments))
    return false;
  theExpression.MakeAtom(tempData, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::StandardFunction
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::StandardFunction");
  if (theExpression.theOperation!=theCommands.opApplyFunction())
    return false;
  if (theExpression.children.size==0)
  { theExpression.errorString=
    "Programming error: function has no name; this should never happen. \
    Please dubug function CommandList::StandardFunction";
    return true;
  }
//  if (theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft
//      (theCommands, inputIndexBoundVars, theExpression, theCommands.opFunction(), theCommands.opPlus()))
//    return true;
//  if (theCommands.EvaluateDoLeftDistributeBracketIsOnTheLeft
//      (theCommands, inputIndexBoundVars, theExpression, theCommands.opFunction(), theCommands.opTimes()))
//    return true;
  Expression& functionNameNode =theExpression.children[0];
  if (functionNameNode.EvaluatesToAtom())
  { const Data& theFunData=functionNameNode.GetAtomicValue();
    if (theFunData.type==Data::typeRational)
    { theExpression.AssignChild(0);
      return true;
    }
    if (theCommands.EvaluateDereferenceOneArgument(theCommands, inputIndexBoundVars, theExpression, comments))
      return true;
  }
  assert(theExpression.children.size==2);
  int theIndex;
  if (functionNameNode.theOperation==theCommands.opList())
    if (theExpression.children[1].EvaluatesToSmallInteger(&theIndex))
      if (theIndex<=functionNameNode.children.size && theIndex>0)
      { functionNameNode.AssignChild(theIndex-1);
        theExpression.AssignChild(0);
        return true;
      }
  if (theCommands.DoTheOperation(theCommands, inputIndexBoundVars, theExpression, comments, theCommands.opApplyFunction()))
    return true;
  if (!functionNameNode.EvaluatesToVariableNonBound())
    return false;
  Function::FunctionAddress theFun;
  theFun= functionNameNode.GetFunctionAddressFromVarName();
  if (theFun==0)
    return false;
  Function& funHandle=functionNameNode.GetFunctionFromVarNamE();
  Expression result;
  result=theExpression.children[1];
  List<std::string>& theArgs=funHandle.theArgumentList.GetElement();
  if (theArgs.size>0)
  { bool isGood=false;
    for (int i=0; i<theArgs.size; i++)
    { Expression& thePattern=funHandle.theArgumentPatterns.GetElement()[i];
      if (!funHandle.theArgumentPatternIsParsed.GetElement()[i])
      { std::cout << "<br>parsing pattern: " << theArgs[i];
        List<SyntacticElement> theSynSoup, theSynStack;
        std::string err;
        if (!theCommands.ParseAndExtractExpressions(theArgs[i], thePattern, theSynSoup, theSynStack, &err))
        { std::cout << "This is a programming error: failed to parse the hard-coded argument list string " << theArgs[i]
          << " encoding the built-in function " << funHandle.ToString(theCommands)
          << ".  The error message was: " << err
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
        std::cout << " to get: " << thePattern.ToString();
      }
      ExpressionPairs tempPairs;
      if (theCommands.ExpressionMatchesPattern(thePattern, result, tempPairs))
      { isGood=true;
        break;
      }
    }
    if (!isGood)
      return false;
  }
  if (result.HasBoundVariables() && !funHandle.flagMayActOnBoundVars)
    return false;
  if(theFun(theCommands, inputIndexBoundVars, result, comments))
  { theExpression=result;
    return true;
  }
  return false;
}

bool CommandList::StandardEqualEqual
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ assert(theExpression.children.size==2);
  Expression& left=theExpression.children[0];
  Expression& right=theExpression.children[1];
  if (left.HasBoundVariables() || right.HasBoundVariables())
    return false;
  if (left==right)
    theExpression.MakeInt(1, theCommands, inputIndexBoundVars);
  else
    theExpression.MakeInt(0, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::StandardLieBracket
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=2)
    return false;
  assert(theExpression.theOperation=theCommands.opLieBracket());
  Expression& leftE=theExpression.children[0];
  Expression& rightE=theExpression.children[1];
  if (!leftE.EvaluatesToAtom() || !rightE.EvaluatesToAtom())
    return false;
  const Data& leftD=leftE.GetAtomicValue();
  const Data& rightD=rightE.GetAtomicValue();
  Data newData(theCommands);
//  std::cout << "<br>attempting to lie bracket " << leftD.ToString() << " and " << rightD.ToString();
  if (!Data::LieBracket(leftD, rightD, newData, comments))
  { //std::cout  << "<br>Lie bracket unsucessful";
    return false;
  }
  theExpression.MakeAtom(newData, theCommands, inputIndexBoundVars);
  return true;
}

bool Expression::IsSmallInteger(int* whichInteger)const
{ if (!this->EvaluatesToAtom())
    return false;
  return this->GetAtomicValue().IsSmallInteger(whichInteger);
}

bool Expression::IsInteger()const
{ if (!this->EvaluatesToAtom())
    return false;
  return this->GetAtomicValue().IsInteger();
}

bool Expression::IsElementUE()const
{ if (!this->EvaluatesToAtom())
    return false;
  return this->GetAtomicValue().type==Data::typeElementUE;
}

bool Expression::IsString()const
{ if (!this->EvaluatesToAtom())
    return false;
  return this->GetAtomicValue().type==Data::typeString;
}

bool Expression::EvaluatesToSmallInteger(int* whichInteger)const
{ if (!this->EvaluatesToAtom())
    return false;
  return this->GetAtomicValue().IsSmallInteger(whichInteger);
}

bool CommandList::StandardUnion
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size==1)
  { theExpression.AssignChild(0);
    return true;
  }
  if (theExpression.children.size==0)
    return false;
  Expression resultExpression;
  resultExpression.reset(theCommands, inputIndexBoundVars);
  resultExpression.theOperation=theCommands.opList();
  for (int i=0; i<theExpression.children.size; i++)
    if (theExpression.children[i].theOperation!=theCommands.opList())
      return false;
  for (int i=0; i<theExpression.children.size; i++)
    resultExpression.children.AddListOnTop(theExpression.children[i].children);
  theExpression=resultExpression;
  theExpression.format=theExpression.formatDefault;
  return true;
}

bool CommandList::StandardUnionNoRepetition
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size==1)
  { theExpression.AssignChild(0);
    return true;
  }
  if (theExpression.children.size==0)
    return false;
  for (int i=0; i<theExpression.children.size; i++)
    if (theExpression.children[i].theOperation!=theCommands.opList())
      return false;
  HashedList<Expression> tempList;
  for (int i=0; i<theExpression.children.size; i++)
    tempList.AddNoRepetition(theExpression.children[i].children);
  theExpression.theOperation=theCommands.opList();
  theExpression.theDatA=-1;
  theExpression.children=tempList;
  theExpression.format=theExpression.formatDefault;
  return true;
}

bool CommandList::StandardDivide
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::StandardDivide");
  if (theExpression.children.size!=2)
    return false;
  Expression& leftE= theExpression.children[0];
  Expression& rightE= theExpression.children[1];
  if (rightE.EvaluatesToRational())
  { Rational tempRat=rightE.GetAtomicValue().GetValuE<Rational>();
    if (tempRat.IsEqualToZero())
      return theExpression.SetError("Error: division by zero.");
    tempRat.Invert();
    theExpression.theOperation=theCommands.opTimes();
    rightE.MakeAtom(tempRat, theCommands, inputIndexBoundVars);
    return true;
  }
  if (!leftE.EvaluatesToAtom() || !rightE.EvaluatesToAtom())
    return false;
  const Data& LeftD=leftE.GetAtomicValue();
  const Data& RightD=rightE.GetAtomicValue();
  Data outputD;
//  std::cout << "<br>attempting to make standard multiplication between <br>" << RightD.ToString() << " and " << LeftD.ToString();
  DataCruncher::CruncherDataTypes theCruncher= theCommands.GetDivCruncher(LeftD.type, RightD.type);
  if (theCruncher==0)
    return false;
  if (!theCruncher(LeftD, RightD, outputD, comments))
    return false;
//  std::cout << "<br> multiplication successful, result: " << outputD.ToString();
  theExpression.MakeAtom(outputD, theCommands, inputIndexBoundVars);
  return true;
}

bool CommandList::StandardMinus
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ if (theExpression.children.size!=1&& theExpression.children.size!=2)
    return theExpression.SetError("Programming error: operation - takes one or two arguments.");
  Expression* toBeTransformed=0;
  if (theExpression.children.size==1)
  { theExpression.AssignChild(0);
    toBeTransformed=&theExpression;
  } else
  { toBeTransformed=& theExpression.children[1];
    theExpression.theOperation=theCommands.opPlus();
  }
  Expression result, minusOne;
  minusOne.MakeInt(-1, theCommands, inputIndexBoundVars);
  result.MakeProducT(theCommands, inputIndexBoundVars, minusOne, *toBeTransformed);
  *toBeTransformed=result;
  //std::cout << toBeTransformed->ToString();
  return true;
}

bool Expression::operator==(const Expression& other)const
{ if (this->theBoss==0)
    assert(false);
  return this->theBoss->ExpressionsAreEqual(*this, other);
}

template <class dataType>
bool CommandList::ExtractPMTDtreeContext
(Context& outputContext, const Expression& theInput, std::stringstream* errorLog)
{ RecursionDepthCounter theRecursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { if (errorLog!=0)
      *errorLog << "Max recursion depth of " << this->MaxRecursionDeptH
      << " exceeded while trying to evaluate polynomial expression "
      << "(i.e. your polynomial expression is too large).";
    return false;
  }
  if (theInput.theOperation==this->opTimes() || theInput.theOperation==this->opPlus() || theInput.theOperation==this->opMinus())
  { for (int i=0; i<theInput.children.size; i++)
      if (!this->ExtractPMTDtreeContext<dataType>(outputContext, theInput.children[i], errorLog))
        return false;
    return true;
  }
  int tempI;
  if (theInput.theOperation==this->opThePower())
    if (theInput.children[1].EvaluatesToRational())
      if (theInput.children[1].GetAtomicValue().IsSmallInteger(&tempI))
        return this->ExtractPMTDtreeContext<dataType>(outputContext, theInput.children[0], errorLog);
  if (theInput.EvaluatesToAtom() && !theInput.EvaluatesToVariableNonBound())
  { if (theInput.GetAtomicValue().theContextIndex!=-1)
    { //std::cout << "<br>ExtractPMTDtreeContext accounted " << theInput.ToString();
      if (!outputContext.MergeContextWith(theInput.GetAtomicValue().GetContext()))
      { if (errorLog!=0)
          *errorLog << " Failed to merge the context " << outputContext.ToString() << " to the context of "
          << theInput.GetAtomicValue().ToString();
        return false;
      }
      //std::cout << " with end commulative context: " << outputContext.ToString();
    }
    return true;
  }
  outputContext.VariableImages.QuickSortAscending();
  outputContext.VariableImages.AddNoRepetition(theInput);
//  std::cout << "<br>ExtractPMTDtreeContext accounted " << theInput.ToString() << " with resulting accummulated context: " << outputContext.ToString();
  return true;
}

template <class dataType>
bool CommandList::EvaluatePMTDtree
 (dataType& output, Context& outputContext, const Expression& theInput, std::stringstream* errorLog)
{ if (!this->ExtractPMTDtreeContext<dataType>(outputContext, theInput, errorLog))
    return false;
//  std::cout << "<br>The extracted context is: " << outputContext.ToString();
  return this->EvaluatePMTDtreeFromContextRecursive(output, outputContext, theInput, errorLog);
}

template <class dataType>
bool CommandList::EvaluatePMTDtreeFromContextRecursive
(dataType& output, const Context& inputContext,
 const Expression& theInput, std::stringstream* errorLog)
{ MacroRegisterFunctionWithName("CommandList::EvaluatePMTDtreeFromContextRecursive");
  RecursionDepthCounter theRecursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { if (errorLog!=0)
      *errorLog << "Max recursion depth of " << this->MaxRecursionDeptH
      << " exceeded while trying to evaluate polynomial expression (i.e. your polynomial expression is too large).";
    return false;
  }
  if (theInput.theOperation==this->opTimes() || theInput.theOperation==this->opPlus() || theInput.theOperation==this->opMinus())
  { dataType bufferData;
    for (int i=0; i<theInput.children.size; i++)
    { if (!this->EvaluatePMTDtreeFromContextRecursive(bufferData, inputContext, theInput.children[i], errorLog))
        return false;
      //std::cout << "<hr>Status outputBuffer data after variable change: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
      //std::cout << "<hr>Status bufferData data after variable change: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
      if (theInput.theOperation==this->opTimes())
      { if (i==0)
          output=bufferData;
        else
        { //std::cout << "<hr>Multiplying: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
          //<< " and " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
          output*=bufferData;
          //std::cout << "<br>Result: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat) << "<br>";
        }
      }
      else if (theInput.theOperation==this->opMinus())
      { if (theInput.children.size==1)
        { output=bufferData;
          output*=-1;
        } else if (i==0)
          output=bufferData;
        else
          output-=(bufferData);
      } else if (theInput.theOperation==this->opPlus())
      { //std::cout << "<hr>Status outputBuffer data before addition: " << outputBuffer.ToString(this->theGlobalVariableS->theDefaultLieFormat);
        if (i==0)
        { output=bufferData;
//          std::cout << "<hr> outputBuffer has been set to: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
//          << ", which should equal the bufferData: " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
        }
        else
        { //std::cout << "<hr>Adding: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat)
          //<< " and " << bufferData.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
          output+=bufferData;
          //std::cout << "<hr>Result: " << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat) << "<br>";
        }
      }
    }
    return true;
  }
  if (theInput.theOperation==this->opThePower())
  { assert(theInput.children.size==2);
    Expression& right= theInput.children[1];
    if (right.EvaluatesToAtom())
    { int thePower=0;
      if (right.GetAtomicValue().IsSmallInteger(&thePower))
      { if(!this->EvaluatePMTDtreeFromContextRecursive(output, inputContext, theInput.children[0], errorLog))
          return false;
        output.RaiseToPower(thePower);
        return true;
      }
    }
  }
  if (theInput.EvaluatesToAtom() && !theInput.EvaluatesToVariableNonBound())
  { Data theData=theInput.GetAtomicValue();
//    std::cout << "<hr>attempting to convert " << theData.ToString() << "<br>";
    if (!theData.ConvertToTypeResizesContextArrays<dataType>(inputContext))
    { if (errorLog!=0)
        *errorLog << "Attempt to convert " << theData.ToString() << " to context " << inputContext.ToString() << " failed.";
       return false;
    }
    output=theData.GetValuE<dataType>();
    return true;
  }
  int theIndex=inputContext.VariableImages.GetIndex(theInput);
  if (theIndex==-1)
  { if (errorLog!=0)
      *errorLog << "Expression" << theInput.ToString() << "  is not contained as a variable image in the context " << inputContext.ToString() << "."
      << " This could be a programming error. ";
    return false;
  }
//  std::cout << "<br>the inputContext is: " << inputContext.ToString();
  output=inputContext.GetPolynomialMonomial<dataType>(theIndex, *this->theGlobalVariableS);
//  std::cout << "<hr>Output buffer status at recursion depth " << RecursionDepth << ": "
//  << outputBuffer.ToString(&this->theGlobalVariableS->theDefaultLieFormat);
  return true;
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
  (const Expression& thePattern, const Expression& input, ExpressionPairs& matchedExpressions,
   std::stringstream* theLog)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  assert(thePattern.theBoss==this && input.theBoss==this);
//  static int ExpressionMatchesPatternDebugCounter=-1;
  const static bool printLocalDebugInfo=false;
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
    out << "Max recursion depth of " << this->MaxRecursionDeptH << " exceeded whlie trying to match expression pattern "
    << thePattern.ToString() << " onto expression " << input.ToString();
    this->evaluationErrors.AddOnTop(out.str());
    return false;
  }
  if (this->opDefine()==input.theOperation)
    return false;
  int opVarB=this->opVariableBound();
  if (thePattern.theOperation== opVarB)
  { int indexLeft= matchedExpressions.BoundVariableIndices.GetIndex(thePattern.theDatA);
    if (indexLeft==-1)
    { matchedExpressions.BoundVariableIndices.AddOnTop(thePattern.theDatA);
      matchedExpressions.variableImages.AddOnTop(input);
      indexLeft=matchedExpressions.BoundVariableIndices.size-1;
    }
    if (!(matchedExpressions.variableImages[indexLeft]==input))
      return false;
    if (printLocalDebugInfo)
      std::cout << "<br><b>Match!</b>";
    return true;
  }
  if (thePattern.theOperation!=input.theOperation || thePattern.theDatA!=input.theDatA || thePattern.children.size!=input.children.size )
    return false;
  for (int i=0; i<thePattern.children.size; i++)
    if (!(this->ExpressionMatchesPattern(thePattern.children[i], input.children[i], matchedExpressions, theLog)))
      return false;
  if (printLocalDebugInfo)
    std::cout << "<br><b>Match!</b>";
  return true;
}

bool CommandList::EvaluateExpressionReturnFalseIfExpressionIsBound
(Expression& theExpression, ExpressionPairs& bufferPairs, std::stringstream* comments)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::EvaluateExpressionReturnFalseIfExpressionIsBound");
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Recursion depth limit of " << this->MaxRecursionDeptH
    << " exceeded while evaluating expressions.";
    theExpression.errorString=out.str();
    if (this->flagMaxRecursionErrorEncountered)
      this->evaluationErrors.AddOnTop(out.str());
    this->flagMaxRecursionErrorEncountered=true;
    return true;
  }
  if (theExpression.errorString!="")
    return true;
  if (theExpression.theOperation<0 || theExpression.theBoss!=this)
    return true;
  if (this->ExpressionStack.Contains(theExpression))
  { std::stringstream errorStream;
    errorStream << "I think I have detected a cycle: " << theExpression.ToString()
    << " is transformed to an expression that contains the starting expression. ";
    theExpression.SetError(errorStream.str());
    return true;
  }
  this->ExpressionStack.AddOnTop(theExpression);
  int indexInCache=-1;
  if (theExpression.IndexBoundVars!=-1)
  { indexInCache=this->theExpressionContext[theExpression.IndexBoundVars].cachedExpressions.GetIndex(theExpression);
    if (indexInCache!=-1)
    { theExpression=
      this->theExpressionContext[theExpression.IndexBoundVars].imagesCachedExpressions[indexInCache];
    } else
      if (this->theExpressionContext[theExpression.IndexBoundVars].cachedExpressions.size<this->MaxNumCachedExpressionPerContext)
      { this->theExpressionContext[theExpression.IndexBoundVars].cachedExpressions.AddOnTop(theExpression);
        indexInCache=this->theExpressionContext[theExpression.IndexBoundVars].cachedExpressions.size-1;
        this->theExpressionContext[theExpression.IndexBoundVars].imagesCachedExpressions.SetSize(indexInCache+1);
      }
  }
  //reduction phase:
  bool resultExpressionIsFree=true;
  bool ReductionOcurred=true;
  int counter=0;
  while (ReductionOcurred)
  { ReductionOcurred=false;
    counter++;
    if (indexInCache!=-1)
      this->theExpressionContext[theExpression.IndexBoundVars].imagesCachedExpressions[indexInCache]=theExpression;
//////------Handling naughty expressions------
    if (this->theGlobalVariableS->GetElapsedSeconds()!=0)
      if (this->theGlobalVariableS->GetElapsedSeconds()>this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2)
      { if (!this->flagTimeLimitErrorDetected)
          std::cout << "<br><b>Max allowed computational time is "
          << this->theGlobalVariableS->MaxComputationTimeSecondsNonPositiveMeansNoLimit/2 << ";  so far, "
          << this->theGlobalVariableS->GetElapsedSeconds()-this->StartTimeEvaluationInSecondS
          << " have elapsed -> aborting computation ungracefully.</b>";
        this->flagTimeLimitErrorDetected=true;
        break;
      }
    if (counter>this->MaxAlgTransformationsPerExpression)
    { if (!this->flagMaxTransformationsErrorEncountered)
      { std::stringstream out;
        out << "<br>Maximum number of algebraic transformations of "
        << this->MaxAlgTransformationsPerExpression << " exceeded."
        << " while simplifying " << theExpression.ToString();
        theExpression.errorString=out.str();
      }
      break;
    }
//////------Handling naughty expressions end------
/////-------Evaluating children -------
    bool foundError=false;
    for (int i=0; i<theExpression.children.size; i++)
    { if(!this->EvaluateExpressionReturnFalseIfExpressionIsBound(theExpression.children[i], bufferPairs, comments))
        resultExpressionIsFree=false;
      if (theExpression.children[i].errorString!="")
      { foundError=true;
        break;
      }
    }
    //->/////-------Default operation handling-------
    Function::FunctionAddress theFun=this->operations[theExpression.theOperation].GetFunction(*this);
    if (theFun!=0)
      if (theFun(*this, theExpression.IndexBoundVars, theExpression, comments))
      { ReductionOcurred=true;
        continue;
      }
    //->/////-------Default operation handling end-------
    if (foundError || !resultExpressionIsFree)
      break;
/////-------Evaluating children end-------
/////-------User-defined pattern matching------
    for (int i=0; i<theExpression.IndexBoundVars; i++)
      if (this->theCommands.children[i].errorString=="")
      { Expression& currentPattern=this->theCommands.children[i];
        if (currentPattern.errorString=="")
          if (currentPattern.theOperation==this->opDefine() || currentPattern.theOperation==this->opDefineConditional())
          { this->TotalNumPatternMatchedPerformed++;
            bufferPairs.reset();
            if(this->ProcessOneExpressionOnePatternOneSub
            (theExpression.IndexBoundVars, currentPattern, theExpression, bufferPairs, comments))
            { ReductionOcurred=true;
              break;
            }
          }
      }
    if (ReductionOcurred)
      continue;
/////-------User-defined pattern matching end------
  }
  this->ExpressionStack.PopIndexSwapWithLast(this->ExpressionStack.size-1);
  if (theExpression.theOperation==this->opVariableBound())
    return false;
  return resultExpressionIsFree;
}

Expression* CommandList::PatternMatch
  (int inputIndexBoundVars, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   Expression* condition, std::stringstream* theLog, bool logAttempts)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>=this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Error: while trying to evaluate expression, the maximum recursion depth of " << this->MaxRecursionDeptH
    << " was exceeded";
    theExpression.errorString=out.str();
    return 0;
  }
  if (!this->ExpressionMatchesPattern(thePattern, theExpression, bufferPairs, theLog))
    return 0;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>found pattern: " << theExpression.ToString() << " -> " << thePattern.ToString();
  if (condition==0)
    return &theExpression;
  Expression tempExp=*condition;
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specializing condition pattern: " << tempExp.ToString();
  this->SpecializeBoundVars(tempExp, inputIndexBoundVars, bufferPairs);
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>Specialized condition: " << tempExp.ToString() << "; evaluating...";
  ExpressionPairs tempPairs;
  this->EvaluateExpressionReturnFalseIfExpressionIsBound
  (tempExp, tempPairs, theLog);
  if (theLog!=0 && logAttempts)
    (*theLog) << "<hr>The evaluated specialized condition: " << tempExp.ToString() << "; evaluating...";
  if (tempExp.EvaluatesToAtom())
    if (tempExp.GetAtomicValue().IsEqualToOne())
      return & theExpression;
  return 0;
}

void CommandList::SpecializeBoundVars
(Expression& toBeSubbed, int targetinputIndexBoundVars, ExpressionPairs& matchedPairs)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  toBeSubbed.IndexBoundVars=targetinputIndexBoundVars;
  if (toBeSubbed.theOperation==this->opVariableBound())
  { int indexMatching= matchedPairs.BoundVariableIndices.GetIndex(toBeSubbed.theDatA);
    toBeSubbed=matchedPairs.variableImages[indexMatching];
//    this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
    return;
  }
  for (int i=0; i<toBeSubbed.children.size; i++)
    this->SpecializeBoundVars(toBeSubbed.children[i], targetinputIndexBoundVars, matchedPairs);
//  this->ExpressionHasBoundVars(toBeSubbed, RecursionDepth+1, MaxRecursionDepth);
}

bool CommandList::ProcessOneExpressionOnePatternOneSub
  (int inputIndexBoundVars, Expression& thePattern, Expression& theExpression, ExpressionPairs& bufferPairs,
   std::stringstream* theLog, bool logAttempts)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  assert(thePattern.theOperation==this->opDefine() ||
  thePattern.theOperation==this->opDefineConditional());
  assert(thePattern.children.size==2 || thePattern.children.size==3);
  if (theLog!=0 && logAttempts)
  { (*theLog) << "<hr>attempting to reduce expression " << theExpression.ToString();
    (*theLog) << " by pattern " << thePattern.ToString();
  }
  Expression& currentPattern=thePattern.children[0];
  Expression* theCondition=0;
  bool isConditionalDefine=thePattern.theOperation==this->opDefineConditional();
  if (isConditionalDefine)
    theCondition=&thePattern.children[1];
  Expression* toBeSubbed=0;
  if ((thePattern.theOperation==this->opDefine()) || isConditionalDefine)
    toBeSubbed=this->PatternMatch
    (inputIndexBoundVars, currentPattern, theExpression, bufferPairs, theCondition, theLog);
  if (toBeSubbed==0)
    return false;
  if (theLog!=0 && logAttempts)
    *theLog << "<br><b>found a match!</b>";
  if (isConditionalDefine)
    *toBeSubbed=thePattern.children[2];
  else
    *toBeSubbed=thePattern.children[1];
  toBeSubbed->IndexBoundVars= inputIndexBoundVars;
  this->SpecializeBoundVars(*toBeSubbed, inputIndexBoundVars, bufferPairs);
  return true;
}

bool CommandList::ExpressionHasBoundVars(Expression& theExpression)
{ RecursionDepthCounter recursionCounter(&this->RecursionDeptH);
  if (this->RecursionDeptH>this->MaxRecursionDeptH)
  { std::stringstream out;
    out << "Max recursion depth of " << this->MaxRecursionDeptH << " exceeded.";
    theExpression.errorString=out.str();
    return false;
  }
  if (theExpression.theOperation==this->opVariableBound())
    return true;
  else
    for (int i=0; i<theExpression.children.size; i++)
      if (this->ExpressionHasBoundVars(theExpression.children[i]))
        return true;
  return false;
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
  this->CreateNewExpressionContext();
  this->parsingLog="";
  for (this->counterInSyntacticSoup=0; this->counterInSyntacticSoup<(*this->CurrrentSyntacticSouP).size; this->counterInSyntacticSoup++)
  { if (this->counterInSyntacticSoup+1<(*this->CurrrentSyntacticSouP).size)
      lookAheadToken=this->controlSequences[(*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup+1].controlIndex];
    else
      lookAheadToken=";";
    (*this->CurrentSyntacticStacK).AddOnTop((*this->CurrrentSyntacticSouP)[this->counterInSyntacticSoup]);
    if ((*this->CurrentSyntacticStacK).LastObject()->controlIndex==this->conEndStatement())
      this->CreateNewExpressionContext();
    Expression& currentExpression=(*this->CurrentSyntacticStacK).LastObject()->theData;
    currentExpression.IndexBoundVars=this->theExpressionContext.size-1;
    while(this->ApplyOneRule(lookAheadToken))
    {}
  }
  bool success=false;
  if ((*this->CurrentSyntacticStacK).size==this->numEmptyTokensStart+1)
  { SyntacticElement& result=(*this->CurrentSyntacticStacK)[this->numEmptyTokensStart];
    if (result.ErrorString=="" && result.controlIndex==this->conExpression())
    { outputExpression=result.theData;
      success=true;
    }
    else if (result.ErrorString!="")
      errorLog << "Syntax error with message: " << result.ErrorString;
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
  ExpressionPairs thePairs;

//  this->theLogs.resize(this->theCommands.size());
//this->ToString();
  std::stringstream comments;
  if (this->syntaxErrors!="")
  { out << "<hr><b>Syntax errors encountered</b><br>";
    out << this->syntaxErrors;
    out << "<hr>";
  }
  Expression StartingExpression=this->theCommands;
  this->RecursionDeptH=0;
  this->EvaluateExpressionReturnFalseIfExpressionIsBound
  (this->theCommands, thePairs, &comments);
  if (this->RecursionDeptH!=0)
  { std::cout << "This is a programming error: the starting recursion depth before evaluation was 0, but after evaluation it is "
    << this->RecursionDeptH << ". Please debug file " << CGI::GetHtmlLinkFromFileName(__FILE__) << " line " << __LINE__ << ".";
    assert(false);
  }
  this->theGlobalVariableS->theDefaultFormat.flagMakingExpressionTableWithLatex=true;
  this->theGlobalVariableS->theDefaultFormat.flagUseLatex=true;
  out << this->theCommands.ToString(&this->theGlobalVariableS->theDefaultFormat, false, false, &comments, true, &StartingExpression); //<< "</td></tr>";
  this->outputString=out.str();
  if (comments.str()!="")
  { std::stringstream commentsStream;
    commentsStream << "<b>Comments.</b><br><span>" << comments.str() << "</span>";
    this->outputCommentsString=commentsStream.str();
  }
}

std::string SyntacticElement::ToString(CommandList& theBoss)const
{ std::stringstream out;
  bool makeTable=this->controlIndex==theBoss.conExpression() || this->controlIndex==theBoss.conError()
  || this->controlIndex==theBoss.conList() ;
  if (makeTable)
    out << "<table border=\"1\"><tr><td>";
  if (this->controlIndex<0)
    out << "Error: control index is not initialized! This is likely a programming error.";
  else
    out << theBoss.controlSequences[this->controlIndex];
  if (makeTable)
  { out << "</td></tr><tr><td>";
    out << this->theData.ToString(0, 10);
    if (this->ErrorString!="")
      out << "</td></tr><tr><td>" << this->ErrorString;
    out << "</td></tr></table>";
  }
  return out.str();
}

Function::FunctionAddress VariableNonBound::GetFunction(CommandList& owner)const
{ if (this->HandlerFunctionIndex==-1)
    return 0;
  assert(this->HandlerFunctionIndex>=0 && this->HandlerFunctionIndex <owner.theFunctions.size);
  return owner.theFunctions[this->HandlerFunctionIndex].theFunction;
}

Function& Expression::GetFunctionFromVarNamE()
{ assert(this->theBoss!=0);
  if (!this->EvaluatesToVariableNonBound())
  { std::cout << "This is a programming error. Attempting to extract a built-in Function name from expression "
    << this->ToString() << " which is not a non-bound variable. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->theBoss->theFunctions[this->GetAtomicValue().GetValuE<VariableNonBound>().HandlerFunctionIndex];
}

Function::FunctionAddress Expression::GetFunctionAddressFromVarName()
{ assert(this->theBoss!=0);
  assert(this->EvaluatesToVariableNonBound());
  return this->GetAtomicValue().GetValuE<VariableNonBound>().GetFunction(*this->theBoss);
}

int Expression::GetNumCols()const
{ if (this->theOperation!=this->theBoss->opList() || this->format!=this->formatMatrix)
    return -1;
  int theMax=1;
  for (int i=0; i<this->children.size; i++)
    if (this->children[i].format==this->formatMatrixRow && this->children[i].theOperation==this->theBoss->opList())
      theMax=MathRoutines::Maximum(this->children[i].children.size, theMax);
  return theMax;
}

bool Expression::operator>(const Expression& other)const
{ if (this->EvaluatesToRational() && other.EvaluatesToRational())
    return this->GetRationalValue()>other.GetRationalValue();
  return this->ToString()>other.ToString();
}

std::string Expression::ToString
(FormatExpressions* theFormat, bool AddBrackets, bool AddCurlyBraces,
 std::stringstream* outComments, bool isFinal, Expression* startingExpression)const
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
  std::string additionalDataComments;
  if (this->errorString!="")
  { if (outComments!=0)
      *outComments << this->errorString << " ";
    return "(Error:SeeComments)";
  }
  if (this->theOperation<0)
  { out << "(NotInitialized)";
    return out.str();
  }
  if (AddBrackets)
    out << "(";
  if (AddCurlyBraces)
    out << "{";
  if (this->EvaluatesToAtom())
  { std::stringstream dataComments;
    out << this->GetAtomicValue().ToString(&dataComments, isFinal, theFormat);
    additionalDataComments=dataComments.str();
  } else if (this->theOperation==this->theBoss->opDefine())
    out << this->children[0].ToString(theFormat, false, false, outComments)
    << ":=" << this->children[1].ToString(theFormat, false, false, outComments);
  else if (this->theOperation==this->theBoss->opIsDenotedBy())
    out << this->children[0].ToString(theFormat, false, false, outComments)
    << ":=:" << this->children[1].ToString(theFormat, false, false, outComments);
  else if (this->theOperation==this->theBoss->opDefineConditional())
    out <<  this->children[0].ToString(theFormat, false, false, outComments) << " :if "
    << this->children[1].ToString(theFormat, true, false, outComments)
    << ":=" << this->children[2].ToString(theFormat, false, false, outComments);
  else if (this->theOperation==this->theBoss->opDivide() )
    out << this->children[0].ToString(theFormat, this->children[0].NeedBracketsForMultiplication(), false, outComments)
    << "/" << this->children[1].ToString(theFormat, this->children[1].NeedBracketsForMultiplication(), false, outComments);
  else if (this->theOperation==this->theBoss->opTensor() )
  { out << this->children[0].ToString(theFormat, this->children[0].NeedBracketsForMultiplication(), false, outComments)
    << "\\otimes " << this->children[1].ToString(theFormat, this->children[1].NeedBracketsForMultiplication(), false, outComments);
  } else if (this->theOperation==this->theBoss->opTimes() )
  { std::string tempS=this->children[0].ToString(theFormat, this->children[0].NeedBracketsForMultiplication(), false, outComments);
    //if (false)
   // {
    if (tempS=="-1")
      tempS="-";
    if (tempS=="1")
      tempS="";
    //} else
      //tempS="("+tempS+")";
    out << tempS;
    if (this->format==this->formatTimesDenotedByStar && tempS!="-" && tempS!="")
      out << "*"; else out << " ";
    out << this->children[1].ToString(theFormat, this->children[1].NeedBracketsForMultiplication(), false, outComments);
  } else if (this->theOperation==this->theBoss->opThePower())
    out << this->children[0].ToString(theFormat, this->children[0].NeedBracketsForThePower(), false, outComments)
    << "^{" << this->children[1].ToString(theFormat, false, false, outComments) << "}";
  else if (this->theOperation==this->theBoss->opPlus() )
  { assert(this->children.size>=2);
    std::string tempS2= this->children[0].ToString(theFormat, this->children[0].NeedBracketsForAddition(), false, outComments);
    std::string tempS=this->children[1].ToString(theFormat, this->children[1].NeedBracketsForAddition(), false, outComments);
    out << tempS2;
    if (tempS2.size()% 200>100)
      out << "\\\\\n";
    if (tempS.size()>0)
      if (tempS[0]!='-')
        out << "+";
    out << tempS;
  } else if (this->theOperation==this->theBoss->opMinus())
  { if ( this->children.size==1)
      out << "-" << this->children[0].ToString
      (theFormat, this->children[0].NeedBracketsForMultiplication(), false, outComments);
    else
    { assert(children.size==2);
      out << this->children[0].ToString(theFormat, this->children[0].NeedBracketsForAddition(), false, outComments)
      << "-" << this->children[1].ToString(theFormat, this->children[1].NeedBracketsForMultiplication(), false, outComments);
    }
  }
  else if (this->theOperation==this->theBoss->opVariableBound())
  { if (this->IndexBoundVars<0 || this->IndexBoundVars>= this->theBoss->theExpressionContext.size)
      out << "(BadContext=" << this->IndexBoundVars << ")";
    assert(this->theBoss->theExpressionContext[this->IndexBoundVars].BoundVariables.size>0);
    assert(this->theBoss->theExpressionContext[this->IndexBoundVars].BoundVariables.size>this->theDatA);
    out << "{{" << this->theBoss->theExpressionContext[this->IndexBoundVars].BoundVariables[this->theDatA] << "}}";
  }
  else if (this->theOperation==this->theBoss->opApplyFunction())
  { assert(this->children.size>=2);
    switch(this->format)
    { case Expression::formatFunctionUseUnderscore:
        out <<  this->children[0].ToString
        (theFormat, false, this->children[0].NeedBracketsForFunctionName(), outComments)
        << "_" << this->children[1].ToString
        (theFormat, false, this->children[1].NeedBracketsForFunctionName(), outComments) << "";
        break;
      case Expression::formatFunctionUseCdot:
        out <<  this->children[0].ToString
        (theFormat, this->children[0].NeedBracketsForFunctionName(), false, outComments)
        << "\\cdot(" << this->children[1].ToString(theFormat, false, false, outComments) << ")";
        break;
      default:
        out << this->children[0].ToString
        (theFormat, this->children[0].NeedBracketsForFunctionName(), false, outComments)
        << "{}(" << this->children[1].ToString(theFormat, this->children[1].NeedBracketsForFunctionArgument(), false, outComments) << ")";
        break;
    }
  }
  else if (this->theOperation==this->theBoss->opEqualEqual())
    out << this->children[0].ToString(theFormat, false, false, outComments)
    << "==" << this->children[1].ToString(theFormat, false, false, outComments);
  else if (this->theOperation==this->theBoss->opList())
  { switch (this->format)
    { case Expression::formatMatrixRow:
        for (int i=0; i<this->children.size; i++)
        { out << this->children[i].ToString(theFormat, false, false, outComments);
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
        for (int i=0; i<this->children.size; i++)
        { out << this->children[i].ToString(theFormat, false, false, outComments);
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
        for (int i=0; i<this->children.size; i++)
        { out << this->children[i].ToString(theFormat, false, false, outComments);
          if (i!=this->children.size-1)
            out << ", \\\\ ";
        }
        out << ")";
        break;
    }
  } else if (this->theOperation==this->theBoss->opLieBracket())
    out << "[" << this->children[0].ToString(theFormat, false, false, outComments)
    << "," << this->children[1].ToString(theFormat, false, false, outComments)
    << "]";
  else if (this->theOperation==this->theBoss->opUnion())
    out << this->children[0].ToString(theFormat, false, false, outComments)
    << "\\cup " << this->children[1].ToString(theFormat, false, false, outComments)
    ;
  else if (this->theOperation==this->theBoss->opUnionNoRepetition())
    out << this->children[0].ToString(theFormat, false, false, outComments)
    << "\\sqcup " << this->children[1].ToString(theFormat, false, false, outComments)
    ;
  else if (this->theOperation==this->theBoss->opEndStatement())
  { if (startingExpression==0)
      out << "<table>";
    for (int i=0; i<this->children.size; i++)
    { out << "<tr><td valign=\"top\">";
      bool createTable=(startingExpression!=0);
      if (createTable)
        createTable=(startingExpression->theOperation==this->theBoss->opEndStatement() && startingExpression->children.size==this->children.size);
      if (createTable)
      { out << "<hr> "
        << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL( startingExpression->children[i].ToString(theFormat, false, false, 0));
        if (i!=this->children.size-1)
          out << ";";
        out << "</td><td valign=\"top\"><hr>";
        if (!this->children[i].IsString() || !isFinal)
          out << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(this->children[i].ToString(theFormat, false, false, outComments));
        else
          out << this->children[i].GetAtomicValue().GetValuE<std::string>();
        if (i!=this->children.size-1)
          out << ";";
      }
      else
      { out << this->children[i].ToString(theFormat, false, false, outComments);
        if (i!=this->children.size-1)
          out << ";";
      }
//      out << "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp Context index: " << this->IndexBoundVars;
      out << "</td></tr>";
    }
    if (startingExpression==0)
      out << "</table>";
  }
  else
    out << "(ProgrammingError:NotDocumented)" ;
  if (AddBrackets)
    out << ")";
  if (AddCurlyBraces)
    out << "}";
  if (this->errorString!="")
    out << ")";
  if (outComments!=0)
    if (additionalDataComments!="")
      *outComments << "Comments to expression " << out.str() << ": " << additionalDataComments;
  if (startingExpression!=0)
  { std::stringstream outTrue;
    outTrue << "<table>";
    outTrue << "<hr><th>Input in LaTeX</th><th>Result in LaTeX</th></tr>";
    outTrue << "<tr><td colspan=\"2\">Double click LaTeX image to get the LaTeX code. "
    << "Javascript LaTeXing courtesy of <a href=\"http://www.math.union.edu/~dpvc/jsmath/\">jsmath</a>: many thanks for your great work!</td></tr>";
    if (this->theOperation==this->theBoss->opEndStatement())
      outTrue << out.str();
    else
    { outTrue << "<tr><td>" << CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(startingExpression->ToString(theFormat));
      if (this->IsString() && isFinal)
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

std::string Expression::ElementToStringPolishForm(int recursionDepth, int maxRecursionDepth)
{ if (maxRecursionDepth>0)
    if(recursionDepth>maxRecursionDepth)
      return "...and so on (maximum depth reached)...";
  if (this->theBoss==0)
    return " non-initialized ";
  std::stringstream out;
  if (this->theOperation<0)
  { out << " operation not initialized ";
    return out.str();
  }
  if (this->children.size>0)
    out << "<table border=\"1\"> <tr><td  align=\"center\" colspan=\"" << this->children.size << "\">";
  out << this->theBoss->operations[this->theOperation].theName << " (operation index " << this->theOperation << ")";
  out << ", " << this->theDatA;
  if (this->children.size>0)
  { out << "</td></tr><td>";
    for (int i=0; i<this->children.size; i++)
    { out << this->children[i].ElementToStringPolishForm(recursionDepth+1, maxRecursionDepth);
      if (i!=this->children.size-1)
        out << "</td><td> ";
    }
    out << "</td></tr>";
  }
  return out.str();
}

std::string VariableNonBound::GetHandlerFunctionName(CommandList& owner)
{ if (this->HandlerFunctionIndex==-1)
    return "";
  return owner.theFunctions[this->HandlerFunctionIndex].theName;
}

void CommandList::AddOperationNoFail
  (const std::string& theOpName,
   const Function::FunctionAddress& theFunAddress,
   const std::string& opArgumentList, const std::string& opDescription,
   const std::string& opExample, bool inputNameUsed
   )
{ VariableNonBound theVar;
  if (theFunAddress!=0)
  { Function currentFunction(theFunAddress, theOpName, opArgumentList, opDescription, opExample, inputNameUsed);
    theVar.HandlerFunctionIndex=this->theFunctions.AddNoRepetitionOrReturnIndexFirst(currentFunction);
  } else
    theVar.HandlerFunctionIndex=-1;
  theVar.theName=theOpName;
  if (this->operations.Contains(theOpName))
  { assert(false);
    return;
  }
  this->operations.AddOnTop(theVar);
}

int CommandList:: AddNonBoundVarReturnVarIndex
(const std::string& theName, const Function::FunctionAddress& funHandler,
  const std::string& argList, const std::string& description, const std::string& exampleArgs
)
{ VariableNonBound theVar;
  theVar.theName=theName;
  if (funHandler!=0)
  { Function theFun(funHandler, theName, argList, description, exampleArgs, true);
    theVar.HandlerFunctionIndex=this->theFunctions.AddNoRepetitionOrReturnIndexFirst(theFun);
  }
  Data theVarData;
  theVarData.MakeVariableNonBound(*this, theVar);
  return this->theData.AddNoRepetitionOrReturnIndexFirst(theVarData);
}

std::string CommandList::ElementToStringNonBoundVars()
{ std::stringstream out;
  std::string openTag1="<span style=\"color:#0000FF\">";
  std::string closeTag1="</span>";
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  out << "<br>\n" << this->theObjectContainer.theNonBoundVars.size << " global variables " << " (= "
  << this->NumPredefinedVars  << " predefined + " << this->theObjectContainer.theNonBoundVars.size-this->NumPredefinedVars
  << " user-defined). <br>Predefined: \n<br>\n";
  for (int i=0; i<this->theObjectContainer.theNonBoundVars.size; i++)
  { out << openTag1 << this->theObjectContainer.theNonBoundVars[i].theName << closeTag1;
    std::string handlerName=this->theObjectContainer.theNonBoundVars[i].GetHandlerFunctionName(*this);
    if (handlerName!="")
      out << " [handled by: " << openTag2 << handlerName << closeTag2 << "]";
    if (i!=this->theObjectContainer.theNonBoundVars.size-1)
    { out << ", ";
      if (i==this->NumPredefinedVars-1 )
        out << "<br>user-defined:\n<br>\n";
    }
  }
  return out.str();
}

std::string Function::ToString(CommandList& theBoss)const
{ std::stringstream out2;
  std::string openTag2="<span style=\"color:#FF0000\">";
  std::string closeTag2="</span>";
  out2 << openTag2 << this->theName << closeTag2;
  std::stringstream out;
  if (!this->flagNameIsVisible)
    out << "This function is invoked indirectly as an operation handler. ";
  out << this->theDescription;
  if (this->theExample!="")
    out << " <br> " << this->theExample << "&nbsp&nbsp&nbsp";
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());
  if (this->theExample!="")
    out2 << "<a href=\""
    << theBoss.DisplayNameCalculator  << "?"
    << " textType=Calculator&textDim=1&textInput="
    << CGI::UnCivilizeStringCGI(this->theExample)
    << "\"> " << " Example" << "</a>" ;
  return out2.str() ;
}

std::string CommandList::ToStringFunctionHandlers()
{ std::stringstream out;
  out << "\n <b>Handler functions (" << this->theFunctions.size << " total).</b><br>\n";
  bool found=false;
  for (int i=0; i<this->theFunctions.size; i++)
    if (this->theFunctions[i].flagNameIsVisible)
    { if (found)
        out << "<br>";
      found=true;
      out << "\n" << this->theFunctions[i].ToString(*this);
    }
  return out.str();
}

std::string ObjectContainer::ToString()
{ std::stringstream out;
  if (this->theLieAlgebras.size>0)
  { out << "Lie algebras created (" << this->theLieAlgebras.size << " total): ";
    for (int i=0; i<this->theLieAlgebras.size; i++)
    { out << this->theLieAlgebras[i].GetLieAlgebraName(true);
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
  out << "<br>\nOperators (" << this->operations.size << " total):<br>\n";
  for (int i=0; i<this->operations.size; i++)
  { out << "\n" << openTag1 << this->operations[i].theName << closeTag1;
    std::string handlerName=this->operations[i].GetHandlerFunctionName(*this);
    if (this->operations[i].GetFunction(*this)!= 0)
      out << " [handled by: " << openTag2 << handlerName << closeTag2 << "]";
    if(i!=this->operations.size-1)
      out << ", ";
  }
  out << this->ElementToStringNonBoundVars();
  out << "<br>\nData entries (" << this->theData.size << " total):\n<br>\n";
  for (int i=0; i<this->theData.size; i++)
  { out << openTag3 << this->theData[i].ToString(0, false) << closeTag3;
    if (i!=this->theData.size-1)
      out  << ", ";
  }
  out << "<hr>";
  for (int k=0; k<this->theExpressionContext.size; k++)
  { ExpressionContext& currentContext=this->theExpressionContext[k];
    out <<"<hr>" << "Context " << k+1;
    out << "<br>\n Cached expressions (" << currentContext.cachedExpressions.size
    << " total):\n<br>\n";
    for (int i=0; i<currentContext.cachedExpressions.size; i++)
    { out << currentContext.cachedExpressions[i].ToString(0, false, false, 0, false)
      << " -> " << currentContext.imagesCachedExpressions[i].ToString(0, false, false, 0, false);
      if (i!=currentContext.cachedExpressions.size-1)
        out << "<br>";
    }
    if (currentContext.BoundVariables.size>0)
    { out << "Bound variables:<br>\n ";
      for (int i=0; i<currentContext.BoundVariables.size; i++)
      { out << currentContext.BoundVariables[i];
        if (i!=currentContext.BoundVariables.size-1)
          out << ", ";
      }
    }
  }
  out2 << CGI::GetHtmlSpanHidableStartsHiddeN(out.str());//, "Expand/collapse.");
  return out2.str();
}

std::string Expression::GetOperation()const
{ if (this->theBoss==0)
    return "(Error:NonInitialized)";
  return this->theBoss->operations[this->theOperation].theName;
}

Rational Expression::GetConstantTerm()const
{ if (this->EvaluatesToRational())
    return this->GetRationalValue();
  if (this->theOperation==this->theBoss->opTimes())
    if (this->children[0].EvaluatesToRational())
      return this->children[0].GetRationalValue();
  return 1;
}

bool Expression::NeedBracketsForFunctionName() const
{ return !(
  this->theOperation==this->theBoss->opVariableBound() ||
  (this->theOperation==this->theBoss->opAtom() && this->EvaluatesToVariableNonBound())
  || ( this->theOperation==this->theBoss->opApplyFunction() && this->format==this->formatFunctionUseUnderscore)
  );
}

bool Expression::NeedBracketsForMultiplication()const
{ //return this->children.size()>1;
  return
  this->theOperation==this->theBoss->opPlus() ||
  this->theOperation==this->theBoss->opMinus()
  ;
}

bool Expression::NeedBracketsForAddition()const
{ if (this->theOperation==this->theBoss->opTimes())
    if (this->children[0].IsInteger())
      if (this->children[0].GetAtomicValue().GetValuE<Rational>()==(-1))
        return true;
  return
  false
  ;
}

bool Expression::NeedBracketsForFunctionArgument()const
{ return this->format!=formatNoBracketsForFunctionArgument;
}

bool Expression::NeedBracketsForThePower()const
{ return
  this->theOperation==this->theBoss->opPlus() ||
  this->theOperation==this->theBoss->opMinus() ||
  this->theOperation==this->theBoss->opTimes() ||
  this->theOperation==this->theBoss->opDivide()
  ;
}

void Expression::MakeInt(int theValue, CommandList& newBoss, int inputIndexBoundVars)
{ this->reset(newBoss, inputIndexBoundVars);
  this->theDatA=newBoss.theData.AddNoRepetitionOrReturnIndexFirst(Data(theValue, newBoss));
  this->theOperation=newBoss.opAtom();
}

void Expression::MakeAtom(const Rational& inputData, CommandList& newBoss, int inputIndexBoundVars)
{ this->MakeAtom(Data(inputData, newBoss), newBoss, inputIndexBoundVars);
}

void Expression::MakeAtom(const Data& inputData, CommandList& newBoss, int inputIndexBoundVars)
{ this->theBoss=&newBoss;
  this->reset(newBoss, inputIndexBoundVars);
  this->theDatA=newBoss.theData.AddNoRepetitionOrReturnIndexFirst(inputData);
  assert(this->theDatA<newBoss.theData.size);
  this->theOperation=newBoss.opAtom();
}

void Expression::MakeVariableNonBounD(CommandList& newBoss, int inputIndexBoundVars, const std::string& input)
{ VariableNonBound theVar;
  theVar.theName=input;
  this->MakeAtom(theVar, newBoss, inputIndexBoundVars);
}

void Expression::MakeAtom(const VariableNonBound& input, CommandList& newBoss, int inputIndexBoundVars)
{ Data tempData;
  tempData.MakeVariableNonBound(newBoss, input);
  this->MakeAtom(tempData, newBoss, inputIndexBoundVars);
}

bool CommandList::ReplaceIntIntBy10IntPlusInt()
{ SyntacticElement& left=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& right=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Data tempData(left.theData.GetAtomicValue());
  tempData*=10;
  tempData+=right.theData.GetAtomicValue();
  left.theData.MakeAtom(tempData, *this, this->theExpressionContext.size-1);
  this->DecreaseStackSetCharacterRanges(1);
  return true;
}

void Data::operator=(const Data& other)
{ if (this==&other)
    return;
  this->type=other.type;
  this->owner=other.owner;
  this->theIndex=other.theIndex;
  this->theContextIndex=other.theContextIndex;
  this->theError=other.theError;
}

bool CommandList::ReplaceXEXEXByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& lefT = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-4];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-2];
  SyntacticElement& result = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-5];
  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  newExpr.children.AddOnTop(lefT.theData);
  newExpr.children.AddOnTop(right.theData);
  newExpr.format=formatOptions;
  result.theData=newExpr;
  result.controlIndex=this->conExpression();
  this->DecreaseStackSetCharacterRanges(4);
  return true;
}

bool CommandList::ReplaceEEndCommandEbyE()
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  assert(left.theData.IndexBoundVars!=-1);
  if (left.theData.theOperation==this->opEndStatement())
    left.theData.children.AddOnTop(right.theData);
  else
  { Expression newExpr;
    newExpr.reset(*this, this->theExpressionContext.size-1);
    newExpr.theOperation=this->opEndStatement();
    newExpr.children.AddOnTop(left.theData);
    newExpr.children.AddOnTop(right.theData);
    newExpr.format=Expression::formatDefault;
    left.theData=newExpr;
  }
  this->DecreaseStackSetCharacterRanges(2);
//    std::cout << (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size()-1].theData.ElementToStringPolishForm();
  return true;
}

bool CommandList::ReplaceEXEByEusingO(int theControlIndex, int formatOptions)
{ SyntacticElement& left = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  SyntacticElement& right = (*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1];
  Expression newExpr;
  newExpr.reset(*this, this->theExpressionContext.size-1);
  newExpr.theOperation=this->GetOperationIndexFromControlIndex(theControlIndex);
  newExpr.children.AddOnTop(left.theData);
  newExpr.children.AddOnTop(right.theData);
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
  (*this->CurrentSyntacticStacK).LastObject()->theData.reset(*this, this->theExpressionContext.size-1);
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

void Context::operator=(const Context& other)
{ this->indexAmbientSSalgebra=other.indexAmbientSSalgebra;
  this->VariableImages=other.VariableImages;
}

bool Context::GetPolySubFromVariableSuperSet
(const Context& theSuperset, PolynomialSubstitution<Rational>& output)const
{ PolynomialSubstitution<Rational> theSub;
  output.SetSize(this->VariableImages.size);
  for (int i=0; i<this->VariableImages.size; i++)
  { int theNewIndex=theSuperset.VariableImages.GetIndex(this->VariableImages[i]);
    if (theNewIndex==-1)
      return false;
    output[i].MakeMonomial(theSuperset.VariableImages.size, theNewIndex ,1);
  }
  return true;
}

template <>
ElementUniversalEnveloping<RationalFunctionOld> Context::GetPolynomialMonomial
(int theIndex, GlobalVariables& theGlobalVariables)const
{ if (this->theOwner==0)
  { std::cout << "This is a programming error: the context has not been properly initialized"
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  ElementUniversalEnveloping<RationalFunctionOld> output;
  RationalFunctionOld theRF;
  theRF.MakeOneLetterMon(this->VariableImages.size, theIndex, 1, theGlobalVariables);
//  std::cout << "<br>this->indexAmbientSSalgebra=  " << this->indexAmbientSSalgebra;
  output.MakeConst(theRF, this->theOwner->theObjectContainer.theLieAlgebras, this->indexAmbientSSalgebra);
  return output;
}

template <>
Polynomial<Rational> Context::GetPolynomialMonomial
(int theIndex, GlobalVariables& theGlobalVariables)const
{ if (this->theOwner==0)
  { std::cout << "This is a programming error: the context has not been properly initialized"
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  Polynomial<Rational> output;
  output.MakeDegreeOne(this->VariableImages.size, theIndex, 1);
  return output;
}

template <>
RationalFunctionOld
Context::GetPolynomialMonomial
(int inputNonZeroIndex, GlobalVariables& theGlobalVariables)const
{ RationalFunctionOld output;
  output.MakeOneLetterMon(this->VariableImages.size, inputNonZeroIndex, 1, theGlobalVariables);
  return output;
}

bool CommandList::RegisterBoundVariable()
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-3];
  const std::string& theVarString=theElt.theData.GetAtomicValue().GetValuE<std::string>();
  if (!this->GetCurrentContextBoundVars().Contains(theVarString))
  { if (!this->theExpressionContext.LastObject()->flagOpDefineEncountered)
      this->GetCurrentContextBoundVars().AddOnTop(theVarString);
    else
    { theElt.controlIndex=this->conError();
      theElt.ErrorString=  "Error: bound variables cannot be declared after the definition operation := .";
    }
  } else
  { this->DecreaseStackSetCharacterRanges(2);
    this->ReplaceXXYByY();
  }
  return true;
}

bool CommandList::ReplaceVXdotsXbyEXdotsX(int numXs)
{ SyntacticElement& theElt=(*this->CurrentSyntacticStacK)[(*this->CurrentSyntacticStacK).size-1-numXs];
  const std::string& theVarString=theElt.theData.GetAtomicValue().GetValuE<std::string>();
  int indexBoundVar=this->GetCurrentContextBoundVars().GetIndex(theVarString);
  if (indexBoundVar!=- 1)
  { theElt.theData.theOperation=this->opVariableBound();
    theElt.theData.theDatA=indexBoundVar;
  } else
    theElt.theData.MakeVariableNonBounD(*this, -1, theVarString);
  theElt.theData.IndexBoundVars=this->theExpressionContext.size-1;
  theElt.controlIndex=this->conExpression();
  return true;
}

std::string Context::ToString()const
{ std::stringstream out;
  for (int i=0; i<this->VariableImages.size; i++)
    out << "<br>Variable " << i+1 << "=" << this->VariableImages[i].ToString();
  out << "<br>Ambient Lie algebra index: " << this->indexAmbientSSalgebra;
  return out.str();
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

void CommandList::AddNonBoundVarMustBeNew
  (const std::string& theName, const Function::FunctionAddress& funHandler,
   const std::string& argList, const std::string& description, const std::string& exampleArgs,
   bool visible)
{ VariableNonBound theV;
  Function theF(funHandler, theName, argList, description, exampleArgs, visible);
  theV.HandlerFunctionIndex= this->theFunctions.AddNoRepetitionOrReturnIndexFirst(theF);
  theV.theName=theName;
  this->theObjectContainer.theNonBoundVars.AddNoRepetition(theV);
}

void Data::MakeLittelmannRootOperator
(int inputIndex, CommandList& theBoss)
{ this->reset(theBoss);
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> theLRO;
  theLRO.Powers.AddOnTop(1);
  theLRO.generatorsIndices.AddOnTop(inputIndex);
  this->theIndex=
  theBoss.theObjectContainer.theLittelmannOperators.AddNoRepetitionOrReturnIndexFirst(theLRO);
  this->type=this->typeLittelmannRootOperator;
}

void Data::MakeLSpath
(CommandList& theBoss, SemisimpleLieAlgebra& owner, List<Vector<Rational> >& waypts)
{ this->reset(theBoss);
  Context tempContext;
  tempContext.indexAmbientSSalgebra= owner.indexInOwner;
  this->theContextIndex=
  theBoss.theObjectContainer.theContexts.AddNoRepetitionOrReturnIndexFirst(tempContext);
  this->type= this->typeLSpath;
  LittelmannPath thePath;
  thePath.owner=&owner.theWeyl;
  thePath.Waypoints=waypts;
  thePath.Simplify();
  this->theIndex= theBoss.theObjectContainer.theLSpaths.AddNoRepetitionOrReturnIndexFirst(thePath);
}


bool CommandList::fWriteGenVermaModAsDiffOperators
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ MacroRegisterFunctionWithName("CommandList::fWriteGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.SetSize(1);
  Context theContext;
  Selection theParSel;
  if (!theCommands.fGetTypeHighestWeightParabolic
      (theCommands, inputIndexBoundVars, theExpression, comments, theHWs[0], theParSel, &theContext))
    return theExpression.SetError("Failed to extract type, highest weight, parabolic selection");
  if (theExpression.errorString!="")
    return true;
  SemisimpleLieAlgebra& theSSalgebra=theExpression.children[0].GetAtomicValue().GetAmbientSSAlgebra();
  FormatExpressions theFormat;
  theContext.GetFormatExpressions(theFormat);
//  std::cout << "highest weights you are asking me for: " << theHws.ToString(&theFormat);
  return theCommands.fWriteGenVermaModAsDiffOperatorInner
  (theCommands, inputIndexBoundVars, theExpression, comments, theHWs, theContext, theParSel,
   theSSalgebra.indexInOwner);
}

bool CommandList::fFreudenthalEval
(CommandList& theCommands, int inputIndexBoundVars, Expression& theExpression, std::stringstream* comments)
{ Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  if (!theCommands.fGetTypeHighestWeightParabolic
      (theCommands, inputIndexBoundVars, theExpression, comments, hwFundamental, tempSel, 0))
    return theExpression.SetError("Failed to extract highest weight and algebra");
  if (theExpression.errorString!="")
    return true;
  if (tempSel.CardinalitySelection>0)
    return theExpression.SetError("Failed to extract highest weight. ");;
  SemisimpleLieAlgebra& theSSalg=theExpression.children[0]
  .GetAtomicValue().GetAmbientSSAlgebra();
  charSSAlgMod<Rational> startingChar, resultChar ;
  hwSimple=theSSalg.theWeyl.GetSimpleCoordinatesFromFundamental(hwFundamental);
  startingChar.MakeFromWeight
  (hwSimple, theCommands.theObjectContainer.theLieAlgebras, theSSalg.indexInOwner)
  ;
  std::string reportString;
  if (!startingChar.FreudenthalEvalMeDominantWeightsOnly
      (resultChar, 10000, &reportString, theCommands.theGlobalVariableS))
    return theExpression.SetError(reportString);
  Data theData;
  theData.MakeChar(theCommands, resultChar);
  theExpression.MakeAtom(theData, theCommands, inputIndexBoundVars);
  return true;
}
