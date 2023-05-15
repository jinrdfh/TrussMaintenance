
queryFile=$1
OrderQFile=$2
dataLine=`wc -l $queryFile | awk '{print $1}'`
echo $dataLine > $OrderQFile
cat $queryFile >> $OrderQFile
