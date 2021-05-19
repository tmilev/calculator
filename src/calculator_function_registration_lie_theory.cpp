// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_lie_theory.h"
#include "calculator_weyl_group_characters.h"
#include "calculator_inner_functions.h"
#include "calculator_functions_polynomial.h"
#include "calculator_inner_typed_functions.h"

void Calculator::initializeFunctionsSemisimpleLieAlgebras() {
  MacroRegisterFunctionWithName("Calculator::initializeFunctionsSemisimpleLieAlgebras");
  Function::Options innerStandard = Function::Options::standard();
  Function::Options innerNoTest = Function::Options::innerNoTest();
  Function::Options innerInvisible = Function::Options::innerInvisible();
  Function::Options innerAdminNoTest = Function::Options::adminNoTest();
  Function::Options innerInvisibleExperimental = Function::Options::innerInvisibleExperimental();
  Function::Options innerAdminNoTestExperimental = Function::Options::innerAdminNoTestExperimental();
  Function::Options innerExperimental = Function::Options::experimental();
  Function::Options adminNoTestInvisibleOffByDefault = Function::Options::adminNoTestInvisibleOffByDefault();
  Function::Options adminNoTestInvisibleExperimental = Function::Options::adminNoTestInvisibleExperimental();
  Function::Options innerInvisibleNoTest = Function::Options::invisibleNoTest();
  Function::Options innerExperimentalNoTest = Function::Options::innerNoTestExperimental();

  this->addOperationHandler(
    "SemisimpleLieAlgebra",
    CalculatorConversions::innerSemisimpleLieAlgebra,
    "",
    "Creates a semisimple Lie algebra. The semisimple Lie algebra "
    "is given via its Dynkin type. A simple Dynkin type is given by "
    "type letter with upper index equal to the "
    "inverse of the scale of the symmetric Cartan "
    "matrix and lower index equal to "
    "the rank of the subalgebra. "
    "For example A^2_3 stands for type "
    "A_3 (sl (4)) with symmetric "
    "Cartan matrix scale equal to 1/2. "
    "If the upper index is omitted, "
    "it is implied to be 1.<hr> "
    "We define the symmetric Cartan matrix "
    "as the matrix whose (i, j)^{th} "
    "entry is the scalar product of the "
    "i^{th} and j^{th} roots. "
    "We assume the roots to be ordered in "
    "the order implied by the "
    "(non-symmetric) Cartan matrices on "
    "page 59 in Humphreys, "
    "Introduction to Lie algebras and representation theory. "
    "If the upper index of the Dynkin type is 1, "
    "the corresponding symmetric Cartan matrix is "
    "assigned the default value. "
    "The default values are chosen "
    "so that the long root has squared "
    "length 2 in types A_n, B_n, D_n, E_6, E_7, E_8, "
    "and F_4, squared length 4 in C_n, "
    "and squared length 6 in type G_2. "
    "<br>If upper index is not equal to 1, "
    "the symmetric "
    "Cartan matrix is given by dividing "
    "the default symmetric "
    "Cartan matrix by the upper index. ",
    "g_{{i}}= GetChevalleyGenerator{}(SemisimpleLieAlgebra{}G_2, i);\n"
    "h_{{i}}= GetCartanGenerator{}(SemisimpleLieAlgebra{}G_2, i);\n"
    "[g_1,g_{- 1}]; \n[g_2, g_{-2}]; \n[h_{1}, g_6]; \n[h_2, g_{-6}]",
    "CalculatorConversions::innerSemisimpleLieAlgebra",
    "SemisimpleLieAlgebra",
    innerStandard
  );
  this->addOperationHandler(
    "WriteSemisimpleLieAlgebra",
    CalculatorLieTheory::writeSemisimpleLieAlgebraToHardDisk,
    "",
    "Writes semisimple Lie algebra structure constants to "
    "the output folder of the calculator. Available to logged-in admins only. ",
    "WriteSemisimpleLieAlgebra(F_4)",
    "Calculator::writeToHardDiskOrPrintSemisimpleLieAlgebra",
    "WriteSemisimpleLieAlgebra",
    innerStandard
  );

  this->addOperationHandler(
    "PrintSemisimpleLieAlgebra",
    CalculatorLieTheory::printSemisimpleLieAlgebraVerbose,
    "",
    "Creates a printout with information about the "
    "semisimple Lie algebra, including "
    "the Lie bracket pairing table. "
    "In addition, this function creates "
    "a graphics of the root system. ",
    "PrintSemisimpleLieAlgebra{}(F_4);\n"
    "PrintSemisimpleLieAlgebra{}(2 G^5_2 + B_3);",
    "Calculator::printSemisimpleLieAlgebraVerbose",
    "PrintSemisimpleLieAlgebra",
    innerStandard
  );
  this->addOperationHandler(
    "PrecomputeSemisimpleLieAlgebraStructure",
    CalculatorFunctions::precomputeSemisimpleLieAlgebraStructure,
    "",
    "Function available to logged-in admins only. "
    "Precomputes all built-in semisimple Lie algebra information. "
    "Argument gives a starting point (0 or negative to start at the beginning). "
    "Turn process monitoring on when using this function. ",
    "PrecomputeSemisimpleLieAlgebraStructure 0",
    "CalculatorFunctions::precomputeSemisimpleLieAlgebraStructure",
    "PrecomputeSemisimpleLieAlgebraStructure",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "GetChevalleyGenerator",
    CalculatorLieTheory::chevalleyGenerator,
    "",
    "First argument must be a semisimple "
    "Lie algebra, second argument must "
    "be an integer from -N to N, "
    "where N is the number of positive "
    "roots of the Lie algebra. "
    "The function returns the "
    "Chevalley-Weyl generator labeled by the root "
    "corresponding to the integer index. "
    "The roots are indexed in the "
    "ordered displayed by the "
    "PrintSemisimpleLieAlgebra function. ",
    "[GetChevalleyGenerator{}(G_2, 6), GetChevalleyGenerator{}(G_2, -6)]",
    "Calculator::chevalleyGenerator",
    "GetChevalleyGenerator",
    innerStandard
  );
  this->addOperationHandler(
    "GetCartanGenerator",
    CalculatorLieTheory::cartanGenerator,
    "",
    "First argument must be a semisimple Lie algebra, "
    "second argument must "
    "be a number between 1 and K, where K is the rank "
    "of the Lie algebra. "
    "In this case the function returns the element of "
    "the Cartan subalgebra that is dual "
    "to the simple root with the same index. "
    "Note that this element of the Cartan subalgebra "
    "is proportional to a Chevalley-Weyl generator "
    "with a coefficient of proportionality "
    "equal to 2/(simple root length squared) ).",
    "GetCartanGenerator{}(G_2, 1)",
    "Calculator::cartanGenerator",
    "GetCartanGenerator",
    innerStandard
  );
  this->addOperationHandler(
    "AdCommonEigenspace",
    CalculatorFunctions::innerAdCommonEigenSpaces,
    "",
    "Computes common eigenspace of the adjoint action of semisimple Lie "
    "algebra elements inside the semisimple Lie algebra. ",
    "AdCommonEigenspace{}(F_4, -5 (g_{9}) +3 (g_{13}) +5 (g_{16}) +4 (g_{10}), g_{14}+g_{22}+g_{20})",
    "Calculator::innerAdCommonEigenSpaces",
    "AdCommonEigenspace",
    innerStandard
  );
  this->addOperationHandler(
    "AttemptExtendingEtoHEFwithHinCartan",
    CalculatorFunctions::innerAttemptExtendingEtoHEFwithHinCartan,
    "",
    "Attempts to embed an element E into an sl(2)-triple over "
    "the rationals, such that the element H is in the "
    "ambient Cartan algebra. ",
    "AttemptExtendingEtoHEFwithHinCartan{}(F_4, g_1+2g_2+3g_3+4g_4)",
    "Calculator::innerAttemptExtendingEtoHEFwithHinCartan",
    "AttemptExtendingEtoHEFwithHinCartan",
    innerStandard
  );
  this->addOperationHandler(
    "PrintMacdonaldPolys",
    CalculatorFunctionsWeylGroup::macdonaldPolys,
    "",
    "Prints macdonald polynomials from a semisimple type. ",
    "PrintMacdonaldPolys{}(B_3)",
    "CalculatorFunctionsWeylGroup::macdonaldPolys",
    "PrintMacdonaldPolys",
    innerStandard
  );
  this->addOperationHandler(
    "CharPoly",
    CalculatorFunctionsLinearAlgebra::characteristicPolynomialMatrix,
    "",
    "Computes the characteristic polynomial of a matrix (= det(A-q*Id)), "
    "provided that the matrix is not too large.",
    "A =\\begin{pmatrix}2 & 3& 5\\\\ 7& 11& 13\\\\ 17&19 &23\\end{pmatrix}; p =MinPolyMatrix{}A",
    "CalculatorFunctions::innerCharPolyMatrix",
    "CharPoly",
    innerStandard
  );
  this->addOperationHandler(
    "MakeCharacterLieAlg",
    CalculatorLieTheory::characterSemisimpleLieAlgebraFiniteDimensional,
    "",
    "Creates character of a semisimple Lie algebra finite dimensional irreducible module. "
    "First argument gives type, second argument gives highest weight in fundamental coordinates.",
    "x = MakeCharacterLieAlg{}(G_2, (1, 0));\n"
    "y = MakeCharacterLieAlg{}(G_2, (0, 1));\n"
    "x * y",
    "Calculator::characterSemisimpleLieAlgebraFiniteDimensional",
    "MakeCharacterLieAlg",
    innerStandard
  );
  this->addOperationHandler(
    "GetLinksToSimpleLieAlgebraPrintouts",
    CalculatorLieTheory::getLinksToSimpleLieAlgebras,
    "",
    "Gets simple Lie algebra links to the calculator.",
    "GetLinksToSimpleLieAlgebraPrintouts{}0",
    "Calculator::getLinksToSimpleLieAlgebras",
    "GetLinksToSimpleLieAlgebraPrintouts",
    innerInvisible
  );
  this->addOperationHandler(
    "SlTwoRealFormStructure",
    CalculatorLieTheory::slTwoRealFormStructureComputeOnDemand,
    "",
    "Computes the Lie algebra structure over Kostant-Sekiguchi sl twos "
    "w.r.t. maximally compact real form.",
    "SlTwoRealFormStructure{}(a_3)",
    "Calculator::slTwoRealFormStructureComputeOnDemand",
    "SlTwoRealFormStructure",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "SlTwoRealFormStructureForceRecompute",
    CalculatorLieTheory::slTwoRealFormStructureForceRecompute,
    "",
    "Same as SlTwoRealFormStructure but forces a recompute. ",
    "SlTwoRealFormStructureForceRecompute{}(a_3)",
    "Calculator::slTwoRealFormStructureForceRecompute",
    "SlTwoRealFormStructureForceRecompute",
    innerInvisible
  );
  this->addOperationHandler(
    "ConesIntersection",
    CalculatorFunctions::innerConesIntersect,
    "",
    "Takes as input two sequences of vectors, generates two cones, "
    "and intersects them using the simplex algorithm. "
    "The output is a string report of the operation. "
    "The first cone is generated over Z_{&gt;} (``strict cone'') "
    "the second cone is generated over Z_{&gt;= 0} (``non-strict cone'').",
    "v_1 = (1, 2, 3);\n"
    "v_2 = (1, 3, 2);\n"
    "v_3 = (3, 1, 1);\n"
    "v_4 = (- 2, 2, 2);\n"
    "ConesIntersection{}((v_1, v_2), (v_3, v_4 ));\n"
    "ConesIntersection{}((v_1, v_2), (v_3, - v_4));",
    "Calculator::innerConesIntersect",
    "ConesIntersection",
    innerStandard
  );

  this->addOperationHandler(
    "HeighestWeightVector",
    CalculatorLieTheory::highestWeightVector,
    "",
    "Highest weight vector in a generalized Verma module. "
    "The first argument gives the semisimple Lie algebra. "
    "The second argument "
    "gives the highest weight in fundamental coordinates. "
    "The third argument parametrizes the parabolic subalgebra, e.g. "
    "(1,0,0) stands for a "
    "parabolic subalgebra with first simple root crossed-out. "
    "The second argument is allowed to have "
    "entries that are not non-negative integers in the "
    "positions in which the third argument has 1's. ",
    "g_{{i}}= GetChevalleyGenerator{}(B_3, i);\n"
    "h_{{i}}= GetCartanGenerator{}(B_3, i);\n"
    "v_\\mu=HeighestWeightVector{} (A_3, (1,0,1),(0,0,0));\n"
    "v_\\lambda =HeighestWeightVector{}(B_3, (x_1,0,1),(1,0,0));\n"
    "h_1g_{- 1}v_\\lambda",
    "CalculatorFunctions::highestWeightVector",
    "HeighestWeightVector",
    innerStandard
  );
  this->addOperationHandler(
    "PrintModule",
    CalculatorLieTheory::printGeneralizedVermaModule,
    "",
    "Makes a report on a finite dimensional Lie algebra module, "
    "or more generally, on a generalized Verma module "
    "(irreducible finite dimensional semisimple Lie algebra "
    "modules are a partial case of generalized Verma modules). "
    "The first argument gives the semisimple Lie algebra. "
    "The second argument gives the highest weight "
    "in fundamental coordinates. The third argument "
    "parametrizes the parabolic subalgebra, e.g. "
    "(1,0) stands for a parabolic subalgebra "
    "(lying in algebra of rank 2) "
    "with first simple root crossed-out. "
    "The second argument is allowed to have "
    "entries that are not non-negative integers "
    "in the positions in which the third argument has 1's. ",
    "PrintModule{}(G_2, (2, 0), (0, 0))",
    "CalculatorFunctions::printGeneralizedVermaModule",
    "PrintModule",
    innerStandard
  );
  this->addOperationHandler(
    "HighestWeightTAAbf",
    CalculatorFunctions::innerHighestWeightTransposeAntiAutomorphismBilinearForm,
    "",
    "Highest weight transpose anti-automorphism bilinear form, a.k.a. "
    "Shapovalov form. Let M be a Verma module "
    "with highest weight vector v given in "
    "fundamental coordinates. Let P:M->M "
    "be a projection map onto Cv that maps "
    "every weight vector of M of weight "
    "different from the highest to 0. "
    "Let u_1, u_2 be two words in the "
    "universal enveloping algebra. "
    "Then define HighestWeightTAAbf(u_1,u_2)= Tr_M (P ( taa(u_1) u_2 ), "
    "where taa() is the transpose anti-automorphism of g. ",
    "g_{{a}}= GetChevalleyGenerator{} (G_2, a);\nHighestWeightTAAbf{}(g_{- 1} g_{-2}, g_{- 1}g_{-2}, (2,2))",
    "CalculatorFunctions::innerHWTAABF",
    "HighestWeightTAAbf",
    innerStandard
  );
  this->addOperationHandler(
    "WeylDimFormula",
    CalculatorLieTheory::weylDimFormula,
    "",
    "Weyl dimension formula. First argument gives "
    "the type of the Weyl group of the simple "
    "Lie algebra in the form Type_Rank (e.g. E_6). "
    "The second argument gives the highest weight in fundamental coordinates. ",
    "WeylDimFormula{}(G_2, (x,0));\nWeylDimFormula{}(B_3, (x,0,0));",
    "CalculatorFunctions::weylDimFormula",
    "WeylDimFormula",
    innerStandard
  );
  this->addOperationHandler(
    "AnimateLittelmannPaths",
    CalculatorLieTheory::animateLittelmannPaths,
    "",
    "Generates all Littelmann-Lakshmibai-Seshadri paths, draws them and animates them. "
    "Presented first on the seminar in Charles University Prague. "
    "The first argument gives the type of the semisimple Lie algebra, "
    "the second gives the highest weight. ",
    "AnimateLittelmannPaths{}(G_2, (2, 0));",
    "CalculatorLieTheory::animateLittelmannPaths",
    "AnimateLittelmannPaths",
    innerStandard
  );
  this->addOperationHandler(
    "DecomposeInducingRepGenVermaModule",
    CalculatorLieTheory::decomposeFDPartGeneralizedVermaModuleOverLeviPart,
    "",
    "Decomposes the inducing module of a generalized "
    "Verma module over the Levi part of a parabolic "
    "smaller than the inducing one. "
    "The first argument gives the type of the algebra. "
    "The second argument gives the highest "
    "weight of the module in "
    "fundamental coordinates. "
    "The third argument gives "
    "the parabolic subalgebra with respect "
    "to which we induce. The last argument "
    "gives the parabolic subalgebra with "
    "respect to whose Levi part we decompose.",
    "DecomposeInducingRepGenVermaModule{}(B_3, (0, 1, 1), (1, 0, 0), (1, 0, 1))",
    "CalculatorFunctions::decomposeFDPartGeneralizedVermaModuleOverLeviPart",
    "DecomposeInducingRepGenVermaModule",
    innerStandard
  );
  this->addOperationHandler(
    "Casimir",
    CalculatorLieTheory::casimir,
    "",
    "Gives the Casimir element. ",
    "Casimir{}(G_2)",
    "Calculator::casimir",
    "Casimir",
    innerStandard
  );
  this->addOperationHandler(
    "CasimirWRTLevi",
    CalculatorLieTheory::casimirWithRespectToLevi,
    "",
    "Gives the Casimir element of a Levi part of "
    "a parabolic subalgebra. First argument = ambient Lie algebra. "
    "Second argument = parabolic selection = 1 "
    "stands for simple root outside of the Levi "
    "component, 0 stands for simple root of the Levi component.",
    "CasimirWRTLevi{}(B_3, (1,0,0))",
    "CalculatorFunctions::casimirWithRespectToLevi",
    "CasimirWRTLevi",
    innerStandard
  );
  this->addOperationHandler(
    "HmmG2inB3",
    CalculatorLieTheory::embedG2InB3,
    "",
    "Embeds elements of the Universal enveloping "
    "of G_2 in B_3, following an embedding found "
    "in a paper by McGovern.",
    "g_{{a}}= GetChevalleyGenerator{} (G_2, a); "
    "HmmG2inB3{}(g_1);\nHmmG2inB3{}(g_2) ",
    "Calculator::embedG2InB3",
    "HmmG2inB3",
    innerStandard
  );
  this->addOperationHandler(
    "DrawRootSystem",
    CalculatorLieTheory::drawRootSystem,
    "",
    "Draws the root system of a semisimple Lie algebra. "
    "Takes one or three arguments: "
    "if one argument is provided, that must "
    "be a semisimple Lie algebra or "
    "a semisimple Lie algebra type; "
    "if three arguments are provided, "
    "the first must be a semisimple "
    "Lie algebra or a semisimple Lie algebra type, "
    "and the second and third argument must be two "
    "rational vectors of dimension "
    "equal to the rank of the semisimple "
    "Lie algebra. The root system is drawn in a "
    "Coxeter plane. If the extra two vectors "
    "are given, they are used to initialize "
    "a preferred projection plane "
    "in the change-to-basis entries "
    "below the graphics. "
    "Clicking the obvious button creates a basis "
    "change animations ideal for presentations.",
    "DrawRootSystem{}(A_7, (1, 0 , 2, 2, 2, 0, 1), (1, 3, 2, 2, 2, 3, 1))",
    "CalculatorFunctions::drawRootSystem",
    "DrawRootSystem",
    innerStandard
  );
  this->addOperationHandler(
    "DrawWeightSupportWithMults",
    CalculatorLieTheory::drawWeightSupportWithMults,
    "",
    "Draws the weight support of an irreducible "
    "finite-dimensional highest weight module. "
    "The first argument gives the type and the second "
    "gives the highest weight given in "
    "fundamental weight basis. The maximum number "
    "of drawn weights allowed is (hard-code) "
    "limited to 10 000. "
    "If the number of weights in the weight "
    "support exceeds this number, only "
    "the first 10 000 weights will be drawn. "
    "<b>Warning. Drawing text (i.e. multiplicities) "
    "is very javascript-processor-intensive. "
    "Use only for *small* examples, "
    "else you might hang your browser. </b>",
    "DrawWeightSupportWithMults{}(B_3, (0, 1, 1));\n"
    "DrawWeightSupportWithMults{}(G_2, (1, 0))",
    "CalculatorFunctions::drawWeightSupportWithMults",
    "DrawWeightSupportWithMults",
    innerStandard
  );
  this->addOperationHandler(
    "DrawWeightSupport",
    CalculatorLieTheory::drawWeightSupport,
    "",
    "Same as DrawWeightSupportWithMults but displays no multiplicities. "
    "Same warning for hanging up your browser "
    "with javascript holds.",
    "DrawWeightSupport{}(B_3, (1, 1, 1));\n"
    "DrawWeightSupport{}(G_2, (1, 2))",
    "CalculatorFunctions::drawWeightSupport",
    "DrawWeightSupport",
    innerStandard
  );
  this->addOperationHandler(
    "SplitFDpartB3overG2CharsOnly",
    CalculatorLieTheory::splitFDpartB3overG2CharsOnly,
    "",
    "Splits the finite dimensional part of the "
    "inducing module of the generalized "
    "Verma module of B_3(so(7)) into G_2-components. "
    "The argument is gives the highest weight of the "
    "generalized Verma module in fundamental "
    "coordinates with respect to so(7). "
    "The arguments which are not small integers "
    "indicate the non-selected "
    "roots of the inducing parabolic subalgebra of B_3. ",
    "SplitFDpartB3overG2CharsOnly(x_1, 2, 0)",
    "CalculatorLieTheory::splitFDpartB3overG2CharsOnly",
    "SplitFDpartB3overG2CharsOnly",
    innerStandard
  );
  this->addOperationHandler(
    "SplitFDpartB3overG2",
    CalculatorLieTheory::splitFDpartB3overG2,
    "",
    "Splits the finite dimensional part of the inducing "
    "module of the generalized Verma module of "
    "B_3(so(7)) into G_2-components. "
    "The argument is gives the highest weight of the "
    "generalized Verma module in fundamental "
    "coordinates with respect to so(7). "
    "The arguments which are not small integers "
    "indicate the non-selected roots of the inducing "
    "parabolic subalgebra of B_3. ",
    "SplitFDpartB3overG2{}(x_1, 1, 0)",
    "CalculatorFunctions::splitFDpartB3overG2",
    "SplitFDpartB3overG2",
    innerInvisibleExperimental
  );
  this->addOperationHandler(
    "PrintB3G2branchingTableCharsOnly",
    CalculatorLieTheory::printB3G2branchingTableCharsOnly,
    "",
    "Creates a table of branching of finite dimensional B_3-modules over G_2. "
    "The argument of the function gives the "
    "maximum height of the B_3-weight. "
    "The second argument indicates the parabolic subalgebra of "
    "B_3- zero entry stands for the corresponding "
    "root space lying in the Levi part of the parabolic. "
    "Non-zero entry means the corresponding "
    "negative root space is not in the parabolic. "
    "The expression given in that coordinate "
    "is used as the corresponding highest weight. ",
    "PrintB3G2branchingTableCharsOnly{}(2, (0,0,0)); "
    "PrintB3G2branchingTableCharsOnly{}(2, (x_1,0,0))",
    "CalculatorLieTheory::printB3G2branchingTableCharsOnly",
    "PrintB3G2branchingTableCharsOnly",
    innerInvisibleExperimental
  );
  this->addOperationHandler(
    "PrintB3G2branchingTable",
    CalculatorLieTheory::printB3G2branchingTable,
    "",
    "Creates a table of branching of finite dimensional B_3-modules over G_2. "
    "The argument of the function gives the maximum height "
    "of the B_3-weight. The function works with arguments 0 or 1; "
    "values of 2 or more must be run off-line.",
    "PrintB3G2branchingTable{}(1, (0,0,0)); "
    "PrintB3G2branchingTable{}(1, (x_1,0,0))",
    "Calculator::printB3G2branchingTable",
    "PrintB3G2branchingTable",
    innerInvisibleExperimental
  );
  this->addOperationHandler(
    "SplitFDTensorGenericGeneralizedVerma",
    CalculatorLieTheory::splitGenericGeneralizedVermaTensorFiniteDimensional,
    "",
    "Experimental, please don't use. Splits generic generalized "
    "Verma module tensor finite dimensional module. ",
    "SplitFDTensorGenericGeneralizedVerma{}(G_2, (1, 0), (x_1, x_2)); ",
    "CalculatorFunctions::splitGenericGeneralizedVermaTensorFiniteDimensional",
    "SplitFDTensorGenericGeneralizedVerma",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "WriteGenVermaAsDiffOperatorsUpToWeightLevel",
    CalculatorLieTheory::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel,
    "",
    "<b>Experimental, please don't use</b>. "
    "Embeds a Lie algebra in the Weyl algebra "
    "with matrix coefficients. The third argument "
    "gives the highest weight. The non-zero entries "
    "of the highest weight give the root spaces "
    "outside of the Levi part of the parabolic. "
    "The second argument gives the weight "
    "level to which the computation should be carried out",
    "WriteGenVermaAsDiffOperatorsUpToWeightLevel{}(B_3, 1, (0, 0, y)); ",
    "CalculatorFunctions::writeGeneralizedVermaModuleAsDifferentialOperatorUpToLevel",
    "WriteGenVermaAsDiffOperatorsUpToWeightLevel",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder",
    CalculatorLieTheory::writeGenVermaModAsDiffOperatorsGeneratorOrder,
    "",
    "Constructs a parabolically induced map from a "
    "semisimple Lie algebra to a Weyl algebra "
    "with matrix coefficients. "
    "More precisely, this function constructs a "
    "generalized Verma module and writes it using "
    "differential operators with matrix coefficients. "
    "The first argument gives the type of the "
    "semisimple Lie algebra, the second argument "
    "gives the highest weight in fundamental coordinates. "
    "The third argument gives the parabolic subalgebra "
    "selection with 0 standing for non-crossed "
    "out roots and 1 for crossed-out roots. "
    "In all non crossed-out root positions, "
    "the coordinates of the highest weight "
    "vector must be small integers (because the highest "
    "weight must be dominant and integral with respect "
    "to the Levi part of the inducing parabolic subalgebra). "
    "In the crossed-out root positions, the coordinates "
    "of the highest weight vector can be "
    "arbitrary polynomial expressions. "
    "The algorithm depends on an order chosen on "
    "the Chevalley-Weyl generators of the nilradical. "
    "Here, we sort the nilradical elements in descending "
    "order relative to the order of their roots. "
    "In addition to the first 3 arguments, this function "
    "accepts further optional arguments, "
    "customizing the notation of the final printout.",
    "MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder{}(B_3, (0,0,0), (0, 0, 1), x, \\partial, a); ",
    "CalculatorLieTheory::writeGenVermaModAsDiffOperatorsGeneratorOrder",
    "MapSemisimpleLieAlgebraInWeylAlgebraGeneratorOrder",
    innerStandard
  );
  this->addOperationHandler(
    "MapSemisimpleLieAlgebraInWeylAlgebra",
    CalculatorLieTheory::writeGenVermaModAsDiffOperatorsNilOrderDescending,
    "",
    "Same as MapSemisimpleLieAlgebraInWeylAlgebra, "
    "but with a different order on the elements "
    "of the nilradical. We sort the elements "
    "of the nilradical as follows. "
    "Consider the partial order given "
    "by the graded lex order on the coordinates that "
    "correspond to crossed-out roots. "
    "Then extend this partial order "
    "by ``breaking the ties'' via the graded "
    "lexicographic order on the non-crossed out roots. "
    "This extension we call "
    "the nil-order. Now this function sorts the "
    "elements of the nilradical in descending nil-order, "
    "that is, elements with higher nil-order come first.",
    "MapSemisimpleLieAlgebraInWeylAlgebra{}(B_3, (0, 0, 0), (0, 0, 1), x, \\partial, a); ",
    "Calculator::writeGenVermaModAsDiffOperatorsNilOrderDescending",
    "MapSemisimpleLieAlgebraInWeylAlgebra",
    innerStandard
  );
  this->addOperationHandler(
    "MapSemisimpleLieAlgebraInWeylAlgebraAllGens",
    CalculatorLieTheory::writeGenVermaModAsDiffOperatorsAllGensNilOrderDescending,
    "",
    "Constructs a parabolically induced map from "
    "a semisimple Lie algebra "
    "to a Weyl algebra with matrix coefficients. "
    "More precisely, this function constructs a "
    "generalized Verma module and writes it using "
    "differential operators with matrix coefficients. "
    "The first argument gives the type of the "
    "semisimple Lie algebra, the second argument "
    "gives the highest weight in fundamental coordinates. "
    "The third argument gives the parabolic "
    "subalgebra selection with 0 standing for "
    "non-crossed out roots and 1 for crossed-out roots. "
    "In all non crossed-out root positions, "
    "the coordinates of the highest weight "
    "vector must be small integers (because the highest "
    "weight must be dominant and integral with "
    "respect to the Levi part of the inducing parabolic subalgebra). "
    "In the crossed-out root positions, the "
    "coordinates of the highest weight vector can be "
    "arbitrary polynomial expressions. "
    "In addition to the first 3 arguments, this function "
    "accepts further optional arguments, customizing the "
    "notation of the final printout. The differential "
    "operator part of the algorithm uses the "
    "nil-order given by the function "
    "MapSemisimpleLieAlgebraInWeylAlgebra.",
    "MapSemisimpleLieAlgebraInWeylAlgebraAllGens{}(B_3, (0,0,0), (0, 0, 1), x, \\partial, a); ",
    "Calculator::writeGenVermaModAsDiffOperatorsAllGensNilOrderDescending",
    "MapSemisimpleLieAlgebraInWeylAlgebraAllGens",
    innerStandard
  );
  this->addOperationHandler(
    "KLcoeffs",
    CalculatorLieTheory::kazhdanLuzstigCoeffificents,
    "",
    "Computes the n by n tables of "
    "1) Kazhdan-Lusztig polynomials, "
    "2) R polynomials and "
    "3) Kazhdan-Lusztig polynomials evaluated at one, where n<=192 "
    "is the size of the Weyl group (i.e. no larger than D_4(so(8)). "
    "The notation follows the original paper by Kazhdan and Lusztig, "
    "\"Representations of Coxeter Groups and Hecke Algebras\". "
    "The algorithm is directly derived from formulas (2.0a-c) there, "
    "as explained in the Wikipedia page on Kazhdan-Lusztig polynomials. "
    "Please note that the 192 by 192 element table takes "
    "almost 3 minutes to compute. "
    "Faster implementations of the KL polynomials are "
    "available from programs by Fokko du Cloux and others "
    "(our simple implementation stores the full table "
    "of R-polynomials and KL-polynomials in RAM memory at "
    "all times, unlike the other more efficient implementations).",
    "KLcoeffs{}(B_3)",
    "Calculator::kazhdanLuzstigCoeffificents",
    "KLcoeffs",
    innerStandard
  );
  this->addOperationHandler(
    "RootSubsystem",
    CalculatorLieTheory::rootSubsystem,
    "",
    "Generates a root subsystem of a simple type. "
    "First argument indicates simple type, second, third, ... arguments "
    "give the generating roots. ",
    "RootSubsystem(F_4, (0, 1, 0, 0), (0, 0, 1, 0), (1, 1, 2, 2))",
    "Calculator::rootSubsystem",
    "RootSubsystem",
    innerStandard
  );
  this->addOperationHandler(
    "PrintRootSubalgebras",
    CalculatorLieTheory::printRootSAs,
    "",
    "Prints sl(2) subalgebras and root subalgebras. "
    "The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
    "PrintRootSubalgebras(E_6)",
    "CalculatorFunctions::printRootSAs",
    "PrintRootSubalgebras",
    innerStandard
  );
  this->addOperationHandler(
    "PrintRootSubalgebrasRecompute",
    CalculatorLieTheory::printRootSubalgebrasForceRecompute,
    "",
    "Same as printRootSubalgebras but forces recomputation. "
    "Use to recompute obsolete or interrupted output files.",
    "PrintRootSubalgebrasRecompute(b_3)",
    "CalculatorFunctions::printRootSAsForceRecompute",
    "PrintRootSubalgebrasRecompute",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "PrintSlTwoSubalgebras",
    CalculatorLieTheory::printSlTwos,
    "",
    "Prints sl(2) subalgebras and root subalgebras. "
    "The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
    "PrintSlTwoSubalgebras(g_2)",
    "CalculatorFunctions::printSltwos",
    "PrintSlTwoSubalgebras",
    innerStandard
  );
  this->addOperationHandler(
    "ParabolicsInfoBruhatGraph",
    CalculatorLieTheory::parabolicWeylGroupsBruhatGraph,
    "",
    "Makes a table with information about the Weyl group of a "
    "parabolic subalgebra of the ambient Lie algebra, "
    "as well as the cosets "
    "(given by minimal coset representatives) of the Weyl subgroup "
    "in question. "
    "The input must have as many integers as there "
    "are simple roots in the ambient Lie algebra. "
    "If the root is crossed out "
    "(i.e. not a root space of the Levi part), "
    "one should put a 1 in the corresponding coordinate. "
    "Otherwise, one should put 0. "
    "For example, for Lie algebra B3(so(7)), calling "
    "parabolicsInfoBruhatGraph(0, 0, 0) gives you the "
    "Weyl group info for the entire algebra; "
    "calling parabolicsInfoBruhatGraph(1,0,0) gives you "
    "info for the Weyl subgroup generated by "
    "the last two simple root. "
    "In the produced graph, the element s_{\\eta_i} "
    "corresponds to a reflection with respect "
    "to the i^th simple root. You will get your "
    "output as a .png file link, you must click onto "
    "the link to see the end result. "
    "<b>Please do not use for subalgebras "
    "larger than B_4 (so(9)). The vpf program has "
    "no problem handling this function up to "
    "E_6 but LaTeX crashes trying to process the output. </b>",
    "parabolicsInfoBruhatGraph{}(B_3,(1,0,0),(1,0,0))",
    "CalculatorFunctions::parabolicWeylGroupsBruhatGraph",
    "ParabolicsInfoBruhatGraph",
    innerStandard
  );
  this->addOperationHandler(
    "TestMonomialBasisConjecture",
    CalculatorLieTheory::testMonomialBaseConjecture,
    "",
    "Tests the monomial basis conjecture from a common work by Jackson and Milev paper. "
    "First argument gives rank bound. "
    "Second argument gives dimension bound. ",
    "TestMonomialBasisConjecture{}(2, 50)",
    "Calculator::testMonomialBaseConjecture",
    "TestMonomialBasisConjecture",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "LSpath",
    CalculatorLieTheory::LSPath,
    "",
    "Lakshmibai-Seshadri path starting at 0. "
    "The first argument gives the semisimple Lie algebra, "
    "the next arguments give the way-points of the path.",
    "LSpath{}(G_2, (0,0), (2,1) )",
    "Calculator::LSPath",
    "LSpath",
    innerStandard
  );
  this->addOperationHandler(
    "LROdefine",
    CalculatorLieTheory::littelmannOperator,
    "",
    "Littelmann root operator e_i, where e_i is the Littelmann "
    "root operator with respect to root of index i. "
    "If i is negative then the e_i root operator is defined to be "
    "the f_\\alpha operator.",
    "e_{{i}}= LROdefine_i; e_{- 1} e_{- 1} LSpath{}(G_2, (0,0), (2,1))",
    "Calculator::littelmannOperator",
    "LROdefine",
    innerExperimental
  );
  this->addOperationHandler(
    "PrintProductDistancesModN",
    CalculatorFunctions::innerFindProductDistanceModN,
    "",
    "First argument = number N. Second argument = list of positive numbers. "
    "This function finds, modulo N, the product "
    "distance between 1 and each number mod N. "
    "We define the product distance between 1 and a "
    "number X as the minimal sum of elements "
    "whose product equals X mod N, where "
    "the elements are taken from the predefined "
    "list of positive integers given by the second argument.",
    "PrintProductDistancesModN(65537, "
    "(97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122)"
    ")",
    "CalculatorFunctions::innerFindProductDistanceModN",
    "PrintProductDistancesModN",
    adminNoTestInvisibleOffByDefault
  );
  this->addOperationHandler(
    "SolveProductSumEquationOverSetModN",
    CalculatorFunctions::innerSolveProductSumEquationOverSetModN,
    "",
    "Tries to find one solution of the system a_1*a_2* ...= X mod N a_1+a_2+...=Y "
    "where the a_i's belong to a "
    "predefined set of positive numbers. ",
    "SolveProductSumEquationOverSetModN(theMod =65537; theProduct =16628; theSum=1286; "
    "theSet = (97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122)) ",
    "CalculatorFunctions::innerFindProductDistanceModN",
    "SolveProductSumEquationOverSetModN",
    adminNoTestInvisibleOffByDefault
  );
  this->addOperationHandler(
    "ComputeFKFT",
    CalculatorLieTheory::computePairingTablesAndFKFTsubalgebras,
    "",
    "Attempts to compute all Fernando-Kac subalgebras according "
    "to the most experimental, latest and greatest algorithm. "
    "Argument must be of type semisimple Lie subalgebras. ",
    "ComputeFKFT( ComputeSemisimpleSubalgebras(c_3))",
    "CalculatorFunctions::computePairingTablesAndFKFTsubalgebras",
    "ComputeFKFT",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "ComputeSemisimpleSubalgebras",
    CalculatorLieTheory::computeSemisimpleSubalgebras,
    "",
    "Computes the semisimple subalgebras of a semisimple "
    "Lie algebra and creates a data structure containing them. ",
    "ComputeSemisimpleSubalgebras(A_2)",
    "CalculatorFunctions::computeSemisimpleSubalgebras",
    "ComputeSemisimpleSubalgebras",
    innerInvisibleNoTest
  );
  this->addOperationHandler(
    "CentralizerChains",
    CalculatorLieTheory::getCentralizerChainsSemisimpleSubalgebras,
    "",
    "This function is disabled by default (takes too long): you have to first call "
    "TurnRulesOn(CentralizerChains). "
    "Please use this function only if running the calculator on "
    "your own machine; don't use it on a public server. "
    "Creates a printout with centralizer chains of semisimple Lie subalgebras. ",
    "CentralizerChains (ComputeSemisimpleSubalgebras{}(B_3))",
    "CalculatorFunctions::getCentralizerChainsSemisimpleSubalgebras",
    "CentralizerChains",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "PrintSemisimpleSubalgebras",
    CalculatorLieTheory::printSemisimpleSubalgebrasRegular,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b> "
    "Prints the semisimple subalgebras of a semisimple Lie algebra. ",
    "PrintSemisimpleSubalgebras(B_3)",
    "Calculator::printSemisimpleSubalgebrasRegular",
    "PrintSemisimpleSubalgebras",
    innerStandard
  );
  this->addOperationHandler(
    "PrintSemisimpleSubalgebrasRecompute",
    CalculatorLieTheory::printSemisimpleSubalgebrasRecompute,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b> "
    "Prints the semisimple subalgebras of a semisimple Lie algebra. ",
    "PrintSemisimpleSubalgebrasRecompute(C_3)",
    "Calculator::printSemisimpleSubalgebrasRecompute",
    "PrintSemisimpleSubalgebrasRecompute",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "PrintSemisimpleSubalgebrasNoCentralizers",
    CalculatorLieTheory::printSemisimpleSubalgebrasNoCentralizers,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b> "
    "Prints the semisimple subalgebra candidates of a Lie algebra. ",
    "PrintSemisimpleSubalgebrasNoCentralizers(A_3)",
    "Calculator::printSemisimpleSubalgebrasNoCentralizers",
    "PrintSemisimpleSubalgebrasNoCentralizers",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "PrintSemisimpleSubalgebrasFull",
    CalculatorLieTheory::printSemisimpleSubalgebrasNilradicals,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b>"
    "Prints the semisimple subalgebras of a semisimple Lie algebra. ",
    "PrintSemisimpleSubalgebrasFull{}(A_2)",
    "Calculator::printSemisimpleSubalgebrasNilradicals",
    "PrintSemisimpleSubalgebrasFull",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "CanBeExtendedParabolicallyTo",
    CalculatorLieTheory::canBeExtendedParabolicallyTo,
    "",
    "Finds whether a Dynkin type extends via the standard parabolic extension to another. ",
    "CanBeExtendedParabolicallyTo(A^3_1, A_5);"
    "CanBeExtendedParabolicallyTo(a_2, b_3);"
    "CanBeExtendedParabolicallyTo(g_2, b_3);"
    "CanBeExtendedParabolicallyTo(d_5, e_6);"
    "CanBeExtendedParabolicallyTo(b_3, a_10);"
    "CanBeExtendedParabolicallyTo(c_2, c_10);"
    "CanBeExtendedParabolicallyTo(b_3+e_6+a_3, d_4+e_8+f_4)",
    "CalculatorFunctions::canBeExtendedParabolicallyTo",
    "CanBeExtendedParabolicallyTo",
    innerStandard
  );
  this->addOperationHandler(
    "EmbedSemisimpleInSemisimple",
    CalculatorFunctions::innerEmbedSemisimpleAlgebraInSemisimpleAlgebra,
    "",
    "Tries to find all embeddings of the first semisimple type into the second. "
    "Records all intermediate subalgebras. ",
    "EmbedSemisimpleInSemisimple{}(G^3_2, B_3);"
    "EmbedSemisimpleInSemisimple{}(G_2, B_3)",
    "CalculatorFunctions::innerEmbedSemisimpleAlgebraInSemisimpleAlgebra",
    "EmbedSemisimpleInSemisimple",
    innerNoTest
  );
  this->addOperationHandler(
    "LoadSemisimpleSubalgebras",
    CalculatorConversions::innerLoadSemisimpleSubalgebras,
    "",
    "<b>This function is being developed and is not implemented fully yet.</b>"
    "Loads a semisimpleSubalgebra from expression. ",
    "LoadSemisimpleSubalgebras {}(EmbedSemisimpleInSemisimple{}(G_2, B_3))",
    "CalculatorConversions::innerLoadSemisimpleSubalgebras",
    "LoadSemisimpleSubalgebras",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "SltwoSubalgebra",
    CalculatorConversions::innerSlTwoSubalgebraPrecomputed,
    "",
    "<b>This function is being developed and is not implemented fully yet. </b>"
    "Loads an sl(2) subalgebra from expression. ",
    "SltwoSubalgebra(\n"
    "B_{3},\n"
    "3 GetChevalleyGenerator((B)_{3}, -2),\n"
    "3 GetChevalleyGenerator( (B)_{3}, 3)\n"
    "+ 6 GetChevalleyGenerator( (B)_{3}, 1)\n"
    "+ 10 / 3 GetChevalleyGenerator( (B)_{3}, 2)\n"
    ")",
    "CalculatorConversions::innerSlTwoSubalgebraPrecomputed",
    "SltwoSubalgebra",
    innerAdminNoTest
  );

  this->addOperationHandler(
    "Freudenthal",
    CalculatorFunctions::innerFreudenthalFormula,
    "",
    "Computes the dominant weights with multiplicities of a "
    "finite dimensional module of a highest weight "
    "given in fundamental coordinates. The first argument gives "
    "the semisimple Lie algebra type, the second argument gives "
    "the highest weight in fundamental coordinates. ",
    "Freudenthal{}(B_3, (2,2,2))",
    "Calculator::innerFreudenthalFormula",
    "Freudenthal",
    innerStandard
  );
  this->addOperationHandler(
    "Killing",
    CalculatorLieTheory::killingForm,
    "",
    "Computes the Killing form product of two elements of semisimple Lie algebra. ",
    "h_{{i}}= GetCartanGenerator{}(F_1, i);"
    "KF{}({{i}},{{j}})=Killing{}(h_i, h_j);"
    "FunctionToMatrix(KF, 4, 4)",
    "CalculatorLieTheory::killingForm",
    "Killing",
    innerStandard
  );
  this->addOperationHandler(
    "FreudenthalFull",
    CalculatorFunctions::innerFreudenthalFull,
    "",
    "Computes the full character and prints it to screen. "
    "Argument format same as the Freudenthal function. ",
    "FreudenthalFull{}(G_2, (2, 1))",
    "Calculator::innerFreudenthalFull",
    "FreudenthalFull",
    innerStandard
  );
  this->addOperationHandler(
    "PerturbSplittingNormal",
    CalculatorFunctions::innerPerturbSplittingNormal,
    "",
    "Takes 3 arguments: normal, cone and general vectors. "
    "Attempts to perturb the normal so that the cone stays on "
    "one side of the normal and so that the general vectors "
    "have as little zero scalar products with the normal as possible. ",
    "PerturbSplittingNormal{}((0,0,0,0), ("
    "(- 1, -2, -2, -2), (- 1, - 1, -2, -2), (0, - 1, -2, -2), "
    "(- 1, - 1, - 1, - 1), (0, - 1, - 1, - 1),"
    "(0, 0, - 1, - 1), (- 1, - 1, 0, 0), (0, - 1, 0, 0), "
    "(- 1, 0, 0, 0), (1, 0, 0, 0), (0, 1, 0, 0), (1, 1, 0, 0),"
    "(0, 0, 1, 1), (0, 1, 1, 1), (1, 1, 1, 1), "
    "(0, 1, 2, 2), (1, 1, 2, 2), (1, 2, 2, 2)),"
    "("
    "(-2, -3, -4, -2), (- 1, -3, -4, -2), (- 1, -2, -4, -2), "
    "(- 1, -2, -3, -2), (- 1, -2, -2, -2), (- 1, -2, -3, - 1),"
    "(- 1, - 1, -2, -2), (- 1, -2, -2, - 1), (0, - 1, -2, -2), "
    "(- 1, - 1, -2, - 1), (- 1, -2, -2, 0),"
    "(0, - 1, -2, - 1), (- 1, - 1, - 1, - 1), (- 1, - 1, -2, 0), "
    "(0, - 1, - 1, - 1), (0, - 1, -2, 0), (- 1, - 1, - 1, 0),"
    "(0, 0, - 1, - 1), (0, - 1, - 1, 0), (- 1, - 1, 0, 0), "
    "(0, 0, 0, - 1), (0, 0, - 1, 0), (0, - 1, 0, 0), (- 1, 0, 0, 0),"
    "(1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1), "
    "(1, 1, 0, 0), (0, 1, 1, 0), (0, 0, 1, 1), (1, 1, 1, 0),"
    "(0, 1, 2, 0), (0, 1, 1, 1), (1, 1, 2, 0), (1, 1, 1, 1), "
    "(0, 1, 2, 1), (1, 2, 2, 0), (1, 1, 2, 1), (0, 1, 2, 2), "
    "(1, 2, 2, 1),"
    "(1, 1, 2, 2), (1, 2, 3, 1), (1, 2, 2, 2), (1, 2, 3, 2), "
    "(1, 2, 4, 2), (1, 3, 4, 2), (2, 3, 4, 2)))",
    "Calculator::innerPerturbSplittingNormal",
    "PerturbSplittingNormal",
    innerStandard
  );
  this->addOperationHandler(
    "TestSpechtModules",
    CalculatorFunctionsWeylGroup::testSpechtModules,
    "",
    "Tests all Specht modules of S_n, where n is the only argument taken by the function.",
    "TestSpechtModules(4); ",
    "CalculatorFunctionsWeylGroup::testSpechtModules",
    "TestSpechtModules",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "MakeElementWeylGroup",
    CalculatorFunctionsWeylGroup::weylGroupElement,
    "",
    "Needs a group name and a list of generators",
    "s_{{i}} = MakeElementWeylGroup{}(A_2, i);\n"
    "(s_1 + s_2)(2s_1 + s_2)(3s_1 + s_2)",
    "CalculatorFunctionsWeylGroup::weylGroupElement",
    "MakeElementWeylGroup",
    innerStandard
  );
  this->addOperationHandler(
    "MakeElementHyperOctahedral",
    CalculatorConversions::innerMakeElementHyperOctahedral,
    "",
    "Needs a group name and a list of generators",
    "s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0);\n"
    "t = MakeElementHyperOctahedral{}((1, 3), 0, 0, 0);\n"
    "s * t * s * t",
    "CalculatorConversions::innerMakeElementHyperOctahedral",
    "MakeElementHyperOctahedral",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupClassicalSignMultiplicities",
    CalculatorFunctionsWeylGroup::signSignatureRootSubsystemsFromKostkaNumbers,
    "",
    "Prints the sign multiplicities of a simple Weyl group of classical type. "
    "See a common article by "
    "T. Folz-Donahue, S. Jackson, T. Milev, A. Noel. ",
    "WeylGroupClassicalSignMultiplicities{}(b_3);\n"
    "WeylGroupClassicalSignMultiplicities{}(b_4);\n"
    "WeylGroupClassicalSignMultiplicities{}(b_5);\n"
    "WeylGroupClassicalSignMultiplicities{}(d_4);\n"
    "WeylGroupClassicalSignMultiplicities{}(d_5);\n"
    "WeylGroupClassicalSignMultiplicities{}(d_6);\n"
    "WeylGroupClassicalSignMultiplicities{}(a_2);\n"
    "WeylGroupClassicalSignMultiplicities{}(a_3);\n"
    "WeylGroupClassicalSignMultiplicities{}(a_4);\n",
    "CalculatorFunctionsWeylGroup::signSignatureRootSubsystemsFromKostkaNumbers",
    "WeylGroupClassicalSignMultiplicities",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupTauSignatures",
    CalculatorFunctionsWeylGroup::signSignatureRootSubsystems,
    "",
    "Prints the tau signatures of a Weyl group. See a common article by "
    "T. Folz-Donahue, S. Jackson, T. Milev, A. Noel. ",
    "WeylGroupTauSignatures{}(b_3);",
    "CalculatorFunctionsWeylGroup::signSignatureRootSubsystems",
    "WeylGroupTauSignatures",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "WeylGroupConjugacyClassesFromAllElements",
    CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements,
    "",
    "For small ranks, computes the conjugacy classes of a Weyl "
    "group by enumerating all elements of the group. ",
    "WeylGroupConjugacyClassesFromAllElements{}(A_2);",
    "CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesFromAllElements",
    "WeylGroupConjugacyClassesFromAllElements",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "WeylGroupOuterConjugacyClassesFromAllElements",
    CalculatorFunctionsWeylGroup::weylGroupOuterConjugacyClassesFromAllElements,
    "",
    "Computes conjugacy classes, identifying classes that "
    "are conjugate using outer automorphisms. ",
    "WeylGroupOuterConjugacyClassesFromAllElements{}(D_4);",
    "CalculatorFunctionsWeylGroup::weylGroupOuterConjugacyClassesFromAllElements",
    "WeylGroupOuterConjugacyClassesFromAllElements",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "WeylGroupConjugacyClassesRepresentatives",
    CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives,
    "",
    "Computes a representative in each conjugacy classes of a "
    "Weyl group by comparing conjugacy classes invariants and by "
    "enumerating conjugacy class orbits. ",
    "WeylGroupConjugacyClassesRepresentatives{}(A_2);",
    "CalculatorFunctionsWeylGroup::weylGroupConjugacyClassesRepresentatives",
    "WeylGroupConjugacyClassesRepresentatives",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "WeylGroupConjugacyClasses",
    CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS,
    "",
    "Loads the conjugacy classes of a Weyl group (hard-coded), "
    "or computes them if rank<=6. ",
    "WeylGroupConjugacyClasses{}(f_4);",
    "CalculatorFunctionsWeylGroup::weylGroupConjugacyClasseS",
    "WeylGroupConjugacyClasses",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupCharTable",
    CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable,
    "",
    "Loads the character table of a Weyl group. "
    "The character tables are hard-coded.",
    "WeylGroupCharTable{}(b_3);",
    "CalculatorFunctionsWeylGroup::weylGroupLoadOrComputeCharTable",
    "WeylGroupCharTable",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupIrrepsAndCharTableComputeFromScratch",
    CalculatorFunctionsWeylGroup::weylGroupIrrepsAndCharTableComputeFromScratch,
    "",
    "<b>Work in progress. Please do not use.</b> "
    "Computes from scratch the irreducible representations "
    "and the character table of a Weyl group.",
    "WeylGroupIrrepsAndCharTableComputeFromScratch{}(b_3);",
    "CalculatorFunctionsWeylGroup::weylGroupIrrepsAndCharTableComputeFromScratch",
    "WeylGroupIrrepsAndCharTableComputeFromScratch",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "WeylOrbitSize",
    CalculatorFunctionsWeylGroup::weylGroupOrbitSize,
    "",
    "Computes the size of a Weyl group orbit of a weight",
    "WeylOrbitSize(E_6, (3, 3, 3, 7, 7, 11));\n"
    "WeylOrbitSize(E_8, (3, 3, 3, 7, 7, 11, 13, 13));",
    "CalculatorFunctionsWeylGroup::weylGroupOrbitSize",
    "WeylOrbitSize",
    innerStandard
  );
  this->addOperationHandler(
    "WeylOrbitSimpleCoords",
    CalculatorFunctionsWeylGroup::weylGroupOrbitSimple,
    "",
    "Generates a Weyl orbit printout from simple coords. "
    "First argument = type. Second argument = weight in simple coords. "
    "The orbit size is cut off at max 1920 elements (type D_5).",
    "WeylOrbitSimpleCoords{}(B_2, (y, y));",
    "CalculatorFunctionsWeylGroup::weylGroupOrbitSimple",
    "WeylOrbitSimpleCoords",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupOrbitOuterSimple",
    CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple,
    "",
    "Generates a Weyl outer orbit printout from simple coords. "
    "The outer orbit is the orbit "
    "under the Weyl group extended with the outer automoprhisms of the Weyl group. "
    "First argument = type. Second argument = weight in simple coords. "
    "The orbit size is cut off at 1921*2 elements. ",
    "WeylGroupOrbitOuterSimple{}(D_4, (1, 0, 0, 0))",
    "CalculatorFunctionsWeylGroup::weylGroupOrbitOuterSimple",
    "WeylGroupOrbitOuterSimple",
    innerStandard
  );
  this->addOperationHandler(
    "WeylOrbitFundCoords",
    CalculatorFunctionsWeylGroup::weylGroupOrbitFund,
    "",
    "Generates a Weyl orbit printout from fundamental coords. "
    "First argument = type. Second argument = weight in fundamental coords. ",
    "WeylOrbitFundCoords{}(B_2, (y, 0));",
    "CalculatorFunctionsWeylGroup::weylGroupOrbitFund",
    "WeylOrbitFundCoords",
    innerStandard
  );
  this->addOperationHandler(
    "WeylOrbitFundRho",
    CalculatorFunctionsWeylGroup::weylGroupOrbitFundRho,
    "",
    "Generates a Weyl orbit printout from fundamental coords. "
    "First argument = type. Second argument = weight in fundamental coords. "
    "Doing the rho-modified action. ",
    "WeylOrbitFundRho{}(B_2, (y, 0) )",
    "CalculatorFunctionsWeylGroup::weylGroupOrbitFundRho",
    "WeylOrbitFundRho",
    innerStandard
  );
  this->addOperationHandler(
    "WeylRaiseToMaximallyDominant",
    CalculatorFunctionsWeylGroup::weylNoOuterRaiseToMaximallyDominant,
    "",
    "Raises a set of rational vectors simultaneously to maximally dominant: "
    "it raises the first vector to dominant, then each consecutive vector is "
    "raised as much as possible without changing the preceding, "
    "alrady raised,  vectors. First argument = type. "
    "Second argument = weight in simple coords. ",
    "WeylRaiseToMaximallyDominant{}(F_4, (3, 6, 8, 4), (- 1, - 3, - 2, 0));\n"
    "WeylRaiseToMaximallyDominant{}(F_4, (3, 6, 8, 4), (0, - 3, - 4, - 2));\n"
    "WeylRaiseToMaximallyDominant{}(F_4, (0, - 3, - 4, - 2) , (3, 6, 8, 4));",
    "CalculatorFunctionsWeylGroup::weylNoOuterRaiseToMaximallyDominant",
    "WeylRaiseToMaximallyDominant",
    innerStandard
  );
  this->addOperationHandler(
    "MakeWeight",
    CalculatorFunctionsWeylGroup::lieAlgebraWeight,
    "",
    "Makes a weight. First argument = type. "
    "Second argument = coordinate. "
    "Third argument = one of the keywords epsilon, simple, "
    "fundamental, standing for the coordinate system. ",
    "\\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon);\n"
    "\\psi_{{a}}=MakeWeight{}(B_3, a, fundamental);\n"
    "\\eta_{{a}}= MakeWeight{}(B_3, a, simple);\n"
    "\\varepsilon_1;\n"
    "\\psi_1;\n"
    "\\eta_3",
    "CalculatorFunctionsWeylGroup::lieAlgebraWeight",
    "MakeWeight",
    innerStandard
  );
  this->addOperationHandler(
    "MakeRho",
    CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight,
    "",
    "Makes the half-sum of the positive roots.",
    "\\rho = MakeRho(B_3);",
    "CalculatorFunctionsWeylGroup::lieAlgebraRhoWeight",
    "MakeRho",
    innerStandard
  );
  this->addOperationHandler(
    "IsOuterAuto",
    CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup,
    "",
    "Checks whether the element is outer automorphism of a root system. "
    "First argument = type. "
    "Second argument = matrix linear operator corresponding written in simple basis. ",
    "A = MakeMatrix"
    "((1, 0, 0, 0, 0, - 1),"
    "(0, 0, 0, 0, 1, - 2),"
    "(0, 0, 1, 0, 0, - 2),"
    "(0, 0, 0, 1, 0, - 3),"
    "(0, 1, 0, 0, 0, - 2),"
    "(0, 0, 0, 0, 0, - 1));"
    "IsOuterAuto{}(e_6, A);",
    "CalculatorFunctionsWeylGroup::isOuterAutoWeylGroup",
    "IsOuterAuto",
    innerStandard
  );
  this->addOperationHandler(
    "WeylOuterRaiseToMaximallyDominant",
    CalculatorFunctionsWeylGroup::weylOuterRaiseToMaximallyDominant,
    "",
    "Same as WeylRaiseToMaximallyDominant but uses outer Weyl group automorphisms as well.",
    "WeylOuterRaiseToMaximallyDominant{}(D_4, (1, 0, 0, 0), (0, 0, 0, 1));\n"
    "WeylOuterRaiseToMaximallyDominant{}(D_4, (0, 0, 0, 1), (1, 0, 0, 0));\n"
    "WeylOuterRaiseToMaximallyDominant{}(D_4, (1, 0, 0, 0), (0, 0, 1, 0));\n"
    "WeylOuterRaiseToMaximallyDominant{}(D_4, (0, 0, 1, 0), (1, 0, 0, 0));\n",
    "CalculatorFunctionsWeylGroup::weylOuterRaiseToMaximallyDominant",
    "WeylOuterRaiseToMaximallyDominant",
    innerStandard
  );
  this->addOperationHandler(
    "GenerateFiniteMultiplicativelyClosedSet",
    CalculatorFunctions::innerGenerateMultiplicativelyClosedSet,
    "",
    "The first argument gives upper_bound to the number of elements of the set. "
    "Generates a finite multiplicatively closed set of at most upper_bound elements, "
    "or returns error indicating the multiplicatively closed "
    "set is larger than the upper bound.",
    "s_1=MakeMatrix{}((- 1, 1, 0), (0, 1, 0), (0, 0, 1));\n"
    "s_2=MakeMatrix{}((1, 0, 0), (1, - 1, 1), (0, 0, 1));\n"
    "s_3=MakeMatrix{}((1, 0, 0), (0, 1, 0), (0, 2, - 1));\n"
    "GenerateFiniteMultiplicativelyClosedSet{}(48, s_1, s_2, s_3);",
    "Calculator::innerGenerateMultiplicativelyClosedSet",
    "GenerateFiniteMultiplicativelyClosedSet",
    innerStandard
  );
  this->addOperationHandler(
    "CartanSA",
    CalculatorLieTheory::constructCartanSubalgebra,
    "",
    "Generates a Cartan subalgebra, code is still experimental.",
    "g_{{i}} = GetChevalleyGenerator{}(b_3, i);\n"
    "h_{{i}} = GetCartanGenerator{}(b_3, i);\n"
    "CartanSA(g_1 + g_3, g_-1 + g_-3, g_2, g_-2);",
    "CalculatorFunctions::constructCartanSubalgebra",
    "CartanSA",
    Function::Options::administrativeExperimentalTested()
  );
  this->addOperationHandler(
    "GenerateVectorSpaceClosedWithRespectToLieBracket",
    CalculatorLieTheory::generateVectorSpaceClosedWithRespectToLieBracket,
    "",
    "Generates a vector space closed with respect to the Lie bracket "
    "from input of type elements of Weyl algebra or Lie algebra. "
    "The first argument of the input is an upper bound for the dimension of the vector space. "
    "The remaining arguments must be differential operators. "
    "The output is a vector space basis printout. "
    "Fails if the dimension of the vector space is larger than the upper bound.",
    "\\partial_{{i}} = ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}} = ElementWeylAlgebraPoly{}(\\partial_i, x_i); "
    "g_1 = (- x_{5} \\partial_{5}^{2} - x_{4} \\partial_{4} \\partial_{5}  - x_{3} \\partial_{3} \\partial_{5} "
    "- x_{2} \\partial_{2} \\partial_{5} + x_{1} \\partial_{2} \\partial_{4} +x_{1} \\partial_{3}^{2});\n"
    "g_- 1 = x_5;\n"
    "g_2 = x_{5} \\partial_{4} - x_{2} \\partial_{1};\n"
    "g_-2 = (x_{4} \\partial_{5} - x_{1} \\partial_{2});\n"
    "g_3 = (2x_{4} \\partial_{3} - x_{3} \\partial_{2});\n"
    "g_-3 = (x_{3} \\partial_{4} - 2x_{2} \\partial_{3});\n"
    "GenerateVectorSpaceClosedWithRespectToLieBracket{}(50, g_1, g_- 1, g_2, g_-2, g_3, g_-3);\n"
    "GenerateVectorSpaceClosedWithRespectToLieBracket{}(50, g_1, g_2, g_-2, g_3, g_-3);\n"
    "GenerateVectorSpaceClosedWithRespectToLieBracket{}(50, g_1, g_- 1, g_2, g_-2, g_3);\n"
    "q_{{i}} = GetChevalleyGenerator{}(F_4, i);\n"
    "s_2 = - q_{- 5} - q_{-6};\n"
    "s_1 = q_{20} + q_{19};\n"
    "s_- 1 = - q_{- 19} - q_{- 20};\n"
    "s_-2 = 2q_{6} + 2q_{5};\n"
    "GenerateVectorSpaceClosedWithRespectToLieBracket(52, s_1, s_2, s_- 1, s_-2);",
    "CalculatorFunctions::generateVectorSpaceClosedWithRespectToLieBracket",
    "GenerateVectorSpaceClosedWithRespectToLieBracket",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupNaturalRep",
    CalculatorFunctionsWeylGroup::weylGroupNaturalRep,
    "",
    "Gets the natural representation of the Weyl group.",
    "WeylGroupNaturalRep{}(B_3)",
    "CalculatorFunctionsWeylGroup::weylGroupNaturalRep",
    "WeylGroupNaturalRep",
    innerStandard
  );
  this->addOperationHandler(
    "WeylGroupGetOuterAutoGenerators",
    CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint,
    "",
    "Gets the generators of the outer automorphism group (the group generated by "
    "the graph automorphisms of the Dynkin diagram.",
    "WeylGroupGetOuterAutoGenerators{}(2D_4)",
    "CalculatorFunctionsWeylGroup::weylGroupOuterAutoGeneratorsPrint",
    "WeylGroupGetOuterAutoGenerators",
    innerStandard
  );
  this->addOperationHandler(
    "MakeVirtualWeylGroupRepresentation",
    CalculatorFunctionsWeylGroup::makeVirtualWeylRep,
    "",
    "Convert a representation into virtual one.",
    "MakeVirtualWeylGroupRepresentation{}(WeylGroupNaturalRep{}(B_3))",
    "CalculatorFunctionsWeylGroup::makeVirtualWeylRep",
    "MakeVirtualWeylGroupRepresentation",
    innerAdminNoTestExperimental
  );
  this->addOperationHandler(
    "RepresentElementHyperoctahedral",
    CalculatorFunctionsWeylGroup::representElementHyperOctahedral,
    "",
    "Represents element of hyperoctahedral into a representation. ",
    "V = HyperOctahedralRepresentation((1, 1), 1);\n"
    "s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0);\n"
    "RepresentElementHyperoctahedral(s, V)",
    "CalculatorFunctionsWeylGroup::representElementHyperOctahedral",
    "RepresentElementHyperoctahedral",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "HyperOctahedralIrreps",
    CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules,
    "",
    "Prints all modules induced from Specht modules. ",
    "HyperOctahedralIrreps(3)",
    "CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules",
    "HyperOctahedralIrreps",
    adminNoTestInvisibleExperimental
  );
  this->addOperationHandler(
    "SpechtModule",
    CalculatorFunctionsWeylGroup::spechtModule,
    "",
    "Gets the Specht module of the partition. ",
    "SpechtModule((3, 2, 1))",
    "CalculatorFunctionsWeylGroup::spechtModule",
    "SpechtModule",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "HyperOctahedralRepresentation",
    CalculatorFunctionsWeylGroup::hyperOctahedralGetOneRepresentation,
    "",
    "Gets one hyperoctahedral representation from two partitions. ",
    "HyperOctahedralRepresentation((1, 1), (1))",
    "CalculatorFunctionsWeylGroup::hyperOctahedralAllModulesInducedFromSpechtModules",
    "HyperOctahedralRepresentation",
    adminNoTestInvisibleExperimental
  );
  this->addOperationHandler(
    "HyperOctahedralGeneratorPrint",
    CalculatorFunctionsWeylGroup::hyperOctahedralPrintGeneratorCommutationRelations,
    "",
    "Prints the generator commutation relations of a hyperoctahedral group. ",
    "HyperOctahedralGeneratorPrint(3)",
    "CalculatorFunctionsWeylGroup::hyperOctahedralPrintGeneratorCommutationRelations",
    "HyperOctahedralGeneratorPrint",
    innerAdminNoTest
  );
  this->addOperationHandler(
    "PrincipalSlTwoIndex",
    CalculatorLieTheory::getPrincipalSl2Index,
    "",
    "Gives the symmetric Cartan corresponding to a given dynkin type.",
    "PrincipalSlTwoIndex(G_2^3 + D_4^2);",
    "CalculatorFunctions::getPrincipalSl2Index",
    "PrincipalSlTwoIndex",
    innerStandard
  );
  this->addOperationHandler(
    "DynkinIndicesSlTwoSubalgebras",
    CalculatorLieTheory::getDynkinIndicesSlTwoSubalgebras,
    "",
    "Fetches the absolute Dynkin indices of the sl(2) subalgebras of the input Dynkin type. "
    "The indices of each simple component are computed by actually constructing "
    "the sl(2)-subalgebras; clearly this can be greatly optimized "
    "but we postpone that for a future version. ",
    "DynkinIndicesSlTwoSubalgebras(2A_2);\n "
    "DynkinIndicesSlTwoSubalgebras(2A_2+A_2^2);\n"
    "DynkinIndicesSlTwoSubalgebras(2A_2+A_2^7);\n"
    "DynkinIndicesSlTwoSubalgebras(2G_2+G_2^7+F_4+F_4^19);",
    "CalculatorFunctions::getDynkinIndicesSlTwoSubalgebras",
    "DynkinIndicesSlTwoSubalgebras",
    innerStandard
  );
  this->addOperationHandler(
    "CartanSymmetric",
    CalculatorLieTheory::getSymmetricCartan,
    "",
    "Gives the symmetric Cartan corresponding to a given dynkin type.",
    "CartanSymmetric(e^7_6);",
    "CalculatorFunctions::getSymmetricCartan",
    "CartanSymmetric",
    innerStandard
  );
  this->addOperationHandler(
    "GrowDynkinType",
    CalculatorLieTheory::growDynkinType,
    "",
    "This is a calculator testing function. Grows a Dynkin "
    "type inside an ambient Dynkin type. ",
    "GrowDynkinType(A^30_1+d^30_4, e_6);\n"
    "GrowDynkinType(g^35_2+B^30_2, e_6);",
    "CalculatorFunctions::growDynkinType",
    "GrowDynkinType",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor,
    this->opElementTensorGVM(),
    this->opElementTensorGVM(),
    "Adds two elements of tensor products of generalized Verma modules. ",
    "v=HeighestWeightVector{}(G_2, (1,0),(0,0));\n"
    "(3/4 v)\\otimes v-3/4 (v\\otimes v)",
    "CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor",
    "AddElementTensorGeneralizedVermaModuleToElementTensorGeneralizedVermaModule",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddWeightToWeight,
    this->opWeightLieAlgPoly(),
    this->opWeightLieAlgPoly(),
    "Adds two weights. ",
    "\\lambda = Polynomial{}\\lambda; "
    "\\varepsilon_{{a}} = MakeWeight{}(B_3, a, epsilon); "
    "(1 / 2 + \\lambda) \\varepsilon_1 + 1 / 2 \\varepsilon_2+ 1 / 2 \\varepsilon_3  ",
    "CalculatorFunctionsBinaryOps::innerAddWeightToWeight",
    "AddWeightLieAlgebraPolynomialToWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "+",
    CalculatorFunctionsBinaryOps::innerAddUEToAny,
    this->opElementUEOverRF(),
    this->opElementUEOverRF(),
    "Adds an element of UE (Universal Enveloping algebra) to an element of UE.",
    "g_{{{i}}} = GetChevalleyGenerator{}(F_{1}, {{i}});\n"
    "h_{{{i}}} = GetCartanGenerator{}(F_{1}, {{i}});\n"
    "[g_{22}+g_{20}+g_{14},g_{- 14}+g_{-20}+g_{-22}]",
    "CalculatorFunctionsBinaryOps::innerAddUEToAny",
    "AddElementUniversalEnvelopingRationalFunctionCoefficientsToElementUniversalEnvelopingRationalFunctionCoefficients",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct,
    this->opElementHyperOctahedral(),
    this->opElementHyperOctahedral(),
    "Multiplies two elements of hyperoctahedral groups. ",
    "s = MakeElementHyperOctahedral{}((1, 2), 1, 0, 0); "
    "t = MakeElementHyperOctahedral{}((1, 3), 0, 0, 0); "
    "s * t * s * t",
    "CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct",
    "MultiplyElementHyperOctahedralByElementHyperOctahedral",
    innerExperimentalNoTest
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg,
    this->opCharSSAlgMod(),
    this->opCharSSAlgMod(),
    "Multiplies two semisimple Lie algebra finite dimensional characters and decomposes using the "
    "Brauer-Klimyk formula, Humphreys J. Introduction to Lie algebras and representation theory, "
    "page 142, exercise 9. ",
    "x = MakeCharacterLieAlg{}(G_2, (1,0));\n"
    "y = MakeCharacterLieAlg{}(G_2, (0,1));\n"
    "x * y",
    "CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg",
    "MultiplyCharacterSemisimpleLieAlgebraModuleBy",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt,
    this->opWeylGroupElement(),
    this->opWeylGroupElement(),
    "Multiplies two Weyl group elements if possible. ",
    "x = MakeElementWeylGroup{}(A_2, 1); x*x",
    "CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt",
    "MultiplyWeylGroupElementByWeylGroupElement",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerWeylGroupElementByInteger,
    this->opWeylGroupElement(),
    this->opRational(),
    "Exponentiates element of a Weyl group to an integer power. ",
    "a = MakeElementWeylGroup{}(f_4, 1);\n"
    "b = MakeElementWeylGroup{}(f_4, 2);\n"
    "c = MakeElementWeylGroup{}(f_4, 3);\n"
    "d = MakeElementWeylGroup{}(f_4, 4);\n"
    "x = a b c d  b a;\n"
    "x^1000001",
    "CalculatorFunctionsBinaryOps::innerPowerWeylGroupElementByInteger",
    "PowerWeylGroupElementByInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylReps,
    this->opWeylGroupVirtualRep(),
    this->opWeylGroupVirtualRep(),
    "Tensor and decompose two virtual Weyl group representations. ",
    "W = WeylGroupNaturalRep{}(B_3);\n"
    "V = MakeVirtualWeylGroupRepresentation{}W;\n"
    "W\\otimes W;\n"
    "V * V",
    "CalculatorFunctionsWeylGroup::tensorAndDecomposeWeylReps",
    "TensorAndDecomposeWeylGroupRepresentations",
    innerExperimentalNoTest
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE,
    this->opRational(),
    this->opElementUEOverRF(),
    "Multiplies rational number by an element universal enveloping algebra.",
    "g_{{i}}= GetChevalleyGenerator{}(F_1, i); h_{{i}}= GetCartanGenerator{}(F_1, i);\n"
    "[g_{22}+g_{20}+g_{14},g_{17}-6/5g_{14}]",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE",
    "MultiplyRationalByUE",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE,
    this->opElementUEOverRF(),
    this->opElementUEOverRF(),
    "Multiplies elment Universal enveloping by element universal enveloping algebra.",
    "g_{{i}}= GetChevalleyGenerator{}(F_1, i);"
    "h_{{i}}= GetCartanGenerator{}(F_1, i) ;\n"
    "[g_{22}+g_{20}+g_{14},g_{17}-6/5g_{14}]",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE",
    "MultiplyUEByUE",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opRational(),
    this->opElementTensorGVM(),
    "Handles multiplying rational number by an element of "
    "tensor product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\nh_{{i}}= GetCartanGenerator{}(X, i);\n"
    "v=HeighestWeightVector{}(G_2, (1,0),(0,0));\n"
    "2/5 v;\n(3/4 v)\\otimes v;\n3/4 (v\\otimes v);\n(3/4 v)\\otimes v-3/4 (v\\otimes v)",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyAnyByEltTensor",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opPolynomialRational(),
    this->opElementTensorGVM(),
    "Handles multiplying polynomial by an element of tensor "
    "product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\n"
    "g_{{i}}= GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X, i);\n"
    "z = Polynomial{}y;\n"
    "v = HeighestWeightVector{}(G_2, (z,1),(1,0));\n"
    "(2*z) v;\n",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyPolynomialByElementTensorGeneralVermaModule",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opRationalFunction(),
    this->opElementTensorGVM(),
    "Handles multiplying rational function number by an element of tensor product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\n"
    "g_{{i}} = GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}} = GetCartanGenerator{}(X, i);\n"
    "z = Polynomial{}y;\n"
    "v = HeighestWeightVector{}(G_2, (z,1),(1,0));\n"
    "1/z v",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyAnyByTensor",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor,
    this->opElementUEOverRF(),
    this->opElementTensorGVM(),
    "Handles acting by element Universal enveloping on an element of "
    "tensor product of generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\nh_{{i}}= GetCartanGenerator{}(X, i);\n"
    "z= Polynomial{}y;\nv=HeighestWeightVector{}(G_2, (z,1),(1,0));\n"
    "h_1 v; \nh_2 v;\n g_1 g_{- 1} v ",
    "CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor",
    "MultiplyElementUniversalEnvelopingRationalFunctionByElementTensorGeneralizedVermaModule",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly,
    this->opWeylGroupElement(),
    this->opWeightLieAlgPoly(),
    "Element of Weyl group action on a weight. ",
    "s_{{a}}=MakeElementWeylGroup(B_3, a);\n"
    "\\varepsilon_{{a}} = MakeWeight{}(B_3, a, epsilon);\n"
    "x = Polynomial{}x;\n"
    "\\mu = x\\varepsilon_1;\n"
    "s_1s_2s_3s_2s_1 \\mu",
    "CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly",
    "MultiplyWeylGroupElementByWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly,
    this->opPolynomialRational(),
    this->opWeightLieAlgPoly(),
    "Carries out multiplication between a rational or polynomial "
    "on left and a weight on the right.",
    "\\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon); "
    "x = Polynomial{}x; x\\varepsilon_1",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly",
    "MultiplyPolynomialByWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "*",
    CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly,
    this->opRational(),
    this->opWeightLieAlgPoly(),
    "Carries out multiplication between a rational or polynomial "
    "on left and a weight on the right.",
    "\\varepsilon_{{a}}=MakeWeight{}(B_3, a, epsilon); "
    "x = Polynomial{}x; x\\varepsilon_1",
    "CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly",
    "MultiplyRaitonalByWeightLieAlgebraPolynomial",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF,
    this->opElementUEOverRF(),
    this->opRational(),
    "Raises element of universal enveloping to integer power. "
    "If the exponent is non-positive integer but the element of the UE is "
    "a single generator with coefficient 1, the exponent will be carried out formally. ",
    "g_{{i}}= GetChevalleyGenerator{}(G_2, i); h_{{i}}= GetCartanGenerator{}(G_2, i) ;"
    "\n (g_1+g_2)^2+ g_1^{1/2}",
    "CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF",
    "PowerElementUniversalEnvelopingBySmallInteger",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF,
    this->opElementUEOverRF(),
    this->opPolynomialRational(),
    "Provided that an element of Universal Enveloping algebra is "
    "a single generator (raised to arbitrary formal polynomial power) with coefficient 1, "
    "raises (formally) the element of the UE to arbitrary polynomial power. ",
    "g_{{i}}= GetChevalleyGenerator{}(G_2, i); h_{{i}}= GetCartanGenerator{}(G_2, i) ;\n"
    "((((g_1)^{Polynomial{}x})^{Polynomial{}y}) +g_2)^2",
    "CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF",
    "PowerUEelementToPolyPower",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "^",
    CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF,
    this->opElementUEOverRF(),
    this->opRationalFunction(),
    "Provided that an element of Universal Enveloping algebra is a single generator "
    "(raised to arbitrary formal RF power) with coefficient 1, raises "
    "(formally) the element of the UE to arbitrary RF power. ",
    "g_{{i}}= GetChevalleyGenerator{}(G_2, i); h_{{i}}= GetCartanGenerator{}(G_2, i);\n "
    "((((g_1)^{Polynomial{}x})^{Polynomial{}y}) +g_2)^2",
    "CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF",
    "PowerUEelementToRFPower",
    innerStandard
  );
  this->addOperationBinaryInnerHandlerWithTypes(
    "\\otimes",
    CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor,
    this->opElementTensorGVM(),
    this->opElementTensorGVM(),
    "Tensor product of two generalized Verma modules. "
    "Not fully tested and documented at the moment. "
    "Will get more documented in the future. ",
    "X = G_2;\n"
    "g_{{i}} = GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X, i);\n"
    "z= Polynomial{}y;\n"
    "v=HeighestWeightVector{}(G_2, (z,1), (1,0));\n"
    "g_{- 1}(v\\otimes v);\n"
    "g_{- 1}g_{- 1}(v\\otimes v)",
    "CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor",
    "TensorElementGeneralizedVermaModuleByElementGeneralizedVermaModule",
    innerStandard
  );
  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketDistribute,
    "",
    "Implementation of the rules [{{a}}+{{b}},{{c}}] = [a, c] +[b, c] "
    "and [{{c}},{{a}}+{{b}}] =[c,a] +[c,b]",
    "[a+b, c]",
    "CalculatorFunctionsBinaryOps::innerLieBracketDistribute",
    "LieBracketDistribute",
    innerStandard
  );
  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketOfMatrices,
    "",
    "Matrix lie bracket. ",
    "h=\\begin{pmatrix}3 & 0 & 0 & 0\\\\\n"
    "0 & 1 & 0 & 0\\\\"
    "0 & 0 & -1 & 0\\\\"
    "0 & 0 & 0 & -3\\\\"
    "\\end{pmatrix};"
    "e=\\begin{pmatrix}0 & 3 & 0 & 0\\\\"
    "0 & 0 & 2 & 0\\\\"
    "0 & 0 & 0 & 3/5\\\\"
    "0 & 0 & 0 & 0\\\\"
    "\\end{pmatrix};"
    "f=\\begin{pmatrix}0 & 0 & 0 & 0\\\\"
    "1 & 0 & 0 & 0\\\\"
    "0 & 2 & 0 & 0\\\\"
    "0 & 0 & 5 & 0\\\\"
    "\\end{pmatrix};",
    "CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant",
    "LieBracketOfMatrices",
    innerStandard
  );

  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketExtractConstant,
    "",
    "Extracts constants from Lie brackets. ",
    "[2a, 3b]",
    "CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant",
    "LieBracketConstants",
    innerStandard
  );
  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketRatPolyOrEWAWithRatPolyOrEWA,
    "",
    "Lie bracket of elements of Weyl algebras = differential operators with polynomial coefficients. ",
    "\\partial_{{i}}= ElementWeylAlgebraDO{}(\\partial_i, x_i);\n"
    "x_{{i}}= Polynomial{}x_i; \n[\\partial_1, x_1]; ",
    "CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA",
    "LieBracketWeylAlgebraElements",
    innerStandard
  );
  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketRatOrUEWithRatOrUE,
    "",
    "Lie bracket of elements of semisimple Lie algebra. ",
    "X =A_1;\ng_{{i}}= GetChevalleyGenerator{}(X,i);\n"
    "h_{{i}}= GetCartanGenerator{}(X,i);\n"
    "[g_1,g_{- 1}] ",
    "CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE",
    "LieBracketSemisimpleLieAlgebras",
    innerStandard
  );
  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketSwapTermsIfNeeded,
    "",
    "Swaps terms in Lie brackets if needed. ",
    "[b, a]",
    "CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded",
    "LieBracketSwapTermsIfNeeded",
    innerStandard
  );
  this->addOperationHandler(
    "[]",
    CalculatorFunctionsBinaryOps::lieBracketJacobiIdentityIfNeeded,
    "",
    "Extracts constants from Lie brackets. ",
    "[2a,3b] ",
    "CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant",
    "LieBracketJacobiIfNeeded",
    innerStandard
  );

  this->addOperationBinaryInnerHandlerWithTypes(
    "\\otimes",
    CalculatorFunctionsWeylGroup::tensorWeylReps,
    this->opWeylGroupRep(),
    this->opWeylGroupRep(),
    "Tensor product of two Weyl group reps. Does not decompose the tensor product. "
    "If you want decomposition, use V*V instead. ",
    "V = WeylGroupNaturalRep{}(B_3); V \\otimes V; V * V",
    "CalculatorFunctionsWeylGroup::tensorWeylReps",
    "TensorWeylGroupRepresentationByWeylGroupRepresentation",
    innerStandard
  );
}
