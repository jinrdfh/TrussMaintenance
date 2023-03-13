import sys
import os
import random
import re

if __name__ == "__main__":
	dataFile = sys.argv[1]
	fRatio = float(sys.argv[2])
	publicFile = sys.argv[3]
	privateFile = sys.argv[4]
	detailPath = sys.argv[5]
	queryFile = sys.argv[6]
	nodeNum = int(sys.argv[7])

	print(dataFile, str(fRatio), publicFile, privateFile, str(nodeNum))

	publicFile_object = open(publicFile, 'w')
	privateFile_object = open(privateFile, 'w')
	queryFile_object = open(queryFile, 'w')
	dataFile_object = open(dataFile)

	pMap = dict()
	pNeib = dict()
	pNode = dict()
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])
		pNode[x] = 1
		pNode[y] = 1

	# desNode = random.choice(pNeibWeight)
	if (nodeNum > len(pNode)):
		nodeNum = len(pNode)
	desNLs = random.sample(pNode.keys(), nodeNum)
	# print("find node: " + str(desNLs))

	dataFile_object.close()
	dataFile_object = open(dataFile)
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])

		if ((x in desNLs) or (y in desNLs)):
			pMap[(x, y)] = 1
			pMap[(y, x)] = 1
			if (not(x in pNeib)):
				pNeib[x] = [y]
			else:
				pNeib[x].append(y)
			if (not(y in pNeib)):
				pNeib[y] = [x]
			else:
				pNeib[y].append(x)
		else:
			publicFile_object.write(line)

	index = 1
	for desNode in desNLs:
		detailFile_object = open(detailPath + "private_%d.txt"%(index), 'w')
		detailIns_object = open(detailPath + "query_%d.txt"%(index), 'w')
		detailN_object = open(detailPath + "queryN_%d.txt"%(index), 'w')
		index += 1

		newLine = "n " + str(desNode) + "\n"
		privateFile_object.write(newLine)
		detailFile_object.write(newLine)
		newLine = str(desNode) + " 0\n"
		queryFile_object.write(newLine)
		detailN_object.write(newLine)

		for otherNode in pNeib[desNode]:
			if (0 == pMap[(desNode, otherNode)]):
				# handled
				continue
			pMap[(desNode, otherNode)] = 0
			pMap[(otherNode, desNode)] = 0
			
			newLine = ""
			if (desNode > otherNode):
				newLine = str(otherNode) + " " + str(desNode) + "\n"
			else:
				newLine = str(desNode) + " " + str(otherNode) + "\n"

			fRanVar = random.random()

			if (fRanVar < fRatio):
				privateFile_object.write(newLine)
				detailFile_object.write(newLine)
				detailIns_object.write(newLine)
				# print("sample edge: " + newLine, end = '')
			else:
				publicFile_object.write(newLine)
				# print("ignore edge: " + newLine, end = '')
		detailFile_object.close( )
		detailIns_object.close( )
		detailN_object.close( )


	dataFile_object.close()

	publicFile_object.close( )
	privateFile_object.close( )
	queryFile_object.close( )

