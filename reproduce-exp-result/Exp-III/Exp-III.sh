datasets=(deezer_RO amazon)
batNum=100

output="Exp-III.csv"

header=",Step 1 (ms),Step 2 (ms)"
echo $header > $output

for dataset in ${datasets[@]}
do
	# generate queries
	./randomPs.sh ../$dataset".txt" oldGraph.txt $batNum >/dev/null
	# ours
	./G2Ours.sh oldGraph.txt oldGraph.myG >/dev/null
	ourIncT=`./avgOursInc.sh oldGraph.myG $batNum`

	# save
	resultLine="$dataset,$ourIncT"
	echo $resultLine >> $output
done
rm oldGraph.txt oldGraph.myG 
