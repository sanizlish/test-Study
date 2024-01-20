https://codeforces.com/contest/1900/problem/D

前置知识
容斥原理
设$$1 \leq x, y \leq N$$, f(k) 代表最大公约数为k的有序数对(x, y)的个数，求f(1)到f(N)的值
容斥原理来考虑，先求出以k为公约数的数对，再剔除所有以k的倍数为公约数的数对（不重复），剩下的就是以k为最大公约数的数对。即：
$$f(k) = 以k为公约数的数对 - 以k的倍数为公约数的数对个数(不重复)$$
以k的倍数为公约数的数对个数(不重复数对) = 所有以k的倍数为最大公约数的数对之和。
$$f(k) = \lfloor \cfrac{N}{k}\rfloor ^2 - \sum_{i=2}^{i\times k\leq N} f(i \times k)$$
这里的$$\lfloor \cfrac{N}{k}\rfloor$$，小于等于N中能被k整除的个数，数对中(x, y)，x和y都能选这么多个，所以有$$\lfloor \cfrac{N}{k}\rfloor ^2$$个数对以k为公约数
for (long long k = N; k >= 1; k--) {
  f[k] = (N / k) * (N / k);
  for (long long i = k + k; i <= N; i += k) f[k] -= f[i];
}
欧拉反演
所有能整除n的数的欧拉函数之和等于n
$$N = \sum_{d|n}\phi{(d)}$$

题意
求下面这个公式
$$\sum_{i=1}^{n}\sum_{j=i+1}^{n}\sum_{k=j+1}^{n}f(a_i, a_j, a_k)$$

题解1
f函数就是求他们三个值中最小的两个值的gcd
因为每个组合都会被取到，且和顺序无关，所以我们可以先排序
然后变成了
$$\sum_{i=1}^{n}\sum_{j=i+1}^{n}gcd(a_i,a_j)\times(n-j)$$
根据欧拉反演，求gcd的式子可以变成
$$gcd(x,y)=\sum_{d|gcd(x,y)}\phi(d)$$
我们根据上面的式子，因为能同时整除x和y的就是可以整除gcd(x, y)，可以得到
$$\sum_{i=1}^{n}\sum_{j=i+1}^{n}\sum_{d|a_i,d|a_j}\phi(d)\times(n-j)$$
先省略k的影响，然后固定j的位置，当前j的贡献就是 枚举i与当前j的所有公因数然后求出公因数的phi之和，也就是
$$\sum_{i=1}^{j-1}\sum_{d|a_i,d|a_j}\phi(d)$$
上面的求和调换一下顺序，我们要先找到公因数，再枚举i的位置，但是我们换一下思维，我们可以先找j的因数，再来枚举i有无这个因数，不就是公因数了吗
不过先求了当前j与之前的i公因数为d的造成贡献；之前是先枚举了i，然后用这个i找到与j的公因数造成的贡献
所以在当前j的位置，我们可以枚举j的因数d，然后看看枚举i能被d整除的个数
$$\sum_{d|a_j} \sum_{i=1}^{j-1} (a_i\%d == 0) \times \phi(d)$$
我们可以记录一下前面有d是i的因数，可以得到
$$\sum_{d|a_j} cnt(d) \times \phi(d)$$
所以原式变成
$$\sum_{j=1}^{n}(n-j) \sum_{d|a_j}\phi(d)\times cnt(d)$$
这里j不会被i影响所以直接从1开始
```C++
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
 
const int N = 1e5 + 10;
 
int phi[N], primers[N], idx;
bool st[N];
 
void init() {
    phi[1] = 1;
    for (int i = 2; i < N; i++) {
        if(!st[i])
            primers[idx++] = i, phi[i] = i - 1;
        for (int j = 0; j < idx && primers[j] * i < N; j++) {
            int x = primers[j] * i;
            st[x] = true;
            if (i % primers[j] == 0) {
                phi[x] = phi[i] * primers[j];
                break;
            }
            phi[x] = phi[i] * (primers[j] - 1);
        }
    }
}
int n;
int a[N], cnt[N];
 
int main() {
    init();
    int t;
    std::cin >> t;
    while(t--) {
        std::cin >> n;
        for (int i = 1; i <= n; i++)
            std::cin >> a[i];
        std::sort(a + 1, a + n + 1);
        memset(cnt, 0, sizeof cnt);
        long long res = 0;
        for (int i = 1; i <= n; i++) {
            int x = a[i];
            long long ans = 0;
            for (int j = 1; j * j <= x; j++) {
                if (x % j == 0) {
                    ans += 1ll * phi[j] * cnt[j]++;
                    if(j * j != x) 
                        ans += 1ll * phi[x / j] * cnt[x / j]++;
                }
            }
            res += ans * (n - i);
        }
        std::cout << res << std::endl;
    }
    return 0;
```
题解2
我们也可以根据容斥定理先计算gcd为d的数对个数
f代表以d为公因数的数对，所以先确定j的位置的时候，如果j有因数d，之前的i有d的数量记作cnt
我们这个j直接和之前有d的i组合可以得到cnt个组合
$$f(d)=\sum_{j=1}^n cnt(d) \times (a_j \% d == 0)\times(n-i)$$
现在不是特定求d了，而是求aj的因数的时候，d是什么就加到哪个f
$$f(d) += \sum_{j=1}^n cnt(d|a_j) \times(n-i)$$
最后我们要求以d为最大公因数的数对个数
根据容斥定理，要减去以d的倍数为最大公因数的数对个数
$$g(i) = f(i) - \sum_{j=2}^{\cfrac{n}{i}}g(i \times j)$$
这里绝不能减去f(i*j)，f代表的是以d为公因数的数对个数
如果减去f(i*j)那么会减去很多重复的数对，因为d可能为x,y的公因数，那么2*d也可能是xy的公因数
比如(8, 16)，4和8都是(8, 16)的公因数，也是2的倍数，在记录的时候f(4)和f(8)都会记录一次
```c++
#include <iostream>
#include <vector>
#include <algorithm>
 
const int N = 1e5 + 10;
 
std::vector<int> p[N];
 
void init() {
    for (int i = 1; i < N; i++)
        for (int j = i; j < N; j += i)
            p[j].push_back(i);
}
 
long long a[N], f[N], cnt[N];
 
int main() {
    init();
    int t;
    std::cin >> t;
    while(t--) {
        for (int i = 0; i < N; i++)
            f[i] = cnt[i] = 0;
        int n;
        std::cin >> n;
        for (int i = 1; i <= n; i++) {
            std::cin >> a[i];
        }
        std::sort(a + 1, a + 1 + n);
        for (int i = 1; i <= n; i++) {
            for (auto j : p[a[i]]) {
                f[j] += cnt[j] * (n - i);
                cnt[j]++;
            }
        }
        long long res = 0;
        for (int i = N - 1; i >= 1; i--) {
            for (int j = i + i; j < N; j += i) 
                f[i] -= f[j];
 
            res += f[i] * i;
        }
 
        std::cout << res << std::endl;
    }
    return 0;
}
```