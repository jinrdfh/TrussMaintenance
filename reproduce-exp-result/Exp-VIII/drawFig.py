import numpy as np
import matplotlib.pyplot as plt
import csv

labels = []
Star = []
Order = []
XH = []

with open('Exp-VIII.csv', mode = 'r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        if (lines[1] == "Star"):
            continue
        id = int(lines[0])
        labels.append(id)
        Star.append(float(lines[1]))
        Order.append(float(lines[2]))
        XH.append(float(lines[3]))

plt.plot(labels, Star, label = "Star")
plt.plot(labels, Order, label = "Order")
plt.plot(labels, XH, label = "XH")

plt.legend()
plt.xlabel("Timestamp")
plt.ylabel("Time (ms)")
plt.savefig('./Exp-VIII.jpg')
# plt.show()
