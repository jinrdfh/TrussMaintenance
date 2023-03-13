dataFile=$1
queryPath="./query"
newNum=$2
batNum=$3

./graphSampleNew.out $dataFile $queryPath $newNum $batNum >/dev/null
for i in $(seq 1 $batNum)
do
      cat $queryPath/"private_$i.txt" | grep "n" | awk '{print $2,0}' > $queryPath/"queryN_$i.txt"
      cat $queryPath/"private_$i.txt" | grep "n" -v > $queryPath/"query_$i.txt"
done
