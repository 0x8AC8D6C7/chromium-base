﻿// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_descriptor_shuffle.h"

#include <process.h>
#include <stddef.h>
#include <ostream>

#if defined(OS_POSIX)
#include <unistd.h>
#endif

#include "base/eintr_wrapper.h"
#include "base/logging.h"

namespace base {

bool PerformInjectiveMultimapDestructive(
    InjectiveMultimap* m, InjectionDelegate* delegate) {
  static const size_t kMaxExtraFDs = 16;
  int extra_fds[kMaxExtraFDs];
  unsigned next_extra_fd = 0;

  // DANGER: this function may not allocate.

  for (InjectiveMultimap::iterator i = m->begin(); i != m->end(); ++i) {
    int temp_fd = -1;

    // We DCHECK the injectiveness of the mapping.
    for (InjectiveMultimap::iterator j = i + 1; j != m->end(); ++j) {
      DCHECK(i->dest != j->dest) << "Both fd " << i->source
          << " and " << j->source << " map to " << i->dest;
    }

    const bool is_identity = i->source == i->dest;

    for (InjectiveMultimap::iterator j = i + 1; j != m->end(); ++j) {
      if (!is_identity && i->dest == j->source) {
        if (temp_fd == -1) {
          if (!delegate->Duplicate(&temp_fd, i->dest))
            return false;
          if (next_extra_fd < kMaxExtraFDs) {
            extra_fds[next_extra_fd++] = temp_fd;
          } else {
            RAW_LOG(ERROR, "PerformInjectiveMultimapDestructive overflowed "
                           "extra_fds. Leaking file descriptors!");
          }
        }

        j->source = temp_fd;
        j->close = false;
      }

      if (i->close && i->source == j->dest)
        i->close = false;

      if (i->close && i->source == j->source) {
        i->close = false;
        j->close = true;
      }
    }

    if (!is_identity) {
      if (!delegate->Move(i->source, i->dest))
        return false;
    }

    if (!is_identity && i->close)
      delegate->Close(i->source);
  }

  for (unsigned i = 0; i < next_extra_fd; i++)
    delegate->Close(extra_fds[i]);

  return true;
}

bool PerformInjectiveMultimap(const InjectiveMultimap& m_in,
                              InjectionDelegate* delegate) {
  InjectiveMultimap m(m_in);
  return PerformInjectiveMultimapDestructive(&m, delegate);
}

bool FileDescriptorTableInjection::Duplicate(int* result, int fd) {
#if defined(OS_WIN)
  *result = HANDLE_EINTR(_dup(fd));
#endif

#if defined(OS_POSIX)
  *result = HANDLE_EINTR(dup(fd));
#endif

  return *result >= 0;
}

bool FileDescriptorTableInjection::Move(int src, int dest) {
#if defined(OS_WIN)
  return HANDLE_EINTR(_dup2(src, dest)) != -1;
#endif

#if defined(OS_POSIX)
  return HANDLE_EINTR(dup2(src, dest)) != -1;
#endif
}

void FileDescriptorTableInjection::Close(int fd) {
#if defined(OS_WIN)
  int ret = HANDLE_EINTR(_close(fd));
  DPCHECK(ret == 0);
#endif

#if defined(OS_POSIX)
  int ret = HANDLE_EINTR(close(fd));
  DPCHECK(ret == 0);
#endif


}

}  // namespace base
