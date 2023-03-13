dataFile=$1
indexFile=$2

dataLine=`wc -l $dataFile | awk '{print $1}'`

pMax=`cat $dataFile | sort -n -k1 | tail -n 1 | awk '{print $1}'`
pOMax=`cat $dataFile | sort -n -k2 | tail -n 1 | awk '{print $2}'`
pMax=`echo -e "$pMax\n$pOMax" | sort -n | tail -n 1`

echo `expr $pMax + 1` `expr $dataLine + 0` > graph.txt
cat $dataFile >> graph.txt

../OrderIndex.out ./graph.txt $indexFile
rm graph.txt
