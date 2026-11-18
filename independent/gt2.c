/* gt2.c - exhaustive search for 2-stage strict group testing, up to d=2 defectives.
 *
 * Usage:
 *   gt2 search n T k [first]   exhaustive search for a design with k stage-1 pools,
 *                              q = T-k stage-2 pools per outcome; first=1 stops at first design
 *   gt2 chi                    reads "m N mask_1..mask_N" from stdin, prints chromatic number
 *                              of the conflict graph for elements 0..m-1 and candidate sets masks
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAXN 16
#define MAXK 10
#define MAXQ 16
#define MAXV 4608
#define VW (MAXV / 64)
#define MAXSETS 1024

static int n_items, K, Tot, Q;
static int sig[MAXN];
static int cnt1[MAXK + 1];
static long long nodes = 0, checks = 0, sols = 0, colnodes = 0, colcalls = 0, bigv = 0;
static int stop_first = 0, stop_flag = 0;

static uint32_t cmask[MAXSETS];
static int elems[MAXN], ne;
static int vset[MAXV], vitem[MAXV];
static int nV, nQ;
static uint64_t A[MAXV][VW];
static int degv[MAXV];
static uint32_t forb[MAXV];
static uint16_t ncol[MAXV][MAXQ];
static int colr[MAXV];

/* ---------- exact DSATUR backtracking colouring with "next unused colour" rule ---------- */
static int dsatur_rec(int ncolored, int maxused)
{
    colnodes++;
    if (ncolored == nV) return 1;
    int best = -1, bs = -1, bd = -1;
    for (int v = 0; v < nV; v++) {
        if (colr[v] >= 0) continue;
        int s = __builtin_popcount(forb[v]);
        if (s > bs || (s == bs && degv[v] > bd)) { best = v; bs = s; bd = degv[v]; }
    }
    if (bs >= nQ) return 0; /* vertex has no admissible colour left */
    int nw = (nV + 63) / 64;
    int top = maxused + 1;
    if (top > nQ - 1) top = nQ - 1;
    for (int c = 0; c <= top; c++) {
        if ((forb[best] >> c) & 1u) continue;
        colr[best] = c;
        for (int w = 0; w < nw; w++) {
            uint64_t b = A[best][w];
            while (b) {
                int u = w * 64 + __builtin_ctzll(b);
                b &= b - 1;
                if (ncol[u][c]++ == 0) forb[u] |= 1u << c;
            }
        }
        int nm = maxused > c ? maxused : c;
        if (dsatur_rec(ncolored + 1, nm)) return 1;
        for (int w = 0; w < nw; w++) {
            uint64_t b = A[best][w];
            while (b) {
                int u = w * 64 + __builtin_ctzll(b);
                b &= b - 1;
                if (--ncol[u][c] == 0) forb[u] &= ~(1u << c);
            }
        }
        colr[best] = -1;
    }
    return 0;
}

static int colorable(int q)
{
    if (nV == 0) return 1;
    if (q == 0) return 0;
    colcalls++;
    if (nV > bigv) bigv = nV;
    nQ = q;
    for (int v = 0; v < nV; v++) {
        colr[v] = -1;
        forb[v] = 0;
        memset(ncol[v], 0, sizeof(uint16_t) * (size_t)q);
    }
    return dsatur_rec(0, -1);
}

/* conflict graph: (C,v) ~ (C',v') iff v in C' or v' in C */
static void build_graph(void)
{
    int nw = (nV + 63) / 64;
    for (int x = 0; x < nV; x++) memset(A[x], 0, sizeof(uint64_t) * (size_t)nw);
    for (int x = 0; x < nV; x++)
        for (int y = x + 1; y < nV; y++) {
            int adj = ((cmask[vset[y]] >> vitem[x]) & 1u) | ((cmask[vset[x]] >> vitem[y]) & 1u);
            if (adj) {
                A[x][y >> 6] |= 1ULL << (y & 63);
                A[y][x >> 6] |= 1ULL << (x & 63);
            }
        }
    for (int x = 0; x < nV; x++) {
        int d = 0;
        for (int w = 0; w < nw; w++) d += __builtin_popcountll(A[x][w]);
        degv[x] = d;
    }
}

/* ---------- candidate structure for outcome W, using items 0..cur-1 ---------- */
static int build_sets(int W, int cur, int cap)
{
    ne = 0;
    for (int i = 0; i < cur; i++)
        if ((sig[i] & ~W) == 0) elems[ne++] = i;
    int N = 0;
    if (W == 0) {
        if (N >= cap) return cap + 1;
        cmask[N++] = 0u;
    }
    for (int a = 0; a < ne; a++)
        if (sig[elems[a]] == W) {
            if (N >= cap) return cap + 1;
            cmask[N++] = 1u << elems[a];
        }
    for (int a = 0; a < ne; a++)
        for (int b = a + 1; b < ne; b++)
            if ((sig[elems[a]] | sig[elems[b]]) == W) {
                if (N >= cap) return cap + 1;
                cmask[N++] = (1u << elems[a]) | (1u << elems[b]);
            }
    return N;
}

/* returns 1 iff r(W) <= Q */
static int check_W(int W, int cur)
{
    checks++;
    int cap = 1 << Q;
    int N = build_sets(W, cur, cap);
    if (N > cap) return 0;          /* necessary: #candidate sets <= 2^q */
    if (N == 0) return 1;           /* r(W)=0 */
    nV = 0;
    for (int c = 0; c < N; c++)
        for (int a = 0; a < ne; a++) {
            int e = elems[a];
            if (!((cmask[c] >> e) & 1u)) {
                if (nV >= MAXV) { fprintf(stderr, "vertex overflow\n"); exit(2); }
                vset[nV] = c;
                vitem[nV] = e;
                nV++;
            }
        }
    if (nV == 0) return 1;
    if (Q == 0) return 0;
    build_graph();
    return colorable(Q);
}

static void print_design(void)
{
    printf("DESIGN n=%d T=%d k=%d q=%d signatures(bitmask):", n_items, Tot, K, Q);
    for (int i = 0; i < n_items; i++) printf(" %d", sig[i]);
    printf("\n   as sets:");
    for (int i = 0; i < n_items; i++) {
        printf(" {");
        int first = 1;
        for (int j = 0; j < K; j++)
            if ((sig[i] >> j) & 1) { printf(first ? "%d" : ",%d", j); first = 0; }
        printf("}");
    }
    printf("\n");
    fflush(stdout);
}

/* ---------- enumeration of nondecreasing signature sequences ---------- */
static void dfs(int d, int lo)
{
    nodes++;
    if ((nodes & ((1LL << 27) - 1)) == 0) {
        fprintf(stderr, "  progress: nodes=%lld checks=%lld sols=%lld\n", nodes, checks, sols);
    }
    if (d == n_items) {
        sols++;
        if (sols == 1) print_design();
        if (stop_first) stop_flag = 1;
        return;
    }
    int full = 1 << K;
    for (int s = lo; s < full && !stop_flag; s++) {
        int j = -1;
        if (s && !(s & (s - 1))) {
            j = __builtin_ctz(s);
            /* symmetry breaking: count of items with signature {j} nonincreasing in j */
            if (j >= 1 && cnt1[j] + 1 > cnt1[j - 1]) continue;
        }
        sig[d] = s;
        if (j >= 0) cnt1[j]++;
        int ok = 1;
        int comp = (full - 1) & ~s;
        int t = comp;
        for (;;) {
            if (!check_W(s | t, d + 1)) { ok = 0; break; }
            if (t == 0) break;
            t = (t - 1) & comp;
        }
        if (ok) dfs(d + 1, s);
        if (j >= 0) cnt1[j]--;
    }
}

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static int chi_mode(void)
{
    int m, N;
    if (scanf("%d %d", &m, &N) != 2) return 1;
    for (int c = 0; c < N; c++) {
        unsigned int x;
        if (scanf("%u", &x) != 1) return 1;
        cmask[c] = x;
    }
    ne = m;
    for (int a = 0; a < m; a++) elems[a] = a;
    nV = 0;
    for (int c = 0; c < N; c++)
        for (int e = 0; e < m; e++)
            if (!((cmask[c] >> e) & 1u)) { vset[nV] = c; vitem[nV] = e; nV++; }
    build_graph();
    for (int q = 0; q <= 15; q++)
        if (colorable(q)) { printf("%d\n", q); return 0; }
    printf("-1\n");
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "chi") == 0) return chi_mode();
    if (argc < 5) {
        fprintf(stderr, "usage: %s search n T k [first] | chi\n", argv[0]);
        return 1;
    }
    n_items = atoi(argv[2]);
    Tot = atoi(argv[3]);
    K = atoi(argv[4]);
    stop_first = (argc > 5) ? atoi(argv[5]) : 0;
    Q = Tot - K;
    if (n_items < 1 || n_items > MAXN || K < 1 || K > MAXK || Q < 0 || Q >= MAXQ || Q > 10 /* cmask[MAXSETS=1024] holds at most 2^Q candidate sets */) {
        fprintf(stderr, "bad parameters\n");
        return 1;
    }
    double t0 = now_sec();
    dfs(0, 0);
    double el = now_sec() - t0;
    printf("RESULT n=%d T=%d k=%d q=%d designs=%lld nodes=%lld checks=%lld colour_calls=%lld colour_nodes=%lld max_vertices=%lld time=%.2fs stop_first=%d complete=%d\n",
           n_items, Tot, K, Q, sols, nodes, checks, colcalls, colnodes, bigv, el, stop_first, stop_flag ? 0 : 1);
    /* Completion flag. This solver has no node limit and never answers "infeasible" on a resource cut-off,
       so a run is COMPLETE unless it was stopped on purpose at the first design (stop_first). */
    printf("ARTEFACT: search log. designs=0 is a NEGATIVE-SEARCH LOG (not an independently checkable certificate); a printed DESIGN is a candidate POSITIVE WITNESS (check it with independent/verify.py)\n");
    if (stop_flag) printf("STATUS: STOPPED AT FIRST DESIGN (stop_first=1; a design exists, search not exhaustive)\n");
    else printf("STATUS: COMPLETE (exhaustive; no node limit in this solver)\n");
    return 0;
}
