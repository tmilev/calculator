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
LIBRARIES_INCLUDED_AT_THE_END=

ifeq ($(hsa), 1)
	CXX=/home/user/gcc/bin/g++
	LDFLAGS+=-L/home/user/gcc/lib64 -Wl,-rpath,/home/user/gcc/lib64 -L/opt/hsa/lib -L/usr/local/lib -lhsa-runtime64 -lhsakmt
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

ifeq ($(nossl), 1)
$(info [1;33mNo openssl requested.[0m) 
else
sslLocation=
ifneq ($(wildcard /usr/lib64/libssl.so),)#location of ssl in CENTOS
  sslLocation=found
endif
ifneq ($(wildcard /usr/lib/x86_64-linux-gnu/libssl.so),)#location of ssl in Ubuntu x86
  sslLocation=found
endif
ifneq ($(wildcard /usr/lib/arm-linux-gnueabihf/libssl.so),)#location of ssl in Ubuntu x86
  sslLocation=found
endif
ifneq ($(wildcard /usr/lib/x86_64-linux-gnu/libssl.so),)#location of ssl in Ubuntu x86
  sslLocation=found
endif
ifneq ($(sslLocation),)
	CFLAGS+= -DMACRO_use_open_ssl 
	LIBRARIES_INCLUDED_AT_THE_END+= -lssl -lcrypto  #WARNING believe it or not, the libraries must come AFTER the executable name 
$(info [1;32mOpenssl found.[0m) 
else
$(info [1;31mNOT FOUND: Openssl.[0m I will attempt to install it once the calculator is compiled.) 
endif
endif

ifeq ($(noMongo), 1)
$(info [1;33mNo mongo requested.[0m) 
else
mongoLocation =
ifneq ($(wildcard /usr/local/lib/libmongoc-1.0.so),)#location of mongoC in Ubuntu
  CFLAGS += -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0
  mongoLocation = /usr/local/
endif
ifneq ($(mongoLocation),)
  CFLAGS+= -DMACRO_use_MongoDB
#  LDFLAGS+= -L/usr/local/lib
  LIBRARIES_INCLUDED_AT_THE_END+= -L/usr/local/lib -lmongoc-1.0 -lbson-1.0
$(info [1;32mMongo found.[0m) 
else
$(info [1;31mNOT FOUND: Mongo.[0m I will attempt to install it once the calculator is compiled.) 
endif
endif
########################
########################

$(info [1;33mCompile flags:  $(CFLAGS)[0m)
$(info [1;34mLinker flags part 1:  $(LDFLAGS))
$(info [1;34mLinker flags part 2: $(LIBRARIES_INCLUDED_AT_THE_END)[0m)

#if this is missing something, add it, or, ls | grep cpp | xargs echo
SOURCES_NO_PATH=\
databasemongo.cpp \
database_mongo_calculator.cpp \
webserver.cpp \
web-routines-1.cpp \
database.cpp \
examRoutines.cpp \
vpfHtmlInterpretationInterface.cpp \
vpfHtmlSnippets.cpp \
test.cpp \
vpfEllipticCurves.cpp \
vpf2Math3_SymmetricGroupsAndGeneralizations.cpp \
vpf4CalculatorFunctionList.cpp \
vpf5_1CalculatorCode.cpp \
vpf5_2javascript_graphics.cpp \
vpf5.cpp \
vpf6_05evaluationroutines.cpp \
vpf6_0parsingRoutines.cpp \
vpf6_1innerTypedFunctions.cpp \
vpf6_2serialization_code.cpp \
vpf6_3calculatorMathRoutines.cpp \
vpf6_35calculatorMathRoutinesPart2.cpp \
vpf6_36calculatorHtmlRoutines.cpp \
vpf6_37calculatorroutines.cpp \
vpf6_38LaTeXRoutines.cpp \
vpf6_4ExpressionsImplementation.cpp \
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
vpf9_92multitasking.cpp \
vpf99_HardcodedData.cpp \
vpf9_9SystemFunctions.cpp \
vpf9.cpp \
vpfCharacters_CalculatorInterface.cpp \
vpfCharacters.cpp \
vpfCrypto.cpp \
vpfCrypto_sha3.cpp \
vpfCrypto_ripemd.cpp \
vpfCrypto_AES.cpp \
vpfFiniteFields.cpp \
vpfGraph.cpp \
vpfJson.cpp \
vpfWebServerInterProcessLogistics.cpp \
vpf9_85TimeDateWrappers.cpp


OBJECTS=$(addprefix bin/, $(notdir $(SOURCES_NO_PATH:.cpp=.o)))
DEPENDENCIES=$(addprefix bin/, $(notdir $(SOURCES_NO_PATH:.cpp=.d)))
#$(info $(OBJECTS))
#$(info )
#$(info $(DEPENDENCIES))
#$(info )

all: directories bin_calculator 
directories: bin
bin:
	mkdir ./bin

bin_calculator: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o ./bin/calculator $(LIBRARIES_INCLUDED_AT_THE_END)

testrun: bin/calculator
	time ./bin/calculator test

bin/%.o:src/%.cpp
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPENDENCIES)

-include $(DEPENDENCIES)
