# 现代C++

## 默认实参

```C++
void f(int, int, int = 10);
void f(int, int = 1, int);
void f(int = 2, int, int);

void f(int, int, int) {
}
```

> 在函数声明中，所有在拥有默认实参的形参之后的形参必须拥有 在这个或同一格作用域中先前声明所提供的默认形参

```C++
void f(int, int, int = 10);
void f(int = 2, int, int); // error, 第二个没有默认实参
void f(int, int = 1, int);

void f(int, int, int) {
}
```

## 用户自定义字面量

```C++
#include <iostream>
#include <string>

// 用户自定义必须加_
std::string operator""_r(const char* str, size_t size) {
        std::cout << str << " " << size << "\n";
        return { str };
}

std::string operator""_i(size_t size) {
        return std::to_string(size);
}

int main() {
        "abcd"_r;
        auto str = 123_i;
        return 0;
}
```

## 成员指针

- 成员指针分为 **成员函数指针** 和 **数据成员指针**，数据成员指针或虚函数成员指针并没有真的指向一个内存，它只是表示在当前的类，那个**字段的位置**，比如`&X::value`表示的知识这个数据成员`value`在X类中的关系
- 数据成员指针和虚函数成员指针式一个类似**偏移量**的东西，**成员函数指针**则真的存储了一个**地址**
- 成员指针也没办法脱离类的实例对象单独使用，不管式非静态数据成员指针还是非静态成员函数指针
- 成员指针不是常规的指针

```C++
#include <iostream>

struct X {
        void f() {
                std::cout << "6";
        }
};

int main() {
        void (X:: * p)() = &X::f;
        X x;
        (x.*p)();
        return 0;
}
```

## 三路比较运算符

```C++
#include <iostream>
#include <string>

struct Test {
        int v{};
        //int operator -(const Test& b) const {
        //        return this->v - b.v;
        //}

        // 按照数据成员顺序对比，且拥有了其他>= <= == > < !=
        auto operator <=> (const Test& b) const = default;
    
    // c++20起 <, <=, >, >=, != 运算符分别从operator<=>和operator==合成
};

//auto operator <=> (const Test& a, const Test& b) {
//        if (a - b < 0)
//                return -1;
//        else if (a - b > 0)
//                return 1;
//        else return 0;
//}

int main() {
        Test t1, t2;

        // _Value 不是给我们使用的
        std::cout << (int)(t1 <=> t2)._Value << '\n';
        return 0;
}
```

## 列表初始化

### 概念

> {} 是 **花括号初始化器列表**

> 花括号初始化器列表**不是表达式，没有类型，****`decltype({1, 2})`****非良构**

> **模板形参可以另行推导**

```C++
template<typename T>
struct Test {
        // 通过第一个参数 推导第二个参数
        Test(T a, T b) noexcept {
                std::cout << a << " " << b << std::endl;
        };
};

int main() {
        Test t{ 1, {} };
        return 0;
}
```

> 使用**关键字auto的类型推导**有一个例外，在复制列表初始化中将任何花括号初始化器列表均推导为**`std::initializer_list`**

```C++
auto p1 = {1, 2, 3}; // 推导为std::initializer_list
auto p2 {1, 2, 3}; // 无法推导
```

### 窄化转换

> 列表初始化通过禁止下列转换对隐式转换加以限制：

- 从浮点类型到整数类型的转换
- 从long double到double或float的转换，以及从double到float的转换，除非来源是常量表达式且不发生溢出
- 从整数类型到浮点类型的转换，除非来源是能将它的值完全存储于目标类型的常量表达式
- 从整数或无作用域枚举类型 到 不能表示原类型所有值的整数类型转换，除非来源是能将它的值完全存储于目标类型的常量表达式
- 从指针或成员指针类型到bool的转换

```C++
struct Test {
    int a, b, c, d;
};
struct Test2 {
    double a, b, c, d;
};

int main() {
    int a{}, b{}, c{}, d{};

    Test t1{ 1.1,1.2,1.3,1.4 };//error 不允许浮点类型到整数类型的转换
    Test2 t2{ 1,2,3,4 };//ok 不允许整数类型到浮点类型的转换，但是它的值是常量表达式
    Test2 t3{ a,b,c,d };//error 不是常量表达式，不允许整数到浮点的转换
    Test2 t4{ 1.1l,1.2l,1.3l,1.4l };//按道理不可以，实际上编译器允许
}
```

> 各大编译器并没有很好的遵守这个，不管是gcc还是msvc，上面这些代码在msvc是全部可以通过编译。

## 模板特化

> 类模板可以偏特化 函数模板不能偏特化

```C++
// 类模板偏特化
#include <iostream>

template <typename T, typename U>
struct Test {
        Test() {}
};

template <typename U>
struct Test<int, U> {
        int a;
};

int main() {
        Test<int, double> t{};
        std::cout << t.a << std::endl;
        return 0;
}
// 函数模板不能偏特化
#include <iostream>

template <typename T, typename U>
void foo(T a, U b) {

};

/* error
template <typename U>
void foo<int, U>(int a, U b) {

}
*/

// 另一个模板
template <typename U>
void foo(int a, U b) {

}

int main() {
        foo(1, 2); // foo<int, int>
        foo(1.1, 22); // foo<double, int>
        return 0;
}
// 一种奇怪的方式

#include <iostream>

template <typename T>
struct Test {
        Test() {}
};

template <typename T>
void foo(T t) {

}
/* error 也是属于偏特化
template <typename T>
void foo<Test<T>>(Test<T> t) {

}
*/

// 重载, 另一个函数模板
template <typename T>
void foo(Test<T> t) {

}

int main() {
        foo(1);
        return 0;
}
```

## 参数包和折叠表达式

```C++
#include <iostream>

template <typename T, typename U>
bool foo(T a, U u) {
        std::cout << a << " " << u << std::endl;
        return std::is_same_v<T, U>;
}

template <typename T, typename ... Ts>
void ff(T t, Ts ... ts) {
    // foo(t, ts) 是一个参数包
        std::cout << (foo(t, ts) && ...) << std::endl;
}

int main() {
        ff(1, 2, 3, 4);
        return 0;
}
/* OUTPUT
1 2
1 3
1 4
1
*/
/*
template <typename T, typename ... Ts>
void ff(T t, Ts ... ts) {
    // (1 && arg) ...
    // 1 && arg 是参数包模式
        std::cout << (1 && arg...) << std::endl;
    
}
*/

#include <iostream>

template <typename T>
T foo(T a) {
        std::cout << a << std::endl;
        return a;
}

template <typename T, typename ... Ts>
void ff(T t, Ts ... ts) {
    // 1 + ts 变成了一个参数包
    // foo(1 + ts) 也是一个参数包 foo(1 + ts1), foo(1 + ts2), .....
        std::cout << (foo(1 + ts) + ...) << std::endl;
}

int main() {
        ff(1, 2, 3, 4);
        return 0;
}

/* OUTPUT
3
4
5
12
*/
```

## 类模板实参推导

### 前置知识

```C++
/*
如果B为true, 则std::enable_if拥有等同于T的公开成员typedef type, 否则, 无该成员typedef 
*/

// 可能的实现
template<bool B, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { typedef T type; };

// 辅助类型
template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type;

/* std::is_same_v

*/

// 可能的实现
template<class T, class U>
struct is_same : std::false_type {};
 
template<class T>
struct is_same<T, T> : std::true_type {};

// 辅助类型
template< class T, class U >
inline constexpr bool is_same_v = is_same<T, U>::value;
```

### 用户推导指引

```C++
#include <iostream>

template <typename T>
struct Test {
    Test(T) {}
    T s;
};

//template <typename T>
//Test(T) -> Test<double>;

Test(const char*) -> Test<std::string>;

template <typename T, size_t size>
struct array {
    T arr[size];
};

/*
std::is_same_v<Tu, Tp> && ...
折叠表达式右展开

std::is_same_v<Tu, Tp>是一个参数包, std::is_same_v<Tu, Tp1>, std::is_same_v<Tu, Tp2>
然后折叠表达式右展开
(std::is_same_v<Tu, Tp1> && std::is_same_v<Tu, Tp2>) && ……

*/
template <class Tu, class... Tp>
array(Tu, Tp...) -> 
    array<std::enable_if_t<(std::is_same_v<Tu, Tp> && ...), Tu>, sizeof...(Tp) + 1>;

int main() {
    Test t(1);
    Test t1("1");

    array arr{ 1, 2, 3 };
    return 0;
}
```

## 强制转换

```C++
#include <iostream>

struct A {
        A() {
                std::cout << "1" << std::endl;
        }
        A(const A&) {
                std::cout << "2" << std::endl;
        }
        virtual void foo() {
                std::cout << "A" << std::endl;
        }
};

struct B : public A {
        void foo() override {
                std::cout << "B" << std::endl;
        }
};

int main() {
        B* b = new B;
    
    // A调用拷贝构造函数
        A a = *b;
        delete b;
        return 0;
}
/* OUTPUT
1
2
*/
#include <iostream>

struct A {
        A() {
                std::cout << "1" << std::endl;
        }
        A(const A&) {
                std::cout << "2" << std::endl;
        }
        virtual void foo() {
                std::cout << "A" << std::endl;
        }
};

struct B : public A {
        void foo() override {
                std::cout << "B" << std::endl;
        }
};

int main() {
        B* b = new B;
    
    // 类似于A a = *b, 创建了一个新的临时A类型对象a
    // 所以调用使用的是A的foo函数.
    static_cast<A>(*b).foo();
        delete b;
        return 0;
}
/* OUTPUT
1
2
A
*/
```

## 模板模板形参

```C++
#include <iostream>

template <typename T>
struct A {

};

template < template <typename T> class S >
struct B {

};

int main() {
        /*
        这里传的是一个模板A, 而不是一个模板类实例 如A<int> (模板类实例能直接用模板接收)
        模板模板形参只需要是一个模板, 传的时候不需要管S里面的模板类型, 因为有可能它并不会被实例化
    */
        B <A> b;
        return 0;
}
```

## 名字查找

```C++
#include <iostream>

namespace s {
        namespace std {
        }
}

namespace Y {
        struct X {};
        void foo(X) {}
}

int main() {
        struct std {};
        using namespace s;
        // std::string a;
        
    // 全局搜索
        ::std::string a;
        
    //实参依赖查找
        endl(::std::cout);
        foo(Y::X());
    
        return 0;
}
```

## 形参包

```C++
#include <iostream>


// 1.有名字的非类型模板形参包
template <size_t ... args>
struct X1 {
    void f() {
        // 6.形参包展开：展开成零个或更多个模式 的逗号分隔列表
        for (const auto& i : { args... }) {
            std::cout << i << " ";
        }
        std::cout << "\n";
    }
};

// 2.可以有名字的类型模板形参包
template <typename ... Args>
void f2(Args... args) { // 5.可以有名字的函数形参包
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

// 3.可以有名字的受约束的类型模板形参包(C++20 起)
template <std::integral ... Args>
void f3(Args ... args) {
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

template <class T1, class T2, class T3>
struct T {

};

// 4.可以有名字的模板模板形参包
template < template <class T1, class T2, class T3> class ... Args>
struct X4 {

};

void print() {
    endl(std::cout);
}

template <typename T, typename ... Args>
void print(T t, Args...args) {
    std::cout << t << " ";
    print(args...); // 6
}

template <size_t ... args>
void init() {
    int array[sizeof...(args)]{ args... };
    for (const auto& i : array) {
        std::cout << i << " ";
    }
    endl(std::cout);
}


int main() {
    X1<1, 2, 3, 4, 5, 6> x1;
    x1.f();
    f2(1, 2, 3, 4, "#");
    f3(1, 2, '*');
    X4<std::basic_string, T, T, T> x4;

    print("1", 5, 4, 2);

    init<1, 2, 3, 4, 5>();
    return 0;
}
// 基类说明符与成员初始化器列表
// 包展开可以用于指定类声明中的基类列表。通常这也意味着它的构造函数也需要在成员初始化器列表中使用包展开，以调用这些基类的构造函数
template<class... Mixins>
class X : public Mixins...
{
public:
    X(const Mixins&... mixins) : Mixins(mixins)... {}
};


// using声明
// 在 using 声明中，省略号可以在声明器列表内出现，这对于从一个形参包进行派生时有用：
template <typename... bases>
struct X : bases...
{
    using bases::g...;
};
X<B, D> x; // OK：引入 B::g 与 D::g
#include <iostream>

template <class ... Ts>
struct overloaded : Ts... {
        // 在 using 声明中，省略号可以在声明器列表内出现
        using Ts::operator()...;
};

template <class...Args>
struct X : Args... {

};

// 模板变量
template <size_t ... args>
constexpr size_t v = (args + ...);

int main() {
    // 这里列表初始化 推导了 模板
        auto c = overloaded{ 
                [](int arg) {std::cout << arg << ' '; }, 
                [](double arg) {std::cout << arg << ' '; }, 
                [](auto arg) {std::cout << arg << ' '; } 
        };
        c(1);
        c(2.2);
        endl(std::cout);
        //X x1 { "2"s };
        std::cout << v<1, 2, 3> << "\n";
        return 0;
}
#include <iostream>

template <typename T>
T& method(T& t) {
    return static_cast<T&>(t);
}

void print() {
    std::cout << std::endl;
}

template <typename T, typename ...Args>
void print(T&& t, Args ... args) {
    std::cout << t << " ";
    print(args...);
}

template <typename ...Args>
void foo(Args&&... args) {
    /* 这里会变成
    print(method<int>(1), method<int>(2), method<int&>(3))
    */
    print(method<Args>(args)...);
}

int main() {
    int a = 3;
    foo(1, 2, a);
}
```

## 折叠表达式

```C++
#include <iostream>

// 右折叠
template <size_t ... args>
constexpr int v = (args - ...);

// 左折叠
template <size_t ... args>
constexpr int v2 = (... - args);

template <size_t ... args>
void foo() {
        ((std::cout << args << ' '), ...);
        endl(std::cout);
}

template <class ... Args>
void print(Args&&... args) {
        (std::cout << ... << args) << '\n';
}

int main() {
        std::cout << v<4, 5, 6> << '\n';
        std::cout << v2<4, 5, 6> << '\n';
        /*
    (std::operator<<(std::cout.operator<<(1UL), ' ')),
    ((std::operator<<(std::cout.operator<<(2UL), ' ')),
    (std::operato<<(std::cout.operator<<(3UL), ' ')));
    简化一下符号, 发现还是右折叠
    (<<(cout<<(1UL), ' '), ( (<<(cout<<(2UL), ' ')), (<<(cout<<(3UL), ' ')) )
    */
        foo<1, 2, 3>();
    
    // <<(<<(std::cout, 1), 2)
        print(1, 2, 3, 4);
        return 0;
}
```

## 全特化与偏特化

```C++
#include <iostream>

template <class T>
struct X {
    void f() { };
};

template <>
struct X<double> {
    void f(int) {}
};

template <class T>
void f() {
    std::cout << "6\n";
}

template <>
void f<double>() {
    std::cout << "7\n";
}

template <class T>
constexpr size_t v = 6;

// 偏特化
template <class T>
constexpr size_t v<const T> = 7;

int main() {
    X<int> x;
    x.f();
    X<double> x2;
    x2.f(1);
    f<int>(); // 6
    f<double>(); // 7
    std::cout << v<int> << std::endl; // 6
    std::cout << v<const int> << std::endl; // 7
    return 0;
}
```

## 实现is_same_v

```C++
#include <iostream>

template <typename T, typename U>
struct is_same {
        const static bool value = false;
};

template <typename T>
struct is_same<T, T> {
        const static bool value = true;
};

template<typename T, typename U>
bool is_same_v = is_same<T, U>::value;

int main() {
        std::cout << std::boolalpha << is_same_v<int, int> << '\n';
        std::cout << std::boolalpha << is_same_v<int, double> << '\n';
        return 0;
}
```

## 约束与概念

- 类型模板形参声明
- 占位类型说明符
- 复合要求

```C++
#include <iostream>
#include <string>

/*
* 这里Test是一个概念
* std::is_same_v<T, int>是一个约束表达式
* 概念不能被显示实例化，显示特化或部分特化
* 
*/
template <class T>
concept Test = std::is_same_v<T, int>;


void f(Test auto t) {

}

// 类型模板形参声明
template <Test T>
void f2(T) {

}

// 复合要求
template <class T>
concept Test2 = requires (T a, T b){
        // 复合约束表达式
        {a + b} -> Test; // a + b推导出来的类型必须符合Test概念的约束
};

// 占位类型说明符
void f3(Test2 auto) {

}

Test auto f4() {
        return 6;
}

/*约束表达式由三种操作
* 合取 &&
* 析取 ||
* 不可分割约束
*/

// 合取
template <class T>
concept Integral = std::is_integral<T>::value;
template <class T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;


// 不可分割约束
//template <>
//template <typename T>
//        requires (S<T>{})
//void f5(T) {
//
//}

template <typename T>
concept Addable = requires (T a, T b) {
        a + b; // 需要表达式a+b 被编译为有效的表达式
        std::is_same_v<T, int>; // 只要能通过编译就行
        // typename T::iterator;
};
void f5(Addable auto t) {

}

// 嵌套要求
template <typename T>
concept Addable2 = requires (T a, T b) {
        a + b; // 需要表达式a+b 被编译为有效的表达式
        requires std::is_same_v<T, int>; // requires子句，要求表达式为真
};

void f6(Addable2 auto t) {

}

// requires子句，要求表达式为真
template<class T>
void f7(T t) requires std::is_same_v<T, int> {

}

int main() {
        f(1);
        //f(1.2); // error
        std::cout << std::boolalpha << Test<int> << '\n'; // true


        f3(1);
        //f3(1.2); // error

        Test auto p = 2;
        
        f5(5);
        
        // 只要requires里面能通过编译就行
        f5(1.1);

        //f5("1"); // error

        f6(6);
        //f6(1.1); // error

        f7(1);
        //f7(1.1); // error
        return 0;
}
```

### Requires表达式

> 产生一个描述约束的bool类型的纯右值表达式。

```C++
reuires {要求序列}
reuires (参数列表) {要求序列}
```

- 简单要求

```C++
template <typename T>
concept Addable = requires (T a, T b) {
    a + b; // 需要表达式a+b 被编译为有效的表达式
};
template <typename T>
concept Addable = requires(T x) { x + x; }; // requires 表达式

template <typename T> requires Addable<T> // requires子句，不是requires表达式 要求为真
T add(T a, T b) { return a + b; }

// 第一个requires是requires子句, 类似于概念
// 第二个requires是requires表达式
// 只要requires表达式能编译就是真
template <typename T>
        requires requires (T x) { x + x; } // 临时约束
T add2(T a, T b) {
        return a + b;
}
```

## 待决名

> 在[模板](https://zh.cppreference.com/w/cpp/language/templates)（[类模板](https://zh.cppreference.com/w/cpp/language/class_template)和[函数模板](https://zh.cppreference.com/w/cpp/language/function_template)）定义中，某些构造的含义可以在不同的实例化间有所不同。特别是，类型和表达式可能会取决于类型模板形参的类型和非类型模板形参的值。

```C++
template<typename T>
struct X : B<T> // "B<T>" 取决于 T
{
    typename T::A* pa; // "T::A" 取决于 T
                       // （此 "typename" 的使用的目的见下文）
    void f(B<T>* pb)
    {
        static int i = B<T>::i; // "B<T>::i" 取决于 T
        pb->j++; // "pb->j" 取决于 T
    }
};
```

## 遮掩继承名称

```C++
#include <iostream>

class Base {
public:
    virtual void foo() {
            std::cout << "Base" << std::endl;
    }
    virtual void foo(int) {
            std::cout << "Base int" << std::endl;
    }
    void method() {

    }
    void method(int) {

    }
};

class Derived : public Base {
public:
    virtual void foo() {
            std::cout << "Derived" << std::endl;
    }
    void method() {

    }
};

int main() {
    // 父类的同名函数(不管是否是重载函数)被子类的同名函数遮掩了
    Derived d;
    d.foo();
    //d.foo(1);    // error
    d.method();
    //d.method(1); // error
    return 0;
}
```

### 解决办法

```C++
#include <iostream>

class Base {
public:
    virtual void foo() {
            std::cout << "Base" << std::endl;
    }
    virtual void foo(int) {
            std::cout << "Base int" << std::endl;
    }
    void method() {

    }
    void method(int) {

    }
};

class Derived : public Base {
public:
    using Base::foo;
    using Base::method;
    virtual void foo() {
            std::cout << "Derived" << std::endl;
    }
    void method() {

    }
};

int main() {
    Derived d;
    d.foo();
    d.foo(1);
    d.method();
    d.method(1);
    return 0;
}
```

## typename

> **如果****解析器****在template中遭遇一个嵌套从属名称，便假设这名称不是一个类型，除非你通过放置关键字typename告诉它是**

```C++
#include <iostream>
#include <vector>

template <typename T>
struct A {
    using Int = int;
    using V = std::vector<T>;
};

struct B {
    using Int = int;
};

template <typename T>
void foo() {
    // template内出现的名称相依于template参数，称为从属名称
    // 从属名称再class内呈嵌套状，则是嵌套从属名称
    // A<T>是从属名称，A<T>::Int是嵌套从属名称
    typename A<T>::Int a; // 编译器无法知道Int是一个类型还是一个变量，我们需要用typename指定
    A<T> b;
}

template <typename T>
void foo2() {
    // T是从属名称，T::Int是嵌套从属名称
    typename T::Int a;
    T b;
}

template <typename T>
struct C : public A<T>::V { // 不可以
    C(int x) : A<T>::V(x) { // 不可以
        typename A<T>::V temp; // 可以
    }
};

template <typename T>
void foo3() {
    typedef typename A<T>::Int Int;
    Int v;
};

int main() {
    foo<int>();
    foo2<B>();
    C<int> c(1);
    foo3<int>();
    return 0;
}
```

## 模板化基类

> 编译器会拒绝在模板化基类(BaseTest<T>)内寻找继承而来的名称，因为这个模板基类可能会被特化，导致没有里面的`TestFoo`函数。
>
> 父类模板中的名称对子类模板不是直接可见的，需要通过`this->`前缀、`using`或显式地特化模板父类来访问父类中的名称。

```C++
class CompanyA {
public:
    void foo() {

    }
};

class CompanyB {
public:
    void foo() {

    }
};

class CompanyC {
public:
    void method() {

    }
};

template <typename T>
class BaseTest {
public:
    void TestFoo() {
        T().foo();
    }
};

template <>
class BaseTest<CompanyC> {
public:
    void TestFooCompanyC() {

    }
};

template <typename T>
class DerivedTest : public BaseTest<T> {
public:
    void TestFooDerived() {
        Testfoo(); // error
    }
};

int main() {
    
}
```

### 解决办法

```C++
template <typename T>
class BaseTest {
public:
    void TestFoo() {
    }
};

template <typename T>
class DerivedTest : public BaseTest<T> {
public:
    void TestFooDerived() {
        this->TestFoo(); // 假设继承TestFoo函数
    }
};
template <typename T>
class BaseTest {
public:
    void TestFoo() {
    }
};

template <typename T>
class DerivedTest : public BaseTest<T> {
public:
    using BaseTest<T>::TestFoo;
    void TestFooDerived() {
        TestFoo(); // 假设位于BaseTest<T>内
    }
};
template <typename T>
class BaseTest {
public:
    void TestFoo() {
    }
};

template <typename T>
class DerivedTest : public BaseTest<T> {
public:
    void TestFooDerived() {
        BaseTest<T>::TestFoo(); // 假设继承TestFoo函数
        // 如果调用的是virtual函数 明确资格修饰会关闭"virtual绑定行为"
    }
};
```

## 泛化函数

```C++
#include <iostream>

template <typename T>
class Test {
public:
    // 没有默认构造函数了
    Test() {
    }

    template <typename U>
    Test(const Test<U>& u) {
            std::cout << "copy" << std::endl;
    }

    template <typename U>
    Test& operator=(const Test<U>& u) {
            std::cout << "operator=" << std::endl;
            return *this;
    }
};

int main() {
    
    Test<double> t1{};
    Test<int> t2{ t1 }; // copy
    std::cout << "----------\n";
    Test<int> t3{ t2 }; // U == T时，这里不会使用泛化copy构造函数模板，而是编译器自动生成一个
    std::cout << "===========\n";
    
    t3 = t1;
    std::cout << "----------\n";
    t3 = t2; // 同上
    return 0;
}

/*    OUTPUT
copy
----------
===========
operator=
----------
*/
template <typename T>
class Test {
public:
    Test() {}
    
    Test(const Test&); // 声明正常的copy函数
    
    template <typename U>
    Test(const Test<U>& u);

    Test& operator=(const Test&); // 声明正常的copy assignment

    template <typename U>
    Test& operator=(const Test<U>& u);
};
```

## template具体化无法隐式转换

> 函数的隐式转换是在函数调用的时候发生，但是调用一个函数，需要知道这个函数的存在，这里要知道这个函数，必须先从funtion template推导出参数类型，才能具体化(实例化)，但是template实参推导时没办法隐式转换。

```C++
#include <iostream>

template <typename T>
class Ration {
public:
    Ration(int) {
    }
    //friend const Ration operator*(const Ration& l, const Ration& r);
};

template <typename T>
const Ration<T> operator*(const Ration<T>& l, const Ration<T>& r) {
    return Ration<T>(1);
}


int main() {
    Ration<int> a(1);
    Ration res = a * 2; // 无法编译
    
}
#include <iostream>

template <typename T>
class Ration {
public:
    Ration(int) {
    }
    
    // Ration<int>实例化的时候，下面的函数声明也实例化了
    friend const Ration operator*(const Ration& l, const Ration& r);
};

// 这里的定义还是没办法实例化，所以链接不到定义
template <typename T>
const Ration<T> operator*(const Ration<T>& l, const Ration<T>& r) {
    return Ration<T>(1);
}


int main() {
    Ration<int> a(1);
    Ration res = a * 2; // 能编译，链接时候出错
}
#include <iostream>

template <typename T>
class Ration {
public:
    Ration(int) {
    }
    
    // 这里的Ration是Ration<T>的简写
    friend const Ration operator*(const Ration& l, const Ration& r) {
        return Ration(1);
    }
};

int main() {
    Ration<int> a(1);
    Ration res = a * 2;  // ok
}
```

# 并发支持库

## std::thread

```C++
#include <iostream>
#include <thread>
#include <vector>

void f(int& t, size_t n) {
    size_t tmp{};
    for (int i = 1; i <= n; i++)
        tmp += i;
    t = tmp;
}

struct F {
    auto operator() () {
        std::cout << std::this_thread::get_id() << '\n';
    }
};

// f1(int(*)(int, int)) 形参是一个函数指针，返回值是int
// f1 f2 f3是相同的
void f1(int(int, int)) {}
void f2(int(g)(int, int)) {}
void f3(int(*g)(int, int)) {}

int main() {
    std::thread t1{ []() {std::cout << std::this_thread::get_id() << "\n"; } };
    int i = 0;
    while (i < 10000000) i++;
    std::cout << "123" << "\n";

    t1.join();

    int v{};
    // std::ref() 传递v引用
    std::thread t2{ f, std::ref(v) , 100 };
    t2.join();
    std::cout << v << std::endl;
    // 等3s
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "--------------------------\n";
    std::vector<std::thread> Threads;
    for (int i = 0; i < 4; i++)
        Threads.emplace_back([]() {std::cout << std::this_thread::get_id() << '\n'; });
    for (auto& i : Threads) {
        i.join();
    }
    std::cout << "-----------------------\n";
    // error -> std::thread t3(f(*)()) 识别为函数
    //std::thread t3(F());

    std::thread t3((F())); // 或使用 {}
    t3.join();
    
    return 0;
}
```

## detach 悬空引用

```C++
#include <iostream>
#include <thread>

struct thread_guard {
    std::thread& t;
    explicit thread_guard(std::thread& t_) : t(t_) {}
    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator= (thread_guard const&) = delete;
};

void f(int& i) { // 那么这个引用就悬空了，引用了已经悬空的地址，造成未定义行为
    i++;
}

void call() {
    int n{}; // n在call执行完会销毁
    
    std::thread t{f, std::ref(n)};
    
    // detach()线程分离，join确保线程在join的地方执行完成，detach不会卡在这里
    // join一定会让子线程在主线程之前执行完，detach可能会在主线程执行完毕后还执行，子线程资源由运行时库回收
    
    // 会造成悬空引用
    //t.detach();
    
    // RAII管理
    ::thread_guard t2(t);

}

int main() {
    call();
    return 0;
}
```

## 移交线程归属权 和 移动传递参数

```C++
#include <iostream>
#include <thread>

struct X {
    X() noexcept { std::cout << "默认构造\n"; }
    X(X&&) noexcept { std::cout << "移动构造\n"; }
    X(const X&) noexcept { std::cout << "拷贝构造\n"; }
};

void f(X&& x) {

}

auto f2() {
    std::thread t{ [](int n) {
            std::cout << n << '\n';
    }, 10};
// 测试了一下, 返回这个非引用t, 也是移动构造构建的临时对象, t应该是一个将亡值
    return t;
}

auto f3() {
    std::cout << std::this_thread::get_id() << '\n';
}

int main() {
    /*X x;
    std::thread t{ f, std::move(x) };
    t.join();*/


    std::thread t;
    t = f2();
    t.join();

    std::thread t2 { [](int n) {
            std::cout << n << '\n';
    }, 10 };
    std::thread t3 = std::move(t2);
    t3.join(); // 不能使用t2.join(); t2没有关联对象
    
    t2 = std::thread(f3);

    t2.join();
    return 0;
}
#include <iostream>

struct X {
    X() noexcept { std::cout << "1\n"; }
    X(X&&) noexcept { std::cout << "2\n"; }
    X(const X&) = delete;
};

X f() {
    X x;
    std::cout << "?\n";
        // 返回的临时对象的构造也是用的移动构造, 应该是这里的x变成了将亡值
    // static_cast<&&>(t) 有一个引用t的临时对象(有身份), 所以是将亡值
    return x;
}

int main() {
    X x = f();
}
/*        OUTPUT
1
?
2
2
*/
```

## 互斥量 和 管理类

```C++
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;
int n = 1;

void f1() {
    {
    // RAII 管理
        std::lock_guard<std::mutex> lc(m);
        //std::cout << "aa\t" << "bb\n";
        n++;
    }
}

void f2() {
    std::lock_guard<std::mutex> lc(m);
    std::cout << n << std::endl;
    //m.lock();
    //std::cout << "cc\t" << "dd\n";
    //m.unlock();
}

int main() {
    for (int i = 0; i < 10; i++) {
        std::thread t{ f1 };
        std::thread t2{ f2 };
        t.join();
        t2.join();
    }
    return 0;
}
```

## std::adopt_lock

```C++
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;

void f() {
    m.lock();
    //...
    std::lock_guard<std::mutex> lc(m, std::adopt_lock); // 走了另一个构造函数
    std::cout << std::thread::hardware_concurrency() << "\n"; // 12，表示本机最多并行12个线程
    std::cout << "123\n";
}



int main() {
    std::thread t{ f };
    t.join();
    return 0;
}
```

## 实现lock_gurad

```C++
#include <iostream>
#include <mutex>
#include <thread>

template <class Mutex>
struct lock_guard {
    explicit lock_guard(Mutex &m) : _m(m) {
        _m.lock();
    }

    ~lock_guard() {
        _m.unlock();
    }
    lock_guard(Mutex& m, std::adopt_lock_t) : _m(m) {

    }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator= (const lock_guard&) = delete;
private:
    Mutex& _m;
};

std::mutex m;

void f() {
    ::lock_guard<std::mutex> lc(m);
    std::cout << "aa\t" << "bb\n";
}

int main() {
    for (int i = 0; i < 10; i++) {
        std::thread t{ f };
        std::thread t2{ f };
        t.join();
        t2.join();
    }
    return 0;
}
```

## std::unique_lock

```C++
#include <iostream>
#include <mutex>
#include <thread>

struct X {
    mutable std::mutex m;

    void f() const {
        std::unique_lock<std::mutex> ul(m);
        std::cout << "aa\t" << std::this_thread::get_id() << '\n';
        ul.unlock(); // 可以自己lock or unlock
    }
};

void f() {}

int main() {
    X x;
    for (int i = 0; i < 10; i++) {
        std::thread t(&X::f, &x);
        std::thread t2(&X::f, &x);
        t.join();
        t2.join();
    }
    int n;
    // addressof 也是取地址
    std::cout << std::boolalpha << (std::addressof(n) == &n) << std::endl;
    return 0;
}
```

## std::scoped_lock

### 概念

##### 通用互斥管理的第二个参数

- `defer_lock_t`不获得互斥的所有权
- `try_to_lock_t`尝试获得互斥的所有权而不阻塞
- `adopt_lock_t`假设调用方线程已拥有互斥的所有权

```C++
// c++17
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m, m2;

void f() {
    // 同时锁住两个互斥

    // (1)
    /*m.lock();
    m2.lock();*/

    // (2) 等价于(1), std::lock函数模板至少需要两个互斥
    //std::lock(m, m2);

    // (3)
    std::scoped_lock sl(m, m2);


}

int main() {
    std::thread t;
    return 0;
}
```

### 实现

```C++
#include <iostream>
#include <mutex>
#include <thread>
#include <tuple>


template <class ... Mutexs>
class scoped_lock {
public:
    explicit scoped_lock(Mutexs& ... mutexs) : my_mutexs(mutexs...) {
        std::lock(mutexs...);
    }

    explicit scoped_lock(std::adopt_lock_t, Mutexs& ... mutexs) : my_mutexs(mutexs...) {

    }

    ~scoped_lock(){
        std::apply([](Mutexs&...mutexs) {(..., mutexs.unlock());}, my_mutexs);
    }

    scoped_lock(const scoped_lock&) = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;

private:
    // 参数包展开, 引用类型
    std::tuple<Mutexs&...>my_mutexs;
};

void f() {
    static std::mutex m1, m2; // 生命周期要比用这个的长

    scoped_lock sl{ m1, m2 };
    std::cout << "aa\t" << "bb\n";
}

int main() {
    for (int i = 0; i < 10; i++) {
        std::thread t{ f };
        std::thread t2{ f };
        t.join();
        t2.join();
    }
// 偏特化的版本和之前实现的std::lock_guard一样
    return 0;
}
```

## call_ones 和 once_flag

```C++
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int n{};
std::mutex m;
std::once_flag fl, fl2, fl3;

void f() {
    std::lock_guard<std::mutex> lc(m);
    n++;
    std::cout << n << '\n';
    
}

void g() {
    std::lock_guard<std::mutex> lc(m);
    n++;
    std::cout << n << '\n';
}

void ff() {
    std::lock_guard<std::mutex> lc(m);
    n++;
    std::cout << n << '\n';
    throw std::runtime_error("123");
}

int main() {
    // 准确执行一次可调用对象，即使多个线程，下面只执行了一次
    std::call_once(fl, f);
    std::call_once(fl, f);
    std::call_once(fl, f);
    std::call_once(fl, g);
    
    std::cout << "-----------\n";

    // 多线程也执行一次
    std::vector<std::thread> v;
    v.emplace_back([] {std::call_once(fl3, f); });
    v.emplace_back([] {std::call_once(fl3, f); });
    v.emplace_back([] {std::call_once(fl3, f); });
    v.emplace_back([] {std::call_once(fl3, f); });
    for (auto& i : v) {
        i.join();
    }

    std::cout << "-----------\n";

    // 抛出异常不会翻转once_flag，这里会执行两次
    try {
        std::call_once(fl2, ff);
    }
    catch (const std::exception) {

    }
    std::call_once(fl2, ff);

    return 0;
}
```

## 读写锁

```C++
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <string>

/*
* shared_mutex来解决读者-写者问题，也就是读写锁，和普通锁不一样
* 读写锁同时只能有一个读者或多个读者，但是不能同时既有读者又有写者，读写锁的性能一般比普通锁要好
* shared_mutex通过lock_shared, unlock_shared, shared_lock进行读者的锁定与解锁
* 通过lock, unlock, unique_lock进行读者的锁定与解锁
*/

std::shared_mutex g_mutex;
std::string g_str;

void readLoop() {
    while (true) {
        std::shared_lock sl(g_mutex); // 共享锁
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << g_str;
    }
}

void writeLoop() {
    int number = 0;
    while (true) {
        std::lock_guard lg(g_mutex); // or  unique_lock
        std::this_thread::sleep_for(std::chrono::seconds(1));
        g_str = std::to_string(number++) + '\n';
    }
}


int main() {

    return 0;
}
```

## 条件变量std::condition_variable

```C++
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <string>
#include <condition_variable>

std::condition_variable cv;        //        全局条件变量对象
std::mutex mtx;                                //        全局互斥锁
bool ready = false;                        //        全局标志位

void do_print_id(int id) {
    std::unique_lock<std::mutex> lck(mtx);
    /*
    * wait 第二个参数: 
    * 如果返回值是true, wait()直接返回，如果一开始ready=true，那么不执行go也不会阻塞
    * 如果是false, 那么wait()将解锁互斥量，并阻塞
    * 直到唤醒 且满足第二个参数返回true
    * 没有第二个参数 则默认false（但是后面不会判断第二参数）
    */
    cv.wait(lck, [] {return ready == true; });        
    std::cout << "thread" << id << '\n';
}

void go() {
    std::lock_guard <std::mutex> lck(mtx);
    ready = true;
    cv.notify_all();
}

int main() {
    std::thread threads[10];
    for (int i = 0; i < 10; ++i)
            threads[i] = std::thread(do_print_id, i);
    std::cout << "10 thread ready to race...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    go();
    for (auto& i : threads)
            i.join();
    return 0;
}
```

## 异步std::async

```C++
#include <iostream>
#include <thread>
#include <future>

constexpr int f(int n) { // 在线程里运行
    int t{};
    for (int i = 0; i < n; i++)
        t += i;
    return t;
}

/* std::thread和std::async的区别是什么
* std::thread一定会创建线程，如果资源不够 会产生错误
* std::async不一定，有可能不会创建线程，也不会产生错误
* 
* std::launch::deferred 延迟调用，不会在子线程里面运行，延迟到调用get或者wait的时候执行
* std::launch::async 在子线程里面执行(强制的)
* 默认情况下是std::launch::async
* std::launch::deferred | std::launch::async 表示的是 是否创建线程，由系统调度决定，如果系统资源紧张就不会创建
* 
*/ 

int main() {
    //std::future<int> ret = std::async(f, 10);
    
    std::future<int> ret = std::async(std::launch::deferred, f, 10);
    std::cout << ret.get() << std::endl;
    //ret.wait();
    return 0;
}
```

## std::future

```C++
#include <iostream>
#include <thread>
#include <future>

int f(int n) { // 在线程里运行
    int t{};
    for (int i = 0; i < n; i++)
        t += i;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return t;
}

/* std::thread和std::async的区别是什么
* std::thread一定会创建线程，如果资源不够 会产生错误
* std::async不一定，有可能不会创建线程，也不会产生错误
* 
* std::launch::deferred 延迟调用，不会在子线程里面运行，延迟到调用get或者wait的时候执行
* std::launch::async 在子线程里面执行(强制的)
* 默认情况下是std::launch::async
* std::launch::deferred | std::launch::async 表示的是 是否创建线程，由系统调度决定，如果系统资源紧张就不会创建
* 
*/ 

/*
* future是“将来”, 配合std::async函数模板，创建一个异步任务，它最常用的成员函数是get()和wait()
* get()调用的时候 要求 线程必须执行完毕 拿到返回值结果，如果线程没有运行完 就会一直等待，像join()
* wait()用于等待线程返回，本身并不返回结果，效果更像join()
*/

int main() {
    //std::future<int> ret = std::async(f, 10);
    
    std::future<int> ret = std::async(std::launch::deferred | std::launch::async, f, 10);
    std::cout << ret.get() << std::endl;
    //ret.wait();
    return 0;
}
/* wait_for
future_status::deferred        共享状态持有的函数正在延迟运行，结果将仅在显式请求时计算
future_status::ready        共享状态就绪
future_status::timeout        共享状态在经过指定的等待时间内仍未就绪
*/

#include <iostream>
#include <future>
#include <thread>
#include <chrono>

int main()
{
    std::future<int> future = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 8;
    });

    std::cout << "waiting...\n";
    std::future_status status;
    do {
        status = future.wait_for(std::chrono::seconds(1));
        if (status == std::future_status::deferred) {
            std::cout << "deferred\n";
        }
        else if (status == std::future_status::timeout) {
            std::cout << "timeout\n";
        }
        else if (status == std::future_status::ready) {
            std::cout << "ready!\n";
        }
    } while (status != std::future_status::ready);

    std::cout << "result is " << future.get() << '\n';
}
```

## std::packaged_task

```C++
#include <iostream>
#include <thread>
#include <future>

void callLambda() {
    std::packaged_task<int(int, int)>task([](int a, int b) {
            std::cout << std::this_thread::get_id() << '\n';
            return a * b; });
    std::future<int> ret = task.get_future();

    task(1, 2);

    std::cout << ret.get() << '\n';
}

void thread_task() {
    std::packaged_task<int(int, int)>task([](int a, int b) {
            std::cout << std::this_thread::get_id() << '\n';
            return a * b; });
    std::future<int> ret = task.get_future();

    std::thread t(std::move(task), 10, 10);
    
    std::cout << ret.get() << '\n';
    t.join();
}

int main() {
    
    std::cout << std::this_thread::get_id() << '\n';
    callLambda(); // 主线程运行task
    std::cout << "-----------------\n";
    thread_task();
    return 0;
}
```

## std::promise

```C++
#include <iostream>
#include <thread>
#include <future>

void mythread(std::promise<int>& pr, int n) {
    int t{};
    for (int i = 0; i < n; i++)
            t += i;
    pr.set_value(t);
}

int main() {
    std::promise<int>pr;
    std::thread t{mythread, std::ref(pr), 10};
    std::future<int> fu = pr.get_future();
    
    auto resurt = fu.get();
    std::cout << resurt << '\n';

    t.join();
    return 0;
}
#include <iostream>
#include <atomic>
#include <thread>
#include <future>

int main() {
    std::promise<void> ready_promise;
    std::future<void> n = ready_promise.get_future();
    std::thread t{ [&] {
            n.wait();
            std::cout << "abc\n";

    }};
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "main\n";
    std::cin.get();
    ready_promise.set_value();
    t.join();
    return 0;
}
```

## 原子std::atomic

```C++
#include <iostream>
#include <atomic>
#include <thread>

int main() {
    std::atomic<int> n{ 1 };
    std::atomic_bool flag;
    // 原子操作
// 如果是非原子操作，多线程中可能会被打断
    n += 5;
    n++;
    n--;
    
    return 0;
}
```