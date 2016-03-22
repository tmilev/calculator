# Featureflags should be self-explanatory.  Pthreads are (so far 2015-11) only
# used for calculator code, but this is likely to change.  Openmp was lightly
# used in some code in late 2015.
# Optimization flags have not actually been tested.  -Og is not used for
# debug builds because building performance matters more than running
# performance.  -O3 doesn't slow down non-debug builds too much, and it is
# expected that this will be built on systems that use it, thus -march=native.
# -flto was experimented with, as it is the way to achieve the vaunted
# benefits of seeing the program before running it, but was ultimately too
# slow
# Those -MMD -Mp things create files that notify make about what the
# header dependencies so things can be correctly rebuilt.  A 'make clean'
# is of course suggested when changing compilation options, not that something
# bad will happen, but something bad is not guaranteed not to happen.
# AllocationStatistics is Thomas' feature to notify the user with a glibc
# prepared stack trace every time an object is allocated that crosses a
# megabyte boundary, in order to find hot spots of massive memory allocation
# it needs -rdynamic due to implementation
# 
FEATUREFLAGS=--std=c++11 -pthread -fopenmp
CFLAGS=-Wall -Wno-address $(FEATUREFLAGS) -c
LDFLAGS=$(FEATUREFLAGS)
LIBRARYINCLUDESEND=

LD_LIBRARY_PATH=LD_LIBRARY_PATH:

ifeq ($(hsa), 1)
	CXX=/home/user/gcc/bin/g++
	LDFLAGS+=-L/home/user/gcc/lib64 -Wl,-rpath,/home/user/gcc/lib64 -L/opt/hsa/lib -lhsa-runtime64 -lhsakmt
else
	CXX=g++
endif

ifeq ($(llvm), 1)
	CXX=clang-3.8
else
	CXX=g++
endif

ifeq ($(debug), 1)
	CFLAGS += -g
	LDFLAGS +=
else
	CFLAGS += -O3 -march=native
	LDFLAGS += -O3 -march=native
endif

ifeq ($(AllocationStatistics), 1)
	CFLAGS += -DAllocationStatistics
	LDFLAGS += -rdynamic
endif

ifeq ($(ssl),1)
	CFLAGS+= -DMACRO_use_open_ssl
	LIBRARYINCLUDESEND+= -lssl -lcrypto #WARNING believe it or not, the libraries must come AFTER the executable name
endif

ifeq ($(mysql),1)
	CFLAGS+= -DMACRO_use_MySQL
	LIBRARYINCLUDESEND+= -L/usr/lib64/mysql -lmysqlclient  #WARNING believe it or not, the libraries must come AFTER the executable name
endif

#if this is missing something, add it, or, ls | grep cpp | xargs echo
SOURCES=test.cpp vpf2Math3_SymmetricGroupsAndGeneralizations.cpp vpf4CalculatorFunctionList.cpp vpf5_1CalculatorCode.cpp vpf5_2javascript_graphics.cpp vpf5.cpp vpf6_05evaluationroutines.cpp vpf6_0parsingRoutines.cpp vpf6_1innerTypedFunctions.cpp vpf6_2serialization_code.cpp vpf6_3calculatorMathRoutines.cpp vpf6_35calculatorMathRoutinesPart2.cpp vpf6_4ExpressionsImplementation.cpp vpf6.cpp vpf7.cpp vpf8.cpp vpf9_1.cpp vpf9_2.cpp vpf9_3RationalRadicals.cpp vpf9_4SemisimpleLieAlgebras.cpp vpf9_5SemisimpleLieAlgebras_RootSubalgebras.cpp vpf9_6SemisimpleLieSubAlgebras.cpp vpf9_7floating_point_routines.cpp vpf9_8GlobalObjects.cpp vpf9_91html_routines_calculator.cpp vpf9_92multitasking.cpp vpf99_HardcodedData.cpp vpf9_9SystemFunctions.cpp vpf9.cpp vpfCalculatorMain.cpp vpfCharacters_CalculatorInterface.cpp vpfCharacters.cpp vpfCrypto.cpp vpfFiniteFields.cpp vpfGraph.cpp vpfJson.cpp vpfWebServer.cpp vpfWebServerInterProcessLogistics.cpp vpf6_5calculator_web_routines.cpp vpf8_1DatabaseInterface_MySQL.cpp vpfWebServerExamAndTeachingRoutines.cpp vpf9_85TimeDateWrappers.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEPS=$(SOURCES:.cpp=.d)

all: directories Debug_calculator 
directories: Debug
Debug:
	mkdir ./Debug

Debug_calculator: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o  ./Debug/calculator $(LIBRARYINCLUDESEND)

testrun: Debug/calculator
	time ./Debug/calculator test

%.o:%.cpp
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPS)

-include $(DEPS)
