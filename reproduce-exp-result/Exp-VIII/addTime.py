import sys
import os
import random
import re
import datetime

if __name__ == "__main__":
    dataFile = sys.argv[1]
    timeFile = sys.argv[2]
    indexEnd = int(sys.argv[3])
    insLabelNum = int(sys.argv[4])

    timeBin = dict()
    timeLs = []
    timeFile_object = open(timeFile)
    for line in timeFile_object:
        tpA = re.split(',|\t|\n', line)
        if "PATENT" in tpA[0]:
            continue
        node = int(tpA[0])
        year = int(tpA[1])
        date = int(tpA[2])
        timeLabel = (year, date)

        if timeLabel in timeBin:
            timeBin[timeLabel].append(node)
        else:
            timeBin[timeLabel] = [node]
            timeLs.append(timeLabel)

    timeFile_object.close()
    timeLs.sort()
    print("total time points count: ", len(timeLs))

    dataFile_object = open(dataFile)

    starBin = dict()
    for line in dataFile_object:
        x = int(re.split(' |\t|\n', line)[0])
        y = int(re.split(' |\t|\n', line)[1])
        if x in starBin:
            starBin[x].append(y)
        else:
            starBin[x] = [y]

    dataFile_object.close()

    offset = indexEnd
    batchNum = 1
    sampleNum = insLabelNum
    curCnt = batchNum - offset
    curId = 0
    batchCnt = 0
    saveFile_object = open("%d.txt"%(curId), 'w')
    for timeLabel in timeLs:
        curCnt += 1
        if (curCnt > batchNum):
            curCnt = 1
            curId += 1
            batchCnt += 1
            print (timeLabel)
            if (batchCnt > sampleNum):
                # enough
                break
            saveFile_object.close()
            saveFile_object = open("%d.txt"%(curId), 'w')
        for node in timeBin[timeLabel]:
            if node in starBin:
                for neighbor in starBin[node]:
                    if (neighbor == node):
                        continue
                    saveFile_object.write("%d %d\n"%(node, neighbor))
    saveFile_object.close()

