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

#ifndef OPENSSL_NO_RMD160

# include "../../include/openssl/ripemd.h"
# include "../../include/openssl/evp.h"
# include "../../include/openssl/objects.h"
# include "../../include/openssl/x509.h"
# include "../../include/openssl/rsa.h"
# include "../include/internal/evp_int.h"

static int init(EVP_MD_CTX *ctx)
{
    return RIPEMD160_Init((RIPEMD160_CTX*) EVP_MD_CTX_md_data(ctx));
}

static int update(EVP_MD_CTX *ctx, const void *data, size_t count)
{
    return RIPEMD160_Update((RIPEMD160_CTX*) EVP_MD_CTX_md_data(ctx), data, count);
}

static int final(EVP_MD_CTX *ctx, unsigned char *md)
{
    return RIPEMD160_Final(md, (RIPEMD160_CTX*) EVP_MD_CTX_md_data(ctx));
}

static const EVP_MD ripemd160_md = {
    NID_ripemd160,
    NID_ripemd160WithRSA,
    RIPEMD160_DIGEST_LENGTH,
    0,
    init,
    update,
    final,
    NULL,
    NULL,
    RIPEMD160_CBLOCK,
    sizeof(EVP_MD *) + sizeof(RIPEMD160_CTX),
};

const EVP_MD *EVP_ripemd160(void)
{
    return &ripemd160_md;
}
#endif
