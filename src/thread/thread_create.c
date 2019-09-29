/* C11 thread thread creation implementation.
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

#include "thread_priv.h"

int
thread_create (tid_t *thread, thread_start_t func, void *arg)
{
  _Static_assert (sizeof (tid_t) == sizeof (pthread_t),
                  "sizeof (tid_t) != sizeof (pthread_t)");

  return thread_error_map (pthread_create (thread, ATTR_C11_THREAD, (void *(*) (void *)) func, arg));
}
