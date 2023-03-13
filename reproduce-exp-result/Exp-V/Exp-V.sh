datasets=(deezer_RO amazon)
subGraphLs=(graph_0_2 graph_0_4 graph_0_6 graph_0_8 graph_1_0)
batNum=100

output="Exp-V.csv"

header=",20%,,,40%,,,60%,,,80%,,,100%"
echo $header > $output
header="Graph,Ours(inc),Ours(dec),Decomposition,Ours(inc),Ours(dec),Decomposition,Ours(inc),Ours(dec),Decomposition,Ours(inc),Ours(dec),Decomposition,Ours(inc),Ours(dec),Decomposition"
echo $header >> $output

for dataset in ${datasets[@]}
do
	python divideG.py ../$dataset".txt" graph
	cp ../$dataset".txt" ./graph_1_0.txt
	resultLine="$dataset"
	for subgraph in ${subGraphLs[@]}
	do
	# insertion
	# generate queries
	./randomPs.sh ./$subgraph".txt" oldGraph.txt $batNum >/dev/null
	# ours
	./G2Ours.sh oldGraph.txt oldGraph.myG >/dev/null
	ourIncT=`./avgOursInc.sh oldGraph.myG $batNum`

	# deletion
	# ours
	ourIndexT=`./G2Ours.sh ./$subgraph".txt" graph.myG | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	ourIndexT=`echo "scale=3; $ourIndexT/1000" | bc`
	ourDecT=`./avgOursDec.sh graph.myG $batNum`

	# save
	resultLine=$resultLine,$ourIncT,$ourDecT,$ourIndexT
	done
	echo $resultLine >> $output
done
rm *.txt oldGraph.myG graph.myG 
