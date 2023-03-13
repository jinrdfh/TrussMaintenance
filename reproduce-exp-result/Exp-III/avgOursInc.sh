indexFile=$1
batchNum=$2
queryPath="query/"
logFile="log.txt"

totalStep1T=0
totalStep2T=0
for i in $( seq 1 $batchNum )
do
../ballsIns.out $indexFile $queryPath"private_$i.txt" $queryPath"queryN_$i.txt" > $logFile
step1T=`cat $logFile | grep "g_lBallDecTime" | awk '{print $2}'`
step2T=`cat $logFile | grep "g_lOutBallTime" | awk '{print $2}'`
totalStep1T=`echo "scale=3; $totalStep1T+$step1T" | bc`
totalStep2T=`echo "scale=3; $totalStep2T+$step2T" | bc`
done
avgStep1T=`echo "scale=3; $totalStep1T/$batchNum" | bc`
avgStep2T=`echo "scale=3; $totalStep2T/$batchNum" | bc`
echo $avgStep1T,$avgStep2T
rm $logFile
