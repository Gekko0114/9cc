#!/bin/bash

cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() {return 3;}
int ret5() {return 5;}
EOF

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -static -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
}

assert 3 'return ret3();'
assert 5 'return ret5();'

assert 0 'return 0;'
assert 42 'return 42;'
assert 2 'return 1+1;'
assert 51 'return 5-21+67;'
assert 233 'return 10 - 34 + 1;'
assert 7 'return 1 + 1 * (3 + 3);'
assert 3 'return (3 + 4)/2;'
assert 253 'return -3;'
assert 7 'return +7;'
assert 200 'return -10*(-20);'

assert 0 'return 0 == 1;'
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
assert 8 'foo=3;bar=5;foo+bar;'
assert 58 'foo = 3 * 20 - (6 / 2); bar = 45 >= 10; foo + bar;'

assert 5 'foo = 5;return foo;'
assert 1 'aaa = 1;return aaa;aaa + 1;'
assert 1 'aaa = 0; return aaa*aaa+1;'
echo if
assert 1 'if (10 >= 5) { return 1; }'
assert 1 'if (10 >= 5) { return 1; } else { return 2; }'
assert 1 'if (10 >= 5) { 1; } else { 2; }'
assert 2 'foo = 10; if (foo < 5) { bar = 1; } else { bar = 2; } return bar;'
echo While
assert 10 'count = 0; while (count < 10) { count = count + 1; }; count;'
echo For
assert 43 'count = 0; for (i = 0; i < 43; i = i + 1) { count = count + 1; }; count;'
echo OK