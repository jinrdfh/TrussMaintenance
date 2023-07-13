cmdId=$1
indexFile=$2
queryPath="query/"
logFile="log_$cmdId.txt"

./Q2XH.sh $queryPath"query_$cmdId.txt" graph_$cmdId.inc
../XH.out -i $indexFile graph_$cmdId.inc > $logFile 2>&1
runningT=`cat $logFile | grep "Time" | sed 's/m/ /g' | awk '{printf("%.3f\n", $4)}'`

echo $runningT
rm $logFile graph_$cmdId.inc
