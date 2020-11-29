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

assert 3 'int main(){return ret3();}'
assert 5 'int main(){return ret5();}'
assert 4 'int main(){return add(1, 3);}'
assert 6 'int main(){return sub(10, 4);}'
assert 21 'int main(){return add6(1, 2, 3, 4, 5, 6);}'

assert 0 'int main(){return 0;}'
assert 42 'int main(){return 42;}'
assert 2 'int main(){return 1+1;}'
assert 51 'int main(){return 5-21+67;}'
assert 233 'int main(){return 10 - 34 + 1;}'
assert 7 'int main(){return 1 + 1 * (3 + 3);}'
assert 3 'int main(){return (3 + 4)/2;}'
assert 253 'int main(){return -3;}'
assert 7 'int main(){return +7;}'
assert 200 'int main(){return -10*(-20);}'

assert 0 'int main(){return 0 == 1;}'
assert 1 'int main(){1 == 1;}'
assert 0 'int main(){5 != 5;}'
assert 1 'int main(){5 != 4;}'
assert 0 'int main(){40 < 1;}'
assert 1 'int main(){3 < 10;}'
assert 0 'int main(){5 > 53;}'
assert 1 'int main(){7 > 3;}'
assert 0 'int main(){7 <= 3;}'
assert 1 'int main(){3 <= 3;}'
assert 1 'int main(){3 <= 7;}'
assert 0 'int main(){3 >= 5;}'
assert 1 'int main(){3 >= 3;}'
assert 1 'int main(){5 >= 3;}'

assert 29 'int main() { int a = 3 + 4; int b = 5 * 6 - 8; return a+b; }'
assert 8 'int main(){int foo=3;int bar=5;foo+bar;}'
assert 58 'int main(){int foo = 3 * 20 - (6 / 2); int bar = 45 >= 10; foo + bar;}'

assert 5 'int main(){int foo = 5;return foo;}'
assert 1 'int main(){int aaa = 1;return aaa;aaa + 1;}'
assert 1 'int main(){int aaa = 0; return aaa*aaa+1;}'
echo if
assert 1 'int main(){if (10 >= 5) { return 1; }}'
assert 1 'int main(){if (10 >= 5) { return 1; } else { return 2; }}'
assert 1 'int main(){if (10 >= 5) { 1; } else { 2; }}'
assert 2 'int main(){int foo = 10; if (foo < 5) { int bar = 1; } else { int bar = 2; } return bar;}'
echo While
assert 10 'int main(){int count = 0; while (count < 10) { count = count + 1; } count;}'
echo For
assert 43 'int main(){int count = 0; int i = 0; for (i = 0; i < 43; i = i + 1) { count = count + 1; } count;}'

echo Pointer
assert 3 'int main() { int x = 3; return *&x; }'
assert 5 'int main() { int x = 5; int y = &x; int z = &y; return **z; }'
assert 7 'int main() { int x = 4; int y = 7; return *(&x + 1); }'
assert 3 'int main() { int x=3; int y=5; return *(&y-1); }'
assert 5 'int main() { int x=3; int y=&x; *y=5; return x; }'
assert 7 'int main() { int x=3; int y=5; *(&x+1)=7; return y; }'
assert 7 'int main() { int x=3; int y=5; *(&y-1)=7; return x; }'
assert 2 'int main() { int x=3; return (&x+2)-&x; }'

assert 7 'int main() {return add2(3, 4);} int add2(int x, int y){return x+y;}'
assert 1 'int main() {return sub2(4, 3);} int sub2(int x, int y){return x-y;}'
assert 55 'int main() {return fib(9);} int fib(int x){if (x<=1) return 1; return fib(x-1)+fib(x-2);}'
echo OK