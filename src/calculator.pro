QT+=core
QT-=gui
TARGET=calculator
CONFIG+=console
CONFIG-=app_bundle
CONFIG+=c++11
QMAKE_CXXFLAGS+=-std=c++11

TEMPLATE=app

DEFINES+=MACRO_use_open_ssl
DEFINES+=MACRO_use_MongoDB
# DEFINES+=MACRO_use_wasm


LIBS+=-lmongoc-1.0
LIBS+=-lbson-1.0

# LIBS+=-lssl
# LIBS+=-lcrypto

INCLUDEPATH+=/usr/local/include/libmongoc-1.0
INCLUDEPATH+=/usr/local/include/libbson-1.0
INCLUDEPATH+=/usr/include/libmongoc-1.0
INCLUDEPATH+=/usr/include/libbson-1.0
HEADERS += \
    abstract_syntax_notation_one_decoder.h \
    calculator.h \
    calculator.h \
    calculator_database_mongo.h \
    calculator_educational_functions_1.h \
    calculator_html_functions.h \
    calculator_html_interpretation.h \
    calculator_inner_functions.h \
    calculator_inner_functions_vector_partition_function_1.h \
    calculator_inner_typed_functions.h \
    calculator_interface.h \
    calculator_weyl_group_characters.h \
    crypto.h \
    database.h \
    general_database_system_independent.h \
    general_file_operations_encodings.h \
    general_list_references.h \
    general_lists.h \
    general_logging_global_variables.h \
    general_maps.h \
    general_memory_saving.h \
    general_multitasking.h \
    general_test.h \
    general_time_date.h \
    math_extra_polynomial_factorization.h \
    signals_infrastructure.h \
    web_api.h \
    json.h \
    macros.h \
    math_extra_algebraic_numbers.h \
    math_extra_drawing_variables.h \
    math_extra_elliptic_curves.h \
    math_extra_elliptic_curves_implementation.h \
    math_extra_finite_groups.h \
    math_extra_finite_groups_implementation.h \
    math_extra_graph.h \
    math_extra_latex_routines.h \
    math_extra_lie_theory_extras.h \
    math_extra_modules_semisimple_lie_algebras.h \
    math_extra_modules_semisimple_lie_algebras_implementation.h \
    math_extra_packed_vector.h \
    math_extra_packed_vector_implementation.h \
    math_extra_semisimple_lie_algebras.h \
    math_extra_semisimple_lie_algebras_implementation.h \
    math_extra_semisimple_lie_algebras_root_subalgebras.h \
    math_extra_semisimple_lie_subalgebras.h \
    math_extra_symmetric_groups_and_generalizations.h \
    math_extra_tree.h \
    math_extra_universal_enveloping.h \
    math_extra_universal_enveloping_implementation.h \
    math_extra_vector_space.h \
    math_extra_weyl_algebras_implementation.h \
    math_general.h \
    math_general_implementation.h \
    math_general_polynomial_computations_advanced_implementation.h \
    math_general_polynomial_computations_basic_implementation.h \
    math_large_integers.h \
    math_subsets_selections.h \
    math_vectors.h \
    multiprocessing.h \
    source_code_formatter.h \
    string_constants.h \
    system_functions_global_objects.h \
    transport_layer_security.h \
    web_assembly.h \
    web_client.h \
    webserver.h \
    serialization_basic.h \
    general_strings.h \
    database_fallback_json.h \
    calculator_problem_storage.h \
    calculator_problem_storage.h \
    crypto_public_domain.h \
    math_rational_function.h \
    math_rational_function_implementation.h \
    calculator_functions_polynomial.h \
    math_extra_differential_operators.h \
    calculator_lie_theory.h


SOURCES += \
    abstract_syntax_notation_one_decoder.cpp \
    abstract_syntax_notation_one_decoder_test.cpp \
    calculator_conversions_1.cpp \
    calculator_educational_functions_1.cpp \
    calculator_expressions.cpp \
    calculator_finite_group_characters.cpp \
    calculator_function_registration.cpp \
    calculator_function_registration_extra.cpp \
    calculator_function_registration_lie_theory.cpp \
    calculator_function_registration_scientific.cpp \
    calculator_function_registration_vector_partition_function.cpp \
    calculator_html_interpretation.cpp \
    calculator_html_routines.cpp \
    calculator_implementation_1.cpp \
    calculator_implementation_10.cpp \
    calculator_implementation_11_trigonometry.cpp \
    calculator_implementation_12_complex_dynamics.cpp \
    calculator_implementation_2.cpp \
    calculator_implementation_3.cpp \
    calculator_implementation_4.cpp \
    calculator_implementation_5.cpp \
    calculator_implementation_6.cpp \
    calculator_implementation_6_test.cpp \
    calculator_implementation_7.cpp \
    calculator_implementation_9.cpp \
    calculator_javascript_graphics.cpp \
    calculator_latex_routines.cpp \
    calculator_parsing_routines.cpp \
    calculator_typed_functions_1.cpp \
    calculator_webserver_functions.cpp \
    crypto.cpp \
    crypto_public_domain_aes.cpp \
    crypto_public_domain_ripemd.cpp \
    crypto_public_domain.cpp \
    crypto_test.cpp \
    database.cpp \
    database_mongo.cpp \
    database_mongo_calculator.cpp \
    date_time_wrappers.cpp \
    exam_routines.cpp \
    general_strings_test.cpp \
    global_objects.cpp \
    html_snippets.cpp \
    json.cpp \
    math_algebraic_numbers.cpp \
    math_algebraic_numbers_test.cpp \
    math_elliptic_curves.cpp \
    math_finite_group_characters.cpp \
    math_floating_point.cpp \
    math_graph.cpp \
    math_hard_coded_data.cpp \
    math_implementation_1.cpp \
    math_implementation_2.cpp \
    math_implementation_3.cpp \
    math_implementation_4.cpp \
    math_implementation_5.cpp \
    math_implementation_5_test.cpp \
    math_implementation_6.cpp \
    math_implementation_6_test.cpp \
    math_semisimple_lie_algebras.cpp \
    math_semisimple_lie_root_subalgebras.cpp \
    math_semisimple_lie_subalgebras.cpp \
    multiprocessing.cpp \
    multitasking.cpp \
    random.cpp \
    random_system.cpp \
    source_code_formatter.cpp \
    string_constants.cpp \
    symmetric_groups_and_generalizations.cpp \
    system_functions.cpp \
    test.cpp \
    transport_layer_security.cpp \
    transport_layer_security_implementation.cpp \
    transport_layer_security_openssl.cpp \
    transport_layer_security_test.cpp \
    web_api_test.cpp \
    web_assembly.cpp \
    web_routines_1.cpp \
    webserver.cpp \
    webserver_fork.cpp \
    calculator_expressions_test.cpp \
    math_implementation_1_test.cpp \
    calculator_expressions_test.cpp \
    calculator_parsing_routines_test.cpp \
    database_fallback_json.cpp \
    main.cpp \
    web_api.cpp \
    json_test.cpp \
    database_test.cpp \
    global_test.cpp \
    crypto_public_domain_ripemd.cpp \
    crypto_public_domain_aes.cpp \
    crypto_sha3.cpp \
    calculator_expression_context.cpp \
    calculator_implementation_8_polynomial.cpp \
    calculator_evaluation_steps.cpp \
    calculator_lie_theory.cpp

