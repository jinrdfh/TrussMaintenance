datasets=(deezer_RO amazon)
starSize=(10 100 1000)
batNum=100

output="Exp-II.csv"

header="Star Size,10,,,,100,,,,1000,,,"
echo $header > $output
header="Graph,XH,NodePP,Order,Ours,XH,NodePP,Order,Ours,XH,NodePP,Order,Ours"
echo $header >> $output

for dataset in ${datasets[@]}
do
	# build index
	# ours
	./G2Ours.sh ../$dataset".txt" graph.myG >/dev/null
	# XH and NodePP
	./G2XH.sh ../$dataset".txt" >/dev/null
	# Order
	./G2Order.sh ../$dataset".txt" graph.order >/dev/null

	resultLine="$dataset"

	for curSize in ${starSize[@]}
	do
	# generate queries
	./randomQ.sh ../$dataset".txt" $curSize $batNum >/dev/null
	# ours
	ourIncT=`./avgOursInc.sh graph.myG $batNum`
	# XH
	XHIncT=`./avgXHInc.sh $batNum`
	# NodePP
	nodePPT=`./avgNodePP.sh $batNum`
	# Order
	OrderIncT=`./avgOrderInc.sh graph.order $batNum`
	resultLine=$resultLine",$XHIncT,$nodePPT,$OrderIncT,$ourIncT"
	done

	# save
	echo $resultLine >> $output
done
rm graph.myG graph.order graph-before.truss
