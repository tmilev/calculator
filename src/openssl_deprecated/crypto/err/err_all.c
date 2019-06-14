/*
 * Copyright 1995-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "../include/internal/err_int.h"
#include "../../include/openssl/asn1err.h"
#include "../../include/openssl/bnerr.h"
#include "../../include/openssl/ecerr.h"
#include "../../include/openssl/buffererr.h"
#include "../../include/openssl/bioerr.h"
#include "../../include/openssl/comperr.h"
#include "../../include/openssl/rsaerr.h"
#include "../../include/openssl/dherr.h"
#include "../../include/openssl/dsaerr.h"
#include "../../include/openssl/evperr.h"
#include "../../include/openssl/objectserr.h"
#include "../../include/openssl/pemerr.h"
#include "../../include/openssl/pkcs7err.h"
#include "../../include/openssl/x509err.h"
#include "../../include/openssl/x509v3err.h"
#include "../../include/openssl/conferr.h"
#include "../../include/openssl/pkcs12err.h"
#include "../../include/openssl/randerr.h"
#include "../../include/internal/dso.h"
#include "../../include/openssl/engineerr.h"
#include "../../include/openssl/uierr.h"
#include "../../include/openssl/ocsperr.h"
#include "../../include/openssl/err.h"
#include "../../include/openssl/tserr.h"
#include "../../include/openssl/cmserr.h"
#include "../../include/openssl/crmferr.h"
#include "../../include/openssl/cterr.h"
#include "../../include/openssl/asyncerr.h"
#include "../../include/openssl/kdferr.h"
#include "../../include/openssl/storeerr.h"
#include "../../include/openssl/esserr.h"
#include "../../include/internal/propertyerr.h"

extern int load_crypto_strings_inited;
int err_load_crypto_strings_int(std::stringstream* commentsOnError) {
  (void) commentsOnError;
  if (load_crypto_strings_inited) {
    return 1;
  }
  load_crypto_strings_inited = 1;
  if (!ERR_load_ERR_strings(0)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to load error strings.\n";
    }
    return 0;
  }
  if (!ERR_load_BN_strings()) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to load big number strings.\n";
    }
    return 0;
  }
  if (!ERR_load_RSA_strings()) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to load RSA strings.\n";
    }
    return 0;
  }
  if (!ERR_load_DH_strings()) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to load DH strings.\n";
    }
    return 0;
  }
  if (!ERR_load_EVP_strings()) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to load EVP strings.\n";
    }
    return 0;
  }
  if (
    !ERR_load_BUF_strings()    ||
    !ERR_load_OBJ_strings()    ||
    !ERR_load_PEM_strings()    ||
    !ERR_load_DSA_strings()    ||
    !ERR_load_X509_strings()   ||
    !ERR_load_ASN1_strings()   ||
    !ERR_load_CONF_strings()   ||
    !ERR_load_CRYPTO_strings() ||
    !ERR_load_COMP_strings()   ||
    !ERR_load_EC_strings()     ||
    !ERR_load_BIO_strings()    ||
    !ERR_load_PKCS7_strings()  ||
    !ERR_load_X509V3_strings() ||
    !ERR_load_PKCS12_strings() ||
    !ERR_load_RAND_strings()   ||
    !ERR_load_DSO_strings()    ||
    !ERR_load_TS_strings()     ||
    !ERR_load_ENGINE_strings() ||
    !ERR_load_OCSP_strings()   ||
    !ERR_load_UI_strings()     ||
    !ERR_load_CMS_strings()    ||
    !ERR_load_CRMF_strings()   ||
    !ERR_load_CT_strings()     ||
    !ERR_load_ESS_strings()    ||
    !ERR_load_ASYNC_strings()  ||
    !ERR_load_KDF_strings()    ||
    !ERR_load_OSSL_STORE_strings() ||
    !ERR_load_PROP_strings()
  ) {
    return 0;
  }
  return 1;
}
