import numpy as np

def tomatrix(arr):
    mat = np.zeros((4,4), int)
    pos = 0
    for I in range(0, 4):
        for J in range(0, 4):
            np.add.at(mat, (I, J), int(arr[pos]))
            pos = pos + 1
    return mat


def printmatrix(mat):
    for I in range(0, 4):
        for J in range(0, 4):
            print(mat[I][J], end="")

mat = tomatrix("2200022000000000")

for i in range(0,4):
    printmatrix(mat)
    #print("")
    for j in range(0,3):
        mat = np.rot90(mat) #3 fois -90° = 1 fois 90°

