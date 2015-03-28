//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasIncluded
#define vpfHeaderSemisimpleLieAlgebrasIncluded

#include "vpfHeader2Math3_FiniteGroups.h"
static ProjectInformationInstance ProjectInfoVpfHeaderSemisimpleLieAlgebras(__FILE__, "Header, semisimple Lie algebras. ");

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
  //2.   [h_{\alpha},g^\beta] =\langle\alpha,\beta\rangle g^\beta
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
  std::string PhysicalNameMainOutputFolder;
  std::string DisplayNameMainOutputFolder;

  bool flagHasNilradicalOrder;
  bool flagDeallocated;
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const SemisimpleLieAlgebra& input)
  { return input.theWeyl.HashFunction();
  }
  void ComputeFolderNames(GlobalVariables& theGlobalVariables);
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
  { this->flagHasNilradicalOrder=false;
    this->flagDeallocated=false;
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
  void OrderNilradical(const Selection& parSelZeroMeansLeviPart, bool useNilWeight, bool ascending);
  void OrderNilradicalFirstTotalWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void OrderNilradicalFirstTotalWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void OrderNilradicalNilWeightAscending(const Selection& parSelZeroMeansLeviPart);
  void OrderNilradicalNilWeightDescending(const Selection& parSelZeroMeansLeviPart);
  void OrderStandardAscending();
  void OrderStandardDescending();

  void OrderSSalgebraForHWbfComputation();
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
  { //stOutput << "<br>num pos roots: " <<  this->GetNumPosRoots();
   // stOutput << " rank: "<< this->GetRank();
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
  bool AttemptExtendingHFtoHEFWRTSubalgebra
  (Vectors<Rational>& RootsWithCharacteristic2, Selection& theZeroCharacteristics, Vectors<Rational>& simpleBasisSA, Vector<Rational>& h,
   ElementSemisimpleLieAlgebra<Rational>& outputE, ElementSemisimpleLieAlgebra<Rational>& outputF, Matrix<Rational>& outputMatrixSystemToBeSolved,
   PolynomialSubstitution<Rational>& outputSystemToBeSolved, Matrix<Rational>& outputSystemColumnVector, GlobalVariables* theGlobalVariables);
  static void FindSl2Subalgebras
  (SemisimpleLieAlgebra& inputOwner, SltwoSubalgebras& output, GlobalVariables* theGlobalVariables);
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

template <class coefficient>
class Weight
{
public:
  SemisimpleLieAlgebra* owner;
  Vector<coefficient> weightFundamentalCoordS;
  static const bool IsMonEqualToZero()
  { return false;
  }
  friend std::ostream& operator << (std::ostream& output, const Weight<coefficient>& input)
  { output << input.ToString();
    return output;
  }
  Weight():owner(0){}
  void CheckNonZeroOwner()const
  { if (this->owner!=0)
      return;
    crash << "This is a programming error: Monomial char has zero owner, which is not allowed by the current function call. " << crash;
  }
  void AccountSingleWeight
  (const Vector<Rational>& currentWeightSimpleCoords, const Vector<Rational>& otherHighestWeightSimpleCoords,
   Rational& theMult, charSSAlgMod<coefficient>& outputAccum)const;
  std::string TensorAndDecompose(const Weight<coefficient>& other, charSSAlgMod<coefficient>& output, GlobalVariables& theGlobalVariables)const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  inline unsigned int HashFunction()const
  { return weightFundamentalCoordS.HashFunction();
  }
  static inline unsigned int HashFunction(const Weight<coefficient>& input)
  { return input.HashFunction();
  }
  void operator+=(const Weight<coefficient>& other)
  { if (this->owner!=other.owner)
      crash << "Attempting to add weights with different owner algebras. " << crash;
    this->weightFundamentalCoordS+=other.weightFundamentalCoordS;
  }
  void operator*=(const coefficient& other)
  { this->weightFundamentalCoordS*=other;
  }
  inline bool operator==(const Weight<coefficient>& other) const
  { return this->weightFundamentalCoordS==other.weightFundamentalCoordS && this->owner==other.owner;
  }
  inline bool operator>(const Weight<coefficient>& other) const
  { return this->weightFundamentalCoordS>other.weightFundamentalCoordS;
  }
};

template <class coefficient>
class charSSAlgMod : public MonomialCollection<Weight<coefficient>, coefficient>
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
  { return input.::MonomialCollection<Weight<coefficient>, coefficient>::HashFunction(input);
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
   SubgroupWeylGroupOLD& outputWeylSub, GlobalVariables& theGlobalVariables);
  int GetIndexExtremeWeightRelativeToWeyl(WeylGroup& theWeyl)const;
  void MakeTrivial(SemisimpleLieAlgebra& inputOwner);
  std::string MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables);
  std::string operator*=(const charSSAlgMod& other);
  std::string operator*=(const Weight<Rational>& other);
  void operator*=(const coefficient& other)
  { this->::MonomialCollection<Weight<coefficient>, coefficient>::operator*=(other);
  }
};

class HomomorphismSemisimpleLieAlgebra
{
public:
  SemisimpleLieAlgebra* domainAlg;
  SemisimpleLieAlgebra* rangeAlg;
  //Let rk=Rank(Domain)
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

#endif
