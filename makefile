# Pthreads are  being phased out in favor of std::mutex and similar.
# Openmp was lightly used in some code by Thomas in late 2015.
# Optimization flags have not actually been tested. 
# 1. -Og is not used for
# debug builds because building performance matters more than running
# performance.  
# 2. -O3 doesn't slow down non-debug builds too much, and it is
# expected that this will be built on systems that use it, thus -march=native.
# 3. -flto was experimented with, as it is the way to achieve the vaunted
# benefits of seeing the program before running it, but was ultimately too
# slow
# The -MMD -Mp flags create files that notify make about 
# header dependencies so sources can be correctly rebuilt.  
# A 'make clean'
# is of course suggested when changing compilation options, not that something
# bad will happen, but something bad is not guaranteed not to happen.
# AllocationStatistics is Thomas' feature to notify the user with a glibc
# prepared stack trace every time an object is allocated that crosses a
# megabyte boundary, in order to find hot spots of massive memory allocation.
# This needs -rdynamic due to implementation.
# 
FEATUREFLAGS= -std=c++0x -pthread -fopenmp
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

########################
########################
##We include mysql and ssl depending on their availability
##This code may need more work in the future

ifeq ($(nossl),1)
else
sslLocation=
ifneq ($(wildcard /usr/lib64/libssl.so),)#location of ssl in CENTOS
  sslLocation=found
endif
ifneq ($(wildcard /usr/lib/x86_64-linux-gnu/libssl.so),)#location of ssl in Ubuntu
  sslLocation=found
endif
ifneq ($(sslLocation),)
	CFLAGS+= -DMACRO_use_open_ssl 
	LIBRARYINCLUDESEND+= -lssl -lcrypto  #WARNING believe it or not, the libraries must come AFTER the executable name 
else
$(info NOT FOUND: (Open)ssl. I will attempt to install it once the calculator is compiled.) 
endif
endif

ifeq ($(nosql),1)
else
mysqlLocation=
ifneq ($(wildcard /usr/lib64/mysql),)#location of mysql in CENTOS
  mysqlLocation=/usr/lib64/mysql
endif
ifneq ($(wildcard /usr/lib/mysql),)#location of mysql in Ubuntu
  mysqlLocation=/usr/lib/mysql
endif
ifneq ($(mysqlLocation),)
  CFLAGS+= -DMACRO_use_MySQL
  LIBRARYINCLUDESEND+= -L$(mysqlLocation) -lmysqlclient  #WARNING believe it or not, the libraries must come AFTER the executable name
else
$(info NOT FOUND: Mysql. I will attempt to install it once the calculator is compiled.) 
endif
endif
########################
########################

$(info Compiling with flags: $(LIBRARYINCLUDESEND)) 

#if this is missing something, add it, or, ls | grep cpp | xargs echo
SOURCES=\
./src/webserver.cpp \
./src/web-routines-1.cpp \
./src/database.cpp \
./src/examRoutines.cpp \
./src/vpfHtmlInterpretationInterface.cpp \
./src/vpfHtmlSnippets.cpp \
./src/test.cpp \
./src/vpf2Math3_SymmetricGroupsAndGeneralizations.cpp \
./src/vpf4CalculatorFunctionList.cpp \
./src/vpf5_1CalculatorCode.cpp \
./src/vpf5_2javascript_graphics.cpp \
./src/vpf5.cpp \
./src/vpf6_05evaluationroutines.cpp \
./src/vpf6_0parsingRoutines.cpp \
./src/vpf6_1innerTypedFunctions.cpp \
./src/vpf6_2serialization_code.cpp \
./src/vpf6_3calculatorMathRoutines.cpp \
./src/vpf6_35calculatorMathRoutinesPart2.cpp \
./src/vpf6_36calculatorHtmlRoutines.cpp \
./src/vpf6_37calculatorroutines.cpp \
./src/vpf6_38LaTeXRoutines.cpp \
./src/vpf6_4ExpressionsImplementation.cpp \
./src/vpf6.cpp \
./src/vpf7.cpp \
./src/vpf8.cpp \
./src/vpf9_1.cpp \
./src/vpf9_2.cpp \
./src/vpf9_3RationalRadicals.cpp \
./src/vpf9_4SemisimpleLieAlgebras.cpp \
./src/vpf9_5SemisimpleLieAlgebras_RootSubalgebras.cpp \
./src/vpf9_6SemisimpleLieSubAlgebras.cpp \
./src/vpf9_7floating_point_routines.cpp \
./src/vpf9_8GlobalObjects.cpp \
./src/vpf9_91html_routines_calculator.cpp \
./src/vpf9_92multitasking.cpp \
./src/vpf99_HardcodedData.cpp \
./src/vpf9_9SystemFunctions.cpp \
./src/vpf9.cpp \
./src/vpfCharacters_CalculatorInterface.cpp \
./src/vpfCharacters.cpp \
./src/vpfCrypto.cpp \
./src/vpfFiniteFields.cpp \
./src/vpfGraph.cpp \
./src/vpfJson.cpp \
./src/vpfWebServerInterProcessLogistics.cpp \
./src/vpf9_85TimeDateWrappers.cpp


OBJECTS=$(SOURCES:.cpp=.o) 
DEPS=$(SOURCES:.cpp=.d)

all: directories bin_calculator 
directories: bin
bin:
	mkdir ./bin

bin_calculator: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o  ./bin/calculator $(LIBRARYINCLUDESEND)

testrun: bin/calculator
	time ./bin/calculator test

%.o:%.cpp
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPS)

-include $(DEPS)
