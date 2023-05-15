dataFile=$1
indexFile=$2

dataLine=`wc -l $dataFile | awk '{print $1}'`

pMax=6009554

echo `expr $pMax + 1` `expr $dataLine + 0` > OrderG.txt
cat $dataFile >> OrderG.txt

../OrderIndex.out ./OrderG.txt $indexFile
rm OrderG.txt
