/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright (c) 2019, Oracle and/or its affiliates.  All rights reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "../include/openssl/crypto.h"
#include "../include/openssl/bn.h"
#include "include/internal/sparse_array.h"

/*
 * How many bits are used to index each level in the tree structre?
 * This setting determines the number of pointers stored in each node of the
 * tree used to represent the sparse array.  Having more pointers reduces the
 * depth of the tree but potentially wastes more memory.  That is, this is a
 * direct space versus time tradeoff.
 *
 * The large memory model uses twelve bits which means that the are 4096
 * pointers in each tree node.  This is more than sufficient to hold the
 * largest defined NID (as of Feb 2019).  This means that using a NID to
 * index a sparse array becomes a constant time single array look up.
 *
 * The small memory model uses four bits which means the tree nodes contain
 * sixteen pointers.  This reduces the amount of unused space significantly
 * at a cost in time.
 *
 * The library builder is also permitted to define other sizes in the closed
 * interval [2, sizeof(ossl_uintmax_t) * 8].
 */
#ifndef OPENSSL_SA_BLOCK_BITS
# ifdef OPENSSL_SMALL_FOOTPRINT
#  define OPENSSL_SA_BLOCK_BITS           4
# else
#  define OPENSSL_SA_BLOCK_BITS           12
# endif
#elif OPENSSL_SA_BLOCK_BITS < 2 || OPENSSL_SA_BLOCK_BITS > (BN_BITS2 - 1)
# error OPENSSL_SA_BLOCK_BITS is out of range
#endif

/*
 * From the number of bits, work out:
 *    the number of pointers in a tree node;
 *    a bit mask to quickly extract an index and
 *    the maximum depth of the tree structure.
  */
#define SA_BLOCK_MAX            (1 << OPENSSL_SA_BLOCK_BITS)
#define SA_BLOCK_MASK           (SA_BLOCK_MAX - 1)
#define SA_BLOCK_MAX_LEVELS     (((int)sizeof(ossl_uintmax_t) * 8 \
                                  + OPENSSL_SA_BLOCK_BITS - 1) \
                                 / OPENSSL_SA_BLOCK_BITS)

struct sparse_array_st {
  std::string name;
  int levelS;
  ossl_uintmax_t top;
  size_t nelem;
  void **nodes;
  std::string ToStringSummary() const;
};

std::string sparse_array_st::ToStringSummary() const {
  std::stringstream out;
  out << "Array: " << name << ", levels: " << this->levelS;
  return out.str();
}

sparse_array_st *OPENSSL_SA_new(const std::string& name) {
  OPENSSL_SA *res = (OPENSSL_SA *) OPENSSL_zalloc(sizeof(*res));
  res->name = name;
  return res;
}

static void sa_doall(const sparse_array_st *sa, void (*node)(void **),
                     void (*leaf)(ossl_uintmax_t, void *, void *), void *arg)
{
    int i[SA_BLOCK_MAX_LEVELS];
    void *nodes[SA_BLOCK_MAX_LEVELS];
    ossl_uintmax_t idx = 0;
    int l = 0;

    i[0] = 0;
    nodes[0] = sa->nodes;
    while (l >= 0) {
        const int n = i[l];
        void ** const p = (void **) nodes[l];

        if (n >= SA_BLOCK_MAX) {
            if (p != NULL && node != NULL)
                (*node)(p);
            l--;
            idx >>= OPENSSL_SA_BLOCK_BITS;
        } else {
            i[l] = n + 1;
            if (p != NULL && p[n] != NULL) {
                idx = (idx & ~SA_BLOCK_MASK) | n;
                if (l < sa->levelS - 1) {
                    i[++l] = 0;
                    nodes[l] = p[n];
                    idx <<= OPENSSL_SA_BLOCK_BITS;
                } else if (leaf != NULL) {
                    (*leaf)(idx, p[n], arg);
                }
            }
        }
    }
}

static void sa_free_node(void **p)
{
    OPENSSL_free(p);
}

static void sa_free_leaf(ossl_uintmax_t n, void *p, void *arg)
{
    OPENSSL_free(p);
}

void OPENSSL_SA_free(OPENSSL_SA *sa)
{
    sa_doall(sa, &sa_free_node, NULL, NULL);
    OPENSSL_free(sa);
}

void OPENSSL_SA_free_leaves(OPENSSL_SA *sa)
{
    sa_doall(sa, &sa_free_node, &sa_free_leaf, NULL);
    OPENSSL_free(sa);
}

/* Wrap this in a structure to avoid compiler warnings */
struct trampoline_st {
    void (*func)(ossl_uintmax_t, void *);
};

static void trampoline(ossl_uintmax_t n, void *l, void *arg)
{
    ((const struct trampoline_st *)arg)->func(n, l);
}

void OPENSSL_SA_doall(const OPENSSL_SA *sa, void (*leaf)(ossl_uintmax_t,
                                                         void *))
{
    struct trampoline_st tramp;

    tramp.func = leaf;
    if (sa != NULL)
        sa_doall(sa, NULL, &trampoline, &tramp);
}

void OPENSSL_SA_doall_arg(const OPENSSL_SA *sa,
                          void (*leaf)(ossl_uintmax_t, void *, void *),
                          void *arg)
{
    if (sa != NULL)
        sa_doall(sa, NULL, leaf, arg);
}

size_t OPENSSL_SA_num(const OPENSSL_SA *sa)
{
    return sa == NULL ? 0 : sa->nelem;
}

void *OPENSSL_SA_get(const sparse_array_st *sa, ossl_uintmax_t n, std::stringstream* commentsOnError) {
  int level;
  if (sa == NULL) {
    if (commentsOnError != 0) {
      *commentsOnError << "Calling openssl getter with empty openssl object is forbidden.\n";
    }
    return NULL;
  }
  if (n > sa->top) {
    if (commentsOnError != 0) {
      *commentsOnError << "Algorithm id is too large.\n";
    }
    return NULL;
  }
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: sa->levels is: " << sa->levelS << ".\n";
  }
  void **p, *r = NULL;
  p = sa->nodes;
  for (level = sa->levelS - 1; p != NULL && level > 0; level--) {
    *commentsOnError << "DEBUG: pointer p is: " << (long long) p << ".\n";
    int indexNonMasked = (n >> (OPENSSL_SA_BLOCK_BITS * level));
    int index = indexNonMasked & SA_BLOCK_MASK;
    p = (void **) p[index];
  }
  if (p != NULL) {
    r = p[n & SA_BLOCK_MASK];
  }
  return r;
}

static ossl_inline void **alloc_node(void)
{
    return (void **) OPENSSL_zalloc(SA_BLOCK_MAX * sizeof(void *));
}

int OPENSSL_SA_set(sparse_array_st *sa, ossl_uintmax_t posn, void *val, std::stringstream *commentsOnError) {
  int i, level = 1;
  ossl_uintmax_t n = posn;
  void **p;
  if (commentsOnError != 0) {
    *commentsOnError << "DEBUG: Setting array: " << sa->name;
  }
  if (sa == NULL) {
    if (commentsOnError != 0) {
      *commentsOnError << "Null sparse array cannot be set.\n";
    }
    return 0;
  }

    for (level = 1; level < SA_BLOCK_MAX_LEVELS; level++)
        if ((n >>= OPENSSL_SA_BLOCK_BITS) == 0)
            break;

    for (;sa->levelS < level; sa->levelS ++) {
        p = alloc_node();
        if (p == NULL)
            return 0;
        p[0] = sa->nodes;
        sa->nodes = p;
    }
    if (sa->top < posn)
        sa->top = posn;

    p = sa->nodes;
    for (level = sa->levelS - 1; level > 0; level--) {
        i = (posn >> (OPENSSL_SA_BLOCK_BITS * level)) & SA_BLOCK_MASK;
        if (p[i] == NULL && (p[i] = alloc_node()) == NULL)
            return 0;
        p = (void**) p[i];
    }
    p += posn & SA_BLOCK_MASK;
    if (val == NULL && *p != NULL)
        sa->nelem--;
    else if (val != NULL && *p == NULL)
        sa->nelem++;
    *p = val;
    return 1;
}