/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OSSL_PROVIDER_H
# define OSSL_PROVIDER_H

# include "../../include/openssl/core.h"


/* Load and unload a provider */
ossl_provider_st *OSSL_PROVIDER_load(openssl_ctx_st *, const char *name, std::stringstream *commentsOnError);
int OSSL_PROVIDER_unload(ossl_provider_st *prov);

const OSSL_ITEM *OSSL_PROVIDER_get_param_types(ossl_provider_st *prov);
int OSSL_PROVIDER_get_params(ossl_provider_st *prov, const OSSL_PARAM params[]);

/* Add a built in providers */
int OSSL_PROVIDER_add_builtin(openssl_ctx_st*, const char *name,
                              OSSL_provider_init_fn *init_fn);


#endif
