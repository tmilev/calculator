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

int err_load_crypto_strings_int(void)
{
    if (
#ifndef OPENSSL_NO_ERR
        ERR_load_ERR_strings() == 0 ||    /* include error strings for SYSerr */
        ERR_load_BN_strings() == 0 ||
# ifndef OPENSSL_NO_RSA
        ERR_load_RSA_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_DH
        ERR_load_DH_strings() == 0 ||
# endif
        ERR_load_EVP_strings() == 0 ||
        ERR_load_BUF_strings() == 0 ||
        ERR_load_OBJ_strings() == 0 ||
        ERR_load_PEM_strings() == 0 ||
# ifndef OPENSSL_NO_DSA
        ERR_load_DSA_strings() == 0 ||
# endif
        ERR_load_X509_strings() == 0 ||
        ERR_load_ASN1_strings() == 0 ||
        ERR_load_CONF_strings() == 0 ||
        ERR_load_CRYPTO_strings() == 0 ||
# ifndef OPENSSL_NO_COMP
        ERR_load_COMP_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_EC
        ERR_load_EC_strings() == 0 ||
# endif
        /* skip ERR_load_SSL_strings() because it is not in this library */
        ERR_load_BIO_strings() == 0 ||
        ERR_load_PKCS7_strings() == 0 ||
        ERR_load_X509V3_strings() == 0 ||
        ERR_load_PKCS12_strings() == 0 ||
        ERR_load_RAND_strings() == 0 ||
        ERR_load_DSO_strings() == 0 ||
# ifndef OPENSSL_NO_TS
        ERR_load_TS_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_ENGINE
        ERR_load_ENGINE_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_OCSP
        ERR_load_OCSP_strings() == 0 ||
# endif
        ERR_load_UI_strings() == 0 ||
# ifndef OPENSSL_NO_CMS
        ERR_load_CMS_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_CMP
        ERR_load_CRMF_strings() == 0 ||
# endif
# ifndef OPENSSL_NO_CT
        ERR_load_CT_strings() == 0 ||
# endif
        ERR_load_ESS_strings() == 0 ||
        ERR_load_ASYNC_strings() == 0 ||
#endif
        ERR_load_KDF_strings() == 0 ||
        ERR_load_OSSL_STORE_strings() == 0 ||
        ERR_load_PROP_strings() == 0)
        return 0;

    return 1;
}
