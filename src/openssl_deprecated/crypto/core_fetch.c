/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stddef.h>

#include "../include/openssl/core.h"
#include "../include/internal/cryptlib.h"
#include "../include/internal/core.h"
#include "../include/internal/property.h"
#include "../include/internal/provider.h"
#include <sstream>
#include "../../vpfHeader1General0_General.h"

struct construct_data_st {
    openssl_ctx_st *libctx;
    ossl_method_store_st *store;
    int operation_id;
    int force_store;
    ossl_method_construct_method_st *mcm;
    void *mcm_data;
};

static int ossl_method_construct_this(ossl_provider_st* provider, void *cbdata, std::stringstream* commentsOnError)
{
    struct construct_data_st *data = (construct_data_st *) cbdata;
    int no_store = 0;    /* Assume caching is ok */
    const OSSL_ALGORITHM *map =
        ossl_provider_query_operation(provider, data->operation_id, &no_store);

    while (map->algorithm_name != NULL) {
        const OSSL_ALGORITHM *thismap = map++;
        void *method = NULL;

        if ((method = data->mcm->construct(thismap->algorithm_name,
                                           thismap->implementation, provider,
                                           data->mcm_data)) == NULL)
            continue;

        /*
         * Note regarding putting the method in stores:
         *
         * we don't need to care if it actually got in or not here.
         * If it didn't get in, it will simply not be available when
         * ossl_method_construct() tries to get it from the store.
         *
         * It is *expected* that the put function increments the refcnt
         * of the passed method.
         */

        if (data->force_store || !no_store) {
            /*
             * If we haven't been told not to store,
             * add to the global store
             */
            data->mcm->put(data->libctx, NULL,
                           thismap->property_definition,
                           method, data->mcm_data, commentsOnError);
        }

        data->mcm->put(data->libctx, data->store,
                       thismap->property_definition,
                       method, data->mcm_data, commentsOnError);

        /* refcnt-- because we're dropping the reference */
        data->mcm->destruct(method, data->mcm_data);
    }

    return 1;
}

void *ossl_method_construct(openssl_ctx_st *libctx,
  int operation_id,
  const char *name,
  const char *propquery,
  int force_store,
  ossl_method_construct_method_st *mcm,
  void *mcm_data,
  std::stringstream* commentsOnError
) {
  MacroRegisterFunctionWithName("ossl_method_construct");
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: got to method construct.\n";
  }

  void *method = mcm->get(libctx, NULL, propquery, mcm_data, commentsOnError);
  if (method != NULL) {
    return method;
  }
  struct construct_data_st cbdata;
  /*
   * We have a temporary store to be able to easily search among new
   * items, or items that should find themselves in the global store.
   */
  std::cout << "DEBUG: got to before alloc_tmp_store\n";
  cbdata.store = (ossl_method_store_st*) mcm->alloc_tmp_store(commentsOnError);
  std::cout << "DEBUG: got to AFTER alloc_tmp_store\n";
  if (cbdata.store == NULL) {
    return method;
  }

  cbdata.libctx = libctx;
  cbdata.operation_id = operation_id;
  cbdata.force_store = force_store;
  cbdata.mcm = mcm;
  cbdata.mcm_data = mcm_data;
  std::cout << "DEBUG: got to before forallloaded\n";
  ossl_provider_forall_loaded(libctx, ossl_method_construct_this, &cbdata, commentsOnError);
  std::cout << "DEBUG: got to after forallloaded\n";
  method = mcm->get(libctx, cbdata.store, propquery, mcm_data, commentsOnError);
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: Executed final mcm get.\n";
    if (method == 0) {
      *commentsOnError << "DEBUG: method is ZERO!!!\n";
    }
  }
  mcm->dealloc_tmp_store(cbdata.store);
  return method;
}