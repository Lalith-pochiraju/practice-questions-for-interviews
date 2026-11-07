#include <stdio.h>

int count[15];
long len[15][15], cr[15][15], dt[15][15];

void build(int id, int k, long x[], long y[]) {
    long ex[15], ey[15];
    int m = 0;

    for (int i = 0; i < k; i++) {
        long dx = x[(i + 1) % k] - x[i];
        long dy = y[(i + 1) % k] - y[i];
        if (m > 0 && ex[m - 1] * dy - ey[m - 1] * dx == 0 && ex[m - 1] * dx + ey[m - 1] * dy > 0) {
            ex[m - 1] += dx;
            ey[m - 1] += dy;
        } else {
            ex[m] = dx;
            ey[m] = dy;
            m++;
        }
    }

    if (m > 1 && ex[m - 1] * ey[0] - ey[m - 1] * ex[0] == 0 && ex[m - 1] * ex[0] + ey[m - 1] * ey[0] > 0) {
        ex[0] += ex[m - 1];
        ey[0] += ey[m - 1];
        m--;
    }

    count[id] = m;
    for (int i = 0; i < m; i++) {
        int j = (i + 1) % m;
        len[id][i] = ex[i] * ex[i] + ey[i] * ey[i];
        cr[id][i] = ex[i] * ey[j] - ey[i] * ex[j];
        dt[id][i] = ex[i] * ex[j] + ey[i] * ey[j];
    }
}

int match(int a, int b) {
    int m = count[a];
    if (m != count[b]) return 0;

    for (int s = 0; s < m; s++) {
        int ok = 1;
        for (int i = 0; i < m; i++) {
            int j = (i + s) % m;
            if (len[a][i] != len[b][j] || cr[a][i] != cr[b][j] || dt[a][i] != dt[b][j]) {
                ok = 0;
                break;
            }
        }
        if (ok) return 1;
    }
    return 0;
}

int main() {
    int n;
    scanf("%d", &n);

    for (int p = 0; p < n; p++) {
        int k;
        long x[15], y[15];
        scanf("%d", &k);
        for (int i = 0; i < k; i++) {
            scanf("%ld %ld", &x[i], &y[i]);
        }
        build(p, k, x, y);
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (match(i, j)) {
                printf("%d %d", i + 1, j + 1);
                return 0;
            }
        }
    }
    return 0;
}
