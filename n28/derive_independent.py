from math import comb

T = 10


def greedy_L(R, k, B):
    """Minimum sum |W|*d(W) with d(W) in [0,B], sum d(W) >= R, filling smallest sizes first."""
    if R <= 0:
        return 0
    rem = R
    total = 0
    for w in range(1, k + 1):
        cap = comb(k, w) * B
        take = min(rem, cap)
        total += w * take
        rem -= take
        if rem == 0:
            break
    assert rem == 0, "requirement exceeds total capacity"
    return total


rows = []
detail = {}
for n in [24, 25, 26, 27, 28, 29]:
    for k in range(1, 10):
        q = T - k
        B = comb(q, q // 2)
        a = n - 18
        H = comb(n, 2) - comb(n - a, 2)
        s = 2 ** (k - 1) * B - H
        U = k * s
        R = (2 ** k - 1) * B - comb(n, 2)
        L = greedy_L(R, k, B)
        if s < 0:
            status = "EXCLUDED (s<0)"
        elif L > U:
            status = "EXCLUDED"
        else:
            status = "OPEN"
        rows.append((n, k, q, B, s, U, R, L, status))
        detail[(n, k)] = (L, U)

hdr = f"{'n':>3} {'k':>2} {'q':>2} {'B':>6} {'s':>14} {'U':>16} {'required':>14} {'L':>14}  status"
print(hdr)
print("-" * len(hdr))
for (n, k, q, B, s, U, R, L, status) in rows:
    print(f"{n:>3} {k:>2} {q:>2} {B:>6} {s:>14} {U:>16} {R:>14} {L:>14}  {status}")

print()
print("n=28 k=8: L=%d U=%d" % detail[(28, 8)])
print("n=28 k=9: L=%d U=%d" % detail[(28, 9)])
