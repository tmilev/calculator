//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_2_h_already_included
#define vpfHeader1_2_h_already_included

#include "vpfHeader1General0_General.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader2Math01_LargeIntArithmetic.h"
#include "vpfHeader2Math02_Vectors.h"

static ProjectInformationInstance ProjectInfoVpfHeader1_2(__FILE__, "Header, math routines. ");

//we are wrapping the math.h c++ functions for portability reasons (if for some reason we want to change from math.h to a better floating point
//library, we only need to change the implementation of the Floatingpoint class.
class FloatingPoint
{
public:
  static double sin(double argument);
  static double cos(double argument);
  static double arctan(double argument);
  static double arcsin(double argument);
  static double sqrt(double argument);
  static double power(double base, double exponent);
  static double log(double argument);
  static double floor(double argument);
};

class WeylGroup;
class AlgebraicClosureRationals;
template <class coefficient>
class MatrixTensor;

class ChevalleyGenerator
{
public:
  SemisimpleLieAlgebra* owneR;
  int theGeneratorIndex;
  ChevalleyGenerator(): owneR(0), theGeneratorIndex(-1){}
  friend std::ostream& operator << (std::ostream& output, const ChevalleyGenerator& theGen)
  { output << theGen.ToString();
    return output;
  }
  bool CheckInitialization()const;
  static const bool IsEqualToZero()
  { return false;
  }
  static inline unsigned int HashFunction(const ChevalleyGenerator& input)
  { return (unsigned) input.theGeneratorIndex;
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  void MakeGenerator(SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex)
  { this->owneR=&inputOwner;
    this->theGeneratorIndex=inputGeneratorIndex;
  }
  void operator=(const ChevalleyGenerator& other)
  { this->owneR=other.owneR;
    this->theGeneratorIndex=other.theGeneratorIndex;
  }
  bool operator>(const ChevalleyGenerator& other)const;
  std::string ToString(FormatExpressions* inputFormat=0)const;
  void CheckConsistencyWithOther(const ChevalleyGenerator& other)const;
  bool operator==(const ChevalleyGenerator& other)const
  { this->CheckConsistencyWithOther(other);
    return this->theGeneratorIndex==other.theGeneratorIndex;
  }
};

template <class coefficient, unsigned int inputHashFunction(const coefficient&)= coefficient::HashFunction>
class MonomialTensor
{
  friend std::ostream& operator <<(std::ostream& output, const MonomialTensor<coefficient, inputHashFunction>& theMon)
  { return output << theMon.ToString();
  }
private:
public:
  List<int> generatorsIndices;
  List<coefficient> Powers;
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool IsEqualToOne()const
  { return this->generatorsIndices.size==0;
  }
  void operator=(List<int>& other)
  { this->generatorsIndices.ReservE(other.size);
    this->Powers.ReservE(other.size);
    this->MakeConst();
    for (int i=0; i<other.size; i++)
      this->MultiplyByGeneratorPowerOnTheRight(other[i], 1);
  }
  void operator=(const MonomialTensor<coefficient, inputHashFunction>& other)
  { this->generatorsIndices=(other.generatorsIndices);
    this->Powers=other.Powers;
  }
  int GetMinNumVars()
  { int result=0;
    for (int i=0; i<this->Powers.size; i++)
      result=MathRoutines::Maximum(result, this->Powers[i].GetMinNumVars());
    return result;
  }
  template<class otherType>
  void operator=(const MonomialTensor<otherType>& other)
  { this->generatorsIndices=(other.generatorsIndices);
    this->Powers.SetSize(other.Powers.size);
    for (int i=0; i<other.Powers.size; i++)
      this->Powers[i]=other.Powers[i];
  }
  bool SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower);
  void MultiplyByGeneratorPowerOnTheLeft(int theGeneratorIndexStandsToTheLeft, const coefficient& thePower);
  unsigned int HashFunction()const
  { int top=MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
    unsigned int result=0;
    for (int i=0; i<top; i++)
      result+=SomeRandomPrimes[i]*this->generatorsIndices[i] + SomeRandomPrimes[top-1-i]* inputHashFunction(this->Powers[i]);
    return result;
  }
  static inline unsigned int HashFunction(const MonomialTensor<coefficient, inputHashFunction>& input)
  { return input.HashFunction();
  }
  void MakeConst()
  { this->generatorsIndices.size=0;
    this->Powers.size=0;
  }
  bool operator>(const MonomialTensor<coefficient, inputHashFunction>& other)const
  { if (other.generatorsIndices.size>this->generatorsIndices.size)
      return false;
    if (other.generatorsIndices.size< this->generatorsIndices.size)
      return true;
    for (int i=0; i<this->generatorsIndices.size; i++)
    { if (other.generatorsIndices[i]>this->generatorsIndices[i])
        return false;
      if (other.generatorsIndices[i]<this->generatorsIndices[i])
        return true;
      if (other.Powers[i]>this->Powers[i])
        return false;
      if (this->Powers[i]>other.Powers[i])
        return true;
    }
    return false;
  }
  bool operator==(const MonomialTensor<coefficient, inputHashFunction>& other)const
  { return this->Powers==other.Powers && this->generatorsIndices==other.generatorsIndices;
  }
  inline void operator*=(const MonomialTensor<coefficient, inputHashFunction>& standsOnTheRight)
  { if (standsOnTheRight.generatorsIndices.size==0)
      return;
    if (this==&standsOnTheRight)
    { MonomialTensor<coefficient, inputHashFunction> tempMon;
      tempMon=standsOnTheRight;
      (*this)*=(tempMon);
      return;
    }
    this->generatorsIndices.SetExpectedSize(standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
    this->Powers.SetExpectedSize(standsOnTheRight.generatorsIndices.size+this->generatorsIndices.size);
    int firstIndex=standsOnTheRight.generatorsIndices[0];
    int i=0;
    if (this->generatorsIndices.size>0)
      if (firstIndex==(*this->generatorsIndices.LastObject()))
      { *this->Powers.LastObject()+=standsOnTheRight.Powers[0];
        i=1;
      }
    for (; i<standsOnTheRight.generatorsIndices.size; i++)
    { this->Powers.AddOnTop(standsOnTheRight.Powers[i]);
      this->generatorsIndices.AddOnTop(standsOnTheRight.generatorsIndices[i]);
    }
  }
};

class MonomialP
{
private:
  //monbody contains the exponents of the variables.
  //IMPORTANT. The monBody of a monomial is NOT unique.
  //Two monomials whose non-zero entries coincide
  //(but otherwise one monomial might have more entries filled with zeroes)
  //are considered to be equal.
  //Therefore special attention must be paid when performing operations with
  //MonomialP's, especially with operator== and MonomialP::HashFunction!
  //Please modify this class in accordance with what was just explained.
  //Note that by the above token I decided to declare operator[] as non-const
  //function and operator() as a const function but returning a copy of the
  //underlying element, rather than a reference to the element.
  //
  //IMPORTANT. The default monomial order, implemented by operator>, is the graded lexicographic
  //last variable strongest order. Other monomial orders are not referred by operator>, but
  // by their explicit names.
  //Note that the MonomialCollection::ToString method uses the FormatExpressions::thePolyMonOrder
  //to sort monomials when displaying polynomials to the screen.
  List<Rational> monBody;
public:
  friend std::ostream& operator << (std::ostream& output, const MonomialP& theMon)
  { output << theMon.ToString();
    return output;
  }
  Rational& operator[](int i)
  { if (i<0)
      crash << "This is a programming error: requested exponent of monomial variable with index " << i << " which is negative. " << crash;
    if (i>=this->monBody.size)
      this->SetNumVariablesSubDeletedVarsByOne(i+1);
    return this->monBody[i];
  }
  Rational operator()(int i)const
  { if (i<0)
      crash << "This is a programming error: requested exponent of monomial variable " << " with index " << i << " which is negative. " << crash;
    if (i>=this->monBody.size)
      return 0;
    return this->monBody[i];
  }
  inline static std::string GetXMLClassName()
  { return "MonomialP";
  }
  unsigned  int HashFunction() const
  { return this->monBody.HashFunction();
  }
  bool HasPositiveOrZeroExponents()const
  { for (int i=0; i<this->monBody.size; i++)
      if (this->monBody[i].IsNegative())
        return false;
    return true;
  }
  static const inline bool IsEqualToZero()
  { return false;
  }
  void ExponentMeBy(const Rational& theExp);
  //Warning: HashFunction must return the same result
  //for equal monomials represented by different monBodies.
  // Two such different representation may differ by extra entries filled in with zeroes.
  static inline unsigned int HashFunction(const MonomialP& input)
  { unsigned int result=0;
    int numCycles=MathRoutines::Minimum(input.monBody.size, SomeRandomPrimesSize);
    for (int i=0; i<numCycles; i++)
      result+=input.monBody[i].HashFunction();
    return result;
  }
  std::string ToString(FormatExpressions* PolyFormat=0)const;
  void MakeOne(int ExpectedNumVars=0)
  { this->monBody.initFillInObject(ExpectedNumVars, (Rational) 0);
  }
  bool operator>(const MonomialP& other)const;
  bool IsDivisibleBy(const MonomialP& other)const;
  int TotalDegreeInt()const
  { int result=-1;
    if (!this->TotalDegree().IsSmallInteger(&result))
      crash << "This is a programming error: total degree of monomial must be a small integer to call this function. " << crash;
    return result;
  }
  Rational TotalDegree()const
  { Rational result=0;
    for (int i=0; i<this->monBody.size; i++)
      result+=this->monBody[i];
    return result;
  }
  inline void MultiplyBy(const MonomialP& other)
  { this->operator*=(other);
  }
  inline void DivideBy(const MonomialP& other)
  { this->operator/=(other);
  }
  bool IsLinear()const
  { return this->IsAConstant() || this->IsLinearNoConstantTerm();
  }
  bool IsLinearNoConstantTerm()const
  { int tempInt;
    return this->IsOneLetterFirstDegree(&tempInt);
  }
  bool IsOneLetterFirstDegree(int* whichLetter=0)const
  { Rational whichDegree;
    if (!this->IsOneLetterNthDegree(whichLetter, &whichDegree))
      return false;
    return whichDegree==1;
  }
  bool IsOneLetterNthDegree(int* whichLetter=0, Rational* whichDegree=0)const
  { int tempI1;
    if (whichLetter==0)
      whichLetter=&tempI1;
    *whichLetter=-1;
    for (int i=0; i<this->monBody.size; i++)
      if (this->monBody[i]!=0)
      { if (whichDegree!=0)
          *whichDegree=this->monBody[i];
        if ((*whichLetter)==-1)
          *whichLetter=i;
        else
          return false;
      }
    return (*whichLetter)!=-1;
  }
  template <class Element>
  bool SubstitutioN(const List<Polynomial<Element> >& TheSubstitution, Polynomial<Element>& output, const Element& theRingUnit=1)const;
  void MakeEi(int LetterIndex, int Power=1, int ExpectedNumVars=0);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { for (int i=this->monBody.size-1; i>=0; i--)
      if (this->monBody[i]!=0)
        return i;
    return -1;
  }
  bool IsGEQpartialOrder(MonomialP& m);
  static bool LeftIsGEQLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { if (left==(right))
      return false;
    return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right)
  { if (left==(right))
      return false;
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftIsGEQLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right)
  { return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { if (left==right)
      return false;
    return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right)
  { if (left==right)
      return false;
    if (left.TotalDegree()>right.TotalDegree())
      return true;
    if (left.TotalDegree() <right.TotalDegree())
      return false;
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftIsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right)
  { return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }

  bool IsGEQLexicographicLastVariableStrongest(const MonomialP& other)const;
  bool IsGEQLexicographicLastVariableWeakest(const MonomialP& other)const;
  bool IsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& other)const;
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  bool IsAConstant()const
  { for (int i=0; i<this->monBody.size; i++)
      if (!this->monBody[i].IsEqualToZero())
        return false;
    return true;
  }
  inline int GetMinNumVars()const
  { return this->monBody.size;
  }
  void Invert()
  { for (int i=0; i<this->monBody.size; i++)
      this->monBody[i].Minus();
  }
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData);
  void RaiseToPower(const Rational& thePower)
  { for (int i=0; i<this->monBody.size; i++)
      this->monBody[i]*=thePower;
  }
  void operator*=(const MonomialP& other);
  void operator/=(const MonomialP& other);
  bool operator==(const MonomialP& other)const;
  template <class coefficient>
  void operator=(const Vector<coefficient>& other)
  { this->monBody=(other);
  }
  void operator=(const MonomialP& other)
  { this->monBody=(other.monBody);
  }
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { this->monBody.ReadFromFile(input);
  }
  void ReadFromFile(std::fstream& input)
  { this->monBody.ReadFromFile(input);
  }
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)const
  { this->monBody.WriteToFile(output);
  }
  void WriteToFile(std::fstream& output)const
  { this->monBody.WriteToFile(output);
  }
};

struct FormatExpressions
{
public:
  //alphabetBases must contain at least two elements
  std::string chevalleyGgeneratorLetter;
  std::string chevalleyHgeneratorLetter;
  std::string fundamentalWeightLetter;
  std::string polyDefaultLetter;
  std::string WeylAlgebraDefaultLetter;
  std::string CustomPlusSign;
  std::string CustomCoeffMonSeparator;
  std::string FDrepLetter;
  std::string simpleRootLetter;
  std::string PathDisplayNameCalculator;
  std::string PathPhysicalOutputFolder;
  std::string PathDisplayOutputFolder;
  std::string PathDisplayServerBaseFolder;
  List<std::string> polyAlphabeT;
  List<std::string> weylAlgebraLetters;
  List<std::string> vectorSpaceEiBasisNames;
  std::string GetPolyLetter(int index)const;
  std::string GetChevalleyHletter(int index)const;
  std::string GetChevalleyGletter(int index)const;
  char AmbientWeylLetter;
  Rational AmbientCartanSymmetricInverseScale;
  int ExtraLinesCounterLatex;
  int NumAmpersandsPerNewLineForLaTeX;
  int MaxRecursionDepthPerExpression;
  int MaxLineLength;
  int MaxLinesPerPage;
  int MatrixColumnVerticalLineIndex;
  bool flagPassCustomCoeffMonSeparatorToCoeffs;
  bool flagMakingExpressionTableWithLatex;
  bool flagUseLatex;
  bool flagUsePNG;
  bool flagUseHTML;
  bool flagUseHtmlAndStoreToHD;
  bool flagUseMathSpanPureVsMouseHover;
//  bool flagDynkinTypeDontUsePlusAndUpperIndex;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsFinal;
  bool flagIncludeMutableInformation;
  bool flagExpressionNewLineAllowed;
  bool flagIncludeLieAlgebraNonTechnicalNames;
  bool flagIncludeLieAlgebraTypes;
  bool flagUseReflectionNotation;
  bool flagCandidateSubalgebraShortReportOnly;

  List<MonomialP>::OrderLeftGreaterThanRight thePolyMonOrder;
  template <typename TemplateMonomial>
  typename List<TemplateMonomial>::OrderLeftGreaterThanRight GetMonOrder();
  FormatExpressions();
};


class MonomialWeylAlgebra
{
  public:
  MonomialP polynomialPart;
  MonomialP differentialPart;
  friend std::ostream& operator << (std::ostream& output, const MonomialWeylAlgebra& theMon)
  { output << theMon.ToString();
    return output;
  }
  static const inline bool IsEqualToZero()
  { return false;
  }
  bool IsAConstant()const
  { return this->polynomialPart.IsAConstant() && this->differentialPart.IsAConstant();
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  static unsigned int HashFunction(const MonomialWeylAlgebra& input)
  { return
    input.polynomialPart.HashFunction()+
    input.differentialPart.HashFunction()*SomeRandomPrimes[0];
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  int GetMinNumVars()const
  { return MathRoutines::Maximum(this->polynomialPart.GetMinNumVars(), this->differentialPart.GetMinNumVars());
  }
  bool operator==(const MonomialWeylAlgebra& other)const
  { return this->polynomialPart==other.polynomialPart &&
    this->differentialPart==other.differentialPart;
  }
  bool operator>(const MonomialWeylAlgebra& other)const
  { if (this->differentialPart>other.differentialPart)
      return true;
    if (other.differentialPart>this->differentialPart)
      return false;
    if (this->polynomialPart>other.polynomialPart)
      return true;
    if(other.polynomialPart>this->polynomialPart)
      return false;
    return false;
  }
  bool HasNonSmallPositiveIntegerDerivation()const
  { for (int i=0; i<this->differentialPart.GetMinNumVars(); i++)
      if (!this->differentialPart(i).IsSmallInteger())
        return true;
    return false;
  }
  void MakeOne(int ExpectedNumVars=0)
  { this->polynomialPart.MakeOne(ExpectedNumVars);
    this->differentialPart.MakeOne(ExpectedNumVars);
  }
};

template <class Element>
std::iostream& operator <<(std::iostream& output, const Polynomial<Element>& input)
{ output << input.ToString();
  return output;
}

template <class TemplateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& output, const MonomialCollection<TemplateMonomial, coefficient>& theCollection);

template <class TemplateMonomial, class coefficient>
class MonomialCollection
{
private:
  void AddOnTop(const MonomialP& tempP);//<-to guard the now unsafe base class method
  void Clear();//<-to guard the now unsafe base class method
  friend std::ostream& operator<< <TemplateMonomial, coefficient>(std::ostream& output, const MonomialCollection<TemplateMonomial, coefficient>& theCollection);
public:
  HashedList<TemplateMonomial> theMonomials;
  List<coefficient> theCoeffs;
  MonomialCollection(){}
  MonomialCollection(const MonomialCollection& other)
  { this->operator=(other);
  }
  bool NeedsBrackets()const
  { return this->size()>1;
  }
  inline static std::string GetXMLClassName()
  { std::string result="MonomialCollection_";
    result.append(TemplateMonomial::GetXMLClassName());
    return result;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  inline int size()const
  { return this->theMonomials.size;
  }
  //BIG FAT WARNING
  //the hash function of Monomial collection must return the same value for
  // monomial collections whose monomials are permuted!
  static unsigned int HashFunction(const MonomialCollection<TemplateMonomial, coefficient>& input)
  { unsigned int result=0;
    for (int i=0; i<input.size(); i++)
      result+= input.theCoeffs[i].HashFunction()*input[i].HashFunction();
    return result;
  }
  void QuickSortAscending(typename List<TemplateMonomial>::OrderLeftGreaterThanRight theOrder=0)
  { List<TemplateMonomial> theSortedMons=this->theMonomials;
    theSortedMons.QuickSortAscending(theOrder, &this->theCoeffs);
    this->theMonomials=theSortedMons;
  }
  void QuickSortDescending(typename List<TemplateMonomial>::OrderLeftGreaterThanRight theOrder=0)
  { List<TemplateMonomial> theSortedMons=this->theMonomials;
    theSortedMons.QuickSortDescending(theOrder, &this->theCoeffs);
    this->theMonomials=theSortedMons;
  }
  void PopMonomial(int index)
  { this->theMonomials.RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  void AddOtherTimesConst(MonomialCollection<TemplateMonomial, coefficient>& other, const coefficient& theConst);
  void PopMonomial(int index, TemplateMonomial& outputMon, coefficient& outputCoeff)
  { outputMon=(*this)[index];
    outputCoeff=this->theCoeffs[index];
    this->theMonomials.RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  inline int HashFunction()const
  { return this->HashFunction(*this);
  }
  coefficient GetCoefficientsSum()const
  { coefficient result=0;
    for (int i=0; i<this->theCoeffs.size; i++)
      result+=this->theCoeffs[i];
    return result;
  }
  void AddMonomial(const TemplateMonomial& inputMon, const coefficient& inputCoeff)
  { this->CleanupMonIndex(this->AddMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  inline bool CleanupMonIndex(int theIndex)
  { if (theIndex!=-1)
      if (this->theCoeffs[theIndex]==0)
      { this->theMonomials.RemoveIndexSwapWithLast(theIndex);
        this->theCoeffs.RemoveIndexSwapWithLast(theIndex);
        return true;
      }
    return false;
  }
  int AddMonomialNoCoeffCleanUpReturnsCoeffIndex(const TemplateMonomial& inputMon, const coefficient& inputCoeff);
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRows
  (List<MonomialCollectionTemplate>& theList, bool *IvemadeARowSwitch=0, HashedList<TemplateMonomial>* seedMonomials=0, Matrix<coefficient>* carbonCopyMatrix=0,
   List<MonomialCollectionTemplate>* carbonCopyList=0);
  template <class MonomialCollectionTemplate>
  static void IntersectVectorSpaces
  (const List<MonomialCollectionTemplate>& vectorSpace1, const List<MonomialCollectionTemplate>& vectorSpace2,
   List<MonomialCollectionTemplate>& outputIntersection, HashedList<TemplateMonomial>* seedMonomials=0);
  template <class MonomialCollectionTemplate>
  static int GetRankIntersectionVectorSpaces
  (List<MonomialCollectionTemplate>& vectorSpace1, List<MonomialCollectionTemplate>& vectorSpace2, HashedList<TemplateMonomial>* seedMonomials=0)
  { List<MonomialCollectionTemplate> listCopy=vectorSpace1;
    listCopy.AddListOnTop(vectorSpace2);
    return vectorSpace1.size+vectorSpace2.size-
    MonomialCollection<TemplateMonomial, coefficient>::GetRankOfSpanOfElements(listCopy, seedMonomials);
  }
  template <class MonomialCollectionTemplate>
  static bool VectorSpacesIntersectionIsNonTrivial
  (List<MonomialCollectionTemplate>& vectorSpace1, List<MonomialCollectionTemplate>& vectorSpace2, HashedList<TemplateMonomial>* seedMonomials=0)
  { return 0!=MonomialCollection<TemplateMonomial, coefficient>::GetRankIntersectionVectorSpaces(vectorSpace1, vectorSpace2, seedMonomials);
  }
  template <class MonomialCollectionTemplate>
  static bool LinSpanContains
  (const List<MonomialCollectionTemplate>& theList, const MonomialCollectionTemplate& input, HashedList<TemplateMonomial>* seedMonomials=0)
  { List<MonomialCollectionTemplate> listCopy=theList;
    MonomialCollection<TemplateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    int startSpanSize=listCopy.size;
    listCopy.AddOnTop(input);
    MonomialCollection<TemplateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size==startSpanSize;
  }
  bool HasRationalCoeffs(MonomialCollection<TemplateMonomial, Rational>* outputConversionToRationals=0)
  { Rational tempRat;
    Rational* theCF=0;
    if (outputConversionToRationals!=0)
    { theCF=&tempRat;
      outputConversionToRationals->MakeZero();
    }
    for (int i=0; i< this->size(); i++)
      if (!this->theCoeffs[i].IsRational(theCF))
        return false;
      else
        if (outputConversionToRationals!=0)
          outputConversionToRationals->AddMonomial((*this)[i], *theCF);
    return true;
  }
  template <class MonomialCollectionTemplate>
  static int GetRankOfSpanOfElements(List<MonomialCollectionTemplate>& theList, HashedList<TemplateMonomial>* seedMonomials=0)
  { List<MonomialCollectionTemplate> listCopy=theList;
    MonomialCollection<TemplateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size;
  }
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRowsDeleteZeroRows(List<MonomialCollectionTemplate >& theList, bool *IvemadeARowSwitch=0, HashedList<TemplateMonomial>* seedMonomials=0)
  { MonomialCollectionTemplate::GaussianEliminationByRows(theList, IvemadeARowSwitch, seedMonomials);
    for (int j=theList.size-1; j>=0; j--)
      if (theList[j].IsEqualToZero())
        theList.size--;
  }
  int SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(const TemplateMonomial& inputMon, const coefficient& inputCoeff);
  void CleanUpZeroCoeffs()
  { for (int i=0; i<this->size; i++)
      if (this->CleanupMonIndex(i))
        i--;
  }
  template<class baseType>
  void SubstitutionCoefficients(const List<Polynomial<baseType> >& theSub)
  { coefficient newCoeff;
    for (int i=0; i<this->size(); i++)
    { newCoeff=this->theCoeffs[i];
      newCoeff.Substitution(theSub);
      if (newCoeff.IsEqualToZero())
      { this->PopMonomial(i);
        i--;
        continue;
      }
      this->theCoeffs[i]=newCoeff;
    }
  }
  void SubtractMonomial(const TemplateMonomial& inputMon, const coefficient& inputCoeff)
  { this->CleanupMonIndex(this->SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  coefficient GetMonomialCoefficient(const TemplateMonomial& inputMon)const
  { int theIndex=this->theMonomials.GetIndex(inputMon);
    if (theIndex==-1)
      return 0;
    return this->theCoeffs[theIndex];
  }
  int TotalDegree();
  void checkConsistency()const
  { this->theMonomials.GrandMasterConsistencyCheck();
    this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
    for (int i =0; i<this->size(); i++)
      this->theCoeffs[i].checkConsistency();
  }
  template <class baseRing>
  baseRing FindGCDCoefficientNumerators()const
  { if (this->size()==0)
      return 1;
    baseRing result, tempCF;
    this->theCoeffs[0].GetNumerator(result);
    for (int i=1; i<this->size(); i++)
    { this->theCoeffs[i].GetNumerator(tempCF);
      coefficient::gcd(result, tempCF, result);
    }
    return result;
  }
  Rational FindGCDCoefficientNumeratorsOverRationals()const
  { if (this->size()==0)
      return 1;
    LargeInt result, tempUI;
    result=this->theCoeffs[0].GetNumeratorRationalPart();
    result.sign=1;
    for (int i=1; i<this->size(); i++)
    { tempUI=this->theCoeffs[i].GetNumeratorRationalPart();
      LargeIntUnsigned::gcd(result.value, tempUI.value, result.value);
    }
    return result;
  }
  Rational FindLCMCoefficientDenominatorsOverRationals()const
  { LargeInt result, tempUI, tempRat;
    result=1;
    for (int i=0; i<this->size(); i++)
    { tempUI=this->theCoeffs[i].GetDenominator();
      LargeIntUnsigned::lcm(result.value, tempUI.value, result.value);
    }
    return result;
  }
  Rational ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy()
  { if (this->size()==0)
      return 1;
    Rational result=1;
    Rational tempRat;
    for (int i=0; i<this->size(); i++)
    { tempRat=this->theCoeffs[i].GetDenominatorRationalPart();
      *this*=tempRat;
      result*=tempRat;
    }
    LargeInt theGCD, tempUI;
    theGCD=this->theCoeffs[0].GetNumeratorRationalPart().GetNumerator();
    theGCD.sign=1;
    for (int i=0; i<this->size(); i++)
    { tempUI=this->theCoeffs[i].GetNumeratorRationalPart().GetNumerator();
      LargeIntUnsigned::gcd(theGCD.value, tempUI.value, theGCD.value);
    }
    tempRat=theGCD;
    *this/=tempRat;
    result/=tempRat;
    return result;
  }

  static std::string GetBlendCoeffAndMon(const TemplateMonomial& inputMon, coefficient& inputCoeff, bool addPlusToFront, FormatExpressions* theFormat=0);
  void CheckNumCoeffsConsistency(const char* fileName, int lineName)const
  { if (this->theCoeffs.size!=this->theMonomials.size)
      crash << "This is a programming error: a monomial collection has " << this->theMonomials.size << " monomials but "
      << this->theCoeffs.size << " coefficients. " << crash;
  }
  bool IsEqualToZero()const;
  int FindMaxPowerOfVariableIndex(int VariableIndex);
  void MakeZero()
  { this->theMonomials.Clear();
    this->theCoeffs.SetSize(0);
  }
  inline int GetMinNumVars()const
  { int result =0;
    for (int i=0; i<this->size(); i++)
      result= MathRoutines::Maximum(result, (*this)[i].GetMinNumVars());
    return result;
  }
  bool IsSmallInteger(int* whichInteger=0)const;
  bool IsInteger(LargeInt* whichInteger=0)const;
  void SetExpectedSize(int theSize)
  { this->theMonomials.SetExpectedSize(theSize);
    this->theCoeffs.SetExpectedSize(theSize);
  }
  bool HasGEQMonomial(TemplateMonomial& m, int& WhichIndex);
  void WriteToFile(std::fstream& output);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { return this->WriteToFile(output);
  }
  void ClearDenominators(Rational& output)
  { output.MakeOne();
    Rational tempRat;
    for (int i=0; i<this->size(); i++)
      if (!this->theCoeffs[i].IsInteger())
      { tempRat=this->theCoeffs[i].GetDenominator();
        *this*=tempRat;
        output*=tempRat;
      }
  }
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  inline bool operator!=(const MonomialCollection<TemplateMonomial, coefficient>& other)const
  { return !(*this==other);
  }
  bool operator==(const MonomialCollection<TemplateMonomial, coefficient>& other)const;
  bool operator==(int x)const;
  inline void operator+=(const MonomialCollection<TemplateMonomial, coefficient>& other);
  MonomialCollection<TemplateMonomial, coefficient> operator*(const coefficient& other)const
  { MonomialCollection<TemplateMonomial, coefficient> result=*this;
    result*=other;
    return result;
  }
//  inline void operator+=(const TemplateMonomial& m)
//  { this->AddMonomial(m, 1);
//  }
  MonomialCollection<TemplateMonomial, coefficient> operator-(const MonomialCollection<TemplateMonomial, coefficient>& other)const
  { MonomialCollection<TemplateMonomial, coefficient> output=*this;
    output-=other;
    return output;
  }
  MonomialCollection<TemplateMonomial, coefficient> operator+(const MonomialCollection<TemplateMonomial, coefficient>& other)const
  { MonomialCollection<TemplateMonomial, coefficient> output=*this;
    output+=other;
    return output;
  }
  void MakeConst(const coefficient& coeff)
  { TemplateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, coeff);
  }
  void MakeOne()
  { this->MakeConst(1);
  }
  inline void operator+=(const coefficient& other)
  { TemplateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, other);
  }
  inline void operator+=(const TemplateMonomial& other)
  { this->AddMonomial(other, 1);
  }
  inline void operator-=(const coefficient& other)
  { TemplateMonomial tempM;
    tempM.MakeOne();
    this->SubtractMonomial(tempM, other);
  }
  template <class OtherType>
  void AssignOtherType(const MonomialCollection<TemplateMonomial, coefficient>& other)
  { this->::HashedList<TemplateMonomial>::operator=(other);
    this->theCoeffs.SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->theCoeffs[i]=other.theCoeffs[i];
  }
  inline void operator-=(const MonomialCollection<TemplateMonomial, coefficient>& other)
  { this->SubtractOtherTimesCoeff(other);
  }
  inline void SubtractOtherTimesCoeff(const MonomialCollection<TemplateMonomial, coefficient>& other, coefficient* inputcf=0);
  template <class otherType>
  inline void operator/=(const otherType& other)
  { if (other==0)
    { crash << "This is a programming error. A MonomialCollection division by zero has been requested: division by zero error should"
      << " be handled before calling operator/=. " << crash;
      return;
    }
    for (int i=0; i<this->theCoeffs.size; i++)
      this->theCoeffs[i]/=other;
  }
  template <class otherType>
  inline void operator*=(const otherType& other)
  { //int commentmewhendone;
    //this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
    for (int i=0; i<this->theCoeffs.size; i++)
    { //int commentmewhendone;
      //this->theCoeffs[i].checkConsistency();
      ////
      this->theCoeffs[i]*=other;
      ////
      //this->theCoeffs[i].checkConsistency();
      if (i==0)
        if (this->theCoeffs[i].IsEqualToZero())//<- to avoid implicit conversion problems, we make the zero check at the first cycle
        { this->MakeZero();
          return;
        }
    }
  }
  inline const TemplateMonomial& operator[](int theIndex)const
  { return this->theMonomials[theIndex];
  }
  void operator=(const TemplateMonomial& other)
  { TemplateMonomial otherCopy=other; //in case other is contained as a monomial in me and gets destroyed when I call
    //this->MakeZero(). This shouldn't yield a measurable slowdown.
    //Further this function is to be used for non-time critical operations.
    this->MakeZero();
    this->AddMonomial(otherCopy, 1);
  }
  template<class otherType>
  void operator=(const MonomialCollection<TemplateMonomial, otherType>& other)
  { this->theCoeffs=other.theCoeffs;
    this->theMonomials=other.theMonomials;
//    int commentwhendone;
 //   this->checkConsistency();
  }
};

class MonomialVector
{
  friend std::ostream& operator << (std::ostream& output, const MonomialVector& theMon)
  { output << theMon.ToString();
    return output;
  }
  public:
  int theIndex;
  MonomialVector():theIndex(-1){}
  MonomialVector(int inputIndex):theIndex(inputIndex){}
  std::string ToString(FormatExpressions* theFormat=0)const;
  inline unsigned int HashFunction()const
  { return (unsigned) this->theIndex;
  }
  static inline unsigned int HashFunction(const MonomialVector& input)
  { return input.HashFunction();
  }
  bool operator==(const MonomialVector& other)const
  { return this->theIndex==other.theIndex;
  }
  void MakeEi(int inputIndex)
  { this->theIndex=inputIndex;
  }
  inline static bool IsEqualToZero()
  { return false;
  }
  bool operator>(const MonomialVector& other)const
  { return this->theIndex>other.theIndex;
  }
};

template <class coefficient>
class VectorSparse : public MonomialCollection<MonomialVector, coefficient>
{
  public:
  void MaKeEi(int NonZeroIndex, const coefficient& theCoeff=1)
  { this->MakeZero();
    MonomialVector theMon;
    theMon.MakeEi(NonZeroIndex);
    this->AddMonomial(theMon, theCoeff);
  }
};

template <class TemplateMonomial, class coefficient>
class ElementMonomialAlgebra: public MonomialCollection<TemplateMonomial, coefficient>
{
  public:
  void MultiplyBy
  (const ElementMonomialAlgebra<TemplateMonomial, coefficient>& other, ElementMonomialAlgebra<TemplateMonomial, coefficient>& output,
   ElementMonomialAlgebra<TemplateMonomial, coefficient>& bufferPoly, TemplateMonomial& bufferMon)const;
  void MultiplyBy
  (const TemplateMonomial& other, const coefficient& theCoeff, ElementMonomialAlgebra<TemplateMonomial, coefficient>& output,
   ElementMonomialAlgebra<TemplateMonomial, coefficient>& bufferPoly, TemplateMonomial& bufferMon)const;
  void MultiplyOnTheLeft(const ElementMonomialAlgebra<TemplateMonomial, coefficient>& standsOnTheLeft)
  { ElementMonomialAlgebra<TemplateMonomial, coefficient> tempMat, bufferPoly;
    TemplateMonomial bufferMon;
    standsOnTheLeft.MultiplyBy(*this, tempMat, bufferPoly, bufferMon);
    *this=tempMat;
  }
  void operator*=(const ElementMonomialAlgebra<TemplateMonomial, coefficient>& other)
  { ElementMonomialAlgebra<TemplateMonomial, coefficient> bufferPoly;
    TemplateMonomial bufferMon;
    this->MultiplyBy(other, *this, bufferPoly, bufferMon);
  }
  inline void operator*=(const coefficient& other)
  { this->::MonomialCollection<TemplateMonomial, coefficient>::operator*= (other);
  }
  void RaiseToPower(int d, ElementMonomialAlgebra<TemplateMonomial, coefficient>& output, const coefficient& theRingUniT);
  void RaiseToPower(int d)
  { if (d==1)
      return;
    ElementMonomialAlgebra<TemplateMonomial, coefficient> theOne;
    theOne.MakeConst(1);
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
};

template<class coefficient>
class Polynomial: public ElementMonomialAlgebra<MonomialP, coefficient>
{
public:
  friend std::iostream& operator << <coefficient>(std::iostream& output, const Polynomial<coefficient>& input);
  Polynomial(int x)
  { this->MakeConst(x, 0);
  }
  Polynomial(){}
  Polynomial(const Polynomial<coefficient>& other)
  { this->operator=(other);
  }
  static bool flagAnErrorHasOccuredTimeToPanic;
  //to avoid code::blocks parsing problems (remove when code::blocks learns to parse parent classes:
  void AddMonomial(const MonomialP& inputMon, const coefficient& inputCoeff)
  { this->::MonomialCollection<MonomialP, coefficient>::AddMonomial(inputMon, inputCoeff);
  }
  unsigned int HashFunction()const
  { return this->::MonomialCollection<MonomialP, coefficient>::HashFunction();
  }
  static inline unsigned int HashFunction(const Polynomial<coefficient>& input)
  { return input.HashFunction();
  }
  void MultiplyBy(const MonomialP& other, const coefficient& theCoeff)
  { Polynomial<coefficient> output;
    output.MakeZero();
    output.AddMonomial(other, theCoeff);
    *this*=output;
  }
  void MultiplyBy(const MonomialP& other, const coefficient& theCoeff, Polynomial<coefficient>& output)const
  { output=*this;
    output.MultiplyBy(other, theCoeff);
  }
  //////////////////////////////////////////////
  void AssignFloor()const
  {}
  void AssignMinPoly(const Matrix<coefficient>& input);
  void AssignMonomialWithExponent(Vector<Rational>& r, const coefficient& theCoeff=1)
  { MonomialP tempM=r;
    this->MakeZero(r.size);
    this->AddMonomial(r);
  }
  void GetConstantTerm(coefficient& output, const coefficient& theRingZero=0)const;
  coefficient GetConstantTerm(const coefficient& theRingZero=0)const
  { coefficient result;
    this->GetConstantTerm(result, theRingZero);
    return result;
  }
  static void GetValuesLagrangeInterpolandsAtConsecutivePoints
  (Vector<Rational>& inputConsecutivePointsOfInterpolation, Vector<Rational>& inputPointsOfEvaluation, Vectors<Rational>& outputValuesInterpolands);
  bool FactorMeOutputIsSmallestDivisor(Polynomial<Rational>& output, std::stringstream* comments);
  void Interpolate(const Vector<coefficient>& thePoints, const Vector<coefficient>& ValuesAtThePoints);
  bool FindOneVarRatRoots(List<Rational>& output);
  void GetCoeffInFrontOfLinearTermVariableIndex(int index, coefficient& output);
  void MakeMonomiaL(int LetterIndex, const Rational& Power, const coefficient& Coeff=1, int ExpectedNumVars=0);
  void MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff);
  void MakeDegreeOne(int NVar, int NonZeroIndex1, int NonZeroIndex2, const coefficient& coeff1, const coefficient& coeff2);
  void MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff1, const coefficient& ConstantTerm);
  void MakeLinPolyFromRootNoConstantTerm(const Vector<Rational> & r);
  void MakeLinPolyFromRootLastCoordConst(const Vector<Rational> & input);
  void MakePolyFromDirectionAndNormal(Vector<coefficient>& direction, Vector<coefficient>& normal, coefficient& Correction);
  bool IsOneVariablePoly(int* whichVariable=0)const;
  bool IsOneVariableNonConstPoly(int* whichVariable=0)const
  { int tempInt;
    if (whichVariable==0)
      whichVariable=&tempInt;
    if (!this->IsOneVariablePoly(whichVariable))
      return false;
    return *whichVariable!=-1;
  }
  Polynomial<coefficient> GetOne()const
  { Polynomial<coefficient> result;
    result.MakeOne();
    return result;
  }
  Polynomial<coefficient> GetZero()const
  { Polynomial<coefficient> result;
    result.MakeZero();
    return result;
  }
  Rational RationalValue()
  { Rational result;
    this->GetConstantTerm(result, 0);
    return result;
  }
  void MakeDeterminantFromSquareMatrix(const Matrix<Polynomial<coefficient> >& theMat);
  void MakeConst(const coefficient& theConst, int ExpectedNumVars=0)
  {// int commentGrandMasterChecksWhenDone=-1;
    //this->GrandMasterConsistencyCheck();
    this->MakeZero();
    //this->GrandMasterConsistencyCheck();
    MonomialP theConstMon;
    theConstMon.MakeOne(ExpectedNumVars);
    this->AddMonomial(theConstMon, theConst);
    //this->GrandMasterConsistencyCheck();
  }
  void MakeOne(int ExpectedNumVars=0);
  Rational ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy()
  { if (this->IsEqualToZero())
      return 1;
    Rational result=this->ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    if (this->theCoeffs[this->GetIndexMaxMonomialLexicographicLastVariableStrongest()].IsNegative())
    { *this*=-1;
      result*=-1;
    }
    return result;
  }
  void ScaleToIntegralNoGCDCoeffs();
  void TimesInteger(int a);
  void DivideBy(const Polynomial<coefficient>& inputDivisor, Polynomial<coefficient>& outputQuotient, Polynomial<coefficient>& outputRemainder)const;
  void DivideByConstant(const coefficient& r);
  void AddConstant(const coefficient& theConst);
  void ShiftVariableIndicesToTheRight(int VarIndexShift);
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  inline void SetDynamicSubtype(int newNumVars)
  { this->SetNumVariablesSubDeletedVarsByOne(newNumVars);
  }
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { int result=-1;
    for (int i=0; i<this->size; i++)
      result=MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    return result;
  }
  void ScaleToPositiveMonomials(MonomialP& outputScale);
  void DecreaseNumVariables(int increment, Polynomial<coefficient>& output);
  bool Substitution(const List<Polynomial<coefficient> >& TheSubstitution, const coefficient& theRingUnit=1, const coefficient& theRingZero=0);
  Rational TotalDegree()const;
  int TotalDegreeInt()const;
  bool IsEqualToOne()const
  { coefficient tempC;
    if (this->IsAConstant(&tempC))
      return tempC.IsEqualToOne();
    return false;
  }
  bool IsMonomialCoeffOne()const
  { if (this->size()!=1)
      return false;
    return this->theCoeffs[0].IsEqualToOne();
  }
  bool IsOneLetterFirstDegree(int* whichLetter=0)const
  { if(this->size()!=1)
      return false;
    return (*this)[0].IsOneLetterFirstDegree(whichLetter);
  }
  bool IsAConstant(coefficient* whichConstant=0)const
  { if (this->size()>1)
      return false;
    if (this->size()==0)
    { if (whichConstant!=0)
        *whichConstant=0;
      return true;
    }
    if (whichConstant!=0)
      *whichConstant=this->theCoeffs[0];
    const MonomialP& theMon=(*this)[0];
    return theMon.IsAConstant();
  }
  bool IsNegative()const
  { coefficient tempC;
    if(!this->IsAConstant(&tempC))
      return false;
    return tempC.IsNegative();
  }
  bool IsLinearNoConstantTerm()
  { for (int i=0; i<this->size; i++)
      if (!this->TheObjects[i].IsLinearNoConstantTerm())
        return false;
    return true;
  }
  bool IsLinear()
  { for (int i=0; i<this->size(); i++)
      if (!(*this)[i].IsLinear())
        return false;
    return true;
  }
  bool IsLinearGetRootConstantTermLastCoordinate(Vector<coefficient>& outputRoot, const coefficient& theZero)
  { outputRoot.MakeZero(this->GetMinNumVars()+1);
    int index;
    for (int i=0; i<this->size(); i++)
      if((*this)[i].IsAConstant())
        *outputRoot.LastObject()=this->theCoeffs[i];
      else
        if ((*this)[i].IsOneLetterFirstDegree(&index))
          outputRoot[index]=this->theCoeffs[i];
        else
          return false;
    return true;
  }
  void RaiseToPower(int d, const coefficient& theRingUniT=1)
  { if (d==1)
      return;
    if (d<0)
      crash << "This is a programming error: attempting to raise the polynomial " << this->ToString() << " to the negative power "
      << d << ". " << crash;
    Polynomial<coefficient> theOne;
    theOne.MakeOne(this->GetMinNumVars());
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
  inline bool GetRootFromLinPolyConstTermLastVariable(Vector<coefficient>& outputRoot, const coefficient& theZero= (coefficient) 0)
  { return this->IsLinearGetRootConstantTermLastCoordinate(outputRoot, theZero);
  }
  Matrix<coefficient> EvaluateUnivariatePoly(const Matrix<coefficient>& input);//<-for univariate polynomials only
  coefficient Evaluate(const Vector<coefficient>& input);
  bool IsProportionalTo(const Polynomial<coefficient>& other, coefficient& TimesMeEqualsOther, const coefficient& theRingUnit)const;
  void DrawElement(GlobalVariables& theGlobalVariables, DrawElementInputOutput& theDrawData, FormatExpressions& PolyFormatLocal);
  const MonomialP& GetMaxMonomial(List<MonomialP>::OrderLeftGreaterThanRight theMonOrder=0)const
  { return (*this)[this->GetIndexMaxMonomial(theMonOrder)];
  }
  int GetIndexMaxMonomial(List<MonomialP>::OrderLeftGreaterThanRight theMonOrder=0)const
  { if (this->size()==0)
      return -1;
    int result=0;
    for (int i=1; i<this->size(); i++)
    { if(theMonOrder!=0)
      { if (theMonOrder((*this)[i], (*this)[result]))
          result=i;
      } else
        if ((*this)[i]>(*this)[result])
          result=i;
    }
    return result;
  }
  int GetIndexMaxMonomialLexicographicLastVariableStrongest()const;
//  void ComponentInFrontOfVariableToPower(int VariableIndex, ListPointers<Polynomial<coefficient> >& output, int UpToPower);
  int GetMaxPowerOfVariableIndex(int VariableIndex);
  bool operator<=(const coefficient& other)const
  { coefficient constME;
    if (!this->IsAConstant(&constME))
    { crash << "This may or may not be a programming error: attempting to compare a non-constant polynomial to "
      << " a constant. I cannot judge at the moment whether allowing that is a good decision. In any case, crashing to let you know. "
      << crash;
      return false;
    }
    return constME<=other;
  }
  bool operator<(const coefficient& other)const
  { coefficient constME;
    if (!this->IsAConstant(&constME))
    { crash << "This may or may not be a programming error: attempting to compare a non-constant polynomial to "
      << " a constant. I cannot judge at the moment whether allowing  that is a good decision. In any case, crashing to let you know. "
      << crash;
      return false;
    }
    return constME<other;
  }

  bool operator>(const Polynomial<coefficient>& other)const
  { if (other.size()==0)
    { if (this->size()==0)
        return false;
      else
        return true;
    }
    if (this->size()==0)
      return false;
    if (this->TotalDegree()>other.TotalDegree())
      return true;
    if (this->TotalDegree()<other.TotalDegree())
      return false;
    int thisMaxMonIndex= this->GetIndexMaxMonomialLexicographicLastVariableStrongest();
    int otherMaxMonIndex= other.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    if((*this)[thisMaxMonIndex]>other[otherMaxMonIndex])
      return true;
    if(other[otherMaxMonIndex]>(*this)[thisMaxMonIndex])
      return false;
    if(this->theCoeffs[thisMaxMonIndex]>other.theCoeffs[otherMaxMonIndex])
      return true;
    if(other.theCoeffs[otherMaxMonIndex]>this->theCoeffs[thisMaxMonIndex])
      return false;
    return false;
  }
  bool operator<=(const Polynomial<coefficient>& other)const
  { return ! (*this>other);
  }
  //has to be rewritten please don't use!
  bool IsGreaterThanZeroLexicographicOrder();
  static bool IsGEQcompareByTopMonomialTotalDegThenLexicographic(const Polynomial<coefficient>& left, const Polynomial<coefficient>& right)
  { if (left.IsEqualToZero())
      return right.IsEqualToZero();
    if (right.IsEqualToZero())
      return true;
    return left[left.GetIndexMaxMonomialTotalDegThenLexicographic()].MonomialP::IsGEQTotalDegThenLexicographic
    (right[right.GetIndexMaxMonomialTotalDegThenLexicographic()]);
  }
  static bool IsGEQcompareByTopMonomialLexicographicLastVarStrongest(const Polynomial<coefficient>& left, const Polynomial<coefficient>& right)
  { if (left.IsEqualToZero())
      return right.IsEqualToZero();
    if (right.IsEqualToZero())
      return true;
    return left[left.GetIndexMaxMonomialLexicographicLastVariableStrongest()].MonomialP::IsGEQLexicographicLastVariableStrongest
    (right[right.GetIndexMaxMonomialLexicographicLastVariableStrongest()]);
  }
  bool IsEqualTo(const Polynomial<coefficient>& p)const
  { return *this==p;
  }
  void operator-=(int x)
  { MonomialP tempMon;
    tempMon.MakeOne();
    this->SubtractMonomial(tempMon, x);
  }
  void operator-=(const coefficient& other)
  { MonomialP tempMon;
    tempMon.MakeOne();
    this->SubtractMonomial(tempMon, other);
  }
  void operator-=(const Polynomial<coefficient>& other)
  { this->::MonomialCollection<MonomialP, coefficient>::operator-=(other);
  }
  void operator*=(const MonomialP& other)
  { Polynomial<coefficient> otherP;
    otherP.MakeZero();
    otherP.AddMonomial(other, 1);
    (*this)*=otherP;
  }
  void operator*=(const Polynomial<coefficient>& other)
  { this->::ElementMonomialAlgebra<MonomialP, coefficient>::operator*=(other);
  }
  Polynomial<coefficient> operator%(const Polynomial<coefficient>& other)
  { Polynomial<coefficient> temp;
    Polynomial<coefficient> result;
    this->DivideBy(other, temp, result);
    return result;
  }
  void operator/=(const Polynomial<coefficient>& other)
  { Polynomial<coefficient> tempMe=*this;
    Polynomial<coefficient> tempRemainder;
    tempMe.DivideBy(other, *this, tempRemainder);
  }
  void operator/=(int other)
  { this->::MonomialCollection<MonomialP, coefficient>::operator/= (other);
  }
  void operator/=(const coefficient& other)
  { this->::MonomialCollection<MonomialP, coefficient>::operator/= (other);
  }
  template <class otherType>
  inline void operator*=(const otherType& other)
  { this->::MonomialCollection<MonomialP, coefficient>::operator*= (other);
  }
  void operator=(const Polynomial<coefficient>& other)
  { this->::MonomialCollection<MonomialP, coefficient>::operator=(other);
  }
  template<class otherType>
  void operator=(const Polynomial<otherType>& other)
  { this->::MonomialCollection<MonomialP, coefficient>::operator=(other);
  }
  void operator=(const coefficient& other)
  { this->MakeConst(other);
  }
  void operator=(int other)
  { coefficient tempCF;
    tempCF=other;
    this->MakeConst(tempCF);
  }
  template <class otherType>
  void AssignOtherType(const Polynomial<otherType>& other)
  { this->NumVars=other.NumVars;
    this->::MonomialCollection<MonomialP, coefficient>::AssignOtherType(other);
  }
};

template <class Element>
class PolynomialSubstitution: public List<Polynomial<Element> >
{
  public:
  //One of the main purposes of this class is to be used for carrying out substitutions.
  //the general format of the substitution is:
  // the i^th element denotes the image of x_i,
  // For example, if polynomials is the array
  // x_1+x_2 (poly in 3 variables)
  // x_1x_3+2 (poly in 3 variables)
  // this mens that it can be applied to polynomials in two variables
  // like this: x_1-> x_1+x_2
  // x_2-> (x_1x_3+2)
  // to produce a polynomial in three variables
  void MakeIdSubstitution(int numVars, const Element& theRingUnit=1);
  void MakeIdLikeInjectionSub(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const Element& theRingUnit);
  //In the following function we have that:
  //the format of the linear substitution is:
  //theSub is a  whose number of rows minus 1 must equal the # number of
  //target variables and whose number of columns must equal the number of variables in
  //the current polynomial (this->NumVariables).
  //The first row denotes the constant term in the substitution of the respective variable!
  //An element in the x-th row and y-th column
  //is defined as Element[x][y] !
  void MakeExponentSubstitution(Matrix<LargeInt>& theSub);
  void PrintPolys(std::string& output, Element& TheRingUnit, Element& TheRingZero);
  void MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<Element>& EndPoint);
  void AddConstant(Element& theConst);
  void TimesConstant(Element& r);
  void DivideByConstant(Element& r);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { int result=-1;
    for (int i=0; i<this->size; i++)
      result=MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    return result;
  }
  void NullifyAll(int NumVars)
  { for (int i=0; i<this->size; i++)
      this->TheObjects[i].MakeZero((int)NumVars);
  }
  std::string ToString(int numDisplayedEltsMinus1ForAll=-1)const
  { std::string tempS;
    this->ToString(tempS, numDisplayedEltsMinus1ForAll);
    return tempS;
  }
  void ComputeDebugString();
  bool operator==(const PolynomialSubstitution& right);
  void Substitution(PolynomialSubstitution<Element>& theSub, int NumVarsTarget)
  { Polynomial<Rational>  tempP;
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].Substitution(theSub, tempP, NumVarsTarget, (Rational) 1);
      this->TheObjects[i]=(tempP);
    }
  }
  void MakeOneParameterSubFromDirection(Vector<Rational>& direction)
  { MonomialP tempM;
    tempM.MakeOne(1);
    tempM.monBody[0]=1;
    this->SetSize(direction.size);
    for (int i=0; i<this->size; i++)
    { this->TheObjects[i].MakeZero(1);
      this->TheObjects[i].AddMonomial(tempM,direction.TheObjects[i]);
    }
  }
  void MakeSubFromMatrixIntAndDen(Matrix<LargeInt>& theMat, LargeIntUnsigned& Den)
  { Matrix<Rational>  tempMat;
    tempMat.AssignMatrixIntWithDen(theMat, Den);
    this->MakeLinearSubConstTermsLastRow(tempMat);
  }
  void ToString(std::string& output, int numDisplayedEltsMinus1ForAll)const
  { std::stringstream out;
    output.clear();
    FormatExpressions PolyFormatLocal;
    if (numDisplayedEltsMinus1ForAll==-1)
      numDisplayedEltsMinus1ForAll=this->size;
    for (int i=0; i<this->size; i++)
    { out << "x_{" << i+1 << "} \\mapsto " << this->TheObjects[i].ToString(&PolyFormatLocal);
      if (i!=this->size-1)
        out << ", ";
    }
    out << ":   ";
    output=out.str();
  }
  void ComputeB(Polynomial<Rational> & output, int cutOffIndex, int theDimension)
  { PolynomialSubstitution EpsForm;
    Polynomial<Rational>  tempP;
    EpsForm.SetSize(theDimension);
    EpsForm.TheObjects[0]=(this->TheObjects[0]);
    for (int i=1; i<theDimension; i++)
    { EpsForm.TheObjects[i]=(this->TheObjects[i]);
      tempP=(this->TheObjects[i-1]);
      tempP*=(-1);
      EpsForm.TheObjects[i]+=(tempP);
    }
    EpsForm.TheObjects[3]+=(this->TheObjects[4]);
    for (int j=cutOffIndex+1; j<theDimension; j++)
      if (!EpsForm.TheObjects[j].IsGreaterThanZeroLexicographicOrder())
        EpsForm.TheObjects[j]*=(-1);
    output.MakeZero();
    for (int i=0; i<theDimension; i++)
      output+=(EpsForm.TheObjects[i]);
  }
  void MakeLinearSubConstTermsLastRow(Matrix<Element> & theMat);
  void MakeConstantShiftCoordinatesAreAdded(Vector<Rational> & shiftPerVariable);
  void MakeLinearSubbedVarsCorrespondToMatRows(Matrix<Rational> & theMat, Vector<Rational> & theConstants);
  void ComputeDiscreteIntegrationUpTo(int d);
  void MakeLinearSubOnLastVariable(int NumVars, Polynomial<Rational> & LastVarSub)
  { this->SetSize(NumVars);
    for (int i=0; i<NumVars-1; i++)
      this->TheObjects[i].MakeDegreeOne(NumVars, i, 1);
    this->TheObjects[NumVars-1]=LastVarSub;
  }
  void MakeSubNVarForOtherChamber(Vector<Rational> & direction, Vector<Rational> & normal, Rational& Correction, GlobalVariables& theGlobalVariables)
  { this->SetSize(direction.size);
    Polynomial<Rational> TempPoly;
    TempPoly.MakePolyFromDirectionAndNormal(direction, normal, Correction);
    for (int i=0; i<direction.size; i++)
    { Polynomial<Rational> TempPoly2;
      TempPoly2=TempPoly;
      TempPoly2*=direction[i];
      TempPoly2*=-1;
      this->TheObjects[i].MakeDegreeOne(direction.size, i, 1);
      this->TheObjects[i]+=TempPoly2;
    }
  }
  void MakeSubAddExtraVarForIntegration(Vector<Rational> & direction)
  { this->SetSize(direction.size);
    for (int i=0; i<direction.size; i++)
    { Rational tempRat; tempRat.Assign(direction.TheObjects[i]); tempRat.Minus();
      this->TheObjects[i].MakeDegreeOne((int)(direction.size+1), i, direction.size, 1, tempRat);
    }
  }
};

//std::iostream& operator<<(std::iostream& output, const RationalFunctionOld& theRF);
//std::iostream& operator>>(std::iostream& input, RationalFunctionOld& theRF);

template<class coefficient>
class GroebnerBasisComputation
{
  public:
  List<MonomialP>::OrderLeftGreaterThanRight theMonOrdeR;
  Polynomial<coefficient> SoPolyBuf;
  Polynomial<coefficient> remainderDivision;
  Polynomial<coefficient> bufPoly;
  Polynomial<coefficient> bufPolyForGaussianElimination;
  MonomialP SoPolyLeftShift;
  MonomialP SoPolyRightShift;
  MonomialP bufferMoN1;
  List<Polynomial<coefficient> > theBasiS;
  List<Polynomial<coefficient> > basisCandidates;
  List<MonomialP> leadingMons;
  List<coefficient> leadingCoeffs;
  int NumberSerreSystemComputations;
  int MaxNumSerreSystemComputations;
  int NumberGBComputations;
  int MaxNumGBComputations;
  int RecursionCounterSerreLikeSystem;
  bool flagBasisGuaranteedToGenerateIdeal;
  bool flagDoProgressReport;
  bool flagDoSortBasis;
  bool flagDoLogDivision;
  bool flagSystemProvenToHaveNoSolution;
  bool flagSystemProvenToHaveSolution;
  bool flagSystemSolvedOverBaseField;
  MemorySaving<List<Polynomial<coefficient> > > intermediateRemainders;
  MemorySaving<List<List<MonomialP> > > intermediateHighlightedMons;
  MemorySaving<List<MonomialP> > intermediateHighestMonDivHighestMon;
  MemorySaving<List<coefficient> > intermediateCoeffs;
  MemorySaving<List<Polynomial<coefficient> > > intermediateSubtractands;
  MemorySaving<List<int> > intermediateSelectedDivisors;
  MemorySaving<Polynomial<coefficient> > startingPoly;
  MemorySaving<List<coefficient> > systemSolution;
  MemorySaving<Selection> solutionsFound;
  void SetSerreLikeSolutionIndex(int theIndex, const coefficient& theConst);
  void GetSubFromPartialSolutionSerreLikeSystem(PolynomialSubstitution<coefficient>& outputSub);
  std::string ToStringSerreLikeSolution(FormatExpressions* theFormat=0);
  std::string GetPolynomialStringSpacedMonomials
  (const Polynomial<coefficient>& thePoly, const HashedList<MonomialP>& theMonomialOrder, const std::string& extraStyle,
   const std::string& extraHighlightStyle, FormatExpressions* theFormat=0, List<MonomialP>* theHighLightedMons=0);
  std::string GetDivisionString(FormatExpressions* theFormat=0);
  bool AddPolyAndReduceBasis(GlobalVariables* theGlobalVariables);
  bool TransformToReducedGroebnerBasis(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables=0);
  bool TransformToReducedGroebnerBasisImprovedAlgorithm(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables=0, int upperComputationBound=-1);
  GroebnerBasisComputation();
  void MakeMinimalBasis();
  static int GetNumEquationsThatWouldBeLinearIfIsubbedVar(int theVarIndex, List<Polynomial<coefficient> >& input);
  static int GetNumVarsToSolveFor(const List<Polynomial<coefficient> >& input);
  static void GetVarsToSolveFor(const List<Polynomial<coefficient> >& input, Selection& output);
  static bool IsContradictoryReducedSystem(const List<Polynomial<coefficient> >& input);
  void RemainderDivisionWithRespectToBasis
  (Polynomial<coefficient>& inputOutput, Polynomial<coefficient>* outputRemainder=0, GlobalVariables* theGlobalVariables=0, int basisIndexToIgnore=-1);
  static std::string GetCalculatorInputFromSystem(const List<Polynomial<coefficient> >& inputSystem);
  void SolveSerreLikeSystem(List<Polynomial<coefficient> >& inputSystem, AlgebraicClosureRationals* theAlgebraicClosure=0, GlobalVariables* theGlobalVariables=0);
  bool HasImpliedSubstitutions
  (List<Polynomial<coefficient> >& inputSystem, PolynomialSubstitution<coefficient>& outputSub, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables)
  ;
  int GetPreferredSerreSystemSubIndex(List<Polynomial<coefficient> >& inputSystem);
  void SolveSerreLikeSystemRecursively(List<Polynomial<coefficient> >& inputSystem, AlgebraicClosureRationals* theAlgebraicClosure, GlobalVariables* theGlobalVariables);
  void BackSubstituteIntoPolySystem(List<PolynomialSubstitution<coefficient> >& theImpliedSubs, GlobalVariables* theGlobalVariables);
  void BackSubstituteIntoSinglePoly(Polynomial<coefficient>& thePoly, int theIndex, PolynomialSubstitution<coefficient>& theFinalSub, GlobalVariables* theGlobalVariables);
  bool AddRemainderToBasis(GlobalVariables* theGlobalVariables);
  bool GetOneVarPolySolution(const Polynomial<coefficient>& thePoly, coefficient& outputSolution, AlgebraicClosureRationals& theAlgebraicClosure, GlobalVariables* theGlobalVariables=0);
   //Criterion from Cox, Little, O'Shea:
  static bool CriterionCLOsh
  (HashedListSpecialized<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >& thePairs,
   List<MonomialP>& theLeadingMons, MonomialP& leadingTermLCM);
  void CheckConsistency();
  void initForGroebnerComputation(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables);
  void initForDivisionAlone(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables);
};

class RationalFunctionOld
{
private:
  void AddSameTypes(const RationalFunctionOld& other)
  { switch (this->expressionType)
    { case RationalFunctionOld::typeRational: this->ratValue+=other.ratValue; break;
      case RationalFunctionOld::typePoly: this->Numerator.GetElement()+=other.Numerator.GetElementConst(); break;
      case RationalFunctionOld::typeRationalFunction: this->AddHonestRF(other); break;
    }
    this->ReduceMemory();
    if(!this->checkConsistency())
      crash << crash;
  }
  void AddHonestRF(const RationalFunctionOld& other);
  void ReduceRFToPoly()
  { if (this->expressionType==this->typeRationalFunction)
    { if (this->Denominator.GetElement().IsAConstant())
      { this->Numerator.GetElement()/=this->Denominator.GetElement().theCoeffs[0];
        this->Denominator.FreeMemory();
        this->expressionType=this->typePoly;
      }
      if (this->Numerator.GetElement().IsEqualToZero())
        this->MakeZero(this->context);
    }
  }
  void ReducePolyToRational()
  { if (this->expressionType==this->typePoly)
      if(this->Numerator.GetElement().IsAConstant())
      { this->expressionType=this->typeRational;
        if (this->Numerator.GetElement().IsEqualToZero())
          this->ratValue.MakeZero();
        else
          this->ratValue=this->Numerator.GetElement().theCoeffs[0];
        this->Numerator.FreeMemory();
      }
//    int commentMEWhenDone;
//    if(!this->checkConsistency())
//      crash << crash;
  }
  bool ConvertToType(int theType);
public:
  friend std::ostream& operator<< (std::ostream& output, const RationalFunctionOld& theRF)
  { output << theRF.ToString();
    return output;
  }
  friend std::istream& operator>> (std::istream& input, RationalFunctionOld& theRF);

  MemorySaving<Polynomial<Rational> > Numerator;
  MemorySaving<Polynomial<Rational> > Denominator;
  GlobalVariables* context;
  Rational ratValue;
  int expressionType;
  enum typeExpression{ typeRational=0, typePoly=1, typeRationalFunction=2, typeError=3};
  std::string ToString(FormatExpressions* theFormat=0)const;
  bool NeedsBrackets()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational: return false;
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst().NeedsBrackets();
      case RationalFunctionOld::typeRationalFunction: return false;
    }
    return false;
  }
  bool FindOneVarRatRoots(List<Rational>& output)
  { if (this->expressionType==this->typeRational)
    { output.SetSize(0);
      return true;
    }
    Polynomial<Rational> tempP;
    this->GetNumerator(tempP);
    return tempP.FindOneVarRatRoots(output);
  }
  Rational GetDenominatorRationalPart()
  { Polynomial<Rational> Num;
    this->GetNumerator(Num);
    return Num.FindLCMCoefficientDenominatorsOverRationals();
  }
  Rational GetNumeratorRationalPart()
  { Polynomial<Rational> Num;
    this->GetNumerator(Num);
    return Num.FindGCDCoefficientNumeratorsOverRationals();
  }
  RationalFunctionOld GetOne()const
  { RationalFunctionOld tempRat;
    tempRat.MakeConst(1, this->context);
    return tempRat;
  }
  RationalFunctionOld GetZero()const
  { RationalFunctionOld tempRat;
    tempRat.MakeZero(this->context);
    return tempRat;
  }
  int GetMinNumVars()const;
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  RationalFunctionOld(const RationalFunctionOld& other): context(0), expressionType(RationalFunctionOld::typeError)
  { this->operator=(other);
  }
  RationalFunctionOld()
  { this->expressionType=this->typeError;
    this->ratValue.MakeZero();
    this->context=0;
  }
  RationalFunctionOld(int other)
  { this->expressionType=this->typeRational;
    this->context=0;
    this->operator=(other);
  }
  RationalFunctionOld(const Rational& other)
  { this->expressionType=this->typeRational;
    this->context=0;
    this->operator=(other);
  }
  Rational RationalValue()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational:  return this->ratValue;
      case RationalFunctionOld::typeError: return 0;
      default:
        return this->Numerator.GetElementConst().GetConstantTerm();
    }
  }
  void RaiseToPower(int thePower);
//  RationalFunctionOld(const RationalFunctionOld& other){this->NumVars=0; this->expressionType=this->typeRational; this->ratValue.MakeZero(); this->operator=(other);}
  void ReduceMemory()
  { this->ReduceRFToPoly();
    this->ReducePolyToRational();
    if(!this->checkConsistency())
      crash << crash;
  }
  void operator=(const Polynomial<Rational> & other)
  { this->expressionType=this->typePoly;
    this->Numerator.GetElement()=other;
    this->ReduceMemory();
  }
  inline unsigned int HashFunction()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational:
        return this->ratValue.HashFunction();
      case RationalFunctionOld::typePoly:
        return this->Numerator.GetElementConst().HashFunction();
      case RationalFunctionOld::typeRationalFunction:
        return this->Numerator.GetElementConst().HashFunction()*SomeRandomPrimes[0]+this->Denominator.GetElementConst().HashFunction()*SomeRandomPrimes[1];
      default:
        return (unsigned)-1;
    }
  }
  static inline unsigned int HashFunction(const RationalFunctionOld& input)
  { return input.HashFunction();
  }
  inline void operator=(int other)
  { this->MakeConst(other, 0);
  }
  inline void operator=(const Rational& other)
  { this->MakeConst(other, 0);
  }
  void operator=(const RationalFunctionOld& other);
  bool checkConsistency()const;
  inline void SetNumVariables(int GoalNumVars)
  { this->SetNumVariablesSubDeletedVarsByOne(GoalNumVars);
  }
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  void MakeOneLetterMoN(int theIndex, const Rational& theCoeff, GlobalVariables& theGlobalVariables, int ExpectedNumVars=0);
  void GetNumerator(Polynomial<Rational>& output)const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational: output.MakeConst(this->ratValue); return;
      default: output=this->Numerator.GetElementConst(); return;
    }
  }
  bool IsNegative()
  { if (this->expressionType==this->typeRational)
      return this->ratValue.IsNegative();
    return false;
  }
  void GetDenominator(Polynomial<Rational>& output)const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRationalFunction:
      if (this->Denominator.IsZeroPointer())
        crash << "This is a programming error: the rational function is supposed to be honest, but the denominator pointer is zero. " << crash;
      output=this->Denominator.GetElementConst();
      return;
      default: output.MakeConst((Rational) 1); return;
    }
  }
  void ClearDenominators
  (RationalFunctionOld& outputWasMultipliedBy)
  ;
  void operator+=(const Polynomial<Rational>& other)
  { RationalFunctionOld tempOther;
    tempOther=other;
    *this+=tempOther;
  }
  void operator+=(const RationalFunctionOld& other);
  inline bool operator==(int other)const
  { if (other==0)
      return this->IsEqualToZero();
    else
      return this->expressionType==this->typeRational && (this->ratValue==other);
  }
  inline bool operator!=(int other)
  { return !(this->operator==(other));
  }
  inline bool operator==(const RationalFunctionOld& other)const
  { return this->IsEqualTo(other);
  }
  void Simplify();
  void SimplifyLeadingCoefficientOnly();
  void operator+=(int theConstant)
  { RationalFunctionOld tempRF;
    tempRF.MakeConst((Rational) theConstant, this->context);
    (*this)+=tempRF;
  }
  void operator*=(const RationalFunctionOld& other);
  void operator*=(const Polynomial<Rational>& other);
  void operator*=(const MonomialP& other);
  void operator*=(const Rational& other);
  void operator*=(int other)
  { *this*=(Rational)other;
  }
//  void operator-=(const Rational& theConstant)
//  { RationalFunctionOld tempRF;
//    tempRF.MakeConst(this->NumVars, -theConstant, this->context);
//    (*this)+=tempRF;
//  }
  bool operator<(const RationalFunctionOld& other)const
  { return other>*this;
  }
  bool operator<=(const RationalFunctionOld& other)const
  { if (*this==other)
      return true;
    return other>*this;
  }
  bool operator>(const RationalFunctionOld& other)const
  { if (this->expressionType<other.expressionType)
      return false;
    if (this->expressionType>other.expressionType)
      return true;
    switch(this->expressionType)
    { case RationalFunctionOld::typeRational:  return this->ratValue>other.ratValue;
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst()>other.Numerator.GetElementConst();
      case RationalFunctionOld::typeRationalFunction:
        if (other.Denominator.GetElementConst()>this->Denominator.GetElementConst())
          return true;
        if (this->Denominator.GetElementConst()>other.Denominator.GetElementConst())
          return false;
        return this->Numerator.GetElementConst()>other.Numerator.GetElementConst();
      default: return false;
    }
  }
  inline void TimesConstant(const Rational& theConst)
  { this->operator*=(theConst);
  }
  void Invert();
  void MakeOne(GlobalVariables* theContext)
  { this->MakeConst(1, theContext);
  }
  void MakeZero(GlobalVariables* theContext=0)
  { this->expressionType=this->typeRational;
    this->ratValue.MakeZero();
    if (theContext!=0)
      this->context=theContext;
    this->Numerator.FreeMemory();
    this->Denominator.FreeMemory();
    if(!this->checkConsistency())
      crash << crash;
  }
  void MakeConst(const Rational& theCoeff, GlobalVariables* theContext)
  { this->MakeZero(theContext);
    this->ratValue=theCoeff;
  }
  bool IsConstant(Rational* whichConstant)const
  { if (this->expressionType!=this->typeRational)
      return false;
    if (whichConstant!=0)
      *whichConstant=this->ratValue;
    return true;
  }
  bool IsInteger()const
  { return this->expressionType==this->typeRational && this->ratValue.IsInteger();
  }
  bool IsSmallInteger(int* whichInteger=0)const
  { return this->expressionType==this->typeRational &&
    this->ratValue.IsSmallInteger(whichInteger);
  }
  bool IsEqualToZero()const
  { return this->expressionType==this->typeRational && this->ratValue.IsEqualToZero();
  }
  bool IsEqualToOne()const
  { return this->expressionType==this->typeRational && this->ratValue.IsEqualToOne();
  }
  bool IsEqualTo(const RationalFunctionOld& other)const
  { if (this->expressionType!=other.expressionType)
      return false;
    switch(this->expressionType)
    { case RationalFunctionOld::typeRationalFunction:  return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst()) && this->Denominator.GetElementConst().IsEqualTo(other.Denominator.GetElementConst());
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst());
      case RationalFunctionOld::typeRational: return this->ratValue==other.ratValue;
    }
    crash << crash;
    return false;
  }
  static void GetRelations(List<Polynomial<Rational> >& theGenerators, GlobalVariables& theGlobalVariables);
  static bool gcdQuicK(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output);
  static void ScaleClearDenominator(List<RationalFunctionOld>& input, Vector<Polynomial<Rational> >& output);
  static void gcd(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output, GlobalVariables* theGlobalVariables=0);
  static void lcm(const Polynomial<Rational>& left, const Polynomial<Rational>& right,
  Polynomial<Rational>& output, GlobalVariables* theGlobalVariables=0);
  inline void operator-=(int other)
  { *this-=(Rational) other;
  }
  inline void operator-=(const Rational& other)
  { if(!this->checkConsistency())
      crash << crash;
    RationalFunctionOld tempRF;
    tempRF.MakeConst(other, this->context);
    tempRF.Minus();
    this->operator+=(tempRF);
    if(!(this->checkConsistency()))
      crash << crash;
  }
  inline void operator-=(const RationalFunctionOld& other)
  { if (!this->checkConsistency())
      crash << crash;
    if(!other.checkConsistency())
      crash << crash;
    if (other.context!=0)
      this->context=other.context;
    RationalFunctionOld tempRF;
    tempRF=other;
    tempRF.Minus();
    this->operator+=(tempRF);
    if (!this->checkConsistency())
      crash << crash;
  }
  inline void operator/=(int other)
  { RationalFunctionOld tempRF;
    tempRF.MakeConst(other, this->context);
    *this/=tempRF;
  }
  inline void operator/=(const Polynomial<Rational>& other)
  { RationalFunctionOld tempRF;
    tempRF=other;
    tempRF.Invert();
    *this*=(tempRF);
    if(!this->checkConsistency())
      crash << crash;
  }
  void operator/=(const RationalFunctionOld& other);
  void Minus()
  { this->operator*=((Rational) -1);
    if(!this->checkConsistency())
      crash << crash;
  }
};

template <class TemplateMonomial, class coefficient>
inline bool MonomialCollection<TemplateMonomial, coefficient>::operator==(int x)const
{ if (x==0)
    return this->size()==0;
  std::cout << "This is either a programming error, or an unforeseen use of operator==. If the second is the case, an audit/careful "
  << "proofreading of the code calling this function is needed; I am crashing just in case. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  return false;
}

template <class TemplateMonomial, class coefficient>
inline bool MonomialCollection<TemplateMonomial, coefficient>::operator==(const MonomialCollection<TemplateMonomial, coefficient>& other)const
{ if (this->theCoeffs.size!=other.theCoeffs.size)
    return false;
  for (int i=0; i<this->size(); i++)
  { int indexInOther=other.theMonomials.GetIndex((*this)[i]);
    if (indexInOther==-1)
      return false;
    if (!(this->theCoeffs[i]==other.theCoeffs[indexInOther]))
      return false;
  }
  return true;
}

template <class TemplateMonomial, class coefficient>
void MonomialCollection<TemplateMonomial, coefficient>::WriteToFile(std::fstream& output)
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << " numMons: " << this->size();
  for (int i=0; i<this->size(); i++)
  { output << " ";
    if (i>0)
      if (! this->theCoeffs[i].BeginsWithMinus())
        output << "+ ";
    this->theCoeffs[i].WriteToFile(output);
    const TemplateMonomial& tempM=(*this)[i];
    tempM.WriteToFile(output);
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class TemplateMonomial, class coefficient>
inline bool MonomialCollection<TemplateMonomial, coefficient>::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ int numReadWords, targetSize;
  XML::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string ReaderString;
  bool result=true;
  input >> ReaderString >> targetSize;
  if (ReaderString!="numMons:" )
  { crash << crash;
    return false;
  }
  this->MakeZero();
  TemplateMonomial tempM;
  this->SetExpectedSize(targetSize);
  input.ignore();
  coefficient theCoeff;
  for (int i=0; i<targetSize; i++)
  { if (input.peek()=='+')
      input >> ReaderString;
//    input.ignore();
    theCoeff.ReadFromFile(input);
    tempM.ReadFromFile(input);
    if (!result)
      break;
    this->AddMonomial(tempM, theCoeff);
  }
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  if(numReadWords!=0)
    crash << crash;
  return result;
}

template <class TemplateMonomial, class coefficient>
inline void ElementMonomialAlgebra<TemplateMonomial, coefficient>::MultiplyBy
(const ElementMonomialAlgebra<TemplateMonomial, coefficient>& other, ElementMonomialAlgebra<TemplateMonomial, coefficient>& output,
 ElementMonomialAlgebra<TemplateMonomial, coefficient>& bufferPoly, TemplateMonomial& bufferMon)const
{ if (other.IsEqualToZero())
  { output.MakeZero();
    return;
  }
  int maxNumMonsFinal=this->size()*other.size();
  bufferPoly.SetExpectedSize(maxNumMonsFinal);
  coefficient theCoeff;
  for (int i=0; i<other.size(); i++)
    for (int j=0; j<this->size(); j++)
    { bufferMon=(*this)[j];
      bufferMon*=(other[i]);
      theCoeff=this->theCoeffs[j];
      theCoeff*=other.theCoeffs[i];
      bufferPoly.AddMonomial(bufferMon, theCoeff);
      ParallelComputing::SafePointDontCallMeFromDestructors();
    }
  output=bufferPoly;
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::IsEqualToZero()const
{ return this->size()==0;
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::IsInteger(LargeInt* whichInteger)const
{ if (this->size()>1)
    return false;
  if(this->size()==0)
  { if (whichInteger!=0)
      *whichInteger=0;
    return true;
  }
  bool result= (*this)[0].IsAConstant();
  if (result)
    result=this->theCoeffs[0].IsInteger(whichInteger);
  return result;
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::IsSmallInteger(int* whichInteger)const
{ if (this->size()>1)
    return false;
  if(this->size()==0)
  { if (whichInteger!=0)
      *whichInteger=0;
    return true;
  }
  bool result= (*this)[0].IsAConstant();
  if (result)
    result=this->theCoeffs[0].IsSmallInteger(whichInteger);
  return result;
}

template <class Element>
void Polynomial<Element>::MakeLinPolyFromRootLastCoordConst(const Vector<Rational>& input)
{ this->MakeZero();
  MonomialP tempM;
  for (int i=0; i<input.size-1; i++)
  { tempM.MakeEi(i);
    this->AddMonomial(tempM, input[i]);
  }
  this->operator+=(*input.LastObject());
}

template <class Element>
void Polynomial<Element>::MakeLinPolyFromRootNoConstantTerm(const Vector<Rational>& r)
{ this->MakeZero();
  MonomialP tempM;
  for (int i=0; i<r.size; i++)
  { tempM.MakeEi(i);
    if (!r[i].IsInteger())
    { crash << "This may or may not be a programming error: requesting a monomial with non-integer exponent. "
      << "It appears this operation should be allowed, however this requires further consideration. I am crashing just in case. "
      << crash;
    }
    this->AddMonomial(tempM, r[i].GetNumerator());
  }
}

template <class TemplateMonomial, class coefficient>
void MonomialCollection<TemplateMonomial, coefficient>::SubtractOtherTimesCoeff(const MonomialCollection<TemplateMonomial, coefficient>& other, coefficient* inputcf)
{ if (this==&other)
  { if (inputcf==0)
    { this->MakeZero();
      return;
    }
    MonomialCollection<TemplateMonomial, coefficient> otherNew=other;
    this->SubtractOtherTimesCoeff(otherNew, inputcf);
    return;
  }
  this->SetExpectedSize(other.size()+this->size());
  coefficient tempCF;
  for (int i=0; i<other.size(); i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    tempCF=other.theCoeffs[i];
    if (inputcf!=0)
      tempCF*=*inputcf;
    this->SubtractMonomial(other[i], tempCF);
  }
}

template <class TemplateMonomial, class coefficient>
void MonomialCollection<TemplateMonomial, coefficient>::AddOtherTimesConst(MonomialCollection<TemplateMonomial, coefficient>& other, const coefficient& theConst)
{ this->SetExpectedSize(other.size()+this->size());
  coefficient tempCF;
  for (int i=0; i<other.size(); i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    tempCF=theConst;
    tempCF*=other.theCoeffs[i];
    this->AddMonomial(other[i], tempCF);
  }
}

template <class TemplateMonomial, class coefficient>
void MonomialCollection<TemplateMonomial, coefficient>::operator+=(const MonomialCollection<TemplateMonomial, coefficient>& other)
{ this->SetExpectedSize(other.size()+this->size());
  for (int i=0; i<other.size(); i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    this->AddMonomial(other[i], other.theCoeffs[i]);
  }
}

template <class TemplateMonomial, class Element>
bool MonomialCollection<TemplateMonomial, Element>::HasGEQMonomial(TemplateMonomial& m, int& WhichIndex)
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IsGEQpartialOrder(m))
    { WhichIndex=i;
      return true;
    }
  WhichIndex=-1;
  return false;
}

template <class TemplateMonomial, class coefficient>
template <class MonomialCollectionTemplate>
void MonomialCollection<TemplateMonomial, coefficient>::GaussianEliminationByRows
(List<MonomialCollectionTemplate>& theList, bool *IvemadeARowSwitch, HashedList<TemplateMonomial>* seedMonomials, Matrix<coefficient>* carbonCopyMatrix, List<MonomialCollectionTemplate>* carbonCopyList)
{ MacroRegisterFunctionWithName("MonomialCollection::GaussianEliminationByRows");
  if (carbonCopyMatrix!=0)
    if (carbonCopyMatrix->NumRows!=theList.size)
    { crash << "This is a programming error: carbon copy matrix has " << carbonCopyMatrix->NumRows << " rows, while the gaussian-eliminated list has " << theList.size
      << " elements; the two numbers must be the same!" << crash;
    }
  if (carbonCopyList!=0)
    if (carbonCopyList->size!=theList.size)
      crash << "This is a programming error: carbon copy list has " << carbonCopyList->size << " elements, while the gaussian-eliminated list has "
      << theList.size << " elements; the two numbers must be the same!" << crash;
  MemorySaving<HashedList<TemplateMonomial> > bufferMons;
  HashedList<TemplateMonomial>& allMons = seedMonomials==0 ? bufferMons.GetElement() : *seedMonomials;
  if (seedMonomials==0)
  { int topBoundNumMons=0;
    for (int i=0; i<theList.size; i++)
      topBoundNumMons+=theList[i].size();
    allMons.SetExpectedSize(topBoundNumMons);
  }
  for (int i=0; i<theList.size; i++)
    allMons.AddOnTopNoRepetition(theList[i].theMonomials);
  allMons.QuickSortAscending();
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
//  std::cout << "<hr>Gaussian elimnation. All mons(" << allMons.size << " total): "
//  << allMons.ToString(&tempFormat);
  tempFormat.flagUseLatex=true;
  if (IvemadeARowSwitch!=0)
    *IvemadeARowSwitch=false;
//  std::cout << "<br><b>starting list:</b> ";
//  for (int i=0; i<theList.size; i++)
//    std::cout << //"<br>" << CGI::GetMathSpanPure
//    (theList[i].ToString(&tempFormat)) << ", ";
  int currentRowIndex=0;
  coefficient tempCF, tempCF2;
  for (int i=0; i<allMons.size && currentRowIndex<theList.size; i++)
  { const TemplateMonomial& currentMon=allMons[i];
    int goodRow=currentRowIndex;
    for (; goodRow<theList.size; goodRow++)
      if (theList[goodRow].theMonomials.Contains(currentMon))
        break;
    if (goodRow>=theList.size)
      continue;
    if (currentRowIndex!=goodRow)
    { theList.SwapTwoIndices(currentRowIndex, goodRow);
      if (carbonCopyList!=0)
        carbonCopyList->SwapTwoIndices(currentRowIndex, goodRow);
      if (carbonCopyMatrix!=0)
        carbonCopyMatrix->SwitchTwoRows(currentRowIndex, goodRow);
      if (IvemadeARowSwitch!=0)
        *IvemadeARowSwitch=true;
    }
    MonomialCollection<TemplateMonomial, coefficient>& currentPivot= theList[currentRowIndex];
    int colIndex=currentPivot.theMonomials.GetIndex(currentMon);
    if (colIndex==-1)
      crash << "This is a programming error. An internal check at the Gaussian elimination method for monomial collections fails. "
      << "Something is wrong. Here is the List you wanted to perform Gaussian elimination upon. " << theList.ToString() << ". " << crash;
    tempCF=currentPivot.theCoeffs[colIndex];
    tempCF.Invert();
    currentPivot*=tempCF;
    if (carbonCopyMatrix!=0)
      carbonCopyMatrix->RowTimesScalar(currentRowIndex, tempCF);
    if (carbonCopyList!=0)
      (*carbonCopyList)[currentRowIndex]*=tempCF;
    for (int j=0; j<theList.size; j++)
      if (j!=currentRowIndex)
      { MonomialCollection<TemplateMonomial, coefficient>& currentOther=theList[j];
        int otherColIndex=currentOther.theMonomials.GetIndex(currentMon);
        if (otherColIndex!=-1)
        { tempCF=currentOther.theCoeffs[otherColIndex];
          //std::cout << "<br>subtracting " << CGI::GetMathSpanPure(currentPivot.ToString())
            //<< " times " << tempCF.ToString() << " from "
            //<< CGI::GetMathSpanPure(currentOther.ToString());
          currentOther.SubtractOtherTimesCoeff(currentPivot, &tempCF);
          if (carbonCopyList!=0)
            (*carbonCopyList)[j].SubtractOtherTimesCoeff((*carbonCopyList)[currentRowIndex], &tempCF);
          if (carbonCopyMatrix!=0)
          { tempCF2=tempCF;
            tempCF2*=-1;
            carbonCopyMatrix->AddTwoRows(currentRowIndex, j, 0, tempCF2);
          }
          //std::cout << "<br>to get " << CGI::GetMathSpanPure(currentOther.ToString());
        }
      }
    currentRowIndex++;
  }
//    std::cout << "<br><b>final list:</b> ";
//  for (int i=0; i<theList.size; i++)
//    std::cout << //"<br>" << CGI::GetMathSpanPure
//    (theList[i].ToString(&tempFormat)) << ", ";
}

template <class TemplateMonomial, class coefficient>
int MonomialCollection<TemplateMonomial, coefficient>::AddMonomialNoCoeffCleanUpReturnsCoeffIndex(const TemplateMonomial& inputMon, const coefficient& inputCoeff)
{ ///
//  this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
  ///
  if (inputCoeff==0 || inputMon.IsEqualToZero())
    return -1;
  int j= this->theMonomials.GetIndex(inputMon);
  if (j>=this->size())
  { crash.theCrashReport << "This is a programming error: function GetIndex evaluated on " << inputMon << " with hash function " << inputMon.HashFunction(inputMon)
    << " returns index " << j << " but I have only " << this->size() << " elements ";
    crash << crash;
  }
  if (j==-1)
  { this->theMonomials.AddOnTop(inputMon);
    this->theCoeffs.AddOnTop(inputCoeff);
    j=this->size()-1;
  } else
  { ///
//    this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
    if (j>=this->theCoeffs.size)
      crash << "This is a programming error. Looking for coefficient index " << j << " when number of coefficients is "
      << this->theCoeffs.size <<  ". " << crash;
    ///
    this->theCoeffs[j]+=inputCoeff;
  }
  return j;
}

template <class TemplateMonomial, class coefficient>
int MonomialCollection<TemplateMonomial, coefficient>::SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(const TemplateMonomial& inputMon, const coefficient& inputCoeff)
{ if (inputCoeff.IsEqualToZero() || inputMon.IsEqualToZero())
    return -1;
  int j= this->theMonomials.GetIndex(inputMon);
  if (j==-1)
  { this->theMonomials.AddOnTop(inputMon);
    this->theCoeffs.AddOnTop(inputCoeff);
    *this->theCoeffs.LastObject()*=-1;
    j=this->theCoeffs.size-1;
  } else
    this->theCoeffs[j]-=inputCoeff;
  return j;
}

template <class TemplateMonomial, class coefficient>
void ElementMonomialAlgebra<TemplateMonomial, coefficient>::RaiseToPower(int d, ElementMonomialAlgebra<TemplateMonomial, coefficient>& output, const coefficient& theRingUniT)
{ ElementMonomialAlgebra<TemplateMonomial, coefficient> tempOne;
  tempOne.MakeConst(theRingUniT);
  output=*this;
  MathRoutines::RaiseToPower(output, d, tempOne);
}

template <class Element>
void PolynomialSubstitution<Element>::PrintPolys(std::string &output, Element& TheRingUnit, Element& TheRingZero)
{ std::stringstream out;
  FormatExpressions PolyFormatLocal;
  for (int i=0; i<this->size; i++)
  { std::string tempS;
    out << i << ". ";
    this->TheObjects[i]->StringPrintOutAppend(tempS, PolyFormatLocal);
    out << tempS << "\r\n";
  }
  output=out.str();
}

template <class Element>
void PolynomialSubstitution<Element>::MakeIdSubstitution(int numVars, const Element& theRingUnit)
{ this->MakeIdLikeInjectionSub(numVars, numVars, theRingUnit);
}

template <class Element>
void PolynomialSubstitution<Element>::MakeIdLikeInjectionSub(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const Element& theRingUnit)
{ if(numStartingVars>numTargetVarsMustBeLargerOrEqual)
    crash << crash;
  this->SetSize(numStartingVars);
  for (int i=0; i<this->size; i++)
  { Polynomial<Element>& currentPoly=this->TheObjects[i];
    currentPoly.MakeDegreeOne(numTargetVarsMustBeLargerOrEqual, i, theRingUnit);
  }
}

template <class Element>
void PolynomialSubstitution<Element>::MakeExponentSubstitution(Matrix<LargeInt>& theSub)
{ Polynomial<Element> tempP;
  MonomialP tempM;
  tempM.MakeOne(theSub.NumRows);
  this->size=0;
  this->SetSize(theSub.NumCols);
  for (int i=0; i<theSub.NumCols; i++)
  { for (int j=0; j<theSub.NumRows; j++)
      tempM[j]= (Rational) theSub(j,i);
    tempP.MakeZero();
    tempP.AddMonomial(tempM, 1);
//    tempP->ComputeDebugString();
    this->TheObjects[i]=tempP;
  }
}

template <class Element>
void PolynomialSubstitution<Element>::MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<Element>& EndPoint)
{ this->SetSize(numVars);
  for (int i=0; i<numVars-1; i++)
    this->TheObjects[i].MakeDegreeOne(numVars, i, Element::TheRingUnit);
  this->TheObjects[numVars-1].CopyFromPoly(EndPoint);
}

class oneFracWithMultiplicitiesAndElongations
{
public:
  ListLight<int> Multiplicities;
  ListLight<int> Elongations;
  void AddMultiplicity(int MultiplicityIncrement, int Elongation);
  int IndexLargestElongation();
  int GetLargestElongation();
  void GetPolyDenominator(Polynomial<LargeInt>& output, int MultiplicityIndex, Vector<Rational>& theExponent);
  int GetMultiplicityLargestElongation();
  int GetLCMElongations();
  int GetTotalMultiplicity() const;
  void invert();
  void init();
  static Vector<Rational> GetCheckSumRoot(int NumVars);
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const oneFracWithMultiplicitiesAndElongations& input)
  { return input.HashFunction();
  }
  void ComputeOneCheckSum(Rational& output, const Vector<Rational>& theExp, int theDimension);
  bool IsHigherThan(oneFracWithMultiplicitiesAndElongations& f);
  void operator=(oneFracWithMultiplicitiesAndElongations& right);
  bool operator==(oneFracWithMultiplicitiesAndElongations& right);
  std::string ToString(int index, bool LatexFormat);
  void OneFracToStringBasisChange
  (PartFractions& owner, int indexElongation, Matrix<LargeInt>& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, int indexInFraction, int theDimension, FormatExpressions& PolyFormatLocal);
};

class LaTeXProcedures
{
public:
  static const int ScaleFactor=40;
  static const int FigureSizeX= 10; //in centimeters
  static const int FigureSizeY=10; //in centimeters
  static const int FigureCenterCoordSystemX= 4; //in centimeters
  static const int FigureCenterCoordSystemY=8; //in centimeters
  static const int TextPrintCenteringAdjustmentX=3;
  static const int TextPrintCenteringAdjustmentY=3;
  static void drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex, std::fstream& output, DrawingVariables& drawInput);
  static void drawTextDirectly(double X1, double Y1, const std::string& theText, int ColorIndex, std::fstream& output);
  static void beginDocument(std::fstream& output);
  static void endLatexDocument(std::fstream& output);
  static void beginPSTricks(std::fstream& output);
  static void endPSTricks(std::fstream& output);
  static void GetStringFromColorIndex(int ColorIndex, std::string &output, DrawingVariables& drawInput);
};

class permutation: public SelectionWithDifferentMaxMultiplicities
{
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int TotalNumElements);
  void incrementAndGetPermutation(List<int>& output);
  void GetPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int GetNumPermutations()
  { return this->getTotalNumSubsets();
  }
};

struct IndicatorWindowVariables
{
public:
  bool Busy;
  int NumProcessedMonomialsCurrentFraction;
  int NumProcessedMonomialsTotal;
  Vector<Rational> modifiedRoot;
  bool flagRootIsModified;
  bool Pause;
  IndicatorWindowVariables(){this->init(); }
  List<std::string> ProgressReportStringS;
  void Assign(IndicatorWindowVariables& right);
  void init()
  { this->Busy=false;
    this->Pause=true;
    this->ProgressReportStringS.SetSize(0);
    this->NumProcessedMonomialsCurrentFraction=0;
    this->NumProcessedMonomialsTotal=0;
    this->modifiedRoot.MakeZero(1);
  }
};

class DrawTextOperation
{
public:
  double X1; double Y1; std::string theText; int ColorIndex; int fontSize; int TextStyle;
  void init(double x1, double y1, const std::string& inputText, int color, int theFontSize, int theTextStyle){ this->X1=x1; this->Y1=y1; this->theText=inputText; this->ColorIndex=color; this->fontSize=theFontSize; this->TextStyle=theTextStyle;}
  void operator=(const DrawTextOperation& other){ this->X1=other.X1; this->Y1=other.Y1; this->theText=other.theText; this->ColorIndex=other.ColorIndex; this->fontSize=other.fontSize; this->TextStyle=other.TextStyle; }
};

class DrawLineOperation
{
public:
  double X1;
  double Y1;
  double X2;
  double Y2;
  int thePenStyle;
  int ColorIndex;
  inline void init(double x1, double y1, double x2, double y2, unsigned long PenStyle, int colorIndex){ this->X1=x1; this->Y1= y1; this->X2=x2; this->Y2=y2; this->thePenStyle=PenStyle; this->ColorIndex= colorIndex;}
  void operator=(const DrawLineOperation& other){ this->X1=other.X1; this->Y1=other.Y1; this->X2=other.X2; this->Y2=other.Y2; this->thePenStyle=other.thePenStyle; this->ColorIndex=other.ColorIndex;}
};

class DrawLineBetweenTwoRootsOperation
{
public:
  Vector<double> v1;
  Vector<double> v2;
  int thePenStyle;
  int ColorIndex;
  double precomputedX1, precomputedY1, precomputedX2, precomputedY2;
  void init(const Vector<Rational> & input1, const Vector<Rational> & input2, unsigned long PenStyle, int colorIndex)
  { if(input1.size!=input2.size)
      crash << crash;
    int theDimension=input1.size;
    this->v1.SetSize(theDimension);
    this->v2.SetSize(theDimension);
    for (int i=0; i<theDimension; i++)
    { this->v1[i]=input1.TheObjects[i].DoubleValue();
      this->v2[i]=input2.TheObjects[i].DoubleValue();
    }
    this->thePenStyle=PenStyle;
    this->ColorIndex=colorIndex;
  }
  void operator=(const DrawLineBetweenTwoRootsOperation& other)
  { this->v1=other.v1;
    this->v2=other.v2;
    this->thePenStyle=other.thePenStyle;
    this->ColorIndex=other.ColorIndex;
    this->precomputedX1=other.precomputedX1;
    this->precomputedY1=other.precomputedY1;
    this->precomputedX2=other.precomputedX2;
    this->precomputedY2=other.precomputedY2;
  }
};

class DrawTextAtVectorOperation
{
public:
  Vector<double> theVector;
  std::string theText;
  int ColorIndex;
  int fontSize;
  int TextStyle;
  double precomputedX, precomputedY;
  void init(const Vector<Rational> & input, const std::string& inputText, int colorIndex, int theFontSize, int theTextStyle)
  { this->theVector.SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->theVector[i]=input.TheObjects[i].DoubleValue();
    this->ColorIndex=colorIndex;
    this->theText=inputText;
    this->fontSize=theFontSize;
    this->TextStyle=theTextStyle;
  }
  void operator=(const DrawTextAtVectorOperation& other)
  { this->theVector=other.theVector;
    this->ColorIndex=other.ColorIndex;
    this->theText=other.theText;
    this->fontSize=other.fontSize;
    this->TextStyle=other.TextStyle;
    this->precomputedX=other.precomputedX;
    this->precomputedY=other.precomputedY;
  }
};

class DrawCircleAtVectorOperation
{
public:
  Vector<double> theVector;
  int theMultiplicity;
  double radius;
  int ColorIndex;
  int thePenStyle;
  double precomputedX, precomputedY;
  void init(const Vector<Rational> & input, double theRadius, unsigned long thePenStylE, int colorIndex)
  { this->theVector.SetSize(input.size);
    for (int i=0; i<input.size; i++)
      this->theVector[i]=input.TheObjects[i].DoubleValue();
    this->ColorIndex=colorIndex;
    this->thePenStyle=thePenStylE;
    this->radius=theRadius;
    this->theMultiplicity=1;
  }
  void operator=(const DrawCircleAtVectorOperation& other)
  { this->theVector=other.theVector;
    this->ColorIndex=other.ColorIndex;
    this->thePenStyle=other.thePenStyle;
    this->radius=other.radius;
    this->precomputedX=other.precomputedX;
    this->precomputedY=other.precomputedY;
    this->theMultiplicity=other.theMultiplicity;
  }
};

class DrawOperations
{
private:
  void changeBasisPreserveAngles(double newX, double newY, GlobalVariables& theGlobalVariables);
public:
  List<int> IndexNthDrawOperation;
  List<int> TypeNthDrawOperation;
  List<DrawTextOperation> theDrawTextOperations;
  List<DrawLineOperation> theDrawLineOperations;
  List<DrawLineBetweenTwoRootsOperation> theDrawLineBetweenTwoRootsOperations;
  List<DrawTextAtVectorOperation> theDrawTextAtVectorOperations;
  List<DrawCircleAtVectorOperation> theDrawCircleAtVectorOperations;
  List<Vectors<double> > toBeHighlightedWhenLabeledVectorHovered;
  Vectors<double> labeledVectors;
  List<std::string> labelsOfLabeledVectors;
  List<List<double> > ProjectionsEiVectors;
  List<Vectors<double> > BasisProjectionPlane;
  static const int GraphicsUnitDefault=150;
  int SelectedCircleMinus2noneMinus1Center; //-2= none, -1=center of coordinate system, nonnegative integers= selectedindex
  Vectors<double> BasisToDrawCirclesAt;
  Matrix<double> theBilinearForm;
  double ClickToleranceX;
  double ClickToleranceY;
  List<double> centerX;
  List<double> centerY;
  List<double> GraphicsUnit;
  bool flagRotatingPreservingAngles;
  bool flagAnimatingMovingCoordSystem;
  bool flagIsPausedWhileAnimating;
  int SelectedPlane;
  std::string DebugString;
  int indexStartingModifiableTextCommands;
  void (*specialOperationsOnBasisChange)(DrawOperations& theOps, GlobalVariables& theGlobalVariables);
  static void projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps, GlobalVariables& theGlobalVariables);
  void MakeMeAStandardBasis(int theDim);
  void operator+=(const DrawOperations& other);
  void initDimensions(Matrix<double>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
  { this->theBilinearForm=bilinearForm;
    this->BasisToDrawCirclesAt=draggableBasis;
    this->BasisProjectionPlane.initFillInObject(NumAnimationFrames, startingPlane);
    this->centerX.initFillInObject(NumAnimationFrames, 300);
    this->centerY.initFillInObject(NumAnimationFrames, 300);
    this->GraphicsUnit.initFillInObject(NumAnimationFrames, DrawOperations::GraphicsUnitDefault);
    this->ComputeProjectionsEiVectors();
  }
  void initDimensions(Matrix<Rational>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
  { Matrix<double> tempMat;
    tempMat.init(bilinearForm.NumRows, bilinearForm.NumCols);
    for (int i=0; i<bilinearForm.NumRows; i++)
      for (int j=0; j<bilinearForm.NumCols; j++)
        tempMat.elements[i][j]=bilinearForm.elements[i][j].DoubleValue();
    this->initDimensions(tempMat, draggableBasis, startingPlane, NumAnimationFrames);
  }
  void initDimensions(int theDim, int numAnimationFrames);
  int GetDimFirstDimensionDependentOperation();
  int GetDimFromBilinearForm();
  inline void GetCoordsDrawingComputeAll(Vector<double>& input, double& X1, double& Y1)
  { X1=this->theBilinearForm.ScalarProduct(input, this->BasisProjectionPlane[this->SelectedPlane][0]);
    Y1=this->theBilinearForm.ScalarProduct(input, this->BasisProjectionPlane[this->SelectedPlane][1]);
    X1=X1*this->GraphicsUnit[this->SelectedPlane] +this->centerX[this->SelectedPlane];
    Y1=Y1*this->GraphicsUnit[this->SelectedPlane] +this->centerY[this->SelectedPlane] ;
  }
  inline void GetCoordsForDrawingProjectionsComputed(Vector<double>& input, double& X1, double& Y1)
  { X1=0; Y1=0;
    for (int j=0; j<input.size; j++)
    { X1+=this->ProjectionsEiVectors[j][0]*input[j];
      Y1+=this->ProjectionsEiVectors[j][1]*input[j];
    }
    X1=X1*this->GraphicsUnit[this->SelectedPlane] +this->centerX[this->SelectedPlane];
    Y1=Y1*this->GraphicsUnit[this->SelectedPlane]+this->centerY[this->SelectedPlane];
  }
  inline void GetCoordsForDrawingProjectionsComputed(Vector<double>& input1, Vector<double>& input2, double& X1, double& Y1, double& X2, double& Y2)
  { X1=0; X2=0; Y1=0; Y2=0;
    for (int j=0; j<input1.size; j++)
    { X1+=this->ProjectionsEiVectors[j][0]*input1[j];
      Y1+=this->ProjectionsEiVectors[j][1]*input1[j];
      X2+=this->ProjectionsEiVectors[j][0]*input2[j];
      Y2+=this->ProjectionsEiVectors[j][1]*input2[j];
    }
    X1=X1*this->GraphicsUnit[this->SelectedPlane]+this->centerX[this->SelectedPlane];
    X2=X2*this->GraphicsUnit[this->SelectedPlane]+this->centerX[this->SelectedPlane];
    Y1=Y1*this->GraphicsUnit[this->SelectedPlane]+this->centerY[this->SelectedPlane];
    Y2=Y2*this->GraphicsUnit[this->SelectedPlane]+this->centerY[this->SelectedPlane];
  }
  void EnsureProperInitialization();
  bool AreWithinClickTolerance(double x1, double y1, double x2, double y2)
  { x1-=x2;
    y1-=y2;
    if (x1<0)
      x1=-x1;
    if (y1<0)
      y1=-y1;
    return x1<=this->ClickToleranceX && y1<=this->ClickToleranceY;
  }
  bool mouseMoveRedraw(int X, int Y, GlobalVariables& theGlobalVariables)
  { if (this->SelectedCircleMinus2noneMinus1Center==-2)
      return false;
    if (this->SelectedCircleMinus2noneMinus1Center==-1)
    { this->centerX[this->SelectedPlane]=X;
      this->centerY[this->SelectedPlane]=Y;
      return true;
    }
    if (this->SelectedCircleMinus2noneMinus1Center>=0)
    { if (this->flagRotatingPreservingAngles)
      { this->changeBasisPreserveAngles((double) X , (double) Y, theGlobalVariables);
        return true;
      }
    }
    return false;
  //  this->draw();
  }
  void click(double x, double y);
  void drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawLineBetweenTwoVectorsBuffer(const Vector<Rational> & vector1, const Vector<Rational> & vector2, unsigned long thePenStyle, int ColorIndex);
  void drawTextAtVectorBuffer(const Vector<Rational> & input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawCircleAtVectorBuffer(const Vector<Rational> & input, double radius, unsigned long thePenStyle, int theColor);
  double getAngleFromXandY(double x, double y, double neighborX, double neighborY);
  void ScaleToUnitLength(Vector<double>& theRoot)
  { double theLength=this->theBilinearForm.ScalarProduct(theRoot, theRoot);
    theLength=FloatingPoint::sqrt(theLength);
    theRoot/=theLength;
  }
  void RotateOutOfPlane
  (std::stringstream& logger, Vector<double>& input, Vector<double>& output, Vector<double>& orthoBasis1, Vector<double>& orthoBasis2,
   double oldTanSquared, double newTanSquared);
  void ModifyToOrthonormalNoShiftSecond(Vector<double>& root1, Vector<double>& root2);
  void ComputeXYsFromProjectionsEisAndGraphicsUnit();
  void ComputeProjectionsEiVectors();
  DrawOperations()
  { this->SelectedPlane=0;
    this->initDimensions(2, 2);
    this->flagAnimatingMovingCoordSystem=false;
    this->specialOperationsOnBasisChange=0;
    this->indexStartingModifiableTextCommands=0;
  }
  void init();
  enum DrawOperationType{ typeDrawLine, typeDrawText, typeDrawLineBetweenTwoVectors, typeDrawTextAtVector, typeDrawCircleAtVector,};
};

class VirtualDrawOp
{
public:
  int theVirtualOp;
  int indexPhysicalFrame;
  int indexPhysicalDrawOp;
  int selectedPlaneInPhysicalDrawOp;
  std::string ToString();
  void operator=(const VirtualDrawOp& other)
  { this->theVirtualOp=other.theVirtualOp;
    this->indexPhysicalFrame=other.indexPhysicalFrame;
    this->indexPhysicalDrawOp=other.indexPhysicalDrawOp;
    this->selectedPlaneInPhysicalDrawOp=other.selectedPlaneInPhysicalDrawOp;
  }
};

class AnimationBuffer
{
public:
//the following items are a part of a dirty hack i need to do for an
//under-stress presentation. Please do not use them !
//they need a rewrite!
  std::string GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension);
  List<DrawingVariables> theFrames;
  DrawOperations theBuffer;
////////////////////////////////////////end of dirty hack
  DrawOperations stillFrame;
  List<DrawOperations> thePhysicalDrawOps;
  List<VirtualDrawOp> theVirtualOpS;
  bool flagAnimating;
  bool flagIsPausedWhileAnimating;
  int indexVirtualOp;
  std::string ToString();
  bool IncrementOpReturnNeedsRedraw();
  int GetIndexCurrentPhysicalFrame();
  void DrawNoInit(DrawingVariables& theDrawingVariables, GlobalVariables& theGlobalVariables);
  AnimationBuffer();
  DrawOperations& GetCurrentDrawOps();
  DrawOperations& GetLastDrawOps();
  int GetIndexCurrentDrawOps();
  int GetNumPhysicalFramesNoStillFrame();
  void MakeZero();
  void operator+=(const AnimationBuffer& other);
  void operator+=(const DrawOperations& other);
  void operator=(const AnimationBuffer& other);
  void AddPause(int numFrames);
  void AddCloneLastFrameAppendOperations(const DrawOperations& other);
  void AddFrameShift(int numFrames);
  enum{typeDrawOps, typePause, typeClearScreen, typeCloneLastFrameAddOps,};
};

class DrawingVariables
{
private:
  drawLineFunction theDrawLineFunction;
  drawTextFunction theDrawTextFunction;
  drawCircleFunction theDrawCircleFunction;
  drawClearScreenFunction theDrawClearScreenFunction;
public:
  enum PenStyles
  { PenStyleInvisible, PenStyleDashed, PenStyleDotted, PenStyleNormal, PenStyleZeroChamber, PenStylePermanentlyZeroChamber, PenStyleLinkToOriginZeroChamber, PenStyleLinkToOrigin, PenStyleLinkToOriginPermanentlyZeroChamber };
  enum TextStyles
  { TextStyleNormal, TextStyleInvisible, TextStyleChamber, TextStyleZeroChamber, TextStylePermanentlyZeroChamber };
  MutexWrapper LockedWhileDrawing;
  int ColorDashes;
  bool flagLaTeXDraw;
  bool flag2DprojectionDraw;
  bool flagDisplayingCreationNumbersInsteadOfDisplayNumbers;
  bool flagDrawChamberIndices;
  bool flagDrawingInvisibles;
  bool flagDrawingLinkToOrigin;
  bool flagFillUserDefinedProjection;
  Vectors<Rational> FillUserDefinedProjection;
  int Selected;
  static int NumHtmlGraphics;
  int textX;
  int textY;
  int fontSizeNormal;
  int fontSizeSubscript;
  int ColorTextDefault;
  int ColorTextZeroChamber;
  int ColorTextPermanentlyZeroChamber;
  int ColorChamberIndicator;
  int ColorWeylChamberWalls;
  int DefaultHtmlWidth;
  int DefaultHtmlHeight;
  void initDrawingVariables(int cX1, int cY1);
  DrawingVariables(int cx, int cy)
  { this->initDrawingVariables(cx, cy);
  }
  DrawingVariables()
  { this->initDrawingVariables(100, 100);
  }
  void SetDrawLineFunction(drawLineFunction theFunction)
  { this->theDrawLineFunction=theFunction;
  }
  void SetDrawTextFunction(drawTextFunction theFunction)
  { this->theDrawTextFunction=theFunction;
  }
  void SetDrawCircleFunction(drawCircleFunction theFunction)
  { this->theDrawCircleFunction=theFunction;
  }
  void SetDrawClearFunction(drawClearScreenFunction theFunction)
  { this->theDrawClearScreenFunction=theFunction;
  }
  int GetColorFromChamberIndex(int index, std::fstream* LaTexOutput);
  static void GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational>& r, double& x, double& y);
  static void ProjectOnToHyperPlaneGraphics(Vector<Rational>& input, Vector<Rational>& output, Vectors<Rational>& directions);
  std::string GetColorHtmlFromColorIndex(int colorIndex);
  std::string GetColorPsTricksFromColorIndex(int colorIndex);
  DrawOperations theBuffer;
  inline int GetActualPenStyleFromFlagsAnd(int inputPenStyle);
  inline int GetActualTextStyleFromFlagsAnd(int inputTextStyle);
  std::string GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension);
  void drawString(DrawElementInputOutput& theDrawData, const std::string& input, int theFontSize, int theTextStyle);
  void drawCoordSystemDirectlly(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile);
  void drawCoordSystemBuffer(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile);
  void drawLineDirectly(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
  void drawLineBuffer(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex);
  void drawTextDirectly(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile);
  void drawBufferNoIniT(DrawOperations& theOps);
  void drawBufferNoIniT()
  { this->drawBufferNoIniT(this->theBuffer);
  }
  //if the LatexOutFile is zero then the procedure defaults to the screen
  void drawLineBufferOld(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex, std::fstream* LatexOutFile);
  inline void drawLineBetweenTwoVectorsBuffer(const Vector<Rational>& r1, const Vector<Rational>& r2, int PenStyle, int PenColor)
  { this->theBuffer.drawLineBetweenTwoVectorsBuffer(r1, r2, PenStyle, PenColor);
  }
  void drawTextAtVectorBuffer(const Vector<Rational>& point, const std::string& inputText, int textColor, int theTextStyle, std::fstream* LatexOutFile);
  void drawCircleAtVectorBuffer(const Vector<Rational>& point, double radius, unsigned long thePenStyle, int theColor);
  void operator=(const DrawingVariables& other)
  { this->theDrawLineFunction=other.theDrawLineFunction;
    this->theDrawTextFunction=other.theDrawTextFunction;
  }
};

template <class Element>
void MathRoutines::RaiseToPower(Element& theElement, int thePower, const Element& theRingUnit)
{ if (thePower<0)
    return;
  if (thePower==1)
    return;
  if (thePower==0)
  { theElement=theRingUnit;
    return;
  }
  Element squares;
	squares=theElement;
	if (thePower<4)
  { for (int i=1; i<thePower; i++)
      theElement*=squares;
    return;
  }
  theElement=theRingUnit;
	while (thePower>0)
	{	if (thePower%2==1)
			theElement*=squares;
		squares*=squares;
		thePower/=2;
	}
}

template<typename Element>
void Matrix<Element>::NonPivotPointsToEigenVectorMatrixForm(Selection& TheNonPivotPoints, Matrix<Element>& output)
{ int RowCounter=0;
  output.init(this->NumCols, 1);
  for (int i=0; i<this->NumCols; i++)
    if (!TheNonPivotPoints.selected[i])
    { output(i,0)=0;
      for (int j=0; j<TheNonPivotPoints.CardinalitySelection; j++)
        output(i,0)-=(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
      RowCounter++;
    } else
      output(i,0)=1;
}

template<typename Element>
void Matrix<Element>::NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, Vector<Element>& output, const Element& theRingUnit, const Element& theRingZero)
{ int RowCounter=0;
  output.SetSize(this->NumCols);
  for (int i=0; i<this->NumCols; i++)
  { if (!TheNonPivotPoints.selected[i])
    { output[i]=theRingZero;
      for (int j=0; j<TheNonPivotPoints.CardinalitySelection; j++)
        output[i]-=(this->elements[RowCounter][TheNonPivotPoints.elements[j]]);
      RowCounter++;
    } else
      output[i]=theRingUnit;
  }
}

class GlobalVariables
{
  //I love doxygen!
  /// @cond
private:
  FeedDataToIndicatorWindow FeedDataToIndicatorWindowDefault;
  double (*getElapsedTimePrivate)();
  void (*callSystem)(const std::string& theSystemCommand);
public:
  double MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  FormatExpressions theDefaultFormat;
//progress report flags:
  bool flagGaussianEliminationProgressReport;

  IndicatorWindowVariables theIndicatorVariables;
  DrawingVariables theDrawingVariables;
  FormatExpressions thePolyFormat;
  Controller theLocalPauseController;


  std::string inputPatH;
  std::string inputDisplayPath;
  std::string IPAdressCaller;

  //buffers:
  MemorySaving<Selection> selWallSelection;
  MemorySaving<Selection> selComputeNormalExcludingIndex;
  MemorySaving<Selection> selApproveSelAgainstOneGenerator;
  MemorySaving<Selection> selSimplexAlg2;
  MemorySaving<Selection> selGetNewVerticesAppend;
  MemorySaving<Selection> selGetNewVerticesAppend2;
  MemorySaving<Selection> selSplitChamber;

  MemorySaving<HashedList<Selection> > hashedSelSimplexAlg;

  MemorySaving<HashedList<Vector<Rational> > > hashedRootsNewChamberSplit;

  MemorySaving<Matrix<Rational> > matIdMatrix;
  MemorySaving<Matrix<Rational> > matOneColumn;
  MemorySaving<Matrix<Rational> > matReduceMonomialByMonomial2;
  MemorySaving<Matrix<Rational> > matReduceMonomialByMonomial;
  MemorySaving<Matrix<Rational> > matConeCondition1;
  MemorySaving<Matrix<Rational> > matConeCondition2;
  MemorySaving<Matrix<Rational> > matConeCondition3;
  MemorySaving<Vector<Rational> > vectConeCondition3;
  MemorySaving<Matrix<Rational> > matComputeEpsCoordsWRTk;
  MemorySaving<Matrix<Rational> > matSimplexAlgorithm1;
  MemorySaving<Matrix<Rational> > matSimplexAlgorithm2;
  MemorySaving<Matrix<Rational> > matSimplexAlgorithm3;
  MemorySaving<Matrix<Rational> > matGetNewVerticesAppend;
  MemorySaving<Matrix<Rational> > matSplitChamberBuff;

  MemorySaving<Polynomial<LargeInt> > PolyLargeIntPartFracBuffer5;
  MemorySaving<Polynomial<LargeInt> > PolyLargeIntPartFracBuffer6;

  MemorySaving<Polynomial<Rational> > RFgcdBuffer1;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer2;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer3;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer4;
  MemorySaving<Polynomial<Rational> > RFgcdBuffer5;

  MemorySaving<List<Polynomial<Rational> > > RFgcdBufferList1;

  MemorySaving<MonomialP> RFgcdBuferMon1;
  MemorySaving<MonomialP> RFgcdBuferMon2;

  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;

  MemorySaving<ReflectionSubgroupWeylGroup> subGroupActionNormalizerCentralizer;

  MemorySaving<DynkinDiagramRootSubalgebra > dynAttemptTheTripleTrick;

  MemorySaving<Vectors<Rational> > rootsNilradicalRoots;
  MemorySaving<Vectors<Rational> > rootsConeConditionHolds2;
  MemorySaving<Vectors<Rational> > rootsAttemptTheTripleTrick;
  MemorySaving<Vectors<Rational> > rootsAttepmtTheTripleTrickWRTSA;
  MemorySaving<Vectors<Rational> > rootsComputeEpsCoordsWRTk2;
  MemorySaving<Vectors<Rational> > rootsComputeEpsCoordsWRTk;
  MemorySaving<Vectors<Rational> > rootsGetEpsilonCoords;
  MemorySaving<Vectors<Rational> > rootsConeWriteToFileBuffer;

  MemorySaving<HashedList<Vector<Rational> > > hashedRootsComputeSubGroupFromGeneratingReflections;

  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso1;
  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso2;
  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso3;
  MemorySaving<List<Vectors<Rational> > > rootsAttemptExtensionIso4;

  MemorySaving<GroebnerBasisComputation<Rational> > theGroebnerBasisComputation;

  MemorySaving<Cone> coneBuffer1NewSplit;
  MemorySaving<Cone> coneBuffer2NewSplit;

  GlobalVariables();
  void SetTimerFunction(double (*timerFunction)())
  { this->getElapsedTimePrivate=timerFunction;
  }
  double GetElapsedSeconds()
  { if (this->getElapsedTimePrivate!=0)
      return this->getElapsedTimePrivate();
    return -1;
  }
  void operator=(const GlobalVariables& other)
  { this->FeedDataToIndicatorWindowDefault=other.FeedDataToIndicatorWindowDefault;
    this->theDrawingVariables=other.theDrawingVariables;
  }
  inline void DrawBufferNoIniT()
  { this->theDrawingVariables.drawBufferNoIniT();
  }
  inline void SetFeedDataToIndicatorWindowDefault(FeedDataToIndicatorWindow input)
  { this->FeedDataToIndicatorWindowDefault=input;
  }
  inline FeedDataToIndicatorWindow GetFeedDataToIndicatorWindowDefault()
  { return this->FeedDataToIndicatorWindowDefault;
  }
  inline void FeedIndicatorWindow(IndicatorWindowVariables& input)
  { if (this->FeedDataToIndicatorWindowDefault!=0)
      this->FeedDataToIndicatorWindowDefault(input);
  }
  void SetCallSystem(void (*theSystemCall)(const std::string&))
  { this->callSystem=theSystemCall;
  }
  void System(const std::string& systemCommand)
  { if (this->callSystem!=0)
      this->callSystem(systemCommand);
  }
  inline void MakeReport()
  { if (this->FeedDataToIndicatorWindowDefault!=0)
      this->FeedDataToIndicatorWindowDefault(this->theIndicatorVariables);
  }
  /// @endcond
};

template <class Element>
void Matrix<Element>::GaussianEliminationEuclideanDomain
(Matrix<Element>* otherMatrix, const Element& theRingMinusUnit, const Element& theRingUnit, bool (*comparisonGEQFunction) (const Element& left, const Element& right),
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("Matrix_Element::GaussianEliminationEuclideanDomain");
  ProgressReport theReport(theGlobalVariables);
  if (otherMatrix==this)
    crash << "This is a programming error: the Carbon copy in the Gaussian elimination coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). " << crash;
  int col=0;
  Element tempElt;
  int row=0;
  while(row<this->NumRows && col<this->NumCols)
  { //std::cout << "<br>****************row: " << row << " status: " << this->ToString(true, false);
    int foundPivotRow=-1;
    for (int i=row; i<this->NumRows; i++)
      if(!this->elements[i][col].IsEqualToZero())
      { foundPivotRow=i;
        break;
      }
    if (foundPivotRow!=-1)
    { /*if (this->elements[foundPivotRow][col].IsEqualToZero())
      { crash << "This is a programming error. "
        << "Something is very wrong: I am getting 0 for a pivot element in "
        << "Gaussian elimination over Euclidean domains. "
        << crash;
      }*/
      this->SwitchTwoRowsWithCarbonCopy(row, foundPivotRow, otherMatrix);
      /*if (this->elements[row][col].IsEqualToZero())
      { crash << "This is a programming error. "
        << "Something is very wrong: I am getting 0 for a pivot element in "
        << "Gaussian elimination over Euclidean domains. "
        << crash;
      }*/
      if (this->elements[row][col].IsNegative())
        this->RowTimesScalarWithCarbonCopy(row, theRingMinusUnit, otherMatrix);
      int ExploringRow= row+1;
//      std::cout << "<br>before second while: " << this->ToString(true, false);
      while (ExploringRow<this->NumRows)
      { if (theGlobalVariables!=0)
        { std::stringstream out;
          out << "Pivotting on row of index " << row +1 << " with exploring row of index " << ExploringRow+1 << "; total rows: " << this->NumRows;
          theReport.Report(out.str());
        }
        Element& PivotElt=this->elements[row][col];
        Element& otherElt=this->elements[ExploringRow][col];
        /*if (PivotElt.IsEqualToZero())
        { crash << "This is a programming error. "
          << "Something is very wrong: I am getting 0 for a pivot element in "
          << "Gaussian elimination over Euclidean domains. "
          << crash;
        }*/
        if (otherElt.IsNegative())
          this->RowTimesScalarWithCarbonCopy(ExploringRow, theRingMinusUnit, otherMatrix);
        bool isSmallerOrEqualTo= comparisonGEQFunction==0 ? PivotElt<=otherElt :
        comparisonGEQFunction(otherElt, PivotElt);
        if (isSmallerOrEqualTo)
        { tempElt=otherElt;
          tempElt/=PivotElt;
          tempElt.AssignFloor();
          this->SubtractRowsWithCarbonCopy(ExploringRow, row, 0, tempElt, otherMatrix);
        }
        if (this->elements[ExploringRow][col].IsEqualToZero())
          ExploringRow++;
        else
          this->SwitchTwoRowsWithCarbonCopy(ExploringRow, row, otherMatrix);
//        std::cout << "<br>second while cycle end: " << this->ToString(true, false);
      }
      Element& PivotElt = this->elements[row][col];
      for (int i=0; i<row; i++)
      { tempElt =this->elements[i][col];
        tempElt/=PivotElt;
//        std::cout << " the floor of " << tempElt.ToString();
        tempElt.AssignFloor();
//        std::cout << " is " << tempElt.ToString();
        this->SubtractRowsWithCarbonCopy(i, row, 0, tempElt, otherMatrix);
        if (this->elements[i][col].IsNegative())
          this->AddTwoRowsWithCarbonCopy(row, i, 0, theRingUnit, otherMatrix);
      }
      row++;
    }
    col++;
//    std::cout << "end of cycle status: " << this->ToString(true, false) << "<br>****************";
  }
}

template <class coefficient>
void Vectors<coefficient>::SelectABasisInSubspace
(const List<Vector<coefficient> >& input, List<Vector<coefficient> >& output, Selection& outputSelectedPivotColumns, GlobalVariables* theGlobalVariables)
{ if (&input==&output)
  { List<Vector<coefficient> > inputCopy=input;
    Vectors<coefficient>::SelectABasisInSubspace(inputCopy, output, outputSelectedPivotColumns, theGlobalVariables);
    return;
  }
  if (input.size==0)
  { output=input;
    return;
  }
  MacroRegisterFunctionWithName("Vectors::SelectABasisInSubspace");
  ProgressReport theReport(theGlobalVariables);
  int theDim=input[0].size;
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size << " generators in dimension " << theDim << "... " ;
    theReport.Report(reportStream.str());
  }
  Matrix<coefficient> theMat;
  int MaxNumRows=MathRoutines::Minimum(input.size, theDim);
  theMat.init(MaxNumRows, theDim);
  int currentRow=0;
  for (int i=0; i<input.size; i++)
  { for (int j=0; j<theDim; j++)
      theMat(currentRow,j)=input[i][j];
    currentRow++;
    if (currentRow==MaxNumRows || i==input.size-1)
    { theMat.GaussianEliminationByRows(0, 0, &outputSelectedPivotColumns);
      currentRow=outputSelectedPivotColumns.CardinalitySelection;
    }
    if (currentRow==MaxNumRows)
      break;
  }
  output.SetSize(outputSelectedPivotColumns.CardinalitySelection);
  for (int i=0; i<output.size; i++)
    theMat.GetVectorFromRow(i, output[i]);
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "Selecting a basis of a vector space with " << input.size << " generators in dimension " << theDim << "... DONE! " ;
    theReport.Report(reportStream.str());
  }
}

template <class Object>
void List<Object>::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes)const
{ int NumWritten=this->size;
  if (UpperLimitForDebugPurposes>0 && UpperLimitForDebugPurposes<NumWritten)
    NumWritten=UpperLimitForDebugPurposes;
  output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "size: " << NumWritten << "\n";
  for (int i=0; i<NumWritten; i++)
  { this->TheObjects[i].WriteToFile(output, theGlobalVariables);
    output << "\n";
    if (theGlobalVariables!=0)
      theGlobalVariables->theLocalPauseController.SafePointDontCallMeFromDestructors();
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class Object>
bool List<Object>::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitForDebugPurposes)
{ std::string tempS;
  int ActualListSize; int NumWordsBeforeTag;
  XML::ReadThroughFirstOpenTag(input, NumWordsBeforeTag, this->GetXMLClassName());
  if(NumWordsBeforeTag!=0)
    crash << crash;
  input >> tempS >> ActualListSize;
  if(tempS!="size:")
    crash << crash;
  if(tempS!="size:")
    return false;
  int CappedListSize = ActualListSize;
  if (UpperLimitForDebugPurposes>0 && UpperLimitForDebugPurposes<CappedListSize)
    CappedListSize=UpperLimitForDebugPurposes;
  this->SetSize(CappedListSize);
  for (int i=0; i<CappedListSize; i++)
  { this->TheObjects[i].ReadFromFile(input, theGlobalVariables);
    //<reporting_and_safepoint_duties>
    if (theGlobalVariables!=0)
    { if (ActualListSize>30)
      { std::stringstream report;
        report << "Reading object number " << i+1 << " out of " << ActualListSize;
        if (CappedListSize<ActualListSize)
          report << " capped at " << CappedListSize;
        ProgressReport tempReport(theGlobalVariables, report.str());
      }
      theGlobalVariables->theLocalPauseController.SafePointDontCallMeFromDestructors();
    }
    //</reporting_and_safepoint_duties>
  }
  bool tempBool= XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsBeforeTag, this->GetXMLClassName());
  if(!tempBool)
    crash << crash;
  return true;
}

template<class Base>
class CompleX
{
  static bool flagEqualityIsApproximate;
  static double EqualityPrecision;
  public:
  Base Im;
  Base Re;
  std::string ToString()
  { std::stringstream tempStream;
    tempStream << *this;
    return tempStream.str();
  }
  void ToString(std::string& output)
  { output=this->ToString();
  }
  friend std::iostream& operator<< <Base>(std::iostream& output, const CompleX<Base>& input);
  void operator*=(const CompleX<Base>& other)
  { CompleX Accum;
    Accum.Re=this->Re*other.Re-this->Im*other.Im;
    Accum.Im=this->Re*other.Im+ this->Im*other.Re;
    this->operator=(Accum);
  }
  void operator=(const CompleX<Base>& other)
  { this->Re=other.Re;
    this->Im=other.Im;
  }
  void operator+=(const CompleX<Base>& other)
  { this->Re+=other.Re;
    this->Im+=other.Im;
  }
  void operator-=(const CompleX<Base>& other)
  { this->Re-=other.Re;
    this->Im-=other.Im;
  }
  void operator=(int other)
  { this->Re=other;
    this->Im=0;
  }
  void operator=(double other)
  { this->Re=other;
    this->Im=0;
  }
  void Invert()
  { Base numerator;
    numerator=this->Re*this->Re+this->Im* this->Im;
    this->Re/=numerator;
    numerator*=-1;
    this->Im/=numerator;
  }
  bool IsEqualToZero()const
  { if(!CompleX<Base>::flagEqualityIsApproximate)
      return this->Im==0 && this->Re==0;
    else
      return
      this->Im<CompleX<Base>::EqualityPrecision && -this->Im<CompleX<Base>::EqualityPrecision &&
      this->Re<CompleX<Base>::EqualityPrecision && -this->Re<CompleX<Base>::EqualityPrecision;
  }
  inline void Minus()
  { this->Im=-this->Im;
    this->Re=-this->Re;
  }
  CompleX(){}
  CompleX(int other)
  { this->operator=(other);
  }
  CompleX(double other)
  { this->operator=(other);
  }
};

template<class coefficient>
class ElementSemisimpleLieAlgebra :public MonomialCollection<ChevalleyGenerator, coefficient>
{
public:
  bool CheckConsistency()const
  { if (this->size()==0)
      return true;
    SemisimpleLieAlgebra* owner= (*this)[0].owneR;
    for (int i=1; i<this->size(); i++)
      if (owner!=(*this)[i].owneR)
        crash << "This is a programming error: ElementSemisimpleLieAlgebra contains Chevalley generators with different owners. " << crash;
    return true;
  }
  bool NeedsBrackets()const;
  Vector<coefficient> GetCartanPart()const;
  void MakeGGenerator(const Vector<Rational>& theRoot, SemisimpleLieAlgebra& inputOwner);
  bool IsElementCartan()const;
  void MakeHgenerator(const Vector<coefficient>& theH, SemisimpleLieAlgebra& inputOwners);
  void MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner);
  void ElementToVectorNegativeRootSpacesFirst(Vector<coefficient>& output)const;
  void AssignVectorNegRootSpacesCartanPosRootSpaces(const Vector<coefficient>& input, SemisimpleLieAlgebra& owner);
  bool GetCoordsInBasis(const List<ElementSemisimpleLieAlgebra<coefficient> >& theBasis, Vector<coefficient>& output, GlobalVariables& theGlobalVariables)const;
  SemisimpleLieAlgebra* GetOwner()const
  { this->CheckConsistency();
    if (this->size()==0)
      return 0;
    return (*this)[0].owneR;
  }
  bool GetCoordsInBasis
  (const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables)const
  { Vector<Rational> tempVect;
    if (! this->GetCoordsInBasis(theBasis, tempVect, theGlobalVariables))
      return false;
    output.SetSize(tempVect.size);
    return true;
  }
  void MultiplyByRational(SemisimpleLieAlgebra& owner, const Rational& theNumber);
  static void GetBasisFromSpanOfElements
  (List<ElementSemisimpleLieAlgebra>& theElements, Vectors<RationalFunctionOld>& outputCoords, List<ElementSemisimpleLieAlgebra>& outputTheBasis, GlobalVariables& theGlobalVariables);
  void ActOnMe(const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner);
  void ActOnMe
  (const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner,
   const RationalFunctionOld& theRingUnit, const RationalFunctionOld& theRingZero, GlobalVariables* theGlobalVariables);
  bool IsACoeffOneChevalleyGenerator(int& outputGenerator, SemisimpleLieAlgebra& owner);
  bool IsProportionalTo(const ElementSemisimpleLieAlgebra& other)const
  { Vector<Rational> tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
   return tempRoot1.IsProportionalTo(tempRoot2);
  }
  bool IsProportionalTo(const ElementSemisimpleLieAlgebra& other, Rational& outputTimesMeEqualsInput)const
  { Vector<Rational> tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.IsProportionalTo(tempRoot2, outputTimesMeEqualsInput);
  }
  bool MustUseBracketsWhenDisplayingMeRaisedToPower();
  unsigned int HashFunction()const
  { return this->indexOfOwnerAlgebra*SomeRandomPrimes[0]+this->::MonomialCollection<ChevalleyGenerator, Rational>::HashFunction()*SomeRandomPrimes[1];
  }
  static unsigned int HashFunction(const ElementSemisimpleLieAlgebra& input)
  { return input.HashFunction();
  }
  template<class otherType>
  void operator=(const ElementSemisimpleLieAlgebra<otherType>& other)
  { this->::MonomialCollection<ChevalleyGenerator, coefficient>::operator=(other);
  }
};

template <class Object>
const Object& MemorySaving<Object>::GetElementConst()const
{ if (this->theValue==0)
    crash << "Programming error: attempting to access zero pointer. " << crash;
  return *this->theValue;
}

template <class Object>
MemorySaving<Object>::~MemorySaving()
{ this->FreeMemory();
}

template <class Object>
void MemorySaving<Object>::FreeMemory()
{ delete this->theValue;
  this->theValue=0;
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter--;
  ParallelComputing::CheckPointerCounters();
#endif
}

template <class Object>
Object& MemorySaving<Object>::GetElement()
{ if (this->theValue==0)
  { this->theValue= new Object;
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter++;
  ParallelComputing::CheckPointerCounters();
#endif
  }
  return *(this->theValue);
}

template <class Element>
int Matrix<Element>::FindPositiveLCMCoefficientDenominatorsTruncated()
{ int result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      result*=(this->elements[i][j].DenShort/Rational::gcdSigned(result, this->elements[i][j].DenShort));
  return result;
}

template <class Element>
LargeIntUnsigned Matrix<Element>::FindPositiveLCMCoefficientDenominators()
{ LargeIntUnsigned result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      result=LargeIntUnsigned::lcm(result, (*this)(i,j).GetDenominator());
  return result;
}

template <class Element>
void Matrix<Element>::GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen)
{ outputDen=this->FindPositiveLCMCoefficientDenominators();
  outputMat.init(this->NumRows, this->NumCols);
  Rational tempRat;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempRat=this->elements[i][j]*outputDen;
      outputMat(i,j)=tempRat.GetDenominator();
    }
}

template <class Element>
int Matrix<Element>::FindPositiveGCDCoefficientNumeratorsTruncated()
{ int result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      if (this->elements[i][j].NumShort!=0)
        result=Rational::gcdSigned(result, this->elements[i][j].NumShort);
  if(result==0)
    crash << crash;
  if (result<0)
    result=-result;
  return result;
}

template <class Element>
void Matrix<Element>::ScaleToIntegralForMinRationalHeightNoSignChange()
{ Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(this->FindPositiveLCMCoefficientDenominatorsTruncated(), this->FindPositiveGCDCoefficientNumeratorsTruncated());
  *this*=(tempRat);
}

template <class Element>
Element Matrix<Element> ::GetDeterminant()
{ Matrix<Element> tempMat=*this;
  Element result;
  tempMat.ComputeDeterminantOverwriteMatrix(result);
  return result;
}

template <class Element>
Element Matrix<Element>::GetTrace()const
{ if (this->NumCols!=this->NumRows)
    crash << "This is either programming error, a mathematical error, or requires a more general definition of trace. Requesting the trace of "
    << " a non-square matrix of " << this->NumRows << " rows and " << this->NumCols << " columns is not allowed. "
    << crash;
  Element acc = 0;
  for(int i=0; i<this->NumCols; i++)
    acc += this->elements[i][i];
  return acc;
}

template <class Element>
Matrix<Element> Matrix<Element>::operator*(const Matrix<Element>& right)const
{ Matrix<Element> tempMat;
  tempMat=right;
  tempMat.MultiplyOnTheLeft(*this);
  return tempMat;
}

template <class Element>
Vector<Element> Matrix<Element>::operator*(const Vector<Element>& v) const
{ if(v.size != NumCols)
    crash  << "matrix application mismatch: matrix with" << NumCols << "columns attempted to multiply vector of length" << v.size << crash;
  Vector<Element> out;
  out.MakeZero(NumRows);
  for(int i=0;i<NumRows;i++)
    for(int j=0;j<NumCols;j++)
      out[i] += elements[i][j] * v[j];
  return out;
}

template <class Element>
void Matrix<Element>::AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den)
{ this->init(theMat.NumRows, theMat.NumCols);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { this->elements[i][j]=theMat.elements[i][j];
      this->elements[i][j]/=Den;
    }
}

template <class coefficient>
void Polynomial<coefficient> ::MakePolyFromDirectionAndNormal(Vector<coefficient>& direction, Vector<coefficient>& normal, coefficient& Correction)
{ Rational tempRat2= Vector<coefficient>::ScalarEuclidean(direction, normal);
  this->MakeZero(direction.size);
  MonomialP tempM;
  for (int i=0; i<direction.size; i++)
  { tempM.MakeEi(i);
    this->AddMonomial(tempM, normal.TheObjects[i]/tempRat2);
  }
  *this+=Correction;
}

template <class coefficient>
bool Vectors<coefficient>::GetNormalSeparatingCones
(List<Vector<coefficient> >& coneStrictlyPositiveCoeffs, List<Vector<coefficient> >& coneNonNegativeCoeffs, Vector<coefficient>& outputNormal,
 GlobalVariables* theGlobalVariables)
{ MemorySaving<Matrix<Rational> > tempA;
  MemorySaving<Matrix<Rational> > tempB;
  MemorySaving<Vector<Rational> > tempX;
  Matrix<Rational>& matA=theGlobalVariables==0 ? tempA.GetElement() : theGlobalVariables->matConeCondition1.GetElement();
  Matrix<Rational>& matb=theGlobalVariables==0 ? tempB.GetElement() : theGlobalVariables->matConeCondition2.GetElement();
  Vector<Rational>& matX=theGlobalVariables==0 ? tempX.GetElement() : theGlobalVariables->vectConeCondition3.GetElement();
  int theDimension=coneStrictlyPositiveCoeffs[0].size;
  if (coneStrictlyPositiveCoeffs.size==0)
  { if (coneNonNegativeCoeffs.size>0)
      outputNormal.MakeZero(coneNonNegativeCoeffs[0].size);
    return true;
  }
  int numRows= coneStrictlyPositiveCoeffs.size + coneNonNegativeCoeffs.size;
  matA.init((int)numRows, (int)theDimension*2+numRows);
  matA.NullifyAll();
  matb.init((int)numRows, 1);
  matb.NullifyAll();
  for (int i=0; i<coneStrictlyPositiveCoeffs.size; i++)
  { for (int k=0; k<theDimension; k++)
    { matA.elements[i][k].Assign(coneStrictlyPositiveCoeffs.TheObjects[i].TheObjects[k]);
      matA.elements[i][k+theDimension].Assign(matA.elements[i][k]);
      matA.elements[i][k+theDimension].Minus();
    }
    matb.elements[i][0].MakeOne();
    matA.elements[i][theDimension*2+i].MakeMOne();
  }
  for (int i=0; i<coneNonNegativeCoeffs.size; i++)
  { int currentRow=i+coneStrictlyPositiveCoeffs.size;
    for (int k=0; k<theDimension; k++)
    { matA.elements[currentRow][k].Assign(coneNonNegativeCoeffs.TheObjects[i].TheObjects[k]);
      matA.elements[currentRow][k+theDimension].Assign(matA.elements[currentRow][k]);
      matA.elements[currentRow][k+theDimension].Minus();
    }
    matA.elements[currentRow][2*theDimension+currentRow].MakeOne();
  }
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  bool result=Matrix<Rational>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(matA, matb, &matX, theGlobalVariables);
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  outputNormal.MakeZero(theDimension);
  for (int i=0; i<theDimension; i++)
    outputNormal[i]=matX[i]-matX[i+theDimension];
  if (result)
  { Rational tempRat;
    for(int i=0; i<coneStrictlyPositiveCoeffs.size; i++)
    { coneStrictlyPositiveCoeffs[i].ScalarEuclidean(outputNormal, tempRat);
      if(!tempRat.IsPositive())
        crash << crash;
    }
    for(int i=0; i<coneNonNegativeCoeffs.size; i++)
    { coneNonNegativeCoeffs[i].ScalarEuclidean(outputNormal, tempRat);
      if (!tempRat.IsNonPositive())
        crash << crash;
    }
  }
//  outputNormal.ComputeDebugString();
  return result;
}

template<class coefficient>
void Matrix<coefficient>::ComputePotentialChangeGradient
(Matrix<coefficient>& matA, Selection& BaseVariables, int NumTrueVariables, int ColumnIndex, Rational& outputChangeGradient, bool& hasAPotentialLeavingVariable)
{ hasAPotentialLeavingVariable = false;
  outputChangeGradient.MakeZero();
  for (int j=0; j<matA.NumRows; j++)
  { if (BaseVariables.elements[j]>=NumTrueVariables)
      outputChangeGradient+=(matA.elements[j][ColumnIndex]);
    hasAPotentialLeavingVariable =hasAPotentialLeavingVariable || matA.elements[j][ColumnIndex].IsPositive();
  }
  if (ColumnIndex>=NumTrueVariables)
    outputChangeGradient-=1;
}

template<class Element>
void Matrix<Element>::GetMaxMovementAndLeavingVariableRow
(Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables, Matrix<Element>& tempMatA, Vector<Element>& inputVectorX, Selection& BaseVariables)
{ LeavingVariableRow=-1;
  maxMovement.MakeZero();
  for(int i=0; i<tempMatA.NumRows; i++)
  { Rational tempRat;
    tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
    if (tempRat.IsPositive())
    { tempRat.Invert();
      tempRat.MultiplyBy(inputVectorX[BaseVariables.elements[i]]);
      if (maxMovement.IsGreaterThan(tempRat)|| (LeavingVariableRow==-1 ))
      { maxMovement.Assign(tempRat);
        LeavingVariableRow=i;
      }
    }
  }
}

template <typename Element>
inline void Matrix<Element>::ActOnMonomialAsDifferentialOperator(const MonomialP& input, Polynomial<Rational>& output)
{ if(this->NumRows!=this->NumCols)
    crash << crash;
  MonomialP tempMon;
  output.MakeZero();
  Rational coeff;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempMon=input;
      coeff=tempMon(j);
      coeff*=this->elements[i][j];
      tempMon[j]-=1;
      tempMon[i]+=1;
      output.AddMonomial(tempMon, coeff);
    }
}

template <typename Element>
void Matrix<Element>::GetZeroEigenSpaceModifyMe(List<Vector<Element> >& output)
{ if (this->NumRows==0)
  { output.SetSize(this->NumCols);
    for (int i=0; i<this->NumCols; i++)
      output[i].MakeEi(this->NumCols, i);
    return;
  }
  Selection nonPivotPts;
  this->GaussianEliminationByRows(0, &nonPivotPts);
  output.SetSize(nonPivotPts.CardinalitySelection);
  for (int i=0; i<nonPivotPts.CardinalitySelection; i++)
  { int currentPivotIndex = nonPivotPts.elements[i];
    output[i].MakeEi(this->NumCols, currentPivotIndex);
    int rowCounter=0;
    for (int j=0; j<this->NumCols; j++)
      if (!nonPivotPts.selected[j])
      { output[i][j]-=this->elements[rowCounter][currentPivotIndex];
        rowCounter++;
      }
  }
}

template <class coefficient>
std::string Vectors<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  std::string tempS;
  bool useLaTeX=false;
  bool useHtml=false;
  bool makeTable=false;
  if (theFormat!=0)
  { useLaTeX=theFormat->flagUseLatex;
    useHtml=theFormat->flagUseHTML;
//    makeTable=theFormat->flagma
  }
  if (!useLaTeX && !useHtml)
    out << this->size << " vectors: \n";
  if (useLaTeX && makeTable)
    out << "\\begin{tabular}{c}";
  if (useHtml && makeTable)
    out << "<table>";
  for (int i=0; i<this->size; i++)
  { tempS=this->TheObjects[i].ToString(theFormat);
    if (useHtml && makeTable)
      out << "<tr><td>";
    out << tempS;
    if (!makeTable && i!=this->size-1)
      out << ", ";
    if (useLaTeX && makeTable)
      out << "\\\\\n";
    if (useHtml && makeTable)
      out << "</td></tr>";
  }
  if (useHtml && makeTable)
    out << "</table>";
  if (useLaTeX && makeTable)
    out << "\\end{tabular}";
  return out.str();
}

template <class Object>
void List<Object>::SubSelection(Selection& theSelection, List<Object>& output)
{ if (&output==this)
  { List<Object> thisCopy=*this;
    thisCopy.SubSelection(theSelection, output);
    return;
  }
  output.SetSize(theSelection.CardinalitySelection);
  for(int i=0; i<theSelection.CardinalitySelection; i++)
    output[i]=(*this)[theSelection.elements[i]];
}

template <class Object>
void List<Object>::IntersectWith(const List<Object>& other, List<Object>& output)const
{ if (&output==&other || this==&output)
  { List<Object> l1=*this;
    List<Object> l2=other;
    l1.IntersectWith(l2, output);
    return;
  }
  HashedList<Object> tempList;
  tempList=*this;
  output.SetSize(0);
  for (int i=0; i<other.size; i++)
    if (tempList.Contains(other[i]))
      output.AddOnTop(other[i]);
}

template <class coefficient>
std::string Vector<coefficient>::ToStringLetterFormat(const std::string& inputLetter, FormatExpressions* theFormat, bool DontIncludeLastVar)const
{ if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  bool found=false;
  int NumVars= DontIncludeLastVar ? this->size-1 : this->size;
  for(int i=0; i<NumVars; i++)
    if (!this->TheObjects[i].IsEqualToZero())
    { tempS=this->TheObjects[i].ToString(theFormat);
      if (this->TheObjects[i].NeedsBrackets())
        tempS="("+tempS+")";
      if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
      if (found)
      { if (tempS.size()>0)
        { if (tempS[0]!='-')
            out << "+";
        } else
          out << "+";
      }
      found=true;
      out << tempS;
      if (theFormat!=0)
        if (theFormat->vectorSpaceEiBasisNames.size>i)
        { out << theFormat->vectorSpaceEiBasisNames[i];
          continue;
        }
      out << inputLetter << "_{" << i+1<< "}";
    }
  return out.str();
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
void MonomialTensor<coefficient, inputHashFunction>::MultiplyByGeneratorPowerOnTheLeft(int theGeneratorIndexStandsOnTheLeft, const coefficient& thePower)
{ if (thePower==0)
    return;
  List<int> newGeneratorIndices;
  List<coefficient> newPowers;
  newGeneratorIndices.SetExpectedSize(this->generatorsIndices.size+1);
  newPowers.SetExpectedSize(this->generatorsIndices.size+1);
  newGeneratorIndices.AddOnTop(theGeneratorIndexStandsOnTheLeft);
  newPowers.AddOnTop(thePower);
  newGeneratorIndices.AddListOnTop(this->generatorsIndices);
  newPowers.AddListOnTop(this->Powers);
  this->generatorsIndices=newGeneratorIndices;
  this->Powers=newPowers;
  this->SimplifyEqualConsecutiveGenerators(0);
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
void MonomialTensor<coefficient, inputHashFunction>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower)
{ if (thePower==0)
    return;
  if (this->generatorsIndices.size>0)
    if (*this->generatorsIndices.LastObject()==theGeneratorIndex)
    { (*this->Powers.LastObject())+=(thePower);
      return;
    }
  this->Powers.AddOnTop(thePower);
  this->generatorsIndices.AddOnTop(theGeneratorIndex);
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
std::string MonomialTensor<coefficient, inputHashFunction>::ToString(FormatExpressions* theFormat)const
{ if (this->generatorsIndices.size==0)
    return "1";
  std::string theLetter= theFormat==0 ?  "g" : theFormat->chevalleyGgeneratorLetter;
  std::stringstream out;
  for (int i=0; i< this->generatorsIndices.size; i++)
  { out << theLetter << "_{" << this->generatorsIndices[i] << "}";
    if (!(this->Powers[i]==1))
      out << "^{" << this->Powers[i] << "}";
  }
  return out.str();
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
bool MonomialTensor<coefficient, inputHashFunction>::SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex)
{ if (this->generatorsIndices.size<1)
    return false;
  if (lowestNonReducedIndex<0)
    lowestNonReducedIndex=0;
  bool result=false;
  for (int next=lowestNonReducedIndex+1; next<this->generatorsIndices.size; next++)
    if (this->generatorsIndices[lowestNonReducedIndex]==this->generatorsIndices[next])
    { result=true;
      this->Powers[lowestNonReducedIndex]+=this->Powers[next];
    }
    else
    { lowestNonReducedIndex++;
      this->Powers[lowestNonReducedIndex]=this->Powers[next];
      this->generatorsIndices[lowestNonReducedIndex]=this->generatorsIndices[next];
    }
  this->generatorsIndices.SetSize(lowestNonReducedIndex+1);
  this->Powers.SetSize(lowestNonReducedIndex+1);
  return result;
}

template <typename Element>
std::string Matrix<Element>::ToStringLatex(FormatExpressions* theFormat)const
{ FormatExpressions formatCopy;
  if (theFormat!=0)
    formatCopy=*theFormat;
  formatCopy.flagUseLatex=true;
  return this->ToString(&formatCopy);
}

template <typename Element>
std::string Matrix<Element>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  std::string tempS;
  bool useHtml= theFormat==0 ? true : theFormat->flagUseHTML;
  bool useLatex= theFormat==0 ? false : theFormat->flagUseLatex;
  if (useHtml)
    out << "<table>";
  if (useLatex)
  { int verticalLineIndex= theFormat==0 ? -1 : theFormat->MatrixColumnVerticalLineIndex;
    out << "\\left(\\begin{array}{";
    for (int j=0; j<this->NumCols; j++)
    { out << "c";
      if (verticalLineIndex==j)
        out << "|";
    }
    out << "}";
  }
  for (int i=0; i<this->NumRows; i++)
  { if (useHtml)
      out << "<tr>";
    for (int j=0; j<this->NumCols; j++)
    { tempS=(*this)(i,j).ToString(theFormat);
      if (useHtml)
        out << "<td>";
      out << tempS;
      if (useLatex)
      { if (j!=this->NumCols-1)
          out << " & ";
        else
          out << "\\\\";
      }
      if (useHtml)
        out << "</td>";
      if (!useHtml && !useLatex)
        out << ", \t ";
    }
    if (useHtml)
      out << "</tr>";
    out << "\n";
  }
  if (useHtml)
    out << "</table>";
  if (useLatex)
    out << "\\end{array}\\right)";
  return out.str();
}

template <class TemplateMonomial, class coefficient>
std::string MonomialCollection<TemplateMonomial, coefficient>::GetBlendCoeffAndMon
(const TemplateMonomial& inputMon, coefficient& inputCoeff, bool addPlusToFront, FormatExpressions* theFormat)
{ std::stringstream out;
  std::string coeffStr=inputCoeff.ToString();
  if (inputMon.IsAConstant())
  { if (coeffStr[0]!='-' && addPlusToFront)
      out << "+" << coeffStr;
    else
      out << coeffStr;
    return out.str();
  }
  std::string monString=inputMon.ToString(theFormat);
  if (coeffStr=="1")
  { if (addPlusToFront)
      out << "+";
    out << monString;
    return out.str();
  }
  if (coeffStr=="-1")
  { out << "-" << monString;
    return out.str();
  }
  if (coeffStr[0]!='-' && addPlusToFront)
    out << "+" << coeffStr << monString;
  else
    out << coeffStr << monString;
  return out.str();
}

template <class TemplateMonomial, class coefficient>
std::string MonomialCollection<TemplateMonomial, coefficient>::ToString(FormatExpressions* theFormat)const
{ if (this->size()==0)
    return "0";
  MacroRegisterFunctionWithName("MonomialCollection::ToString");
  std::stringstream out;
  std::string tempS1, tempS2;
  List<TemplateMonomial> sortedMons;
  sortedMons=this->theMonomials;
  //If this line fails to link, you must do the following.
  //You need to implement FormatExpressions::GetMonOrder<TemplateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  //wish to use a custom one.
  typename List<TemplateMonomial>::OrderLeftGreaterThanRight
  theOrder= theFormat==0? 0: theFormat->GetMonOrder<TemplateMonomial>();
  sortedMons.QuickSortDescending(theOrder);
//  out << "(hash: " << this->HashFunction() << ")";
  int cutOffCounter=0;
  bool useCustomPlus=false;
  bool useCustomTimes=false;
  int MaxLineLength=theFormat==0? 200 : theFormat->MaxLineLength;
  int NumAmpersandsPerNewLineForLaTeX=theFormat==0? 1: theFormat->NumAmpersandsPerNewLineForLaTeX;
  bool flagUseLaTeX=theFormat==0? false: theFormat->flagUseLatex;
  bool flagUseHTML=theFormat==0? false: theFormat->flagUseHTML;
  std::string oldCustomTimes="";
  if (theFormat!=0)
  { useCustomPlus=(theFormat->CustomPlusSign!="");
    useCustomTimes=(theFormat->CustomCoeffMonSeparator!="");
    if (theFormat->flagPassCustomCoeffMonSeparatorToCoeffs==false)
    { oldCustomTimes=theFormat->CustomCoeffMonSeparator;
      theFormat->CustomCoeffMonSeparator="";
    }
  }
  for (int i=0; i<sortedMons.size; i++)
  { TemplateMonomial& currentMon=sortedMons[i];
    coefficient& currentCoeff=this->theCoeffs[this->theMonomials.GetIndex(currentMon)];
    if (currentCoeff.NeedsBrackets())
      tempS1="("+currentCoeff.ToString(theFormat)+ ")";
    else
      tempS1=currentCoeff.ToString(theFormat);
    tempS2=currentMon.ToString(theFormat);
    if (tempS2!="")
    { if (!useCustomTimes)
      { if (tempS1=="1" && tempS2!="1")
          tempS1="";
        if (tempS1=="-1"&& tempS2!="1")
          tempS1="-";
        if(tempS2!="1")
          tempS1+=tempS2;
      } else
      { tempS1+=oldCustomTimes;
        tempS1+=tempS2;
      }
    }
    if (i>0)
    { if (!useCustomPlus)
      { if (tempS1.size()>0)
        { if (tempS1[0]!='-')
          { out << "+";
            cutOffCounter+=1;
          }
        } else
        { out << "+";
          cutOffCounter+=1;
        }
      } else
        out << theFormat->CustomPlusSign;
    }
    out << tempS1;
    cutOffCounter+=tempS1.size();
    if (MaxLineLength>0)
      if (cutOffCounter>MaxLineLength)
      { cutOffCounter=0;
        if (flagUseLaTeX && i!=sortedMons.size-1)
        { out << " \\\\";
          for (int k=0; k<NumAmpersandsPerNewLineForLaTeX; k++)
            out << "&";
          out << " ";
        }
        if (flagUseHTML && !flagUseLaTeX)
          out << " <br>";
      }
  }
  if (theFormat!=0)
    theFormat->CustomCoeffMonSeparator=oldCustomTimes;
  return out.str();
}

template <typename Element>
inline void Matrix<Element>::AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const Element& scalar, GlobalVariables* theGlobalVariables)
{ ProgressReport theReport(theGlobalVariables);
  bool doProgressReport=
  theGlobalVariables==0 ? false : theGlobalVariables->flagGaussianEliminationProgressReport;
  Element tempElement;
  for (int i = StartColIndex; i< this->NumCols; i++)
  { tempElement=this->elements[fromRowIndex][i];
    tempElement*=scalar;
    if (doProgressReport)
    { std::stringstream out;
      out << "Processing row, element " << i+1 << " out of " << this->NumCols;
      theReport.Report(out.str());
    }
    this->elements[ToRowIndex][i]+=tempElement;
  }
}

template <typename Element>
void Matrix<Element>::GaussianEliminationByRows(Matrix<Element>* carbonCopyMat, Selection* outputNonPivotColumns, Selection* outputPivotColumns, GlobalVariables* theGlobalVariables)
{ //Checking for bees
  if (this->NumRows==0)
    crash << "This is a programming error: requesting to do Gaussian elimination on a matrix with "
    << " zero rows. " << crash;
  if (carbonCopyMat!=0)
    if (carbonCopyMat->NumRows!=this->NumRows)
      crash << "This is a programming error: requesting to do Gaussian elimination with carbon copy, however the matrix has "
      << this->NumRows << " rows, while the carbon copy has " << carbonCopyMat->NumRows << " rows. " << crash;
  ///////////////////
  int tempI;
  int NumFoundPivots = 0;
  int MaxRankMat = MathRoutines::Minimum(this->NumRows, this->NumCols);
  Element tempElement;
  if (outputNonPivotColumns!=0)
    outputNonPivotColumns->init(this->NumCols);
  if (outputPivotColumns!=0)
    outputPivotColumns->init(this->NumCols);
  bool doProgressReport= theGlobalVariables==0 ? false : theGlobalVariables->flagGaussianEliminationProgressReport;
  ProgressReport theReport(theGlobalVariables);
  //Initialization done! Time to do actual work:
  for (int i=0; i<this->NumCols; i++)
  { if (NumFoundPivots == MaxRankMat)
    { if (outputNonPivotColumns!=0)
        for (int j =i; j<this->NumCols; j++)
          outputNonPivotColumns->AddSelectionAppendNewIndex(j);
      return;
    }
    tempI = this->FindPivot(i, NumFoundPivots);
    if (tempI==-1)
    { if (outputNonPivotColumns!=0)
        outputNonPivotColumns->AddSelectionAppendNewIndex(i);
      continue;
    }
    if (outputPivotColumns!=0)
      outputPivotColumns->AddSelectionAppendNewIndex(i);
    this->SwitchTwoRows(NumFoundPivots, tempI);
    if (carbonCopyMat!=0)
      carbonCopyMat->SwitchTwoRows(NumFoundPivots, tempI);
    tempElement=this->elements[NumFoundPivots][i];
    tempElement.Invert();
    this->RowTimesScalar(NumFoundPivots, tempElement);
    if (carbonCopyMat!=0)
      carbonCopyMat->RowTimesScalar(NumFoundPivots, tempElement);
    for (int j = 0; j<this->NumRows; j++)
      if (j!=NumFoundPivots)
        if (!this->elements[j][i].IsEqualToZero())
        { tempElement=(this->elements[j][i]);
          tempElement.Minus();
          if (doProgressReport)
          { std::stringstream reportStream;
            reportStream << "Gaussian elimination (" << this->NumRows << "x" << this->NumCols << "): column " << i+1 << " out of " << this->NumCols
            << ".\n<br>Pivot row: " << NumFoundPivots+1 << ", eliminating row " << j+1 << " out of " << this->NumRows;
            theReport.Report(reportStream.str());
          }
          this->AddTwoRows(NumFoundPivots, j, i, tempElement, theGlobalVariables);
          if (carbonCopyMat!=0)
            carbonCopyMat->AddTwoRows(NumFoundPivots, j, 0, tempElement, theGlobalVariables);
          //if(!tempElement.checkConsistency())
          //  crash << crash;
          //this->ComputeDebugString();
        }
    NumFoundPivots++;
  }
}

unsigned int MathRoutines::ListIntsHash(const List<int>& input)
{ unsigned int result=0;
  for (int i =0; i<input.size; i++)
    result+=SomeRandomPrimes[i]*input[i];
  return result;
}

class Lattice
{
  void TestGaussianEliminationEuclideanDomainRationals(Matrix<Rational> & output);
public:
  inline static const std::string GetXMLClassName()
  { return "Lattice";
  }
  Matrix<Rational> basisRationalForm;
  Matrix<LargeInt> basis;
  LargeIntUnsigned Den;
  int GetDim()const{return this->basis.NumCols;}
  int GetRank()const{return this->basis.NumRows;}
  void IntersectWith(const Lattice& other);
  bool FindOnePreimageInLatticeOf
    (const Matrix<Rational>& theLinearMap, const Vectors<Rational>& input, Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
;
  void IntersectWithPreimageOfLattice
  (const Matrix<Rational> & theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
;
  void IntersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator);
  static bool GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection
  (Vector<Rational>& startingPoint, Vector<Rational>& theAffineHyperplane, Vector<Rational>& theDirection, Vector<Rational>& outputPoint)
  ;
  void GetDefaultFundamentalDomainInternalPoint(Vector<Rational>& output);
  bool GetInternalPointInConeForSomeFundamentalDomain
(Vector<Rational>& output, Cone& coneContainingOutputPoint, GlobalVariables& theGlobalVariables)
  ;
  void GetRootOnLatticeSmallestPositiveProportionalTo
(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
  ;
  void GetRougherLatticeFromAffineHyperplaneDirectionAndLattice
  (const Vector<Rational>& theDirection, Vector<Rational>& outputDirectionMultipleOnLattice, Vector<Rational>& theShift, Vector<Rational>& theAffineHyperplane,
   Vectors<Rational>& outputRepresentatives,
   Vectors<Rational>& movementInDirectionPerRepresentative,
   Lattice& outputRougherLattice,
   GlobalVariables& theGlobalVariables)
     ;
  void ApplyLinearMap
  (Matrix<Rational>& theMap, Lattice& output)
  ;
  void IntersectWithBothOfMaxRank(const Lattice& other);
  void GetDualLattice(Lattice& output)const;
  bool IsInLattice(const Vector<Rational>& theVector)const
  { Vector<Rational> tempVect=theVector;
    if (!this->ReduceVector(tempVect))
      return false;
    return tempVect.IsEqualToZero();
  }
  //returns false if the vector is not in the vector space spanned by the lattice
  bool ReduceVector(Vector<Rational>& theVector) const;
  //In the following two functions, the format of the matrix theSub of the substitution is as follows.
  //Let the ambient dimension be n, and the coordinates be x_1,..., x_n.
  //Let the new vector space be of dimension m, with coordinates y_1,..., y_m.
  //Then theSub is an n by m matrix, where the i^th row of the matrix gives the expression of x_i via the y_j's.
  //In addition, we require that n>=m (otherwise, in general, we do not expect to get a lattice).
  //For example, if we want to carry out the substitution
  //x_1=y_1+y_2, x_2=y_1-y_2, x_3=y_1, then
  //theSub should be initialized as:
  //1  1
  //1 -1
  //1  0
  bool SubstitutionHomogeneous
    (const Matrix<Rational>& theSub, GlobalVariables& theGlobalVariables)
;
  bool SubstitutionHomogeneous
    (const PolynomialSubstitution<Rational>& theSub, GlobalVariables& theGlobalVariables)
;
//the following function follows the same convention as the preceding except that we allow n<m. However,
// in order to assure that the preimage of the lattice is a lattice,
//we provide as input an ambient lattice in the new vector space of dimension m
  bool SubstitutionHomogeneous(const Matrix<Rational>& theSub, Lattice& resultIsSubsetOf, GlobalVariables& theGlobalVariables);
  void Reduce();
  void IntersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis);
  void IntersectWithLinearSubspaceGivenByNormals(const Vectors<Rational>& theSubspaceNormals);
  void IntersectWithLinearSubspaceGivenByNormal(const Vector<Rational>& theNormal);
  static bool GetHomogeneousSubMatFromSubIgnoreConstantTerms
  (const PolynomialSubstitution<Rational>& theSub, Matrix<Rational>& output, GlobalVariables& theGlobalVariables)
  ;
  //returning false means that the lattice given as rougher is not actually rougher than the current lattice
  //or that there are too many representatives
  bool GetAllRepresentatives
  (const Lattice& rougherLattice, Vectors<Rational>& output)const
  ;
  bool GetAllRepresentativesProjectingDownTo
  (const Lattice& rougherLattice, Vectors<Rational>& startingShifts, Vectors<Rational>& output)const
  ;
  inline std::string ToString()const{return this->ToString(true, false);}
  std::string ToString(bool useHtml, bool useLatex)const;
  bool operator==(const Lattice& other){return this->basisRationalForm==other.basisRationalForm;}
  void operator=(const Lattice& other)
  { this->basis=other.basis;
    this->Den=other.Den;
    this->basisRationalForm=other.basisRationalForm;
  }
  void WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
  ;
  bool ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables)
  ;
  void MakeZn(int theDim);
  void RefineByOtherLattice(const Lattice& other);
  void MakeFromRoots
  (const Vectors<Rational>& input)
  ;
  Lattice(){}
  Lattice(const Lattice& other){this->operator=(other);}
  void MakeFromMat
  (const Matrix<Rational>& input)
  ;
};

class QuasiPolynomial
{
public:
  inline static std::string GetXMLClassName(){ return "Quasipolynomial";}
  int GetNumVars()const{return this->AmbientLatticeReduced.basis.NumRows;}
  GlobalVariables* buffers;
  Lattice AmbientLatticeReduced;
  Vectors<Rational> LatticeShifts;
  std::string DebugString;
  List<Polynomial<Rational> > valueOnEachLatticeShift;
  std::string ToString(bool useHtml, bool useLatex){return this->ToString(useHtml, useLatex, 0);}
  std::string ToString(bool useHtml, bool useLatex, FormatExpressions* thePolyFormat);
  void ComputeDebugString(){this->DebugString=this->ToString(false, false);}
  Rational Evaluate(const Vector<Rational>& input);
  void AddLatticeShift(const Polynomial<Rational>& input, const Vector<Rational>& inputShift);
  void AddAssumingLatticeIsSame(const QuasiPolynomial& other);
  void MakeRougherLattice(const Lattice& latticeToRoughenBy);
  void MakeFromPolyShiftAndLattice
  (const Polynomial<Rational>& inputPoly, const MonomialP& theShift, const Lattice& theLattice, GlobalVariables& theGlobalVariables);
  void MakeZeroLatTiceZn(int theDim);
  void MakeZeroOverLattice(Lattice& theLattice);
//  bool ExtractLinearMapAndTranslationFromSub
//  ()
 // ;
  bool IsEqualToZero()const {return this->valueOnEachLatticeShift.size==0;}
  void Substitution
  (const Matrix<Rational> & mapFromNewSpaceToOldSpace, const Vector<Rational> & inputTranslationSubtractedFromArgument,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  void Substitution
  (const Matrix<Rational>& mapFromNewSpaceToOldSpace,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  void Substitution
  (const Vector<Rational>& inputTranslationSubtractedFromArgument,
   QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
  ;
  bool SubstitutionLessVariables
  (const PolynomialSubstitution<Rational>& theSub, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
  ;
  void operator+=(const QuasiPolynomial& other);
  QuasiPolynomial(){this->buffers=0;}
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  QuasiPolynomial(const QuasiPolynomial& other){this->operator=(other);}
  void operator*=(const Rational& theConst);
  void operator=(const QuasiPolynomial& other)
  { this->AmbientLatticeReduced=other.AmbientLatticeReduced;
    this->LatticeShifts=other.LatticeShifts;
    this->valueOnEachLatticeShift=other.valueOnEachLatticeShift;
    this->buffers=other.buffers;
  }

};

class PartFraction: ListLight<oneFracWithMultiplicitiesAndElongations>
{
private:
  void findPivot();
  void findInitialPivot();
  //void intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent);
  bool rootIsInFractionCone (PartFractions& owner, Vector<Rational>* theRoot, GlobalVariables& theGlobalVariables)const;
  friend class PartFractions;
  friend class partFractionPolynomialSubstitution;
public:
  std::string DebugString;
  int LastDistinguishedIndex;
  int FileStoragePosition;
  bool PowerSeriesCoefficientIsComputed;
  bool IsIrrelevant;
  bool RelevanceIsComputed;
  List<int> IndicesNonZeroMults;
  friend std::ostream& operator << (std::ostream& output, const PartFraction& input)
  { output << " Not implemented, please fix. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    return output;
  }
  static const bool IsEqualToZero(){return false;}
  bool RemoveRedundantShortRootsClassicalRootSystem
  (PartFractions& owner, Vector<Rational>* Indicator, Polynomial<LargeInt>& buffer1, int theDimension, GlobalVariables& theGlobalVariables)
  ;
  bool RemoveRedundantShortRoots(PartFractions& owner, Vector<Rational>* Indicator, GlobalVariables& theGlobalVariables, int theDimension);
  bool AlreadyAccountedForInGUIDisplay;
  static bool flagAnErrorHasOccurredTimeToPanic;
//  static  bool flagUsingPrecomputedOrlikSolomonBases;
  static std::fstream TheBigDump;
  static bool UseGlobalCollector;
  static bool MakingConsistencyCheck;
  static Rational CheckSum, CheckSum2;
  static Vector<Rational> theVectorToBePartitioned;
//  static ListPointers<PartFraction> GlobalCollectorPartFraction;
  void ComputePolyCorrespondingToOneMonomial
  (QuasiPolynomial& outputQP, const MonomialP& theMon, Vectors<Rational>& normals,
   Lattice& theLattice, GlobalVariables& theGlobalVariables)const
  ;
  static void EvaluateIntPoly
  (const Polynomial<LargeInt>& input, const Vector<Rational>& values, Rational& output)
  ;
  static void MakePolynomialFromOneNormal
  (Vector<Rational>& normal, const MonomialP& shiftRational, int theMult,
   Polynomial<Rational>& output);
  void ComputeNormals
  (PartFractions& owner, Vectors<Rational>& output, int theDimension, Matrix<Rational>& buffer)
  ;
  int ComputeGainingMultiplicityIndexInLinearRelation
  (bool flagUsingOrlikSolomon, Matrix<Rational>& theLinearRelation)
  ;
  void GetRootsFromDenominator
  (PartFractions& owner, Vectors<Rational>& output)const
;
  void GetVectorPartitionFunction
  (PartFractions& owner, Polynomial<LargeInt>& theCoeff, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
  ;
  LargeInt EvaluateIntPolyAtOne(Polynomial<LargeInt>& input);
  //void InsertNewRootIndex(int index);
  //void MultiplyMinusShiftBy (int* theRoot, int Multiplicity);
  void MultiplyCoeffBy(Rational& r);
  void decomposeAMinusNB
  (int indexA, int indexB, int n, int indexAminusNB, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
   GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator, PartFractions& owner)
   ;
  bool DecomposeFromLinRelation
  (Matrix<Rational> & theLinearRelation, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
   Vector<Rational>* Indicator, GlobalVariables& theGlobalVariables, bool flagUsingOSbasis, List<Vector<Rational> >& startingVectors)
   ;
  void ComputeOneCheckSuM
  (PartFractions& owner, Rational& output, int theDimension, GlobalVariables& theGlobalVariables)const
  ;
  bool ReduceMeOnce
  (const Polynomial<LargeInt>& myCoeff, Polynomial<LargeInt>& outputCoeff, GlobalVariables& theGlobalVariables,
   Vector<Rational>* Indicator, Vectors<Rational>& startingVectors)
  ;
  void ReduceMonomialByMonomial(PartFractions& owner, int myIndex, GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  void ApplySzenesVergneFormulA
  (List<Vector<Rational> >& startingVectors,  List<int>& theSelectedIndices, List<int>& theElongations, int GainingMultiplicityIndex,
   int ElongationGainingMultiplicityIndex, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
   GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator)
  ;
  void ApplyGeneralizedSzenesVergneFormulA
  (List<int>& theSelectedIndices, List<int>& theGreatestElongations, List<int>& theCoefficients,
   int GainingMultiplicityIndex, int ElongationGainingMultiplicityIndex, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
   GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator, List<Vector<Rational> >& startingVectors)
   ;
  bool CheckForOrlikSolomonAdmissibility(List<int>& theSelectedIndices);
  bool reduceOnceTotalOrderMethod
  (MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables,
   Vector<Rational>* Indicator, PartFractions& owner)
   ;
//  void reduceOnceOrlikSolomonBasis(PartFractions&Accum);
  bool reduceOnceGeneralMethodNoOSBasis
  (PartFractions& owner, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables,
   Vector<Rational>* Indicator, Vectors<Rational>& bufferVectors, Matrix<Rational>& bufferMat)
   ;
  bool ReduceOnceGeneralMethod
  (PartFractions& owner, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables,
   Vector<Rational>* Indicator, Vectors<Rational>& bufferVectors, Matrix<Rational>& bufferMat)
  ;
  bool AreEqual(PartFraction& p);
  bool IsReduced();
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const PartFraction& input)
  { return input.HashFunction();
  }
  int MultiplyByOneFrac(oneFracWithMultiplicitiesAndElongations& f);
  void init(int numRoots);
  //int Elongate(int indexElongatedFraction, int theElongation);
  void ComputeIndicesNonZeroMults();
  bool DecreasePowerOneFrac(int index, int increment);
  //void GetNumerator(Polynomial<Rational> & output);
  //void SetNumerator(Polynomial<Rational> & input);
  void PrepareFraction
  (int indexA, int indexB, int AminusNBindex, bool indexAisNullified, PartFraction& output,
   Polynomial<LargeInt>& AminusNbetaPoly, Polynomial<LargeInt>& outputCommonCoeff)
  ;
  void Assign(const PartFraction& p);
  void AssignDenominatorOnly(const PartFraction& p);
  void AssignNoIndicesNonZeroMults(PartFraction& p);
  int getSmallestNonZeroIndexGreaterThanOrEqualTo(PartFractions& owner, int minIndex);
  int ControlLineSizeFracs(std::string& output, FormatExpressions& PolyFormatLocal);
  int ControlLineSizeStringPolys(std::string& output, FormatExpressions& PolyFormatLocal);
  //void swap(int indexA, int indexB);
  PartFraction();
  ~PartFraction();
  void GetPolyReduceMonomialByMonomial(PartFractions& owner, GlobalVariables& theGlobalVariables, Vector<Rational>& theExponent, int StartMonomialPower, int DenPowerReduction, int startDenominatorPower, Polynomial<LargeInt>& output);
  void ReduceMonomialByMonomialModifyOneMonomial
  (PartFractions& Accum, GlobalVariables& theGlobalVariables, SelectionWithDifferentMaxMultiplicities& thePowers,
   List<int>& thePowersSigned, MonomialP& input, LargeInt& inputCoeff)
  ;
  void GetAlphaMinusNBetaPoly
  (PartFractions& owner, int indexA, int indexB, int n, Polynomial<LargeInt>& output)
  ;
  void GetNElongationPolyWithMonomialContribution
  (List<Vector<Rational> >& startingVectors, List<int>& theSelectedIndices, List<int>& theCoefficients,
   List<int>& theGreatestElongations, int theIndex, Polynomial<LargeInt>& output, int theDimension)
  ;
  void GetNElongationPoly
  (List<Vector<Rational> >& startingVectors, int index, int baseElongation, int LengthOfGeometricSeries,
   Polynomial<LargeInt>& output, int theDimension)
  ;
  static void GetNElongationPoly(Vector<Rational>& exponent, int n, Polynomial<LargeInt>& output, int theDimension);
  int GetNumProportionalVectorsClassicalRootSystems(PartFractions& owner);
  bool operator==(const PartFraction& right)const;
  void operator=(const PartFraction& right);
  bool initFromRoots(PartFractions& owner, Vectors<Rational>& input);
  std::string ToString
  (PartFractions& owner, bool LatexFormat, bool includeVPsummand, bool includeNumerator,
   FormatExpressions& PolyFormatLocal, GlobalVariables& theGlobalVariables, int& NumLinesUsed)
   ;
  void ReadFromFile(PartFractions& owner, std::fstream& input, GlobalVariables* theGlobalVariables, int theDimension);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)const;
  int SizeWithoutDebugString()const;
};

class Cone
{
  void ComputeVerticesFromNormalsNoFakeVertices(GlobalVariables* theGlobalVariables=0);
  bool EliminateFakeNormalsUsingVertices(int theDiM, int numAddedFakeWalls, GlobalVariables* theGlobalVariables=0);
public:
  inline static const std::string GetXMLClassName()
  { return "Cone";
  }
  bool flagIsTheZeroCone;
  Vectors<Rational> Vertices;
  Vectors<Rational> Normals;
//  bool flagHasSufficientlyManyVertices;
  int LowestIndexNotCheckedForChopping;
  int LowestIndexNotCheckedForSlicingInDirection;
  std::string ToString(FormatExpressions* theFormat=0)const;
  void TransformToWeylProjective(ConeComplex& owner, GlobalVariables& theGlobalVariables);
  std::string DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  void GetLinesContainedInCone(Vectors<Rational>& output);
//  void GetOrthogonalComplementCone(Cone& output, GlobalVariables* theGlobalVariables=0);
  void TranslateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector);
  bool IsAnHonest1DEdgeAffine(const Vector<Rational>& vertex1, const Vector<Rational>& vertex2)const
  { int numCommonWalls=0;
    for (int i=0; i<this->Normals.size; i++)
      if(vertex1.ScalarEuclidean(this->Normals[i]).IsEqualToZero() &&
         vertex2.ScalarEuclidean(this->Normals[i]).IsEqualToZero())
      { numCommonWalls++;
        if (numCommonWalls==this->GetDim()-2)
          return true;
      }
    return false;
  }
  bool IsTheEntireSpace()
  { return this->Normals.size==0 && this->flagIsTheZeroCone;
  }
  bool IsAnHonest1DEdgeAffine(int vertexIndex1, int vertexIndex2)const
  { Vector<Rational>& vertex1=this->Vertices[vertexIndex1];
    Vector<Rational>& vertex2=this->Vertices[vertexIndex2];
    return this->IsAnHonest1DEdgeAffine(vertex1, vertex2);
  }
  bool DrawMeLastCoordAffine(bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat, int ChamberWallColor=0)const;
  bool DrawMeProjective(Vector<Rational>* coordCenterTranslation, bool initTheDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)const;
  bool IsInCone(const Vector<Rational>& point) const;
  bool IsInCone(const Vectors<Rational>& vertices)const
  { for (int i=0; i<vertices.size; i++)
      if (!this->IsInCone(vertices[i]))
        return false;
    return true;
  }
  bool GetLatticePointsInCone
  (Lattice& theLattice, Vector<Rational>& theShift, int upperBoundPointsInEachDim, bool lastCoordinateIsOne,
   Vectors<Rational>& outputPoints, Vector<Rational>* shiftAllPointsBy)const;
  bool MakeConvexHullOfMeAnd(const Cone& other, GlobalVariables& theGlobalVariables);
  void ChangeBasis(Matrix<Rational>& theLinearMap, GlobalVariables& theGlobalVariables);
  std::string DebugString;
  int GetDim()const
  { if (this->Normals.size==0)
      return 0;
    return this->Normals[0].size;
  }
  void SliceInDirection(Vector<Rational>& theDirection, ConeComplex& output, GlobalVariables& theGlobalVariables);
  bool CreateFromNormalS
  (Vectors<Rational>& inputNormals, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables* theGlobalVariables=0);
  //returns false if the cone is non-proper, i.e. when either
  //1) the cone is empty or is of smaller dimension than it should be
  //2) the resulting cone is the entire space
  bool CreateFromNormals(Vectors<Rational>& inputNormals, GlobalVariables* theGlobalVariables=0)
  { return this->CreateFromNormalS(inputNormals, false, theGlobalVariables);
  }
  bool CreateFromVertices(const Vectors<Rational>& inputVertices, GlobalVariables* theGlobalVariables=0);
  void GetInternalPoint(Vector<Rational>& output)const
  { if (this->Vertices.size<=0)
      return;
    this->Vertices.sum(output, this->Vertices[0].size);
  }
  Vector<Rational> GetInternalPoint()const
  { Vector<Rational> result;
    this->GetInternalPoint(result);
    return result;
  }
  unsigned int HashFunction()const
  { return this->Vertices.HashFunction();
  }
  static inline unsigned int HashFunction(const Cone& input)
  { return input.HashFunction();
  }
  bool ProduceNormalFromTwoNormalsAndSlicingDirection
  (Vector<Rational> & SlicingDirection, Vector<Rational>& normal1, Vector<Rational>& normal2, Vector<Rational>& output);
  bool ReadFromFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(std::fstream& input, Vectors<Rational>& buffer, GlobalVariables* theGlobalVariables);
  void operator=(const Cone& other)
  { //this->flagHasSufficientlyManyVertices=other.flagHasSufficientlyManyVertices;
    this->flagIsTheZeroCone=other.flagIsTheZeroCone;
    this->Vertices=other.Vertices;
    this->Normals=other.Normals;
    this->LowestIndexNotCheckedForSlicingInDirection=other.LowestIndexNotCheckedForSlicingInDirection;
    this->LowestIndexNotCheckedForChopping=other.LowestIndexNotCheckedForChopping;
  }
  Cone(const Cone& other)
  { this->operator=(other);
  }
  Cone()
  { this->LowestIndexNotCheckedForSlicingInDirection=0;
    this->LowestIndexNotCheckedForChopping=0;
    this->flagIsTheZeroCone=true;
    //this->flagHasSufficientlyManyVertices=true;
  }
  void IntersectAHyperplane(Vector<Rational>& theNormal, Cone& outputConeLowerDim, GlobalVariables& theGlobalVariables);
  bool GetRootFromLPolyConstantTermGoesToLastVariable(Polynomial<Rational>& inputLPoly, Vector<Rational>& output);
  bool SolveLPolyEqualsZeroIAmProjective(Polynomial<Rational>& inputLPoly, Cone& outputCone, GlobalVariables& theGlobalVariables);
  bool SolveLQuasiPolyEqualsZeroIAmProjective(QuasiPolynomial& inputLQP, List<Cone>& outputConesOverEachLatticeShift, GlobalVariables& theGlobalVariables);
  bool operator>(const Cone& other)const
  { return this->Normals>other.Normals;
  }
  bool operator==(const Cone& other)const
  { return this->flagIsTheZeroCone==other.flagIsTheZeroCone && this->Normals==other.Normals;
  }
};

class ConeLatticeAndShift
{
  public:
  inline static std::string GetXMLClassName()
  { return "ConeLatticeShift";
  }
  Cone theProjectivizedCone;
  Lattice theLattice;
  Vector<Rational> theShift;
  void FindExtremaInDirectionOverLatticeOneNonParam
  (Vector<Rational>& theLPToMaximizeAffine, Vectors<Rational>& outputAppendLPToMaximizeAffine, List<ConeLatticeAndShift>& outputAppend, GlobalVariables& theGlobalVariables);
  void operator=(const ConeLatticeAndShift& other)
  { this->theProjectivizedCone=other.theProjectivizedCone;
    this->theLattice=other.theLattice;
    this->theShift=other.theShift;
  }
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase
  (Vector<Rational>& theLPToMaximizeAffine, Vectors<Rational>& outputAppendLPToMaximizeAffine, List<ConeLatticeAndShift>& outputAppend,
   Matrix<Rational>& theProjectionLatticeLevel, GlobalVariables& theGlobalVariables);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  std::string ToString(FormatExpressions& theFormat);
  int GetDimProjectivized()
  { return this->theProjectivizedCone.GetDim();
  }
  int GetDimAffine()
  { return this->theProjectivizedCone.GetDim()-1;
  }
};

class ConeComplex : public HashedList<Cone>
{
public:
  bool flagChambersHaveTooFewVertices;
  bool flagIsRefined;
//  List<int> NonRefinedChambers;
  int indexLowestNonRefinedChamber;
  HashedList<Vector<Rational> > splittingNormals;
  Vectors<Rational> slicingDirections;
  Cone ConvexHull;
  std::string DebugString;
  void RefineOneStep(GlobalVariables& theGlobalVariables);
  void Refine(GlobalVariables& theGlobalVariables);
  void RefineMakeCommonRefinement(const ConeComplex& other, GlobalVariables& theGlobalVariables);
  void Sort(GlobalVariables& theGlobalVariables);
  void RefineAndSort(GlobalVariables& theGlobalVariables);
  void FindMaxmumOverNonDisjointChambers(Vectors<Rational>& theMaximaOverEachChamber, Vectors<Rational>& outputMaxima, GlobalVariables& theGlobalVariables);
  void MakeAffineAndTransformToProjectiveDimPlusOne(Vector<Rational>& affinePoint, ConeComplex& output, GlobalVariables& theGlobalVariables);
  void TransformToWeylProjective(GlobalVariables& theGlobalVariables);
  int GetDim()
  { if (this->size<=0)
      return -1;
    return this->TheObjects[0].GetDim();
  }
  bool AddNonRefinedChamberOnTopNoRepetition(const Cone& newCone, GlobalVariables& theGlobalVariables);
  void PopChamberSwapWithLast(int index);
  void GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(Vectors<Rational>& output)const;
  bool DrawMeLastCoordAffine(bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  void TranslateMeMyLastCoordinateAffinization(Vector<Rational> & theTranslationVector, GlobalVariables& theGlobalVariables);
  void InitFromDirectionsAndRefine(Vectors<Rational>& inputVectors, GlobalVariables& theGlobalVariables);
  void InitFromAffineDirectionsAndRefine(Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints, GlobalVariables& theGlobalVariables);
  std::string DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  bool DrawMeProjective(Vector<Rational>* coordCenterTranslation, bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string ToString()
  { return this->ToString(false, false);
  }
  std::string ToString(bool useLatex, bool useHtml);
  void ComputeDebugString()
  { this->DebugString=this->ToString();
  }
  int GetLowestIndexchamberContaining(const Vector<Rational>& theRoot)const
  { for (int i=0; i<this->size; i++)
      if (this->TheObjects[i].IsInCone(theRoot))
        return i;
    return -1;
  }
  bool findMaxLFOverConeProjective
  (const Cone& input, List<Polynomial<Rational> >& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables);
  bool findMaxLFOverConeProjective
  (const Cone& input, Vectors<Rational>& inputLFsLastCoordConst, List<int>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables);
  void initFromCones
  (List<Vectors<Rational> >& NormalsOfCones, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables);
  void initFromCones
  (List<Cone>& NormalsOfCones, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables);
  bool SplitChamber
  (int indexChamberBeingRefined, bool weAreSlicingInDirection, bool weAreChopping, Vector<Rational>& killerNormal, GlobalVariables& theGlobalVariables);
  void GetNewVerticesAppend
  (Cone& myDyingCone, Vector<Rational>& killerNormal, HashedList<Vector<Rational> >& outputVertices, GlobalVariables& theGlobalVariables);
  void init()
  { this->splittingNormals.Clear();
    this->slicingDirections.size=0;
    this->Clear();
    this->indexLowestNonRefinedChamber=0;
    this->ConvexHull.Normals.size=0;
    this->ConvexHull.Vertices.size=0;
    this->ConvexHull.flagIsTheZeroCone=true;
  }
  ConeComplex(const ConeComplex& other)
  { this->operator=(other);
  }
  ConeComplex()
  { this->flagChambersHaveTooFewVertices=false;
    this->flagIsRefined=false;
  }
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
  { this->WriteToFile(output, theGlobalVariables, -1);
  }
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimit);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { return this->ReadFromFile(input, theGlobalVariables, -1);
  }
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes);
  void operator=(const ConeComplex& other)
  { this->::HashedList<Cone>::operator=(other);
    this->splittingNormals=other.splittingNormals;
    this->slicingDirections=other.slicingDirections;
    this->indexLowestNonRefinedChamber=other.indexLowestNonRefinedChamber;
    this->flagIsRefined=other.flagIsRefined;
    this->flagChambersHaveTooFewVertices=other.flagChambersHaveTooFewVertices;
  }
};

class PartFractions: public MonomialCollection<PartFraction, Polynomial<LargeInt> >
{ bool splitPartial(GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  void initCommon();
  PartFractions(const PartFractions& other);
public:
  int AmbientDimension;
  int IndexLowestNonProcessed;
  int IndexCurrentlyProcessed;
  int HighestIndex;
  int NumberIrrelevantFractions;
  int NumberRelevantReducedFractions;
  int NumMonomialsInTheNumerators;
  int NumGeneratorsInTheNumerators;
  int NumRelevantNonReducedFractions;
  //int NumRelevantMonomialsInNonReducedFractions;
  //int NumRelevantGeneratorsInNonReducedFractions;
  static int NumMonomialsInNumeratorsRelevantFractions;
  int NumGeneratorsRelevenatFractions;
  int NumMonomialsInNumeratorsIrrelevantFractions;
  int NumGeneratorsIrrelevantFractions;
  int NumTotalReduced;
  int NumProcessedForVPFfractions;
  int NumRunsReduceMonomialByMonomial;
  //GlobalVariables theGlobalVariables;
  static int NumProcessedForVPFMonomialsTotal;
  static std::fstream ComputedContributionsList;
  static const int MaxReadFileBufferSize= 33554432; //= 32 MB of read buffer size
  std::string DebugString;
  Rational StartCheckSum;
  Rational EndCheckSum;
  static Rational CheckSum;
  bool flagDiscardingFractions;
  bool flagUsingOrlikSolomonBasis;
  bool flagInitialized;
  int LimitSplittingSteps;
  int SplitStepsCounter;
  ConeComplex theChambers;
  static  bool flagSplitTestModeNoNumerators;
  static  bool flagAnErrorHasOccurredTimeToPanic;
  static  bool flagMakingProgressReport;
  static  bool flagUsingCheckSum;
  static int flagMaxNumStringOutputLines;

  HashedList<Vector<Rational> > startingVectors;
  Matrix<int> TableAllowedAminusB;
  Matrix<int> TableAllowedAminus2B;
  Selection IndicesRedundantShortRoots;
  List<int> IndicesDoublesOfRedundantShortRoots;
  int NumNonRedundantShortRoots;
  Vector<Rational> weights;
  void initFromRoots(Vectors<Rational>& theAlgorithmBasis, Vector<Rational>* theWeights);
  int AddRootAndSort(Vector<Rational>& theRoot);
  int AddRootPreserveOrder(Vector<Rational>& theRoot);
  int getIndex(const Vector<Rational>& TheRoot);
  int getIndexDoubleOfARoot(const Vector<Rational>& TheRoot);
  void ComputeTable(int theDimension);
  void PrepareCheckSums(GlobalVariables& theGlobalVariables);
  std::string DoTheFullComputationReturnLatexFileString
  (GlobalVariables& theGlobalVariables, Vectors<Rational>& toBePartitioned, FormatExpressions& theFormat, std::string* outputHtml);
  bool ArgumentsAllowed(Vectors<Rational>& theArguments, std::string& outputWhatWentWrong, GlobalVariables& theGlobalVariables);
  bool AssureIndicatorRegularity(GlobalVariables& theGlobalVariables, Vector<Rational> & theIndicator);
  void CompareCheckSums(GlobalVariables& theGlobalVariables);
  void ComputeDebugString(GlobalVariables& theGlobalVariables);
  void ComputeDebugStringNoNumerator(GlobalVariables& theGlobalVariables);
  void ComputeDebugStringWithVPfunction(GlobalVariables& theGlobalVariables);
  void ComputePolyCorrespondingToOneMonomial
  (QuasiPolynomial& outputQP, int monomialIndex, Vectors<Rational>& normals, Lattice& theLattice, GlobalVariables& theGlobalVariables);
  void ComputeDebugStringBasisChange(Matrix<LargeInt>& VarChange, GlobalVariables& theGlobalVariables);
  bool initFromRoots(Vectors<Rational>& input, GlobalVariables& theGlobalVariables);
  void initAndSplit(Vectors<Rational>& input, GlobalVariables& theGlobalVariables);
  void Run(Vectors<Rational>& input, GlobalVariables& theGlobalVariables);
  //row index is the index of the Vector<Rational> ; column(second) index is the coordinate index
  void RemoveRedundantShortRootsClassicalRootSystem(GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  void RemoveRedundantShortRoots(GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  bool RemoveRedundantShortRootsIndex
  (MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, int theIndex, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator);
  bool splitClassicalRootSystem(bool ShouldElongate, GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  bool split(GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  void ComputeKostantFunctionFromWeylGroup
  (char WeylGroupLetter, int WeylGroupNumber, QuasiPolynomial& output, Vector<Rational>* ChamberIndicator, bool UseOldData, bool StoreToFile, GlobalVariables&  theGlobalVariables);
  bool IsHigherThanWRTWeight
  (const Vector<Rational>& left, const Vector<Rational>& r, const Vector<Rational>& theWeights);
  void ComputeSupport(List<Vectors<Rational> >& output, std::stringstream& outputString);
  void ComputeOneCheckSum(Rational& output, GlobalVariables& theGlobalVariables);
  void AccountPartFractionInternals(int sign, int index, Vector<Rational> * Indicator, GlobalVariables& theGlobalVariables);
  void PopIndexHashChooseSwapByLowestNonProcessedAndAccount(int index, GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  void PopIndexSwapLastHashAndAccount(int index, GlobalVariables& theGlobalVariables, Vector<Rational> * Indicator);
  void PrepareIndicatorVariables();
  void initFromOtherPartFractions(PartFractions& input, GlobalVariables& theGlobalVariables);
  void IncreaseHighestIndex(int increment);
  std::string ToString(GlobalVariables& theGlobalVariables, FormatExpressions& theFormat){std::string tempS; this->ToString(tempS, theGlobalVariables, theFormat);  return tempS;}
  void ToString(std::string& output, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat);
  int ToString(std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat);
  int ElementToStringBasisChange
  (Matrix<LargeInt>& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, bool includeVPsummand,
   bool includeNumerator, GlobalVariables& theGlobalVariables, FormatExpressions& PolyFormatLocal);
  int ElementToStringOutputToFile(std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables);
  int ElementToStringBasisChangeOutputToFile(Matrix<LargeInt>& VarChange, bool UsingVarChange, std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables);
  bool GetVectorPartitionFunction
  (QuasiPolynomial& output, Vector<Rational> & newIndicator, GlobalVariables& theGlobalVariables)
  ;
  bool VerifyFileComputedContributions(GlobalVariables& theGlobalVariables);
  void WriteToFileComputedContributions(std::fstream& output, GlobalVariables&  theGlobalVariables);
  int ReadFromFileComputedContributions(std::fstream& input, GlobalVariables&  theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void ResetRelevanceIsComputed()
  { crash << "This is not implemented yet. " << crash;
  }
  PartFractions();
  int SizeWithoutDebugString();
  bool CheckForMinimalityDecompositionWithRespectToRoot(Vector<Rational>  *theRoot, GlobalVariables& theGlobalVariables);
  void MakeProgressReportSplittingMainPart(GlobalVariables& theGlobalVariables);
  void MakeProgressReportRemovingRedundantRoots(GlobalVariables& theGlobalVariables);
  void MakeProgressReportUncoveringBrackets(GlobalVariables& theGlobalVariables);
  void MakeProgressVPFcomputation(GlobalVariables& theGlobalVariables);
};

class DynkinSimpleType
{ friend std::ostream& operator << (std::ostream& output, const DynkinSimpleType& theMon)
  { output << theMon.ToString();
    return output;
  }
  public:
  char theLetter;
  int theRank;
  Rational CartanSymmetricInverseScale;
  DynkinSimpleType(): theLetter('X'), theRank(-1), CartanSymmetricInverseScale(0){}
  DynkinSimpleType(char inputChar, int inputRank, const Rational& inputScale=1)
  : theLetter(inputChar), theRank(inputRank), CartanSymmetricInverseScale(inputScale)
  {}
  void MakeAone()
  { this->theLetter='A';
    this->theRank=1;
    this->CartanSymmetricInverseScale=1;
  }
  int GetRootSystemSize()const;
  int GetRootSystemPlusRank()const
  { return this->GetRootSystemSize()+this->theRank;
  }
  int GetSSAlgDim()const
  { return this->GetRootSystemSize()+this->theRank;
  }
  void MakeArbitrary(char inputLetter, int inputRank, Rational inputLengthFirstCorRootSquared)
  { if ((inputLetter!= 'A' && inputLetter!='B' && inputLetter!= 'C' && inputLetter!='D' &&
         inputLetter!= 'E' && inputLetter!='F' && inputLetter!= 'G') || inputRank<=0 )
      crash << "This is a programming error. Requested to create a simple Dynkin type of type " << inputLetter << " and rank "
      << inputRank << ". This is not allowed: I only accept types A, B, C, D, E, F and G and non-negative ranks. " << crash;
    if (inputLetter=='G')
      inputRank=2;
    if (inputLetter=='F')
      inputRank=4;
    if (inputLetter=='E' && inputRank>8)
      inputRank=8;
    if (inputRank==1)
      inputLetter='A';
    this->theRank=inputRank;
    this->theLetter=inputLetter;
    this->CartanSymmetricInverseScale=inputLengthFirstCorRootSquared;
  }
  void GetCoCartanSymmetric(Matrix<Rational>& output)const;
  static void GetCoCartanSymmetric(const Matrix<Rational>& input, Matrix<Rational>& output);
  void GetCartanSymmetric(Matrix<Rational>& output)const;
  void GetAn(int n, Matrix<Rational>& output)const;
  void GetBn(int n, Matrix<Rational>& output)const;
  void GetCn(int n, Matrix<Rational>& output)const;
  void GetDn(int n, Matrix<Rational>& output)const;
  void GetEn(int n, Matrix<Rational>& output)const;
  void GetF4(Matrix<Rational>& output)const;
  void GetG2(Matrix<Rational>& output)const;
  void Grow(List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots)const;
//  bool IsPossibleCoRootLength(const Rational& input)const;
  void operator=(const DynkinSimpleType& other)
  { this->theLetter=other.theLetter;
    this->theRank=other.theRank;
    this->CartanSymmetricInverseScale=other.CartanSymmetricInverseScale;
  }
  bool operator==(const DynkinSimpleType& other)const
  { return
    this->theLetter==other.theLetter && this->theRank==other.theRank &&
    this->CartanSymmetricInverseScale==other.CartanSymmetricInverseScale;
  }
  static unsigned int HashFunction(const DynkinSimpleType& input)
  { return ((unsigned int)
    input.theLetter)*2+input.theRank +SomeRandomPrimes[0]*input.CartanSymmetricInverseScale.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  inline bool IsEqualToZero()const
  { return false;
  }
  void GetAutomorphismActingOnVectorColumn(MatrixTensor<Rational>& output, int AutoIndex)const;
  Rational GetDefaultCoRootLengthSquared(int rootIndex)const;
  Rational GetDefaultRootLengthSquared(int rootIndex)const;
  Rational GetLongRootLengthSquared()const;

  Rational GetRatioRootSquaredToFirstSquared(int rootIndex)const;
  static Rational GetRatioLongRootToFirst(char inputWeylLetter, int inputRank);
  bool CanBeExtendedParabolicallyTo(const DynkinSimpleType& otherType)const;
  bool HasEasySubalgebras()const;
  Rational GetRatioLongRootToFirst()const
  { return this->GetRatioLongRootToFirst(this->theLetter, this->theRank);
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  void operator++(int);
  //DynksinSimpleTypes are compared as follows.
  //First, we compare the length of the first co-root. Larger co-root length= larger simple type.
  //Second, we comare ranks. Larger rank = larger simple type. Note that an sl(2) with large co-root length will be considered
  //larger than an sl(100) with small root length.
  //Finally, we compare types. The convention here is: larger root system=larger type. In other words, A<D<B<C<E<F<G
  bool operator>(const DynkinSimpleType& other)const;
  static void GetEpsilonMatrix(char WeylLetter, int WeylRank, Matrix<Rational>& output);
  inline bool operator<(const DynkinSimpleType& other)const
  { return other>*this;
  }
  bool operator<(int otherRank)const;
};

//this class needs a fix: it should not inherit monomial collection, rather have a monomial collection as a private member.
//Most important operations with Dynkin types require fixed order of monomils, which makes using monomial collections
// a bad design decision.
class DynkinType: public MonomialCollection<DynkinSimpleType, Rational>
{
public:
  void GetLettersTypesMults
  (List<char>* outputLetters=0, List<int>* outputRanks=0, List<int>* outputMults=0, List<Rational>* outputFirstCoRootLengthsSquared=0)const;
  void GetTypesWithMults(List<DynkinSimpleType>& output)const;
  bool IsOfSimpleType(char inputType, int inputRank)const
  { char currentType;
    int currentRank;
    if (!this->IsSimple(&currentType, &currentRank))
      return false;
    return currentType==inputType && currentRank==inputRank;
  }
  static void GetOuterAutosGeneratorsOneTypeActOnVectorColumn(List<MatrixTensor<Rational> >& output, const DynkinSimpleType& theType, int multiplicity);
  void GetOuterAutosGeneratorsActOnVectorColumn(List<MatrixTensor<Rational> >& output);
  bool IsSimple(char* outputtype=0, int* outputRank=0, Rational* outputLength=0)const;
  void GetSortedDynkinTypes(List<DynkinSimpleType>& output)const;
  void SortTheDynkinTypes();
  bool Grow(const List<Rational>& allowedInverseScales, int AmbientWeylDim, List<DynkinType>& output, List<List<int> >* outputPermutationRoots)const;
  bool ContainsType(char theTypeLetter)const;
  void GetDynkinTypeWithDefaultScales(DynkinType& output)const;
  DynkinSimpleType GetGreatestSimpleType()const;
  DynkinSimpleType GetSmallestSimpleType()const;
  Rational GetSizeWeylGroupByFormula()const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  std::string ToStringRelativeToAmbientType(const DynkinSimpleType& ambientType, FormatExpressions* theFormat=0)const;
  void ScaleFirstCoRootSquaredLength(const Rational& multiplyCoRootSquaredLengthBy);
  int GetMult(int SimpleTypeIdentifier)const
  { int result;
    if(!this->theCoeffs[SimpleTypeIdentifier].IsSmallInteger(&result))
      crash << "This is a programming error: Dynkin type has multiplicity that is not a small integer " << crash;
    return result;
  }
  int GetNumSimpleComponents()const;
  Rational GetRankRational()const;
  int GetRank()const;
  int GetRootSystemSize()const
  { Rational result=0;
    for (int i=0; i<this->size(); i++)
      result+=this->theCoeffs[i]*(*this)[i].GetRootSystemSize();
    int intResult;
    if (!result.IsSmallInteger(&intResult))
      crash << "This is a programming error: multiplicity of simple type is not a small integer. " << crash;
    return intResult;
  }
  int GetRootSystemPlusRank()const
  { Rational result=0;
    for (int i=0; i<this->size(); i++)
      result+=this->theCoeffs[i]*(*this)[i].GetRootSystemPlusRank();
    int intResult;
    if (!result.IsSmallInteger(&intResult))
      crash << "This is a programming error: multiplicity of simple type is not a small integer. " << crash;
    return intResult;
  }
  bool IsTypeA_1()const;
  static int GetIndexPreimageFromRootInjection(int inputIndex, const List<int>& inputRootInjection);
  bool CanBeExtendedParabolicallyTo(const DynkinType& other)const;
  bool CanBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other)const;
  void MakeSimpleType(char type, int rank, const Rational* inputFirstCoRootSqLength=0);
  void GetEpsilonMatrix(Matrix<Rational>& output)const;
  void GetCoCartanSymmetric(Matrix<Rational>& output)const;
  void GetCartanSymmetric(Matrix<Rational>& output)const;
  void GetCartanSymmetricDefaultLengthKeepComponentOrder(Matrix<Rational>& output)const;
  int GetCoxeterEdgeWeight(int v, int w);
  std::string GetLieAlgebraName(FormatExpressions* theFormat=0)const;
  std::string GetWeylGroupName(FormatExpressions* theFormat=0)const;
  bool HasExceptionalComponent()const;
  bool operator>(const DynkinType& other)const;
  void operator=(const MonomialCollection<DynkinSimpleType, Rational>& other)
  { this->::MonomialCollection<DynkinSimpleType, Rational>::operator=(other);
  }
};

template <class coefficient>
class WeylGroupRepresentation
{
  private:
  List<Matrix<coefficient> > theElementImages;
  List<bool> theElementIsComputed;
  Vector<coefficient> theCharacter;
  List<Matrix<coefficient> > classFunctionMatrices;
  List<bool> classFunctionMatricesComputed;
  friend class WeylGroup;
  WeylGroupRepresentation* parent;
  Vectors<coefficient> vectorSpaceBasis;
  Matrix<coefficient> gramMatrixInverted;
  public:
  WeylGroup* OwnerGroup;
  List<std::string> names;
  WeylGroupRepresentation():parent(0), OwnerGroup(0){}
  unsigned int HashFunction() const;
  bool CheckInitialization()const;
  bool CheckAllSimpleGensAreOK()const;
  static unsigned int HashFunction(const WeylGroupRepresentation<coefficient>& input)
  { return input.HashFunction();
  }
  void ComputeAllGeneratorImagesFromSimple(GlobalVariables* theGlobalVariables=0);
  Vector<coefficient>& GetCharacter();
  void GetLargestDenominatorSimpleGens(LargeIntUnsigned& outputLCM, LargeIntUnsigned& outputDen)const;
  void reset(WeylGroup* inputOwner);
  void CheckRepIsMultiplicativelyClosed();
  void GetClassFunctionMatrix(Vector<coefficient>& virtualCharacter, Matrix<coefficient>& outputMat, GlobalVariables* theGlobalVariables=0);
  int GetDim()const
  { if (this->theElementImages.size==1)
      return this->theElementImages[0].NumRows;
    return this->theElementImages[1].NumRows;
  }
  void Restrict
  (const Vectors<coefficient>& VectorSpaceBasisSubrep, const Vector<Rational>& remainingCharacter, WeylGroupRepresentation<coefficient>& output,
   GlobalVariables* theGlobalVariables=0);
  bool DecomposeMeIntoIrrepsRecursive(Vector<Rational>& outputIrrepMults, GlobalVariables* theGlobalVariables=0);
  bool DecomposeMeIntoIrreps(Vector<Rational>& outputIrrepMults, GlobalVariables* theGlobalVariables=0);
  coefficient GetNumberOfComponents();
  void operator*=(const WeylGroupRepresentation<coefficient>& other);
  bool operator==(const WeylGroupRepresentation<coefficient>& other)const
  { return this->OwnerGroup==other.OwnerGroup && this->theCharacter==other.theCharacter;
  }
  void SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace);
  std::string GetName() const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  Matrix<coefficient>& GetElementImage(int elementIndex);
  void SetElementImage(int elementIndex, const Matrix<coefficient>& input)
  { this->theElementImages[elementIndex] = input;
    this->theElementIsComputed[elementIndex] = true;
  }
  bool operator>(const WeylGroupRepresentation<coefficient>& other)const
  { return this->theCharacter.isGreaterThanLexicographic(other.theCharacter);
  }
  bool operator<(const WeylGroupRepresentation<coefficient>& other)const
  { return other.theCharacter.isGreaterThanLexicographic(this->theCharacter);
  }
};

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::CheckInitialization()const
{ if (this->OwnerGroup==0)
  { crash << "This is a programming error: working with a representation with non-initialized owner Weyl group. " << crash;
    return false;
  }
  return true;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::reset(WeylGroup* inputOwner)
{ this->OwnerGroup=inputOwner;
  this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  this->theCharacter.SetSize(0);
  this->theElementImages.SetSize(this->OwnerGroup->theElements.size);
  this->theElementIsComputed.initFillInObject(this->OwnerGroup->theElements.size, false);
  this->classFunctionMatrices.SetSize(this->OwnerGroup->conjugacyClasses.size);
  this->classFunctionMatricesComputed.initFillInObject(this->OwnerGroup->conjugacyClasses.size, false);
}

class ElementWeylGroup
{
public:
  WeylGroup* owner;
  List<int> reflections;
  ElementWeylGroup():owner(0)
  {}

  bool CheckInitialization()
  { if (this->owner==0)
    { crash << "This is a programming error: non-initialized element Weyl group. " << crash;
      return false;
    }
    return true;
  }
  void MakeCanonical();
  void ToString(std::string& output)
  { output=this->ToString();
  }
  std::string ToString(FormatExpressions* theFormat=0, List<int>* DisplayIndicesOfSimpleRoots=0)const
  { return this->ToString(-1, theFormat, DisplayIndicesOfSimpleRoots);
  }
  std::string ToString(int NumSimpleGens, FormatExpressions* theFormat=0, List<int>* DisplayIndicesOfSimpleRoots=0)const;
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const ElementWeylGroup& input)
  { return input.HashFunction();
  }
  void operator*=(const ElementWeylGroup& other);
  ElementWeylGroup operator*(const ElementWeylGroup& other) const
  { ElementWeylGroup result=*this;
    result*=other;
    return result;
  }
  Vector<Rational> operator*(const Vector<Rational>& v) const;
  ElementWeylGroup Inverse() const;

  bool operator==(const ElementWeylGroup& other)const
  { if (this->owner!=other.owner)
      return false;
    return this->reflections==other.reflections;
  }
};

template <typename coefficient>
Matrix<coefficient>& WeylGroupRepresentation<coefficient>::GetElementImage(int elementIndex)
{ this->CheckInitialization();
  this->OwnerGroup->CheckInitializationFDrepComputation();
  Matrix<coefficient>& theMat=this->theElementImages[elementIndex];
  if (this->theElementIsComputed[elementIndex])
    return theMat;
  const ElementWeylGroup& theElt=this->OwnerGroup->theElements[elementIndex];
  this->theElementIsComputed[elementIndex]=true;
  theMat.MakeIdMatrix(this->GetDim());
  for (int i=0; i<theElt.reflections.size; i++)
    theMat.MultiplyOnTheLeft(this->theElementImages[theElt.reflections[i]+1]);
  return theMat;
}

class WeylGroup
{
//  Matrix<int> CartanSymmetricIntBuffer;
//  void UpdateIntBuffer()
//  { this->CartanSymmetricIntBuffer.init(this->CartanSymmetric.NumRows, this->CartanSymmetric.NumCols);
//    for (int i=0; i<this->CartanSymmetric.NumRows; i++)
//      for (int j=0; j<this->CartanSymmetric.NumCols; j++)
//        this->CartanSymmetricIntBuffer.elements[i][j]=this->CartanSymmetric.elements[i][j].NumShort;
//  }
  Matrix<Rational> FundamentalToSimpleCoords;
  Matrix<Rational> SimpleToFundamentalCoords;
  MemorySaving<Matrix<Rational> > MatrixSendsSimpleVectorsToEpsilonVectors;
  bool flagFundamentalToSimpleMatricesAreComputed;
  bool flagOuterAutosComputed;
  inline void ComputeFundamentalToSimpleMatrices()
  { if (flagFundamentalToSimpleMatricesAreComputed)
      return;
    Vectors<Rational> fundamentalBasis;
    this->GetFundamentalWeightsInSimpleCoordinates(fundamentalBasis);
    this->FundamentalToSimpleCoords.AssignVectorsToRows(fundamentalBasis);
    this->FundamentalToSimpleCoords.Transpose();
    this->SimpleToFundamentalCoords=this->FundamentalToSimpleCoords;
    this->SimpleToFundamentalCoords.Invert();
    this->flagFundamentalToSimpleMatricesAreComputed=true;
  }
public:
  DynkinType theDynkinType;
  Matrix<Rational> CartanSymmetric;
  Matrix<Rational> CoCartanSymmetric;
  HashedList<ElementWeylGroup> theElements;
  int size;
  Vector<Rational> rho;
  HashedList<Vector<Rational> > rhoOrbit;
  HashedList<Vector<Rational> > RootSystem;
  Vectors<Rational> RootsOfBorel;

  List<MatrixTensor<Rational> > OuterAutomorphisms;
  List<List<int> > conjugacyClasses;
  List<WeylGroupRepresentation<Rational> > irreps;
  List<Vector<Rational> > characterTable;

  static bool flagAnErrorHasOcurredTimeToPanic;
  bool flagDeallocated;
//  void MakeFromParSel(Vector<Rational> & parSel, WeylGroup& input);
  void init()
  { this->flagFundamentalToSimpleMatricesAreComputed=false;
  }
  void ComputeConjugacyClasses(GlobalVariables* theGlobalVariables=0);
  void ComputeIrreducibleRepresentations(GlobalVariables* theGlobalVariables=0);
  void ComputeExtremeRootInTheSameKMod(const Vectors<Rational>& inputSimpleBasisK, const Vector<Rational>& inputRoot, Vector<Rational>& output, bool lookingForHighest);
  void AddIrreducibleRepresentation(const WeylGroupRepresentation<Rational>& p);
  void AddCharacter(const Vector<Rational>& X);
  void ComputeRho(bool Recompute);
  std::string ToString(FormatExpressions* theFormat=0);
  void MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared=0);
  void MakeFromDynkinType(const DynkinType& inputType);
  void MakeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType);
  void ComputeCoCartanSymmetricFromCartanSymmetric();
  void ComputeExternalAutos();
  bool CheckConsistency()const;
  bool CheckInitializationFDrepComputation()const;
  template <typename coefficient>
  coefficient GetHermitianProduct(const Vector<coefficient>& leftCharacter, const Vector<coefficient>& rightCharacter)const;
  template <typename coefficient>
  Vector<coefficient> GetCharacterProduct(const Vector<coefficient>& leftCharacter, const Vector<coefficient>& rightCharacter)const;
  template <typename coefficient>
  inline coefficient GetCharacterNorm
  (const Vector<coefficient>& theCharacter)const
  { return this->GetHermitianProduct(theCharacter, theCharacter);
  }
  void GetSignCharacter(Vector<Rational>& out);
  void StandardRepresentation(WeylGroupRepresentation<Rational>& output);
  void GenerateAdditivelyClosedSubset(Vectors<Rational>& input, Vectors<Rational>& output);
  Rational GetKillingDivTraceRatio();
  Rational EstimateNumDominantWeightsBelow(Vector<Rational> & inputHWsimpleCoords, GlobalVariables& theGlobalVariables);
  bool ContainsARootNonStronglyPerpendicularTo(Vectors<Rational>& theVectors, Vector<Rational>& input);
  int NumRootsConnectedTo(Vectors<Rational>& theVectors, Vector<Rational>& input);
  void GetHighestWeightsAllRepsDimLessThanOrEqualTo(List<Vector<Rational> >& outputHighestWeightsFundCoords, int inputDimBound);
  Rational GetLongestRootLengthSquared();
  static unsigned int HashFunction(const WeylGroup& input)
  { return input.CartanSymmetric.HashFunction();
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  WeylGroup();
  ~WeylGroup()
  { this->flagDeallocated=true;
  }
  bool IsOfSimpleType(char desiredType, int desiredRank)const
  { return this->theDynkinType.IsOfSimpleType(desiredType, desiredRank);
  }
  Matrix<Rational>* GetMatrixFundamentalToSimpleCoords()
  { this->ComputeFundamentalToSimpleMatrices();
    return &this->FundamentalToSimpleCoords;
  }
  Matrix<Rational>* GetMatrixSimpleToFundamentalCoords()
  { this->ComputeFundamentalToSimpleMatrices();
    return &this->SimpleToFundamentalCoords;
  }
  template<class coefficient>
  Vector<coefficient> GetSimpleCoordinatesFromFundamental(const Vector<coefficient>& inputInFundamentalCoords);
  template<class coefficient>
  Vectors<coefficient> GetSimpleCoordinatesFromFundamental(const Vectors<coefficient>& inputInFundamentalCoords);
  template<class coefficient>
  Vector<coefficient> GetFundamentalCoordinatesFromSimple(const Vector<coefficient>& inputInSimpleCoords);
  template<class coefficient>
  Vector<coefficient> GetDualCoordinatesFromSimple(const Vector<coefficient>& inputInSimpleCoords)
  { return this->GetDualCoordinatesFromFundamental(this->GetFundamentalCoordinatesFromSimple(inputInSimpleCoords));
  }
  template <class coefficient>
  Vector<coefficient> GetDualCoordinatesFromFundamental(const Vector<coefficient>& inputInFundamentalCoords)
  { Vector<coefficient> result=inputInFundamentalCoords;
    if (result.size!=this->GetDim())
      crash << "This is a programming error. The input fundamental weight has " << result.size << " coordinates, while the rank of the Weyl group is "
      << this->GetDim() << ". " << crash;
    for (int i=0; i<result.size; i++)
      result[i]*=this->CartanSymmetric.elements[i][i]/2;
    return result;
  }
  template <class coefficient>
  coefficient GetScalarProdSimpleRoot(const Vector<coefficient>& input, int indexSimpleRoot)
  { if (indexSimpleRoot<0 || indexSimpleRoot>=this->GetDim())
      crash << "This is a programming error. Attempting to take scalar product with simple root of index " << indexSimpleRoot
      << " which is impossible, as the rank of the Weyl group is " << this->GetDim() << ". " << crash;
    coefficient result, buffer;
    result=input[0].GetZero();//<-the value of zero is not known at compile time (example: multivariate polynomials have unknown #variables)
    Rational* currentRow=this->CartanSymmetric.elements[indexSimpleRoot];
    for (int i=0; i<input.size; i++)
    { buffer=input[i];
      buffer*=currentRow[i];
      result+=buffer;
    }
    return result;
  }
  template <class coefficient>
  coefficient WeylDimFormulaSimpleCoords(Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit=1);
  template <class coefficient>
  coefficient WeylDimFormulaFundamentalCoords(Vector<coefficient>& weightFundCoords, const coefficient& theRingUnit=1);
  template <class coefficient>
  void RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign=0, bool* stabilizerFound=0, ElementWeylGroup* raisingElt=0);
  bool AreMaximallyDominant(List<Vector<Rational> >& theWeights, bool useOuterAutos);
  template <class coefficient>
  void RaiseToMaximallyDominant(List<Vector<coefficient> >& theWeight, bool useOuterAutos);
  void GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables);
  void GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output)const;
  void GetStandardRepresentationMatrix(int g, Matrix<Rational>& output) const;
  void DrawRootSystem
  (DrawingVariables& outputDV, bool wipeCanvas, GlobalVariables& theGlobalVariables, bool drawWeylChamber, Vector<Rational> * bluePoint=0,
   bool LabelDynkinDiagramVertices=false, Vectors<Rational>* predefinedProjectionPlane=0);
  bool HasStronglyPerpendicularDecompositionWRT
  (Vector<Rational>& input, int UpperBoundNumBetas, Vectors<Rational>& theSet, Vectors<Rational>& output,
   List<Rational>& outputCoeffs, bool IntegralCoefficientsOnly);
//  void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities);
//  void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks){ this->MakeFromDynkinType(theLetters, theRanks, 0); }
  //void GetLongRootLength(Rational& output);
  static bool IsAddmisibleDynkinType(char candidateLetter, int n);
  //the below will not do anything if the inputLetter is not a valid Dynkin letter
  static void TransformToAdmissibleDynkinType(char inputLetter, int& outputRank);
  void GetEpsilonCoords(const Vector<Rational>& input, Vector<Rational>& output);
  void GetEpsilonCoords(const List<Vector<Rational> >& input, Vectors<Rational>& output);
  bool IsStronglyPerpendicularTo(const Vector<Rational>& input, const Vector<Rational>& other);
  bool IsStronglyPerpendicularTo(const Vector<Rational>& input, const Vectors<Rational>& others);
  void GetEpsilonCoordsWRTsubalgebra(Vectors<Rational>& generators, List<Vector<Rational> >& input, Vectors<Rational>& output);
  bool LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup& left, ElementWeylGroup& right);
  void GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output);
  template <class coefficient>
  bool GetAlLDominantWeightsHWFDIM
  (Vector<coefficient> & highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
   int upperBoundDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables);
  template <class coefficient>
  bool FreudenthalEval
  (Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
   List<coefficient>& outputMultsSimpleCoords, std::string* outputDetails, GlobalVariables* theGlobalVariables, int UpperBoundFreudenthal);
  void GetWeylChamber(Cone& output, GlobalVariables& theGlobalVariables);
  std::string GenerateWeightSupportMethoD1
  (Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights, GlobalVariables& theGlobalVariables);
  void GetIntegralLatticeInSimpleCoordinates(Lattice& output);
  void GetFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output);
  inline int GetDim()const{return this->CartanSymmetric.NumRows;}
  void ComputeAllElements(int UpperLimitNumElements=0);
  void ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output);
  void ComputeRootsOfBorel(Vectors<Rational>& output);
  static Rational GetSizeWeylGroupByFormula(char weylLetter, int theDim);
  bool IsARoot(const Vector<Rational>& input)const
  { return this->RootSystem.Contains(input);
  }
  void GenerateRootSubsystem(Vectors<Rational>& theRoots);
  template <class coefficient>
  bool GenerateOuterOrbit
  (Vectors<coefficient>& theRoots, HashedList<Vector<coefficient> >& output, HashedList<ElementWeylGroup>* outputSubset=0,  int UpperLimitNumElements=-1);
  template <class coefficient>
  bool GenerateOrbit
  (Vectors<coefficient>& theRoots, bool RhoAction, HashedList<Vector<coefficient> >& output, bool UseMinusRho, int expectedOrbitSize=-1,
   HashedList<ElementWeylGroup>* outputSubset=0, int UpperLimitNumElements=-1);
//  int GetNumRootsFromFormula();
  void GenerateRootSystem();
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  void ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho);
  void ProjectOnTwoPlane(Vector<Rational> & orthonormalBasisVector1, Vector<Rational> & orthonormalBasisVector2, GlobalVariables& theGlobalVariables);
  void GetLowestElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup* outputWeylElt, Vectors<Rational>& bufferEiBAsis, bool RhoAction, bool UseMinusRho, int* sign=0,
   bool* stabilizerFound=0)
  { this->GetExtremeElementInOrbit(inputOutput, outputWeylElt, bufferEiBAsis, true, RhoAction, UseMinusRho, sign, stabilizerFound);
  }
  void GetHighestElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup* outputWeylElt, Vectors<Rational>& bufferEiBAsis, bool RhoAction,
   bool UseMinusRho, int* sign, bool* stabilizerFound)
  { this->GetExtremeElementInOrbit(inputOutput, outputWeylElt, bufferEiBAsis, false, RhoAction, UseMinusRho, sign, stabilizerFound);
  }
  void GetExtremeElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup* outputWeylElt, Vectors<Rational>& bufferEiBAsis,
   bool findLowest, bool RhoAction, bool UseMinusRho, int* sign, bool* stabilizerFound);
  void GetLongestWeylElt(ElementWeylGroup& outputWeylElt);
  bool IsEigenSpaceGeneratorCoxeterElement(Vector<Rational> & input);
  void GetCoxeterElement(ElementWeylGroup& outputWeylElt)
  { outputWeylElt.reflections.SetSize(this->GetDim());
    for (int i=0; i<outputWeylElt.reflections.size; i++)
      outputWeylElt.reflections[i]=i;
  }
  template <class Element>
  void ActOn(const ElementWeylGroup& theGroupElement, Vector<Element>& theVector)const
  { for (int i=0; i<theGroupElement.reflections.size; i++)
      this->SimpleReflection(theGroupElement.reflections[i], theVector);
  }
  template <class Element>
  void ActOnRhoModified(const ElementWeylGroup& theGroupElement, Vector<Element>& theVector)const
  { for (int i=0; i<theGroupElement.reflections.size; i++)
      this->SimpleReflectionRhoModified(theGroupElement.reflections[i], theVector);
  }
  template <class Element>
  void ActOnRhoModified(int indexOfWeylElement, Vector<Element>& theVector)const
  { this->ActOnRhoModified(this->theElements[indexOfWeylElement], theVector);
  }
  template <class Element>
  void ActOn(int indexOfWeylElement, Vector<Element>& theVector)const
  { this->ActOn(this->theElements[indexOfWeylElement], theVector);
  }
  template <class Element>
  void ActOnDual(int index,Vector<Element>& theVector, bool RhoAction, const Element& theRingZero);
  //theRoot is a list of the simple coordinates of the Vector<Rational>
  //theRoot serves as both input and output
  void ActOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  void ActOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  void ActOnDualSpaceElementByGroupElement(int index, Vector<Rational>& theDualSpaceElement, bool RhoAction);
  void SimpleReflectionRoot(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  template <class Element>
  void SimpleReflection(int index, Vector<Element>& theVector)const;
  template <class Element>
  void SimpleReflectionRhoModified(int index, Vector<Element>& theVector)const;
  template <class Element>
  void SimpleReflectionMinusRhoModified(int index, Vector<Element>& theVector)const;
  int GetRootReflection(int rootIndex);
  void GetGeneratorList(int g, List<int>& out)const;
  int Multiply(int g, int h) const;
  int Invert(int g) const;
  void GetMatrixOfElement(int theIndex, Matrix<Rational>& outputMatrix)const ;
  void GetMatrixOfElement(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix)const;
  void GetElementOfMatrix(Matrix<Rational>& input, ElementWeylGroup &output);
  void SimpleReflectionDualSpace(int index, Vector<Rational>& DualSpaceElement);
  void SimpleReflectionRootAlg(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  bool IsPositiveOrPerpWRTh(const Vector<Rational>& input, const Vector<Rational>& theH)
  { return this->RootScalarCartanRoot(input, theH).IsPositiveOrZero();
  }
  template<class coefficient>
  void ReflectBetaWRTAlpha(const Vector<Rational>& alpha, const Vector<coefficient>& Beta, bool RhoAction, Vector<coefficient>& Output)const;
  bool IsRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot);
  template <class leftType, class rightType>
  void RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output)const;
  double RootScalarCartanRoot(const Vector<double>& r1, const Vector<double>& r2)const
  { if (r1.size!=r2.size || r1.size!=this->GetDim())
    { crash.theCrashReport << "This is a programming error: attempting to take the root system scalar product of "
      << "vectors of different dimension or of dimension different from that of the ambient Lie algebra. The two input vectors were "
      << r1 << " and " << r2 << " and the rank of the Weyl group is " << this->GetDim() << ". ";
      crash << crash;
    }
    double result=0;
    for (int i=0; i<this->GetDim(); i++)
      for (int j=0; j<this->GetDim(); j++)
        result+=this->CartanSymmetric.elements[i][j].DoubleValue()*r1.TheObjects[i]*r2.TheObjects[j];
    return result;
  }
  template <class leftType, class rightType>
  leftType RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2)const
  { leftType tempRat;
    this->RootScalarCartanRoot(r1, r2, tempRat);
    return tempRat;
  }
  //the below functions perturbs input so that inputH has non-zero scalar product with Vectors<Rational> of the Vector<Rational>  system,
  //without changing the inputH-sign of any Vector<Rational>  that had a non-zero scalar product to begin with
  void PerturbWeightToRegularWRTrootSystem(const Vector<Rational>& inputH, Vector<Rational>& output);
  template <class coefficient>
  bool IsDominantWRTgenerator(const Vector<coefficient>& theWeight, int generatorIndex);
  template <class coefficient>
  bool IsDominantWeight(const Vector<coefficient>& theWeight);
  static void TransformToSimpleBasisGenerators(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem);
  static void TransformToSimpleBasisGeneratorsArbitraryCoords(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem);
  void TransformToSimpleBasisGeneratorsWRTh(Vectors<Rational>& theGens, const Vector<Rational>& theH);
  bool operator==(const WeylGroup& other)const;
  void operator+=(const WeylGroup& other);
};

template <class Element>
void WeylGroup::SimpleReflectionRhoModified(int index, Vector<Element>& theVector)const
{ Element alphaShift, tempRat;
  alphaShift=0;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=(theVector[i]);
    tempRat*=this->CartanSymmetric.elements[index][i]*(-2);
    alphaShift+=(tempRat);
  }
  alphaShift/=(this->CartanSymmetric.elements[index][index]);
  alphaShift-=1;
  theVector[index]+=(alphaShift);
}

template <class Element>
void WeylGroup::SimpleReflectionMinusRhoModified(int index, Vector<Element>& theVector)const
{ Element alphaShift, tempRat;
  alphaShift=0;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=theVector[i];
    tempRat*=this->CartanSymmetric.elements[index][i];
    alphaShift+=tempRat;
  }
  alphaShift*=-2;
  alphaShift/=(this->CartanSymmetric.elements[index][index]);
  alphaShift+=(1);
  theVector[index]+=(alphaShift);
}

template <class Element>
void WeylGroup::SimpleReflection(int index, Vector<Element>& theVector)const
{ if (index<0 || index>=this->CartanSymmetric.NumCols)
    crash << "This is a programming error: simple reflection with respect to index " << index+1 << " in a Weyl group of rank "
    << this->GetDim() << crash;
  Element alphaShift, tempRat;
  alphaShift=0;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=theVector[i];
    tempRat*=this->CartanSymmetric.elements[index][i];
    alphaShift+=tempRat;
  }
  alphaShift*=-2;
  alphaShift/=this->CartanSymmetric.elements[index][index];
  theVector[index]+=alphaShift;
}

class ReflectionSubgroupWeylGroup: public HashedList<ElementWeylGroup>
{
public:
  bool truncated;
  WeylGroup AmbientWeyl;
  WeylGroup Elements;
  List<ElementWeylGroup> RepresentativesQuotientAmbientOrder;
  Vectors<Rational> simpleGenerators;
  //format: each element of of the group is a list of generators, reflections with respect to the simple generators, and outer
  //automorphisms.
  //format of the externalAutomorphisms:
  // For a fixed external automorphism (of type Vectors<Rational>) the i^th entry gives the image
  //of the simple root  with 1 on the i^th coordinate
  List<Vectors<Rational> > ExternalAutomorphisms;
  HashedList<Vector<Rational> > RootSubsystem;
  Vectors<Rational> RootsOfBorel;
  void ToString(std::string& output, bool displayElements);
  void GetGroupElementsIndexedAsAmbientGroup(List<ElementWeylGroup>& output);
  std::string ElementToStringBruhatGraph();
  std::string ElementToStringCosetGraph();
  std::string ElementToStringFromLayersAndArrows(List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices);
  std::string ToString(bool displayElements=true)
  { std::string tempS;
    this->ToString(tempS, displayElements);
    return tempS;
  }
  Vector<Rational> GetRho();
  template <class coefficient>
  void RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign=0, bool* stabilizerFound=0);
  template <class coefficient>
  bool FreudenthalEvalIrrepIsWRTLeviPart
  (const Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
   List<coefficient>& outputMultsSimpleCoordS, std::string& outputDetails, GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal);
  bool MakeParabolicFromSelectionSimpleRoots
  (WeylGroup& inputWeyl, const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements);
  void MakeParabolicFromSelectionSimpleRoots
  (WeylGroup& inputWeyl, const Vector<Rational>& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements);
  bool GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra
  (Vector<Rational>& highestWeightSimpleCoords, HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
   int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables);
  template <class coefficient>
  bool GetAlLDominantWeightsHWFDIM
  (Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
   int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables);
  bool DrawContour
  (const Vector<Rational>& highestWeightSimpleCoord, DrawingVariables& theDV, GlobalVariables& theGlobalVariables, int theColor,
   int UpperBoundVertices);
//The dirty C++ language forces that the body of this function appear after the definitions of IsDominantWRTgenerator.
//Apparently the algorithm of making an oriented acyclic graph totally ordered is a too difficult task for the designers of c++
// so I have to do it for them.
  template <class coefficient>
  bool IsDominantWeight(const Vector<coefficient>& theWeight);
  template <class coefficient>
  bool IsDominantWRTgenerator(const Vector<coefficient>& theWeight, int generatorIndex);
  template <class coefficient>
  coefficient WeylDimFormulaSimpleCoords(const Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit=1);
  void FindQuotientRepresentatives(int UpperLimit);
  void GetMatrixOfElement(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix)const;
  template <class coefficient>
  bool GenerateOrbitReturnFalseIfTruncated(const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, int UpperLimitNumElements);
  bool ComputeSubGroupFromGeneratingReflections
  (Vectors<Rational>* inputGenerators, List<Vectors<Rational> >* inputExternalAutos, GlobalVariables* theGlobalVariables, int UpperLimitNumElements,
   bool recomputeAmbientRho);
  void ComputeRootSubsystem();
  template <class coefficient>
  void ActByElement(int index, Vector<coefficient>& theRoot)const;
  template <class coefficient>
  void ActByElement(int index, Vector<coefficient>& input, Vector<coefficient>& output)const
  { this->ActByElement(this->TheObjects[index], input, output);
  }
  template <class coefficient>
  void ActByElement
  (const ElementWeylGroup& theElement, const Vector<coefficient>& input, Vector<coefficient>& output)const;
  template <class coefficient>
  void ActByElement(int index, const Vectors<coefficient>& input, Vectors<coefficient>& output)const
  { this->ActByElement(this->TheObjects[index], input, output);
  }
  template <class coefficient>
  void ActByElement
  (const ElementWeylGroup& theElement, const Vectors<coefficient>& input, Vectors<coefficient>& output)const;
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void operator=(const ReflectionSubgroupWeylGroup& other);
};

template <class coefficient>
bool WeylGroup::IsDominantWeight(const Vector<coefficient>& theWeight)
{ int theDimension= this->GetDim();
  for (int i=0; i<theDimension; i++)
    if (!this->IsDominantWRTgenerator(theWeight, i))
      return false;
  return true;
}

template <class coefficient>
bool ReflectionSubgroupWeylGroup::IsDominantWeight(const Vector<coefficient>& theWeight)
{ for (int i=0; i<this->simpleGenerators.size; i++)
    if (! this->IsDominantWRTgenerator(theWeight, i))
      return false;
  return true;
}

class DynkinDiagramRootSubalgebra
{
public:
//  std::string DynkinStrinG;
  List<Vectors<Rational> > SimpleBasesConnectedComponents;
  //to each connected component of the simple bases corresponds
  //its dynkin string with the same index
  List<DynkinSimpleType> SimpleComponentTypes;
  List<int> indicesThreeNodes;
  List<List<int> > indicesEnds;
  List<List<int> > sameTypeComponents;
  List<int> indexUniComponent;
  List<int> indexInUniComponent;
  std::string ToStringRelativeToAmbientType(const DynkinSimpleType& ambientType)const;
  int RankTotal();
  int NumRootsGeneratedByDiagram();
  void Sort();
  void GetDynkinType(DynkinType& output)const;
  void SwapDynkinStrings(int i, int j);
  bool LetterIsDynkinGreaterThanLetter(char letter1, char letter2);
  //the below function takes as an input a set of roots and computes the corredponding Dynkin diagram of the
  //root  subsystem. Note: the simleBasisInput is required to be a set of simple roots.
  //The procedure calls a
  //transformation to simple basis on the simpleBasisInput, so your input will get changed if it wasn't
  //simple as required!
  inline void ComputeDiagramTypeModifyInput(Vectors<Rational>& simpleBasisInput, WeylGroup& theWeyl)
  { MacroRegisterFunctionWithName("ComputeDiagramTypeModifyInput");
    theWeyl.TransformToSimpleBasisGenerators(simpleBasisInput, theWeyl.RootSystem);
    this->ComputeDiagramTypeKeepInput(simpleBasisInput, theWeyl.CartanSymmetric);
  }
  //the below function is just as the above but doesn't modify simpleBasisInput
  void ComputeDiagramTypeKeepInput(const Vectors<Rational>& simpleBasisInput, const Matrix<Rational>& theBilinearForm);
  void ComputeDiagramTypeModifyInputRelative
  (Vectors<Rational>& inputOutputSimpleWeightSystem, const HashedList<Vector<Rational> >& weightSystem, const Matrix<Rational>& theBilinearForm)
  { MacroRegisterFunctionWithName("ComputeDiagramTypeModifyInputRelative");
    WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords(inputOutputSimpleWeightSystem, weightSystem);
    this->ComputeDiagramTypeKeepInput(inputOutputSimpleWeightSystem, theBilinearForm);
  }
  void ComputeDynkinStrings(const Matrix<Rational>& theBilinearForm);
  void ComputeDynkinString(int indexComponent, const Matrix<Rational>& theBilinearForm);
  int numberOfThreeValencyNodes(int indexComponent, const Matrix<Rational>& theBilinearForm);
  bool operator==(const DynkinDiagramRootSubalgebra& right) const;
  bool IsGreaterThan(DynkinDiagramRootSubalgebra& right);
  Rational GetSizeCorrespondingWeylGroupByFormula();
  void GetAutomorphism(List<List<int> >& output, int index);
  void GetAutomorphisms(List<List<List<int> > >& output);
  void GetMapFromPermutation(Vectors<Rational>& domain, Vectors<Rational>& range, List<int>& thePerm, List<List<List<int> > >& theAutos, SelectionWithDifferentMaxMultiplicities& theAutosPerm, DynkinDiagramRootSubalgebra& right);
};

class VectorPartition
{
public:
  Vectors<Rational> PartitioningRoots;
  Vector<Rational> theRoot;
  //format: each element of thePartitions gives an array whose entries give
  // the multiplicity of the weights. I.e. if PartitioningRoots has 2 elements, then thePartitions[0]
  // would have 2 elements: the first giving the multiplicity of PartitioningRoots[0]
  //and the second - the multiplicity of
  // PartitioningRoots[1]
  List<List<int> > thePartitions;
  std::string DebugString;
  std::string ToString(bool useHtml);
  void ComputeDebugString(bool useHtml)
  { this->DebugString=this->ToString(useHtml);
  }
  int ComputeVectorPartitionFunctionSmall(Vector<Rational>& theRoot, Vectors<Rational>& theRoots);
  void ComputeAllPartitions();
  void ComputeAllPartitionsRecursive(int currentIndex, List<int>& CurrentPartition, int UpperBoundEachIndex, Vector<Rational>& toBePartitioned);
};

class SemisimpleLieAlgebra
{
public:
  bool flagAnErrorHasOccurredTimeToPanic;
  WeylGroup theWeyl;
  //format:
  //the Chevalley constants are listed in the same order as the root system of the Weyl group
  // i.e. if \alpha is the root at the i^th position in this->theWyl.RootSystem and \beta -
  //the root  at the j^th position, then
  //the chevalley constant N_{\alpha\beta} given by [g^\alpha, g^\beta]=N_{\alpha\beta}g^{\alpha+\beta}
  //will be located at the ij^{th} entry in the below matrix.
  //Let $\alpha$ be a root . Then our choice of the elements of the Cartan subalgebra is such that
  //1.   [g^{\alpha}, g^{-\alpha}]=h_\alpha * (2/ \langle\alpha,\alpha\rangle)
  //2.   [h_{\alpha},g^\beta] :=\langle\alpha,\beta\rangle g^\beta
  //Reference: Samelson, Notes on Lie algebras, pages 46-51
  Matrix<Rational> ChevalleyConstants;
  Matrix<bool> Computed;
  //The below gives a total ordering to all generators, including the elements of the Cartan
  //the order is:  We put first the generators corresponding to the negative roots in ascending order,
  //we put second the elements of the Cartan
  //we put last the positive roots in ascending order.
//  Matrix<int> theLiebracketPairingIndices;
  Matrix<ElementSemisimpleLieAlgebra<Rational> > theLiebrackets;
//  List<int> OppositeRootSpaces;
  List<int> UEGeneratorOrderIncludingCartanElts;
  bool flagDeallocated;
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const SemisimpleLieAlgebra& input)
  { return input.theWeyl.HashFunction();
  }
  template <class coefficient>
  void GetGenericElementCartan(ElementSemisimpleLieAlgebra<Polynomial<coefficient> >& output, int indexFirstVar=0)
  { output.MakeZero();
    ChevalleyGenerator theGen;
    Polynomial<coefficient> theCf;
    for (int i=0; i<this->GetRank(); i++)
    { theGen.MakeGenerator(*this, this->GetCartanGeneratorIndex(i));
      theCf.MakeMonomiaL(indexFirstVar+i, 1, 1);
      output.AddMonomial(theGen, theCf);
    }
  }
  template <class coefficient>
  void GetGenericElementNegativeBorelNilradical(ElementSemisimpleLieAlgebra<Polynomial<coefficient> >& output, int indexFirstVar=0)
  { output.MakeZero();
    ChevalleyGenerator theGen;
    Polynomial<coefficient> theCf;
    for (int i=0; i<this->GetNumPosRoots(); i++)
    { theGen.MakeGenerator(*this, i);
      theCf.MakeMonomiaL(indexFirstVar+i, 1, 1);
      output.AddMonomial(theGen, theCf);
    }
  }
  int GetOppositeGeneratorIndex(int theIndex)
  { return this->GetNumGenerators()-theIndex-1;
  }
  bool IsASimpleGenerator(int generatorIndex)
  { return
    (generatorIndex< this->GetNumPosRoots() && generatorIndex>=this->GetNumPosRoots()-this->GetRank()) ||
    (generatorIndex>=this->GetNumPosRoots()+this->GetRank() && generatorIndex<this->GetNumPosRoots()+this->GetRank()*2);
  }
  SemisimpleLieAlgebra()
  { this->flagDeallocated=false;
  }
  ~SemisimpleLieAlgebra()
  { this->flagDeallocated=true;
  }
  bool CheckConsistency()const;
  template <class coefficient>
  void GenerateLieSubalgebra(List<ElementSemisimpleLieAlgebra<coefficient> >& inputOutputGenerators);
  void ComputeMultTable(GlobalVariables& theGlobalVariables);
  bool IsOfSimpleType(char desiredType, int desiredRank)const
  { return this->theWeyl.IsOfSimpleType(desiredType, desiredRank);
  }
  template <class coefficient>
  void GetCommonCentralizer
  (const List<ElementSemisimpleLieAlgebra<coefficient> >& inputElementsToCentralize, List<ElementSemisimpleLieAlgebra<coefficient> >& outputCentralizingElements);
  void GetChevalleyGeneratorAsLieBracketsSimpleGens
  (int generatorIndex, List<int>& outputIndicesFormatAd0Ad1Ad2etc, Rational& outputMultiplyLieBracketsToGetGenerator);
  std::string ToString(FormatExpressions* inputFormat=0);
  std::string GetStringFromChevalleyGenerator(int theIndex, FormatExpressions* thePolynomialFormat)const;
  bool CommutatorIsNonZero(int leftIndex, int rightIndex)
  { return !this->theLiebrackets.elements[leftIndex][rightIndex].IsEqualToZero();
  }
  std::string GetLieAlgebraName()const
  { return this->theWeyl.theDynkinType.GetLieAlgebraName();
  }
  void GetMinusTransposeAuto(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  void GenerateWeightSupportMethod2(Vector<Rational>& theHighestWeight, Vectors<Rational>& output, GlobalVariables& theGlobalVariables);
  inline int GetNumGenerators()const
  { return this->theWeyl.CartanSymmetric.NumRows+this->theWeyl.RootSystem.size;
  }
  inline int GetNumPosRoots()const
  { return this->theWeyl.RootsOfBorel.size;
  }
  inline int GetRank()const
  { return this->theWeyl.CartanSymmetric.NumRows;
  }
  void OrderSetNilradicalNegativeMost(Selection& parSelZeroMeansLeviPart);
  void OrderSetNilradicalNegativeMostReversed(Selection& parSelZeroMeansLeviPart);
  void OrderSSalgebraForHWbfComputation();
  void OrderSSLieAlgebraStandard();
  int GetCartanGeneratorIndex(int simpleRootIndex)
  { return this->theWeyl.RootsOfBorel.size+simpleRootIndex;
  }
  int GetGeneratorFromRoot(const Vector<Rational>& input)
  { return this->GetGeneratorFromRootIndex(this->theWeyl.RootSystem.GetIndex(input));
  }
  int GetRootIndexFromDisplayIndex(int theIndex);
  int GetGeneratorFromDisplayIndex(int theIndex)
  { if (theIndex<0)
      return theIndex+this->GetNumPosRoots();
    return theIndex+this->GetNumPosRoots()+this->GetRank()-1;
  }
  int GetGeneratorFromRootIndex(int theIndex)const;
  int GetDisplayIndexFromRootIndex(int theIndex)const;
  //the below function returns an negative number if the chevalley generator is an element of the Cartan subalgebra
  int GetRootIndexFromGenerator(int theIndex)const;
  int GetCartanIndexFromGenerator(int theIndex)
  { return theIndex+this->theWeyl.RootsOfBorel.size;
  }
  int GetDisplayIndexFromGenerator(int theIndex)const
  { //std::cout << "<br>num pos roots: " <<  this->GetNumPosRoots();
   // std::cout << " rank: "<< this->GetRank();
    if (theIndex<this->GetNumPosRoots())
      return theIndex-this->GetNumPosRoots();
    if (theIndex>=this->GetNumPosRoots()+this->GetRank())
      return theIndex+1-this->GetNumPosRoots()-this->GetRank();
    return theIndex-this->GetNumPosRoots();
  }
  bool AreOrderedProperly(int leftIndex, int rightIndex);
  bool IsGeneratorFromCartan(int theIndex)const
  { return theIndex>=this->GetNumPosRoots() && theIndex<this->GetNumPosRoots()+this->GetRank();
  }
  bool AreOppositeRootSpaces(int leftIndex, int rightIndex)
  { if(this->IsGeneratorFromCartan(leftIndex) || this->IsGeneratorFromCartan(rightIndex))
      return false;
    int left  = this->GetRootIndexFromGenerator(leftIndex);
    int right=this->GetRootIndexFromGenerator(rightIndex);
    return (this->theWeyl.RootSystem[left]+this->theWeyl.RootSystem[right]).IsEqualToZero();
  }
  void GenerateVermaMonomials(Vector<Rational>& highestWeight, GlobalVariables& theGlobalVariables);
  void ComputeChevalleyConstants(GlobalVariables* theGlobalVariables);
  template<class coefficient>
  coefficient GetKillingForm(const ElementSemisimpleLieAlgebra<coefficient>& left, const ElementSemisimpleLieAlgebra<coefficient>& right);
  template <class coefficient>
  coefficient GetKillingFormProductWRTLevi
  (const ElementSemisimpleLieAlgebra<coefficient>& left, const ElementSemisimpleLieAlgebra<coefficient>& right, const Selection& rootsNotInLevi);
  template<class coefficient>
  void LieBracket(const ElementSemisimpleLieAlgebra<coefficient>& g1, const ElementSemisimpleLieAlgebra<coefficient>& g2, ElementSemisimpleLieAlgebra<coefficient>& output);
  //Setup: \gamma+\delta=\epsilon+\zeta=\eta is a Vector<Rational> .
  //then the below function computes n_{-\epsilon, -\zeta}
  void ComputeOneChevalleyConstant(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta);
  void ExploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ);
  void ExploitSymmetryChevalleyConstants(int indexI, int indexJ);
  void ExploitTheCyclicTrick(int i, int j, int k);
  bool GetMaxQForWhichBetaMinusQAlphaIsARoot(const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output)const;
  Rational GetConstant(const Vector<Rational>& root1, const Vector<Rational>& root2);
  bool CheckClosedness(std::string& output, GlobalVariables& theGlobalVariables);
  void ElementToStringVermaMonomials(std::string& output);
  void ElementToStringEmbedding(std::string& output);
  Vector<Rational> GetWeightOfGenerator(int index)
  { if (index<this->GetNumPosRoots())
      return this->theWeyl.RootSystem[index];
    if (index>=this->GetRank()+this->GetNumPosRoots())
      return this->theWeyl.RootSystem[index-this->GetRank()];
    Vector<Rational> result;
    result.MakeZero(this->GetRank());
    return result;
  }
  //returns true if returning constant, false if returning element of h
  bool GetConstantOrHElement(const Vector<Rational> & root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH);
  bool TestForConsistency(GlobalVariables& theGlobalVariables);
  bool AttempTFindingHEF
  (ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputH, ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputE,
   ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& inputOutputF, std::stringstream* logStream=0, GlobalVariables* theGlobalVariables=0);
  bool AttemptExtendingEtoHEFwithHinCartan
  (ElementSemisimpleLieAlgebra<AlgebraicNumber>& theE, ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputH,
   ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF, std::stringstream* logStream=0, GlobalVariables* theGlobalVariables=0);
  bool AttemptExtendingHtoHEFwithHinCartan
  (ElementSemisimpleLieAlgebra<Rational>& theH, ElementSemisimpleLieAlgebra<Rational>& outputE,
   ElementSemisimpleLieAlgebra<Rational>& outputF, GlobalVariables* theGlobalVariables);
  bool AttemptExtendingHEtoHEFWRTSubalgebra
  (Vectors<Rational>& RootsWithCharacteristic2, Selection& theZeroCharacteristics, Vectors<Rational>& simpleBasisSA, Vector<Rational>& h,
   ElementSemisimpleLieAlgebra<Rational>& outputE, ElementSemisimpleLieAlgebra<Rational>& outputF, Matrix<Rational>& outputMatrixSystemToBeSolved,
   PolynomialSubstitution<Rational>& outputSystemToBeSolved, Matrix<Rational>& outputSystemColumnVector, GlobalVariables& theGlobalVariables);
  static void FindSl2Subalgebras(SemisimpleLieAlgebra& inputOwner, SltwoSubalgebras& output, GlobalVariables& theGlobalVariables);
  void GetSl2SubalgebraFromRootSA(GlobalVariables& theGlobalVariables);
  template<class coefficient>
  void GetAd(Matrix<coefficient>& output, ElementSemisimpleLieAlgebra<coefficient>& e);
  void initHEFSystemFromECoeffs
  (int theRelativeDimension, Vectors<Rational>& rootsInPlay, Vectors<Rational>& simpleBasisSA, Vectors<Rational>& SelectedExtraPositiveRoots,
   int numberVariables, int numRootsChar2, int halfNumberVariables, Vector<Rational>& targetH, Matrix<Rational>& inputFCoeffs,
   Matrix<Rational>& outputMatrixSystemToBeSolved, Matrix<Rational>& outputSystemColumnVector, PolynomialSubstitution<Rational>& outputSystemToBeSolved);
  void MakeChevalleyTestReport(int i, int j, int k, int Total, GlobalVariables& theGlobalVariables);
  bool IsInTheWeightSupport(Vector<Rational>& theWeight, Vector<Rational>& highestWeight, GlobalVariables& theGlobalVariables);
  void GenerateOneMonomialPerWeightInTheWeightSupport(Vector<Rational>& theHighestWeight, GlobalVariables& theGlobalVariables);
  void CreateEmbeddingFromFDModuleHaving1dimWeightSpaces(Vector<Rational>& theHighestWeight, GlobalVariables& theGlobalVariables);
  int GetLengthStringAlongAlphaThroughBeta(Vector<Rational>& alpha, Vector<Rational>& beta, int& distanceToHighestWeight, Vectors<Rational>& weightSupport);
  void ComputeOneAutomorphism(GlobalVariables& theGlobalVariables, Matrix<Rational>& outputAuto,  bool useNegativeRootsFirst);
  bool operator==(const SemisimpleLieAlgebra& other)const
  { return this->theWeyl==other.theWeyl;
  }
};

class HomomorphismSemisimpleLieAlgebra
{
public:
  SemisimpleLieAlgebra* domainAlg;
  SemisimpleLieAlgebra* rangeAlg;
  //Let rk:=Rank(Domain)
  //format of ImagesSimpleChevalleyGenerators: the first rk elements give
  //the images of the Chevalley generators corresponding to simple positive roots
  //the second rk elements give the images of the Chevalley generators corresponding to simple
  //negative roots
  List<ElementSemisimpleLieAlgebra<Rational> > imagesSimpleChevalleyGenerators;
  //format of ImagesAllChevalleyGenerators: the Generators are given in the same order as
  //the one used in MonomialUniversalEnveloping
  List<ElementSemisimpleLieAlgebra<Rational> > imagesAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > domainAllChevalleyGenerators;
  List<ElementSemisimpleLieAlgebra<Rational> > GmodK;
  Vectors<Rational> RestrictedRootSystem;
  Vectors<Rational> ImagesCartanDomain;
  SemisimpleLieAlgebra& theDomain()
  { if (domainAlg==0)
      crash << "This is a programming error: non-initialized HomomorphismSemisimpleLieAlgebra. " << crash;
    return *domainAlg;
  }
  SemisimpleLieAlgebra& theRange()
  { if (rangeAlg==0)
      crash << "This is a programming error: non-initialized HomomorphismSemisimpleLieAlgebra. " << crash;
    return *rangeAlg;
  }
  HomomorphismSemisimpleLieAlgebra(): domainAlg(0), rangeAlg(0){}
  std::string DebugString;
  void GetWeightsGmodKInSimpleCoordsK(Vectors<Rational>& outputWeights, GlobalVariables& theGlobalVariables)
  { this->GetWeightsWrtKInSimpleCoordsK(outputWeights, this->GmodK, theGlobalVariables);
  }
  void GetWeightsKInSimpleCoordsK(Vectors<Rational>& outputWeights, GlobalVariables& theGlobalVariables)
  { this->GetWeightsWrtKInSimpleCoordsK(outputWeights, this->imagesAllChevalleyGenerators, theGlobalVariables);
  }
  void GetWeightsWrtKInSimpleCoordsK
  (Vectors<Rational>& outputWeights, List<ElementSemisimpleLieAlgebra<Rational> >& inputElts, GlobalVariables& theGlobalVariables);
  void ToString(std::string& output, GlobalVariables& theGlobalVariables)
  { this->ToString(output, false, theGlobalVariables);
  }
  void ToString(std::string& output, bool useHtml, GlobalVariables& theGlobalVariables);
  void MakeGinGWithId(char theWeylLetter, int theWeylDim, ListReferences<SemisimpleLieAlgebra>& ownerOfAlgebras, GlobalVariables& theGlobalVariables);
  void ProjectOntoSmallCartan(Vector<Rational>& input, Vector<Rational> & output, GlobalVariables& theGlobalVariables);
  void ProjectOntoSmallCartan(Vectors<Rational>& input, Vectors<Rational>& output, GlobalVariables& theGlobalVariables);
  void GetMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output);
  void ComputeDebugString(GlobalVariables& theGlobalVariables)
  { this->ToString(this->DebugString, theGlobalVariables);
  }
  void ComputeDebugString(bool useHtml, GlobalVariables& theGlobalVariables)
  { this->ToString(this->DebugString, useHtml, theGlobalVariables);
  }
  std::string ToString(GlobalVariables& theGlobalVariables)
  { std::string tempS;
    this->ToString(tempS, theGlobalVariables);
    return tempS;
  }
  void GetRestrictionAmbientRootSystemToTheSmallerCartanSA(Vectors<Rational>& output, GlobalVariables& theGlobalVariables);
  bool ComputeHomomorphismFromImagesSimpleChevalleyGenerators(GlobalVariables& theGlobalVariables);
  bool CheckClosednessLieBracket(GlobalVariables& theGlobalVariables);
  void ApplyHomomorphism(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output);
  bool ApplyHomomorphism
  (const ElementUniversalEnveloping<RationalFunctionOld>& input, ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables);
  bool ApplyHomomorphism
  (const MonomialUniversalEnveloping<RationalFunctionOld>& input, const RationalFunctionOld& theCoeff, ElementUniversalEnveloping<RationalFunctionOld>& output, GlobalVariables& theGlobalVariables);
};

class SemisimpleLieAlgebraOrdered
{
public:
  SemisimpleLieAlgebra theOwner;
  //the format of the order is arbitrary except for the following requirements:
  //-All elements of the order must be either 1) nilpotent or 2) elements of the Cartan
  //-Let the number of positive roots be N and the rank be K. Then the indices N,..., N+K-1 must
  //   correspond to the elements of the Cartan.
  List<ElementSemisimpleLieAlgebra<Rational> > theOrder;
  //The order of chevalley generators is as follows. First come negative roots,
  //then elements of cartan, then positive Vectors<Rational>
  //The weights are in increasing order
  //The i^th column of the matrix gives the coordinates of the i^th Chevalley generator
  //in the current coordinates
  Matrix<Rational> ChevalleyGeneratorsInCurrentCoords;
  void AssignGeneratorCoeffOne(int theIndex, ElementSemisimpleLieAlgebra<Rational>& output)
  { output.operator=(this->theOrder[theIndex]);
  }
  int GetDisplayIndexFromGeneratorIndex(int GeneratorIndex);
  void GetLinearCombinationFrom(ElementSemisimpleLieAlgebra<Rational>& input, Vector<Rational>& theCoeffs);
  void init(List<ElementSemisimpleLieAlgebra<Rational> >& inputOrder, SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables);
 void initDefaultOrder(SemisimpleLieAlgebra& owner, GlobalVariables& theGlobalVariables);
};

template <class coefficient>
class ElementWeylAlgebra : public MonomialCollection<MonomialWeylAlgebra, coefficient>
{
public:
  unsigned inline int HashFunction()const
  { return this->::MonomialCollection<MonomialWeylAlgebra, coefficient>::HashFunction();
  }
  static inline unsigned int HashFunction(const ElementWeylAlgebra& input)
  { return input.HashFunction();
  }
  void MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars);
  void MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void Makexixj(int i, int j, int NumVars);
  void Makexi(int i, int NumVars);
  void Makedi(int i, int NumVars);
  void Makedidj(int i, int j, int NumVars);
  void Makexidj(int i, int j, int NumVars);
  static void GetStandardOrderDiffOperatorCorrespondingToNraisedTo
  (const Rational& inputRationalPower, int indexVar, ElementWeylAlgebra& outputDO, Polynomial<Rational>& outputDenominator, GlobalVariables& theGlobalVariables);
  void FourierTransform (ElementWeylAlgebra<coefficient>& output)const;
  bool ActOnPolynomial(Polynomial<Rational>& thePoly)const;
  void SetNumVariables(int newNumVars);
  void MultiplyOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft, GlobalVariables* theGlobalVariables=0);
  static void LieBracket(const ElementWeylAlgebra& left, const ElementWeylAlgebra& right, ElementWeylAlgebra& output);
  void LieBracketOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft, GlobalVariables* theGlobalVariables=0);
  void LieBracketOnTheLeftMakeReport(const ElementWeylAlgebra& standsOnTheLeft, GlobalVariables* theGlobalVariables=0);
  void LieBracketOnTheRightMakeReport(const ElementWeylAlgebra& standsOnTheRight, GlobalVariables* theGlobalVariables=0);
  void LieBracketOnTheRight(const ElementWeylAlgebra& standsOnTheRight, GlobalVariables* theGlobalVariables=0);
  bool Substitution(const PolynomialSubstitution<Rational>& SubPolyPart, const PolynomialSubstitution<Rational>& SubDiffPArt);
  void MakeOne(int ExpectedNumVars=0)
  { MonomialWeylAlgebra tempMon;
    tempMon.polynomialPart.MakeOne(ExpectedNumVars);
    tempMon.differentialPart.MakeOne(ExpectedNumVars);
    this->MakeZero();
    this->AddMonomial(tempMon, 1);
  }
  bool IsPolynomial(Polynomial<coefficient>* whichPoly=0)const;
  bool HasNonSmallPositiveIntegerDerivation()const;
  void RaiseToPower(int thePower);
  void MultiplyTwoMonomials(const MonomialWeylAlgebra& left, const MonomialWeylAlgebra& right, ElementWeylAlgebra& output)const;
  void AssignPolynomial(const Polynomial<Rational>& input)
  { this->MakeZero();
    MonomialWeylAlgebra tempM;
    for (int i=0; i<input.size(); i++)
    { tempM.polynomialPart=input[i];
      this->AddMonomial(tempM, input.theCoeffs[i]);
    }
  }
  void operator=(const std::string& input);
  inline void operator*=(const coefficient& other)
  { this->MonomialCollection<MonomialWeylAlgebra, coefficient>::operator*=(other);
  }
  void operator*=(const ElementWeylAlgebra& other);
  bool IsLetter(char theLetter);
  bool IsIndex(char theIndex);
  bool IsNumber(char theNumber);
};

class LittelmannPath
{
public:
  WeylGroup* owner;
  Vectors<Rational> Waypoints;
  void MakeFromWeightInSimpleCoords(const Vector<Rational>& weightInSimpleCoords, WeylGroup& theOwner);
  void MakeFromWaypoints(Vectors<Rational>& weightsInSimpleCoords, WeylGroup& theOwner)
  { this->owner=&theOwner;
    this->Waypoints=weightsInSimpleCoords;
    this->Simplify();
  }
  void ActByFalpha(int indexAlpha);
  void ActByEalpha(int indexAlpha);
  void ActByEFDisplayIndex(int displayIndex);
//   List<Rational> Speeds;
  void operator+=(const LittelmannPath& other)
  { this->Waypoints.ReservE(this->Waypoints.size+other.Waypoints.size);
    Vector<Rational> endPoint=*this->Waypoints.LastObject();
    for (int i=0; i<other.Waypoints.size; i++)
      this->Waypoints.AddOnTop(other.Waypoints[i]+endPoint);
  }
  bool IsAdaptedString(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theString);
  std::string ElementToStringIndicesToCalculatorOutput(LittelmannPath& inputStartingPath, List<int>& input);
  std::string ElementToStringOperatorSequenceStartingOnMe(List<int>& input);
  bool GenerateOrbit
  (List<LittelmannPath>& output, List<List<int> >& outputOperators, GlobalVariables& theGlobalVariables, int UpperBoundNumElts,
   Selection* parabolicNonSelectedAreInLeviPart=0);
  std::string GenerateOrbitAndAnimate(GlobalVariables& theGlobalVariables);
  bool MinimaAreIntegral();
  std::string ToString(bool useSimpleCoords=true, bool useArrows=true, bool includeDominance=false)const;
  void Simplify();
  unsigned int HashFunction()const
  { return this->Waypoints.HashFunction();
  }
  static inline unsigned int HashFunction(const LittelmannPath& input)
  { return input.HashFunction();
  }
  bool IsEqualToZero()const
  { return this->Waypoints.size==0;
  }
  void operator=(const LittelmannPath& other)
  { this->Waypoints=other.Waypoints;
    this->owner=other.owner;
  }
  bool operator==(const LittelmannPath& other)const
  { return this->Waypoints==other.Waypoints;
  }
};

class ConeLatticeAndShiftMaxComputation
{ public:
  inline static const std::string GetXMLClassName(){ return "ConeLatticeAndShiftMaxComputation";}
  int numNonParaM;
  int numProcessedNonParam;
  List<ConeComplex> complexStartingPerRepresentative;
  List<ConeComplex> complexRefinedPerRepresentative;
  List<List<Vectors<Rational> > > theMaximaCandidates;
  List<Vectors<Rational> > startingLPtoMaximize;
  List<Vectors<Rational> > finalMaxima;
  Lattice theStartingLattice;
  Lattice theFinalRougherLattice;
  Vector<Rational> theStartingRepresentative;
  Vectors<Rational> theFinalRepresentatives;
  List<ConeLatticeAndShift> theConesLargerDim;
  List<ConeLatticeAndShift> theConesSmallerDim;
  List<List<ConeComplex> > finalMaximaChambers;
  List<List<List<int> > > finalMaximaChambersIndicesMaxFunctions;
  List<bool> IsInfinity;
  Vectors<Rational> LPtoMaximizeLargerDim;
  Vectors<Rational> LPtoMaximizeSmallerDim;

  std::string ToString(FormatExpressions* theFormat=0);
  void init(Vector<Rational>& theNEq, Cone& startingCone, Lattice& startingLattice, Vector<Rational> & startingShift);
  void FindExtremaParametricStep1(Controller& thePauseController, bool assumeNewConesHaveSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables);
  void FindExtremaParametricStep2TrimChamberForMultOne(Controller& thePauseController, GlobalVariables& theGlobalVariables);
  void FindExtremaParametricStep3(Controller& thePauseController, GlobalVariables& theGlobalVariables);
  void FindExtremaParametricStep4(Controller& thePauseController, GlobalVariables& theGlobalVariables);
  void FindExtremaParametricStep5(Controller& thePauseController, GlobalVariables& theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes);
};

class PiecewiseQuasipolynomial
{
  public:
  //Note: PiecewiseQuasipolynomial assumes that its variable GlobalVariables* theBuffers
  // is always initialized with a non-zero object.
  //This is an engineering decision that yet has to be proven good: please follow the assumption,
  //but keep in mind that a better solution might exist, and be ready to switch.
  //Let the piecewise quasipolynomial be in n variables.
  //Then the theProjectivizedComplex is an n+1-dimensional complex,
  //which is the projectivization of the n-dim affine complex representing the
  //domain of the piecewise quasipoly.
  ConeComplex theProjectivizedComplex;
  List<QuasiPolynomial> theQPs;
  GlobalVariables* theBuffers;
  int NumVariables;
  void operator=(PiecewiseQuasipolynomial& other)
  { this->theBuffers=other.theBuffers;
    this->NumVariables=other.NumVariables;
    this->theQPs=other.theQPs;
    this->theProjectivizedComplex=other.theProjectivizedComplex;
  }
  std::string ToString(bool useLatex, bool useHtml);
  PiecewiseQuasipolynomial(){this->theBuffers=0;}
  PiecewiseQuasipolynomial(GlobalVariables& PermanentGlobalVariables)
  { this->theBuffers=& PermanentGlobalVariables;
  }
  void DrawMe(DrawingVariables& theDrawingVars, int numLatticePointsPerDim, Cone* RestrictingChamber=0, Vector<Rational> * distinguishedPoint=0);
  int GetNumVars()
  { return this->NumVariables;
  }
  inline void MakeCommonRefinement(const PiecewiseQuasipolynomial& other)
  { this->MakeCommonRefinement(other.theProjectivizedComplex);
  }
  void MakeCommonRefinement(const ConeComplex& other);
  void TranslateArgument(Vector<Rational>& translateToBeAddedToArgument, GlobalVariables& theGlobalVariables);
  bool MakeVPF(Vectors<Rational>& theRoots, std::string& outputstring, GlobalVariables& theGlobalVariables);
  Rational Evaluate(const Vector<Rational>& thePoint);
  Rational EvaluateInputProjectivized(const Vector<Rational>& thePoint);
  void MakeZero(int numVars, GlobalVariables& theGlobalVariables)
  { this->NumVariables=numVars;
    this->theProjectivizedComplex.init();
    this->theBuffers=& theGlobalVariables;
    this->theQPs.size=0;
  }
  void operator+=(const PiecewiseQuasipolynomial& other);
  void operator*=(const Rational& other);
  void operator=(const PiecewiseQuasipolynomial& other);
};

template <class coefficient>
class MonomialChar
{
public:
  SemisimpleLieAlgebra* owner;
  Vector<coefficient> weightFundamentalCoordS;
  static const bool IsEqualToZero()
  { return false;
  }
  friend std::ostream& operator << (std::ostream& output, const MonomialChar<coefficient>& input)
  { output << input.ToString();
    return output;
  }
  MonomialChar():owner(0){}
  void CheckNonZeroOwner()const
  { if (this->owner!=0)
      return;
    crash << "This is a programming error: Monomial char has zero owner, which is not allowed by the current function call. " << crash;
  }
  void AccountSingleWeight
  (const Vector<Rational>& currentWeightSimpleCoords, const Vector<Rational>& otherHighestWeightSimpleCoords,
   Rational& theMult, charSSAlgMod<coefficient>& outputAccum)const;
  std::string TensorAndDecompose(const MonomialChar<coefficient>& other, charSSAlgMod<coefficient>& output, GlobalVariables& theGlobalVariables)const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  inline unsigned int HashFunction()const
  { return weightFundamentalCoordS.HashFunction();
  }
  static inline unsigned int HashFunction(const MonomialChar<coefficient>& input)
  { return input.HashFunction();
  }
  inline bool operator==(const MonomialChar<coefficient>& other) const
  { return this->weightFundamentalCoordS==other.weightFundamentalCoordS && this->owner==other.owner;
  }
  inline bool operator>(const MonomialChar<coefficient>& other) const
  { return this->weightFundamentalCoordS>other.weightFundamentalCoordS;
  }
};

template <class coefficient>
class charSSAlgMod : public MonomialCollection<MonomialChar<coefficient>, coefficient>
{
  public:
  void CheckConsistency()const
  { if (this->size()==0)
      return;
    const SemisimpleLieAlgebra* owner=(*this)[0].owner;
    for (int i=1; i<this->size(); i++)
      if ((*this)[i].owner!=owner)
        crash << "This is a programming error: charSSAlgMod contains elements belonging to different semisimple Lie algebras. " << crash;
  }
  void CheckNonZeroOwner()const
  { this->CheckConsistency();
    if (this->GetOwner()==0)
      crash << "This is a programming error: charSSAlgMod has no owner semisimple Lie algebra, which is not allowed at by the calling function. " << crash;
  }
  bool IsEqualToZero()
  { return this->size()==0;
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const charSSAlgMod<coefficient>& input)
  { return input.::MonomialCollection<MonomialChar<coefficient>, coefficient>::HashFunction(input);
  }
  void GetDual(charSSAlgMod<coefficient>& output)const;
  void MakeFromWeight(const Vector<coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner);
  bool SplitCharOverRedSubalg(std::string* Report, charSSAlgMod& output, branchingData& inputData, GlobalVariables& theGlobalVariables);
  bool GetDominantCharacterWRTsubalgebra
  (charSSAlgMod& outputCharOwnerSetToZero, std::string& outputDetails, GlobalVariables& theGlobalVariables, int upperBoundNumDominantWeights);
  bool FreudenthalEvalMeDominantWeightsOnly
  (charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables);
  bool FreudenthalEvalMeFullCharacter
  (charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables);
  bool DrawMeNoMults(std::string& outputDetails, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars, int upperBoundWeights)
  { return this->DrawMe(outputDetails, theGlobalVariables, theDrawingVars, upperBoundWeights, false);
  }
  bool DrawMeWithMults(std::string& outputDetails, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars, int upperBoundWeights)
  { return this->DrawMe(outputDetails, theGlobalVariables, theDrawingVars, upperBoundWeights, true);
  }
  void DrawMeAssumeCharIsOverCartan(WeylGroup& actualAmbientWeyl, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars)const;
  SemisimpleLieAlgebra* GetOwner()const
  { if (this->size()==0)
      crash << "This is a programming error: requesting owner semisimple Lie algebra of zero character. " << crash;
    return (*this)[0].owner;
  }
  bool DrawMe(std::string& outputDetails, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults);
  bool SplitOverLeviMonsEncodeHIGHESTWeight
  (std::string* Report, charSSAlgMod& output, const Selection& splittingParSel, const Selection& ParSelFDInducingPart,
   ReflectionSubgroupWeylGroup& outputWeylSub, GlobalVariables& theGlobalVariables);
  int GetIndexExtremeWeightRelativeToWeyl(WeylGroup& theWeyl)const;
  void MakeTrivial(SemisimpleLieAlgebra& inputOwner);
  std::string MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables);
  std::string operator*=(const charSSAlgMod& other);
  std::string operator*=(const MonomialChar<Rational>& other);
  void operator*=(const coefficient& other)
  { this->::MonomialCollection<MonomialChar<coefficient>, coefficient>::operator*=(other);
  }
};

//the following data is isolated in a struct because it is
//way too large a lump to pass separately
struct branchingData
{ HomomorphismSemisimpleLieAlgebra theHmm;
  FormatExpressions theFormat;
  Vector<RationalFunctionOld> theWeightFundCoords;
  charSSAlgMod<RationalFunctionOld> theAmbientChar;
  charSSAlgMod<RationalFunctionOld> theSmallCharFDpart;
  Selection selInducing;
  Selection selSmallParSel;
  Selection SelSplittingParSel;
  Vectors<Rational> weightsNilradicalLarge;
  Vectors<Rational> weightsNilradicalSmall;
  Vectors<Rational> weightsNilModPreNil;
  List<int> indicesNilradicalSmall;
  List<int> indicesNilradicalLarge;
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalLarge;
  List<ElementSemisimpleLieAlgebra<Rational> > nilradicalSmall;
  List<ElementSemisimpleLieAlgebra<Rational> > NilModPreNil;
  Vectors<RationalFunctionOld> outputWeightsFundCoordS;
  Vectors<RationalFunctionOld> outputWeightsSimpleCoords;
  Vectors<RationalFunctionOld> g2Weights;
  Vectors<RationalFunctionOld> g2DualWeights;
  Vectors<RationalFunctionOld> leviEigenSpace;
  Vectors<Rational> generatorsSmallSub;
  HashedList<RationalFunctionOld> theCharacterDifferences;
  List<ElementUniversalEnveloping<RationalFunctionOld> > outputEigenWords;
  List<RationalFunctionOld> theChars;
  List<ElementSumGeneralizedVermas<RationalFunctionOld> > theEigenVectorS;
  List<ElementUniversalEnveloping<RationalFunctionOld> > theUEelts;
  List<Rational> additionalMultipliers;
  List<RationalFunctionOld> theShapovalovProducts;
  List<ElementSumGeneralizedVermas<RationalFunctionOld> > theEigenVectorsLevi;
  ReflectionSubgroupWeylGroup WeylFD;
  ReflectionSubgroupWeylGroup WeylFDSmallAsSubInLarge;
  ReflectionSubgroupWeylGroup WeylFDSmall;
  std::string GetStringCasimirProjector(int theIndex, const Rational& additionalMultiple);
  template <class coefficient>
  Vector<coefficient> ProjectWeight(Vector<coefficient>& input);
  void resetOutputData();
  void initAssumingParSelAndHmmInittedPart1NoSubgroups(GlobalVariables& theGlobalVariables);
  void initAssumingParSelAndHmmInittedPart2Subgroups(GlobalVariables& theGlobalVariables);
  void initAssumingParSelAndHmmInitted(GlobalVariables& theGlobalVariables)
  { MacroRegisterFunctionWithName("branchingData::initAssumingParSelAndHmmInitted");
    this->initAssumingParSelAndHmmInittedPart1NoSubgroups(theGlobalVariables);
    this->initAssumingParSelAndHmmInittedPart2Subgroups(theGlobalVariables);
  }
};

template <class coefficient>
Vector<coefficient> branchingData::ProjectWeight(Vector<coefficient>& input)
{ Vector<coefficient> result;
  Vector<coefficient> fundCoordsSmaller;
  fundCoordsSmaller.MakeZero(this->theHmm.theDomain().GetRank());
  for (int j=0; j<this->theHmm.theDomain().GetRank(); j++)
  { fundCoordsSmaller[j]=this->theHmm.theRange().theWeyl.RootScalarCartanRoot(input, theHmm.ImagesCartanDomain[j]);
    fundCoordsSmaller[j]/=this->theHmm.theDomain().theWeyl.CartanSymmetric.elements[j][j]/2;
  }
  result=this->theHmm.theDomain().theWeyl.GetSimpleCoordinatesFromFundamental(fundCoordsSmaller);
  return result;
}

class MonomialMatrix
{
  friend std::ostream& operator << (std::ostream& output, const MonomialMatrix& theMon)
  { output << theMon.ToString();
    return output;
  }
  public:
  int vIndex;
  int dualIndex;
  bool IsId;
  MonomialMatrix(const MonomialMatrix& other)
  { this->operator=(other);
  }
  MonomialMatrix():vIndex(-1), dualIndex(-1), IsId(false){}
  MonomialMatrix(int i, int j): vIndex(i), dualIndex(j), IsId(false){}
  void operator=(const MonomialMatrix& other)
  { this->vIndex=other.vIndex;
    this->dualIndex=other.dualIndex;
    this->IsId=other.IsId;
  }
  void MakeEij(int i, int j)
  { this->dualIndex=j;
    this->vIndex=i;
    this->IsId=false;
  }
  void MakeOne()
  { this->MakeIdSpecial();
  }
  void Transpose()
  { MathRoutines::swap(this->vIndex, this->dualIndex);
  }
  void MakeZero()
  { this->IsId=false;
    this->vIndex=-1;
    this->dualIndex=-1;
  }
  bool IsEqualToZero()const
  { return !this->IsId && this->vIndex==-1 && this->dualIndex==-1;
  }
  bool operator==(const MonomialMatrix& other)const
  { return this->vIndex==other.vIndex && this->dualIndex==other.dualIndex && this->IsId==other.IsId;
  }
  static unsigned int HashFunction(const MonomialMatrix& input)
  { return input.vIndex*SomeRandomPrimes[0]+input.dualIndex*SomeRandomPrimes[1]+input.IsId;
  }
  inline unsigned int HashFunction()const
  { return HashFunction(*this);
  }
  bool operator>(const MonomialMatrix& other)const
  { if (this->IsId!=other.IsId)
      return this->IsId>other.IsId;
    if (this->vIndex==other.vIndex)
      return this->dualIndex>other.dualIndex;
    return this->vIndex>other.vIndex;
  }
  void MakeIdSpecial()
  { this->vIndex=-1;
    this->dualIndex=-1;
    this->IsId=true;
  }
  std::string ToString(FormatExpressions* theFormat=0)const
  { std::stringstream out;
    if (!this->IsId)
      out << "m_{" << this->vIndex+1 << "}\\otimes " << "m^*_{" << this->dualIndex+1 << "}";
    else
      out << "id";
    return out.str();
  }
  void operator*=(const MonomialMatrix& other)
  { MacroRegisterFunctionWithName("MonomialMatrix<coefficient>::operator*=");
    if (this==&other)
    { MonomialMatrix otherCopy;
      otherCopy=other;
      *this*=otherCopy;
      return;
    }
    if (other.IsId)
      return;
    if (this->IsId)
    { *this=other;
      return;
    }
    if (this->dualIndex==other.vIndex)
      this->dualIndex=other.dualIndex;
    else
      this->MakeZero();
  }
};

template <class coefficient>
class MatrixTensor: public ElementMonomialAlgebra<MonomialMatrix, coefficient>
{
public:
  void MakeIdSpecial()
  { this->MakeZero();
    MonomialMatrix theMon;
    theMon.MakeIdSpecial();
    this->AddMonomial(theMon, 1);
  }
  void MakeId(int numVars)
  { this->MakeZero();
    MonomialMatrix theMon;
    for (int i=0; i<numVars; i++)
    { theMon.dualIndex=i;
      theMon.vIndex=i;
      this->AddMonomial(theMon, 1);
    }
  }
  void Invert();
  int GetMinNumRows()const
  { int result=-1;
    for (int i=0; i<this->size(); i++)
      result=MathRoutines::Maximum(result, (*this)[i].vIndex);
    return result+1;
  }
  int GetMinNumCols()const
  { int result=-1;
    for (int i=0; i<this->size(); i++)
      result=MathRoutines::Maximum(result, (*this)[i].dualIndex);
    return result+1;
  }
  int GetMinNumColsNumRows()const
  { int result=-1;
    for (int i=0; i<this->size(); i++)
    { result=MathRoutines::Maximum(result, (*this)[i].dualIndex);
      result=MathRoutines::Maximum(result, (*this)[i].vIndex);
    }
    return result+1;
  }
  coefficient GetDeterminant()const
  { Matrix<coefficient> theMat;
    this->GetMatrix(theMat, this->GetMinNumColsNumRows());
    return theMat.GetDeterminant();
  }
  void DirectSumWith(const MatrixTensor<coefficient>& other);
  void GetVectorsSparseFromRowsIncludeZeroRows(List<VectorSparse<coefficient> >& output, int MinNumRows=-1);

  bool IsPositiveDefinite()
  { Matrix<coefficient> other;
    this->GetMatrix(other, this->GetMinNumColsNumRows());
    return other.IsPositiveDefinite();
  }
  void operator=(const Matrix<coefficient>& other)
  { this->MakeZero();
    MonomialMatrix theMon;
    for (int i=0; i<other.NumRows; i++)
      for (int j=0; j<other.NumCols; j++)
        if (!other.elements[i][j].IsEqualToZero())
        { theMon.dualIndex=j;
          theMon.vIndex=i;
          this->AddMonomial(theMon, other.elements[i][j]);
        }
  }
  void AssignTensorProduct(const MatrixTensor<coefficient>& left, const MatrixTensor<coefficient>& right)
  { //handle lazy programmers:
    if (this==&left || this==& right)
    { MatrixTensor<coefficient> leftCopy=left;
      MatrixTensor<coefficient> rightCopy=right;
      this->AssignTensorProduct(leftCopy, rightCopy);
      return;
    }
    //The basis of the tensor product vector space MUST be in the SAME order as the one used by Matrix::AssignTensorProduct.
    //int leftDomainDim=left.GetMinNumCols();
    int rightDomainDim=right.GetMinNumCols();
    //int leftRangeDim=left.GetMinNumRows();
    int rightRangeDim=right.GetMinNumRows();
    MonomialMatrix tempM;
    this->MakeZero();
    coefficient tempCF;
    for (int i=0; i<left.size(); i++)
      for (int j=0; j<right.size(); j++)
      { tempM.dualIndex=left[i].dualIndex* rightDomainDim+right[j].dualIndex;
        tempM.vIndex=left[i].vIndex*rightRangeDim+right[j].vIndex;
        tempCF=left.theCoeffs[i];
        tempCF*=right.theCoeffs[j];
        this->AddMonomial(tempM, tempCF);
      }
  }
  void Substitution(const PolynomialSubstitution<Rational>& theSub)
  { MatrixTensor<coefficient> thisCopy=*this;
    this->MakeZero();
    coefficient tempCF;
    for (int i=0; i<thisCopy.size(); i++)
    { tempCF=thisCopy.theCoeffs[i];
      tempCF.Substitution(theSub);
      this->AddMonomial(thisCopy[i] , tempCF);
    }
  }
  void Transpose()
  { MatrixTensor<coefficient> output;
    MonomialMatrix theMon;
    output.MakeZero();
    for (int i=0; i<this->size(); i++)
    { theMon=(*this)[i];
      theMon.Transpose();
      output.AddMonomial(theMon, this->theCoeffs[i]);
    }
    *this=output;
  }
  void SetNumVariables(int newNumVars)
  { MatrixTensor<coefficient> thisCopy=*this;
    this->MakeZero();
    coefficient tempCF;
    for (int i=0; i<thisCopy.size; i++)
    { tempCF=thisCopy.theCoeffs[i];
      tempCF.SetNumVariables(newNumVars);
      this->AddMonomial(thisCopy[i] , tempCF);
    }
  }
  void LieBracketOnTheLeft(const MatrixTensor<coefficient>& standsOnTheLeft)
  { MacroRegisterFunctionWithName("MatrixTensor<coefficient>::LieBracketOnTheLeft");
    MatrixTensor<coefficient> output;
    MonomialMatrix theMon;
    output.MakeZero();
    output.SetExpectedSize(this->size()*standsOnTheLeft.size()*2);
    coefficient tempCF;
    for (int i=0; i<this->size(); i++)
      for (int j=0; j<standsOnTheLeft.size(); j++)
      { tempCF=this->theCoeffs[i];
        tempCF*=standsOnTheLeft.theCoeffs[j];
        theMon=standsOnTheLeft[j];
        theMon*=(*this)[i];
        output.AddMonomial(theMon, tempCF);
        tempCF*=-1;
        theMon=(*this)[i];
        theMon*=standsOnTheLeft[j];
        output.AddMonomial(theMon, tempCF);
      }
    *this=output;
  }
  std::string ToStringMatForm(FormatExpressions* theFormat=0)const
  { if (this->IsEqualToZero())
      return "(0)";
    Matrix<coefficient> tempMat;
    this->GetMatrix(tempMat, this->GetMinNumColsNumRows());
    return tempMat.ToString(theFormat);
  }
  void GetMatrix(Matrix<coefficient>& output, int theDim)const
  { theDim=MathRoutines::Maximum(theDim, this->GetMinNumColsNumRows());
    output.init(theDim, theDim);
    output.NullifyAll();
    for (int i=0; i<this->size(); i++)
      if ((*this)[i].IsId)
        for (int j=0; j<theDim; j++)
          output(j,j)+= this->theCoeffs[i];
      else
        output((*this)[i].vIndex,(*this)[i].dualIndex)+=this->theCoeffs[i];
  }
  template <class otherType>
  void AssignVectorsToColumns(const Vectors<otherType>& inputVectors)
  { this->MakeZero();
    for (int i=0; i<inputVectors.size; i++)
      for (int j=0; j<inputVectors[i].size; j++)
        this->AddMonomial(MonomialMatrix(j,i), inputVectors[i][j]);
  }
  template <class otherType>
  void AssignVectorsToRows(const List<VectorSparse<otherType> >& inputVectors)
  { this->MakeZero();
    for (int i=0; i<inputVectors.size; i++)
      for (int j=0; j<inputVectors[i].size(); j++)
        this->AddMonomial(MonomialMatrix(i,inputVectors[i][j].theIndex), inputVectors[i].theCoeffs[j]);
  }
  void GaussianEliminationByRowsMatrix(MatrixTensor<coefficient>* carbonCopyMat=0);
  template <class otherType>
  void ActOnVectorColumn(const Vector<otherType>& input, Vector<otherType>& output)const
  { if (&input==&output)
    { Vector<otherType> inputCopy=input;
      this->ActOnVectorColumn(inputCopy, output);
      return;
    }
    output.MakeZero(this->GetMinNumRows());
    otherType currentCF;
    for (int i=0; i<this->size(); i++)
    { //note that, at the cost of one extra implicit conversion below, we preserve the order of multiplication:
      //first is matrix element, then vector coordinate. The code should work as-is for non-commutative fields.
      //(think in the generality of quaternion matrix acting on quaternion-coefficient polynomials!)
      currentCF=this->theCoeffs[i];
      currentCF*=input[(*this)[i].dualIndex];
      output[(*this)[i].vIndex]+= currentCF;
    }
  }
  void ActOnVectorROWSOnTheLeft(const List<Vector<coefficient> >& inputStandToTheLeftAsVectorRows, List<Vector<coefficient> >& output)const
  { if (&inputStandToTheLeftAsVectorRows==&output)
    { List<Vector<coefficient> > inputCopy=inputStandToTheLeftAsVectorRows;
      this->ActOnVectorROWSOnTheLeft(inputCopy, output);
      return;
    }
    output.SetSize(this->GetMinNumRows());
    int numColsTarget=inputStandToTheLeftAsVectorRows[0].size;
    if (this->GetMinNumCols()!=inputStandToTheLeftAsVectorRows.size)
    { crash << "This is a programming error: attemtping to act by matrix " << this->ToString() << " (" << this->GetMinNumCols() << " columns) "
      << " on the " << inputStandToTheLeftAsVectorRows.size << " vector-rows: " << inputStandToTheLeftAsVectorRows.ToString() << ". "
      << crash;
    }
    for (int i=0; i<inputStandToTheLeftAsVectorRows.size; i++)
      output[i].MakeZero(numColsTarget);
    for (int i=0; i<this->size(); i++)
      for (int j=0; j<numColsTarget; j++)
        output[(*this)[i].vIndex][j]+=inputStandToTheLeftAsVectorRows[(*this)[i].dualIndex][j]*this->theCoeffs[i];
  }
  template <class otherType>
  void ActOnVectorsColumn(Vectors<otherType>& inputOutput)const
  { for (int i=0; i<inputOutput.size; i++)
      this->ActOnVectorColumn(inputOutput[i], inputOutput[i]);
  }
  template <class otherType>
  void ActOnVectorColumn(VectorSparse<otherType>& inputOutput)const
  { VectorSparse<otherType> output;
    output.MakeZero();
    otherType tempElt;
    MonomialVector tempVM;
    for (int i=0; i<this->size(); i++)
      for (int j=0; j<inputOutput.size(); j++)
        if((*this)[i].dualIndex==inputOutput[j].theIndex)
        { tempVM.theIndex=(*this)[i].vIndex;
          tempElt=inputOutput.theCoeffs[j];
          tempElt*=this->theCoeffs[i];
          output.AddMonomial(tempVM, tempElt);
        }
    inputOutput=output;
  }
  inline unsigned int HashFunction()const
  { return this->::MonomialCollection<MonomialMatrix, coefficient>::HashFunction();
  }
  static inline unsigned int HashFunction(const MatrixTensor<coefficient>& input)
  { return input.HashFunction();
  }
};

template <class coefficient>
void MatrixTensor<coefficient>::GetVectorsSparseFromRowsIncludeZeroRows(List<VectorSparse<coefficient> >& output, int MinNumRows)
{ MinNumRows=MathRoutines::Maximum(MinNumRows, this->GetMinNumRows());
  output.SetSize(MinNumRows);
  for (int i=0; i<output.size; i++)
    output[i].MakeZero();
  for (int i=0; i<this->size(); i++)
  { int rowIndex=(*this)[i].vIndex;
    int colIndex=(*this)[i].dualIndex;
    output[rowIndex].AddMonomial(MonomialVector(colIndex), this->theCoeffs[i]);
  }
}

template <class coefficient>
void MatrixTensor<coefficient>::GaussianEliminationByRowsMatrix(MatrixTensor<coefficient>* carbonCopyMat)
{ List<VectorSparse<coefficient> > theRows, theCarbonCopyRows;
  int numRows=this->GetMinNumRows();
  if (carbonCopyMat!=0)
  { numRows=MathRoutines::Maximum(numRows, carbonCopyMat->GetMinNumRows());
    carbonCopyMat->GetVectorsSparseFromRowsIncludeZeroRows(theCarbonCopyRows, numRows);
  }
  this->GetVectorsSparseFromRowsIncludeZeroRows(theRows, numRows);
  List<VectorSparse<coefficient> >* theCarbonCopyPointer=carbonCopyMat==0 ? 0 : &theCarbonCopyRows;
  VectorSparse<coefficient>::GaussianEliminationByRows(theRows, 0, 0, 0, theCarbonCopyPointer);
  this->AssignVectorsToRows(theRows);
  if (carbonCopyMat!=0)
    carbonCopyMat->AssignVectorsToRows(theCarbonCopyRows);
}

template <class coefficient>
void MatrixTensor<coefficient>::DirectSumWith(const MatrixTensor<coefficient>& other)
{ if (&other==this)
  { MatrixTensor<coefficient> otherCopy=other;
    this->DirectSumWith(otherCopy);
    return;
  }
  int indexShift= this->GetMinNumColsNumRows();
  this->SetExpectedSize(this->size()+other.size());
  MonomialMatrix currentM;
  for (int i=0; i<other.size(); i++)
  { currentM.vIndex=other[i].vIndex+indexShift;
    currentM.dualIndex=other[i].dualIndex+indexShift;
    this->AddMonomial(currentM, other.theCoeffs[i]);
  }
}

template <class coefficient>
void MatrixTensor<coefficient>::Invert()
{ MatrixTensor<coefficient> theId;
  theId.MakeId(this->GetMinNumColsNumRows());
  MatrixTensor<coefficient> result=theId;
//  std::cout << "<hr>Inverting: " << this->ToStringMatForm();
  this->GaussianEliminationByRowsMatrix(&result);
  if (*this!=theId)
    crash << "This is a programming error: attempting to invert a non-invertable matrix tensor. After Gaussian elimination, the matrix equals "
    << this->ToStringMatForm() << " but should instead be equal to " << theId.ToStringMatForm() << crash;
  *this=result;
}

template<class coefficient>
class MonomialGeneralizedVerma
{
  public:
  ModuleSSalgebra<coefficient>* owneR;
  MonomialUniversalEnveloping<coefficient> theMonCoeffOne;
  int indexFDVector;
  MonomialGeneralizedVerma(): owneR(0), indexFDVector(-1) { }
  friend std::ostream& operator << (std::ostream& output, const MonomialGeneralizedVerma<coefficient>& theGen)
  { output << theGen.ToString();
    return output;
  }
  void MultiplyMeByUEEltOnTheLefT
  (const ElementUniversalEnveloping<coefficient>& theUE, ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero)const;
  void operator=(const MonomialGeneralizedVerma<coefficient>& other)
  { this->owneR=other.owneR;
    this->indexFDVector=other.indexFDVector;
    this->theMonCoeffOne=other.theMonCoeffOne;
  }

  std::string ToString(FormatExpressions* theFormat=0, bool includeV=true)const;
  static const bool IsEqualToZero()
  { return false;
  }
  bool operator==(const MonomialGeneralizedVerma<coefficient>& other)const
  { if (this->indexFDVector==other.indexFDVector && this->owneR==other.owneR)
      return this->theMonCoeffOne==other.theMonCoeffOne;
    return false;
  }
  void SetNumVariables(int GoalNumVars)
  { if (this->owneR->size<=this->indexInOwner)
      crash << crash;
    this->theMonCoeffOne.SetNumVariables(GoalNumVars);
    this->owneR->TheObjects[this->indexInOwner].SetNumVariables(GoalNumVars);
  }
  void Substitution(const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods);
  unsigned int HashFunction()const
  { return this->indexFDVector*SomeRandomPrimes[0]+ ((unsigned int)(uintptr_t)this->owneR)*SomeRandomPrimes[1];
  }
  static inline unsigned int HashFunction(const MonomialGeneralizedVerma<coefficient>& input)
  { return input.HashFunction();
  }
  bool operator>(const MonomialGeneralizedVerma<coefficient>& other)
  { if (this->owneR!=other.owneR)
  // use of ulong is correct on i386, amd64, and a number of other popular platforms
  // uintptr_t is only available in c++0x
      return (unsigned long)this->owneR>(unsigned long)other.owneR;
    if (this->indexFDVector!=other.indexFDVector)
      return this->indexFDVector>other.indexFDVector;
    return this->theMonCoeffOne>other.theMonCoeffOne;
  }
  void ReduceMe(ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero)const;
  bool IsHWV()const
  { if (!this->theMonCoeffOne.IsEqualToOne())
      return false;
//    std::cout << "<br>hi, my name is: " << this->ToString() << " and my index is: " << this->indexFDVector
//    << " and this->GetOwner().GetDim()-1 is " << this->GetOwner().GetDim()-1;
    return this->GetOwner().GetDim()-1==this->indexFDVector;
  }
  void MakeConst(ModuleSSalgebra<coefficient>& inputOwner)
  { this->owneR=&inputOwner;
    this->theMonCoeffOne.MakeOne(*inputOwner.owneR);
  }
  ModuleSSalgebra<coefficient>& GetOwner()const
  { return *this->owneR;
  }
};

template<class coefficient>
class ElementSumGeneralizedVermas : public MonomialCollection<MonomialGeneralizedVerma<coefficient>, coefficient>
{
  public:
//  ModuleSSalgebra<coefficient>* owneR;
  void MultiplyMeByUEEltOnTheLeft(const ElementUniversalEnveloping<coefficient>& theUE, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero);
  unsigned int HashFunction()const
  { return this->MonomialCollection<MonomialGeneralizedVerma<coefficient>, coefficient >::HashFunction();
  }
  static unsigned int HashFunction(const ElementSumGeneralizedVermas<coefficient>& input)
  { return input.HashFunction();
  }
  ElementSumGeneralizedVermas(){}
  void MakeHWV(ModuleSSalgebra<coefficient>& theOwner, const coefficient& theRingUnit);
  int GetNumVars()
  { if (this->owneR==0)
      return -1;
    if (this->owneR->size==0)
      return -1;
    int theAnswer=this->owneR->TheObjects[0].GetNumVars();
    for (int i=1; i<this->owneR->size; i++)
      if (theAnswer!=this->owneR->TheObjects[i].GetNumVars())
        return -1;
    return theAnswer;
  }
  bool ExtractElementUE(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& theOwner);
  inline void operator=(const ElementSumGeneralizedVermas<coefficient>& other)
  { this->::MonomialCollection<MonomialGeneralizedVerma<coefficient>, coefficient>:: operator=(other);
  }
};

template <class coefficient>
class MonomialTensorGeneralizedVermas
{
public:
  List<MonomialGeneralizedVerma<coefficient> > theMons;
  void SimplifyNormalizeCoeffs
  ()
  ;
  friend std::ostream& operator << (std::ostream& output, const MonomialTensorGeneralizedVermas<coefficient>& input)
  { output << input.ToString();
    return output;
  }
  static const bool IsEqualToZero(){return false;}
  int GetNumVars()
  { return this->Coefficient.GetNumVars();
  }
  void operator*=(const MonomialTensorGeneralizedVermas<coefficient>& other)
  { if (this==&other)
    { MonomialTensorGeneralizedVermas<coefficient> tempMon1;
      tempMon1=other;
      *this*=tempMon1;
      return;
    }
    this->theMons.AddListOnTop(other.theMons);
  }
  void operator*=(const MonomialGeneralizedVerma<coefficient>& other)
  { this->theMons.AddOnTop(other);
  }
  unsigned int HashFunction()const
  { int numCycles=MathRoutines::Minimum(SomeRandomPrimesSize, this->theMons.size);
    unsigned int result=0;
    for (int i=0; i<numCycles; i++)
      result+=SomeRandomPrimes[i]*this->theMons[i].HashFunction();
    return result;
  }
  static unsigned int HashFunction(const MonomialTensorGeneralizedVermas<coefficient>& input)
  { return input.HashFunction();
  }
  void SetNumVariables(int GoalNumVars)
  { for (int i=0; i<this->theMons.size; i++)
      this->theMons[i].SetNumVariables(GoalNumVars);
  }
  void Substitution(const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods)
  { for (int i=0; i<this->theMons.size; i++)
      this->theMons[i].Substitution(theSub, theMods);
  }
  std::string ToString(FormatExpressions* theFormat=0, bool includeV=true)const
  ;
  MonomialTensorGeneralizedVermas(){}
  void operator=(const MonomialTensorGeneralizedVermas<coefficient>& other)
  { this->theMons=other.theMons;
  }
  void operator=(const MonomialGeneralizedVerma<coefficient>& other)
  ;
  bool operator==(const MonomialTensorGeneralizedVermas<coefficient>& other)const
  { if (this->theMons.size!=other.theMons.size)
      return false;
    for(int i=0; i<this->theMons.size; i++)
      if(!(this->theMons[i]==other.theMons[i]))
        return false;
    return true;
  }
  bool IsHWV()const
  { if (this->theMons.size!=1)
      return false;
    return this->theMons[0].IsHWV();
  }
  bool operator>(const MonomialTensorGeneralizedVermas<coefficient>& other)const
  { if (this->theMons.size>other.theMons.size)
      return true;
    if (other.theMons.size>this->theMons.size)
      return false;
    ///This might need a rewrite. As it is, it will cause monomials to be sorted according to the
    ///alphabetical order of their human-readable strings. If I have time, I will make a better scheme for
    ///comparison.
    return this->ToString()>other.ToString();
  }
};

template <class coefficient>
void MonomialTensorGeneralizedVermas <coefficient>::SimplifyNormalizeCoeffs()
{ if (this->Coefficient.IsEqualToZero())
  { this->MakeZero(this->Coefficient.GetZero());
    return;
  }
  for (int i=0; i<this->theMons.size; i++)
  { coefficient& currentCF=this->theMons[i].Coefficient;
    if (currentCF.IsEqualToZero())
    { this->MakeZero(this->Coefficient.GetZero());
      return;
    }
    this->Coefficient*=currentCF;
    currentCF=currentCF.GetOne();
  }
}

class PolynomialOverModule;

class slTwoInSlN
{
  int GetModuleIndexFromHighestWeightVector(const Matrix<Rational> & input)
  { Rational tempRat;
    for (int i=0; i<this->theHighestWeightVectors.size; i++)
      if (this->theHighestWeightVectors.TheObjects[i].IsProportionalTo(input, tempRat))
        return i;
    return -1;
  }
public:
  int theDimension;
  Matrix<Rational>  theH;
  Matrix<Rational>  theE;
  Matrix<Rational>  theF;

  List<int> thePartition;
  List<Matrix<Rational> > theProjectors;
  List<Matrix<Rational> > theHighestWeightVectors;
  List<List<Matrix<Rational> > > theGmodKModules;
  List<List<List<int> > > PairingTable;
  void GetIsPlusKIndexingFrom(int input, int& s, int& k);
  std::string ElementMatrixToTensorString(const Matrix<Rational> & input, bool useHtml);
  std::string initFromModuleDecomposition(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable);
  std::string initPairingTable(bool useHtml);
  std::string ElementModuleIndexToString(int input, bool useHtml);
  std::string GetNotationString(bool useHtml);
  bool ComputeInvariantsOfDegree
  (List<int>& decompositionDimensions, int theDegree, List<Polynomial<Rational> >& output, std::string& outputError, GlobalVariables& theGlobalVariables);
  std::string PairTwoIndices(List<int>& output, int leftIndex, int rightIndex, bool useHtml);
  void ExtractHighestWeightVectorsFromVector(Matrix<Rational> & input, List<Matrix<Rational> >& outputDecompositionOfInput, List<Matrix<Rational> >& outputTheHWVectors);
  void ClimbDownFromHighestWeightAlongSl2String(Matrix<Rational> & input, Matrix<Rational> & output, Rational& outputCoeff, int generatorPower);
  void ClimbUpFromVector(Matrix<Rational> & input, Matrix<Rational> & outputLastNonZero, int& largestPowerNotKillingInput);
};

class GeneralizedVermaModuleCharacters
{
public:
  inline static const std::string GetXMLClassName(){ return "GeneralizedVermaCharacters";}
  Controller thePauseControlleR;
  List<Matrix<Rational> > theLinearOperators;
  //the first k variables correspond to the Cartan of the smaller Lie algebra
  //the next l variables correspond to the Cartan of the larger Lie algebra
  //the last variable is the projectivization
  List<Matrix<Rational> > theLinearOperatorsExtended;
  Vector<Rational>  NonIntegralOriginModificationBasisChanged;
  std::fstream theMultiplicitiesMaxOutput;
  std::fstream theMultiplicitiesMaxOutputReport2;
  Vectors<Rational> GmodKnegativeWeightS;
  Vectors<Rational> GmodKNegWeightsBasisChanged;
  Matrix<Rational>  preferredBasisChangE;
  Matrix<Rational>  preferredBasisChangeInversE;
  Vectors<Rational> preferredBasiS;
  Cone PreimageWeylChamberLargerAlgebra;
  Cone WeylChamberSmallerAlgebra;
  Cone PreimageWeylChamberSmallerAlgebra;
  Lattice theExtendedIntegralLatticeMatForM;
  List<QuasiPolynomial> theQPsNonSubstituted;
  List<List<QuasiPolynomial> > theQPsSubstituted;
  List<QuasiPolynomial> theMultiplicities;
  HomomorphismSemisimpleLieAlgebra theHmm;
//  List<QuasiPolynomial> theMultiplicitiesExtremaCandidates;
  int UpperLimitChambersForDebugPurposes;
  int numNonZeroMults;
  Selection ParabolicLeviPartRootSpacesZeroStandsForSelected;
  Selection ParabolicSelectionSmallerAlgebra;
  List<Rational> theCoeffs;
  Vectors<Rational> theTranslationS;
  Vectors<Rational> theTranslationsProjectedBasisChanged;
  PartFractions thePfs;
//  List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
  ConeComplex projectivizedParamComplex;
  ConeLatticeAndShiftMaxComputation theMaxComputation;
  ConeComplex smallerAlgebraChamber;
  ConeComplex projectivizedChambeR;
  std::stringstream log;
  WeylGroup WeylSmaller;
  WeylGroup WeylLarger;
  int computationPhase;
  int NumProcessedConesParam;
  int NumProcessedExtremaEqualOne;
  std::string ComputeMultsLargerAlgebraHighestWeight(Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel, GlobalVariables& theGlobalVariables);
  std::string CheckMultiplicitiesVsOrbits(GlobalVariables& theGlobalVariables);
  std::string ElementToStringMultiplicitiesReport(GlobalVariables& theGlobalVariables);
  void IncrementComputation(Vector<Rational>& parabolicSel, GlobalVariables& theGlobalVariables);
  std::string PrepareReport(GlobalVariables& theGlobalVariables);
  GeneralizedVermaModuleCharacters()
  { this->UpperLimitChambersForDebugPurposes=-1;
    this->computationPhase=0;
    this->NumProcessedConesParam=0;
    this->NumProcessedExtremaEqualOne=0;
    this->numNonZeroMults=0;
  }
  void ReadFromDefaultFile(GlobalVariables* theGlobalVariables);
  void WriteToDefaultFile(GlobalVariables* theGlobalVariables);
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  bool ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
  { std::string tempS;
    int numReadWords;
    Vector<Rational>  parSel; parSel=this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
    if (theGlobalVariables!=0)
    { //this->theParser.theHmm.MakeG2InB3(this->theParser, *theGlobalVariables);
//      this->initFromHomomorphism(parSel, this->theParser.theHmm, *theGlobalVariables);
    } else
    { GlobalVariables tempGlobalVars;
//      this->theParser.theHmm.MakeG2InB3(this->theParser, tempGlobalVars);
  //    this->initFromHomomorphism(parSel, this->theParser.theHmm, tempGlobalVars);
    }
    XML::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
    input >> tempS >> this->computationPhase;
    if (tempS!="ComputationPhase:")
      crash << crash;
    bool result=true;
    if (this->computationPhase!=0)
      result= this->ReadFromFileNoComputationPhase(input, theGlobalVariables);
    XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
    return result;
  }
  bool ReadFromFileNoComputationPhase(std::fstream& input, GlobalVariables* theGlobalVariables);
  std::string PrepareReportOneCone(FormatExpressions& theFormat, const Cone& theCone, GlobalVariables& theGlobalVariables);
  void GetProjection(int indexOperator, const Vector<Rational> & input, Vector<Rational> & output);
  void SplitByMultiplicityFreeWall(Cone& theCone, ConeComplex& output);
  void InitTheMaxComputation(GlobalVariables& theGlobalVariables);
  void ComputeQPsFromChamberComplex(GlobalVariables& theGlobalVariables);
  void GetSubFromIndex(PolynomialSubstitution<Rational>& outputSub, Matrix<LargeInt>& outputMat, LargeIntUnsigned& ouputDen, int theIndex);
  void SortMultiplicities(GlobalVariables& theGlobalVariables);
  void GetSubFromNonParamArray(Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams);
  void initQPs(GlobalVariables& theGlobalVariables);
  void initFromHomomorphism(Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables);
  void TransformToWeylProjectiveStep1(GlobalVariables& theGlobalVariables);
  void TransformToWeylProjectiveStep2(GlobalVariables& theGlobalVariables);
  void TransformToWeylProjective(int indexOperator, Vector<Rational> & startingNormal, Vector<Rational> & outputNormal);
};


template<typename Element>
void Matrix<Element>::GetVectorFromRow(int rowIndex, Vector<Element>& output)const
{ output.SetSize(this->NumCols);
  for (int i=0; i<this->NumCols; i++)
    output.TheObjects[i]=this->elements[rowIndex][i];
}

template<typename Element>
void Matrix<Element>::GetVectorFromColumn(int colIndex, Vector<Element>& output)const
{ output.SetSize(this->NumRows);
  for (int i=0; i<this->NumRows; i++)
    output[i]=this->elements[i][colIndex];
}

class KLpolys: public HashedList<Vector<Rational> >
{
public:
  WeylGroup* TheWeylGroup;
  List<int> TheMultiplicities;
  List<bool> Explored;
  int NextToExplore;
  int LowestNonExplored;
  List<List<int> > BruhatOrder;
  List<List<int> > SimpleReflectionsActionList;
  List<List<int> > InverseBruhatOrder;
  std::string DebugString;
  List<List<Polynomial<Rational> > > theKLPolys;
  List<List<Polynomial<Rational> > > theRPolys;
  List<List<Rational> > theKLcoeffs;
  void KLcoeffsToString(List<int>& theKLCoeffs, std::string& output);
  void FindNextToExplore();
  int FindMinimalBruhatNonExplored(List<bool>& theExplored);
  int FindMaximalBruhatNonExplored(List<bool>& theExplored);
  void initTheMults();
  void Compute(int x);
  void Check();
  //returns true if computation succeeded, false otherwise
  bool ComputeRxy(int x, int y, int SimpleReflectionIndex);
  void ComputeKLxy(int x, int y);
  bool IsMaxNonEplored(int index);
  bool IndexGEQIndex(int a, int b);
  bool IndexGreaterThanIndex(int a, int b);
  void ComputeDebugString();
  std::string ToString(FormatExpressions* theFormat=0);
  void MergeBruhatLists(int fromList, int toList);
  std::string KLPolysToString(FormatExpressions* theFormat=0);
  void ComputeKLcoefficients();
  int ChamberIndicatorToIndex(Vector<Rational>& ChamberIndicator);
  std::string RPolysToString(FormatExpressions* theFormat=0);
  bool ComputeKLPolys(WeylGroup* theWeylGroup);
  void ComputeRPolys();
  int ComputeProductfromSimpleReflectionsActionList(int x, int y);
  void WriteKLCoeffsToFile(std::fstream& output, List<int>& KLcoeff, int TopIndex);
  //returns the TopIndex of the KL coefficients
  int ReadKLCoeffsFromFile(std::fstream& input, List<int>& output);
  KLpolys(){this->TheWeylGroup=0; }
  void GeneratePartialBruhatOrder();
  void ExtendOrder();
  void ComputeFullBruhatOrder();
  void initFromWeyl(WeylGroup* theWeylGroup);
};

template <class Element>
void Matrix<Element>::ComputeDeterminantOverwriteMatrix(Element& output, const Element& theRingOne, const Element& theRingZero)
{ int tempI;
  output=theRingOne;
  Element tempRat;
  if (this->NumCols!=this->NumRows)
    crash << crash;
  int dim =this->NumCols;
  for (int i=0; i<dim; i++)
  {  //this->ComputeDebugString();
    tempI = this->FindPivot(i, i);
    if (tempI==-1)
    { output=theRingZero;
      return;
    }
    this->SwitchTwoRows(i, tempI);
    if(tempI!=i)
      output*=-1;
    tempRat=(this->elements[i][i]);
    output*=(tempRat);
    tempRat.Invert();
    this->RowTimesScalar(i, tempRat);
    for (int j=i+1; j<dim; j++)
      if (!this->elements[j][i].IsEqualToZero())
      { tempRat=(this->elements[j][i]);
        tempRat.Minus();
        this->AddTwoRows (i, j, i, tempRat);
      }
  }
}

template<class Element>
void Matrix<Element>::Substitution(const PolynomialSubstitution<Rational>& theSub)
{ for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      this->elements[i][j].Substitution(theSub);
}

template <class coefficient>
std::string MonomialTensorGeneralizedVermas<coefficient>::ToString(FormatExpressions* theFormat, bool includeV)const
{ std::stringstream out;
  std::string tempS;
  if (this->theMons.size>1)
    for (int i=0; i<this->theMons.size; i++)
    { bool ishwv=this->theMons[i].IsHWV();
      if (!ishwv)
        out << "(";
      out << this->theMons[i].ToString(theFormat, includeV);
      if (!ishwv)
        out  << ")";
      if (i!=this->theMons.size-1)
        out << "\\otimes ";
    }
  else
    out << this->theMons[0].ToString(theFormat, includeV);
//  std::cout << "<br>" << out.str() << " has " << this->theMons.size << " multiplicands with hash functions: ";
//  for (int i=0; i<this->theMons.size; i++)
//    std::cout << this->theMons[i].HashFunction() << ", ";
  return out.str();
}

template <class coefficient>
std::string MonomialGeneralizedVerma<coefficient>::ToString(FormatExpressions* theFormat, bool includeV)const
{ if (this->owneR==0)
    crash << "This is a programming error: non-initialized generalized Verma monomial (owner is 0)." << crash;
  ModuleSSalgebra<coefficient>& theMod=*this->owneR;
  std::string tempS;
  if (tempS=="1")
    tempS="";
  if (tempS=="-1")
    tempS="-";
  tempS+=this->theMonCoeffOne.ToString(theFormat);
  if (tempS=="1")
    tempS="";
  if (tempS=="-1")
    tempS="-";
  bool needsCdot=(tempS!="1" && tempS!="-" && tempS!="");
  std::stringstream out;
  out << tempS;
  tempS= theMod.theGeneratingWordsNonReduced[this->indexFDVector].ToString(theFormat);
  if (tempS!="1")
    out << tempS;
  needsCdot=needsCdot || (tempS!="1" && tempS!="-");
  if (needsCdot)
    out << "\\cdot ";
  if (includeV)
    out << theMod.ElementToStringHWV(theFormat);
//  std::cout << "<br>MonomialP " << out.str() << " has indexInOwner " << this->indexInOwner;
  return out.str();
}

template <class TemplateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& output, const MonomialCollection<TemplateMonomial, coefficient>& theCollection)
{ if (theCollection.size()==0)
  { output << "0";
    return output;
  }
  std::string tempS1, tempS2;
  List<TemplateMonomial> sortedMons;
  sortedMons=theCollection.theMonomials;
  sortedMons.QuickSortDescending();
//  out << "(hash: " << this->HashFunction() << ")";
  int cutOffCounter=0;
  std::string oldCustomTimes="";
  for (int i=0; i<sortedMons.size; i++)
  { TemplateMonomial& currentMon=sortedMons[i];
    std::stringstream tempStream;
    coefficient& currentCoeff=theCollection.theCoeffs[theCollection.theMonomials.GetIndex(currentMon)];
    tempStream << currentCoeff;
    tempS1=tempStream.str();
    tempS2=currentMon.ToString();
    if (tempS1=="1" && tempS2!="1")
      tempS1="";
    if (tempS1=="-1"&& tempS2!="1")
      tempS1="-";
    if(tempS2!="1")
      tempS1+=tempS2;
    if (i>0)
    { if (tempS1.size()>0)
      { if (tempS1[0]!='-')
        { output << "+";
          cutOffCounter+=1;
        }
      } else
      { output << "+";
        cutOffCounter+=1;
      }
    }
    output << tempS1;
    cutOffCounter+=tempS1.size();
  }
  return output;
}

template <class Element>
void PolynomialSubstitution<Element>::MakeLinearSubConstTermsLastRow(Matrix<Element>& theMat)
{ this->SetSize(theMat.NumCols);
  MonomialP tempM;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].MakeZero();
    for (int j=0; j<theMat.NumRows-1; j++)
    { tempM.MakeOne(theMat.NumRows-1);
      tempM[j]=1;
      this->TheObjects[i].AddMonomial(tempM, theMat.elements[j][i]);
    }
    this->TheObjects[i]+=(theMat.elements[theMat.NumRows-1][i]);
  }
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods)
{ //std::cout << "<br>ze ue mon before sub: " << this->theMonCoeffOne.ToString();
  this->theMonCoeffOne.Substitution(theSub);
  //std::cout << "<br>ze ue mon after sub: " << this->theMonCoeffOne.ToString();
  ModuleSSalgebra<coefficient> newOwner;
  newOwner=*this->owneR;
  newOwner.Substitution(theSub);
  //std::cout << "<br>old index in owner: " << this->indexInOwner;
  int newModIndex=theMods.AddNoRepetitionOrReturnIndexFirst(newOwner);
  this->owneR=&theMods[newModIndex];
  //std::cout << "<br>new index in owner: " << this->indexInOwner;
}

template <class coefficient>
void MonomialTensorGeneralizedVermas<coefficient>::operator=
(const MonomialGeneralizedVerma<coefficient>& other)
{ this->theMons.SetSize(1);
  this->theMons[0]=other;
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::MultiplyMeByUEEltOnTheLefT
(const ElementUniversalEnveloping<coefficient>& theUE, ElementSumGeneralizedVermas<coefficient>& output,
GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero)const
{ MacroRegisterFunctionWithName("MonomialGeneralizedVerma<coefficient>::MultiplyMeByUEEltOnTheLefT");
  MonomialGeneralizedVerma<coefficient> currentMon;
  output.MakeZero();
  ElementSumGeneralizedVermas<coefficient> buffer;
  ProgressReport theReport(&theGlobalVariables);
  for (int j=0; j<theUE.size(); j++)
  { currentMon.theMonCoeffOne=theUE[j];
//    std::cout << "<br>currentMon: " << currentMon.theMonCoeffOne.ToString();
    currentMon.theMonCoeffOne*=this->theMonCoeffOne;
//    std::cout << "<br>currentMon after multi: " << currentMon.theMonCoeffOne.ToString();
    currentMon.owneR=this->owneR;
    currentMon.indexFDVector=this->indexFDVector;
    currentMon.owneR=this->owneR;
//    std::cout << "<hr>Applying " <<theUE.theCoeffs[j].ToString() << " times " << theUE[j].ToString() << " on " << this->ToString();
    std::stringstream reportStream;
    reportStream << "reducing mon: " << currentMon.ToString() << ", index" << j+1 << " out of " << theUE.size() << "...";
//    std::cout << "reducing mon: " << currentMon.ToString() << ", index" << j+1 << " out of " << theUE.size() << "...";
    theReport.Report(reportStream.str());
    currentMon.ReduceMe(buffer, theGlobalVariables, theRingUnit, theRingZero);
    reportStream << " done.";
//    std::cout << " done.";
    theReport.Report(reportStream.str());
//    std::cout << "<br>buffer: " << buffer.ToString() << " multiplied by " << theUE.theCoeffs[j].ToString();
    buffer*=theUE.theCoeffs[j];
    output+=buffer;
//    std::cout << " equals: " << buffer.ToString();
  }
//  std::cout << "<br>result: " << this->ToString();
}

template <class coefficient>
void ElementSumGeneralizedVermas<coefficient>::MultiplyMeByUEEltOnTheLeft
  (const ElementUniversalEnveloping<coefficient>& theUE, GlobalVariables& theGlobalVariables,
   const coefficient& theRingUnit, const coefficient& theRingZero)
{ MacroRegisterFunctionWithName("ElementSumGeneralizedVermas<coefficient>::MultiplyMeByUEEltOnTheLeft");
  ElementSumGeneralizedVermas<coefficient> buffer, Accum;
//std::cout << "<br>Multiplying " << this->ToString() << " by " << theUE.ToString();
  Accum.MakeZero();
  for (int i=0; i<this->size(); i++)
  {// std::cout << "<br>Multiplying " << this->TheObjects[i].ToString() << " by " << theUE.ToString() << " by " << this->theCoeffs[i].ToString();
    (*this)[i].MultiplyMeByUEEltOnTheLefT(theUE, buffer, theGlobalVariables, theRingUnit, theRingZero);
    //std::cout << "<br>buffer " << buffer.ToString() << " multiplied by coeff " << this->theCoeffs[i].ToString();
    buffer*=this->theCoeffs[i];
//    std::cout << "<br>to obtain " << buffer.ToString();
    Accum+=buffer;
//    std::cout << " <br> to accummulate to " << Accum.ToString();
  }
  *this=Accum;
//  std::cout << "<br>To get in the damned end: " << this->ToString();
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::ReduceMe(ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero)const
{ MacroRegisterFunctionWithName("MonomialGeneralizedVerma::ReduceMe");
  //std::cout << "<hr><hr>Reducing  " << this->ToString();
  ModuleSSalgebra<coefficient>& theMod=*this->owneR;
  output.MakeZero();
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon=this->theMonCoeffOne;
  tempMon*=theMod.theGeneratingWordsNonReduced[this->indexFDVector];
  int indexCheck=theMod.theGeneratingWordsNonReduced.GetIndex(tempMon);
  if (indexCheck!=-1)
  { MonomialGeneralizedVerma<coefficient> basisMon;
    basisMon.MakeConst(*this->owneR);
    basisMon.indexFDVector=indexCheck;
    output.AddMonomial(basisMon, theRingUnit);
//    std::cout << "<br>Reduced " << this->ToString() << " to " << output.ToString() << " = " << basisMon.ToString();
//    std::cout << "<br> index check is " << indexCheck << " corresponding to " << theMod.theGeneratingWordsNonReduced[indexCheck].ToString();
//    theGlobalVariables.MakeProgressReport("Monomial basis of fd part. ", 2);
    return;
  }
//  std::cout << "<br>Not a monomial basis of fd part";
//  theGlobalVariables.MakeProgressReport("Monomial not basis of fd part. ", 2);
  theMod.GetOwner().OrderSetNilradicalNegativeMost(theMod.parabolicSelectionNonSelectedAreElementsLevi);
//  std::cout << "<br>";
//  for (int i=0; i<theMod.GetOwner().UEGeneratorOrderIncludingCartanElts.size; i++)
//  { std::cout << "<br>generator index " << i << " has order " << theMod.GetOwner().UEGeneratorOrderIncludingCartanElts[i];
//  }
  ElementUniversalEnveloping<coefficient> theUEelt;
  theUEelt.MakeZero(*this->GetOwner().owneR);
  theUEelt.AddMonomial(this->theMonCoeffOne, theRingUnit);
//  std::cout << " <br>the monomial:" << this->ToString();
  theUEelt.Simplify(&theGlobalVariables, theRingUnit, theRingZero);
//  std::cout << " <br>the corresponding ue with F.D. part cut off: " << theUEelt.ToString();

  MonomialUniversalEnveloping<coefficient> currentMon;
  MonomialGeneralizedVerma<coefficient> newMon;
  MatrixTensor<coefficient> tempMat1, tempMat2;
  //std::cout << theMod.ToString();
  //std::cout << "<br>theMod.theModuleWeightsSimpleCoords.size: "
  //<< theMod.theModuleWeightsSimpleCoords.size;
  ProgressReport theReport(&theGlobalVariables);
  coefficient theCF;
  for (int l=0; l<theUEelt.size(); l++)
  { currentMon=theUEelt[l];
    //std::cout << "<br> Processing monomial " << currentMon.ToString();
    tempMat1.MakeIdSpecial();
    for (int k=currentMon.Powers.size-1; k>=0; k--)
    { std::stringstream reportStream;
      reportStream << "accounting monomial " << currentMon.ToString() << " of index " << l+1 << " out of "
      << theUEelt.size() << " and letter index " << currentMon.Powers.size-k << " out of " << currentMon.Powers.size << "...";
      theReport.Report(reportStream.str());
      int thePower;
      if (!currentMon.Powers[k].IsSmallInteger(&thePower))
        break;
      int theIndex=currentMon.generatorsIndices[k];
      if (theMod.HasFreeAction(theIndex))
        break;
      tempMat2=tempMat1;
      tempMat1=theMod.GetActionGeneratorIndeX
      (theIndex, theGlobalVariables, theRingUnit, theRingZero);
      tempMat1.RaiseToPower(thePower);
      tempMat1*=tempMat2;
      currentMon.Powers.size--;
      currentMon.generatorsIndices.size--;
      reportStream << "done!";
      theReport.Report(reportStream.str());
    }
//    std::cout << "<br> Action is the " << currentMon.ToString() << " free action plus <br>" << tempMat1.ToString();
    newMon.owneR=this->owneR;
    for (int i=0; i<tempMat1.size(); i++)
    { int otherIndex=-1;
      if (tempMat1[i].dualIndex==this->indexFDVector)
        otherIndex=tempMat1[i].vIndex;
      if (tempMat1[i].IsId)
        otherIndex=this->indexFDVector;
      if (otherIndex!=-1)
      { newMon.theMonCoeffOne=currentMon;
        newMon.indexFDVector=otherIndex;
        theCF=theUEelt.theCoeffs[l];
        theCF*=tempMat1.theCoeffs[i];
        //std::cout << "<br>adding to " << output.ToString()
        //<< " the monomial " << newMon.ToString() << " with coefficient "
        //<< theCF.ToString() << " to obtain ";
        output.AddMonomial(newMon, theCF);
        //std::cout << output.ToString();
      }
    }
  }
//  std::cout << "<br>Matrix of the action: " << tempMat1.ToString();
//  std::cout << "<br> Final output: " << output.ToString();
  theMod.GetOwner().OrderSSLieAlgebraStandard();
}

template<class coefficient>
coefficient WeylGroup::WeylDimFormulaSimpleCoords(Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit)
{ coefficient Result, buffer;
  Vector<coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  rhoOverNewRing=this->rho;//<-type conversion here!
  Result=theRingUnit;
//  std::cout << "<br>doing the weyl dim formula with: " << theWeightInSimpleCoords.ToString();
//  std::cout << "<br>rho is:" << rhoOverNewRing.ToString();
//  std::cout << "<br>rho before conversion: " << this->rho.ToString();
//  std::cout << "<br>we get: ";
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { rootOfBorelNewRing=this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho=rhoOverNewRing+theWeightInSimpleCoords;
    buffer=(this->RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing));
    buffer/=this->RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
//    std::cout << "(" << buffer.ToString() << ")";
    Result*=buffer;
  }
  return Result;
}

template<class coefficient>
coefficient ReflectionSubgroupWeylGroup::WeylDimFormulaSimpleCoords(const Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit)
{ coefficient Result, buffer;
  Vector<coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  Vector<Rational> rho;
  this->RootsOfBorel.sum(rho, this->AmbientWeyl.GetDim());
  rho/=2;
  rhoOverNewRing=rho;//<-type conversion here!
  Result=theRingUnit;
//  std::cout << "<br>doing the weyl dim formula with: " << theWeightInSimpleCoords.ToString();
//  std::cout << "<br>rho is:" << rhoOverNewRing.ToString();
//  std::cout << "<br>we get: ";
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { rootOfBorelNewRing=this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho=rhoOverNewRing+theWeightInSimpleCoords;
    buffer=(this->AmbientWeyl.RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing));
    buffer/=this->AmbientWeyl.RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
//    std::cout << "(" << buffer.ToString() << ")";
    Result*=buffer;
  }
  return Result;
}

template<class coefficient>
Vector<coefficient> WeylGroup::GetFundamentalCoordinatesFromSimple
(const Vector<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational> & tempMat=*this->GetMatrixSimpleToFundamentalCoords();
  Vector<coefficient> result=inputInFundamentalCoords;
  tempMat.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vector<coefficient> WeylGroup::GetSimpleCoordinatesFromFundamental
(const Vector<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat=*this->GetMatrixFundamentalToSimpleCoords();
  Vector<coefficient> result;
  result=inputInFundamentalCoords;
//  std::cout << "<br>transition matrix from fundamental to simple: " << tempMat.ToString();
  tempMat.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vectors<coefficient> WeylGroup::GetSimpleCoordinatesFromFundamental
(const Vectors<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat=*this->GetMatrixFundamentalToSimpleCoords();
  Vectors<coefficient> result;
  result=inputInFundamentalCoords;
//  std::cout << "<br>transition matrix from fundamental to simple: " << tempMat.ToString();
  tempMat.ActOnVectorsColumn(result);
  return result;
}

template<class coefficient>
coefficient WeylGroup::WeylDimFormulaFundamentalCoords(Vector<coefficient>& weightFundCoords, const coefficient& theRingUnit)
{ Vector<coefficient> theWeightInSimpleCoords;
  theWeightInSimpleCoords = this->GetSimpleCoordinatesFromFundamental(weightFundCoords);
  return this->WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
}

template<class leftType, class rightType>
void WeylGroup::RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output)const
{ if (r1.size!=r2.size || r1.size!=this->GetDim())
  { crash.theCrashReport << "This is a programming error: attempting to get the scalar product of the weight " << r1 << " (dimension " << r1.size
    << ") with the weight " << r2 << " (dimension " << r2.size << "), while the dimension of the ambient Weyl group is " << this->GetDim()
    << ". ";
    crash << crash;
  }
  output=r1[0].GetZero();
  leftType buffer;
  for (int i=0; i<this->CartanSymmetric.NumRows; i++)
    for (int j=0; j<this->CartanSymmetric.NumCols; j++)
    { buffer=r1[i];
      buffer*=r2[j];
      buffer*=this->CartanSymmetric.elements[i][j];
      output+=(buffer);
    }
}

template<class coefficient>
void Vectors<coefficient>::IntersectTwoLinSpaces
(const List<Vector<coefficient> >& firstSpace, const List<Vector<coefficient> >& secondSpace, List<Vector<coefficient> >& output, GlobalVariables* theGlobalVariables)
{ //std::cout << "<br>*****Debugging Intersection linear spaces: ";
  //std::cout << "<br>input first space: " << firstSpace.ToString();
  //std::cout << "<br>input second space: " << secondSpace.ToString();
  Vectors<coefficient> firstReduced, secondReduced;
  Selection tempSel;
  Vectors<coefficient>::SelectABasisInSubspace(firstSpace, firstReduced, tempSel, theGlobalVariables);
  Vectors<coefficient>::SelectABasisInSubspace(secondSpace, secondReduced, tempSel, theGlobalVariables);
//  std::cout << "<br>first selected basis: " << firstReduced.ToString();
//  std::cout << "<br>second selected basis: " << secondReduced.ToString();
  if (firstReduced.size==0 || secondReduced.size==0)
  { output.size=0;
    return;
  }
  int theDim=firstReduced[0].size;
  Matrix<coefficient> theMat;
  theMat.init(theDim, firstReduced.size+secondReduced.size);
  for (int i=0; i<theDim; i++)
  { for (int j=0; j<firstReduced.size; j++)
      theMat(i,j)=firstReduced[j][i];
    for (int j=0; j<secondReduced.size; j++)
    { theMat(i,firstReduced.size+j)=0;
      theMat(i,firstReduced.size+j)-=secondReduced[j][i];
    }
  }
//  std::cout << "<br>The matrix before the gaussian elimination:" << theMat.ToString();
  theMat.GaussianEliminationByRows(0, &tempSel);
//  std::cout << "<br>The matrix after the gaussian elimination:" << theMat.ToString();
  output.ReservE(tempSel.CardinalitySelection);
  output.size=0;
  Vector<coefficient> nextIntersection;
  for(int i=0; i<tempSel.CardinalitySelection; i++)
  { int currentIndex=tempSel.elements[i];
//    std::cout << "<br>current pivot index : " << currentIndex;
    if(currentIndex<firstReduced.size)
      crash << crash;
    nextIntersection.MakeZero(theDim);
    for (int j=0; j<firstReduced.size; j++)
      if (!tempSel.selected[j])
        nextIntersection+=firstReduced[j]*theMat.elements[j][currentIndex];
    output.AddOnTop(nextIntersection);
  }
//  std::cout << "<br> final output: " << output.ToString();
//  std::cout << "<br>******************End of debugging linear space intersections";
}

template <class coefficient>
bool ReflectionSubgroupWeylGroup::GetAlLDominantWeightsHWFDIM
(Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ReflectionSubgroupWeylGroup::GetAlLDominantWeightsHWFDIM");
  std::stringstream out;
  this->ComputeRootSubsystem();
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<coefficient> highestWeightTrue=highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<coefficient> highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.GetVectorRational().SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  Vector<coefficient> currentWeight, currentWeightRaisedToDominant;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<coefficient> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<this->RootsOfBorel.size; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
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
    out << "<hr>This message is generated either because the number of weights has exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter is the case, make sure to send an angry email to the author(s).";
  outputDetails=out.str();
  //std::cout << "<hr><hr>Total time spent generating weights: " << -startTime+theGlobalVariables.GetElapsedSeconds();
  return (numTotalWeightsFound<=upperBoundDominantWeights);
}

template <class coefficient>
void ReflectionSubgroupWeylGroup::RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign, bool* stabilizerFound)
{ MacroRegisterFunctionWithName("ReflectionSubgroupWeylGroup::RaiseToDominantWeight");
  if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  Rational theScalarProd;
//  int theDim=this->AmbientWeyl.GetDim();
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<this->simpleGenerators.size; i++)
    { if (! this->IsDominantWRTgenerator(theWeight, i))
      { found=true;
        this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[i], theWeight, false, theWeight);
        if (sign!=0)
          *sign*=-1;
      }
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
    }
  }
//  std::cout << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

template <class coefficient>
void WeylGroup::ReflectBetaWRTAlpha(const Vector<Rational>& alpha, const Vector<coefficient>& Beta, bool RhoAction, Vector<coefficient>& Output)const
{ coefficient bufferCoeff, alphaShift, lengthA;
  Vector<coefficient> result;
  result=(Beta);
  alphaShift=Beta[0].GetZero();//<-the zero of coefficient is not known at compile time (think multivariate polynomials)
  lengthA=alphaShift;
  if (RhoAction)
    result+=(this->rho);//<-implicit type conversion here if coefficient is not Rational
  for (int i=0; i<this->CartanSymmetric.NumRows; i++)
    for (int j=0; j<this->CartanSymmetric.NumCols; j++)
    { bufferCoeff=(result[j]);
      bufferCoeff*=(alpha[i]);
      bufferCoeff*=(this->CartanSymmetric.elements[i][j]*(-2));
      alphaShift+=(bufferCoeff);
      bufferCoeff=(alpha[i]);
      bufferCoeff*=(alpha[j]);
      bufferCoeff*=(this->CartanSymmetric.elements[i][j]);
      lengthA+=(bufferCoeff);
    }
  alphaShift/=(lengthA);
  Output.SetSize(this->CartanSymmetric.NumRows);
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { bufferCoeff=(alphaShift);
    bufferCoeff*=(alpha[i]);
    bufferCoeff+=(result[i]);
    Output[i]=(bufferCoeff);
  }
  if (RhoAction)
    Output-=this->rho;
}

template <class coefficient>
std::string MonomialChar<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool useOmega=true;
  std::string oldCustomPlus;
  std::string VectorSpaceLetter="V";
  if (theFormat!=0)
  { useOmega=(theFormat->fundamentalWeightLetter=="");
    oldCustomPlus=theFormat->CustomPlusSign;
    theFormat->CustomPlusSign="";
    VectorSpaceLetter=theFormat->FDrepLetter;
  }
  if (useOmega)
    out << VectorSpaceLetter << "_{" << this->weightFundamentalCoordS.ToStringLetterFormat("\\omega", theFormat) << "}";
  else
    out << VectorSpaceLetter << "_{"
    << this->weightFundamentalCoordS.ToStringLetterFormat(theFormat->fundamentalWeightLetter, theFormat) << "}";
  if (theFormat!=0)
    theFormat->CustomPlusSign=oldCustomPlus;
  return out.str();
}

template<class coefficient>
bool charSSAlgMod<coefficient>::DrawMe
(std::string& outputDetails, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults)
{ MacroRegisterFunctionWithName("charSSAlgMod::DrawMe");
  this->CheckNonZeroOwner();
  charSSAlgMod<coefficient> CharCartan;
  bool result= this->FreudenthalEvalMeDominantWeightsOnly
  (CharCartan, upperBoundWeights, &outputDetails, &theGlobalVariables);
  std::stringstream out;
  Vectors<Rational> currentOrbit;
  WeylGroup& theWeyl=this->GetOwner()->theWeyl;
  theWeyl.DrawRootSystem(theDrawingVars, false, theGlobalVariables, true);
  int totalNumWeights=0;
  Vectors<coefficient> dominantWeightsNonHashed;
  HashedList<Vector<coefficient> > finalWeights;
  Vector<Rational> convertor;
  for (int i=0; i< CharCartan.size(); i++)
  { const MonomialChar<coefficient>& currentMon=CharCartan[i];
    dominantWeightsNonHashed.size=0;
    dominantWeightsNonHashed.AddOnTop(theWeyl.GetSimpleCoordinatesFromFundamental(currentMon.weightFundamentalCoordS));
    bool isTrimmed=!theWeyl.GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0,  0, upperBoundWeights);
    totalNumWeights+=finalWeights.size;
    if (isTrimmed || totalNumWeights>upperBoundWeights)
    { out << "Did not generate all weights of the module due to RAM limits. ";
      result=false;
      break;
    }
    for (int j=0; j<finalWeights.size; j++)
    { convertor=finalWeights[j].GetVectorRational();
      theDrawingVars.drawCircleAtVectorBuffer(convertor, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
      if (useMults)
        theDrawingVars.drawTextAtVectorBuffer
        (convertor, CharCartan.theCoeffs[i].ToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
    }
  }
  out << "<br>Number of computed weights: " << totalNumWeights << ". ";
  if (result && totalNumWeights<upperBoundWeights)
    out << "<br>All weights were computed and are drawn. <br>";
  else
    out << "<br><b> Not all weights were computed. </b>";
  outputDetails=out.str();
  return result;
}

template <class coefficient>
void charSSAlgMod<coefficient>::DrawMeAssumeCharIsOverCartan(WeylGroup& actualAmbientWeyl, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars)const
{ if (actualAmbientWeyl.GetDim()<2)
    return;
  Vector<coefficient> actualWeight;
  Vector<Rational> actualWeightRationalPart;

  actualAmbientWeyl.DrawRootSystem(theDrawingVars, true, theGlobalVariables, false, 0, false);
  for (int j=0; j<this->size(); j++)
  { actualWeight=actualAmbientWeyl.GetSimpleCoordinatesFromFundamental((*this)[j].weightFundamentalCoordS);
    actualWeightRationalPart=actualWeight.GetVectorRational(); // <-type conversion here!
    theDrawingVars.drawCircleAtVectorBuffer(actualWeightRationalPart, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
    theDrawingVars.drawTextAtVectorBuffer(actualWeightRationalPart, this->theCoeffs[j].ToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
  }
}

template <class coefficient>
bool ReflectionSubgroupWeylGroup::GenerateOrbitReturnFalseIfTruncated(const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, int UpperLimitNumElements)
{ HashedList<Vector<coefficient> > theOrbit;
  bool result = true;
  theOrbit.Clear();
  Vector<coefficient> tempRoot;
  theOrbit.AddOnTop(input);
  MemorySaving<Vectors<coefficient> >ExternalAutosOverAmbientField;
  for (int i=0; i<theOrbit.size; i++)
  { for (int j=0; j<this->simpleGenerators.size; j++)
    { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[j], theOrbit[i], false, tempRoot);
//      int oldsize=theOrbit.size;
//      std::string debugString=tempRoot.ToString() ;
      theOrbit.AddOnTopNoRepetition(tempRoot);
//      if (oldsize<theOrbit.size)
//        std::cout << "<br>" << debugString << " with hash " << tempRoot.HashFunction() << " added, ";
//      else
//        std::cout << "<br>" << debugString << " with hash " << tempRoot.HashFunction() << " NOT added, ";
    }
    for (int j=1; j<this->ExternalAutomorphisms.size; j++)
    { ExternalAutosOverAmbientField.GetElement()=this->ExternalAutomorphisms[j];
      theOrbit[i].GetCoordsInBasiS(ExternalAutosOverAmbientField.GetElement(), tempRoot);
      theOrbit.AddOnTopNoRepetition(tempRoot);
    }
    if (UpperLimitNumElements>0)
      if (theOrbit.size>=UpperLimitNumElements)
      { result=false;
        break;
      }
  }
  outputOrbit=(theOrbit);
  return result;
}

template <class coefficient>
bool ReflectionSubgroupWeylGroup::FreudenthalEvalIrrepIsWRTLeviPart
(const Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoordS,
 List<coefficient>& outputMultsSimpleCoords, std::string& outputDetails, GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
{ MacroRegisterFunctionWithName("ReflectionSubgroupWeylGroup::FreudenthalEvalIrrepIsWRTLeviPart");
  //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRootSubsystem();
  Vector<Rational> EiVect;
  List<bool> Explored;
  coefficient theRingZero;
  theRingZero=inputHWfundamentalCoords[0].GetZero();
  /////////////////////////
  Vector<coefficient> hwSimpleCoordsLeviPart, hwSimpleCoordsNilPart;
  hwSimpleCoordsLeviPart=inputHWfundamentalCoords;
  hwSimpleCoordsNilPart=inputHWfundamentalCoords;
  for (int i=0; i<hwSimpleCoordsLeviPart.size; i++)
  { EiVect.MakeEi(hwSimpleCoordsLeviPart.size, i);
    if (!this->RootsOfBorel.Contains(EiVect))
      hwSimpleCoordsLeviPart[i]=theRingZero;
    else
      hwSimpleCoordsNilPart[i]=theRingZero;
  }
  hwSimpleCoordsLeviPart=this->AmbientWeyl.GetSimpleCoordinatesFromFundamental(hwSimpleCoordsLeviPart);
  hwSimpleCoordsNilPart=this->AmbientWeyl.GetSimpleCoordinatesFromFundamental(hwSimpleCoordsNilPart);
//  std::cout << "highest weight levi part simple coords: " << hwSimpleCoordsLeviPart.ToString();
//  std::cout << "highest weight nil part siple coords: " << hwSimpleCoordsNilPart.ToString();
  ///////////////////////////
  HashedList<Vector<coefficient> > outputDomWeightsSimpleCoordsLeviPart;

  if (!this->GetAlLDominantWeightsHWFDIM(hwSimpleCoordsLeviPart, outputDomWeightsSimpleCoordsLeviPart, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
  { std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of " << UpperBoundFreudenthal
    << ". Please check out whether LiE's implementation of the Freudenthal formula can do your computation.";
    outputDetails=errorLog.str();
    return false;
  }
//  std::cout << "Highest weight: " << hwSimpleCoords.ToString() << "<br>Dominant weights wrt. levi part("
//  << outputDomWeightsSimpleCoordsLeviPart.size << "):<br> ";
//  for (int i=0; i<outputDomWeightsSimpleCoordsLeviPart.size; i++)
//    std::cout << "<br>" << outputDomWeightsSimpleCoordsLeviPart[i].ToString();
  Explored.initFillInObject(outputDomWeightsSimpleCoordsLeviPart.size, false);
  outputMultsSimpleCoords.SetSize(outputDomWeightsSimpleCoordsLeviPart.size);
  Vector<coefficient> currentWeight, currentDominantRepresentative;
  Vector<coefficient> Rho;
  Rho=this->GetRho();//<-implicit type conversion here!
//  std::cout << "<br> Rho equals: " << Rho.ToString();
  //out << "<br> Rho equals: " << Rho.ToString();
  coefficient hwPlusRhoSquared;
  hwPlusRhoSquared=this->AmbientWeyl.RootScalarCartanRoot(hwSimpleCoordsLeviPart+Rho, hwSimpleCoordsLeviPart+Rho);
  Explored[0]=true;
  outputMultsSimpleCoords[0]=1;
//  std::cout << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing=0;
//  static double timeSpentRaisingWeights=0;
//std::cout << "Freudenthal eval w.r.t subalgebra: positive root subsystem: " <<  this->RootsOfBorel.ToString();
  Vector<coefficient> convertor;
  coefficient bufferCoeff;
  ProgressReport theReport(&theGlobalVariables);
  for (int k=1; k< outputDomWeightsSimpleCoordsLeviPart.size; k++)
  { Explored[k]=true;
    coefficient& currentAccum=outputMultsSimpleCoords[k];
    currentAccum=0;
    for (int j=0; j<this->RootsOfBorel.size; j++)
      for (int i=1; ; i++)
      { convertor=this->RootsOfBorel[j];
        convertor*=i;
        currentWeight= outputDomWeightsSimpleCoordsLeviPart[k]+convertor;
        currentDominantRepresentative=currentWeight;
//        double startLocal=theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        std::cout << "<br>currentDominant representative: " << currentDominantRepresentative.ToString();
        int theIndex=outputDomWeightsSimpleCoordsLeviPart.GetIndex(currentDominantRepresentative);
//        std::cout << "<br>index of currentDomain rep: " << theIndex;
        //totalTimeSpentOnHashIndexing+=theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex==-1)
          break;
//        std::cout << "<br> summing over weight: " << currentWeight.ToString();
        if (!Explored[theIndex])
        { std::stringstream errorLog;
          errorLog << "This is an internal error check. If you see it, that means "
          << " that the Freudenthal algorithm implementation is "
          << " wrong (because the author of the implementation was dumb enough to"
          << " try to write less code than what is "
          << " suggested by LiE).";
          outputDetails=errorLog.str();
          return false;
        }
        convertor=this->RootsOfBorel[j];//<-implicit type conversion here!
        bufferCoeff=this->AmbientWeyl.RootScalarCartanRoot(currentWeight, convertor);
        bufferCoeff*=outputMultsSimpleCoords[theIndex];
        currentAccum+=bufferCoeff;
//        std::cout << "<hr>current weight: " << currentWeight.ToString();
//        std::cout << "<br>current dominant representative " << currentDominantRepresentative.ToString();
      }
    currentAccum*=2;
   // std::cout << "<br>hwPlusRhoSquared: " << hwPlusRhoSquared.ToString();
    bufferCoeff=hwPlusRhoSquared;
    bufferCoeff-=this->AmbientWeyl.RootScalarCartanRoot(outputDomWeightsSimpleCoordsLeviPart[k]+Rho, outputDomWeightsSimpleCoordsLeviPart[k]+Rho);
    //bufferCoeff now holds the denominator participating in the Freudenthal formula.
    if(bufferCoeff.IsEqualToZero())
      crash << crash;
    currentAccum/=bufferCoeff;
//    std::cout << "<br>Coeff we divide by: " << bufferCoeff.ToString()
 //   ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k+1 << " out of " << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
    theReport.Report(out.str());
//    std::cout
  //  << "<hr> Computed the multiplicities of " << k+1 << " out of " << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
 //   theGlobalVariables.MakeStatusReport(out.str());
//    std::cout << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    std::cout << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    std::cout << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
  outputDominantWeightsSimpleCoordS.Clear();
  outputDominantWeightsSimpleCoordS.SetExpectedSize(outputDomWeightsSimpleCoordsLeviPart.size);
  for (int i=0; i<outputDomWeightsSimpleCoordsLeviPart.size; i++)
    outputDominantWeightsSimpleCoordS.AddOnTop(outputDomWeightsSimpleCoordsLeviPart[i]+hwSimpleCoordsNilPart);
//  std::cout << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

template <class coefficient>
bool charSSAlgMod<coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight
(std::string* Report, charSSAlgMod& output, const Selection& splittingParSel, const Selection& ParSelFDInducingPart,
 ReflectionSubgroupWeylGroup& outputWeylSub, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("charSSAlgMod<coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight");
  this->CheckNonZeroOwner();
  std::stringstream out;
  std::string tempS;
//  std::cout << "Splitting parabolic selection: " << splittingParSel.ToString();
  if (this->GetOwner()->GetRank()!=splittingParSel.MaxSize)
    crash << "This is a programming error: parabolic selection selects out of " << splittingParSel.MaxSize
    << " elements while the weyl group is of rank " << this->GetOwner()->GetRank() << ". " << crash;
  outputWeylSub.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, splittingParSel, theGlobalVariables, 1);
  outputWeylSub.ComputeRootSubsystem();
  ReflectionSubgroupWeylGroup complementGroup;
  Selection invertedSel;
  invertedSel=splittingParSel;
  invertedSel.InvertSelection();
  complementGroup.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, invertedSel, theGlobalVariables, 1);
  complementGroup.ComputeRootSubsystem();
  out << outputWeylSub.ToString(false);
//  std::cout << "<hr> the Weyl subgroup: " << outputWeylSub.ToString(false);
//  std::cout << this->ToString();
//  std::cout << out.str();
  charSSAlgMod charAmbientFDWeyl, remainingCharDominantLevi;
  ReflectionSubgroupWeylGroup theFDWeyl;
  theFDWeyl.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, ParSelFDInducingPart, theGlobalVariables, 1);
  MonomialChar<coefficient> tempMon, localHighest;
  List<coefficient> tempMults;
  HashedList<Vector<coefficient> > tempHashedRoots;
  WeylGroup& theWeyL=this->GetOwner()->theWeyl;
  charAmbientFDWeyl.MakeZero();
  coefficient bufferCoeff, highestCoeff;
  out << "Starting character: " << this->ToString();
  tempMon.owner=this->GetOwner();
  for (int i=0; i<this->size(); i++)
  { const MonomialChar<coefficient>& currentMon=(*this)[i];
    if (!theFDWeyl.FreudenthalEvalIrrepIsWRTLeviPart(currentMon.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
//    std::cout << "<hr>FreudenthalEval on " << currentMon.ToString() << " generated the following report: "
//    << tempS << "<hr>";
    for (int j=0; j<tempHashedRoots.size; j++)
    { bufferCoeff=this->theCoeffs[i];
      tempMon.weightFundamentalCoordS=theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      bufferCoeff*=tempMults[j];
      charAmbientFDWeyl.AddMonomial(tempMon, bufferCoeff);
    }
  }
//  std::cout << "<hr>" << tempS;
//  std::cout << "<hr>Freudenthal eval ends up being: " << charAmbientFDWeyl.ToString();

  remainingCharDominantLevi.MakeZero();
  Vectors<coefficient> orbitDom;
  tempMon.owner=this->GetOwner();
  for (int i=0; i<charAmbientFDWeyl.size(); i++)
  { orbitDom.SetSize(0);
    if (!theFDWeyl.GenerateOrbitReturnFalseIfTruncated
        (theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS), orbitDom, 10000))
    { out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS).ToString();
      if (Report!=0)
        *Report=out.str();
      return false;
    }
//    std::cout << "<hr>the orbit with highest weight "
//    << theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoords).ToString()
//    << " is: ";
//    for (int l=0; l<orbitDom.size; l++)
//      std::cout <<"<br>" << orbitDom[l].ToString();
//    std::cout << "<hr>of them dominant are: <br>";
    for (int k=0; k<orbitDom.size; k++)
      if (outputWeylSub.IsDominantWeight(orbitDom[k]))
      { tempMon.weightFundamentalCoordS=theWeyL.GetFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi.AddMonomial(tempMon, charAmbientFDWeyl.theCoeffs[i]);
//        std::cout << "<br>" << orbitDom[k].ToString() << " with coeff " << charAmbientFDWeyl.theCoeffs[i].ToString();
      }
  }
  output.MakeZero();
  out << "<br>Character w.r.t Levi part: " << CGI::GetMathMouseHover(remainingCharDominantLevi.ToString());
//  std::cout << "<br>Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginArrayL
// (remainingCharDominantLevi.ToString());

  Vector<coefficient> simpleGeneratorBaseField;
  while(!remainingCharDominantLevi.IsEqualToZero())
  { localHighest=*remainingCharDominantLevi.theMonomials.LastObject();
    for (bool Found=true; Found; )
    { Found=false;
      for (int i=0; i<outputWeylSub.simpleGenerators.size; i++)
      { tempMon=localHighest;
        simpleGeneratorBaseField=outputWeylSub.simpleGenerators[i]; // <- implicit type conversion here!
        tempMon.weightFundamentalCoordS+=this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharDominantLevi.theMonomials.Contains(tempMon))
        { localHighest=tempMon;
          Found=true;
        }
      }
    }
    highestCoeff=remainingCharDominantLevi.theCoeffs[remainingCharDominantLevi.theMonomials.GetIndex(localHighest)];
    output.AddMonomial(localHighest, highestCoeff);
    if (!outputWeylSub.FreudenthalEvalIrrepIsWRTLeviPart
        (localHighest.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
//    std::cout << "<hr>accounting " << localHighest.ToString() << " with coeff "
//    << highestCoeff.ToString() << "<br>"
//    << remainingCharDominantLevi.ToString();
    for (int i=0; i<tempHashedRoots.size; i++)
    { tempMon.owner=this->GetOwner();
      tempMon.weightFundamentalCoordS=theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoeff=tempMults[i];
      bufferCoeff*=highestCoeff;
      remainingCharDominantLevi.SubtractMonomial(tempMon, bufferCoeff);
//      std::cout << "<br>-(" << bufferCoeff.ToString() << ")" << tempMon.ToString();
    }
//    std::cout << "<br>remaining character after accounting:<br>" << remainingCharDominantLevi.ToString();
  }
  out << "<br>Character w.r.t Levi part: " << CGI::GetMathMouseHover(output.ToString());
  if (Report!=0)
  { //out << "<hr>"  << "The split character is: " << output.ToString("V", "\\omega", false);
    DrawingVariables theDV;
    std::string tempS;
    this->DrawMeNoMults(tempS, theGlobalVariables, theDV, 10000);
    Vector<Rational> tempRoot;
    out << "<hr>In the following weight visualization, a yellow line is drawn if the corresponding weights are "
    << " simple reflections of one another, with respect to a simple root of the Levi part of the parabolic subalgebra. ";
    for (int i=0; i<output.size(); i++)
    { tempRoot=theWeyL.GetSimpleCoordinatesFromFundamental(output[i].weightFundamentalCoordS).GetVectorRational();
      outputWeylSub.DrawContour(tempRoot, theDV, theGlobalVariables, CGI::RedGreenBlue(200, 200, 0), 1000);
      std::stringstream tempStream;
      tempStream << output.theCoeffs[i].ToString();
      theDV.drawTextAtVectorBuffer(tempRoot, tempStream.str(), 0, DrawingVariables::PenStyleNormal, 0);
    }
    out << "<hr>" << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyL.GetDim());
    *Report=out.str();
  }
  return true;
}

template <class coefficient>
void charSSAlgMod<coefficient>::MakeTrivial(SemisimpleLieAlgebra& inputOwner)
{ this->MakeZero();
  MonomialChar<Rational> tempMon;
  tempMon.owner=&inputOwner;
  tempMon.weightFundamentalCoordS.MakeZero(inputOwner.GetRank());
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
std::string charSSAlgMod<coefficient>::operator*=(const charSSAlgMod& other)
{ GlobalVariables theGlobalVariables;
  return this->MultiplyBy(other, theGlobalVariables);
}

template <class coefficient>
std::string charSSAlgMod<coefficient>::MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables)
{ if (this->GetOwner()!=other.GetOwner() || this->GetOwner()==0)
    crash << "This is a programming error: attempting to multiply characters of different or non-initialized semisimple Lie algebras." << crash;
  this->SetExpectedSize(other.size()+this->size());
  charSSAlgMod result, summand;
  result.MakeZero();
  std::string potentialError;
  coefficient theCF;
  for (int i=0; i<this->size(); i++)
    for (int j=0; j<other.size(); j++)
    { const MonomialChar<Rational>& left = (*this)[i];
      const MonomialChar<Rational>& right=other[j];
      potentialError=left.TensorAndDecompose(right, summand, theGlobalVariables);
      if (potentialError!="")
        return potentialError;
      theCF=this->theCoeffs[i];
      theCF*=other.theCoeffs[j];
      summand*=theCF;
      result+=summand;
    }
  this->operator=(result);
  return "";
}

template <class coefficient>
void MonomialChar<coefficient>::AccountSingleWeight
(const Vector<Rational>& currentWeightSimpleCoords, const Vector<Rational>& otherHighestWeightSimpleCoords,
 Rational& theMult, charSSAlgMod<coefficient>& outputAccum)const
{ //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J., Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  //std::cout << "<hr>Accounting " << currentWeightSimpleCoords.ToString()
  //<< " with coefficient " << finalCoeff.ToString();
  MacroRegisterFunctionWithName("MonomialChar_CoefficientType::AccountSingleWeight");
  this->CheckNonZeroOwner();
  Vector<Rational> dominant=currentWeightSimpleCoords;
  dominant+=otherHighestWeightSimpleCoords;
  WeylGroup& theWeyl = this->owner->theWeyl;
  dominant+=theWeyl.rho;
  int sign;
  // a weight has no stabilizer if and only if it is not stabilized by all root reflections.
  for (int i=0; i<theWeyl.RootsOfBorel.size; i++)
    if (theWeyl.RootScalarCartanRoot(dominant, theWeyl.RootsOfBorel[i]).IsEqualToZero())
      return;
//  std::cout << "<br> Before raising to dominant, in simple coords: " << dominant.ToString() << "<br>";
  theWeyl.RaiseToDominantWeight(dominant, &sign);
//  std::cout << "After raising to dominant: " << dominant.ToString() << "<br>";
  dominant-=theWeyl.rho;
  if (!theWeyl.IsDominantWeight(dominant))
    return;
  MonomialChar<Rational> tempMon;
  tempMon.owner=this->owner;
  tempMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(dominant);
  coefficient coeffChange;
  coeffChange= theMult;
  coeffChange*=sign;
//  std::cout << "; final contribution: " << tempMon.Coefficient.ToString()
//  << "*" << tempMon.weightFundamentalCoords.ToString() << "<br>";
  outputAccum.AddMonomial(tempMon, coeffChange);
}

template <class coefficient>
std::string MonomialChar<coefficient>::TensorAndDecompose
(const MonomialChar<coefficient>& other, charSSAlgMod<coefficient>& output, GlobalVariables& theGlobalVariables)const
{ //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J. Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  MacroRegisterFunctionWithName("MonomialChar_CoefficientType::TensorAndDecompose");
  this->CheckNonZeroOwner();
  std::stringstream errorLog;
  std::string tempS;
  output.MakeZero();
  WeylGroup& theWeyl=this->owner->theWeyl;
  Vector<Rational> leftHWFundCoords;
  leftHWFundCoords=this->weightFundamentalCoordS;
  Vector<Rational> rightHWFundCoords;
  rightHWFundCoords =other.weightFundamentalCoordS;

  Rational leftTotalDim= theWeyl.WeylDimFormulaFundamentalCoords(leftHWFundCoords);
  Rational rightTotalDim= theWeyl.WeylDimFormulaFundamentalCoords(rightHWFundCoords);
  if (leftTotalDim>rightTotalDim)
  { MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(leftHWFundCoords, rightHWFundCoords);
  }
  HashedList<Vector<Rational> > weightsLeftSimpleCoords;
  List<Rational> multsLeft;
  if (!theWeyl.FreudenthalEval(leftHWFundCoords, weightsLeftSimpleCoords, multsLeft, &tempS, &theGlobalVariables, 1000000))
  { errorLog << "Freudenthal formula generated error: " << tempS;
    return errorLog.str();
  }
  HashedList<Vector<Rational> > currentOrbit;
  const int OrbitSizeHardLimit=10000000;
//  int theRank=theWeyl.GetDim();
  Vector<Rational> rightHWSimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(rightHWFundCoords);
  Vectors<Rational> tempRoots;
  tempRoots.SetSize(1);
//  std::cout << "weights of smaller module: " << weightsLeftSimpleCoords.ToString();
  for (int i=0; i<weightsLeftSimpleCoords.size; i++)
  { tempRoots[0]=weightsLeftSimpleCoords[i];
    theWeyl.GenerateOrbit(tempRoots, false, currentOrbit, false, 0, 0, OrbitSizeHardLimit);
    if (currentOrbit.size>=OrbitSizeHardLimit)
    { errorLog << "Error: orbit layer size exceeded hard-coded limit of " << OrbitSizeHardLimit << ".";
      return errorLog.str();
    }
    for (int j=0; j<currentOrbit.size; j++)
      this->AccountSingleWeight(currentOrbit[j], rightHWSimpleCoords, multsLeft[i], output);
  }
//  std::cout << "<hr><hr><hr><hr>";
  return errorLog.str();
}

template <class coefficient>
bool charSSAlgMod<coefficient>::FreudenthalEvalMeFullCharacter
(charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::FreudenthalEvalMeFullCharacter");
  this->CheckNonZeroOwner();
  charSSAlgMod<coefficient> domChar;
  if (!this->FreudenthalEvalMeDominantWeightsOnly(domChar, upperBoundNumDominantWeights, outputDetails, theGlobalVariables))
    return false;
  outputCharOwnerSetToZero.MakeZero();
  Vectors<Rational> theVect;
  HashedList<Vector<Rational> > theOrbit;
  theVect.SetSize(1);
  MonomialChar<coefficient> tempMon;
  tempMon.owner=0;
  for (int i=0; i<domChar.size(); i++)
  { theVect[0]=this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental(domChar[i].weightFundamentalCoordS);
    if (!(this->GetOwner()->theWeyl.GenerateOrbit(theVect, false, theOrbit, false, -1, 0, upperBoundNumDominantWeights)))
    { if (outputDetails!=0)
        *outputDetails= "failed to generate orbit (possibly too large?)";
      return false;
    }
    for (int j=0; j<theOrbit.size; j++)
    { tempMon.weightFundamentalCoordS=this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(theOrbit[j]);
      outputCharOwnerSetToZero.AddMonomial(tempMon, domChar.theCoeffs[i]);
    }
  }
  return true;
}

template <class coefficient>
void charSSAlgMod<coefficient>::GetDual(charSSAlgMod<coefficient>& output)const
{ if (&output==this)
  { charSSAlgMod<coefficient> thisCopy=*this;
    thisCopy.GetDual(output);
    return;
  }
  MonomialChar<coefficient> tempM;
  output.MakeZero();
  for (int i=0; i<this->size(); i++)
  { tempM=(*this)[i];
    tempM.weightFundamentalCoordS.Minus();
    output.AddMonomial(tempM, this->theCoeffs[i]);
  }
}

template <class coefficient>
bool charSSAlgMod<coefficient>::FreudenthalEvalMeDominantWeightsOnly
(charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::FreudenthalEvalMeDominantWeightsOnly");
  if (&outputCharOwnerSetToZero==this)
  { charSSAlgMod<coefficient> thisCopy=*this;
    return thisCopy.FreudenthalEvalMeDominantWeightsOnly
    (outputCharOwnerSetToZero, upperBoundNumDominantWeights, outputDetails, theGlobalVariables);
  }
  this->CheckNonZeroOwner();
  outputCharOwnerSetToZero.MakeZero();
  Vector<coefficient> currentWeightFundCoords;
  List<coefficient> currentMults;
  HashedList<Vector<coefficient> > currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  MonomialChar<coefficient> tempMon;
  tempMon.owner=0;
  coefficient bufferCoeff;
  for (int i=0; i<this->size(); i++)
  { currentWeightFundCoords=(*this)[i].weightFundamentalCoordS;
    if (!this->GetOwner()->theWeyl.FreudenthalEval
    (currentWeightFundCoords, currentWeights, currentMults, &localDetail, theGlobalVariables, upperBoundNumDominantWeights))
    { if (outputDetails!=0)
      { localErrors << "Encountered error while evaluating freudenthal formula. Error details: " << localDetail
        << "<br> Further computation detail: " << localDetails.str();
        *outputDetails=localErrors.str();
      }
      return false;
    }
    if (localDetail!="")
      localDetails << "<br>MonomialP " << i+1 << " computation details: " << localDetail;
    for (int j=0; j<currentWeights.size; j++)
    { tempMon.weightFundamentalCoordS=this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(currentWeights[j]);
      bufferCoeff=this->theCoeffs[i];
      bufferCoeff*=currentMults[j];
      outputCharOwnerSetToZero.AddMonomial(tempMon, bufferCoeff);
    }
  }
  if (outputDetails!=0)
    *outputDetails=localDetails.str();
  return true;
}

template <class coefficient>
bool WeylGroup::GenerateOrbit
(Vectors<coefficient>& theRoots, bool RhoAction, HashedList<Vector<coefficient> >& output, bool UseMinusRho, int expectedOrbitSize,
 HashedList<ElementWeylGroup>* outputSubset, int UpperLimitNumElements)
{ output.Clear();
  for (int i=0; i<theRoots.size; i++)
    output.AddOnTop(theRoots[i]);
  Vector<coefficient> currentRoot;
  ElementWeylGroup currentElt;
  currentElt.owner=this;
  if (expectedOrbitSize<=0)
    if (!this->theDynkinType.GetSizeWeylGroupByFormula().IsSmallInteger(&expectedOrbitSize))
      expectedOrbitSize=-1;
  if (UpperLimitNumElements>0 && expectedOrbitSize>UpperLimitNumElements)
    expectedOrbitSize=UpperLimitNumElements;
  //std::cout << "<hr>Setting expected orbit size: " << expectedOrbitSize;
  output.SetExpectedSize(expectedOrbitSize);
  if (outputSubset!=0)
  { if (UpperLimitNumElements>0)
      expectedOrbitSize=MathRoutines::Minimum(UpperLimitNumElements, expectedOrbitSize);
    currentElt.reflections.SetSize(0);
    outputSubset->SetExpectedSize(expectedOrbitSize);
    outputSubset->Clear();
    outputSubset->AddOnTop(currentElt);
  }
  for (int i=0; i<output.size; i++)
    for (int j=0; j<this->CartanSymmetric.NumRows; j++)
    { currentRoot=output[i];
      //if (this->flagAnErrorHasOcurredTimeToPanic)
      //{ currentRoot.ComputeDebugString();
      //}
      if (!RhoAction)
        this->SimpleReflection(j, currentRoot);
      else if (!UseMinusRho)
        this->SimpleReflectionRhoModified(j, currentRoot);
      else
        this->SimpleReflectionMinusRhoModified(j, currentRoot);
      //if (this->flagAnErrorHasOcurredTimeToPanic)
      //{ currentRoot.ComputeDebugString();
      //}
      if (output.AddOnTopNoRepetition(currentRoot))
        if (outputSubset!=0)
        { currentElt=outputSubset->TheObjects[i];
          currentElt.reflections.AddOnTop(j);
          currentElt.MakeCanonical();
          outputSubset->AddOnTop(currentElt);
        }
      if (UpperLimitNumElements>0)
        if (output.size>=UpperLimitNumElements)
          return false;
    }
  return true;
}

template <class coefficient>
bool WeylGroup::FreudenthalEval
(Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
 List<coefficient>& outputMultsSimpleCoords, std::string* outputDetails, GlobalVariables* theGlobalVariables, int UpperBoundFreudenthal)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  MacroRegisterFunctionWithName("WeylGroup::FreudenthalEval");
  for (int i=0; i<inputHWfundamentalCoords.size; i++)
    if (inputHWfundamentalCoords[i]<0)
    { if (outputDetails!=0)
        *outputDetails= "The highest weight is not dominant and I cannot apply the Freudenthal formula.";
      return false;
    }
  this->ComputeRho(true);
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(this->GetDim());
  List<bool> Explored;
  Vector<coefficient> hwSimpleCoords=this->GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->GetAlLDominantWeightsHWFDIM(hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
    return false;
  Explored.initFillInObject(outputDominantWeightsSimpleCoords.size, false);
  outputMultsSimpleCoords.SetSize(outputDominantWeightsSimpleCoords.size);
  Vector<coefficient> currentWeight, currentDominantRepresentative, convertor;
  coefficient hwPlusRhoSquared;
  convertor=hwSimpleCoords;
  convertor+=this->rho;//<-implicit type conversion here!!!!
  hwPlusRhoSquared=this->RootScalarCartanRoot(convertor, convertor);
  outputMultsSimpleCoords[0]=1;
  Explored[0]=true;
//  std::cout << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing=0;
//  static double timeSpentRaisingWeights=0;
  coefficient BufferCoeff;
  ProgressReport theReport(theGlobalVariables);
  for (int k=1; k<outputDominantWeightsSimpleCoords.size; k++)
  { Explored[k]=true;
    coefficient& currentAccum=outputMultsSimpleCoords[k];
    currentAccum=0;
    for (int j=0; j<this->RootsOfBorel.size; j++)
      for (int i=1; ; i++)
      { currentWeight=outputDominantWeightsSimpleCoords[k];
        currentWeight+=this->RootsOfBorel[j]*i;
        currentDominantRepresentative=currentWeight;
//        double startLocal=theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        timeSpentRaisingWeights+=theGlobalVariables.GetElapsedSeconds()-startLocal;
        //double beforeHash=theGlobalVariables.GetElapsedSeconds();
        int theIndex=outputDominantWeightsSimpleCoords.GetIndex(currentDominantRepresentative);
        //totalTimeSpentOnHashIndexing+=theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex==-1)
          break;
//        std::cout << "<br> summing over weight: " << currentWeight.ToString();
        if (!Explored[theIndex])
        { if (outputDetails!=0)
          { std::stringstream errorLog;
            errorLog << "This is an internal error check. If you see it, that means that the Freudenthal algorithm implementation is "
            << " wrong (because the author of the implementation was dumb enough to try to write less code than what is suggested by LiE).";
            *outputDetails=errorLog.str();
          }
          return false;
        }
        BufferCoeff=this->RootScalarCartanRoot(currentWeight, this->RootsOfBorel[j]);
        BufferCoeff*=outputMultsSimpleCoords[theIndex];
        currentAccum+=BufferCoeff;
      }
    currentAccum*=2;
    convertor=outputDominantWeightsSimpleCoords[k];
    convertor+=this->rho;
    BufferCoeff=hwPlusRhoSquared;
    BufferCoeff-=this->RootScalarCartanRoot(convertor, convertor);
    if (BufferCoeff==0)
      crash << "This is a programming or a mathematical error. I get that the denominator in the Freundenthal formula is zero. "
      << " The highest weight is " << inputHWfundamentalCoords.ToString() << ". The Weyl group details follow. " << this->ToString() << crash;
    currentAccum/=BufferCoeff;

//    std::cout << "<br>Coeff we divide by: " << (hwPlusRhoSquared-this->RootScalarCartanRoot
 //   (outputDominantWeightsSimpleCoords[k]+this->rho, outputDominantWeightsSimpleCoords[k]+this->rho))
  //  .ToString()
   // ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k+1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theReport.Report(out.str());
//    std::cout
//    << "<hr> Computed the multiplicities of " << k+1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
//    std::cout << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    std::cout << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    std::cout << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
//  std::cout << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

template<class coefficient>
bool WeylGroup::GetAlLDominantWeightsHWFDIM
(Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<coefficient> highestWeightTrue;
  highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<coefficient> highestWeightFundCoords=this->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
  { if (outputDetails!=0)
    { out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.ToString()
      << " which equals " << highestWeightFundCoords.ToString() << "  in fundamental coordinates is not integral dominant.<br>";
      *outputDetails=out.str();
    }
    return false;
  }
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.GetVectorRational().SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem=this->RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsSimpleCoords.SetHashSizE(10000);
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  int numPosRoots=this->RootsOfBorel.size;
  Vector<coefficient> currentWeight;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for(int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords; lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<coefficient> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    std::cout << ". Time spent after accounting at height level "
//    << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    std::cout << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
    if (numTotalWeightsFound>upperBoundDominantWeights && upperBoundDominantWeights>0)
    { out << "<hr>The number of weights has exceeded the RAM memory limits, aborting the weight generation. ";
      return false;
    }
  }
  if (outputDetails!=0)
  { out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
    *outputDetails=out.str();
  }
  return true;
}

template <class coefficient>
void ElementSumGeneralizedVermas<coefficient>::MakeHWV(ModuleSSalgebra<coefficient>& theOwner, const coefficient& theRingUnit)
{ this->MakeZero();
  MonomialGeneralizedVerma<coefficient> theMon;
  theMon.indexFDVector=theOwner.theGeneratingWordsNonReduced.size-1;
  theMon.theMonCoeffOne.MakeOne(theOwner.GetOwner());
  theMon.owneR=&theOwner;
  this->AddMonomial(theMon, theRingUnit);
}

template <class coefficient>
void ReflectionSubgroupWeylGroup::ActByElement(int index, Vector<coefficient>& theRoot)const
{ Vector<coefficient> tempRoot;
  this->ActByElement(index, theRoot, tempRoot);
  theRoot=(tempRoot);
}

template <class coefficient>
void ReflectionSubgroupWeylGroup::ActByElement(const ElementWeylGroup& theElement, const Vector<coefficient>& input, Vector<coefficient>& output)const
{ if(&input==&output)
    crash << crash;
  int NumElts=theElement.reflections.size;
  Vector<coefficient> tempRoot, tempRoot2;
  output=(input);
  for (int i=0; i<NumElts; i++)
  { int tempI=theElement.reflections[i];
    if(tempI<this->simpleGenerators.size)
      this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[tempI], output, false, output);
    else
    { tempI-=this->simpleGenerators.size;
      tempRoot.MakeZero(input.size);
      for (int j=0; j<output.size; j++)
      { tempRoot2=this->ExternalAutomorphisms[tempI][j];
        tempRoot2*=output[j];
        tempRoot+=tempRoot2;
      }
    }
  }
}

template<class coefficient>
void ReflectionSubgroupWeylGroup::ActByElement(const ElementWeylGroup& theElement, const Vectors<coefficient>& input, Vectors<coefficient>& output)const
{ if(&input==&output)
    crash << crash;
  output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    this->ActByElement(theElement, input[i], output[i]);
}

template <class coefficient>
void WeylGroup::RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign, bool* stabilizerFound, ElementWeylGroup* raisingElt)
{ if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  coefficient theScalarProd;
  int theDim=this->GetDim();
  if (raisingElt!=0)
    raisingElt->reflections.SetSize(0);
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<theDim; i++)
    { theScalarProd=this->GetScalarProdSimpleRoot(theWeight, i);
      if (theScalarProd.IsNegative())
      { found=true;
        theScalarProd*=2;
        theScalarProd/=this->CartanSymmetric.elements[i][i];
        theWeight[i]-=theScalarProd;
        if (sign!=0)
          *sign*=-1;
        if (raisingElt!=0)
          raisingElt->reflections.AddOnTop(i);
      }
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
    }
  }
//  std::cout << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}


template<class Element>
bool Matrix<Element>::IsPositiveDefinite()
{ if (this->NumRows!=this->NumCols)
    crash << "This is a programming error: attempting to evaluate whether a matrix is positive definite, but the matrix is not square. " << crash;
  Element det;
  Matrix<Element> tempMat;
  for (int i=0; i< this->NumRows; i++)
  { tempMat.init(i+1, i+1);
    for (int j=0; j<tempMat.NumCols; j++)
      for (int k=0; k<tempMat.NumCols; k++)
        tempMat.elements[j][k]=this->elements[j][k];
    det=tempMat.GetDeterminant();
    if (det<=0)
      return false;
  }
  return true;
}

template <class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::GetCoordsInBasis(const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<coefficient>& output, GlobalVariables& theGlobalVariables)const
{ if (theBasis.size==0)
    return false;
  if (this->IsEqualToZero())
  { output.MakeZero(theBasis.size);
    return true;
  }
  MacroRegisterFunctionWithName("ElementSemisimpleLieAlgebra::GetCoordsInBasis");
  Vectors<coefficient> tempBasis;
  Vector<coefficient> tempRoot;
  tempBasis.SetSize(theBasis.size);
  for (int i=0 ; i<theBasis.size; i++)
    theBasis[i].ElementToVectorNegativeRootSpacesFirst(tempBasis[i]);
  this->ElementToVectorNegativeRootSpacesFirst(tempRoot);
  return tempRoot.GetCoordsInBasiS(tempBasis, output);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner)
{ //Changing RootSystem order invalidates this function!
  if (&inputOwner==0)
    crash << " This is a programming error: 0 pointer to Semisimple Lie algebra. " << crash;
  this->MakeZero();
  ChevalleyGenerator tempGenerator;
  tempGenerator.MakeGenerator(inputOwner, generatorIndex);
  this->AddMonomial(tempGenerator, 1);
}

template <class coefficient>
void SemisimpleLieAlgebra::LieBracket
(const ElementSemisimpleLieAlgebra<coefficient>& g1, const ElementSemisimpleLieAlgebra<coefficient>& g2, ElementSemisimpleLieAlgebra<coefficient>& output)
{ if (&output==&g1 || &output==&g2)
  { ElementSemisimpleLieAlgebra<coefficient> outputNew;
    this->LieBracket(g1, g2, outputNew);
    output=outputNew;
    return;
  }
  output.MakeZero();
  if (g1.IsEqualToZero() || g2.IsEqualToZero())
    return;
  int maxNumMonsFinal=g1.size()*g2.size();
  output.SetExpectedSize(maxNumMonsFinal);
  coefficient theCoeff;
  ElementSemisimpleLieAlgebra<coefficient> buffer;
  for (int i=0; i<g1.size(); i++)
    for (int j=0; j<g2.size(); j++)
    { buffer=this->theLiebrackets.elements[g1[i].theGeneratorIndex][g2[j].theGeneratorIndex];
      theCoeff=g1.theCoeffs[i];
      theCoeff*=g2.theCoeffs[j];
      buffer*=theCoeff;
      output+=buffer;
    }
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::ActOnMe(const ElementSemisimpleLieAlgebra<coefficient>& theElt, ElementSemisimpleLieAlgebra<coefficient>& output, SemisimpleLieAlgebra& owner)
{ owner.LieBracket(theElt, *this, output);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::ActOnMe
(const ElementSemisimpleLieAlgebra<coefficient>& theElt, ElementSemisimpleLieAlgebra<coefficient>& output, SemisimpleLieAlgebra& owner,
 const RationalFunctionOld& theRingUnit, const RationalFunctionOld& theRingZero, GlobalVariables* theGlobalVariables)
{ owner.LieBracket(theElt, *this, output);
}

template <class coefficient>
Vector<coefficient> ElementSemisimpleLieAlgebra<coefficient>::GetCartanPart()const
{ Vector<coefficient> result;
  if (this->IsEqualToZero())
  { result.MakeZero(0);
    return result;
  }
  ChevalleyGenerator tempGen;
  SemisimpleLieAlgebra* owner=this->GetOwner();
  int theRank=owner->GetRank();
  int numPosRoots=owner->GetNumPosRoots();
  result.MakeZero(theRank);
//  std::cout << "<br>Zero vector in GetCartanPart is: " << result.ToString();
  if (theRank<=0 || owner==0)
    crash << "This is a programming error: the owner of a semisimple Lie algebra element is non-present or corrupted. " << crash;
  for (int i=0; i<theRank; i++)
  { tempGen.MakeGenerator(*owner, i+numPosRoots);
    int currentIndex=this->theMonomials.GetIndex(tempGen);
    if (currentIndex!=-1)
      result[i]+=this->theCoeffs[currentIndex];
  }
//  std::cout << "<br>GetCartanPart is returning vector: " << result.ToString();
  return result;
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::MakeHgenerator(const Vector<coefficient>& theH, SemisimpleLieAlgebra& inputOwner)
{ ChevalleyGenerator tempGen;
  this->MakeZero();
  for (int i=0; i<theH.size; i++)
  { tempGen.MakeGenerator(inputOwner, inputOwner.GetCartanIndexFromGenerator(i));
    this->AddMonomial(tempGen, theH[i]);
  }
}

template <class coefficient>
void charSSAlgMod<coefficient>::MakeFromWeight(const Vector<coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner)
{ this->MakeZero();
  if (inputWeightSimpleCoords.size!=inputOwner->GetRank())
    crash << "This is a programming error: attempting to create a character from highest weight in simple coords "
    << inputWeightSimpleCoords.ToString() << "(" << inputWeightSimpleCoords.size << " coordinates) while the owner semisimple "
    << " Lie algebra is of rank " << (inputOwner->GetRank()) << crash;
  MonomialChar<coefficient> theMon;
  theMon.owner=inputOwner;
  theMon.weightFundamentalCoordS=
  inputOwner->theWeyl.GetFundamentalCoordinatesFromSimple(inputWeightSimpleCoords);
  this->AddMonomial(theMon, 1);
}

template<class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::IsElementCartan()const
{ if (this->size()==0)
    return true;
  SemisimpleLieAlgebra* owner= (*this)[0].owneR;
  for (int i=0; i<this->size(); i++)
    if (!owner->IsGeneratorFromCartan((*this)[i].theGeneratorIndex))
      return false;
  return true;
}

template <typename coefficient>
unsigned int WeylGroupRepresentation<coefficient>::HashFunction()const
{ unsigned int result= this->OwnerGroup==0? 0 : this->OwnerGroup->HashFunction();
  result+=this->theCharacter.HashFunction();
  return result;
}

template <typename coefficient>
std::string WeylGroupRepresentation<coefficient>::GetName() const
{ std::string name;
  for(int i=0; i<this->names.size; i++)
  { name.append(this->names[i]);
    if(i!=this->names.size-1)
      name.append(" aka ");
  }
  return name;
}

template <typename coefficient>
std::string WeylGroupRepresentation<coefficient>::ToString(FormatExpressions* theFormat)const
{ if (this->OwnerGroup==0)
    return "non-initialized representation";
  std::stringstream out;
  if (this->theCharacter.size>0)
    out << "Character: " << this->theCharacter.ToString(theFormat) << " of length " << this->OwnerGroup->GetCharacterNorm(this->theCharacter);
  else
    out << "Character needs to be computed.";
  int theRank=this->OwnerGroup->GetDim();
  LargeIntUnsigned theLCM, theDen;
  this->GetLargestDenominatorSimpleGens(theLCM, theDen);
  out << "\n<br>\n LCM denominators simple generators: " << theLCM.ToString() << ", largest denominator: " << theDen.ToString();
  out << "\n<br>\nThe simple generators (" << theRank << " total):<br> ";
  std::stringstream forYourCopyConvenience;
  for (int i=1; i<theRank+1; i++)
    if (this->theElementIsComputed[i])
    { std::stringstream tempStream;
      tempStream << "s_" << i << ":=MatrixRationals{}" << this->theElementImages[i].ToString(theFormat) << "; \\\\\n";
      forYourCopyConvenience << tempStream.str();
      out << CGI::GetMathSpanPure("\\begin{array}{l}"+ tempStream.str()+"\\end{array}", 3000);
    } else
      out << "Element s_{" << i << "} not computed ";
  out << "<br>For your copy convenience: <br>" << forYourCopyConvenience.str();
  return out.str();
}

template <typename coefficient>
coefficient WeylGroup::GetHermitianProduct(const Vector<coefficient>& leftCharacter, const Vector<coefficient>& rightCharacter)const
{ this->CheckInitializationFDrepComputation();
  coefficient result = 0;
  for(int i=0; i<this->conjugacyClasses.size; i++)
    result +=leftCharacter[i].GetComplexConjugate() * rightCharacter[i].GetComplexConjugate() * this->conjugacyClasses[i].size;
  result/=this->theElements.size;
  return result;
}

template <typename coefficient>
Vector<coefficient> WeylGroup::GetCharacterProduct(const Vector<coefficient>& leftCharacter, const Vector<coefficient>& rightCharacter) const
{ Vector<coefficient> result;
  result.SetSize(this->conjugacyClasses.size);
  for(int i=0; i<this->conjugacyClasses.size; i++)
    result[i] = leftCharacter[i]*rightCharacter[i];
  return result;
}

template <class coefficient>
std::string Vectors<coefficient>::ElementsToInequalitiesString(bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat)const
{ std::stringstream out;
  std::string tempS;
  std::string theLetter="x";
  if (useLatex)
    out << "\\begin{array}{l}";
  for (int i=0; i<this->size; i++)
  { Vector<Rational>& current=(*this)[i];
    tempS= current.ToStringLetterFormat(theFormat.polyDefaultLetter, &theFormat, LastVarIsConstant);
    if (tempS=="")
      out << "(0";
    out << tempS;
    if (!LastVarIsConstant)
    { if (useLatex)
        out << "\\geq 0\\\\";
      else
        out << "=>0\n";
    } else
    { if (useLatex)
        out << "\\geq " << (-(*current.LastObject())).ToString() << "\\\\";
      else
        out << "=>" <<  (-(*current.LastObject())).ToString();
    }
    if (tempS=="")
      out << ")";
    if (useHtml)
      out << "<br>";
    else
      out << "\n";
  }
  if (useLatex)
    out << "\\end{array}";
  return out.str();
}

template <class coefficient>
void SemisimpleLieAlgebra::GetAd(Matrix<coefficient>& output, ElementSemisimpleLieAlgebra<coefficient>& e)
{ int NumGenerators=this->GetNumGenerators();
  output.init(NumGenerators, NumGenerators);
  output.NullifyAll();
  ElementSemisimpleLieAlgebra<coefficient> theGen, theResult;
  for (int i=0; i<NumGenerators; i++)
  { theGen.MakeGenerator(i, *this);
    this->LieBracket(e, theGen, theResult);
    for (int j=0; j<theResult.size(); j++)
      output(theResult[j].theGeneratorIndex, i)=theResult.theCoeffs[j];
  }
}

template <class coefficient>
void SemisimpleLieAlgebra::GetCommonCentralizer
(const List<ElementSemisimpleLieAlgebra<coefficient> >& inputElementsToCentralize, List<ElementSemisimpleLieAlgebra<coefficient> >& outputCentralizingElements)
{ Matrix<coefficient> tempAd, commonAd;
  for (int i=0; i<inputElementsToCentralize.size; i++)
  { this->GetAd(tempAd, inputElementsToCentralize[i]);
    //tempAd.Transpose();
    commonAd.AppendMatrixToTheBottom(tempAd);
  }
  Vectors<coefficient> outputV;
  commonAd.GetZeroEigenSpace(outputV);
//  std::cout << "<br>Common ad: " << commonAd.ToString();
//  std::cout << "<br>Eigenvectors: " << outputV.ToString();
  outputCentralizingElements.SetSize(outputV.size);
  for (int i=0; i<outputV.size; i++)
  { ElementSemisimpleLieAlgebra<coefficient>& currentElt=outputCentralizingElements[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(outputV[i], *this);
  }
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::ElementToVectorNegativeRootSpacesFirst(Vector<coefficient>& output)const
{ if (this->IsEqualToZero())
  { output.MakeZero(0);
    return;
  }
  output.MakeZero(this->GetOwner()->GetNumGenerators());
  for (int i=0; i<this->size(); i++)
    output[(*this)[i].theGeneratorIndex]=this->theCoeffs[i];
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::MakeGGenerator(const Vector<Rational>& theRoot, SemisimpleLieAlgebra& inputOwner)
{ this->MakeGenerator(inputOwner.GetGeneratorFromRoot(theRoot), inputOwner);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::AssignVectorNegRootSpacesCartanPosRootSpaces(const Vector<coefficient>& input, SemisimpleLieAlgebra& owner)
{ //Changing RootSystem order invalidates this function!
  this->MakeZero();
  ChevalleyGenerator tempGenerator;
  for (int i=0; i<input.size; i++)
    if (input[i]!=0)
    { tempGenerator.MakeGenerator(owner, i);
      this->AddMonomial(tempGenerator, input[i]);
    }
}

#endif
