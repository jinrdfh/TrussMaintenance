indexFile=$1
queryNum=$2
resultFile=$3
queryPath="query/"
logFile="log.txt"

./Q2Ours.sh $queryPath/$queryNum.txt star.txt node.txt >/dev/null
../ballsIns.out $indexFile star.txt node.txt $resultFile > $logFile
runningT=`cat $logFile | grep "total query time" | awk '{print $4}'`

echo $runningT
rm $logFile star.txt node.txt
