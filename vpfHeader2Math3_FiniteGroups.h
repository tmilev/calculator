//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfCharacterHeader_alreadyIncluded
#define vpfCharacterHeader_alreadyIncluded
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math8_VectorSpace.h"

//To do: move Weyl groups to this file. Eliminate all redundant code and organize nicely.

static ProjectInformationInstance ProjectInfoVpfFiniteGroups(__FILE__, "Header file, finite groups. Work in progress.");

template <typename somegroup, class elementSomeGroup>
class Subgroup;

template <typename ElementEuclideanDomain>
struct DivisionResult
{ ElementEuclideanDomain quotient;
  ElementEuclideanDomain remainder;
};

template<class someFiniteGroup, typename coefficient>
class ClassFunction
{
public:
  someFiniteGroup *G;
  Vector<coefficient> data;
  ClassFunction():G(0){} //the syntax :G(0) initializes the pointer G with 0.
  //Although there may be a minor speed penalty
  //(such a speed penalty is system dependent and possibly
  //even hardware dependent),
  //it is worth it to initialize all pointers with 0.
  void MakeZero(someFiniteGroup& inputWeyl);
  bool IsEqualToZero()const
  { return this->data.IsEqualToZero();
  }
  coefficient InnerProduct(const ClassFunction &other) const;
  coefficient Norm() const;
  ClassFunction operator*(const ClassFunction &other) const;
  ClassFunction Sym2() const;
  ClassFunction Alt2() const;
  ClassFunction operator+(const ClassFunction &other) const;
  ClassFunction operator-(const ClassFunction &other) const;
  ClassFunction ReducedWithChars(const List<ClassFunction> chars = 0);
  coefficient& operator[](int i) const;
  std::string ToString(FormatExpressions* theFormat=0) const;
  static unsigned int HashFunction(const ClassFunction& input);
  inline unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  void operator*=(const coefficient& inputCF)
  { this->data*=inputCF;
  }
  ClassFunction operator*(const coefficient& inputCF)const
  { ClassFunction result=*this;
    result.data*=inputCF;
    return result;
  }
  void operator+=(const ClassFunction& right)
  { this->data+=right.data;
  }
  void operator-=(const ClassFunction& right)
  { this->data-=right.data;
  }
  void operator*=(const ClassFunction& right);
  bool operator==(const ClassFunction& other) const;
  bool operator!=(const ClassFunction& other) const
  { return ! (*this==other);
  }
  bool operator>(const ClassFunction& right) const;
  bool operator<(const ClassFunction& right) const
  { if (*this==right || *this>right)
      return false;
    return true;
  }
};

//assumptions on the FiniteGroup:
//1. The finite group has a small number of conjugacy classes. Z/10000000Z is not OK
template <typename elementSomeGroup>
class FiniteGroup
{
protected:
  LargeInt sizePrivate;
public:
  List<elementSomeGroup> generators;
  HashedList<elementSomeGroup> theElements;
  HashedList<elementSomeGroup> unionGeneratorsCC;
  struct ConjugacyClass
  {
  public:
    ConjugacyClass()
    { this->flagRepresentativeComputed=false;
      this->flagElementsComputed=false;
    }
    LargeInt size;
    List<int> indicesEltsInOwner;
    List<elementSomeGroup> theElements;
    elementSomeGroup representative;
    bool flagRepresentativeComputed;
    bool flagElementsComputed;
  };
  List<ConjugacyClass> conjugacyClasseS;
  HashedList<Polynomial<Rational> > CCsStandardRepCharPolys;
  List<ClassFunction<FiniteGroup<elementSomeGroup>, Rational> > characterTable;

  //<-The character polynomials in the ``standard representation''.
  //The ``standard representation'' is specified by the elementSomeGroup class.
  //It is up to the user of the FiniteGroup template to define which representation is
  //``standard'' - this is not a serious restriction as one can always choose the trivial representation.
  bool flagCCsComputed;
  bool flagCCRepresentativesComputed;
  bool flagAllElementsAreComputed;
  bool flagCharPolysAreComputed;
  bool flagGeneratorsConjugacyClassesComputed;

  bool flagDeallocated;
  FiniteGroup(): flagDeallocated(false)
  { this->init();
  }
  virtual ~FiniteGroup()
  { this->flagDeallocated=true;
  }
  virtual void InitGenerators()
  {}
  bool CheckConsistency()const
  { if (this->flagDeallocated)
      crash << "This is a programming error: use after free of Finite group. " << crash;
    return true;
  }
  bool CheckInitialization()const;
  bool CheckConjugacyClassRepsMatchCCsizes(GlobalVariables* theGlobalVariables);
  bool CheckOrthogonalityCharTable(GlobalVariables* theGlobalVariables);
  void SetSizE(const LargeInt& inputSize)
  { this->sizePrivate=inputSize;
  }
  void init();
  std::string ToString(FormatExpressions* theFormat=0)const;
  std::string ToStringElements(FormatExpressions* theFormat=0)const;
  std::string ToStringConjugacyClasses(FormatExpressions* theFormat=0)const;
  int ConjugacyClassCount()const;
  LargeInt size()const;
  virtual LargeInt GetGroupSizeByFormula()const
  { return -1;
  } //non-positive result means no formula is known.
  bool AreConjugate(const elementSomeGroup& left, const elementSomeGroup& right);

  bool ComputeAllElements(int MaxElements=-1, GlobalVariables* theGlobalVariables=0);
  void ComputeCCfromAllElements(GlobalVariables* theGlobalVariables);
  void ComputeCCfromCCindicesInAllElements(const List<List<int> >& ccIndices);

  void ComputeCCSizeOrCCFromRepresentative
(ConjugacyClass& inputOutputClass, bool storeCC, GlobalVariables* theGlobalVariables)
;
  bool RegisterCCclass
(const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants, GlobalVariables* theGlobalVariables)
 ;
  bool ComputeCCRepresentatives
  (GlobalVariables* theGlobalVariables)
  ;
  void ComputeGeneratorsConjugacyClasses
  (GlobalVariables* theGlobalVariables)
  ;
  void ComputeCCSizesAndRepresentatives(GlobalVariables* theGlobalVariables);
  void GetSignCharacter(Vector<Rational>& outputCharacter);
  template <typename coefficient>
  coefficient GetHermitianProduct(const Vector<coefficient>& leftCharacter, const Vector<coefficient>& rightCharacter)const;
};

template <class templateWeylGroup>
class ElementWeylGroup
{ friend std::ostream& operator << (std::ostream& output, const ElementWeylGroup<templateWeylGroup>& theElement)
  { output << theElement.ToString();
    return output;
  }
public:
  templateWeylGroup* owner;
  List<int> generatorsLastAppliedFirst;
  bool flagDeallocated;
  ElementWeylGroup(const ElementWeylGroup& other)
  { this->flagDeallocated=false;
    this->operator=(other);
  }
  ElementWeylGroup():owner(0), flagDeallocated(false)
  {
  }
  ~ElementWeylGroup()
  { this->flagDeallocated=true;
  }
  void operator=(const ElementWeylGroup& other)
  { this->owner=other.owner;
    this->generatorsLastAppliedFirst=other.generatorsLastAppliedFirst;
  }
  void ActOn(Vector<Rational>& inputOutput)const
  { this->owner->ActOn(*this, inputOutput);
  }
  bool CheckConsistency()const
  { if (this->flagDeallocated)
      crash << "Programming error: use after free of class ElementWeylGroup" << crash;
    return true;
  }
  bool CheckInitialization()const
  { this->CheckConsistency();
    if (this->owner==0)
    { crash << "This is a programming error: non-initialized element Weyl group. " << crash;
      return false;
    }
    this->owner->CheckConsistency();
    return true;
  }
  int Sign()const
  { return this->generatorsLastAppliedFirst.size %2 ==0 ? 1 : -1;
  }
  static void Conjugate(const ElementWeylGroup& elementWeConjugateBy, const ElementWeylGroup& inputToBeConjugated, ElementWeylGroup& output);
  void MakeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroup& inputWeyl);
  void MakeSimpleReflection(int simpleRootIndex, WeylGroup& inputWeyl);
  void MakeRootReflection(const Vector<Rational>& mustBeRoot, WeylGroup& inputWeyl);
  void MakeCanonical();
  void MakeID(templateWeylGroup& inputWeyl);
  void MakeID(const ElementWeylGroup& initializeFrom);

  void ToString(std::string& output)
  { output=this->ToString();
  }
  std::string ToStringInvariants(FormatExpressions* theFormat)const;
  std::string ToString(FormatExpressions* theFormat=0, List<int>* DisplayIndicesOfSimpleRoots=0)const
  { return this->ToString(-1, theFormat, DisplayIndicesOfSimpleRoots);
  }
  std::string ToString(int NumSimpleGens, FormatExpressions* theFormat=0, List<int>* DisplayIndicesOfSimpleRoots=0)const;
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const ElementWeylGroup<templateWeylGroup>& input)
  { return input.HashFunction();
  }
  void operator*=(const ElementWeylGroup<WeylGroup>& other);
  ElementWeylGroup<WeylGroup> operator*(const ElementWeylGroup<WeylGroup>& other) const
  { ElementWeylGroup<WeylGroup> result=*this;
    result*=other;
    return result;
  }
  static inline bool IsEqualToZero()
  { return false;
  }
  void GetCycleStructure(VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult)const;
  Vector<Rational> operator*(const Vector<Rational>& v) const;
  ElementWeylGroup<WeylGroup> Inverse() const;
  bool HasDifferentConjugacyInvariantsFrom(const ElementWeylGroup<templateWeylGroup>& right)const;
  void GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& output)const;
  bool operator==(const ElementWeylGroup<WeylGroup>& other)const
  { if (this->owner!=other.owner)
      return false;
    return this->generatorsLastAppliedFirst==other.generatorsLastAppliedFirst;
  }
  void MakeFromReadableReflections(WeylGroup& input, bool dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections);
  bool operator>(const ElementWeylGroup<templateWeylGroup>& other)const;
};

template <class coefficient>
class FinitelyGeneratedMatrixMonoid
{
  public:
  List<MatrixTensor<coefficient> > theGenerators;
  HashedList<MatrixTensor<coefficient> > theElements;
  bool GenerateElements(int upperBoundNonPositiveMeansNoLimit, GlobalVariables* theGlobalVariables=0);
  std::string ToString(FormatExpressions* theFormat=0)const;
};

template <class coefficient>
bool FinitelyGeneratedMatrixMonoid<coefficient>::GenerateElements(int upperBoundNonPositiveMeansNoLimit, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FinitelyGeneratedMatrixMonoid::GenerateElements");
  this->theElements.Clear();
  this->theElements.AddOnTopNoRepetition(theGenerators);
  MatrixTensor<coefficient> currentElement;
  for (int i=0; i<this->theElements.size; i++)
    for (int j=0; j<this->theGenerators.size; j++)
    { currentElement=this->theGenerators[j];
      currentElement*=this->theElements[i];
      this->theElements.AddOnTopNoRepetition(currentElement);
      if (upperBoundNonPositiveMeansNoLimit>0)
        if (this->theElements.size>upperBoundNonPositiveMeansNoLimit)
          return false;
    }
  return true;
}

template <class coefficient>
class WeylGroupVirtualRepresentation;

template <class coefficient>
class WeylGroupRepresentation;

class SubgroupRootReflections;

class WeylGroup : public FiniteGroup<ElementWeylGroup<WeylGroup> >
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
  bool flagOuterAutosGeneratorsComputed;
  bool flagAllOuterAutosComputed;
  inline void ComputeFundamentalToSimpleMatrices()
  { if (this->flagFundamentalToSimpleMatricesAreComputed)
      return;
    Vectors<Rational> fundamentalBasis;
    this->GetFundamentalWeightsInSimpleCoordinates(fundamentalBasis);
    this->FundamentalToSimpleCoords.AssignVectorsToRows(fundamentalBasis);
    this->FundamentalToSimpleCoords.Transpose();
    this->SimpleToFundamentalCoords=this->FundamentalToSimpleCoords;
    this->SimpleToFundamentalCoords.Invert();
    this->flagFundamentalToSimpleMatricesAreComputed=true;
  }
  bool LoadConjugacyClasses();
  bool LoadCharTable();
public:
  bool flagIrrepsAreComputed;
  bool flagCharTableIsComputed;

  DynkinType theDynkinType;
  Matrix<Rational> CartanSymmetric;
  Matrix<Rational> CoCartanSymmetric;
  Vector<Rational> rho;
  HashedList<Vector<Rational> > rhoOrbit;
  HashedList<Vector<Rational> > RootSystem;
  Vectors<Rational> RootsOfBorel;

  MemorySaving<FinitelyGeneratedMatrixMonoid<Rational> > theOuterAutos;
  List<ElementWeylGroup<WeylGroup> > squaresFirstConjugacyClassRep;

  List<WeylGroupRepresentation<Rational> > irreps;
//  void MakeFromParSel(Vector<Rational> & parSel, WeylGroup& input);

  typedef FiniteGroup<ElementWeylGroup<WeylGroup> > WeylGroupBase;
  void init();
  static void GetCoCartanSymmetric(const Matrix<Rational>& input, Matrix<Rational>& output);
  void ComputeSquares(GlobalVariables* theGlobalVariables);
  void ComputeInitialIrreps(GlobalVariables* theGlobalVariablesd);
  void ComputeConjugacyClassesThomasVersion();
  void GetSignSignatureParabolics(List<SubgroupRootReflections>& outputSubgroups, GlobalVariables* theGlobalVariables=0);
  void GetSignSignatureAllRootSubsystems(List<SubgroupRootReflections>& outputSubgroups, GlobalVariables* theGlobalVariables=0);
  void GetSignSignatureRootSubgroups
  (List<SubgroupRootReflections>& outputSubgroups,
   const List<Vectors<Rational> >& rootsGeneratingReflections,
   GlobalVariables* theGlobalVariables=0)
   ;

  bool LoadConjugacyClassesHelper();

  void ComputeOrLoadCharacterTable(GlobalVariables* theGlobalVariables=0, std::stringstream* reportStream=0);
  void ComputeOrLoadConjugacyClasses(GlobalVariables* theGlobalVariables=0, std::stringstream* reportStream=0);

  void ComputeIrreducibleRepresentationsTodorsVersion(GlobalVariables* theGlobalVariables=0);
  void ComputeIrreducibleRepresentationsThomasVersion(GlobalVariables* theGlobalVariables=0);
  void ComputeExtremeRootInTheSameKMod(const Vectors<Rational>& inputSimpleBasisK, const Vector<Rational>& inputRoot, Vector<Rational>& output, bool lookingForHighest);
  void AddIrreducibleRepresentation(const WeylGroupRepresentation<Rational>& p);
  void AddCharacter(const ClassFunction<WeylGroup::WeylGroupBase, Rational>& X);
  void ComputeRho(bool Recompute);
  std::string ToString(FormatExpressions* theFormat=0);
  std::string ToStringCppConjugacyClasses(FormatExpressions* theFormat=0);
  std::string ToStringCppCharTable(FormatExpressions* theFormat=0);
  void MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared=0);
  void MakeFromDynkinType(const DynkinType& inputType);
  void MakeFinalSteps();
  void InitGenerators();
  void MakeMeFromMyCartanSymmetric();
  void MakeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType);
  void ComputeCoCartanSymmetricFromCartanSymmetric();
  void ComputeOuterAutoGenerators();
  void ComputeOuterAutos();
  bool CheckConsistency()const;
  bool CheckInitializationFDrepComputation()const;
  bool CheckInitializationConjugacyClasses()const;
  void GetSignCharacter(Vector<Rational>& out);
  void GetStandardRepresentation(WeylGroupRepresentation<Rational>& output);
  void GetSignRepresentation(WeylGroupRepresentation<Rational>& output);
  void GetTrivialRepresentation(WeylGroupRepresentation<Rational>& output);
  void GenerateAdditivelyClosedSubset(Vectors<Rational>& input, Vectors<Rational>& output);
  Rational GetKillingDivTraceRatio();
  Rational EstimateNumDominantWeightsBelow(Vector<Rational>& inputHWsimpleCoords, GlobalVariables& theGlobalVariables);
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
  coefficient GetScalarProdSimpleRoot(const Vector<coefficient>& input, int indexSimpleRoot)const
  { if (indexSimpleRoot<0 || indexSimpleRoot>=this->GetDim())
      crash << "This is a programming error. Attempting to take scalar product with simple root of index " << indexSimpleRoot
      << " which is impossible, as the rank of the Weyl group is " << this->GetDim() << ". " << crash;
    coefficient result, buffer;
    result=0;
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
  void RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign=0, bool* stabilizerFound=0, ElementWeylGroup<WeylGroup>* raisingElt=0);
  bool AreMaximallyDominant(List<Vector<Rational> >& theWeights, bool useOuterAutos);
  template <class coefficient>
  void RaiseToMaximallyDominant(List<Vector<coefficient> >& theWeights, bool useOuterAutos);
  void GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables);
  void GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output)const;
  ElementWeylGroup<WeylGroup> SimpleConjugation(int i, const ElementWeylGroup<WeylGroup>& vv);
  Matrix<Rational> SimpleReflectionMatrix(int indexSimpleRoot)const;
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
  bool LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup<WeylGroup>& left, ElementWeylGroup<WeylGroup>& right);
  bool IsElementWeylGroup(const MatrixTensor<Rational>& theMat);
  bool IsElementWeylGroupOrOuterAuto(const MatrixTensor<Rational>& theMat);
  void GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output);
  template <class coefficient>
  bool GetAlLDominantWeightsHWFDIM
  (Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
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
  inline int GetDim()const
  { return this->CartanSymmetric.NumRows;
  }
  void ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output);
  void ComputeRootsOfBorel(Vectors<Rational>& output);
  LargeInt GetGroupSizeByFormula()const
  { return this->theDynkinType.GetWeylGroupSizeByFormula();
  }
  static LargeInt GetGroupSizeByFormula(char weylLetter, int theDim);
  bool IsARoot(const Vector<Rational>& input)const
  { return this->RootSystem.Contains(input);
  }
  void GenerateRootSubsystem(Vectors<Rational>& theRoots);
  template <class coefficient>
  bool GenerateOuterOrbit
  (Vectors<coefficient>& theWeights, HashedList<Vector<coefficient> >& output, HashedList<ElementWeylGroup<WeylGroup> >* outputSubset=0,  int UpperLimitNumElements=-1);
  template <class coefficient>
  bool GenerateOrbit
  (Vectors<coefficient>& theWeights, bool RhoAction, HashedList<Vector<coefficient> >& output, bool UseMinusRho, int expectedOrbitSize=-1,
   HashedList<ElementWeylGroup<WeylGroup> >* outputSubset=0, int UpperLimitNumElements=-1, GlobalVariables* theGlobalVariables=0);
  template <class coefficient>
  bool GenerateOrbit
  (Vector<coefficient>& theWeight, bool RhoAction, HashedList<Vector<coefficient> >& output, bool UseMinusRho, int expectedOrbitSize=-1,
   HashedList<ElementWeylGroup<WeylGroup> >* outputSubset=0, int UpperLimitNumElements=-1, GlobalVariables* theGlobalVariables=0)
  { Vectors<Rational> theWeights;
    theWeights.AddOnTop(theWeight);
    return this->GenerateOrbit(theWeights, RhoAction, output, UseMinusRho, expectedOrbitSize, outputSubset, UpperLimitNumElements, theGlobalVariables);
  }
//  int GetNumRootsFromFormula();
  void GenerateRootSystem();
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  void ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho);
  void ProjectOnTwoPlane(Vector<Rational> & orthonormalBasisVector1, Vector<Rational> & orthonormalBasisVector2, GlobalVariables& theGlobalVariables);
  void GetLowestElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup<WeylGroup>* outputWeylElt, Vectors<Rational>& bufferEiBAsis, bool RhoAction, bool UseMinusRho, int* sign=0,
   bool* stabilizerFound=0)
  { this->GetExtremeElementInOrbit(inputOutput, outputWeylElt, bufferEiBAsis, true, RhoAction, UseMinusRho, sign, stabilizerFound);
  }
  void GetHighestElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup<WeylGroup>* outputWeylElt, Vectors<Rational>& bufferEiBAsis, bool RhoAction,
   bool UseMinusRho, int* sign, bool* stabilizerFound)
  { this->GetExtremeElementInOrbit(inputOutput, outputWeylElt, bufferEiBAsis, false, RhoAction, UseMinusRho, sign, stabilizerFound);
  }
  void GetExtremeElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup<WeylGroup>* outputWeylElt, Vectors<Rational>& bufferEiBAsis,
   bool findLowest, bool RhoAction, bool UseMinusRho, int* sign, bool* stabilizerFound);
  void GetLongestWeylElt(ElementWeylGroup<WeylGroup>& outputWeylElt);
  bool IsEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input);
  void GetCoxeterElement(ElementWeylGroup<WeylGroup>& outputWeylElt)
  { outputWeylElt.generatorsLastAppliedFirst.SetSize(this->GetDim());
    for (int i=0; i<outputWeylElt.generatorsLastAppliedFirst.size; i++)
      outputWeylElt.generatorsLastAppliedFirst[i]=i;
  }
  Vector<Rational> ApplyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational> &vv) const;

  template <class coefficient>
  void ActOn(const ElementWeylGroup<WeylGroup>& theGroupElement, Vector<coefficient>& theVector)const
  { for (int i=theGroupElement.generatorsLastAppliedFirst.size-1; i>=0; i--)
      this->SimpleReflection(theGroupElement.generatorsLastAppliedFirst[i], theVector);
  }
  template <class coefficient>
  void ActOnRhoModified(const ElementWeylGroup<WeylGroup>& theGroupElement, Vector<coefficient>& theVector)const
  { for (int i=theGroupElement.generatorsLastAppliedFirst.size-1; i>=0; i--)
      this->SimpleReflectionRhoModified(theGroupElement.generatorsLastAppliedFirst[i], theVector);
  }
  template <class coefficient>
  void ActOnRhoModified(int indexOfWeylElement, Vector<coefficient>& theVector)const
  { this->ActOnRhoModified(this->theElements[indexOfWeylElement], theVector);
  }
  template <class coefficient>
  void ActOn(int indexOfWeylElement, Vector<coefficient>& theVector)const
  { this->ActOn(this->theElements[indexOfWeylElement], theVector);
  }
  template <class coefficient>
  void ActOnDual(int index,Vector<coefficient>& theVector, bool RhoAction, const coefficient& theRingZero);
  //theRoot is a list of the simple coordinates of the Vector<Rational>
  //theRoot serves as both input and output
  void ActOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  void ActOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  void ActOnDualSpaceElementByGroupElement(int index, Vector<Rational>& theDualSpaceElement, bool RhoAction);
  void SimpleReflectionRoot(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  template <class coefficient>
  void SimpleReflection(int index, Vector<coefficient>& theVector)const;
  template <class coefficient>
  void SimpleReflectionRhoModified(int index, Vector<coefficient>& theVector)const;
  template <class coefficient>
  void SimpleReflectionMinusRhoModified(int index, Vector<coefficient>& theVector)const;
  ElementWeylGroup<WeylGroup> GetRootReflection(int rootIndex);
  void GetGeneratorList(int g, List<int>& out)const;
  int MultiplyElements(int left, int right) const;
  //<-multiply elements is better name than multiply (groups can be tensored/direct product multiplied.
  //MultiplyElements is unambiguous.
  int Invert(int g) const;
  Matrix<Rational> GetMatrixStandardRep(int elementIndex)const;
  void GetMatrixStandardRep(const ElementWeylGroup<WeylGroup>& input, Matrix<Rational>& outputMatrix)const;
  Matrix<Rational> GetMatrixStandardRep(const ElementWeylGroup<WeylGroup>& input)const
  { Matrix<Rational> result;
    this->GetMatrixStandardRep(input, result);
    return result;
  }
  void GetElementOfMatrix(Matrix<Rational>& input, ElementWeylGroup<WeylGroup> &output);
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
        result+=this->CartanSymmetric.elements[i][j].DoubleValue()*r1[i]*r2[j];
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
  int operator()(int i, int j) const;
  bool operator==(const WeylGroup& other)const;
  void operator+=(const WeylGroup& other);
};

template <class coefficient>
class WeylGroupRepresentation
{
  private:
  List<Matrix<coefficient> > theElementImageS;
  List<bool> theElementIsComputed;
  ClassFunction<WeylGroup::WeylGroupBase, coefficient> theCharacteR;
  List<Matrix<coefficient> > classFunctionMatrices;
  List<bool> classFunctionMatricesComputed;
  List<Matrix<coefficient> > generatorS;
  friend class WeylGroup;
  WeylGroupRepresentation* parent;
  Vectors<coefficient> basis;
  Matrix<coefficient> gramMatrixInverted;
  public:
  bool flagCharacterIsComputed;
  WeylGroup* ownerGroup;
  List<std::string> names;

  WeylGroupRepresentation()
  { this->reset();
  }
  unsigned int HashFunction() const;
  bool CheckInitialization()const;
  bool CheckAllSimpleGensAreOK()const;
  static unsigned int HashFunction(const WeylGroupRepresentation<coefficient>& input)
  { return input.HashFunction();
  }
  void ComputeAllGeneratorImagesFromSimple(GlobalVariables* theGlobalVariables=0);
  const ClassFunction<WeylGroup::WeylGroupBase, coefficient>& GetCharacter();
  VectorSpace<coefficient> FindDecentBasis() const;
  void MultiplyBy(const WeylGroupRepresentation<coefficient>& other, WeylGroupRepresentation<coefficient>& output) const;
  void GetLargestDenominatorSimpleGens(LargeIntUnsigned& outputLCM, LargeIntUnsigned& outputDen)const;

  void reset();
  void init(WeylGroup& inputOwner);
  void CheckRepIsMultiplicativelyClosed();
  void GetClassFunctionMatrix(ClassFunction<WeylGroup::WeylGroupBase, coefficient>& inputChar, Matrix<coefficient>& outputMat, GlobalVariables* theGlobalVariables=0);
  void ClassFunctionMatrix(ClassFunction<WeylGroup::WeylGroupBase, coefficient>& inputCF, Matrix<coefficient>& outputMat, GlobalVariables* theGlobalVariables=0);
  int GetDim()const;
  void Restrict
  (const Vectors<coefficient>& VectorSpaceBasisSubrep, const ClassFunction<WeylGroup::WeylGroupBase, Rational>& remainingCharacter, WeylGroupRepresentation<coefficient>& output,
   GlobalVariables* theGlobalVariables=0);
  bool DecomposeTodorsVersionRecursive
  (WeylGroupVirtualRepresentation<coefficient>& outputIrrepMults, GlobalVariables* theGlobalVariables=0)
  ;
  bool DecomposeTodorsVersion
  (WeylGroupVirtualRepresentation<coefficient>& outputIrrepMults, GlobalVariables* theGlobalVariables=0)
  ;
  List<WeylGroupRepresentation<coefficient> > DecomposeThomasVersion();

  WeylGroupRepresentation<coefficient> Reduced() const;

  coefficient GetNumberOfComponents();
  void operator*=(const WeylGroupRepresentation<coefficient>& other);
  WeylGroupRepresentation<coefficient> operator*(const WeylGroupRepresentation<coefficient>& other)const
  { WeylGroupRepresentation<coefficient> result=*this;
    result*=other;
    return result;
  }
  bool operator==(const WeylGroupRepresentation<coefficient>& other)const
  { return this->ownerGroup==other.ownerGroup && this->theCharacteR==other.theCharacteR;
  }
  void SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace);
  std::string GetName() const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  Matrix<coefficient>& GetMatrixElement(int groupElementIndex);
  void GetMatrixElement(const ElementWeylGroup<WeylGroup>& input, Matrix<coefficient>& output);
  Matrix<coefficient> GetMatrixElement(const ElementWeylGroup<WeylGroup>& input);
  void SetElementImage(int elementIndex, const Matrix<coefficient>& input)
  { this->theElementImageS[elementIndex] = input;
    this->theElementIsComputed[elementIndex] = true;
  }
  bool operator>(const WeylGroupRepresentation<coefficient>& other)const;
  bool operator<(const WeylGroupRepresentation<coefficient>& other)const;
};

template <class coefficient>
class WeylGroupVirtualRepresentation : public MonomialCollection<ClassFunction<WeylGroup::WeylGroupBase, coefficient>, Rational>
{
public:
  void operator*=(const WeylGroupVirtualRepresentation<coefficient>& other);
  void AssignWeylGroupRep(const WeylGroupRepresentation<Rational>& other, GlobalVariables* theGlobalVariables=0);
  inline static unsigned int HashFunction(const WeylGroupVirtualRepresentation<coefficient>& input)
  { return input.HashFunction();
  }
  inline unsigned int HashFunction()const
  { return this->::MonomialCollection<ClassFunction<WeylGroup::WeylGroupBase, coefficient>, Rational>::HashFunction();
  }
};

//This class iterates over all elements of the orbit of a single element
//using the generators of the group.
//The element can be an element of the group, representation, etc.
//(note that the group elements can be interpretted as elements of
//the group ring representation).
template <class elementGroup, class elementRepresentation>
class OrbitIterator
{
public:
  typedef void (*GroupAction)(const elementGroup& actingElement, const elementRepresentation& inputElementActedUpon, elementRepresentation& output);
private:
  HashedList<elementRepresentation> privateLayer1, privateLayer2, privateLayer3;
  elementRepresentation bufferRepresentationElement;
public:
  HashedList<elementRepresentation>* previousLayer;
  HashedList<elementRepresentation>* currentLayer;
  HashedList<elementRepresentation>* nextLayer;
  OrbitIterator::GroupAction theGroupAction;
  List<elementGroup> theGroupGeneratingElements;
  int indexCurrentElement;
  OrbitIterator()
  { this->reset();
  }
  void reset()
  { this->previousLayer=&this->privateLayer1;
    this->currentLayer=&this->privateLayer2;
    this->nextLayer=&this->privateLayer3;
    this->theGroupAction=0;
    this->indexCurrentElement=-1;
  }
  const elementRepresentation& GetCurrentElement();
  bool IncrementReturnFalseIfPastLast();
  void init
  (const List<elementGroup>& inputGenerators, const elementRepresentation& inputElement, OrbitIterator::GroupAction inputGroupAction);
};

class OrbitIteratorWeylGroup: public OrbitIterator<ElementWeylGroup<WeylGroup>, ElementWeylGroup<WeylGroup> >
{
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<WeylGroup, coefficient> X);

template <typename somegroup, class elementSomeGroup>
class Subgroup : public FiniteGroup<elementSomeGroup>
{
public:
  somegroup *parent;
  List<int> ccRepresentativesPreimages;
  List<int> generatorPreimages;
  Subgroup();
  bool CheckInitialization();
  void init();
  LargeInt GetGroupSizeByFormula()const
  { return -1;
  }
  void initFromGroupAndGenerators(somegroup& inputGroup, const List<elementSomeGroup>& inputGenerators);
  void ComputeCCRepresentativesPreimages(GlobalVariables* theGlobalVariables);
  void ComputeCCSizesRepresentativesPreimages(GlobalVariables* theGlobalVariables)
  { this->ComputeCCSizesAndRepresentatives(theGlobalVariables);
    this->ComputeCCRepresentativesPreimages(theGlobalVariables);
  }
};

class SubgroupWeylGroup: public Subgroup<WeylGroup, ElementWeylGroup<WeylGroup> >
{
public:
  List<Rational> tauSignature;
  void ComputeTauSignature(GlobalVariables* theGlobalVariables);
  void GetSignCharacter(Vector<Rational>& out);
};

class SubgroupRootReflections : public SubgroupWeylGroup
{
public:
  Matrix<Rational> SubCartanSymmetric;
  DynkinType theDynkinType;
  Vectors<Rational> generatingSimpleRoots;
  void InitGenerators();
  void MakeParabolicSubgroup(WeylGroup& G, const Selection& inputGeneratingSimpleRoots);
  void MakeFromRoots(WeylGroup& G, const Vectors<Rational>& inputRootReflections);
  LargeInt GetGroupSizeByFormula()const
  { return this->theDynkinType.GetWeylGroupSizeByFormula();
  }
  void ComputeDynkinType();
  void ComputeCCSizesRepresentativesPreimages(GlobalVariables* theGlobalVariables);
};

class SubgroupWeylGroupOLD: public HashedList<ElementWeylGroup<WeylGroup> >
{
public:
  bool truncated;
  WeylGroup AmbientWeyl;
  WeylGroup Elements;
  List<ElementWeylGroup<WeylGroup> > RepresentativesQuotientAmbientOrder;
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
  void GetGroupElementsIndexedAsAmbientGroup(List<ElementWeylGroup<WeylGroup> >& output);
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
  void GetMatrixOfElement(const ElementWeylGroup<WeylGroup>& input, Matrix<Rational>& outputMatrix)const;
  template <class coefficient>
  bool GenerateOrbitReturnFalseIfTruncated(const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, int UpperLimitNumElements);
  bool ComputeSubGroupFromGeneratingReflections
  (Vectors<Rational>* inputGenerators, List<Vectors<Rational> >* inputExternalAutos, GlobalVariables* theGlobalVariables, int UpperLimitNumElements,
   bool recomputeAmbientRho);
  void ComputeRootSubsystem();
  template <class coefficient>
  void ActByElement(int index, Vector<coefficient>& theRoot)const;
  template <class coefficient>
  void ActByElement(int index, Vector<coefficient>& input, Vector<coefficient>& output)const;
  template <class coefficient>
  void ActByElement
  (const ElementWeylGroup<WeylGroup>& theElement, const Vector<coefficient>& input, Vector<coefficient>& output)const;
  template <class coefficient>
  void ActByElement(int index, const Vectors<coefficient>& input, Vectors<coefficient>& output)const
  { this->ActByElement(this->TheObjects[index], input, output);
  }
  template <class coefficient>
  void ActByElement
  (const ElementWeylGroup<WeylGroup>& theElement, const Vectors<coefficient>& input, Vectors<coefficient>& output)const;
  void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables);
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables);
  void operator=(const SubgroupWeylGroupOLD& other);
};

template <typename coefficient>
class ElementWeylGroupRing : public ElementMonomialAlgebra<ElementWeylGroup<WeylGroup>, coefficient>
{
public:
  void MakeEi(WeylGroup* G, int i);
  void MakeFromClassFunction(WeylGroup* G, const List<coefficient>& l);
  void MakeFromClassFunction(const ClassFunction<WeylGroup, coefficient>& l);
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<coefficient> g);

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeEi(WeylGroup *GG, int i)
{ ElementWeylGroup<WeylGroup> theMon;
  theMon=GG->theElements[i];
  *this=theMon;
}

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeFromClassFunction(const ClassFunction<WeylGroup, coefficient>& l)
{ MakeFromClassFunction(l.G,l.data);
}

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeFromClassFunction(WeylGroup* GG, const List<coefficient>& l)
{ MacroRegisterFunctionWithName("ElementWeylGroupRing::MakeFromClassFunction");
  if (GG==0)
    crash << "Weyl group pointer not allowed to be zero. " << crash;
  this->MakeZero();
  ElementWeylGroup<WeylGroup> theMon;
  for(int i=0; i<GG->ConjugacyClassCount(); i++)
    if (l[i]!=0)
      for(int j=0; j<GG->conjugacyClasseS.size; j++)
        this->AddMonomial(GG->conjugacyClasseS[i].theElements[j], l[i]);
}

//Matrix<Element>
template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<coefficient> g)
{ out << g.data;
  return out;
}

//---------------------------------------------------------------------------Finite group characters --------------------------------
//--------------------------------Finite group characters----------------------------

template<class someFiniteGroup, typename coefficient>
coefficient ClassFunction<someFiniteGroup, coefficient>::InnerProduct(const ClassFunction<someFiniteGroup, coefficient>& other) const
{ return this->G->GetHermitianProduct(this->data, other.data);
}

template<class someFiniteGroup, typename coefficient>
coefficient ClassFunction<someFiniteGroup, coefficient>::Norm() const
{ return this->InnerProduct(*this);
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::operator*(const ClassFunction<someFiniteGroup, coefficient>& other) const
{ ClassFunction<someFiniteGroup, coefficient> result=*this;
  result*=other;
  return result;
}

template<class someFiniteGroup, typename coefficient>
void ClassFunction<someFiniteGroup, coefficient>::operator*=(const ClassFunction<someFiniteGroup, coefficient>& right)
{ if (this->G!=right.G)
    crash << "Attempting to multiply class functions belonging to different groups.";
  for(int i=0; i<this->data.size; i++)
    this->data[i]*=right[i];
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::Sym2() const
{ ClassFunction<someFiniteGroup, coefficient> l;
  l.G = G;
  l.data.SetExpectedSize(G->ConjugacyClassCount());
  for(int i=0; i<G->ConjugacyClassCount(); i++)
    l.data.AddOnTop((this->data[i] * this->data[i] + this->data[G->squaresFirstConjugacyClassRep[i]])/2);
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::Alt2() const
{ ClassFunction<someFiniteGroup, coefficient> l;
  l.G = G;
  l.data.SetExpectedSize(G->ConjugacyClassCount());
  for(int i=0; i<G->ConjugacyClassCount(); i++)
    l.data.AddOnTop((this->data[i] * this->data[i] - this->data[G->squaresFirstConjugacyClassRep[i]])/2);
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::operator+(const ClassFunction<someFiniteGroup, coefficient>& other) const
{ ClassFunction<someFiniteGroup, coefficient> l=*this;
  l.data+=other.data;
  //this is slightly faster, but way too much code:
  /*l.G=this->G;
  l.data.SetSize(G->ConjugacyClassCount());
  for(int i=0; i<G->ConjugacyClassCount(); i++)
    l.data[i]=this->data[i] + other[i];*/
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::operator-(const ClassFunction &other) const
{ ClassFunction<someFiniteGroup, coefficient> l=*this;
  l.data+=other.data;
  //this is slightly faster, but way too much code:
  /*l.G = G;
  l.data.SetExpectedSize(G->ccCount);
  for(int i=0; i<G->ccCount; i++){
        l.data.AddOnTop(this->data[i] - other[i]);
    }*/
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::ReducedWithChars(const List<ClassFunction<someFiniteGroup, coefficient> > cchars)
{ ClassFunction<someFiniteGroup, coefficient> X = *this;
  if(X.Norm() == 0)
    return X;
  List<ClassFunction<someFiniteGroup, coefficient> > chars;
  if(cchars == 0)
    chars = this->G->characterTable;
  else
    chars = cchars;
  bool outerChanged = false;
  bool innerChanged = false;
  ClassFunction X2;
  int n;
  do
  { for(int i=0; i<chars.size; i++)
    { do
      { innerChanged = false;
        X2 = X - chars[i];
        // No virtual characters allowed
        coefficient n = X2[0];
        if(n<0)
          continue;
        bool bad = false;
        for(int iii=0; iii<X2.data.size; iii++){
          if(n<X2[iii]||n<-X2[iii])
            bad = true;
        }
        if(bad)
          continue;
        if(X2.Norm() < X.Norm())
        { X = X2;
          if(X.Norm() == 0)
            return X;
          innerChanged = true;
//          if(X[0]<0) // negative virtual character alert
//            for(int i=0; i<X.data.size; i++)
//              X.data[i] = -X.data[i];
        }
      } while(innerChanged);
    }
  } while(outerChanged);
  return X;
}

// Univariate dense polynomials.
template <typename coefficient>
class UDPolynomial
{
public:
   // "So the last shall be first, and the first last" -- Matthew 20:12
  List<coefficient> data;
  UDPolynomial(){}
  UDPolynomial(const UDPolynomial<coefficient>& other)
  { this->data=other.data;
  }
//  UDPolynomial<coefficient> operator+(const UDPolynomial<coefficient>& right) const;
  void operator+=(const UDPolynomial<coefficient>& right);
//  UDPolynomial<coefficient> operator-(const UDPolynomial<coefficient>& right) const;
  void operator-=(const UDPolynomial<coefficient>& right);
  UDPolynomial<coefficient> operator*(const UDPolynomial<coefficient>& right) const;
//  UDPolynomial<coefficient> operator*(const coefficient& right) const;
  void operator*=(const coefficient& right);
  void operator*=(const UDPolynomial<coefficient>& other)
  { *this=(*this)*other;
  }
  UDPolynomial<coefficient> TimesXn(int n) const;
// Quick divisibility test
// bool DivisibleBy(const UDPolynomial<coefficient>& divisor) const;
  struct DivisionResult<UDPolynomial<coefficient> > DivideBy(const UDPolynomial<coefficient>& right) const;
  UDPolynomial<coefficient> operator/(const UDPolynomial<coefficient>& divisor) const;
  UDPolynomial<coefficient> operator%(const UDPolynomial<coefficient>& divisor) const;
  void operator/=(const coefficient& right);
  void operator/=(const UDPolynomial<coefficient>& right)
  { *this=(*this/right);
  }
  coefficient operator()(const coefficient& x) const;
  void ClearDenominators();
  void FormalDerivative();
  void SquareFree();
  List<coefficient> GetRoots() const;
  void DoKronecker() const;
// static List<UDPolynomial<coefficient> > LagrangeInterpolants(List<coefficient> xs);
  coefficient& operator[](int i) const;
  bool operator<(const UDPolynomial<coefficient>& right) const;
  bool operator==(int other) const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  void AssignMinPoly(const Matrix<coefficient>& input);
  void AssignCharPoly(const Matrix<coefficient>& input); // method due to Urbain Le Verrier
};

template <typename coefficient>
void UDPolynomial<coefficient>::AssignMinPoly(const Matrix<coefficient>& input)
{ int n = input.NumCols;
  this->data.SetSize(1);
  this->data[0] = 1;
  for(int col = 0; col < n; col++)
  { VectorSpace<coefficient> vs;
    Vector<coefficient> v,w;
    v.MakeEi(n,col);
    vs.AddVectorToBasis(v);
    for(int i=0; i<n; i++)
    { w = input*v;
      if(!vs.AddVectorToBasis(w))
        break;
      v = w;
    }
    Vector<coefficient> p = vs.basis.PutInBasis(w);
    UDPolynomial<coefficient> out;
    out.data.SetSize(p.size+1);
    for(int i=0; i<p.size; i++)
      out.data[i] = -p[i];
    out.data[p.size] = 1;
    *this = MathRoutines::lcm (*this, out);
  }
}

template <typename coefficient>
void UDPolynomial<coefficient>::AssignCharPoly(const Matrix<coefficient>& input)
{ int n = input.NumCols;
  this->data.SetSize(n+1);
  this->data[0] = 1;
  Matrix<coefficient> acc;
  acc = input;
  for(int i=1; i<n; i++)
  { this->data[i] = -acc.GetTrace()/i;
    for(int j=0; j<n; j++)
      acc.elements[j][j] += this->data[i];
    acc.MultiplyOnTheLeft(input);
  }
  this->data[n] = -acc.GetTrace()/n;
}

template <typename coefficient>
coefficient& UDPolynomial<coefficient>::operator[](int i) const
{ return data[i];
}

template <typename coefficient>
coefficient UDPolynomial<coefficient>::operator()(const coefficient &x) const
{  coefficient acc = 0;
   coefficient y = 1;
   for(int i=0; i<data.size; i++)
   {  acc += y*data[i];
      y *= x;
   }
   return acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator+=(const UDPolynomial<coefficient>& right)
{  int t = min(right.data.size, data.size);
   for(int i=0; i<t; i++)
      data[i] += right.data[i];

   if(right.data.size > data.size)
   {  int n = data.size;
      data.SetSize(right.data.size);
      for(int i=n; i<right.data.size; i++)
         data[i] = right.data[i];
   }
   else
      while((data.size != 0) and (data[data.size-1] != 0))
         data.size--;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator-=(const UDPolynomial<coefficient>& right)
{  // int t = min(right.data.size, data.size); // wtf lol
  int t = right.data.size;
  if(data.size < t)
    t = data.size;

  for(int i=0; i<t; i++)
    data[i] -= right.data[i];

  if(right.data.size > data.size)
  {  int n = data.size;
     data.SetSize(right.data.size);
     for(int i=n; i<right.data.size; i++)
       data[i] = -right.data[i];
  }
  else
    while((data.size != 0) and (data[data.size-1] == 0))
      data.size--;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator*(const UDPolynomial<coefficient>& right) const
{  UDPolynomial<coefficient> out;
   out.data.SetSize(data.size+right.data.size-1);
   for(int i=0; i<out.data.size; i++)
      out.data[i] = 0;
   for(int i=0; i<data.size; i++)
      for(int j=0; j<right.data.size; j++)
         out.data[i+j] += data[i]*right.data[j];
   return out;
}

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::TimesXn(int n) const
{  UDPolynomial<coefficient> out;
   out.data.SetSize(data.size+n);
   for(int i=0; i<n; i++)
     out.data[i] = 0;
   // not memcpy()
   for(int i=0; i<data.size; i++)
      out.data[i+n] = data[i];

   return out;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator*=(const coefficient& right)
{  for(int i=0; i<data.size; i++)
      data[i] *= right;
}

template <class coefficient>
std::string UDPolynomial<coefficient>::ToString(FormatExpressions* theFormat)const
{ Polynomial<coefficient> tempP;
  tempP.MakeZero();
  MonomialP tempM;
  for (int i=0; i<this->data.size; i++)
  { tempM.MakeEi(0, i, 1);
    tempP.AddMonomial(tempM, this->data[i]);
  }
  return tempP.ToString(theFormat);
}

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const UDPolynomial<coefficient>& p)
{ FormatExpressions tempFormat;
  tempFormat.polyAlphabeT.SetSize(1);
  tempFormat.polyAlphabeT[0]="q";
  return out << p.ToString(&tempFormat);
}

#endif

