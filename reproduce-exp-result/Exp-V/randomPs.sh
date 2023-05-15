dataFile=$1
graphFile=$2
sampleNum=$3
batNum=$4

python ./randomNum.py $dataFile $graphFile query/private.txt query/query.txt ./query/ $sampleNum $batNum
