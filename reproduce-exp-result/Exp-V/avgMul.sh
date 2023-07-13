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
totalT=0
for runningT in `cat $logFile`
do
    totalT=`echo "scale=3; $totalT+$runningT" | bc`
done
echo "scale=3; $totalT/$totalNum" | bc
rm $logFile

