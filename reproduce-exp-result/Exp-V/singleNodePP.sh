queryPath=$1
cmdId=$2
indexFile=$3
logFile="nodePP_$cmdId.log"

#wc -l $queryPath"query_$i.txt"
lineNum=`wc -l $queryPath"query_$cmdId.txt" | awk '{print $1}'`
if [ 0 -eq $lineNum ]; then
runningT=0
elif [ 1 -eq $lineNum ]; then
../nodePP.out $indexFile $queryPath"query_$cmdId.txt" result_$cmdId.truss -insert edge > $logFile
runningT=`cat $logFile | grep "total time" | awk '{printf"%d", $4}'`
else
../nodePP.out $indexFile $queryPath"query_$cmdId.txt" result_$cmdId.truss -insert node > $logFile
runningT=`cat $logFile | grep "total time" | awk '{printf"%d", $4}'`
fi
echo $runningT

rm $logFile result_$cmdId.truss
