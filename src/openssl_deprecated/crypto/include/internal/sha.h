/*
 * Copyright 2018 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright (c) 2018, Oracle and/or its affiliates.  All rights reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_INTERNAL_SHA_H
# define HEADER_INTERNAL_SHA_H

# include "../../../include/openssl/opensslconf.h"
#include "../../../include/openssl/sha.h"
int sha512_224_init(SHA512_CTX *);
int sha512_256_init(SHA512_CTX *);

#endif