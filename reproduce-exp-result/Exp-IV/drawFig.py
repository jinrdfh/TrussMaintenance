import numpy as np
import matplotlib.pyplot as plt
import csv

datasets = []
labels = []
XH = {}
NodePP = {}
Order = {}
Star = {}

with open('Exp-IV.csv', mode = 'r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        # print (len(lines))
        # print (lines)
        if (len(lines) < 13):
            continue
        if (lines[0] == "Size of star motif"):
            firstLine = [lines[1], lines[5], lines[9], lines[13], lines[17]]
            labels = firstLine
            continue
        if (lines[0] == "Graph"):
            continue
        datasets.append(lines[0])
        if (len(lines) < 17):
            XH[lines[0]] = [float(lines[1]), float(lines[5]), float(lines[9])]
            NodePP[lines[0]] = [float(lines[2]), float(lines[6]), float(lines[10])]
            Order[lines[0]] = [float(lines[3]), float(lines[7]), float(lines[11])]
            Star[lines[0]] = [float(lines[4]), float(lines[8]), float(lines[12])]
        else:
            XH[lines[0]] = [float(lines[1]), float(lines[5]), float(lines[9]), float(lines[13]), float(lines[17])]
            NodePP[lines[0]] = [float(lines[2]), float(lines[6]), float(lines[10]), float(lines[14]), float(lines[18])]
            Order[lines[0]] = [float(lines[3]), float(lines[7]), float(lines[11]), float(lines[15]), float(lines[19])]
            Star[lines[0]] = [float(lines[4]), float(lines[8]), float(lines[12]), float(lines[16]), float(lines[20])]

plt.figure(figsize=(18, 4))
for subFigId in range(len(datasets)):
    Axes=plt.subplot(1, len(datasets), subFigId + 1)
    Axes.set_yscale('log')
    labelLen = len(XH[datasets[subFigId]])
    plt.plot(labels[0:labelLen], XH[datasets[subFigId]], label = "XH")
    plt.plot(labels[0:labelLen], NodePP[datasets[subFigId]], label = "NodePP")
    plt.plot(labels[0:labelLen], Order[datasets[subFigId]], label = "Order")
    plt.plot(labels[0:labelLen], Star[datasets[subFigId]], label = "Star")

    plt.legend()
    plt.xlabel("Size of star motif")
    plt.ylabel("Time (ms)")
    plt.title(datasets[subFigId])

plt.savefig('./Exp-IV.jpg')
# plt.show()