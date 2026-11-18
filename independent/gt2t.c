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
static int use_types = 0;
static int use_twin_flag = 0;
static int gid_arr[MAXN];
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


/* ---------- exact alternative: pool-type CSP (equivalent formulation) ----------
 * r(W) <= q  iff  exists t: E -> subsets of [q] such that for every candidate set C and v in E\C,
 * t(v) is not a subset of union_{c in C} t(c).  (pool j = {v : j in t(v)}).
 * Elements never required as "v" get type 0 WLOG (shrinking unions only helps).
 */
#define TM 16
#define TD 256
typedef struct {
    uint8_t in[TM][TD];
    int size[TM];
    int val[TM];
} TDom;
static int tm, tD;
static int treqd[TM];
static int tcn;                 /* number of constraints */
static uint32_t tc_set[2048];   /* C mask over local elements */
static int tc_v[2048];          /* v local element */
static long long tnodes_c = 0;
static int tpa[TM], tpb[TM], ntp = 0;   /* twin chain pairs: t(a) <= t(b) */
static int use_twin = 0;

static int tremove_if(TDom *d, int x, uint32_t U, uint32_t R, int mode)
{
    /* mode 0: remove submasks of U; mode 1: remove supersets of R */
    int ch = 0;
    for (int t = 0; t < tD; t++) {
        if (!d->in[x][t]) continue;
        int rm = (mode == 0) ? ((t & ~(int)U) == 0) : (((int)R & t) == (int)R);
        if (rm) {
            d->in[x][t] = 0;
            d->size[x]--;
            ch = 1;
        }
    }
    if (d->size[x] == 1) {
        for (int t = 0; t < tD; t++) if (d->in[x][t]) { d->val[x] = t; break; }
    }
    return ch;
}

static int tremove_thr(TDom *d, int x, int thr, int mode)
{
    /* mode 0: remove types < thr ; mode 1: remove types > thr */
    int ch = 0;
    for (int t = 0; t < tD; t++) {
        if (!d->in[x][t]) continue;
        if ((mode == 0 && t < thr) || (mode == 1 && t > thr)) {
            d->in[x][t] = 0; d->size[x]--; ch = 1;
        }
    }
    if (d->size[x] == 1)
        for (int t = 0; t < tD; t++) if (d->in[x][t]) { d->val[x] = t; break; }
    return ch;
}

static int tpropagate(TDom *d)
{
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; use_twin && i < ntp; i++) {
            int a = tpa[i], b = tpb[i];
            if (d->size[a] == 1 && d->size[b] == 1) { if (d->val[a] > d->val[b]) return 0; continue; }
            if (d->size[a] == 1) { changed |= tremove_thr(d, b, d->val[a], 0); if (d->size[b] == 0) return 0; }
            if (d->size[b] == 1) { changed |= tremove_thr(d, a, d->val[b], 1); if (d->size[a] == 0) return 0; }
        }
        for (int c = 0; c < tcn; c++) {
            uint32_t C = tc_set[c];
            int v = tc_v[c];
            int nunassC = 0, uU = 0, uA = 0, uu = -1;
            uint32_t Uall = 0;
            for (int e = 0; e < tm; e++) if ((C >> e) & 1u) {
                if (d->size[e] == 1) Uall |= (uint32_t)d->val[e];
                else { nunassC++; uu = e; }
            }
            (void)uU; (void)uA;
            int vfix = (d->size[v] == 1);
            if (nunassC == 0) {
                if (vfix) {
                    int tv = d->val[v];
                    if ((tv & ~(int)Uall) == 0) return 0;
                } else {
                    int ch = tremove_if(d, v, Uall, 0, 0);
                    if (d->size[v] == 0) return 0;
                    changed |= ch;
                }
            } else if (nunassC == 1 && vfix) {
                uint32_t R = (uint32_t)d->val[v] & ~Uall;
                if (R == 0) return 0;
                int ch = tremove_if(d, uu, 0, R, 1);
                if (d->size[uu] == 0) return 0;
                changed |= ch;
            }
        }
    }
    return 1;
}

static int tsearch(TDom *d)
{
    tnodes_c++;
    int best = -1, bsz = 1 << 30;
    for (int e = 0; e < tm; e++)
        if (d->size[e] > 1 && d->size[e] < bsz) { bsz = d->size[e]; best = e; }
    if (best < 0) return 1;  /* all fixed; propagation has checked everything */
    for (int t = 0; t < tD; t++) {
        if (!d->in[best][t]) continue;
        TDom cp = *d;
        for (int u = 0; u < tD; u++) cp.in[best][u] = (u == t);
        cp.size[best] = 1;
        cp.val[best] = t;
        if (tpropagate(&cp) && tsearch(&cp)) return 1;
    }
    return 0;
}

/* elements 0..m-1 with candidate sets given as masks; returns 1 iff exists type assignment with 2^q types */
static int types_feasible(int m, const uint32_t *sets, int N, int q, const int *grp, uint32_t emask)
{
    if (N == 0) return 1;
    tm = m;
    ntp = 0;
    if (grp) {
        int last[TM];
        for (int g = 0; g < TM; g++) last[g] = -1;
        for (int e = 0; e < m; e++) {
            int g = grp[e];
            if (last[g] >= 0) { tpa[ntp] = last[g]; tpb[ntp] = e; ntp++; }
            last[g] = e;
        }
    }
    tD = 1 << q;
    tcn = 0;
    for (int e = 0; e < m; e++) treqd[e] = 0;
    for (int c = 0; c < N; c++)
        for (int v = 0; v < m; v++)
            if (((emask >> v) & 1u) && !((sets[c] >> v) & 1u)) {
                tc_set[tcn] = sets[c];
                tc_v[tcn] = v;
                tcn++;
                treqd[v] = 1;
            }
    if (tcn == 0) return 1;
    if (q == 0) return 0;
    TDom d;
    for (int e = 0; e < m; e++) {
        if (treqd[e] || grp) {   /* type-0 shortcut is not compatible with twin ordering */
            for (int t = 0; t < tD; t++) d.in[e][t] = 1;
            d.size[e] = tD;
        } else {
            /* never required: type 0 WLOG */
            for (int t = 0; t < tD; t++) d.in[e][t] = (t == 0);
            d.size[e] = 1;
            d.val[e] = 0;
        }
    }
    for (int e = 0; e < m; e++) if (d.size[e] == 1) d.val[e] = 0;
    if (!tpropagate(&d)) return 0;
    return tsearch(&d);
}


static const int *sig_grp(int cur)
{
    if (!use_twin_flag) return NULL;
    for (int e = 0; e < cur; e++) {
        gid_arr[e] = e;
        for (int f = 0; f < e; f++) if (sig[f] == sig[e]) { gid_arr[e] = gid_arr[f]; break; }
    }
    return gid_arr;
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
    if (use_types) { uint32_t em = 0; for (int a = 0; a < ne; a++) em |= 1u << elems[a]; use_twin = use_twin_flag; return types_feasible(cur, cmask, N, Q, sig_grp(cur), em); }
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

static int tchi_mode(void)
{
    int m, N;
    if (scanf("%d %d", &m, &N) != 2) return 1;
    for (int c = 0; c < N; c++) { unsigned int x; if (scanf("%u", &x) != 1) return 1; cmask[c] = x; }
    for (int q = 0; q <= 12; q++)
        if (types_feasible(m, cmask, N, q, NULL, (m >= 32) ? 0xffffffffu : ((1u << m) - 1))) { printf("%d\n", q); return 0; }
    printf("-1\n");
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "chi") == 0) return chi_mode();
    if (argc >= 2 && strcmp(argv[1], "tchi") == 0) return tchi_mode();
    if (argc < 5) {
        fprintf(stderr, "usage: %s search n T k [first] | chi\n", argv[0]);
        return 1;
    }
    n_items = atoi(argv[2]);
    Tot = atoi(argv[3]);
    K = atoi(argv[4]);
    stop_first = (argc > 5) ? atoi(argv[5]) : 0;
    use_types = (argc > 6) ? atoi(argv[6]) : 0;
    use_twin_flag = (argc > 7) ? atoi(argv[7]) : 0;
    Q = Tot - K;
    if (n_items < 1 || n_items > MAXN || K < 1 || K > MAXK || Q < 0 || Q >= MAXQ || Q > 10 /* cmask[MAXSETS=1024] holds at most 2^Q candidate sets */) {
        fprintf(stderr, "bad parameters\n");
        return 1;
    }
    double t0 = now_sec();
    dfs(0, 0);
    double el = now_sec() - t0;
    printf("RESULT method=%s n=%d T=%d k=%d q=%d designs=%lld nodes=%lld checks=%lld colour_calls=%lld colour_nodes=%lld max_vertices=%lld time=%.2fs stop_first=%d complete=%d\n",
           use_types ? (use_twin_flag ? "types+twin" : "types") : "colouring", n_items, Tot, K, Q, sols, nodes, checks, colcalls, colnodes, bigv, el, stop_first, stop_flag ? 0 : 1);
    /* Completion flag. This solver has no node limit and never answers "infeasible" on a resource cut-off,
       so a run is COMPLETE unless it was stopped on purpose at the first design (stop_first). */
    printf("ARTEFACT: search log. designs=0 is a NEGATIVE-SEARCH LOG (not an independently checkable certificate); a printed DESIGN is a candidate POSITIVE WITNESS (check it with independent/verify.py)\n");
    if (stop_flag) printf("STATUS: STOPPED AT FIRST DESIGN (stop_first=1; a design exists, search not exhaustive)\n");
    else printf("STATUS: COMPLETE (exhaustive; no node limit in this solver)\n");
    return 0;
}
