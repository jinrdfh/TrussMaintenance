import numpy as np
import matplotlib.pyplot as plt
import csv

datasets = []
labels = []
Star_I = {}
Star_D = {}

with open('Exp-VII.csv', mode = 'r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        # print (len(lines))
        # print (lines)
        if (len(lines) < 14):
            continue
        if (lines[0] == "Varying |E|"):
            firstLine = [lines[1], lines[4], lines[7], lines[10], lines[13]]
            labels = firstLine
            continue
        if (lines[0] == "Graph"):
            continue
        datasets.append(lines[0])
        cur_I = [float(lines[1]), float(lines[4]), float(lines[7]), float(lines[10]), float(lines[13])]
        cur_D = [float(lines[2]), float(lines[5]), float(lines[8]), float(lines[11]), float(lines[14])]
        Star_I[lines[0]] = cur_I
        Star_D[lines[0]] = cur_D

Axes1=plt.subplot(1, 2, 1)
Axes1.set_yscale('log')
plt.plot(labels, Star_I[datasets[0]], label = "Insertion")
plt.plot(labels, Star_D[datasets[0]], label = "Deletion")

plt.legend()
plt.xlabel("Varying |E|")
plt.ylabel("Time (ms)")
plt.title(datasets[0])

Axes2=plt.subplot(1, 2, 2)
Axes2.set_yscale('log')
plt.plot(labels, Star_I[datasets[1]], label = "Insertion")
plt.plot(labels, Star_D[datasets[1]], label = "Deletion")

plt.legend()
plt.xlabel("Varying |E|")
plt.ylabel("Time (ms)")
plt.title(datasets[1])
plt.savefig('./Exp-VII.jpg')
# plt.show()