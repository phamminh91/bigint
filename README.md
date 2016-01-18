An immutable big integer arithmetic library in C
------------------------------------------------

## Feature:
- base 10<sup>9</sup> implementation

## API
- `bi_fromstring(const char *)`
- `bi_add(const bigint *, const bigint *)`
- `bi_sub(const bigint *, const bigint *)`
- `bi_mult(const bigint *, const bigint *)`
- `bi_div(const bigint *, const bigint *)`
- `bi_factorial(const bigint *)`
- `bi_negate(const bigint *)`
- `bi_cmp(const bigint *, const bigint *)`
- `bi_equal(const bigint *, const bigint *)`
- `bi_is_zero(const bigint *, const bigint *)`
- `bi_is_one(const bigint *, const bigint *)`
- `bi_is_minus_one(const bigint *, const bigint *)`
- `bi_delete(const bigint *)`
- `bi_print(const bigint *)`

## Known bugs:
- cannot compile with clang `-O2`

