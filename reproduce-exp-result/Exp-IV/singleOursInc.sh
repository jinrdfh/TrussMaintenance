cmdId=$1
indexFile=$2
queryPath="query/"
logFile="log_$cmdId.txt"

../ballsIns.out $indexFile $queryPath"private_$cmdId.txt" $queryPath"queryN_$cmdId.txt" > $logFile
runningT=`cat $logFile | grep "total query time" | awk '{print $4}'`

echo $runningT
rm $logFile
