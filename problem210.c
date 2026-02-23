#include <stdio.h>
#include <math.h>
#include <time.h>

typedef unsigned long long ull;

// 浮動小数点誤差を避けるための安全な整数平方根関数
ull isqrt(ull n) {
    if (n == 0) return 0;
    ull root = (ull)sqrt((double)n);
    // 精度保証のための微調整
    while ((root + 1ULL) * (root + 1ULL) <= n) {
        root++;
    }
    while (root * root > n) {
        root--;
    }
    return root;
}

int main() {
    // 実行時間計測スタート
    clock_t start_time = clock();

    ull r = 1000000000ULL;
    ull k = r / 4ULL;

    // 領域1 (Oで鈍角) と 領域2 (Cで鈍角) の格子点の合計: 16k^2 + 8k^2 = 24k^2
    ull ans = 24ULL * k * k;

    // 領域3 (Bで鈍角) の格子点のカウント
    ull limit_r2 = 2ULL * k * k;
    ull c_in = 0;

    // u = 0 の場合の処理 (kが偶数の場合のみ発生)
    if (k % 2 == 0) {
        ull limit_v2 = limit_r2 - 1ULL;
        ull vmax = isqrt(limit_v2);
        c_in += 2ULL * (vmax / 2ULL) + 1ULL;
    }

    // u > 0 の場合を走査 (対称性を利用して後で2倍する)
    ull sum_pos = 0;
    ull start_u = (k % 2 == 0) ? 2ULL : 1ULL;

    for (ull u = start_u; u * u < limit_r2; u += 2ULL) {
        ull limit_v2 = limit_r2 - u * u - 1ULL;
        ull vmax = isqrt(limit_v2);
        
        ull count = 0;
        if (k % 2 == 0) {
            count = 2ULL * (vmax / 2ULL) + 1ULL;
        } else {
            count = 2ULL * ((vmax + 1ULL) / 2ULL);
        }
        sum_pos += count;
    }

    c_in += 2ULL * sum_pos;
    ans += c_in;

    // O, B, C が一直線上(y=x)に並んでしまう縮退ケースを除外
    ans -= (k - 1ULL);

    // 実行時間計測エンド
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("N(%llu) = %llu\n", r, ans);
    printf("Execution Time: %.4f seconds\n", time_spent);

    return 0;
}
