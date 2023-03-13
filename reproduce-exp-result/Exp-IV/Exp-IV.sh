datasets=(deezer_RO amazon)
batNum=100

output="Exp-IV.csv"

header=",|E|,|AffBall|,|L_CHG|,|T_CHG|"
echo $header > $output

for dataset in ${datasets[@]}
do
	# generate queries
	./randomPs.sh ../$dataset".txt" oldGraph.txt $batNum >/dev/null
	# ours
	./G2Ours.sh oldGraph.txt oldGraph.myG >/dev/null
	dataSize=`wc -l ../$dataset".txt" | awk '{print $1}'`
	ourInc=`./avgOursInc.sh oldGraph.myG $batNum`

	# save
	resultLine="$dataset,$dataSize,$ourInc"
	echo $resultLine >> $output
done
rm oldGraph.txt oldGraph.myG 
