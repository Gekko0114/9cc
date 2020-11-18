#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
}

assert 0 '0;'
assert 42 '42;'
assert 2 '1+1;'
assert 51 '5-21+67;'
assert 233 '10 - 34 + 1;'
assert 7 '1 + 1 * (3 + 3);'
assert 3 '(3 + 4)/2;'
assert 253 '-3;'
assert 7 '+7;'
assert 200 '-10*(-20);'

assert 0 '0 == 1;'
assert 1 '1 == 1;'
assert 0 '5 != 5;'
assert 1 '5 != 4;'
assert 0 '40 < 1;'
assert 1 '3 < 10;'
assert 0 '5 > 53;'
assert 1 '7 > 3;'
assert 0 '7 <= 3;'
assert 1 '3 <= 3;'
assert 1 '3 <= 7;'
assert 0 '3 >= 5;'
assert 1 '3 >= 3;'
assert 1 '5 >= 3;'

assert 29 'a=3+4;b=5*6-8;a+b;'
echo OK