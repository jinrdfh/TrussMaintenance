datasets=(test)
indexEnd=1
batNum=10

output="Exp-VII.csv"
true > $output

for dataset in ${datasets[@]}
do
	header="Timestamps,Ours,Order,Decomposition"
	echo $header >> $output

	#  generate queries
	cd query/
	python ../addTime.py ../$dataset.txt ../info.txt $indexEnd $batNum >/dev/null
	cd ..

	# build index
	cp query/0.txt graph.txt
	# ours         
	./G2Ours.sh graph.txt graph.myG >/dev/null         
	# Order         
	./G2Order.sh graph.txt graph.order >/dev/null

	for i in $( seq 1 $batNum )
	do
	# ours
	ourIncT=`./runOursInc.sh graph.myG $i result.myG`
	# Order
	OrderIncT=`./runOrderInc.sh graph.order $i`

	# i+1 index
	# ours
	mv result.myG graph.myG
	# Order
	cat query/$i.txt >> graph.txt
	OrderIndexT=`./G2Order.sh graph.txt graph.order | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	
	# save
	resultLine=$i,$ourIncT,$OrderIncT,$OrderIndexT
	echo $resultLine >> $output
	done
done
rm graph.txt graph.myG graph.order
