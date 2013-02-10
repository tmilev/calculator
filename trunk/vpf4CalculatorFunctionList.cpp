//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf4cpp(__FILE__, "List of calculator functions. ");
//This file lists calculator funcitons only. Please do not use for any other purposes.

void CommandList::initPredefinedInnerFunctions()
{ this->AddOperationInnerHandler
("gcdPoly", &this->fGCD, "",
   "Greatest common divisor polynomial of two polynomials. \
   The divisor is scaled so that all coefficients are relatively prime integers, \
   and so that the leading monomial under the graded lexicographic order (x_2>x_1, etc.)\
   has positive coefficient.",
   "gcdPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+\
   2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+\
   x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+\
   x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2},\
   x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}-\
   4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}-\
   2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-\
   x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}  )")
   ;
  this->AddOperationInnerHandler
("lcmPoly", &this->innerLCM, "",
   "Least common multiple of two polynomials.\
   The output is scaled so that all coefficients are relatively prime integers, \
   and so that the leading monomial under the graded lexicographic order (x_2>x_1, etc.)\
   has positive coefficient.",
   "lcmPoly{}(-x_{13}^{2}x_{15}^{3}x_{21}^{2}x_{22}-2x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+\
   2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{17}x_{20}x_{21}^{2}+\
   x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}+\
   x_{13}^{2}x_{15}x_{17}^{2}x_{19}x_{21}^{2},\
   x_{13}^{3}x_{15}x_{18}x_{21}^{2}x_{22}-2x_{13}^{3}x_{17}^{2}x_{21}^{3}-\
   4x_{13}^{2}x_{14}x_{15}x_{17}x_{21}^{3}+2x_{13}^{2}x_{15}^{2}x_{16}x_{21}^{3}-\
   2x_{13}^{3}x_{17}x_{18}x_{20}x_{21}^{2}+2x_{13}^{2}x_{14}x_{17}x_{18}x_{19}x_{21}^{2}-\
   x_{13}^{2}x_{15}x_{16}x_{18}x_{19}x_{21}^{2}  )")
   ;
  this->AddOperationInnerHandler
("PolyDivQuotient", &this->fPolynomialDivisionQuotient, "",
   "Quotient of two polynomials under division with remainder. ",
   "a:=Polynomial{}(x_1^3x_2+x_1^2x_2^3x_3);\nb:=Polynomial{}(x_1^2+x_3);\
   \nq:=PolyDivQuotient{}(a,b); \nr:=PolyDivRemainder{}(a,b); \na-q*b-r;", true)
   ;
  this->AddOperationInnerHandler
("PolyDivRemainder", &this->fPolynomialDivisionRemainder, "",
   "Remainder of polynomial division under division with remainder. ",
   "a:=Polynomial{}(x_1^3x_2+x_1^2x_2^3x_3);\n b:=Polynomial{}(x_1^2+x_3);\
   \nq:=PolyDivQuotient{}(a,b); \nr:=PolyDivRemainder{}(a,b); \na-q*b-r;", true)
   ;
  this->AddOperationInnerHandler
  ("suffixNotationForPostScript", this->innerSuffixNotationForPostScript, "",
   "Suffix notation. for postscript, used to quickly generate pstricks drawings in LaTeX.  \
   ",
   "suffixNotationForPostScript{}((1/3 +a+b)*c)")
   ;
this->AddOperationInnerHandler ("drawPolar", this->innerDrawPolarRfunctionTheta, "",
   "Draws polar curve given in polar coordinates in the form \
   r=f(t), where t is the angle variable. The angle variable is measured in degrees. \
   The first argument gives the function, the second and third argument give the upper and \
   lower bounds of the angle. \
   ",
   "drawPolar{}(1+sin {} t, 0, 6.283)")
   ;
this->AddOperationInnerHandler ("plot2D", this->innerPlot2D, "",
   "Makes a 2d plot of a function given in the form \
   y=f(x). The the second and third argument give the upper and \
   lower bounds of x. Plots may be added together- adding plots superimposes the plots. \
   ",
   "plot2D{}(x+1, 0, 5)")
   ;

  this->AddOperationInnerHandler
("IsInteger", this->fIsInteger, "",
 " If the argument has no bound variables, returns 1 if the argument is an integer, 0 otherwise. ",
 "IsInteger{}a;\nIsInteger{}1;\nf{}{{a}}:=IsInteger{}a;\nf{}1;\nf{}b");
  this->AddOperationInnerHandler
  ("SemisimpleLieAlgebra", this->innerSSLieAlgebra, "",
   "Creates a semisimple Lie algebra. \
    The type of the semisimple Lie algebra is given in the format\
    such as A_n or, alternatively, in the format A^k_n, \
    where the capital letter stands for the type, \
    the lower index stands for the rank, and the upper index stands for \
    the length of the first co-root squared. \
    Here, we define i^th co-root length squared as 4/(length squared of the \
    i^th simple root). \
    <br>When omitted, the upper index is substituted \
    with a default value. \
    <br>The upper index determines the scaling of \
    the symmetric Cartan matrix.  A non-default value for the upper index \
    will result in rescaling the structure constants of the corresponding \
    Chevalley-Weyl generators (in general making those constants non-integral).\
    <b>Remark.</b> It is advisable to omit the upper index (i.e., use default values) \
    when computing with a single semisimple Lie algebra. \
    <br>The allowed types are A_n, n>=1, B_n, n>=2, C_n, n>=3, D_n, n>=4,\
    E_6, E_7, E_8, F_4, G_2 (A^2_n, B^2_n, C^2_n, D^2_n, E^2_n, F^1_4, G^2_2 in extended notation). \
    The roots are ordered in order the order implied by \
    the (non-symmetric) Cartan matrices on page 59 in Huphreys, \
    Introduction to Lie algebras and representation theory. \
    By default (when the upper index is not present), the roots are scaled so that the long \
    root has squared length 2 in types \
    A_n, B_n, D_n, E_6, E_7, E_8, length 4 in types F_4, C_n, and length 6 in type G_2. \
    Except for F_4, our default root length convention coincides with Humphreys', paragraph 12.1\
    (for F_4, Humphreys' convention dictates long root length squared 2). \
    If the upper index is present, the root length squared of the first simple root equals \
    (upper index)*(default first root length squared)^2/4.\
    ",
   "g_{{i}}:=getChevalleyGenerator{}(SemisimpleLieAlgebra{}G_2, i);\
   \nh_{{i}}:=getCartanGenerator{}(SemisimpleLieAlgebra{}G_2, i);\
    \n[g_1,g_{-1}]; \n[g_2, g_{-2}]; \n[h_{1}, g_6]; \n[h_2, g_{-6}]");
  this->AddOperationInnerHandler
  ("printSemisimpleLieAlgebra", &this->innerPrintSSLieAlgebraVerbose, "",
   "Creates a quite detailed \
   printout with information about \
   the semisimple Lie algebra, including the Lie bracket pairing table. \
   In addition, this function creates a graphics of the root system. Warning. \
   For E_8 is quite heavy and might put a slower computer to sleep.  \
   ",
   "printSemisimpleLieAlgebra{}(F_4);\nprintSemisimpleLieAlgebra{}(2G^5_2+B_3);");
  this->AddOperationInnerHandler
  ("getChevalleyGenerator", this->innerGetChevGen, "",
   "First argument must be a semisimple Lie algebra, second argument must \
   be an integer from -N to N,\
   where N is the number of positive roots of the Lie algebra.\
   The function returns the Chevalley-Weyl generator labeled by the root\
   corresponding to the integer index. The roots are indexed in the ordered displayed by the \
   printSemisimpleLieAlgebra function. ",
   "[getChevalleyGenerator{}(G_2, 6), getChevalleyGenerator{}(G_2, -6)]");
  this->AddOperationInnerHandler
  ("getCartanGenerator", this->innerGetCartanGen, "",
   "First argument must be a semisimple Lie algebra, second argument must \
   be a number between 1 and K, where K is the rank of the Lie algebra.\
   In this case the function returns the element of the Cartan subalgebra that is dual \
   to the simple root with the same index. Note that this element of the Cartan subalgebra\
   is proportional to a Chevalley-Weyl generator with a coefficient of proportionality \
   equal to 2/(simple root length squared) ).",
   "getCartanGenerator{}(G_2, 1)");

  this->AddOperationInnerHandler
  ("Polynomial", this->innerPolynomial, "",
   "Creates a polynomial expression. ",
   "Polynomial{}((x-2y+z-1)^2(x+y-z))");
  this->AddOperationInnerHandler
  ("UE", this->innerElementUniversalEnvelopingAlgebra, "",
   "Creates an atomic representation of an element of a universal enveloping algebra.",
   "g_{{i}}:=getChevalleyGenerator{}(SemisimpleLieAlgebra{}G_2, i);\
   \nh_{{i}}:=getCartanGenerator{}(SemisimpleLieAlgebra{}G_2, i);\
   \nUE{}(g_1g_2-g_2g_1+g_3^2)");
  this->AddOperationInnerHandler
  ("MatrixRationals", this->innerMatrixRational,"",
   "Creates an internal c++ matrix structure from double list of rationals. \
   ", "s_1:=MatrixRationals{}((-1,-1,0,0), (0,1,0,0), (0,0,1,0), (0,0,0,1));\
   \ns_2:=MatrixRationals{}((1,0,0,0), (-1,-1,-1,0), (0,0,1,0), (0,0,0,1)); \
   \ns_3:=MatrixRationals{}((1,0,0,0), (0,1,0,0), (0,-2,-1,-1), (0,0,0,1)); \
   \ns_4:=MatrixRationals{}((1,0,0,0), (0,1,0,0), (0,0,1,0), (0,0,-1,-1)); ");

  this->AddOperationInnerHandler
  ("FunctionToMatrix", this->innerFunctionToMatrix,"",
   "Creates a matrix from a function. The first argument gives the function, the second argument the number of rows, \
   the third- the number of columns.\
   ", "X:=FunctionToMatrix{}(A,5,5);\n A_({{a}},{{b}}):=a/b;\n X; \\det {} X");
  this->AddOperationInnerHandler
  ("\\det", this->innerDeterminant, "",
   "Tries to convert to a matrix of ratinonals or matrix of rational functions and computes the determinant if\
   not too large. \
   ", "X:=FunctionToMatrix{}(A,5,5);\n A_({{a}},{{b}}):=1/(a+b);\n X; \\det {} X");

/*  this->AddOperationInnerHandler
  ("Union", this->innerUnion, "",
   "Makes a union of the elements of its arguments. Same action as \\cup but different syntax; useful for matrices. ",
   "X:=FunctionToMatrix{}(A,3,4); Union{}X; A{}({{i}},{{j}}):=i*i-j*j; Union{}X ");
  this->AddOperationInnerHandler
  ("UnionNoRepetition", this->innerUnionNoRepetition, "",
   "Same action as \\sqcup (union no repetition) but different syntax; useful for matrices. ",
   "X:=FunctionToMatrix{}(A,3,4); UnionNoRepetition{}X; A{}({{i}},{{j}}):=i*i-j*j; UnionNoRepetition{}X");
*/  this->AddOperationInnerHandler
  ("hwv", this->fHWV, "",
   "Highest weight vector in a generalized Verma module. \
   The first argument gives the semisimple Lie algebra. The second argument \
   gives the highest weight in fundamental coordinates. \
   The third argument parametrizes the parabolic subalgebra, e.g. (1,0,0) stands for a \
   parabolic subalgebra with first simple Vector<Rational> crossed-out. The second argument is allowed to have \
   entries that are not non-negative integers in the positions in which the third argument has 1's. ",
   "g_{{i}}:=getChevalleyGenerator{}(B_3, i);h_{{i}}:=getCartanGenerator{}(B_3, i);\n v_\\mu:=hwv{} (A_3, (1,0,1),(0,0,0)) ;  v_\\lambda:=hwv{}(B_3, (x_1,0,1),(1,0,0));\nh_1g_{-1} v_\\lambda");
  this->AddOperationInnerHandler
  ("hwTAAbf", this->fHWTAABF, "",
   "Highest weight transpose anti-automorphism bilinear form, a.k.a. Shapovalov form. \
   Let M be a Verma module with highest weight vector v given in \
   fundamental coordinates. Let P:M->M\
   be a projection map onto Cv that maps every weight vector of M of weight different from the \
   highest to 0. Let u_1, u_2 be two words in the universal enveloping algebra. Then define hwTAAbf(u_1,u_2):=\
   Tr_M (P ( taa(u_1) u_2 ), where taa() is the transpose anti-automorphism of g. ",
   "g:=SemisimpleLieAlgebra{} G_2;\nhwTAAbf{}(g_{-1} g_{-2}, g_{-1}g_{-2}, (2,2))");
  this->AddOperationInnerHandler
  ("WeylDimFormula", this->fWeylDimFormula, "",
   "Weyl dimension formula. First argument gives the type of the Weyl group of the simple\
    Lie algebra in the form Type_Rank (e.g. E_6).\
   The second argument gives the highest weight in fundamental coordinates. ",
   "WeylDimFormula{}(G_2, (x,0));\nWeylDimFormula{}(B_3, (x,0,0));");
  this->AddOperationInnerHandler
  ("animateLittelmannPaths", this->fAnimateLittelmannPaths, "",
   "Generates all Littelmann-Lakshmibai-Seshadri paths, draws them and animates them. Presented first on the seminar in Charles University Prague. \
   The first argument gives the type of the semisimple  Lie algebra, the second gives the highest weight. \
   ",
   "animateLittelmannPaths{}(G_2, (2,0));");
  this->AddOperationInnerHandler
  ("DecomposeInducingRepGenVermaModule",
   this->fDecomposeFDPartGeneralizedVermaModuleOverLeviPart, "",
   "Decomposes the inducing module of a generalized Verma module over the Levi part of a parabolic smaller than the inducing one.\
   The first argument gives the type of the algebra. The second argument gives the highest weight of the module in \
   fundamental coordinates. The third argument gives the parabolic subalgebra with respect to which we induce. \
   The last argument gives the parabolic subalgebra with respect to whose Levi part we decompose.",
   "DecomposeInducingRepGenVermaModule{}(B_3,(0, 1,1),(1,0,0), (1,0,1))");
  this->AddOperationInnerHandler
  ("Casimir", this->fCasimir, "",
   "Gives the Casimir element. ",
   "Casimir{}(G_2)");
  this->AddOperationInnerHandler
  ("hmmG2inB3", this->fEmbedG2inB3, "",
   "Embeds elements of the Universal enveloping of G_2 in B_3, following an embedding found in\
    a paper by McGovern.",
   "g:=SemisimpleLieAlgebra{}G_2; hmmG2inB3{}(g_1);\nhmmG2inB3{}(g_2) ");
  this->AddOperationInnerHandler
  ("drawWeightSupportWithMults", this->fDrawWeightSupportWithMults, "",
   "Draws the weight support of an irreducible finite-dimensional highest weight module. \
   The first argument gives the type and the second gives the highest weight given in \
   fundamendal weight basis. \
   The maximum number of drawn weights allowed is (hard-code) limited to 10 000. If \
   the number of weights in the weight support exceeds this number, only the first 10 000 weights will be drawn. \
   <b> Warning. Drawing text (i.e. multiplicities) \
   is very javascrtipt-processor-intensive. Use only for *small* examples, else you might hang your browser. </b>",
   "drawWeightSupportWithMults{}(B_3,(0,1,1));\n drawWeightSupportWithMults{}(G_2,(1,0))");
  this->AddOperationInnerHandler
  ("drawWeightSupport", this->fDrawWeightSupport, "",
   "Same as drawWeightSupportWithMults but displays no multiplicities. Same warning for hanging up your browser \
    with javascript holds.",
   "drawWeightSupport{}(B_3,(1,1,1)); drawWeightSupport{}(G_2,(1,2))");
  this->AddOperationInnerHandler
  ("SplitFDpartB3overG2CharsOnly", this->fSplitFDpartB3overG2CharsOnly, "",
   "Splits the finite dimensional part of the inducing module of the generalized Verma module of\
    B_3(so(7)) into G_2-components. \
   The argument is gives the highest weight of the generalized Verma module in fundamental \
   coordinates with respect to so(7). \
   The arguments which are not small integers indicate the non-selected roots of the inducing parabolic\
    subalgebra of B_3. ",
   "SplitFDpartB3overG2CharsOnly{}(x_1,2,0)");
  this->AddOperationInnerHandler
  ("SplitFDpartB3overG2", this->fSplitFDpartB3overG2, "",
   "Splits the finite dimensional part of the inducing module of the generalized Verma module of \
   B_3(so(7)) into G_2-components. \
   The argument is gives the highest weight of the generalized Verma module in fundamental \
   coordinates with respect to so(7). \
   The arguments which are not small integers indicate the non-selected roots of the inducing parabolic \
   subalgebra of B_3. ",
   "SplitFDpartB3overG2{}(x_1,1,0)");
  this->AddOperationInnerHandler
  ("PrintB3G2branchingTableCharsOnly",this->fPrintB3G2branchingTableCharsOnly, "",
   "Creates a table of branching of finite dimensional B_3-modules over G_2. The argument of the \
   function gives the maximum height \
   of the B_3-weight. The second argument indicates the parabolic subalgebra of B_3- zero entry \
   stands for the corresponding root space lying \
   in the Levi part of the parabolic. Non-zero entry means the corresponding negative root space is \
   not in the parabolic. The expression given \
   in that coordinate is used as the corresponding highest weight. ",
   "PrintB3G2branchingTableCharsOnly{}(2, (0,0,0)); PrintB3G2branchingTableCharsOnly{}(2, (x_1,0,0))");
  this->AddOperationInnerHandler
  ("PrintB3G2branchingTable", this->fPrintB3G2branchingTable, "",
   "Creates a table of branching of finite dimensional B_3-modules over G_2. \
    The argument of the function gives the maximum height \
   of the B_3-weight. The function works with arguments 0 or 1; values of 2 or more must be run off-line.",
   "PrintB3G2branchingTable{}(1, (0,0,0)); PrintB3G2branchingTable{}(1, (x_1,0,0))");
  this->AddOperationInnerHandler
  ("SplitFDTensorGenericGeneralizedVerma", this->fSplitGenericGenVermaTensorFD, "",
   "Experimental, please don't use. Splits generic generalized Verma module tensor finite dimensional module. ",
   "SplitFDTensorGenericGeneralizedVerma{}(G_2, (1, 0), (x_1, x_2)); ");
  this->AddOperationInnerHandler
  ("WriteGenVermaAsDiffOperatorsUpToWeightLevel",
   this->fWriteGenVermaModAsDiffOperatorUpToLevel, "",
   "<b>Experimental, please don't use</b>. Embeds a Lie algebra \
   in the Weyl algebra with matrix coefficients.\
   The third argument gives the highest weight. \
   The non-zero entries of the highest weight give the\
   root spaces outside of the Levi part of the parabolic. \
   The second argument gives the weight level to which the computation should be carried out",
   "WriteGenVermaAsDiffOperatorsUpToWeightLevel{}(B_3, 1, (0, 0, x_3)); ");
  this->AddOperationInnerHandler
  ("EmbedSSalgebraInWeylAlgebra", this->fWriteGenVermaModAsDiffOperators, "",
   "Experimental, please don't use. Embeds a Lie algebra \
   in the Weyl algebra with matrix coefficients.\
   The second argument gives the highest weight. \
   The third argument gives the\
   parabolic subalgebra selection. \
   The first argument gives the type of the Lie algebra. ",
   "EmbedSSalgebraInWeylAlgebra{}(B_3, (0,0,0), (0, 0, 1)); ");
  this->AddOperationInnerHandler
  ("WeylOrbitSimpleCoords", this->fWeylOrbitSimple, "",
   "Generates a Weyl orbit printout from simple coords.\
    First argument = type. Second argument = weight in simple coords. The orbit size is cut off at max 1920 elements (type D_5).",
   "WeylOrbitSimpleCoords{}(B_2, (y, y));");
  this->AddOperationInnerHandler
  ("WeylOrbitFundCoords", this->fWeylOrbitFund, "",
   "Generates a Weyl orbit printout from fundamental coords.\
    First argument = type. Second argument = weight in fundamental coords. ",
   "WeylOrbitFundCoords{}(B_2, (y, 0));");
  this->AddOperationInnerHandler
  ("WeylOrbitFundRho", this->fWeylOrbitFundRho, "",
   "Generates a Weyl orbit printout from fundamental coords.\
    First argument = type. Second argument = weight in fundamental coords. Doing the rho-modified action. ",
   "WeylOrbitFundRho{}(B_2, (y, 0) )");
  this->AddOperationInnerHandler
  ("KLcoeffs", this->fKLcoeffs, "",
   "Computes the n by n tables of 1) Kazhdan-Lusztig polynomials, 2) R polynomials and 3) Kazhdan-Lusztig \
   polynomials evaluated at one, where n<=192  is the size of the Weyl group (i.e. no larger than D_4(so(8)).\
   The notation follows the original paper by Kazhdan and Lusztig, \"\
   Representations of Coxeter Groups and Hecke Algebras\"\
   . The algorithm is directly derived from formulas (2.0a-c) there, as explained in \
   the Wikipedia page on Kazhdan-Lusztig polynomials. \
   Please note that the 192 by 192 element table takes almost 3 minutes to compute.\
   Faster implementations of the KL polynomials are available from programs by Fokko du Cloux and others\
   (our simple implementation stores the full table of R-polynomials and KL-polynomials in RAM memory at \
   all times, unlike\
   the other more efficient implementations).",
   "KLcoeffs{}(B_3)");
  this->AddOperationInnerHandler
  ("printRootSubalgebras", this->fprintRootSAs, "",
   "Prints sl(2) subalgebras and root subalgebras. \
   The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
   "printRootSubalgebras{}(E_6)");
  this->AddOperationInnerHandler
  ("printSlTwoSubalgebras", this->fprintSltwos, "",
   "Prints sl(2) subalgebras and root subalgebras. \
   The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
   "printSlTwoSubalgebras{}(E_6)");

   this->AddOperationInnerHandler
  ("parabolicsInfoBruhatGraph", this->fParabolicWeylGroupsBruhatGraph, "",
   " Makes a table with information about the Weyl group of a parabolic subalgebra of the ambient Lie algebra, \
   as well as the cosets \
   (given by minimal coset representatives) of the Weyl subgroup in question. \
   The input must have as many integers as there are simple roots in the ambient \
   Lie algebra. If the root is crossed out (i.e. not a root space of the Levi part), one should put a 1 in the corresponding \
   coordinate. \
   Otherwise, one should put 0. For example, for Lie algebra B3(so(7)), \
   calling parabolicsInfoBruhatGraph(0,0,0) gives you the Weyl group info for the entire algebra; \
   calling parabolicsInfoBruhatGraph(1,0,0) gives you info for the Weyl subgroup generated by \
   the last two simple root. \
   In the produced graph, the element s_{\\eta_i} corresponds to a reflection with respect to the i^th simple root. \
   You will get your output as a .png file link, you must click onto the link to see the end result. \
   <b>Please do not use for subalgebras larger than B_4 (so(9)). The vpf program has no problem handling this \
   function up to E_6 but LaTeX crashes trying to process the output. </b>",
   "parabolicsInfoBruhatGraph{}(B_3,(1,0,0),(1,0,0))");
  this->AddOperationInnerHandler
  ("fTestMonomialBasisConjecture", this->fTestMonomialBaseConjecture, "",
   "Tests the monomial basis conjecture from the Jackson-Milev paper. First argument gives rank bound. \
    Second argument gives dimension bound. ",
   "fTestMonomialBasisConjecture{}(2, 50)");
/*  this->AddOperationInnerHandler
  ("DecomposeCharGenVermaToIrreps", & this->fDecomposeCharGenVerma, "",
   "<b>This is an experiment; I have no mathematical proof that this function works. \
   If you are seeing this function then I forgot to comment it out before updating the calculator. \
   Please don't use this function.</b> Decomposes the \
   character of a generalized Verma module as a sum of characters of irreducible highest weight \
    modules.\
    First argument = type. Second argument = highest weight. Non-integer entries give parabolic selection. ",
   "DecomposeCharGenVermaToIrreps{}(G_2, (x_1, 0))");*/
  this->AddOperationInnerHandler
  ("LSpath", this->fLSPath, "",
   "Lakshmibai-Seshadri path starting at 0. The first argument gives the semisimple Lie algebra, \
    the next arguments give the waypoints of the path.",
   "LSpath{}(G_2, (0,0), (2,1) )");
  this->AddOperationInnerHandler
  ("LROdefine", this->fLittelmannOperator, "",
   "Littelmann root operator e_i, where e_i is the Littelmann root operator with \
   respect to root of index i. If i is negative then the e_i root operator is defined to be \
   the f_\alpha operator.",
   "e_{{i}}:=LROdefine_i; e_{-1} e_{-1} LSpath{}(G_2, (0,0), (2,1))");
  this->AddOperationInnerHandler
  ("InvertMatrixVerbose", this->fInvertMatrix, "",
   "Inverts a matrix of rationals if invertible, in any other case generates an error. Makes a detailed \
   printout of all Gaussian elimantion steps. Originally intended for demonstrations to linear algebra\
   / calculus students. ",
   "InvertMatrixVerbose{}((1,2),(2,3))");
  this->AddOperationInnerHandler
  ("SolveNseparableQuasiLinearSystem", this->fSolveSeparableBilinearSystem, "",
   "Attempts to solve a polynomial system in n+m variables such that it is a linear system \
   in the first n variables with coefficients in the polynomial \
   ring generated by the remaining m  variables. \
   The function takes as argument the list of polynomials of the system. \
   The variables are ordered   in lexicographic order; suppose the variables are x_1<...< x_p. \
   Then the variables are split in two buckets. The first bucket has those x_i \
   such that x_i appears raised only to first power\
   such that the coefficient of x_i is a polynomial in the variables x_q>x_i, and all others. \
   Then the variables in the first bucket are assumed to be the first n variables. ",
   "SolveNseparableQuasiLinearSystem{}(\
   x_{12}x_{24}-x_{10}x_{22}-2x_{8}x_{20}-x_{7}x_{19}+1, \
x_{11}x_{24}-x_{10}x_{23}-x_{8}x_{21},\
x_{9}x_{24}-x_{8}x_{23}+x_{7}x_{21},\
x_{6}x_{24}+2x_{5}x_{23}-x_{4}x_{22}+2x_{3}x_{21}-2x_{2}x_{20}-x_{1}x_{19},\
x_{12}x_{23}-x_{11}x_{22}-x_{9}x_{20},\
x_{11}x_{23}+x_{10}x_{22}+x_{8}x_{20}-1,\
x_{9}x_{23}+x_{8}x_{22}-x_{7}x_{20},\
x_{12}x_{21}-x_{11}x_{20}+x_{9}x_{19},\
x_{11}x_{21}+x_{10}x_{20}-x_{8}x_{19},\
x_{9}x_{21}+x_{8}x_{20}+x_{7}x_{19}-1,\
x_{12}x_{18}+2x_{11}x_{17}-x_{10}x_{16}+2x_{9}x_{15}-2x_{8}x_{14}-x_{7}x_{13},\
x_{6}x_{18}-x_{4}x_{16}-2x_{2}x_{14}-x_{1}x_{13}+1,\
x_{5}x_{18}+x_{4}x_{17}+x_{2}x_{15},\
x_{3}x_{18}+x_{2}x_{17}-x_{1}x_{15},\
x_{6}x_{17}+x_{5}x_{16}+x_{3}x_{14},\
x_{5}x_{17}+x_{4}x_{16}+x_{2}x_{14}-1,\
x_{3}x_{17}+x_{2}x_{16}-x_{1}x_{14},\
x_{6}x_{15}+x_{5}x_{14}-x_{3}x_{13},\
x_{5}x_{15}+x_{4}x_{14}-x_{2}x_{13},\
x_{3}x_{15}+x_{2}x_{14}+x_{1}x_{13}-1)", false);

  this->AddOperationInnerHandler
  ("GroebnerLexUpperLimit", this->fGroebnerLex, "",
   "Transforms to a reduced Groebner basis using the  \
    lexicographic order. \
    The lexicographic order is inherited from the comparison of the underlying expressions.\
    <b>The first argument gives an upper bound to the number of polynomial operations allowed.</b> \
    A non-positive number signifies no upper limit, however please do not use (this is a public \
    web server and multiple instances of a large computation might hog it up). \
    The resulting printout will let your know whether the upper limit was hit or not. \
    <br>\
    <b>Description of the algorithm.</b>\
    Let f_1 and f_2 be two polynomials. Following Cox, Little, O'Shea, \
    \"Ideals, Varieties, Algorithms\", page 81,\
    denote by S(f_1, f_2) the symmetric difference of f_1 and f_2. More precisely, let\
    x^{\\gamma_1} be the leading monomial of f_1 and x^{\\gamma_2} \
    be the leading monomial of f_2, with leading coefficients c_1 and c_2. Then define \
    S(f_1, f_2):= c_2* f_1* lcm (x^\\gamma_1, \\gamma_2)/x^\\gamma_1 -\
    c_1* f_2 *lcm (x^\\gamma_1, x^\\gamma_2)/x^\\gamma_2. \
    Here lcm stands for least common multiple of monomials, defined in the obvious way. \
    <br>\n1. Allocate two buckets of polynomials - one \"main\" bucket and\
    additional bucket. At any given momonet in the algorithm, \
    the union of main bucket and the additional bucket should give a basis of the ideal.  \
    <br>\n2. Move all input elements into the additional bucket.\
    <br>\n3. Call the <b>reduce additional bucket</b> subroutine. In the c++ implementation\
    the function is  called GroebnerBasisComputation::AddPolyAndReduceBasis.\
    <br>\n4. Set changed to be true.\
    <br>\n5. While (changed)\
    <br>\n5.1 For all elements f_1, f_2 in the main bucket form the symmetric difference \
    S(f_1, f_2) and add it to the additional bucket. \
    <br>\n5.2 Call the <b>reduce additional bucket</b> subroutine.  \
    and set changed to be equal to the result of the subroutine.\
    <br><b>Reduce additional bucket</b> subroutine.\
    <br>\n1. Set changedMainBucket to be false.\
    <br>\n2.While (additional bucket is not empty)\
    <br>\n2.1. While (additional bucket is not empty)\
    <br>\n2.1.1 Remove the top element from the additional bucket; call that element currentElement.\
    <br>\n2.1.2 Divide currentElement by the elements of the main bucket, and \
    record the resulting remainder element in currentRemainder.\
    Here we use the multivariate \
    polynomial divis ion algorithm, page 62 of Cox, Little, O'Shea, \
    \"Ideals, Varieties, Algorithms\".\
    <br>\n2.1.3 If currentRemainder is non-zero, add it to the main bucket \
    and set changedMainBucket to be true. \
    <br>\n2.2 For each element in the main bucket \
    <br>\n2.2.1 Call the considered element currentElement.\
    <br>\n2.2.2 Divide currentElement by the remaining elements (excluding currentElement itself)\
    in the main bucket. Call the remainder of the division currentRemainder.\
    <br>\n2.2.3 If currentRemainder is not equal to currentElement, \
    remove currentElement from the main bucket and add currentRemainder to the additional bucket. \
    Note that this operation modifies the main bucket: each element of the main bucket must be\
    traversed exactly once by the current cycle, but the division is carried with \
    the modified state of the main bucket. \
    <br>\n3. Return changedMainBucket. \
   ",
   "GroebnerLexUpperLimit{}(10000, s^2+c^2+1, a-s^4, b-c^4 );\
   \nGroebnerLexUpperLimit{}(5, s^2+c^2+1, a-s^4, b-c^4 );");
  this->AddOperationInnerHandler
  ("GroebnerGrLexUpperLimit", this->fGroebnerGrLex, "",
   "Transforms to a reduced Groebner basis relative to the graded \
   lexicographic order. In the graded lexicographic order, monomials are first compared by\
   total degree, then by lexicographic order. \
   The lexicographic order is inherited from the comparison of the underlying expressions. \
   <b>The first argument gives an upper bound to the number of polynomial operations allowed.</b> \
   A non-positive number signifies no upper limit, however please do not use (this is a public \
   web server and multiple instances of a large computation might hog it up). \
   The resulting printout will let your know whether the upper limit was hit or not. \
   For a description of the algorithm used see the description of function GroebnerLexUpperLimit.",
   "GroebnerGrLexUpperLimit{}(10000, a^2+b^2+1, x-a^4, y-b^4 );\n \
   GroebnerGrLexUpperLimit{}(5, a^2+b^2+1, x-a^4, y-b^4 )");
  this->AddOperationInnerHandler
  ("experimentalPrintSemisimpleSubalgebras", this->fSSsubalgebras, "",
   " <b>This function is being developed and is not imiplemented fully yet. </b> \
   Prints the semisimple subalgebras of a semisimple Lie algebra. ",
   "experimentalPrintSemisimpleSubalgebras{}(A_2)", true);
  this->AddOperationInnerHandler
  ("experimentalEmbedSemisimpleInSemisimple", this->fEmbedSSalgInSSalg, "",
   " <b>This function is being developed and is not imiplemented fully yet. </b> \
   Prints all embeddings of the first subalgebra into the second. ",
   "experimentalEmbedSemisimpleInSemisimple{}(G_2, B_3)", true);
//     this->AddOperationInnerHandler
//  ("printAllPartitions", & this->fPrintAllPartitions, "",
//   "Prints (Kostant) partitions . ",
//   "printAllPartitions{}(A_2, (2,3))");
//  this->AddOperationInnerHandler
//  ("Differential", & this->fDifferential, "",
//   "Differential. ",
//   "d{}{{a}}:=Differential{}a;\nx:=Polynomial{}x;\nd{}(x^2/(x+1))");
//  this->AddOperationInnerHandler
//  ("\\sqrt", & this->fSqrt, "",
//   "Square root of a rational, implemented as algebraic extension of the rationals. ",
//   "\\sqrt{}{3+2\\sqrt{}2}");

  this->AddOperationInnerHandler
  ("FactorOneVarPolyOverRationals", this->fFactor, "",
   "Factors a one variable polynomial over the rationals using Kroenecker's method. \
    After clearing denominators, assume the poly has integer coefficients.\
    If looking for an integer coefficient divisor of degree k, \
    plug in k different integer values of the poly\
    and finds the set of all possible divisors of the value of poly at the k points.\
    this gives a finite set of possibilities for the divisors, as interpolated by\
    Lagrange polynomials.",
   "FactorOneVarPolyOverRationals{}{x^2-4}", false);
  this->AddOperationInnerHandler
  ("Freudenthal", this->fFreudenthalEval, "",
   "Computes the dominant weights with multiplicities of a finite dimensional \
   module of a highest weight given in fundamental coordinates. The first argument gives \
   the semisimple Lie algebra type, the second argument gives the highest weight in \
   fundamental coordinates. ",
   "Freudenthal{}(B_3, (2,2,2))", true)
   ;

//  this->AddOperationInnerHandler
//  ("minPoly", & this->fMinPoly, "",
//   "If the argument of the function is an algebraic number returns its minimal polynomial, else does nothing. ",
//   "minPoly{}(\\sqrt{}2+\\sqrt{}3)");
  this->NumPredefinedVars=this->operationS.size;
}

void CommandList::initPredefinedStandardOperations()
{ //IMPORTANT.
  //The order of registration of operation handlers for the same operation name
  // defines the order in which operation
  //handlers are called. Operations handlers registered first are executed first.
  //The order only matters for different handlers of the same operation.
  //This order is very important, as some of the
  //handlers will act properly only if the preceding ones have been carried through
  //(example: outerExtractBaseMultiplication requires outerAssociate).
  //Note that one can easily transform the code so that the order does not matter.
  //One can do that by ``packing'' the the correct order of operations into a super-function,
  //or by making the corresponding handlers call each other as needed.
  //A combination of these two was indeed the original design approach, however what is used now
  //is more modular, conceptual, and easier to test: in short, the better engineering solution.
  this->AddOperationBinaryInnerHandlerWithTypes
  ("+", this->innerAddRatToRat, this->opRational(), this->opRational(),
   "Adds two rational numbers. ",
   "2+3", true);
  this->AddOperationOuterHandler
  ("+", this->outerPlus, "",
   "Collects all terms (over the rationals), adding up terms proportional up to a rational number. \
    Zero summands are removed, unless zero is the only term left. ", "1+a-2a_1+1/2+a_1", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("+", this->innerAddEltTensorToEltTensor, this->opElementTensorGVM(), this->opElementTensorGVM(),
   "Adds two elements of tensor products of generalized Verma modules. ",
   "v:=hwv{}(G_2, (1,0),(0,0));\
   \n(3/4 v)\\otimes v-3/4 (v\\otimes v)", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("+", this->innerAddPlotToPlot, this->opCalculusPlot(), this->opCalculusPlot(),
   "Superimposes two plots. ",
   "plot2D{}(sin{}(x), -5, 5)+ plot2D{}(1/sin{}(x ), 0.01, 3.14)", true);

  this->AddOperationOuterHandler
  ("-", this->outerMinus, "",
   "Transforms a-b to a+(-1)*b and -b to (-1)*b. Equivalent to a rule \
   -{{b}}:=MinnusOne*b; {{a}}-{{b}}:=a+MinnusOne*b", "-1+(-5)", true);

  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyRatByRat, this->opRational(), this->opRational(),
   "Multiplies two rationals. ",
   "2*3", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyRatOrPolyByRatOrPoly, this->opPoly(), this->opRational(),
   "Multiplies polynomial by a rational (polynomial comes first). ",
   "2*Polynomial{}(a+b);\nPolynomial{}(a+b)/2;\nPolynomial{}((a+b)^3)*Polynomial{}((a+c)^3);", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyRatOrPolyByRatOrPoly, this->opRational(), this->opPoly(),
   "Multiplies two rational by a polynomial (rational comes first). ",
   "2*Polynomial{}(a+b);\nPolynomial{}(a+b)/2;\nPolynomial{}((a+b)^3)*Polynomial{}((a+c)^3);", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyRatOrPolyByRatOrPoly, this->opPoly(), this->opPoly(),
   "Multiplies two polynomials. ",
   "2*Polynomial{}(a+b);\nPolynomial{}(a+b)/2;\nPolynomial{}((a+b)^3)*Polynomial{}((a+c)^3);", true);
  this->AddOperationOuterHandler
  ("*", this->outerDistributeTimes, "",
   "Distributive law (left and right).",
   "(a+b)*c; \n a*(b+c)", true);
  this->AddOperationOuterHandler
  ("*", this->outerAssociate, "",
   "Associative law: reorders the multiplicative tree in standard form. ",
   "(a*b)*(c*(d*e)*f) - a*b*c*d*e*f;(a*b)*(c*(e*d)*f) - a*b*c*d*e*f", true);
  this->AddOperationOuterHandler
  ("*", this->outerExtractBaseMultiplication, "",
   "Pulls rationals in the front of multiplicative terms.",
   "2*((3*c)*(4*d)); 3*((a*(d-d))b*c)", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyAnyByEltTensor, this->opRational(), this->opElementTensorGVM(),
   "Handles multiplying rational number by an element of tensor product of generalized Verma modules. \
   Not fully tested and documented at the moment.  \
   Will get more documented in the future. ",
   "X:=G_2;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X, i);  \
   \nv:=hwv{}(G_2, (1,0),(0,0));\
   \n2/5 v;\n(3/4 v)\\otimes v;\n3/4 (v\\otimes v);\n(3/4 v)\\otimes v-3/4 (v\\otimes v)", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyAnyByEltTensor, this->opPoly(), this->opElementTensorGVM(),
   "Handles multiplying polynomial by an element of tensor product of generalized Verma modules. \
   Not fully tested and documented at the moment.  \
   Will get more documented in the future. ",
   "X:=G_2;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X, i);  \
   \nz:=Polynomial{}y;\
   \nv:=hwv{}(G_2, (z,1),(1,0));\
   \n(2*z) v;\n", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyAnyByEltTensor, this->opRationalFunction(), this->opElementTensorGVM(),
   "Handles multiplying rational function number by an element of tensor product of generalized Verma modules. \
   Not fully tested and documented at the moment.  \
   Will get more documented in the future. ",
   "X:=G_2;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X, i);  \
   \nz:=Polynomial{}y;\nv:=hwv{}(G_2, (z,1),(1,0));\
   \n1/z v", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("*", this->innerMultiplyAnyByEltTensor, this->opElementUEoverRF(), this->opElementTensorGVM(),
   "Handles acting by element Universal enveloping on an element of tensor product of generalized Verma modules. \
   Not fully tested and documented at the moment.  \
   Will get more documented in the future. ",
   "X:=G_2;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X, i);  \
   \nz:=Polynomial{}y;\nv:=hwv{}(G_2, (z,1),(1,0));\
   \n h_1 v; \nh_2 v;\n g_1 g_{-1} v ", true);

  this->AddOperationOuterHandler
  ("/", this->outerDivide, "",
    "If b is rational substitutes (anything)/b with anything* (1/b).",
    "6/15+(a+b)/5; 6/4+3/0", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("/", this->innerDivideRatByRat, this->opRational(), this->opRational(),
   "Divides two rational numbers. ",
   "4/6; 2/0;", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("/", this->innerDivideRFOrPolyOrRatByRFOrPoly, this->opRational(), this->opPoly(),
   "Divides rational by polynomial (to get a rational function).",
   "z:=Polynomial{}(x^2+y^2);\n1/z", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("/", this->innerDivideRFOrPolyOrRatByRFOrPoly, this->opPoly(), this->opPoly(),
   "Divides polynomial by polynomial (to get a rational function). ",
   "Polynomial{}(-x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}+x_{2}+1)/\
   \nPolynomial{}(x_{1}^{2}x_{2}x_{3}-x_{1}^{2}x_{3}-x_{2}+1) ", true);
  this->AddOperationBinaryInnerHandlerWithTypes
  ("\\otimes", this->innerTensorEltTensorByEltTensor, this->opElementTensorGVM(), this->opElementTensorGVM(),
   "Tensor product of two generalized Verma modules. \
   Not fully tested and documented at the moment.  \
   Will get more documented in the future. ",
   "X:=G_2;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X, i);  \
   \nz:=Polynomial{}y;\nv:=hwv{}(G_2, (z,1),(1,0));\
   \ng_{-1}(v\\otimes v);\
   \ng_{-1}g_{-1}(v\\otimes v)", true);

  this->AddOperationOuterHandler
  ("\\otimes", this->outerTensor, "",
   "Please do note use (or use at your own risk): this is work-in-progress. \
   Will be documented when implemented and tested. Tensor product of \
   generalized Verma modules. ",
   "X:=G_2;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X,i);  \
   \nv:=hwv{}(X, (1,0),(0,0));\
   \ng_{-1}(v\\otimes v);\
   \ng_{-1}g_{-1}(v\\otimes v); ", true);

  this->AddOperationOuterHandler
  ("[]", this->outerLieBracket, "",
   "Lie bracket.",
   "X:=A_1;\ng_{{i}}:=getChevalleyGenerator{}(X,i);\nh_{{i}}:=getCartanGenerator{}(X,i);\n[g_1,g_{-1}] ", true);
  this->AddOperationOuterHandler
  (":=:", this->StandardIsDenotedBy, "", "The operation :=: is the \"is denoted by\" operation. \
   The expression a:=:b always reduces to \
   a:=b. In addition to the transformation, the pair of expressions a,b is registered in a \
   special global \"registry\". This has the following effect. Every time \
   the expression b is met, it is displayed on the screen as a. We note that subsequent \
   occurrences of the expression a will first be replaced by b (as mandated\
   by the a:=b command), but then displayed on the screen as a.", "x:=:y;\ny;\nz;\nz:=y;\nz ", true);
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
  this->AddOperationOuterHandler
  ("==", this->outerEqualEqual, "",
   "If either the left or the right argument contains a bound variable does nothing. \
    Else evaluates to 1 if the left argument equals the right argument.",
   "x==y;\nx==1;\nIsEqualToX{} {{a}}:=a==x;\nIsEqualToX{}y;\nIsEqualToX{}x;\
   \nIsEqualToX{}1;\nx:=1;\nIsEqualToX{}1", true);
  this->AddOperationOuterHandler
  ("\\cup", this->outerUnion, "",
   "If all arguments of \\cup are of type list, substitutes the expression with \
   a list containing \
   the union of all members (with repetition).",
   "x\\cup MakeSequence{} x \\cup MakeSequence{}x \\cup (a,b,x)", true);
  this->AddOperationOuterHandler
  ("\\sqcup", this->outerUnionNoRepetition, "",
   "If all arguments of \\sqcup are of type list, substitutes the expression with a list \
   containing \
   the union of all members; all repeating members are discarded.",
   "(x,y,x)\\sqcup(1,x,y,2)", true);
}
