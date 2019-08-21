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

LIBS+=-lmongoc-1.0
LIBS+=-lbson-1.0

# LIBS+=-lssl
# LIBS+=-lcrypto

INCLUDEPATH+=/usr/local/include/libmongoc-1.0
INCLUDEPATH+=/usr/local/include/libbson-1.0

HEADERS += \
    source_code_formatter.h \
    string_constants.h \
    vpf.h \
    vpfFiniteFields.h \
    vpfHeader1General0_General.h \
    vpfHeader1General1_ListReferences.h \
    vpfHeader1General2Multitasking.h \
    vpfHeader1General3_Test.h \
    vpfHeader1General4General_Logging_GlobalVariables.h \
    vpfHeader1General5TimeDate.h \
    vpfHeader1General6Maps.h \
    vpfHeader1General7FileOperations_Encodings.h \
    vpfHeader1General8DatabaseSystemIndependent.h \
    vpfHeader2Math01_LargeIntArithmetic.h \
    vpfHeader2Math02_Vectors.h \
    vpfHeader2Math0_General.h \
    vpfHeader2Math10_LaTeXRoutines.h \
    vpfHeader2Math11_EllipticCurves.h \
    vpfHeader2Math15_UniversalEnveloping.h \
    vpfHeader2Math1_2SemisimpleLieAlgebras_RootSubalgebras.h \
    vpfHeader2Math1_3SemisimpleLieSubalgebras.h \
    vpfHeader2Math1_LieTheoryExtras.h \
    vpfHeader2Math1_SemisimpleLieAlgebras.h \
    vpfHeader2Math2_AlgebraicNumbers.h \
    vpfHeader2Math3_FiniteGroups.h \
    vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h \
    vpfHeader2Math4_5_Tree.h \
    vpfHeader2Math4_Graph.h \
    vpfHeader2Math5_SubsetsSelections.h \
    vpfHeader2Math6_ModulesSSLieAlgebras.h \
    vpfHeader2Math7_PackedVector.h \
    vpfHeader2Math8_VectorSpace.h \
    vpfHeader2Math9DrawingVariables.h \
    vpfHeader3Calculator0_Interface.h \
    vpfHeader3Calculator1_InnerTypedFunctions.h \
    vpfHeader3Calculator2_InnerFunctions.h \
    vpfHeader3Calculator3_WeylGroupCharacters.h \
    vpfHeader3Calculator4HtmlFunctions.h \
    vpfHeader3Calculator5_Database_Mongo.h \
    vpfHeader4SystemFunctionsGlobalObjects.h \
    vpfHeader5Crypto.h \
    vpfHeader6WebServerInterprocessLogistics.h \
    vpfHeader7DatabaseInterface.h \
    vpfHeader7DatabaseInterface_Mongodb.h \
    vpfHeader8HtmlInterpretation.h \
    vpfHeader8HtmlInterpretationInterface.h \
    vpfHeader8HtmlSnippets.h \
    vpfImplementationHeader2Math051_PolynomialComputations_Basic.h \
    vpfImplementationHeader2Math052_PolynomialComputations_Advanced.h \
    vpfImplementationHeader2Math0_General.h \
    vpfImplementationHeader2Math11_EllipticCurves.h \
    vpfImplementationHeader2Math15_UniversalEnveloping.h \
    vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h \
    vpfImplementationHeader2Math3_FiniteGroups.h \
    vpfImplementationHeader2Math3_WeylAlgebra.h \
    vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h \
    vpfImplementationHeader2Math7_PackedVector.h \
    vpfJson.h \
    vpfMacros.h \
    vpfPackedVector.h \
    webserver.h \
#    boringssl/crypto/asn1/asn1_locl.h \
#    boringssl/crypto/bio/internal.h \
#    boringssl/crypto/bytestring/internal.h \
#    boringssl/crypto/chacha/internal.h \
#    boringssl/crypto/cipher_extra/internal.h \
#    boringssl/crypto/conf/conf_def.h \
#    boringssl/crypto/conf/internal.h \
#    boringssl/crypto/err/internal.h \
#    boringssl/crypto/evp/internal.h \
#    boringssl/crypto/fipsmodule/aes/internal.h \
#    boringssl/crypto/fipsmodule/bn/internal.h \
#    boringssl/crypto/fipsmodule/bn/rsaz_exp.h \
#    boringssl/crypto/fipsmodule/cipher/internal.h \
#    boringssl/crypto/fipsmodule/des/internal.h \
#    boringssl/crypto/fipsmodule/digest/internal.h \
#    boringssl/crypto/fipsmodule/digest/md32_common.h \
#    boringssl/crypto/fipsmodule/ec/internal.h \
#    boringssl/crypto/fipsmodule/ec/p256-x86_64-table.h \
#    boringssl/crypto/fipsmodule/ec/p256-x86_64.h \
#    boringssl/crypto/fipsmodule/md5/internal.h \
#    boringssl/crypto/fipsmodule/modes/internal.h \
#    boringssl/crypto/fipsmodule/rand/internal.h \
#    boringssl/crypto/fipsmodule/rsa/internal.h \
#    boringssl/crypto/fipsmodule/sha/internal.h \
#    boringssl/crypto/fipsmodule/tls/internal.h \
#    boringssl/crypto/fipsmodule/delocate.h \
#    boringssl/crypto/hrss/internal.h \
#    boringssl/crypto/obj/obj_dat.h \
#    boringssl/crypto/pkcs7/internal.h \
#    boringssl/crypto/pkcs8/internal.h \
#    boringssl/crypto/poly1305/internal.h \
#    boringssl/crypto/pool/internal.h \
#    boringssl/crypto/test/abi_test.h \
#    boringssl/crypto/test/file_test.h \
#    boringssl/crypto/test/gtest_main.h \
#    boringssl/crypto/test/test_util.h \
#    boringssl/crypto/test/wycheproof_util.h \
#    boringssl/crypto/x509/charmap.h \
#    boringssl/crypto/x509/internal.h \
#    boringssl/crypto/x509/vpm_int.h \
#    boringssl/crypto/x509v3/ext_dat.h \
#    boringssl/crypto/x509v3/internal.h \
#    boringssl/crypto/x509v3/pcy_int.h \
#    boringssl/crypto/cpu-arm-linux.h \
#    boringssl/crypto/internal.h \
#    boringssl/decrepit/cast/internal.h \
#    boringssl/decrepit/ripemd/internal.h \
#    boringssl/decrepit/macros.h \
#    boringssl/fipstools/cavp_test_util.h \
#    boringssl/include/openssl/aead.h \
#    boringssl/include/openssl/aes.h \
#    boringssl/include/openssl/arm_arch.h \
#    boringssl/include/openssl/asn1.h \
#    boringssl/include/openssl/asn1_mac.h \
#    boringssl/include/openssl/asn1t.h \
#    boringssl/include/openssl/base.h \
#    boringssl/include/openssl/base64.h \
#    boringssl/include/openssl/bio.h \
#    boringssl/include/openssl/blowfish.h \
#    boringssl/include/openssl/bn.h \
#    boringssl/include/openssl/buf.h \
#    boringssl/include/openssl/buffer.h \
#    boringssl/include/openssl/bytestring.h \
#    boringssl/include/openssl/cast.h \
#    boringssl/include/openssl/chacha.h \
#    boringssl/include/openssl/cipher.h \
#    boringssl/include/openssl/cmac.h \
#    boringssl/include/openssl/conf.h \
#    boringssl/include/openssl/cpu.h \
#    boringssl/include/openssl/crypto.h \
#    boringssl/include/openssl/curve25519.h \
#    boringssl/include/openssl/des.h \
#    boringssl/include/openssl/dh.h \
#    boringssl/include/openssl/digest.h \
#    boringssl/include/openssl/dsa.h \
#    boringssl/include/openssl/dtls1.h \
#    boringssl/include/openssl/e_os2.h \
#    boringssl/include/openssl/ec.h \
#    boringssl/include/openssl/ec_key.h \
#    boringssl/include/openssl/ecdh.h \
#    boringssl/include/openssl/ecdsa.h \
#    boringssl/include/openssl/engine.h \
#    boringssl/include/openssl/err.h \
#    boringssl/include/openssl/evp.h \
#    boringssl/include/openssl/ex_data.h \
#    boringssl/include/openssl/hkdf.h \
#    boringssl/include/openssl/hmac.h \
#    boringssl/include/openssl/hrss.h \
#    boringssl/include/openssl/is_boringssl.h \
#    boringssl/include/openssl/lhash.h \
#    boringssl/include/openssl/md4.h \
#    boringssl/include/openssl/md5.h \
#    boringssl/include/openssl/mem.h \
#    boringssl/include/openssl/nid.h \
#    boringssl/include/openssl/obj.h \
#    boringssl/include/openssl/obj_mac.h \
#    boringssl/include/openssl/objects.h \
#    boringssl/include/openssl/opensslconf.h \
#    boringssl/include/openssl/opensslv.h \
#    boringssl/include/openssl/ossl_typ.h \
#    boringssl/include/openssl/pem.h \
#    boringssl/include/openssl/pkcs12.h \
#    boringssl/include/openssl/pkcs7.h \
#    boringssl/include/openssl/pkcs8.h \
#    boringssl/include/openssl/poly1305.h \
#    boringssl/include/openssl/pool.h \
#    boringssl/include/openssl/rand.h \
#    boringssl/include/openssl/rc4.h \
#    boringssl/include/openssl/ripemd.h \
#    boringssl/include/openssl/rsa.h \
#    boringssl/include/openssl/safestack.h \
#    boringssl/include/openssl/sha.h \
#    boringssl/include/openssl/span.h \
#    boringssl/include/openssl/srtp.h \
#    boringssl/include/openssl/ssl.h \
#    boringssl/include/openssl/ssl3.h \
#    boringssl/include/openssl/stack.h \
#    boringssl/include/openssl/thread.h \
#    boringssl/include/openssl/tls1.h \
#    boringssl/include/openssl/type_check.h \
#    boringssl/include/openssl/x509.h \
#    boringssl/include/openssl/x509_vfy.h \
#    boringssl/include/openssl/x509v3.h \
#    boringssl/ssl/test/runner/curve25519/const_amd64.h \
#    boringssl/ssl/test/async_bio.h \
#    boringssl/ssl/test/fuzzer.h \
#    boringssl/ssl/test/fuzzer_tags.h \
#    boringssl/ssl/test/handshake_util.h \
#    boringssl/ssl/test/packeted_bio.h \
#    boringssl/ssl/test/settings_writer.h \
#    boringssl/ssl/test/test_config.h \
#    boringssl/ssl/test/test_state.h \
#    boringssl/ssl/internal.h \
#    boringssl/third_party/fiat/curve25519_32.h \
#    boringssl/third_party/fiat/curve25519_64.h \
#    boringssl/third_party/fiat/curve25519_tables.h \
#    boringssl/third_party/fiat/internal.h \
#    boringssl/third_party/fiat/p256_32.h \
#    boringssl/third_party/fiat/p256_64.h \
#    boringssl/third_party/googletest/include/gtest/internal/custom/gtest-port.h \
#    boringssl/third_party/googletest/include/gtest/internal/custom/gtest-printers.h \
#    boringssl/third_party/googletest/include/gtest/internal/custom/gtest.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-death-test-internal.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-filepath.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-internal.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-param-util.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-port-arch.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-port.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-string.h \
#    boringssl/third_party/googletest/include/gtest/internal/gtest-type-util.h \
#    boringssl/third_party/googletest/include/gtest/gtest-death-test.h \
#    boringssl/third_party/googletest/include/gtest/gtest-matchers.h \
#    boringssl/third_party/googletest/include/gtest/gtest-message.h \
#    boringssl/third_party/googletest/include/gtest/gtest-param-test.h \
#    boringssl/third_party/googletest/include/gtest/gtest-printers.h \
#    boringssl/third_party/googletest/include/gtest/gtest-spi.h \
#    boringssl/third_party/googletest/include/gtest/gtest-test-part.h \
#    boringssl/third_party/googletest/include/gtest/gtest-typed-test.h \
#    boringssl/third_party/googletest/include/gtest/gtest.h \
#    boringssl/third_party/googletest/include/gtest/gtest_pred_impl.h \
#    boringssl/third_party/googletest/include/gtest/gtest_prod.h \
#    boringssl/third_party/googletest/samples/prime_tables.h \
#    boringssl/third_party/googletest/samples/sample1.h \
#    boringssl/third_party/googletest/samples/sample2.h \
#    boringssl/third_party/googletest/samples/sample3-inl.h \
#    boringssl/third_party/googletest/samples/sample4.h \
#    boringssl/third_party/googletest/src/gtest-internal-inl.h \
#    boringssl/third_party/googletest/test/googletest-param-test-test.h \
#    boringssl/third_party/googletest/test/gtest-typed-test_test.h \
#    boringssl/third_party/googletest/test/production.h \
#    boringssl/third_party/googletest/xcode/Samples/FrameworkSample/widget.h \
#    boringssl/third_party/sike/fpx.h \
#    boringssl/third_party/sike/isogeny.h \
#    boringssl/third_party/sike/sike.h \
#    boringssl/third_party/sike/utils.h \
#    boringssl/tool/internal.h \
#    boringssl/tool/transport_common.h \
    source_code_formatter.h \
    string_constants.h \
    vpf.h \
    vpfFiniteFields.h \
    vpfHeader1General0_General.h \
    vpfHeader1General1_ListReferences.h \
    vpfHeader1General2Multitasking.h \
    vpfHeader1General3_Test.h \
    vpfHeader1General4General_Logging_GlobalVariables.h \
    vpfHeader1General5TimeDate.h \
    vpfHeader1General6Maps.h \
    vpfHeader1General7FileOperations_Encodings.h \
    vpfHeader1General8DatabaseSystemIndependent.h \
    vpfHeader2Math01_LargeIntArithmetic.h \
    vpfHeader2Math02_Vectors.h \
    vpfHeader2Math0_General.h \
    vpfHeader2Math10_LaTeXRoutines.h \
    vpfHeader2Math11_EllipticCurves.h \
    vpfHeader2Math15_UniversalEnveloping.h \
    vpfHeader2Math1_2SemisimpleLieAlgebras_RootSubalgebras.h \
    vpfHeader2Math1_3SemisimpleLieSubalgebras.h \
    vpfHeader2Math1_LieTheoryExtras.h \
    vpfHeader2Math1_SemisimpleLieAlgebras.h \
    vpfHeader2Math2_AlgebraicNumbers.h \
    vpfHeader2Math3_FiniteGroups.h \
    vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h \
    vpfHeader2Math4_5_Tree.h \
    vpfHeader2Math4_Graph.h \
    vpfHeader2Math5_SubsetsSelections.h \
    vpfHeader2Math6_ModulesSSLieAlgebras.h \
    vpfHeader2Math7_PackedVector.h \
    vpfHeader2Math8_VectorSpace.h \
    vpfHeader2Math9DrawingVariables.h \
    vpfHeader3Calculator0_Interface.h \
    vpfHeader3Calculator1_InnerTypedFunctions.h \
    vpfHeader3Calculator2_InnerFunctions.h \
    vpfHeader3Calculator3_WeylGroupCharacters.h \
    vpfHeader3Calculator4HtmlFunctions.h \
    vpfHeader3Calculator5_Database_Mongo.h \
    vpfHeader4SystemFunctionsGlobalObjects.h \
    vpfHeader5Crypto.h \
    vpfHeader6WebServerInterprocessLogistics.h \
    vpfHeader7DatabaseInterface.h \
    vpfHeader7DatabaseInterface_Mongodb.h \
    vpfHeader8HtmlInterpretation.h \
    vpfHeader8HtmlInterpretationInterface.h \
    vpfHeader8HtmlSnippets.h \
    vpfImplementationHeader2Math051_PolynomialComputations_Basic.h \
    vpfImplementationHeader2Math052_PolynomialComputations_Advanced.h \
    vpfImplementationHeader2Math0_General.h \
    vpfImplementationHeader2Math11_EllipticCurves.h \
    vpfImplementationHeader2Math15_UniversalEnveloping.h \
    vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h \
    vpfImplementationHeader2Math3_FiniteGroups.h \
    vpfImplementationHeader2Math3_WeylAlgebra.h \
    vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h \
    vpfImplementationHeader2Math7_PackedVector.h \
    vpfJson.h \
    vpfMacros.h \
    vpfPackedVector.h \
    webserver.h \
    transport_layer_security.h \
    abstract_syntax_notation_one_decoder.h \
    vpfHeader1General9_MemorySaving.h

SOURCES += \
    database.cpp \
    database_mongo_calculator.cpp \
    databasemongo.cpp \
    examRoutines.cpp \
    source_code_formatter.cpp \
    string_constants.cpp \
    test.cpp \
    vpf2Math3_SymmetricGroupsAndGeneralizations.cpp \
    vpf4CalculatorFunctionList.cpp \
    vpf5.cpp \
    vpf5_1CalculatorCode.cpp \
    vpf5_2javascript_graphics.cpp \
    vpf6.cpp \
    vpf6_05evaluationroutines.cpp \
    vpf6_0parsingRoutines.cpp \
    vpf6_1innerTypedFunctions.cpp \
    vpf6_2serialization_code.cpp \
    vpf6_35calculatorMathRoutinesPart2.cpp \
    vpf6_36calculatorHtmlRoutines.cpp \
    vpf6_37calculatorroutines.cpp \
    vpf6_38LaTeXRoutines.cpp \
    vpf6_3calculatorMathRoutines.cpp \
    vpf6_4ExpressionsImplementation.cpp \
    vpf7.cpp \
    vpf8.cpp \
    vpf9.cpp \
    vpf99_HardcodedData.cpp \
    vpf9_1.cpp \
    vpf9_2.cpp \
    vpf9_3RationalRadicals.cpp \
    vpf9_4SemisimpleLieAlgebras.cpp \
    vpf9_5SemisimpleLieAlgebras_RootSubalgebras.cpp \
    vpf9_6SemisimpleLieSubAlgebras.cpp \
    vpf9_7floating_point_routines.cpp \
    vpf9_85TimeDateWrappers.cpp \
    vpf9_8GlobalObjects.cpp \
    vpf9_92multitasking.cpp \
    vpf9_9SystemFunctions.cpp \
    vpfCharacters.cpp \
    vpfCharacters_CalculatorInterface.cpp \
    vpfCrypto.cpp \
    vpfCrypto_AES.cpp \
    vpfCrypto_ripemd.cpp \
    vpfCrypto_sha3.cpp \
    vpfEllipticCurves.cpp \
    vpfFiniteFields.cpp \
    vpfGraph.cpp \
    vpfHtmlInterpretationInterface.cpp \
    vpfHtmlSnippets.cpp \
    vpfJson.cpp \
    vpfWebServerInterProcessLogistics.cpp \
    web-routines-1.cpp \
    webserver.cpp \
#    boringssl/crypto/asn1/asn1_test.cc \
#    boringssl/crypto/base64/base64_test.cc \
#    boringssl/crypto/bio/bio_test.cc \
#    boringssl/crypto/buf/buf_test.cc \
#    boringssl/crypto/bytestring/bytestring_test.cc \
#    boringssl/crypto/chacha/chacha_test.cc \
#    boringssl/crypto/cipher_extra/aead_test.cc \
#    boringssl/crypto/cipher_extra/cipher_test.cc \
#    boringssl/crypto/cmac/cmac_test.cc \
#    boringssl/crypto/curve25519/ed25519_test.cc \
#    boringssl/crypto/curve25519/spake25519_test.cc \
#    boringssl/crypto/curve25519/x25519_test.cc \
#    boringssl/crypto/dh/dh_test.cc \
#    boringssl/crypto/digest_extra/digest_test.cc \
#    boringssl/crypto/dsa/dsa_test.cc \
#    boringssl/crypto/ecdh_extra/ecdh_test.cc \
#    boringssl/crypto/err/err_test.cc \
#    boringssl/crypto/evp/evp_extra_test.cc \
#    boringssl/crypto/evp/evp_test.cc \
#    boringssl/crypto/evp/pbkdf_test.cc \
#    boringssl/crypto/evp/scrypt_test.cc \
#    boringssl/crypto/fipsmodule/aes/aes_test.cc \
#    boringssl/crypto/fipsmodule/bn/bn_test.cc \
#    boringssl/crypto/fipsmodule/ec/ec_test.cc \
#    boringssl/crypto/fipsmodule/ec/p256-x86_64_test.cc \
#    boringssl/crypto/fipsmodule/ecdsa/ecdsa_test.cc \
#    boringssl/crypto/fipsmodule/md5/md5_test.cc \
#    boringssl/crypto/fipsmodule/modes/gcm_test.cc \
#    boringssl/crypto/fipsmodule/rand/ctrdrbg_test.cc \
#    boringssl/crypto/fipsmodule/sha/sha_test.cc \
#    boringssl/crypto/hkdf/hkdf_test.cc \
#    boringssl/crypto/hmac_extra/hmac_test.cc \
#    boringssl/crypto/hrss/hrss_test.cc \
#    boringssl/crypto/lhash/lhash_test.cc \
#    boringssl/crypto/obj/obj_test.cc \
#    boringssl/crypto/pem/pem_test.cc \
#    boringssl/crypto/pkcs7/pkcs7_test.cc \
#    boringssl/crypto/pkcs8/pkcs12_test.cc \
#    boringssl/crypto/pkcs8/pkcs8_test.cc \
#    boringssl/crypto/poly1305/poly1305_test.cc \
#    boringssl/crypto/pool/pool_test.cc \
#    boringssl/crypto/rand_extra/rand_test.cc \
#    boringssl/crypto/rsa_extra/rsa_test.cc \
#    boringssl/crypto/stack/stack_test.cc \
#    boringssl/crypto/test/abi_test.cc \
#    boringssl/crypto/test/file_test.cc \
#    boringssl/crypto/test/file_test_gtest.cc \
#    boringssl/crypto/test/gtest_main.cc \
#    boringssl/crypto/test/malloc.cc \
#    boringssl/crypto/test/test_util.cc \
#    boringssl/crypto/test/wycheproof_util.cc \
#    boringssl/crypto/x509/x509_test.cc \
#    boringssl/crypto/x509/x509_time_test.cc \
#    boringssl/crypto/x509v3/tab_test.cc \
#    boringssl/crypto/x509v3/v3name_test.cc \
#    boringssl/crypto/abi_self_test.cc \
#    boringssl/crypto/compiler_test.cc \
#    boringssl/crypto/constant_time_test.cc \
#    boringssl/crypto/cpu-arm-linux_test.cc \
#    boringssl/crypto/impl_dispatch_test.cc \
#    boringssl/crypto/refcount_test.cc \
#    boringssl/crypto/self_test.cc \
#    boringssl/crypto/thread_test.cc \
#    boringssl/decrepit/blowfish/blowfish_test.cc \
#    boringssl/decrepit/cast/cast_test.cc \
#    boringssl/decrepit/cfb/cfb_test.cc \
#    boringssl/decrepit/ripemd/ripemd_test.cc \
#    boringssl/fipstools/cavp_aes_gcm_test.cc \
#    boringssl/fipstools/cavp_aes_test.cc \
#    boringssl/fipstools/cavp_ctr_drbg_test.cc \
#    boringssl/fipstools/cavp_ecdsa2_keypair_test.cc \
#    boringssl/fipstools/cavp_ecdsa2_pkv_test.cc \
#    boringssl/fipstools/cavp_ecdsa2_siggen_test.cc \
#    boringssl/fipstools/cavp_ecdsa2_sigver_test.cc \
#    boringssl/fipstools/cavp_hmac_test.cc \
#    boringssl/fipstools/cavp_kas_test.cc \
#    boringssl/fipstools/cavp_keywrap_test.cc \
#    boringssl/fipstools/cavp_main.cc \
#    boringssl/fipstools/cavp_rsa2_keygen_test.cc \
#    boringssl/fipstools/cavp_rsa2_siggen_test.cc \
#    boringssl/fipstools/cavp_rsa2_sigver_test.cc \
#    boringssl/fipstools/cavp_sha_monte_test.cc \
#    boringssl/fipstools/cavp_sha_test.cc \
#    boringssl/fipstools/cavp_tdes_test.cc \
#    boringssl/fipstools/cavp_test_util.cc \
#    boringssl/fipstools/cavp_tlskdf_test.cc \
#    boringssl/ssl/test/async_bio.cc \
#    boringssl/ssl/test/bssl_shim.cc \
#    boringssl/ssl/test/handshake_util.cc \
#    boringssl/ssl/test/handshaker.cc \
#    boringssl/ssl/test/packeted_bio.cc \
#    boringssl/ssl/test/settings_writer.cc \
#    boringssl/ssl/test/test_config.cc \
#    boringssl/ssl/test/test_state.cc \
#    boringssl/ssl/bio_ssl.cc \
#    boringssl/ssl/d1_both.cc \
#    boringssl/ssl/d1_lib.cc \
#    boringssl/ssl/d1_pkt.cc \
#    boringssl/ssl/d1_srtp.cc \
#    boringssl/ssl/dtls_method.cc \
#    boringssl/ssl/dtls_record.cc \
#    boringssl/ssl/handoff.cc \
#    boringssl/ssl/handshake.cc \
#    boringssl/ssl/handshake_client.cc \
#    boringssl/ssl/handshake_server.cc \
#    boringssl/ssl/s3_both.cc \
#    boringssl/ssl/s3_lib.cc \
#    boringssl/ssl/s3_pkt.cc \
#    boringssl/ssl/span_test.cc \
#    boringssl/ssl/ssl_aead_ctx.cc \
#    boringssl/ssl/ssl_asn1.cc \
#    boringssl/ssl/ssl_buffer.cc \
#    boringssl/ssl/ssl_cert.cc \
#    boringssl/ssl/ssl_cipher.cc \
#    boringssl/ssl/ssl_file.cc \
#    boringssl/ssl/ssl_key_share.cc \
#    boringssl/ssl/ssl_lib.cc \
#    boringssl/ssl/ssl_privkey.cc \
#    boringssl/ssl/ssl_session.cc \
#    boringssl/ssl/ssl_stat.cc \
#    boringssl/ssl/ssl_test.cc \
#    boringssl/ssl/ssl_transcript.cc \
#    boringssl/ssl/ssl_versions.cc \
#    boringssl/ssl/ssl_x509.cc \
#    boringssl/ssl/t1_enc.cc \
#    boringssl/ssl/t1_lib.cc \
#    boringssl/ssl/tls13_both.cc \
#    boringssl/ssl/tls13_client.cc \
#    boringssl/ssl/tls13_enc.cc \
#    boringssl/ssl/tls13_server.cc \
#    boringssl/ssl/tls_method.cc \
#    boringssl/ssl/tls_record.cc \
#    boringssl/third_party/googletest/codegear/gtest_all.cc \
#    boringssl/third_party/googletest/codegear/gtest_link.cc \
#    boringssl/third_party/googletest/samples/sample1.cc \
#    boringssl/third_party/googletest/samples/sample10_unittest.cc \
#    boringssl/third_party/googletest/samples/sample1_unittest.cc \
#    boringssl/third_party/googletest/samples/sample2.cc \
#    boringssl/third_party/googletest/samples/sample2_unittest.cc \
#    boringssl/third_party/googletest/samples/sample3_unittest.cc \
#    boringssl/third_party/googletest/samples/sample4.cc \
#    boringssl/third_party/googletest/samples/sample4_unittest.cc \
#    boringssl/third_party/googletest/samples/sample5_unittest.cc \
#    boringssl/third_party/googletest/samples/sample6_unittest.cc \
#    boringssl/third_party/googletest/samples/sample7_unittest.cc \
#    boringssl/third_party/googletest/samples/sample8_unittest.cc \
#    boringssl/third_party/googletest/samples/sample9_unittest.cc \
#    boringssl/third_party/googletest/src/gtest-all.cc \
#    boringssl/third_party/googletest/src/gtest-death-test.cc \
#    boringssl/third_party/googletest/src/gtest-filepath.cc \
#    boringssl/third_party/googletest/src/gtest-matchers.cc \
#    boringssl/third_party/googletest/src/gtest-port.cc \
#    boringssl/third_party/googletest/src/gtest-printers.cc \
#    boringssl/third_party/googletest/src/gtest-test-part.cc \
#    boringssl/third_party/googletest/src/gtest-typed-test.cc \
#    boringssl/third_party/googletest/src/gtest.cc \
#    boringssl/third_party/googletest/src/gtest_main.cc \
#    boringssl/third_party/googletest/test/googletest-break-on-failure-unittest_.cc \
#    boringssl/third_party/googletest/test/googletest-catch-exceptions-test_.cc \
#    boringssl/third_party/googletest/test/googletest-color-test_.cc \
#    boringssl/third_party/googletest/test/googletest-death-test-test.cc \
#    boringssl/third_party/googletest/test/googletest-death-test_ex_test.cc \
#    boringssl/third_party/googletest/test/googletest-env-var-test_.cc \
#    boringssl/third_party/googletest/test/googletest-filepath-test.cc \
#    boringssl/third_party/googletest/test/googletest-filter-unittest_.cc \
#    boringssl/third_party/googletest/test/googletest-list-tests-unittest_.cc \
#    boringssl/third_party/googletest/test/googletest-listener-test.cc \
#    boringssl/third_party/googletest/test/googletest-message-test.cc \
#    boringssl/third_party/googletest/test/googletest-options-test.cc \
#    boringssl/third_party/googletest/test/googletest-output-test_.cc \
#    boringssl/third_party/googletest/test/googletest-param-test-invalid-name1-test_.cc \
#    boringssl/third_party/googletest/test/googletest-param-test-invalid-name2-test_.cc \
#    boringssl/third_party/googletest/test/googletest-param-test-test.cc \
#    boringssl/third_party/googletest/test/googletest-param-test2-test.cc \
#    boringssl/third_party/googletest/test/googletest-port-test.cc \
#    boringssl/third_party/googletest/test/googletest-printers-test.cc \
#    boringssl/third_party/googletest/test/googletest-shuffle-test_.cc \
#    boringssl/third_party/googletest/test/googletest-test-part-test.cc \
#    boringssl/third_party/googletest/test/googletest-test2_test.cc \
#    boringssl/third_party/googletest/test/googletest-throw-on-failure-test_.cc \
#    boringssl/third_party/googletest/test/googletest-uninitialized-test_.cc \
#    boringssl/third_party/googletest/test/gtest-typed-test2_test.cc \
#    boringssl/third_party/googletest/test/gtest-typed-test_test.cc \
#    boringssl/third_party/googletest/test/gtest-unittest-api_test.cc \
#    boringssl/third_party/googletest/test/gtest_all_test.cc \
#    boringssl/third_party/googletest/test/gtest_assert_by_exception_test.cc \
#    boringssl/third_party/googletest/test/gtest_environment_test.cc \
#    boringssl/third_party/googletest/test/gtest_help_test_.cc \
#    boringssl/third_party/googletest/test/gtest_list_output_unittest_.cc \
#    boringssl/third_party/googletest/test/gtest_main_unittest.cc \
#    boringssl/third_party/googletest/test/gtest_no_test_unittest.cc \
#    boringssl/third_party/googletest/test/gtest_pred_impl_unittest.cc \
#    boringssl/third_party/googletest/test/gtest_premature_exit_test.cc \
#    boringssl/third_party/googletest/test/gtest_prod_test.cc \
#    boringssl/third_party/googletest/test/gtest_repeat_test.cc \
#    boringssl/third_party/googletest/test/gtest_skip_in_environment_setup_test.cc \
#    boringssl/third_party/googletest/test/gtest_skip_test.cc \
#    boringssl/third_party/googletest/test/gtest_sole_header_test.cc \
#    boringssl/third_party/googletest/test/gtest_stress_test.cc \
#    boringssl/third_party/googletest/test/gtest_test_macro_stack_footprint_test.cc \
#    boringssl/third_party/googletest/test/gtest_testbridge_test_.cc \
#    boringssl/third_party/googletest/test/gtest_throw_on_failure_ex_test.cc \
#    boringssl/third_party/googletest/test/gtest_unittest.cc \
#    boringssl/third_party/googletest/test/gtest_xml_outfile1_test_.cc \
#    boringssl/third_party/googletest/test/gtest_xml_outfile2_test_.cc \
#    boringssl/third_party/googletest/test/gtest_xml_output_unittest_.cc \
#    boringssl/third_party/googletest/test/production.cc \
#    boringssl/third_party/googletest/xcode/Samples/FrameworkSample/widget.cc \
#    boringssl/third_party/googletest/xcode/Samples/FrameworkSample/widget_test.cc \
#    boringssl/third_party/sike/sike_test.cc \
#    boringssl/tool/args.cc \
#    boringssl/tool/ciphers.cc \
#    boringssl/tool/client.cc \
#    boringssl/tool/const.cc \
#    boringssl/tool/digest.cc \
#    boringssl/tool/file.cc \
#    boringssl/tool/generate_ed25519.cc \
#    boringssl/tool/genrsa.cc \
#    boringssl/tool/pkcs12.cc \
#    boringssl/tool/rand.cc \
#    boringssl/tool/server.cc \
#    boringssl/tool/sign.cc \
#    boringssl/tool/speed.cc \
#    boringssl/tool/tool.cc \
#    boringssl/tool/transport_common.cc \
#    boringssl/crypto/asn1/a_bitstr.c \
#    boringssl/crypto/asn1/a_bool.c \
#    boringssl/crypto/asn1/a_d2i_fp.c \
#    boringssl/crypto/asn1/a_dup.c \
#    boringssl/crypto/asn1/a_enum.c \
#    boringssl/crypto/asn1/a_gentm.c \
#    boringssl/crypto/asn1/a_i2d_fp.c \
#    boringssl/crypto/asn1/a_int.c \
#    boringssl/crypto/asn1/a_mbstr.c \
#    boringssl/crypto/asn1/a_object.c \
#    boringssl/crypto/asn1/a_octet.c \
#    boringssl/crypto/asn1/a_print.c \
#    boringssl/crypto/asn1/a_strnid.c \
#    boringssl/crypto/asn1/a_time.c \
#    boringssl/crypto/asn1/a_type.c \
#    boringssl/crypto/asn1/a_utctm.c \
#    boringssl/crypto/asn1/a_utf8.c \
#    boringssl/crypto/asn1/asn1_lib.c \
#    boringssl/crypto/asn1/asn1_par.c \
#    boringssl/crypto/asn1/asn_pack.c \
#    boringssl/crypto/asn1/f_enum.c \
#    boringssl/crypto/asn1/f_int.c \
#    boringssl/crypto/asn1/f_string.c \
#    boringssl/crypto/asn1/tasn_dec.c \
#    boringssl/crypto/asn1/tasn_enc.c \
#    boringssl/crypto/asn1/tasn_fre.c \
#    boringssl/crypto/asn1/tasn_new.c \
#    boringssl/crypto/asn1/tasn_typ.c \
#    boringssl/crypto/asn1/tasn_utl.c \
#    boringssl/crypto/asn1/time_support.c \
#    boringssl/crypto/base64/base64.c \
#    boringssl/crypto/bio/bio.c \
#    boringssl/crypto/bio/bio_mem.c \
#    boringssl/crypto/bio/connect.c \
#    boringssl/crypto/bio/fd.c \
#    boringssl/crypto/bio/file.c \
#    boringssl/crypto/bio/hexdump.c \
#    boringssl/crypto/bio/pair.c \
#    boringssl/crypto/bio/printf.c \
#    boringssl/crypto/bio/socket.c \
#    boringssl/crypto/bio/socket_helper.c \
#    boringssl/crypto/bn_extra/bn_asn1.c \
#    boringssl/crypto/bn_extra/convert.c \
#    boringssl/crypto/buf/buf.c \
#    boringssl/crypto/bytestring/asn1_compat.c \
#    boringssl/crypto/bytestring/ber.c \
#    boringssl/crypto/bytestring/cbb.c \
#    boringssl/crypto/bytestring/cbs.c \
#    boringssl/crypto/bytestring/unicode.c \
#    boringssl/crypto/chacha/chacha.c \
#    boringssl/crypto/cipher_extra/cipher_extra.c \
#    boringssl/crypto/cipher_extra/derive_key.c \
#    boringssl/crypto/cipher_extra/e_aesccm.c \
#    boringssl/crypto/cipher_extra/e_aesctrhmac.c \
#    boringssl/crypto/cipher_extra/e_aesgcmsiv.c \
#    boringssl/crypto/cipher_extra/e_chacha20poly1305.c \
#    boringssl/crypto/cipher_extra/e_null.c \
#    boringssl/crypto/cipher_extra/e_rc2.c \
#    boringssl/crypto/cipher_extra/e_rc4.c \
#    boringssl/crypto/cipher_extra/e_tls.c \
#    boringssl/crypto/cipher_extra/tls_cbc.c \
#    boringssl/crypto/cmac/cmac.c \
#    boringssl/crypto/conf/conf.c \
#    boringssl/crypto/curve25519/spake25519.c \
#    boringssl/crypto/dh/check.c \
#    boringssl/crypto/dh/dh.c \
#    boringssl/crypto/dh/dh_asn1.c \
#    boringssl/crypto/dh/params.c \
#    boringssl/crypto/digest_extra/digest_extra.c \
#    boringssl/crypto/dsa/dsa.c \
#    boringssl/crypto/dsa/dsa_asn1.c \
#    boringssl/crypto/ec_extra/ec_asn1.c \
#    boringssl/crypto/ecdh_extra/ecdh_extra.c \
#    boringssl/crypto/ecdsa_extra/ecdsa_asn1.c \
#    boringssl/crypto/engine/engine.c \
#    boringssl/crypto/err/err.c \
#    boringssl/crypto/evp/digestsign.c \
#    boringssl/crypto/evp/evp.c \
#    boringssl/crypto/evp/evp_asn1.c \
#    boringssl/crypto/evp/evp_ctx.c \
#    boringssl/crypto/evp/p_dsa_asn1.c \
#    boringssl/crypto/evp/p_ec.c \
#    boringssl/crypto/evp/p_ec_asn1.c \
#    boringssl/crypto/evp/p_ed25519.c \
#    boringssl/crypto/evp/p_ed25519_asn1.c \
#    boringssl/crypto/evp/p_rsa.c \
#    boringssl/crypto/evp/p_rsa_asn1.c \
#    boringssl/crypto/evp/pbkdf.c \
#    boringssl/crypto/evp/print.c \
#    boringssl/crypto/evp/scrypt.c \
#    boringssl/crypto/evp/sign.c \
#    boringssl/crypto/fipsmodule/aes/aes.c \
#    boringssl/crypto/fipsmodule/aes/key_wrap.c \
#    boringssl/crypto/fipsmodule/aes/mode_wrappers.c \
#    boringssl/crypto/fipsmodule/bn/asm/x86_64-gcc.c \
#    boringssl/crypto/fipsmodule/bn/add.c \
#    boringssl/crypto/fipsmodule/bn/bn.c \
#    boringssl/crypto/fipsmodule/bn/bytes.c \
#    boringssl/crypto/fipsmodule/bn/cmp.c \
#    boringssl/crypto/fipsmodule/bn/ctx.c \
#    boringssl/crypto/fipsmodule/bn/div.c \
#    boringssl/crypto/fipsmodule/bn/div_extra.c \
#    boringssl/crypto/fipsmodule/bn/exponentiation.c \
#    boringssl/crypto/fipsmodule/bn/gcd.c \
#    boringssl/crypto/fipsmodule/bn/gcd_extra.c \
#    boringssl/crypto/fipsmodule/bn/generic.c \
#    boringssl/crypto/fipsmodule/bn/jacobi.c \
#    boringssl/crypto/fipsmodule/bn/montgomery.c \
#    boringssl/crypto/fipsmodule/bn/montgomery_inv.c \
#    boringssl/crypto/fipsmodule/bn/mul.c \
#    boringssl/crypto/fipsmodule/bn/prime.c \
#    boringssl/crypto/fipsmodule/bn/random.c \
#    boringssl/crypto/fipsmodule/bn/rsaz_exp.c \
#    boringssl/crypto/fipsmodule/bn/shift.c \
#    boringssl/crypto/fipsmodule/bn/sqrt.c \
#    boringssl/crypto/fipsmodule/cipher/aead.c \
#    boringssl/crypto/fipsmodule/cipher/cipher.c \
#    boringssl/crypto/fipsmodule/cipher/e_aes.c \
#    boringssl/crypto/fipsmodule/cipher/e_des.c \
#    boringssl/crypto/fipsmodule/des/des.c \
#    boringssl/crypto/fipsmodule/digest/digest.c \
#    boringssl/crypto/fipsmodule/digest/digests.c \
#    boringssl/crypto/fipsmodule/ec/ec.c \
#    boringssl/crypto/fipsmodule/ec/ec_key.c \
#    boringssl/crypto/fipsmodule/ec/ec_montgomery.c \
#    boringssl/crypto/fipsmodule/ec/felem.c \
#    boringssl/crypto/fipsmodule/ec/oct.c \
#    boringssl/crypto/fipsmodule/ec/p224-64.c \
#    boringssl/crypto/fipsmodule/ec/p256-x86_64.c \
#    boringssl/crypto/fipsmodule/ec/scalar.c \
#    boringssl/crypto/fipsmodule/ec/simple.c \
#    boringssl/crypto/fipsmodule/ec/simple_mul.c \
#    boringssl/crypto/fipsmodule/ec/util.c \
#    boringssl/crypto/fipsmodule/ec/wnaf.c \
#    boringssl/crypto/fipsmodule/ecdh/ecdh.c \
#    boringssl/crypto/fipsmodule/ecdsa/ecdsa.c \
#    boringssl/crypto/fipsmodule/hmac/hmac.c \
#    boringssl/crypto/fipsmodule/md4/md4.c \
#    boringssl/crypto/fipsmodule/md5/md5.c \
#    boringssl/crypto/fipsmodule/modes/cbc.c \
#    boringssl/crypto/fipsmodule/modes/ccm.c \
#    boringssl/crypto/fipsmodule/modes/cfb.c \
#    boringssl/crypto/fipsmodule/modes/ctr.c \
#    boringssl/crypto/fipsmodule/modes/gcm.c \
#    boringssl/crypto/fipsmodule/modes/ofb.c \
#    boringssl/crypto/fipsmodule/modes/polyval.c \
#    boringssl/crypto/fipsmodule/rand/ctrdrbg.c \
#    boringssl/crypto/fipsmodule/rand/rand.c \
#    boringssl/crypto/fipsmodule/rand/urandom.c \
#    boringssl/crypto/fipsmodule/rsa/blinding.c \
#    boringssl/crypto/fipsmodule/rsa/padding.c \
#    boringssl/crypto/fipsmodule/rsa/rsa.c \
#    boringssl/crypto/fipsmodule/rsa/rsa_impl.c \
#    boringssl/crypto/fipsmodule/self_check/self_check.c \
#    boringssl/crypto/fipsmodule/sha/sha1-altivec.c \
#    boringssl/crypto/fipsmodule/sha/sha1.c \
#    boringssl/crypto/fipsmodule/sha/sha256.c \
#    boringssl/crypto/fipsmodule/sha/sha512.c \
#    boringssl/crypto/fipsmodule/tls/kdf.c \
#    boringssl/crypto/fipsmodule/bcm.c \
#    boringssl/crypto/fipsmodule/fips_shared_support.c \
#    boringssl/crypto/fipsmodule/is_fips.c \
#    boringssl/crypto/hkdf/hkdf.c \
#    boringssl/crypto/hrss/hrss.c \
#    boringssl/crypto/lhash/lhash.c \
#    boringssl/crypto/obj/obj.c \
#    boringssl/crypto/obj/obj_xref.c \
#    boringssl/crypto/pem/pem_all.c \
#    boringssl/crypto/pem/pem_info.c \
#    boringssl/crypto/pem/pem_lib.c \
#    boringssl/crypto/pem/pem_oth.c \
#    boringssl/crypto/pem/pem_pk8.c \
#    boringssl/crypto/pem/pem_pkey.c \
#    boringssl/crypto/pem/pem_x509.c \
#    boringssl/crypto/pem/pem_xaux.c \
#    boringssl/crypto/pkcs7/pkcs7.c \
#    boringssl/crypto/pkcs7/pkcs7_x509.c \
#    boringssl/crypto/pkcs8/p5_pbev2.c \
#    boringssl/crypto/pkcs8/pkcs8.c \
#    boringssl/crypto/pkcs8/pkcs8_x509.c \
#    boringssl/crypto/poly1305/poly1305.c \
#    boringssl/crypto/poly1305/poly1305_arm.c \
#    boringssl/crypto/poly1305/poly1305_vec.c \
#    boringssl/crypto/pool/pool.c \
#    boringssl/crypto/rand_extra/deterministic.c \
#    boringssl/crypto/rand_extra/forkunsafe.c \
#    boringssl/crypto/rand_extra/fuchsia.c \
#    boringssl/crypto/rand_extra/rand_extra.c \
#    boringssl/crypto/rand_extra/windows.c \
#    boringssl/crypto/rc4/rc4.c \
#    boringssl/crypto/rsa_extra/rsa_asn1.c \
#    boringssl/crypto/rsa_extra/rsa_print.c \
#    boringssl/crypto/stack/stack.c \
#    boringssl/crypto/x509/a_digest.c \
#    boringssl/crypto/x509/a_sign.c \
#    boringssl/crypto/x509/a_strex.c \
#    boringssl/crypto/x509/a_verify.c \
#    boringssl/crypto/x509/algorithm.c \
#    boringssl/crypto/x509/asn1_gen.c \
#    boringssl/crypto/x509/by_dir.c \
#    boringssl/crypto/x509/by_file.c \
#    boringssl/crypto/x509/i2d_pr.c \
#    boringssl/crypto/x509/rsa_pss.c \
#    boringssl/crypto/x509/t_crl.c \
#    boringssl/crypto/x509/t_req.c \
#    boringssl/crypto/x509/t_x509.c \
#    boringssl/crypto/x509/t_x509a.c \
#    boringssl/crypto/x509/x509.c \
#    boringssl/crypto/x509/x509_att.c \
#    boringssl/crypto/x509/x509_cmp.c \
#    boringssl/crypto/x509/x509_d2.c \
#    boringssl/crypto/x509/x509_def.c \
#    boringssl/crypto/x509/x509_ext.c \
#    boringssl/crypto/x509/x509_lu.c \
#    boringssl/crypto/x509/x509_obj.c \
#    boringssl/crypto/x509/x509_r2x.c \
#    boringssl/crypto/x509/x509_req.c \
#    boringssl/crypto/x509/x509_set.c \
#    boringssl/crypto/x509/x509_trs.c \
#    boringssl/crypto/x509/x509_txt.c \
#    boringssl/crypto/x509/x509_v3.c \
#    boringssl/crypto/x509/x509_vfy.c \
#    boringssl/crypto/x509/x509_vpm.c \
#    boringssl/crypto/x509/x509cset.c \
#    boringssl/crypto/x509/x509name.c \
#    boringssl/crypto/x509/x509rset.c \
#    boringssl/crypto/x509/x509spki.c \
#    boringssl/crypto/x509/x_algor.c \
#    boringssl/crypto/x509/x_all.c \
#    boringssl/crypto/x509/x_attrib.c \
#    boringssl/crypto/x509/x_crl.c \
#    boringssl/crypto/x509/x_exten.c \
#    boringssl/crypto/x509/x_info.c \
#    boringssl/crypto/x509/x_name.c \
#    boringssl/crypto/x509/x_pkey.c \
#    boringssl/crypto/x509/x_pubkey.c \
#    boringssl/crypto/x509/x_req.c \
#    boringssl/crypto/x509/x_sig.c \
#    boringssl/crypto/x509/x_spki.c \
#    boringssl/crypto/x509/x_val.c \
#    boringssl/crypto/x509/x_x509.c \
#    boringssl/crypto/x509/x_x509a.c \
#    boringssl/crypto/x509v3/pcy_cache.c \
#    boringssl/crypto/x509v3/pcy_data.c \
#    boringssl/crypto/x509v3/pcy_lib.c \
#    boringssl/crypto/x509v3/pcy_map.c \
#    boringssl/crypto/x509v3/pcy_node.c \
#    boringssl/crypto/x509v3/pcy_tree.c \
#    boringssl/crypto/x509v3/v3_akey.c \
#    boringssl/crypto/x509v3/v3_akeya.c \
#    boringssl/crypto/x509v3/v3_alt.c \
#    boringssl/crypto/x509v3/v3_bcons.c \
#    boringssl/crypto/x509v3/v3_bitst.c \
#    boringssl/crypto/x509v3/v3_conf.c \
#    boringssl/crypto/x509v3/v3_cpols.c \
#    boringssl/crypto/x509v3/v3_crld.c \
#    boringssl/crypto/x509v3/v3_enum.c \
#    boringssl/crypto/x509v3/v3_extku.c \
#    boringssl/crypto/x509v3/v3_genn.c \
#    boringssl/crypto/x509v3/v3_ia5.c \
#    boringssl/crypto/x509v3/v3_info.c \
#    boringssl/crypto/x509v3/v3_int.c \
#    boringssl/crypto/x509v3/v3_lib.c \
#    boringssl/crypto/x509v3/v3_ncons.c \
#    boringssl/crypto/x509v3/v3_ocsp.c \
#    boringssl/crypto/x509v3/v3_pci.c \
#    boringssl/crypto/x509v3/v3_pcia.c \
#    boringssl/crypto/x509v3/v3_pcons.c \
#    boringssl/crypto/x509v3/v3_pku.c \
#    boringssl/crypto/x509v3/v3_pmaps.c \
#    boringssl/crypto/x509v3/v3_prn.c \
#    boringssl/crypto/x509v3/v3_purp.c \
#    boringssl/crypto/x509v3/v3_skey.c \
#    boringssl/crypto/x509v3/v3_sxnet.c \
#    boringssl/crypto/x509v3/v3_utl.c \
#    boringssl/crypto/cpu-aarch64-fuchsia.c \
#    boringssl/crypto/cpu-aarch64-linux.c \
#    boringssl/crypto/cpu-arm-linux.c \
#    boringssl/crypto/cpu-arm.c \
#    boringssl/crypto/cpu-intel.c \
#    boringssl/crypto/cpu-ppc64le.c \
#    boringssl/crypto/crypto.c \
#    boringssl/crypto/ex_data.c \
#    boringssl/crypto/mem.c \
#    boringssl/crypto/refcount_c11.c \
#    boringssl/crypto/refcount_lock.c \
#    boringssl/crypto/thread.c \
#    boringssl/crypto/thread_none.c \
#    boringssl/crypto/thread_pthread.c \
#    boringssl/crypto/thread_win.c \
#    boringssl/decrepit/bio/base64_bio.c \
#    boringssl/decrepit/blowfish/blowfish.c \
#    boringssl/decrepit/cast/cast.c \
#    boringssl/decrepit/cast/cast_tables.c \
#    boringssl/decrepit/cfb/cfb.c \
#    boringssl/decrepit/des/cfb64ede.c \
#    boringssl/decrepit/dh/dh_decrepit.c \
#    boringssl/decrepit/dsa/dsa_decrepit.c \
#    boringssl/decrepit/evp/dss1.c \
#    boringssl/decrepit/evp/evp_do_all.c \
#    boringssl/decrepit/obj/obj_decrepit.c \
#    boringssl/decrepit/rc4/rc4_decrepit.c \
#    boringssl/decrepit/ripemd/ripemd.c \
#    boringssl/decrepit/rsa/rsa_decrepit.c \
#    boringssl/decrepit/ssl/ssl_decrepit.c \
#    boringssl/decrepit/x509/x509_decrepit.c \
#    boringssl/decrepit/xts/xts.c \
#    boringssl/fipstools/test_fips.c \
#    boringssl/ssl/ssl_c_test.c \
#    boringssl/third_party/fiat/curve25519.c \
#    boringssl/third_party/fiat/p256.c \
#    boringssl/third_party/sike/asm/fp_generic.c \
#    boringssl/third_party/sike/fpx.c \
#    boringssl/third_party/sike/isogeny.c \
#    boringssl/third_party/sike/P503.c \
#    boringssl/third_party/sike/sike.c \
#    boringssl/crypto/err/err_data.c \
#    boringssl/crypto/err/err_data.c \
    transport_layer_security.cpp \
    transport_layer_security_openssl.cpp \
    transport_layer_security_implementation.cpp \
    abstract_syntax_notation_one_decoder.cpp
