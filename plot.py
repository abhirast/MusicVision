import matplotlib.pyplot as plt
import numpy as np

f = open('drums.txt')
nums = f.readlines()

vals = map(lambda t:map(float, t.strip().split()), nums)
x, y = zip(*vals)

y = np.array(y)

res = []
for i in range(1, len(y) - 1):
    res.append(np.mean(y[i-1:i+2]))

plt.subplot(211), plt.plot(y)
plt.subplot(212), plt.plot(res)
plt.show()
