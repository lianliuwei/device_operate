// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_BIG_ENDIAN_H_
#define NET_BASE_BIG_ENDIAN_H_

#include "base/basictypes.h"

// Read an integer (signed or unsigned) from |buf| in Big Endian order.
// Note: this loop is unrolled with -O1 and above.
// NOTE(szym): glibc dns-canon.c and SpdyFrameBuilder use
// ntohs(*(uint16_t*)ptr) which is potentially unaligned.
// This would cause SIGBUS on ARMv5 or earlier and ARMv6-M.
template<typename T>
inline void ReadBigEndian(const char buf[], T* out) {
  *out = buf[0];
  for (size_t i = 1; i < sizeof(T); ++i) {
    *out <<= 8;
    // Must cast to uint8 to avoid clobbering by sign extension.
    *out |= static_cast<uint8>(buf[i]);
  }
}

// Write an integer (signed or unsigned) |val| to 
// |buf| in Big Endian order.
// Note: this loop is unrolled with -O1 and above.
template<typename T>
inline void WriteBigEndian(char buf[], T val) {
  for (size_t i = 0; i < sizeof(T); ++i) {
    buf[sizeof(T)-i-1] = static_cast<char>(val & 0xFF);
    val >>= 8;
  }
}


// Specializations to make clang happy about the (dead code) shifts above.
template<>
inline void ReadBigEndian<uint8>(const char buf[], uint8* out) {
  *out = buf[0];
}

template<>
inline void WriteBigEndian<uint8>(char buf[], uint8 val) {
  buf[0] = static_cast<char>(val);
}

// little version
template<typename T>
inline void ReadLittleEndian(const char buf[], T* out) {
  *out = buf[sizeof(T)-1];
  for (size_t i = 1; i < sizeof(T); ++i) {
    *out <<= 8;
    // Must cast to uint8 to avoid clobbering by sign extension.
    *out |= static_cast<uint8>(buf[sizeof(T)-i-1]);
  }
}

template<typename T>
inline void WriteLittleEndian(char buf[], T val) {
  for (size_t i = 0; i < sizeof(T); ++i) {
    buf[i] = static_cast<char>(val & 0xFF);
    val >>= 8;
  }
}

template<>
inline void ReadLittleEndian<uint8>(const char buf[], uint8* out) {
  *out = buf[0];
}

template<>
inline void WriteLittleEndian<uint8>(char buf[], uint8 val) {
  buf[0] = static_cast<char>(val);
}

#endif  // NET_BASE_BIG_ENDIAN_H_

