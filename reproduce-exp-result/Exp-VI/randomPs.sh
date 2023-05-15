dataFile=$1
graphFile=$2
batNum=$3
neibR=$4
overlapR=$5

python ./randomOverlapNodes.py $dataFile $graphFile query/private.txt query/query.txt ./query/ $neibR $overlapR $batNum
