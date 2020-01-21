# License
Copyright © 2008-2018 Todor Emilov Milev

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


# Older licenses
Older versions of this software were licensed under the terms of LGPL 3.0.
As of 2018, April 26, the license has been changed to the Apache License, Version 2.0.

-------------------------------------------------------------------------------


The calculator includes prebuilt files from the following projects. 
Their respective licenses have been listed here for your convenience.
1. MathJax, included in minified form (Apache 2.0).
2. MathQuill, included in minified form, licensed under Mozilla Public License 2.0.
3. Ace Editor, included in minified form, licensed under BSD license.
4. JQuery, included in minified form, licensed under MIT License.

In addition, the calculator project contains code snippets from the following
sources.

5. Tiny AES in C, https://github.com/kokke/tiny-AES-c
6. https://github.com/sipa/Coin25519
7. https://github.com/brainhub/SHA3IUF


The code snippet notices are included for your convenience below.
--------------------------------------------------------------------------------
5. Tiny AES in C, https://github.com/kokke/tiny-AES-c

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>


--------------------------------------------------------------------------------
6. https://github.com/sipa/Coin25519

// adapted by Pieter Wuille in 2012; all changes are in the public domain

/*
 *
 *  RIPEMD160.c : RIPEMD-160 implementation
 *
 * Written in 2008 by Dwayne C. Litzenberger <dlitz@dlitz.net>
 *
 * ===================================================================
 * The contents of this file are dedicated to the public domain.  To
 * the extent that dedication to the public domain is not available,
 * everyone is granted a worldwide, perpetual, royalty-free,
 * non-exclusive license to exercise all rights associated with the
 * contents of this file for any purpose whatsoever.
 * No rights are reserved.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ===================================================================
 *
 * Country of origin: Canada
 *
 * This implementation (written in C) is based on an implementation the author
 * wrote in Python.
 *
 * This implementation was written with reference to the RIPEMD-160
 * specification, which is available at:
 * http://homes.esat.kuleuven.be/~cosicart/pdf/AB-9601/
 *
 * It is also documented in the _Handbook of Applied Cryptography_, as
 * Algorithm 9.55.  It's on page 30 of the following PDF file:
 * http://www.cacr.math.uwaterloo.ca/hac/about/chap9.pdf
 *
 * The RIPEMD-160 specification doesn't really tell us how to do padding, but
 * since RIPEMD-160 is inspired by MD4, you can use the padding algorithm from
 * RFC 1320.
 *
 * According to http://www.users.zetnet.co.uk/hopwood/crypto/scan/md.html:
 *   "RIPEMD-160 is big-bit-endian, little-byte-endian, and left-justified."
 */


--------------------------------------------------------------------------------
7. https://github.com/brainhub/SHA3IUF/blob/master/sha3.c

/* -------------------------------------------------------------------------
 * Works when compiled for either 32-bit or 64-bit targets, optimized for 
 * 64 bit.
 *
 * Canonical implementation of Init/Update/Finalize for SHA-3 byte input. 
 *
 * SHA3-256, SHA3-384, SHA-512 are implemented. SHA-224 can easily be added.
 *
 * Based on code from http://keccak.noekeon.org/ .
 *
 * I place the code that I wrote into public domain, free to use. 
 *
 * I would appreciate if you give credits to this work if you used it to 
 * write or test * your code.
 *
 * Aug 2015. Andrey Jivsov. crypto@brainhub.org
 * ---------------------------------------------------------------------- */

