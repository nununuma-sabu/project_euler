#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/**
 * Project Euler Problem 83:
 * グリッド上を上下左右に移動できる
 * 左上のマスから右下のマスまでの最短距離を求める
 */

#define MAX_N 200            // 想定される最大グリッドサイズ（余裕を持たせています）
#define MAX_HEAP_NODES 1000000 // ヒープの最大サイズ

// 優先度付きキューのノード構造体
typedef struct {
    long long cost;
    int r;
    int c;
} Node;

Node heap[MAX_HEAP_NODES];
int heap_size = 0;

// Min-Heapへのプッシュ
void push(long long cost, int r, int c) {
    int i = heap_size++;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p].cost <= cost) break;
        heap[i] = heap[p];
        i = p;
    }
    heap[i].cost = cost;
    heap[i].r = r;
    heap[i].c = c;
}

// Min-Heapからのポップ
Node pop() {
    Node min_node = heap[0];
    Node last = heap[--heap_size];
    int i = 0;
    while (i * 2 + 1 < heap_size) {
        int left = i * 2 + 1;
        int right = i * 2 + 2;
        int min_child = left;
        if (right < heap_size && heap[right].cost < heap[left].cost) {
            min_child = right;
        }
        if (last.cost <= heap[min_child].cost) break;
        heap[i] = heap[min_child];
        i = min_child;
    }
    heap[i] = last;
    return min_node;
}

int main() {
    // 実行時間計測開始
    clock_t start_time = clock();

    // ファイル読み込み
    FILE *f = fopen("matrix.txt", "r");
    if (!f) {
        fprintf(stderr, "ファイル 'matrix.txt' を開けませんでした。\n");
        return 1;
    }

    // 動的配列の代わりに、十分に大きい静的配列を確保
    static int grid[MAX_N][MAX_N];
    static long long dist[MAX_N][MAX_N];
    
    char line[8192]; // 1行の最大文字数
    int n = 0;

    // カンマ区切りでパースしてグリッドを構築
    while (fgets(line, sizeof(line), f)) {
        int c = 0;
        char *tok = strtok(line, ",\r\n");
        while (tok) {
            grid[n][c++] = atoi(tok);
            tok = strtok(NULL, ",\r\n");
        }
        n++;
    }
    fclose(f);

    // 距離配列を巨大な値(LLONG_MAX)で初期化
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dist[i][j] = LLONG_MAX;
        }
    }

    // ダイクストラ法による探索
    dist[0][0] = grid[0][0];
    push(grid[0][0], 0, 0);

    // 上、下、左、右への移動用配列
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    long long answer = -1;

    while (heap_size > 0) {
        Node current = pop();
        long long cost = current.cost;
        int r = current.r;
        int c = current.c;

        if (cost != dist[r][c]) {
            continue;
        }

        if (r == n - 1 && c == n - 1) {
            answer = cost; // ゴールの最短が確定
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            
            if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
                long long ncst = cost + grid[nr][nc];
                if (ncst < dist[nr][nc]) {
                    dist[nr][nc] = ncst;
                    push(ncst, nr, nc);
                }
            }
        }
    }

    if (answer != -1) {
        printf("Answer: %lld\n", answer);
    } else {
        printf("到達不能（通常この問題では起きません）\n");
    }

    // 実行時間計測終了
    clock_t end_time = clock();

    // 経過時間（ミリ秒）
    double elapsed_ms = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;

    // 結果表示
    printf("Execution Time: %.3f ms\n", elapsed_ms);

    return 0;
}
