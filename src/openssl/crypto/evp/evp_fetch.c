/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stddef.h>
#include "../../include/openssl/ossl_typ.h"
#include "../../include/openssl/evp.h"
#include "../../include/openssl/core.h"
#include "../../include/internal/cryptlib.h"
#include "../../include/internal/thread_once.h"
#include "../include/internal/asn1_int.h"
#include "../../include/internal/property.h"
#include "../../include/internal/core.h"
#include "../include/internal/evp_int.h"    /* evp_locl.h needs it */
#include "evp_locl.h"
#include <sstream>

/* The OpenSSL library context index for the default method store */
static int default_method_store_index = -1;

static void default_method_store_free(void *vstore)
{
    ossl_method_store_free((OSSL_METHOD_STORE *) vstore);
}

static void *default_method_store_new(void)
{
    return ossl_method_store_new();
}


static const OPENSSL_CTX_METHOD default_method_store_method = {
    default_method_store_new,
    default_method_store_free,
};

static int default_method_store_init(void)
{
    default_method_store_index =
        openssl_ctx_new_index(&default_method_store_method);

    return default_method_store_index != -1;
}

static CRYPTO_ONCE default_method_store_init_flag = CRYPTO_ONCE_STATIC_INIT;
DEFINE_RUN_ONCE_STATIC(do_default_method_store_init)
{
    return OPENSSL_init_crypto(0, NULL)
        && default_method_store_init();
}

/* Data to be passed through ossl_method_construct() */
struct method_data_st {
    const char *name;
    int nid;
    ossl_method_construct_method_st *mcm;
    void *(*method_from_dispatch)(int nid, const OSSL_DISPATCH *,
                                  OSSL_PROVIDER *);
    int (*refcnt_up_method)(void *method);
    void (*destruct_method)(void *method);
    int (*nid_method)(void *method);
};

/*
 * Generic routines to fetch / create EVP methods with ossl_method_construct()
 */
static void *alloc_tmp_method_store(void)
{
    return ossl_method_store_new();
}

 static void dealloc_tmp_method_store(void *store)
{
    if (store != NULL)
        ossl_method_store_free((OSSL_METHOD_STORE *) store);
}

static OSSL_METHOD_STORE *get_default_method_store(openssl_ctx_st *libctx)
{
    if (!RUN_ONCE(&default_method_store_init_flag,
                  do_default_method_store_init))
        return NULL;
    return (OSSL_METHOD_STORE *) openssl_ctx_get_data(libctx, default_method_store_index);
}

static void *get_method_from_store(
  openssl_ctx_st *libctx, void *store, const char *propquery, void *data, std::stringstream* commentsOnError
) {
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: Here I am in get_method_from_store.\n";
  }
  struct method_data_st* methdata = (method_data_st*) data;
  void *method = NULL;
  if (store == NULL) {
    store = get_default_method_store(libctx);
    if (store == NULL) {
      if (commentsOnError != 0) {
        *commentsOnError << "Failed to get default method store.\n";
      }
      return 0;
    }
  }
  ossl_method_store_fetch((OSSL_METHOD_STORE *) store, methdata->nid, propquery, &method, commentsOnError);
  if (method != NULL) {
    if (!methdata->refcnt_up_method(method)) {
      method = NULL;
      if (commentsOnError != 0) {
        *commentsOnError << "Failed call to refcnt_up_method.\n";
      }
    }
  } else if (commentsOnError != 0){
    *commentsOnError << "Failed to fetch method.\n";
  }
  return method;
}

static int put_method_in_store(openssl_ctx_st *libctx, void *store,
                               const char *propdef,
                               void *method, void *data)
{
    struct method_data_st *methdata = (method_data_st *) data;
    int nid = methdata->nid_method(method);

    if (nid == NID_undef)
        return 0;

    if (store == NULL
        && (store = get_default_method_store(libctx)) == NULL)
        return 0;

    if (methdata->refcnt_up_method(method)
        && ossl_method_store_add((OSSL_METHOD_STORE*) store, nid, propdef, method,
                                 methdata->destruct_method))
        return 1;
    return 0;
}

static void *construct_method(const char *algorithm_name,
                              const OSSL_DISPATCH *fns, OSSL_PROVIDER *prov,
                              void *data)
{
    struct method_data_st *methdata = (method_data_st *) data;
    void *method = NULL;
    int nid = OBJ_sn2nid(algorithm_name);

    if (nid == NID_undef) {
        /* Create a new NID for that name on the fly */
        ASN1_OBJECT tmpobj;

        /* This is the same as OBJ_create() but without requiring a OID */
        tmpobj.nid = OBJ_new_nid(1);
        tmpobj.sn = tmpobj.ln = methdata->name;
        tmpobj.flags = ASN1_OBJECT_FLAG_DYNAMIC;
        tmpobj.length = 0;
        tmpobj.data = NULL;

        nid = OBJ_add_object(&tmpobj);
    }

    if (nid == NID_undef)
        return NULL;

    method = methdata->method_from_dispatch(nid, fns, prov);
    if (method == NULL)
        return NULL;
    return method;
}

static void destruct_method(void *method, void *data)
{
    struct method_data_st *methdata = (method_data_st *) data;

    methdata->destruct_method(method);
}

void *evp_generic_fetch(openssl_ctx_st *libctx,
  int operation_id,
  const char *algorithm,
  const char *properties,
  void *(*new_method)(int nid, const OSSL_DISPATCH *fns, OSSL_PROVIDER *prov),
  int (*upref_method)(void *),
  void (*free_method)(void *),
  int (*nid_method)(void *),
  std::stringstream* commentsOnError
) {
  int nid = OBJ_sn2nid(algorithm);
  void *method = NULL;
  if (nid == NID_undef || !ossl_method_store_cache_get(NULL, nid, properties, &method)) {
    ossl_method_construct_method_st mcm;
    // use of struct allocation makes tracing origins very difficult.
    mcm.alloc_tmp_store = alloc_tmp_method_store;
    mcm.dealloc_tmp_store = dealloc_tmp_method_store;
    mcm.construct = construct_method;
    mcm.destruct = destruct_method;
    mcm.get = get_method_from_store;
    mcm.put = put_method_in_store;
    struct method_data_st mcmdata;
    mcmdata.nid = nid;
    mcmdata.mcm = &mcm;
    mcmdata.method_from_dispatch = new_method;
    mcmdata.destruct_method = free_method;
    mcmdata.refcnt_up_method = upref_method;
    mcmdata.destruct_method = free_method;
    mcmdata.nid_method = nid_method;
    method = ossl_method_construct(
      libctx, operation_id, algorithm, properties, 0 /* !force_cache */, &mcm, &mcmdata, commentsOnError
    );
    ossl_method_store_cache_set(NULL, nid, properties, method);
    if (commentsOnError != 0) {
      *commentsOnError << "DEBUG: ossl method store cache set executed.\n";
    }
  }
  return method;
}

int EVP_set_default_properties(openssl_ctx_st *libctx, const char *propq)
{
    OSSL_METHOD_STORE *store = get_default_method_store(libctx);

    if (store != NULL)
        return ossl_method_store_set_global_properties(store, propq);
    EVPerr(EVP_F_EVP_SET_DEFAULT_PROPERTIES, ERR_R_INTERNAL_ERROR);
    return 0;
}
