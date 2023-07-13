# dataset name, only two small datasets here
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
# repeat times
batNum=`cat ../repeat.txt`
# output file name
output="Exp-I.csv"
# table head
header=",Star Insertion (ms),,,,Star Deletion (ms),,,Index Size (MB),,,,Indexing Time (seconds),,,"
echo $header > $output
header="Graph,XH,NodePP,Order,Ours,XH,Order,Ours,XH,NodePP,Order,Ours,XH,NodePP,Order,Ours"
echo $header >> $output

for dataset in ${datasets[@]}
do
	# insertion
	# ours
	ourIncT=`./avgMul.sh $batNum singleOursInc.sh ../data/$dataset"_query/" ../data/$dataset"_sample.myG"`
	# XH
	XHIncT=`./avgMul.sh $batNum singleXHInc.sh ../data/$dataset"_query/" ../data/$dataset"_sample.truss"`
	# NodePP
	nodePPT=`./avgMul.sh $batNum singleNodePP.sh ../data/$dataset"_query/" ../data/$dataset"_sample.truss"`
	# Order
	OrderIncT=`./avgMul.sh $batNum singleOrderInc.sh ../data/$dataset"_query/" ../data/$dataset"_sample.order"`

	# deletion
	# ours
	ourIndexT=`cat ../data/$dataset".log" | grep Star | awk '{print $4}'`
	ourIndexSize=`du -m ../data/$dataset".myG" | awk '{print $1}'`
	ourDecT=`./avgMul.sh $batNum singleOursDec.sh ../data/$dataset"_query/" ../data/$dataset".myG"`
	# XH
	TrussT=`cat ../data/$dataset".log" | grep XH | awk '{print $4}'`
	TrussSize=`du -m ../data/$dataset".truss" | awk '{print $1}'`    
	XHDecT=`./avgMul.sh $batNum singleXHDec.sh ../data/$dataset"_query/" ../data/$dataset".truss"`
	# Order
	OrderIndexT=`cat ../data/$dataset".log" | grep Order | awk '{print $4}'`
	OrderIndexSize=`du -m ../data/$dataset".order" | awk '{print $1}'`
	OrderDecT=`./avgMul.sh $batNum singleOrderDec.sh ../data/$dataset"_query/" ../data/$dataset".order"`

	# save
	resultLine="$dataset,$XHIncT,$nodePPT,$OrderIncT,$ourIncT,$XHDecT,$OrderDecT,$ourDecT,$TrussSize,$TrussSize,$OrderIndexSize,$ourIndexSize,$TrussT,$TrussT,$OrderIndexT,$ourIndexT"
	echo $resultLine >> $output

done
