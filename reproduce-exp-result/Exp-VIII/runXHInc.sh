
indexFile=$1
queryNum=$2
queryPath="query/"
logFile="log.txt"

./Q2XH.sh $queryPath$queryNum.txt graph_$queryNum.inc
../XH.out -i $indexFile graph_$queryNum.inc > $logFile 2>&1
runningT=`cat $logFile | grep "Time" | sed 's/m/ /g' | awk '{printf("%.3f\n", $4)}'`

echo $runningT
rm $logFile graph_$queryNum.inc