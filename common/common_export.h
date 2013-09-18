// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#if defined(COMPONENT_BUILD2)
#if defined(WIN32)

#if defined(COMMON_IMPLEMENTATION)
#define COMMON_EXPORT __declspec(dllexport)
#else
#define COMMON_EXPORT __declspec(dllimport)
#endif  // defined(COMMON_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(COMMON_IMPLEMENTATION)
#define COMMON_EXPORT __attribute__((visibility("default")))
#else
#define COMMON_EXPORT
#endif  // defined(COMMON_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define COMMON_EXPORT
#endif
