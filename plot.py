import matplotlib.pyplot as plt
import numpy as np

f = open('drums.txt')
nums = f.readlines()

vals = map(lambda t:map(int, t.strip().split()), nums)
y, x, _ = zip(*vals)

y = np.array(y)

res = []
for i in range(4, len(y) - 4):
    res.append(np.mean(y[i-4:i+5]))

plt.subplot(211), plt.plot(y)
plt.subplot(212), plt.plot(res)
plt.show()
