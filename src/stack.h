
#ifndef _STACK_H
#define _STACK_H 1

#include <stdbool.h>
#include <stddef.h>

struct stack
{
  void **sp;
  size_t n_used;
  size_t n_alloc;
  void (*dtor) (void *);
};

bool is_empty (struct stack *s);

void stack_init (struct stack *s, void (*dtor) (void *));
void stack_push (struct stack *s, void *data);
void *stack_pop (struct stack *s);
void *stack_peek (struct stack *s);
void *stack_at (struct stack *s, size_t i);
void stack_delete_all (struct stack *s);
void stack_clear (struct stack *s);

#endif /* _STACK_H */
