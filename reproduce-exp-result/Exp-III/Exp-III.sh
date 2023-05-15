
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
batNum=100

output="Exp-III.csv"

header=",|E|,|B_in|,|L_CHG|,|T_CHG|"
echo $header > $output

for dataset in ${datasets[@]}
do
	dataSize=`wc -l ../data/$dataset".txt" | awk '{print $1}'`
	ourInc=`./avgOursInc.sh ../data/$dataset"_sample.myG" ../data/$dataset"_query/" $batNum`

	# save
	resultLine="$dataset,$dataSize,$ourInc"
	echo $resultLine >> $output
done
