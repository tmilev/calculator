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

extern IndicatorWindowVariables IndicatorWindowGlobalVariables;
class ElementWeylAlgebra
{
private:
  PolynomialRationalCoeff StandardOrder;
public:
  std::string DebugString;
  void ComputeDebugString(bool useBeginEqnArray, bool useXYs){this->ElementToString(this->DebugString, useXYs, true, useBeginEqnArray);};
  void ElementToString(std::string& output, ListBasicObjects<std::string>& alphabet, bool useLatex, bool useBeginEqnArray);
	void ElementToString(std::string& output, bool useXYs, bool useLatex, bool useBeginEqnArray);
  int NumVariables;
  void MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars);
  void MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void Makexixj(int i, int j, int NumVars);
  void Makexi(int i, int NumVars);
  void Makedi(int i, int NumVars);
  void Makedidj(int i, int j, int NumVars);
  void Makexidj(int i, int j, int NumVars);
  void Nullify(int NumVars);
  void SetNumVariablesPreserveExistingOnes(int newNumVars);
  void TimesConstant(Rational& theConstant)
  { this->StandardOrder.TimesConstant(theConstant);
  };
  void MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
  void MultiplyOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables);
  void LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
  void LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report);
  void LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report);
  void LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables);
  void Assign(const ElementWeylAlgebra& other)
  { this->StandardOrder.Assign(other.StandardOrder);
    this->NumVariables=other.NumVariables;
  };
  void Subtract(ElementWeylAlgebra& other)
  { this->StandardOrder.Subtract(other.StandardOrder);
  };
  void MakeConst (int NumVars, Rational& theConst);
  void Add(ElementWeylAlgebra& other)
  { this->StandardOrder.AddPolynomial(other.StandardOrder);
  };
  void MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput, GlobalVariables& theGlobalVariables);
  ElementWeylAlgebra()
  { this->NumVariables=0; };
  void operator=(const std::string& input);
  bool IsLetter(char theLetter);
  bool IsIndex(char theIndex);
  bool IsNumber(char theNumber);
};

class WeylParser;
class WeylParserNode
{
  public:
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(DebugString);};
  void ElementToString(std::string& output);
  WeylParser* owner;
  int indexParent;
  ListBasicObjects<int> children;
  ElementWeylAlgebra Value;
  int Operation; //using tokenTypes from class WeylParser
  void operator=(const WeylParserNode& other)
  { this->owner=other.owner;
    this->children.CopyFromBase(other.children);
    this->indexParent=other.indexParent;
  };
  void Evaluate(GlobalVariables& theGlobalVariables);
  WeylParserNode();
};

//the below class was written and implemented by an idea of helios from the forum of www.cplusplus.com
class WeylParser: public ListBasicObjects<WeylParserNode>
{
  public:
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(DebugString);};
  void ElementToString(std::string& output);
  enum tokenTypes
  { tokenExpression, tokenEmpty, tokenDigit, tokenPlus, tokenMinus, tokenUnderscore,  tokenTimes, tokenDivide, tokenPower, tokenOpenBracket, tokenCloseBracket,
    tokenOpenLieBracket, tokenCloseLieBracket, tokenX, tokenPartialDerivative, tokenComma, tokenLieBracket
  };
  ListBasicObjects<int> TokenBuffer;
  ListBasicObjects<int> ValueBuffer;
  ListBasicObjects<int> TokenStack;
  ListBasicObjects<int> ValueStack;
  int numEmptyTokensAtBeginning;
  int NumVariables;
  ElementWeylAlgebra Value;
  void ParserInit(const std::string& input);
  void Evaluate(GlobalVariables& theGlobalVariables);
  void Parse(const std::string& input);
  void ParseNoInit(int indexFrom, int indexTo);
  void ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables);
  bool ApplyRules(int lookAheadToken);
  bool lookAheadTokenProhibitsPlus(int theToken);
  bool lookAheadTokenProhibitsTimes(int theToken);
  void AddLetterOnTop(int index);
  void AddPartialOnTop(int index);
  void Own(int indexParent, int indexChild1, int indexChild2);
  void ExtendOnTop(int numNew);
  void TokenToStringStream(std::stringstream& out, int theToken);
  void AddPlusOnTop();
  void AddMinusOnTop();
  void AddTimesOnTop();
  void AddLieBracketOnTop();
  void DecreaseStackSetExpressionLastNode(int Decrease);
  void ComputeNumberOfVariablesAndAdjustNodes();
  WeylParser()
  { this->numEmptyTokensAtBeginning=5;
  };
};

bool ElementWeylAlgebra::IsLetter(char theLetter)
{
  return false;
}

void ElementWeylAlgebra::operator=(const std::string& input)
{ /*for (int i=0; i<input.size(); i++)
  { std::ostream formatter;
    if (input.at(i)=='x')

  }*/
}

void ElementWeylAlgebra::MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput, GlobalVariables& theGlobalVariables)
{ Monomial<Rational> buffer;
  SelectionWithDifferentMaxMultiplicities& tempSel=theGlobalVariables.selWeylAlgebra1;
  assert(left.NumVariables%2==0);
  int theDimension=left.NumVariables/2;
  tempSel.Multiplicities.initFillInObject(theDimension, 0);
  tempSel.MaxMultiplicities.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
    tempSel.MaxMultiplicities.TheObjects[i]=left.degrees[theDimension+i];
  tempSel.elements.initFillInObject(theDimension,0);
  buffer.init(left.NumVariables);
  assert(left.NumVariables==right.NumVariables);
  OrderedOutput.Nullify(left.NumVariables);
  int numCycles= tempSel.getTotalNumSubsets();
  Rational coeffProd;
  coeffProd.Assign(left.Coefficient);
  coeffProd.MultiplyBy(right.Coefficient);
  for (int i=0; i<numCycles; i++)
  { buffer.Coefficient.Assign(coeffProd);
    for (int k=0; k< theDimension; k++)
    { short multDrop=(short)tempSel.Multiplicities.TheObjects[k];
      buffer.Coefficient.MultiplyBy(Rational::NChooseK(left.degrees[theDimension+k], multDrop)*Rational::NChooseK(right.degrees[k], multDrop)* Rational::Factorial(multDrop));
      buffer.degrees[k]=left.degrees[k]+right.degrees[k]-multDrop;
      buffer.degrees[k+theDimension]= left.degrees[k+theDimension]+right.degrees[k+theDimension]-multDrop;
    }
    OrderedOutput.AddMonomial(buffer);
    tempSel.IncrementSubset();
  }
}

void ElementWeylAlgebra::LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report)
{ std::stringstream out;
  this->ComputeDebugString(false, false);
  standsOnTheLeft.ComputeDebugString(false, false);
  out << "\\begin{eqnarray*}&&["<< standsOnTheLeft.DebugString<<" , "<< this->DebugString<< " ]= ";
  this->LieBracketOnTheLeft(standsOnTheLeft, theGlobalVariables);
  this->ComputeDebugString(false, false);
  out << this->DebugString<<"\\end{eqnarray*}\n";
  report=out.str();
}

void ElementWeylAlgebra::LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report)
{ std::stringstream out;
  this->ComputeDebugString(false, false);
  standsOnTheRight.ComputeDebugString(false, false);
  out << "\\begin{eqnarray*}&&["<< this->DebugString<< " , "<<standsOnTheRight.DebugString<<" ]= ";
  this->LieBracketOnTheRight(standsOnTheRight, theGlobalVariables);
  this->ComputeDebugString(false, false);
  out << this->DebugString<<"\\end{eqnarray*}\n";
  report=out.str();
}

void ElementWeylAlgebra::LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1.Assign(*this);
  tempEl1.MultiplyOnTheLeft(standsOnTheLeft, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2.Assign(standsOnTheLeft);
  tempEl2.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  this->Assign(tempEl1);
  this->Subtract(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MakeConst(int NumVars, Rational& theConst)
{ this->Nullify(NumVariables);
  Monomial<Rational> tempM;
  tempM.init((short)this->NumVariables*2);
  tempM.Coefficient.Assign(theConst);
  this->StandardOrder.AddMonomial(tempM);
}

void ElementWeylAlgebra::LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1.Assign(standsOnTheRight);
  tempEl1.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2.Assign(*this);
  tempEl2.MultiplyOnTheLeft(standsOnTheRight, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  this->Assign(tempEl1);
  this->Subtract(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff buffer;
  PolynomialRationalCoeff Accum;
  Accum.Nullify((short) this->NumVariables*2);
  for (int j=0; j<standsOnTheLeft.StandardOrder.size; j++)
    for (int i=0; i<this->StandardOrder.size; i++)
    { this->MultiplyTwoMonomials(standsOnTheLeft.StandardOrder.TheObjects[j], this->StandardOrder.TheObjects[i], buffer, theGlobalVariables);
      Accum.AddPolynomial(buffer);
    }
  this->StandardOrder.Assign(Accum);
}

void ElementWeylAlgebra::MultiplyOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff buffer;
  PolynomialRationalCoeff Accum;
  Accum.Nullify((short) this->NumVariables*2);
  for (int j=0; j<standsOnTheRight.StandardOrder.size; j++)
    for (int i=0; i<this->StandardOrder.size; i++)
    { this->MultiplyTwoMonomials(this->StandardOrder.TheObjects[i], standsOnTheRight.StandardOrder.TheObjects[j], buffer, theGlobalVariables);
      Accum.AddPolynomial(buffer);
    }
  this->StandardOrder.Assign(Accum);
}

void ElementWeylAlgebra::ElementToString(std::string& output, ListBasicObjects<std::string>& alphabet, bool useLatex, bool useBeginEqnArray)
{ std::stringstream out;
  std::string tempS;
  int numLettersSinceLastNewLine=0;
  if (useLatex && useBeginEqnArray)
    out << "\\begin{eqnarray*}&&";
  for (int i=0; i<this->StandardOrder.size; i++)
  { this->StandardOrder.TheObjects[i].Coefficient.ElementToString(tempS);
    bool hasMinus=(tempS[0]=='-');
    if (this->StandardOrder.TheObjects[i].TotalDegree()!=0)
    { if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
    }
    if (!hasMinus && i!=0)
      out<<'+';
    out << tempS;
    for (int k=0; k<this->NumVariables; k++)
      if (this->StandardOrder.TheObjects[i].degrees[k]!=0)
      { out <<"{"<<alphabet.TheObjects[k]<<"}";
        int tempI=(int) this->StandardOrder.TheObjects[i].degrees[k];
        if (tempI!=1)
          out <<"^{"<< tempI<<"}";
      }
    for (int k=0; k<this->NumVariables; k++)
      if (this->StandardOrder.TheObjects[i].degrees[this->NumVariables+ k]!=0)
      { out <<"\\partial_{"<<alphabet.TheObjects[k]<<"}";
				numLettersSinceLastNewLine++;
        int tempI=this->StandardOrder.TheObjects[i].degrees[this->NumVariables+k];
        if (tempI!=1)
          out <<"^{" <<tempI<<"}";
      }
    if (numLettersSinceLastNewLine>12 && i!= this->StandardOrder.size-1 && useLatex)
    { numLettersSinceLastNewLine=0;
      out <<"\\\\&&\n";
    }
  }
  if (useLatex && useBeginEqnArray)
    out << "\\end{eqnarray*}";
  output=out.str();
}

void ElementWeylAlgebra::ElementToString(std::string& output, bool useXYs, bool useLatex, bool useBeginEqnArray)
{ ListBasicObjects<std::string> alphabet;
  alphabet.SetSizeExpandOnTopNoObjectInit(this->NumVariables);
  int numCycles=this->NumVariables;
  if (useXYs)
  { assert(this->NumVariables%2==0);
    numCycles= this->NumVariables/2;
  }
  for (int i=0; i<numCycles; i++)
  { std::stringstream out, out2;
    out << "x_"<<i+1;
    alphabet.TheObjects[i]= out.str();
    if (useXYs)
    { out2<< "y_"<< i+1;
      alphabet.TheObjects[i+numCycles]= out2.str();
    }
  }
  this->ElementToString(output, alphabet, useLatex, useBeginEqnArray);
}

void ElementWeylAlgebra::SetNumVariablesPreserveExistingOnes(int newNumVars)
{ if (newNumVars<this->NumVariables)
    this->NumVariables=newNumVars;
  PolynomialRationalCoeff Accum;
  Accum.Nullify((short)newNumVars*2);
  Accum.MakeActualSizeAtLeastExpandOnTop(this->StandardOrder.size);
  Monomial<Rational> tempM;
  for (int i=0; i<this->StandardOrder.size; i++)
  { tempM.init((short)newNumVars*2);
    for (int j=0; j< this->NumVariables; j++)
    { tempM.degrees[j]=this->StandardOrder.TheObjects[i].degrees[j];
      tempM.degrees[j+newNumVars]=this->StandardOrder.TheObjects[i].degrees[j+this->NumVariables];
    }
    tempM.Coefficient.Assign(this->StandardOrder.TheObjects[i].Coefficient);
    Accum.AddMonomial(tempM);
  }
  this->NumVariables= newNumVars;
  this->StandardOrder.Assign(Accum);
}

void ElementWeylAlgebra::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars)
{ this->Nullify(NumVars*2);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]=1;
  tempMon.degrees[j+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon);

  tempMon.Coefficient.MakeMOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[j]=1;
  tempMon.degrees[i+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->Nullify(NumVars*2);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]=1;
  tempMon.degrees[j+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon);

  tempMon.Coefficient.MakeMOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[j+NumVars]=1;
  tempMon.degrees[i+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->Nullify(NumVars*2);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i+NumVars]=1;
  tempMon.degrees[j+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon);

  tempMon.Coefficient.MakeMOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[j+NumVars]=1;
  tempMon.degrees[i+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makedidj(int i, int j, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i+NumVars]++;
  tempMon.degrees[j+NumVars]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makexixj(int i, int j, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]++;
  tempMon.degrees[j]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makexi(int i, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makedi(int i, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i+NumVars]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makexidj(int i, int j, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]=1;
  tempMon.degrees[NumVars+j]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Nullify(int NumVars)
{ this->NumVariables=NumVars;
  this->StandardOrder.Nullify ((short)this->NumVariables*2);
}

int DebugCounter=-1;
WeylParser debugParser;

void WeylParser::ParserInit(const std::string& input)
{ this->TokenStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->MakeActualSizeAtLeastExpandOnTop(input.size());
  this->TokenStack.size=0;
  this->ValueStack.size=0;
  this->size=0;
  for (int i=0; i<(signed) input.size(); i++)
  { switch (input.at(i))
    { case '0': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(0); break;
      case '1': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(1); break;
      case '2': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(2); break;
      case '3': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(3); break;
      case '4': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(4); break;
      case '5': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(5); break;
      case '6': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(6); break;
      case '7': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(7); break;
      case '8': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(8); break;
      case '9': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(9); break;
      case '[': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenOpenLieBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case ']': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenCloseLieBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case '(': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenOpenBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case ',': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenComma); this->ValueBuffer.AddObjectOnTop(0); break;
      case ')': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenCloseBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case '^': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenPower); this->ValueBuffer.AddObjectOnTop(0); break;
      case '+': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenPlus); this->ValueBuffer.AddObjectOnTop(0); break;
      case '-': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenMinus); this->ValueBuffer.AddObjectOnTop(0); break;
      case '_': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenUnderscore); this->ValueBuffer.AddObjectOnTop(0); break;
      case '/': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDivide); this->ValueBuffer.AddObjectOnTop(0); break;
      case 'x': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenX); this->ValueBuffer.AddObjectOnTop(0); break;
      case 'd': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenPartialDerivative); this->ValueBuffer.AddObjectOnTop(0); break;
    }
  }
  this->ValueStack.size=0;
  this->TokenStack.size=0;
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(this->ValueBuffer.size);
  this->TokenStack.MakeActualSizeAtLeastExpandOnTop(this->TokenBuffer.size);
  for (int i=0; i<this->numEmptyTokensAtBeginning; i++)
  { this->TokenStack.AddObjectOnTop(this->tokenEmpty);
    this->ValueStack.AddObjectOnTop(0);
  }
}

void WeylParser::Parse(const std::string& input)
{ this->ParserInit(input);
  this->ParseNoInit(0, this->TokenBuffer.size-1);
  this->ComputeNumberOfVariablesAndAdjustNodes();
}

void WeylParser::ParseNoInit(int indexFrom, int indexTo)
{ if (indexFrom<0 || indexTo>= this->TokenBuffer.size)
    return;
  for (int i=indexFrom; i<=indexTo; i++)
  { this->ValueStack.AddObjectOnTop(this->ValueBuffer.TheObjects[i]);
    this->TokenStack.AddObjectOnTop(this->TokenBuffer.TheObjects[i]);
    int lookAheadToken=this->tokenEmpty;
    if (i<this->TokenBuffer.size-1)
      lookAheadToken=this->TokenBuffer.TheObjects[i+1];
    while(this->ApplyRules(lookAheadToken))
    {
    }
  }
}

bool WeylParser::lookAheadTokenProhibitsPlus(int theToken)
{ if (theToken==this->tokenPlus)
    return false;
  if (theToken==this->tokenMinus)
    return false;
  if (theToken==this->tokenCloseBracket)
    return false;
  if (theToken==this->tokenCloseLieBracket)
    return false;
  return true;
}

bool WeylParser::lookAheadTokenProhibitsTimes(int theToken)
{ if (theToken==this->tokenPower)
    return true;
  return false;
}

bool WeylParser::ApplyRules(int lookAheadToken)
{ if (this->TokenStack.size<2)
    return false;
  int IndexLast=this->TokenStack.size-1;
  if (this->TokenStack.TheObjects[IndexLast]==this->tokenEmpty)
  { this->TokenStack.PopLastObject();
    this->ValueStack.PopLastObject();
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast]== this->tokenCloseLieBracket && this->TokenStack.TheObjects[IndexLast-1]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-2]==this->tokenComma && this->TokenStack.TheObjects[IndexLast-3]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-4]==this->tokenOpenLieBracket)
  { this->AddLieBracketOnTop();
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast-1]==this->tokenUnderscore && this->TokenStack.TheObjects[IndexLast]==this->tokenDigit && this->TokenStack.TheObjects[IndexLast-2]==this->tokenPartialDerivative)
  { this->AddPartialOnTop(this->ValueStack.TheObjects[IndexLast]);
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast-1]==this->tokenUnderscore && this->TokenStack.TheObjects[IndexLast]==this->tokenDigit && this->TokenStack.TheObjects[IndexLast-2]==this->tokenX)
  { this->AddLetterOnTop(this->ValueStack.TheObjects[IndexLast]);
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast]==this->tokenExpression && this->TokenStack.TheObjects[IndexLast-1]==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->AddTimesOnTop();
    return true;
  }
  if( this->TokenStack.TheObjects[IndexLast-1]==this->tokenPlus && this->TokenStack.TheObjects[IndexLast]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-2]==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddPlusOnTop();
    return true;
  }
  if( this->TokenStack.TheObjects[IndexLast-1]==this->tokenMinus && this->TokenStack.TheObjects[IndexLast]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-2]==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddMinusOnTop();
    return true;
  }
  return false;
}

void WeylParser::DecreaseStackSetExpressionLastNode(int Decrease)
{ this->TokenStack.size-=Decrease;
  this->ValueStack.size-=Decrease;
  *this->TokenStack.LastObject()=this->tokenExpression;
  *this->ValueStack.LastObject()=this->size-1;
}

void WeylParser::AddLetterOnTop(int index)
{ if (index<=0)
    return;
  this->ExtendOnTop(1);
  this->LastObject()->Value.Makexi(index-1, index);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddPartialOnTop(int index)
{ if (index<=0)
    return;
  this->ExtendOnTop(1);
  this->LastObject()->Value.Makedi(index-1, index);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddPlusOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenPlus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddMinusOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMinus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddTimesOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenTimes;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-2], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void WeylParser::AddLieBracketOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenLieBracket;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-4], this->ValueStack.TheObjects[this->ValueStack.size-2]);
  this->DecreaseStackSetExpressionLastNode(4);
}

void WeylParser::ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  this->Parse(input);
  out <<"\\begin{eqnarray*}&&" << input<<" = \\\\\n";
  this->Evaluate(theGlobalVariables);
  this->Value.ElementToString(tempS, false, true, false);
  out <<tempS;
  out <<"\\end{eqnarray*}";
  output=out.str();
}

void WeylParser::Own(int indexParent, int indexChild1, int indexChild2)
{ WeylParserNode* theNode= & this->TheObjects[indexParent];
  theNode->children.SetSizeExpandOnTopNoObjectInit(2);
  theNode->children.TheObjects[0]=indexChild1;
  theNode->children.TheObjects[1]=indexChild2;
  this->TheObjects[indexChild1].indexParent= indexParent;
  this->TheObjects[indexChild2].indexParent= indexParent;
}

void WeylParser::Evaluate(GlobalVariables& theGlobalVariables)
{ if (this->TokenStack.size== this->numEmptyTokensAtBeginning+1)
    if (*this->TokenStack.LastObject()==this->tokenExpression)
    { WeylParserNode* theNode=&this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]];
      theNode->Evaluate(theGlobalVariables);
      this->Value.Assign(theNode->Value);
    }
}

void WeylParser::ExtendOnTop(int numNew)
{ this->SetSizeExpandOnTopNoObjectInit(this->size+numNew);
  for (int i=0; i<numNew; i++)
    this->TheObjects[this->size-1-i].owner=this;
}

void WeylParserNode::Evaluate(GlobalVariables& theGlobalVariables)
{ Rational tempRat;
  switch (this->Operation)
  { case WeylParser::tokenPlus: this->Value.Nullify(owner->NumVariables);
      for (int i=0; i<this->children.size; i++)
      { this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
        this->Value.Add(this->owner->TheObjects[this->children.TheObjects[i]].Value);
      }
      break;
    case WeylParser::tokenTimes: tempRat.MakeOne();
      this->Value.MakeConst(owner->NumVariables, tempRat);
      for (int i=0; i<this->children.size; i++)
      { this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
        this->Value.MultiplyOnTheRight(this->owner->TheObjects[this->children.TheObjects[i]].Value, theGlobalVariables);
      }
      break;
    case WeylParser::tokenLieBracket:
      this->owner->TheObjects[children.TheObjects[0]].Evaluate(theGlobalVariables);
      this->owner->TheObjects[children.TheObjects[1]].Evaluate(theGlobalVariables);
      this->Value.Assign(owner->TheObjects[children.TheObjects[0]].Value);
      this->Value.LieBracketOnTheRight(owner->TheObjects[children.TheObjects[1]].Value, theGlobalVariables);
      break;
    }

}

WeylParserNode::WeylParserNode()
{ this->Operation= WeylParser::tokenEmpty;
}


void WeylParser::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  out <<"\nToken stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->TokenStack.size; i++)
  { this->TokenToStringStream(out, this->TokenStack.TheObjects[i]);
    out <<", ";
  }
  out <<"\nValue stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->ValueStack.size; i++)
    out <<this->ValueStack.TheObjects[i]<<", ";
  out <<"\nElements: ";
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS);
    out << " Index: "<< i<<" " <<tempS<<"; ";
  }
  output=out.str();
}

void WeylParser::TokenToStringStream(std::stringstream& out, int theToken)
{ switch(theToken)
  { case WeylParser::tokenX: out <<"x"; break;
    case WeylParser::tokenDigit: out <<"D"; break;
    case WeylParser::tokenPlus: out <<"+"; break;
    case WeylParser::tokenUnderscore: out <<"_"; break;
    case WeylParser::tokenEmpty: out <<" "; break;
    case WeylParser::tokenExpression: out<<"E"; break;
    case WeylParser::tokenOpenLieBracket: out <<"["; break;
    case WeylParser::tokenCloseLieBracket: out <<"]"; break;
    case WeylParser::tokenLieBracket: out <<"[,]"; break;
    case WeylParser::tokenComma: out <<","; break;
    case WeylParser::tokenPartialDerivative: out <<"d"; break;
    case WeylParser::tokenTimes: out << "*"; break;
    case WeylParser::tokenMinus: out<< "-"; break;
    default: out <<"?"; break;
  }
}

void WeylParser::ComputeNumberOfVariablesAndAdjustNodes()
{ this->NumVariables=0;
  for(int i=0; i<this->size; i++)
    if (this->NumVariables< this->TheObjects[i].Value.NumVariables)
      this->NumVariables=this->TheObjects[i].Value.NumVariables;
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Value.SetNumVariablesPreserveExistingOnes(this->NumVariables);
}

void WeylParserNode::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  owner->TokenToStringStream(out, this->Operation);
  out <<" Value: ";
  this->Value.ElementToString(tempS, false, false, false);
  out << " " <<tempS<<" Children: ";
  for (int i=0; i<this->children.size; i++)
    out << this->children.TheObjects[i]<<", ";
  out <<"\n";
  output=out.str();
}


class IrreducibleFiniteDimensionalModule
{
  public:
  DyckPaths thePaths;//the Weyl group inside the Dyck paths is assumed to parametrize the module.
  MatrixLargeRational thePathMatrix;
  MatrixLargeRational theMatrix;
  roots theColumnsOfTheMatrix;
  void InitAndPrepareTheChambersForComputation(int IndexWeyl, CombinatorialChamberContainer& theChambers, GlobalVariables& theGlobalVariables);
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(this->DebugString);};
  void ElementToString(std::string& output);
};

void IrreducibleFiniteDimensionalModule::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  this->theMatrix.ElementToSting(tempS);
  out <<"The matrix :\n "<< tempS<<"\n\n";
  this->thePathMatrix.ElementToSting(tempS);
  out <<"The matrix of the polytope:\n "<< tempS<<"\n\n";
  this->thePaths.ElementToString(tempS, false);
  out << tempS;
  output = out.str();
}

void IrreducibleFiniteDimensionalModule::InitAndPrepareTheChambersForComputation(int IndexWeyl, CombinatorialChamberContainer& theChambers, GlobalVariables& theGlobalVariables)
{// reference:  E. Feigin, G. Fourier, P. Littelmann, "PBW filtration and bases for irreducible modules in type A_n", preprint 2009
  int theDimension= IndexWeyl;
  this->thePaths.AmbientWeyl.MakeAn(theDimension);
  this->thePaths.GenerateAllDyckPathsTypesABC();
  this->thePaths.ComputeGoodPathsExcludeTrivialPath();
  this->thePathMatrix.init( this->thePaths.GoodPaths.size, this->thePaths.PositiveRoots.size);
  this->thePathMatrix.NullifyAll(0);
  for (int i=0; i<this->thePaths.GoodPaths.size; i++)
    for (int j=0; j<this->thePaths.TheObjects[this->thePaths.GoodPaths.TheObjects[i]].thePathNodes.size; j++)
      this->thePathMatrix.elements[i][this->thePaths.TheObjects[this->thePaths.GoodPaths.TheObjects[i]].thePathNodes.TheObjects[j]]=1;
  this->theMatrix.Assign(this->thePathMatrix);
  this->theMatrix.Resize(this->thePaths.GoodPaths.size + theDimension, this->thePaths.PositiveRoots.size+this->thePaths.GoodPaths.size, true);
  for (int j=0; j<theDimension; j++)
  { for (int i=0; i<this->thePaths.PositiveRoots.size; i++)
      this->theMatrix.elements[j+this->thePaths.GoodPaths.size][i]= this->thePaths.PositiveRoots.TheObjects[i].TheObjects[j];
    for (int i=0; i<this->thePaths.GoodPaths.size; i++)
      this->theMatrix.elements[j+this->thePaths.GoodPaths.size][i+this->thePaths.PositiveRoots.size]=0;
  }
  for (int i=0; i<this->thePaths.GoodPaths.size; i++)
    for (int j=0; j<this->thePaths.GoodPaths.size; j++)
      this->theMatrix.elements[i][j+this->thePaths.PositiveRoots.size]=(i==j)? 1 : 0;
  this->theColumnsOfTheMatrix.AssignMatrixColumns(this->theMatrix);
  theChambers.theDirections.CopyFromBase(this->theColumnsOfTheMatrix);
  theChambers.AmbientDimension= this->theMatrix.NumRows;
  theChambers.CurrentIndex=theChambers.AmbientDimension-1;
  theChambers.theDirections.ComputeDebugString();

}

void main_test_function(std::string& output, GlobalVariables& theGlobalVariables, ComputationSetup& theSetup, bool flagComputationAlreadyLoaded)
{ std::stringstream out;
  std::string tempS;
  ElementWeylAlgebra theElement, tempEl1, tempEl2, tempEl3, tempEl4, tempEl5, Accum;
  //out <<"\\documentclass{article}\\usepackage{latexsym}\\usepackage{amssymb}\\usepackage{amsmath}\n";
  //out << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}";
 // out <<"\\begin{document}";
/*  WeylParser theParser;
  theParser.ParseAndCompute("x_1+x_1", tempS);*/
  /*
  if (DebugCounter==-1)
    debugParser.ParserInit("[x_1x_2, x_3d_2]");
  else
    debugParser.ParseNoInit(DebugCounter, DebugCounter);
  DebugCounter++;
  debugParser.ComputeDebugString();
  out << debugParser.DebugString;
  out << "\n";
  if (DebugCounter> debugParser.TokenBuffer.size)
  { debugParser.ComputeNumberOfVariablesAndAdjustNodes();
    debugParser.Evaluate(theGlobalVariables);
  }
  WeylParser theParser;
  theParser.ParseAndCompute("[x_1x_2, x_3d_2]", tempS, theGlobalVariables);
  debugParser.Value.ComputeDebugString(false,false);
  out << debugParser.Value.DebugString;
  out << "\n\n"<<tempS;*/
  SimpleLieAlgebra theG;

  theG.FindSl2Subalgebras('D', 6, theGlobalVariables);
  IndicatorWindowGlobalVariables.StatusString1= theG.DebugString;
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
  return;
  IrreducibleFiniteDimensionalModule theModule;
  QuasiPolynomial tempP;
  if (!flagComputationAlreadyLoaded)
    theModule.InitAndPrepareTheChambersForComputation(3, theSetup.theChambers,theGlobalVariables);
  theSetup.theChambers.flagMustStop=false;
  theSetup.theChambers.flagIsRunning=true;
  theSetup.theChambers.flagReachSafePointASAP=false;
  theSetup.flagExperiment2ChambersAlreadyLoaded=true;
  theSetup.theChambers.SliceTheEuclideanSpace(theGlobalVariables);
/*  tempEl1.Makexixj(0,1,4);
  Rational tempRat=-1;
  tempEl2.Makedidj(0,1,4);
  tempEl2.TimesConstant(tempRat);
  theElement.Assign(tempEl2);
  theElement.LieBracketOnTheLeftMakeReport(tempEl1, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl1, theGlobalVariables, tempS);
  out << tempS;
  theElement.Assign(tempEl2);
  theElement.LieBracketOnTheLeftMakeReport(tempEl1, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl2, theGlobalVariables, tempS);
  out << tempS;
  tempEl3.Makexixj(0,0,4);
  tempEl4.Makedidj(0,0,4);
  tempEl4.TimesConstant(tempRat);
  theElement.Assign(tempEl4);
  theElement.LieBracketOnTheLeftMakeReport(tempEl3, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl4, theGlobalVariables, tempS);
  out <<tempS;
  theElement.Assign(tempEl4);
  theElement.LieBracketOnTheLeftMakeReport(tempEl3, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl3, theGlobalVariables, tempS);
  out <<tempS;
  tempEl3.Makexidj(0,1,4);
//  tempEl3.TimesConstant(tempRat);
  tempEl4.Makexidj(1,0,4);
  theElement.Assign(tempEl3);
  theElement.LieBracketOnTheRightMakeReport(tempEl4, theGlobalVariables, tempS);
  out << tempS;*/
  /*tempEl1.MakeGEpsPlusEpsInTypeD(0,1, 4);
  tempEl2.MakeGEpsMinusEpsInTypeD(0,1,4);
  tempEl3.MakeGEpsPlusEpsInTypeD(2,3, 4);
  tempEl4.MakeGEpsMinusEpsInTypeD(2,3,4);
  tempEl5.MakeGMinusEpsMinusEpsInTypeD(0,2,4);
  tempEl1.ComputeDebugString(false);
  tempEl2.ComputeDebugString(false);
  tempEl3.ComputeDebugString(false);
  tempEl4.ComputeDebugString(false);
  tempEl5.ComputeDebugString(false);



  ElementWeylAlgebra theH, tempEl6;
  theH.MakeGEpsPlusEpsInTypeD(0,2,4);
  tempEl6.MakeGMinusEpsMinusEpsInTypeD(0,2,4);
  theH.LieBracketOnTheLeft(tempEl6, theGlobalVariables);
  theH.ComputeDebugString(false);
  out<<"\n\\begin{eqnarray*}&&h=";
  out <<theH.DebugString;
  out <<"\n\\end{eqnarray*}\n";
  tempEl6.Assign(theH);
  tempEl6.LieBracketOnTheLeft(theH, theGlobalVariables);
  tempEl6.ComputeDebugString(false);
  out<<"\n\\begin{eqnarray*}&&[h,h]=";
  out <<tempEl6.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  out<<"\n\\begin{eqnarray*}&&-[h,g^{-\\varepsilon_1-\\varepsilon_3}]=\n\\\\";
  out<<"&&-["<<theH.DebugString<<","<<tempEl5.DebugString <<"]=\n\\\\";
  tempEl6.Assign(theH);
  tempEl6.LieBracketOnTheLeft(tempEl5, theGlobalVariables);
  tempEl6.ComputeDebugString(false);
  out <<tempEl6.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  out<<"\n\\begin{eqnarray*}\n&&(g^{-\\varepsilon_1-\\varepsilon_3})^2 g ^{\\varepsilon_1+\\varepsilon_2}g ^{\\varepsilon_1-\\varepsilon_2}g ^{\\varepsilon_3+\\varepsilon_4}g ^{\\varepsilon_3-\\varepsilon_4}\\\\\n ";
  out <<"&&=("<<tempEl5.DebugString<<")^2("<<tempEl1.DebugString<< ")("<<tempEl2.DebugString <<")("
      << tempEl3.DebugString <<")("<< tempEl4.DebugString<<")=\\\\";
  Accum.Assign(tempEl4);
  Accum.MultiplyOnTheLeft(tempEl3, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out <<"&&("<<tempEl5.DebugString<<")^2("<<tempEl1.DebugString<< ")("<<tempEl2.DebugString <<")("
      << Accum.DebugString<<")=";
  out <<"\\end{eqnarray*}\n";
  out<<"\n\\begin{eqnarray*}&&\n";
  Accum.MultiplyOnTheLeft(tempEl2, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out <<"("<<tempEl5.DebugString<<")^2("<<tempEl1.DebugString<< ")("<< Accum.DebugString<<")=\\\\\n";
  out <<"\\end{eqnarray*}\n";
  out<<"\n\\begin{eqnarray*}&&\n";
  Accum.MultiplyOnTheLeft(tempEl1, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out <<"("<<tempEl5.DebugString<<")^2("<< Accum.DebugString<<")=\\\\\n";
  out <<"\\end{eqnarray*}\n";
  Accum.MultiplyOnTheLeft(tempEl5, theGlobalVariables);
  Accum.MultiplyOnTheLeft(tempEl5, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out<<"\n\\begin{eqnarray*}&&\n";
  out <<Accum.DebugString;
  out <<"\n\\end{eqnarray*}\n";



  out<<"\n\\begin{eqnarray*}&&[h,\\bullet]=";
  theH.LieBracketOnTheLeft(Accum, theGlobalVariables);
  theH.ComputeDebugString(false);
  out <<theH.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  tempEl2.MakeGEpsMinusEpsInTypeD(0,1,4);
  tempEl2.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\ng^{\\varepsilon_1-\\varepsilon_2}= ";
  out <<tempEl2.DebugString;
  out <<"\n\\end{eqnarray*}\n";
  tempEl3.MakeGMinusEpsMinusEpsInTypeD(0,1,4);
  tempEl3.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\ng^{-\\varepsilon_1-\\varepsilon_2}= ";
  out <<tempEl3.DebugString;
  out <<"\n\\end{eqnarray*}\n";
  tempEl4.Assign(tempEl3);
  tempEl4.MultiplyOnTheLeft(tempEl1,theGlobalVariables);
  tempEl4.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\ng^{\\varepsilon_1+\\varepsilon_2}g^{-\\varepsilon_1-\\varepsilon_2}= ";
  out <<tempEl4.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  tempEl4.Assign(tempEl3);
  tempEl4.LieBracketOnTheLeft(tempEl1,theGlobalVariables);
  tempEl4.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\n[g^{\\varepsilon_1+\\varepsilon_2},g^{-\\varepsilon_1-\\varepsilon_2}]= ";
  out <<tempEl4.DebugString;

  out <<"\n\\end{eqnarray*}\n";
*/

//  out<<"\\end{document}";
  output=out.str();
  IndicatorWindowGlobalVariables.StatusString1=output;
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  if(theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
    theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void DyckPaths::ComputeGoodPathsExcludeTrivialPath()
{ int counter=0;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IamComplete(*this))
      counter++;
  this->GoodPaths.MakeActualSizeAtLeastExpandOnTop(counter);
  this->GoodPaths.size=0;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IamComplete(*this))
      this->GoodPaths.AddObjectOnTop(i);
}


void CombinatorialChamber::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ output << "Flags_and_indices: ";
  output << this->flagHasZeroPolynomial << " "<< this->flagExplored<< " " <<this->flagPermanentlyZero <<" ";
  output << this->IndexInOwnerComplex << " "<< this->IndexStartingCrossSectionNormal <<" "<< this->DisplayNumber <<"\nVertices:\n";
  this->AllVertices.WriteToFile(output, theGlobalVariables);
  output <<"Internal_point: ";
  this->InternalPoint.WriteToFile(output);
  output << "\nInternalWalls:\n";
  this->InternalWalls.WriteToFile(output, theGlobalVariables);
  output<<"\n" << this->Externalwalls.size<<"\n";
  for (int i=0; i<this->Externalwalls.size; i++)
  { this->Externalwalls.TheObjects[i].WriteToFile(output);
    output<<" ";
  }
}

void CombinatorialChamber::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables, CombinatorialChamberContainer& owner)
{ std::string tempS;
  int tempI;
  input >> tempS;
  input >> this->flagHasZeroPolynomial >> this->flagExplored>>this->flagPermanentlyZero;
  input >> this->IndexInOwnerComplex >> this->IndexStartingCrossSectionNormal;
  input >> this->DisplayNumber;
  input >> tempS;
  this->AllVertices.ReadFromFile(input, theGlobalVariables);
  input>>tempS;
  this->InternalPoint.ReadFromFile(input);
  input >> tempS;
  assert(tempS =="InternalWalls:");
  this->InternalWalls.ReadFromFile(input, theGlobalVariables);
  input >> tempI;
  this->Externalwalls.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->Externalwalls.size; i++)
    this->Externalwalls.TheObjects[i].ReadFromFile(input, owner);
}

void CombinatorialChamberContainer::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->LabelChamberIndicesProperly();
  output <<"Num_pointers: "<< this->size<<"\n";
///////////////////////////////////////////////////
  output<<"Dimension: "<< this->AmbientDimension<<" ";
  output << "CurrentIndex: "<< this->CurrentIndex<<"\n";
  output << "Directions:\n";
  this->theDirections.WriteToFile(output, theGlobalVariables);
  output <<"\nNext_index_to_slice: "<< this->indexNextChamberToSlice<<"\n";
  output <<"FirstNonExploredIndex: " << this->FirstNonExploredIndex<<" ";
  this->TheGlobalConeNormals.WriteToFile(output, theGlobalVariables);
  output<<"\n";
  this->startingCones.WriteToFile(output, theGlobalVariables);
////////////////////////////////////////////////////
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { output <<"\nChamber:\n";
      this->TheObjects[i]->WriteToFile(output, theGlobalVariables);
    } else
      output <<"Empty\n";
}

void CombinatorialChamberContainer::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  input.seekg(0);
  this->KillAllElements();
  int tempI;
  input >> tempS >> tempI;
  this->initAndCreateNewObjects(tempI);
///////////////////////////////////////////////////
  input >> tempS >> this->AmbientDimension;
  input >> tempS >> this->CurrentIndex;
  input >> tempS;
  this->theDirections.ReadFromFile(input, theGlobalVariables);
  input >> tempS >> this->indexNextChamberToSlice;
  input >> tempS >> this->FirstNonExploredIndex;
  this->TheGlobalConeNormals.ReadFromFile(input, theGlobalVariables);
  this->startingCones.ReadFromFile(input, theGlobalVariables);
////////////////////////////////////////////////////
  for (int i=0; i<this->size; i++)
  { input >> tempS;
    if (tempS=="Chamber:")
      this->TheObjects[i]->ReadFromFile(input, theGlobalVariables, *this);
    else
    { assert (tempS=="Empty");
      delete this->TheObjects[i];
      this->TheObjects[i]=0;
    }
  }
  this->flagIsRunning=true;
  this->flagMustStop=false;
  this->flagReachSafePointASAP=false;
}

void WallData::WriteToFile(std::fstream& output)
{ //output << this->indexInOwnerChamber <<" ";
  this->normal.WriteToFile(output);
  output<< " "<< this->NeighborsAlongWall.size<<" ";
  assert(this->NeighborsAlongWall.size==this->IndicesMirrorWalls.size);
  for (int i=0; i<this->NeighborsAlongWall.size; i++ )
    if (this->NeighborsAlongWall.TheObjects[i]==0)
			output<< -1<<" "<<-1<<" ";
    else
    { //assert(this->MirrorWall.TheObjects[i]!=0);
      output << this->NeighborsAlongWall.TheObjects[i]->IndexInOwnerComplex << " " << this->IndicesMirrorWalls.TheObjects[i]<<" ";
    }
}

void WallData::ReadFromFile(std::fstream& input, CombinatorialChamberContainer& owner)
{ //input >>this->indexInOwnerChamber;
  this->normal.ReadFromFile(input);
  int tempI, indexN, indexW;
  input >>tempI;
  this->NeighborsAlongWall.SetSizeExpandOnTopNoObjectInit(tempI);
  this->IndicesMirrorWalls.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->NeighborsAlongWall.size; i++ )
  { input >> indexN>> indexW;
    if (indexN==-1)
    { this->NeighborsAlongWall.TheObjects[i]=0;
      this->IndicesMirrorWalls.TheObjects[i]=-1;
    }
    else
    { this->NeighborsAlongWall.TheObjects[i]= owner.TheObjects[indexN];
      this->IndicesMirrorWalls.TheObjects[i]=indexW;
    }
  }
}

void CombinatorialChamberContainer::WriteToDefaultFile()
{ std::fstream tempF;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(tempF, "./theChambers.txt", false, true, false);
  GlobalVariables theGlobalVariables;
  this->WriteToFile(tempF, theGlobalVariables);
  tempF.close();
}

void CombinatorialChamberContainer::ReadFromDefaultFile()
{ std::fstream tempF;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(tempF, "./theChambers.txt", false, false, false);
  GlobalVariables theGlobalVariables;
  this->ReadFromFile(tempF, theGlobalVariables);
  tempF.close();
}

void CombinatorialChamberContainer::PauseSlicing()
{
#ifdef WIN32
/*  WaitForSingleObject(this->mutexFlagCriticalSectionEntered, INFINITE);
  this->flagReachSafePointASAP=true;
  ReleaseMutex(this->mutexFlagCriticalSectionEntered);
  while(!this->flagIsRunning)
  {}*/
#else
  pthread_mutex_lock(&this->mutexFlagCriticalSectionEntered);
  this->flagReachSafePointASAP=true;
  pthread_mutex_unlock(&this->mutexFlagCriticalSectionEntered);
  while (!this->flagIsRunning)
  {}
#endif
}

void CombinatorialChamberContainer::ResumeSlicing()
{
#ifdef WIN32
//  WaitForSingleObject(this->mutexFlagCriticalSectionEntered, INFINITE);
//  this->flagIsRunning=true;
//  SetEvent(this->condContinue);
//  ReleaseMutex(this->mutexFlagCriticalSectionEntered);
#else
  pthread_mutex_lock(&this->mutexFlagCriticalSectionEntered);
  this->flagIsRunning=true;
  pthread_cond_signal(&this->condContinue);
  pthread_mutex_unlock(&this->mutexFlagCriticalSectionEntered);
#endif
}


void CombinatorialChamberContainer::SliceTheEuclideanSpace(GlobalVariables& theGlobalVariables)
{ this->SliceTheEuclideanSpace(this->theDirections, this->CurrentIndex, this->AmbientDimension, 0, theGlobalVariables);
}

void Cone::WriteToFile(std::fstream &output, GlobalVariables &theGlobalVariables)
{ this->roots::WriteToFile(output, theGlobalVariables);
  output <<"\nChamberTestArray: "<<this->ChamberTestArray.size<<" ";
  for (int i=0; i<this->ChamberTestArray.size; i++)
    output<< this->ChamberTestArray.TheObjects[i]<<" ";
}

void Cone::ReadFromFile(std::fstream &input, GlobalVariables &theGlobalVariables)
{ std::string tempS; int tempI;
  this->roots::ReadFromFile(input, theGlobalVariables);
  input >> tempS>> tempI;
  this->ChamberTestArray.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->ChamberTestArray.size; i++)
    input >> this->ChamberTestArray.TheObjects[i];
}

void simplicialCones::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->theFacets.WriteToFile(output);
  output <<"\nConesHavingFixedNormal: ";
  output << this->ConesHavingFixedNormal.size<<" ";
  for (int i=0; i<this->ConesHavingFixedNormal.size; i++)
  { output << this->ConesHavingFixedNormal.TheObjects[i].size<<" ";
    for (int j=0; j<this->ConesHavingFixedNormal.TheObjects[i].size; j++)
      output<< this->ConesHavingFixedNormal.TheObjects[i].TheObjects[j]<<" ";
  }
  output <<"\ntheCones: "<<this->size<< " ";
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].WriteToFile(output, theGlobalVariables);
    output <<" ";
  }
}

void simplicialCones::ReadFromFile(std::fstream &input, GlobalVariables& theGlobalVariables)
{ std::string tempS; int tempI;
  this->theFacets.ReadFromFile(input);
  input >>tempS;
  assert(tempS=="ConesHavingFixedNormal:");
  input >> tempI;
  this->ConesHavingFixedNormal.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->ConesHavingFixedNormal.size; i++)
  { input >> tempI;
    this->ConesHavingFixedNormal.TheObjects[i].SetSizeExpandOnTopNoObjectInit(tempI);
    for (int j=0; j<this->ConesHavingFixedNormal.TheObjects[i].size; j++)
      input>>this->ConesHavingFixedNormal.TheObjects[i].TheObjects[j];
   }
  input>>tempS>>tempI;
  assert(tempS=="theCones:");
  this->SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].ReadFromFile(input, theGlobalVariables);
}

void hashedRoots::WriteToFile(std::fstream &output)
{ int theDimension=0;
  if (this->size>0)
    theDimension= this->TheObjects[0].size;
  output << "Num|dim: " <<this->size<<" "<< theDimension<<" ";
  for (int i=0; i<this->size; i++)
    for (int j=0; j<theDimension; j++)
    { this->TheObjects[i].TheObjects[j].WriteToFile(output);
      output<<" ";
    }
}

void hashedRoots::ReadFromFile(std::fstream &input)
{ int theDimension; std::string tempS;
  int theSize;
  this->ClearTheObjects();
  input >> tempS >> theSize>> theDimension;
  this->MakeActualSizeAtLeastExpandOnTop(theSize);
  root tempRoot;
  tempRoot.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theSize; i++)
  { for (int j=0; j<theDimension; j++)
      tempRoot.TheObjects[j].ReadFromFile(input);
    this->AddObjectOnTopHash(tempRoot);
   }
}

void slTwo::ElementToString(std::string& output, bool useHtml, bool useLatex, GlobalVariables& theGlobalVariables)
{ std::stringstream out;  std::string tempS;
  this->theH.ElementToString(tempS, *this->owner, useHtml, useLatex);
  out <<"\n\n$h=$" <<tempS;
  this->theE.ElementToString(tempS, *this->owner, useHtml, useLatex);
  out <<"\n\n$e=$" <<tempS;
  this->theF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  out <<"\n\n$f=$" <<tempS;
  output = out.str();
}

void slTwo::ElementToHtml(std::string& filePath)
{ std::fstream theFile;
  std::string theFileName=filePath;
  theFileName.append("theSlTwo.txt");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, filePath, false, true, false);
}

void SimpleLieAlgebra::FindSl2Subalgebras(char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables)
{ rootSubalgebras theRootSAs;
  SltwoSubalgebras tempSl2s;
  theRootSAs.GenerateAllRootSubalgebrasUpToIsomorphism(theGlobalVariables, WeylLetter, WeylRank, true, false);
  theRootSAs.ComputeDebugString(false, false, false, 0, 0,theGlobalVariables);
//  IndicatorWindowGlobalVariables.StatusString1=theRootSAs.DebugString;
  //IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  //theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
  theRootSAs.TheObjects[0].GetSsl2Subalgebras(tempSl2s, theGlobalVariables);
  tempSl2s.ComputeDebugString(theGlobalVariables, theRootSAs.AmbientWeyl, false, false);
  this->DebugString= tempSl2s.DebugString;

}

void rootSubalgebra::GetSsl2Subalgebras(SltwoSubalgebras& output, GlobalVariables& theGlobalVariables)
{ //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1-10.4
  Selection theRootsWithZeroCharacteristic;
  DynkinDiagramRootSubalgebra tempDiagram;
  int theRelativeDimension = this->SimpleBasisK.size;
  theRootsWithZeroCharacteristic.init(theRelativeDimension);
  SimpleLieAlgebra theLieAlgebra;
  MatrixLargeRational InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k=0; k<theRelativeDimension; k++)
    for (int j=0; j<theRelativeDimension; j++)
      InvertedRelativeKillingForm.elements[k][j]=this->AmbientWeyl.RootScalarKillingFormMatrixRoot(this->SimpleBasisK.TheObjects[k], this->SimpleBasisK.TheObjects[j]);
  InvertedRelativeKillingForm.Invert(theGlobalVariables);
  theLieAlgebra.ComputeChevalleyConstants(this->AmbientWeyl.WeylLetter, this->AmbientWeyl.KillingFormMatrix.NumRows, theGlobalVariables);
  //int relativeDimension =
  int numCycles= MathRoutines::TwoToTheNth(theRootsWithZeroCharacteristic.MaxSize);
  roots tempRoots;
  tempRoots.MakeActualSizeAtLeastExpandOnTop(theRootsWithZeroCharacteristic.MaxSize);
  roots relativeRootSystem;
  this->PositiveRootsK.GetCoordsInBasis(this->SimpleBasisK, relativeRootSystem, theGlobalVariables);
  slTwo theSl2;
  for (int i=0; i<numCycles; i++, theRootsWithZeroCharacteristic.incrementSelection())
  { tempRoots.size=0;
    for (int j=0; j<theRootsWithZeroCharacteristic.CardinalitySelection; j++)
      tempRoots.AddObjectOnTop(this->SimpleBasisK.TheObjects[theRootsWithZeroCharacteristic.elements[j]]);
    tempDiagram.ComputeDiagramType(tempRoots, this->AmbientWeyl);
    int theSlack=0;
    for (int j=0; j<relativeRootSystem.size; j++)
    { Rational DimTwoSpace=0;
      for (int k=0; k<theRelativeDimension; k++)
        if (!theRootsWithZeroCharacteristic.selected[k])
        { DimTwoSpace+=relativeRootSystem.TheObjects[j].TheObjects[k];
          if (DimTwoSpace>1)
            break;
        }
      if (DimTwoSpace==1)
        theSlack++;
    }
    int theDynkinEpsilon = tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is (but generators still have to be found)
    //this is done in the below code.
    theRootsWithZeroCharacteristic.ComputeDebugString();
    if (theDynkinEpsilon==0)
    { theSl2.theE.Nullify(theLieAlgebra);
      theSl2.theH.Nullify(theLieAlgebra);
      root tempRoot, tempRoot2;
      tempRoot.MakeZero(theRelativeDimension);
      for (int k=0; k<theRelativeDimension; k++)
        if(!theRootsWithZeroCharacteristic.selected[k])
        { theSl2.theE.SetCoefficient(this->SimpleBasisK.TheObjects[k], 1, theLieAlgebra);
          tempRoot.TheObjects[k]=2;
        }
      InvertedRelativeKillingForm.ActOnAroot(tempRoot, tempRoot2);
      theSl2.theH.Hcomponent.MakeZero(this->AmbientWeyl.KillingFormMatrix.NumRows);
      for (int j=0; j<theRelativeDimension; j++)
        theSl2.theH.Hcomponent+= this->SimpleBasisK.TheObjects[j]*tempRoot2.TheObjects[j];
      theSl2.ComputeDebugString(false, false, theGlobalVariables);
      if(theLieAlgebra.AttemptExtendingHEtoHEF(theSl2.theH.Hcomponent, theSl2.theE, theSl2.theF, theGlobalVariables))
        output.AddObjectOnTopHash(theSl2);
    }
  }
}
