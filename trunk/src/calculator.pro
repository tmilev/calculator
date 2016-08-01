QT+= core
QT-= gui
TARGET = calculator
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app


DEFINES+=MACRO_use_MySQL
DEFINES+=MACRO_use_open_ssl
LIBS+=-L/usr/lib64/mysql
LIBS+=-lmysqlclient
LIBS+=-lssl
LIBS+=-lcrypto

SOURCES += \
    custom/vpf6_5calculator_web_routines.cpp \
    custom/vpfCalculatorMain.cpp \
    custom/vpfWebServer.cpp \
    custom/vpf8_1DatabaseInterface_MySQL.cpp \
    test.cpp \
    vpf2Math3_SymmetricGroupsAndGeneralizations.cpp \
    vpf4CalculatorFunctionList.cpp \
    vpf5_1CalculatorCode.cpp \
    vpf5_2javascript_graphics.cpp \
    vpf5.cpp \
    vpf6_0parsingRoutines.cpp \
    vpf6_1innerTypedFunctions.cpp \
    vpf6_2serialization_code.cpp \
    vpf6_3calculatorMathRoutines.cpp \
    vpf6_4ExpressionsImplementation.cpp \
    vpf6_05evaluationroutines.cpp \
    vpf6_35calculatorMathRoutinesPart2.cpp \
    vpf6.cpp \
    vpf7.cpp \
    vpf8.cpp \
    vpf9_1.cpp \
    vpf9_2.cpp \
    vpf9_3RationalRadicals.cpp \
    vpf9_4SemisimpleLieAlgebras.cpp \
    vpf9_5SemisimpleLieAlgebras_RootSubalgebras.cpp \
    vpf9_6SemisimpleLieSubAlgebras.cpp \
    vpf9_7floating_point_routines.cpp \
    vpf9_8GlobalObjects.cpp \
    vpf9_9SystemFunctions.cpp \
    vpf9_85TimeDateWrappers.cpp \
    vpf9_91html_routines_calculator.cpp \
    vpf9_92multitasking.cpp \
    vpf9.cpp \
    vpf99_HardcodedData.cpp \
    vpfCharacters_CalculatorInterface.cpp \
    vpfCharacters.cpp \
    vpfCrypto.cpp \
    vpfFiniteFields.cpp \
    vpfGraph.cpp \
    vpfJson.cpp \
    vpfWebServerExamAndTeachingRoutines.cpp \
    vpfWebServerInterProcessLogistics.cpp \
    vpf6_36calculatorHtmlRoutines.cpp \
    vpfHtmlInterpretationInterface.cpp \
    vpfHtmlSnippets.cpp

HEADERS += \
    vpf.h \
    vpfFiniteFields.h \
    vpfHeader1General0_General.h \
    vpfHeader1General1_ListReferences.h \
    vpfHeader1General2Multitasking.h \
    vpfHeader1General3_Test.h \
    vpfHeader1General4General_Logging_GlobalVariables.h \
    vpfHeader1General5TimeDate.h \
    vpfHeader2Math0_General.h \
    vpfHeader2Math1_2SemisimpleLieAlgebras_RootSubalgebras.h \
    vpfHeader2Math1_3SemisimpleLieSubalgebras.h \
    vpfHeader2Math01_LargeIntArithmetic.h \
    vpfHeader2Math1_LieTheoryExtras.h \
    vpfHeader2Math1_SemisimpleLieAlgebras.h \
    vpfHeader2Math2_AlgebraicNumbers.h \
    vpfHeader2Math02_Vectors.h \
    vpfHeader2Math3_FiniteGroups.h \
    vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h \
    vpfHeader2Math4_Graph.h \
    vpfHeader2Math5_SubsetsSelections.h \
    vpfHeader2Math6_ModulesSSLieAlgebras.h \
    vpfHeader2Math7_PackedVector.h \
    vpfHeader2Math8_VectorSpace.h \
    vpfHeader2Math9DrawingVariables.h \
    vpfHeader2Math15_UniversalEnveloping.h \
    vpfHeader3Calculator0_Interface.h \
    vpfHeader3Calculator1_InnerTypedFunctions.h \
    vpfHeader3Calculator2_InnerFunctions.h \
    vpfHeader3Calculator3_WeylGroupCharacters.h \
    vpfHeader4SystemFunctionsGlobalObjects.h \
    vpfHeader5Crypto.h \
    vpfHeader6WebServerInterprocessLogistics.h \
    vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h \
    vpfImplementationHeader2Math3_FiniteGroups.h \
    vpfImplementationHeader2Math3_WeylAlgebra.h \
    vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h \
    vpfImplementationHeader2Math7_PackedVector.h \
    vpfImplementationHeader2Math15_UniversalEnveloping.h \
    vpfImplementationHeader2Math051_PolynomialComputations_Basic.h \
    vpfImplementationHeader2Math052_PolynomialComputations_Advanced.h \
    vpfJson.h \
    vpfMacros.h \
    vpfPackedVector.h \
    vpfHeader3Calculator4HtmlFunctions.h \
    vpfHeader1General6Maps.h \
    vpfHeader1General7FileOperations_Encodings.h \
    custom/vpfHeader6WebServer.h \
    vpfHeader7DatabaseInterface_MySQL.h \
    vpfHeader8HtmlSnippets.h \
    vpfHeader8HtmlInterpretationInterface.h \
    vpfHeader8HtmlInterpretation.h \
    vpfHeader6WebServer2_Interface.h \
    vpfWebServerCommon.h \
    vpfHeader7DatabaseShared.h
