/* Internal C11 thread definitions.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _THREAD_DEFS_H
#define _THREAD_DEFS_H 1

#include <features.h>

#include <errno.h>
#include <pthread.h>

#include "thread.h"

static __always_inline int
thread_error_map (int error_code)
{
  switch (error_code)
    {
    case 0:
      return thread_success;
    case ENOMEM:
      return thread_nomem;
    case ETIMEDOUT:
      return thread_timedout;
    case EBUSY:
      return thread_busy;
    default:
      return thread_error;
    }
}

#endif /* _THREAD_DEFS_H */
