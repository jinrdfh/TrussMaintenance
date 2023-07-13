import numpy as np
import matplotlib.pyplot as plt
import csv

labels = []
XH_I = []
XH_D = []
Order_I = []
Order_D = []
Star_I = []
Star_D = []

with open('Exp-V.csv', mode = 'r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        if (lines[1] == "Insertion (ms)"):
            continue
        if (lines[1] == "XH"):
            continue
        labels.append(lines[0])
        XH_I.append(float(lines[1]))
        Order_I.append(float(lines[2]))
        Star_I.append(float(lines[3]))
        XH_D.append(float(lines[4]))
        Order_D.append(float(lines[5]))
        Star_D.append(float(lines[6]))

size = len(labels)
x = np.arange(size)

total_width, n = 0.8, 3
width = total_width / n
x = x - (total_width - width) / 2

plt.axes(yscale = "log")
plt.bar(x, XH_I,  width=width, label='XH')
plt.bar(x + width, Order_I, width=width, label='Order')
plt.bar(x + width * 2, Star_I, width=width, label='Star')
plt.legend()
plt.ylabel("Time (ms)")
plt.xticks(x + width * (n - 1) / 2, labels)
plt.title("Single-edge insertion")
plt.savefig('./Exp-V(insertion).jpg')
# plt.show()

plt.clf()
plt.axes(yscale = "log")
plt.bar(x, XH_D,  width=width, label='XH')
plt.bar(x + width, Order_D, width=width, label='Order')
plt.bar(x + width * 2, Star_D, width=width, label='Star')
plt.legend()
plt.ylabel("Time (ms)")
plt.xticks(x + width * (n - 1) / 2, labels)
plt.title("Single-edge deletion")
plt.savefig('./Exp-V(deletion).jpg')
# plt.show()