import numpy as np
import matplotlib.pyplot as plt
import csv

labels = []
Star = []
Order = []

with open('Exp-VI.csv', mode = 'r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        if (lines[1] == "Star"):
            continue
        id = int(lines[0])
        if (id % 10 != 0):
            continue
        labels.append(id)
        Star.append(float(lines[1]))
        Order.append(float(lines[2]))

size = len(labels)
x = np.arange(size)

plt.plot(labels, Star, label = "Star")
plt.plot(labels, Order, label = "Order")

plt.legend()
plt.xlabel("The number of inserted star motifs")
plt.ylabel("Time (ms)")
plt.savefig('./Exp-VI.jpg')
# plt.show()
