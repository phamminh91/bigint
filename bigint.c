#include "bigint.h"

static inline int bi_opposite_sign(const bigint* a, const bigint * b);

bigint* bi_fromstring(const char *str) {
  // verify string format

  // allocate the bigint struct
  bigint* retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;

  // determine sign
  if (*str == '-') {
    retval->positive = false;
    ++str;
  } else
    retval->positive = true;

  // skip zero
  while (*str == '0')
    str++;

  // Bigint ZERO
  if (*str == '\0') {
    retval->xlen = 0;
    retval->digits = 0;
    retval->x = NULL;
    retval->positive = true;
    return retval;
  }

  // calculate size of x
  int digits = (int)strlen(str);
  int xlen = (digits + 8) / 9;

  // allocate memory for x
  int *x = malloc(digits * sizeof(int));
  if (!x) {
    free(retval);
    return NULL;
  }

  // initialize content of x
  int i = digits-1;
  int at = 1;
  for (; i >= 8; i -= 9) {
    at = i/9;
    x[xlen-at-1] = (str[i-8] - '0') * 100000000
                 + (str[i-7] - '0') * 10000000
                 + (str[i-6] - '0') * 1000000
                 + (str[i-5] - '0') * 100000
                 + (str[i-4] - '0') * 10000
                 + (str[i-3] - '0') * 1000
                 + (str[i-2] - '0') * 100
                 + (str[i-1] - '0') * 10
                 + (str[i] - '0');
  }

  if (at > 0) {
    int power = 1;
    int accum = 0;
    for (; i >= 0; --i) {
      accum += power * (str[i] - '0');
      power *= 10;
    }
    x[xlen-1] = accum;
  }

  retval->xlen = xlen;
  retval->x = x;
  retval->digits = digits;

  return retval;
}

void bi_delete(bigint* a) {
  if (a) {
    free(a->x);
    free(a);
  }
}

bigint* bi_add(const bigint *a, const bigint *b) {
  // One operand is NULL
  if (!(a && b))
    return NULL;

  // One operand is bigint zero
  if (bi_is_zero(a))
    return bi_copy(b);
  if (bi_is_zero(b))
    return bi_copy(a);

  // Allocate memory for bigint
  bigint* retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;

  // subtract or add?
  if (bi_opposite_sign(a, b)) {
    retval->digits = b->digits;
    retval->xlen = b->xlen;
    retval->x = b->x;
    retval->positive = !(b->positive);
    bigint* res = bi_sub(a, retval);
    free(retval);
    return res;
  }

  // From here on, a and b will have the same sign
  if (a->digits > b->digits) {
    const bigint* tmp = a;
    a = b;
    b = tmp;
  }

  int* ax = a->x;
  int* bx = b->x;
  int axlen = a->xlen;
  int bxlen = b->xlen;
  int xlen = bxlen;

  int *x = malloc((bxlen + 1) * sizeof(int));
  if (!x) {
    free(retval);
    return NULL;
  }

  long sum = 0;
  int i = 0;
  for (; i < axlen; i++) {
    sum += (long)ax[i] + (long)bx[i];
    x[i] = (int)(sum % BASE);
    sum /= BASE;
  }

  for (; i < bxlen; i++) {
    sum += (long)bx[i];
    x[i] = (int)(sum % BASE);
    sum /= BASE;
  }
  x[i] = (int)sum;

  if (sum > 0)
    xlen++;

  // calculate number of digits
  int ndigits;
  if (xlen == 1) {
    ndigits = 0;
    for (int t = x[0]; t >= 1; t /= 10)
      ++ndigits;
  } else if (sum) {
    ndigits = bxlen * 9 + 1;
  } else {
    // if b->xlen is a multiple of nine (9, 18...)
    int mod = b->digits % 9;
    ndigits = bxlen * 9 + mod - 9 * (mod != 0);
  }
  retval->xlen = xlen;
  retval->x = x;
  retval->digits = ndigits;
  retval->positive = a->positive;

  return retval;
}

bigint* bi_sub(const bigint *a, const bigint *b) {
  // One operand is NULL
  if (!(a && b))
    return NULL;

  bigint* retval;

  // One operand is bigint zero
  if (bi_is_zero(a)) {
    retval = bi_copy(b);
    retval->positive = !retval->positive;
    return retval;
  }
  if (bi_is_zero(b))
    return bi_copy(a);

  // Cannot allocate memory for bigint
  retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;

  // if opposite sign then calculate using addition
  if (bi_opposite_sign(a, b)) {
    retval->digits = b->digits;
    retval->xlen = b->xlen;
    retval->x = b->x;
    retval->positive = !(b->positive);
    bigint* res = bi_add(a, retval);
    free(retval);
    return res;
  }

  bool swapped = false;
  if (!a->positive) {
    const bigint* tmp = a;
    a = b;
    b = tmp;
  }

  if (bi_cmp(a, b) < 0) {
    const bigint* tmp = a;
    a = b;
    b = tmp;
    swapped = true;
  }

  int axlen = a->xlen;
  int bxlen = b->xlen;
  int* ax = a->x;
  int* bx = b->x;
  int xlen = axlen > bxlen ? axlen : bxlen;

  int* x = malloc(xlen * sizeof(int));
  if (!x) {
    free(retval);
    return NULL;
  }

  int ia = 0;
  int ib = 0;
  int diff = 0;
  bool zero = true; // is the two operand the same
  int firstoctet = 0;
  if (axlen == 1) {
    diff = ax[0] - bx[0];
    x[0] = diff;
    zero = diff == 0;
  } else {
    // axlen, bxlen must be at least 1 now
    while (ia < axlen || ib < bxlen) {
      if (ia < axlen) {
        diff += ax[ia];
        ++ia;
      }
      if (ib < bxlen) {
        diff -= bx[ib];
        ++ib;
      }

      if (diff < 0) {
        x[ia - 1] = diff + BASE;
        diff = -1;
      } else {
        x[ia - 1] = diff % BASE;
        diff /= BASE;
      }
      zero = zero && !x[ia-1];
      if (x[ia - 1])
        firstoctet = ia - 1;
    }
  }

  if (zero) {
    free(x);
    retval->x = NULL;
  } else
    retval->x = x;

  // TODO: if x is over allocated, resize it
  if (x[firstoctet] < 0)
    x[0] = -x[0];
  // swapped,d >= 0,positive
  // 0 0 0
  // 0 1 1
  // 1 0 1
  // 1 1 0
  retval->positive = (diff >= 0) ^ swapped;
  int ndigits = 9 * firstoctet;
  for (int t = x[firstoctet]; t >= 1; t /= 10)
    ++ndigits;

  retval->xlen = firstoctet + 1;
  retval->digits = ndigits;

  return retval;
}

bigint* bi_mul(const bigint *a, const bigint *b) {
  // One operand is NULL
  if (!(a && b))
    return NULL;


  // One operand is bigint zero
  if (bi_is_zero(a) || bi_is_zero(b))
    return bi_zero();

  bigint* retval;
  int* xa = a->x;
  int* xb = b->x;
  int alen = a->xlen;
  int blen = b->xlen;

  // One operand is bigint one or minus one
  if (a->x[0] == 1 && a->xlen == 1) {
    retval = bi_copy(b);
    retval->positive = !(a->positive ^ b->positive);
    return retval;
  }

  if (b->x[0] == 1 && b->xlen == 1) {
    retval = bi_copy(a);
    retval->positive = !(a->positive ^ b->positive);
    return retval;
  }

  // Cannot allocate memory for bigint
  retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;

  int xlen = (a->digits + b->digits + 8) / 9;
  int* x = calloc(xlen, sizeof(int));
  if (!x) {
    free(retval);
    return NULL;
  }

  // Long multiplication
  long product;
  for (int ib = 0; ib < blen; ib++) {
    product = 0L;
    for (int ia = 0; ia < alen; ia++) {
      product += (long)xa[ia] * (long)xb[ib] + (long)x[ia + ib];
      x[ia + ib] = (int)(product % BASE);
      product /= BASE;
    }
    x[ib + alen] += product;
  }

  // find xlen and digits
  int lastnonzeroindex = xlen - 1;
  while (x[lastnonzeroindex] == 0)
    lastnonzeroindex--;
  int ndigits = 9 * lastnonzeroindex;
  for (int t = x[lastnonzeroindex]; t >= 1; t /= 10)
    ++ndigits;

  retval->positive = !(a->positive ^ b->positive);
  retval->digits = ndigits;
  retval->xlen = lastnonzeroindex + 1;
  retval->x = x;
  return retval;
}

bigint* bi_div(const bigint *a, const bigint *b) {
  return NULL;
}

bigint* bi_factorial(const bigint *a) {
  if (!a)
    return NULL;

  bigint* one = bi_fromstring("1");

  if (bi_is_zero(a))
    return one;

  if (!a->positive) {
    bi_delete(one);
    return NULL;
  }

  bigint* newretval;
  bigint* newprev;
  bigint* retval = bi_copy(one);
  bigint* prev = bi_copy(a);

  if (!retval)
    return NULL;

  // TODO: is it better if we use mutable bigint here?

  while (!bi_is_zero(prev)) {
    newretval = bi_mul(retval, prev);
    bi_delete(retval);
    if (!retval)
      return NULL;
    retval = newretval;

    newprev = bi_sub(prev, one);
    bi_delete(prev);
    if (!prev)
      return NULL;
    prev = newprev;
  }

  bi_delete(prev);

  return retval;
}

void bi_print(const bigint* a) {
  if (!a) {
    puts("NULL");
    return;
  }

  if (!(a->x)) {
    puts("0");
    return;
  }

  int xlen = a->xlen;
  int* x = a->x;
  char sign = a->positive ? ' ' : '-';

  printf("xlen,digits: %4d, %4d, %c", xlen, a->digits, sign);
  printf("%d ", x[xlen-1]);
  for (int i = xlen - 2; i >= 0; --i)
    printf("%09d ", x[i]);
  printf("\n");
}

int bi_cmp(const bigint *a, const bigint *b) {
  // same bigint
  if (a == b)
    return 0;

  // one of two operands is null
  if (a == NULL)
    return -1;
  if (b == NULL)
    return 1;

  int adigits = a->digits;
  int bdigits = b->digits;
  bool apositive = a->positive;
  int* ax = a->x;
  int* bx = b->x;

  // one of two operands is zero
  if (ax == NULL) {
    if (bx == NULL)
      return 0;
    if (b->positive)
      return -1;
    return 1;
  }

  // opposite sign
  if (apositive ^ b->positive)
    return (apositive) ? 1 : -1;

  // same sign

  // different number of digits
  if (adigits < bdigits)
    return apositive ? -1 : 1;
  if (adigits > bdigits)
    return apositive ? 1 : -1;

  // same number of digits
  for (int i = a->xlen - 1; i >= 0; --i) {
    if (ax[i] < bx[i])
      return apositive ? -1 : 1;
    else if (ax[i] > bx[i])
      return apositive ? 1 : -1;
  }

  return 0;
}

bool bi_equal(const bigint *a, const bigint *b) {
  return bi_cmp(a, b) == 0;
}

bigint* bi_copy(const bigint* a) {
  if (!a)
    return NULL;

  bigint* retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;

  int xlen = a->xlen;

  if (a->x == NULL) {
    retval->xlen = 0;
    retval->digits = 0;
    retval->x = NULL;
    retval->positive = true;
    return retval;
  }

  int* x = malloc(xlen * sizeof(int));
  if (!x) {
    free(retval);
    return NULL;
  }

  memcpy(x, a->x, xlen * sizeof(int));
  retval->xlen = xlen;
  retval->digits = a->digits;
  retval->positive = a->positive;
  retval->x = x;

  return retval;
}

inline bool bi_is_zero(const bigint* a) {
  return (a != NULL && a->x == NULL);
}

inline bool bi_is_one(const bigint* a) {
  return (a != NULL && a->xlen == 1 && a->positive && a->x[0] == 1);
}

inline bool bi_is_minus_one(const bigint* a) {
  return (a != NULL && a->xlen == 1 && !a->positive && a->x[0] == 1);
}

inline int bi_opposite_sign(const bigint* a, const bigint * b) {
  return a->positive ^ b->positive;
}

bigint* bi_zero() {
  bigint* retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;
  retval->x = NULL;
  retval->positive = true;
  return retval;
}

bigint* bi_negate(const bigint* a) {
  if (!a)
    return NULL;

  bigint* retval = malloc(sizeof(bigint));
  if (!retval)
    return NULL;

  if (bi_is_zero(a)) {
    retval->x = NULL;
    retval->positive = true;
    return retval;
  }

  int *x = malloc(a->xlen * sizeof(int));
  if (!x) {
    free(retval);
    return NULL;
  }

  memcpy(x, a->x, a->xlen * sizeof(int));
  retval->x = x;
  retval->xlen = a->xlen;
  retval->digits = a->digits;
  retval->positive = !a->positive;
  return retval;
}
