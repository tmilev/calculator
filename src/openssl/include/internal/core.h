/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OSSL_INTERNAL_CORE_H
# define OSSL_INTERNAL_CORE_H

/*
 * namespaces:
 *
 * ossl_method_         Core Method API
 */

/*
 * construct an arbitrary method from a dispatch table found by looking
 * up a match for the < operation_id, name, property > combination.
 * constructor and destructor are the constructor and destructor for that
 * arbitrary object.
 *
 * These objects are normally cached, unless the provider says not to cache.
 * However, force_cache can be used to force caching whatever the provider
 * says (for example, because the application knows better).
 */
typedef struct ossl_method_construct_method_st {
    /* Create store */
    void *(*alloc_tmp_store)(void);
    /* Remove a store */
    void (*dealloc_tmp_store)(void *store);
    /* Get an already existing method from a store */
    void *(*get)(OPENSSL_CTX *libctx, void *store, const char *propquery,
                 void *data);
    /* Store a method in a store */
    int (*put)(OPENSSL_CTX *libctx, void *store, const char *propdef,
               void *method, void *data);
    /* Construct a new method */
    void *(*construct)(const char *algorithm_name, const OSSL_DISPATCH *fns,
                       OSSL_PROVIDER *prov, void *data);
    /* Destruct a method */
    void (*destruct)(void *method, void *data);
} OSSL_METHOD_CONSTRUCT_METHOD;

void *ossl_method_construct(OPENSSL_CTX *ctx, int operation_id,
                            const char *name, const char *properties,
                            int force_cache,
                            OSSL_METHOD_CONSTRUCT_METHOD *mcm, void *mcm_data);

#endif
