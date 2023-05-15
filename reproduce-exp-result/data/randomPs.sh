dataFile=$1
ratio=0.5
graphFile=$2
batNum=$3
savePath=$4

python ../randomPMulNode.py $dataFile $ratio $graphFile $savePath/private.txt ./$savePath/ $savePath/query.txt $batNum
