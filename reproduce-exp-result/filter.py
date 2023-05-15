import sys

fp = open(sys.argv[1])

dCheck = {}
curK = 0
i = 0

for line in fp:
	if len(line.split('\n')[0].split(' ')) < 2:
		continue
	x = line.split('\n')[0].split(' ')[0]
	y = line.split('\n')[0].split(' ')[1]

	x = int(x)
	y = int(y)
	dCheck[(x, y)] = 1
	dCheck[(y, x)] = 1
	i = i + 1
fp.close()

fo = open(sys.argv[2], "w")
for key in dCheck:
	x = key[0]
	y = key[1]
	if (x < y):
		fo.write(str(x) + " " + str(y) + "\n")

fo.close()


