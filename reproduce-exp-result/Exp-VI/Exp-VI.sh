datasets=(deezer_RO)
batNum=100

output="Exp-VI.csv"
true > $output

for dataset in ${datasets[@]}
do
	header="Number,Ours,Order"
	echo $header >> $output

	#  generate queries
	./randomPs.sh ../$dataset".txt" oldGraph.txt $batNum >/dev/null

	# build index
	# ours         
	./G2Ours.sh oldGraph.txt graph.myG >/dev/null         
	# Order         
	./G2Order.sh oldGraph.txt graph.order >/dev/null

	for i in $( seq 1 $batNum )
	do
	# sum queries
	./sum.sh $i

	# ours
	ourIncT=`./runOursInc.sh graph.myG $i`
	# Order
	OrderIncT=`./runOrderInc.sh graph.order $i`

	# save
	resultLine=$i,$ourIncT,$OrderIncT
	echo $resultLine >> $output
	done
done
rm oldGraph.txt graph.myG graph.order
