/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright (c) 2019, Oracle and/or its affiliates.  All rights reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_SPARSE_ARRAY_H
# define HEADER_SPARSE_ARRAY_H

# include "../../../include/openssl/e_os2.h"
#include <sstream>
# define SPARSE_ARRAY_OF(type) struct sparse_array_st_ ## type

typedef struct sparse_array_st OPENSSL_SA;
OPENSSL_SA *OPENSSL_SA_new();
void OPENSSL_SA_free(OPENSSL_SA *sa);
void OPENSSL_SA_free_leaves(OPENSSL_SA *sa);
size_t OPENSSL_SA_num(const OPENSSL_SA *sa);
void OPENSSL_SA_doall(const OPENSSL_SA *sa,
                      void (*leaf)(ossl_uintmax_t, void *));
void OPENSSL_SA_doall_arg(const OPENSSL_SA *sa,
                          void (*leaf)(ossl_uintmax_t, void *, void *), void *);
void *OPENSSL_SA_get(const OPENSSL_SA *sa, ossl_uintmax_t n, std::stringstream *commentsOnError);
int OPENSSL_SA_set(OPENSSL_SA *sa, ossl_uintmax_t n, void *val);

#endif
