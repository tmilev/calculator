/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <string.h>
#include <stdio.h>
#include "../../include/openssl/core.h"
#include "../../include/openssl/core_numbers.h"
#include "../../include/openssl/core_names.h"
#include "../../include/openssl/params.h"
#include "../../include/openssl/err.h"
#include "../../include/openssl/evp.h"
/* TODO(3.0): Needed for dummy_evp_call(). To be removed */
#include "../../crypto/include/internal/sha.h"
#include "../../include/internal/cryptlib.h"
#include "../../include/internal/property.h"
#include "../../crypto/include/internal/evp_int.h"

/* Functions provided by the core */
static OSSL_core_get_param_types_fn *c_get_param_types = NULL;
static OSSL_core_get_params_fn *c_get_params = NULL;
static OSSL_core_put_error_fn *c_put_error = NULL;

/* Parameters we provide to the core */
static const OSSL_ITEM fips_param_types[] = {
    { OSSL_PARAM_UTF8_PTR, OSSL_PROV_PARAM_NAME },
    { OSSL_PARAM_UTF8_PTR, OSSL_PROV_PARAM_VERSION },
    { OSSL_PARAM_UTF8_PTR, OSSL_PROV_PARAM_BUILDINFO },
    { 0, NULL }
};

/* TODO(3.0): To be removed */
static int dummy_evp_call(openssl_ctx_st *libctx)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_MD *sha256 = EVP_MD_fetch(libctx, "SHA256", NULL, 0);
    char msg[] = "Hello World!";
    const unsigned char exptd[] = {
        0x7f, 0x83, 0xb1, 0x65, 0x7f, 0xf1, 0xfc, 0x53, 0xb9, 0x2d, 0xc1, 0x81,
        0x48, 0xa1, 0xd6, 0x5d, 0xfc, 0x2d, 0x4b, 0x1f, 0xa3, 0xd6, 0x77, 0x28,
        0x4a, 0xdd, 0xd2, 0x00, 0x12, 0x6d, 0x90, 0x69
    };
    unsigned int dgstlen = 0;
    unsigned char dgst[SHA256_DIGEST_LENGTH];
    int ret = 0;

    if (ctx == NULL || sha256 == NULL)
        goto err;

    if (!EVP_DigestInit_ex(ctx, sha256, NULL, 0))
        goto err;
    if (!EVP_DigestUpdate(ctx, msg, sizeof(msg) - 1))
        goto err;
    if (!EVP_DigestFinal(ctx, dgst, &dgstlen))
        goto err;
    if (dgstlen != sizeof(exptd) || memcmp(dgst, exptd, sizeof(exptd)) != 0)
        goto err;

    ret = 1;
 err:
    EVP_MD_CTX_free(ctx);
    EVP_MD_meth_free(sha256);
    return ret;
}

static const OSSL_ITEM *fips_get_param_types(const ossl_provider_st *prov)
{
    return fips_param_types;
}

static int fips_get_params(const ossl_provider_st *prov,
                            const OSSL_PARAM params[])
{
    const OSSL_PARAM *p;

    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_NAME);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, "OpenSSL FIPS Provider"))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_VERSION);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, OPENSSL_VERSION_STR))
        return 0;
    p = OSSL_PARAM_locate(params, OSSL_PROV_PARAM_BUILDINFO);
    if (p != NULL && !OSSL_PARAM_set_utf8_ptr(p, OPENSSL_FULL_VERSION_STR))
        return 0;

    return 1;
}

extern const OSSL_DISPATCH sha256_functions[];

static const OSSL_ALGORITHM fips_digests[] = {
    { "SHA256", "fips=yes", sha256_functions },
    { NULL, NULL, NULL }
};

static const OSSL_ALGORITHM *fips_query(ossl_provider_st *prov,
                                         int operation_id,
                                         int *no_cache)
{
    *no_cache = 0;
    switch (operation_id) {
    case OSSL_OP_DIGEST:
        return fips_digests;
    }
    return NULL;
}

/* Functions we provide to the core */
static const OSSL_DISPATCH fips_dispatch_table[] = {
    /*
     * To release our resources we just need to free the OPENSSL_CTX so we just
     * use OPENSSL_CTX_free directly as our teardown function
     */
    { OSSL_FUNC_PROVIDER_TEARDOWN, (void (*)(void))OPENSSL_CTX_free },
    { OSSL_FUNC_PROVIDER_GET_PARAM_TYPES, (void (*)(void))fips_get_param_types },
    { OSSL_FUNC_PROVIDER_GET_PARAMS, (void (*)(void))fips_get_params },
    { OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))fips_query },
    { 0, NULL }
};

/* Functions we provide to ourself */
static const OSSL_DISPATCH intern_dispatch_table[] = {
    { OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))fips_query },
    { 0, NULL }
};


int OSSL_provider_init(
  const ossl_provider_st *provider,
  const OSSL_DISPATCH *in,
  const OSSL_DISPATCH **out,
  void **provctx,
  std::stringstream* commentsOnError
) {
  std::cout << "DEBUG: HERE I AM!!!!!!!!!!!!!!!!!!!!!!!!GOT ";
    for (; in->function_id != 0; in++) {
        switch (in->function_id) {
        case OSSL_FUNC_CORE_GET_PARAM_TYPES:
            c_get_param_types = OSSL_get_core_get_param_types(in);
            break;
        case OSSL_FUNC_CORE_GET_PARAMS:
            c_get_params = OSSL_get_core_get_params(in);
            break;
        case OSSL_FUNC_CORE_PUT_ERROR:
            c_put_error = OSSL_get_core_put_error(in);
            break;
        /* Just ignore anything we don't understand */
        default:
            break;
        }
    }
    openssl_ctx_st *ctx = OPENSSL_CTX_new();
    if (ctx == NULL)
        return 0;

    /*
     * TODO(3.0): Remove me. This is just a dummy call to demonstrate making
     * EVP calls from within the FIPS module.
     */
    if (false) {
      if (!dummy_evp_call(ctx)) {
          OPENSSL_CTX_free(ctx);
          return 0;
      }
    }

    *out = fips_dispatch_table;
    *provctx = ctx;
    return 1;
}

/*
 * The internal init function used when the FIPS module uses EVP to call
 * another algorithm also in the FIPS module. This is a recursive call that has
 * been made from within the FIPS module itself. Normally we are responsible for
 * providing our own provctx value, but in this recursive case it has been
 * pre-populated for us with the same library context that was used in the EVP
 * call that initiated this recursive call - so we don't need to do anything
 * further with that parameter. This only works because we *know* in the core
 * code that the FIPS module uses a library context for its provctx. This is
 * not generally true for all providers.
 */
OSSL_provider_init_fn fips_intern_provider_init;
int fips_intern_provider_init(const ossl_provider_st *provider,
                              const OSSL_DISPATCH *in,
                              const OSSL_DISPATCH **out,
                              void **provctx)
{
    *out = intern_dispatch_table;
    return 1;
}
