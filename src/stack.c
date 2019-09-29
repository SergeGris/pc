
#include <config.h>

#include "stack.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <xalloc.h>

bool
is_empty (struct stack *s)
{
  return s->n_used == 0 || s->sp == NULL || *s->sp == NULL;
}

void
stack_init (struct stack *s, size_t type_size, stack_dtor_t dtor)
{
  s->sp = xnmalloc ((s->n_alloc = 32), sizeof (*s) * type_size);
  *s->sp = NULL;
  s->n_used = 0;
  s->type_size = type_size;
  s->dtor = dtor;
}

void
stack_push (struct stack *s, void *data)
{
  if (s->n_used == s->n_alloc)
    {
      s->sp -= s->n_used;
      s->sp = xnrealloc (s->sp, s->n_alloc, 9 * s->type_size * sizeof (*s) / 8);
      s->sp += s->n_used;
      s->n_alloc = 9 * s->n_alloc / 8;
    }

  *s->sp = data;
  s->sp += s->type_size;
  s->n_used++;
}
void *
stack_pop (struct stack *s)
{
  assert (!is_empty (s));

  if (s->n_used > 32 && s->n_used / 2 - 1 > s->n_alloc)
    {
      s->sp -= s->n_used;
      s->sp = xnrealloc (s->sp, s->n_alloc, s->type_size * sizeof (*s) / 2);
      s->sp += s->n_used;
      s->n_alloc = s->n_alloc / 2;
    }

  s->n_used--;
  void *ret = *s->sp;
  s->sp -= s->type_size;
  return ret;
}
void *
stack_peek (struct stack *s)
{
  assert (!is_empty (s));

  return *s->sp;
}

void *
stack_at (struct stack *s, size_t i)
{
  assert (s->n_used >= i && !is_empty (s));

  return s->sp[(i - s->n_used) * s->type_size];
}

void
stack_clear (struct stack *s)
{
  assert (!is_empty (s));

  while (!is_empty (s))
    s->dtor (stack_pop (s));

  free (s->sp - s->n_used * s->type_size);
  s->sp = NULL;
  s->n_alloc = 0;
  s->n_used = 0;
}