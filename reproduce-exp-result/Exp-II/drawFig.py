import numpy as np
import matplotlib.pyplot as plt
import csv

labels = []
dataI = []
dataII = []
with open('Exp-II.csv', mode = 'r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        if (lines[1] == "Phase I (ms)"):
            continue
        labels.append(lines[0])
        dataI.append(float(lines[1]))
        dataII.append(float(lines[2]))

size = len(labels)
x = np.arange(size)

total_width, n = 0.8, 2
width = total_width / n
x = x - (total_width - width) / 2

plt.axes(yscale = "log")
plt.bar(x, dataI,  width=width, label='Phase I')
plt.bar(x + width, dataII, width=width, label='Phase II')
plt.legend()
plt.ylabel("Time (ms)")
plt.xticks(x + width / 2, labels)
plt.savefig('./Exp-II.jpg')
# plt.show()