
queryPath=$1
batchNum=$2
logFile="log.txt"

totalT=0
for i in $( seq 1 $batchNum )
do
#wc -l $queryPath"query_$i.txt"
lineNum=`wc -l $queryPath"query_$i.txt" | awk '{print $1}'`
if [ 0 -eq $lineNum ]; then
runningT=0
elif [ 1 -eq $lineNum ]; then
../nodePP.out graph-before.truss $queryPath"query_$i.txt" result.truss -insert edge > $logFile
runningT=`cat $logFile | grep "total time" | awk '{printf"%d", $4}'`
else
../nodePP.out graph-before.truss $queryPath"query_$i.txt" result.truss -insert node > $logFile
runningT=`cat $logFile | grep "total time" | awk '{printf"%d", $4}'`
fi
#echo $runningT
totalT=`echo "scale=3; $totalT+$runningT" | bc`
done
echo "scale=3; $totalT/$batchNum" | bc
rm $logFile result.truss
