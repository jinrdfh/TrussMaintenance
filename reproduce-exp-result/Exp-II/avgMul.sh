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
totalStep1T=0
for runningT in `cat $logFile | grep "g_lBallDecTime" | awk '{print $2}'`
do
    totalStep1T=`echo "scale=3; $totalStep1T+$runningT" | bc`
done
totalStep2T=0
for runningT in `cat $logFile | grep "g_lOutBallTime" | awk '{print $2}'`
do
    totalStep2T=`echo "scale=3; $totalStep2T+$runningT" | bc`
done
avgStep1T=`echo "scale=3; $totalStep1T/$totalNum" | bc`
avgStep2T=`echo "scale=3; $totalStep2T/$totalNum" | bc`
echo $avgStep1T,$avgStep2T
rm $logFile

