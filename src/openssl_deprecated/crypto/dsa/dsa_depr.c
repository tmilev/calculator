/*
 * Copyright 2002-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * This file contains deprecated function(s) that are now wrappers to the new
 * version(s).
 */

/*
 * Parameter generation follows the updated Appendix 2.2 for FIPS PUB 186,
 * also Appendix 2.2 of FIPS PUB 186-1 (i.e. use SHA as defined in FIPS PUB
 * 180-1)
 */
#define xxxHASH    EVP_sha1()

#include "../../include/openssl/opensslconf.h"
#if OPENSSL_API_0_9_8
NON_EMPTY_TRANSLATION_UNIT
#else

# include <stdio.h>
# include <time.h>
# include "../../include/internal/cryptlib.h"
# include "../../include/openssl/evp.h"
# include "../../include/openssl/bn.h"
# include "../../include/openssl/dsa.h"
# include "../../include/openssl/sha.h"

DSA *DSA_generate_parameters(int bits,
                             unsigned char *seed_in, int seed_len,
                             int *counter_ret, unsigned long *h_ret,
                             void (*callback) (int, int, void *),
                             void *cb_arg)
{
    BN_GENCB *cb;
    DSA *ret;

    if ((ret = DSA_new()) == NULL)
        return NULL;
    cb = BN_GENCB_new();
    if (cb == NULL)
        goto err;

    BN_GENCB_set_old(cb, callback, cb_arg);

    if (DSA_generate_parameters_ex(ret, bits, seed_in, seed_len,
                                   counter_ret, h_ret, cb)) {
        BN_GENCB_free(cb);
        return ret;
    }
    BN_GENCB_free(cb);
err:
    DSA_free(ret);
    return NULL;
}
#endif