indexFile=$1
queryNum=$2
queryPath="query/"
logFile="log.txt"

../ballsIns.out $indexFile $queryPath"total$queryNum.pri" $queryPath"total$queryNum.N" > $logFile
runningT=`cat $logFile | grep "total query time" | awk '{print $4}'`

echo $runningT
rm $logFile
