from math import *

import perm

#         a    b    c    d    e    f    g    h    i    j    k    l    m    n    o    p    q    r    s    t    u    v    w    x    y    z
PROB = (8.2, 1.5, 2.8, 4.3,12.7, 2.2, 2.0, 6.1, 6.9, 0.2, 0.8, 4.0, 2.4, 6.7, 7.5, 1.9, 0.1, 5.9, 6.3, 9.1, 2.8, 1.0, 2.4, 0.2, 1.9, 0.1)

def norm():
    global PROB
    sum = 0
    for i in PROB: sum += i
    PROB = tuple(i / sum for i in PROB)
norm()

def factorial(n):
    f = 1
    for i in range(2, n + 1): f *= i
    return f

def A(N, K):
    f = 1
    for i in range(K + 1, N + K + 1): f *= i
    return f

def Cnn(N, args):
    f = factorial(N)
    for a in args: f /= factorial(a)
    return f

# part 1
# M -- vector of # of each letter in sentense
# N -- length of the sentense
def part1(M, N):
    assert len(M) == len(PROB)
    M_total = 0
    for m in M: M_total += m
    assert M_total <= N

    p_base = 1
    for l in range(len(PROB)):
        p_base *= PROB[l] ** M[l]
    p_base *= Cnn(N, M)

    if M_total == N: return p_base

    p_extra = 0
    np = perm.Perm(len(PROB), N - M_total)
    while np.next():
        pp = np.get()
        this_extra_p = 1
        for i in range(len(PROB)):
            this_extra_p *= PROB[i] ** pp[i]
            this_extra_p /= A(pp[i], M[i])
        p_extra += this_extra_p

    return p_base * p_extra

    p = 0
    np = perm.Perm(len(PROB), N)
    while np.next():
        pp = np.get()

        cont = False
        for i in range(len(PROB)):
            if pp[i] < M[i]:
                cont = True
                break
        if cont: continue
        print(pp)

        this_p = 1
        for i in range(len(PROB)):
            this_p *= PROB[i] ** pp[i]
            this_p /= factorial(pp[i])
        p += this_p
    p *= factorial(N)
    return p

if __name__ == "__main__":
    phrase = "emerald therapeutics"
    M = [0] * len(PROB)
    L = 0
    for i in phrase:
        if i >= 'a' and i <= 'z':
            M[ord(i) - ord('a')] += 1
            L += 1
    print("M: ", M)

    new_prob = []
    new_M = []

    z_prob = 0

    l = 0
    for i in range(len(PROB)):
        if M[i]:
            new_prob += [PROB[i]]
            new_M += [M[i]]
        else:
            z_prob += PROB[i]

    new_prob += [z_prob]
    new_M += [0]

    print("new_prob: ", new_prob, " len: ", len(new_prob))
    print("new_M: ", new_M, " len: ", len(new_M))

    PROB = new_prob
    M = new_M

    print(L, "\n", M)
    for i in range(100):
        N = L + i
        print("%d -> %g" % (N, part1(M, N)))
