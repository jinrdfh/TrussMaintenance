
queryPath=$1
batchNum=$2
logFile="log.txt"

totalT=0
for i in $( seq 1 $batchNum )
do
./Q2XH.sh $queryPath"query_$i.txt" graph.inc
../XH.out -i ./ > $logFile 2>&1
runningT=`cat $logFile | grep "Time" | sed 's/m/ /g' | awk '{printf("%.3f\n", $4)}'`
totalT=`echo "scale=3; $totalT+$runningT" | bc`
done
echo "scale=3; $totalT/$batchNum" | bc
rm $logFile graph_inc.truss graph.inc
