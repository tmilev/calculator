/*
 * Copyright 2015-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdlib.h>
#include <string.h>
#include "../../include/openssl/crypto.h"

#include "../include/internal/poly1305.h"
#include "poly1305_local.h"

size_t Poly1305_ctx_size(void)
{
    return sizeof(struct poly1305_context);
}

/* pick 32-bit unsigned integer in little endian order */
static unsigned int U8TOU32(const unsigned char *p)
{
    return (((unsigned int)(p[0] & 0xff)) |
            ((unsigned int)(p[1] & 0xff) << 8) |
            ((unsigned int)(p[2] & 0xff) << 16) |
            ((unsigned int)(p[3] & 0xff) << 24));
}

/*
 * Implementations can be classified by amount of significant bits in
 * words making up the multi-precision value, or in other words radix
 * or base of numerical representation, e.g. base 2^64, base 2^32,
 * base 2^26. Complementary characteristic is how wide is the result of
 * multiplication of pair of digits, e.g. it would take 128 bits to
 * accommodate multiplication result in base 2^64 case. These are used
 * interchangeably. To describe implementation that is. But interface
 * is designed to isolate this so that low-level primitives implemented
 * in assembly can be self-contained/self-coherent.
 */
/*
 * Even though there is __int128 reference implementation targeting
 * 64-bit platforms provided below, it's not obvious that it's optimal
 * choice for every one of them. Depending on instruction set overall
 * amount of instructions can be comparable to one in __int64
 * implementation. Amount of multiplication instructions would be lower,
 * but not necessarily overall. And in out-of-order execution context,
 * it is the latter that can be crucial...
 *
 * On related note. Poly1305 author, D. J. Bernstein, discusses and
 * provides floating-point implementations of the algorithm in question.
 * It made a lot of sense by the time of introduction, because most
 * then-modern processors didn't have pipelined integer multiplier.
 * [Not to mention that some had non-constant timing for integer
 * multiplications.] Floating-point instructions on the other hand could
 * be issued every cycle, which allowed to achieve better performance.
 * Nowadays, with SIMD and/or out-or-order execution, shared or
 * even emulated FPU, it's more complicated, and floating-point
 * implementation is not necessarily optimal choice in every situation,
 * rather contrary...
 *
 *                                              <appro@openssl.org>
 */

typedef unsigned int u32;

/*
 * poly1305_blocks processes a multiple of POLY1305_BLOCK_SIZE blocks
 * of |inp| no longer than |len|. Behaviour for |len| not divisible by
 * block size is unspecified in general case, even though in reference
 * implementation the trailing chunk is simply ignored. Per algorithm
 * specification, every input block, complete or last partial, is to be
 * padded with a bit past most significant byte. The latter kind is then
 * padded with zeros till block size. This last partial block padding
 * is caller(*)'s responsibility, and because of this the last partial
 * block is always processed with separate call with |len| set to
 * POLY1305_BLOCK_SIZE and |padbit| to 0. In all other cases |padbit|
 * should be set to 1 to perform implicit padding with 128th bit.
 * poly1305_blocks does not actually check for this constraint though,
 * it's caller(*)'s responsibility to comply.
 *
 * (*)  In the context "caller" is not application code, but higher
 *      level Poly1305_* from this very module, so that quirks are
 *      handled locally.
 */
static void
poly1305_blocks(void *ctx, const unsigned char *inp, size_t len, u32 padbit);

/*
 * Type-agnostic "rip-off" from constant_time_locl.h
 */
# define CONSTANT_TIME_CARRY(a,b) ( \
         (a ^ ((a ^ b) | ((a - b) ^ b))) >> (sizeof(a) * 8 - 1) \
         )

# if (defined(__SIZEOF_INT128__) && __SIZEOF_INT128__==16) && \
     (defined(__SIZEOF_LONG__) && __SIZEOF_LONG__==8)

typedef unsigned long u64;
typedef __uint128_t u128;

typedef struct {
    u64 h[3];
    u64 r[2];
} poly1305_internal;

/* pick 32-bit unsigned integer in little endian order */
static u64 U8TOU64(const unsigned char *p)
{
    return (((u64)(p[0] & 0xff)) |
            ((u64)(p[1] & 0xff) << 8) |
            ((u64)(p[2] & 0xff) << 16) |
            ((u64)(p[3] & 0xff) << 24) |
            ((u64)(p[4] & 0xff) << 32) |
            ((u64)(p[5] & 0xff) << 40) |
            ((u64)(p[6] & 0xff) << 48) |
            ((u64)(p[7] & 0xff) << 56));
}

/* store a 32-bit unsigned integer in little endian */
static void U64TO8(unsigned char *p, u64 v)
{
    p[0] = (unsigned char)((v) & 0xff);
    p[1] = (unsigned char)((v >> 8) & 0xff);
    p[2] = (unsigned char)((v >> 16) & 0xff);
    p[3] = (unsigned char)((v >> 24) & 0xff);
    p[4] = (unsigned char)((v >> 32) & 0xff);
    p[5] = (unsigned char)((v >> 40) & 0xff);
    p[6] = (unsigned char)((v >> 48) & 0xff);
    p[7] = (unsigned char)((v >> 56) & 0xff);
}

void poly1305_init(void *ctx, const unsigned char key[16])
{
    poly1305_internal *st = (poly1305_internal *) ctx;

    /* h = 0 */
    st->h[0] = 0;
    st->h[1] = 0;
    st->h[2] = 0;

    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    st->r[0] = U8TOU64(&key[0]) & 0x0ffffffc0fffffff;
    st->r[1] = U8TOU64(&key[8]) & 0x0ffffffc0ffffffc;
}

static void
poly1305_blocks(void *ctx, const unsigned char *inp, size_t len, u32 padbit)
{
    poly1305_internal *st = (poly1305_internal *)ctx;
    u64 r0, r1;
    u64 s1;
    u64 h0, h1, h2, c;
    u128 d0, d1;

    r0 = st->r[0];
    r1 = st->r[1];

    s1 = r1 + (r1 >> 2);

    h0 = st->h[0];
    h1 = st->h[1];
    h2 = st->h[2];

    while (len >= POLY1305_BLOCK_SIZE) {
        /* h += m[i] */
        h0 = (u64)(d0 = (u128)h0 + U8TOU64(inp + 0));
        h1 = (u64)(d1 = (u128)h1 + (d0 >> 64) + U8TOU64(inp + 8));
        /*
         * padbit can be zero only when original len was
         * POLY1306_BLOCK_SIZE, but we don't check
         */
        h2 += (u64)(d1 >> 64) + padbit;

        /* h *= r "%" p, where "%" stands for "partial remainder" */
        d0 = ((u128)h0 * r0) +
             ((u128)h1 * s1);
        d1 = ((u128)h0 * r1) +
             ((u128)h1 * r0) +
             (h2 * s1);
        h2 = (h2 * r0);

        /* last reduction step: */
        /* a) h2:h0 = h2<<128 + d1<<64 + d0 */
        h0 = (u64)d0;
        h1 = (u64)(d1 += d0 >> 64);
        h2 += (u64)(d1 >> 64);
        /* b) (h2:h0 += (h2:h0>>130) * 5) %= 2^130 */
        c = (h2 >> 2) + (h2 & ~3UL);
        h2 &= 3;
        h0 += c;
        h1 += (c = CONSTANT_TIME_CARRY(h0,c));
        h2 += CONSTANT_TIME_CARRY(h1,c);
        /*
         * Occasional overflows to 3rd bit of h2 are taken care of
         * "naturally". If after this point we end up at the top of
         * this loop, then the overflow bit will be accounted for
         * in next iteration. If we end up in poly1305_emit, then
         * comparison to modulus below will still count as "carry
         * into 131st bit", so that properly reduced value will be
         * picked in conditional move.
         */

        inp += POLY1305_BLOCK_SIZE;
        len -= POLY1305_BLOCK_SIZE;
    }

    st->h[0] = h0;
    st->h[1] = h1;
    st->h[2] = h2;
}

static void poly1305_emit(void *ctx, unsigned char mac[16],
                          const u32 nonce[4])
{
    poly1305_internal *st = (poly1305_internal *) ctx;
    u64 h0, h1, h2;
    u64 g0, g1, g2;
    u128 t;
    u64 mask;

    h0 = st->h[0];
    h1 = st->h[1];
    h2 = st->h[2];

    /* compare to modulus by computing h + -p */
    g0 = (u64)(t = (u128)h0 + 5);
    g1 = (u64)(t = (u128)h1 + (t >> 64));
    g2 = h2 + (u64)(t >> 64);

    /* if there was carry into 131st bit, h1:h0 = g1:g0 */
    mask = 0 - (g2 >> 2);
    g0 &= mask;
    g1 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | g0;
    h1 = (h1 & mask) | g1;

    /* mac = (h + nonce) % (2^128) */
    h0 = (u64)(t = (u128)h0 + nonce[0] + ((u64)nonce[1]<<32));
    h1 = (u64)(t = (u128)h1 + nonce[2] + ((u64)nonce[3]<<32) + (t >> 64));

    U64TO8(mac + 0, h0);
    U64TO8(mac + 8, h1);
}



#else
int poly1305_init(void *ctx, const unsigned char key[16], void *func);
void poly1305_blocks(void *ctx, const unsigned char *inp, size_t len,
                     unsigned int padbit);
void poly1305_emit(void *ctx, unsigned char mac[16],
                   const unsigned int nonce[4]);
#endif

void Poly1305_Init(POLY1305 *ctx, const unsigned char key[32])
{
    ctx->nonce[0] = U8TOU32(&key[16]);
    ctx->nonce[1] = U8TOU32(&key[20]);
    ctx->nonce[2] = U8TOU32(&key[24]);
    ctx->nonce[3] = U8TOU32(&key[28]);

#ifndef POLY1305_ASM
    poly1305_init(ctx->opaque, key);
#else
    /*
     * Unlike reference poly1305_init assembly counterpart is expected
     * to return a value: non-zero if it initializes ctx->func, and zero
     * otherwise. Latter is to simplify assembly in cases when there no
     * multiple code paths to switch between.
     */
    if (!poly1305_init(ctx->opaque, key, &ctx->func)) {
        ctx->func.blocks = poly1305_blocks;
        ctx->func.emit = poly1305_emit;
    }
#endif

    ctx->num = 0;

}

#ifdef POLY1305_ASM
/*
 * This "eclipses" poly1305_blocks and poly1305_emit, but it's
 * conscious choice imposed by -Wshadow compiler warnings.
 */
# define poly1305_blocks (*poly1305_blocks_p)
# define poly1305_emit   (*poly1305_emit_p)
#endif

void Poly1305_Update(POLY1305 *ctx, const unsigned char *inp, size_t len)
{
#ifdef POLY1305_ASM
    /*
     * As documented, poly1305_blocks is never called with input
     * longer than single block and padbit argument set to 0. This
     * property is fluently used in assembly modules to optimize
     * padbit handling on loop boundary.
     */
    poly1305_blocks_f poly1305_blocks_p = ctx->func.blocks;
#endif
    size_t rem, num;

    if ((num = ctx->num)) {
        rem = POLY1305_BLOCK_SIZE - num;
        if (len >= rem) {
            memcpy(ctx->data + num, inp, rem);
            poly1305_blocks(ctx->opaque, ctx->data, POLY1305_BLOCK_SIZE, 1);
            inp += rem;
            len -= rem;
        } else {
            /* Still not enough data to process a block. */
            memcpy(ctx->data + num, inp, len);
            ctx->num = num + len;
            return;
        }
    }

    rem = len % POLY1305_BLOCK_SIZE;
    len -= rem;

    if (len >= POLY1305_BLOCK_SIZE) {
        poly1305_blocks(ctx->opaque, inp, len, 1);
        inp += len;
    }

    if (rem)
        memcpy(ctx->data, inp, rem);

    ctx->num = rem;
}

void Poly1305_Final(POLY1305 *ctx, unsigned char mac[16])
{
#ifdef POLY1305_ASM
    poly1305_blocks_f poly1305_blocks_p = ctx->func.blocks;
    poly1305_emit_f poly1305_emit_p = ctx->func.emit;
#endif
    size_t num;

    if ((num = ctx->num)) {
        ctx->data[num++] = 1;   /* pad bit */
        while (num < POLY1305_BLOCK_SIZE)
            ctx->data[num++] = 0;
        poly1305_blocks(ctx->opaque, ctx->data, POLY1305_BLOCK_SIZE, 0);
    }

    poly1305_emit(ctx->opaque, mac, ctx->nonce);

    /* zero out the state */
    OPENSSL_cleanse(ctx, sizeof(*ctx));
}
