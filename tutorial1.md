# Introduction #

This is tutorial of Giyeok language.


# Hello World #

I will start from printing out "Hello World" just as the other tutorials. Create a new file in the same folder with the interpreter binary("naive1.exe") and copy the following code into the file you generated.
```
println("Hello world!");
```
Then open the command prompt window, and move to the folder where naive1.exe is in, and type "naive1 FILENAME". Of course, FILENAME should be replaced with the name of file you generated, that is you should type "naive1 helloworld.gy" if you generated "helloworld.gy" file.
You will show "Hello World!" on command prompt window.


# Define function(1) #

You can define function as the following.
```
fn myprintln(int a) {
  println(a);
  println(a);
}
```
And you can call this function like this.
```
myprintln(5);
```
This "myprintln" function will get one integer parameter and print the number twice, and it is called with number 5, so you will get the result such as
```
5
5
```
The name of function should be follow the Giyeok language's identifier naming rule. It should be alphanumeric string started with alphabet or string of special characters(~`!@$%^&,+**-/?<>=|:), and it is case sensitive.
For example, "abc", "abc1", "+", "-", "<>", "!=" are OK, but "1abc", "abc?abc", "has?" are wrong. And "abc" and "ABC" are different.**


# Use basic types #

In Giyeok language, every value is object. Integer number 1 is an object, float number 1.1 is object, string "abc" is object, and even boolean value 'true' is an object. Therefore, you can call the member function of those values. For example, integer values have member function named "neg()", so you can call it like this.
```
1.__neg__();
println(1.__neg__());
```
Integer values have add(int a0), sub(int a0), mul(int a0), div(int a0), etc. as well. You can find the list of member functions of basic types in [basictypes](basictypes.md).


# Define function(2) #

You can write functions in functional way, that is, without block.
```
fn increment1(int a) {
  return a.__add__(1);
}
fn increment2(int a) a.__add__(1);
```
Function increment1 is written with block, and increment2 without block. However, these two functions do exactly same thing.
```
fn increment1(int a) {
  return a.__add__(1);
}
fn increment2(int a) a.__add__(1);

println(increment1(1));
println(increment2(1));
```
The result of the code above is, of course,
```
2
2
```


# Define function(3) #

Functions may have no, one, or more than one parameter(s).
```
fn f_a() println("");
fn f_b(int a0) println(a0);
fn f_c(int a0; int a1) println(a1);
```
Function f\_a has no parameter, and f\_b has one parameter, and f\_c has two parameters. If the function has more than one parameters, parameters should be separated with semicolon(;), not comma(,).
You can call the functions above like this.
```
f_a();
f_b(1);
f_c(1;2);
```
Note that the values to parameters are also separated with semicolon, not comma.


# Define operator #

In Giyeok language, you can define operators, and you MUST define all the operators before you use, that means Giyeok does support NO OPERATORS as default. For example, you cannot use "1+1" without defining + operator.
To define + operator, you can use the following code.
```
operator infix 3 +;
```
It declares + as infix operator which has operator priority of 3. Operator priority is higher with smaller number. If you want to declare + and - of same operator priority at the same time, you can use this code.
```
operator infix 3 + -;
```
You can define prefix operators as well. (However, you cannot define postfix operators)
```
operator prefix 1 -;
```
After you define type, priority, and names of operators, you should define their contents with functions.
```
operator infix 3 + -;
operator prefix 1 -;

fn +(int a; int b) a.__add__(b);
fn -(int a; int b) a.__sub__(b);
fn -(int a) a.__neg__();
```
Functions with two parameters will be used for infix operators, and ones with one parameter will be used for prefix operators.

After you define + and - operators and specify the contents of the functions, you can use those like this.
```
operator infix 3 + -;
operator prefix 1 -;

fn +(int a; int b) a.__add__(b);
fn -(int a; int b) a.__sub__(b);
fn -(int a) a.__neg__();

println(5+5);
println(7-3);
println(-(5+2));
```
The result of this code will be
```
10
4
-7
```
You can rewrite the function increment2 introduced above like this.
```
fn increment2(int a) a.__add__(1);
fn increment2(int a) a+1;
```

Because declaring all the operators for each code will be inconvenient, class gy.lang has some declarations of basic operators, and you can use those operators by including line "use gy.lang;". You can see the list of basic operators in [basicoperators](basicoperators.md).


# Define function(4) #

Parameters can be defined with no type restriction or one or more than one type restrictions.
```
fn abc(a; b; c) a+b+c;
fn def(int d; int e; int f) d+e+f;
fn ghi(int float g; int float h; str i) g+h+i;
```
Function ghi will get (int;int;str), (float;int;str), (int;float;str), or (float;float;str).


# Define function(5) #

In Giyeok, you can write more than one functions which have same name.
```
fn +(int a; int b) a.__add__(b);
fn +(str a; str b) a.__append__(b);
```
When you call this + functions, GIyeok will find and call one appropriate function, which is the declared type(s) of each parameter is fit to the given values.
```
operator infix 3 +;
println(1+2);
println("hello "+"world");
```


# Define function(6) #

You can put some condition to a function.
```
use gy.lang;

fn factorial(int i) i*factorial(i-1);
fn factorial(int i if i==0) 1;
```