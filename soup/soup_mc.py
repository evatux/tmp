import random

#         a    b    c    d    e    f    g    h    i    j    k    l    m    n    o    p    q    r    s    t    u    v    w    x    y    z
PROB = (8.2, 1.5, 2.8, 4.3,12.7, 2.2, 2.0, 6.1, 6.9, 0.2, 0.8, 4.0, 2.4, 6.7, 7.5, 1.9, 0.1, 5.9, 6.3, 9.1, 2.8, 1.0, 2.4, 0.2, 1.9, 0.1)
M = None

phrase = "emerald therapeutics"

def norm():
    global PROB
    sum = 0
    for i in PROB: sum += i
    PROB = tuple(i / sum for i in PROB)

def simplify():
    global PROB
    global M

    M = [0] * len(PROB)
    L = 0
    for i in phrase:
        if i >= 'a' and i <= 'z':
            M[ord(i) - ord('a')] += 1
            L += 1

    new_prob = []
    new_M = []

    z_prob = 0

    for i in range(len(PROB)):
        if M[i]:
            new_prob += [PROB[i]]
            new_M += [M[i]]
        else:
            z_prob += PROB[i]

    new_prob += [z_prob]
    new_M += [0]

    PROB = new_prob
    M = new_M

def simulate(L):
    c = random.choices(range(len(PROB)), PROB, k=L)
    for i in range(len(M)):
        if c.count(i) < M[i]:
            return False
    return True

norm()
simplify()

success = 0
nruns = 4000000
L = 84 # seems like the answer is 85

percent = 0
for i in range(nruns):
    if i % (nruns // 100) == 0:
        if i % (nruns // 10) == 0:
            print("\n %3d" % (i * 100 // nruns), end=' ')
        print('.', end=' ', flush=True)
    if simulate(L):
        success += 1

print("\nprob: %g" % (1.0 * success / nruns))
