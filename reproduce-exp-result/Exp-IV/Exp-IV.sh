
datasets=(Deezer Amazon Orkut Wise)
# datasets=(Deezer)
# smallStarSize=(10 100 1000)
# largeStarSize=(10 100 1000 10000 100000)
batNum=`cat ../repeat.txt`

output="Exp-IV.csv"

header="Size of star motif,10,,,,100,,,,1000,,,,10000,,,,100000,,,"
echo $header > $output
header="Graph,XH,NodePP,Order,Star,XH,NodePP,Order,Star,XH,NodePP,Order,Star"
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
	ourIncT=`./avgMul.sh $batNum singleOursInc.sh ../data/$dataset".myG"`
	# XH
	XHIncT=`./avgMul.sh $batNum singleXHInc.sh ../data/$dataset".truss"`
	# NodePP
	nodePPT=`./avgMul.sh $batNum singleNodePP.sh ../data/$dataset".truss"`
	# Order
	OrderIncT=`./avgMul.sh $batNum singleOrderInc.sh ../data/$dataset".order"`
	resultLine=$resultLine",$XHIncT,$nodePPT,$OrderIncT,$ourIncT"
	done

	# save
	echo $resultLine >> $output
done

python drawFig.py

