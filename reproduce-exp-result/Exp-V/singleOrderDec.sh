queryPath=$1
cmdId=$2
indexFile=$3
logFile="log_$cmdId.txt"

./Q2Order.sh $queryPath"query_$cmdId.txt" graph_$cmdId.dec
../Order.out bdelete $indexFile graph_$cmdId.dec result_$cmdId.order > $logFile 2>&1
runningT=`cat $logFile | grep costs | awk '{print $5}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`

echo $runningT
rm $logFile graph_$cmdId.dec
