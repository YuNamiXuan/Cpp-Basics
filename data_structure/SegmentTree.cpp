#include <iostream>
using namespace std;

#define lc p << 1
#define rc p << 1 | 1
#define N 500005

int n, w[N];

struct node {
    int l, r, sum, add;
} tr[N * 4];

// Push up (update parent node)
void pushup(int p) {
    tr[p].sum = tr[lc].sum + tr[rc].sum; 
}

// Push down (lazy propagation)
void pushdown(int p) {
    int add = tr[p].add;
    if (add) {
        tr[lc].sum += (tr[lc].r - tr[lc].l + 1) * add;
        tr[rc].sum += (tr[rc].r - tr[rc].l + 1) * add;
        tr[lc].add += add;
        tr[rc].add += add; 
        tr[p].add = 0;
    }
}

// Build segment tree
void build(int p, int l, int r) {
    tr[p] = {l, r, w[l], 0};
    if (l == r) return;
    int m = l + r >> 1;
    build(lc, l, m);
    build(rc, m+1, r);
    pushup(p); 
}

// Point update
void update(int p, int x, int k) {
    if (tr[p].l == x && tr[p].r == x) {
        tr[p].sum += k;
        return;
    }
    pushdown(p);
    int m = tr[p].l + tr[p].r >> 1;
    if (x <= m) {
        update(lc, x, k);
    } else {
        update(rc, x, k);
    }
    pushup(p);
}

// Range query
int query(int p, int x, int y) {
    if (tr[p].l >= x && tr[p].r <= y) {
        return tr[p].sum;
    }
    pushdown(p);
    int m = tr[p].l + tr[p].r >> 1;
    int sum = 0;
    if (x <= m) sum += query(lc, x, y);
    if (y > m) sum += query(rc, x, y);
    return sum;
}

// Range update
void update(int p, int x, int y, int k) {
    if (tr[p].l >= x && tr[p].r <= y) {
        tr[p].sum += (tr[p].r - tr[p].l + 1) * k;
        tr[p].add += k;
        return;
    }
    pushdown(p);
    int m = tr[p].l + tr[p].r >> 1;
    if (x <= m) update(lc, x, y, k);
    if (y > m) update(rc, x, y, k);
    pushup(p);
}

int main() {
    // Test data
    n = 5;
    w[1] = 1;
    w[2] = 3;
    w[3] = 5;
    w[4] = 7;
    w[5] = 9;

    // Build segment tree
    build(1, 1, n);

    // Test 1: Initial range query
    cout << "Initial array: [1, 3, 5, 7, 9]" << endl;
    cout << "Query range [2,4] sum: " << query(1, 2, 4) << " (Expected: 15)" << endl;

    // Test 2: Point update
    update(1, 3, 10); // w[3] += 10
    cout << "\nPoint update: w[3] += 10" << endl;
    cout << "Query range [2,4] sum: " << query(1, 2, 4) << " (Expected: 25)" << endl;

    // Test 3: Range update
    update(1, 1, 3, 5); // [1,3] += 5
    cout << "\nRange update: [1,3] += 5" << endl;
    cout << "Query range [1,5] sum: " << query(1, 1, 5) << " (Expected: 50)" << endl;
    cout << "Detailed array state:" << endl;
    for (int i = 1; i <= n; i++) {
        cout << "w[" << i << "] = " << query(1, i, i) << endl;
    }

    return 0;
}