/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright (c) 2019, Oracle and/or its affiliates.  All rights reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_PROPERTY_H
# define HEADER_PROPERTY_H

typedef struct ossl_method_store_st OSSL_METHOD_STORE;

/* Implementation store functions */
OSSL_METHOD_STORE *ossl_method_store_new(void);
void ossl_method_store_free(OSSL_METHOD_STORE *store);
int ossl_method_store_add(OSSL_METHOD_STORE *store, int nid,
                          const char *properties, void *implementation,
                          void (*implementation_destruct)(void *));
int ossl_method_store_remove(OSSL_METHOD_STORE *store,
                             int nid, const void *implementation);
int ossl_method_store_fetch(OSSL_METHOD_STORE *store, int nid,
                            const char *prop_query, void **result);
int ossl_method_store_set_global_properties(OSSL_METHOD_STORE *store,
                                            const char *prop_query);

/* proeprty query cache functions */
int ossl_method_store_cache_get(OSSL_METHOD_STORE *store, int nid,
                                const char *prop_query, void **result);
int ossl_method_store_cache_set(OSSL_METHOD_STORE *store, int nid,
                                const char *prop_query, void *result);

#endif
