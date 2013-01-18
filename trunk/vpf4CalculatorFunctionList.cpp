//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf4cpp(__FILE__, "List of calculator functions. ");
//This file lists calculator funcitons only. Please do not use for any other purposes.

void CommandList::initPredefinedVars()
{ this->AddOperationMustBeNew
("gcdPoly", &this->fGCD, 0,  "",
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
  this->AddOperationMustBeNew
("lcmPoly", &this->fLCM, 0, "",
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
  this->AddOperationMustBeNew
("PolyDivQuotient", &this->fPolynomialDivisionQuotient, 0, "",
   "Quotient of two polynomials under division with remainder. ",
   "a:=Polynomial{}(x_1^3x_2+x_1^2x_2^3x_3);\nb:=Polynomial{}(x_1^2+x_3);\
   \nq:=PolyDivQuotient{}(a,b); \nr:=PolyDivRemainder{}(a,b); \na-q*b-r;", false)
   ;
  this->AddOperationMustBeNew
("PolyDivRemainder", &this->fPolynomialDivisionRemainder, 0, "",
   "Remainder of polynomial division under division with remainder. ",
   "a:=Polynomial{}(x_1^3x_2+x_1^2x_2^3x_3);\n b:=Polynomial{}(x_1^2+x_3);\
   \nq:=PolyDivQuotient{}(a,b); \nr:=PolyDivRemainder{}(a,b); \na-q*b-r;", false)
   ;
  this->AddOperationMustBeNew
  ("suffixNotationForPostScript", this->fSuffixNotationForPostScript, 0, "",
   "Suffix notation. for postscript, used to quickly generate pstricks drawings in LaTeX.  \
   ",
   "suffixNotationForPostScript{}((1/3 +a+b)*c)")
   ;
this->AddOperationMustBeNew ("drawPolar", this->fDrawPolarRfunctionTheta, 0, "",
   "Draws polar curve given in polar coordinates in the form \
   r=f(t), where t is the angle variable. The angle variable is measured in degrees. \
   The first argument gives the function, the second and third argument give the upper and \
   lower bounds of the angle. \
   ",
   "drawPolar{}(1+sin {} t, 0, 360)")
   ;

  this->AddOperationMustBeNew
("IsInteger", &this->fIsInteger,0, "",
 " If the argument has no bound variables, returns 1 if the argument is an integer, 0 otherwise. ",
 "IsInteger{}a;\nIsInteger{}1;\nf{}{{a}}:=IsInteger{}a;\nf{}1;\nf{}b");
  this->AddOperationMustBeNew
  ("SemisimpleLieAlgebra", &this->fSSAlgebraShort, 0, "",
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
    <br> A semisimple Lie algebra is treated as a function \
    that returns the elements of a semisimple Lie algebra.  \
    Let h_i:=(SemisimpleLieAlgebra{}G_2)_(0,i). Then h_i stands for the \
    element of the Cartan dual to the i^th root, i.e., for the element which \
    has the same action on a root space as the scalar product with the i^th simple root.\
    Let g_i:=(SemisimpleLieAlgebra{}G_2)_(i). Then g_i stands for the i^th Chevalley-Weyl \
    generator, where the Chevalley-Weyl generators are ordered according to the graded \
    lexicographic order in the simple coordinates of the roots. \
    The indices run in the ranges -r,..., -1 and 1,..., r, \
    where r is the number of roots in the root system. \
    The roots of indices 1,..., k are the simple roots, where k is the rank of the Lie algebra. \
    The labeling scheme is best illustated by running the function printSemisimpleLieAlgebra. ",
   "g:=SemisimpleLieAlgebra{}G_2; g_1; g_2; g_{0,1}; [[g_1,g_2], [g_{-1}, g_{-2}]]");
  this->AddOperationMustBeNew
  ("printSemisimpleLieAlgebra", &this->fSSAlgebraVerbose, 0, "",
   "Creates a quite detailed \
   printout with information about \
   the semisimple Lie algebra, including the Lie bracket pairing table. \
   In addition, this function creates a graphics of the root system. Warning. \
   For E_8 is quite heavy and might put a slower computer to sleep.  \
   ",
   "printSemisimpleLieAlgebra{}(F_4);\nprintSemisimpleLieAlgebra{}(2G^5_2+B_3);");
  this->AddOperationMustBeNew
  ("Polynomial", this->fPolynomial, 0, "",
   "Creates an atomic representation of a polynomial expression. ",
   "Polynomial{}((x+y+z)^2)");
  this->AddOperationMustBeNew
  ("UE", this->fElementUniversalEnvelopingAlgebra, 0, "",
   "Creates an atomic representation of an element of a universal enveloping algebra.",
   "g:=SemisimpleLieAlgebra{} G_2;\nUE{}(g_1g_2-g_2g_1+g_3^2)");
  this->AddOperationMustBeNew
  ("FunctionToMatrix", this->fMatrix, 0,"",
   "Creates a matrix from a function. The first argument gives the function, the second argument the number of rows, \
   the third- the number of columns.\
   ", "X:=FunctionToMatrix{}(A,5,6);\n A{}({{a}},{{b}}):=a+b;\n X;");
  this->AddOperationMustBeNew
  ("Union", this->fUnion, 0, "",
   "Makes a union of the elements of its arguments. Same action as \\cup but different syntax; useful for matrices. ",
   "X:=FunctionToMatrix{}(A,3,4); Union{}X; A{}({{i}},{{j}}):=i*i-j*j; Union{}X ");
  this->AddOperationMustBeNew
  ("UnionNoRepetition", this->fUnionNoRepetition, 0, "",
   "Same action as \\sqcup (union no repetition) but different syntax; useful for matrices. ",
   "X:=FunctionToMatrix{}(A,3,4); UnionNoRepetition{}X; A{}({{i}},{{j}}):=i*i-j*j; UnionNoRepetition{}X");
  this->AddOperationMustBeNew
  ("hwv", this->fHWV, 0, "",
   "Highest weight vector in a generalized Verma module. \
   The first argument gives the semisimple Lie algebra. The second argument \
   gives the highest weight in fundamental coordinates. \
   The third argument parametrizes the parabolic subalgebra, e.g. (1,0,0) stands for a \
   parabolic subalgebra with first simple Vector<Rational> crossed-out. The second argument is allowed to have \
   entries that are not non-negative integers in the positions in which the third argument has 1's. ",
   "g:=SemisimpleLieAlgebra{} B_3;\n v_\\mu:=hwv{} (A_3, (1,0,1),(0,0,0)) ;  v_\\lambda:=hwv{}(B_3, (x_1,0,1),(1,0,0));\ng_{0,1}g_{-1} v_\\lambda");
  this->AddOperationMustBeNew
  ("hwTAAbf", this->fHWTAABF, 0, "",
   "Highest weight transpose anti-automorphism bilinear form, a.k.a. Shapovalov form. \
   Let M be a Verma module with highest weight vector v given in \
   fundamental coordinates. Let P:M->M\
   be a projection map onto Cv that maps every weight vector of M of weight different from the \
   highest to 0. Let u_1, u_2 be two words in the universal enveloping algebra. Then define hwTAAbf(u_1,u_2):=\
   Tr_M (P ( taa(u_1) u_2 ), where taa() is the transpose anti-automorphism of g. ",
   "g:=SemisimpleLieAlgebra{} G_2;\nhwTAAbf{}(g_{-1} g_{-2}, g_{-1}g_{-2}, (2,2))");
  this->AddOperationMustBeNew
  ("WeylDimFormula", this->fWeylDimFormula, 0, "",
   "Weyl dimension formula. First argument gives the type of the Weyl group of the simple\
    Lie algebra in the form Type_Rank (e.g. E_6).\
   The second argument gives the highest weight in fundamental coordinates. ",
   "WeylDimFormula{}(G_2, (x,0));\nWeylDimFormula{}(B_3, (x,0,0));");
  this->AddOperationMustBeNew
  ("animateLittelmannPaths", this->fAnimateLittelmannPaths, 0, "",
   "Generates all Littelmann-Lakshmibai-Seshadri paths, draws them and animates them. Presented first on the seminar in Charles University Prague. \
   The first argument gives the type of the semisimple  Lie algebra, the second gives the highest weight. \
   ",
   "animateLittelmannPaths{}(G_2, (2,0));");
  this->AddOperationMustBeNew
  ("DecomposeInducingRepGenVermaModule",
   this->fDecomposeFDPartGeneralizedVermaModuleOverLeviPart, 0, "",
   "Decomposes the inducing module of a generalized Verma module over the Levi part of a parabolic smaller than the inducing one.\
   The first argument gives the type of the algebra. The second argument gives the highest weight of the module in \
   fundamental coordinates. The third argument gives the parabolic subalgebra with respect to which we induce. \
   The last argument gives the parabolic subalgebra with respect to whose Levi part we decompose.",
   "DecomposeInducingRepGenVermaModule{}(B_3,(0, 1,1),(1,0,0), (1,0,1))");
  this->AddOperationMustBeNew
  ("Casimir", this->fCasimir, 0, "",
   "Gives the Casimir element. ",
   "Casimir{}(G_2)");
  this->AddOperationMustBeNew
  ("hmmG2inB3", this->fEmbedG2inB3, 0, "",
   "Embeds elements of the Universal enveloping of G_2 in B_3, following an embedding found in\
    a paper by McGovern.",
   "g:=SemisimpleLieAlgebra{}G_2; hmmG2inB3{}(g_1);\nhmmG2inB3{}(g_2) ");
  this->AddOperationMustBeNew
  ("drawWeightSupportWithMults", this->fDrawWeightSupportWithMults, 0, "",
   "Draws the weight support of an irreducible finite-dimensional highest weight module. \
   The first argument gives the type and the second gives the highest weight given in \
   fundamendal weight basis. \
   The maximum number of drawn weights allowed is (hard-code) limited to 10 000. If \
   the number of weights in the weight support exceeds this number, only the first 10 000 weights will be drawn. \
   <b> Warning. Drawing text (i.e. multiplicities) \
   is very javascrtipt-processor-intensive. Use only for *small* examples, else you might hang your browser. </b>",
   "drawWeightSupportWithMults{}(B_3,(0,1,1));\n drawWeightSupportWithMults{}(G_2,(1,0))");
  this->AddOperationMustBeNew
  ("drawWeightSupport", this->fDrawWeightSupport, 0, "",
   "Same as drawWeightSupportWithMults but displays no multiplicities. Same warning for hanging up your browser \
    with javascript holds.",
   "drawWeightSupport{}(B_3,(1,1,1)); drawWeightSupport{}(G_2,(1,2))");
  this->AddOperationMustBeNew
  ("SplitFDpartB3overG2CharsOnly", this->fSplitFDpartB3overG2CharsOnly, 0, "",
   "Splits the finite dimensional part of the inducing module of the generalized Verma module of\
    B_3(so(7)) into G_2-components. \
   The argument is gives the highest weight of the generalized Verma module in fundamental \
   coordinates with respect to so(7). \
   The arguments which are not small integers indicate the non-selected roots of the inducing parabolic\
    subalgebra of B_3. ",
   "SplitFDpartB3overG2CharsOnly{}(x_1,2,0)");
  this->AddOperationMustBeNew
  ("SplitFDpartB3overG2", this->fSplitFDpartB3overG2, 0, "",
   "Splits the finite dimensional part of the inducing module of the generalized Verma module of \
   B_3(so(7)) into G_2-components. \
   The argument is gives the highest weight of the generalized Verma module in fundamental \
   coordinates with respect to so(7). \
   The arguments which are not small integers indicate the non-selected roots of the inducing parabolic \
   subalgebra of B_3. ",
   "SplitFDpartB3overG2{}(x_1,1,0)");
  this->AddOperationMustBeNew
  ("PrintB3G2branchingTableCharsOnly",this->fPrintB3G2branchingTableCharsOnly, 0, "",
   "Creates a table of branching of finite dimensional B_3-modules over G_2. The argument of the \
   function gives the maximum height \
   of the B_3-weight. The second argument indicates the parabolic subalgebra of B_3- zero entry \
   stands for the corresponding root space lying \
   in the Levi part of the parabolic. Non-zero entry means the corresponding negative root space is \
   not in the parabolic. The expression given \
   in that coordinate is used as the corresponding highest weight. ",
   "PrintB3G2branchingTableCharsOnly{}(2, (0,0,0)); PrintB3G2branchingTableCharsOnly{}(2, (x_1,0,0))");
  this->AddOperationMustBeNew
  ("PrintB3G2branchingTable", this->fPrintB3G2branchingTable, 0, "",
   "Creates a table of branching of finite dimensional B_3-modules over G_2. \
    The argument of the function gives the maximum height \
   of the B_3-weight. The function works with arguments 0 or 1; values of 2 or more must be run off-line.",
   "PrintB3G2branchingTable{}(1, (0,0,0)); PrintB3G2branchingTable{}(1, (x_1,0,0))");
  this->AddOperationMustBeNew
  ("SplitFDTensorGenericGeneralizedVerma", this->fSplitGenericGenVermaTensorFD, 0, "",
   "Experimental, please don't use. Splits generic generalized Verma module tensor finite dimensional module. ",
   "SplitFDTensorGenericGeneralizedVerma{}(G_2, (1, 0), (x_1, x_2)); ");
  this->AddOperationMustBeNew
  ("WriteGenVermaAsDiffOperatorsUpToWeightLevel",
   this->fWriteGenVermaModAsDiffOperatorUpToLevel, 0, "",
   "<b>Experimental, please don't use</b>. Embeds a Lie algebra \
   in the Weyl algebra with matrix coefficients.\
   The third argument gives the highest weight. \
   The non-zero entries of the highest weight give the\
   root spaces outside of the Levi part of the parabolic. \
   The second argument gives the weight level to which the computation should be carried out",
   "WriteGenVermaAsDiffOperatorsUpToWeightLevel{}(B_3, 1, (0, 0, x_3)); ");
  this->AddOperationMustBeNew
  ("EmbedSSalgebraInWeylAlgebra", this->fWriteGenVermaModAsDiffOperators, 0, "",
   "Experimental, please don't use. Embeds a Lie algebra \
   in the Weyl algebra with matrix coefficients.\
   The second argument gives the highest weight. \
   The third argument gives the\
   parabolic subalgebra selection. \
   The first argument gives the type of the Lie algebra. ",
   "EmbedSSalgebraInWeylAlgebra{}(B_3, (0,0,0), (0, 0, 1)); ");
  this->AddOperationMustBeNew
  ("WeylOrbitSimpleCoords", this->fWeylOrbitSimple, 0, "",
   "Generates a Weyl orbit printout from simple coords.\
    First argument = type. Second argument = weight in simple coords. The orbit size is cut off at max 1920 elements (type D_5).",
   "WeylOrbitSimpleCoords{}(B_2, (y, y));");
  this->AddOperationMustBeNew
  ("WeylOrbitFundCoords", this->fWeylOrbitFund, 0, "",
   "Generates a Weyl orbit printout from fundamental coords.\
    First argument = type. Second argument = weight in fundamental coords. ",
   "WeylOrbitFundCoords{}(B_2, (y, 0));");
  this->AddOperationMustBeNew
  ("WeylOrbitFundRho", this->fWeylOrbitFundRho, 0, "",
   "Generates a Weyl orbit printout from fundamental coords.\
    First argument = type. Second argument = weight in fundamental coords. Doing the rho-modified action. ",
   "WeylOrbitFundRho{}(B_2, (y, 0) )");
  this->AddOperationMustBeNew
  ("KLcoeffs", this->fKLcoeffs, 0, "",
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
  this->AddOperationMustBeNew
  ("printRootSubalgebras", this->fprintRootSAs, 0, "",
   "Prints sl(2) subalgebras and root subalgebras. \
   The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
   "printRootSubalgebras{}(E_6)");
  this->AddOperationMustBeNew
  ("printSlTwoSubalgebras", this->fprintSltwos, 0, "",
   "Prints sl(2) subalgebras and root subalgebras. \
   The argument gives the type of the Lie algebra in the form Type_Rank (e.g. E_6).",
   "printSlTwoSubalgebras{}(E_6)");

   this->AddOperationMustBeNew
  ("parabolicsInfoBruhatGraph", this->fParabolicWeylGroupsBruhatGraph, 0, "",
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
  this->AddOperationMustBeNew
  ("fTestMonomialBasisConjecture", this->fTestMonomialBaseConjecture, 0, "",
   "Tests the monomial basis conjecture from the Jackson-Milev paper. First argument gives rank bound. \
    Second argument gives dimension bound. ",
   "fTestMonomialBasisConjecture{}(2, 50)");
/*  this->AddOperationMustBeNew
  ("DecomposeCharGenVermaToIrreps", & this->fDecomposeCharGenVerma, "",
   "<b>This is an experiment; I have no mathematical proof that this function works. \
   If you are seeing this function then I forgot to comment it out before updating the calculator. \
   Please don't use this function.</b> Decomposes the \
   character of a generalized Verma module as a sum of characters of irreducible highest weight \
    modules.\
    First argument = type. Second argument = highest weight. Non-integer entries give parabolic selection. ",
   "DecomposeCharGenVermaToIrreps{}(G_2, (x_1, 0))");*/
  this->AddOperationMustBeNew
  ("LSpath", this->fLSPath, 0, "",
   "Lakshmibai-Seshadri path starting at 0. The first argument gives the semisimple Lie algebra, \
    the next arguments give the waypoints of the path.",
   "LSpath{}(G_2, (0,0), (2,1) )");
  this->AddOperationMustBeNew
  ("LROdefine", this->fLittelmannOperator, 0, "",
   "Littelmann root operator e_i, where e_i is the Littelmann root operator with \
   respect to root of index i. If i is negative then the e_i root operator is defined to be \
   the f_\alpha operator.",
   "e_{{i}}:=LROdefine_i; e_{-1} e_{-1} LSpath{}(G_2, (0,0), (2,1))");
  this->AddOperationMustBeNew
  ("InvertMatrixVerbose", this->fInvertMatrix, 0, "",
   "Inverts a matrix of rationals if invertible, in any other case generates an error. Makes a detailed \
   printout of all Gaussian elimantion steps. Originally intended for demonstrations to linear algebra\
   / calculus students. ",
   "InvertMatrixVerbose{}((1,2),(2,3))");
  this->AddOperationMustBeNew
  ("SolveNseparableQuasiLinearSystem", this->fSolveSeparableBilinearSystem, 0, "",
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

  this->AddOperationMustBeNew
  ("GroebnerLexUpperLimit", this->fGroebnerLex, 0, "",
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
  this->AddOperationMustBeNew
  ("GroebnerGrLexUpperLimit", this->fGroebnerGrLex, 0, "",
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
  this->AddOperationMustBeNew
  ("experimentalPrintSemisimpleSubalgebras", this->fSSsubalgebras, 0, "",
   " <b>This function is being developed and is not imiplemented fully yet. </b> \
   Prints the semisimple subalgebras of a semisimple Lie algebra. ",
   "experimentalPrintSemisimpleSubalgebras{}(A_2)", true);
  this->AddOperationMustBeNew
  ("experimentalEmbedSemisimpleInSemisimple", this->fEmbedSSalgInSSalg, 0, "",
   " <b>This function is being developed and is not imiplemented fully yet. </b> \
   Prints all embeddings of the first subalgebra into the second. ",
   "experimentalEmbedSemisimpleInSemisimple{}(G_2, B_3)", true);
//     this->AddOperationMustBeNew
//  ("printAllPartitions", & this->fPrintAllPartitions, "",
//   "Prints (Kostant) partitions . ",
//   "printAllPartitions{}(A_2, (2,3))");
//  this->AddOperationMustBeNew
//  ("Differential", & this->fDifferential, "",
//   "Differential. ",
//   "d{}{{a}}:=Differential{}a;\nx:=Polynomial{}x;\nd{}(x^2/(x+1))");
//  this->AddOperationMustBeNew
//  ("\\sqrt", & this->fSqrt, "",
//   "Square root of a rational, implemented as algebraic extension of the rationals. ",
//   "\\sqrt{}{3+2\\sqrt{}2}");

  this->AddOperationMustBeNew
  ("FactorOneVarPolyOverRationals", this->fFactor, 0, "",
   "Factors a one variable polynomial over the rationals using Kroenecker's method. \
    After clearing denominators, assume the poly has integer coefficients.\
    If looking for an integer coefficient divisor of degree k, \
    plug in k different integer values of the poly\
    and finds the set of all possible divisors of the value of poly at the k points.\
    this gives a finite set of possibilities for the divisors, as interpolated by\
    Lagrange polynomials.",
   "FactorOneVarPolyOverRationals{}{x^2-4}", false);
  this->AddOperationMustBeNew
  ("Freudenthal", this->fFreudenthalEval, 0, "",
   "Computes the dominant weights with multiplicities of a finite dimensional \
   module of a highest weight given in fundamental coordinates. The first argument gives \
   the semisimple Lie algebra type, the second argument gives the highest weight in \
   fundamental coordinates. ",
   "Freudenthal{}(B_3, (2,2,2))", true)
   ;

//  this->AddOperationMustBeNew
//  ("minPoly", & this->fMinPoly, "",
//   "If the argument of the function is an algebraic number returns its minimal polynomial, else does nothing. ",
//   "minPoly{}(\\sqrt{}2+\\sqrt{}3)");
  this->NumPredefinedVars=this->operationS.size;
}

