# dataset name, only two small datasets here
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
# repeat times
batNum=`cat ../repeat.txt`
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
	ourIncT=`./avgMul.sh $batNum singleOursInc.sh "./query/" oldGraph.myG`
	# XH
	./G2XH.sh oldGraph.txt >/dev/null
	XHIncT=`./avgMul.sh $batNum singleXHInc.sh "./query/" graph-before.truss`
	
	# Order
	./G2Order.sh oldGraph.txt oldGraph.order >/dev/null
	OrderIncT=`./avgMul.sh $batNum singleOrderInc.sh "./query/" oldGraph.order`

	# deletion
	# ours
	ourDecT=`./avgMul.sh $batNum singleOursDec.sh "./query/" ../data/$dataset".myG"`
	# XH      
	XHDecT=`./avgMul.sh $batNum singleXHDec.sh "./query/" ../data/$dataset".truss"`
	# Order
	OrderDecT=`./avgMul.sh $batNum singleOrderDec.sh "./query/" ../data/$dataset".order"`

	# save
	resultLine="$dataset,$XHIncT,$OrderIncT,$ourIncT,$ourDecT,$OrderDecT,$ourDecT"
	echo $resultLine >> $output
done
rm oldGraph.txt oldGraph.myG oldGraph.order graph-before.truss

python drawFig.py
