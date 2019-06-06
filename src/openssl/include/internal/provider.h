/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OSSL_INTERNAL_PROVIDER_H
# define OSSL_INTERNAL_PROVIDER_H

# include "../../include/openssl/core.h"
# include "../../include/internal/dso.h"
# include "../../include/internal/symhacks.h"
#include <sstream>

/*
 * namespaces:
 *
 * ossl_provider_       Provider Object internal API
 * OSSL_PROVIDER        Provider Object
 */

/* Provider Object finder, constructor and destructor */
ossl_provider_st* ossl_provider_find(openssl_ctx_st *libctx, const char *name);
ossl_provider_st* ossl_provider_new(openssl_ctx_st *libctx, const char *name,
                                 OSSL_provider_init_fn *init_function);
int ossl_provider_upref(ossl_provider_st *prov);
void ossl_provider_free(ossl_provider_st *prov);

/* Setters */
int ossl_provider_set_fallback(ossl_provider_st *prov);
int ossl_provider_set_module_path(ossl_provider_st *prov, const char *module_path);
int ossl_provider_add_parameter(ossl_provider_st *prov, const char *name, const char *value);

/*
 * Activate the Provider
 * If the Provider is a module, the module will be loaded
 * Inactivation is done by freeing the Provider
 */
int ossl_provider_activate(ossl_provider_st *prov);

/* Iterate over all loaded providers */
int ossl_provider_forall_loaded(
  openssl_ctx_st *,
  int (*cb)(ossl_provider_st* provider, void *cbdata),
  void *cbdata,
  std::stringstream* commentsOnError
);

/* Getters for other library functions */
const char *ossl_provider_name(ossl_provider_st *prov);
const DSO *ossl_provider_dso(ossl_provider_st *prov);
const char *ossl_provider_module_name(ossl_provider_st *prov);
const char *ossl_provider_module_path(ossl_provider_st *prov);

/* Thin wrappers around calls to the provider */
void ossl_provider_teardown(const ossl_provider_st *prov);
const OSSL_ITEM *ossl_provider_get_param_types(const ossl_provider_st *prov);
int ossl_provider_get_params(const ossl_provider_st *prov,
                             const OSSL_PARAM params[]);
const OSSL_ALGORITHM *ossl_provider_query_operation(const ossl_provider_st *prov,
                                                    int operation_id,
                                                    int *no_cache);

/* Configuration */
void ossl_provider_add_conf_module(void);

#endif
