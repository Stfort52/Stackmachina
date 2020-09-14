#!/usr/bin/python3
from z3 import *

X = [BitVec("x{}".format(i), 32) for i in range(32)]
Y = [BitVec("y{}".format(i), 32) for i in range(5)]
Z = [BitVec("z{}".format(i), 32) for i in range(3)]
T = [BitVec("t{}".format(i), 32) for i in range(2)]
s = Solver()


for x in X:
    s.add(Or(And(x > 64, x < 91), And(x > 96, x < 126)))
    s.add(x != 124)

for x in X[5:31]:
    s.add(x != ord('{'))
    s.add(x != ord('}'))

s.add(X[0] == ord('P'))
s.add(X[1] == ord('L'))
s.add(X[2] == ord('U'))
s.add(X[3] == ord('S'))
s.add(X[4] == ord('{'))
s.add(X[31] == ord('}'))

s.add(Y[0] == (X[0] | (X[1] << 8) | (X[2] << 16) | (X[3] << 24))
      ^ (X[4] | (X[5] << 8) | (X[6] << 16) | (X[7] << 24)))
s.add(Y[1] == (X[8] | (X[9] << 8) | (X[10] << 16) | (X[11] << 24))
      ^ (X[12] | (X[13] << 8) | (X[14] << 16) | (X[15] << 24)))
s.add(Y[2] == X[16] + (X[17] << 8) + (X[18] << 16) + (X[19] << 24))
s.add(Y[3] == (X[20] | (X[21] << 8) | (X[22] << 16) | (X[23] << 24))
      % (X[24] | (X[25] << 8) | (X[26] << 16) | (X[27] << 24)))
s.add(Y[4] == (X[16] << 24) + (X[17] << 16) + (X[18] << 8) + (X[19] << 0))

s.add(Z[0] == Y[0] ^ Y[3]) #9
s.add(Z[1] == Y[1] + (Y[2] + Y[4])) #10
s.add(Z[2] == Y[1] * Y[2] + Y[3] * Y[4]) #11
#s.add(Y[0] != 0) #4
#s.add(Y[1] != 0) #5
#s.add(Y[2] != 0) #6
#s.add(Y[3] != 0) #7
#s.add(Y[4] != 0) #8
s.add(Z[2] == 486950476)
s.add(Z[1] == 3014964405)
s.add(Z[0] == 1667595355)
s.add(T[0] == (X[20] | (X[21] << 8) | (X[22] << 16) | (X[23] << 24)))
s.add(T[1] == (X[24] | (X[25] << 8) | (X[26] << 16) | (X[27] << 24)))
cnt = 0
for i in range(25):
    s.add(X[5+i] < X[6+i])
print(s.check())
if s.check() == sat:
    m = s.model()
    print(m)
    print("".join(list(map(lambda x: chr(m.evaluate(x).as_long()), X))))
    print("sum:", sum(map(lambda x: m.evaluate(x).as_long(), X)))

while s.check() == sat and cnt < 100:
    cnt += 1
    m = s.model()
    #print("".join(list(map(lambda x:chr(m.evaluate(x).as_long()), X))))
    print(*list(map(lambda x: m.evaluate(x).as_long(), Y)))
    A = list()
    for i, j in enumerate(list(map(lambda x: m.evaluate(x).as_long(), X))):
        A += [X[i] != j]

    s.add(Or(A))

    # print(A)
    '''
    B=list()
    for i, j in enumerate(list(map(lambda x: m.evaluate(x).as_long(), Y))):
        B += [Y[i] != j]
    s.add(Or(B))
    '''
    # print(s.check())

print("cnt:", cnt)

print("end!")
