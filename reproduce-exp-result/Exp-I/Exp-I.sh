# dataset name, only two small datasets here
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
# repeat times
batNum=100
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
	ourIncT=`./avgOursInc.sh ../data/$dataset"_sample.myG" ../data/$dataset"_query/" $batNum`
	# XH
	cp ../data/$dataset"_sample.truss" graph-before.truss
	XHIncT=`./avgXHInc.sh ../data/$dataset"_query/" $batNum`
	# NodePP
	nodePPT=`./avgNodePP.sh ../data/$dataset"_query/" $batNum`
	rm graph-before.truss
	# Order
	OrderIncT=`./avgOrderInc.sh ../data/$dataset"_sample.order" ../data/$dataset"_query/" $batNum`

	# deletion
	# ours
	ourIndexT=`cat ../data/$dataset".log" | grep Star | awk '{print $4}'`
	ourIndexSize=`du -m ../data/$dataset".myG" | awk '{print $1}'`
	ourDecT=`./avgOursDec.sh ../data/$dataset".myG" ../data/$dataset"_query/" $batNum`
	# XH
	TrussT=`cat ../data/$dataset".log" | grep XH | awk '{print $4}'`
	cp ../data/$dataset".truss" graph-before.truss
	TrussSize=`du -m graph-before.truss | awk '{print $1}'`         
	XHDecT=`./avgXHDec.sh ../data/$dataset"_query/" $batNum`
	# Order
	OrderIndexT=`cat ../data/$dataset".log" | grep Order | awk '{print $4}'`
	OrderIndexSize=`du -m ../data/$dataset".order" | awk '{print $1}'`
	OrderDecT=`./avgOrderDec.sh ../data/$dataset".order" ../data/$dataset"_query/" $batNum`

	# save
	resultLine="$dataset,$XHIncT,$nodePPT,$OrderIncT,$ourIncT,$ourDecT,$OrderDecT,$ourDecT,$TrussSize,$TrussSize,$OrderIndexSize,$ourIndexSize,$TrussT,$TrussT,$OrderIndexT,$ourIndexT"
	echo $resultLine >> $output

	# clear
	rm graph-before.truss
done
