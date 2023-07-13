cmdId=$1
queryPath="query/"
logFile="log_$cmdId.txt"

./Q2XH.sh $queryPath"$cmdId.txt" graph.inc
../XH.out -i ./ > $logFile 2>&1
runningT=`cat $logFile | grep "Time" | sed 's/m/ /g' | awk '{printf("%.3f\n", $4)}'`

echo $runningT
rm $logFile graph.inc
