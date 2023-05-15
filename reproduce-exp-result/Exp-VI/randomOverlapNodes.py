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
	sampleR = float(sys.argv[6])
	overlapR = float(sys.argv[7])
	nodeNum = int(sys.argv[8])

	print(dataFile, publicFile, privateFile, str(sampleR), str(overlapR), str(nodeNum))

	overlapNodesNum = int(overlapR * nodeNum)
	randomNodesNum = nodeNum - overlapNodesNum

	pNode = dict()
	dataFile_object = open(dataFile)
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])

		if x in pNode:
			pNode[x] = pNode[x] + 1
		else:
			pNode[x] = 1
		if y in pNode:
			pNode[y] = pNode[y] + 1
		else:
			pNode[y] = 1

	dataFile_object.close()

	print ("total nodes: " + str(len(pNode)))
	desNLs = []
	if (randomNodesNum > len(pNode)):
		# not enough
		# desNLs = random.sample(slaveNodes, nodeNum - len(allNodes))
		# desNLs.extend(allNodes)
		desNLs = pNode.keys()
		randomNodesNum = len(pNode)
	else:
		desNLs = random.sample(pNode.keys(), randomNodesNum)

	print ("random nodes: " + str(len(desNLs)))
	adjList = {}
	neighbors = {}
	dataFile_object = open(dataFile)
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])

		if x in desNLs:
			if x in adjList:
				adjList[x].append(y)
			else:
				adjList[x] = [y]

			if not( y in desNLs):
				neighbors[y] = True
		elif y in desNLs:
			if y in adjList:
				adjList[y].append(x)
			else:
				adjList[y] = [x]
			if not( x in desNLs):
				neighbors[x] = True
	dataFile_object.close()

	overlapNodes = []
	if (overlapNodesNum > len(neighbors)):
		# not enough
		overlapNodes = neighbors.keys()
		overlapNodesNum = len(neighbors)
	else:
		overlapNodes = random.sample(neighbors.keys(), overlapNodesNum)

	print ("overlap nodes: " + str(len(overlapNodes)))

	dataFile_object = open(dataFile)
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])

		if x in overlapNodes:
			if not(y in desNLs):
				if x in adjList:
					adjList[x].append(y)
				else:
					adjList[x] = [y]

		elif y in overlapNodes:
			if not(x in desNLs):
				if y in adjList:
					adjList[y].append(x)
				else:
					adjList[y] = [x]
	dataFile_object.close()

	desNLs.extend(overlapNodes)
	print ("total sample nodes: " + str(len(desNLs)))

	pMap = {}
	adjSampleList = {}
	for key in adjList:
		sampleNeib = int(sampleR * len(adjList[key]))
		newLine = "n " + str(key) + "\n"
		if (sampleNeib > len(adjList[key])):
			sampleNeib = len(adjList[key])
		adjSampleList[key] = random.sample(adjList[key], sampleNeib)
		for y in adjSampleList[key]:
			pMap[(key, y)] = 1
			pMap[(y, key)] = 1

	dataFile_object = open(dataFile)
	publicFile_object = open(publicFile, 'w')
	for line in dataFile_object:
		x = int(re.split(' |\t|\n', line)[0])
		y = int(re.split(' |\t|\n', line)[1])

		if not((x, y) in pMap):
			publicFile_object.write(line)
	dataFile_object.close()
	publicFile_object.close()

	index = 1
	privateFile_object = open(privateFile, 'w')
	queryFile_object = open(queryFile, 'w')
	for desNode in adjSampleList:
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

		for otherNode in adjSampleList[desNode]:
			newLine = ""
			if (desNode > otherNode):
				newLine = str(otherNode) + " " + str(desNode) + "\n"
			else:
				newLine = str(desNode) + " " + str(otherNode) + "\n"

			privateFile_object.write(newLine)
			detailFile_object.write(newLine)
			detailIns_object.write(newLine)
		detailFile_object.close()
		detailIns_object.close()
		detailN_object.close()

	print ("Sample nodes: " + str(index - 1))

	privateFile_object.close()
	queryFile_object.close()



