/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "../../include/internal/cryptlib.h"
#include "../../include/openssl/bn.h"
#include "../../include/openssl/evp.h"
#include "../../include/openssl/objects.h"
#include "../../include/openssl/rsa.h"
#include "../../include/openssl/dsa.h"
#include "../../include/openssl/ec.h"

int i2d_PublicKey(const EVP_PKEY *a, unsigned char **pp)
{
    switch (EVP_PKEY_id(a)) {
#ifndef OPENSSL_NO_RSA
    case EVP_PKEY_RSA:
        return i2d_RSAPublicKey(EVP_PKEY_get0_RSA(a), pp);
#endif
#ifndef OPENSSL_NO_DSA
    case EVP_PKEY_DSA:
        return i2d_DSAPublicKey(EVP_PKEY_get0_DSA(a), pp);
#endif
#ifndef OPENSSL_NO_EC
    case EVP_PKEY_EC:
        return i2o_ECPublicKey(EVP_PKEY_get0_EC_KEY(a), pp);
#endif
    default:
        ASN1err(ASN1_F_I2D_PUBLICKEY, ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE);
        return -1;
    }
}