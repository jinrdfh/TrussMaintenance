datasets=(Patents)
indexEnd=1800
batNum=100
logFile="XH.log"

output="Exp-VIII.csv"
true > $output

for dataset in ${datasets[@]}
do
	header="Timestamps,Star,Order,XH"
	echo $header >> $output

	#  generate queries
	cd query/
	python ../addTime.py ../../data/$dataset.txt ../../data/apat63_99.txt $indexEnd $batNum >/dev/null
	cd ..

	# build index
	cp query/0.txt graph.txt
	# ours         
	./G2Ours.sh graph.txt graph.myG >/dev/null         
	# Order         
	./G2Order.sh graph.txt graph.order >/dev/null
	# XH
	./G2XH.sh graph.txt >/dev/null

	for i in $( seq 1 $batNum )
	do
	# ours
	ourIncT=`./runOursInc.sh graph.myG $i result.myG`
	# Order
	OrderIncT=`./runOrderInc.sh graph.order $i`
	# XH
	./runXHInc.sh graph-before.truss $i >> $logFile
	XHIncT=`tail -n 1 $logFile`

	# i+1 index
	# ours
	mv result.myG graph.myG
	# Order
	cat query/$i.txt >> graph.txt
	OrderIndexT=`./G2Order.sh graph.txt graph.order | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	# XH
	./G2XH.sh graph.txt >/dev/null
	
	# save
	resultLine=$i,$ourIncT,$OrderIncT,$XHIncT
	echo $resultLine >> $output
	done
done
rm $logFile graph.txt graph.myG graph.order graph-before.truss

python drawFig.py
