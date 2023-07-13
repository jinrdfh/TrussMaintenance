
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
batNum=`cat ../repeat.txt`

output="Exp-III.csv"

header=",|E|,|B_in|,|L_CHG|,|T_CHG|"
echo $header > $output

for dataset in ${datasets[@]}
do
	dataSize=`wc -l ../data/$dataset".txt" | awk '{print $1}'`

    ourInc=`./avgMul.sh $batNum singleOursInc.sh ../data/$dataset"_query/" ../data/$dataset"_sample.myG"`

	# save
	resultLine="$dataset,$dataSize,$ourInc"
	echo $resultLine >> $output
done
