# dataset name, only two small datasets here
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
# repeat times
batNum=100

for dataset in ${datasets[@]}
do
	# insertion
	# generate queries
	mkdir $dataset"_query"
	./randomPs.sh $dataset".txt" $dataset"_sample.txt" $batNum $dataset"_query" >/dev/null
	# ours
	./G2Ours.sh $dataset"_sample.txt" $dataset"_sample.myG" >/dev/null
	# XH
	./G2XH.sh $dataset"_sample.txt" >/dev/null
	mv graph-before.truss $dataset"_sample.truss"
	# Order
	./G2Order.sh $dataset"_sample.txt" $dataset"_sample.order" >/dev/null

	# deletion
	# ours
	ourIndexT=`./G2Ours.sh $dataset".txt" $dataset".myG" | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	ourIndexT=`echo "scale=3; $ourIndexT/1000" | bc`
	# XH
	TrussT=`./G2XH.sh $dataset".txt" | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`         
	TrussT=`echo "scale=3; $TrussT/1000" | bc`
	mv graph-before.truss $dataset".truss"     
	# Order
	OrderIndexT=`./G2Order.sh $dataset".txt" $dataset".order" | grep Index | awk '{print $3}' | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g"`
	OrderIndexT=`echo "scale=3; $OrderIndexT/1000" | bc`

	# save
	true > $dataset".log"
	echo "Star indexing time: "$ourIndexT" ms" >> $dataset".log"
	echo "XH indexing time: "$TrussT" ms" >> $dataset".log"
	echo "Order indexing time: "$OrderIndexT" ms" >> $dataset".log"

done
