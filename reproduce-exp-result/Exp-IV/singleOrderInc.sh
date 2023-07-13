cmdId=$1
indexFile=$2
queryPath="query/"
logFile="log_$cmdId.txt"

./Q2Order.sh $queryPath"query_$cmdId.txt" graph_$cmdId.inc
../Order.out binsert $indexFile graph_$cmdId.inc result_$cmdId.order > $logFile 2>&1
runningT=`cat $logFile | grep costs | awk '{print $5}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`

echo $runningT
rm $logFile graph_$cmdId.inc
