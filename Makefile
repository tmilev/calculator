# 1. The -O3 flag doesn't slow down builds too much, and it is
# expected that this will be built on systems that use it, thus -march=native.
# 2. The -MMD -Mp flags create files that notify make about
# header dependencies so sources can be correctly rebuilt.  
# A 'make clean'
# is of course suggested when changing compilation options, not that something
# bad will happen, but something bad is not guaranteed not to happen.
# AllocationStatistics is a likely-to-be-deprecated
# feature to notify the user with a glibc
# prepared stack trace every time an object is allocated that crosses a
# megabyte boundary, in order to find hot spots of massive memory allocation.
# This needs -rdynamic due to implementation.
# 

OPTIMIZATION_FLAGS=
# Use native architecture instructions.
# Did not notice significant difference in performance with / without the flag. 
# Tested: compute 2019 factorial and openssl-send the result back.
# 
# OPTIMIZATION_FLAGS+=-march=native

# - With O2 optimization. 
# -- Speed: ~84ms to compute 2019 factorial and openssl-send the result back
# on my current dev laptop. 
# -- Compilation time: 95 seconds on same machine.
# - Without O2 optimization.
# -- Speed: ~121ms to compute 2019 and openssl-send the result back.
# -- Compilation time: 30 seconds.
# -O3 "optimization" is known to break linkage, please DO NOT USE.
#
# Conclusion: O2 optimization speeds up computations by 40%, 
# slows down compilation by a factor of three. 
# Decision: for now, do not use optimizations:
# 40% speed up not worth the 3-fold increase in build times.
# 

# OPTIMIZATION_FLAGS+=-O2


CFLAGS=-Wpedantic -Wall -Wextra -std=c++0x $(OPTIMIZATION_FLAGS) -c
LDFLAGS=-pthread $(OPTIMIZATION_FLAGS)
LIBRARIES_INCLUDED_AT_THE_END=
CXX=g++

ifeq ($(llvm), 1)
	CXX=clang-3.8
else
	CXX=g++
endif

ifeq ($(AllocationStatistics), 1)
	CFLAGS+=-DAllocationStatistics
	LDFLAGS+=-rdynamic
endif

########################
########################
## We include mysql and ssl depending on their availability
## This code may need more work in the future
ifeq ($(noMongo), 1)
$(info [1;31mNo mongo requested.[0m) 
else
mongoLocation =
ifneq ($(wildcard /usr/local/lib/libmongoc-1.0.so),)#location of mongoC in Ubuntu
  CFLAGS+=-I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0
  mongoLocation=/usr/local/
endif
ifneq ($(mongoLocation),)
  CFLAGS+=-DMACRO_use_MongoDB
#  LDFLAGS+= -L/usr/local/lib
  LIBRARIES_INCLUDED_AT_THE_END+=-L/usr/local/lib -lmongoc-1.0 -lbson-1.0
$(info [1;32mMongo found.[0m) 
else
$(info [1;31mNOT FOUND: Mongo.[0m The calculator will run without a database and proper login.) 
endif
endif

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
  CFLAGS+=-DMACRO_use_open_ssl 
  LIBRARIES_INCLUDED_AT_THE_END+=-lssl -lcrypto # WARNING believe it or not, the libraries must come AFTER the executable name
$(info [1;32mOpenssl found.[0m) 
else
$(info [1;31mNOT FOUND: Openssl.[0m I will attempt to install it once the calculator is compiled.) 
endif
endif
########################
########################

$(info [1;33mCompile flags: $(CFLAGS)[0m)
$(info [1;34mLinker flags part 1: $(LDFLAGS))
$(info [1;34mLinker flags part 2: $(LIBRARIES_INCLUDED_AT_THE_END)[0m)

#if this is missing something, add it, or, ls | grep cpp | xargs echo
SOURCES_RELATIVE_PATH=\
    web_api.cpp \
    webserver.cpp \
    abstract_syntax_notation_one_decoder.cpp \
    abstract_syntax_notation_one_decoder_test.cpp \
    calculator_conversions_1.cpp \
    calculator_expressions.cpp \
    calculator_expression_context.cpp \
    calculator_expressions_test.cpp \
    calculator_finite_group_characters.cpp \
    calculator_function_registration.cpp \
    calculator_html_interpretation.cpp \
    calculator_html_routines.cpp \
    calculator_implementation_1.cpp \
    calculator_implementation_2.cpp \
    calculator_implementation_3.cpp \
    calculator_implementation_4.cpp \
    calculator_implementation_5.cpp \
    calculator_implementation_6.cpp \
    calculator_implementation_7.cpp \
    calculator_implementation_8_polynomial.cpp \
    calculator_implementation_9.cpp \
    calculator_javascript_graphics.cpp \
    calculator_latex_routines.cpp \
    calculator_parsing_routines.cpp \
    calculator_parsing_routines_test.cpp \
    calculator_typed_functions_1.cpp \
    calculator_webserver_functions.cpp \
    crypto.cpp \
    crypto_sha3.cpp \
    crypto_test.cpp \
    database.cpp \
    database_test.cpp \
    database_mongo.cpp \
    database_fallback_json.cpp \
    database_mongo_calculator.cpp \
    date_time_wrappers.cpp \
    examRoutines.cpp \
    global_objects.cpp \
    global_test.cpp \
    html_snippets.cpp \
    json.cpp \
    json_test.cpp \
    math_algebraic_numbers.cpp \
    math_elliptic_curves.cpp \
    math_finite_group_characters.cpp \
    math_floating_point.cpp \
    math_graph.cpp \
    math_hard_coded_data.cpp \
    math_implementation_1.cpp \
    math_implementation_1_test.cpp \
    math_implementation_2.cpp \
    math_implementation_3.cpp \
    math_implementation_4.cpp \
    math_implementation_5.cpp \
    math_implementation_6.cpp \
    math_implementation_5_test.cpp \
    math_semisimple_Lie_algebras.cpp \
    math_semisimple_Lie_root_subalgebras.cpp \
    math_semisimple_Lie_subalgebras.cpp \
    multiprocessing.cpp \
    multitasking.cpp \
    random_system.cpp \
    source_code_formatter.cpp \
    string_constants.cpp \
    symmetric_groups_and_generalizations.cpp \
    system_functions.cpp \
    test.cpp \
    main.cpp \
    transport_layer_security.cpp \
    transport_layer_security_implementation.cpp \
    transport_layer_security_openssl.cpp \
    transport_layer_security_test.cpp \
    web_routines_1.cpp \
    webserver_fork.cpp

SOURCES_PUBLIC_DOMAIN_RELATIVE_PATH_CPP=\
    crypto_public_domain_aes.cpp \
    crypto_public_domain_ripemd.cpp \
    crypto_public_domain.cpp 


SOURCES_RELATIVE_PATH_C=\

SOURCES_RELATIVE_PATH_CC=\

OBJECTS=

ifneq ($(noPublicDomain), 1) 
OBJECTS+=$(addprefix bin/, $(SOURCES_PUBLIC_DOMAIN_RELATIVE_PATH_CPP:.cpp=.o))
endif

OBJECTS+=$(addprefix bin/, $(SOURCES_RELATIVE_PATH:.cpp=.o))
OBJECTS+=$(addprefix bin/, $(SOURCES_RELATIVE_PATH_C:.c=.o))
OBJECTS+=$(addprefix bin/, $(SOURCES_RELATIVE_PATH_CC:.cc=.o))


DEPENDENCIES=$(OBJECTS:.o=.d)
DIRECTORIES=$(dir $(OBJECTS))

# Uncomment the following lines to print
# the objects/dependencies file names.
# $(info $(OBJECTS))
# $(info )
# $(info $(DEPENDENCIES))
# $(info )
# $(info Directories: $(DIRECTORIES))
# $(info )


all: bin_calculator 

bin_calculator: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o ./calculator $(LIBRARIES_INCLUDED_AT_THE_END)

testrun: calculator
	time ./calculator test

bin/%.o:src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@
	
clean:
	rm -f $(OBJECTS) $(DEPENDENCIES) ./calculator

-include $(DEPENDENCIES)
