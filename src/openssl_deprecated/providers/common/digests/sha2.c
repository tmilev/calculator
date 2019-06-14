/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "../../../include/openssl/sha.h"
#include "../../../include/openssl/crypto.h"
#include "../../../include/openssl/core_numbers.h"
#include "../include/internal/provider_algs.h"

/*
 * Forward declaration of everything implemented here.  This is not strictly
 * necessary for the compiler, but provides an assurance that the signatures
 * of the functions in the dispatch table are correct.
 */
#if 0                           /* Not defined here */
static OSSL_OP_digest_init_fn sha256_init;
static OSSL_OP_digest_update_fn sha256_update;
#endif
static OSSL_OP_digest_final_fn sha256_final;
static OSSL_OP_digest_freectx_fn sha256_freectx;
static OSSL_OP_digest_dupctx_fn sha256_dupctx;
static OSSL_OP_digest_size_fn sha256_size;
static OSSL_OP_digest_block_size_fn sha256_size;

static int sha256_final(void *ctx, unsigned char *md, size_t *mdl, size_t mdsz)
{
    if (mdsz >= SHA256_DIGEST_LENGTH && SHA256_Final(md, (SHA256_CTX*) ctx)) {
        *mdl = SHA256_DIGEST_LENGTH;
        return 1;
    }

    return 0;
}

void *sha256_newctx(void *provctx)
{
    SHA256_CTX *ctx = (SHA256_CTX *) OPENSSL_zalloc(sizeof(*ctx));

    return ctx;
}

static void sha256_freectx(void *vctx)
{
    SHA256_CTX *ctx = (SHA256_CTX *)vctx;

    OPENSSL_clear_free(ctx,  sizeof(*ctx));
}

static void *sha256_dupctx(void *ctx)
{
    SHA256_CTX *in = (SHA256_CTX *)ctx;
    SHA256_CTX *ret = (SHA256_CTX *) OPENSSL_malloc(sizeof(*ret));

    *ret = *in;

    return ret;
}

static size_t sha256_size(void)
{
    return SHA256_DIGEST_LENGTH;
}

static size_t sha256_block_size(void)
{
    return SHA256_CBLOCK;
}

const OSSL_DISPATCH sha256_functions[] = {
    { OSSL_FUNC_DIGEST_NEWCTX, (void (*)(void)) sha256_newctx },
    { OSSL_FUNC_DIGEST_INIT, (void (*)(void))SHA256_Init },
    { OSSL_FUNC_DIGEST_UPDATE, (void (*)(void))SHA256_Update },
    { OSSL_FUNC_DIGEST_FINAL, (void (*)(void))sha256_final },
    { OSSL_FUNC_DIGEST_FREECTX, (void (*)(void))sha256_freectx },
    { OSSL_FUNC_DIGEST_DUPCTX, (void (*)(void))sha256_dupctx },
    { OSSL_FUNC_DIGEST_SIZE, (void (*)(void))sha256_size },
    { OSSL_FUNC_DIGEST_BLOCK_SIZE, (void (*)(void))sha256_block_size },
    { 0, NULL }
};
