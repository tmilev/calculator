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

FEATUREFLAGS= -std=c++0x -pthread
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
  #	LIBRARIES_INCLUDED_AT_THE_END+= -lssl -lcrypto  #WARNING believe it or not, the libraries must come AFTER the executable name
$(info [1;32mOpenssl found.[0m) 
else
$(info [1;31mNOT FOUND: Openssl.[0m I will attempt to install it once the calculator is compiled.) 
endif
endif

ifeq ($(noMongo), 1)
$(info [1;31mNo mongo requested.[0m) 
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
SOURCES_RELATIVE_PATH=\
transport_security_layer.cpp \
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
vpf9_85TimeDateWrappers.cpp \
string_constants.cpp \
source_code_formatter.cpp

SOURCES_RELATIVE_PATH_C=\
boringssl/crypto/asn1/a_bitstr.c \
boringssl/crypto/asn1/a_bool.c \
boringssl/crypto/asn1/a_d2i_fp.c \
boringssl/crypto/asn1/a_dup.c \
boringssl/crypto/asn1/a_enum.c \
boringssl/crypto/asn1/a_gentm.c \
boringssl/crypto/asn1/a_i2d_fp.c \
boringssl/crypto/asn1/a_int.c \
boringssl/crypto/asn1/a_mbstr.c \
boringssl/crypto/asn1/a_object.c \
boringssl/crypto/asn1/a_octet.c \
boringssl/crypto/asn1/a_print.c \
boringssl/crypto/asn1/a_strnid.c \
boringssl/crypto/asn1/a_time.c \
boringssl/crypto/asn1/a_type.c \
boringssl/crypto/asn1/a_utctm.c \
boringssl/crypto/asn1/a_utf8.c \
boringssl/crypto/asn1/asn1_lib.c \
boringssl/crypto/asn1/asn1_par.c \
boringssl/crypto/asn1/asn_pack.c \
boringssl/crypto/asn1/f_enum.c \
boringssl/crypto/asn1/f_int.c \
boringssl/crypto/asn1/f_string.c \
boringssl/crypto/asn1/tasn_dec.c \
boringssl/crypto/asn1/tasn_enc.c \
boringssl/crypto/asn1/tasn_fre.c \
boringssl/crypto/asn1/tasn_new.c \
boringssl/crypto/asn1/tasn_typ.c \
boringssl/crypto/asn1/tasn_utl.c \
boringssl/crypto/asn1/time_support.c \
boringssl/crypto/base64/base64.c \
boringssl/crypto/bio/bio.c \
boringssl/crypto/bio/bio_mem.c \
boringssl/crypto/bio/connect.c \
boringssl/crypto/bio/fd.c \
boringssl/crypto/bio/file.c \
boringssl/crypto/bio/hexdump.c \
boringssl/crypto/bio/pair.c \
boringssl/crypto/bio/printf.c \
boringssl/crypto/bio/socket.c \
boringssl/crypto/bio/socket_helper.c \
boringssl/crypto/bn_extra/bn_asn1.c \
boringssl/crypto/bn_extra/convert.c \
boringssl/crypto/buf/buf.c \
boringssl/crypto/bytestring/asn1_compat.c \
boringssl/crypto/bytestring/ber.c \
boringssl/crypto/bytestring/cbb.c \
boringssl/crypto/bytestring/cbs.c \
boringssl/crypto/bytestring/unicode.c \
boringssl/crypto/chacha/chacha.c \
boringssl/crypto/cmac/cmac.c \
boringssl/crypto/conf/conf.c \
boringssl/crypto/curve25519/spake25519.c \
boringssl/crypto/dh/check.c \
boringssl/crypto/dh/dh.c \
boringssl/crypto/dh/dh_asn1.c \
boringssl/crypto/dh/params.c \
boringssl/crypto/digest_extra/digest_extra.c \
boringssl/crypto/dsa/dsa.c \
boringssl/crypto/dsa/dsa_asn1.c \
boringssl/crypto/ec_extra/ec_asn1.c \
boringssl/crypto/ecdh_extra/ecdh_extra.c \
boringssl/crypto/ecdsa_extra/ecdsa_asn1.c \
boringssl/crypto/engine/engine.c \
boringssl/crypto/err/err.c \
boringssl/crypto/evp/digestsign.c \
boringssl/crypto/evp/evp.c \
boringssl/crypto/evp/evp_asn1.c \
boringssl/crypto/evp/evp_ctx.c \
boringssl/crypto/evp/p_dsa_asn1.c \
boringssl/crypto/evp/p_ec.c \
boringssl/crypto/evp/p_ec_asn1.c \
boringssl/crypto/evp/p_ed25519.c \
boringssl/crypto/evp/p_ed25519_asn1.c \
boringssl/crypto/evp/p_rsa.c \
boringssl/crypto/evp/p_rsa_asn1.c \
boringssl/crypto/evp/pbkdf.c \
boringssl/crypto/evp/print.c \
boringssl/crypto/evp/scrypt.c \
boringssl/crypto/evp/sign.c \
boringssl/crypto/hkdf/hkdf.c \
boringssl/crypto/hrss/hrss.c \
boringssl/crypto/lhash/lhash.c \
boringssl/crypto/obj/obj.c \
boringssl/crypto/obj/obj_xref.c \
boringssl/crypto/pem/pem_all.c \
boringssl/crypto/pem/pem_info.c \
boringssl/crypto/pem/pem_lib.c \
boringssl/crypto/pem/pem_oth.c \
boringssl/crypto/pem/pem_pk8.c \
boringssl/crypto/pem/pem_pkey.c \
boringssl/crypto/pem/pem_x509.c \
boringssl/crypto/pem/pem_xaux.c \
boringssl/crypto/pkcs7/pkcs7.c \
boringssl/crypto/pkcs7/pkcs7_x509.c \
boringssl/crypto/pkcs8/p5_pbev2.c \
boringssl/crypto/pkcs8/pkcs8.c \
boringssl/crypto/pkcs8/pkcs8_x509.c \
boringssl/crypto/poly1305/poly1305.c \
boringssl/crypto/poly1305/poly1305_arm.c \
boringssl/crypto/poly1305/poly1305_vec.c \
boringssl/crypto/pool/pool.c \
boringssl/crypto/rand_extra/deterministic.c \
boringssl/crypto/rand_extra/forkunsafe.c \
boringssl/crypto/rand_extra/fuchsia.c \
boringssl/crypto/rand_extra/rand_extra.c \
boringssl/crypto/rand_extra/windows.c \
boringssl/crypto/rc4/rc4.c \
boringssl/crypto/rsa_extra/rsa_asn1.c \
boringssl/crypto/rsa_extra/rsa_print.c \
boringssl/crypto/stack/stack.c \
boringssl/crypto/x509/a_digest.c \
boringssl/crypto/x509/a_sign.c \
boringssl/crypto/x509/a_strex.c \
boringssl/crypto/x509/a_verify.c \
boringssl/crypto/x509/algorithm.c \
boringssl/crypto/x509/asn1_gen.c \
boringssl/crypto/x509/by_dir.c \
boringssl/crypto/x509/by_file.c \
boringssl/crypto/x509/i2d_pr.c \
boringssl/crypto/x509/rsa_pss.c \
boringssl/crypto/x509/t_crl.c \
boringssl/crypto/x509/t_req.c \
boringssl/crypto/x509/t_x509.c \
boringssl/crypto/x509/t_x509a.c \
boringssl/crypto/x509/x509.c \
boringssl/crypto/x509/x509_att.c \
boringssl/crypto/x509/x509_cmp.c \
boringssl/crypto/x509/x509_d2.c \
boringssl/crypto/x509/x509_def.c \
boringssl/crypto/x509/x509_ext.c \
boringssl/crypto/x509/x509_lu.c \
boringssl/crypto/x509/x509_obj.c \
boringssl/crypto/x509/x509_r2x.c \
boringssl/crypto/x509/x509_req.c \
boringssl/crypto/x509/x509_set.c \
boringssl/crypto/x509/x509_trs.c \
boringssl/crypto/x509/x509_txt.c \
boringssl/crypto/x509/x509_v3.c \
boringssl/crypto/x509/x509_vfy.c \
boringssl/crypto/x509/x509_vpm.c \
boringssl/crypto/x509/x509cset.c \
boringssl/crypto/x509/x509name.c \
boringssl/crypto/x509/x509rset.c \
boringssl/crypto/x509/x509spki.c \
boringssl/crypto/x509/x_algor.c \
boringssl/crypto/x509/x_all.c \
boringssl/crypto/x509/x_attrib.c \
boringssl/crypto/x509/x_crl.c \
boringssl/crypto/x509/x_exten.c \
boringssl/crypto/x509/x_info.c \
boringssl/crypto/x509/x_name.c \
boringssl/crypto/x509/x_pkey.c \
boringssl/crypto/x509/x_pubkey.c \
boringssl/crypto/x509/x_req.c \
boringssl/crypto/x509/x_sig.c \
boringssl/crypto/x509/x_spki.c \
boringssl/crypto/x509/x_val.c \
boringssl/crypto/x509/x_x509.c \
boringssl/crypto/x509/x_x509a.c \
boringssl/crypto/x509v3/pcy_cache.c \
boringssl/crypto/x509v3/pcy_data.c \
boringssl/crypto/x509v3/pcy_lib.c \
boringssl/crypto/x509v3/pcy_map.c \
boringssl/crypto/x509v3/pcy_node.c \
boringssl/crypto/x509v3/pcy_tree.c \
boringssl/crypto/x509v3/v3_akey.c \
boringssl/crypto/x509v3/v3_akeya.c \
boringssl/crypto/x509v3/v3_alt.c \
boringssl/crypto/x509v3/v3_bcons.c \
boringssl/crypto/x509v3/v3_bitst.c \
boringssl/crypto/x509v3/v3_conf.c \
boringssl/crypto/x509v3/v3_cpols.c \
boringssl/crypto/x509v3/v3_crld.c \
boringssl/crypto/x509v3/v3_enum.c \
boringssl/crypto/x509v3/v3_extku.c \
boringssl/crypto/x509v3/v3_genn.c \
boringssl/crypto/x509v3/v3_ia5.c \
boringssl/crypto/x509v3/v3_info.c \
boringssl/crypto/x509v3/v3_int.c \
boringssl/crypto/x509v3/v3_lib.c \
boringssl/crypto/x509v3/v3_ncons.c \
boringssl/crypto/x509v3/v3_ocsp.c \
boringssl/crypto/x509v3/v3_pci.c \
boringssl/crypto/x509v3/v3_pcia.c \
boringssl/crypto/x509v3/v3_pcons.c \
boringssl/crypto/x509v3/v3_pku.c \
boringssl/crypto/x509v3/v3_pmaps.c \
boringssl/crypto/x509v3/v3_prn.c \
boringssl/crypto/x509v3/v3_purp.c \
boringssl/crypto/x509v3/v3_skey.c \
boringssl/crypto/x509v3/v3_sxnet.c \
boringssl/crypto/x509v3/v3_utl.c \
boringssl/crypto/cpu-aarch64-fuchsia.c \
boringssl/crypto/cpu-aarch64-linux.c \
boringssl/crypto/cpu-arm-linux.c \
boringssl/crypto/cpu-arm.c \
boringssl/crypto/cpu-intel.c \
boringssl/crypto/cpu-ppc64le.c \
boringssl/crypto/crypto.c \
boringssl/crypto/ex_data.c \
boringssl/crypto/mem.c \
boringssl/crypto/refcount_c11.c \
boringssl/crypto/refcount_lock.c \
boringssl/crypto/thread.c \
boringssl/crypto/thread_none.c \
boringssl/crypto/thread_pthread.c \
boringssl/crypto/thread_win.c \
boringssl/crypto/fipsmodule/aes/aes.c \
boringssl/crypto/fipsmodule/aes/key_wrap.c \
boringssl/crypto/fipsmodule/aes/mode_wrappers.c \
boringssl/crypto/fipsmodule/bn/asm/x86_64-gcc.c \
boringssl/crypto/fipsmodule/bn/add.c \
boringssl/crypto/fipsmodule/bn/bn.c \
boringssl/crypto/fipsmodule/bn/bytes.c \
boringssl/crypto/fipsmodule/bn/cmp.c \
boringssl/crypto/fipsmodule/bn/ctx.c \
boringssl/crypto/fipsmodule/bn/div.c \
boringssl/crypto/fipsmodule/bn/div_extra.c \
boringssl/crypto/fipsmodule/bn/exponentiation.c \
boringssl/crypto/fipsmodule/bn/gcd.c \
boringssl/crypto/fipsmodule/bn/gcd_extra.c \
boringssl/crypto/fipsmodule/bn/generic.c \
boringssl/crypto/fipsmodule/bn/jacobi.c \
boringssl/crypto/fipsmodule/bn/montgomery.c \
boringssl/crypto/fipsmodule/bn/montgomery_inv.c \
boringssl/crypto/fipsmodule/bn/mul.c \
boringssl/crypto/fipsmodule/bn/prime.c \
boringssl/crypto/fipsmodule/bn/random.c \
boringssl/crypto/fipsmodule/bn/rsaz_exp.c \
boringssl/crypto/fipsmodule/bn/shift.c \
boringssl/crypto/fipsmodule/bn/sqrt.c \
boringssl/crypto/fipsmodule/cipher/aead.c \
boringssl/crypto/fipsmodule/cipher/cipher.c \
boringssl/crypto/fipsmodule/cipher/e_aes.c \
boringssl/crypto/fipsmodule/cipher/e_des.c \
boringssl/crypto/fipsmodule/des/des.c \
boringssl/crypto/fipsmodule/digest/digest.c \
boringssl/crypto/fipsmodule/digest/digests.c \
boringssl/crypto/fipsmodule/ec/ec.c \
boringssl/crypto/fipsmodule/ec/ec_key.c \
boringssl/crypto/fipsmodule/ec/ec_montgomery.c \
boringssl/crypto/fipsmodule/ec/felem.c \
boringssl/crypto/fipsmodule/ec/oct.c \
boringssl/crypto/fipsmodule/ec/p224-64.c \
boringssl/crypto/fipsmodule/ec/p256-x86_64.c \
boringssl/crypto/fipsmodule/ec/scalar.c \
boringssl/crypto/fipsmodule/ec/simple.c \
boringssl/crypto/fipsmodule/ec/simple_mul.c \
boringssl/crypto/fipsmodule/ec/util.c \
boringssl/crypto/fipsmodule/ec/wnaf.c \
boringssl/crypto/fipsmodule/ecdh/ecdh.c \
boringssl/crypto/fipsmodule/ecdsa/ecdsa.c \
boringssl/crypto/fipsmodule/hmac/hmac.c \
boringssl/crypto/fipsmodule/md4/md4.c \
boringssl/crypto/fipsmodule/md5/md5.c \
boringssl/crypto/fipsmodule/modes/cbc.c \
boringssl/crypto/fipsmodule/modes/ccm.c \
boringssl/crypto/fipsmodule/modes/cfb.c \
boringssl/crypto/fipsmodule/modes/ctr.c \
boringssl/crypto/fipsmodule/modes/gcm.c \
boringssl/crypto/fipsmodule/modes/ofb.c \
boringssl/crypto/fipsmodule/modes/polyval.c \
boringssl/crypto/fipsmodule/rand/ctrdrbg.c \
boringssl/crypto/fipsmodule/rand/rand.c \
boringssl/crypto/fipsmodule/rand/urandom.c \
boringssl/crypto/fipsmodule/rsa/blinding.c \
boringssl/crypto/fipsmodule/rsa/padding.c \
boringssl/crypto/fipsmodule/rsa/rsa.c \
boringssl/crypto/fipsmodule/rsa/rsa_impl.c \
boringssl/crypto/fipsmodule/self_check/self_check.c \
boringssl/crypto/fipsmodule/sha/sha1-altivec.c \
boringssl/crypto/fipsmodule/sha/sha1.c \
boringssl/crypto/fipsmodule/sha/sha256.c \
boringssl/crypto/fipsmodule/sha/sha512.c \
boringssl/crypto/fipsmodule/tls/kdf.c \
boringssl/crypto/fipsmodule/fips_shared_support.c \
boringssl/crypto/fipsmodule/is_fips.c \
boringssl/decrepit/bio/base64_bio.c \
boringssl/decrepit/blowfish/blowfish.c \
boringssl/decrepit/cast/cast.c \
boringssl/decrepit/cast/cast_tables.c \
boringssl/decrepit/cfb/cfb.c \
boringssl/decrepit/des/cfb64ede.c \
boringssl/decrepit/dh/dh_decrepit.c \
boringssl/decrepit/dsa/dsa_decrepit.c \
boringssl/decrepit/evp/dss1.c \
boringssl/decrepit/evp/evp_do_all.c \
boringssl/decrepit/obj/obj_decrepit.c \
boringssl/decrepit/rc4/rc4_decrepit.c \
boringssl/decrepit/ripemd/ripemd.c \
boringssl/decrepit/rsa/rsa_decrepit.c \
boringssl/decrepit/ssl/ssl_decrepit.c \
boringssl/decrepit/x509/x509_decrepit.c \
boringssl/decrepit/xts/xts.c \
boringssl/third_party/fiat/curve25519.c \
boringssl/third_party/fiat/p256.c \
boringssl/third_party/sike/asm/fp_generic.c \
boringssl/third_party/sike/fpx.c \
boringssl/third_party/sike/isogeny.c \
boringssl/third_party/sike/P503.c \
boringssl/third_party/sike/sike.c \

SOURCES_RELATIVE_PATH_CC=\
boringssl/ssl/bio_ssl.cc \
boringssl/ssl/d1_both.cc \
boringssl/ssl/d1_lib.cc \
boringssl/ssl/d1_pkt.cc \
boringssl/ssl/d1_srtp.cc \
boringssl/ssl/dtls_method.cc \
boringssl/ssl/dtls_record.cc \
boringssl/ssl/handoff.cc \
boringssl/ssl/handshake.cc \
boringssl/ssl/handshake_client.cc \
boringssl/ssl/handshake_server.cc \
boringssl/ssl/s3_both.cc \
boringssl/ssl/s3_lib.cc \
boringssl/ssl/s3_pkt.cc \
boringssl/ssl/ssl_aead_ctx.cc \
boringssl/ssl/ssl_asn1.cc \
boringssl/ssl/ssl_buffer.cc \
boringssl/ssl/ssl_cert.cc \
boringssl/ssl/ssl_cipher.cc \
boringssl/ssl/ssl_file.cc \
boringssl/ssl/ssl_key_share.cc \
boringssl/ssl/ssl_lib.cc \
boringssl/ssl/ssl_privkey.cc \
boringssl/ssl/ssl_session.cc \
boringssl/ssl/ssl_stat.cc \
boringssl/ssl/ssl_transcript.cc \
boringssl/ssl/ssl_versions.cc \
boringssl/ssl/ssl_x509.cc \
boringssl/ssl/t1_enc.cc \
boringssl/ssl/t1_lib.cc \
boringssl/ssl/tls13_both.cc \
boringssl/ssl/tls13_client.cc \
boringssl/ssl/tls13_enc.cc \
boringssl/ssl/tls13_server.cc \
boringssl/ssl/tls_method.cc \
boringssl/ssl/tls_record.cc


OBJECTS=$(addprefix bin/, $(SOURCES_RELATIVE_PATH:.cpp=.o))
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
	$(CXX) $(LDFLAGS) $(OBJECTS) -o ./bin/calculator $(LIBRARIES_INCLUDED_AT_THE_END)

testrun: bin/calculator
	time ./bin/calculator test

bin/%.o:src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

bin/boringssl/%.o:src/boringssl/%.c
	mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

bin/boringssl/%.o:src/boringssl/%.cc
	mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -MP $< -o $@

clean:
	rm -f $(OBJECTS) $(DEPENDENCIES)

-include $(DEPENDENCIES)
