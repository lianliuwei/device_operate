// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

namespace common {

// A class with this type may be registered via
// BrowserThread::SetDelegate.
//
// If registered as such, it will receive an Init() call right before
// the BrowserThread in question starts its message loop (and right
// after the BrowserThread has done its own initialization), and a
// CleanUp call right after the message loop ends (and before the
// BrowserThread has done its own clean-up).
class CommonThreadDelegate {
 public:
  virtual ~CommonThreadDelegate() {}

  // Called just prior to starting the message loop.
  virtual void Init() = 0;

  // Called just after the message loop ends.
  virtual void CleanUp() = 0;
};

}  // namespace common
