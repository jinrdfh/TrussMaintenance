import sys
import os
import random
import re

if __name__ == "__main__":
	dataFile = sys.argv[1]
	dataLabel = sys.argv[2]

	binWid = 0.2

	binNum = int(1 / binWid)

	dataFile_object = open(dataFile)

	saveFileLs = []
	for i in range(binNum - 1):
		tpId = int((i + 1) * binWid * 10)
		tpFile = "%s_0_%d.txt"%(dataLabel, tpId)
		tp_object = open(tpFile, 'w')
		saveFileLs.append(tp_object)

	for line in dataFile_object:
		fRanVar = random.random()
		binPos = int(fRanVar / binWid)
		# ignore last bin
		if binPos >= len(saveFileLs):
			continue
		for i in range(binPos, len(saveFileLs)):
			saveFileLs[i].write(line)

	for i in range(len(saveFileLs)):
		saveFileLs[i].close()
