indexFile=$1
queryPath=$2
batchNum=$3
logFile="log.txt"

totalT=0
for i in $( seq 1 $batchNum )
do
../ballsIns.out $indexFile $queryPath"private_$i.txt" $queryPath"queryN_$i.txt" > $logFile
runningT=`cat $logFile | grep "total query time" | awk '{print $4}'`
totalT=`echo "scale=3; $totalT+$runningT" | bc`
done
echo "scale=3; $totalT/$batchNum" | bc
rm $logFile
