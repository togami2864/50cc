#!/bin/bash

assert(){
    expected="$1"
    input="$2"

    ./main "$input" > tmp.s
    cd func
    cc -c func.c
    cd ..
    cc -o tmp tmp.s func/func.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
}

# define variables with type "int"
assert 5 "int main() {
  int x;
  x = 5;
  return x;
}"

assert 3 "int main() {
  int a;
  a = 4;
  return fib(a);
}
int fib(int n){
  if(n == 0) return 0;
  if(n == 1) return 1;
  return  fib(n - 1)+ fib(n -2);
}
"

# basic test case
# assert 2 "main(){return 2;}"
# assert 42 "main()return 42;"
# assert 21 "main(){return 5+20-4;}"
# assert 2 "main(){return 5 - 3;}"
# assert 47 "main(){return 5+6*7;}"
# assert 15 "main(){return 5*(9-6);}"
# assert 4 "main(){return (3+5)/2;}"
# assert 10 "main(){return -10+20;}"

# assert 0 "main(){return 0==1;}"
# assert 1 "main(){return 42==42;}"
# assert 1 "main(){return 0!=1;}"
# assert 0 "main(){return 42!=42;}"

# assert 1 "main(){return 0<1;}"
# assert 0 "main(){return 1<1;}"
# assert 0 "main(){return 2<1;}"
# assert 1 "main(){return 0<=1;}"
# assert 1 "main(){return 1<=1;}"
# assert 0 "main(){return 2<=1;}"

# assert 1 "main(){return 1>0;}"
# assert 0 "main(){return 1>1;}"
# assert 0 "main(){return 1>2;}"
# assert 1 "main(){return 1>=0;}"
# assert 1 "main(){return 1>=1;}"
# assert 0 "main(){return 1>=2;}"

# # with variables
# assert 14 "main(){
# int a;
# int b;
# a = 3;
# b = 5 * 6 - 8;
# return a + b / 2;}"

# assert 6 "main(){
# int foo;
# int bar;
# foo = 1;
# bar = 2 + 3;
# return foo + bar;}"

# if statement
# assert 6 "main(){if (1 != 1) return 1;
# else return 6;
# return 2;}
# "

# with function
# assert 3 "
# main() return func(1, 2);
# func(int a, int b) { return a + b; }
# "

# pointer

# assert 4 "main(){x = 4;
# y = &x;
# return *y;
# }"

# assert 4 "
# main() return func(1, 2, 3);
# func(a, b, c) { return a + c; }
# "




# assert 10 "i = 0;
# while (i < 10) i = i + 1;
# return i;"

# assert  10 "a = 0;
# for (i = 0; i < 10; i = i + 1) a = a + 1;
# return a;"

# assert 10 "a = 0;
# for (;a < 10;) a = a + 1;
# return a;"

# assert 10 "a = 0;
# for(;;){
#   a = a + 1;
#   if(a == 5) return 10;
# }
# return 1;
# "

# assert 10 "foo();"

# assert 0 "fooo(4, 1);"
# assert 0 "foooo(1, 1, 1);"


echo OK