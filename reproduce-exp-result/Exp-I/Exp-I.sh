# dataset name, only two small datasets here
datasets=(deezer_RO amazon)
# repeat times
batNum=100
# output file name
output="Exp-I.csv"
# table head
header=",Insertion (ms),,,,Removal (ms),,,Space (MB),,,,Index Creation (seconds),,,"
echo $header > $output
header="Graph,XH,NodePP,Order,Ours,XH,Order,Ours,XH,NodePP,Order,Ours,XH,NodePP,Order,Ours"
echo $header >> $output

for dataset in ${datasets[@]}
do
	# insertion
	# generate queries
	./randomPs.sh ../$dataset".txt" oldGraph.txt $batNum >/dev/null
	# ours
	./G2Ours.sh oldGraph.txt oldGraph.myG >/dev/null
	ourIncT=`./avgOursInc.sh oldGraph.myG $batNum`
	# XH
	./G2XH.sh oldGraph.txt >/dev/null
	XHIncT=`./avgXHInc.sh $batNum`
	# NodePP
	nodePPT=`./avgNodePP.sh $batNum`
	# Order
	./G2Order.sh oldGraph.txt oldGraph.order >/dev/null
	OrderIncT=`./avgOrderInc.sh oldGraph.order $batNum`

	# deletion
	# ours
	ourIndexT=`./G2Ours.sh ../$dataset".txt" graph.myG | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	ourIndexT=`echo "scale=3; $ourIndexT/1000" | bc`
	ourIndexSize=`du -m graph.myG | awk '{print $1}'`
	ourDecT=`./avgOursDec.sh graph.myG $batNum`
	# XH
	TrussT=`./G2XH.sh ../$dataset".txt" | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`         
	TrussT=`echo "scale=3; $TrussT/1000" | bc`
	TrussSize=`du -m graph-before.truss | awk '{print $1}'`         
	XHDecT=`./avgXHDec.sh $batNum`
	# Order
	OrderIndexT=`./G2Order.sh ../$dataset".txt" graph.order | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	OrderIndexT=`echo "scale=3; $OrderIndexT/1000" | bc`
	OrderIndexSize=`du -m graph.order | awk '{print $1}'`
	OrderDecT=`./avgOrderDec.sh graph.order $batNum`

	# save
	resultLine="$dataset,$XHIncT,$nodePPT,$OrderIncT,$ourIncT,$ourDecT,$OrderDecT,$ourDecT,$TrussSize,$TrussSize,$OrderIndexSize,$ourIndexSize,$TrussT,$TrussT,$OrderIndexT,$ourIndexT"
	echo $resultLine >> $output
done
rm oldGraph.txt oldGraph.myG graph.myG oldGraph.order graph.order graph-before.truss
