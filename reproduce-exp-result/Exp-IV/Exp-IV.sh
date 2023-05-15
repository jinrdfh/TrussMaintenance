
datasets=(Deezer Amazon Orkut Wise)
# datasets=(Deezer)
# smallStarSize=(10 100 1000)
# largeStarSize=(10 100 1000 10000 100000)
batNum=100

output="Exp-IV.csv"

header="Size of star motif,10,,,,100,,,,1000,,,,10000,,,,100000,,,"
echo $header > $output
header="Graph,XH,NodePP,Order,Ours,XH,NodePP,Order,Ours,XH,NodePP,Order,Ours"
echo $header >> $output

for dataset in ${datasets[@]}
do

	resultLine="$dataset"

	graphSize=`wc -l ../data/$dataset".txt" | awk '{print $1}'`
	starSize=(10 100 1000)
	if [ $graphSize -gt 10000000 ]
	then
		starSize=(10 100 1000 10000 100000)
	fi

	for curSize in ${starSize[@]}
	do
	echo $curSize
	# generate queries
	./randomQ.sh ../data/$dataset".txt" $curSize $batNum >/dev/null
	# ours
	ourIncT=`./avgOursInc.sh ../data/$dataset".myG" $batNum`
	# XH
	cp ../data/$dataset".truss" graph-before.truss
	XHIncT=`./avgXHInc.sh $batNum`
	# NodePP
	nodePPT=`./avgNodePP.sh $batNum`
	rm graph-before.truss
	# Order
	OrderIncT=`./avgOrderInc.sh ../data/$dataset".order" $batNum`
	resultLine=$resultLine",$XHIncT,$nodePPT,$OrderIncT,$ourIncT"
	done

	# save
	echo $resultLine >> $output
done

