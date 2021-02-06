class Perm:
    def __init__(self, L, N):
        self.L = L + N - 1
        self.S = L - 1
        self.N = N
        self.x = [0] * self.N + [1] * (self.S)
        self.started = False

    def next(self):
        if not self.started:
            self.started = True
            return True

        if self.x[0] == 1:
            nnz = 1
            for i in range(1, self.L - 1):
                if self.x[i] == 0: break
                nnz += 1
            if nnz == self.S: return False

            for i in range(nnz): self.x[i] = 0
            for i in range(nnz, self.L - 1):
                if self.x[i] == 0 and self.x[i + 1] == 1:
                    self.x[i], self.x[i + 1] = 1, 0
                    for j in range(i - nnz, i): self.x[j] = 1
                    return True

        for i in range(self.L - 1):
            if self.x[i] == 0 and self.x[i + 1] == 1:
                self.x[i], self.x[i + 1] = self.x[i + 1], self.x[i]
                return True
        return False

    def get(self):
        ret = [0] * (self.S + 1)
        val, pos = 0, 0
        for i in reversed(range(self.L)):
            if self.x[i] == 1:
                ret[pos] = val
                pos += 1
                val = 0
            else:
                val += 1
        ret[pos] = val
        return ret

# p = Perm(4, 3)
# while p.next(): print(p.get())
