# PAT1002 A+B for Polynomials
This time, you are supposed to find A+B where A and B are two polynomials.
## Input
Each input file contains one test case. Each case occupies 2 lines, and each line contains the information of a polynomial: K N1 aN1 N2 aN2 … NK aNK, where K is the number of nonzero terms in the polynomial, Ni and aNi (i=1, 2, …, K) are the exponents and coefficients, respectively. It is given that 1 <= K <= 10, 0 <= NK < … < N2 < N1 <=1000.
## Output
For each test case you should output the sum of A and B in one line, with the same format as the input. Notice that there must be NO extra space at the end of each line. Please be accurate to 1 decimal place.
## Sample Input
>2 1 2.4 0 3.2  
2 2 1.5 1 0.5
## Sample Output
>3 2 1.5 1 2.9 0 3.2  
## 笔记
1. 判断浮点型是否为`fabs(e.second - 0) <numeric_limits<double>().epsilon()*10`
2. 小数点保留两位：`std::fixed << setprecision(1)`
3. 利用了数据结构map
```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <iomanip>
#include <functional>
#include <cmath>

int main()
{
    using namespace std;

    int n;
    cin >> n;
    double coef;
    int expo;
    map<int, double, greater<int>> poly;
    for (size_t i = 0; i < n; ++i)
    {
        cin >> expo >> coef;
        poly[expo] = coef;
    }
    cin >> n;
    for (size_t i = 0; i < n; ++i)
    {
        cin >> expo >> coef;
        poly[expo] += coef;
    }
    int nonzero = 0;
    auto epsi = numeric_limits<double>().epsilon() *4;

    for (const auto &e : poly)
    {
        if (fabs(e.second - 0) > epsi)
            ++nonzero;
    }
    cout << nonzero;
    for (const auto &e : poly)
    {
        if (fabs(e.second - 0) > epsi)
        {
            cout << ' ' << e.first << ' ' << std::fixed << setprecision(1) << e.second;
        }
    }
    return 0;
}
```
