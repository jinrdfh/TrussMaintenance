
datasets=(Deezer Amazon DBLP Skitter Patents Pokec LJ Orkut Wise)
# datasets=(Deezer)
batNum=`cat ../repeat.txt`

output="Exp-II.csv"

header=",Phase I (ms),Phase II (ms)"
echo $header > $output

for dataset in ${datasets[@]}
do
    ourIncT=`./avgMul.sh $batNum singleOursInc.sh ../data/$dataset"_query/" ../data/$dataset"_sample.myG"`

	# save
	resultLine="$dataset,$ourIncT"
	echo $resultLine >> $output
done

python drawFig.py
