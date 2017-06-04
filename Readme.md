# TinyCompiler

## 序言

1. 项目概述
	
    本项目是基于flex，bison以及LLVM，使用c++11实现的类C语法编译器，使用flex结合yacc对源代码进行词法、语法分析；在语法分析阶段生成整个源代码相应的抽象语法树后，根据LLVM IR（Intermediate Representation）模块中定义的中间代码语法输出符合LLVM中间语言语法、机器无关的中间代码；最后，本项目通过调用LLVM Back ends模块的接口，根据本地指令集与操作系统架构，将中间代码编译成二进制目标代码。编译生成的目标代码之后可直接编译生成可执行文件，或与其他目标代码链接生成可执行文件。

    本项目解析的语法与是C语言的一个子集，但部分语法存在区别，这些将在最后的测试用例中具体说明。目前已支持的数据类型包括：void, int，float，double，char，string，bool，自定义结构体，以及它们的数组（包括多维数组）。支持的主要语法包括：变量的声明、初始化（多维数组暂不支持），函数声明，函数调用（传递参数类型可以是任意已支持类型），外部函数声明和调用，控制流语句if-else、for、while及其嵌套使用，单行注释等。以上提到的类型和语法均已通过编译成目标代码并生成二进制文件运行测试。

2. 相关术语
	- LLVM：LLVM是一个自由软件项目，它是一种编译器基础设施，以C++写成。LLVM提供了完整编译系统的中间层，它会将中间语言（Intermediate form，IF）从编译器取出与最优化，最优化后的IF接着被转换及链接到目标平台的汇编语言。LLVM支持与语言无关的指令集架构及类型系统[7]。每个在静态单赋值形式（SSA）的指令集代表着，每个变量（被称为具有类型的寄存器）仅被赋值一次，这简化了变量间相依性的分析。LLVM允许代码被静态的编译，包含在传统的GCC系统底下，或是类似JAVA等后期编译才将IF编译成机器码所使用的即时编译（JIT）技术。
	- flex：flex（快速词法分析产生器，英语：fast lexical analyzer generator）是一种词法分析程序。它是lex的开放源代码版本，以BSD许可证发布。通常与GNU bison一同运作，但是它本身不是GNU计划的一部分。
	- bison：GNU bison（Bison意为犎牛；而Yacc与意为牦牛的Yak同音）是一个自由软件，用于自动生成语法分析器程序，实际上可用于所有常见的操作系统。Bison把LALR形式的上下文无关文法描述转换为可做语法分析的C或C++程序。在新近版本中，Bison增加了对GLR语法分析算法的支持。

3. 源代码结构

    TinyCompiler\
    ├── ASTNodes.h 抽象语法树节点的定义\
    ├── CodeGen.cpp 抽象语法树生成中间代码\
    ├── CodeGen.h\
    ├── Makefile\
    ├── ObjGen.cpp 中间代码生成目标代码\
    ├── ObjGen.h\
    ├── Readme.md\
    ├── TypeSystem.cpp 类型系统实现\
    ├── TypeSystem.h\
    ├── grammar.y 语法分析yacc文件\
    ├── main.cpp TinyCompiler程序入口\
    ├── test.input 用于测试词法、语法、语义分析的源代码\
    ├── testmain.cpp 用于测试链接目标文件的源代码\
    ├── token.l 词法分析lex文件\
    └── utils.cpp tonging

4. 使用说明
    - 编译TinyCompiler
    ```
    make
    ```
    - 使用TinyCompiler编译test.c文件，将目标代码输出到output.o
    ```
    cat test.c | compiler
    ```
    用g++链接output.o生成可执行文件
    ```
    g++ output.o -o test
    ./test
    ```
    使用test.input, testmain.cpp文件自动测试编译、链接
    ```
    make test
    make testlink
    ```

5. 参考资料

    [LLVM Language Reference Manual](http://llvm.org/docs/LangRef.html)

    [LLVM Tutorial](http://releases.llvm.org/3.9.0/docs/tutorial/index.html)

    [gnuu - writing your own toy compiler](http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/)

## 语法定义

    本编译器实现的语法是类似标准C的语法，存在的几个区别包括去掉了标准C的每个语句后的分号；不显式提供指针类型，但可以对变量取地址；暂不支持多维数组初始化；强制要求写return语句等。

基本语法
```c
int func(int a, int b){
    if( a > b ){
        return func(b * 0.5)
    }else if( a == b ){
        return func(a * b)
    }else{
        return 0
    }
}

int main(int argc, string[1] argv){
    int i = 0
    for( ; i<argc; i=i+1){
        func(i, argc)
    }
    while( 1 ){}
    return 0
}

```

结构体使用
```c
struct Point{
    int x
    int y
}
int func(struct Point p){
    return p.x
}
int test(){
    struct Point p
    p.x = 1
    p.y = 3
    func(p)
    return 0
}

```

数组使用
```c
int testArray(){
    int[10] oneDim = [1,2,3,4]
    int[3][4] twoDim
    int i, j
    for(i=0; i<3; i=i+1){
        for(j=0; j<4; j=j+1){
            twoDim[i][j] = 0
        }
    }
    return 0
}
```

外部函数使用
```c

extern int printf(string format)
extern int scanf(string format)

int testExtern(){
    string input
    scanf("%s", &input)
    printf("%d, %f, input = %s", 1, 0.1, input)
    return 0
}
```

## 词法分析

词法分析是编译器实现中相对简单的一步。想好所需要实现的语法后，需要将输入转化为一系列的内部标记token。本项目定义的token包括：C语言关键字if、else、return、for、while、struct、int、double、extern等每个关键字对应的token，加减乘除、位运算、括号、逗号、取地址等符号token，以及表示用户自定义的标志符（identifier）的token、数字常量token（整型和浮点数）、字符串常量token等。

我们需要先在yacc源文件grammar.y中声明这些token，并在lex源文件token.l中定义这些token对应的操作，即遇到identifier、数字、字符串等token就将其内容保存到一个string对象中，并返回相应的tokenid以便之后bison程序用来进行语法分析；如果遇到if、else、while或运算符等静态的token就直接返回tokenid。


## 语法分析

1. 实现方式

    语法分析是本项目的关键环节之一。由于源代码是以字符串即字节流的形式存在的，难以进行后续的编译解释工作，因此我们需要将源代码转化成能够反映语法规则的数据结构，即抽象语法树，之后才能利用抽象语法树进行中间代码的生成。

    
    首先我们需要针对每种类型的语法，如变量声明、变量赋值、函数定义、控制流定义等语法定义其相应的抽象语法树，以便能够将任意符合语法标准的源代码都能够转化成一棵抽象语法树。

2. 抽象语法树实现

    TinyCompiler支持语言的语法主要可以分为两类，有返回值的Expression——表达式，以及没有返回值的Statement——语句，这样一来所有合法的语法语句都可以归为表达式或语句中的一种。
    
    因此，根据OOP思想，我们只需要定义抽象语法树的根节点Node类，以及两种语法对应的NStatement以及NExpression类，作为所有语法的抽象基类。之后所有语法只需要继承这两个类并实现其接口即可。

    ![img](http://i1.piimg.com/1949/415a445cff09fb08.jpg)    
    

## 语义分析
    

## 优化考虑

## 中间代码生成

1. 

## 目标代码生成


## 测试用例

