// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef MURMURHASH3_H_INCLUDED
#define MURMURHASH3_H_INCLUDED

#include <stdint.h>

// MurmurHash3 produces 32-bit hash values on x86 platform
uint32_t MurmurHash3 (const void * key, int len, uint32_t seed);

#endif // MURMURHASH3_H_INCLUDED
