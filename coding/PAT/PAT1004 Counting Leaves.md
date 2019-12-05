# 1004 Counting Leaves
A family hierarchy is usually presented by a pedigree tree. Your job is to count those family members who have no child.
## Input
Each input file contains one test case. Each case starts with a line containing 0 < N < 100, the number of nodes in a tree, and M (< N), the number of non-leaf nodes. Then M lines follow, each in the format:
ID K ID[1] ID[2] … ID[K]  
where ID is a two-digit number representing a given non-leaf node, K is the number of its children, followed by a sequence of two-digit ID’s of its children. For the sake of simplicity, let us fix the root ID to be 01.
## Output
For each test case, you are supposed to count those family members who have no child for every seniority level starting from the root. The numbers must be printed in a line, separated by a space, and there must be no extra space at the end of each line.  
The sample case represents a tree with only 2 nodes, where 01 is the root and 02 is its only child. Hence on the root 01 level, there is 0 leaf node; and on the next level, there is 1 leaf node. Then we should output “0 1” in a line.
## Sample Input
>2 1  
01 1 02
## Sample Output
>0 1
## 笔记
1. 如果变量较小，使用short
2. 不能使用引用的数组，引用的底层实现是指针常量 即：`int *const p;`
3. 本代码知识点：树的遍历方法，输出末尾不带空格的数组
## 方法一 先建树 后遍历（遍历求每个子代的genId）
``` cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <queue>

struct member
{
    // short id;
    unsigned genId;
    bool has_child = false;
    std::vector<member *> childs;
};

int main()
{
    using namespace std;

    int N, M;
    cin >> N >> M;
    unordered_map<short, member> map;

    short id;
    unsigned childN;
    for (size_t i = 0; i < M; ++i)
    {
        cin >> id >> childN;
        map[id].has_child = true;
        short childId;
        for (size_t j = 0; j < childN; ++j)
        {
            cin >> childId;
            map[id].childs.emplace_back(&map[childId]);
        }
    }

    queue<member *> q;
    map[01].genId = 0;
    q.push(&map[01]);
    unsigned genMax = 0;
    while (!q.empty())
    {
        auto &cur = q.front();
        genMax = max(genMax, cur->genId);
        // if(cur.has_child)
        for (auto &each_child : cur->childs)
        {
            each_child->genId = cur->genId + 1;
            q.push(each_child);
        }
        q.pop();
    }

    vector<unsigned> ans(genMax + 1, 0);
    for (auto &person : map)
    {
        if (!person.second.has_child)
            ++ans[person.second.genId];
    }
    cout << ans[0];
    for(size_t i = 1; i <ans.size(); ++i)
    {
        cout << ' ' << ans[i];
    }
    cout << endl;
    return 0;
}
```
## 方法二 先将个节点的父子关系（即输入）原封不动地存到stringstream中 再遍历
```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <sstream>
#include <stack>
struct member
{
    // short id;
    unsigned genId;
    bool has_child = false;
};
int main()
{
    using namespace std;

    int N, M;
    cin >> N >> M;
    unordered_map<short, stringstream> srcInfo;
    unordered_map<short, member> map;

    short id;
    for (size_t i = 0; i < M; ++i)
    {
        // 下面这句话的调用顺序是未定义的，可能会先计算机get内的内容
        // (cin >> id).get(*srcInfo[id].rdbuf());
        cin >> id;
        cin.get(*srcInfo[id].rdbuf());
        map[id].has_child = true;
    }

    stack<short> s;
    map[01].genId = 0;
    if (map[01].has_child)
        s.push(01);
    unsigned genMax = 0;
    while (!s.empty())
    {
        // 使用stack的话记得要先把栈顶弹出，之后才能压入
        // tip：使用queue更好！
        auto curId = s.top();
        s.pop();
        auto &curSstream = srcInfo[curId];
        unsigned childN;
        curSstream >> childN;
        short childId;
        for (size_t i = 0; i < childN; ++i)
        {
            curSstream >> childId;
            map[childId].genId = map[curId].genId + 1;
            genMax = max(genMax, map[childId].genId);
            if (map[childId].has_child)
                s.push(childId);
        }
    }

    vector<unsigned> ans(genMax + 1, 0);
    for (auto &e : map)
    {
        if (!e.second.has_child)
            ++ans[e.second.genId];
    }

    cout << ans[0];
    for (size_t i = 1; i < ans.size(); ++i)
    {
        cout << ' ' << ans[i];
    }
    cout << endl;

    return 0;
}
```
