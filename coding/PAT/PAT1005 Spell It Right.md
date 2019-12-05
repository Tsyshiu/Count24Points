# 1005 Spell It Right (20)
Given a non-negative integer N, your task is to compute the sum of all the digits of N, and output every digit of the sum in English.
## Input Specification:
Each input file contains one test case. Each case occupies one line which contains an N (<= 10100).
## Output Specification:
For each test case, output in one line the digits of the sum in English words. There must be one space between two consecutive words, but no extra space at the end of a line.
### Sample Input:
12345
### Sample Output:
one five
## 笔记
1. 字符串常量是存在静态存储区的，标准写法只有`const char *str`，没有`char *str`
2. `char d[]="abc";` 中d的地址也是不能改变的，即`d=str(×)`
3. `const char *a;` a 可变，*a 不可变  
`char b[];`　b 不可变，b[x]可变

``` cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

inline const char * cvt2eng(const char &n)
{
    switch(n)
    {
        case '0':
            return "zero";
        case '1':
            return "one";
        case '2':
            return "two";
        case '3':
            return "three";
        case '4':
            return "four";
        case '5':
            return "five";
        case '6':
            return "six";
        case '7':
            return "seven";
        case '8':
            return "eight";
        case '9':
            return "nine";
    }
}

int main()
{
    using namespace std;

    string Nstr;
    getline(cin, Nstr);
    int ans=0;
    for(const auto &e:Nstr)
    {
        ans += e - '0';
    }

    string ansStr(to_string(ans));
    cout << cvt2eng(ansStr[0]);
    for(size_t i = 1; i <ansStr.size(); ++i)
    {
        cout << ' '<<cvt2eng(ansStr[i]);
    }
    std::cout << std::endl;

    return 0;
}
```
