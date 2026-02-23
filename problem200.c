#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned long long ull;

// ----------------------------------------
// 高速な素数判定 (Miller-Rabin)
// ----------------------------------------

// 64bit整数の乗算におけるオーバーフローを防ぐ
ull mul_mod(ull a, ull b, ull m) {
    return (ull)((__uint128_t)a * b % m);
}

// 繰り返し二乗法
ull pow_mod(ull base, ull exp, ull mod) {
    ull res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = mul_mod(res, base, mod);
        base = mul_mod(base, base, mod);
        exp /= 2;
    }
    return res;
}

// Miller-Rabin素数判定 (64bit整数対応)
bool is_prime(ull n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;
    
    ull d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }
    
    // 64bit整数の範囲なら、以下の底を試せば確実
    static const ull bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (int i = 0; i < 12; i++) {
        ull a = bases[i];
        if (n <= a) break;
        ull x = pow_mod(a, d, n);
        if (x == 1 || x == n - 1) continue;
        
        bool composite = true;
        for (int r = 1; r < s; r++) {
            x = mul_mod(x, x, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) return false;
    }
    return true;
}

// ----------------------------------------
// 問題特有の条件判定
// ----------------------------------------

// 連続する部分文字列に "200" を持つか
bool has_200(ull n) {
    while (n >= 200) {
        if (n % 1000 == 200) return true;
        n /= 10;
    }
    return false;
}

// 耐素数性 (prime-proof) の判定
bool is_prime_proof(ull n) {
    ull pow10 = 1;
    ull temp = n;
    while (temp > 0) {
        int current_digit = temp % 10;
        for (int d = 0; d <= 9; d++) {
            if (d == current_digit) continue;
            // 最上位桁を0にする変更は桁数が変わってしまうためスキップ
            if (temp < 10 && d == 0) continue; 
            
            ull modified = n - current_digit * pow10 + d * pow10;
            // 1つでも素数になれば、耐素数性はない
            if (is_prime(modified)) return false;
        }
        temp /= 10;
        pow10 *= 10;
    }
    return true; // どの桁を変えても素数にならなかった
}

// ----------------------------------------
// 優先度付きキュー (最小ヒープ)
// ----------------------------------------
typedef struct {
    ull val;    // スキューブの値 (p^2 * q^3)
    int p_idx;  // pの素数インデックス
    int q_idx;  // qの素数インデックス
} Element;

#define HEAP_CAPACITY 50000
Element heap[HEAP_CAPACITY];
int heap_size = 0;

void push(Element e) {
    int i = heap_size++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p].val <= e.val) break;
        heap[i] = heap[p];
        i = p;
    }
    heap[i] = e;
}

Element pop() {
    Element ret = heap[0];
    Element e = heap[--heap_size];
    int i = 0;
    while (i * 2 + 1 < heap_size) {
        int left = i * 2 + 1;
        int right = i * 2 + 2;
        int min_child = left;
        if (right < heap_size && heap[right].val < heap[left].val) {
            min_child = right;
        }
        if (heap[min_child].val >= e.val) break;
        heap[i] = heap[min_child];
        i = min_child;
    }
    heap[i] = e;
    return ret;
}

// ----------------------------------------
// 素数生成 (エラトステネスの篩)
// ----------------------------------------
#define MAX_PRIME 1000000
bool sieve[MAX_PRIME + 1];
int primes[MAX_PRIME];
int prime_count = 0;

void generate_primes() {
    for (int p = 2; p <= MAX_PRIME; p++) {
        if (!sieve[p]) {
            primes[prime_count++] = p;
            for (int i = p * 2; i <= MAX_PRIME; i += p) {
                sieve[i] = true;
            }
        }
    }
}

// ----------------------------------------
// メイン処理
// ----------------------------------------
int main() {
    // 実行時間計測スタート
    clock_t start_time = clock();
    
    // 1. 素数の準備
    generate_primes();
    
    // 2. ヒープの初期化
    // 各qに対して、最小のpを組み合わせてヒープに投入
    // 十分な数のq (ここでは2万個) を初期セットアップ
    int init_q_count = 20000; 
    for (int j = 0; j < init_q_count && j < prime_count; j++) {
        int p_idx = 0;
        if (p_idx == j) p_idx++; // pとqは異なる素数
        
        ull q = primes[j];
        ull p = primes[p_idx];
        
        __uint128_t val128 = (__uint128_t)p * p * q * q * q;
        if (val128 <= ~(0ULL)) { // オーバーフローチェック
            Element e = {(ull)val128, p_idx, j};
            push(e);
        }
    }
    
    // 3. 探索開始
    int count = 0;
    
    while (heap_size > 0) {
        Element e = pop();
        
        // 条件チェック
        if (has_200(e.val)) {
            if (is_prime_proof(e.val)) {
                count++;
                if (count == 200) {
                    printf("Answer: %llu\n", e.val);
                    break;
                }
            }
        }
        
        // 次のpを組み合わせてヒープに投入
        int next_p_idx = e.p_idx + 1;
        if (next_p_idx == e.q_idx) next_p_idx++;
        
        if (next_p_idx < prime_count) {
            ull q = primes[e.q_idx];
            ull p = primes[next_p_idx];
            __uint128_t val128 = (__uint128_t)p * p * q * q * q;
            
            // 64bitに収まる範囲のみ追加
            if (val128 <= ~(0ULL)) {
                Element next_e = {(ull)val128, next_p_idx, e.q_idx};
                push(next_e);
            }
        }
    }
    
    // 実行時間計測エンド
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Execution Time: %.4f seconds\n", time_spent);
    
    return 0;
}
