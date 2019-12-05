/**货币系统---背包问题
题目描述
母牛们不但创建了他们自己的政府而且选择了建立了自己的货币系统。
[In their own rebellious way],他们对货币的数值感到好奇。
传统地，一个货币系统是由1,5,10,20 或 25,50, 和 100的单位面值组成的。
母牛想知道有多少种不同的方法来用货币系统中的货币来构造一个确定的数值。
举例来说, 使用一个货币系统 {1,2,5,10,...}产生 18单位面值的一些可能的方法是:18x1, 9x2, 8x2+2x1, 3x5+2+1,等等其它。
写一个程序来计算有多少种方法用给定的货币系统来构造一定数量的面值。
保证总数将会适合long long (C/C++) 和 Int64 (Free Pascal)。

输入
输入包含多组测试数据
货币系统中货币的种类数目是 V 。 (1<= V<=25)
要构造的数量钱是 N 。 (1<= N<=10,000)
第 1 行:   二整数， V 和 N
第 2 ..V+1行： 可用的货币 V 个整数。

输出
单独的一行包含那个可能的构造的方案数。 

样例输入
3 10
1 2 5

样例输出
10
 */
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <numeric>

int main()
{
    using namespace std;
    int V, N;
    while (cin >> N >> V)
    {
        vector<long long> dp(V+1);
        vector<int> w;
        w.reserve(N);
        copy_n(istream_iterator<int>(cin), N, back_inserter(w));
         
        for(size_t i = 0; i <= V; i++)
        {
            // dp[0] 为1已经由下式包含
            dp[i] = i % w[0]==0?1:0;
        }
                
        for(size_t i = 1; i < N; i++)
        {
            for(size_t v = w[i]; v <=V; ++v)
            {
                dp[v] = dp[v] + dp[v - w[i]];
            }
        }

        cout << dp[V] << endl;
    }
    return 0;
}
