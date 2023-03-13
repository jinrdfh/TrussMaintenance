
dataFile=$1
starFile=$2
nodeFile=$3

./star.out $dataFile $starFile
cat $starFile | grep "n" | awk '{printf("%d 0\n", $2)}' > $nodeFile
