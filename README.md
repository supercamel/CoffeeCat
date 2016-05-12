<h1>CoffeeCat</h1>
<p>CoffeeCat is a programming language and a source-to-source compiler that produces C++. It is in early development. Check the status below.
<h2>Why?</h2>
<p>CoffeeCat is a high level language for micro-controllers. 
Before CoffeeCat, if you wanted to program a micro-controller (such as an Arduino or STM32 board), you would probably need to use C or C++. 
These are great languages but they can be a little tedious to develop with. 
<p>The CoffeeCat language is simple and convenient. It has a Python-esque feel and provides high-level abstraction and safety without adding any bloat. The CoffeeCat compiler turns CoffeeCat into equivalent C++ code, so you have the simplicity of a high level language with the speed and efficiency of C++. 
<p>The compiler only produces a subset of C++. It does not use exceptions, nor does it use new/delete. Method access modifiers (public/protected/private) are also removed and all member methods and variables are made public. As they say in Python circles, "Simple is better than complex" and "We're all consenting adults here".
<p>CoffeeCat is completely interchangeable with C++. C++ code can create CoffeeCat classes and call CoffeeCat functions, and vice versa. This means the enormous wealth of C++ code out there can easily be used from CoffeeCat. 
<h2>The Syntax</h2>
<p>CoffeeCat uses indents to specify blocks of code. The CoffeeCat lexer identifies both four spaces and tabs as an indent, so unlike Python it won't choke if you use tabs and spaces interchangeably. 

<p>CoffeeCat source
```
def int main():
    cout << "Hello world" << endl
```
Output main.cpp
```
#include "main.h"

int main()
{
    cout << "Hello world" << endl;
}
```
main.h
```
#ifndef MAIN_H
#define MAIN_H

#include <iostream>
using namespace std;

int main();

#endif
```

<h2>Compiler</h2>
<p>The CoffeeCat compiler has a full recursive descent parser parser that uses backtracking, lookahead and elements of precedence parsing to generate a parse tree. The parse tree is then traversed using a visitor pattern algorithm to generate C++ source and header files. 
<p>CoffeeCat might seem to simply convert Python-esque syntax into C++ syntax, but it's more than a simple syntax translator. It's a full programming language that completely breaks down the input source files to generate new but equivalent C++. By giving the compiler so much insight into the CoffeeCat code, all manner of interesting transformations and possibilities arise. 

<h2>Development Status</h2>
<p>Presently, the parser is in an infantile state. It is only capable of parsing functions and expressions. It does not even to IF-ELSE or loop statements yet.
