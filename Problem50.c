#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>  // 時間計測用

/**
 * Project Euler Problem 50:
 * 「100万未満の素数を、連続する素数の和で表したとき、
 * その連続項数（長さ）が最大になる素数は何か？」
 */

#define LIMIT 1000000

// is_prime[x] = x が素数なら true（0..LIMIT-1 までをふるいで作る）
bool is_prime[LIMIT];

// ふるいで列挙した素数を格納する配列（LIMIT 未満の素数個数は約 78498 なので 100000 あれば十分）
int primes[100000];

// prefix[k] = primes[0] + ... + primes[k-1]（先頭からの累積和）
// 区間和を O(1) で求めるために使う。和は int を超える可能性があるので int64_t
int64_t prefix[100000];

/**
 * エラトステネスのふるいで [0, n) の素数表 is_prime を作り、
 * 素数を primes 配列に詰め、個数を prime_count に返す。
 */
void sieve(int n, int *prime_count) {
    // まず全て素数候補(true)として初期化し、0 と 1 は素数ではないので false
    for (int i = 0; i < n; i++) is_prime[i] = true;
    if (n > 0) is_prime[0] = false;
    if (n > 1) is_prime[1] = false;

    // p*p から消していけばよいので、p は sqrt(n-1) まで見れば十分
    int limit_sqrt = (int)sqrt(n - 1);
    for (int p = 2; p <= limit_sqrt; p++) {
        if (is_prime[p]) {
            // p が素数なら、その倍数を合成数としてマーキング
            for (int i = p * p; i < n; i += p) {
                is_prime[i] = false;
            }
        }
    }

    // is_prime を元に素数一覧 primes[] を作る
    *prime_count = 0;
    for (int i = 0; i < n; i++) {
        if (is_prime[i]) {
            primes[*prime_count] = i;
            (*prime_count)++;
        }
    }
}

/**
 * limit 未満で
 * 「連続する素数の和で表せる素数」のうち、連続長が最大のものを探す。
 *
 * best_sum: 条件を満たす素数（最大長のもの）
 * best_len: そのときの連続項数
 */
void solve(int limit, int *best_sum, int *best_len) {
    int prime_count = 0;

    // 1) limit 未満の素数判定表と素数リストを作る
    sieve(limit, &prime_count);

    // 2) 素数の累積和 prefix を作る
    // prefix[0]=0, prefix[i+1]=primes[0]..primes[i] の合計
    prefix[0] = 0;
    int64_t s = 0;
    for (int i = 0; i < prime_count; i++) {
        s += primes[i];
        prefix[i + 1] = s;
    }

    *best_sum = 0;
    *best_len = 0;

    // 3) 「先頭から i 個足した累積和」が limit を超える直前の i-1 を
    //    連続長の上限(max_len)の目安として求める
    //    （長さ L を大きい方から試すための開始点）
    int max_len = 0;
    for (int i = 1; i <= prime_count; i++) {
        if (prefix[i] >= limit) {
            // prefix[i] が初めて limit 以上になった → i 個は長すぎるので i-1 まで
            max_len = i - 1;
            break;
        }
    }
    if (max_len == 0) max_len = prime_count;

    // 4) 長さ L を「最大候補 → 1」へ降順に試す
    //    見つかった時点でそれが最長なので即終了できる
    for (int L = max_len; L > 0; L--) {
        bool found = false;

        // i を開始位置として、primes[i]..primes[i+L-1] の和を調べる
        for (int i = 0; i <= prime_count - L; i++) {
            // 区間和: prefix[i+L] - prefix[i]
            int64_t total = prefix[i + L] - prefix[i];

            // primes は増加、prefix も増加なので、開始位置 i を増やすほど total は基本増える
            // total が limit 以上なら、それ以降はさらに大きくなるため、この L では打ち切り
            if (total >= limit) break;

            // total が素数なら条件成立
            // is_prime は [0, limit) の範囲で定義されているので total < limit が保証されているここで参照できる
            if (is_prime[total]) {
                *best_sum = (int)total;
                *best_len = L;
                found = true;
                break; // この L で見つかったら開始位置探索を終了
            }
        }

        // 長さ L で見つかった = これが最長（L を降順で試しているため）
        if (found) break;
    }
}

int main() {
    int ans = 0;
    int length = 0;

    // 実行時間計測開始
    clock_t start_time = clock();

    // 問題を解く
    solve(LIMIT, &ans, &length);

    // 実行時間計測終了
    clock_t end_time = clock();

    // 経過時間（ミリ秒）
    double elapsed_ms = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;

    // 結果表示
    printf("Answer: %d\n", ans);       // 997651
    printf("Length: %d\n", length);    // 543（543個の連続する素数の和）
    printf("Execution Time: %.3f ms\n", elapsed_ms);

    return 0;
}
