﻿// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/pending_task.h"

namespace base {

PendingTask::PendingTask(const tracked_objects::Location& posted_from,
                         const base::Closure& task)
  : task(task),
    posted_from(posted_from),
    sequence_num(0),
    nestable(true) {

  delayed_run_time = TimeTicks();
  time_posted = TimeTicks();
}

PendingTask::PendingTask(const tracked_objects::Location& posted_from,
                         const base::Closure& task,
                         TimeTicks delayed_run_time,
                         bool nestable)
    : task(task),
      posted_from(posted_from),
      sequence_num(0),
      nestable(nestable) {

  delayed_run_time = TimeTicks();
  time_posted = TimeTicks();
}

PendingTask::~PendingTask() {
}

bool PendingTask::operator<(const PendingTask& other) const {
  // Since the top of a priority queue is defined as the "greatest" element, we
  // need to invert the comparison here.  We want the smaller time to be at the
  // top of the heap.

  if (delayed_run_time < other.delayed_run_time)
    return false;

  if (delayed_run_time > other.delayed_run_time)
    return true;

  // If the times happen to match, then we use the sequence number to decide.
  // Compare the difference to support integer roll-over.
  return (sequence_num - other.sequence_num) > 0;
}

void TaskQueue::Swap(TaskQueue* queue) {
  c.swap(queue->c);  // Calls std::deque::swap.
}

}  // namespace base
