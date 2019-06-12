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
#include <sstream>

/* Implementation store functions */
struct ossl_method_store_st;
ossl_method_store_st *ossl_method_store_new(std::stringstream *commentsOnError);
void ossl_method_store_free(ossl_method_store_st *store);
int ossl_method_store_add(
  ossl_method_store_st *store,
  int nid,
  const char *properties,
  void *implementation,
  void (*implementation_destruct)(void *),
  std::stringstream *commentsOnError
);
int ossl_method_store_remove(
  ossl_method_store_st *store,
  int nid,
  const void *implementation
);
int ossl_method_store_fetch(
  ossl_method_store_st *store,
  int nid,
  const char *prop_query,
  void **result,
  std::stringstream *commentsOnError
);
int ossl_method_store_set_global_properties(ossl_method_store_st *store,
                                            const char *prop_query);

/* proeprty query cache functions */
int ossl_method_store_cache_get(ossl_method_store_st *store, int nid,
                                const char *prop_query, void **result);
int ossl_method_store_cache_set(ossl_method_store_st *store, int nid,
                                const char *prop_query, void *result);

#endif
