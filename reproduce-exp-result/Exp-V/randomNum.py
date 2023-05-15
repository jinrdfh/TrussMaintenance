import sys
import os
import random
import re

if __name__ == "__main__":
	dataFile = sys.argv[1]
	publicFile = sys.argv[2]
	privateFile = sys.argv[3]
	queryFile = sys.argv[4]
	detailPath = sys.argv[5]
	sampleNum = int(sys.argv[6])
	batchNum = int(sys.argv[7])

	print(dataFile, str(sampleNum), str(batchNum))

	sampleEPool = dict()
	sampleRate = 0.001
	while (len(sampleEPool) < sampleNum * batchNum):
		dataFile_object = open(dataFile)
		for line in dataFile_object:
			fRanVar = random.random()

			if (fRanVar > sampleRate):
				continue
			x = int(re.split(' |\t|\n', line)[0])
			y = int(re.split(' |\t|\n', line)[1])
			sampleEPool[(x, y)] = 0
		dataFile_object.close()

		if (1 <= sampleRate):
			break
		if (0 == len(sampleEPool)):
			sampleRate = 1
			continue
		if (len(sampleEPool) < sampleNum * batchNum):
			sampleRate *= sampleNum * batchNum / len(sampleEPool) 

	edgeNum = sampleNum * batchNum
	if (edgeNum > len(sampleEPool)):
		edgeNum = len(sampleEPool)
	desELs = random.sample(sampleEPool.keys(), edgeNum)
	pageELs = {}
	cnt = 0
	for key in desELs:
		if (0 == cnt % sampleNum):
			pageELs[cnt // sampleNum] = []
		pageELs[cnt // sampleNum].append(key)
		sampleEPool[key] = 1
		cnt += 1

	dataFile_object = open(dataFile)
	publicFile_object = open(publicFile, 'w')
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])

		if ((x, y) in sampleEPool):
			if (1 == sampleEPool[(x, y)]):
				continue
		publicFile_object.write(line)
	dataFile_object.close()
	publicFile_object.close()

	privateFile_object = open(privateFile, 'w')
	queryFile_object = open(queryFile, 'w')
	for key in desELs:
		newLine = "n " + str(key[0]) + "\n"
		privateFile_object.write(newLine)
		newLine = str(key[0]) + " " + str(key[1]) + "\n"
		privateFile_object.write(newLine)
		newLine = str(key[0]) + " 0\n"
		queryFile_object.write(newLine)

	privateFile_object.close()
	queryFile_object.close()

	for index in pageELs:
		detailFile_object = open(detailPath + "private_%d.txt"%(index+1), 'w')
		detailIns_object = open(detailPath + "query_%d.txt"%(index+1), 'w')
		detailN_object = open(detailPath + "queryN_%d.txt"%(index+1), 'w')
		for paXY in pageELs[index]:
			newLine = "n " + str(paXY[0]) + "\n"
			detailFile_object.write(newLine)
			newLine = str(paXY[0]) + " " + str(paXY[1]) + "\n"
			detailFile_object.write(newLine)
			detailIns_object.write(newLine)
			newLine = str(paXY[0]) + " 0\n"
			detailN_object.write(newLine)
		detailFile_object.close()
		detailIns_object.close()
		detailN_object.close()

