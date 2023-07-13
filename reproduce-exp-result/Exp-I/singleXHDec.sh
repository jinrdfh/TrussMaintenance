queryPath=$1
cmdId=$2
indexFile=$3
logFile="log_$cmdId.txt"

./Q2XH.sh $queryPath"query_$cmdId.txt" graph_$cmdId.dec
../XH.out -d $indexFile graph_$cmdId.dec > $logFile 2>&1
runningT=`cat $logFile | grep "Time" | sed 's/m/ /g' | awk '{printf("%.3f\n", $4)}'`

echo $runningT
rm $logFile graph_$cmdId.dec
