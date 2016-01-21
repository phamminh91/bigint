#ifndef BIGINT_H_
#define BIGINT_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BASE 1000000000

typedef struct bigint bigint;

struct bigint {
  bool positive;
  int digits;
  int xlen;
  int* x; // 222222222111111111 is stored as x->|11111111|222222222|
};

bigint* bi_copy(const bigint *);
bigint* bi_fromstring(const char *str);
void bi_delete(bigint *);

int bi_cmp(const bigint *, const bigint *);
bool bi_equal(const bigint *, const bigint *);
bool bi_is_zero(const bigint *);
bool bi_is_one(const bigint *);
bool bi_is_minus_one(const bigint *);

bigint* bi_zero();
bigint* bi_negate(const bigint *);

bigint* bi_add(const bigint *, const bigint *);
bigint* bi_sub(const bigint *, const bigint *);
bigint* bi_mul(const bigint *, const bigint *);
bigint* bi_div(const bigint *, const bigint *);

bigint* bi_factorial(const bigint *);

void bi_print(const bigint *);

#endif

