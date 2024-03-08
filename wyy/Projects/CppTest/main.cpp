#include<iostream>
#include<algorithm>
#include<cstring>
using namespace std;
int n;
long long a[1005],dp[1005];

int main() {
    while (scanf("%d",&n),n!=0) {
        for (int i = 1; i <= n; i++)cin >> a[i], dp[i] = 0;
        int flag = 0;
        for (int i = 1; i <= n; i++) {
            int j = i + 1;
            if (a[i] < a[j])flag = 1;
        }
        if (flag == 0) {
            cout << a[1] << endl;
            continue;
        }
        for (int i = 1; i <= n; i++) {
            dp[i] = a[i];
            for (int j = 1; j <= i; j++) {
                if (a[i] > a[j]) {
                    dp[i] = max(dp[i], dp[j] + a[i]);
                }
            }
        }
        long long m = -1;
        for (int i = 1; i <= n; i++) {
            m = max(m, dp[i]);
        }
        cout << m << endl;
    }
    return 0;
}