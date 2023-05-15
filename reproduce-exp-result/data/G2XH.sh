dataFile=$1

rm ./graph-before.truss 2>/dev/null

../XHIndex.out $dataFile graph-before.truss
