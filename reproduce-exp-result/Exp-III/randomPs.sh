dataFile=$1
ratio=0.5
graphFile=$2
batNum=$3

python ../randomPMulNode.py $dataFile $ratio $graphFile query/private.txt ./query/ query/query.txt $batNum
