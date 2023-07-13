queryPath=$1
cmdId=$2
indexFile=$3
logFile="log_$cmdId.txt"

../ballsIns.out $indexFile $queryPath"private_$cmdId.txt" $queryPath"queryN_$cmdId.txt" > $logFile

cat $logFile | grep "g_lBallDecTime"
cat $logFile | grep "g_lOutBallTime"
rm $logFile
