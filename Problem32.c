#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/**
 * 文字列が "1" から "9" までの数字を各1回ずつ含んでいるか判定する
 */
bool is_1_to_9_pandigital(const char *s) {
    if (strlen(s) != 9) {
        return false;
    }

    int mask = 0;
    for (int i = 0; s[i] != '\0'; i++) {
        // '0'が含まれていたらNG
        if (s[i] == '0') {
            return false;
        }

        int digit = s[i] - '0';
        int bit = 1 << (digit - 1);

        // すでにその数字が登場していたらNG (重複)
        if (mask & bit) {
            return false;
        }
        
        // ビットを立てる
        mask |= bit;
    }

    // 1~9全てのビットが立っていればOK (2^9 - 1 = 511 = 0x1FF)
    return mask == 0x1FF;
}

long long solve() {
    // 積 c は4桁 (最大9999) なので、インデックス管理用の配列を用意
    bool seen_products[10000] = {false};
    long long total_sum = 0;
    char buffer[32]; // 連結文字列用バッファ

    // パターン1: (1桁) * (4桁) = (4桁)
    for (int a = 1; a < 10; a++) {
        for (int b = 1234; b < 9877; b++) {
            int c = a * b;
            // cが4桁かチェック
            if (c >= 1000 && c <= 9999) {
                // 文字列として連結 "a" + "b" + "c"
                sprintf(buffer, "%d%d%d", a, b, c);
                
                if (is_1_to_9_pandigital(buffer)) {
                    // まだセットに追加されていない場合のみ加算
                    if (!seen_products[c]) {
                        seen_products[c] = true;
                        total_sum += c;
                    }
                }
            }
        }
    }

    // パターン2: (2桁) * (3桁) = (4桁)
    for (int a = 12; a < 100; a++) {
        for (int b = 123; b < 988; b++) {
            int c = a * b;
            // cが4桁かチェック
            if (c >= 1000 && c <= 9999) {
                sprintf(buffer, "%d%d%d", a, b, c);
                
                if (is_1_to_9_pandigital(buffer)) {
                    if (!seen_products[c]) {
                        seen_products[c] = true;
                        total_sum += c;
                    }
                }
            }
        }
    }

    return total_sum;
}

int main() {
    printf("%lld\n", solve());
    return 0;
}
