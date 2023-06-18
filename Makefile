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
# -- Speed: ~121ms to compute 2019 factorial and openssl-send the result back.
# -- Compilation time: 30 seconds.
# -O3 "optimization" is known to break linkage, please DO NOT USE.
#
# Conclusion: O2 optimization speeds up computations by 40%, 
# slows down compilation by a factor of three. 

ifeq ($(optimize), 1)
$(info Optimization flag selected. [1;31mThis may slow down compilation a great deal.[0m) 
OPTIMIZATION_FLAGS+=-O2
endif

CFLAGS=-Wpedantic -Wall -Wextra -std=c++0x $(OPTIMIZATION_FLAGS) -c -pthread
LIBRARIES_INCLUDED_AT_THE_END=
compiler=g++
TARGET=calculator
OBJECT_FILE_OUTPUT=bin/

ifeq ($(llvm), 1)
	compiler=clang-3.8
endif

ifeq ($(wasm), 1)
    compiler=./emsdk/upstream/emscripten/emcc
    CFLAGS+=-DMACRO_use_wasm 
    CFLAGS+=-s
    # No pthread
    LDFLAGS=$(OPTIMIZATION_FLAGS)
    LDFLAGS+=-s LLD_REPORT_UNDEFINED -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s EXPORTED_FUNCTIONS='["_callCalculator", "_free"]'
    $(info [1;31mWeb assembly: turning off ssl and mongoDB.[0m) 
    TARGET=calculator_html/web_assembly/calculator.js
    OBJECT_FILE_OUTPUT=bin/wasm/
    nossl=1
    noMongo=1
else
    LDFLAGS=-pthread $(OPTIMIZATION_FLAGS)
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
$(info [1;31mNo mongoDB requested.[0m) 
else
mongoLocation =
ifneq ($(wildcard /usr/share/lintian/overrides/libmongoc-1.0-0),) 
  mongoLocation=/usr/share/lintian/overrides
endif
ifneq ($(wildcard /usr/local/lib/libmongoc-1.0.so),) #location of mongoC in Ubuntu
  mongoLocation=/usr/local/lib
endif
ifneq ($(mongoLocation),)
  CFLAGS+=-I/usr/include/libmongoc-1.0
  CFLAGS+=-I/usr/include/libbson-1.0
  CFLAGS+=-I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0
  CFLAGS+=-DMACRO_use_MongoDB
#  LDFLAGS+= -L/usr/local/lib
  LIBRARIES_INCLUDED_AT_THE_END+=-L/$(mongoLocation) -lmongoc-1.0 -lbson-1.0
$(info [1;32mMongo found.[0m) 
else
$(info [1;31mNOT FOUND: Mongo.[0m The calculator will run using a fallback database.)
noMongo=1
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
ifneq ($(wildcard /lib/x86_64-linux-gnu/libssl3.so),)#location of ssl in Debian GNU/Linux rodete
  sslLocation=found
endif

ifneq ($(sslLocation),)
  CFLAGS+=-DMACRO_use_open_ssl 
  LIBRARIES_INCLUDED_AT_THE_END+=-lssl -lcrypto # WARNING believe it or not, the libraries must come AFTER the executable name
$(info [1;32mOpenssl found.[0m) 
else
$(info [1;31mNOT FOUND: Openssl.[0m Login+database won't run.) 
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
    web_api_test.cpp \
    webserver.cpp \
    abstract_syntax_notation_one_decoder.cpp \
    abstract_syntax_notation_one_decoder_test.cpp \
    calculator_conversions_1.cpp \
    calculator_educational_functions_1.cpp \
    calculator_expressions.cpp \
    calculator_expression_context.cpp \
    calculator_expressions_test.cpp \
    calculator_evaluation_steps.cpp \
    calculator_lie_theory.cpp \
    calculator_finite_group_characters.cpp \
    calculator_function_registration.cpp \
    calculator_function_registration_extra.cpp \
    calculator_function_registration_scientific.cpp \
    calculator_function_registration_lie_theory.cpp \
    calculator_html_interpretation.cpp \
    calculator_function_registration_vector_partition_function.cpp \
    calculator_html_routines.cpp \
    calculator_implementation_1.cpp \
    calculator_implementation_2.cpp \
    calculator_implementation_3.cpp \
    calculator_implementation_4.cpp \
    calculator_implementation_5.cpp \
    calculator_implementation_6.cpp \
    calculator_implementation_6_test.cpp \
    calculator_implementation_7.cpp \
    calculator_implementation_8_polynomial.cpp \
    calculator_implementation_9.cpp \
    calculator_implementation_10.cpp \
    calculator_implementation_11_trigonometry.cpp \
    calculator_implementation_12_complex_dynamics.cpp \
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
    database_local.cpp \
    date_time_wrappers.cpp \
    exam_routines.cpp \
    global_objects.cpp \
    global_test.cpp \
    html_snippets.cpp \
    json.cpp \
    json_test.cpp \
    general_strings_test.cpp \
    math_algebraic_numbers.cpp \
    math_algebraic_numbers_test.cpp \
    math_elliptic_curves.cpp \
    math_finite_group_characters.cpp \
    math_floating_point.cpp \
    math_graph.cpp \
    math_graphics.cpp \
    math_hard_coded_data.cpp \
    math_implementation_1.cpp \
    math_implementation_1_test.cpp \
    math_implementation_2.cpp \
    math_implementation_3.cpp \
    math_implementation_4.cpp \
    math_implementation_5.cpp \
    math_implementation_6.cpp \
    math_implementation_5_test.cpp \
    math_implementation_6_test.cpp \
    math_lattices.cpp \
    math_vector_partition_functions.cpp \
    math_semisimple_lie_algebras.cpp \
    math_semisimple_lie_root_subalgebras.cpp \
    math_semisimple_lie_subalgebras.cpp \
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
    web_assembly.cpp \
    webserver_fork.cpp

SOURCES_PUBLIC_DOMAIN_RELATIVE_PATH_CPP=\
    crypto_public_domain_aes.cpp \
    crypto_public_domain_ripemd.cpp \
    crypto_public_domain.cpp 


SOURCES_RELATIVE_PATH_C=\

SOURCES_RELATIVE_PATH_CC=\

OBJECTS=

ifneq ($(noPublicDomain), 1) 
OBJECTS+=$(addprefix $(OBJECT_FILE_OUTPUT), $(SOURCES_PUBLIC_DOMAIN_RELATIVE_PATH_CPP:.cpp=.o))
endif

OBJECTS+=$(addprefix $(OBJECT_FILE_OUTPUT), $(SOURCES_RELATIVE_PATH:.cpp=.o))
OBJECTS+=$(addprefix $(OBJECT_FILE_OUTPUT), $(SOURCES_RELATIVE_PATH_C:.c=.o))
OBJECTS+=$(addprefix $(OBJECT_FILE_OUTPUT), $(SOURCES_RELATIVE_PATH_CC:.cc=.o))


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
	$(compiler) $(LDFLAGS) $(OBJECTS) -o $(TARGET) $(LIBRARIES_INCLUDED_AT_THE_END)

test: bin_calculator
	time ./calculator test

$(OBJECT_FILE_OUTPUT)%.o:src/%.cpp
	mkdir -p $(@D)
	$(compiler) $(CFLAGS) -MMD -MP $< -o $@
	
clean:
	rm -f $(OBJECTS) $(DEPENDENCIES) ./calculator

-include $(DEPENDENCIES)
