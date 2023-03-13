batchNum=$1
queryPath="query"
priFile=$queryPath/total$1.pri
queryFile=$queryPath/total$1.N
insertFile=$queryPath/total$1.ins

true > $priFile
true > $queryFile
true > $insertFile
for i in $( seq 1 $batchNum )
do
	cat $queryPath/private_$i.txt >> $priFile
	cat $queryPath/queryN_$i.txt >> $queryFile
done
cat $priFile | grep -v n > $insertFile


