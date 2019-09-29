
#ifndef _STACK_H
#define _STACK_H 1

#include <stdbool.h>
#include <stddef.h>

typedef void (*stack_dtor_t) (void *);

typedef struct stack
{
  void **sp;
  size_t n_used;
  size_t n_alloc;
  size_t type_size;
  stack_dtor_t dtor;
} stack_t[1];

bool is_empty (struct stack *s);

void stack_init (struct stack *s, size_t type_size, stack_dtor_t dtor);
void stack_push (struct stack *s, void *data);
void *stack_pop (struct stack *s);
void *stack_peek (struct stack *s);
void *stack_at (struct stack *s, size_t i);
void stack_clear (struct stack *s);
void set_destructor (stack_dtor_t d);

#endif /* _STACK_H */
