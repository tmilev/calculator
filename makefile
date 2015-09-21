FEATUREFLAGS=--std=c++11 -pthread -fopenmp
CFLAGS=-Wall -Wno-address $(FEATUREFLAGS) -c
LDFLAGS=$(FEATUREFLAGS)

ifeq ($(hsa), 1)
	CXX=/home/user/gcc/bin/g++
	LDFLAGS+=-L/home/user/gcc/lib64 -Wl,-rpath,/home/user/gcc/lib64 -L/opt/hsa/lib -lhsa-runtime64 -lhsakmt
else
	CXX=g++
endif

ifeq ($(debug), 1)
	CFLAGS += -g
else
	CFLAGS += -O3 -march=native
endif

#if this is missing something, add it, or, ls | grep cpp | xargs echo
SOURCES=vpf4CalculatorFunctionList.cpp vpf5_1CalculatorCode.cpp vpf5_2javascript_graphics.cpp vpf5.cpp vpf6_05evaluationroutines.cpp vpf6_0parsingRoutines.cpp vpf6_1innerTypedFunctions.cpp vpf6_2serialization_code.cpp vpf6_3calculatorMathRoutines.cpp vpf6_4ExpressionsImplementation.cpp vpf6.cpp vpf7.cpp vpf8.cpp vpf9_1.cpp vpf9_2.cpp vpf9_3RationalRadicals.cpp vpf9_4SemisimpleLieAlgebras.cpp vpf9_5SemisimpleLieAlgebras_RootSubalgebras.cpp vpf9_6SemisimpleLieSubAlgebras.cpp vpf9_7floating_point_routines.cpp vpf9_8GlobalObjects.cpp vpf9_91html_routines_calculator.cpp vpf9_92multitasking.cpp vpf99_HardcodedData.cpp vpf9_9SystemFunctions.cpp vpf9.cpp vpfCharacters_CalculatorInterface.cpp vpfCharacters.cpp vpfCrypo.cpp vpfFiniteFields.cpp vpfGraph.cpp vpfJson.cpp vpfWebServer.cpp vpf2Math3_SymmetricGroupsAndGeneralizations.cpp

OBJECTS=$(SOURCES:.cpp=.o)
DEPS=$(SOURCES:.cpp=.d)

all: directories calculator 
directories: Debug
Debug:
	mkdir ./Debug

calculator: $(OBJECTS) vpfCalculatorMain.o
	$(CXX) $(LDFLAGS) $(OBJECTS) vpfCalculatorMain.o -o ./Debug/calculator

test: $(OBJECTS) test.o
	$(CXX) $(LDFLAGS) $(OBJECTS) test.o -o test

testrun: test
	./test

%.o:%.cpp
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPS) vpfCalculatorMain.o test.o

-include $(DEPS)
