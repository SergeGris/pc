
#include <config.h>

#include "stack.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "xalloc.h"

bool
is_empty (struct stack *s)
{
  return s == NULL || s->sp == NULL || s->n_alloc == 0 || s->n_used == 0;
}

void
stack_init (struct stack *s, void (*dtor) (void *))
{
  assert (is_empty (s));

  s->n_alloc = 64;
  s->sp = xnmalloc (s->n_alloc, sizeof (void *));
  *s->sp = NULL;
  s->n_used = 0;
  s->dtor = dtor;
}
void
stack_push (struct stack *s, void *data)
{
  assert (s->n_alloc > 0);

  if (s->n_used == s->n_alloc)
    {
      s->sp -= s->n_used * sizeof (void *);
      s->sp = xnrealloc (s->sp, s->n_alloc, 2 * sizeof (void *));
      s->sp += s->n_used * sizeof (void *);
      s->n_alloc *= 2;
    }

  *s->sp = data;
  s->sp += sizeof (void *);
  s->n_used++;
}
void *
stack_pop (struct stack *s)
{
  assert (!is_empty (s));

  if (s->n_alloc > 64 && s->n_alloc / 2 >= s->n_used)
    {
      s->sp -= s->n_used * sizeof (void *);
      s->sp = xnrealloc (s->sp, s->n_alloc, sizeof (void *) / 2);
      s->sp += s->n_used * sizeof (void *);
      s->n_alloc /= 2;
    }

  s->n_used--;
  s->sp -= sizeof (void *);
  return *s->sp;
}
void *
stack_peek (struct stack *s)
{
  assert (!is_empty (s));

  return s->sp[-sizeof (void *)];
}

void *
stack_at (struct stack *s, size_t i)
{
  assert (s->n_used >= i);
  assert (!is_empty (s));

  return (void **) (((char **) s->sp)[(i - s->n_used) * sizeof (void *)]);
}

void
stack_delete_all (struct stack *s)
{
  while (s->n_used != 0)
    s->dtor (stack_pop (s));
}

void
stack_clear (struct stack *s)
{
  if (s->sp == NULL)
    return;

  if (s->dtor != NULL)
    stack_delete_all (s);

  s->sp -= s->n_used * sizeof (void *);
  free (s->sp);
  s->sp = NULL;
  s->n_alloc = 0;
}
