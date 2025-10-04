# Example builds commands.
# 1. Build calculator with default settings, slowly:
#    make
# 2. Build calculator with default settings, 20 jobs in parallel (needs RAM):
#    make -j20
# 3. Build calculator with optimization:
#    make -j20 optimize=1
# 4. Build calculator with clang++. Requires clang++ be installed:
#    make -j20 llvm=1
# 5. Build calculator without openssl (no https, no login/passwords):
#    make -j20 nossl=1
# 6. Build calculator.wasm + files in calculator_html/web_assembly/.
#    Requires web assembly toolkit:
#    make -j20 wasm=1



# 1. The -MMD -Mp flags create files that notify make about
#    header dependencies so sources can be correctly rebuilt.  
# 2. Use 'make clean' when changing compilation options.
# 3. AllocationStatistics is a likely-to-be-deprecated
#    feature to notify the user with a glibc
#    prepared stack trace every time an object is allocated that crosses a
#    megabyte boundary, in order to find hot spots of massive memory allocation.
#    This needs -rdynamic due to implementation.

OPTIMIZATION_FLAGS=

ifeq ($(optimize), 1)
$(info Optimization flag selected. [1;31mThis may slow down compilation a great deal.[0m) 
# Tested: compute 2019 factorial and openssl-send the result back.
# - With O2 optimization. 
# -- Speed: ~84ms to compute 2019 factorial and openssl-send the result back
# on my current dev laptop. 
# -- Compilation time: 95 seconds on same machine.
# - Without O2 optimization.
# -- Speed: ~121ms to compute 2019 factorial and openssl-send the result back.
# -- Compilation time: 30 seconds.
# -O3 "optimization" is known to break linkage.
# Conclusion: O2 optimization speeds up computations by 40%, 
# slows down compilation by a factor of three. 
OPTIMIZATION_FLAGS+=-O3
endif

CFLAGS=-Wpedantic -Wall -Wextra $(OPTIMIZATION_FLAGS) -c -pthread
LIBRARIES_INCLUDED_AT_THE_END=
compiler=g++
TARGET=calculator
OBJECT_FILE_OUTPUT=bin/

ifeq ($(llvm), 1)
	compiler=clang++
	# CFLAGS+=-stdlib=libc++ -std=c++11
	# On Ubuntu, install clang with: 
	# sudo apt install clang 
	# You will likely also need to install:
	# sudo apt install libc++-dev
	# sudo apt install libstdc++-dev
else
    # flag known to be necessary in some deployment environments.
    CFLAGS+=-std=c++0x
endif

ifeq ($(wasm), 1)
    compiler=./emsdk/upstream/emscripten/emcc
    CFLAGS+=-DMACRO_use_wasm 
    CFLAGS+=-s
    # No pthread
    LDFLAGS=$(OPTIMIZATION_FLAGS)
    LDFLAGS+=-s LLD_REPORT_UNDEFINED -sASSERTIONS -sALLOW_MEMORY_GROWTH -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s EXPORTED_FUNCTIONS='["_callCalculator", "_free"]'
    $(info [1;31mWeb assembly: turning off ssl.[0m) 
    TARGET=calculator_html/web_assembly/calculator.js
    OBJECT_FILE_OUTPUT=bin/wasm/
    nossl=1
else
    LDFLAGS=-pthread $(OPTIMIZATION_FLAGS)
endif



ifeq ($(AllocationStatistics), 1)
	CFLAGS+=-DAllocationStatistics
	LDFLAGS+=-rdynamic
endif

# Ssl dependencies.

foundlocalssl=
foundssl=
ifeq ($(nossl), 1)
$(info [1;33mNo openssl requested.[0m) 
else
ifneq ($(wildcard src/openssl/Makefile),)
  # Found a local openssl installation.
  # Ignore the system openssl libraries.
  foundlocalssl=found
  LDFLAGS+=-I./src/openssl/include/ -L./src/openssl/
endif

ifneq ($(wildcard /usr/lib64/libssl.so),)#location of ssl in CENTOS
foundssl=found
endif
ifneq ($(wildcard /usr/lib/x86_64-linux-gnu/libssl.so),)#location of ssl in Ubuntu x86
foundssl=found
endif
ifneq ($(wildcard /usr/lib/arm-linux-gnueabihf/libssl.so),)#location of ssl in Ubuntu x86
foundssl=found
endif
ifneq ($(wildcard /usr/lib/x86_64-linux-gnu/libssl.so),)#location of ssl in Ubuntu x86
foundssl=found
endif
ifneq ($(wildcard /lib/x86_64-linux-gnu/libssl3.so),)#location of ssl in Debian GNU/Linux rodete
foundssl=found
endif
endif

ifneq ($(foundlocalssl),)
  $(info [1;32mUsing *local* openssl.[0m) 
  CFLAGS+=-DMACRO_use_open_ssl 
  LIBRARIES_INCLUDED_AT_THE_END+= -l:libssl.a -l:libcrypto.a # WARNING believe it or not, the libraries must come AFTER the executable name
else 
ifneq ($(foundssl),)
  $(info [1;32mUsing *system* openssl.[0m)   
  CFLAGS+=-DMACRO_use_open_ssl 
  LIBRARIES_INCLUDED_AT_THE_END+= -lssl -lcrypto # WARNING believe it or not, the libraries must come AFTER the executable name
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
    calculator_expressions_mathml.cpp \
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
    database_user.cpp \
    database_test.cpp \
    database_internal.cpp \
    database_problem_data.cpp \
    database_backup.cpp \
    email_user.cpp \
    date_time_wrappers.cpp \
    exam_routines.cpp \
    global_objects.cpp \
    global_test.cpp \
    html_snippets.cpp \
    json.cpp \
    json_parsing.cpp \
    json_test.cpp \
    general_strings_test.cpp \
    math_algebraic_numbers.cpp \
    math_algebraic_numbers_test.cpp \
    math_basics.cpp \
    math_conversions.cpp \
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
    math_lie_theory_test.cpp \
    math_mathml.cpp \
    math_modular_arithmetic.cpp \
    math_vector_partition_functions.cpp \
    math_semisimple_lie_algebras.cpp \
    math_semisimple_lie_root_subalgebras.cpp \
    math_semisimple_lie_subalgebras.cpp \
    math_semisimple_lie_subalgebras_sltwos.cpp \
    math_semisimple_lie_subalgebras_sltwos_arbitrary_constants.cpp \
    multiprocessing.cpp \
    multitasking.cpp \
    network.cpp \
    random_system.cpp \
    source_code_formatter.cpp \
    string_constants.cpp \
    symmetric_groups_and_generalizations.cpp \
    system_functions.cpp \
    test.cpp \
    math_vectors_test.cpp \
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
ifneq ($(foundlocalssl),)
	$(MAKE) -C src/openssl
endif
	$(compiler) $(LDFLAGS) $(OBJECTS) -o $(TARGET) $(LIBRARIES_INCLUDED_AT_THE_END)

test: bin_calculator
	time ./calculator test

$(OBJECT_FILE_OUTPUT)%.o:src/%.cpp
	mkdir -p $(@D)
	$(compiler) $(CFLAGS) -MMD -MP $< -o $@
	
clean:
	rm -f $(OBJECTS) $(DEPENDENCIES) ./calculator

-include $(DEPENDENCIES)
