indexFile=$1
queryNum=$2
queryPath="query/"
logFile="log.txt"

./Q2Order.sh $queryPath$queryNum.txt graph.inc
../Order.out binsert $indexFile graph.inc result.order > $logFile 2>&1
runningT=`cat $logFile | grep costs | awk '{print $5}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
echo $runningT
rm $logFile graph.inc
