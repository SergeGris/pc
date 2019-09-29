/* C11 thread mutex initialization implementation.
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

#include <stdalign.h>

#include "thread_defs.h"

extern int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind);

int
mutex_init (mutex_t *mutex, int type)
{
  _Static_assert (sizeof (mutex_t) == sizeof (pthread_mutex_t),
                  "sizeof (mutex_t) != sizeof (pthread_mutex_t)");
  _Static_assert (alignof (mutex_t) == alignof (pthread_mutex_t),
                  "alignof (mutex_t) != alignof (pthread_mutex_t)");

  pthread_mutexattr_t attr;
  pthread_mutexattr_init (&attr);

  /* Another possible solution would be to set the flags directly in
     mutex object.  */
  switch (type)
    {
    case mutex_plain | mutex_recursive:
    case mutex_timed | mutex_recursive:
      pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);
      break;
    case mutex_plain:
    case mutex_timed: /* No difference between both in standard.  */
    default:
      pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_TIMED_NP);
      break;
    }

  /* pthread_mutexattr_destroy implementation is a noop.  */
  return thread_error_map (pthread_mutex_init ((pthread_mutex_t *) mutex, &attr));
}
