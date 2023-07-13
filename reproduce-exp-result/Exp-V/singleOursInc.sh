queryPath=$1
cmdId=$2
indexFile=$3
logFile="log_$cmdId.txt"

../ballsIns.out $indexFile $queryPath"private_$cmdId.txt" $queryPath"queryN_$cmdId.txt" > $logFile
runningT=`cat $logFile | grep "total query time" | awk '{print $4}'`

echo $runningT
rm $logFile
