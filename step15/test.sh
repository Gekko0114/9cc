#!/bin/bash

cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() {return 3;}
int ret5() {return 5;}
int add(int x, int y) {return x + y; }
int sub(int x, int y) {return x - y; }
int add6(int a, int b, int c, int d, int e, int f) {
  return a + b + c + d + e + f;
}
EOF

assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s tmp2.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
}

assert 3 'main(){return ret3();}'
assert 5 'main(){return ret5();}'
assert 4 'main(){return add(1, 3);}'
assert 6 'main(){return sub(10, 4);}'
assert 21 'main(){return add6(1, 2, 3, 4, 5, 6);}'

assert 0 'main(){return 0;}'
assert 42 'main(){return 42;}'
assert 2 'main(){return 1+1;}'
assert 51 'main(){return 5-21+67;}'
assert 233 'main(){return 10 - 34 + 1;}'
assert 7 'main(){return 1 + 1 * (3 + 3);}'
assert 3 'main(){return (3 + 4)/2;}'
assert 253 'main(){return -3;}'
assert 7 'main(){return +7;}'
assert 200 'main(){return -10*(-20);}'

assert 0 'main(){return 0 == 1;}'
assert 1 'main(){1 == 1;}'
assert 0 'main(){5 != 5;}'
assert 1 'main(){5 != 4;}'
assert 0 'main(){40 < 1;}'
assert 1 'main(){3 < 10;}'
assert 0 'main(){5 > 53;}'
assert 1 'main(){7 > 3;}'
assert 0 'main(){7 <= 3;}'
assert 1 'main(){3 <= 3;}'
assert 1 'main(){3 <= 7;}'
assert 0 'main(){3 >= 5;}'
assert 1 'main(){3 >= 3;}'
assert 1 'main(){5 >= 3;}'

assert 29 'main(){a=3+4;b=5*6-8;a+b;}'
assert 8 'main(){foo=3;bar=5;foo+bar;}'
assert 58 'main(){foo = 3 * 20 - (6 / 2); bar = 45 >= 10; foo + bar;}'

assert 5 'main(){foo = 5;return foo;}'
assert 1 'main(){aaa = 1;return aaa;aaa + 1;}'
assert 1 'main(){aaa = 0; return aaa*aaa+1;}'
echo if
assert 1 'main(){if (10 >= 5) { return 1; }}'
assert 1 'main(){if (10 >= 5) { return 1; } else { return 2; }}'
assert 1 'main(){if (10 >= 5) { 1; } else { 2; }}'
assert 2 'main(){foo = 10; if (foo < 5) { bar = 1; } else { bar = 2; } return bar;}'
echo While
assert 10 'main(){count = 0; while (count < 10) { count = count + 1; }; count;}'
echo For
assert 43 'main(){count = 0; for (i = 0; i < 43; i = i + 1) { count = count + 1; }; count;}'
echo OK