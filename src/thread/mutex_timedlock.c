/* C11 thread mutex timed lock implementation.
   Copyright (C) 2018-2019Free Software Foundation, Inc.
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

#include "thread_defs.h"

extern int pthread_mutex_timedlock (pthread_mutex_t *__mutex, const struct timespec *__abstime);

int
mutex_timedlock (mutex_t *__restrict mutex, const struct timespec *__restrict time_point)
{
  return thread_error_map (pthread_mutex_timedlock ((pthread_mutex_t *) mutex, time_point));
}
