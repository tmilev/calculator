/*
 * Copyright 2002-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include "../../include/openssl/crypto.h"
#include "../../include/internal/cryptlib.h"
#include "../../include/openssl/conf.h"
#include "../../include/openssl/x509.h"
#include "../../include/openssl/asn1.h"
#include "../../include/openssl/engine.h"
#include "../../include/internal/provider.h"
#include "conf_lcl.h"

/* Load all OpenSSL builtin modules */

void OPENSSL_load_builtin_modules(void)
{
    /* Add builtin modules here */
    ASN1_add_oid_module();
    ASN1_add_stable_module();
#ifndef OPENSSL_NO_ENGINE
    ENGINE_add_conf_module();
#endif
    EVP_add_alg_module();
    conf_add_ssl_module();
    ossl_provider_add_conf_module();
}