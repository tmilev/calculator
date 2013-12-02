//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderLieTheoryMiscellaneous
#define vpfHeaderLieTheoryMiscellaneous

#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
static ProjectInformationInstance ProjectInfoVpfHeaderLieTheoryMiscellaneous(__FILE__, "Header, Lie theory, miscellaneous. ");

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
  SubgroupWeylGroupOLD WeylFD;
  SubgroupWeylGroupOLD WeylFDSmallAsSubInLarge;
  SubgroupWeylGroupOLD WeylFDSmall;
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

#endif
