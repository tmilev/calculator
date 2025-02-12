/*
 * Copyright 2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */
#include "../include/openssl/crypto.h"

#ifdef OPENSSL_SYS_UNIX
# ifndef OPENSSL_NO_DEPRECATED_3_0

void OPENSSL_fork_prepare(void)
{
}

void OPENSSL_fork_parent(void)
{
}

void OPENSSL_fork_child(void)
{
}

# endif
#endif
