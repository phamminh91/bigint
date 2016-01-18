#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "bigint.h"

void test_bi_leading_zero();
void test_bi_representation();
void test_bi_cmp();
void test_bi_add();
void test_bi_sub();
void test_bi_mult();
void test_bi_div();
void test_bi_factorial();
void test_bi_julia();
void test_bi_julia_integrated();
void bi_assert(bigint* expected, bigint* actual);

int main() {
  test_bi_representation();
  test_bi_cmp();
  test_bi_add();
  test_bi_sub();
  test_bi_mult();
  test_bi_div();
  test_bi_factorial();
  
  test_bi_julia();
  test_bi_julia_integrated();
  
  return 0;
}

void bi_assert(bigint* expected, bigint* actual) {
  int cmp = bi_cmp(expected, actual);
  if (cmp != 0) {
    bi_print(actual);
    bi_print(expected);
    assert(false);
  }
}

void test_bi_representation() {
  bigint* a;
  
  // zero
  a = bi_fromstring("0");
  assert (a->xlen == 0);
  assert (a->digits == 0);
  assert (a->x == NULL);
  bi_delete(a);
  
  // one
  a = bi_fromstring("1");
  assert (a->xlen == 1);
  assert (a->digits == 1);
  assert (a->x != NULL);
  assert (a->x[0] == 1U);
  bi_delete(a);
  
  // 1 octet
  a = bi_fromstring("1234");
  assert (a->xlen == 1);
  assert (a->digits == 4);
  assert (a->x != NULL);
  assert (a->x[0] == 1234U);
  bi_delete(a);
  
  a = bi_fromstring("123456789");
  assert (a->xlen == 1);
  assert (a->digits == 9);
  assert (a->x != NULL);
  assert (a->x[0] == 123456789U);
  bi_delete(a);
  
  // 2 octet
  a = bi_fromstring("1234567890");
  assert (a->xlen == 2);
  assert (a->digits == 10);
  assert (a->x != NULL);
  assert (a->x[0] == 234567890U);
  assert (a->x[1] == 1U);
  bi_delete(a);
  
  
  // multiple octets (with leading zero at each octet)
  a = bi_fromstring("11023456789023456789023456789");
  assert (a->xlen == 4);
  assert (a->digits == 29);
  assert (a->x[0] == 23456789U);
  assert (a->x[1] == 23456789U);
  assert (a->x[2] == 23456789U);
  assert (a->x[3] == 11U);
  bi_delete(a);
  
  // large input
  a = bi_fromstring("123456789123456789123456789123456789123456789123456789");
  assert (a->xlen == 6);
  assert (a->digits == 54);
  assert (a->x != NULL);
  bi_delete(a);
  
  // leading zero
  test_bi_leading_zero();
  
  // negative
  a = bi_fromstring("-1");
  assert (a->xlen == 1);
  assert (a->digits == 1);
  assert (a->positive == 0);
  assert (a->x != NULL);
  assert (a->x[0] == 1U);
  bi_delete(a);
  
  puts("test_bi_representation: OK");
}

void test_bi_leading_zero() {
  bigint* a;
  a = bi_fromstring("010");
  assert (a->xlen == 1);
  assert (a->digits == 2);
  assert (a->x != NULL);
  assert (a->x[0] == 10U);
  bi_delete(a);
  
  a = bi_fromstring("0000000010");
  assert (a->xlen == 1);
  assert (a->digits == 2);
  assert (a->x != NULL);
  assert (a->x[0] == 10U);
  bi_delete(a);
  
  a = bi_fromstring("00");
  assert (a->xlen == 0);
  assert (a->digits == 0);
  assert (a->x == NULL);
  bi_delete(a);
  
  a = bi_fromstring("0000000000");
  assert (a->xlen == 0);
  assert (a->digits == 0);
  assert (a->x == NULL);
  bi_delete(a);
  
  
  a = bi_fromstring("0000000000000000100000000000");
  assert (a->xlen == 2);
  assert (a->digits == 12);
  assert (a->x != NULL);
  assert (a->x[0] == 0U);
  assert (a->x[1] == 100U);
  bi_delete(a);
}

void test_bi_cmp() {
  bigint* a;
  bigint* b;
  
  // zero
  a = bi_fromstring("0");
  b = bi_fromstring("0");
  assert (bi_cmp(a, b) == 0);
  bi_delete(a);
  bi_delete(b);
  
  a = bi_fromstring("0");
  b = bi_fromstring("1");
  assert (bi_cmp(a, b) == -1);
  assert (bi_cmp(b, a) == 1);
  bi_delete(a);
  bi_delete(b);
  
  // one octet
  a = bi_fromstring("12");
  b = bi_fromstring("123");
  assert (bi_cmp(a, b) == -1);
  bi_delete(a);
  bi_delete(b);
  
  a = bi_fromstring("12");
  b = bi_fromstring("1");
  assert (bi_cmp(a, b) == 1);
  bi_delete(a);
  bi_delete(b);
  
  a = bi_fromstring("123456789");
  b = bi_fromstring("123456788");
  assert (bi_cmp(a, b) == 1);
  bi_delete(a);
  bi_delete(b);
  
  // TODO: two octet
  a = bi_fromstring("123456789123456789");
  b = bi_fromstring("12345678123456789");
  assert (bi_cmp(a, b) == 1);
  bi_delete(a);
  bi_delete(b);
  
  // TODO: negative
  
  // large input
  a = bi_fromstring("123456789123456789123456789123456789123456789123456789");
  b = bi_fromstring("123456789123456789123456889123456789123456789123456789");
  assert (bi_cmp(a, b) == -1);
  bi_delete(a);
  bi_delete(b);
  
  puts("test_bi_cmp: OK");
}

void test_bi_add() {
  bigint* a;
  bigint* b;
  bigint* c;
  bigint* expected;
  
  // zero
  a = bi_fromstring("1");
  b = bi_fromstring("0");
  c = bi_add(a, b);
  expected = bi_fromstring("1");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("1234567890");
  b = bi_fromstring("0");
  c = bi_add(a, b);
  expected = bi_fromstring("1234567890");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // number of digits increases
  a = bi_fromstring("1");
  b = bi_fromstring("9");
  c = bi_add(a, b);
  expected = bi_fromstring("10");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // no carry to next octet
  a = bi_fromstring("1111");
  b = bi_fromstring("2222");
  c = bi_add(a, b);
  expected = bi_fromstring("3333");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("11111");
  b = bi_fromstring("2222");
  c = bi_add(a, b);
  expected = bi_fromstring("13333");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("111111999");
  b = bi_fromstring("1");
  c = bi_add(a, b);
  expected = bi_fromstring("111112000");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // carry to next octet
  a = bi_fromstring("999999999");
  b = bi_fromstring("1");
  c = bi_add(a, b);
  expected = bi_fromstring("1000000000");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("999999999999999999");
  b = bi_fromstring("1");
  c = bi_add(a, b);
  expected = bi_fromstring("1000000000000000000");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // negative
  a = bi_fromstring("-1");
  b = bi_fromstring("-1");
  c = bi_add(a, b);
  expected = bi_fromstring("-2");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("-1");
  b = bi_fromstring("-9");
  c = bi_add(a, b);
  expected = bi_fromstring("-10");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("-999999999");
  b = bi_fromstring("-1");
  c = bi_add(a, b);
  expected = bi_fromstring("-1000000000");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // large input
  a = bi_fromstring("123456789123456789123456789123456789123456789123456789");
  b = bi_fromstring("1");
  c = bi_add(a, b);
  expected = bi_fromstring("123456789123456789123456789123456789123456789123456790");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("999999999999999999999999999999999999999999999");
  b = bi_fromstring("1");
  c = bi_add(a, b);
  expected = bi_fromstring("1000000000000000000000000000000000000000000000");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // fuzz
  a = bi_fromstring(       "234871908243719870132147912847109428731");
  b = bi_fromstring(                     "1284190248910284190432840");
  c = bi_add(a, b);
  expected = bi_fromstring("234871908243721154322396823131299861571");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  puts("test_bi_add: OK");
}

void test_bi_sub() {
  bigint* a;
  bigint* b;
  bigint* c;
  bigint* expected;
  
  // zero
  a = bi_fromstring("1");
  b = bi_fromstring("0");
  c = bi_sub(a, b);
  expected = bi_fromstring("1");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // zero
  a = bi_fromstring("0");
  b = bi_fromstring("1");
  c = bi_sub(a, b);
  expected = bi_fromstring("-1");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("0");
  b = bi_fromstring("1234567890");
  c = bi_sub(a, b);
  expected = bi_fromstring("-1234567890");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // normal
  a = bi_fromstring("123");
  b = bi_fromstring("11");
  c = bi_sub(a, b);
  expected = bi_fromstring("112");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring( "11");
  b = bi_fromstring("123");
  c = bi_sub(a, b);
  expected = bi_fromstring("-112");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // number of digits decreases
  a = bi_fromstring("10");
  b = bi_fromstring("1");
  c = bi_sub(a, b);
  expected = bi_fromstring("9");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("1");
  b = bi_fromstring("10");
  c = bi_sub(a, b);
  expected = bi_fromstring("-9");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // carry to next octet
  a = bi_fromstring("1000000000");
  b = bi_fromstring("1");
  c = bi_sub(a, b);
  expected = bi_fromstring("999999999");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // a = bi_fromstring("11111");
  // b = bi_fromstring("2222");
  // c = bi_add(a, b);
  // expected = bi_fromstring("13333");
  // bi_assert(bi_cmp(c, expected) == 0);
  // bi_delete(a);
  // bi_delete(b);
  // bi_delete(c);
  // bi_delete(expected);
  
  // a = bi_fromstring("111111999");
  // b = bi_fromstring("1");
  // c = bi_add(a, b);
  // expected = bi_fromstring("111112000");
  // bi_assert(bi_cmp(c, expected) == 0);
  // bi_delete(a);
  // bi_delete(b);
  // bi_delete(c);
  // bi_delete(expected);
  
  // // carry to next octet
  // a = bi_fromstring("999999999");
  // b = bi_fromstring("1");
  // c = bi_add(a, b);
  // expected = bi_fromstring("1000000000");
  // bi_assert(bi_cmp(c, expected) == 0);
  // bi_delete(a);
  // bi_delete(b);
  // bi_delete(c);
  // bi_delete(expected);
  
  // a = bi_fromstring("999999999999999999");
  // b = bi_fromstring("1");
  // c = bi_add(a, b);
  // expected = bi_fromstring("1000000000000000000");
  // bi_assert(bi_cmp(c, expected) == 0);
  // bi_delete(a);
  // bi_delete(b);
  // bi_delete(c);
  // bi_delete(expected);
  
  // negative
  a = bi_fromstring("-1");
  b = bi_fromstring("-1");
  c = bi_sub(a, b);
  expected = bi_fromstring("0");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("-1");
  b = bi_fromstring("-9");
  c = bi_sub(a, b);
  expected = bi_fromstring("8");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("-999999999");
  b = bi_fromstring("-1");
  c = bi_sub(a, b);
  expected = bi_fromstring("-999999998");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // large input
  a = bi_fromstring("123456789123456789123456789123456789123456789123456789");
  b = bi_fromstring("1");
  c = bi_sub(a, b);
  expected = bi_fromstring("123456789123456789123456789123456789123456789123456788");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("1000000000000000000000000000000000000000000000");
  b = bi_fromstring("1");
  c = bi_sub(a, b);
  expected = bi_fromstring("999999999999999999999999999999999999999999999");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // fuzz
  a = bi_fromstring(       "234871908243719870132147912847109428731");
  b = bi_fromstring(                     "1284190248910284190432840");
  c = bi_sub(a, b);
  expected = bi_fromstring("234871908243718585941899002562918995891");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring(                     "1284190248910284190432840");
  b = bi_fromstring(       "234871908243719870132147912847109428731");
  c = bi_sub(a, b);
  expected = bi_fromstring("-234871908243718585941899002562918995891");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  puts("test_bi_sub: OK");
}

void test_bi_mult() {
  bigint* a;
  bigint* b;
  bigint* c;
  bigint* expected;
  
  // multiply by zero
  a = bi_fromstring("3");
  b = bi_fromstring("0");
  c = bi_mult(a, b);
  expected = bi_fromstring("0");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("0");
  b = bi_fromstring("3");
  c = bi_mult(a, b);
  expected = bi_fromstring("0");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // multiply by one
  a = bi_fromstring("3");
  b = bi_fromstring("1");
  c = bi_mult(a, b);
  expected = bi_fromstring("3");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("1");
  b = bi_fromstring("3");
  c = bi_mult(a, b);
  expected = bi_fromstring("3");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // multiply by minus one
  a = bi_fromstring("3");
  b = bi_fromstring("-1");
  c = bi_mult(a, b);
  expected = bi_fromstring("-3");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("-1");
  b = bi_fromstring("3");
  c = bi_mult(a, b);
  expected = bi_fromstring("-3");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // one octet
  a = bi_fromstring("12");
  b = bi_fromstring("1000");
  c = bi_mult(a, b);
  expected = bi_fromstring("12000");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("333333333");
  b = bi_fromstring("3");
  c = bi_mult(a, b);
  expected = bi_fromstring("999999999");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // two octets, no carry to next octet
  a = bi_fromstring("222222222333333333");
  b = bi_fromstring("3");
  c = bi_mult(a, b);
  expected = bi_fromstring("666666666999999999");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  // fuzz
  a = bi_fromstring("21347912471890274120984");
  b = bi_fromstring("7149018249021341907091234");
  c = bi_mult(a, b);
  expected = bi_fromstring("152616615840053874381093927627970984545641854256");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  a = bi_fromstring("1234567890123456782");
  b = bi_fromstring("47");
  c = bi_mult(a, b);
  expected = bi_fromstring("58024690835802468754");
  bi_assert(expected, c);
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(expected);
  
  puts("test_bi_mult: OK");
}

void test_bi_div() {
  puts("test_bi_div: OK");
}

void test_bi_factorial() {
  bigint* a;
  bigint* b;
  bigint* expected;
  
  a = bi_fromstring("5");
  b = bi_factorial(a);
  expected = bi_fromstring("120");
  bi_assert(expected, b);
  bi_delete(a);
  bi_delete(b);
  bi_delete(expected);
  
  a = bi_fromstring("30");
  b = bi_factorial(a);
  expected = bi_fromstring("265252859812191058636308480000000");
  bi_assert(expected, b);
  bi_delete(a);
  bi_delete(b);
  bi_delete(expected);
  
  puts("test_bi_factorial: OK");
}

/* Tests below are from http://www.mit.edu/afs.new/athena/software/julia_v0.3/julia/test/bigint.jl */
void test_bi_julia() {
  bigint* a = bi_fromstring("123456789012345678901234567890");
  bigint* b = bi_fromstring("123456789012345678901234567891");
  bigint* c = bi_fromstring("246913578024691357802469135780");
  bigint* d = bi_fromstring("-246913578024691357802469135780");
  bigint* one = bi_fromstring("1");
  bigint* two = bi_fromstring("2");
  
  bigint* z;
  bigint* y;
  
  // a+1 == b
  z = bi_add(a, one);
  bi_assert(b, z);
  bi_delete(z);
  
  // b != a
  assert(bi_cmp(a, b) != 0);
  
  // b > a
  assert(bi_cmp(b, a) > 0);
  
  // b > a
  assert(!(bi_cmp(b, a) < 0));
  
  // c == a*2
  z = bi_mult(a, two);
  bi_assert(c, z);
  bi_delete(z);
  
  // c-a == a
  z = bi_sub(c, a);
  bi_assert(a, z);
  bi_delete(z);
  
  // c == a+a
  z = bi_add(a, a);
  bi_assert(c, z);
  bi_delete(z);
  
  // c+1 == a+b
  y = bi_add(c, one);
  z = bi_add(a, b);
  bi_assert(z, y);
  bi_delete(y);
  bi_delete(z);
  
  // d == -c
  bigint* negc = bi_negate(c);
  bi_assert(negc, d);
  bi_delete(negc);
  
  // factorial 40
  bigint* expected = bi_fromstring("815915283247897734345611269596115894272000000000");
  bigint* forty = bi_fromstring("40");
  z = bi_factorial(forty);
  bi_assert(expected, z);
  bi_delete(z);
  bi_delete(expected);
  bi_delete(forty);
  
  // clean up
  bi_delete(a);
  bi_delete(b);
  bi_delete(c);
  bi_delete(d);
  bi_delete(one);
  bi_delete(two);
}

void test_bi_julia_integrated() {
  // integrated arithmetic
  bigint* a = bi_fromstring("315135");
  bigint* b = bi_fromstring("12412");
  bigint* c = bi_fromstring("3426495623485904783478347");
  bigint* d = bi_fromstring("-1398984130");
  bigint* f = bi_fromstring("2413804710837418037418307081437315263635345357386985747464");
  bigint* g = bi_fromstring("-1");
  bigint* z;
  bigint* z1;
  bigint* z2;
  bigint* z3;
  bigint* z4;
  bigint* expected;
  
  //  @test +(a, b) == BigInt("327547")
  z = bi_add(a, b);
  expected = bi_fromstring("327547");
  bi_assert(expected, z);
  bi_delete(expected);
  bi_delete(z);
  
  //  @test +(a, b, c) == BigInt("3426495623485904783805894")
  z = bi_add(a, b);
  z1 = bi_add(z, c);
  expected = bi_fromstring("3426495623485904783805894");
  bi_assert(expected, z1);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(expected);
  
  //  @test +(a, b, c, d) == BigInt("3426495623485903384821764")
  z = bi_add(a, b);
  z1 = bi_add(z, c);
  z2 = bi_add(z1, d);
  expected = bi_fromstring("3426495623485903384821764");
  bi_assert(expected, z2);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(z2);
  bi_delete(expected);
  
  //  @test +(a, b, c, d, f) == BigInt("2413804710837418037418307081437318690130968843290370569228")
  z = bi_add(a, b);
  z1 = bi_add(z, c);
  z2 = bi_add(z1, d);
  z3 = bi_add(z2, f);
  expected = bi_fromstring("2413804710837418037418307081437318690130968843290370569228");
  bi_assert(expected, z3);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(z2);
  bi_delete(z3);
  bi_delete(expected);
  
  //  @test +(a, b, c, d, f, g) == BigInt("2413804710837418037418307081437318690130968843290370569227")
  z = bi_add(a, b);
  z1 = bi_add(z, c);
  z2 = bi_add(z1, d);
  z3 = bi_add(z2, f);
  z4 = bi_add(z3, g);
  expected = bi_fromstring("2413804710837418037418307081437318690130968843290370569227");
  bi_assert(expected, z4);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(z2);
  bi_delete(z3);
  bi_delete(z4);
  bi_delete(expected);
  
  //  @test *(a, b) == BigInt("3911455620")
  z = bi_mult(a, b);
  expected = bi_fromstring("3911455620");
  bi_assert(expected, z);
  bi_delete(expected);
  bi_delete(z);
  
  //  @test *(a, b, c) == BigInt("13402585563389346256121263521460140")
  z = bi_mult(a, b);
  z1 = bi_mult(z, c);
  expected = bi_fromstring("13402585563389346256121263521460140");
  bi_assert(expected, z1);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(expected);
  
  //  @test *(a, b, c, d) == BigInt("-18750004504148804423388563022070650287578200")
  z = bi_mult(a, b);
  z1 = bi_mult(z, c);
  z2 = bi_mult(z1, d);
  expected = bi_fromstring("-18750004504148804423388563022070650287578200");
  bi_assert(expected, z2);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(z2);
  bi_delete(expected);
  
  //  @test *(a, b, c, d, f) == BigInt("-45258849200337190631492857400003938881995610529251881450243326128168934937055005474972396281351684800")
  z = bi_mult(a, b);
  z1 = bi_mult(z, c);
  z2 = bi_mult(z1, d);
  z3 = bi_mult(z2, f);
  expected = bi_fromstring("-45258849200337190631492857400003938881995610529251881450243326128168934937055005474972396281351684800");
  bi_assert(expected, z3);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(z2);
  bi_delete(z3);
  bi_delete(expected);
  
  //  @test *(a, b, c, d, f, g) == BigInt("45258849200337190631492857400003938881995610529251881450243326128168934937055005474972396281351684800")
  z = bi_mult(a, b);
  z1 = bi_mult(z, c);
  z2 = bi_mult(z1, d);
  z3 = bi_mult(z2, f);
  z4 = bi_mult(z3, g);
  expected = bi_fromstring("45258849200337190631492857400003938881995610529251881450243326128168934937055005474972396281351684800");
  bi_assert(expected, z4);
  bi_delete(z);
  bi_delete(z1);
  bi_delete(z2);
  bi_delete(z3);
  bi_delete(z4);
  bi_delete(expected);
}
