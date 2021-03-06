// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

// This file describes various types used to describe and filter notifications
// that pass through the NotificationService.
//
// Only notifications that are fired from the content module should be here. We
// should never have a notification that is fired by the embedder and listened
// to by content.
namespace common {

enum NotificationType {
  NOTIFICATION_CONTENT_START = 0,

  // General -----------------------------------------------------------------

  // Special signal value to represent an interest in all notifications.
  // Not valid when posting a notification.
  NOTIFICATION_ALL = NOTIFICATION_CONTENT_START,

  // The app is done processing user actions, now is a good time to do
  // some background work.
  NOTIFICATION_IDLE,

  // Means that the app has just started doing something in response to a
  // user action, and that background processes shouldn't run if avoidable.
  NOTIFICATION_BUSY,

  // Custom notifications used by the embedder should start from here.
  NOTIFICATION_CONTENT_END,
};

}  // namespace content
