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

#ifndef OPENSSL_NO_MD2

# include "../../include/openssl/evp.h"
# include "../../include/openssl/objects.h"
# include "../../include/openssl/x509.h"
# include "../../include/openssl/md2.h"
# include "../../include/openssl/rsa.h"

#include "../include/internal/evp_int.h"

static int init(EVP_MD_CTX *ctx)
{
    return MD2_Init((MD2_CTX*) EVP_MD_CTX_md_data(ctx));
}

static int update(EVP_MD_CTX *ctx, const void *data, size_t count)
{
    return MD2_Update((MD2_CTX*) EVP_MD_CTX_md_data(ctx), (unsigned char*) data, count);
}

static int final(EVP_MD_CTX *ctx, unsigned char *md)
{
    return MD2_Final(md, (MD2_CTX*) EVP_MD_CTX_md_data(ctx));
}

static const EVP_MD md2_md = {
    NID_md2,
    NID_md2WithRSAEncryption,
    MD2_DIGEST_LENGTH,
    0,
    init,
    update,
    final,
    NULL,
    NULL,
    MD2_BLOCK,
    sizeof(EVP_MD *) + sizeof(MD2_CTX),
};

const EVP_MD *EVP_md2(void)
{
    return &md2_md;
}
#endif