#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>  // 時間計測用に追加

/**
 * https://projecteuler.net/problem=50
 * https://odz.sakura.ne.jp/projecteuler/?Problem+50
 * 100万未満の素数を連続する素数の和で表したときに最長になるのはどの素数か?
 */

#define LIMIT 1000000

// グローバル変数
bool is_prime[LIMIT];
int primes[100000];
int64_t prefix[100000];

void sieve(int n, int *prime_count) {
    for (int i = 0; i < n; i++) is_prime[i] = true;
    if (n > 0) is_prime[0] = false;
    if (n > 1) is_prime[1] = false;

    int limit_sqrt = (int)sqrt(n - 1);
    for (int p = 2; p <= limit_sqrt; p++) {
        if (is_prime[p]) {
            for (int i = p * p; i < n; i += p) {
                is_prime[i] = false;
            }
        }
    }

    *prime_count = 0;
    for (int i = 0; i < n; i++) {
        if (is_prime[i]) {
            primes[*prime_count] = i;
            (*prime_count)++;
        }
    }
}

void solve(int limit, int *best_sum, int *best_len) {
    int prime_count = 0;
    sieve(limit, &prime_count);

    prefix[0] = 0;
    int64_t s = 0;
    for (int i = 0; i < prime_count; i++) {
        s += primes[i];
        prefix[i + 1] = s;
    }

    *best_sum = 0;
    *best_len = 0;

    int max_len = 0;
    for (int i = 1; i <= prime_count; i++) {
        if (prefix[i] >= limit) {
            max_len = i - 1;
            break;
        }
    }
    if (max_len == 0) max_len = prime_count;

    for (int L = max_len; L > 0; L--) {
        bool found = false;
        for (int i = 0; i <= prime_count - L; i++) {
            int64_t total = prefix[i + L] - prefix[i];
            
            if (total >= limit) break;
            
            if (is_prime[total]) {
                *best_sum = (int)total;
                *best_len = L;
                found = true;
                break;
            }
        }
        if (found) break;
    }
}

int main() {
    int ans = 0;
    int length = 0;
    
    // 計測開始
    clock_t start_time = clock();
    
    solve(LIMIT, &ans, &length);
    
    // 計測終了
    clock_t end_time = clock();
    
    // 経過時間をミリ秒(ms)に変換
    double elapsed_ms = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
    
    printf("Answer: %d\n", ans);       // 997651 答え
    printf("Length: %d\n", length);    // 543個の連続した素数の和で表せる
    printf("Execution Time: %.3f ms\n", elapsed_ms); // 実行時間を表示
    
    return 0;
}
