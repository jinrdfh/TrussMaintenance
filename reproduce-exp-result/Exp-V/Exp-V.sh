# dataset name, only two small datasets here
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
# repeat times
batNum=100
# output file name
output="Exp-V.csv"
# table head
header=",Insertion (ms),,,Deletion (ms),,,"
echo $header > $output
header="Graph,XH,Order,Ours,XH,Order,Ours"
echo $header >> $output

for dataset in ${datasets[@]}
do
	# insertion
	# generate queries
	./randomPs.sh ../data/$dataset".txt" oldGraph.txt 1 $batNum >/dev/null
	# ours
	./G2Ours.sh oldGraph.txt oldGraph.myG >/dev/null
	ourIncT=`./avgOursInc.sh oldGraph.myG $batNum`
	# XH
	./G2XH.sh oldGraph.txt >/dev/null
	XHIncT=`./avgXHInc.sh $batNum`
	# Order
	./G2Order.sh oldGraph.txt oldGraph.order >/dev/null
	OrderIncT=`./avgOrderInc.sh oldGraph.order $batNum`

	# deletion
	# ours
	ourDecT=`./avgOursDec.sh ../data/$dataset".myG" $batNum`
	# XH      
	cp ../data/$dataset".truss" graph-before.truss
	XHDecT=`./avgXHDec.sh $batNum`
	# Order
	OrderDecT=`./avgOrderDec.sh ../data/$dataset".order" $batNum`

	# save
	resultLine="$dataset,$XHIncT,$OrderIncT,$ourIncT,$ourDecT,$OrderDecT,$ourDecT"
	echo $resultLine >> $output
done
rm oldGraph.txt oldGraph.myG oldGraph.order graph-before.truss
