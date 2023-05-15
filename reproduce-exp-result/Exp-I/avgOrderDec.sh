indexFile=$1
queryPath=$2
batchNum=$3
logFile="log.txt"

totalT=0
for i in $( seq 1 $batchNum )
do
./Q2Order.sh $queryPath"query_$i.txt" graph.del
../Order.out bdelete $indexFile graph.del result.order > $logFile 2>&1
runningT=`cat $logFile | grep costs | awk '{print $5}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
totalT=`echo "scale=3; $totalT+$runningT" | bc`
done
echo "scale=3; $totalT/$batchNum" | bc
rm $logFile graph.del
