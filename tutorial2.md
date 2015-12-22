# Introduction #

This tutorial shows how to write Giyeok program.


# Variable and assignment #

```
assign a 1;
assign a 2;
```
```
fn =(var a; b) assign a b;
fn :=(var a; b) assign a b;
```


# `if` statement #

```
if 1==1; println("1 is equal to 1");
else println("1 is not equalto 1");
```


# `while` statement #

```
use gy.lang;

a=0;
while a<5; {
  a+=1;
  println(a);
}
```

```
1
2
3
4
5
```

```
use gy.lang;

fn range(int s; int e; int t) while s<e; (s+=t)-t;
```


# `for` statement #

```
for i in range(5); println(i);
```

```
for i in [7;5;1;6;2]; println(i);
```

```
for i in [[-1;0];[1;0];[0;-1];[0;1]]; println("("+(5+i[0])+", "+(5+i[1])+")");
```


# Blocks and `return` statement #

```
{
  a=1;
  b=2;
  println(a);
  println(b);
}
```

```
fn add(int a; int b) {
  return a+b;
}
```

```
a={ return 5; }
println(a);
```


# Function sets #

```
add={{
fn (int float a; int float b) a.__add__(b);
fn (str a; str b) a.__append__(b);
}};
println(add("abc";"def"));
println(add(5;1.2));
```


# `use` statement #

```
class a {
  class b {
    # Some contents in a.b
  }
}

use a.b;
```

```
class a {
  class b {
    # Some contents in a.b
  }
}

# Some contents in a.b
```