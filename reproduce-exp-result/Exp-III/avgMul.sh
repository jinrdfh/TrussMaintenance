totalNum=$1
progName=$2
queryPath=$3
param=$4

poolSize=`cat ../poolSize.txt | grep $progName | sed 's/\r/ /g' | awk '{print $2}'`

# echo "type: " $progName "pool Size: " $poolSize
logFile="log.txt"

# set pool
tempfifo=$$.fifo
mkfifo $tempfifo
exec 1000<>$tempfifo
rm -rf $tempfifo
for i in $( seq 1 $poolSize )
do
        echo >&1000
done
true > $logFile
for i in $( seq 1 $totalNum )
do
    read -u1000                
    {                          
            ./$progName $queryPath $i $param >> $logFile
            echo >&1000        
    } &                        
done

wait
totalBall=0
for Size in `cat $logFile | grep "ball size" | awk '{print $4}'`
do
    totalBall=`echo "scale=3; $totalBall+$Size" | bc`
done
totalL_Chg=0
for Size in `cat $logFile | grep "BFS" | awk '{print $6}'`
do
    totalL_Chg=`echo "scale=3; $totalL_Chg+$Size" | bc`
done
totalT_Chg=0
for Size in `cat $logFile | grep "upgraded" | awk '{print $8}'`
do
    totalT_Chg=`echo "scale=3; $totalT_Chg+$Size" | bc`
done
avgBall=`echo "scale=3; $totalBall/$totalNum" | bc`
avgL_Chg=`echo "scale=3; $totalL_Chg/$totalNum" | bc`
avgT_Chg=`echo "scale=3; $totalT_Chg/$totalNum" | bc`
echo $avgBall,$avgL_Chg,$avgT_Chg
rm $logFile

