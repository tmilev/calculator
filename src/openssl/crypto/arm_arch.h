/*
 * Copyright 2011-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef __ARM_ARCH_H__
# define __ARM_ARCH_H__

# ifndef __ASSEMBLER__
extern unsigned int OPENSSL_armcap_P;
# endif

# define ARMV7_NEON      (1<<0)
# define ARMV7_TICK      (1<<1)
# define ARMV8_AES       (1<<2)
# define ARMV8_SHA1      (1<<3)
# define ARMV8_SHA256    (1<<4)
# define ARMV8_PMULL     (1<<5)
# define ARMV8_SHA512    (1<<6)

#endif
