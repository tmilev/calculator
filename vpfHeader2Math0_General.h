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
  static double arccos(double argument);
  static double arctan(double argument);
  static double arcsin(double argument);
  static double sqrt(double argument);
  static double power(double base, double exponent);
  static double log(double argument);
  static double floor(double argument);
  static std::string DoubleToString(double input);
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
  static const bool IsMonEqualToZero()
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
  MonomialP(int letterIndex)
  { this->monBody.initFillInObject(letterIndex+1, 0);
    this->monBody[letterIndex]=1;
  }
  MonomialP(int letterIndex, int power)
  { this->monBody.initFillInObject(letterIndex+1, 0);
    this->monBody[letterIndex]=power;
  }
  MonomialP(const MonomialP& other)
  { *this=other;
  }
  MonomialP(){}
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
  static const inline bool IsMonEqualToZero()
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
  { return this->IsConstant() || this->IsLinearNoConstantTerm();
  }
  bool IsLinearNoConstantTerm(int* whichLetter=0)const
  { return this->IsOneLetterFirstDegree(whichLetter);
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
  template <class coefficient>
  bool SubstitutioN(const List<Polynomial<coefficient> >& TheSubstitution, Polynomial<coefficient>& output, const coefficient& theRingUnit=1)const;
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
  bool IsConstant()const
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
  std::string PathDisplayServerBaseFolder;
  std::string PathPhysicalCurrentOutputFolder;
  std::string PathDisplayCurrentOutputFolder;
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
  bool flagDynkinTypeDontUsePlusAndExponent;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsFinal;
  bool flagIncludeMutableInformation;
  bool flagExpressionNewLineAllowed;
  bool flagIncludeLieAlgebraNonTechnicalNames;
  bool flagIncludeLieAlgebraTypes;
  bool flagUseReflectionNotation;
  bool flagCandidateSubalgebraShortReportOnly;
  bool flagSupressDynkinIndexOne;
  bool flagFormatWeightAsVectorSpaceIndex;
  bool flagFormatMatrixAsLinearSystem;
  bool flagUseFrac;
  bool flagSuppresOneIn1overXtimesY;

  List<MonomialP>::OrderLeftGreaterThanRight thePolyMonOrder;
  template <typename templateMonomial>
  typename List<templateMonomial>::OrderLeftGreaterThanRight GetMonOrder();
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
  static const inline bool IsMonEqualToZero()
  { return false;
  }
  bool IsConstant()const
  { return this->polynomialPart.IsConstant() && this->differentialPart.IsConstant();
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

template <class coefficient>
std::iostream& operator <<(std::iostream& output, const Polynomial<coefficient>& input)
{ output << input.ToString();
  return output;
}

template <class templateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& output, const MonomialCollection<templateMonomial, coefficient>& theCollection);

template <class templateMonomial, class coefficient>
class MonomialCollection
{
private:
  void AddOnTop(const MonomialP& tempP);//<-to guard the now unsafe base class method
  void Clear();//<-to guard the now unsafe base class method
  friend std::ostream& operator<< <templateMonomial, coefficient>(std::ostream& output, const MonomialCollection<templateMonomial, coefficient>& theCollection);
public:
  HashedList<templateMonomial> theMonomials;
  List<coefficient> theCoeffs;
  MonomialCollection(){}
  MonomialCollection(const MonomialCollection& other)
  { this->operator=(other);
  }
  bool NeedsParenthesisForMultiplication()const
  { return this->size()>1;
  }
  inline static std::string GetXMLClassName()
  { std::string result="MonomialCollection_";
    result.append(templateMonomial::GetXMLClassName());
    return result;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  inline int size()const
  { return this->theMonomials.size;
  }
  //BIG FAT WARNING
  //the hash function of Monomial collection must return the same value for
  // monomial collections whose monomials are permuted!
  static unsigned int HashFunction(const MonomialCollection<templateMonomial, coefficient>& input)
  { unsigned int result=0;
    for (int i=0; i<input.size(); i++)
      result+= input.theCoeffs[i].HashFunction()*input[i].HashFunction();
    return result;
  }
  void QuickSortAscending(typename List<templateMonomial>::OrderLeftGreaterThanRight theOrder=0)
  { List<templateMonomial> theSortedMons=this->theMonomials;
    theSortedMons.QuickSortAscending(theOrder, &this->theCoeffs);
    this->theMonomials=theSortedMons;
  }
  void QuickSortDescending(typename List<templateMonomial>::OrderLeftGreaterThanRight theOrder=0)
  { List<templateMonomial> theSortedMons=this->theMonomials;
    theSortedMons.QuickSortDescending(theOrder, &this->theCoeffs);
    this->theMonomials=theSortedMons;
  }
  void PopMonomial(int index)
  { this->theMonomials.RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  void AddOtherTimesConst(MonomialCollection<templateMonomial, coefficient>& other, const coefficient& theConst);
  void PopMonomial(int index, templateMonomial& outputMon, coefficient& outputCoeff)
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
  void AddMonomial(const templateMonomial& inputMon, const coefficient& inputCoeff)
  { this->CleanupMonIndex(this->AddMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  void GetMinMonomial(templateMonomial& outputMon, coefficient& outputCF)const
  { if (this->IsEqualToZero())
      crash << "This is a programming error: calling GetMinMon on a zero monomial collection is forbidden. " << crash;
    outputMon=(*this)[0];
    outputCF=this->theCoeffs[0];
    for (int i=1; i<this->size(); i++)
      if (outputMon>(*this)[i])
      { outputMon=(*this)[i];
        outputCF=this->theCoeffs[i];
      }
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
  int AddMonomialNoCoeffCleanUpReturnsCoeffIndex(const templateMonomial& inputMon, const coefficient& inputCoeff);
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRows
  (List<MonomialCollectionTemplate>& theList, bool *IvemadeARowSwitch=0, HashedList<templateMonomial>* seedMonomials=0, Matrix<coefficient>* carbonCopyMatrix=0,
   List<MonomialCollectionTemplate>* carbonCopyList=0);
  template <class MonomialCollectionTemplate>
  static void IntersectVectorSpaces
  (const List<MonomialCollectionTemplate>& vectorSpace1, const List<MonomialCollectionTemplate>& vectorSpace2,
   List<MonomialCollectionTemplate>& outputIntersection, HashedList<templateMonomial>* seedMonomials=0);
  template <class MonomialCollectionTemplate>
  static int GetRankIntersectionVectorSpaces
  (List<MonomialCollectionTemplate>& vectorSpace1, List<MonomialCollectionTemplate>& vectorSpace2, HashedList<templateMonomial>* seedMonomials=0)
  { List<MonomialCollectionTemplate> listCopy=vectorSpace1;
    listCopy.AddListOnTop(vectorSpace2);
    return vectorSpace1.size+vectorSpace2.size-
    MonomialCollection<templateMonomial, coefficient>::GetRankOfSpanOfElements(listCopy, seedMonomials);
  }
  template <class MonomialCollectionTemplate>
  static bool VectorSpacesIntersectionIsNonTrivial
  (List<MonomialCollectionTemplate>& vectorSpace1, List<MonomialCollectionTemplate>& vectorSpace2, HashedList<templateMonomial>* seedMonomials=0)
  { return 0!=MonomialCollection<templateMonomial, coefficient>::GetRankIntersectionVectorSpaces(vectorSpace1, vectorSpace2, seedMonomials);
  }
  template <class MonomialCollectionTemplate>
  static bool LinSpanContains
  (const List<MonomialCollectionTemplate>& theList, const MonomialCollectionTemplate& input, HashedList<templateMonomial>* seedMonomials=0)
  { List<MonomialCollectionTemplate> listCopy=theList;
    MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    int startSpanSize=listCopy.size;
    listCopy.AddOnTop(input);
    MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size==startSpanSize;
  }
  template <class MonomialCollectionTemplate>
  static bool LinSpanContainsGetFirstLinearCombination
  (const List<MonomialCollectionTemplate>& theList, const MonomialCollectionTemplate& input, Vector<coefficient>& outputFirstLinearCombination,
   HashedList<templateMonomial>* seedMonomials=0);
  bool HasRationalCoeffs(MonomialCollection<templateMonomial, Rational>* outputConversionToRationals=0)
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
  static int GetRankOfSpanOfElements(List<MonomialCollectionTemplate>& theList, HashedList<templateMonomial>* seedMonomials=0)
  { List<MonomialCollectionTemplate> listCopy=theList;
    MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size;
  }
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRowsDeleteZeroRows(List<MonomialCollectionTemplate >& theList, bool *IvemadeARowSwitch=0, HashedList<templateMonomial>* seedMonomials=0)
  { MonomialCollectionTemplate::GaussianEliminationByRows(theList, IvemadeARowSwitch, seedMonomials);
    for (int j=theList.size-1; j>=0; j--)
      if (theList[j].IsEqualToZero())
        theList.size--;
  }
  int SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(const templateMonomial& inputMon, const coefficient& inputCoeff);
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
  void SubtractMonomial(const templateMonomial& inputMon, const coefficient& inputCoeff)
  { this->CleanupMonIndex(this->SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  coefficient GetMonomialCoefficient(const templateMonomial& inputMon)const
  { int theIndex=this->theMonomials.GetIndex(inputMon);
    if (theIndex==-1)
      return 0;
    return this->theCoeffs[theIndex];
  }
  bool operator>(const MonomialCollection<templateMonomial, coefficient>& other)const
  { if (this->size()>other.size())
      return true;
    if (this->size()<other.size())
      return false;
    MonomialCollection<templateMonomial, coefficient> leftCopy=*this;
    MonomialCollection<templateMonomial, coefficient> rightCopy=other;
    leftCopy.QuickSortAscending();
    rightCopy.QuickSortAscending();
    for (int i=leftCopy.size()-1; i>=0; i--)
    { if (leftCopy[i]>rightCopy[i])
        return true;
      if (rightCopy[i]>leftCopy[i])
        return false;
      if (leftCopy.theCoeffs[i]>rightCopy.theCoeffs[i])
        return true;
      if (rightCopy.theCoeffs[i]>leftCopy.theCoeffs[i])
        return false;
    }
    return false;
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

  static std::string GetBlendCoeffAndMon(const templateMonomial& inputMon, coefficient& inputCoeff, bool addPlusToFront, FormatExpressions* theFormat=0);
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
  bool HasGEQMonomial(templateMonomial& m, int& WhichIndex);
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
  inline bool operator!=(const MonomialCollection<templateMonomial, coefficient>& other)const
  { return !(*this==other);
  }
  bool operator==(const MonomialCollection<templateMonomial, coefficient>& other)const;
  bool operator==(int x)const;
  inline void operator+=(const MonomialCollection<templateMonomial, coefficient>& other);
  MonomialCollection<templateMonomial, coefficient> operator*(const coefficient& other)const
  { MonomialCollection<templateMonomial, coefficient> result=*this;
    result*=other;
    return result;
  }
//  inline void operator+=(const templateMonomial& m)
//  { this->AddMonomial(m, 1);
//  }
  MonomialCollection<templateMonomial, coefficient> operator-(const MonomialCollection<templateMonomial, coefficient>& other)const
  { MonomialCollection<templateMonomial, coefficient> output=*this;
    output-=other;
    return output;
  }
  MonomialCollection<templateMonomial, coefficient> operator+(const MonomialCollection<templateMonomial, coefficient>& other)const
  { MonomialCollection<templateMonomial, coefficient> output=*this;
    output+=other;
    return output;
  }
  void MakeConst(const coefficient& coeff)
  { templateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, coeff);
  }
  void MakeOne()
  { this->MakeConst(1);
  }
  inline void operator+=(const int& other)
  { templateMonomial tempM;
    coefficient theCF=other;
    tempM.MakeOne();
    this->AddMonomial(tempM, theCF);
  }

  inline void operator+=(const coefficient& other)
  { templateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, other);
  }
  inline void operator+=(const templateMonomial& other)
  { this->AddMonomial(other, 1);
  }
  inline void operator-=(const templateMonomial& other)
  { this->SubtractMonomial(other, 1);
  }
  inline void operator-=(const coefficient& other)
  { templateMonomial tempM;
    tempM.MakeOne();
    this->SubtractMonomial(tempM, other);
  }
  template <class OtherType>
  void AssignOtherType(const MonomialCollection<templateMonomial, coefficient>& other)
  { this->::HashedList<templateMonomial>::operator=(other);
    this->theCoeffs.SetSize(other.size);
    for (int i=0; i<other.size; i++)
      this->theCoeffs[i]=other.theCoeffs[i];
  }
  inline void operator-=(const MonomialCollection<templateMonomial, coefficient>& other)
  { this->SubtractOtherTimesCoeff(other);
  }
  inline void SubtractOtherTimesCoeff(const MonomialCollection<templateMonomial, coefficient>& other, coefficient* inputcf=0);
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
  inline const templateMonomial& operator[](int theIndex)const
  { return this->theMonomials[theIndex];
  }
  void operator=(const templateMonomial& other)
  { templateMonomial otherCopy=other; //in case other is contained as a monomial in me and gets destroyed when I call
    //this->MakeZero(). This shouldn't yield a measurable slowdown.
    //Further this function is to be used for non-time critical operations.
    this->MakeZero();
    this->AddMonomial(otherCopy, 1);
  }
  template<class otherType>
  void operator=(const MonomialCollection<templateMonomial, otherType>& other)
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
  inline static bool IsMonEqualToZero()
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
  int GetLargestParticipatingBasisIndex()
  { int result=-1;
    for (int i=0; i<this->size(); i++)
      result=MathRoutines::Maximum(result, (*this)[i].theIndex);
    return result;
  }
  bool operator>(const VectorSparse<coefficient>& other)const
  { return this->::MonomialCollection<MonomialVector, coefficient>::operator>(other);
  }
};

template <class templateMonomial, class coefficient>
class ElementMonomialAlgebra: public MonomialCollection<templateMonomial, coefficient>
{
  public:
  void MultiplyBy
  (const ElementMonomialAlgebra<templateMonomial, coefficient>& other, ElementMonomialAlgebra<templateMonomial, coefficient>& output,
   ElementMonomialAlgebra<templateMonomial, coefficient>& bufferPoly, templateMonomial& bufferMon)const;
  void MultiplyBy
  (const templateMonomial& other, const coefficient& theCoeff, ElementMonomialAlgebra<templateMonomial, coefficient>& output,
   ElementMonomialAlgebra<templateMonomial, coefficient>& bufferPoly, templateMonomial& bufferMon)const;
  void MultiplyOnTheLeft(const ElementMonomialAlgebra<templateMonomial, coefficient>& standsOnTheLeft)
  { ElementMonomialAlgebra<templateMonomial, coefficient> tempMat, bufferPoly;
    templateMonomial bufferMon;
    standsOnTheLeft.MultiplyBy(*this, tempMat, bufferPoly, bufferMon);
    *this=tempMat;
  }
  void operator*=(const ElementMonomialAlgebra<templateMonomial, coefficient>& other)
  { ElementMonomialAlgebra<templateMonomial, coefficient> bufferPoly;
    templateMonomial bufferMon;
    this->MultiplyBy(other, *this, bufferPoly, bufferMon);
  }
  inline void operator*=(const coefficient& other)
  { this->::MonomialCollection<templateMonomial, coefficient>::operator*= (other);
  }
  void RaiseToPower(int d, ElementMonomialAlgebra<templateMonomial, coefficient>& output, const coefficient& theRingUniT);
  void RaiseToPower(int d)
  { if (d==1)
      return;
    ElementMonomialAlgebra<templateMonomial, coefficient> theOne;
    theOne.MakeConst(1);
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
};

template<class coefficient>
struct PolynomialOrder
{
  public:
  List<MonomialP>::OrderLeftGreaterThanRight theMonOrder;
  bool CompareLeftGreaterThanRight(const Polynomial<coefficient>& left, const Polynomial<coefficient>& right) const;
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
  void AssignCharPoly(const Matrix<coefficient>& input);
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
  static bool IsMonEqualToZero()
  { return false;
  }
  static void GetValuesLagrangeInterpolandsAtConsecutivePoints
  (Vector<Rational>& inputConsecutivePointsOfInterpolation, Vector<Rational>& inputPointsOfEvaluation, Vectors<Rational>& outputValuesInterpolands);
  bool FactorMeOutputIsADivisor(Polynomial<Rational>& output, std::stringstream* comments);
  bool FactorMeNormalizedFactors
  (Rational& outputCoeff, List<Polynomial<Rational> >& outputFactors, std::stringstream* comments)const;
  bool FactorMe(List<Polynomial<Rational> >& outputFactors, std::stringstream* comments)const;
  void Interpolate(const Vector<coefficient>& thePoints, const Vector<coefficient>& ValuesAtThePoints);
  bool FindOneVarRatRoots(List<Rational>& output);
  coefficient GetDiscriminant();
  void GetCoeffInFrontOfLinearTermVariableIndex(int index, coefficient& output);
  void MakeMonomiaL(int LetterIndex, const Rational& Power, const coefficient& Coeff=1, int ExpectedNumVars=0)
  { if (LetterIndex<0)
    crash << "This is a programming error: the index" << LetterIndex+1 << " is  non-positive which is not allowed. " << crash;
    int numVars=MathRoutines::Maximum(LetterIndex+1, ExpectedNumVars);
    this->MakeZero();
    MonomialP tempM;
    tempM.MakeOne(numVars);
    tempM[LetterIndex]=Power;
    this->AddMonomial(tempM, Coeff);
  }
  void MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff);
  void MakeDegreeOne(int NVar, int NonZeroIndex1, int NonZeroIndex2, const coefficient& coeff1, const coefficient& coeff2);
  void MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff1, const coefficient& ConstantTerm);
  void MakeLinPolyFromRootNoConstantTerm(const Vector<Rational> & r);
  void MakeLinPolyFromRootLastCoordConst(const Vector<Rational> & input);
  void MakePolyFromDirectionAndNormal(Vector<coefficient>& direction, Vector<coefficient>& normal, coefficient& Correction);
  static bool GetLinearSystemFromLinearPolys
  (const List<Polynomial<coefficient> >& theLinPolys, Matrix<coefficient>& homogenousPart,
   Matrix<coefficient>& constTerms);
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
  void GetPolyWithPolyCoeff
  (Selection& theNonCoefficientVariables, Polynomial<Polynomial<coefficient> >& output)const
  ;
  void GetPolyUnivariateWithPolyCoeffs
  (int theVar, Polynomial<Polynomial<coefficient> >& output)const
  ;
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
    if (this->IsConstant(&tempC))
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
  bool IsConstant(coefficient* whichConstant=0)const
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
    return theMon.IsConstant();
  }
  bool IsNegative()const
  { coefficient tempC;
    if(!this->IsConstant(&tempC))
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
      if((*this)[i].IsConstant())
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
    if (!this->IsConstant(&constME))
    { crash << "This may or may not be a programming error: attempting to compare a non-constant polynomial to "
      << " a constant. I cannot judge at the moment whether allowing that is a good decision. In any case, crashing to let you know. "
      << crash;
      return false;
    }
    return constME<=other;
  }
  bool operator<(const coefficient& other)const
  { coefficient constME;
    if (!this->IsConstant(&constME))
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

template <class coefficient>
class PolynomialSubstitution: public List<Polynomial<coefficient> >
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
  void MakeIdSubstitution(int numVars, const coefficient& theRingUnit=1);
  void MakeIdLikeInjectionSub(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const coefficient& theRingUnit);
  //In the following function we have that:
  //the format of the linear substitution is:
  //theSub is a  whose number of rows minus 1 must equal the # number of
  //target variables and whose number of columns must equal the number of variables in
  //the current polynomial (this->NumVariables).
  //The first row denotes the constant term in the substitution of the respective variable!
  //An element in the x-th row and y-th column
  //is defined as element [x][y] !
  void MakeExponentSubstitution(Matrix<LargeInt>& theSub);
  void PrintPolys(std::string& output, coefficient& TheRingUnit, coefficient& TheRingZero);
  void MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<coefficient>& EndPoint);
  void AddConstant(coefficient& theConst);
  void TimesConstant(coefficient& r);
  void DivideByConstant(coefficient& r);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
  { int result=-1;
    for (int i=0; i<this->size; i++)
      result=MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    return result;
  }
  void MakeZero(int NumVars)
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
  void Substitution(PolynomialSubstitution<coefficient>& theSub, int NumVarsTarget)
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
  void MakeLinearSubConstTermsLastRow(Matrix<coefficient> & theMat);
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
  PolynomialOrder<coefficient> thePolynomialOrder;
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
  int MaxNumSerreSystemComputationsPreferred;
  int NumberGBComputations;
  int MaxNumGBComputations;
  int NumberSerreVariablesOneGenerator;
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
  FormatExpressions theFormat;
  void SetSerreLikeSolutionIndex(int theIndex, const coefficient& theConst);
  void GetSubFromPartialSolutionSerreLikeSystem(PolynomialSubstitution<coefficient>& outputSub);
  std::string ToStringSerreLikeSolution();
  std::string GetPolynomialStringSpacedMonomialsHtml
  (const Polynomial<coefficient>& thePoly, const HashedList<MonomialP>& theMonomialOrder, const std::string& extraStyle,
   const std::string& extraHighlightStyle, List<MonomialP>* theHighLightedMons=0);
  std::string GetPolynomialStringSpacedMonomialsLaTeX
  (const Polynomial<coefficient>& thePoly, const HashedList<MonomialP>& theMonomialOrder,
   bool underline=false, std::string* highlightColor=0, List<MonomialP>* theHighLightedMons=0);
  std::string GetDivisionStringHtml();
  std::string GetDivisionStringLaTeX();
  bool AddPolyAndReduceBasis(GlobalVariables* theGlobalVariables);
  bool TransformToReducedGroebnerBasis(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables=0);
  bool TransformToReducedGroebnerBasisImprovedAlgorithm(List<Polynomial<coefficient> >& inputOutpuT, GlobalVariables* theGlobalVariables=0, int upperComputationBound=-1);
  GroebnerBasisComputation();
  void MakeMinimalBasis();
  int GetNumVars()const;
  std::string ToStringLetterOrder(bool addDollars)const;
  static int GetNumEquationsThatWouldBeLinearIfIsubbedVar(int theVarIndex, List<Polynomial<coefficient> >& input);
  static int GetNumVarsToSolveFor(const List<Polynomial<coefficient> >& input);
  static void GetVarsToSolveFor(const List<Polynomial<coefficient> >& input, Selection& output);
  static bool IsContradictoryReducedSystem(const List<Polynomial<coefficient> >& input);
  void RemainderDivisionWithRespectToBasis
  (Polynomial<coefficient>& inputOutput, Polynomial<coefficient>* outputRemainder=0, GlobalVariables* theGlobalVariables=0, int basisIndexToIgnore=-1);
  std::string ToStringCalculatorInputFromSystem(const List<Polynomial<coefficient> >& inputSystem);
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
    { if (this->Denominator.GetElement().IsConstant())
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
      if(this->Numerator.GetElement().IsConstant())
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
  bool NeedsParenthesisForMultiplication()const
  { switch(this->expressionType)
    { case RationalFunctionOld::typeRational: return false;
      case RationalFunctionOld::typePoly: return this->Numerator.GetElementConst().NeedsParenthesisForMultiplication();
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
  void MakeOneLetterMoN(int theIndex, const Rational& theCoeff, GlobalVariables* theGlobalVariables=0, int ExpectedNumVars=0);
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
  void MakeMonomiaL(int LetterIndex, const Rational& Power, const Rational& Coeff=1, int ExpectedNumVars=0);
  void MakeConst(const Rational& theCoeff, GlobalVariables* theContext)
  { this->MakeZero(theContext);
    this->ratValue=theCoeff;
  }
  bool IsConstant(Rational* whichConstant=0)const
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

template <class templateMonomial, class coefficient>
inline bool MonomialCollection<templateMonomial, coefficient>::operator==(int x)const
{ if (x==0)
    return this->size()==0;
  crash << "This is either a programming error, or an unforeseen use of operator==. If the second is the case, an audit/careful "
  << "proofreading of the code calling this function is needed; I am crashing just in case. " << crash;
  return false;
}

template <class templateMonomial, class coefficient>
inline bool MonomialCollection<templateMonomial, coefficient>::operator==(const MonomialCollection<templateMonomial, coefficient>& other)const
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

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::WriteToFile(std::fstream& output)
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << " numMons: " << this->size();
  for (int i=0; i<this->size(); i++)
  { output << " ";
    if (i>0)
      if (! this->theCoeffs[i].BeginsWithMinus())
        output << "+ ";
    this->theCoeffs[i].WriteToFile(output);
    const templateMonomial& tempM=(*this)[i];
    tempM.WriteToFile(output);
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class templateMonomial, class coefficient>
inline bool MonomialCollection<templateMonomial, coefficient>::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
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
  templateMonomial tempM;
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

template <class templateMonomial, class coefficient>
inline void ElementMonomialAlgebra<templateMonomial, coefficient>::MultiplyBy
(const ElementMonomialAlgebra<templateMonomial, coefficient>& other, ElementMonomialAlgebra<templateMonomial, coefficient>& output,
 ElementMonomialAlgebra<templateMonomial, coefficient>& bufferPoly, templateMonomial& bufferMon)const
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

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::IsEqualToZero()const
{ return this->size()==0;
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::IsInteger(LargeInt* whichInteger)const
{ if (this->size()>1)
    return false;
  if(this->size()==0)
  { if (whichInteger!=0)
      *whichInteger=0;
    return true;
  }
  bool result= (*this)[0].IsConstant();
  if (result)
    result=this->theCoeffs[0].IsInteger(whichInteger);
  return result;
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::IsSmallInteger(int* whichInteger)const
{ if (this->size()>1)
    return false;
  if(this->size()==0)
  { if (whichInteger!=0)
      *whichInteger=0;
    return true;
  }
  bool result= (*this)[0].IsConstant();
  if (result)
    result=this->theCoeffs[0].IsSmallInteger(whichInteger);
  return result;
}

template <class coefficient>
void Polynomial<coefficient>::MakeLinPolyFromRootLastCoordConst(const Vector<Rational>& input)
{ this->MakeZero();
  MonomialP tempM;
  for (int i=0; i<input.size-1; i++)
  { tempM.MakeEi(i);
    this->AddMonomial(tempM, input[i]);
  }
  this->operator+=(*input.LastObject());
}

template <class coefficient>
void Polynomial<coefficient>::MakeLinPolyFromRootNoConstantTerm(const Vector<Rational>& r)
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

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::SubtractOtherTimesCoeff(const MonomialCollection<templateMonomial, coefficient>& other, coefficient* inputcf)
{ if (this==&other)
  { if (inputcf==0)
    { this->MakeZero();
      return;
    }
    MonomialCollection<templateMonomial, coefficient> otherNew=other;
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

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::AddOtherTimesConst(MonomialCollection<templateMonomial, coefficient>& other, const coefficient& theConst)
{ this->SetExpectedSize(other.size()+this->size());
  coefficient tempCF;
  for (int i=0; i<other.size(); i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    tempCF=theConst;
    tempCF*=other.theCoeffs[i];
    this->AddMonomial(other[i], tempCF);
  }
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::operator+=(const MonomialCollection<templateMonomial, coefficient>& other)
{ this->SetExpectedSize(other.size()+this->size());
  for (int i=0; i<other.size(); i++)
  { ParallelComputing::SafePointDontCallMeFromDestructors();
    this->AddMonomial(other[i], other.theCoeffs[i]);
  }
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::HasGEQMonomial(templateMonomial& m, int& WhichIndex)
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IsGEQpartialOrder(m))
    { WhichIndex=i;
      return true;
    }
  WhichIndex=-1;
  return false;
}

template <class templateMonomial, class coefficient>
template <class MonomialCollectionTemplate>
bool MonomialCollection<templateMonomial, coefficient>::LinSpanContainsGetFirstLinearCombination
(const List<MonomialCollectionTemplate>& theList, const MonomialCollectionTemplate& input, Vector<coefficient>& outputFirstLinearCombination,
 HashedList<templateMonomial>* seedMonomials)
{ List<MonomialCollectionTemplate> listCopy=theList;
  Matrix<coefficient> theRowOperations;
  theRowOperations.MakeIdMatrix(theList.size);
  MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRows(listCopy, 0, seedMonomials, &theRowOperations);
  MonomialCollectionTemplate remainderFromInput=input;
  templateMonomial currentMon;
  coefficient CFminMon, CFinRemainder;
  outputFirstLinearCombination.MakeZero(listCopy.size);
  for (int i=0; i<listCopy.size; i++)
  { if (listCopy[i].IsEqualToZero())
      break;
    listCopy[i].GetMinMonomial(currentMon, CFminMon);
    CFinRemainder=remainderFromInput.GetMonomialCoefficient(currentMon);
    outputFirstLinearCombination[i]=CFinRemainder;
    outputFirstLinearCombination[i]/=CFminMon;
    remainderFromInput-=listCopy[i]*outputFirstLinearCombination[i];
  }
  if (!remainderFromInput.IsEqualToZero())
    return false;
  theRowOperations.ActMultiplyVectorRowOnTheRight(outputFirstLinearCombination);
  return true;
}

template <class templateMonomial, class coefficient>
template <class MonomialCollectionTemplate>
void MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRows
(List<MonomialCollectionTemplate>& theList, bool *IvemadeARowSwitch, HashedList<templateMonomial>* seedMonomials, Matrix<coefficient>* carbonCopyMatrix, List<MonomialCollectionTemplate>* carbonCopyList)
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
  MemorySaving<HashedList<templateMonomial> > bufferMons;
  HashedList<templateMonomial>& allMons = seedMonomials==0 ? bufferMons.GetElement() : *seedMonomials;
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
//  stOutput << "<hr>Gaussian elimnation. All mons(" << allMons.size << " total): "
//  << allMons.ToString(&tempFormat);
  tempFormat.flagUseLatex=true;
  if (IvemadeARowSwitch!=0)
    *IvemadeARowSwitch=false;
//  stOutput << "<br><b>starting list:</b> ";
//  for (int i=0; i<theList.size; i++)
//    stOutput << //"<br>" << CGI::GetMathSpanPure
//    (theList[i].ToString(&tempFormat)) << ", ";
  int currentRowIndex=0;
  coefficient tempCF, tempCF2;
  for (int i=0; i<allMons.size && currentRowIndex<theList.size; i++)
  { const templateMonomial& currentMon=allMons[i];
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
    MonomialCollection<templateMonomial, coefficient>& currentPivot= theList[currentRowIndex];
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
      { MonomialCollection<templateMonomial, coefficient>& currentOther=theList[j];
        int otherColIndex=currentOther.theMonomials.GetIndex(currentMon);
        if (otherColIndex!=-1)
        { tempCF=currentOther.theCoeffs[otherColIndex];
          //stOutput << "<br>subtracting " << CGI::GetMathSpanPure(currentPivot.ToString())
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
          //stOutput << "<br>to get " << CGI::GetMathSpanPure(currentOther.ToString());
        }
      }
    currentRowIndex++;
  }
//    stOutput << "<br><b>final list:</b> ";
//  for (int i=0; i<theList.size; i++)
//    stOutput << //"<br>" << CGI::GetMathSpanPure
//    (theList[i].ToString(&tempFormat)) << ", ";
}

template <class templateMonomial, class coefficient>
int MonomialCollection<templateMonomial, coefficient>::AddMonomialNoCoeffCleanUpReturnsCoeffIndex(const templateMonomial& inputMon, const coefficient& inputCoeff)
{ ///
//  this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
  ///
  if (inputCoeff==0 || inputMon.IsMonEqualToZero())
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

template <class templateMonomial, class coefficient>
int MonomialCollection<templateMonomial, coefficient>::SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(const templateMonomial& inputMon, const coefficient& inputCoeff)
{ if (inputCoeff.IsEqualToZero() || inputMon.IsMonEqualToZero())
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

template <class templateMonomial, class coefficient>
void ElementMonomialAlgebra<templateMonomial, coefficient>::RaiseToPower(int d, ElementMonomialAlgebra<templateMonomial, coefficient>& output, const coefficient& theRingUniT)
{ ElementMonomialAlgebra<templateMonomial, coefficient> tempOne;
  tempOne.MakeConst(theRingUniT);
  output=*this;
  MathRoutines::RaiseToPower(output, d, tempOne);
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::PrintPolys(std::string &output, coefficient& TheRingUnit, coefficient& TheRingZero)
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

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeIdSubstitution(int numVars, const coefficient& theRingUnit)
{ this->MakeIdLikeInjectionSub(numVars, numVars, theRingUnit);
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeIdLikeInjectionSub(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const coefficient& theRingUnit)
{ if(numStartingVars>numTargetVarsMustBeLargerOrEqual)
    crash << crash;
  this->SetSize(numStartingVars);
  for (int i=0; i<this->size; i++)
  { Polynomial<coefficient>& currentPoly=this->TheObjects[i];
    currentPoly.MakeDegreeOne(numTargetVarsMustBeLargerOrEqual, i, theRingUnit);
  }
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeExponentSubstitution(Matrix<LargeInt>& theSub)
{ Polynomial<coefficient> tempP;
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

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<coefficient>& EndPoint)
{ this->SetSize(numVars);
  for (int i=0; i<numVars-1; i++)
    this->TheObjects[i].MakeDegreeOne(numVars, i, coefficient::TheRingUnit);
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
    { this->v1[i]=input1.TheObjects[i].GetDoubleValue();
      this->v2[i]=input2.TheObjects[i].GetDoubleValue();
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
      this->theVector[i]=input.TheObjects[i].GetDoubleValue();
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
      this->theVector[i]=input.TheObjects[i].GetDoubleValue();
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
        tempMat.elements[i][j]=bilinearForm.elements[i][j].GetDoubleValue();
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
  //MutexWrapper LockedWhileDrawing;
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

template <class coefficient>
void MathRoutines::RaiseToPower(coefficient& theElement, int thePower, const coefficient& theRingUnit)
{ if (thePower<0)
    return;
  if (thePower==1)
    return;
  if (thePower==0)
  { theElement=theRingUnit;
    return;
  }
  coefficient squares;
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

template<typename coefficient>
void Matrix<coefficient>::NonPivotPointsToEigenVectorMatrixForm(Selection& TheNonPivotPoints, Matrix<coefficient>& output)
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

template<typename coefficient>
void Matrix<coefficient>::NonPivotPointsToEigenVector(Selection& TheNonPivotPoints, Vector<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero)
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
  double (*getElapsedTimePrivate)();
  void (*callSystem)(const std::string& theSystemCommand);
public:
  void (*IndicatorStringOutputFunction)(const std::string& input);
  void (*sleepFunction)(int microseconds);
  void (*WebServerReturnDisplayIndicatorCloseConnection)();
  void (*PauseUponUserRequest)();
  void Pause()
  { if (this->PauseUponUserRequest!=0)
      this->PauseUponUserRequest();
  }
  void FallAsleep(int microseconds)
  { if (this->sleepFunction!=0)
      this->sleepFunction(microseconds);
  }
//  double MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit;
  double MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  double MaxComputationTimeBeforeWeTakeAction;
//  bool flagLogInterProcessCommunication;
  bool flagUsingBuiltInWebServer;
  bool flagAllowUseOfThreadsAndMutexes;

  FormatExpressions theDefaultFormat;
//status flags:
  bool flagComputationCompletE;
  bool flagComputationStarted;
  bool flagDisplayTimeOutExplanation;
  bool flagTimedOutComputationIsDone;
  bool flagOutputTimedOut;
//progress report flags:
  bool flagReportEverything;
  bool flagReportLargeIntArithmetic;
  bool flagReportGaussianElimination;

  List<std::string> ProgressReportStringS;
  DrawingVariables theDrawingVariables;
  FormatExpressions thePolyFormat;
  Controller theLocalPauseController;

  std::string PhysicalPathServerBase;
  std::string PhysicalPathOutputFolder;
  std::string PhysicalNameIndicatorWithPath;
  std::string PhysicalNameDefaultOutputNoPatH;
  std::string PhysicalNameDefaultOutputWithPath;
  std::string PhysicalNameFolderBelowExecutable;
  std::string PhysicalNameExecutableNoPath;
  std::string PhysicalNameExecutableWithPath;

  std::string DisplayPathServerBase;
  std::string DisplayPathExecutable;
  std::string DisplayPathOutputFolder;
  std::string DisplayNameDefaultOutputNoPath;
  std::string DisplayNameDefaultOutputWithPath;
  std::string DisplayNameExecutableWithPath;
  std::string DisplayNameIndicatorWithPath;
  std::string DisplayNameCalculatorWithPath;


  std::string IPAdressCaller;
  std::string defaultUserLabel;
  //buffers:
  MemorySaving<Selection> selWallSelection;
  MemorySaving<Selection> selComputeNormalExcludingIndex;
  MemorySaving<Selection> selApproveSelAgainstOneGenerator;
  MemorySaving<Selection> selSimplexAlg2;
  MemorySaving<Selection> selGetNewVerticesAppend;
  MemorySaving<Selection> selGetNewVerticesAppend2;
  MemorySaving<Selection> selSplitChamber;

  MutexWrapper MutexRegisterFunctionStaticFiasco;
  MutexWrapper MutexParallelComputingStaticFiasco;
  MutexWrapper MutexWebWorkerStaticFiasco;

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

  MemorySaving<DynkinDiagramRootSubalgebra > dynGetEpsCoords;

  MemorySaving<SubgroupWeylGroupOLD> subGroupActionNormalizerCentralizer;

  MemorySaving<HashedList<Vector<Rational> > > hashedRootsComputeSubGroupFromGeneratingReflections;

  MemorySaving<GroebnerBasisComputation<Rational> > theGroebnerBasisComputation;

  GlobalVariables();
  void SetTimerFunction(double (*timerFunction)())
  { this->getElapsedTimePrivate=timerFunction;
  }
  double GetElapsedSeconds()
  { if (this->getElapsedTimePrivate!=0)
      return this->getElapsedTimePrivate();
    return -1;
  }
  void initDefaultFolderAndFileNames
(const std::string& inputPhysicalExecutableWithPathServerBaseIsFolderBelow,
 const std::string& scrambledIP);

  void operator=(const GlobalVariables& other)
  { this->IndicatorStringOutputFunction=other.IndicatorStringOutputFunction;
    this->theDrawingVariables=other.theDrawingVariables;
  }
  inline void DrawBufferNoIniT()
  { this->theDrawingVariables.drawBufferNoIniT();
  }
  void SetCallSystem(void (*theSystemCall)(const std::string&))
  { this->callSystem=theSystemCall;
  }
  void System(const std::string& systemCommand)
  { if (this->callSystem!=0)
      this->callSystem(systemCommand);
  }
  std::string ToStringFolderInfo()const;
  inline void MakeReport(const std::string& input)
  { if (this->IndicatorStringOutputFunction!=0)
      this->IndicatorStringOutputFunction(input);
  }
  void MakeReport();
  /// @endcond
};

template <class coefficient>
void Matrix<coefficient>::GaussianEliminationEuclideanDomain
(Matrix<coefficient>* otherMatrix, const coefficient& theRingMinusUnit, const coefficient& theRingUnit, bool (*comparisonGEQFunction) (const coefficient& left, const coefficient& right),
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("Matrix_Element::GaussianEliminationEuclideanDomain");
  ProgressReport theReport(theGlobalVariables);
  if (otherMatrix==this)
    crash << "This is a programming error: the Carbon copy in the Gaussian elimination coincides with the matrix which we are row-reducing "
    << "(most probably this is a wrong pointer typo). " << crash;
  int col=0;
  coefficient tempElt;
  int row=0;
  while(row<this->NumRows && col<this->NumCols)
  { //stOutput << "<br>****************row: " << row << " status: " << this->ToString(true, false);
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
//      stOutput << "<br>before second while: " << this->ToString(true, false);
      while (ExploringRow<this->NumRows)
      { if (theGlobalVariables!=0)
        { std::stringstream out;
          out << "Pivotting on row of index " << row +1 << " with exploring row of index " << ExploringRow+1 << "; total rows: " << this->NumRows;
          theReport.Report(out.str());
        }
        coefficient& PivotElt=this->elements[row][col];
        coefficient& otherElt=this->elements[ExploringRow][col];
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
//        stOutput << "<br>second while cycle end: " << this->ToString(true, false);
      }
      coefficient& PivotElt = this->elements[row][col];
      for (int i=0; i<row; i++)
      { tempElt =this->elements[i][col];
        tempElt/=PivotElt;
//        stOutput << " the floor of " << tempElt.ToString();
        tempElt.AssignFloor();
//        stOutput << " is " << tempElt.ToString();
        this->SubtractRowsWithCarbonCopy(i, row, 0, tempElt, otherMatrix);
        if (this->elements[i][col].IsNegative())
          this->AddTwoRowsWithCarbonCopy(row, i, 0, theRingUnit, otherMatrix);
      }
      row++;
    }
    col++;
//    stOutput << "end of cycle status: " << this->ToString(true, false) << "<br>****************";
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

template<class coefficient>
class CompleX
{
  static bool flagEqualityIsApproximate;
  static double EqualityPrecision;
  public:
  coefficient Im;
  coefficient Re;
  std::string ToString(FormatExpressions* theFormat=0)
  { std::stringstream tempStream;
    tempStream << *this;
    return tempStream.str();
  }
  friend std::iostream& operator<<(std::iostream& output, const CompleX<coefficient>& input)
  { output << input.Re << "+i" << input.Im;
    return output;
  }
  void operator*=(const CompleX<coefficient>& other)
  { CompleX Accum;
    Accum.Re=this->Re*other.Re-this->Im*other.Im;
    Accum.Im=this->Re*other.Im+ this->Im*other.Re;
    this->operator=(Accum);
  }
  void operator=(const CompleX<coefficient>& other)
  { this->Re=other.Re;
    this->Im=other.Im;
  }
  void operator+=(const CompleX<coefficient>& other)
  { this->Re+=other.Re;
    this->Im+=other.Im;
  }
  void operator-=(const CompleX<coefficient>& other)
  { this->Re-=other.Re;
    this->Im-=other.Im;
  }
  void operator=(int other)
  { this->Re=other;
    this->Im=0;
  }
  bool operator==(int other)const
  { CompleX<coefficient> otherComplex;
    otherComplex=other;
    return *this==otherComplex;
  }
  bool operator==(CompleX<coefficient>& other)const
  { CompleX<coefficient> difference;
    difference=*this;
    difference-=other;
    return difference.IsEqualToZero();
  }
  void operator=(double other)
  { this->Re=other;
    this->Im=0;
  }
  void Invert()
  { coefficient numerator;
    numerator=this->Re*this->Re+this->Im* this->Im;
    this->Re/=numerator;
    numerator*=-1;
    this->Im/=numerator;
  }
  bool IsEqualToZero()const
  { if(!CompleX<coefficient>::flagEqualityIsApproximate)
      return this->Im==0 && this->Re==0;
    else
      return
      this->Im<CompleX<coefficient>::EqualityPrecision && -this->Im<CompleX<coefficient>::EqualityPrecision &&
      this->Re<CompleX<coefficient>::EqualityPrecision && -this->Re<CompleX<coefficient>::EqualityPrecision;
  }
  inline void Minus()
  { this->Im=-this->Im;
    this->Re=-this->Re;
  }
  bool NeedsParenthesisForMultiplication()
  { if (this->Re==0 && this->Im>=0)
      return false;
    if (this->Im==0 && this->Re>=0)
      return false;
    return true;
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
  bool NeedsParenthesisForMultiplication()const;
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
  Vector<Rational> GetRootIMustBeWeight()const;
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

template <class coefficient>
int Matrix<coefficient>::FindPositiveLCMCoefficientDenominatorsTruncated()
{ int result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      result*=(this->elements[i][j].DenShort/Rational::gcdSigned(result, this->elements[i][j].DenShort));
  return result;
}

template <class coefficient>
LargeIntUnsigned Matrix<coefficient>::FindPositiveLCMCoefficientDenominators()
{ LargeIntUnsigned result=1;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      result=LargeIntUnsigned::lcm(result, (*this)(i,j).GetDenominator());
  return result;
}

template <class coefficient>
void Matrix<coefficient>::GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen)
{ outputDen=this->FindPositiveLCMCoefficientDenominators();
  outputMat.init(this->NumRows, this->NumCols);
  Rational tempRat;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempRat=this->elements[i][j]*outputDen;
      outputMat(i,j)=tempRat.GetDenominator();
    }
}

template <class coefficient>
int Matrix<coefficient>::FindPositiveGCDCoefficientNumeratorsTruncated()
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

template <class coefficient>
void Matrix<coefficient>::ScaleToIntegralForMinRationalHeightNoSignChange()
{ Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(this->FindPositiveLCMCoefficientDenominatorsTruncated(), this->FindPositiveGCDCoefficientNumeratorsTruncated());
  *this*=(tempRat);
}

template <class coefficient>
coefficient Matrix<coefficient> ::GetDeterminant()
{ Matrix<coefficient> tempMat=*this;
  coefficient result;
  tempMat.ComputeDeterminantOverwriteMatrix(result);
  return result;
}

template <class coefficient>
coefficient Matrix<coefficient>::GetTrace()const
{ if (this->NumCols!=this->NumRows)
    crash << "This is either programming error, a mathematical error, or requires a more general definition of trace. Requesting the trace of "
    << " a non-square matrix of " << this->NumRows << " rows and " << this->NumCols << " columns is not allowed. "
    << crash;
  coefficient acc = 0;
  for(int i=0; i<this->NumCols; i++)
    acc += this->elements[i][i];
  return acc;
}

template <class coefficient>
Matrix<coefficient> Matrix<coefficient>::operator*(const Matrix<coefficient>& right)const
{ Matrix<coefficient> tempMat;
  tempMat=right;
  tempMat.MultiplyOnTheLeft(*this);
  return tempMat;
}

template <class coefficient>
Vector<coefficient> Matrix<coefficient>::operator*(const Vector<coefficient>& v) const
{ if(v.size != NumCols)
    crash << "matrix application mismatch: matrix with" << NumCols << "columns attempted to multiply vector of length" << v.size << crash;
  Vector<coefficient> out;
  out.MakeZero(NumRows);
  for(int i=0;i<NumRows;i++)
    for(int j=0;j<NumCols;j++)
      out[i] += elements[i][j] * v[j];
  return out;
}

template <class coefficient>
void Matrix<coefficient>::AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den)
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
  matA.MakeZero();
  matb.init((int)numRows, 1);
  matb.MakeZero();
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

template<class coefficient>
void Matrix<coefficient>::GetMaxMovementAndLeavingVariableRow
(Rational& maxMovement, int& LeavingVariableRow, int EnteringVariable, int NumTrueVariables, Matrix<coefficient>& tempMatA,
 Vector<coefficient>& inputVectorX, Selection& BaseVariables)
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

template <typename coefficient>
inline void Matrix<coefficient>::ActOnMonomialAsDifferentialOperator(const MonomialP& input, Polynomial<Rational>& output)
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

template <typename coefficient>
void Matrix<coefficient>::GetZeroEigenSpaceModifyMe(List<Vector<coefficient> >& output)
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
void List<Object>::SubSelection(const Selection& theSelection, List<Object>& output)
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
      if (this->TheObjects[i].NeedsParenthesisForMultiplication())
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

template <typename coefficient>
std::string Matrix<coefficient>::ToStringLatex(FormatExpressions* theFormat)const
{ FormatExpressions formatCopy;
  if (theFormat!=0)
    formatCopy=*theFormat;
  formatCopy.flagUseLatex=true;
  formatCopy.flagUseHTML=false;
  return this->ToString(&formatCopy);
}

template <typename coefficient>
std::string Matrix<coefficient>::ToStringSystemLatex
(Matrix<coefficient>* constTerms, FormatExpressions* theFormat)const
{ std::stringstream out;
  bool constTermsAreGood=
  (constTerms==0) ? false : (constTerms->NumRows==this->NumRows && constTerms->NumCols>0);
  out << "\\begin{array}{l";
  for (int j=0; j<this->NumCols; j++)
    out << "l";
  out << "}";
  std::string currentEntry;
  Polynomial<coefficient> theMon;
  for (int i=0; i<this->NumRows; i++)
  { bool foundNonZeroEntry=false;
    for (int j=0; j<this->NumCols; j++)
    { if (!((*this)(i,j)==0))
      { theMon.MakeMonomiaL(j, 1, (*this)(i,j));
        currentEntry=theMon.ToString(theFormat);
        if (currentEntry=="")
          crash << "Empty strings not allowed as result of ToString() function call. " << crash;
        if (currentEntry[0]!='-' && foundNonZeroEntry)
          out << "+";
        out << currentEntry;
        foundNonZeroEntry=true;
      }
      if (j==this->NumCols-1 && !foundNonZeroEntry)
        out << "0";
      out << " & ";
    }
    out << "=";
    if (!constTermsAreGood)
      out << "0";
    else
      out << (*constTerms)(i,0).ToString(theFormat);
    out << "\\\\";
  }
  out << "\\end{array}";
  return out.str();
}

template <typename coefficient>
std::string Matrix<coefficient>::ToString(FormatExpressions* theFormat)const
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

template <class templateMonomial, class coefficient>
std::string MonomialCollection<templateMonomial, coefficient>::GetBlendCoeffAndMon
(const templateMonomial& inputMon, coefficient& inputCoeff, bool addPlusToFront, FormatExpressions* theFormat)
{ std::stringstream out;
  std::string coeffStr=inputCoeff.ToString(theFormat);
  if (inputMon.IsConstant())
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

template <class templateMonomial, class coefficient>
std::string MonomialCollection<templateMonomial, coefficient>::ToString(FormatExpressions* theFormat)const
{ if (this->size()==0)
    return "0";
  MacroRegisterFunctionWithName("MonomialCollection::ToString");
  std::stringstream out;
  std::string tempS1, tempS2;
  List<templateMonomial> sortedMons;
  sortedMons=this->theMonomials;
  //If this line fails to link, you must do the following.
  //You need to implement FormatExpressions::GetMonOrder<templateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  //wish to use a custom one.
  typename List<templateMonomial>::OrderLeftGreaterThanRight
  theOrder= theFormat==0? 0: theFormat->GetMonOrder<templateMonomial>();
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
  { templateMonomial& currentMon=sortedMons[i];
    coefficient& currentCoeff=this->theCoeffs[this->theMonomials.GetIndex(currentMon)];
    if (currentCoeff.NeedsParenthesisForMultiplication())
      tempS1="("+currentCoeff.ToString(theFormat)+ ")";
    else
      tempS1=currentCoeff.ToString(theFormat);
    tempS2=currentMon.ToString(theFormat);
    if (tempS2!="")
    { bool useFracSpecial=false;
      if (theFormat!=0)
        if (theFormat->flagUseFrac && theFormat->flagSuppresOneIn1overXtimesY)
          useFracSpecial=true;
      if (useFracSpecial)
      { std::string tempS3;
        if (MathRoutines::StringBeginsWith(tempS1, "\\frac{1}", &tempS3))
          tempS1="\\frac{"+ tempS2 + "}" +tempS3;
        else if (MathRoutines::StringBeginsWith(tempS1, "-\\frac{1}", &tempS3))
          tempS1="-\\frac{"+ tempS2 + "}" +tempS3;
        else
          useFracSpecial=false;
      }
      if (!useFracSpecial)
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

template <typename coefficient>
inline void Matrix<coefficient>::AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const coefficient& scalar, GlobalVariables* theGlobalVariables)
{ ProgressReport theReport(theGlobalVariables);
  bool doProgressReport=false;
  if (theGlobalVariables!=0)
    doProgressReport=theGlobalVariables->flagReportGaussianElimination || theGlobalVariables->flagReportEverything;
  coefficient tempElement;
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

template <typename coefficient>
void Matrix<coefficient>::GaussianEliminationByRows
(Matrix<coefficient>* carbonCopyMat, Selection* outputNonPivotColumns,
 Selection* outputPivotColumns, GlobalVariables* theGlobalVariables,
 std::stringstream* humanReadableReport, FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("Matrix::GaussianEliminationByRows");
  //Checking for bees
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
  coefficient tempElement;
  if (outputNonPivotColumns!=0)
    outputNonPivotColumns->init(this->NumCols);
  if (outputPivotColumns!=0)
    outputPivotColumns->init(this->NumCols);
  bool doProgressReport= theGlobalVariables==0 ? false :
  theGlobalVariables->flagReportGaussianElimination || theGlobalVariables->flagReportEverything;
  bool formatAsLinearSystem= theFormat==0? false : theFormat->flagFormatMatrixAsLinearSystem;
  bool useHtmlInReport=theFormat==0? true : theFormat->flagUseHTML;
  ProgressReport theReport(theGlobalVariables);
  if (humanReadableReport!=0)
  { if(useHtmlInReport)
      *humanReadableReport << "\n\n\n\n<table><tr><td style=\"border-bottom:3pt solid black;\">System status</td>"
      << "<td style=\"border-bottom:3pt solid black;\">action</td></tr>";
    else
      *humanReadableReport << "\n\n\\begin{longtable}{cc} System status&Action \\\\\\hline\n";
  }
  //Initialization done! Time to do actual work:
  for (int i=0; i<this->NumCols; i++)
  { if (NumFoundPivots == MaxRankMat)
    { if (outputNonPivotColumns!=0)
        for (int j =i; j<this->NumCols; j++)
          outputNonPivotColumns->AddSelectionAppendNewIndex(j);
      break;
    }
    tempI = this->FindPivot(i, NumFoundPivots);
    if (tempI==-1)
    { if (outputNonPivotColumns!=0)
        outputNonPivotColumns->AddSelectionAppendNewIndex(i);
      continue;
    }
    if (humanReadableReport!=0)
    { if (useHtmlInReport)
      { *humanReadableReport << "<tr><td style=\"border-bottom:1pt solid black;\">";
        if (formatAsLinearSystem)
          *humanReadableReport << CGI::GetMathSpanPure(this->ToStringSystemLatex(carbonCopyMat, theFormat),-1);
        else
          *humanReadableReport << CGI::GetMathSpanPure(this->ToStringLatex(),-1);
        *humanReadableReport << "</td><td style=\"border-bottom:1pt solid black;\">Selected pivot column "
        << i+1 << ". ";
        if (NumFoundPivots!=tempI)
          *humanReadableReport << "Swapping rows so the pivot row is number " << NumFoundPivots << ". ";
      } else
      { if (formatAsLinearSystem)
          *humanReadableReport << "$" << this->ToStringSystemLatex(carbonCopyMat, theFormat) << "$";
        else
          *humanReadableReport << "$" << this->ToStringLatex() << "$";
        *humanReadableReport << "& Selected pivot column " << i+1 << ". ";
        if (NumFoundPivots!=tempI)
          *humanReadableReport << "Swapping rows so the pivot row is number " << NumFoundPivots << ". ";
      }
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
    if (humanReadableReport!=0)
    { if (useHtmlInReport)
        *humanReadableReport << "Eliminated the non-zero entries in the pivot column</td></tr>";
      else
        *humanReadableReport << "Eliminated the non-zero entries in the pivot column. \\\\\\hline\n";
    }
    NumFoundPivots++;
  }
  if (humanReadableReport!=0)
  { if (useHtmlInReport)
    { if (formatAsLinearSystem)
        *humanReadableReport << "<tr><td>" << CGI::GetMathSpanPure(this->ToStringSystemLatex(carbonCopyMat, theFormat),-1)
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
      else
        *humanReadableReport << "<tr><td>" << CGI::GetMathSpanPure(this->ToStringLatex())
        << "</td><td> Final result.</td></tr></table>\n\n\n\n";
    } else
    { if (formatAsLinearSystem)
        *humanReadableReport << "$" << this->ToStringSystemLatex(carbonCopyMat, theFormat)
        << "$& Final result.\\\\\n";
      else
        *humanReadableReport << "$" << this->ToStringLatex() << "$& Final result.\\\\\n";
      *humanReadableReport << "\\end{longtable}";
    }
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
  { crash << " Not implemented, please fix. " << crash;
    return output;
  }
  static const bool IsMonEqualToZero()
  { return false;
  }
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
  friend std::ostream& operator << (std::ostream& output, const Cone& theCone)
  { output << theCone.ToString();
    return output;
  }
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
  Rational GetPrincipalSlTwoCSInverseScale()const;
  void GetCoCartanSymmetric(Matrix<Rational>& output)const;
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
  inline bool IsMonEqualToZero()const
  { return false;
  }
  void GetAutomorphismActingOnVectorColumn(MatrixTensor<Rational>& output, int AutoIndex)const;
  Rational GetDefaultCoRootLengthSquared(int rootIndex)const;
  Rational GetDefaultRootLengthSquared(int rootIndex)const;
  Rational GetDefaultLongRootLengthSquared()const;
  Rational GetLongRootLengthSquared()const;
  Rational GetRatioRootSquaredToFirstSquared(int rootIndex)const;
  static Rational GetRatioLongRootToFirst(char inputWeylLetter, int inputRank);

  bool CanBeExtendedParabolicallyTo(const DynkinSimpleType& otherType)const;
  bool CanBeExtendedParabolicallyOrIsEqualTo(const DynkinSimpleType& otherType)const
  { if (*this==otherType)
      return true;
    return this->CanBeExtendedParabolicallyTo(otherType);
  }
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
  unsigned int HashFunction()const
  { return this->MonomialCollection<DynkinSimpleType, Rational>::HashFunction();
  }
  static unsigned int HashFunction(const DynkinType& input)
  { return MonomialCollection<DynkinSimpleType, Rational>::HashFunction(input);
  }
  void GetOuterAutosGeneratorsActOnVectorColumn(List<MatrixTensor<Rational> >& output);
  bool IsSimple(char* outputtype=0, int* outputRank=0, Rational* outputLength=0)const;
  void GetSortedDynkinTypes(List<DynkinSimpleType>& output)const;
  void SortTheDynkinTypes();
  bool Grow(const List<Rational>& allowedInverseScales, int AmbientWeylDim, List<DynkinType>& output, List<List<int> >* outputPermutationRoots)const;
  bool ContainsType(char theTypeLetter)const;
  void GetDynkinTypeWithDefaultScales(DynkinType& output)const;
  DynkinSimpleType GetGreatestSimpleType()const;
  DynkinSimpleType GetSmallestSimpleType()const;
  LargeInt GetWeylGroupSizeByFormula()const;
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
  void ComputeDiagramTypeModifyInput(Vectors<Rational>& inputRoots, WeylGroup& theWeyl);
  //the below function is just as the above but doesn't modify simpleBasisInput
  void ComputeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput, const Matrix<Rational>& theBilinearForm);
  void ComputeDiagramTypeModifyInputRelative
  (Vectors<Rational>& inputOutputSimpleWeightSystem, const HashedList<Vector<Rational> >& weightSystem, const Matrix<Rational>& theBilinearForm);
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
  bool IsMonEqualToZero()const
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
  bool IsID()const
  { int theDim=this->GetMinNumColsNumRows();
    Selection theSel;
    theSel.init(theDim);
    for (int i=0; i<this->size(); i++)
    { if ((*this)[i].vIndex!=(*this)[i].dualIndex)
        return false;
      if (this->theCoeffs[i]!=1)
        return false;
      theSel.AddSelectionAppendNewIndex((*this)[i].vIndex);
    }
    return theSel.CardinalitySelection==theDim;
  }
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
    output.MakeZero();
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
//  stOutput << "<hr>Inverting: " << this->ToStringMatForm();
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
  (const ElementUniversalEnveloping<coefficient>& theUE,
   ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables
   )const;
  void operator=(const MonomialGeneralizedVerma<coefficient>& other)
  { this->owneR=other.owneR;
    this->indexFDVector=other.indexFDVector;
    this->theMonCoeffOne=other.theMonCoeffOne;
  }

  std::string ToString(FormatExpressions* theFormat=0, bool includeV=true)const;
  static const bool IsMonEqualToZero()
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
  void ReduceMe
  (ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables)const;
  bool IsHWV()const
  { if (!this->theMonCoeffOne.IsEqualToOne())
      return false;
//    stOutput << "<br>hi, my name is: " << this->ToString() << " and my index is: " << this->indexFDVector
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
  void MultiplyMeByUEEltOnTheLeft
  (const ElementUniversalEnveloping<coefficient>& theUE, GlobalVariables& theGlobalVariables);
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
  static const bool IsMonEqualToZero()
  { return false;
  }
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

template<typename coefficient>
void Matrix<coefficient>::GetVectorFromRow(int rowIndex, Vector<coefficient>& output)const
{ output.SetSize(this->NumCols);
  for (int i=0; i<this->NumCols; i++)
    output.TheObjects[i]=this->elements[rowIndex][i];
}

template<typename coefficient>
void Matrix<coefficient>::GetVectorFromColumn(int colIndex, Vector<coefficient>& output)const
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

template <class coefficient>
void Matrix<coefficient>::ComputeDeterminantOverwriteMatrix(coefficient& output, const coefficient& theRingOne, const coefficient& theRingZero)
{ int tempI;
  output=theRingOne;
  coefficient tempRat;
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

template<class coefficient>
void Matrix<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub)
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
//  stOutput << "<br>" << out.str() << " has " << this->theMons.size << " multiplicands with hash functions: ";
//  for (int i=0; i<this->theMons.size; i++)
//    stOutput << this->theMons[i].HashFunction() << ", ";
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
//  stOutput << "<br>MonomialP " << out.str() << " has indexInOwner " << this->indexInOwner;
  return out.str();
}

template <class templateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& output, const MonomialCollection<templateMonomial, coefficient>& theCollection)
{ if (theCollection.size()==0)
  { output << "0";
    return output;
  }
  std::string tempS1, tempS2;
  List<templateMonomial> sortedMons;
  sortedMons=theCollection.theMonomials;
  sortedMons.QuickSortDescending();
//  out << "(hash: " << this->HashFunction() << ")";
  int cutOffCounter=0;
  std::string oldCustomTimes="";
  for (int i=0; i<sortedMons.size; i++)
  { templateMonomial& currentMon=sortedMons[i];
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

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeLinearSubConstTermsLastRow(Matrix<coefficient>& theMat)
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
{ //stOutput << "<br>ze ue mon before sub: " << this->theMonCoeffOne.ToString();
  this->theMonCoeffOne.Substitution(theSub);
  //stOutput << "<br>ze ue mon after sub: " << this->theMonCoeffOne.ToString();
  ModuleSSalgebra<coefficient> newOwner;
  newOwner=*this->owneR;
  newOwner.Substitution(theSub);
  //stOutput << "<br>old index in owner: " << this->indexInOwner;
  int newModIndex=theMods.AddNoRepetitionOrReturnIndexFirst(newOwner);
  this->owneR=&theMods[newModIndex];
  //stOutput << "<br>new index in owner: " << this->indexInOwner;
}

template <class coefficient>
void MonomialTensorGeneralizedVermas<coefficient>::operator=
(const MonomialGeneralizedVerma<coefficient>& other)
{ this->theMons.SetSize(1);
  this->theMons[0]=other;
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::MultiplyMeByUEEltOnTheLefT
(const ElementUniversalEnveloping<coefficient>& theUE,
 ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables)const
{ MacroRegisterFunctionWithName("MonomialGeneralizedVerma<coefficient>::MultiplyMeByUEEltOnTheLefT");
  MonomialGeneralizedVerma<coefficient> currentMon;
  output.MakeZero();
  ElementSumGeneralizedVermas<coefficient> buffer;
  ProgressReport theReport(&theGlobalVariables);
  if (!this->GetOwner().owneR->flagHasNilradicalOrder)
    crash << "Calling generalized verma module simplification requires nilradical order on the generators. "
    << crash;
  for (int j=0; j<theUE.size(); j++)
  { currentMon.theMonCoeffOne=theUE[j];
//    stOutput << "<br>currentMon: " << currentMon.theMonCoeffOne.ToString();
    currentMon.theMonCoeffOne*=this->theMonCoeffOne;
//    stOutput << "<br>currentMon after multi: " << currentMon.theMonCoeffOne.ToString();
    currentMon.owneR=this->owneR;
    currentMon.indexFDVector=this->indexFDVector;
    currentMon.owneR=this->owneR;
//    stOutput << "<hr>Applying " <<theUE.theCoeffs[j].ToString() << " times " << theUE[j].ToString() << " on " << this->ToString();
    std::stringstream reportStream;
    reportStream << "reducing mon: " << currentMon.ToString() << ", index" << j+1 << " out of " << theUE.size() << "...";
//    stOutput << "reducing mon: " << currentMon.ToString() << ", index" << j+1 << " out of " << theUE.size() << "...";
    theReport.Report(reportStream.str());
    currentMon.ReduceMe(buffer, theGlobalVariables);
    reportStream << " done.";
//    stOutput << " done.";
    theReport.Report(reportStream.str());
//    stOutput << "<br>buffer: " << buffer.ToString() << " multiplied by " << theUE.theCoeffs[j].ToString();
    buffer*=theUE.theCoeffs[j];
    output+=buffer;
//    stOutput << " equals: " << buffer.ToString();
  }
//  stOutput << "<br>result: " << this->ToString();
}

template <class coefficient>
void ElementSumGeneralizedVermas<coefficient>::MultiplyMeByUEEltOnTheLeft
  (const ElementUniversalEnveloping<coefficient>& theUE, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ElementSumGeneralizedVermas<coefficient>::MultiplyMeByUEEltOnTheLeft");
  ElementSumGeneralizedVermas<coefficient> buffer, Accum;
//stOutput << "<br>Multiplying " << this->ToString() << " by " << theUE.ToString();
  Accum.MakeZero();
  for (int i=0; i<this->size(); i++)
  {// stOutput << "<br>Multiplying " << this->TheObjects[i].ToString() << " by " << theUE.ToString() << " by " << this->theCoeffs[i].ToString();
    (*this)[i].MultiplyMeByUEEltOnTheLefT(theUE, buffer, theGlobalVariables);
    //stOutput << "<br>buffer " << buffer.ToString() << " multiplied by coeff " << this->theCoeffs[i].ToString();
    buffer*=this->theCoeffs[i];
//    stOutput << "<br>to obtain " << buffer.ToString();
    Accum+=buffer;
//    stOutput << " <br> to accummulate to " << Accum.ToString();
  }
  *this=Accum;
//  stOutput << "<br>To get in the damned end: " << this->ToString();
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::ReduceMe
(ElementSumGeneralizedVermas<coefficient>& output, GlobalVariables& theGlobalVariables)const
{ MacroRegisterFunctionWithName("MonomialGeneralizedVerma::ReduceMe");
  //stOutput << "<hr><hr>Reducing  " << this->ToString();
  ModuleSSalgebra<coefficient>& theMod=*this->owneR;
  output.MakeZero();
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon=this->theMonCoeffOne;
  tempMon*=theMod.theGeneratingWordsNonReduced[this->indexFDVector];
  int indexCheck=theMod.theGeneratingWordsNonReduced.GetIndex(tempMon);
  if (!this->owneR->owneR->flagHasNilradicalOrder)
    crash << "Owner needs nilradical order!!!" << crash;
  if (indexCheck!=-1)
  { MonomialGeneralizedVerma<coefficient> basisMon;
    basisMon.MakeConst(*this->owneR);
    basisMon.indexFDVector=indexCheck;
    output.AddMonomial(basisMon, 1);
//    stOutput << "<br>Reduced " << this->ToString() << " to " << output.ToString() << " = " << basisMon.ToString();
//    stOutput << "<br> index check is " << indexCheck << " corresponding to " << theMod.theGeneratingWordsNonReduced[indexCheck].ToString();
//    theGlobalVariables.MakeProgressReport("Monomial basis of fd part. ", 2);
    return;
  }
//  stOutput << "<br>Not a monomial basis of fd part";
//  theGlobalVariables.MakeProgressReport("Monomial not basis of fd part. ", 2);
//  theMod.GetOwner().OrderSetNilradicalNegativeMost(theMod.parabolicSelectionNonSelectedAreElementsLevi);
//  stOutput << "<br>";
//  for (int i=0; i<theMod.GetOwner().UEGeneratorOrderIncludingCartanElts.size; i++)
//  { stOutput << "<br>generator index " << i << " has order " << theMod.GetOwner().UEGeneratorOrderIncludingCartanElts[i];
//  }
  ElementUniversalEnveloping<coefficient> theUEelt;
  theUEelt.MakeZero(*this->GetOwner().owneR);
  theUEelt.AddMonomial(this->theMonCoeffOne, 1);
//  stOutput << " <br>the monomial:" << this->ToString();
  theUEelt.Simplify(&theGlobalVariables, 1, 0);
//  stOutput << " <br>the corresponding ue with F.D. part cut off: " << theUEelt.ToString();

  MonomialUniversalEnveloping<coefficient> currentMon;
  MonomialGeneralizedVerma<coefficient> newMon;
  MatrixTensor<coefficient> tempMat1, tempMat2;
  //stOutput << theMod.ToString();
  //stOutput << "<br>theMod.theModuleWeightsSimpleCoords.size: "
  //<< theMod.theModuleWeightsSimpleCoords.size;
  ProgressReport theReport(&theGlobalVariables);
  coefficient theCF;
  for (int l=0; l<theUEelt.size(); l++)
  { currentMon=theUEelt[l];
    //stOutput << "<br> Processing monomial " << currentMon.ToString();
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
      tempMat1=theMod.GetActionGeneratorIndeX(theIndex, theGlobalVariables);
      tempMat1.RaiseToPower(thePower);
      tempMat1*=tempMat2;
      currentMon.Powers.size--;
      currentMon.generatorsIndices.size--;
      reportStream << "done!";
      theReport.Report(reportStream.str());
    }
//    stOutput << "<br> Action is the " << currentMon.ToString() << " free action plus <br>" << tempMat1.ToString();
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
        //stOutput << "<br>adding to " << output.ToString()
        //<< " the monomial " << newMon.ToString() << " with coefficient "
        //<< theCF.ToString() << " to obtain ";
        output.AddMonomial(newMon, theCF);
        //stOutput << output.ToString();
      }
    }
  }
//  stOutput << "<br>Matrix of the action: " << tempMat1.ToString();
//  stOutput << "<br> Final output: " << output.ToString();
  theMod.GetOwner().OrderStandardAscending();
}

template<class coefficient>
void Vectors<coefficient>::IntersectTwoLinSpaces
(const List<Vector<coefficient> >& firstSpace, const List<Vector<coefficient> >& secondSpace, List<Vector<coefficient> >& output, GlobalVariables* theGlobalVariables)
{ //stOutput << "<br>*****Debugging Intersection linear spaces: ";
  //stOutput << "<br>input first space: " << firstSpace.ToString();
  //stOutput << "<br>input second space: " << secondSpace.ToString();
  Vectors<coefficient> firstReduced, secondReduced;
  Selection tempSel;
  Vectors<coefficient>::SelectABasisInSubspace(firstSpace, firstReduced, tempSel, theGlobalVariables);
  Vectors<coefficient>::SelectABasisInSubspace(secondSpace, secondReduced, tempSel, theGlobalVariables);
//  stOutput << "<br>first selected basis: " << firstReduced.ToString();
//  stOutput << "<br>second selected basis: " << secondReduced.ToString();
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
//  stOutput << "<br>The matrix before the gaussian elimination:" << theMat.ToString();
  theMat.GaussianEliminationByRows(0, &tempSel);
//  stOutput << "<br>The matrix after the gaussian elimination:" << theMat.ToString();
  output.ReservE(tempSel.CardinalitySelection);
  output.size=0;
  Vector<coefficient> nextIntersection;
  for(int i=0; i<tempSel.CardinalitySelection; i++)
  { int currentIndex=tempSel.elements[i];
//    stOutput << "<br>current pivot index : " << currentIndex;
    if(currentIndex<firstReduced.size)
      crash << crash;
    nextIntersection.MakeZero(theDim);
    for (int j=0; j<firstReduced.size; j++)
      if (!tempSel.selected[j])
        nextIntersection+=firstReduced[j]*theMat.elements[j][currentIndex];
    output.AddOnTop(nextIntersection);
  }
//  stOutput << "<br> final output: " << output.ToString();
//  stOutput << "<br>******************End of debugging linear space intersections";
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

template<class coefficient>
bool Matrix<coefficient>::IsPositiveDefinite()
{ if (this->NumRows!=this->NumCols)
    crash << "This is a programming error: attempting to evaluate whether a matrix is positive definite, but the matrix is not square. " << crash;
  coefficient det;
  Matrix<coefficient> tempMat;
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

#endif
