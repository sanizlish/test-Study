#include <iostream>
#include "skiplist.h"

int main() {
     {
        //使用lambda
        auto cmp = [](const std::string& a, const std::string& b) {return a.length() < b.length(); };
        SkipList < std::string, int, decltype(cmp)> list(cmp);
        list.insert("aab", 1321);
        list.insert("hello", 54342);
        list.insert("world", 544);
        for (auto it = list.begin(); it != list.end(); it++) {
            std::cout << it->key << " " << it->value << std::endl;
        }
    }

    std::cout << "==================================" << std::endl;
    
    {
        //使用仿函数
        struct cmp {
            bool operator()(int a, int b) {
                return a > b;
            }
        };
        SkipList < int, int, cmp> list{};
        for (int i = 1; i <= 10; i++)list.insert(rand()%20, rand());
        for (auto it = list.find(10); it != list.end(); it++) {
            std::cout << it->key << " " << it->value << std::endl;
        }
    }

    std::cout << "==================================" << std::endl;

    {
        //默认小于号
        SkipList<int, int>list;
        list.insert(1, 3);
        list.insert(1, 3);
        list.insert(4, 3);
        list.insert(5, 3);
        list.insert(1, 3);
        list.insert(4, 3);
        for (auto it = list.begin(); it != list.end();it++) {
            std::cout << it->key << " " << it->value << std::endl;
        }
       
    }

    {
        //可以添加 T && 实现move语义
        //添加重载 []
    }
    return 0;
}