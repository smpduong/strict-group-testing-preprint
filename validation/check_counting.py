from math import comb
# Deficit mass fills the lowest nonempty outcome weights first.
for k, B, expected in [(8, 2, 308), (9, 1, 349)]:
    need = (2**k - 1)*B - comb(28, 2)
    cost = 0
    for w in range(1, k+1):
        take = min(need, comb(k,w)*B)
        cost += w*take
        need -= take
    assert need == 0 and cost == expected and cost > 31*k
    print(f"PASS k={k}: minimum weighted deficit {cost} > {31*k}")

# Nonadaptive endpoints: all pair outcomes must form an antichain.
for n, T in [(19, 9), (28, 10)]:
    pairs, cap = comb(n, 2), comb(T, T//2)
    assert pairs > cap
    print(f"PASS nonadaptive n={n}, T={T}: {pairs} > {cap}")
