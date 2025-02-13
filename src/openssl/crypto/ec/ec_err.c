/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2023 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "../include/openssl/err.h"
#include <openssl/ecerr.h>
#include "crypto/ecerr.h"

#ifndef OPENSSL_NO_EC

# ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA EC_str_reasons[] = {
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_ASN1_ERROR), "asn1 error"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_BAD_SIGNATURE), "bad signature"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_BIGNUM_OUT_OF_RANGE), "bignum out of range"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_BUFFER_TOO_SMALL), "buffer too small"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_CANNOT_INVERT), "cannot invert"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_COORDINATES_OUT_OF_RANGE),
    "coordinates out of range"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_CURVE_DOES_NOT_SUPPORT_ECDH),
    "curve does not support ecdh"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_CURVE_DOES_NOT_SUPPORT_ECDSA),
    "curve does not support ecdsa"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_CURVE_DOES_NOT_SUPPORT_SIGNING),
    "curve does not support signing"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_DECODE_ERROR), "decode error"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_DISCRIMINANT_IS_ZERO),
    "discriminant is zero"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_EC_GROUP_NEW_BY_NAME_FAILURE),
    "ec group new by name failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_EXPLICIT_PARAMS_NOT_SUPPORTED),
    "explicit params not supported"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_FAILED_MAKING_PUBLIC_KEY),
    "failed making public key"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_FIELD_TOO_LARGE), "field too large"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_GF2M_NOT_SUPPORTED), "gf2m not supported"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_GROUP2PKPARAMETERS_FAILURE),
    "group2pkparameters failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_I2D_ECPKPARAMETERS_FAILURE),
    "i2d ecpkparameters failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INCOMPATIBLE_OBJECTS),
    "incompatible objects"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_A), "invalid a"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_ARGUMENT), "invalid argument"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_B), "invalid b"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_COFACTOR), "invalid cofactor"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_COMPRESSED_POINT),
    "invalid compressed point"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_COMPRESSION_BIT),
    "invalid compression bit"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_CURVE), "invalid curve"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_DIGEST), "invalid digest"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_DIGEST_TYPE), "invalid digest type"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_ENCODING), "invalid encoding"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_FIELD), "invalid field"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_FORM), "invalid form"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_GENERATOR), "invalid generator"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_GROUP_ORDER), "invalid group order"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_KEY), "invalid key"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_LENGTH), "invalid length"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_NAMED_GROUP_CONVERSION),
    "invalid named group conversion"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_OUTPUT_LENGTH),
    "invalid output length"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_P), "invalid p"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_PEER_KEY), "invalid peer key"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_PENTANOMIAL_BASIS),
    "invalid pentanomial basis"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_PRIVATE_KEY), "invalid private key"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_SEED), "invalid seed"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_INVALID_TRINOMIAL_BASIS),
    "invalid trinomial basis"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_KDF_PARAMETER_ERROR), "kdf parameter error"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_KEYS_NOT_SET), "keys not set"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_LADDER_POST_FAILURE), "ladder post failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_LADDER_PRE_FAILURE), "ladder pre failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_LADDER_STEP_FAILURE), "ladder step failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_MISSING_OID), "missing OID"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_MISSING_PARAMETERS), "missing parameters"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_MISSING_PRIVATE_KEY), "missing private key"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_NEED_NEW_SETUP_VALUES),
    "need new setup values"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_NOT_A_NIST_PRIME), "not a NIST prime"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_NOT_IMPLEMENTED), "not implemented"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_NOT_INITIALIZED), "not initialized"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_NO_PARAMETERS_SET), "no parameters set"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_NO_PRIVATE_VALUE), "no private value"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_OPERATION_NOT_SUPPORTED),
    "operation not supported"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_PASSED_NULL_PARAMETER),
    "passed null parameter"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_PEER_KEY_ERROR), "peer key error"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_POINT_ARITHMETIC_FAILURE),
    "point arithmetic failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_POINT_AT_INFINITY), "point at infinity"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_POINT_COORDINATES_BLIND_FAILURE),
    "point coordinates blind failure"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_POINT_IS_NOT_ON_CURVE),
    "point is not on curve"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_RANDOM_NUMBER_GENERATION_FAILED),
    "random number generation failed"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_SHARED_INFO_ERROR), "shared info error"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_SLOT_FULL), "slot full"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_TOO_MANY_RETRIES), "too many retries"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_UNDEFINED_GENERATOR), "undefined generator"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_UNDEFINED_ORDER), "undefined order"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_UNKNOWN_COFACTOR), "unknown cofactor"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_UNKNOWN_GROUP), "unknown group"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_UNKNOWN_ORDER), "unknown order"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_UNSUPPORTED_FIELD), "unsupported field"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_WRONG_CURVE_PARAMETERS),
    "wrong curve parameters"},
    {ERR_PACK(ERR_LIB_EC, 0, EC_R_WRONG_ORDER), "wrong order"},
    {0, NULL}
};

# endif

int ossl_err_load_EC_strings(void)
{
# ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(EC_str_reasons[0].error) == NULL)
        ERR_load_strings_const(EC_str_reasons);
# endif
    return 1;
}
#else
NON_EMPTY_TRANSLATION_UNIT
#endif
