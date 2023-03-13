
queryFile=$1
XHQFile=$2
dataLine=`wc -l $queryFile | awk '{print $1}'`
echo $dataLine > $XHQFile
cat $queryFile >> $XHQFile
