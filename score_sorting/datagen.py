#!python.exe
import random

n = 100
perm = [2,3,1,4,6,5]

# n
print(n)

# permutation
print(' '.join(map(str,perm)))

# random data
for i in range(n * 6):
    print(min(100, int(random.normalvariate(75,15))))
