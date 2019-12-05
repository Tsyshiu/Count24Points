/*
*本题见算法笔记P134
*木棒切割问题
*切割N根木棒，得到至少K段相同长度的木棒，求长度最长能为多少
*/
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>

static std::vector<int> rods; // 木棒长度
int f(int &mid)
{
    int k = 0;
    for (const auto &rod : rods)
    {
        k += rod / mid;
    }
    return k;
}

int main()
{
    using namespace std;
    int K, N; // K要大于等于1,N要大于等于1
    while (cin >> K >> N)
    {
        rods.clear();
        rods.reserve(N);
        copy_n(istream_iterator<int>(cin), N, back_inserter(rods));

        int left = 1, right = *(max_element(rods.cbegin(), rods.cend()));
        // 寻找第一个满足(只能得到少于K段的程度)条件的
        while (left < right)
        {
            int mid = (left + right) / 2;
            if (f(mid) < K)
                right = mid;
            else
                left = mid + 1;
        }
        int k = left - 1;

        cout << k << endl;
    }
    return 0;
}
