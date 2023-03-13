indexFile=$1
batchNum=$2
queryPath="query/"
logFile="log.txt"

totalSize=0
totalL=0
totalK=0
for i in $( seq 1 $batchNum )
do
../ballsIns.out $indexFile $queryPath"private_$i.txt" $queryPath"queryN_$i.txt" > $logFile
ballSize=`cat $logFile | grep "ball size" | awk '{print $4}'`
L_Chg=`cat $logFile | grep "BFS" | awk '{print $6}'`
K_Chg=`cat $logFile | grep "upgraded" | awk '{print $8}'`
totalSize=`echo "scale=3; $totalSize+$ballSize" | bc`
totalL=`echo "scale=3; $totalL+$L_Chg" | bc`
totalK=`echo "scale=3; $totalK+$K_Chg" | bc`
done
avgSize=`echo "scale=3; $totalSize/$batchNum" | bc`
avgL=`echo "scale=3; $totalL/$batchNum" | bc`
avgK=`echo "scale=3; $totalK/$batchNum" | bc`
echo $avgSize,$avgL,$avgK
rm $logFile
